#include "DungeonRace.h"
#include <AvalonRandom.h>
#include <AvalonMd5.h>

#include <DropItemMgr.h>
#include <CLDungeonProtocol.h>
#include <CLDungeonChestDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLMailDefine.h>
#include <CLDungeonPrefixLimitDataEntry.h>
#include <CLSortListProtocol.h>
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLTeamRewardDataEntry.h>
#include <CLLevelAdapterDataEntry.h>
#include <CLDungeonGuaranteeDropDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLDungeonDailyDropDataEntry.h>
#include <CLDungeonTimeDataEntry.h>
#include <CLRelayServerProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLActivitySuitAdditionDataEntry.h>
#include <CLDungeonVerifyProbDataEntry.h>
#include <CLShopItemDataEntry.h>
#include <CLTeamProtocol.h>
#include <CLDropStrengthenControlDataEntry.h>
#include <DropItemMgr.h>
#include <CLFriendWelfareAddDataEntry.h>
#include <CLGuildDungeonLvlDataEntry.h>
#include <CLDungeonClearTimeDataEntry.h>
#include <CLItemProtocol.h>
#include <CLGuildProtocol.h>
#include <CLCrossDungeonDefine.h>

#include "WSApplication.h"
#include "DungeonRaceMgr.h"
#include "DungeonMonsterGenerator.h"
#include "WSPlayerMgr.h"
#include "WSItemTransaction.h"
#include "SysRecordMgr.h"
#include "WorldSysNotify.h"
#include "WSServiceSwitchMgr.h"
#include "GuildMgr.h"
#include "PunishmentMgr.h"
#include "VerifyServerMgr.h"
#include "WSActivityMonsterMgr.h"
#include "AnnouncementMgr.h"
#include "WSActivityMgr.h"
#include "GuildDungeonMgr.h"
#include "GuildDungeonVerifyMgr.h"
#include "WSTeamMgr.h"
#include "AccountMgr.h"

// 随机数上传间隔帧数
const int CHECKSUM_INTERVAL = 50;
// 每一帧间隔时间(ms)
const int FRAME_INTERVAL = 16;

/**
*@brief 比赛事务
*/
class DungeonRaceCheckTransaction : public ItemTransaction
{
public:
	DungeonRaceCheckTransaction(WSPlayer *player, ObjID_t id, DungeonRaceTransactionType type)
		: ItemTransaction(player), m_raceId(id), m_type(type) {}
	~DungeonRaceCheckTransaction() {}

	void OnCheckSucced(WSPlayer *pPlayer)
	{
		auto race = DungeonRaceMgr::Instance()->FindDungeonRaceById(m_raceId);
		if (!race)
		{
			UnLock();
			return;
		}

		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON, race->GetDungeonID());

		bool success = false;
		if (m_type == DUNGEON_RACE_OPEN_PAY_CHEST)
		{
			success = race->OnCheckOpenPayChestRet(pPlayer, m_param.pos);
			reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON_CHEST, race->GetDungeonID());
		}
		else if (m_type == DUNGEON_RACE_MULTI_RACE_END_DROP)
		{
			success = race->OnCheckMultiRaceEndDropRet(pPlayer, m_param.muilti);
			reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON_MULTI_RACE_END_DROP, race->GetDungeonID());
		}
		else if (m_type == DUNGEON_RACE_REVIVE)
		{
			success = race->OnCheckReviveRet(pPlayer, m_param2.reviveId, m_param.targetId) == ErrorCode::SUCCESS;
			reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON_REVIVE, race->GetDungeonID());
		}

		if (success)
		{
			RemoveItem(reason);
		}
		else
		{
			UnLock();
		}
	}

	void SetParam(UInt8 value) { m_param.pos = value; }
	void SetParam(ObjID_t value) { m_param.targetId = value; }
	void SetParam2(UInt32 value) { m_param2.reviveId = value; }

	void OnCheckFailed(WSPlayer *pPlayer)
	{
		if (m_type == DUNGEON_RACE_REVIVE)
		{
			CLProtocol::SceneDungeonReviveRes res;
			res.result = ErrorCode::DUNGEON_REVIVE_NOT_ENOUGH_REVIVE_COIN;
			pPlayer->SendProtocol(res);
		}

		UnLock();
	}

private:
	// 比赛id
	ObjID_t						m_raceId;
	// 类型
	DungeonRaceTransactionType	m_type;

	union DungeonRaceTransactionParam
	{
		UInt8					pos;
		UInt8					muilti;
		ObjID_t					targetId;
		UInt32					reviveId;
	};
	
	DungeonRaceTransactionParam		m_param;
	DungeonRaceTransactionParam		m_param2;
};

DungeonRace::DungeonRace()
{
	m_id = 0;
	m_hellDungeonEntryId = 0;
	m_status = DRS_INIT;
	m_dungeonId = 0;
	m_hellDungeonEntryId = 0;
	m_dungeonData = NULL;
	m_startAreaId = 0;
	m_passedAreaIndex = 0;
	m_isHellMode = false;
	m_hellHardMode = HELL_MODE_NULL;
	m_dropItemSeed = 0;
	m_openedChestMask = 0;
	m_relayServerId = 0;
	m_startTime = 0;
	m_teamId = 0;
	m_DropCountState = false;
	m_lastFrame = 0;
	m_VSId = 0;
	m_MysticalMerchantId = 0;
	m_HasSendRaceOver = false;
	m_bSendMasterDiscipleInteractionLog = false;
	m_flagGuildDungeon = false;
	m_TriggerHiddenRoom = 0;
	m_isGeneratePublishDrops = false;
	m_rollEndTime = 0;
	m_isSingleRoll = false;
	m_isRollSettled = false;
}

DungeonRace::~DungeonRace()
{
	if (_IsVerifying())
	{
		CLProtocol::VerifyReportRaceDismiss report;
		report.raceId = GetID();
		_SendToVerifyServer(report);
	}

	// 发送战斗结束消息到scene
	if (m_HasSendRaceOver == false)
	{
		m_HasSendRaceOver = true;

		CLProtocol::WorldTeamDungeonRaceOver over;
		over.raceId = GetID();
		over.teamId = GetTeamID();
		over.cityMonster = GetCityMonster();
		Router::BroadcastToScene(over);
	}

	// 计算没有拾取的稀有物品
	for (auto area : m_areas)
	{
		for (const auto& monster : area->GetAllMonster())
		{
			for (const auto& dropItem : monster.dropItems)
			{
				if (!dropItem.isPickUp)
				{
					SysRecordMgr::Instance()->RecordGotRareDropSolution(dropItem.roleId, dropItem.solutionId, -1);
				}

				if (!dropItem.isPickUp && dropItem.isRareControl)
				{
					SysRecordMgr::Instance()->RecordGotRareItem(dropItem.roleId, dropItem.typeId, -1 * (int)dropItem.num);
				}
			}
		}
	}

	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player)
		{
			continue;
		}

		for (const auto& dropItem : player->GetBossDropItems())
		{
			if (!dropItem.isPickUp && dropItem.isRareControl)
			{
				SysRecordMgr::Instance()->RecordGotRareItem(dropItem.roleId, dropItem.typeId, -1 * (int)dropItem.num);
			}
		}
	}
	


	for (const auto& task : m_chestTasks)
	{
		for (auto& chest : task.GetAllChest())
		{
			if (!chest.isOpened && chest.isRareControl)
			{
				SysRecordMgr::Instance()->RecordGotRareItem(task.GetRoleID(), chest.typeId, -1 * (int)chest.num);
			}
		}
	}

	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (player)
		{
			delete player;
		}
	}

	for (auto area : m_areas)
	{
		delete area;
	}

	if (_IsVerifying())
	{
		CLProtocol::VerifyReportRaceDismiss report;
		report.raceId = GetID();
		_SendToVerifyServer(report);
	}
}

void DungeonRace::Tick(const Avalon::Time& now)
{
	/*for (UInt32 i = 0; i < m_chestTasks.size();)
	{
		auto& task = m_chestTasks[i];
		if (!task.IsTimeOut(now))
		{
			i++;
			continue;
		}

		// 自动打开普通宝箱
		auto player = WSPlayerMgr::Instance()->FindNormalPlayer(task.GetRoleID());
		if (!OnPlayerOpenChest(player, UINT8_MAX))
		{
			ErrorLogStream << "player(" << task.GetRoleID() << ") open chest failed." << LogStream::eos;
		}

		for (auto& chest : task.GetAllChest())
		{
			if (!chest.isOpened && chest.isRareControl)
			{
				SysRecordMgr::Instance()->RecordGotRareItem(task.GetRoleID(), chest.typeId, -1 * (int)chest.num);
			}
		}

		m_chestTasks.erase(m_chestTasks.begin() + i);
	}*/
	if (!m_isSingleRoll && _CheckCanSettleRollDrop())
	{
		_SettleRollDrops();
	}
}

void DungeonRace::AddPlayer(const DungeonRacePlayerInfo& info)
{
	auto roleId = info.raceInfo.roleId;

	if (FindDungeonPlayer(roleId))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") repeat add player(" << roleId << ")" << LogStream::eos;
		return;
	}

	auto addition = info.addition;
	WSActivityMgr::Instance()->GetActivityDropAddition(m_dungeonId, addition);

	auto wsPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(GetDungeonID());
	if (wsPlayer != NULL && dungeonData != NULL)
	{
		wsPlayer->GetAdventureTeamDungeonAddition(addition, dungeonData);
	}

	DungeonPlayer* player = new DungeonPlayer();
	player->SetID(roleId);
	player->SetAccID(info.accId);
	player->SetZoneID(info.zoneId);
	player->SetRacePlayerInfo(info.raceInfo);
	player->SetOccu(info.raceInfo.occu);
	player->SetLevel(info.raceInfo.level);
	player->SetTaskList(info.taskList);
	player->SetDungeonAddition(addition);
	player->SetDailyCount(info.dailyCount);
	player->SetTotalCount(info.totalCount);
	player->AddUsedTime(info.usedTime);
	player->SetRemainHp(info.remainHp);
	player->SetRemainMp(info.remainMp);
	player->SetFreeOpenGoldChest(info.freeGoldChest != 0);
	player->SetAssist(info.isAssist != 0);
	player->SetAddedDropNum(info.addedMonsterDropNum);
	player->SetVipLvl(info.vipLvl);

	InfoLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") isAsset(" << player->IsAssist() << ")." << LogStream::eos;

	if (wsPlayer && wsPlayer->IsVeteranReturn())
	{
		player->SetVeteranReturn();
	}

	if (wsPlayer != nullptr && wsPlayer->IsInMonthCard())
	{
		player->SetBoMonthCard();
	}

	if (wsPlayer != nullptr && wsPlayer->HaveGiftRightCard())
	{
		player->SetBoGiftRightCard();
	}

	m_players[player->GetID()] = player;
}

DungeonPlayer* DungeonRace::FindDungeonPlayer(UInt64 roleId)
{
	auto itr = m_players.find(roleId);
	return itr == m_players.end() ? NULL : itr->second;
}

void DungeonRace::OnPlayerLeave(UInt64 roleId, bool isDisconnect)
{
	auto player = FindDungeonPlayer(roleId);
	if (!player)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << roleId << ")." << LogStream::eos;
		return;
	}

	player->SetLeave(true);
	player->SetDisconnect(isDisconnect);
}

bool DungeonRace::IsAllPlayerLeave()
{
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player)
		{
			continue;
		}

		if (!player->IsLeave())
		{
			return false;
		}
	}

	return true;
}

void DungeonRace::SendDungeonLogWhenAllPlayerLeave()
{
	for (auto itr : m_players)
	{
		auto dungeonPlayer = itr.second;
		//if (!dungeonPlayer || dungeonPlayer->HasRaceEnd())
		if (!dungeonPlayer)
		{
			continue;
		}

		/*Sys::SendUdpLog("dungeon", LOG_DUNGEON_FORMAT, "", dungeonPlayer->GetZoneID(), (UInt32)CURRENT_TIME.Sec(), dungeonPlayer->GetID(), dungeonPlayer->GetAccID(),
			dungeonPlayer->GetLevel(), 0, GetID(), GetDungeonID(), GetDungeonData()->type, GetDungeonData()->name.c_str(), m_dungeonData->hardType,
			m_hellHardMode, CURRENT_TIME.Sec() - m_startTime, dungeonPlayer->GetLevelScore(), dungeonPlayer->GetReviveCount());*/
	}
}

bool DungeonRace::HasPlayerDisconnect()
{
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player)
		{
			continue;
		}

		if (player->IsDisconnect())
		{
			return true;
		}
	}

	return false;
}

void DungeonRace::GetAreaInfo(UInt64 roleId, std::vector<CLProtocol::SceneDungeonArea>& areas)
{
	for (const auto area : m_areas)
	{
		if (!area)
		{
			continue;
		}

		CLProtocol::SceneDungeonArea info;
		area->GetInfo(roleId, info);

		areas.push_back(info);
	}
}

void DungeonRace::GetHellInfo(UInt64 roleId, CLProtocol::DungeonHellInfo& info)
{
	if (!m_dungeonData || !m_isHellMode || m_dungeonData->hellAreaList.empty())
	{
		return;
	}

	info.mode = m_hellHardMode;
	info.areaId = m_hellAreaId;

	auto hellArea = _FindArea(info.areaId);
	if (!hellArea)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find hell area(" << info.areaId << ")." << LogStream::eos;
		return;
	}

	// 直接生成3波的数据
	UInt32 maxWave = 3;
	for (UInt32 i = 0; i < maxWave; i++)
	{
		CLProtocol::DungeonHellWaveInfo waveInfo;
		waveInfo.wave = i;
		info.waveInfoes.push_back(waveInfo);
	}

	for (auto& monster : hellArea->GetAllMonster())
	{
		if (!monster.isHell)
		{
			continue;
		}

		if (monster.hellWave >= maxWave)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") dungeon(" << GetDungeonID() << ") has invalid monster("
				<< monster.typeId << ") wave(" << monster.hellWave << ")." << LogStream::eos;
			continue;
		}

		auto monsterInfo = monster.GetInfo(roleId);

		// 怪物掉落都放到深渊掉落里
		for (auto& drop : monsterInfo.dropItems)
		{
			info.dropItems.push_back(drop);
		}
		monsterInfo.dropItems.clear();

		info.waveInfoes[monster.hellWave].monsters.push_back(monsterInfo);
	}
}

bool DungeonRace::IsTimeToDestory(const Avalon::Time& now)
{
	if (GetStatus() != DRS_WAIT_CLOSE)
	{
		return false;
	}

	if (now < m_destoryTime)
	{
		return false;
	}

	return true;
}

bool DungeonRace::IsPlayerRaceEnd(ObjID_t id)
{
	auto dungeonPlayer = FindDungeonPlayer(id);
	if (!dungeonPlayer)
	{
		return false;
	}

	return dungeonPlayer->HasRaceEnd();
}

UInt32 DungeonRace::GetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId)
{
	return SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX, SYS_BEHAVIOR_OWNER_ROLE, roleId, _GetNormalHardDungeonID(dungeonId));
}

UInt32 DungeonRace::SetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex)
{
	UInt32 tmpRecord = GetDungeonAreaIndex(roleId, dungeonId);
	// 如果原值是0，需要判断下是不是已经收到结算之后再来设置
	if (WSApplication::Instance()->CheckDevilDropBug() && tmpRecord == 0)
	{
		auto dungeonPlayer = FindDungeonPlayer(roleId);
		if (dungeonPlayer && dungeonPlayer->GetLevelScore() > LS_C)
		{
			InfoLogStream << "race(" << GetID() << ") player(" << roleId << ") dungeon(" << dungeonId << ") set area(" << areaIndex << ") after race end." << LogStream::eos;
			return 0;
		}

	}
// 	if (tmpRecord == 0 && areaIndex > 1)
// 	{
// 		return tmpRecord;
// 	}

	UInt32 tmpMask = 1 << areaIndex;
	if ((tmpRecord & tmpMask) == 0)
	{
		tmpRecord = tmpRecord | tmpMask;
		SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX, SYS_BEHAVIOR_OWNER_ROLE, roleId, _GetNormalHardDungeonID(dungeonId), tmpRecord);
	}

	return tmpRecord;
}

UInt32 DungeonRace::ResetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId)
{
	SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX, SYS_BEHAVIOR_OWNER_ROLE, roleId, _GetNormalHardDungeonID(dungeonId), 0);
	return 0;
}

bool DungeonRace::IsSetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex)
{
	UInt32 tmpRecord = GetDungeonAreaIndex(roleId, dungeonId);
	UInt32 tmpMask = 1 << areaIndex;
	if ((tmpRecord & tmpMask) == 0)
	{
		return false;
	}

	return true;
}

void DungeonRace::EndGuildDungeon()
{
	if (GetSession() == 0)
	{
		// 单人地下城退出
		CLProtocol::WorldGuildDungeonEndNotify notify;
		notify.dungeonId = GetDungeonID();
		for (auto& iter : m_players)
		{
			UInt64 playerId = iter.second->GetID();
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(playerId);
			if (NULL == player)
			{
				ErrorLogStream << "not find player:" << playerId << " dungeonId:" << GetDungeonID() << LogStream::eos;
				continue;
			}

			player->SendProtocol(notify);
			InfoLogStream << "notify guild dungeon quit raceId:" << GetID() << " dungeonId:" << GetDungeonID() 
				<< " playerId:" << playerId << LogStream::eos;
		}
	}
	else
	{
		// 通知队伍退出
		_NotifyRelayPlayerEvent(0, SYNC_END_RACE, CDER_TEAMCOPY_FILED_DESTORY, 0);
		InfoLogStream << "notify guild dungeon quit raceId:" << GetID() << " dungeonId:" << GetDungeonID() << LogStream::eos;
	}
}

void DungeonRace::NotifyGuildDungeonBossOddBlood(UInt64 oddBlood, UInt64 totalBlood)
{
	CLProtocol::WorldGuildDungeonBossOddBlood notify;
	notify.bossOddBlood = oddBlood;
	notify.bossTotalBlood = totalBlood;

	for (auto& iter : m_players)
	{
		UInt64 playerId = iter.second->GetID();
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(playerId);
		if (player != NULL)
		{
			player->SendProtocol(notify);
		}
	}
}

UInt32 DungeonRace::StartRace(ObjID_t session, const Avalon::SockAddr& addr, UInt32 relayServerId)
{
	if (!m_dungeonData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	m_startTime = CURRENT_TIME.Sec();
	m_relayServerId = relayServerId;
	m_sessionId = session;

	// 判断是不是公会地下城，是否需要带回boss血量
	CLProtocol::GuildDungeonInfo guildDungeonInfo;
	if (m_dungeonData->subType == DST_GUILD_DUNGEON)
	{
		if (!GuildDungeonMgr::Instance()->SwitchIsOpen())
		{
			return ErrorCode::GUILD_DUNGEON_NOT_OPEN;
		}

		// 检查队伍里的人是否全都是一个公会的
		if (m_players.empty()) return ErrorCode::TEAM_JOIN_TEAM_UNEXIST;
		UInt32 guild_dungeon_player_limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_PLAYER_LEVEL_LIMIT);
		UInt64 guild_id = 0;
		for (auto& itr : m_players)
		{
			auto dungeonPlayer = itr.second;
			if (NULL == dungeonPlayer)
				continue;

			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(dungeonPlayer->GetID());
			if (NULL == player)
			{
				ErrorLogStream << "guild dungeon race(" << GetID() << ") start race, player(" << dungeonPlayer->GetID() << ") offline." << LogStream::eos;
				continue;
			}

			if (guild_id != 0 && guild_id != player->GetGuildID())
			{
				ErrorLogStream << "dungeon race(" << GetID() << ") start race, not same guild race!!! guild:" << guild_id << LogStream::eos;
				return ErrorCode::GUILD_DUNGEON_NOT_SAME_GUILD;
			}

			guild_id = player->GetGuildID();

			if (player->GetLevel() < guild_dungeon_player_limit)
			{
				// 玩家等级不够挑战公会地下城
				ErrorLogStream << "dungeon race(" << GetID() << ") start race, player(" << dungeonPlayer->GetID() << ") name:(" << player->GetName()
					<< ") level:(" << player->GetLevel() << ") limit!" << LogStream::eos;
				return ErrorCode::GUILD_DUNGEON_PLAYER_LEVEL_LIMIT;
			}
		}

		if (guild_id == 0)
		{
			ErrorLogStream << "guild dungeon race(" << GetID() << ") start race failed!!! guild:" << guild_id << LogStream::eos;
			return ErrorCode::GUILD_BATTLE_NOT_EXIST;
		}

		Guild* guild = GuildMgr::Instance()->FindGuildByID(guild_id);
		if (NULL == guild)
		{
			ErrorLogStream << "guild dungeon race(" << GetID() << ") start race failed!!! not guild:" << guild_id << LogStream::eos;
			return ErrorCode::GUILD_NOT_IN_GUILD;
		}

		UInt32 guild_dungeon_limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT);
		if (guild->GetLevel() < guild_dungeon_limit)
		{
			ErrorLogStream << "guild dungeon race(" << GetID() << ") start race failed!!! guild level limit:" << guild_id << LogStream::eos;
			return ErrorCode::GUILD_DUNGEON_GUILD_LEVEL_LIMIT;
		}

		UInt32 ret = GuildDungeonMgr::Instance()->StartGuildDungeon(guild_id, GetDungeonID(), GetID(), guildDungeonInfo);
		if (ret != ErrorCode::SUCCESS)
		{
			ErrorLogStream << "guild dungeon race(" << GetID() << ") get info failed!!! guild:" << guild_id << " dungeonId:" << GetDungeonID() << LogStream::eos;
			return ret;
		}
	}

	// 计算关卡区域
	if (!_GenAreas())
	{
		ErrorLogStream << "race(" << GetID() << ") gen areas failed." << LogStream::eos;
		return false;
	}

	// 计算BOSS掉落
	_GenBossDropItems();

	// 计算神秘商人
	_RandMysticalMerchant();

	// 计算好友经验加成系数
	_GenFriendExpRate();

	// 设置状态
	SetStatus(DRS_RACING);

	auto startArea = _FindArea(m_startAreaId);
	if (!startArea && (!m_dungeonData || m_dungeonData->subType != DST_TREASUREMAP))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find start area("
			<< m_startAreaId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
	}

	DebugLogStream << "dungeon race(" << GetID() << ") start racing." << LogStream::eos;

	// 发送返回给客户端
	/*CLProtocol::WorldNotifyRaceStart raceStartInfo;
	raceStartInfo.raceType = RT_DUNGEON;
	raceStartInfo.dungeonID = GetDungeonID();
	raceStartInfo.session = session;
	raceStartInfo.addr = addr;*/

	std::vector<RacePlayerInfo> racePlayerInfoes;

	for (auto& itr : m_players)
	{
		auto dungeonPlayer = itr.second;

		dungeonPlayer->OnEnterArea(m_startAreaId);

		auto racePlayerInfo = dungeonPlayer->GetRacePlayerInfo();
		//raceStartInfo.players.push_back(racePlayerInfo);
		racePlayerInfoes.push_back(racePlayerInfo);
	}

	CLProtocol::SceneDungeonStartRes tmpRes;
	_GenEncryptKey(tmpRes);

	for (auto& itr : m_players)
	{
		auto dungeonPlayer = itr.second;

		auto player = WSPlayerMgr::Instance()->FindPlayer(dungeonPlayer->GetID());
		if (!player)
		{
			DebugLogStream << "dungeon race(" << GetID() << ") start race, player(" << dungeonPlayer->GetID() << ") offline." << LogStream::eos;
			continue;
		}

		// 设置进入公会地下城的CD
		if (m_dungeonData->subType == DST_GUILD_DUNGEON && GuildDungeonMgr::Instance()->SwitchIsOpen())
		{
			UInt32 guildDungeonCD = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_COLD_TME);
			if (player->GetGuildDungeonCD() + guildDungeonCD > CURRENT_TIME.Sec())
			{
				// 广播给队友，进入冷却时间
				for (auto& tt : m_players)
				{
					auto playerCD = WSPlayerMgr::Instance()->FindPlayer(tt.second->GetID());
					if (!playerCD)
						continue;

					UInt32 sec = player->GetGuildDungeonCD() + guildDungeonCD - CURRENT_TIME.Sec();

					// 单人进入
					if (m_players.size() == 1)
					{
						playerCD->SendNotify(9971, sec);
					}
					else   // 组队进入
					{						
						playerCD->SendNotify(9972, player->GetName(), sec);
					}
				}

				return ErrorCode::GUILD_DUNGEON_CD;
			}

			player->SetGuildDungeonCD();
		}

		/*raceStartInfo.roleId = dungeonPlayer->GetID();
		raceStartInfo.dungeonID = GetDungeonID();
		player->SendProtocol(raceStartInfo);*/

		CLProtocol::SceneDungeonStartRes res;
		_GetStartInfo(dungeonPlayer, racePlayerInfoes, addr, res);
		res.guildDungeonInfo = guildDungeonInfo;
		res.zjslDungeonInfo = m_ZjslDungeonInfo;
		res.key1 = tmpRes.key1;
		res.key2 = tmpRes.key2;
		res.key3 = tmpRes.key3;
		res.key4 = tmpRes.key4;
		memset(res.md5, 0, sizeof(res.md5));

		Avalon::Packet* packet = Avalon::Packet::Create();
		if (res.Encode(packet))
		{
			Avalon::md5_encode((char*)res.md5, packet->GetBuffer(), packet->GetSize() - sizeof(res.md5));
		}
		Avalon::Packet::Destroy(packet);

		player->SendProtocol(res);

		UInt8 hellAutoClose = 0;
		if (GetTeamID())
		{
			auto team = WSTeamMgr::Instance()->FindTeam(GetTeamID());
			if (team && team->IsSetOption(TO_HELL_AUTO_CLOSE))
			{
				hellAutoClose = 1;
			}
		}
		player->SendDungeonStartUdpLog(GetDungeonID(), GetDungeonData()->type, GetDungeonData()->name, m_hellHardMode, IsTeamMode(), dungeonPlayer->GetDungenAdditionByType(DART_FRIEND_EXP_RATE) > 0 ? 1 : 0, hellAutoClose);

		OnPlayerEnterRace(player);

		CLProtocol::WorldDungeonEnterRaceRes enterRes;
		enterRes.result = ErrorCode::SUCCESS;
		player->SendProtocol(enterRes);
	}

	{
		// 发送初始化消息到relay
		CLProtocol::SceneDungeonStartRes res;
		_GetStartInfo(NULL, racePlayerInfoes, addr, res);
		res.guildDungeonInfo = guildDungeonInfo;
		res.zjslDungeonInfo = m_ZjslDungeonInfo;

		if (m_sessionId > 0)
		{
			CLProtocol::RelaySvrRaceInitPacket notify;
			notify.raceId = m_sessionId;
			if (res.Encode(notify.packet))
			{
				Router::SendToRelay(m_relayServerId, notify);
			}
		}
		else
		{
			if (_IsNeedVerify())
			{
				// 单人的情况下，由world来创建验证任务
				auto node = VerifyServerMgr::Instance()->SelectNode();
				if (node)
				{
					CLProtocol::VerifyCreateTaskReq req;
					res.session = GetID();
					req.raceId = GetID();
					req.raceType = (UInt8)RT_DUNGEON;
					if (res.Encode(req.initPacket))
					{
						node->SendProtocol(req);
					}
				}
				else
				{
					ErrorLogStream << "race(" << GetID() << ") no useful verify node." << LogStream::eos;
				}
			}
		}
	}

	return ErrorCode::SUCCESS;
}

void DungeonRace::GenDropStatistic(UInt32 times)
{
	Avalon::SockAddr addr;
	SetDropCountState(false);
	StartRace(0, addr, 0);

	if (GetDungeonData()->rollDropId > 0)
	{
		m_allDropSet.push_back(GetDungeonData()->rollDropId);
	}

	auto player = m_players.begin()->second;

	for (UInt32 i = 0; i < times; i++)
	{
		for (auto dropSet : m_allDropSet)
		{
			auto addition = player->GetDungeonAddition();
			GenDropItemResult genRet = DropItemMgr::Instance()->GenDropItems(m_dungeonId, 0, dropSet, player->GetVipLvl(),  player->GetOccu(), player->GetTaskList(), addition, m_OpenedActivityList, player->GetExtra());
			for (auto drop : genRet.dropItems)
			{
				DropKey key(drop.typeId, drop.equipType);
				auto& info = m_DropCount[key];
				info.num += drop.num;
				info.times++;
			}
		}

		UInt8 multi = 1;
		UInt8 finalMulti = _GetRaceEndDropBaseMulti(player) * multi;
		auto result = DropItemMgr::Instance()->GenRaceEndDrop(player->GetID(), m_dungeonId, player->GetLevel(), player->GetDungeonAddition(), m_OpenedActivityList, player->GetExtra());

		std::map<DropKey, ItemReward> items;
		for (auto item : result.dropItems)
		{
			DropKey key(item.typeId, item.equipType);
			auto& reward = items[key];
			reward.id = item.typeId;
			reward.num += item.num * finalMulti;
		}
		for (auto item : items)
		{
			DropKey key(item.first);
			auto& info = m_DropCount[key];
			info.num += item.second.num;
			info.times++;
		}
	}
	
}

void DungeonRace::RecordDropCountToFile(WSPlayer* player, UInt32 dungenId, UInt8 isHell, UInt32 times)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	std::string rootpath = ROOT_PATH;
	std::string accidStr = Avalon::StringUtil::ConvertToString(player->GetAccId());
	std::string dungenIdStr = Avalon::StringUtil::ConvertToString(dungenId);
	std::string isHellStr = Avalon::StringUtil::ConvertToString(isHell);
	std::string timesStr = Avalon::StringUtil::ConvertToString(times);
	std::string hellMode = Avalon::StringUtil::ConvertToString((UInt8)m_hellHardMode);

	std::string postfix = accidStr + "_" + dungenIdStr + "_" + isHellStr + "_" + timesStr + "_" + hellMode + ".csv";
	std::string filename = rootpath + "TropItemCount/" + "TropItemCount_" + postfix;

	std::ofstream ofs;
	ofs.open(filename.c_str());
	if (!ofs.good())
	{
		ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
		return;
	}

	for (auto elem : m_DropCount)
	{
		std::ostringstream oss;
		oss << elem.first.itemTypeId << ", " << (UInt32)elem.first.equipType << ", " << elem.second.times << ", " << elem.second.num << "\n";
		std::string line = oss.str();
		ofs.write(line.c_str(), line.size());
	}

	ofs.close();
}

void DungeonRace::OnCreateVerifyReturn(UInt32 nodeid)
{
	m_VSId = nodeid;
	_StartVerify();
}

void DungeonRace::OnVerifySuccess(ObjID_t roleId)
{
	
}

bool DungeonRace::_IsNeedVerify()
{
	if (!WSApplication::Instance()->IsOpenVerifyDungeon())
	{
		return false;
	}

	auto data = DungeonVerifyProbDataEntryMgr::Instance()->FindEntry(m_dungeonId);
	if (!data)
	{
		// 如果找不到，一定要验证
		return true;
	}

	if (Avalon::Random::SelectInHundred(data->prob))
	{
		return true;
	}

	return false;
}

void DungeonRace::_StartVerify()
{
	// 开始验证
	DebugLogStream << "race(" << GetID() << ") start verify..." << LogStream::eos;

	auto playerItr = m_players.begin();
	if (playerItr == m_players.end() || !playerItr->second)
	{
		ErrorLogStream << "race(" << GetID() << ") invalid player info." << LogStream::eos;
		// 这种情况就不用验了
		m_VSId = 0;
		return;
	}
	ObjID_t roleId = playerItr->second->GetID();

	// 先把之前产生的帧发到验证服务器
	{
		CLProtocol::VerifyReportFrame report;
		report.raceId = GetID();

		for (auto& frame : m_frames)
		{
			report.frames.push_back(frame);

			// 一次只发部分帧
			if (report.frames.size() == 1000)
			{
				_SendToVerifyServer(report);
				report.frames.clear();
			}
		}

		if (!report.frames.empty())
		{
			_SendToVerifyServer(report);
		}
	}

	// 上传校验帧
	{
		CLProtocol::VerifyReportChecksum report;
		report.raceId = GetID();

		PlayerChecksumInfo playerChecksum;
		playerChecksum.roleId = roleId;
		for (auto& checksum : m_checksums)
		{
			ChecksumInfo info;
			info.frame = checksum.framesequence;
			info.checksum = checksum.checksum;
			playerChecksum.checksums.push_back(info);
		}
		report.playerChecksums.push_back(playerChecksum);

		_SendToVerifyServer(report);
	}
}

void DungeonRace::_SendToVerifyServer(Avalon::Protocol& protocol)
{
	if (m_VSId == 0)
	{
		return;
	}

	try
	{
		if (!VerifyServerMgr::Instance()->SendProtocol(m_VSId, protocol))
		{
			ErrorLogStream << "race(" << GetID() << ") send packet to verify failed" << LogStream::eos;
		}
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << "race(" << GetID() << ") send packet to verify failed, " << e.what() << LogStream::eos;

		m_VSId = 0;
	}
}

bool DungeonRace::OnPlayerEnterRace(WSPlayer* player)
{
	if (!player)
	{
		return false;
	}

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "race(" << GetID() << ") can't find dungeon player(" << player->GetID() << ")." << LogStream::eos;
		return false;
	}

	if (dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "race(" << GetID() << ") dungeon player(" << player->GetID() << ") is racing." << LogStream::eos;
		return false;
	}

	DebugLogStream << "race(" << GetID() << ") dungeon(" << GetDungeonID() << ") player(" << player->GetID() << ") enter race." << LogStream::eos;

	dungeonPlayer->SetRacing(true);

	CLProtocol::WorldDungeonStartRaceRes res;
	res.roleId = player->GetID();
	res.dungeonId = GetDungeonID();
	res.isHell = m_isHellMode ? 1 : 0;
	res.isAssist = dungeonPlayer->IsAssist() ? 1 : 0;
	res.isContinue = 0;
	res.result = ErrorCode::SUCCESS;
	player->SendToScene(res);

	return true;
}

bool DungeonRace::OnPlayerRaceEnd(WSPlayer* player, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16], UInt32 lastFrame, 
									UInt32 maxDamage, UInt32 skillId, UInt32 param, UInt64 totalDamage, UInt64 bossDamage, UInt32 playerRemainHp, 
									UInt32 playerRemainMp, UInt32 boss1ID, UInt32 boss2ID, UInt32 boss1RemainHp, UInt32 boss2RemainHp, UInt32 lastChecksum)
{
	if (!player)
	{
		return false;
	}

	DebugLogStream << "player(" << player->GetID() << ") dungeon(" << GetDungeonID() << ") race(" 
		<< GetID() << ") request to race end lastFrame(" << lastFrame << ") recvLastFrame(" 
		<< m_lastFrame << ") lastChecksum(" << lastChecksum << ")." << LogStream::eos;

	// 补上最后一帧对应的随机数, 只有当最后一帧不上随机数上传间隔帧数(50帧)的倍数时需要
	//	1. 最后一帧是250帧，就不需要
	//	2. 最后一帧是249帧，就需要补上249真对应的随机数
	if (WSApplication::Instance()->IsOpenDungeonRaceEndChecksum() && _IsVerifying() && m_sessionId == 0 && lastFrame % CHECKSUM_INTERVAL != 0)
	{
		InfoLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") send last frame(" << lastFrame 
						<< ") checksum(" << lastChecksum << ") to verify server." << LogStream::eos;

		ChecksumInfo checksum;
		checksum.frame = lastFrame;
		checksum.checksum = lastChecksum;

		PlayerChecksumInfo info;
		info.roleId = player->GetID();
		info.checksums.push_back(checksum);

		CLProtocol::VerifyReportChecksum reportChecksums;
		reportChecksums.raceId = GetID();
		reportChecksums.playerChecksums.push_back(info);
		_SendToVerifyServer(reportChecksums);
	}

	GameInfoLogStream("Damage") << "race(" << GetID() << ") dungeon(" << GetDungeonID() << ") player(" << player->GetID() << ") level(" 
		<< player->GetLevel() << ") occu(" << player->GetOccu() << ") usedTime(" << usedTime << ") maxDamage(" << maxDamage << ") skill(" 
		<< skillId << ") param(" << param << ") totalDamage(" << totalDamage << ") bossDamage(" <<bossDamage << ") playerRemainHp(" << playerRemainHp 
		<< ") playerRemainMp(" << playerRemainMp << ") boss1ID(" << boss1ID << ") boss2ID(" << boss2ID << ") boss1RemainHp(" << boss1RemainHp
		<< ") boss2RemainHp(" << boss2RemainHp << ")." << LogStream::eos;

	if (_IsVerifying())
	{
		CLProtocol::RelaySvrDungeonRaceEndReq raceend;
		raceend.raceEndInfo.gamesessionId = GetID();
		raceend.raceEndInfo.dungeonId = GetDungeonID();
		raceend.raceEndInfo.usedTime = usedTime;

		DungeonPlayerRaceEndInfo info;
		info.roleId = player->GetID();
		info.pos = 0;
		info.score = score;
		memset(info.md5, 0, sizeof(info.md5));
		info.beHitCount = beHitCount;
		info.bossDamage = bossDamage;
		info.playerRemainHp = playerRemainHp;
		info.playerRemainMp = playerRemainMp;
		info.boss1ID = boss1ID;
		info.boss2ID = boss2ID;
		info.boss1RemainHp = boss1RemainHp;
		info.boss2RemainHp = boss2RemainHp;

		raceend.raceEndInfo.infoes.push_back(info);

		// 上报到验证服务器
		CLProtocol::VerifyReportRaceEnd report;
		report.raceId = GetID();
		report.roleId = player->GetID();
		if (raceend.Encode(report.packet))
		{
			_SendToVerifyServer(report);
		}
	}

	if (m_sessionId == 0 && (lastFrame == 0 || m_lastFrame != lastFrame))
	{
		DebugLogStream << "player(" << player->GetID() << ") dungeon race(" << GetID() << ") invalid frame, lastFrame(" << m_lastFrame << ") needLastFrame(" << lastFrame << ")." << LogStream::eos;
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_LAST_FRAME_ERR);
		if (WSApplication::Instance()->GetCheatPunish() > CHEAT_PUNISH_LOG)
		{
			return false;
		}
	}

	DungeonPlayer* dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << player->GetID() << ")." << LogStream::eos;
		return false;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is not racing." << LogStream::eos;
		return false;
	}

	if (!_CheckRaceEndRequestMd5(dungeonPlayer, score, beHitCount, usedTime, md5))
	{
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_RACE_END_REQUEST_MD5_ERR);
		return false;
	}

	/*if (dungeonPlayer->IsLeave())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is already leave." << LogStream::eos;
		return false;
	}*/

	if (dungeonPlayer->HasRaceEnd())
	{
		ErrorLogStream << "player(" << player->GetID() << ") dungeon race(" << GetID() << ") repeat race end." << LogStream::eos;
		return false;
	}

	if (score > LS_C && !_CheckDungeonUsedTime(dungeonPlayer, 0, CURRENT_TIME.MSec() - m_startTime * Avalon::Time::MSECS_OF_SEC))
	{
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_DUNGEON_USED_TIME_ABNOR);
		if (WSApplication::Instance()->GetCheatPunish() > CHEAT_PUNISH_LOG)
		{
			return false;
		}
	}

	if (score > LS_C)
	{
		if (m_dungeonData->costFatiguePerArea > 0 && m_dungeonData->clearMinAreaNum > dungeonPlayer->GetEnteredAreaCount())
		{
			UInt16 rmFagiue = (m_dungeonData->clearMinAreaNum - dungeonPlayer->GetEnteredAreaCount()) * m_dungeonData->costFatiguePerArea;
			UInt32 addRate = dungeonPlayer->GetDungenAdditionByType(DART_FATIGUE_BURNING_RATE);
			if (addRate > 0)
			{
				rmFagiue *= ((UInt16)addRate + 100) / 100;
			}

			CLProtocol::WorldDungeonNotifyConsumeFatigue consume;
			consume.roleId = dungeonPlayer->GetID();
			consume.value = rmFagiue;
			consume.dungeonDataId = m_dungeonData->id;
			player->SendToScene(consume);
		}

		// 清除玩家隐藏房间标记
		_ClearPlayerHiddenRoomFlag();
	}

	// 死亡之塔的时间是之前计算好的
	if (m_dungeonData && m_dungeonData->type != LT_TOWER)
	{
		dungeonPlayer->AddUsedTime(usedTime);
	}

	// 结算经验
	DungeonAdditionInfo additionInfo;
	UInt32 raceEndExp = _GetRaceEndExpReward(dungeonPlayer, score, additionInfo);

	// 计算VIP金币加成数值
	UInt32 totalGoldDrop = dungeonPlayer->GetGoldDropNum();
	//auto totalGoldAddRate = dungeonPlayer->GetGoldAddition();
	//auto vipGoldAddRate = dungeonPlayer->GetVipGoldAddition();
	auto totalGoldAddRate = dungeonPlayer->GetDungenAdditionByType(DART_GOLD_RATE);
	auto vipGoldAddRate = dungeonPlayer->GetDungenAdditionByType(DART_VIP_GOLD_RATE);
	UInt32 vipGoldDropNum = totalGoldDrop * 100 / (100 + totalGoldAddRate) * vipGoldAddRate / 100;
	additionInfo.addition[DUNGEON_GOLD_ADD_VIP] = vipGoldDropNum;

	// 计算组队奖励
	ItemReward teamReward = _GenTeamReward(player, score);

	// 月卡额外金币奖励
	UInt32 monthCardGoldReward = 0;
	if (m_dungeonData && m_dungeonData->subType != DST_GOLD && player->IsInMonthCard())
	{
		UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_RACE_GOLD_RATE);
		monthCardGoldReward = rate * dungeonPlayer->GetBindGoldDropNum() / 100;
	}

	// 金币称号额外金币奖励
	UInt32 goldTitleGoldReward = 0;
	if (m_dungeonData && m_dungeonData->subType != DST_GOLD && dungeonPlayer->HasGoldTitle())
	{
		UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLD_TITLE_DUNGEON_GOLD_RATE);
		goldTitleGoldReward = rate * dungeonPlayer->GetBindGoldDropNum() / 100;
	}

	// 疲劳燃烧加成
	UInt32 fatigueBurnAdd = (100 + dungeonPlayer->GetDungenAdditionByType(DART_FATIGUE_BURNING_RATE)) / 100;
	raceEndExp *= fatigueBurnAdd;
	for (UInt32 addType = (UInt32)DUNGEON_EXP_ADD_BUFF; addType < (UInt32)DUNGEON_ADD_NUM; ++addType)
	{
		additionInfo.addition[(DungeonAdditionType)addType] *= fatigueBurnAdd;
	}
	//monthCardGoldReward *= fatigueBurnAdd;
	//goldTitleGoldReward *= fatigueBurnAdd;

	// 计算老兵回归奖励
	ItemReward verteranReturnDropReward = _GenVeteranReturnDropReward(player, score);

	// 通知到Scene
	CLProtocol::WorldDungeonNotifyRaceEnd notify;
	notify.raceId = GetID();
	notify.cityMonster = GetCityMonster();
	notify.roleId = player->GetID();
	notify.dungeonId = GetDungeonID();
	notify.raceEndExp = raceEndExp;
	notify.endAreaId = dungeonPlayer->GetCurAreaID();
	notify.score = score;
	notify.beHitCount = beHitCount;
	notify.enteredAreaCount = dungeonPlayer->GetEnteredAreaCount();
	notify.usedTime = dungeonPlayer->GetUsedTime();
	notify.reviveCount = dungeonPlayer->GetReviveCount();
	notify.teamReward = teamReward;
	notify.isAssist = dungeonPlayer->IsAssist();
	notify.monthcardGoldReward = monthCardGoldReward;
	notify.goldTitleGoldReward = goldTitleGoldReward;
	notify.mysticalMerchantId = score > LS_C ? m_MysticalMerchantId : 0;
	if (verteranReturnDropReward.id > 0)
	{
		notify.raceEndRewards.push_back(verteranReturnDropReward);
	}
	if (m_players.size() > 1)
	{
		for (auto itr : m_players)
		{
			auto other_player = itr.second;
			if (other_player && other_player->GetID() != player->GetID())
			{
				notify.teammateIds.push_back(other_player->GetID());
			}
		}
	}
	notify.playerRemainHp = playerRemainHp;
	notify.playerRemainMp = playerRemainMp;
	notify.boss1ID = boss1ID;
	notify.boss2ID = boss2ID;
	notify.boss1RemainHp = boss1RemainHp;
	notify.boss2RemainHp = boss2RemainHp;
	player->SendToScene(notify);

	// 地下城简单防外挂
	if (score > LS_C)
	{
		auto accountInfo = AccountMgr::Instance()->FindAccount(player->GetAccId());
		if (accountInfo)
		{
			auto clearTimeDataEntry = DungeonClearTimeDataEntryMgr::Instance()->FindEntry(GetDungeonID());
			auto roleValueScore = accountInfo->FindRoleValueScoreInfo(player->GetID());
			if (clearTimeDataEntry && roleValueScore && clearTimeDataEntry->score > roleValueScore->roleValueScore)
			{
				player->SendUdpLog("dungeon_clear_time", LOG_DUNGEON_CLEAR_TIME, GetDungeonID(), score, usedTime, roleValueScore->roleValueScore, clearTimeDataEntry->score);
			}
		}
	}

	dungeonPlayer->OnRaceEnd(score);

	if (score > LS_C)
	{
		_GenRollDrops();
	}

	CLProtocol::SceneDungeonRaceEndRes res;
	res.score = score;
	res.usedTime = dungeonPlayer->GetUsedTime();
	res.result = ErrorCode::SUCCESS;
	res.killMonsterTotalExp = dungeonPlayer->KillMonsterTotalExp();
	res.raceEndExp = raceEndExp;
	res.addition = additionInfo;
	res.teamReward = teamReward;
	res.hasRaceEndChest = 0;
	res.monthcartGoldReward = monthCardGoldReward;
	res.goldTitleGoldReward = goldTitleGoldReward;
	res.veteranReturnReward = verteranReturnDropReward;

	res.fatigueBurnType = FBT_NONE;
	if (fatigueBurnAdd > 1)
	{
		res.fatigueBurnType = FBT_COMMON;

		if (dungeonPlayer->GetDungeonAddition().dropAddition[DIT_CHEST] > 0)
		{
			res.fatigueBurnType = FBT_ADVANCED;
		}
	}

	// 助战模式下没有结算掉落
	if (!dungeonPlayer->IsAssist() && score > LS_C && DropItemMgr::Instance()->HasRaceEndDrop(m_dungeonId, dungeonPlayer->GetLevel()))
	{
		res.hasRaceEndDrop = 1;
		res.raceEndDropBaseMulti = _GetRaceEndDropBaseMulti(dungeonPlayer);
	}
	else
	{
		res.hasRaceEndDrop = 0;
		res.raceEndDropBaseMulti = 0;
	}

	// 没有结算掉落就计算结算宝箱
	if (score > LS_C)
	{
		_GenChest(player);
		auto task = _FindDungeonChestTask(player->GetID());
		if (task)
		{
			res.hasRaceEndChest = 1;
			res.chestDoubleFlag = (UInt8)dungeonPlayer->GetDungenAdditionByType(DART_GUILD_TERR_CHEST_DOUBLE_FLAG);

			// 工会领地翻牌加倍了，要通知到Scene
			if (dungeonPlayer->GetDungenAdditionByType(DART_GUILD_TERR_CHEST_DOUBLE_FLAG) > 0)
			{
				player->SyncEventToScene(SET_GUILD_TERR_CHEST_DOUBLE);
			}
		}

		// 如果是每日必掉的图，要记录下打过的次数
		auto normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);
		auto dailyDropData = DungeonDailyDropDataEntryMgr::Instance()->FindEntry(normalHardDungeonId);
		if (dailyDropData && (!WSApplication::Instance()->IsNewDailyDrop() || !dungeonPlayer->IsRecordDailyDropNum()))
		{
			auto normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);
			UInt32 oldNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId);
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId, 1);
			UInt32 newNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId);
			InfoLogStream << "race(" << GetID() << ") player(" << player->GetID() << ", " << normalHardDungeonId << ") daily drop num(" << oldNum << ") => (" << newNum << ")." << LogStream::eos;
			dungeonPlayer->SetRecordDailyDropNum();
		}
		
	}

	//有公共掉落
	if (GetDungeonData()->rollDropId > 0 && score > LS_C)
	{
		//多人roll
		if (!m_isSingleRoll)
		{
			if (!dungeonPlayer->IsAssist())
			{
				for (auto itr : m_rollItems)
				{
					res.rollReward.push_back(itr.second.ToClient());
				}
			}
		}
		else
		{
			//只有一个人消耗精力,公共掉落直接拾取
			ObjID_t playerId = 0; 
			if (_GetOnlyNotAssistPlayer(playerId) && playerId == player->GetID())
			{
				_SettleRollSingleMode();
				for (auto itr : m_rollItems)
				{
					DropItem& dropItem = itr.second.dropItem;
					res.rollSingleReward.push_back(ItemReward(dropItem.typeId, dropItem.num, 0, dropItem.strenth, 0, dropItem.equipType, 0));
				}
			}
		}
	}
	
	player->SendProtocol(res);

	// 发送通关房间日志
	player->SendDungeonClearAreaUdpLog(m_dungeonId, m_dungeonData->type, m_dungeonData->name, m_hellHardMode, notify.endAreaId, CURRENT_TIME.Sec() - dungeonPlayer->GetEnterAreaTime());

	//Sys::SendUdpLog("dungeon", LOG_DUNGEON_FORMAT, dungeonPlayer->GetZoneID(), (UInt32)CURRENT_TIME.Sec(), GetID(), GetDungeonID(), m_dungeonData->hardType,
	//	m_hellHardMode, dungeonPlayer->GetID(), dungeonPlayer->GetAccID(), dungeonPlayer->GetLevel(), (UInt32)score, usedTime);

	// 神秘商人
	UInt32 shopId = 0;
	auto mysticalMerchantData = MysticalMerchantDataEntryMgr::Instance()->FindEntry(m_MysticalMerchantId);
	if (mysticalMerchantData != NULL)
	{
		shopId = mysticalMerchantData->shopId;
	}

	{
		// 结算日志
		UInt32 hpItemIdNum = dungeonPlayer->GetBufferItemUsedNum(200001001);
		UInt8 hellAutoClose = 0;
		if (GetTeamID())
		{
			auto team = WSTeamMgr::Instance()->FindTeam(GetTeamID());
			if (team && team->IsSetOption(TO_HELL_AUTO_CLOSE))
			{
				hellAutoClose = 1;
			}
		}

		Sys::SendUdpLog("dungeon", LOG_DUNGEON_FORMAT, GAME_VERSION.c_str(), dungeonPlayer->GetZoneID(), (UInt32)CURRENT_TIME.Sec(), player->GetPf().c_str(), player->GetIp().c_str(), dungeonPlayer->GetID(), dungeonPlayer->GetAccID(),
			dungeonPlayer->GetLevel(), player->GetVipLvl(), player->GetOccu(), GetID(), GetDungeonID(), GetDungeonData()->type, GetDungeonData()->name.c_str(), m_dungeonData->hardType,
			m_hellHardMode, CURRENT_TIME.Sec() - m_startTime, dungeonPlayer->GetLevelScore(), dungeonPlayer->GetReviveCount(), (UInt8)dungeonPlayer->IsAssist(), shopId, IsTeamMode(), hpItemIdNum,
			hellAutoClose);
	}
	

	// 发送战斗结束消息到scene
	if (m_HasSendRaceOver == false)
	{
		m_HasSendRaceOver = true;

		CLProtocol::WorldTeamDungeonRaceOver over;
		over.raceId = GetID();
		over.teamId = GetTeamID();
		over.cityMonster = GetCityMonster();
		Router::BroadcastToScene(over);
	}

	if (m_players.size() > 1 && m_bSendMasterDiscipleInteractionLog == false)
	{
		m_bSendMasterDiscipleInteractionLog = true;
		SendMasterDiscipleInteractionLog();
	}

	if (m_dungeonData->subType == DST_GUILD_DUNGEON)
	{
		if (bossDamage == 0)
		{
			ErrorLogStream <<"Guild Dungeon RaceId:"<<m_id<<" DungeonId:"<<GetDungeonID()<<" BossDamage 0"<< LogStream::eos;
		}

		GuildDungeonMgr::Instance()->AddJoinPlayer(player->GetGuildID(), player->GetID());

		// 记录公会地下城伤害
#ifdef _DEBUG
			UInt64 tempVal = player->GetGuildDungeonDebugDamage();

			InfoLogStream << "GuildDungeon DEBUG Damage RaceId:"<<GetID()<<" PlayerId:"<<player->GetID()<<" playerName:"<<player->GetName() << " debugVal:"<<tempVal<<" bossDamage:"<<bossDamage << LogStream::eos;

			if (tempVal == 0)
				tempVal = bossDamage;

			// 大boss，需要走验证
			GuildDungeonVerifyMgr::Instance()->AddVerifyInfo(GetID(), player->GetID(), tempVal, player->GetGuildID(), GetDungeonID(), usedTime, player->GetName(), player->GetOccu());
#else
			// 大boss，需要走验证
			GuildDungeonVerifyMgr::Instance()->AddVerifyInfo(GetID(), player->GetID(), bossDamage, player->GetGuildID(), GetDungeonID(), usedTime, player->GetName(), player->GetOccu());
#endif
		
		const GuildDungeonCfg* cfg =  GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(GetDungeonID());
		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (cfg != NULL && guild != NULL)
		{
			UInt64 oddBlood = GuildDungeonMgr::Instance()->GetBossOddBlood(guild->GetID(), GetDungeonID());

			player->SendUdpLog("guild_dungeon", LOG_GUILD_DUNGEON, GetDungeonID(), GetDungeonData()->name.c_str(), cfg != NULL ? cfg->dungeonLvl : 0, score, totalDamage, bossDamage, usedTime, dungeonPlayer->GetReviveCount(), IsTeamMode(), guild->GetName().c_str(), guild->GetLevel(), oddBlood > bossDamage ? oddBlood - bossDamage : 0);
		}

		if (!m_flagGuildDungeon && dungeonPlayer->GetLevelScore() > LS_C)
		{
			m_flagGuildDungeon = true;
			GuildDungeonMgr::Instance()->AddGuildDungeonBattleCnt(player->GetGuildID(), GetDungeonID());
 		}
	}

	player->OnPlayerClearDungeon(m_dungeonData, score, usedTime, dungeonPlayer->IsAssist());

	return true;
}

void DungeonRace::OnPlayerKilledMonster(WSPlayer* player, std::vector<UInt32>& monsterIds)
{
	if (!player)
	{
		return;
	}

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << player->GetID() << ")." << LogStream::eos;
		return;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is not racing." << LogStream::eos;
		return;
	}

	CLProtocol::SceneDungeonKillMonsterRes res;

	for (auto monsterId : monsterIds)
	{
		auto monster = _FindMonster(monsterId);
		if (!monster)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to kill unexist monster(" << monsterId << ")." << LogStream::eos;
			continue;
		}

		if (!_OnPlayerKillMonster(player, dungeonPlayer, monster))
		{
			continue;
		}

		res.monsterIds.push_back(monsterId);
	}

	player->SendProtocol(res);
}

void DungeonRace::OnPlayerClearAreaMonsters(WSPlayer* player, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16], UInt32 lastFrame)
{
	if (!player)
	{
		return;
	}

	// 只有死亡之塔需要记录这个东西
	if (!m_dungeonData || m_dungeonData->type != LT_TOWER)
	{
		return;
	}

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << player->GetID() << ")." << LogStream::eos;
		return;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is not racing." << LogStream::eos;
		return;
	}

	if (m_sessionId == 0 && (lastFrame == 0 || m_lastFrame < lastFrame))
	{
		DebugLogStream << "player(" << player->GetID() << ") dungeon race(" << GetID() << ") invalid frame, lastFrame(" << m_lastFrame << ") needLastFrame(" << lastFrame << ")." << LogStream::eos;
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_LAST_FRAME_ERR);
		return;
	}

	if (!_CheckClearAreaRequestMd5(dungeonPlayer, usedTime, remainHp, remainMp, md5))
	{
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_CLEAT_AREA_REQUEST_MD5_ERR);
		return;
	}

	UInt32 curAreaId = dungeonPlayer->GetCurAreaID();
	if (dungeonPlayer->HasGotAreaReward(dungeonPlayer->GetCurAreaID()))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") dungeon player(" << player->GetID() << ") has got area(" << curAreaId << ") reward." << LogStream::eos;
		return;
	}

	dungeonPlayer->SetGotAreaReward(dungeonPlayer->GetCurAreaID());
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") clear area(" << dungeonPlayer->GetCurAreaID() << ") usedTime("
		<< usedTime << ") remainHp(" << remainHp << ") remainMp(" << remainMp << ")." << LogStream::eos;

	auto floor = _GetTowerFloorByAreaID(m_dungeonId, curAreaId);
	if (floor % 5 == 1)
	{
		dungeonPlayer->SetUsedTime(0);
	}

	dungeonPlayer->AddUsedTime(usedTime);
	dungeonPlayer->SetRemainHp(remainHp);
	dungeonPlayer->SetRemainMp(remainMp);

	if (!_CheckDungeonUsedTime(dungeonPlayer, floor, dungeonPlayer->GetUsedTime()))
	{
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_DUNGEON_USED_TIME_ABNOR);
		if (WSApplication::Instance()->GetCheatPunish() > CHEAT_PUNISH_LOG)
		{
			return;
		}
	}

	auto deathFloorData = DeathTowerAwardDataEntryMgr::Instance()->FindEntry(floor);
	if (!deathFloorData)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") invalid floor(" << floor << ") can't find data." << LogStream::eos;
		return;
	}

	if (deathFloorData->levelLimit > player->GetLevel())
	{
		// 等级限制
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") invalid floor(" << floor << ") need level(" << deathFloorData->levelLimit << ")." << LogStream::eos;
		return;
	}

	CLProtocol::WorldDungeonNotifyClearAreaMonster notify;
	notify.roleId = player->GetID();
	notify.dungeonId = GetDungeonID();
	notify.areaId = curAreaId;
	notify.usedTime = dungeonPlayer->GetUsedTime();
	notify.remainHp = remainHp;
	notify.remainMp = remainMp;
	player->SendToScene(notify);
}

UInt32 DungeonRace::OnPlayerEnterNextArea(WSPlayer* player, UInt32 areaId, UInt32 lastFrame, const DungeonStaticValue& staticVal)
{
	if (!player)
	{
		return ErrorCode::DUNGEON_ENTER_AREA_SYSTEM_ERROR;
	}

#ifdef _VERIFY_LOG_
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") request enter next area(" << areaId << ") lastFrame(" << lastFrame << ")." << LogStream::eos;
#endif

	static int s_values[] = { 1, 2, 5, 10, 50, 60, 100, 150, 200, 250, 500, 700, 1000, 1500, 2000, 3600, 5000, 10000,
		100000, 99999, 20000, 4000, 3000, 400, 360, 300, 180, 30, 3 };
	// 检测静态数值是不是被改变了
	if (!staticVal.IsValid(s_values, sizeof(s_values) / sizeof(s_values[0])))
	{
		// 作弊了
		ErrorLogStream << "dungeon(" << GetID() << ") player(" << player->GetID() << ") invalid static value(" << staticVal.ToString() << ")." << LogStream::eos;

		if (WSApplication::Instance()->KickWhenStaticValueCheckError())
		{
			player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);
			player->SendCheatLog(PLAYER_CHEAT_REASON_STATIC_VALUE_INVALID, GetID(), GetDungeonID());
		}

		return ErrorCode::DUNGEON_ENTER_AREA_SYSTEM_ERROR;
	}

	ObjID_t roleId = player->GetID();
	DungeonPlayer* dungeonPlayer = FindDungeonPlayer(roleId);
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << roleId << ")" << LogStream::eos;
		return ErrorCode::DUNGEON_ENTER_AREA_SYSTEM_ERROR;
	}

	if (dungeonPlayer->IsLeave())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") is already leave." << LogStream::eos;
		return ErrorCode::DUNGEON_ENTER_AREA_NOT_IN_DUNGEON;
	}

	UInt32 curAreaId = dungeonPlayer->GetCurAreaID();
	if (curAreaId == areaId)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") is already in area(" << areaId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_ENTER_AREA_REPEAT;
	}

	DungeonArea* curArea = _FindArea(curAreaId);
	if (!curArea)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << roleId << ")'s cur area(" << curAreaId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_ENTER_AREA_NOT_EXIST;
	}

	DungeonArea* nextArea = _FindArea(areaId);
	if (!nextArea)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << roleId << ")'s next area(" << areaId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_ENTER_AREA_NOT_EXIST;
	}
	
	bool checkAddIntimacy = false;
	if (!dungeonPlayer->HasClearedArea(dungeonPlayer->GetCurAreaID()))
	{
		// 第一次通关这个房间
		// 发送通关房间日志
		player->SendDungeonClearAreaUdpLog(m_dungeonId, m_dungeonData->type, m_dungeonData->name, m_hellHardMode, dungeonPlayer->GetCurAreaID(), CURRENT_TIME.Sec() - dungeonPlayer->GetEnterAreaTime());
		checkAddIntimacy = true;
	}

	if (!dungeonPlayer->HasEnteredArea(areaId))
	{
		// 第一次进入这个area
		// 扣体力

		if (!dungeonPlayer->IsAssist() && m_dungeonData->costFatiguePerArea > 0)
		{
			UInt16 rmFagiue = m_dungeonData->costFatiguePerArea;
			UInt32 addRate = dungeonPlayer->GetDungenAdditionByType(DART_FATIGUE_BURNING_RATE);
			if (addRate > 0)
			{
				rmFagiue *= ((UInt16)addRate + 100) / 100;
			}

			CLProtocol::WorldDungeonNotifyConsumeFatigue notify;
			notify.roleId = player->GetID();
			notify.value = rmFagiue;
			notify.dungeonDataId = m_dungeonData->id;
			player->SendToScene(notify);
		}

		// 过门时检测是不是收到了所有帧
		if (WSApplication::Instance()->CheckEnterAreaFrame() && m_sessionId == 0 && dungeonPlayer->GetEnteredAreaCount() >= WSApplication::Instance()->GetCheckAreaStartIndex())
		{
			// 过门时候的帧数肯定不会是0
			if (lastFrame == 0 || m_lastFrame < lastFrame)
			{
				DebugLogStream << "player(" << player->GetID() << ") dungeon race(" << GetID() << ") invalid frame, lastFrame(" << m_lastFrame << ") needLastFrame(" << lastFrame << ")." << LogStream::eos;
				player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);
				player->SendCheatLog(PLAYER_CHEAT_REASON_LAST_FRAME_ERR, GetID(), GetDungeonID());
				return ErrorCode::DUNGEON_ENTER_AREA_NOT_EXIST;
			}
		}
	}

	dungeonPlayer->OnEnterArea(areaId);
	if (checkAddIntimacy && m_teamId > 0)
	{
		checkAndAddIntimacy(curAreaId);
	}
#ifdef _VERIFY_LOG_
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") leave area("
		<< curAreaId << ") enter next area(" << areaId << ")." << LogStream::eos;
#endif

	return ErrorCode::SUCCESS;
}

void DungeonRace::OnPlayerPickUpDropItems(WSPlayer* player, UInt32 lastFrame, std::vector<UInt32>& dropItemIds, UInt8 md5[16])
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneDungeonRewardRes res;
	ObjID_t roleId = player->GetID();

	DungeonPlayer* dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << player->GetID() << ")." << LogStream::eos;
		return;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is not racing." << LogStream::eos;
		return;
	}

	if (WSApplication::Instance()->CheckPickDropFrame() && !_CheckDropRequestMd5(dungeonPlayer, lastFrame, dropItemIds, md5))
	{
		DebugLogStream << "player(" << player->GetID() << ") dungeon race(" << GetID() << ") invalid md5." << LogStream::eos;
		player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);
		player->SendCheatLog(PLAYER_CHEAT_REASON_PICK_UP_DROP_REQUEST_MD5_ERR, GetID(), GetDungeonID());
		return;
	}

	ItemRewardVec itemReward;

	for (auto itemId : dropItemIds)
	{
		auto item = _FindDropItem(itemId, dungeonPlayer, true);
		if (!item)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") request to pick unexist item("
				<< itemId << ")" << LogStream::eos;
			continue;
		}

// 		ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(item->typeId);
// 		if (NULL == itemEntry)
// 		{
// 			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") request to pick not find item("
// 				<< item->typeId << ")" << LogStream::eos;
// 			continue;
// 		}
// 
// 		// 如果掉落是公会地下城buff，则加到公会上
// 		if (itemEntry->type == ITEM_GUILD_BUFF)
// 		{
// 			GuildDungeonMgr::Instance()->AddGuildDungeonDropBuff(player->GetGuildID(), GetDungeonID(), itemId);
// 		}

		/*
		ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
		if (NULL == itemEntry)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") request to pick not find item("
				<< itemId << ")" << LogStream::eos;
			continue;
		}

		// 如果掉落是公会地下城buff，则加到公会上
		if (itemEntry->type == ITEM_GUILD_BUFF)
		{
			GuildDungeonMgr::Instance()->AddGuildDungeonDropBuff(player->GetGuildID(), GetDungeonID(), itemId);
		}
		*/

		if (item->roleId != roleId)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") other player(" << item->roleId
				<< ")'s item(" << itemId << ")" << LogStream::eos;
			continue;
		}

		if (item->isPickUp)
		{
			/*ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") repeat pick up item("
				<< itemId << ")" << LogStream::eos;*/
			continue;
		}
		
		if (item->vipDropLimitId > 0)
		{
			auto remainNum = SysRecordMgr::Instance()->VipLimitItemRemainNum(roleId, item->typeId, item->num, item->vipDropLimitId, player->GetVipLvl());
			if (remainNum == 0 || item->num > remainNum)
			{
				ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") pick up item("
					<< itemId << ") fail, vip limit max" << LogStream::eos;
				continue;
			}
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_ITEM_GET_NUM, SYS_BEHAVIOR_OWNER_ROLE, roleId, item->typeId, item->num);
		}
		// 拾取掉落时检测之前收到的帧数据
		if (WSApplication::Instance()->CheckPickDropFrame() && m_sessionId == 0)
		{
			// 拾取掉落时帧数肯定不会是0
			if (/*lastFrame == 0 || */m_lastFrame < lastFrame)
			{
				DebugLogStream << "player(" << player->GetID() << ") dungeon race(" << GetID() << ") invalid frame, lastFrame(" << m_lastFrame << ") needLastFrame(" << lastFrame << ")." << LogStream::eos;
				player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);
				return;
			}
		}

		// 添加物品
		item->isPickUp = true;

		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(item->typeId);

		if (itemData != NULL && itemData->color == ItemQuality::ITEM_QUALITY_YELLOW)
		{
			player->SendDungeonDropUdpLog(GetDungeonID(), itemData->id, (UInt32)itemData->color, (UInt32)itemData->needLevel);

			//橙色装备
			if (itemData->type == ITEM_EQUIP)
			{
				DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") pick up item("
					<< itemId << ", " << item->typeId << "," << item->num << ")" << ", dungeon sub type : " << m_dungeonData->subType
					<< ", player size : " << UInt8(m_players.size()) << LogStream::eos;

				//深渊
				if (m_dungeonData->subType == DST_HELL || m_dungeonData->subType == DST_HELL_ENTRY)
				{
					//组队
					if (m_players.size() > 1)
					{
						//发通知
						CLProtocol::WorldDungeonNotifyGetYellowEquip notifyMsg;
						notifyMsg.roleId = roleId;
						notifyMsg.itemId = itemData->id;
						_BroadcastProtocol(notifyMsg);
					}
				}
			}			
		}

		if (itemData && (itemData->subType == ST_ITEM_GOLD || itemData->subType == ST_BIND_GOLD))
		{
			dungeonPlayer->AddGoldDrop(item->num);

			if (itemData->subType == ST_BIND_GOLD)
			{
				dungeonPlayer->AddBindGoldDrop(item->num);
			}
		}

		if (itemData && itemData->color >= WSApplication::Instance()->GetNeedRecordDropQuality())
		{
			DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") pick up item("
				<< itemId << ", " << item->typeId << "," << item->num << ")" << LogStream::eos;
		}

		res.pickedItems.push_back(itemId);

		_PushItemToReward(itemReward, item->typeId, item->num, item->strenth, item->equipType);

		if (item->monsterDailyMaxDropNum > 0)
		{
			SetDungeonAreaIndex(dungeonPlayer->GetID(), m_dungeonId, item->monsterDungeonAreaIndex);

			//if (dungeonPlayer->IsAddedDropNumMonster(item->monsterNormalHardId))
			//{
			//	// 已经加过了
			//	InfoLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") has added monster(" << item->monsterNormalHardId << ")'s daily drop num." << LogStream::eos;
			//}
			//else
			//{
			//	UInt32 monsterNormalHardId = item->monsterNormalHardId;
			//	UInt32 dailyDropNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_MONSTER_DROP, dungeonPlayer->GetID(), monsterNormalHardId);
			//	UInt32 addedDropNum = dungeonPlayer->GetAddedDropNum();

			//	// 今天的次数还没用完
			//	if (dailyDropNum < item->monsterDailyMaxDropNum + addedDropNum)
			//	{
			//		SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON_MONSTER_DROP, dungeonPlayer->GetID(), monsterNormalHardId, 1);

			//		InfoLogStream << "race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") add monster(" << monsterNormalHardId << ")'s drop num(" << dailyDropNum + 1 << ")." << LogStream::eos;
			//	}

			//	dungeonPlayer->AddDropNumMonster(monsterNormalHardId);
			//}
		}

		if (WSApplication::Instance()->IsNewDailyDrop() && item->isDailyDrop && !dungeonPlayer->IsRecordDailyDropNum())
		{
			// 如果是每日必掉的道具，要记录下掉落次数
			auto normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId); 
			UInt32 oldNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId);
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId, 1);
			UInt32 newNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId);
			DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ", " << normalHardDungeonId << ") daily drop num(" << oldNum << ") => (" << newNum << ")." << LogStream::eos;
			dungeonPlayer->SetRecordDailyDropNum();
		}
	}

	player->SendProtocol(res);

	if (!itemReward.empty())
	{
		CLProtocol::WorldDungeonNotifyGotItems notify;
		notify.roleId = roleId;
		notify.dungeonId = GetDungeonID();
		notify.items = itemReward;
		player->SendToScene(notify);
	}
}

bool DungeonRace::OnPlayerOpenChest(WSPlayer* player, UInt8 pos)
{
	if (!player)
	{
		return false;
	}

	DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") request to open chest(" << pos << ")." << LogStream::eos;

	if (pos == UINT8_MAX)
	{
		pos = _GenUnopenedChestPos();
	}

	if (pos == UINT8_MAX)
	{
		return false;
	}

	if ((m_openedChestMask & (1 << pos)))
	{
		// 这个格子已经被别的玩家打开了，这里需要通知客户端
		return false;
	}

	DungeonChestType type = _GetChestTypeByPos(pos);

	DungeonChestTask* task = _FindDungeonChestTask(player->GetID());
	if (!task)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") can't find chest task." << LogStream::eos;
		return false;
	}

	if (type == DCT_NORMAL)
	{
		if (!_OpenChest(player, task, DCT_NORMAL, pos))
		{
			InfoLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") open chest(" << pos << ") failed." << LogStream::eos;
			return false;
		}
		task->SetOpenedNormalChest();
		_OpenChest(player, task, DCT_VIP, pos + 8);
	}
	else if (type == DCT_PAY)
	{
		if (task->GetPayChestCost() > 0)
		{
			DungeonRaceCheckTransaction* transaction = new DungeonRaceCheckTransaction(player, GetID(), DUNGEON_RACE_OPEN_PAY_CHEST);
			transaction->SetItem(task->GetPayChestCostItemID(), task->GetPayChestCost());
			transaction->SetParam(pos);
			player->BeginItemTransaction(transaction);

			DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") start to check pay chest, need gold("
				<< task->GetPayChestCost() << ")." << LogStream::eos;
		}
		else
		{
			OnCheckOpenPayChestRet(player, pos);
			player->SyncEventToScene(SET_OPEN_FREE_GOLD_CHEST);
		}

		return true;
	}
	else
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to open unknown chest pos(" << pos << ")." << LogStream::eos;
		return false;
	}

	m_openedChestMask |= 1 << pos;

	return true;
}

bool DungeonRace::OnPlayerRequestRaceEndDrop(WSPlayer* player, UInt8 multiple)
{
	if (!player)
	{
		return false;
	}

	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") level(" << player->GetLevel() << ") request race end drop, dungeon("
		<< m_dungeonId << ") multi(" << multiple << ")." << LogStream::eos;

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << player->GetID() << ")." << LogStream::eos;
		return false;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is not racing." << LogStream::eos;
		return false;
	}

	if (dungeonPlayer->GetLevelScore() <= LS_C)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") score(" << (UInt8)dungeonPlayer->GetLevelScore() << ")." << LogStream::eos;
		return false;
	}

	if (dungeonPlayer->HasGotRaceEndDrop())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") repeated to get race end drop." << LogStream::eos;
		return false;
	}

	if (!DropItemMgr::Instance()->HasRaceEndDrop(m_dungeonId, player->GetLevel()))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want get unexist race end drop, dungeon("
			<< m_dungeonId << ") level(" << player->GetLevel() << ")." << LogStream::eos;
		return false;
	}

	auto result = DropItemMgr::Instance()->GenRaceEndDrop(player->GetID(), m_dungeonId, dungeonPlayer->GetLevel(), dungeonPlayer->GetDungeonAddition(), m_OpenedActivityList, dungeonPlayer->GetExtra());
	if (result.dropItems.empty())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") dungeon("
			<< m_dungeonId << ") level(" << player->GetLevel() << ") race end drop item is empty." << LogStream::eos;
		return false;
	}

	for (auto& dropItem : result.dropItems)
	{
		if (dropItem.isRareControl)
		{
			// 记录稀有控制
			SysRecordMgr::Instance()->RecordGotRareItem(player->GetID(), dropItem.typeId, dropItem.num);
		}
	}

	/*if (multiple == 3)
	{
		bool openTriple = false;
		player->GetVipValue(VIP_PRIVILEGE_SOUTH_VALLEY_THREE, openTriple);
		if (!openTriple)
		{
			return false;
		}
	}*/

	UInt32 cost = _MultiRaceEndDropCost(multiple);
	if (cost == UINT32_MAX)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") multi(" << multiple << ") raceEndDrop failed." << LogStream::eos;
		return false;
	}

	if (cost > 0)
	{
		DungeonRaceCheckTransaction* transaction = new DungeonRaceCheckTransaction(player, GetID(), DUNGEON_RACE_MULTI_RACE_END_DROP);
		transaction->SetBindGold(cost);
		transaction->SetParam(multiple);
		player->BeginItemTransaction(transaction);
		return true;
	}

	OnCheckMultiRaceEndDropRet(player, multiple);

	return true;
}

UInt32 DungeonRace::OnPlayerReviveRequest(WSPlayer* player, ObjID_t targetId, UInt32 reviveId, UInt32 reviveCoinNum)
{
	if (!player)
	{
		return ErrorCode::DUNGEON_REVIVE_PLAYER_NOT_EXIST;
	}

	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to revive player("
		<< targetId << ") reviveId(" << reviveId << ") need revive coin num(" << reviveCoinNum << ")." << LogStream::eos;

	auto target = FindDungeonPlayer(targetId);
	if (!target)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to revive player("
			<< targetId << ") reviveId(" << reviveId << ") target no in race." << LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_PLAYER_NOT_EXIST;
	}

	// 说明已经复活过了
	if (m_reviveTasks.find(reviveId) != m_reviveTasks.end())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to revive player("
			<< targetId << ") reviveId(" << reviveId << ") repeated." << LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_REPEAT;
	}

	if (m_dungeonData && m_dungeonData->maxReviveTimes >= 0 && m_dungeonData->maxReviveTimes <= target->GetReviveCount())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to revive player("
			<< targetId << ") no remain revive times cur(" << target->GetReviveCount() << ") max(" << m_dungeonData->maxReviveTimes << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_NO_REMAIN_TIMES;
	}

	DungeonRaceCheckTransaction* transaction = new DungeonRaceCheckTransaction(player, GetID(), DUNGEON_RACE_REVIVE);
	transaction->SetReviveCoin(1);
	transaction->SetParam(targetId);
	transaction->SetParam2(reviveId);
	player->BeginItemTransaction(transaction);

	return ErrorCode::SUCCESS;
}

UInt32 DungeonRace::OnPlayerReportFrame(WSPlayer* player, const std::vector<CLProtocol::Frame>& frames, const std::vector<CLProtocol::FrameChecksum>& checksums)
{
	if (!player)
	{
		return m_lastFrame;
	}

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer || dungeonPlayer->HasRaceEnd() || frames.empty())
	{
		return m_lastFrame;
	}

#ifdef _VERIFY_LOG_
	{
		UInt32 begin = frames[0].sequence;
		UInt32 end = frames[frames.size() - 1].sequence;
		DebugLogStream << "dungeon(" << GetID() << ") player(" << player->GetID() << ") recv frame from(" << begin << ") to(" << end << ")." << LogStream::eos;
	}
#endif

	CLProtocol::VerifyReportFrame reportFrames;
	reportFrames.raceId = GetID();

	CLProtocol::VerifyReportChecksum reportChecksums;
	reportChecksums.raceId = GetID();

	for (UInt32 i = 1; i < frames.size(); i++) {
		auto curFrame = frames[i].sequence;
		auto prevFrame = frames[i - 1].sequence;
		if (curFrame <= prevFrame)
		{
			ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") recv invalid frame(" << curFrame << ") prevFrame(" << prevFrame << ")." << LogStream::eos;
			/*_OnPlayerCheat(player);
			return m_lastFrame;*/
		}
	}

	UInt32 lastFrame = m_lastFrame;
	for (auto& frame : frames)
	{
		if (frame.sequence <= lastFrame)
		{
			continue;
		}

		lastFrame = frame.sequence;

		if (_IsVerifying())
		{
			reportFrames.frames.push_back(frame);
		}

		m_frames.push_back(frame);
	}
	m_lastFrame = lastFrame;

	lastFrame = m_checksums.empty() ? 0 : m_checksums[m_checksums.size() - 1].framesequence;
	for (auto& checksum : checksums)
	{
		if (checksum.framesequence <= lastFrame)
		{
			continue;
		}

		if (checksum.framesequence != lastFrame + CHECKSUM_INTERVAL)
		{
			ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") recv invalid checksum frame(" 
							<< checksum.framesequence << ") lastFrame(" << lastFrame << ")." << LogStream::eos;
			_OnPlayerCheat(player, PLAYER_CHEAT_REASON_REPORT_FRAME_ERR);
			return m_lastFrame;
		}

		lastFrame = checksum.framesequence;
		m_checksums.push_back(checksum);

		if (_IsVerifying())
		{
			PlayerChecksumInfo info;
			info.roleId = player->GetID();
			ChecksumInfo checkInfo;
			checkInfo.frame = checksum.framesequence;
			checkInfo.checksum = checksum.checksum;
			info.checksums.push_back(checkInfo);
			reportChecksums.playerChecksums.push_back(info);
		}
	}

	// 最终的随机数个数
	int needChecksumNum = m_lastFrame / CHECKSUM_INTERVAL;
	if (m_checksums.size() != (size_t)needChecksumNum)
	{
		ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") recv invalid checksum num("
			<< (UInt32)m_checksums.size() << ") lastFrame(" << m_lastFrame << ")." << LogStream::eos;
		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_REPORT_FRAME_ERR);
		return m_lastFrame;
	}

	// 根据最后一帧计算关卡时间，判断有没有加速
	UInt32 needTime = m_lastFrame * FRAME_INTERVAL;
	UInt32 raceTime = (UInt32)((CURRENT_TIME.Sec() - m_startTime) * 1000);
	UInt32 RACE_TIME_DEVIATION = 10 * Avalon::Time::MSECS_OF_SEC;
	if (m_dungeonData && m_dungeonData->subType != DST_COMBO && raceTime + RACE_TIME_DEVIATION < needTime)
	{
		ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") lastFrame("
			<< m_lastFrame << ") needTime(" << needTime << ") racetime(" << raceTime << ")." << LogStream::eos;
		_OnPlayerSpeedUpCheat(player);
		return m_lastFrame;
	}

	// 上报到验证服务器
	if (_IsVerifying())
	{
		if (!reportFrames.frames.empty())
		{
#ifdef _VERIFY_LOG_
			UInt32 startFrame = reportFrames.frames[0].sequence;
			auto lastFrame =  reportFrames.frames[reportFrames.frames.size() - 1].sequence;
			DebugLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") " 
							<< startFrame << " => " << lastFrame << ") size(" << (UInt32)reportFrames.frames.size() << ")." << LogStream::eos;
#endif
			_SendToVerifyServer(reportFrames);
		}

		if (!reportChecksums.playerChecksums.empty())
		{
			_SendToVerifyServer(reportChecksums);
		}
	}

	return m_lastFrame;
}

bool DungeonRace::OnCheckOpenPayChestRet(WSPlayer* player, UInt8 pos)
{
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") check race end drop success, pos(" << pos << ")." << LogStream::eos;

	if ((m_openedChestMask & (1 << pos)))
	{
		// 这个格子已经被别的玩家打开了，这里需要通知客户端
		return false;
	}

	DungeonChestType type = _GetChestTypeByPos(pos);

	DungeonChestTask* task = _FindDungeonChestTask(player->GetID());
	if (!task)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") can't find chest task." << LogStream::eos;
		return false;
	}

	if (type != DCT_PAY)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") check pay chest returned, chest("
						<< pos << ") is not a pay chest." << LogStream::eos;
		return false;
	}

	if (!_OpenChest(player, task, DCT_PAY, pos))
	{
		return false;
	}

	m_openedChestMask |= 1 << pos;
	return true;
}

bool DungeonRace::OnCheckMultiRaceEndDropRet(WSPlayer* player, UInt8 multi)
{
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") check race end drop success, multi(" << multi << ")." << LogStream::eos;

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << player->GetID() << ")." << LogStream::eos;
		return false;
	}

	if (dungeonPlayer->HasGotRaceEndDrop())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") repeated to get race end drop." << LogStream::eos;
		return false;
	}

	auto result = DropItemMgr::Instance()->GenRaceEndDrop(player->GetID(), m_dungeonId, dungeonPlayer->GetLevel(), dungeonPlayer->GetDungeonAddition(), m_OpenedActivityList, dungeonPlayer->GetExtra());
	if (result.dropItems.empty())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") dungeon("
			<< m_dungeonId << ") level(" << player->GetLevel() << ") race end drop item is empty." << LogStream::eos;
		return false;
	}

	UInt8 finalMulti = _GetRaceEndDropBaseMulti(dungeonPlayer) * multi;
	if (finalMulti == 0)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") multi(" << multi << ") raceEndDrop failed." << LogStream::eos;
		return false;
	}

	CLProtocol::SceneDungeonEndDropRes res;
	res.multi = finalMulti;

	ItemRewardVec rewards;
	
	//计算保底掉落
	UInt32 dungeonID = GetDungeonID();
	std::vector<UInt32> tmpDropSetIds;
	auto guaranteeDropDatas = DungeonGuaranteeDropDataEntryMgr::Instance()->GetDataEntryByDungeonID(dungeonID);
	for (auto guaranteeDropData : guaranteeDropDatas)
	{
		if (guaranteeDropData)
		{
			//重置类型判断
			SysBehavior behavior = SYS_BEHAVIOR_NULL;
			switch (guaranteeDropData->resetType)
			{
			case 0:
				behavior = SYS_BEHAVIOR_TOTAL_DUNGEON_GUARANTEE_DROP;
				break;
			case 1:
				behavior = SYS_BEHAVIOR_DAILY_DUNGEON_GUARANTEE_DROP;
				break;
			case 2:
				behavior = SYS_BEHAVIOR_WEEKLY_DUNGEON_GUARANTEE_DROP;
				break;
			case 3:
				behavior = SYS_BEHAVIOR_MONTHLY_DUNGEON_GUARANTEE_DROP;
				break;
			default:
				continue;
			}

			//VIP等级限制判断
			if (guaranteeDropData->vipLevelLimitMin > 0)
			{
				auto tmpPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(dungeonPlayer->GetID());
				if (!tmpPlayer || (tmpPlayer->GetVipLvl() < guaranteeDropData->vipLevelLimitMin || tmpPlayer->GetVipLvl() > guaranteeDropData->vipLevelLimitMax))
				{
					continue;
				}
			}

			UInt32 dropNeedTimes = SysRecordMgr::Instance()->GetRecordValue(behavior, SYS_BEHAVIOR_OWNER_ROLE, dungeonPlayer->GetID(), guaranteeDropData->id);
			if (dropNeedTimes == 0)
			{
				// 说明之前没计算过，重新计算一个值
				dropNeedTimes = Avalon::Random::RandBetween(guaranteeDropData->minTimes, guaranteeDropData->maxTimes);
			}

			if (dropNeedTimes > 0)
			{
				dropNeedTimes--;
				if (dropNeedTimes == 0)
				{
					// 添加保底掉落
					tmpDropSetIds.push_back(guaranteeDropData->dropSetId);

					dropNeedTimes = Avalon::Random::RandBetween(guaranteeDropData->minTimes, guaranteeDropData->maxTimes);

					DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") add hell guarantee drop set(" << guaranteeDropData->dropSetId << ")." << LogStream::eos;
				}

				DebugLogStream << "player(" << dungeonPlayer->GetID() << ") id(" << guaranteeDropData->id << ") dungeon(" << dungeonID << ") guarantee drop need times(" << dropNeedTimes << ")." << LogStream::eos;
				SysRecordMgr::Instance()->SetRecord(behavior, SYS_BEHAVIOR_OWNER_ROLE, dungeonPlayer->GetID(), guaranteeDropData->id, dropNeedTimes);
			}
		}
	}

	if (tmpDropSetIds.size() > 0)
	{
		auto dropItems = _GenDropItemsByDropSet(dungeonPlayer, tmpDropSetIds, 0, 0);
		for (auto& item : dropItems)
		{
			_PushItemToReward(rewards, item.typeId, item.num, item.strenth, item.equipType);

			SceneDungeonDropItem dropItem;
			dropItem.id = 0;
			dropItem.typeId = item.typeId;
			dropItem.num = item.num;
			dropItem.strenth = item.strenth;
			res.items.push_back(dropItem);
		}
	}

	for (auto item : result.dropItems)
	{
		ItemReward reward;
		reward.id = item.typeId;
		reward.num = item.num * finalMulti;
		reward.strenth = item.strenth;
		reward.equipType = item.equipType;

		_PushItemToReward(rewards, reward.id, reward.num, reward.strenth, reward.equipType);
		//rewards.push_back(reward);

		SceneDungeonDropItem dropItem;
		dropItem.id = 0;
		dropItem.typeId = item.typeId;
		dropItem.num = item.num;
		dropItem.strenth = item.strenth;
		dropItem.isDouble = item.CheckMark(DIMT_ACTIVE_EXTRA_DROP);

		bool pile = false;

		do {
			if (!m_dungeonData)
			{
				break;
			}

			// 如果是周常深渊，不需要合并道具
			if (m_dungeonData->subType == DST_WEEK_HELL || m_dungeonData->subType == DST_WEEK_HELL_PER || m_dungeonData->subType == DST_TREASUREMAP)
			{
				break;
			}

			for (auto& tmpItem : res.items)
			{
				if (tmpItem.typeId == item.typeId)
				{
					tmpItem.num += item.num;
					pile = true;
					break;
				}
			}
		} while (0);
		
		if (!pile)
		{
			res.items.push_back(dropItem);
		}

		if (item.isRareControl)
		{
			// 记录稀有控制
			SysRecordMgr::Instance()->RecordGotRareItem(player->GetID(), item.typeId, item.num);
		}

		// 记录方案的稀有控制
		SysRecordMgr::Instance()->RecordGotRareDropSolution(player->GetID(), item.dropSolutionId, 1);
	}

	player->SendProtocol(res);

	// 处理公会副本的buff道具
	ItemRewardVec::iterator iter = rewards.begin();
	while (iter != rewards.end())
	{
		ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (NULL == itemEntry)
		{
			ErrorLogStream << "OnCheckMultiRaceEndDropRet dungeon race(" << GetID()<< ") player(" << player->GetID() <<
				" not find item entry("<< iter->id<<")"<< LogStream::eos;
			++iter;
			continue;
		}

		if (itemEntry->type == ITEM_GUILD_BUFF)
		{
			iter = rewards.erase(iter);
		}
		else
			++iter;
	}

	if (!rewards.empty())
	{
		CLProtocol::WorldDungeonNotifyGotItems notify;
		notify.roleId = player->GetID();
		notify.dungeonId = GetDungeonID();
		notify.items = rewards;
		player->SendToScene(notify);
	}

	dungeonPlayer->SetGotRaceEndDrop();
	return true;
}

UInt32 DungeonRace::OnCheckReviveRet(WSPlayer* player, UInt32 reviveId, ObjID_t targetId)
{
	CLProtocol::SceneDungeonReviveRes res;

	// 说明已经复活过了
	if (m_reviveTasks.find(reviveId) != m_reviveTasks.end())
	{
		res.result = ErrorCode::DUNGEON_REVIVE_REPEAT;
		player->SendProtocol(res);

		DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") check revive success, reviveID(" << reviveId
			<< ") target(" << targetId << "), target is already revived." << LogStream::eos;

		return res.result;
	}

	auto target = FindDungeonPlayer(targetId);
	if (target == NULL)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to revive player("
			<< targetId << ") reviveId(" << reviveId << ") target no in race." << LogStream::eos;

		res.result = ErrorCode::DUNGEON_REVIVE_PLAYER_NOT_EXIST;
		player->SendProtocol(res);
		return res.result;
	}

	if (m_dungeonData && m_dungeonData->maxReviveTimes >= 0 && m_dungeonData->maxReviveTimes <= target->GetReviveCount())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to revive player("
			<< targetId << ") no remain revive times cur(" << target->GetReviveCount() << ") max(" << m_dungeonData->maxReviveTimes << ")." << LogStream::eos;
		res.result = ErrorCode::DUNGEON_REVIVE_NO_REMAIN_TIMES;
		player->SendProtocol(res);
		return res.result;
	}

	target->IncReviveCount();

	m_reviveTasks.insert(reviveId);
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());

	player->SyncEventToScene(SET_REVIVE, targetId);

	player->SendReviveUdpLog(GetDungeonID(), targetId, dungeonPlayer->GetCurAreaID());

	if (GetRelayServerID() > 0)
	{
		_NotifyRelayPlayerEvent(player->GetID(), SYNC_PLAYER_EVENT_REVIVE, targetId, 0);
	}

	DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") check revive success, reviveID(" << reviveId
		<< ") target(" << targetId << ")." << LogStream::eos;

	return ErrorCode::SUCCESS;
}

bool DungeonRace::_GenAreas()
{
	// 计算所有开放的活动
	{
		m_OpenedActivityList.clear();
		auto& openActivitys = WSActivityMgr::Instance()->GetOpenActivitys();
		for (auto& activity : openActivitys)
		{
			if (activity.state == AS_IN)
			{
				m_OpenedActivityList.insert(activity.id);
			}
		}
	}

	DungeonMonsterGenerateContext context;
	context.dungeonData = m_dungeonData;
	context.playerNum = (UInt32)m_players.size();
	context.dungeonLevel = m_dungeonData->level;
	context.hardType = m_dungeonData->hardType;
	for (int i = DAPT_ACTIVITY_MONSTER; i <= DAPT_ACTIVITY_MONSTER_BIG_BOSS; i++)
	{
		auto activityMonsterInfo = WSActivityMonsterMgr::Instance()->GetMonsterGroupID(_GetNormalHardDungeonID(m_dungeonId), (DungeonAreaPointType)i);
		

		// 加成信息
		UInt32 additionPlayerNum = 0;
		ActivitySuitAdditionDataEntry* addition = NULL;
		for (auto itr : m_players)
		{
			auto dungeonPlayer = itr.second;
			if (!dungeonPlayer)
			{
				continue;
			}

			// 先找时装加成，再找月卡加成，这两个只会有一种加成
			auto tmpAddition = WSActivityMonsterMgr::Instance()->GetActivitySuitAddition(activityMonsterInfo.activityId, dungeonPlayer);
			if (!tmpAddition)
			{
				tmpAddition = WSActivityMonsterMgr::Instance()->GetActivityMonthCardAddition(activityMonsterInfo.activityId, dungeonPlayer);
				if (!tmpAddition)
				{
					continue;
				}
				DebugLogStream << "race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") got month card addition(" << tmpAddition->id << ")." << LogStream::eos;
			}

			additionPlayerNum++;
			dungeonPlayer->AddActivityDoubleDropInfo(activityMonsterInfo.activityId, tmpAddition->doubleDropItems, tmpAddition->doubleDropRate);
			if (addition == NULL)
			{
				addition = tmpAddition;
			}
		}

		if (addition && additionPlayerNum > 0 && additionPlayerNum <= (sizeof(addition->monsterRateAddition) / sizeof(addition->monsterRateAddition[0])))
		{
			activityMonsterInfo.prob += addition->monsterRateAddition[additionPlayerNum - 1];
		}

		context.activityMonsterGroups[i] = activityMonsterInfo;
	}

	// 春节随机副本不需要生成房间
	if (m_dungeonData && m_dungeonData->subType == DST_TREASUREMAP)
	{
		m_startAreaId = 0;
		return true;
	}

	std::vector<UInt32> areaIds;
	for (auto itr = m_dungeonData->areaList.begin() + m_passedAreaIndex; itr != m_dungeonData->areaList.end(); ++itr)
	{
		auto areaGroup = *itr;

		auto areaId = areaGroup.randArea(areaIds);
		if (areaId == 0)
		{
			ErrorLogStream << "dungeon(" << GetID() << ", " << GetDungeonID() << ") gen areaId failed." << LogStream::eos;
			return false;
		}

		DungeonArea* area = new DungeonArea();
		area->SetID(areaId);
		_GenMonsters(context, *area);

		m_areas.push_back(area);
		areaIds.push_back(areaId);
	}

	if (!_GenStartArea())
	{
		return false;
	}

	// 先生成词缀，深渊怪是不生成词缀的
	_GenMonsterPrefix();

	// 计算深渊怪
	_GenHellMonsters(context);

	// 计算是否触发隐藏房间
	_GenTriggerHiddenRoom();

	return true;
}

bool DungeonRace::_GenStartArea()
{
	auto itr = m_dungeonData->areaList.begin();
	if (m_passedAreaIndex > 0)
	{
		if (m_passedAreaIndex >= m_dungeonData->areaList.size())
		{
			ErrorLogStream << "race(" << GetID() << ") Dungeon(" << GetDungeonID() << ") can't find passed area by index(" << m_passedAreaIndex << ")." << LogStream::eos;
			return false;
		}

		itr += m_passedAreaIndex;
	}

	if (itr == m_dungeonData->areaList.end())
	{
		return false;
	}

	if (m_passedAreaIndex > 0)
	{
		// 死亡之塔没有随机房间
		std::vector<UInt32> ids;
		m_startAreaId = itr->randArea(ids);
	}
	else
	{
		// 武道会关卡是有随机的，所以其实房间由真正的房间列表决定
		if (m_dungeonData->subType == DST_WUDAOHUI)
		{
			if (m_areas.empty() || !m_areas[0])
			{
				ErrorLogStream << "race(" << GetID() << ") Dungeon(" << GetDungeonID() << ") area list is empty." << LogStream::eos;
				return false;
			}

			m_startAreaId = m_areas[0]->GetID();
		}
		else
		{
			m_startAreaId = m_dungeonData->startAreaId;
		}
		
	}

	// 如果是死亡之塔，需要判断每层的开放等级
	if (m_dungeonData->type == LT_TOWER)
	{
		auto floor = _GetTowerFloorByAreaID(m_dungeonId, m_startAreaId);
		auto deathFloorData = DeathTowerAwardDataEntryMgr::Instance()->FindEntry(floor);

		auto player = m_players.empty() ? NULL : m_players.begin()->second;
		if (!player)
		{
			ErrorLogStream << "race(" << GetID() << ") player list is null." << LogStream::eos;
			return false;
		}

		if (!deathFloorData)
		{
			ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") invalid floor(" << floor << ") can't find data." << LogStream::eos;
			return false;
		}

		if (deathFloorData->levelLimit > player->GetLevel())
		{
			// 等级限制
			ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") invalid floor(" << floor << ") need level(" << deathFloorData->levelLimit << ")." << LogStream::eos;
			return false;
		}
	}

	return true;
}

// 刷怪
void DungeonRace::_GenMonsters(DungeonMonsterGenerateContext& context, DungeonArea& area)
{
	// 生成怪物
	VDungeonMonster monsters;
	DungeonMonsterGenerator::Instance()->GenMonstersByArea(context, area.GetID(), _GetAdapterLevel(), monsters);

	for (auto& monster : monsters)
	{
		monster.areaId = area.GetID();
		_GenDropItems(monster);

		if ((monster.type == MT_BOX) && monster.dropItems.empty() && !monster.willSummonMonster)
		{
			// 没有掉落并且不会召唤出怪物的可破坏物就不用生成了
			continue;
		}

		// 只有结算收益的图，所有怪物都没有经验
		if (m_dungeonData && m_dungeonData->onlyRaceEndProfit)
		{
			monster.exp = 0;
		}

		area.AddMonster(monster);
	}
}

bool DungeonRace::_GenHellMonsters(DungeonMonsterGenerateContext& context)
{
	if (!m_isHellMode)
	{
		return true;
	}

	if (!m_dungeonData || !_GenHellAreaID())
	{
		return false;
	}

	auto hellArea = _FindArea(m_hellAreaId);
	if (!hellArea)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find hell area(" << m_hellAreaId << ")." << LogStream::eos;
		return false;
	}

	DebugLogStream << "dungeon race(" << GetID() << ") gen hell monsters in area(" << m_hellAreaId << ")." << LogStream::eos;

	VDungeonMonster monsters;
	context.hellMonsterLevel = m_dungeonData->level;
	context.hardType = m_dungeonData->hardType;
	if (!DungeonMonsterGenerator::Instance()->GenHellMonsters(context, monsters))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") dungeon(" << GetDungeonID() << ") generate hell monster failed." << LogStream::eos;
		return false;
	}

	m_hellHardMode = context.hellMode;

	for (UInt32 i = 0; i < monsters.size(); i++)
	{
		auto& monster = monsters[i];
		monster.typeId = monster.typeId / 10 * 10 + m_dungeonData->hardType + 1;
		if (i == monsters.size() - 1)
		{
			// 深渊模式只为最后一只怪生成掉落
			_GenDropItems(monster);
		}
		hellArea->AddMonster(monster);
	}

	return true;
}

bool DungeonRace::_GenBossDropItems()
{
	if (!m_dungeonData || m_dungeonData->onlyRaceEndProfit)
	{
		return false;
	}

	// 掉落方案
	std::vector<UInt32> dropSetIds;

	// 关卡掉落
	UInt32 dungeonDropSetID = m_dungeonData->dungeonDropItem;
	if (dungeonDropSetID)
	{
		dropSetIds.push_back(dungeonDropSetID);
	}

	// 活动掉落
	for (auto dropSetId : m_dungeonData->activityDropItems)
	{
		if (dropSetId)
		{
			dropSetIds.push_back(dropSetId);
		}
	}

	// 为每个玩家计算掉落
	for (auto itr : m_players)
	{
		auto dungeonPlayer = itr.second;
		if (!dungeonPlayer || dungeonPlayer->IsAssist())
		{
			continue;
		}

		auto tmpDropSetIds = dropSetIds;
		if (m_dungeonData->dungeonFirstDropItem > 0 && dungeonPlayer->GetTotalCount() == 0)
		{
			tmpDropSetIds.push_back(m_dungeonData->dungeonFirstDropItem);
		}

		auto addition = dungeonPlayer->GetDungeonAddition();

		auto dropItems = _GenDropItemsByDropSet(dungeonPlayer, tmpDropSetIds, 0, 0);

		// 每日必掉
		auto normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);
		auto dailyDropData = DungeonDailyDropDataEntryMgr::Instance()->FindEntry(normalHardDungeonId);
		if (dailyDropData && SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, dungeonPlayer->GetID(), normalHardDungeonId) < dailyDropData->dailyNum)
		{
			for (auto& dailyDropItem : dailyDropData->items)
			{
				UInt32 itemId = dailyDropItem.id;
				UInt32 itemNum = DropItemMgr::Instance()->RandFromRange(dailyDropItem.minNum, dailyDropItem.maxNum);

				bool hasDroped = false;
				for (auto& item : dropItems)
				{
					if (item.typeId == itemId)
					{
						hasDroped = true;
						item.isDailyDrop = true;
						break;
					}
				}

				if (!hasDroped)
				{
					PlayerDropItem dropItem;
					dropItem.roleId = dungeonPlayer->GetID();
					dropItem.id = _GenDropItemID();
					dropItem.typeId = itemId;
					dropItem.num = itemNum;
					dropItem.isRareControl = false;
					dropItem.taskId = 0;
					dropItem.isDailyDrop = true;
					if (dropItem.equipType == EQUIP_NORMAL)
					{
						dropItem.strenth = DropStrengthenControlDataEntryMgr::Instance()->RandomItemStrenth(dropItem.typeId);
					}
					else
					{
						dropItem.strenth = 0;
					}
					
					dropItems.push_back(dropItem);

					UInt32 multiplyRate = addition.dropMultiplyRateAddition[dailyDropItem.itemType];

					for (UInt32 idx = 0; idx < multiplyRate; ++idx)
					{
						PlayerDropItem dropItem;
						dropItem.roleId = dungeonPlayer->GetID();
						dropItem.id = _GenDropItemID();
						dropItem.typeId = itemId;
						dropItem.num = itemNum;
						dropItem.isRareControl = false;
						dropItem.taskId = 0;
						if (dropItem.equipType == EQUIP_NORMAL)
						{
							dropItem.strenth = DropStrengthenControlDataEntryMgr::Instance()->RandomItemStrenth(dropItem.typeId);
						}
						else
						{
							dropItem.strenth = 0;
						}

						dropItems.push_back(dropItem);

						//DebugLogStream << "Player(" << context.roleId << ") multiply num, dropset(" << dropSet->id << ") gen drop item(" << item.id << ", " << num << "), drop type=" << item.itemType << LogStream::eos;
					}
				}
			}
			
		}


		dungeonPlayer->SetBossDropItems(dropItems);
	}

	return true;
}

void DungeonRace::_GenMonsterPrefix()
{
	// 深渊、远古、死亡之塔、异界不生成词缀怪
	if (m_dungeonData->subType == DST_YUANGU || m_dungeonData->IsHellDungeon() || m_dungeonData->subType == DST_SIWANGZHITA || m_dungeonData->subType == DST_DEVILDDOM)
	{
		return;
	}

	UInt32 adapterLevel = _GetAdapterLevel();
	UInt32 dungeonLevel = adapterLevel ? adapterLevel : m_dungeonData->level;
	auto data = _GetPrefixMonsterLimit(m_dungeonData->hardType, dungeonLevel);
	if (!data)
	{
		return;
	}

	UInt32 prefixMonsterNum = data->prefixMonsterTotalNum;
	UInt32 maxPerAreaPrefixMonsterNum = _GetPerAreaPrefixMonsterMaxNum(prefixMonsterNum);

	std::map<UInt32, std::vector<UInt32>> monsters;

	UInt32 totalMonsterNum = 0;
	for (auto area : m_areas)
	{
		for (auto& monster : area->GetAllMonster())
		{
			if (monster.type == MT_MONSTER)
			{
				monsters[area->GetID()].push_back(monster.id);
				totalMonsterNum++;
			}
		}

	}

	std::map<UInt32, UInt32> areaPrefixMonsterNum;
	for (UInt32 i = 0; i < prefixMonsterNum && totalMonsterNum > 0; i++)
	{
		UInt32 monsterIndex = Avalon::Random::RandBetween(0, totalMonsterNum);
		for (auto itr : monsters)
		{
			if (itr.second.size() <= monsterIndex)
			{
				monsterIndex -= itr.second.size();
				continue;
			}

			totalMonsterNum--;
			UInt32 areaId = itr.first;
			UInt32 monsterId = itr.second[monsterIndex];
			itr.second.erase(itr.second.begin() + monsterIndex);

			if (++areaPrefixMonsterNum[areaId] >= maxPerAreaPrefixMonsterNum || itr.second.empty())
			{
				monsters.erase(areaId);
			}

			// 计算词缀
			DungeonMonster* monster = _FindMonster(monsterId);
			if (!monster)
			{
				ErrorLogStream << "dungeon race(" << GetID() << ") can't find monster(" << monsterId << ")." << LogStream::eos;
				break;
			}

			std::vector<UInt8> prefixes = DungeonMonsterGenerator::Instance()->GenMonsterPrefix(data->lowPrefixNum, data->seniorPrefixNum);
			monster->prefixes = prefixes;

			//DebugLogStream << "dungeon race(" << GetID() << ") monster(" << monsterId << ") gen prefix." << LogStream::eos;

			break;
		}
	}
}

// 计算掉落物
void DungeonRace::_GenDropItems(DungeonMonster& monster)
{
	// 只有结算收益的图，所有怪物都没有掉落
	if (m_dungeonData && m_dungeonData->onlyRaceEndProfit)
	{
		return;
	}

	// 掉落方案
	std::vector<UInt32> dropSetIds;

	if (monster.isHell)
	{
		// 深渊模式，直接用地下城表中配的深渊掉落
		auto hellDropIDs = _GetHellDropID(m_hellHardMode);
		for (auto hellDropID : hellDropIDs)
		{
			dropSetIds.push_back(hellDropID);
		}
	}
	else
	{
		// 怪物的特定掉落
		dropSetIds = monster.dropSet;

		// 等级通用掉落
		if (monster.attrDataEntry)
		{
			for (auto dropSetId : monster.attrDataEntry->dropSetIds)
			{
				dropSetIds.push_back(dropSetId);
			}
		}

		// 任务掉落
		for (auto taskDrop : m_dungeonData->taskDropItems)
		{
			dropSetIds.push_back(taskDrop);
		}

		// 如果是活动怪，只掉落对应活动的掉落
		if (!monster.activityMonsterId)
		{
			// 关卡获取通用掉落
			UInt32 commonDropSetID = _GetCommonDropSetID(monster.type);
			if (commonDropSetID)
			{
				dropSetIds.push_back(commonDropSetID);
			}
		}

	}

	// 为每个玩家计算掉落
	for (auto itr : m_players)
	{
		auto dungeonPlayer = itr.second;
		if (!dungeonPlayer || dungeonPlayer->IsAssist())
		{
			continue;
		}

		if (monster.dailyMaxDropNum)
		{
			auto index = _GetIndexByAreaID(monster.areaId);
			if (IsSetDungeonAreaIndex(dungeonPlayer->GetID(), m_dungeonId, index))
			{
				continue;
			}
		}

		auto tmpDropSetIds = dropSetIds;

		// 深渊保底掉落
		if (monster.isHell && !dungeonPlayer->IsGenGuaranteeDrop())
		{
			dungeonPlayer->SetGenGuaranteeDrop();

			auto guaranteeDropDatas = DungeonGuaranteeDropDataEntryMgr::Instance()->GetDataEntryByDungeonID(m_hellDungeonEntryId);

			for (auto guaranteeDropData : guaranteeDropDatas)
			{
				if (guaranteeDropData)
				{
					//重置类型判断
					SysBehavior behavior = SYS_BEHAVIOR_NULL;
					switch (guaranteeDropData->resetType)
					{
					case 0:
						behavior = SYS_BEHAVIOR_TOTAL_DUNGEON_GUARANTEE_DROP;
						break;
					case 1:
						behavior = SYS_BEHAVIOR_DAILY_DUNGEON_GUARANTEE_DROP;
						break;
					case 2:
						behavior = SYS_BEHAVIOR_WEEKLY_DUNGEON_GUARANTEE_DROP;
						break;
					case 3:
						behavior = SYS_BEHAVIOR_MONTHLY_DUNGEON_GUARANTEE_DROP;
						break;
					default:
						continue;
					}

					//VIP等级限制判断
					if (guaranteeDropData->vipLevelLimitMin > 0)
					{
						auto tmpPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(dungeonPlayer->GetID());
						if (!tmpPlayer || (tmpPlayer->GetVipLvl() < guaranteeDropData->vipLevelLimitMin || tmpPlayer->GetVipLvl() > guaranteeDropData->vipLevelLimitMax))
						{
							continue;
						}
					}

					UInt32 dropNeedTimes = SysRecordMgr::Instance()->GetRecordValue(behavior, SYS_BEHAVIOR_OWNER_ROLE, dungeonPlayer->GetID(), guaranteeDropData->id);
					if (dropNeedTimes == 0)
					{
						// 说明之前没计算过，重新计算一个值
						dropNeedTimes = Avalon::Random::RandBetween(guaranteeDropData->minTimes, guaranteeDropData->maxTimes);
					}

					if (dropNeedTimes > 0)
					{
						dropNeedTimes--;
						if (dropNeedTimes == 0)
						{
							// 添加保底掉落
							tmpDropSetIds.push_back(guaranteeDropData->dropSetId);
							dropNeedTimes = Avalon::Random::RandBetween(guaranteeDropData->minTimes, guaranteeDropData->maxTimes);

							DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") add hell guarantee dropset(" << guaranteeDropData->dropSetId << ")." << LogStream::eos;
						}

						DebugLogStream << "player(" << dungeonPlayer->GetID() << ") id(" << guaranteeDropData->id << ") hell(" << m_hellDungeonEntryId << ") guarantee drop need times(" << dropNeedTimes << ")." << LogStream::eos;
						SysRecordMgr::Instance()->SetRecord(behavior, SYS_BEHAVIOR_OWNER_ROLE, dungeonPlayer->GetID(), guaranteeDropData->id, dropNeedTimes);
					}
				}
			}
		}
		
		// 如果是活动怪，只掉落对应活动的掉落
		if (monster.activityMonsterId)
		{
			auto activityMonster = WSActivityMonsterMgr::Instance()->FindMonster(monster.activityMonsterId);
			if (activityMonster)
			{
				auto dorpSetId = activityMonster->GetDropSetByLevel(dungeonPlayer->GetLevel());
				if (dorpSetId != 0)
				{
					tmpDropSetIds.push_back(dorpSetId);
				}

				InfoLogStream << "race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") add activity monster(" << monster.activityMonsterId << ", " << monster.id << ", " << monster.typeId << ")." << LogStream::eos;
			}
		}


		auto dropItems = _GenDropItemsByDropSet(dungeonPlayer, tmpDropSetIds, monster.id, monster.typeId);
		for (auto& item : dropItems)
		{
			item.monsterNormalHardId = monster.GetMonsterNormalHardTypeID();
			item.monsterDailyMaxDropNum = monster.dailyMaxDropNum;
			if (item.monsterDailyMaxDropNum > 0)
			{
				item.monsterDungeonAreaIndex = _GetIndexByAreaID(monster.areaId);
			}
			monster.dropItems.push_back(item);

			// 如果是活动怪，有可能掉落翻倍
			if (monster.activityMonsterId)
			{
				UInt32 doubleRate = dungeonPlayer->GetActivityDoubleDropRate(monster.activityMonsterId, item.typeId);
				if (doubleRate > 0 && Avalon::Random::SelectInHundred(doubleRate))
				{
					auto doubleDropItem = item;
					doubleDropItem.id = _GenDropItemID();
					doubleDropItem.isDouble = true;
					InfoLogStream << "race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") activity monster("
						<< monster.activityMonsterId << ", " << monster.id << ") got double drop(" << item.typeId << ", " << item.num << ")." << LogStream::eos;
					monster.dropItems.push_back(doubleDropItem);
				}
			}
		}
	}
}

std::vector<PlayerDropItem> DungeonRace::_GenDropItemsByDropSet(DungeonPlayer* player, std::vector<UInt32> dropSetIds, UInt32 monsterId, UInt32 monsterTypeId)
{
	std::vector<PlayerDropItem> dropItems;

	if (!player || player->IsAssist())
	{
		return dropItems;
	}

	for (auto dropSetID : dropSetIds)
	{
		m_allDropSet.push_back(dropSetID);

		ObjID_t roleId = player->GetID();
		PlayerDropItem dropItem;
		dropItem.roleId = roleId;

		auto addition = player->GetDungeonAddition();
		GenDropItemResult genRet = DropItemMgr::Instance()->GenDropItems(GetDungeonID(), roleId, dropSetID, player->GetVipLvl(), player->GetOccu(), player->GetTaskList(), addition, m_OpenedActivityList, player->GetExtra());
		for (auto& item : genRet.dropItems)
		{
			dropItem.id = _GenDropItemID();
			dropItem = item.GetDropItem();
			dropItem.isRareControl = item.isRareControl;
			dropItem.taskId = item.taskId;
			dropItem.notice = item.notice;
			dropItem.solutionId = item.dropSolutionId;
			dropItem.equipType = item.equipType;
			dropItem.vipDropLimitId = item.vipDropLimitId;
			dropItem.isDouble = item.CheckMark(DIMT_ACTIVE_EXTRA_DROP);

			if (dropItem.equipType == EQUIP_NORMAL)
			{
				dropItem.strenth = DropStrengthenControlDataEntryMgr::Instance()->RandomItemStrenth(dropItem.typeId);
			}
			else
			{
				dropItem.strenth = 0;
			}

			if (dropItem.vipDropLimitId > 0)
			{
				auto remainNum = SysRecordMgr::Instance()->VipLimitItemRemainNum(roleId, dropItem.typeId, dropItem.num, dropItem.vipDropLimitId, player->GetVipLvl());

				if (remainNum == 0)
				{
					continue;
				}

				dropItem.num = remainNum;
			}

			if (dropItem.isRareControl)
			{
				auto remainNum = SysRecordMgr::Instance()->RareItemRemainNum(roleId, dropItem.id, dropItem.num);
				if (remainNum == 0)
				{
					continue;
				}

				dropItem.num = remainNum;

				// 记录稀有控制
				SysRecordMgr::Instance()->RecordGotRareItem(roleId, dropItem.typeId, dropItem.num);
			}



			SysRecordMgr::Instance()->RecordGotRareDropSolution(roleId, item.dropSolutionId, 1);

			auto itemData = ItemDataEntryMgr::Instance()->FindEntry(dropItem.typeId);
			if (itemData && itemData->color >= WSApplication::Instance()->GetNeedRecordDropQuality())
			{
				DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") generated item("
					<< dropItem.id << "," << item.typeId << ") num(" << item.num << ") from drop set("
					<< dropSetID << ") to monster(" << monsterId << "," << monsterTypeId << ")." << LogStream::eos;
			}
			

			dropItems.push_back(dropItem);
		}
	}

	if (m_DropCountState)
	{
		for (auto item : dropItems)
		{
			UInt32 id = item.typeId;
			UInt8 equipType = item.equipType;
			UInt64 num = item.num;
			DropKey key(id, equipType);

			auto iter = m_DropCount.find(key);
			if (iter == m_DropCount.end())
			{
				DropCount count;
				count.num = num;
				count.times = 1;
				m_DropCount[key] = count;
				continue;
			}

			DropCount& count = iter->second;
			count.times++;
			count.num += num;
		}
	}

	return dropItems;
}

UInt32 DungeonRace::_GetCommonDropSetID(MonsterType type)
{
	if (!m_dungeonData)
	{
		return 0;
	}

	UInt32 dropID = 0;
	if (type == MT_MONSTER)
	{
		dropID = m_dungeonData->normalDropItem;
	}
	else if (type == MT_ELITE)
	{
		dropID = m_dungeonData->eliteDropItem;
	}
	else if (type == MT_BOSS)
	{
		dropID = m_dungeonData->bossDropItem;
	}
	else if (type == MT_BOX)
	{
		dropID = m_dungeonData->destructDropItem;
	}
	else if (type == MT_EASTER_EGG)
	{
		dropID = m_dungeonData->easterEggDropItem;
	}

	return dropID;
}

void DungeonRace::_GenChest(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	ObjID_t roleId = player->GetID();

	auto dungeonPlayer = FindDungeonPlayer(roleId);
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << roleId << ")." << LogStream::eos;
		return;
	}

	// 助战模式下没有翻牌
	if (dungeonPlayer->IsAssist())
	{
		DebugLogStream << "dungeon race(" << GetID() << ")  player(" << roleId << ") in assist mode, can't gen chest." << LogStream::eos;
		return;
	}

	if (_FindDungeonChestTask(roleId))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") want to repeat gen chest." << LogStream::eos;
		return;
	}

	auto chestData = DungeonChestDataEntryMgr::Instance()->FindData(GetDungeonID(), dungeonPlayer->GetLevelScore());
	if (!chestData)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId
			<< ") can't find dungeon(" << GetDungeonID() << ")'s chest data." << LogStream::eos;
		return;
	}

	DungeonChestTask task;
	task.SetRoleID(roleId);

	if (dungeonPlayer->IsFreeOpenGoldChest())
	{
		task.SetPayChestCost(0);
		task.SetPayChestCostItemID(chestData->payChestCostItemId);
	}
	else
	{
		task.SetPayChestCostItemID(chestData->payChestCostItemId);
		task.SetPayChestCost(chestData->payChestCost);
	}
	
	// 计算金币奖励
	GenDropItemResult goldRet = DropItemMgr::Instance()->GenRaceEndChest(GetDungeonID(), roleId, chestData->goldDropSetId, dungeonPlayer->GetOccu());
	if (goldRet.dropItems.empty() || goldRet.dropItems.size() == 1)
	{
		UInt32 num = goldRet.dropItems.empty() ? 0 : goldRet.dropItems[0].num;
		task.SetGoldReward(num);
	}
	else
	{
		DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId
			<< ") dungeon(" << GetDungeonID() << ") gen gold chest failed." << LogStream::eos;
	}

	CLProtocol::SceneDungeonChestReq req;

	// 计算不同宝箱
	for (auto i = 0; i < DCT_NUM; i++)
	{
		//if (i == DCT_VIP)
		//{
		//	// 如果是VIP宝箱，要判断这个人是不是月卡用户
		//	if (!player->IsInMonthCard())
		//	{
		//		continue;
		//	}
		//}

		UInt32 dropSetId = chestData->chestDropoSetIds[i];
		GenDropItemResult chestGenRet = DropItemMgr::Instance()->GenRaceEndChest(GetDungeonID(), roleId, dropSetId, player->GetOccu(), dungeonPlayer->GetDungeonAddition());
		if (chestGenRet.dropItems.size() != 1 && chestGenRet.dropItems.size() != 0)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId
				<< ") dungeon(" << GetDungeonID() << ") gen chest(" << dropSetId << ") failed." << LogStream::eos;
			continue;
		}

		for (auto& item : chestGenRet.dropItems)
		{
			DebugLogStream << "dungeon(" << GetDungeonID() << ") race(" << GetID() << ") player(" << player->GetID() << ") chest(" << i << ") drop item(" << item.typeId << ", " << item.num << ")." << LogStream::eos;
		}

		DungeonChest chest;
		chest.type = (DungeonChestType)i;
		if (chestGenRet.dropItems.size() == 1)
		{
			chest = chestGenRet.dropItems[0];
		}

		if (chest.isRareControl)
		{
			// 记录稀有控制
			SysRecordMgr::Instance()->RecordGotRareItem(player->GetID(), chest.typeId, chest.num);
		}

		// 翻牌翻倍
		if (dungeonPlayer->GetDungenAdditionByType(DART_GUILD_TERR_CHEST_DOUBLE_FLAG) & (1 << i))
		{
			chest.num *= 2;
		}

		if (i == DCT_NORMAL)
		{
			chest.goldReward = task.GetGoldReward();
		}

		task.AddChest(chest);
	}

	UInt32 waitTime = m_players.size() == 1 ? 5000 : 10 * 1000;
	task.SetTimeoutTime(Avalon::Time::CurrentTime() + waitTime);

	// 下发给客户端
	CLProtocol::SceneDungeonChestNotify notify;
	notify.payChestCostItemId = task.GetPayChestCostItemID();
	notify.payChestCost = task.GetPayChestCost();
	player->SendProtocol(notify);

	m_chestTasks.push_back(task);
}

UInt32 DungeonRace::_GenHellAreaID()
{
	if (!m_dungeonData || m_dungeonData->hellAreaList.empty())
	{
		m_hellAreaId = 0;
	}
	else
	{
		m_hellAreaId = m_dungeonData->hellAreaList[Avalon::Random::RandBetween(0, m_dungeonData->hellAreaList.size() - 1)];
	}
	
	return m_hellAreaId;
}

ItemReward DungeonRace::_GenTeamReward(WSPlayer* player, LevelScore score)
{
	ItemReward reward;
	if (!player || score <= LS_C)
	{
		return reward;
	}

	DungeonPlayer* dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") gen team reward but player not in race." << LogStream::eos;
		return reward;
	}

	bool hasReward = false;
	// 看看队友有没有是好友或者同工会的
	for (auto itr : m_players)
	{
		auto teammate = itr.second;
		if (!teammate || teammate->GetID() == player->GetID())
		{
			continue;
		}

		// 判断是不是好友
		WSPlayer* teammatePlayer = WSPlayerMgr::Instance()->FindNormalPlayer(teammate->GetID());
		if (WSRelationMgr::IsMutualFriend(player, teammatePlayer)
			|| WSRelationMgr::IsMasterDisciple(player, teammatePlayer))
		{
			hasReward = true;
			break;
		}
		/*if (player->GetRelationMgr().FindRelation(RELATION_FRIEND, teammate->GetID()))
		{
			hasReward = true;
			break;
		}
		*/
		// 判断是不是同工会的
		if (GuildMgr::Instance()->IsInSameGuild(player->GetID(), teammate->GetID()))
		{
			hasReward = true;
			break;
		}
	}

	if (!hasReward)
	{
		return reward;
	}

	class TeamRewardVistor : public Avalon::DataEntryVisitor<TeamRewardDataEntry>
	{
	public:
		TeamRewardVistor(ObjID_t roleId_) : roleId(roleId_), totalProb(0) {}

		bool operator()(TeamRewardDataEntry* dataEntry)
		{
			if (dataEntry->dailyLimit != 0)
			{
				auto num = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_TEAM_REWARD, SYS_BEHAVIOR_OWNER_ROLE, roleId, dataEntry->id);
				if (dataEntry->dailyLimit <= num)
				{
					return true;
				}
			}

			datas.push_back(dataEntry);
			totalProb += dataEntry->prob;
			return true;
		}

		ObjID_t roleId;
		std::vector<TeamRewardDataEntry*> datas;
		UInt32 totalProb;
	};

	TeamRewardVistor vistor(player->GetID());
	TeamRewardDataEntryMgr::Instance()->Visit(vistor);

	if (vistor.totalProb == 0 || vistor.datas.empty())
	{
		return reward;
	}

	UInt32 prob = Avalon::Random::RandBetween(0, 99);
	
	// 计算究竟掉什么
	TeamRewardDataEntry* rewardData = NULL;
	for (auto data : vistor.datas)
	{
		if (prob < data->prob)
		{
			rewardData = data;
			break;
		}
		prob -= data->prob;
	}

	if (rewardData == NULL)
	{
		return reward;
	}

	if (rewardData->type == TEAM_AWARD_RACE_END_EXP)
	{
		UInt32 expNum = _GetDungeonRaceEndExp(GetDungeonID(), dungeonPlayer->GetLevel()) * rewardData->rewardId / 100;
		if (expNum > 0)
		{
			reward.id = ITEM_EXP;
			reward.num = expNum;
		}
	}
	else if (rewardData->type == TEAM_AWARD_ITEM)
	{
		reward.id = rewardData->rewardId;
		reward.num = rewardData->num;
	}

	if (rewardData->dailyLimit > 0)
	{
		SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_TEAM_REWARD, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), rewardData->id, rewardData->num);
	}

	DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") team reward(" << rewardData->type << ", " << rewardData->rewardId << ", " << rewardData->num << ")." << LogStream::eos;

	return reward;
}


ItemReward DungeonRace::_GenVeteranReturnDropReward(WSPlayer* player, LevelScore score)
{
	ItemReward reward;

	// 队伍中一定要超过1个人
	if (m_players.size() <= 1)
	{
		return reward;
	}

	// 一定要获胜
	if (score <= LS_C)
	{
		return reward;
	}

	// 只有深渊关卡有回归奖励
	if (!m_dungeonData || (m_dungeonData->subType != DST_HELL && m_dungeonData->subType != DST_LIMIT_TIME_HELL && m_dungeonData->subType != DST_FREE_LIMIT_TIME_HELL))
	{
		return reward;
	}

	// 检查队伍里有没有回归玩家
	bool hasVeteranReturnPlayer = false;
	for (auto& itr : m_players)
	{
		DungeonPlayer* gamer = itr.second;
		if (gamer->IsVeteranReturn())
		{
			hasVeteranReturnPlayer = true;
		}
	}

	if (!hasVeteranReturnPlayer)
	{
		return reward;
	}

	UInt32 rewardItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RETURN_HELL_DROP_GIFT);
	UInt32 maxTimes = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RETURN_HELL_DROP_DAILY_MAX_NUM);
	UInt32 curTimes = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_VETERAN_RETURN_DROP, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), 0);

	if (curTimes >= maxTimes)
	{
		DebugLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") no remain times, current times(" << curTimes << ")." << LogStream::eos;
		return reward;
	}

	// 更新次数
	SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_VETERAN_RETURN_DROP, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), 0, 1);
	curTimes++;

	InfoLogStream << "player(" << player->GetID() << ") dungeon(" << GetID() << ", " << GetDungeonID() << ") got veteran reward(" 
					<< rewardItemId << ") curTimes(" << curTimes << ") maxTimes(" << maxTimes << ")." << LogStream::eos;
	
	reward.id = rewardItemId;
	reward.num = 1;

	return reward;
}

void DungeonRace::_GenFriendExpRate()
{
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player) continue;
		WSPlayer* selfPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(player->GetID());
		if (!selfPlayer) continue;
		bool haveFriend = false;
		UInt32 maxIntimacy = 0;
		for (auto itrnel : m_players)
		{
			auto other = itrnel.second;
			if (!other) continue;
			if (player->GetID() == other->GetID()) continue;
			WSPlayer* otherPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(other->GetID());
			if (!otherPlayer) continue;
			if (WSRelationMgr::IsMutualFriend(selfPlayer, otherPlayer)
				|| WSRelationMgr::IsMasterDisciple(selfPlayer, otherPlayer)) //互为好友或师徒
			{
				haveFriend = true;
				WSRelation* relation = selfPlayer->GetRelationMgr().GetRelation(otherPlayer->GetID());
				if (relation && relation->GetIntimacy() > maxIntimacy)
				{
					maxIntimacy = relation->GetIntimacy();
				}
			}
		}
		if (haveFriend)
		{
			UInt32 rate = FriendWelfareAddDataEntryMgr::Instance()->GetExpAddProb(maxIntimacy);
			player->SetDungenAdditionByType(DART_FRIEND_EXP_RATE, rate);
		}
	}
}

void DungeonRace::_GenTriggerHiddenRoom()
{
	UInt32 normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);
	if (normalHardDungeonId != 3201000)
	{
		return;
	}

	// 计算隐藏房间概率
	
	// 队伍概率(百分比)
	UInt32 teamProb = 0;
	UInt32 playerNum = 0;
	UInt32 initProb = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TRIGGER_DUNGEON_HIDDEN_ROOM_PROB);
	for (auto itr : m_players)
	{
		playerNum++;
		auto roleid = itr.first;
		auto lastTriggerResult = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_HIDDEN_ROOM_FLAG,
									SYS_BEHAVIOR_OWNER_ROLE, roleid, normalHardDungeonId);
		// 如果上一次已经有结果了，那么概率就是0
		if (lastTriggerResult == 0)
		{
			teamProb += initProb;
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON_HIDDEN_ROOM_FLAG,
				SYS_BEHAVIOR_OWNER_ROLE, roleid, normalHardDungeonId, 1);
		}
	}

	if (playerNum == 0)
	{
		teamProb = 0;
	}
	else
	{
		teamProb /= playerNum;
	}

	m_TriggerHiddenRoom = Avalon::Random::SelectInHundred(teamProb);

	DebugLogStream << "dungeon race(" << GetID() << ") dungeon(" << m_dungeonId << ") hidden room prob(" << teamProb << ") result(" << m_TriggerHiddenRoom << ")." << LogStream::eos;
}

UInt32 DungeonRace::_GenDropItemID()
{
	return ++m_dropItemSeed;
}

void DungeonRace::_GetStartInfo(DungeonPlayer* dungeonPlayer, std::vector<RacePlayerInfo>& racePlayerInfoes, const Avalon::SockAddr& addr, CLProtocol::SceneDungeonStartRes& res)
{
	ObjID_t roleId = dungeonPlayer == NULL ? 0 : dungeonPlayer->GetID();
	res.openAutoBattle = WSServiceSwitchMgr::Instance()->GetAutoBattleMode();
	res.dungeonId = GetDungeonID();
	res.startAreaId = GetStartAreaID();
	res.result = 0;
	GetAreaInfo(roleId, res.areas);
	GetHellInfo(roleId, res.hellInfo);
	res.session = m_sessionId == 0 ? GetID() : m_sessionId;
	res.addr = addr;
	res.players = racePlayerInfoes;
	res.isRecordLog = WSApplication::Instance()->IsRecordDungeonLog() ? 1 : 0;
	res.isRecordReplay = WSApplication::Instance()->IsRecordDungeonReplay() ? 1 : 0;
	res.battleFlag = WSApplication::Instance()->GetBattleFlag();
	// 打上隐藏房间标记
	if (m_TriggerHiddenRoom)
	{
		res.battleFlag |= (UInt64)1 << Eff_Devilddom_hidden_room;
	}

	// 打上是否是单机的标志
	if (m_sessionId == 0)
	{
		res.battleFlag |= (UInt64)1 << Eff_Single_race;
	}
	else
	{
		res.battleFlag &= ~((UInt64)1 << Eff_Single_race);
	}

	// 如果有队伍，判断深渊是否自动关闭
	res.hellAutoClose = 0;
	if (GetTeamID() > 0)
	{
		auto team = WSTeamMgr::Instance()->FindTeam(GetTeamID());
		if (team && team->IsSetOption(TO_HELL_AUTO_CLOSE))
		{
			res.hellAutoClose = 1;
		}
	}

	if (dungeonPlayer)
	{
		res.dropOverMonsters = dungeonPlayer->GetDropOverMonsters();
	}

	if (dungeonPlayer)
	{
		for (const auto& bossDrop : dungeonPlayer->GetBossDropItems())
		{
			res.bossDropItems.push_back(bossDrop.GetSceneDungeonDropItem());
		}
	}
}

bool DungeonRace::_OnPlayerKillMonster(WSPlayer* player, DungeonPlayer* dungeonPlayer, DungeonMonster* monster)
{
	if (!player || !dungeonPlayer || !monster)
	{
		return false;
	}

	if (dungeonPlayer->HasKilledMonster(monster->id))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") repeat to kill monster(" << monster->id << ")." << LogStream::eos;
		return false;
	}

	if (!dungeonPlayer->HasEnteredArea(monster->areaId))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") want to kill monster(" << monster->id << ") but not enter area(" << monster->areaId << ")." << LogStream::eos;
		return false;
	}

	UInt32 exp = monster->GetExp(dungeonPlayer);

	// 疲劳燃烧翻倍
	exp *= (100 + dungeonPlayer->GetDungenAdditionByType(DART_FATIGUE_BURNING_RATE)) / 100;

	CLProtocol::WorldDungeonNotifyKillMonster notify;
	notify.roleId = player->GetID();
	notify.dungeonId = GetDungeonID();
	notify.monsterId = monster->GetMonsterBaseTypeID();
	notify.exp = exp;
	notify.isAssit = dungeonPlayer->IsAssist();
	player->SendToScene(notify);
	
	dungeonPlayer->OnKillMonster(monster->id, exp);
	player->OnOwnedNpcDied(monster->GetMonsterBaseTypeID(), monster->typeId, GetDungeonID());

	// 如果是活动怪，有可能有提示
	if (monster->activityMonsterId)
	{
		auto activityMonster = WSActivityMonsterMgr::Instance()->FindMonster(monster->activityMonsterId);
		if (activityMonster && activityMonster->killNotice)
		{
			// 合并掉落
			std::map<UInt32, UInt32> dropItems;
			for (auto& dropItem : monster->dropItems)
			{
				if (!dropItem.notice || dropItem.roleId != dungeonPlayer->GetID())
				{
					continue;
				}

				if (dropItems.find(dropItem.typeId) == dropItems.end())
				{
					dropItems[dropItem.typeId] = dropItem.num;
				}
				else
				{
					dropItems[dropItem.typeId] += dropItem.num;
				}
			}

			std::string itemStr = "";
			for (auto itr : dropItems)
			{
				auto typeId = itr.first;
				auto num = itr.second;
				auto itemData = ItemDataEntryMgr::Instance()->FindEntry(typeId);
				if (!itemData)
				{
					continue;
				}

				std::string str = "";
				if (num == 1)
				{
					str = itemData->name;
				}
				else
				{
					str = itemData->name + "*" + Avalon::StringUtil::ConvertToString(num);
				}

				if (itemStr == "")
				{
					itemStr = str;
				}
				else
				{
					itemStr = itemStr + "," + str;
				}
			}

			if (itemStr != "")
			{
				AnnouncementMgr::Instance()->PublicAnnouncement(0, activityMonster->killNotice, CURRENT_TIME.Sec(), player->GetName(), itemStr);
			}
		}
	}

	if (monster->dailyMaxDropNum)
	{
		auto index = _GetIndexByAreaID(monster->areaId);
		SetDungeonAreaIndex(dungeonPlayer->GetID(), m_dungeonId, index);

		//UInt32 dailyDropNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_MONSTER_DROP, dungeonPlayer->GetID(), monster->GetMonsterNormalHardTypeID());
		//UInt32 addedDropNum = dungeonPlayer->GetAddedDropNum();

		//// 今天的次数还没用完
		//if (dailyDropNum < monster->dailyMaxDropNum + addedDropNum)
		//{
		//	SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON_MONSTER_DROP, dungeonPlayer->GetID(), monster->GetMonsterNormalHardTypeID(), 1);

		//	InfoLogStream << "race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") add monster(" << monster->GetMonsterNormalHardTypeID() << ")'s drop num(" << dailyDropNum + 1 << ")." << LogStream::eos;
		//}
	}


	return true;
}

PlayerDropItem* DungeonRace::_FindDropItem(UInt32 itemId, DungeonPlayer* player, bool mustEnteredArea)
{
	if (!player)
	{
		return NULL;
	}

	for (auto area : m_areas)
	{
		if (!area)
		{
			continue;
		}

		if (mustEnteredArea && !player->HasEnteredArea(area->GetID()))
		{
			continue;
		}

		PlayerDropItem* item = area->FindDropItem(itemId);
		if (item)
		{
			return item;
		}
	}

	// 不在普通怪物掉落中，就从BOSS掉落中找
	auto bossItem = player->FindBossDropItem(itemId);
	if (bossItem)
	{
		if (m_dungeonData->costFatiguePerArea > 0 && m_dungeonData->clearMinAreaNum > player->GetEnteredAreaCount())
		{
			InfoLogStream << "race(" << GetID() << ") dungeon(" << GetDungeonID() << ") player(" << player->GetAccID() << ", " 
				<< player->GetID() << ") entered area(" << player->GetEnteredAreaCount() << ") min(" << m_dungeonData->clearMinAreaNum << ")." << LogStream::eos;
			bossItem = NULL;
		}
	}

	return bossItem;
}

UInt8 DungeonRace::_RandomItemStrenth(UInt32 itemId)
{
	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!itemEntry || itemEntry->type != ITEM_EQUIP)
	{
		return 0;
	}

	UInt32 tmpKey = DropStrengthenControlDataEntry::GenKey(itemEntry->needLevel, itemEntry->color, itemEntry->color2);
	auto controlDataEntry = DropStrengthenControlDataEntryMgr::Instance()->FindEntry(tmpKey);
	if (!controlDataEntry || controlDataEntry->strengthen.size() != controlDataEntry->weight.size())
	{
		return 0;
	}

	UInt32 totalWeight = 0;
	for (size_t i = 0; i < controlDataEntry->weight.size(); ++i)
	{
		totalWeight += controlDataEntry->weight[i];
	}

	UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
	for (size_t i = 0; i < controlDataEntry->weight.size(); ++i)
	{
		if (controlDataEntry->weight[i] >= randWeight)
		{
			return controlDataEntry->strengthen[i];
		}
		else
		{
			randWeight -= controlDataEntry->weight[i];
		}
	}

	return 0;
}

void DungeonRace::_PushItemToReward(ItemRewardVec& reward, UInt32 itemId, UInt32 num, UInt8 strenth, UInt8 equipType)
{
	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!itemEntry)
	{
		ErrorLogStream << "push item to mail, but can't find item(" << itemId << ")." << LogStream::eos;
		return;
	}

	// 尝试合并到原有的物品上
	for (auto& item : reward)
	{
		if (item.id != itemId || item.strenth != strenth || item.equipType != equipType)
		{
			continue;
		}

		UInt16 remainNum = itemEntry->maxNum - item.num;
		if (remainNum == 0)
		{
			continue;
		}

		UInt16 addedNum = remainNum < num ? remainNum : num;
		item.num += addedNum;
		num -= addedNum;
		if (num == 0)
		{
			return;
		}
	}

	// 说明还有剩余
	while (num > 0 && itemEntry->maxNum > 0)
	{
		int pileNum = num > itemEntry->maxNum ? itemEntry->maxNum : num;

		ItemReward item;
		item.id = itemId;
		item.num = pileNum;
		item.strenth = strenth;
		item.equipType = equipType;
		reward.push_back(item);

		num -= pileNum;
	}

	if (num > 0)
	{
		ErrorLogStream << "add item(" << itemId << ") num(" << num << ") failed." << LogStream::eos;
	}
}

UInt32 DungeonRace::_GetRaceEndExpReward(DungeonPlayer* player, LevelScore score, DungeonAdditionInfo& addition)
{
	// 助战模式下不能获得结算经验
	if (!m_dungeonData || !player || player->IsAssist())
	{
		return 0;
	}

	// 只有结算收益的图，如果失败了就没有结算经验
	if (m_dungeonData->onlyRaceEndProfit && score <= LS_C)
	{
		return 0;
	}

	int expAdditionByScore[LS_NUM] = { -30, -25, -20, -15, -10, 0 };
	int endExpAddition = score >= LS_NUM ? 0 : expAdditionByScore[score];
	int expAddition = 0;

	// 当前难度结算经验
	UInt32 baseExp = _GetDungeonRaceEndExp(m_dungeonData->id, player->GetLevel());
	// 普通难度基础经验
	UInt32 normalBaseExp = baseExp;

	auto normalHardDungeon = DungeonDataEntryMgr::Instance()->FindEntry(_GetNormalHardDungeonID(m_dungeonId));
	if (normalHardDungeon)
	{
		normalBaseExp = _GetDungeonRaceEndExp(normalHardDungeon->id, player->GetLevel());;
	}

	/*
		新加功能，失败情况下 经验 = 基础经验 * 通过的房间数 / 最小通关房间数
																		add by zjy
																		2016.11.11
	*/
	if (score == LS_C)
	{
		auto enteredAreaCount =  player->GetEnteredAreaCount();
		if (enteredAreaCount <= 1)
		{
			baseExp = 0;
			normalBaseExp = 0;
		}

		auto clearAreaCount = enteredAreaCount - 1;
		if (clearAreaCount < (int)m_dungeonData->clearMinAreaNum && m_dungeonData->clearMinAreaNum)
		{
			baseExp = (UInt32)(baseExp * (float)clearAreaCount / m_dungeonData->clearMinAreaNum);
			normalBaseExp = (UInt32)(normalBaseExp * (float)clearAreaCount / m_dungeonData->clearMinAreaNum);
		}

		UInt32 maxExp = 1500 * clearAreaCount;
		baseExp = baseExp > maxExp ? maxExp : baseExp;
		normalBaseExp = normalBaseExp > maxExp ? maxExp : normalBaseExp;
	}

	auto guildSkillExpAddition = 0;
	auto buffExpAddition = 0;
	auto vipExpAddition = 0;
	auto masterExpAddition = 0;
	auto opActivityExpAddition = 0;
	auto friendExpAddition = 0;
	auto adventureTeamExpAddition = 0;
	if (player)
	{
		//vipExpAddition = player->GetVipExpAddition();
		//guildSkillExpAddition = player->GetGuildSkillAddition();
		//buffExpAddition = player->GetBuffExpAddition();
		//masterExpAddition = player->GetMasterExpAddition();
		vipExpAddition = player->GetDungenAdditionByType(DART_VIP_EXP_RATE);
		guildSkillExpAddition = player->GetDungenAdditionByType(DART_GUID_SKILL_EXP_RATE);
		buffExpAddition = player->GetDungenAdditionByType(DART_BUFF_EXP_RATE);
		masterExpAddition = player->GetDungenAdditionByType(DART_MASTER_EXP_RATE);
		opActivityExpAddition = player->GetDungenAdditionByType(DART_OP_ACTVITY_EXP_RATE);
		friendExpAddition = player->GetDungenAdditionByType(DART_FRIEND_EXP_RATE);
		adventureTeamExpAddition = player->GetDungenAdditionByType(DART_ADVENTURE_TEAM_EXP_RATE);
		expAddition += guildSkillExpAddition + buffExpAddition + vipExpAddition + masterExpAddition + opActivityExpAddition + friendExpAddition + adventureTeamExpAddition;
	}

	// 基础经验(普通难度C的经验)
	addition.addition[DUNGEON_EXP_BASE] = normalBaseExp * (100 + expAdditionByScore[0]) / 100;
	// 难度加成
	addition.addition[DUNGEON_EXP_ADD_HARD] = baseExp * (100 + endExpAddition) / 100 - normalBaseExp * (100 + endExpAddition) / 100;
	// 评分加成
	addition.addition[DUNGEON_EXP_ADD_SCORE] = normalBaseExp * (100 + endExpAddition) / 100 - normalBaseExp * (100 + expAdditionByScore[0]) / 100;
	// 公会技能
	addition.addition[DUNGEON_EXP_ADD_GUILD] = baseExp * (100 + endExpAddition) / 100 * (guildSkillExpAddition) / 100;
	// buff
	addition.addition[DUNGEON_EXP_ADD_BUFF] = baseExp * (100 + endExpAddition) / 100 * (buffExpAddition) / 100;
	// vip加成
	addition.addition[DUNGEON_EXP_ADD_VIP] = baseExp * (100 + endExpAddition) / 100 * (vipExpAddition) / 100;
	// 师徒
	addition.addition[DUNGEON_EXP_ADD_MASTER] = baseExp * (100 + endExpAddition) / 100 * (masterExpAddition) / 100;
	//好友
	addition.addition[DUNGEON_EXP_ADD_FRIEND] = baseExp * (100 + endExpAddition) / 100 * (friendExpAddition) / 100;
	// 冒险队
	addition.addition[DUNGEON_EXP_ADD_ADVENTURE_TEAM] = baseExp * (100 + endExpAddition) / 100 * (adventureTeamExpAddition) / 100;

	UInt32 finalExp = baseExp * (100 + endExpAddition) / 100 * (100 + expAddition) / 100;
	return finalExp;
}

UInt32 DungeonRace::_GetDungeonRaceEndExp(UInt32 dungeonId, UInt16 level)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 0;
	}

	if (dungeonData->isExpLevelAdapt)
	{
		return LevelAdapterDataEntryMgr::Instance()->GetValue(dungeonData->expReward, level);
	}
	return dungeonData->expReward;
}

UInt16 DungeonRace::_GetAdapterLevel()
{
	if (m_dungeonData->monsterLevelAdaptType == DMLAT_MONSTER)
	{
		return 0;
	}
	else if (m_dungeonData->monsterLevelAdaptType == DMLAT_DUNGEON)
	{
		return m_dungeonData->level;
	}
	else if (m_dungeonData->monsterLevelAdaptType == DMLAT_TEAM)
	{
		UInt32 totalLevel = 0;
		UInt32 num = 0;
		for (auto itr : m_players)
		{
			auto dungeonPlayer = itr.second;
			if (!dungeonPlayer)
			{
				continue;
			}

			num++;
			totalLevel += dungeonPlayer->GetLevel();
		}

		if (num == 0)
		{
			return 0;
		}

		UInt32 level = totalLevel / num;
		if (m_dungeonData->maxHardAdaptLevel > 0 && m_dungeonData->maxHardAdaptLevel < level)
		{
			level = m_dungeonData->maxHardAdaptLevel;
		}

		return level;
	}
	else if (m_dungeonData->monsterLevelAdaptType == DMLAT_CITY_MONSTER)
	{
		std::vector<UInt16> levels;
		for (auto itr : m_players)
		{
			auto dungeonPlayer = itr.second;
			if (!dungeonPlayer)
			{
				continue;
			}

			levels.push_back(dungeonPlayer->GetLevel());
		}

		// 从大到小排序
		std::sort(levels.rbegin(), levels.rend());
		if (levels.size() == 1)
		{
			return levels[0];
		}
		else if (levels.size() == 2)
		{
			return (levels[0] + levels[1]) / 2;
		}
		else if (levels.size() == 3)
		{
			return (levels[0] * 50 + levels[1] * 30 + levels[2] * 20) / 100;
		}
	}
	
	return 0;
}

DungeonArea* DungeonRace::_FindArea(UInt32 areaId)
{
	for (auto area : m_areas)
	{
		if (!area)
		{
			continue;
		}

		if (area->GetID() == areaId)
		{
			return area;
		}
	}

	return NULL;
}

DungeonMonster* DungeonRace::_FindMonster(UInt32 id)
{
	for (auto area : m_areas)
	{
		if (!area)
		{
			continue;
		}

		auto monster = area->FindMonster(id);
		if (monster)
		{
			return monster;
		}
	}

	return NULL;
}

DungeonPrefixLimitDataEntry* DungeonRace::_GetPrefixMonsterLimit(DungeonHardType type, UInt32 level)
{
	class CountPrefixNumVisitor : public Avalon::DataEntryVisitor<DungeonPrefixLimitDataEntry>
	{
	public:
		bool operator()(DungeonPrefixLimitDataEntry *dataEntry)
		{
			if (dataEntry == NULL)
			{
				return false;
			}

			datas.push_back(dataEntry);
			return true;
		}

		std::vector<DungeonPrefixLimitDataEntry*> datas;
	};

	CountPrefixNumVisitor visitor;
	DungeonPrefixLimitDataMgr::Instance()->Visit(visitor);

	for (auto data : visitor.datas)
	{
		if (!data)
		{
			continue;
		}

		if (data->hardType == type && data->minLevel <= level && data->maxLevel >= level)
		{
			return data;
		}
	}

	return NULL;
}

UInt8 DungeonRace::_GetPerAreaPrefixMonsterMaxNum(UInt32 totalNum)
{
	UInt32 areaNum = m_areas.size();
	if (areaNum == 0)
	{
		return 0;
	}

	return totalNum / areaNum + ((totalNum % areaNum != 0) ? 1 : 0);
}

UInt16 DungeonRace::_HellModeDropAddition()
{
	if (!m_dungeonData)
	{
		return 0;
	}

	static UInt16 s_hellDropAddition[] = { 100, 120, 120, 150, 150, 200, 200, 300 };
	UInt32 index = (m_dungeonData->hardType > LHT_NORMAL) ? ((m_dungeonData->hardType - 1) * 2) : 0;
	if (m_hellHardMode == HELL_MODE_HARD)
	{
		index += 1;
	}

	if (index >= sizeof(s_hellDropAddition) / sizeof(s_hellDropAddition[0]))
	{
		return 0;
	}

	return s_hellDropAddition[index];
}

UInt32 DungeonRace::_MultiRaceEndDropCost(UInt8 multi)
{
	if (multi == 1)
	{
		return 0;
	}

	for (auto& costInfo : m_dungeonData->raceEndDropMultiCost)
	{
		if (costInfo.multi == multi)
		{
			return costInfo.cost;
		}
	}

	return UINT32_MAX;
}

UInt8 DungeonRace::_GetRaceEndDropBaseMulti(DungeonPlayer* dungeonPlayer)
{
	UInt8 multi = 1;
	if (!dungeonPlayer || !m_dungeonData)
	{
		return multi;
	}

	auto dailyCount = dungeonPlayer->GetDailyCount();
	if (dailyCount == 0 || dailyCount > m_dungeonData->raceEndDropBaseMulti.size())
	{
		return multi;
	}

	return m_dungeonData->raceEndDropBaseMulti[dailyCount - 1];
}

const std::vector<UInt32>& DungeonRace::_GetHellDropID(DungeonHellMode mode)
{
	static std::vector<UInt32> s_Empty;
	s_Empty.clear();

	if (!m_dungeonData)
	{
		return s_Empty;
	}

	size_t index = mode;
	if (index < sizeof(m_dungeonData->hellDrops) / sizeof(m_dungeonData->hellDrops[0]))
	{
		s_Empty = m_dungeonData->hellDrops[index];
	}

	//拼接深渊柱子的活动掉落
	if (m_dungeonData->hellActivityDropItems.size() > 0)
	{
		s_Empty.insert(s_Empty.end(), m_dungeonData->hellActivityDropItems.begin(), m_dungeonData->hellActivityDropItems.end());
	}
	
	return s_Empty;
}

void DungeonRace::_BroadcastProtocol(Avalon::Protocol& protocol)
{
	for (auto itr : m_players)
	{
		auto dungeonPlayer = itr.second;
		if (!dungeonPlayer)
		{
			continue;
		}

		auto player = WSPlayerMgr::Instance()->FindPlayer(dungeonPlayer->GetID());
		if (player)
		{
			player->SendProtocol(protocol);
		}
	}
}

UInt32 DungeonRace::_GetTowerFloorByAreaID(UInt32 dungeonId, UInt32 areaId)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 0;
	}

	auto index = dungeonData->GetAreaIndex(areaId);
	if (index < 0)
	{
		ErrorLogStream << "dungeon(" << dungeonId << ") can't find area(" << areaId << ") in area list." << LogStream::eos;
		return 0;
	}

	return index + 1;
}

UInt32 DungeonRace::_GetIndexByAreaID(UInt32 areaId)
{
	auto index = m_dungeonData->GetAreaIndex(areaId);
	if (index < 0)
	{
		ErrorLogStream << "dungeon(" << m_dungeonData->GetKey() << ") can't find area(" << areaId << ") in area list." << LogStream::eos;
		return 0;
	}

	return index + 1;
}

UInt32 DungeonRace::_GetNormalHardDungeonID(UInt32 dungeonId)
{
	// ID的个位是难度编码
	return dungeonId / 10 * 10;
}

void DungeonRace::_NotifyRelayPlayerEvent(ObjID_t roleId, UInt8 type, ObjID_t param1, ObjID_t param2)
{
	CLProtocol::SceneServerSyncPlayerEvent sync;
	sync.raceId = GetSession();
	sync.roleId = roleId;
	sync.type = type;
	sync.param1 = param1;
	sync.param2 = param2;
	Router::SendToRelay(GetRelayServerID(), sync);
}

bool DungeonRace::_CheckDungeonUsedTime(DungeonPlayer* dungeonPlayer, UInt32 floor, UInt32 usedTime)
{
	if (!WSApplication::Instance()->IsCheckMD5())
	{
		return true;
	}

	// 多人的情况下不验证
	if (!dungeonPlayer || m_players.size() > 1)
	{
		return true;
	}

	auto data = DungeonFinishTimeDataEntryMgr::Instance()->FindEntry(DungeonFinishTimeDataEntry::MakeKey(m_dungeonId, floor));
	if (!data)
	{
		return true;
	}

	UInt16 level = dungeonPlayer->GetLevel();
	UInt32 minTime = 0;
	if (level <= 29)
	{
		minTime = data->minTime_29;
	}
	else if (level <= 39)
	{
		minTime = data->minTime_39;
	}
	else if (level <= 49)
	{
		minTime = data->minTime_49;
	}
	else if (level <= 59)
	{
		minTime = data->minTime_59;
	}
	else
	{
		minTime = data->minTime_69;
	}

	// 如果最小时间是0，说明不用验证
	if (minTime == 0)
	{
		return true;
	}

	if (usedTime < minTime * Avalon::Time::MSECS_OF_SEC)
	{
		ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << dungeonPlayer->GetID() << ") level(" << dungeonPlayer->GetLevel() << ") occu(" << dungeonPlayer->GetOccu() << ") floor(" << floor << ") used time(" << usedTime << ")ms min(" << minTime * Avalon::Time::MSECS_OF_SEC << ")." << LogStream::eos;
		return false;
	}

	return true;
}

void DungeonRace::_OnPlayerCheat(WSPlayer* player, PlayerCheatReason cheatReason)
{
	if (!player)
	{
		return;
	}

	player->SendCheatLog(cheatReason, GetID(), GetDungeonID());

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer || dungeonPlayer->IsCheat())
	{
		return;
	}

	dungeonPlayer->SetCheat();
	InfoLogStream << "race(" << GetID() << ") player(" << player->GetAccId() << ", " << player->GetID() << ") cheat..." << LogStream::eos;

	// 自动封号
	// ...
	
	if (WSApplication::Instance()->GetCheatPunish() <= CHEAT_PUNISH_LOG)
	{
		return;
	}

	player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);

	if (WSApplication::Instance()->GetCheatPunish() <= CHEAT_PUNISH_KICK)
{
		return;
	}

	auto times = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_DUNGEON_CHEAT, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), 0);
	SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_TOTAL_DUNGEON_CHEAT, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), 0, 1);
	if (times == 0)
	{
		UInt32 punishSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHEAT_FIRST_TIME_PUNISH_SEC);
		PunishmentMgr::Instance()->Punish(player->GetAccId(), PUNISH_FORBIDLOGIN, punishSec, FORBID_LOGIN_CHEAT_FIRST_TIME);
}
	else
	{
		UInt32 punishSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHEAT_FOREVER_PUNISH_SEC);
		PunishmentMgr::Instance()->Punish(player->GetAccId(), PUNISH_FORBIDLOGIN, punishSec, FORBID_LOGIN_CHEAT_FOREVER);
	}
}

void DungeonRace::_OnPlayerSpeedUpCheat(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	player->SendCheatLog(PLAYER_CHAET_REASON_SPEED_UP, GetID(), GetDungeonID());

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer || dungeonPlayer->IsSpeedCheat())
	{
		return;
	}

	dungeonPlayer->SetSpeedCheat();
	InfoLogStream << "race(" << GetID() << ") player(" << player->GetAccId() << ", " << player->GetID() << ") speed cheat..." << LogStream::eos;

	if (!WSApplication::Instance()->KickWhenSpeedCheat())
	{
		return;
	}

	player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);
}

DungeonChestTask* DungeonRace::_FindDungeonChestTask(ObjID_t roleId)
{
	for (auto& task : m_chestTasks)
	{
		if (task.GetRoleID() == roleId)
		{
			return &task;
		}
	}

	return NULL;
}

UInt8 DungeonRace::_GenUnopenedChestPos()
{
	std::vector<UInt8> allUnopenedPos;
	for (int i = 0; i < 4; i++)
	{
		if (!(m_openedChestMask & (1 << i)))
		{
			allUnopenedPos.push_back((UInt8)i);
		}
	}

	// 出错了...
	if (allUnopenedPos.empty())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") failed to gen unopened chest pos." << LogStream::eos;
		return UINT8_MAX;
	}
	
	int index = Avalon::Random::RandBetween(0, allUnopenedPos.size() - 1);
	return allUnopenedPos[index];
	
}

DungeonChestType DungeonRace::_GetChestTypeByPos(UInt8 pos)
{
	if (pos < 4)
	{
		return DCT_NORMAL;
	}
	else if (pos < 8)
	{
		return DCT_PAY;
	}
	else
	{
		return DCT_NUM;
	}
}

bool DungeonRace::_OpenChest(WSPlayer* player, DungeonChestTask* task, DungeonChestType type, UInt8 pos)
{
	if (!player || !task)
	{
		return false;
	}

	DungeonChest* chest = task->GetChest(type);
	if (!chest || chest->isOpened)
	{
		return false;
	}

	ItemRewardVec itemReward;

	if (chest->equipType == EQUIP_NORMAL)
	{
		chest->strenth = DropStrengthenControlDataEntryMgr::Instance()->RandomItemStrenth(chest->typeId);
	}
	else
	{
		chest->strenth = 0;
	}
	_PushItemToReward(itemReward, chest->typeId, chest->num, chest->strenth, chest->equipType);

	chest->isOpened = true;

	// 发给客户端
	CLProtocol::SceneDungeonOpenChestRes res;
	res.roleId = player->GetID();
	res.pos = pos;
	res.chest = *chest;

	_BroadcastProtocol(res);

	for (auto item : itemReward)
	{
		DebugLogStream << "chestType:" << type << " race(" << GetID() << ") player(" << player->GetID() << ") get chest item(" << item.id << ", " << item.num << ")." << LogStream::eos;
	}

	if (type == DCT_VIP && !player->IsInMonthCard())
	{
		// 放入月卡暂存箱
		CLProtocol::SceneItemDepositSync sync;
		sync.roleId = player->GetID();
		sync.dungeonId = GetDungeonID();
		sync.rewardVec = itemReward;
		player->SendToScene(sync);
	}
	else
	{
		CLProtocol::WorldDungeonNotifyGotItems notify;
		notify.roleId = player->GetID();
		notify.dungeonId = GetDungeonID();
		notify.items = itemReward;
		notify.gold = chest->goldReward;
		notify.reason = GetReason(SOURCE_TYPE_DUNGEON_BOX, m_dungeonId, type);
		player->SendToScene(notify);
	}

	return true;
}

bool DungeonRace::_CheckDropRequestMd5(DungeonPlayer* dungeonPlayer, UInt32 lastFrame, const std::vector<UInt32>& dropItemIds, UInt8 md5[16])
{
	if (!dungeonPlayer)
	{
		return false;
	}

	std::stringstream ss;
	ss << lastFrame;
	for (auto id : dropItemIds)
	{
		ss << id;
	}
	ss << "###" << m_encryptKey;

	UInt8 newMd5[16];
	std::string source = ss.str();
	Avalon::md5_encode((char*)newMd5, (void*)source.c_str(), source.length());

	for (UInt32 i = 0; i < sizeof(newMd5); i++)
	{
		if (md5[i] != newMd5[i])
		{
			return false;
		}
	}

	return true;
}

bool DungeonRace::_CheckClearAreaRequestMd5(DungeonPlayer* dungeonPlayer, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16])
{
	if (!dungeonPlayer)
	{
		return false;
	}

	std::stringstream ss;
	ss << usedTime << remainHp << remainMp << "###" << m_encryptKey;

	UInt8 newMd5[16];
	std::string source = ss.str();
	Avalon::md5_encode((char*)newMd5, (void*)source.c_str(), source.length());

	for (size_t i = 0; i < sizeof(newMd5); ++i)
	{
		if (md5[i] != newMd5[i])
		{
			return false;
		}
	}

	return true;
}

bool DungeonRace::_CheckRaceEndRequestMd5(DungeonPlayer* dungeonPlayer, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16])
{
	// 多人情况下不检查结算
	if (m_players.size() > 1)
	{
		return true;
	}

	if (!dungeonPlayer)
	{
		return false;
	}

	std::stringstream ss;
	ss << (UInt32)score << beHitCount << usedTime << "###" << m_encryptKey;

	UInt8 newMd5[16];
	std::string source = ss.str();
	Avalon::md5_encode((char*)newMd5, (void*)source.c_str(), source.length());

	for (UInt32 i = 0; i < sizeof(newMd5); i++)
	{
		if (md5[i] != newMd5[i])
		{
			return false;
		}
	}

	return true;
}

void DungeonRace::_GenEncryptKey(CLProtocol::SceneDungeonStartRes& res)
{
	res.key1 = Avalon::Random::Rand();
	res.key2 = Avalon::Random::Rand();
	res.key3 = Avalon::Random::Rand();
	res.key4 = Avalon::Random::Rand();

	m_encryptKey = ((res.key1 & res.key4) ^ (res.key3 | (res.key4 << 16))) | res.key2;
}

void DungeonRace::_RandMysticalMerchant()
{
	class MysticalMerchantDataVistor : public Avalon::DataEntryVisitor<MysticalMerchantDataEntry>
	{
	public:
		bool operator()(MysticalMerchantDataEntry* dataEntry)
		{
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<MysticalMerchantDataEntry*> datas;
	};

	MysticalMerchantDataVistor vistor;
	MysticalMerchantDataEntryMgr::Instance()->Visit(vistor);

	// 权重集
	std::vector<UInt32> wts;
	// 随机阈值
	Int32 randThreshold = vistor.datas.size();

	for (auto data : vistor.datas)
	{
		if (!data) continue;

		wts.push_back(data->weight);
	}

	if (wts.empty())
	{
		ErrorLogStream << "Weights are empty!" << LogStream::eos;
		return;
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= randThreshold || hitIdx < 0)
	{
		ErrorLogStream << "Rand weight failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return;
	}

	auto hitData = vistor.datas[hitIdx];
	if (!hitData)
	{
		ErrorLogStream << "Hit data is null, hitIdx=" << hitIdx << LogStream::eos;
		return;
	}

	m_MysticalMerchantId = hitData->id;
}

void  DungeonRace::checkAndAddIntimacy(UInt32 areaId)
{
	if (m_teamId < 1 || m_players.size() < 2)
	{
		return;
	}
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (player && !player->HasClearedArea(areaId))
		{
			return;
		}
	}
	
	//房间内玩家都首次通关房间
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (player)
		{
			WSPlayer* selfPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(player->GetID());
			if (selfPlayer)
			{
				for (auto itrnel : m_players)
				{
					auto other = itrnel.second;
					if (other)
					{
						if (player->GetID() == other->GetID())
						{
							continue;
						}
						WSPlayer* otherPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(other->GetID());
						if (otherPlayer)
						{
							if (WSRelationMgr::IsMutualFriend(selfPlayer, otherPlayer)
								|| WSRelationMgr::IsMasterDisciple(selfPlayer, otherPlayer)) //互为好友或师徒
							{
								selfPlayer->AddIntimacy(otherPlayer->GetID(), 1);
							}
						}
					}
				}
			}
		}
	}
}
void DungeonRace::SendMasterDiscipleInteractionLog()
{
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (player)
		{
			WSPlayer* selfPlayer = WSPlayerMgr::Instance()->FindPlayer(player->GetID());
			if (selfPlayer)
			{
				for (auto itrnel : m_players)
				{
					auto other = itrnel.second;
					if (other)
					{
						if (player->GetID() == other->GetID())
						{
							continue;
						}

						if (selfPlayer->GetRelationMgr().FindRelation(RelationType::RELATION_DISCIPLE, other->GetID()))
						{
							selfPlayer->SendMasterDiscipleInteractionLog(other->GetID(), selfPlayer->GetID(), GetDungeonID(), UInt32(CURRENT_TIME.Sec()));
						}
					}
				}
			}
		}
	}
	//SendUdpLog("mastersys_interaction", LOG_MASTERSYS_INTERACTION, discipleId, masterId, dungeonId, timestamp);
}

void DungeonRace::_ClearPlayerHiddenRoomFlag()
{
	UInt32 normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);

	if (normalHardDungeonId != 3201000)
	{
		return;
	}

	for (auto itr : m_players)
	{
		auto roleid = itr.first;
		SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_DUNGEON_HIDDEN_ROOM_FLAG,
			SYS_BEHAVIOR_OWNER_ROLE, roleid, normalHardDungeonId, 0);
	}
}

void DungeonRace::_GenRollDrops()
{
	if (m_isGeneratePublishDrops)
	{
		return;
	}

	if (!GetDungeonData())
	{
		return;
	}

	if (GetDungeonData()->rollDropId == 0)
	{
		return;
	}

	m_isGeneratePublishDrops = true;

	UInt32 dropNum = _GenRollDropNum();

	//初始玩家roll信息
	std::vector<PlayerRollInfo> initPlayerRollInfo;
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player)
		{
			continue;
		}
		if (player->IsAssist())
		{
			continue;
		}
		PlayerRollInfo info;
		info.playerId = player->GetID();
		info.playerName = player->GetRacePlayerInfo().name;
		initPlayerRollInfo.push_back(info);
	}

	UInt32 index = 1;
	for (UInt32 i = 0; i < dropNum; ++i)
	{
		auto dropResult = DropItemMgr::Instance()->GenDropItems(GetDungeonData()->GetKey(), 0, GetDungeonData()->rollDropId, 0);

		for (auto dropItem : dropResult.dropItems)
		{
			DungeonRollItem rollItem;
			rollItem.index = index++;
			rollItem.dropItem = dropItem;
			rollItem.playerRollInfos = initPlayerRollInfo;
			m_rollItems[rollItem.index] = rollItem;
		}
	}

	if (dropNum == 1)
	{
		m_isSingleRoll = true;
	}

	UInt32 rollTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DUNGEON_ROLL_TIME) + 2;
	rollTime = (rollTime == 0) ? ROLL_DROP_WAITE_TIEM : rollTime;
	m_rollEndTime = (UInt32)CURRENT_TIME.Sec() + rollTime;
}

UInt32 DungeonRace::_GenRollDropNum()
{
	UInt32 num = 0;
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player)
		{
			continue;
		}
		if (player->IsAssist())
		{
			continue;
		}
		++num;
	}
	return num;
}

bool  DungeonRace::_IsEliteDungeon()
{
	return GetDungeonData() && GetDungeonData()->threeType == DTT_TEAM_ELITE;
}

DungeonRollItem* DungeonRace::_FindRollItem(UInt8 index)
{
	auto iter = m_rollItems.find(index);
	if (iter == m_rollItems.end())
	{
		return NULL;
	}

	return &iter->second;
}

void DungeonRace::_SettleRollDrops()
{
	if (m_isRollSettled)
	{
		return;
	}

	if (m_rollItems.size() == 0 || m_isSingleRoll)
	{
		return;
	}

	//for (auto item : m_rollItems)
	//{
	//	DungeonRollItem& rollItem = item.second;
	//	_SettleOneRollDrop(rollItem);
	//}

	std::map<UInt8, DungeonRollItem>::iterator  iter = m_rollItems.begin();
	for (; iter != m_rollItems.end(); iter++)
	{
		DungeonRollItem& rollItem = iter->second;
		_SettleOneRollDrop(rollItem);
	}

	CLProtocol::WorldDungeonRollItemResult res;
	for (auto item : m_rollItems)
	{
		DungeonRollItem& rollItem = item.second;
		for (auto playerRollInfo : rollItem.playerRollInfos)
		{
			if (playerRollInfo.opType == RIE_INVALID)
			{
				continue;
			}
			RollDropResultItem rollResult;
			rollResult.rollIndex = rollItem.index;
			rollResult.opType = playerRollInfo.opType;
			rollResult.point = playerRollInfo.point;
			rollResult.playerId = playerRollInfo.playerId;
			rollResult.playerName = playerRollInfo.playerName;
			res.items.push_back(rollResult);
		}
	}

	for (auto it : m_players)
	{
		DungeonPlayer* dungeonPlayer = it.second;
		if (!dungeonPlayer || dungeonPlayer->IsAssist())
		{
			continue;
		}
		auto player = WSPlayerMgr::Instance()->FindPlayer(dungeonPlayer->GetID());
		if (player)
		{
			player->SendProtocol(res);
		}
	}

	//_BroadcastProtocol(res);
	m_isRollSettled = true;

	std::ostringstream logss;
	logss << "roll drops settle list ";
	UInt32 num = 0;
	for (auto it : res.items)
	{
		logss << " [" << ++num << "] : ";
		logss << "rollIndex = " << (UInt32)it.rollIndex << ", playerId = " << it.playerId << ", playerName = " << it.playerName
			<< ", opType = " << (UInt32)it.opType << ", point = " << it.point;
	}
	InfoLogStream << logss.str() << LogStream::eos;
}

void DungeonRace::_SettleOneRollDrop(DungeonRollItem& rollItem)
{
	//检查是否全部谦让
	bool isAllModest = true;
	for (auto rollInfo : rollItem.playerRollInfos)
	{
		if (rollInfo.opType == RIE_NEED)
		{
			isAllModest = false;
			break;
		}
	}
	//全部谦让,重新生成roll点
	if (isAllModest)
	{
		rollItem.RandomRollPointForAllModest();
	}

	//选择roll点最大的发送掉落
	rollItem.SendDropItem(GetID(), GetDungeonID());

	//发送日志
	for (auto rollInfo : rollItem.playerRollInfos)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(rollInfo.playerId);
		if (!player)
		{
			continue;
		}
		player->SendGungeonRollUdpLog(GetDungeonID(), rollItem.index, 
			rollItem.dropItem.typeId, rollItem.dropItem.num, rollInfo.opType, rollInfo.point);
	}
}

bool DungeonRace::_GetOnlyNotAssistPlayer(ObjID_t& playerId)
{
	UInt32 num = 0;
	for (auto itr : m_players)
	{
		auto player = itr.second;
		if (!player)
		{
			continue;
		}
		if (player->IsAssist())
		{
			continue;
		}
		playerId = player->GetID();
		++num;
	}
	if (num != 1)
	{
		return false;
	}
	return true;
}

void DungeonRace::_SettleRollSingleMode()
{
	if (!m_isSingleRoll)
	{
		return;
	}
	
	if (m_isRollSettled)
	{
		return;
	}

	m_isRollSettled = true;

	UInt64 playerId = 0;
	if (!_GetOnlyNotAssistPlayer(playerId))
	{
		ErrorLogStream << "_SettleRollSingleMode failed _GetOnlyNotAssistPlayer is false race id =" 
			<< GetID() << ", session id = " << GetSession() << LogStream::eos;
		return;
	}

	if (playerId == 0)
	{
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
	if (player)
	{
		CLProtocol::SceneAddRewardReq req;
		req.reason = GetReason(LogSourceType::SOURCE_DUNGEON_ROLL_ADD_REWARD, GetID(), GetDungeonID());
		req.notice = (UInt8)false;
		req.roleId = playerId;
		for (auto rollItem : m_rollItems)
		{
			DropItem& dropItem = rollItem.second.dropItem;
			req.rewards.push_back(ItemReward(dropItem.typeId, dropItem.num, 0, dropItem.strenth, 0, dropItem.equipType, 0));
		}
		Router::BroadcastToScene(req);
		InfoLogStream << "_SettleRollSingleMode success race id =" << GetID() << ", session id = " << GetSession() << ", send reward to pack player : "
			<< playerId << LogStream::eos;
	}
	else
	{
		//不在线,发送邮件
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		std::string strReason = GetReason(LogSourceType::SOURCE_DUNGEON_ROLL_ADD_REWARD, GetID(), GetDungeonID());
		std::vector<ItemReward> depositItems;

		for (auto rollItem : m_rollItems)
		{
			DropItem& dropItem = rollItem.second.dropItem;
			depositItems.push_back(ItemReward(dropItem.typeId, dropItem.num, 0, dropItem.strenth, 0, dropItem.equipType, 0));
		}

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(12014, buffer, sizeof(buffer));
		strSenderName = buffer;

		//标题
		SysNotifyMgr::Instance()->MakeNotify(12015, buffer, sizeof(buffer));
		strTitle = buffer;

		//正文
		SysNotifyMgr::Instance()->MakeNotify(12016, buffer, sizeof(buffer));
		strContent = buffer;

		if (MailBox::SendSysMail(playerId, strSenderName, strTitle, strContent, strReason, depositItems) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "send roll reward mail failed" << "to role(" << playerId << ") title("
				<< strTitle << ")." << LogStream::eos;
		}
		InfoLogStream << "_SettleRollSingleMode success race id =" << GetID() << ", session id = " << GetSession() << ", send reward by mail to player : "
			<< playerId << LogStream::eos;
	}
}

bool  DungeonRace::_CheckCanSettleRollDrop()
{
	if (m_isRollSettled)
	{
		return false;
	}

	if (m_rollEndTime == 0)
	{
		return false;
	}

	if ((UInt32)CURRENT_TIME.Sec() >= m_rollEndTime)
	{
		return true;
	}

	for (auto iter : m_rollItems)
	{
		DungeonRollItem& rollItem = iter.second;
		for (auto rollinfo : rollItem.playerRollInfos)
		{
			if (rollinfo.opType == RIE_INVALID)
			{
				return false;
			}
		}
	}

	return true;
}

UInt32  DungeonRace::HandleOneRollReq(WSPlayer* player, UInt8  dropIndex, UInt8 opType, UInt32& rollPoint)
{
	if (!player)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	DungeonRollItem* rollItem = _FindRollItem(dropIndex);
	if (!rollItem)
	{
		ErrorLogStream << "handle roll fail, not find roll item" << ", player id = " << player->GetID()<< ", dropIndex=" << (UInt32)dropIndex << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (opType > RIE_MODEST || opType <= RIE_INVALID)
	{
		ErrorLogStream << "handle roll fail, err optype" << ", player id = " << player->GetID() << ", optype = " << (UInt32)opType << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	PlayerRollInfo* pPlayerRollInfo = rollItem->GetOnePlayerInfo(player->GetID());
	if (!pPlayerRollInfo)
	{
		ErrorLogStream << "not find PlayerRollInfo player id : "<< player->GetID() << ", accid : "<< player->GetAccId() << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}


	rollPoint = 0;

	pPlayerRollInfo->playerName = player->GetName();
	pPlayerRollInfo->opType = opType;

	if (opType == RIE_NEED) //需要
	{
		//生成roll点
		pPlayerRollInfo->point = rollItem->RandomRollPoint();
		rollPoint = pPlayerRollInfo->point;
		InfoLogStream << "new gen roll point, player id = " << player->GetID() << ", accid = " << player->GetAccId() << ", roll index = "
			<< (UInt32)dropIndex << ", point = " << pPlayerRollInfo->point << LogStream::eos;
	}

	return ErrorCode::SUCCESS;
}
