#include "DungeonRace.h"
#include <AvalonRandom.h>
#include <AvalonMd5.h>

#include <CLErrorCode.h>
#include <CLGameSessionProtocol.h>
#include <CLItemDataEntry.h>
#include <CLTeamProtocol.h>

#include "DungeonRaceMgr.h"
#include "DungeonMonsterGenerator.h"
#include "DSRouter.h"
#include "DSApplication.h"
#include "DSItemTransaction.h"
#include "udplog_typedef.h"
#include "CLCrossDungeonDefine.h"
#include "DSApplication.h"
#include "DungeonSysNotify.h"

/**
*@brief 比赛事务
*/
class DungeonRaceCheckTransaction : public DSItemTransaction
{
public:
	DungeonRaceCheckTransaction(DungeonPlayer *player, ObjID_t id, DungeonRaceTransactionType type)
		: DSItemTransaction(player), m_raceId(id), m_type(type) {
		m_reviveHp = 0;
		m_reviveMp = 0;
		m_reviveItem = 0;
		m_reviveItemNum = 0;
	}
	~DungeonRaceCheckTransaction() {}

	void OnCheckSucced(DSPlayer *pPlayer)
	{
		auto race = DungeonRaceMgr::Instance()->FindDungeonRaceById(m_raceId);
		if (!race)
		{
			UnLock();
			return;
		}

		DungeonPlayer* dungeonPlayer = dynamic_cast<DungeonPlayer*> (pPlayer);
		if (!dungeonPlayer)
		{
			UnLock();
			return;
		}

		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON, race->GetDungeonID());

		bool success = false;
	
		if (m_type == DUNGEON_RACE_REVIVE)
		{
			success = race->OnCheckReviveRet(dungeonPlayer, m_param2.reviveId, m_param.targetId, m_reviveHp, m_reviveMp) == ErrorCode::SUCCESS;
			reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON_REVIVE, race->GetDungeonID());
		}

		if (success)
		{
			RemoveItem(reason);
			UInt32 reviveItem = GetReviveItem();
			if (reviveItem > 0)
			{
				auto itemData = ItemDataEntryMgr::Instance()->FindEntry(reviveItem);
				if (itemData && itemData->subType == ST_USE_DEATH_RESURRECTION
					&& race->GetRelayServerID() > 0)
				{
					CLProtocol::SceneServerSyncPlayerEvent sync;
					sync.raceId = race->GetSession();
					sync.roleId = dungeonPlayer->GetID();
					sync.type = SYNC_USE_ITEM;
					sync.param1 = reviveItem;
					sync.param2 = GetReviveItemNum();
					DSRouter::Instance()->SendToRelay(race->GetRelayServerID(), sync);
				}
			}
		}
		else
		{
			UnLock();
		}
	}

	void SetParam(UInt8 value) { m_param.pos = value; }
	void SetParam(ObjID_t value) { m_param.targetId = value; }
	void SetParam2(UInt32 value) { m_param2.reviveId = value; }

	void SetReviveItem(UInt32 itemId) { m_reviveItem = itemId; }
	UInt32 GetReviveItem() { return m_reviveItem; }
	void SetReviveItemNum(UInt32 num) { m_reviveItemNum = num; }
	UInt32 GetReviveItemNum() { return m_reviveItemNum; }
	void SetReviveHp(UInt32 hp) { m_reviveHp = hp; }
	UInt32 GetReviveHp() { return  m_reviveHp; }
	void SetReviveMp(UInt32 mp) { m_reviveMp = mp; }
	UInt32 GetReviveMp() { return m_reviveMp; }

	void OnCheckFailed(DSPlayer *pPlayer)
	{
		DungeonPlayer* dungeonPlayer = dynamic_cast<DungeonPlayer*> (pPlayer);
		if (!dungeonPlayer)
		{
			UnLock();
			return;
		}

		if (m_type == DUNGEON_RACE_REVIVE)
		{
			CLProtocol::SceneDungeonReviveRes res;
			res.result = ErrorCode::DUNGEON_REVIVE_NOT_ENOUGH_REVIVE_COIN;
			dungeonPlayer->sendToClient(res);
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
	UInt32							m_reviveHp;
	UInt32							m_reviveMp;
	UInt32							m_reviveItem;
	UInt32							m_reviveItemNum;
};

DungeonRace::DungeonRace()
{
	m_id = 0;
	m_status = DRS_INIT;
	m_dungeonId = 0;
	m_dungeonData = NULL;
	m_startAreaId = 0;
	m_passedAreaIndex = 0;
	m_relayServerId = 0;
	m_startTime = 0;
	m_teamId = 0;
	m_lastFrame = 0;
	m_VSId = 0;
	m_HasSendRaceOver = false;
	m_flagGuildDungeon = false;
	m_isSettled = false;
	m_AddTeamReviveCount = 0;
	m_teamReviveCount = 0;
	m_battleTempRaceId = 0;
}

DungeonRace::~DungeonRace()
{
	if (_IsVerifying())
	{
		
	}

	// 发送战斗结束消息到scene
	if (m_HasSendRaceOver == false && GetMonster().guid > 0 && m_players.size() > 0)
	{
		m_HasSendRaceOver = true;
		auto it = m_players.begin();
		DungeonPlayer* player = it->second;
		if (player)
		{
			CLProtocol::WorldTeamDungeonRaceOver notify;
			notify.raceId = GetID();
			notify.teamId = 0;
			notify.cityMonster = GetMonster();

			player->SendToScene(notify);
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
}

void DungeonRace::Tick(const Avalon::Time& now)
{

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

	DungeonPlayer* player = new DungeonPlayer();

	player->SetServiceSessionId(this->m_serviceId);
	player->SetSourceSceneId(info.soureSceneId);
	player->SetSrcZoneId(info.zoneId);


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

	_GiveOnPickDropItem(player);
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
	//if (!m_dungeonData || !m_isHellMode || m_dungeonData->hellAreaList.empty())
	//{
	//	return;
	//}

	//info.mode = m_hellHardMode;
	//info.areaId = m_hellAreaId;

	//auto hellArea = _FindArea(info.areaId);
	//if (!hellArea)
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") can't find hell area(" << info.areaId << ")." << LogStream::eos;
	//	return;
	//}

	//// 直接生成3波的数据
	//UInt32 maxWave = 3;
	//for (UInt32 i = 0; i < maxWave; i++)
	//{
	//	Protocol::DungeonHellWaveInfo waveInfo;
	//	waveInfo.wave = i;
	//	info.waveInfoes.push_back(waveInfo);
	//}

	//for (auto& monster : hellArea->GetAllMonster())
	//{
	//	if (!monster.isHell)
	//	{
	//		continue;
	//	}

	//	if (monster.hellWave >= maxWave)
	//	{
	//		ErrorLogStream << "dungeon race(" << GetID() << ") dungeon(" << GetDungeonID() << ") has invalid monster("
	//			<< monster.typeId << ") wave(" << monster.hellWave << ")." << LogStream::eos;
	//		continue;
	//	}

	//	auto monsterInfo = monster.GetInfo(roleId);

	//	// 怪物掉落都放到深渊掉落里

	//	for (auto& drop : monsterInfo.dropItems)
	//	{
	//		info.dropItems.push_back(drop);
	//	}
	//	monsterInfo.dropItems.clear();

	//	info.waveInfoes[monster.hellWave].monsters.push_back(monsterInfo);
	//}
}

bool DungeonRace::IsTimeToDestory(const Avalon::Time& now)
{
	//UInt8 ss = GetStatus();
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
	return 1;
	//return SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX, SYS_BEHAVIOR_OWNER_ROLE, roleId, _GetNormalHardDungeonID(dungeonId));
}

UInt32 DungeonRace::SetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex)
{
	UInt32 tmpRecord = GetDungeonAreaIndex(roleId, dungeonId);
	//简单处理一下重置为0以后再设置高位的消息时序问题
// 	if (tmpRecord == 0 && areaIndex > 1)
// 	{
// 		return tmpRecord;
// 	}

	UInt32 tmpMask = 1 << areaIndex;
	if ((tmpRecord & tmpMask) == 0)
	{
		tmpRecord = tmpRecord | tmpMask;
		//SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX, SYS_BEHAVIOR_OWNER_ROLE, roleId, _GetNormalHardDungeonID(dungeonId), tmpRecord);
	}

	return tmpRecord;
}

UInt32 DungeonRace::ResetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId)
{
	//SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX, SYS_BEHAVIOR_OWNER_ROLE, roleId, _GetNormalHardDungeonID(dungeonId), 0);
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

UInt32 DungeonRace::StartRace(ObjID_t session, const Avalon::SockAddr& addr, UInt32 relayServerId)
{
	if (!m_dungeonData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	m_startTime = CURRENT_TIME.Sec();
	m_relayServerId = relayServerId;
	m_sessionId = session;

	// 计算关卡区域
	_GenAreas();

	// 计算BOSS掉落
	_GenBossDropItems();

	// 设置状态
	SetStatus(DRS_RACING);

	auto startArea = _FindArea(m_startAreaId);
	if (!startArea)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find start area("
			<< m_startAreaId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
	}

	InfoLogStream << "dungeon race(" << GetID() << ") start racing." << LogStream::eos;
	std::ostringstream logss;
	logss << "dungeon race(" << GetID() << "), session(" << GetSessionId() << "), " << "start racing, player ids: ";
	for (auto& itr : m_players)
	{
		auto dungeonPlayer = itr.second;
		logss << "accId(" << dungeonPlayer->GetAccID() << "), roleId(" << dungeonPlayer->GetID() << ")" << ", name(" <<dungeonPlayer->GetName() << ")";
	}

	InfoLogStream << logss.str() << LogStream::eos;

	CLProtocol::DungenServerTeamStartRaceRes protocol;
	protocol.result = ErrorCode::SUCCESS;
	protocol.isContinue = false;
	protocol.teamId = GetTeamID();
	protocol.dungeonId = GetDungeonID();
	protocol.gamesessionID = GetSessionId();
	protocol.battleTempRaceId = GetBattleTempRaceId();

	this->SendToService(protocol);

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

		CLProtocol::SceneDungeonStartRes res;
		_GetStartInfo(dungeonPlayer, racePlayerInfoes, addr, res);
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

		dungeonPlayer->sendToClient(res);

		dungeonPlayer->SendDungeonStartUdpLog(GetDungeonID(), GetDungeonData()->type, GetDungeonData()->name, HELL_MODE_NULL, IsTeamMode(), dungeonPlayer->GetDungenAdditionByType(DART_FRIEND_EXP_RATE) > 0 ? 1 : 0, 0);

		OnPlayerEnterRace(dungeonPlayer);
	}

	{
		// 发送初始化消息到relay
		CLProtocol::SceneDungeonStartRes res;
		_GetStartInfo(NULL, racePlayerInfoes, addr, res);

		if (m_sessionId > 0)
		{
			CLProtocol::RelaySvrRaceInitPacket notify;
			notify.raceId = m_sessionId;
			if (res.Encode(notify.packet))
			{
				DSRouter::Instance()->SendToRelay(m_relayServerId, notify);
			}
		}
	}

	if (GetMonster().guid > 0 && m_players.size() > 0)
	{
		auto it = m_players.begin();
		DungeonPlayer* player = it->second;
		if (player)
		{
			CLProtocol::WorldTeamDungeonRaceStart notify;
			notify.raceId = GetID();
			notify.teamId = 0;
			notify.cityMonster = GetMonster();

			player->SendToScene(notify);
		}
	}
	return ErrorCode::SUCCESS;
}

void NotifyPlayerStartDunegonRes()
{

}

void DungeonRace::OnCreateVerifyReturn(UInt32 nodeid)
{
	m_VSId = nodeid;
	_StartVerify();
}

void DungeonRace::_StartVerify()
{
	//todo later
	return;
	//// 开始验证
	//DevLogStream << "race(" << GetID() << ") start verify..." << LogStream::eos;

	//auto playerItr = m_players.begin();
	//if (playerItr == m_players.end() || !playerItr->second)
	//{
	//	ErrorLogStream << "race(" << GetID() << ") invalid player info." << LogStream::eos;
	//	// 这种情况就不用验了
	//	m_VSId = 0;
	//	return;
	//}
	//ObjID_t roleId = playerItr->second->GetID();

	//// 先把之前产生的帧发到验证服务器
	//{
	//	Protocol::VerifyReportFrame report;
	//	report.raceId = GetID();

	//	for (auto& frame : m_frames)
	//	{
	//		report.frames.push_back(frame);

	//		// 一次只发部分帧
	//		if (report.frames.size() == 10000)
	//		{
	//			_SendToVerifyServer(report);
	//			report.frames.clear();
	//		}
	//	}

	//	if (!report.frames.empty())
	//	{
	//		_SendToVerifyServer(report);
	//	}
	//}

	//// 上传校验帧
	//{
	//	Protocol::VerifyReportChecksum report;
	//	report.raceId = GetID();

	//	PlayerChecksumInfo playerChecksum;
	//	playerChecksum.roleId = roleId;
	//	for (auto& checksum : m_checksums)
	//	{
	//		ChecksumInfo info;
	//		info.frame = checksum.framesequence;
	//		info.checksum = checksum.checksum;
	//		playerChecksum.checksums.push_back(info);
	//	}
	//	report.playerChecksums.push_back(playerChecksum);

	//	_SendToVerifyServer(report);
	//}
}

void DungeonRace::_SendToVerifyServer(Avalon::Protocol& protocol)
{
	//todo later
	return;
	///*if (m_VSId == 0)
	//{
	//	return;
	//}

	//VerifyServerMgr::Instance()->SendProtocol(m_VSId, protocol);*/
}

bool DungeonRace::OnPlayerEnterRace(DungeonPlayer* dungeonPlayer)
{
	if (!dungeonPlayer)
	{
		return false;
	}

	if (dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "race(" << GetID() << ") dungeon player(" << dungeonPlayer->GetID() << ") is racing." << LogStream::eos;
		return false;
	}

	DebugLogStream << "race(" << GetID() << ") dungeon player(" << dungeonPlayer->GetID() << ") enter race." << LogStream::eos;

	dungeonPlayer->SetRacing(true);

	CLProtocol::WorldDungeonStartRaceRes res;
	res.roleId = dungeonPlayer->GetID();
	res.dungeonId = GetDungeonID();
	res.isHell =  0;
	res.isAssist =  0;
	res.isContinue = 0;
	res.result = ErrorCode::SUCCESS;
	dungeonPlayer->SendToScene(res);

	return true;
}

bool DungeonRace::OnPlayerRaceEnd(DungeonPlayer* dungeonPlayer, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16], UInt32 lastFrame, UInt32 maxDamage, UInt32 skillId, UInt32 param, UInt64 totalDamage, UInt64 bossDamage)
{
	//todo late
	//return true;
	if (!dungeonPlayer)
	{
		return false;
	}

	DebugLogStream << "player(" << dungeonPlayer->GetID() << ") dungeon race(" << GetID() << ") , sessionId : " << GetSessionId() << ", request to race end, score = " << score << LogStream::eos;

	GameInfoLogStream("Damage") << "race(" << GetID() << "), sessionId : " << GetSessionId() << ", dungeon(" << GetDungeonID() << ") player(" << dungeonPlayer->GetID() << ") level("
		<< dungeonPlayer->GetLevel() << ") occu(" << dungeonPlayer->GetOccu() << ") usedTime(" << usedTime << ") maxDamage(" << maxDamage << ") skill("
		<< skillId << ") param(" << param << ") totalDamage(" << totalDamage << ") bossDamage("<<bossDamage<<")."<< LogStream::eos;

	//if (_IsVerifying())
	//{
	//	Protocol::RelaySvrDungeonRaceEndReq raceend;
	//	raceend.raceEndInfo.gamesessionId = GetID();
	//	raceend.raceEndInfo.dungeonId = GetDungeonID();
	//	raceend.raceEndInfo.usedTime = usedTime;

	//	DungeonPlayerRaceEndInfo info;
	//	info.roleId = player->GetID();
	//	info.pos = 0;
	//	info.score = score;
	//	memset(info.md5, 0, sizeof(info.md5));
	//	info.beHitCount = beHitCount;
	//	info.bossDamage = bossDamage;

	//	raceend.raceEndInfo.infoes.push_back(info);

	//	// 上报到验证服务器
	//	Protocol::VerifyReportRaceEnd report;
	//	report.raceId = GetID();
	//	report.roleId = player->GetID();
	//	if (raceend.Encode(report.packet))
	//	{
	//		_SendToVerifyServer(report);
	//	}
	//}

	if (m_sessionId == 0 && (lastFrame == 0 || m_lastFrame != lastFrame))
	{
		DebugLogStream << "player(" << dungeonPlayer->GetID() << ") dungeon race(" << GetID() << "), sessionId : " << GetSessionId() << ", invalid frame, lastFrame(" << m_lastFrame << ") needLastFrame(" << lastFrame << ")." << LogStream::eos;
		//_OnPlayerCheat(player);
		//return false;
	}

	//DungeonPlayer* dungeonPlayer = FindDungeonPlayer(player->GetID());
	//if (!dungeonPlayer)
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") can't find player(" << player->GetID() << ")." << LogStream::eos;
	//	return false;
	//}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") , sessionId : " << GetSessionId() << ", player(" << dungeonPlayer->GetID() << ") is not racing." << LogStream::eos;
		return false;
	}

	///*if (!_CheckRaceEndRequestMd5(dungeonPlayer, score, beHitCount, usedTime, md5))
	//{
	//	_OnPlayerCheat(dungeonPlayer, PLAYER_CHEAT_REASON_RACE_END_REQUEST_MD5_ERR);
	//	return false;
	//}*/

	/*if (dungeonPlayer->IsLeave())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") is already leave." << LogStream::eos;
		return false;
	}*/

	if (dungeonPlayer->HasRaceEnd())
	{
		ErrorLogStream << "player(" << dungeonPlayer->GetID() << ") dungeon race(" << GetID() << ") , sessionId : " << GetSessionId() << ", repeat race end." << LogStream::eos;
		return false;
	}

	///*if (score > LS_C && !_CheckDungeonUsedTime(dungeonPlayer, 0, CURRENT_TIME.MSec() - m_startTime * Avalon::Time::MSECS_OF_SEC))
	//{
	//	_OnPlayerCheat(player, PLAYER_CHEAT_REASON_DUNGEON_USED_TIME_ABNOR);
	//	if (WSApplication::Instance()->GetCheatPunish() > CHEAT_PUNISH_LOG)
	//	{
	//		return false;
	//	}
	//}*/

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
			dungeonPlayer->SendToScene(consume);
		}

		// 清除玩家隐藏房间标记
		//_ClearPlayerHiddenRoomFlag();
	}

	// 死亡之塔的时间是之前计算好的
	///*if (m_dungeonData && m_dungeonData->type != LT_TOWER)
	//{
	//	dungeonPlayer->AddUsedTime(usedTime);
	//}*/

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

	//// 计算组队奖励
	//ItemReward teamReward = _GenTeamReward(dungeonPlayer, score);
	ItemReward teamReward;
	//// 月卡额外金币奖励
	//UInt32 monthCardGoldReward = 0;
	//if (m_dungeonData && m_dungeonData->subType != DST_GOLD && player->IsInMonthCard())
	//{
	//	UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_RACE_GOLD_RATE);
	//	monthCardGoldReward = rate * dungeonPlayer->GetBindGoldDropNum() / 100;
	//}

	//// 金币称号额外金币奖励
	//UInt32 goldTitleGoldReward = 0;
	//if (m_dungeonData && m_dungeonData->subType != DST_GOLD && dungeonPlayer->HasGoldTitle())
	//{
	//	UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLD_TITLE_DUNGEON_GOLD_RATE);
	//	goldTitleGoldReward = rate * dungeonPlayer->GetBindGoldDropNum() / 100;
	//}

	//// 疲劳燃烧加成
	//UInt32 fatigueBurnAdd = (100 + dungeonPlayer->GetDungenAdditionByType(DART_FATIGUE_BURNING_RATE)) / 100;
	//raceEndExp *= fatigueBurnAdd;
	//for (UInt32 addType = (UInt32)DUNGEON_EXP_ADD_BUFF; addType < (UInt32)DUNGEON_ADD_NUM; ++addType)
	//{
	//	additionInfo.addition[(DungeonAdditionType)addType] *= fatigueBurnAdd;
	//}
	//monthCardGoldReward *= fatigueBurnAdd;
	//goldTitleGoldReward *= fatigueBurnAdd;

	// 计算老兵回归奖励
	//ItemReward verteranReturnDropReward = _GenVeteranReturnDropReward(player, score);
	ItemReward verteranReturnDropReward;
	// 通知到Scene
	CLProtocol::WorldDungeonNotifyRaceEnd notify;
	notify.raceId = GetID();
	notify.cityMonster = GetMonster();
	notify.roleId = dungeonPlayer->GetID();
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
	//notify.monthcardGoldReward = monthCardGoldReward;
	//notify.goldTitleGoldReward = goldTitleGoldReward;
	notify.monthcardGoldReward = 0;
	notify.goldTitleGoldReward = 0;
	//notify.mysticalMerchantId = score > LS_C ? m_MysticalMerchantId : 0;
	notify.mysticalMerchantId = 0;
	//if (verteranReturnDropReward.id > 0)
	//{
	//	notify.raceEndRewards.push_back(verteranReturnDropReward);
	//}
	if (m_players.size() > 1)
	{
		for (auto itr : m_players)
		{
			auto other_player = itr.second;
			if (other_player && other_player->GetID() != dungeonPlayer->GetID())
			{
				notify.teammateIds.push_back(other_player->GetID());
			}
		}
	}
	dungeonPlayer->SendToScene(notify);

	dungeonPlayer->OnRaceEnd(score);

	CLProtocol::SceneDungeonRaceEndRes res;
	res.score = score;
	res.usedTime = dungeonPlayer->GetUsedTime();
	res.result = ErrorCode::SUCCESS;
	res.killMonsterTotalExp = dungeonPlayer->KillMonsterTotalExp();
	res.raceEndExp = raceEndExp;
	res.addition = additionInfo;
	res.teamReward = teamReward;
	res.hasRaceEndChest = 0;
	///*res.monthcartGoldReward = monthCardGoldReward;
	//res.goldTitleGoldReward = goldTitleGoldReward;
	//res.veteranReturnReward = verteranReturnDropReward;*/
	res.monthcartGoldReward = 0;
	res.goldTitleGoldReward = 0;
	res.veteranReturnReward = verteranReturnDropReward;

	res.fatigueBurnType = FBT_NONE;
	///*if (fatigueBurnAdd > 1)
	//{
	//	res.fatigueBurnType = FBT_COMMON;

	//	if (dungeonPlayer->GetDungeonAddition().dropAddition[DIT_CHEST] > 0)
	//	{
	//		res.fatigueBurnType = FBT_ADVANCED;
	//	}
	//}*/

	// 助战模式下没有结算掉落
	if (!dungeonPlayer->IsAssist() && score > LS_C && DropItemMgr::Instance()->HasRaceEndDrop(m_dungeonId, dungeonPlayer->GetLevel()))
	{
		res.hasRaceEndDrop = 1;
		//res.raceEndDropBaseMulti = _GetRaceEndDropBaseMulti(dungeonPlayer);
		res.raceEndDropBaseMulti = 0;
	}
	else
	{
		res.hasRaceEndDrop = 0;
		res.raceEndDropBaseMulti = 0;
	}

	// 没有结算掉落就计算结算宝箱
	//if (score > LS_C)
	//{
	//	_GenChest(player);
	//	auto task = _FindDungeonChestTask(player->GetID());
	//	if (task)
	//	{
	//		res.hasRaceEndChest = 1;
	//		res.chestDoubleFlag = (UInt8)dungeonPlayer->GetDungenAdditionByType(DART_GUILD_TERR_CHEST_DOUBLE_FLAG);

	//		// 工会领地翻牌加倍了，要通知到Scene
	//		if (dungeonPlayer->GetDungenAdditionByType(DART_GUILD_TERR_CHEST_DOUBLE_FLAG) > 0)
	//		{
	//			player->SyncEventToScene(SET_GUILD_TERR_CHEST_DOUBLE);
	//		}
	//	}

	//	// 如果是每日必掉的图，要记录下打过的次数
	//	auto normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);
	//	auto dailyDropData = DungeonDailyDropDataEntryMgr::Instance()->FindEntry(normalHardDungeonId);
	//	if (dailyDropData)
	//	{
	//		SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), normalHardDungeonId, 1);
	//	}
	//	
	//}

	DebugLogStream << "race(" << GetID() << ") send race end to player(" << dungeonPlayer->GetID() << ")." << LogStream::eos;
	dungeonPlayer->sendToClient(res);



	// 发送通关房间日志
	dungeonPlayer->SendDungeonClearAreaUdpLog(m_dungeonId, m_dungeonData->type, m_dungeonData->name, HELL_MODE_NULL, notify.endAreaId, CURRENT_TIME.Sec() - dungeonPlayer->GetEnterAreaTime());

	//Sys::SendUdpLog("dungeon", LOG_DUNGEON_FORMAT, dungeonPlayer->GetZoneID(), (UInt32)CURRENT_TIME.Sec(), GetID(), GetDungeonID(), m_dungeonData->hardType,
	//	m_hellHardMode, dungeonPlayer->GetID(), dungeonPlayer->GetAccID(), dungeonPlayer->GetLevel(), (UInt32)score, usedTime);

	// 神秘商人
	//UInt32 shopId = 0;
	//auto mysticalMerchantData = MysticalMerchantDataEntryMgr::Instance()->FindEntry(m_MysticalMerchantId);
	//if (mysticalMerchantData != NULL)
	//{
	//	shopId = mysticalMerchantData->shopId;
	//}

	{
		// 结算日志
		UInt32 hpItemIdNum = dungeonPlayer->GetBufferItemUsedNum(200001001);
		UInt8 hellAutoClose = 0;
		//if (GetTeamID())
		//{
		//	auto team = WSTeamMgr::Instance()->FindTeam(GetTeamID());
		//	if (team && team->IsSetOption(TO_HELL_AUTO_CLOSE))
		//	{
		//		hellAutoClose = 1;
		//	}
		//}

		//Sys::SendUdpLog("dungeon", LOG_DUNGEON_FORMAT, GAME_VERSION.c_str(), dungeonPlayer->GetZoneID(), (UInt32)CURRENT_TIME.Sec(), player->GetPf().c_str(), player->GetIp().c_str(), dungeonPlayer->GetID(), dungeonPlayer->GetAccID(),
		//	dungeonPlayer->GetLevel(), player->GetVipLvl(), player->GetOccu(), GetID(), GetDungeonID(), GetDungeonData()->type, GetDungeonData()->name.c_str(), m_dungeonData->hardType,
		//	m_hellHardMode, CURRENT_TIME.Sec() - m_startTime, dungeonPlayer->GetLevelScore(), dungeonPlayer->GetReviveCount(), (UInt8)dungeonPlayer->IsAssist(), shopId, IsTeamMode(), hpItemIdNum,
		//	hellAutoClose);
		Sys::SendUdpLog("dungeon", LOG_DUNGEON_FORMAT, GAME_VERSION.c_str(), dungeonPlayer->GetZoneID(), (UInt32)CURRENT_TIME.Sec(), "", "", dungeonPlayer->GetID(), dungeonPlayer->GetAccID(),
			dungeonPlayer->GetLevel(), 0, dungeonPlayer->GetOccu(), GetID(), GetDungeonID(), GetDungeonData()->type, GetDungeonData()->name.c_str(), m_dungeonData->hardType, 
			0, CURRENT_TIME.Sec() - m_startTime, dungeonPlayer->GetLevelScore(), dungeonPlayer->GetReviveCount(), (UInt8)dungeonPlayer->IsAssist(), 0, 0, hpItemIdNum,
			hellAutoClose);
	}
	

	// 发送战斗结束消息到scene
	if (m_HasSendRaceOver == false)
	{
		m_HasSendRaceOver = true;

		CLProtocol::WorldTeamDungeonRaceOver over;
		over.raceId = GetID();
		over.teamId = GetTeamID();
		over.cityMonster = GetMonster();
		//Router::BroadcastToScene(over);
		dungeonPlayer->SendToScene(over);
	}

	/*if (m_players.size() > 1 && m_bSendMasterDiscipleInteractionLog == false)
	{
		m_bSendMasterDiscipleInteractionLog = true;
		SendMasterDiscipleInteractionLog();
	}*/

//	if (m_dungeonData->subType == DST_GUILD_DUNGEON)
//	{
//		if (bossDamage == 0)
//		{
//			ErrorLogStream <<"Guild Dungeon RaceId:"<<m_id<<" DungeonId:"<<GetDungeonID()<<" BossDamage 0"<< LogStream::eos;
//		}
//
//		GuildDungeonMgr::Instance()->AddJoinPlayer(player->GetGuildID(), player->GetID());
//
//		// 记录公会地下城伤害
//#ifdef _DEBUG
//			UInt64 tempVal = player->GetGuildDungeonDebugDamage();
//
//			InfoLogStream << "GuildDungeon DEBUG Damage RaceId:"<<GetID()<<" PlayerId:"<<player->GetID()<<" playerName:"<<player->GetName() << " debugVal:"<<tempVal<<" bossDamage:"<<bossDamage << LogStream::eos;
//
//			if (tempVal == 0)
//				tempVal = bossDamage;
//
//			// 大boss，需要走验证
//			GuildDungeonVerifyMgr::Instance()->AddVerifyInfo(GetID(), player->GetID(), tempVal, player->GetGuildID(), GetDungeonID(), usedTime, player->GetName(), player->GetOccu());
//#else
//			// 大boss，需要走验证
//			GuildDungeonVerifyMgr::Instance()->AddVerifyInfo(GetID(), player->GetID(), bossDamage, player->GetGuildID(), GetDungeonID(), usedTime, player->GetName(), player->GetOccu());
//#endif
//		
//		const GuildDungeonCfg* cfg =  GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(GetDungeonID());
//		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
//		if (cfg != NULL && guild != NULL)
//		{
//			UInt64 oddBlood = GuildDungeonMgr::Instance()->GetBossOddBlood(guild->GetID(), GetDungeonID());
//
//			player->SendUdpLog("guild_dungeon", LOG_GUILD_DUNGEON, GetDungeonID(), GetDungeonData()->name.c_str(), cfg != NULL ? cfg->dungeonLvl : 0, score, totalDamage, bossDamage, usedTime, dungeonPlayer->GetReviveCount(), IsTeamMode(), guild->GetName().c_str(), guild->GetLevel(), oddBlood > bossDamage ? oddBlood - bossDamage : 0);
//		}
//
//		if (!m_flagGuildDungeon && dungeonPlayer->GetLevelScore() > LS_C)
//		{
//			m_flagGuildDungeon = true;
//			GuildDungeonMgr::Instance()->AddGuildDungeonBattleCnt(player->GetGuildID(), GetDungeonID());
// 		}
//	}

	//player->OnPlayerClearDungeon(m_dungeonData, score, usedTime);

	return true;
}

bool DungeonRace::OnRaceEnd(UInt8 reason, DungeonRaceEndInfo& raceEndInfo)
{
	//this->SetStatus(DRS_RACE_END);
	InfoLogStream << "enter OnRaceEnd race(" << GetID() << "), sessionId : " << GetSessionId() << ", reason : " << reason
		<< ", raceEndInfo : " << raceEndInfo.ToString() << LogStream::eos;

	if (raceEndInfo.infoes.size() != m_players.size())
	{
		std::ostringstream logstr;
		logstr << " DungeonRace::m_players ids:";
		for (auto iter : m_players)
		{
			if (!iter.second)
				continue;
			
			logstr << "(" <<iter.second->GetID() << "), ";
		}
		ErrorLogStream << "raceEndInfo player num is err, race(" << GetID() << "), sessionId : " << GetSessionId() 
			<<"m_players.size() : "<< (UInt32)m_players.size() << logstr.str() << LogStream::eos;
	}

	UInt8 raceResult = CDERT_FAIL;
	//结算每个玩家
	for (auto& endInfo : raceEndInfo.infoes)
	{
		if (endInfo.score > LS_C)
		{
			raceResult = CDERT_SUCC;
		}
		auto dungeonPlayer = FindDungeonPlayer(endInfo.roleId);
		if (!dungeonPlayer)
		{
			InfoLogStream << "not find player in race when end race, race(" << GetID() << "), sessionId : " << GetSessionId() << ", reason : " << reason  
				<< ", player id : " << endInfo.roleId << LogStream::eos;
			continue;
		}
		OnPlayerRaceEnd(dungeonPlayer, (LevelScore)endInfo.score, endInfo.beHitCount, raceEndInfo.usedTime, endInfo.md5, 0, 0, 0, 0, 0, endInfo.bossDamage);
	}

	InfoLogStream << "send DungeonSvrDungeonRaceEnd in OnRaceEnd, race(" << GetID() << "), sessionId : " << GetSessionId() << ", reason : " << reason << LogStream::eos;
	//结算消息发给service
	CLProtocol::DungeonSvrDungeonRaceEnd res;
	res.teamId = m_teamId;
	res.result = raceResult;
	res.endInfo = raceEndInfo;
	res.gamesessionID = m_sessionId;
	SendToService(res);
	//this->SetStatus(DRS_WAIT_CLOSE);
	this->SetIsSettled(true);

	// 发送战斗结束消息到scene
	if (m_HasSendRaceOver == false)
	{
		m_HasSendRaceOver = true;

		CLProtocol::WorldTeamDungeonRaceOver over;
		over.raceId = GetID();
		over.teamId = GetTeamID();
		over.cityMonster = GetMonster();
		DSRouter::Instance()->SendToScene(GetServiceId(),  GetSourceSceneNodeId(), over);
	}

	return true;
}

void DungeonRace::OnPlayerKilledMonster(DungeonPlayer* dungeonPlayer, std::vector<UInt32>& monsterIds)
{
	if (!dungeonPlayer)
	{
		return;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") is not racing." << LogStream::eos;
		return;
	}

	CLProtocol::SceneDungeonKillMonsterRes res;

	for (auto monsterId : monsterIds)
	{
		auto monster = _FindMonster(monsterId);
		if (!monster)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to kill unexist monster(" << monsterId << ")." << LogStream::eos;
			continue;
		}

		if (!_OnPlayerKillMonster(dungeonPlayer, monster))
		{
			continue;
		}

		res.monsterIds.push_back(monsterId);
	}

	dungeonPlayer->sendToClient(res);
}

void DungeonRace::OnPlayerKilledMonster(DungeonPlayer* dungeonPlayer, std::vector<DungeonKilledMonsterInfo>& infoes)
{
	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << dungeonPlayer->GetID() << ")." << LogStream::eos;
		return;
	}

	std::vector<UInt32> monsterIds;

	for (auto info : infoes)
	{
		auto monsterId = info.monsterId;
		auto monster = _FindMonster(monsterId);
		if (!monster)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to kill unexist monster(" << monsterId << ")." << LogStream::eos;
			continue;
		}

		if (!dungeonPlayer->HasEnteredArea(monster->areaId))
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to kill monster(" << monsterId << ") but not enter area(" << monster->areaId << ")." << LogStream::eos;
			continue;
		}

		if (!_CheckMonsterDamage(dungeonPlayer, monster, info))
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") invalid damage " << info.ToString() << LogStream::eos;
			continue;
		}

		monsterIds.push_back(monsterId);
	}

	OnPlayerKilledMonster(dungeonPlayer, monsterIds);
}


void DungeonRace::OnPlayerClearAreaMonsters(DungeonPlayer* dungeonPlayer, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16], UInt32 lastFrame)
{
	//todo later
	return;

	// 只有死亡之塔需要记录这个东西
	if (!m_dungeonData || m_dungeonData->type != LT_TOWER)
	{
		return;
	}

	if (!dungeonPlayer)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << dungeonPlayer->GetID() << ")." << LogStream::eos;
		return;
	}

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") is not racing." << LogStream::eos;
		return;
	}

	if (m_sessionId == 0 && (lastFrame == 0 || m_lastFrame != lastFrame))
	{
		DebugLogStream << "player(" << dungeonPlayer->GetID() << ") dungeon race(" << GetID() << ") invalid frame, lastFrame(" << m_lastFrame << ") needLastFrame(" << lastFrame << ")." << LogStream::eos;
		_OnPlayerCheat(dungeonPlayer, PLAYER_CHEAT_REASON_LAST_FRAME_ERR);
		return;
	}

	if (!_CheckClearAreaRequestMd5(dungeonPlayer, usedTime, remainHp, remainMp, md5))
	{
		_OnPlayerCheat(dungeonPlayer, PLAYER_CHEAT_REASON_CLEAT_AREA_REQUEST_MD5_ERR);
		return;
	}

	UInt32 curAreaId = dungeonPlayer->GetCurAreaID();
	if (dungeonPlayer->HasGotAreaReward(dungeonPlayer->GetCurAreaID()))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") dungeon player(" << dungeonPlayer->GetID() << ") has got area(" << curAreaId << ") reward." << LogStream::eos;
		return;
	}

	dungeonPlayer->SetGotAreaReward(dungeonPlayer->GetCurAreaID());
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") clear area(" << dungeonPlayer->GetCurAreaID() << ") usedTime("
		<< usedTime << ") remainHp(" << remainHp << ") remainMp(" << remainMp << ")." << LogStream::eos;

	///*auto floor = _GetTowerFloorByAreaID(m_dungeonId, curAreaId);
	//if (floor % 5 == 1)
	//{
	//	dungeonPlayer->SetUsedTime(0);
	//}

	//dungeonPlayer->AddUsedTime(usedTime);
	//dungeonPlayer->SetRemainHp(remainHp);
	//dungeonPlayer->SetRemainMp(remainMp);*/

	///*if (!_CheckDungeonUsedTime(dungeonPlayer, floor, dungeonPlayer->GetUsedTime()))
	//{
	//	_OnPlayerCheat(player, PLAYER_CHEAT_REASON_DUNGEON_USED_TIME_ABNOR);
	//	if (WSApplication::Instance()->GetCheatPunish() > CHEAT_PUNISH_LOG)
	//	{
	//		return;
	//	}
	//}*/

	CLProtocol::WorldDungeonNotifyClearAreaMonster notify;
	notify.roleId = dungeonPlayer->GetID();
	notify.dungeonId = GetDungeonID();
	notify.areaId = curAreaId;
	notify.usedTime = dungeonPlayer->GetUsedTime();
	notify.remainHp = remainHp;
	notify.remainMp = remainMp;
	dungeonPlayer->SendToScene(notify);
}

UInt32 DungeonRace::OnPlayerEnterNextArea(DungeonPlayer* dungeonPlayer, UInt32 areaId)
{
	if (!dungeonPlayer)
	{
		return ErrorCode::DUNGEON_ENTER_AREA_SYSTEM_ERROR;
	}

	ObjID_t roleId = dungeonPlayer->GetID();

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
		//dungeonPlayer->SendDungeonClearAreaUdpLog(m_dungeonId, m_dungeonData->type, m_dungeonData->name, m_hellHardMode, dungeonPlayer->GetCurAreaID(), CURRENT_TIME.Sec() - dungeonPlayer->GetEnterAreaTime());
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
			notify.roleId = dungeonPlayer->GetID();
			notify.value = rmFagiue;
			notify.dungeonDataId = m_dungeonData->id;
			dungeonPlayer->SendToScene(notify);
		}
	}

	dungeonPlayer->OnEnterArea(areaId);
	if (checkAddIntimacy && m_teamId > 0)
	{
		//checkAndAddIntimacy(curAreaId);
	}
	DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") leave area("
		<< curAreaId << ") enter next area(" << areaId << ")." << LogStream::eos;

	return ErrorCode::SUCCESS;
}

void DungeonRace::OnPlayerPickUpDropItems(DungeonPlayer* dungeonPlayer, UInt32 lastFrame, std::vector<UInt32>& dropItemIds, UInt8 md5[16])
{
	if (!dungeonPlayer)
	{
		return;
	}

	CLProtocol::SceneDungeonRewardRes res;
	ObjID_t roleId = dungeonPlayer->GetID();

	if (!dungeonPlayer->IsRacing())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") is not racing." << LogStream::eos;
		return;
	}

	if (DSApplication::Instance()->CheckPickDropFrame() && !_CheckDropRequestMd5(dungeonPlayer, lastFrame, dropItemIds, md5))
	{
		DebugLogStream << "player(" << dungeonPlayer->GetID() << ") dungeon race(" << GetID() << ") invalid md5." << LogStream::eos;
		//dungeonPlayer->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);
		//dungeonPlayer->SendCheatLog(PLAYER_CHEAT_REASON_PICK_UP_DROP_REQUEST_MD5_ERR, GetID(), GetDungeonID());
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

		if (item->roleId != roleId)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") other player(" << item->roleId
				<< ")'s item(" << itemId << ")" << LogStream::eos;
			continue;
		}

		if (item->isPickUp)
		{
			ErrorLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") repeat pick up item("
				<< itemId << ")" << LogStream::eos;
			continue;
		}


		// 添加物品
		DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") pick up item("
			<< itemId << ", " << item->typeId << "," << item->num << ")" << LogStream::eos;
		item->isPickUp = true;

		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(item->typeId);

		if (itemData != NULL && itemData->color == ItemQuality::ITEM_QUALITY_YELLOW)
		{
			//dungeonPlayer->SendDungeonDropUdpLog(GetDungeonID(), itemData->id, (UInt32)itemData->color, (UInt32)itemData->needLevel);
		}

		if (itemData && (itemData->subType == ST_ITEM_GOLD || itemData->subType == ST_BIND_GOLD))
		{
			dungeonPlayer->AddGoldDrop(item->num);

			if (itemData->subType == ST_BIND_GOLD)
			{
				dungeonPlayer->AddBindGoldDrop(item->num);
			}
		}

		res.pickedItems.push_back(itemId);

		_PushItemToReward(itemReward, item->typeId, item->num, item->strenth, item->equipType);

		if (item->monsterDailyMaxDropNum > 0)
		{
			//SetDungeonAreaIndex(dungeonPlayer->GetID(), m_dungeonId, item->monsterDungeonAreaIndex);

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
	}

	dungeonPlayer->sendToClient(res);

	if (!itemReward.empty())
	{
		CLProtocol::WorldDungeonNotifyGotItems notify;
		notify.roleId = roleId;
		notify.dungeonId = GetDungeonID();
		notify.items = itemReward;
		dungeonPlayer->SendToScene(notify);
	}
}

bool DungeonRace::OnPlayerRequestRaceEndDrop(DungeonPlayer* dungeonPlayer, UInt8 multiple)
{
	//todo later
	return true;
	//if (!dungeonPlayer)
	//{
	//	return false;
	//}

	//DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") level(" << dungeonPlayer->GetLevel() << ") request race end drop, dungeon("
	//	<< m_dungeonId << ") multi(" << multiple << ")." << LogStream::eos;

	//auto dungeonPlayer = FindDungeonPlayer(dungeonPlayer->GetID());
	//if (!dungeonPlayer)
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << dungeonPlayer->GetID() << ")." << LogStream::eos;
	//	return false;
	//}

	//if (!dungeonPlayer->IsRacing())
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") is not racing." << LogStream::eos;
	//	return false;
	//}

	//if (dungeonPlayer->GetLevelScore() <= LS_C)
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") score(" << (UInt8)dungeonPlayer->GetLevelScore() << ")." << LogStream::eos;
	//	return false;
	//}

	//if (dungeonPlayer->HasGotRaceEndDrop())
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") repeated to get race end drop." << LogStream::eos;
	//	return false;
	//}

	//if (!DropItemMgr::Instance()->HasRaceEndDrop(m_dungeonId, dungeonPlayer->GetLevel()))
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want get unexist race end drop, dungeon("
	//		<< m_dungeonId << ") level(" << dungeonPlayer->GetLevel() << ")." << LogStream::eos;
	//	return false;
	//}

	//auto result = DropItemMgr::Instance()->GenRaceEndDrop(dungeonPlayer->GetID(), m_dungeonId, dungeonPlayer->GetLevel(), dungeonPlayer->GetDungeonAddition());
	//if (result.dropItems.empty())
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") dungeon("
	//		<< m_dungeonId << ") level(" << dungeonPlayer->GetLevel() << ") race end drop item is empty." << LogStream::eos;
	//	return false;
	//}

	//for (auto& dropItem : result.dropItems)
	//{
	//	if (dropItem.isRareControl)
	//	{
	//		// 记录稀有控制
	//		//SysRecordMgr::Instance()->RecordGotRareItem(player->GetID(), dropItem.typeId, dropItem.num);
	//	}
	//}

	///*if (multiple == 3)
	//{
	//bool openTriple = false;
	//player->GetVipValue(VIP_PRIVILEGE_SOUTH_VALLEY_THREE, openTriple);
	//if (!openTriple)
	//{
	//return false;
	//}
	//}*/

	//UInt32 cost = _MultiRaceEndDropCost(multiple);
	//if (cost == UINT32_MAX)
	//{
	//	ErrorLogStream << "dungeon race(" << GetID() << ") player(" << player->GetID() << ") multi(" << multiple << ") raceEndDrop failed." << LogStream::eos;
	//	return false;
	//}

	//if (cost > 0)
	//{
	//	DungeonRaceCheckTransaction* transaction = new DungeonRaceCheckTransaction(player, GetID(), DUNGEON_RACE_MULTI_RACE_END_DROP);
	//	transaction->SetBindGold(cost);
	//	transaction->SetParam(multiple);
	//	player->BeginItemTransaction(transaction);
	//	return true;
	//}

	//OnCheckMultiRaceEndDropRet(player, multiple);

	//return true;
}

UInt32 DungeonRace::OnPlayerReviveRequest(DungeonPlayer* dungeonPlayer, ObjID_t targetId, UInt32 reviveId, UInt32 reviveCoinNum,
	UInt32  reviveItem, UInt32 reviveItemNum, UInt32 reviveHp, UInt32 reviveMp)
{
	if (!dungeonPlayer)
	{
		return ErrorCode::DUNGEON_REVIVE_PLAYER_NOT_EXIST;
	}

	DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
		<< targetId << ") reviveId(" << reviveId << ") need revive coin num(" << reviveCoinNum << ")." 
		<< " need revive item(" << reviveItem << "), num(" << reviveItemNum << ")" << LogStream::eos;

	auto target = FindDungeonPlayer(targetId);
	if (!target)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") reviveId(" << reviveId << ") target no in race." << LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_PLAYER_NOT_EXIST;
	}

	// 说明已经复活过了
	if (m_reviveTasks.find(reviveId) != m_reviveTasks.end())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") reviveId(" << reviveId << ") repeated." << LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_REPEAT;
	}

	UInt32 teamMaxReviveCnt = m_dungeonData->maxTeamCopyReviveTimes + GetAddTeamReviveCount();
	if (m_dungeonData && teamMaxReviveCnt > 0 && teamMaxReviveCnt <= this->GetTeamReviveCount())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") no remain team revive times cur(" << this->GetTeamReviveCount() << ") max(" << teamMaxReviveCnt << ")."
			<< LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_NO_TEAM_REMAIN_TIMES;
	}

	if (m_dungeonData && m_dungeonData->maxReviveTimes > 0 && m_dungeonData->maxReviveTimes <= target->GetReviveCount())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") no remain revive times cur(" << target->GetReviveCount() << ") max(" << m_dungeonData->maxReviveTimes << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_REVIVE_NO_REMAIN_TIMES;
	}

	DungeonRaceCheckTransaction* transaction = new DungeonRaceCheckTransaction(dungeonPlayer, GetID(), DUNGEON_RACE_REVIVE);
	if (reviveCoinNum > 0)
	{
		transaction->SetReviveCoin(1);
	}
	if (reviveItem > 0)
	{
		transaction->SetItem(reviveItem, reviveItemNum);
		transaction->SetReviveItem(reviveItem);
		transaction->SetReviveItemNum(reviveItemNum);
	}

	transaction->SetParam(targetId);
	transaction->SetParam2(reviveId);
	transaction->SetReviveHp(reviveHp);
	transaction->SetReviveMp(reviveMp);

	transaction->SetDsRaceId(GetID());
	dungeonPlayer->BeginItemTransaction(transaction);

	return ErrorCode::SUCCESS;
}

UInt32 DungeonRace::OnCheckReviveRet(DungeonPlayer* dungeonPlayer, UInt32 reviveId, ObjID_t targetId, UInt32 hp, UInt32 mp)
{
	CLProtocol::SceneDungeonReviveRes res;

	// 说明已经复活过了
	if (m_reviveTasks.find(reviveId) != m_reviveTasks.end())
	{
		res.result = ErrorCode::DUNGEON_REVIVE_REPEAT;
		dungeonPlayer->sendToClient(res);

		DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") check revive success, reviveID(" << reviveId
			<< ") target(" << targetId << "), target is already revived." << LogStream::eos;

		return res.result;
	}

	auto target = FindDungeonPlayer(targetId);
	if (target == NULL)
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") reviveId(" << reviveId << ") target no in race." << LogStream::eos;

		res.result = ErrorCode::DUNGEON_REVIVE_PLAYER_NOT_EXIST;
		dungeonPlayer->sendToClient(res);
		return res.result;
	}

	UInt32 teamMaxReviveCnt = m_dungeonData->maxTeamCopyReviveTimes + GetAddTeamReviveCount();
	if (m_dungeonData && teamMaxReviveCnt > 0 && teamMaxReviveCnt <= this->GetTeamReviveCount())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") no remain team revive times cur(" << this->GetTeamReviveCount() << ") max(" << teamMaxReviveCnt << ")." << LogStream::eos;
		res.result = ErrorCode::DUNGEON_REVIVE_NO_TEAM_REMAIN_TIMES;
		dungeonPlayer->sendToClient(res);
		return res.result;
	}

	if (m_dungeonData && m_dungeonData->maxReviveTimes > 0 && m_dungeonData->maxReviveTimes <= target->GetReviveCount())
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") want to revive player("
			<< targetId << ") no remain revive times cur(" << target->GetReviveCount() << ") max(" << m_dungeonData->maxReviveTimes << ")." << LogStream::eos;
		res.result = ErrorCode::DUNGEON_REVIVE_NO_REMAIN_TIMES;
		dungeonPlayer->sendToClient(res);
		return res.result;
	}

	this->IncTeamReviveCount();
	target->IncReviveCount();

	m_reviveTasks.insert(reviveId);
	res.result = ErrorCode::SUCCESS;
	dungeonPlayer->sendToClient(res);

	dungeonPlayer->SyncEventToScene(SET_REVIVE, targetId);

	dungeonPlayer->SendReviveUdpLog(GetDungeonID(), targetId, dungeonPlayer->GetCurAreaID());

	if (GetRelayServerID() > 0)
	{
		CLProtocol::SceneServerSyncPlayerEvent sync;
		sync.raceId = GetSession();
		sync.roleId = dungeonPlayer->GetID();
		sync.type = SYNC_PLAYER_EVENT_REVIVE;
		sync.param1 = targetId;
		
		sync.param2 = hp;
		sync.param2 = sync.param2 << 32;
		sync.param2 |= mp;
		DSRouter::Instance()->SendToRelay(GetRelayServerID(), sync);
	}

	DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") check revive success, reviveID(" << reviveId
		<< ") target(" << targetId << ")." << LogStream::eos;

	return ErrorCode::SUCCESS;
}

UInt32 DungeonRace::OnPlayerReportFrame(DungeonPlayer* player, const std::vector<CLProtocol::Frame>& frames, const std::vector<CLProtocol::FrameChecksum>& checksums)
{
	//todo late
	return m_lastFrame;
	//if (frames.empty())
	//{
	//	return m_lastFrame;
	//}

	//Protocol::VerifyReportFrame reportFrames;
	//reportFrames.raceId = GetID();

	//Protocol::VerifyReportChecksum reportChecksums;
	//reportChecksums.raceId = GetID();

	//UInt32 lastFrame = m_lastFrame;
	//for (auto& frame : frames)
	//{
	//	if (frame.sequence <= lastFrame)
	//	{
	//		// 这种情况说明在未收到的帧数据中有之前的帧，这种情况是不可能的，比如说 1，2，5，3，4
	//		if (lastFrame >= m_lastFrame)
	//		{
	//			ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") recv invalid frame(" << frame.sequence << ") lastFrame(" << lastFrame << ")." << LogStream::eos;
	//			_OnPlayerCheat(player, PLAYER_CHEAT_REASON_LAST_FRAME_ERR);
	//			return m_lastFrame;
	//		}

	//		continue;
	//	}

	//	lastFrame = frame.sequence;

	//	if (_IsVerifying())
	//	{
	//		reportFrames.frames.push_back(frame);
	//	}

	//	m_frames.push_back(frame);
	//}
	//m_lastFrame = lastFrame;

	//const int CHECKSUM_INTERVAL = 50;

	//lastFrame = m_checksums.empty() ? 0 : m_checksums[m_checksums.size() - 1].framesequence;
	//for (auto& checksum : checksums)
	//{
	//	if (checksum.framesequence <= lastFrame)
	//	{
	//		continue;
	//	}

	//	if (checksum.framesequence != lastFrame + CHECKSUM_INTERVAL)
	//	{
	//		ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") recv invalid checksum frame(" 
	//						<< checksum.framesequence << ") lastFrame(" << lastFrame << ")." << LogStream::eos;
	//		_OnPlayerCheat(player, PLAYER_CHEAT_REASON_REPORT_FRAME_ERR);
	//		return m_lastFrame;
	//	}

	//	lastFrame = checksum.framesequence;
	//	m_checksums.push_back(checksum);

	//	if (_IsVerifying())
	//	{
	//		PlayerChecksumInfo info;
	//		info.roleId = player->GetID();
	//		ChecksumInfo checkInfo;
	//		checkInfo.frame = checksum.framesequence;
	//		checkInfo.checksum = checksum.checksum;
	//		info.checksums.push_back(checkInfo);
	//		reportChecksums.playerChecksums.push_back(info);
	//	}
	//}

	//// 最终的随机数个数
	//int needChecksumNum = m_lastFrame / CHECKSUM_INTERVAL;
	//if (m_checksums.size() != (size_t)needChecksumNum)
	//{
	//	ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << player->GetID() << ") recv invalid checksum num("
	//		<< (UInt32)m_checksums.size() << ") lastFrame(" << m_lastFrame << ")." << LogStream::eos;
	//	_OnPlayerCheat(player, PLAYER_CHEAT_REASON_REPORT_FRAME_ERR);
	//	return m_lastFrame;
	//}

	//// 上报到验证服务器
	//if (_IsVerifying())
	//{
	//	if (!reportFrames.frames.empty())
	//	{
	//		_SendToVerifyServer(reportFrames);
	//	}

	//	if (!reportChecksums.playerChecksums.empty())
	//	{
	//		_SendToVerifyServer(reportChecksums);
	//	}
	//}

	//return m_lastFrame;
}

bool DungeonRace::_GenAreas()
{
	DungeonMonsterGenerateContext context;
	context.playerNum = (UInt32)m_players.size();
	context.dungeonLevel = m_dungeonData->level;
	
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

		//DebugLogStream << "race(" << GetID() << ") gen monster(" << monster.id << ", " << monster.typeId << ", " << monster.pointId << ")." << LogStream::eos;
	}
}

bool DungeonRace::_GenBossDropItems()
{
	//todo late
	return true;
	//if (!m_dungeonData || m_dungeonData->onlyRaceEndProfit)
	//{
	//	return false;
	//}

	//// 掉落方案
	//std::vector<UInt32> dropSetIds;

	//// 关卡掉落
	//UInt32 dungeonDropSetID = m_dungeonData->dungeonDropItem;
	//if (dungeonDropSetID)
	//{
	//	dropSetIds.push_back(dungeonDropSetID);
	//}

	//// 活动掉落
	//for (auto dropSetId : m_dungeonData->activityDropItems)
	//{
	//	if (dropSetId)
	//	{
	//		dropSetIds.push_back(dropSetId);
	//	}
	//}

	//// 为每个玩家计算掉落
	//for (auto itr : m_players)
	//{
	//	auto dungeonPlayer = itr.second;
	//	if (!dungeonPlayer || dungeonPlayer->IsAssist())
	//	{
	//		continue;
	//	}

	//	auto tmpDropSetIds = dropSetIds;
	//	if (m_dungeonData->dungeonFirstDropItem > 0 && dungeonPlayer->GetTotalCount() == 0)
	//	{
	//		tmpDropSetIds.push_back(m_dungeonData->dungeonFirstDropItem);
	//	}

	//	auto addition = dungeonPlayer->GetDungeonAddition();

	//	auto dropItems = _GenDropItemsByDropSet(dungeonPlayer, tmpDropSetIds, 0, 0);

	//	// 每日必掉
	//	auto normalHardDungeonId = _GetNormalHardDungeonID(m_dungeonId);
	//	auto dailyDropData = DungeonDailyDropDataEntryMgr::Instance()->FindEntry(normalHardDungeonId);
	//	if (dailyDropData && SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON, SYS_BEHAVIOR_OWNER_ROLE, dungeonPlayer->GetID(), normalHardDungeonId) < dailyDropData->dailyNum)
	//	{
	//		for (auto& dailyDropItem : dailyDropData->items)
	//		{
	//			UInt32 itemId = dailyDropItem.id;
	//			UInt32 itemNum = DropItemMgr::Instance()->RandFromRange(dailyDropItem.minNum, dailyDropItem.maxNum);

	//			bool hasDroped = false;
	//			for (auto& item : dropItems)
	//			{
	//				if (item.typeId == itemId)
	//				{
	//					hasDroped = true;
	//					break;
	//				}
	//			}

	//			if (!hasDroped)
	//			{
	//				PlayerDropItem dropItem;
	//				dropItem.roleId = dungeonPlayer->GetID();
	//				dropItem.id = _GenDropItemID();
	//				dropItem.typeId = itemId;
	//				dropItem.num = itemNum;
	//				dropItem.isRareControl = false;
	//				dropItem.taskId = 0;
	//				if (dropItem.equipType == EQUIP_NORMAL)
	//				{
	//					dropItem.strenth = DropStrengthenControlDataEntryMgr::Instance()->RandomItemStrenth(dropItem.typeId);
	//				}
	//				else
	//				{
	//					dropItem.strenth = 0;
	//				}

	//				dropItems.push_back(dropItem);

	//				UInt32 multiplyRate = addition.dropMultiplyRateAddition[dailyDropItem.itemType];

	//				for (UInt32 idx = 0; idx < multiplyRate; ++idx)
	//				{
	//					PlayerDropItem dropItem;
	//					dropItem.roleId = dungeonPlayer->GetID();
	//					dropItem.id = _GenDropItemID();
	//					dropItem.typeId = itemId;
	//					dropItem.num = itemNum;
	//					dropItem.isRareControl = false;
	//					dropItem.taskId = 0;
	//					if (dropItem.equipType == EQUIP_NORMAL)
	//					{
	//						dropItem.strenth = DropStrengthenControlDataEntryMgr::Instance()->RandomItemStrenth(dropItem.typeId);
	//					}
	//					else
	//					{
	//						dropItem.strenth = 0;
	//					}

	//					dropItems.push_back(dropItem);

	//					//DebugLogStream << "Player(" << context.roleId << ") multiply num, dropset(" << dropSet->id << ") gen drop item(" << item.id << ", " << num << "), drop type=" << item.itemType << LogStream::eos;
	//				}
	//			}
	//		}

	//	}


	//	dungeonPlayer->SetBossDropItems(dropItems);
	//}

	//return true;
}

void DungeonRace::_GenMonsterPrefix()
{
	// 深渊、远古、死亡之塔、异界不生成词缀怪
	if (m_dungeonData->subType == DST_YUANGU || m_dungeonData->IsHellDungeon() 
		|| m_dungeonData->subType == DST_SIWANGZHITA || m_dungeonData->subType == DST_DEVILDDOM
		|| DST_RAID_DUNGEON == m_dungeonData->subType)
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
		}

		auto tmpDropSetIds = dropSetIds;

		// 深渊保底掉落
		if (monster.isHell && !dungeonPlayer->IsGenGuaranteeDrop())
		{
		}

		// 如果是活动怪，只掉落对应活动的掉落
		if (monster.activityMonsterId)
		{
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
			}
		}
	}
}

std::vector<PlayerDropItem> DungeonRace::_GenDropItemsByDropSet(DungeonPlayer* player, std::vector<UInt32> dropSetIds, UInt32 monsterId, UInt32 monsterTypeId)
{
	std::vector<PlayerDropItem> dropItems;

	if (!player)
	{
		return dropItems;
	}

	for (auto dropSetID : dropSetIds)
	{
		ObjID_t roleId = player->GetID();
		PlayerDropItem dropItem;
		dropItem.roleId = roleId;

		//auto addition = player->GetDungeonAddition();
		GenDropItemResult genRet = DropItemMgr::Instance()->GenDropItems(GetDungeonID(), roleId, dropSetID, 0, player->GetOccu(), player->GetTaskList());
		for (auto& item : genRet.dropItems)
		{
			dropItem.id = _GenDropItemID();
			dropItem = item.GetDropItem();
			dropItem.isRareControl = item.isRareControl;
			dropItem.taskId = item.taskId;
			dropItem.notice = item.notice;
			dropItem.solutionId = item.dropSolutionId;
			dropItem.equipType = item.equipType;
			dropItem.strenth = 0;

			DebugLogStream << "dungeon race(" << GetID() << ") player(" << roleId << ") generated item("
				<< dropItem.id << "," << item.typeId << ") num(" << item.num << ") from drop set("
				<< dropSetID << ") to monster(" << monsterId << "," << monsterTypeId << ")." << LogStream::eos;

			dropItems.push_back(dropItem);
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

UInt32 DungeonRace::_GenDropItemID()
{
	return ++m_dropItemSeed;
}

ItemReward DungeonRace::_GenTeamReward(DungeonPlayer* dungeonPlayer, LevelScore score)
{
	//todo later
	ItemReward reward;
	return reward;

	//ItemReward reward;
	//if (!dungeonPlayer || score <= LS_C)
	//{
	//	return reward;
	//}

	//bool hasReward = false;
	//// 看看队友有没有是好友或者同工会的
	//for (auto itr : m_players)
	//{
	//	auto teammate = itr.second;
	//	if (!teammate || teammate->GetID() == player->GetID())
	//	{
	//		continue;
	//	}

	//	// 判断是不是好友
	//	WSPlayer* teammatePlayer = WSPlayerMgr::Instance()->FindNormalPlayer(teammate->GetID());
	//	if (WSRelationMgr::IsMutualFriend(player, teammatePlayer)
	//		|| WSRelationMgr::IsMasterDisciple(player, teammatePlayer))
	//	{
	//		hasReward = true;
	//		break;
	//	}
	//	/*if (player->GetRelationMgr().FindRelation(RELATION_FRIEND, teammate->GetID()))
	//	{
	//	hasReward = true;
	//	break;
	//	}
	//	*/
	//	// 判断是不是同工会的
	//	if (GuildMgr::Instance()->IsInSameGuild(player->GetID(), teammate->GetID()))
	//	{
	//		hasReward = true;
	//		break;
	//	}
	//}

	//if (!hasReward)
	//{
	//	return reward;
	//}

	//class TeamRewardVistor : public Avalon::DataEntryVisitor<TeamRewardDataEntry>
	//{
	//public:
	//	TeamRewardVistor(ObjID_t roleId_) : roleId(roleId_), totalProb(0) {}

	//	bool operator()(TeamRewardDataEntry* dataEntry)
	//	{
	//		if (dataEntry->dailyLimit != 0)
	//		{
	//			auto num = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_TEAM_REWARD, SYS_BEHAVIOR_OWNER_ROLE, roleId, dataEntry->id);
	//			if (dataEntry->dailyLimit <= num)
	//			{
	//				return true;
	//			}
	//		}

	//		datas.push_back(dataEntry);
	//		totalProb += dataEntry->prob;
	//		return true;
	//	}

	//	ObjID_t roleId;
	//	std::vector<TeamRewardDataEntry*> datas;
	//	UInt32 totalProb;
	//};

	//TeamRewardVistor vistor(player->GetID());
	//TeamRewardDataEntryMgr::Instance()->Visit(vistor);

	//if (vistor.totalProb == 0 || vistor.datas.empty())
	//{
	//	return reward;
	//}

	//UInt32 prob = Avalon::Random::RandBetween(0, 99);

	//// 计算究竟掉什么
	//TeamRewardDataEntry* rewardData = NULL;
	//for (auto data : vistor.datas)
	//{
	//	if (prob < data->prob)
	//	{
	//		rewardData = data;
	//		break;
	//	}
	//	prob -= data->prob;
	//}

	//if (rewardData == NULL)
	//{
	//	return reward;
	//}

	//if (rewardData->type == TEAM_AWARD_RACE_END_EXP)
	//{
	//	UInt32 expNum = _GetDungeonRaceEndExp(GetDungeonID(), dungeonPlayer->GetLevel()) * rewardData->rewardId / 100;
	//	if (expNum > 0)
	//	{
	//		reward.id = ITEM_EXP;
	//		reward.num = expNum;
	//	}
	//}
	//else if (rewardData->type == TEAM_AWARD_ITEM)
	//{
	//	reward.id = rewardData->rewardId;
	//	reward.num = rewardData->num;
	//}

	//if (rewardData->dailyLimit > 0)
	//{
	//	SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_TEAM_REWARD, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), rewardData->id, rewardData->num);
	//}

	//DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") team reward(" << rewardData->type << ", " << rewardData->rewardId << ", " << rewardData->num << ")." << LogStream::eos;

	//return reward;
}

void DungeonRace::_GetStartInfo(DungeonPlayer* dungeonPlayer, std::vector<RacePlayerInfo>& racePlayerInfoes, const Avalon::SockAddr& addr, CLProtocol::SceneDungeonStartRes& res)
{
	ObjID_t roleId = dungeonPlayer == NULL ? 0 : dungeonPlayer->GetID();
	//res.openAutoBattle = WSServiceSwitchMgr::Instance()->GetAutoBattleMode();
	res.openAutoBattle = true;
	res.dungeonId = GetDungeonID();
	res.startAreaId = GetStartAreaID();
	res.result = 0;
	GetAreaInfo(roleId, res.areas);
	GetHellInfo(roleId, res.hellInfo);
	res.session = m_sessionId == 0 ? GetID() : m_sessionId;
	res.addr = addr;
	res.players = racePlayerInfoes;
	res.isRecordLog = DSApplication::Instance()->IsRecordDungeonLog() ? 1 : 0;
	res.isRecordReplay = DSApplication::Instance()->IsRecordDungeonReplay() ? 1 : 0;
	//res.battleFlag = WSApplication::Instance()->GetBattleFlag();
	//// 打上隐藏房间标记
	//if (m_TriggerHiddenRoom)
	//{
	//	res.battleFlag |= Eff_Devilddom_hidden_room;
	//}

	// 如果有队伍，判断深渊是否自动关闭
	res.hellAutoClose = 0;
	///*if (GetTeamID() > 0)
	//{
	//	auto team = WSTeamMgr::Instance()->FindTeam(GetTeamID());
	//	if (team && team->IsSetOption(TO_HELL_AUTO_CLOSE))
	//	{
	//		res.hellAutoClose = 1;
	//	}
	//}*/

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

	res.clearedDungeonIds = this->GetClearedDungeonIds();
}

bool DungeonRace::_OnPlayerKillMonster(DungeonPlayer* dungeonPlayer, DungeonMonster* monster)
{
	if (!dungeonPlayer || !monster)
	{
		return false;
	}

	if (dungeonPlayer->HasKilledMonster(monster->id))
	{
		ErrorLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") repeat to kill monster(" << monster->id << ")." << LogStream::eos;
		return false;
	}

	UInt32 exp = monster->GetExp(dungeonPlayer);

	// 疲劳燃烧翻倍
	exp *= (100 + dungeonPlayer->GetDungenAdditionByType(DART_FATIGUE_BURNING_RATE)) / 100;

	CLProtocol::WorldDungeonNotifyKillMonster notify;
	notify.roleId = dungeonPlayer->GetID();
	notify.dungeonId = GetDungeonID();
	notify.monsterId = monster->GetMonsterBaseTypeID();
	notify.exp = exp;
	dungeonPlayer->SendToScene(notify);

	dungeonPlayer->OnKillMonster(monster->id, exp);
	//player->OnOwnedNpcDied(monster->GetMonsterBaseTypeID(), monster->typeId, GetDungeonID());

	if (monster->dailyMaxDropNum)
	{
		auto index = _GetIndexByAreaID(monster->areaId);
		SetDungeonAreaIndex(dungeonPlayer->GetID(), m_dungeonId, index);
	}

	DebugLogStream << "dungeon race(" << GetID() << ") player(" << dungeonPlayer->GetID() << ") kill monster ("
		<< monster->id << "," << monster->typeId << ") get exp(" << exp << ")." << LogStream::eos;

	return true;
}

PlayerDropItem*  DungeonRace::_FindDropItem(UInt32 itemId, DungeonPlayer* player, bool mustEnteredArea)
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
	return player->FindBossDropItem(itemId);
}


UInt8 DungeonRace::_RandomItemStrenth(UInt32 itemId)
{
	//todo later
	return 0;

	///*ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	//if (!itemEntry || itemEntry->type != ITEM_EQUIP)
	//{
	//	return 0;
	//}

	//UInt32 tmpKey = DropStrengthenControlDataEntry::GenKey(itemEntry->needLevel, itemEntry->color, itemEntry->color2);
	//auto controlDataEntry = DropStrengthenControlDataEntryMgr::Instance()->FindEntry(tmpKey);
	//if (!controlDataEntry || controlDataEntry->strengthen.size() != controlDataEntry->weight.size())
	//{
	//	return 0;
	//}

	//UInt32 totalWeight = 0;
	//for (size_t i = 0; i < controlDataEntry->weight.size(); ++i)
	//{
	//	totalWeight += controlDataEntry->weight[i];
	//}

	//UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
	//for (size_t i = 0; i < controlDataEntry->weight.size(); ++i)
	//{
	//	if (controlDataEntry->weight[i] >= randWeight)
	//	{
	//		return controlDataEntry->strengthen[i];
	//	}
	//	else
	//	{
	//		randWeight -= controlDataEntry->weight[i];
	//	}
	//}

	//return 0;*/
}

void DungeonRace::_PushItemToReward(ItemRewardVec& reward, UInt32 itemId, UInt16 num, UInt8 strenth, UInt8 equipType)
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
	//todo later
	return 0;
	//// 助战模式下不能获得结算经验
	//if (!m_dungeonData || !player || player->IsAssist())
	//{
	//	return 0;
	//}

	//// 只有结算收益的图，如果失败了就没有结算经验
	//if (m_dungeonData->onlyRaceEndProfit && score <= LS_C)
	//{
	//	return 0;
	//}

	//int expAdditionByScore[LS_NUM] = { -30, -25, -20, -15, -10, 0 };
	//int endExpAddition = score >= LS_NUM ? 0 : expAdditionByScore[score];
	//int expAddition = 0;

	//// 当前难度结算经验
	//UInt32 baseExp = _GetDungeonRaceEndExp(m_dungeonData->id, player->GetLevel());
	//// 普通难度基础经验
	//UInt32 normalBaseExp = baseExp;

	//auto normalHardDungeon = DungeonDataEntryMgr::Instance()->FindEntry(_GetNormalHardDungeonID(m_dungeonId));
	//if (normalHardDungeon)
	//{
	//	normalBaseExp = _GetDungeonRaceEndExp(normalHardDungeon->id, player->GetLevel());;
	//}

	///*
	//新加功能，失败情况下 经验 = 基础经验 * 通过的房间数 / 最小通关房间数
	//add by zjy
	//2016.11.11
	//*/
	//if (score == LS_C)
	//{
	//	auto enteredAreaCount = player->GetEnteredAreaCount();
	//	if (enteredAreaCount <= 1)
	//	{
	//		baseExp = 0;
	//		normalBaseExp = 0;
	//	}

	//	auto clearAreaCount = enteredAreaCount - 1;
	//	if (clearAreaCount < (int)m_dungeonData->clearMinAreaNum && m_dungeonData->clearMinAreaNum)
	//	{
	//		baseExp = (UInt32)(baseExp * (float)clearAreaCount / m_dungeonData->clearMinAreaNum);
	//		normalBaseExp = (UInt32)(normalBaseExp * (float)clearAreaCount / m_dungeonData->clearMinAreaNum);
	//	}

	//	UInt32 maxExp = 1500 * clearAreaCount;
	//	baseExp = baseExp > maxExp ? maxExp : baseExp;
	//	normalBaseExp = normalBaseExp > maxExp ? maxExp : normalBaseExp;
	//}

	//auto guildSkillExpAddition = 0;
	//auto buffExpAddition = 0;
	//auto vipExpAddition = 0;
	//auto masterExpAddition = 0;
	//auto opActivityExpAddition = 0;
	//auto friendExpAddition = 0;
	//auto adventureTeamExpAddition = 0;
	//if (player)
	//{
	//	//vipExpAddition = player->GetVipExpAddition();
	//	//guildSkillExpAddition = player->GetGuildSkillAddition();
	//	//buffExpAddition = player->GetBuffExpAddition();
	//	//masterExpAddition = player->GetMasterExpAddition();
	//	vipExpAddition = player->GetDungenAdditionByType(DART_VIP_EXP_RATE);
	//	guildSkillExpAddition = player->GetDungenAdditionByType(DART_GUID_SKILL_EXP_RATE);
	//	buffExpAddition = player->GetDungenAdditionByType(DART_BUFF_EXP_RATE);
	//	masterExpAddition = player->GetDungenAdditionByType(DART_MASTER_EXP_RATE);
	//	opActivityExpAddition = player->GetDungenAdditionByType(DART_OP_ACTVITY_EXP_RATE);
	//	friendExpAddition = player->GetDungenAdditionByType(DART_FRIEND_EXP_RATE);
	//	adventureTeamExpAddition = player->GetDungenAdditionByType(DART_ADVENTURE_TEAM_EXP_RATE);
	//	expAddition += guildSkillExpAddition + buffExpAddition + vipExpAddition + masterExpAddition + opActivityExpAddition + friendExpAddition + adventureTeamExpAddition;
	//}

	//// 难度加成
	//addition.addition[DUNGEON_EXP_ADD_HARD] = baseExp * (100 + endExpAddition) / 100 - normalBaseExp * (100 + endExpAddition) / 100;
	//// 评分加成
	//addition.addition[DUNGEON_EXP_ADD_SCORE] = normalBaseExp * (100 + endExpAddition) / 100 - normalBaseExp * (100 + expAdditionByScore[0]) / 100;
	//// 公会技能
	//addition.addition[DUNGEON_EXP_ADD_GUILD] = baseExp * (100 + endExpAddition) / 100 * (guildSkillExpAddition) / 100;
	//// buff
	//addition.addition[DUNGEON_EXP_ADD_BUFF] = baseExp * (100 + endExpAddition) / 100 * (buffExpAddition) / 100;
	//// vip加成
	//addition.addition[DUNGEON_EXP_ADD_VIP] = baseExp * (100 + endExpAddition) / 100 * (vipExpAddition) / 100;
	//// 师徒
	//addition.addition[DUNGEON_EXP_ADD_MASTER] = baseExp * (100 + endExpAddition) / 100 * (masterExpAddition) / 100;
	////好友
	//addition.addition[DUNGEON_EXP_ADD_FRIEND] = baseExp * (100 + endExpAddition) / 100 * (friendExpAddition) / 100;
	//// 冒险队
	//addition.addition[DUNGEON_EXP_ADD_ADVENTURE_TEAM] = baseExp * (100 + endExpAddition) / 100 * (adventureTeamExpAddition) / 100;

	//UInt32 finalExp = baseExp * (100 + endExpAddition) / 100 * (100 + expAddition) / 100;
	//return finalExp;
}

UInt32 DungeonRace::_GetDungeonRaceEndExp(UInt32 dungeonId, UInt16 level)
{
	//todo later
	return 0;
	//auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	//if (!dungeonData)
	//{
	//	return 0;
	//}

	//if (dungeonData->isExpLevelAdapt)
	//{
	//	return LevelAdapterDataEntryMgr::Instance()->GetValue(dungeonData->expReward, level);
	//}
	//return dungeonData->expReward;
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

void DungeonRace::_BroadcastProtocol(Avalon::Protocol& protocol)
{
	for (auto itr : m_players)
	{
		auto dungeonPlayer = itr.second;
		if (!dungeonPlayer)
		{
			continue;
		}

		dungeonPlayer->sendToClient(protocol);
	}
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

void  DungeonRace::_GiveOnPickDropItem(DungeonPlayer* dungeonPlayer)
{
	/*if (!player)
	{
	return;
	}

	auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	if (!dungeonPlayer)
	{
	ErrorLogStream << "dungeon race(" << GetID() << ") can't find dungeon player(" << player->GetID() << ")." << LogStream::eos;
	return;
	}

	std::vector<UInt32> pickupItemIds;
	for (auto area : m_areas)
	{
	for (auto monster : area->GetAllMonster())
	{
	if (!dungeonPlayer->HasKilledMonster(monster.id))
	{
	continue;
	}

	for (const auto& item : monster.dropItems)
	{
	if (item.roleId != player->GetID() || item.isPickUp)
	{
	continue;
	}

	InfoLogStream << "dungeon race(" << GetID() << ") dropitem(" << item.id << ", " << item.typeId << ") unpick..." << LogStream::eos;
	pickupItemIds.push_back(item.id);
	}
	}
	}

	if (dungeonPlayer->HasRaceEnd())
	{
	for (auto bossDrop : dungeonPlayer->GetBossDropItems())
	{
	if (!bossDrop.isPickUp)
	{
	pickupItemIds.push_back(bossDrop.id);

	InfoLogStream << "dungeon race(" << GetID() << ") boss drop(" << bossDrop.id << ", " << bossDrop.typeId << ") unpick..." << LogStream::eos;
	}
	}
	}


	if (!pickupItemIds.empty())
	{
	OnPlayerPickUpDropItems(player, pickupItemIds);
	}
	*/
}

bool DungeonRace::_CheckMonsterDamage(DungeonPlayer* dungeonPlayer, DungeonMonster* monster, const DungeonKilledMonsterInfo& info)
{
	if (!dungeonPlayer || !monster)
	{
		return false;
	}

	// 多人或者是没有掉落的怪物不验证合法性
	if (m_players.size() != 1 || monster->dropItems.empty())
	{
		return true;
	}

	// 验证怪物受到伤害的合法性
	// ToDo...

	return true;
}

bool DungeonRace::_CheckDungeonUsedTime(DungeonPlayer* dungeonPlayer, UInt32 floor, UInt32 usedTime)
{
	//todo later
	return 0;
	//if (!DSApplication::Instance()->IsCheckMD5())
	//{
	//	return true;
	//}

	//// 多人的情况下不验证
	//if (!dungeonPlayer || m_players.size() > 1)
	//{
	//	return true;
	//}

	//auto data = DungeonFinishTimeDataEntryMgr::Instance()->FindEntry(DungeonFinishTimeDataEntry::MakeKey(m_dungeonId, floor));
	//if (!data)
	//{
	//	return true;
	//}

	//UInt16 level = dungeonPlayer->GetLevel();
	//UInt32 minTime = 0;
	//if (level <= 29)
	//{
	//	minTime = data->minTime_29;
	//}
	//else if (level <= 39)
	//{
	//	minTime = data->minTime_39;
	//}
	//else if (level <= 49)
	//{
	//	minTime = data->minTime_49;
	//}
	//else if (level <= 59)
	//{
	//	minTime = data->minTime_59;
	//}
	//else
	//{
	//	minTime = data->minTime_69;
	//}

	//// 如果最小时间是0，说明不用验证
	//if (minTime == 0)
	//{
	//	return true;
	//}

	//if (usedTime < minTime * Avalon::Time::MSECS_OF_SEC)
	//{
	//	ErrorLogStream << "race(" << GetID() << ") dungeon(" << m_dungeonId << ") player(" << dungeonPlayer->GetID() << ") level(" << dungeonPlayer->GetLevel() << ") occu(" << dungeonPlayer->GetOccu() << ") floor(" << floor << ") used time(" << usedTime << ")ms min(" << minTime * Avalon::Time::MSECS_OF_SEC << ")." << LogStream::eos;
	//	return false;
	//}

	//return true;
}

void DungeonRace::_OnPlayerCheat(DungeonPlayer* dungeonPlayer, PlayerCheatReason cheatReason)
{
	//todo later
	return;
	//if (!dungeonPlayer)
	//{
	//	return;
	//}

	//player->SendCheatLog(cheatReason, GetID(), GetDungeonID());

	//auto dungeonPlayer = FindDungeonPlayer(player->GetID());
	//if (!dungeonPlayer || dungeonPlayer->IsCheat())
	//{
	//	return;
	//}

	//dungeonPlayer->SetCheat();
	//InfoLogStream << "race(" << GetID() << ") player(" << player->GetAccId() << ", " << player->GetID() << ") cheat..." << LogStream::eos;

	//// 自动封号
	//// ...
	//
	//if (WSApplication::Instance()->GetCheatPunish() <= CHEAT_PUNISH_LOG)
	//{
	//	return;
	//}

	//player->KickOff(ErrorCode::SYS_CHEAT_FIRST_FORBIT_LOGIN);

	//if (WSApplication::Instance()->GetCheatPunish() <= CHEAT_PUNISH_KICK)
	//{
	//	return;
	//}

	//auto times = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_DUNGEON_CHEAT, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), 0);
	//SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_TOTAL_DUNGEON_CHEAT, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), 0, 1);
	//if (times == 0)
	//{
	//	UInt32 punishSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHEAT_FIRST_TIME_PUNISH_SEC);
	//	PunishmentMgr::Instance()->Punish(player->GetAccId(), PUNISH_FORBIDLOGIN, punishSec, FORBID_LOGIN_CHEAT_FIRST_TIME);
	//}
	//else
	//{
	//	UInt32 punishSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHEAT_FOREVER_PUNISH_SEC);
	//	PunishmentMgr::Instance()->Punish(player->GetAccId(), PUNISH_FORBIDLOGIN, punishSec, FORBID_LOGIN_CHEAT_FOREVER);
	//}
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

bool DungeonRace::SendToService(Avalon::Protocol& protocol)
{
	return DSRouter::Instance()->SendToService(m_serviceId, protocol);
}

bool DungeonRace::SendToRelay(Avalon::Protocol& protocol)
{
	return DSRouter::Instance()->SendToRelay(m_relayServerId, protocol);
}

void DungeonRace::OnDestory()
{
	if (!IsSettled())
	{
		InfoLogStream << "send DungeonSvrDungeonRaceEnd in OnDestory, sessionId : " << GetSessionId() << LogStream::eos;
		DungeonRaceEndInfo raceEndInfo;
		raceEndInfo.gamesessionId = GetSessionId();
		raceEndInfo.dungeonId = GetDungeonID();
		//结算消息发给service
		CLProtocol::DungeonSvrDungeonRaceEnd res;
		res.teamId = m_teamId;
		res.result = CDERT_FAIL;
		res.endInfo = raceEndInfo;
		SendToService(res);
	}
}

bool DungeonRace::GetFighter(ObjID_t roleId, CLProtocol::DungeonFighter& fighter)
{
	for (auto f : m_fighters)
	{
		if (f.roleId == roleId)
		{
			fighter = f;
			return true;
		}
	}
	return false;
}