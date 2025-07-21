#include "GuildDungeonMgr.h"
#include "GuildDungeon.h"
#include "GuildMgr.h"
#include "WSSelectInitDataCallback.h"
#include "AnnouncementMgr.h"
#include "CLSysNotify.h"
#include "FigureStatueMgr.h"
#include "WSServiceSwitchMgr.h"
#include "GuildDungeonVerifyMgr.h"
#include "GuildAuctionMgr.h"
#include "PlayerJoins.h"

#include <CLGuildDefine.h>
#include <CLGuildProtocol.h>
#include <CLDungeonProtocol.h>
#include <CLGuildDungeonTypeDataEntry.h>
#include <CLGuildDungeonTimeDataEntry.h>
#include <CLGuildDungeonRewardDataEntry.h>
#include <CLGuildDungeonLotteryDataEntry.h>
#include <CLGuildDungeonLvlDataEntry.h>
#include <CLGuildDungeonDamageCoeffDataEntry.h>
#include <CLRecordClient.h>
#include <CLSyncObjectProtocol.h>

// 通关排行榜数量
#define CLEAR_GATE_RANK		(5)
// 职业伤害系数基数
#define OCCU_DAMAGE_COEFF_BASE		(10000)
// 数据分几步加载
#define GUILD_DUNGEON_LOAD_NUM (7)

UInt32 GetGuildDungeonLimitLvl()
{
	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT);
}

UInt32 GetGuildDungeonPlayerLimitLvl()
{
	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_PLAYER_LEVEL_LIMIT);
}

/**
*@brief 加载公会地下城回调
*/
class SelectGuildDungeonCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonStatus(this);
	}
};

/**
*@brief 加载公会地下城回调
*/
class SelectGuildDungeonInfoCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonData(this);
	}
};

/**
*@brief 加载公会地下城战斗数据回调
*/
class SelectGuildDungeonBattleCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonBattle(this);
	}
};

/**
*@brief 加载公会地下城玩家伤害记录回调
*/
class SelectGuildDungeonDamageCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonDamage(this);
	}
};

/**
*@brief 加载公会地下城玩家领奖回调
*/
class SelectGuildDungeonRewardCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonReward(this);
	}
};

/**
*@brief 加载公会地下城参与玩家回调
*/
class SelectGuildDungeonJoinCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonJoin(this);
	}
};

/**
*@brief 加载公会地下城随机回调
*/
class SelectGuildDungeonRandCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildDungeonMgr::Instance()->LoadGuildDungeonRand(this);
	}
};

//****************************************************


bool DamageLowSort(GuildDungeonGateTime& a, GuildDungeonGateTime& b)
{
	return (a.spendTime < b.spendTime);
}

GuildDungeonMgr::GuildDungeonMgr()
{
	m_DungeonStatus = GUILD_DUNGEON_END;
	m_StatusStartStamp = 0;
	m_LoadFlag = 0;
	m_IsRecvCheck = false;
	m_Week = CURRENT_DATE.WDay();

	m_RandDungenNumMap[GUILD_DUNGEON_MID] = 3;
	m_RandDungenNumMap[GUILD_DUNGEON_HIGH] = 1;
}

GuildDungeonMgr::~GuildDungeonMgr()
{

}

void GuildDungeonMgr::Init()
{
	// 加载公会地下城信息
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectGuildDungeonCallback());
}

void GuildDungeonMgr::OnTick(const Avalon::Time &now)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (m_LoadFlag < GUILD_DUNGEON_LOAD_NUM) return;

	UInt32 victory_chat_interval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_VICTORY_CHAT_INTERVAL);
	UInt32 victory_chat_cnt = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_VICTORY_CHAT_CNT);

	// 发送胜利公告
	for (auto& iter : m_GuildDungeonMap)
	{
		if (NULL == iter.second)
			continue;

		UInt32 chat_cnt = iter.second->GetVictoryPubCnt();
		UInt64 chat_stamp = iter.second->GetVictoryPubTime();
		if (chat_cnt != 0 && chat_cnt < victory_chat_cnt && now.Sec() > chat_stamp + victory_chat_interval)
		{
			iter.second->AddVictoryPubCnt();
			iter.second->SetVictoryPubTime();
			iter.second->GetGuildPtr()->Chat(SysNotifyMgr::Instance()->MakeString(9942));
		}
	}

	if ((Int32)m_Week != CURRENT_DATE.WDay())
	{
		InfoLogStream << "OnTick Change Week:"<<m_Week << " To:" << CURRENT_DATE.WDay() << LogStream::eos;
		m_Week = CURRENT_DATE.WDay();
		InitTickMap();
	}

	ExecuteTick(now);
}

void GuildDungeonMgr::Final()
{
	ClearGuildDungeon();
}

bool GuildDungeonMgr::IsGuildDungeon()
{
	if (!SwitchIsOpen())
	{
		return false;
	}

	return m_DungeonStatus != GUILD_DUNGEON_END;
}

bool GuildDungeonMgr::IsStartDungeon()
{
	if (!SwitchIsOpen())
	{
		return false;
	}

	return m_DungeonStatus == GUILD_DUNGEON_START;
}

bool GuildDungeonMgr::IsJoinOrLeaveGuild()
{
	if (!SwitchIsOpen())
	{
		return true;
	}

	if (m_DungeonStatus == GUILD_DUNGEON_PREPARE || m_DungeonStatus == GUILD_DUNGEON_END)
	{
		return true;
	}

	return false;
}

bool GuildDungeonMgr::SwitchIsOpen()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_DUNGEON))
	{
		return false;
	}

	return true;
}

void GuildDungeonMgr::NotifyGuildDungeonActivity(WSPlayer* player)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (NULL == player) return;

	if (m_DungeonStatus != GUILD_DUNGEON_END && player->GetLevel() >= GetGuildDungeonPlayerLimitLvl())
	{
		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (NULL == guild || guild->GetLevel() < GetGuildDungeonLimitLvl())
		{
			return;
		}

		CLProtocol::WorldGuildDungeonStatusSync dungeonStatusSync;
		dungeonStatusSync.dungeonStatus = m_DungeonStatus;

		player->SendProtocol(dungeonStatusSync);
	}
}

void GuildDungeonMgr::LoadData()
{
	// 加载公会地下城信息
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon_info");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectGuildDungeonInfoCallback());
}

bool GuildDungeonMgr::LoadGuildDungeonStatus(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		m_DungeonStatus = callback->GetData("dungeon_status");
		m_StatusStartStamp = callback->GetData("start_time");
	}

	m_startStamp = GuildDungeonTimeDataEntryMgr::Instance()->GetLastStatusTimeStamp(m_StatusStartStamp, GUILD_DUNGEON_START);

	m_LoadFlag++;

	InfoLogStream <<"LoadGuildDungeonStatus DungeonStatus:"<<m_DungeonStatus<<" Last Stamp:"<<m_StatusStartStamp<< LogStream::eos;

	Avalon::Time StatusTime(m_StatusStartStamp * 1000);
	UInt64 statusDay = GetDayBeginTime(StatusTime);
	UInt64 currentDay = GetDayBeginTime(CURRENT_TIME);
	bool dayChanged = (m_StatusStartStamp != 0 && (statusDay != currentDay));

	// 跨天结算
	if (dayChanged && m_DungeonStatus != GUILD_DUNGEON_END)
	{
		InfoLogStream << "LoadGuildDungeonStatus dayChanged:" << m_DungeonStatus << " Last Stamp:" << m_StatusStartStamp << LogStream::eos;

		if (m_DungeonStatus == GUILD_DUNGEON_START)
			DungeonReward();

		m_DungeonStatus = GUILD_DUNGEON_END;
		m_StatusStartStamp = CURRENT_TIME.Sec();

		std::ostringstream cond;
		cond << "1=1";
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon", cond.str());
		cmd->PutData("dungeon_status", m_DungeonStatus);
		cmd->PutData("start_time", m_StatusStartStamp);
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	InitTickMap();

	return true;
}

bool GuildDungeonMgr::LoadGuildDungeonData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt64 guildId = callback->GetData("guild_id");
		Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
		if (NULL == guild)
		{
			ErrorLogStream << "LoadGuildDungeonData Not Find Guild Error!!! guildId:" << guildId << LogStream::eos;
			continue;
		}

		UInt32 dungeonType = callback->GetData("dungeon_type");
		UInt64 totalDamage = callback->GetData("total_damage");
		UInt64 spendTime = callback->GetData("spend_time");

		GuildDungeon* guildDungeon = new GuildDungeon();
		guildDungeon->SetGuildPtr(guild);
		guildDungeon->AddTotalDamage(totalDamage);
		guildDungeon->SetDungeonType(dungeonType);

		m_GuildDungeonMap[guildId] = guildDungeon;

		if (spendTime != 0)
		{
			GuildDungeonGateTime gateTime;
			gateTime.guildId = guildId;
			gateTime.spendTime = spendTime;
			m_GateTimeVec.push_back(gateTime);
		}
	}

	std::sort(m_GateTimeVec.begin(), m_GateTimeVec.end(), DamageLowSort);

	m_LoadFlag ++;

	// 加载公会地下城战斗信息
	CLSelectCommand* cmd_battle = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon_battle");
	CLRecordClient::Instance()->SendCommand(cmd_battle, new SelectGuildDungeonBattleCallback());

	// 加载公会地下城玩家伤害信息
	CLSelectCommand* cmd_damage = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon_player_damage");
	CLRecordClient::Instance()->SendCommand(cmd_damage, new SelectGuildDungeonDamageCallback());

	// 加载公会地下城领奖信息
	CLSelectCommand* cmd_reward = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon_reward");
	CLRecordClient::Instance()->SendCommand(cmd_reward, new SelectGuildDungeonRewardCallback());

	// 加载公会地下城参与的玩家
	CLSelectCommand* cmd_join = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon_player_join");
	CLRecordClient::Instance()->SendCommand(cmd_join, new SelectGuildDungeonJoinCallback());

	// 加载公会地下城随机
	CLSelectCommand* cmd_rand = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_dungeon_rand");
	CLRecordClient::Instance()->SendCommand(cmd_rand, new SelectGuildDungeonRandCallback());

	return true;
}

bool GuildDungeonMgr::LoadGuildDungeonBattle(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt64 guildId = callback->GetData("guild_id");
		GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
		if (NULL == guildDungeon)
		{
			ErrorLogStream <<"LoadGuildDungeonBattle Find GuildDungeon Failed! guildId:"<< guildId << LogStream::eos;
			continue;
		}

		UInt32 dungeonId = callback->GetData("dungeon_id");
		UInt32 battleCnt = callback->GetData("battle_cnt");
		UInt64 bossOddBlood = callback->GetData("boss_odd_blood");
		std::string dropBuff = callback->GetData("drop_buff").GetString();

		guildDungeon->AddBattleRecord(dungeonId, battleCnt, bossOddBlood, dropBuff);
	}

	m_LoadFlag++;

	return true;
}

bool GuildDungeonMgr::LoadGuildDungeonDamage(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt64 guildId = callback->GetData("guild_id");
		GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
		if (NULL == guildDungeon)
		{
			ErrorLogStream << "LoadGuildDungeonDamage Find GuildDungeon Failed! guildId:" << guildId << LogStream::eos;
			continue;
		}

		UInt64 playerId = callback->GetData("player_id");
		UInt64 damagePoint = callback->GetData("damage_val");
		std::string playerName = callback->GetData("player_name").GetString();

		guildDungeon->AddPlayerDamage(playerId, damagePoint, playerName);
	}

	m_LoadFlag++;

	return true;
}

bool GuildDungeonMgr::LoadGuildDungeonReward(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt64 playerId = callback->GetData("player_id");
		AddObtainRewardPlayer(playerId);
	}

	m_LoadFlag++;

	return true;
}

bool GuildDungeonMgr::LoadGuildDungeonJoin(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt64 guildId = callback->GetData("guild_id");
		GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
		if (NULL == guildDungeon)
		{
			ErrorLogStream << "LoadGuildDungeonJoin Find GuildDungeon Failed! guildId:" << guildId << LogStream::eos;
			continue;
		}

		UInt64 playerId = callback->GetData("player_id");

		guildDungeon->AddJoinPlayer(playerId);
	}

	m_LoadFlag++;

	return true;
}

bool GuildDungeonMgr::LoadGuildDungeonRand(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt32 dungeonType = callback->GetData("dungeon_type");
		UInt32 dungeonId = callback->GetData("dungeon_id");

		AddRandDungeon(dungeonType, dungeonId);
	}

	m_LoadFlag++;

	return true;
}

void GuildDungeonMgr::GuildDismiss(UInt64 guildId)
{
	auto iter = m_GuildDungeonMap.find(guildId);
	if (iter == m_GuildDungeonMap.end())
		return;

	GuildDungeon* guildDungeon = iter->second;
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "Not Find Guild Dungeon! GuildId:"<< guildId << LogStream::eos;
		m_GuildDungeonMap.erase(iter);
		return;
	}

	guildDungeon->Final();
	CL_SAFE_DELETE(guildDungeon);
	m_GuildDungeonMap.erase(iter);

	std::ostringstream cond;
	cond << "guild_id=" << guildId;
	CLDeleteCommand* cmd_1 = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_dungeon_battle", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd_1);

	CLDeleteCommand* cmd_2 = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_dungeon_info", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd_2);

	CLDeleteCommand* cmd_3 = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_dungeon_player_damage", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd_3);

	CLDeleteCommand* cmd_4 = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_dungeon_player_join", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd_4);
}

void GuildDungeonMgr::HandleGuildDungeonInfoReq(WSPlayer* player)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (!IsGuildDungeon()) return;
	if (player == NULL) return;
	if (player->GetGuildID() == 0) return;

	CLProtocol::WorldGuildDungeonInfoRes res;

	GuildDungeon* guildDungeon = FindGuildDungeon(player->GetGuildID());
	if (NULL == guildDungeon)
	{
		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (NULL == guild)
		{
			ErrorLogStream << "HandleGuildDungeonInfoReq Not Find Guild Error!!! guildId:" << player->GetGuildID() << LogStream::eos;
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			player->SendProtocol(res);
			return ;
		}

		// 公会副本开启需要等级
		if (guild->GetLevel() < GetGuildDungeonLimitLvl())
		{
			ErrorLogStream << "HandleGuildDungeonInfoReq Guild Level Limit!!! guildId:" << player->GetGuildID() << " Lvl:" << guild->GetLevel() << LogStream::eos;
			res.result = ErrorCode::GUILD_DUNGEON_GUILD_LEVEL_LIMIT;
			player->SendProtocol(res);
			return ;
		}

		guildDungeon = CreateGuildDungeon(guild);
	}
	
	if (NULL == guildDungeon)
	{
		ErrorLogStream <<" HandleGuildDungeonInfoReq Failed! GuildId:"<<player->GetGuildID()<< LogStream::eos;
		res.result = ErrorCode::GUILD_DUNGEON_ERROR;
		player->SendProtocol(res);
		return;
	}

	res.dungeonStatus = m_DungeonStatus;

	if (m_TickMap.empty())
	{
		res.statusEndStamp = 0;
	}
	else
	{
		res.statusEndStamp = UInt32(m_TickMap.begin()->first);
	}

	res.isReward = PlayerIsObtainReward(player->GetID()) ? 1 : 0;
	
	const GuildDungeonBattleMap& battleMap = guildDungeon->GetBattleRecordMap();
	for (auto& tt : battleMap)
	{
		const GuildDungeonCfg* cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(tt.second->dungeonId);
		if (NULL == cfg)
		{
			ErrorLogStream << "GuildDungeonLvlDataEntryMgr Find Cfg Failed!!! id:" << tt.second->dungeonId << LogStream::eos;
			continue;
		}

		if (cfg->dungeonLvl == GUILD_DUNGEON_MID || cfg->dungeonLvl == GUILD_DUNGEON_HIGH)
		{
			CLProtocol::GuildDungeonBossBlood bossBlood;
			bossBlood.dungeonId = tt.second->dungeonId;
			bossBlood.oddBlood = tt.second->oddBlood;
			bossBlood.verifyBlood = GuildDungeonVerifyMgr::Instance()->
				GetVerifyDamage(guildDungeon->GetGuildPtr()->GetID(), tt.second->dungeonId);

			res.bossBlood.push_back(bossBlood);
		}
	}

	// 通关用时排行
	std::vector<GuildDungeonGateTime>::iterator iter = m_GateTimeVec.begin();
	for (;iter != m_GateTimeVec.end();++iter)
	{
		if (res.clearGateVec.size() >= CLEAR_GATE_RANK)
			break;

		if (iter->spendTime == 0)
			continue;

		Guild* guild = GuildMgr::Instance()->FindGuildByID(iter->guildId);
		if (NULL == guild)
			continue;

		CLProtocol::GuildDungeonClearGateTime gateTime;
		gateTime.guildName = guild->GetName();
		gateTime.spendTime = iter->spendTime;
		res.clearGateVec.push_back(gateTime);
	}

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void GuildDungeonMgr::HandleGuildDungeonDamageRankReq(WSPlayer* player)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (!IsGuildDungeon()) return;
	if (player == NULL) return;
	if (player->GetGuildID() == 0) return;

	CLProtocol::WorldGuildDungeonDamageRankRes res;

	GuildDungeon* guildDungeon = FindGuildDungeon(player->GetGuildID());
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "HandleGuildDungeonDamageRankReq Failed !!! guildId:" << player->GetGuildID() << LogStream::eos;
		player->SendProtocol(res);
		return;
	}
	
	const GuildDungeonDamageVec& damageVec = guildDungeon->GetDamageVec();
	
	UInt32 k = 1;
	for (auto& it : damageVec)
	{
		if (NULL == it)
			continue;

		CLProtocol::GuildDungeonDamage damage;
		damage.rank = k;
		damage.damageVal = it->damagePoint;
		damage.playerId = it->playerId;
		damage.playerName = it->playerName;

		res.damageVec.push_back(damage);

		k++;
	}

	player->SendProtocol(res);
}

void GuildDungeonMgr::HandleGuildDungeonCopyReq(WSPlayer* player)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (!IsGuildDungeon()) return;
	if (player == NULL) return;
	if (player->GetGuildID() == 0) return;

	CLProtocol::WorldGuildDungeonCopyRes res;
	GuildDungeon* guildDungeon = FindGuildDungeon(player->GetGuildID());
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "HandleGuildDungeonCopyReq Failed !!! guildId:" << player->GetGuildID() << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	const GuildDungeonBattleMap& battle_record = guildDungeon->GetBattleRecordMap();
	for (auto& iter : battle_record)
	{
		res.BattleRecordVec.push_back(*iter.second);
	}

	player->SendProtocol(res);
}

void GuildDungeonMgr::HandleGuildDungeonLotteryReq(WSPlayer* player)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (!IsGuildDungeon()) return;
	if (player == NULL) return;
	if (player->GetGuildID() == 0) return;

	CLProtocol::WorldGuildDungeonLotteryRes res;

	GuildDungeon* guildDungeon = FindGuildDungeon(player->GetGuildID());
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "HandleGuildDungeonLotteryReq Failed !!! guildId:" << player->GetGuildID() << LogStream::eos;
		res.result = ErrorCode::GUILD_DUNGEON_ERROR;
		player->SendProtocol(res);
		return;
	}

	// 检查boss是否已经打死
	if ( !guildDungeon->CheckAllBossIsDead())
	{
		ErrorLogStream << "HandleGuildDungeonLotteryReq Boss No Dead !!! guildId:" << player->GetGuildID() << LogStream::eos;
		res.result = ErrorCode::GUILD_DUNGEON_NOT_FINISH;
		player->SendProtocol(res);
		return;
	}

	// 玩家是否已经领取了奖励
	if (PlayerIsObtainReward(player->GetID()))
	{
		ErrorLogStream << "HandleGuildDungeonLotteryReq Player Repeated Reward !!! guildId:" << 
			player->GetGuildID() << " player:" << player->GetID() <<LogStream::eos;
		res.result = ErrorCode::GUILD_DUNGEON_REWARD_GET_REPEATED;
		player->SendProtocol(res);
		return;
	}

	// 根据玩家打出的伤害抽奖
	UInt32 damageRate = GetPlayerDamageRate(player->GetGuildID(), player->GetID());
	RewardDetail* rewardDetail = GuildDungeonRewardDataEntryMgr::Instance()->GetRewardDetail(E_GUILD_DUNGEON_REWARD_VICTORY, guildDungeon->GetDungeonType(), damageRate);
	if (NULL == rewardDetail)
	{
		ErrorLogStream << "HandleGuildDungeonLotteryReq Reward Error !!! guildId:" << 
			player->GetGuildID()<< " player:"<<player->GetID() << LogStream::eos;
		res.result = ErrorCode::GUILD_DUNGEON_REWARD_ERROR;
		player->SendProtocol(res);
		return;
	}

	// 抽奖
	ItemRewardVec itemRewardVec;
	for (UInt32 groupSize = 0; groupSize < rewardDetail->rewardInfo.size(); ++groupSize)
	{
		RewardInfo* rewardInfo = rewardDetail->rewardInfo[groupSize];
		if (NULL == rewardInfo)
			continue;

		for (UInt32 k = 0; k < rewardInfo->rewardCnt; ++k)
		{
			auto rewardItem = GuildDungeonLotteryDataEntryMgr::Instance()->GetGuildDungeonReward(rewardInfo->rewardGroup);
			if (rewardItem)
			{
				ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(rewardItem->itemId);
				if (NULL == itemEntry)
				{
					ErrorLogStream << "HandleGuildDungeonLotteryReq not find itemEntry Failed! itemId:" << rewardItem->itemId << LogStream::eos;
					continue;
				}

				GuildDungeonReward item;
				item.itemId = rewardItem->itemId;
				item.itemNum = rewardItem->itemNum;
				item.isHighVal = rewardItem->isHighVal;

				res.lotteryItemVec.push_back(item);

				ItemReward reward;
				reward.id = rewardItem->itemId;
				reward.num = rewardItem->itemNum;
				itemRewardVec.push_back(reward);

				// 抽奖公告
				guildDungeon->GetGuildPtr()->Chat(SysNotifyMgr::Instance()->MakeString(9944, player->GetName(), itemEntry->name));

				// 高级道具抽奖公告
				if (item.isHighVal != 0)
				{
					AnnouncementMgr::Instance()->PublicAnnouncement(0, 90, (UInt32)CURRENT_TIME.Sec(), guildDungeon->GetGuildPtr()->GetName(), player->GetName(), itemEntry->name);
				}
			}
		}
	}

	// 给玩家加道具
	if (!itemRewardVec.empty())
	{
		UInt32 id = GuildDungeonLvlDataEntryMgr::Instance()->GetFinalDungeonId(guildDungeon->GetDungeonType());
		player->AddItem(GetReason(SOURCE_TYPE_GUILD_DUNGEON_VICTORY, player->GetGuildID(), id), itemRewardVec, false);
	}

	// 增加领奖玩家
	AddObtainRewardPlayer(player->GetID(), true);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void GuildDungeonMgr::HandleGuildDungeonStatueReq(WSPlayer* player)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	CLProtocol::WorldGuildDungeonStatueRes res;
	FigureStatueMgr::Instance()->GetGuildDungeonFigureStatue(player->GetGuildID(), res.figureStatue);
	player->SendProtocol(res);
}

void GuildDungeonMgr::SyncGuildDungeonStatus()
{
	if (!SwitchIsOpen())
	{
		return;
	}

	CLProtocol::WorldGuildDungeonStatusSync dungeonStatusSync;
	dungeonStatusSync.dungeonStatus = m_DungeonStatus;
	GuildMgr::Instance()->Broadcast(dungeonStatusSync, GetGuildDungeonLimitLvl(), GetGuildDungeonPlayerLimitLvl());
}

GuildDungeon* GuildDungeonMgr::FindGuildDungeon(UInt64 guildId)
{
	auto iter = m_GuildDungeonMap.find(guildId);
	return iter != m_GuildDungeonMap.end() ? iter->second : NULL;
}

GuildDungeon* GuildDungeonMgr::CreateGuildDungeon(Guild* guildPtr)
{
	if (NULL == guildPtr) return NULL;

	GuildDungeon* guildDungeon = new GuildDungeon();
	if (!guildDungeon->OnInit(guildPtr))
	{
		ErrorLogStream << "CreateGuildDungeon Init Failed !!! guildId:" << guildPtr->GetID() << LogStream::eos;
		return NULL;
	}

	m_GuildDungeonMap[guildPtr->GetID()] = guildDungeon;

	InfoLogStream << "CreateGuildDungeon guildId:" << guildPtr->GetID() << LogStream::eos;

	return guildDungeon;
}

void GuildDungeonMgr::SwitchDungeonStatus(UInt32 status, UInt64 startStamp)
{
	if (!IsVaildStatus(status)) 
		return;

	InfoLogStream << "Guild Dungeon Switch Status_1:" << m_DungeonStatus << " To Status_2:" << status << " Start Time:" << startStamp << LogStream::eos;

	// 更新当前状态
	m_DungeonStatus = (GuildDungeonStatus)status;
	m_StatusStartStamp = startStamp;

	switch (status)
	{
	case GUILD_DUNGEON_PREPARE: DungeonPrepare(); break;
	case GUILD_DUNGEON_START: DungeonStart(); break;
	case GUILD_DUNGEON_REWARD: DungeonReward();	break;
	case GUILD_DUNGEON_END: DungeonEnd(); break;
	default:return;
	}

	if (status == GUILD_DUNGEON_PREPARE)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("dungeon_status", m_DungeonStatus);
		cmd->PutData("start_time", m_StatusStartStamp);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		std::ostringstream cond;
		cond << "1=1";
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon", cond.str());
		cmd->PutData("dungeon_status", status);
		cmd->PutData("start_time", m_StatusStartStamp);
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	// 地下城状态改变，通知客户端
	SyncGuildDungeonStatus();
}

void GuildDungeonMgr::DungeonPrepare()
{
	// 清除上次活动的数据
	ClearGuildDungeon();
	m_ObtainRewardPlayer.clear();
	m_GateTimeVec.clear();
	m_RandDungeonMap.clear();

	ClearDBTable("t_guild_dungeon");
	ClearDBTable("t_guild_dungeon_info");
	ClearDBTable("t_guild_dungeon_battle");
	ClearDBTable("t_guild_dungeon_player_damage");
	ClearDBTable("t_guild_dungeon_player_join");
	ClearDBTable("t_guild_dungeon_reward");
	ClearDBTable("t_guild_dungeon_rand");

	// 清除雕像的所有数据
	FigureStatueMgr::Instance()->ClearGuildDungeonFigureStatue();

	// 随机难度
	RandDungeon(GUILD_DUNGEON_MID);
	RandDungeon(GUILD_DUNGEON_HIGH);
}

void GuildDungeonMgr::DungeonStart()
{
	m_startStamp = CURRENT_TIME.Sec(); 

	// 活动开启走马灯
	AnnouncementMgr::Instance()->PublicAnnouncement(0, 89, (UInt32)CURRENT_TIME.Sec());

	// 公会频道通知
	GuildMgr::Instance()->GuildChat(SysNotifyMgr::Instance()->MakeString(9941), GetGuildDungeonLimitLvl());
}

void GuildDungeonMgr::DungeonReward()
{
	for (auto& iter : m_GuildDungeonMap)
	{
		GuildDungeon* dungeon = iter.second;
		if (NULL == dungeon)
		{
			ErrorLogStream << "DungeonReward not find guild Failed !!! guildId:" << iter.first << LogStream::eos;
			continue;
		}

		// 已经胜利了，并且发过公告
		if (dungeon->GetVictoryPubCnt() != 0)
			continue;

		// 所有boss都死了，肯定已经发过参与奖励
		if (dungeon->CheckAllBossIsDead())
			continue;

		// 每个公会伤害值的前三名，树立雕像
		GuildDungeonStatue(iter.first, dungeon->GetDamageVec());

		dungeon->GetGuildPtr()->Chat(SysNotifyMgr::Instance()->MakeString(9943));

		// 发送参与的邮件奖励
		SendAttendRewardMail(iter.first, dungeon);
	}
}

void GuildDungeonMgr::DungeonEnd()
{
	
}

void GuildDungeonMgr::InitTickMap()
{
	m_TickMap.clear();

	TimeStatusMap* time_map = GuildDungeonTimeDataEntryMgr::Instance()->GetCurTimeStatusMap(m_Week);
	if (NULL == time_map || time_map->empty())
	{
		ErrorLogStream <<"InitTickMap Not Find Week Config:"<<m_Week<< LogStream::eos;
		return;
	}

	UInt64 day_start = GetDayBeginTime(CURRENT_TIME) / 1000;
	UInt64 end_time = day_start + (--time_map->end())->first;
	if (CURRENT_TIME.Sec() > end_time)
		return;

	TimeStatusMap::iterator iter = time_map->begin();
	for (; iter != time_map->end(); ++iter)
	{
		UInt64 temp_time = iter->first + day_start;
		if (temp_time <= m_StatusStartStamp)
			continue;

		m_TickMap[temp_time] = iter->second;
	}
}

void GuildDungeonMgr::ExecuteTick(const Avalon::Time &now)
{
	if (m_TickMap.empty()) return;

	UInt32 status = GUILD_DUNGEON_MAX;
	auto iter = m_TickMap.begin();
	if (now.Sec() >= iter->first)
	{
		m_StatusStartStamp = iter->first;
		status = iter->second;
		m_TickMap.erase(iter);
	}
	
	SwitchDungeonStatus(status, m_StatusStartStamp);
}

bool GuildDungeonMgr::IsVaildStatus(UInt32 status)
{
	switch (status)
	{
		case GUILD_DUNGEON_END:
		case GUILD_DUNGEON_PREPARE:
		case GUILD_DUNGEON_START:
		case GUILD_DUNGEON_REWARD:
			return true;

		default:break;
	}

	return false;
}

void GuildDungeonMgr::AddObtainRewardPlayer(UInt64 playerId, bool isSave)
{
	m_ObtainRewardPlayer.insert(playerId);

	if (isSave)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_reward", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("player_id", playerId);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

bool GuildDungeonMgr::PlayerIsObtainReward(UInt64 playerId)
{
	auto iter = m_ObtainRewardPlayer.find(playerId);
	return iter != m_ObtainRewardPlayer.end();
}

UInt32 GuildDungeonMgr::GetPlayerDamageRate(UInt64 guildId, UInt64 playerId)
{
	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "GetPlayerDamageRate Not Find Guild Failed !!! guildId:" << guildId << " playerId:" << playerId << LogStream::eos;
		return 0;
	}

	// 伤害万分比
	if (guildDungeon->GetTotalDamage() == 0)
		return 0;

	UInt64 playerDamage = 0;
	for (auto& it : guildDungeon->GetDamageVec())
	{
		if (it->playerId != playerId)
			continue;

		playerDamage = it->damagePoint;
		break;
	}

	UInt32 val = (UInt32)ceil(playerDamage * 10000 / guildDungeon->GetTotalDamage());

	return val <= 10000 ? val : 10000;
}

void GuildDungeonMgr::RecordDungeonDamage(UInt64 guildId, UInt32 dungeonId, UInt64 playerId, UInt32 occu, UInt64 damageVal, 
	UInt64 dungeonRaceId, const std::string& playerName, UInt32 fightTime)
{
	if (!IsStartDungeon())
	{
		return;
	}

	if (damageVal == 0) return;

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "RecordDungeonDamage Not Find Guild Failed !!! guildId:" << guildId << " dungeonId:" << dungeonId << LogStream::eos;
		return;
	}

	UInt64 preTime = GuildDungeonTimeDataEntryMgr::Instance()->GetLastStatusTimeStamp(CURRENT_TIME.Sec(), GUILD_DUNGEON_PREPARE);
	if (preTime != 0)
	{
		PlayerJoinMgr::Instance()->AddOnePlayer(PJET_THR_TYPE_GUILD_DUNGEON, preTime, guildId, playerId);
	}
	else
	{
		ErrorLogStream << "error preTime equal zero !" << LogStream::eos;
	}

	// 如果boss已经死了，后面的伤害不计算
	if (guildDungeon->GetBossOddBlood(dungeonId) == 0)
	{
		ErrorLogStream << "RecordDungeonDamage Boss Yet Dead!!! guildId:" << guildId << " dungeonId:" << dungeonId
			<< " playerId:" << playerId << " playerName:" << playerName << LogStream::eos;
		return;
	}

	guildDungeon->CostBossBlood(dungeonId, damageVal);

	const GuildDungeonCfg* dungeon_cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(dungeonId);
	if (NULL == dungeon_cfg)
	{
		ErrorLogStream << "GuildDungeonLvlDataEntryMgr Find Config Failed!!! id:" << dungeonId << LogStream::eos;
		return;
	}

	InfoLogStream << "RecordDungeonDamage guildId:" << guildId << " dungeonId:" << dungeonId <<
		" playerId:" << playerId << " damage:" << damageVal << " playerName:" << playerName << 
		" Occu:" << occu << " fightTime:" << fightTime << LogStream::eos;

	// 计算伤害积分
	UInt32 coeff = GuildDungeonDamageCoeffDataEntryMgr::Instance()->GetDamageCoeff(occu);
	if (coeff == 0)
	{
		ErrorLogStream << "GuildDungeonDamageCoeffDataEntryMgr Find Config Failed!!! occu:" << occu << LogStream::eos;
		coeff = OCCU_DAMAGE_COEFF_BASE;
	}

	// 公会地下城增加玩家伤害	
	UInt64 damagePoint = UInt64(damageVal * coeff) / OCCU_DAMAGE_COEFF_BASE;
	guildDungeon->AddPlayerDamage(playerId, damagePoint, playerName, true);
	guildDungeon->AddTotalDamage(damagePoint);
	guildDungeon->UpdateTotalDamageDB();

	if (dungeon_cfg->dungeonLvl == GUILD_DUNGEON_HIGH)
	{
		// 通知boss剩余血量
		guildDungeon->NotifyRaceBossOddBlood(dungeonId, dungeonRaceId);

		// 大boss已经死了
		if (guildDungeon->GetBossOddBlood(dungeonId) == 0)
		{
			BroadcastBossDead(guildDungeon, dungeonId);
			UInt32  dungeonType = guildDungeon->GetDungeonType();

			std::string dungeonTypeName = GuildDungeonTypeDataEntryMgr::Instance()->GetGuildDungeonTypeName(dungeonType);

			// 发送公会频道消息
			guildDungeon->AddVictoryPubCnt();
			guildDungeon->SetVictoryPubTime();
			guildDungeon->GetGuildPtr()->AddGuildEvent(SysNotifyMgr::Instance()->MakeString(9942, dungeonTypeName));

			// 生成雕像
			GuildDungeonStatue(guildId, guildDungeon->GetDamageVec());

			// 记录通关用时
			AddClearGateRank(guildId);

			// 发送参与的邮件奖励
			SendAttendRewardMail(guildId, guildDungeon);

			// 通知里面还在打的人退出
			guildDungeon->EndDungeonRace(dungeonId, dungeonRaceId);

			// 获得拍卖物品
			if (WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
			{
				GuildAuctionReward(guildId, guildDungeon->GetDungeonType(), m_GateTimeVec.size());
			}

			Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);

			if (guild != NULL)
			{
				std::vector<UInt32> conds(GuildHonorConditionType::GUILD_COND_MAX , 0);
				conds[GuildHonorConditionType::GUILD_COND_MEMBER] = 1;
				guild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_GUILD_DUNGEON, conds);
			}
			InfoLogStream << "RecordDungeonDamage Guild Dungeon Victory GuildId:" << guildId << " GuildType:" <<
				guildDungeon->GetDungeonType() << " boss totalDamage:" << guildDungeon->GetTotalDamage() << LogStream::eos;
		}
	}
	else if (dungeon_cfg->dungeonLvl == GUILD_DUNGEON_MID)
	{
		// 通知boss剩余血量
		guildDungeon->NotifyRaceBossOddBlood(dungeonId, dungeonRaceId);

		// 小boss已经死了，广播给公会所有成员消息，所有参与的玩家获得一个宝箱
		if (guildDungeon->GetBossOddBlood(dungeonId) == 0)
		{
			BroadcastBossDead(guildDungeon, dungeonId);

			// 通知里面还在打的人退出
			guildDungeon->EndDungeonRace(dungeonId, dungeonRaceId);

			// 抽奖
			ItemRewardVec rewardVec;
			GetRewardByType(dungeon_cfg->rewardType, guildDungeon->GetDungeonType(), rewardVec);

			if (rewardVec.empty())
			{
				ErrorLogStream << " rewardVec Empty GuildId:" << guildId << " dungeonId:" << dungeon_cfg->dungeonId << " playerId:" << playerId << " playerName:" << playerName << LogStream::eos;
				return;
			}

			std::string sender = SysNotifyMgr::Instance()->MakeString(9966);
			std::string title = SysNotifyMgr::Instance()->MakeString(9967);
			std::string content = SysNotifyMgr::Instance()->MakeString(9968, dungeon_cfg->dungeonName);
			std::string reason = GetReason(SOURCE_TYPE_GUILD_DUNGEON_VICTORY, guildDungeon->GetGuildPtr()->GetID(), dungeon_cfg->dungeonId);

			// 给参与的人发个宝箱
			const std::set<UInt64>& joinSet = guildDungeon->GetJoinSet();
			for (UInt64 playerId : joinSet)
			{
				if (MailBox::SendSysMail(playerId, sender, title, content, reason, rewardVec) != ErrorCode::SUCCESS)
				{
					// 发送邮件失败
					ErrorLogStream << "Guild Dungeon Send Battle Failed Mail Reward Error !!! PlayerId:" << playerId <<LogStream::eos;
					continue;
				}

				InfoLogStream << "Small Boss GuildId:" << guildId << " dungeonId:" << dungeon_cfg->dungeonId << " playerId:" << playerId << LogStream::eos;
			}
		}
	}
}

void GuildDungeonMgr::AddGuildDungeonBattleCnt(UInt64 guildId, UInt32 dungeonId)
{
	if (!IsStartDungeon())
	{
		return;
	}

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "AddGuildDungeonBattleCnt Not Find Guild Failed !!! guildId:" << guildId << " dungeonId:" << dungeonId << LogStream::eos;
		return;
	}

	const GuildDungeonCfg* dungeon_cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(dungeonId);
	if (NULL == dungeon_cfg)
	{
		ErrorLogStream << "Find Dungeon Cfg Failed! DungeonId:" << dungeonId << LogStream::eos;
		return;
	}

	guildDungeon->AddBattleCnt(dungeonId);

	if (dungeon_cfg->dropBuff != 0)
	{
		guildDungeon->AddDropbuff(dungeonId, dungeon_cfg->dropBuff);
		guildDungeon->UpdateDropBuffDB(dungeonId);
	}
}

void GuildDungeonMgr::AddJoinPlayer(UInt64 guildId, UInt64 playerId)
{
	if (!IsStartDungeon())
	{
		return;
	}

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "AddJoinPlayer Not Find Guild Failed !!! guildId:" << guildId << LogStream::eos;
		return;
	}

	guildDungeon->AddJoinPlayer(playerId, true);
}

void GuildDungeonMgr::GuildLevelUp(Guild* guildPtr)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (NULL == guildPtr) return;
	
	// 如果能找到该公会，在玩家上线的时候有通知
	//auto iter = m_GuildDungeonMap.find(guildPtr->GetID());
	//if (iter != m_GuildDungeonMap.end())
	//	return;

	if (guildPtr->GetLevel() >= GetGuildDungeonLimitLvl() && m_DungeonStatus != GUILD_DUNGEON_END)
	{
		CLProtocol::WorldGuildDungeonStatusSync dungeonStatusSync;
		dungeonStatusSync.dungeonStatus = m_DungeonStatus;

		guildPtr->Broadcast(dungeonStatusSync, GetGuildDungeonPlayerLimitLvl());
	}
}

UInt64 GuildDungeonMgr::GetBossOddBlood(UInt64 guildId, UInt32 dungeonId)
{
	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
		return 0;

	GuildDungeonBattleRecord* battle = guildDungeon->GetGuildDungeonBattleRecord(dungeonId);
	if (NULL == battle)
		return 0;

	return battle->oddBlood;
}

void GuildDungeonMgr::GuildAuctionBonus(UInt64 guildId, UInt32 rewardNum)
{
	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "Not Find Guild Dungeon Id:" << guildId << " rewardNum:" << rewardNum << LogStream::eos;
		return;
	}

	const std::set<UInt64>& joinSet = guildDungeon->GetJoinSet();
	UInt32 joinPlayerNum = joinSet.size();
	if (joinPlayerNum == 0)
	{
		ErrorLogStream << "Guild Auction Bouns Player 0 GuildId:" << guildId << " rewardNum:" << rewardNum << LogStream::eos;
		return;
	}

	InfoLogStream << "GuildAuctionBonus GuildId:" << guildId << " rewardNum:"
		<< rewardNum << " JoinPlayerNum:" << joinPlayerNum << LogStream::eos;

	UInt32 passGateRank = GetPassGateTimeRank(guildId);
	RewardDetail* rewardDetail = GuildDungeonRewardDataEntryMgr::Instance()->GetRewardDetail(
		E_GUILD_DUNGEON_REWARD_AUCTION, guildDungeon->GetDungeonType(), passGateRank);
	if (NULL == rewardDetail)
	{
		ErrorLogStream << "Guild Dungeon Reward Not Find !!! guildId:" << guildId << " DungeonType:" 
			<< guildDungeon->GetDungeonType() << " rank:" << passGateRank << LogStream::eos;
		return;
	}

	ItemRewardVec rewardVec;
	std::string title, content;
	std::string sender = SysNotifyMgr::Instance()->MakeString(10009);
	std::string reason = GetReason(SOURCE_TYPE_GUILD_AUCTION_BONUS, guildId);
	UInt32 bonusCfg = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_AUCTION_BONUS_NUM);
	if (rewardNum >= bonusCfg)
	{
		UInt32 bonusNum = rewardNum / joinPlayerNum;

		// 个人分成限制
		if (rewardDetail->ownerBonusLimit != 0 && bonusNum > rewardDetail->ownerBonusLimit)
		{
			bonusNum = rewardDetail->ownerBonusLimit;
			InfoLogStream << "Guild Auction BonusNum:" << bonusNum << " Owner Limit:" << rewardDetail->ownerBonusLimit << LogStream::eos;
		}

		ItemReward reward(GUILD_AUCTION_COIN, bonusNum, 0, 0, 0);
		rewardVec.push_back(reward);

		title = SysNotifyMgr::Instance()->MakeString(10014);
		content = SysNotifyMgr::Instance()->MakeString(10015);
	}
	else
	{
		title = SysNotifyMgr::Instance()->MakeString(10028);
		content = SysNotifyMgr::Instance()->MakeString(10029, bonusCfg);
	}

	for (UInt64 playerId : joinSet)
	{
		if (MailBox::SendSysMail(playerId, sender, title, content, reason, rewardVec) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "Guild Dungeon Send Auction Bonus Mail Reward Error !!! PlayerId:" << playerId << LogStream::eos;
			continue;
		}

		InfoLogStream << "GuildAuctionBonus GuildId:" << guildId << " playerId:"
			<< playerId << LogStream::eos;
	}
}

void GuildDungeonMgr::GetRandDungeon(UInt32 dungeonType, std::vector<UInt32>& randVec)
{
	auto iter = m_RandDungeonMap.find(dungeonType);
	if (iter == m_RandDungeonMap.end())
		return;

	randVec = iter->second;
}

void GuildDungeonMgr::SendAttendRewardMail(UInt64 guildId, GuildDungeon* dungeon)
{
	if (NULL == dungeon) 
		return;

	// 抽奖
	ItemRewardVec rewardVec;
	GetRewardByType(E_GUILD_DUNGEON_REWARD_ATTEND, dungeon->GetDungeonType(), rewardVec);

	if (rewardVec.empty())
	{
		InfoLogStream <<"SendBattleFaildRewardMail guildId:"<<guildId<<" No Reward!!!"<< LogStream::eos;
		return;
	}

	std::string sender = SysNotifyMgr::Instance()->MakeString(9938);
	std::string title = SysNotifyMgr::Instance()->MakeString(9939);
	std::string content = SysNotifyMgr::Instance()->MakeString(9940);
	std::string reason = GetReason(SOURCE_TYPE_GUILD_DUNGEON_ATTEND, dungeon->GetGuildPtr()->GetID(), dungeon->GetDungeonType());

	const std::set<UInt64>& joinSet = dungeon->GetJoinSet();
	for (UInt64 playerId : joinSet)
	{
		if (MailBox::SendSysMail(playerId, sender, title, content, reason, rewardVec) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "Guild Dungeon Send Battle Attand Mail Reward Error !!! PlayerId:" <<playerId <<LogStream::eos;
			continue;
		}

		InfoLogStream << "SendAttendRewardMail GuildId:" << guildId << " playerId:"
			<< playerId << LogStream::eos;
	}
}

void GuildDungeonMgr::GuildDungeonStatue(UInt64 guildId, const GuildDungeonDamageVec& damageVec)
{
	UInt32 rank = FST_GUILD_DUNGEON_FIRST;
	for (auto& it : damageVec)
	{
		if (rank > FST_GUILD_DUNGEON_THIRD)
			break;

		FigureStatueMgr::Instance()->CreateGuildDungeonFigureStatue(guildId, it->playerId, rank);
		rank++;
	}

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "GuildDungeonStatue Not Find Guild Failed !!! guildId:" << guildId << LogStream::eos;
		return;
	}

	Guild* guild = guildDungeon->GetGuildPtr();
	if (NULL == guild)
	{
		ErrorLogStream << "GuildDungeonStatue Guild Ptr NULL !!! guildId:" << guildId << LogStream::eos;
		return;
	}

	CLProtocol::WorldGuildDungeonStatueRes protocol;
	FigureStatueMgr::Instance()->GetGuildDungeonFigureStatue(guildId, protocol.figureStatue);
	guild->Broadcast(protocol);
}

void GuildDungeonMgr::AddClearGateRank(UInt64 guildId)
{
	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "Not Find Guild Error!!! guildId:" << guildId << LogStream::eos;
		return;
	}

	if (CURRENT_TIME.Sec() >= m_startStamp)
	{
		GuildDungeonGateTime gateTime;
		gateTime.guildId = guildId;
		gateTime.spendTime = CURRENT_TIME.Sec() - m_startStamp;
		m_GateTimeVec.push_back(gateTime);
		std::sort(m_GateTimeVec.begin(), m_GateTimeVec.end(), DamageLowSort);

		// 存库
		std::ostringstream cond;
		cond << "guild_id=" << guildId;
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon_info", cond.str());
		cmd->PutData("spend_time", gateTime.spendTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		ErrorLogStream << "Clear Gate Spend Time Error CurrTime:" << CURRENT_TIME.Sec() << " StartTime:" << m_startStamp << LogStream::eos;
	}
}

void GuildDungeonMgr::BroadcastBossDead(GuildDungeon* guildDungeon, UInt32 dungeonId)
{
	if (NULL == guildDungeon)
		return;

	CLProtocol::WorldGuildDungeonBossDeadNotify notify;
	notify.dungeonId = dungeonId;
	guildDungeon->GetGuildPtr()->Broadcast(notify);
}

void GuildDungeonMgr::GetRewardByType(UInt32 rewardType, UInt32 dungeonType, ItemRewardVec& rewardVec)
{
	RewardDetail* rewardDetail = GuildDungeonRewardDataEntryMgr::Instance()->GetRewardDetail(rewardType, dungeonType);
	if (NULL == rewardDetail)
	{
		ErrorLogStream << "Reward Error rewardType:" << rewardType << " dungeonType:" << dungeonType << LogStream::eos;
		return;
	}

	for (UInt32 groupSize = 0; groupSize < rewardDetail->rewardInfo.size(); ++groupSize)
	{
		RewardInfo* rewardInfo = rewardDetail->rewardInfo[groupSize];
		if (NULL == rewardInfo)
			continue;

		for (UInt32 k = 0; k < rewardInfo->rewardCnt; ++k)
		{
			auto rewardItem = GuildDungeonLotteryDataEntryMgr::Instance()->GetGuildDungeonReward(rewardInfo->rewardGroup);
			if (!rewardItem)
				continue;

			ItemReward reward;
			reward.id = rewardItem->itemId;
			reward.num = rewardItem->itemNum;
			rewardVec.push_back(reward);
		}
	}
}

void GuildDungeonMgr::GuildAuctionReward(UInt64 guildId, UInt32 dungeonType, UInt32 guildRank)
{
	RewardDetail* rewardDetail = GuildDungeonRewardDataEntryMgr::Instance()->GetRewardDetail(E_GUILD_DUNGEON_REWARD_AUCTION, dungeonType, guildRank);
	if (NULL == rewardDetail)
		return;

	// 先按权重随机出，有多少组奖励
	UInt32 randNum = Avalon::Random::RandBetween(0, rewardDetail->rewardSchemeWeight);
	auto iter = rewardDetail->rewardSchemeMap.lower_bound(randNum);
	if (iter == rewardDetail->rewardSchemeMap.end())
	{
		ErrorLogStream << "RewardSchemeMap Error rewardSchemeWeight:" << rewardDetail->rewardSchemeWeight << " RandNum:" << randNum << LogStream::eos;
		return;
	}

	UInt32 groupSize = iter->second;
	groupSize--;

	// 继续随机，出哪几个奖励组
	UInt32 firstGroup = 0;
	randNum = Avalon::Random::RandBetween(0, rewardDetail->rewardGroupWeight);
	auto iterRand = rewardDetail->rewardInfoMap.lower_bound(randNum);
	if (iterRand == rewardDetail->rewardInfoMap.end())
	{
		ErrorLogStream << " Rand Auction Reward Error!" << LogStream::eos;
		return;
	}
	else
	{
		RewardInfo* rewardInfo = iterRand->second;
		if (NULL == rewardInfo)
		{
			ErrorLogStream << "RewardInfo Null! Weight:" << iterRand->first << LogStream::eos;
			return;
		}
		firstGroup = rewardInfo->rewardGroup;

		GuildDungeonRewardVec dungeonRewardVec;
		GuildDungeonLotteryDataEntryMgr::Instance()->GetGuildDungeonReward(rewardInfo->rewardGroup, rewardInfo->rewardCnt, dungeonRewardVec);

		ItemRewardVec rewardVec;
		for (UInt32 k = 0; k < dungeonRewardVec.size(); ++k)
		{
			ItemReward reward;
			reward.id = dungeonRewardVec[k].itemId;
			reward.num = dungeonRewardVec[k].itemNum;
			rewardVec.push_back(reward);
		}

		GuildAuctionMgr::Instance()->ObtainAuctionReward(guildId, rewardInfo->rewardGroup, rewardInfo->auctionPrice, rewardInfo->fixPrice, rewardInfo->addPirce, rewardVec);
	}

	ItemRewardVec rewardVec;
	std::random_shuffle(rewardDetail->rewardInfo.begin(), rewardDetail->rewardInfo.end());
	for (UInt32 n = 0; n < groupSize; ++n)
	{
		if (rewardDetail->rewardInfo.size() <= n) return;
		RewardInfo* rewardInfo = rewardDetail->rewardInfo[n];
		if (NULL == rewardInfo)
		{
			ErrorLogStream << "RewardInfo Null! Weight:" << iterRand->first << LogStream::eos;
			continue;
		}

		if (rewardInfo->rewardGroup == firstGroup)
		{
			groupSize++;
			continue;
		}

		GuildDungeonRewardVec dungeonRewardVec;
		dungeonRewardVec.clear();
		GuildDungeonLotteryDataEntryMgr::Instance()->GetGuildDungeonReward(rewardInfo->rewardGroup, rewardInfo->rewardCnt, dungeonRewardVec);

		rewardVec.clear();
		for (UInt32 k = 0; k < dungeonRewardVec.size(); ++k)
		{
			ItemReward reward;
			reward.id = dungeonRewardVec[k].itemId;
			reward.num = dungeonRewardVec[k].itemNum;
			rewardVec.push_back(reward);
		}

		if (rewardVec.empty())
		{
			ErrorLogStream << "GuildAuctionReward Empty GuildId:" << guildId << " Type:" << dungeonType << LogStream::eos;
			continue;
		}

		GuildAuctionMgr::Instance()->ObtainAuctionReward(guildId, rewardInfo->rewardGroup, rewardInfo->auctionPrice, rewardInfo->fixPrice, rewardInfo->addPirce, rewardVec);
	}
}

UInt32 GuildDungeonMgr::StartGuildDungeon(UInt64 guildId, UInt32 dungeonId, UInt64 dungeonRaceId, 
	CLProtocol::GuildDungeonInfo& guildDungeonInfo)
{
	if (!IsStartDungeon())
	{
		return ErrorCode::GUILD_DUNGEON_NOT_OPEN;
	}

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
		if (NULL == guild)
		{
			ErrorLogStream << "StartGuildDungeon Not Find Guild Error!!! guildId:" << guildId << LogStream::eos;
			return ErrorCode::GUILD_NOT_IN_GUILD;
		}

		guildDungeon = CreateGuildDungeon(guild);
		if (NULL == guildDungeon)
		{
			ErrorLogStream << "StartGuildDungeon CreateGuildDungeon Failed !!! guildId:" << guildId << LogStream::eos;
			return ErrorCode::GUILD_DUNGEON_ERROR;
		}
	}

	GuildDungeonBattleRecord* battle = guildDungeon->GetGuildDungeonBattleRecord(dungeonId);
	if (NULL == battle)
	{
		ErrorLogStream << "StartGuildDungeon Not Find Battle Record Failed !!! guildId:" << guildId << LogStream::eos;
		return ErrorCode::GUILD_DUNGEON_ERROR;
	}

	const GuildDungeonCfg* dungeon_cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(dungeonId);
	if (NULL == dungeon_cfg)
	{
		ErrorLogStream << "StartGuildDungeon Get Dungeon Cfg Failed! DungeonId:" << dungeonId << LogStream::eos;
		return ErrorCode::GUILD_DUNGEON_CFG_ERROR;
	}
	
	if (dungeon_cfg->dungeonLvl == GUILD_DUNGEON_LOW)
	{
		if (guildDungeon->CheckAllBossIsDead())
		{
			ErrorLogStream << "StartGuildDungeon Boss Dead! GuildId:" << guildId << " DungeonId:" << dungeonId << LogStream::eos;
			return ErrorCode::GUILD_DUNGEON_GATE_FINISH;
		}
	}
	else
	{
		// boss挂了，不允许进入
		if (dungeon_cfg->bossId != 0 && battle->oddBlood == 0)
		{
			ErrorLogStream << "StartGuildDungeon Boss Dead! GuildId:" << guildId << " DungeonId:" << dungeonId << LogStream::eos;
			return ErrorCode::GUILD_DUNGEON_GATE_FINISH;
		}

		// 增加比赛
		guildDungeon->AddDungeonRace(dungeonId, dungeonRaceId);
	}

	guildDungeonInfo.bossOddBlood = battle->oddBlood;
	guildDungeonInfo.bossTotalBlood = dungeon_cfg->bossBlood;

	if (dungeon_cfg->dungeonLvl == GUILD_DUNGEON_HIGH)
	{
		std::map<UInt32, UInt32> tempBuffMap;
		const GuildDungeonBattleMap& dungeonBattleMap = guildDungeon->GetBattleRecordMap();
		for (auto& iter : dungeonBattleMap)
		{
			for (auto& it : iter.second->buffVec)
			{
				tempBuffMap[it.buffId] += it.buffLvl;
			}
		}

		// 合并相同的buff
		for (auto& tt : tempBuffMap)
		{
			CLProtocol::DungeonBuff buff;
			buff.buffId = tt.first;
			buff.buffLvl = tt.second;
			guildDungeonInfo.buffVec.push_back(buff);
		}
	}

	InfoLogStream << "StartGuildDungeon Enter Guild Dungeon guildId:" << guildId <<
		" dungeonId:" << dungeonId << " buffVec:" << guildDungeonInfo.SerializeBuffVec() << LogStream::eos;

	return ErrorCode::SUCCESS;
}

void GuildDungeonMgr::ClearDBTable(const std::string& _tableName)
{
	std::ostringstream cond;
	cond << "1 = 1";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(_tableName, cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildDungeonMgr::ClearGuildDungeon()
{
	for (auto& iter : m_GuildDungeonMap)
	{
		iter.second->Final();
		CL_SAFE_DELETE(iter.second);
	}

	m_GuildDungeonMap.clear();
}

void GuildDungeonMgr::AddRandDungeon(UInt32 dungeonType, UInt32 dungeonId)
{
	auto iter = m_RandDungeonMap.find(dungeonType);
	if (iter == m_RandDungeonMap.end())
	{
		std::vector<UInt32> tempVec;
		tempVec.push_back(dungeonId);

		m_RandDungeonMap[dungeonType] = tempVec;
	}
	else
	{
		iter->second.push_back(dungeonId);
	}
}

void GuildDungeonMgr::RandDungeon(UInt32 guildLvl)
{
	std::vector<UInt32> allTypeVec;
	GuildDungeonTypeDataEntryMgr::Instance()->GetDungeonType(allTypeVec);
	for (auto& type : allTypeVec)
	{
		GuildDungeonCfgVec dungeonCfgVec;
		GuildDungeonLvlDataEntryMgr::Instance()->GetDungeonCfgVec(type, guildLvl, dungeonCfgVec);
		if (dungeonCfgVec.empty())
		{
			ErrorLogStream << "GuildDungeonCfgVec Empty! type:" << type << LogStream::eos;
			continue;
		}

		if (dungeonCfgVec.size() > m_RandDungenNumMap[guildLvl])
			std::random_shuffle(dungeonCfgVec.begin(), dungeonCfgVec.end());

		UInt32 num = 0;
		for (auto& iter : dungeonCfgVec)
		{
			if (num == m_RandDungenNumMap[guildLvl])
				break;

			if (NULL == iter)
			{
				ErrorLogStream << "GuildDungeonCfg Null!!" << LogStream::eos;
				continue;
			}

			num++;
			AddRandDungeon(type, iter->dungeonId);

			CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_rand", CLRecordClient::Instance()->GenGuid());
			cmd->PutData("dungeon_type", type);
			cmd->PutData("dungeon_id", iter->dungeonId);
			CLRecordClient::Instance()->SendCommand(cmd);
		}
	}
}

UInt32 GuildDungeonMgr::GetPassGateTimeRank(UInt64 guildId)
{
	UInt32 rank = 0;
	for (auto& iter : m_GateTimeVec)
	{
		++rank;
		if (iter.guildId == guildId)
		{
			return rank;
		}
	}

	return 0;
}

void GuildDungeonMgr::GMUpdateStatus(UInt32 status)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	if (status == GUILD_DUNGEON_MAX)
	{
		InfoLogStream << "GMUpdateStatus To: " << status << LogStream::eos;
		InitTickMap();
		return;
	}

	if (!IsVaildStatus(status))	return;

	InfoLogStream << "GMUpdateStatus To: " << status << LogStream::eos;

	if (status == m_DungeonStatus) return;

	m_TickMap.clear();
	switch (status)
	{
		case GUILD_DUNGEON_PREPARE:
		{
			  if (m_DungeonStatus == GUILD_DUNGEON_START)
			  {
				  SwitchDungeonStatus(GUILD_DUNGEON_REWARD, CURRENT_TIME.Sec() - 5);
				  SwitchDungeonStatus(GUILD_DUNGEON_END, CURRENT_TIME.Sec() - 3);
			  }
			  else if (m_DungeonStatus == GUILD_DUNGEON_REWARD)
			  {
				  SwitchDungeonStatus(GUILD_DUNGEON_END, CURRENT_TIME.Sec() - 5);
			  }
		}
		break;
		case GUILD_DUNGEON_START:
		{
			if (m_DungeonStatus == GUILD_DUNGEON_REWARD)
			{
				SwitchDungeonStatus(GUILD_DUNGEON_END, CURRENT_TIME.Sec() - 5);
				SwitchDungeonStatus(GUILD_DUNGEON_PREPARE, CURRENT_TIME.Sec() - 3);
			}
			else if (m_DungeonStatus == GUILD_DUNGEON_END)
			{
				SwitchDungeonStatus(GUILD_DUNGEON_PREPARE, CURRENT_TIME.Sec() - 5);
			}
		}
		break;
		case GUILD_DUNGEON_REWARD:
		{
			if (m_DungeonStatus == GUILD_DUNGEON_END)
			{
				SwitchDungeonStatus(GUILD_DUNGEON_PREPARE, CURRENT_TIME.Sec() - 5);
				SwitchDungeonStatus(GUILD_DUNGEON_START, CURRENT_TIME.Sec() - 3);
			}
			else if (m_DungeonStatus == GUILD_DUNGEON_PREPARE)
			{
				SwitchDungeonStatus(GUILD_DUNGEON_START, CURRENT_TIME.Sec() - 5);
			}
		}
		break;
		case GUILD_DUNGEON_END:
		{
			if (m_DungeonStatus == GUILD_DUNGEON_PREPARE)
			{
				SwitchDungeonStatus(GUILD_DUNGEON_START, CURRENT_TIME.Sec() - 5);
				SwitchDungeonStatus(GUILD_DUNGEON_REWARD, CURRENT_TIME.Sec() - 3);
			}
			else if (m_DungeonStatus == GUILD_DUNGEON_START)
			{
				SwitchDungeonStatus(GUILD_DUNGEON_REWARD, CURRENT_TIME.Sec() - 5);
			}
		}
		break;
		default: return;
	}

	UInt64 now_stamp = CURRENT_TIME.Sec();
	m_DungeonStatus = status;
	if (m_DungeonStatus == GUILD_DUNGEON_START)
	{
		m_startStamp = CURRENT_TIME.Sec();
	}

	m_StatusStartStamp = now_stamp;
	SwitchDungeonStatus(status, now_stamp);

	UInt32 temp_status = m_DungeonStatus;
	while (temp_status < GUILD_DUNGEON_MAX)
	{
		UInt64 temp_stamp = 0;
		temp_status++;

		if (temp_status == GUILD_DUNGEON_START)
		{
			temp_stamp = now_stamp + 60 * 30;
		}
		else if (temp_status == GUILD_DUNGEON_REWARD)
		{
			if (m_DungeonStatus == GUILD_DUNGEON_PREPARE)
			{
				temp_stamp = now_stamp + 3600 * 2 + 60 * 30;
			}
			else if (m_DungeonStatus == GUILD_DUNGEON_START)
			{
				temp_stamp = now_stamp + 3600 * 2;
			}
		}
		else if (temp_status == GUILD_DUNGEON_MAX)
		{
			if (m_DungeonStatus == GUILD_DUNGEON_PREPARE)
			{
				temp_stamp = now_stamp + 3600 * 2 + 60 * 30 + 60 * 30;
			}
			else if (m_DungeonStatus == GUILD_DUNGEON_START)
			{
				temp_stamp = now_stamp + 3600 * 2 + 60 * 30;
			}
			else if (m_DungeonStatus == GUILD_DUNGEON_REWARD)
			{
				temp_stamp = now_stamp + 60 * 30;
			}

			m_TickMap[temp_stamp] = GUILD_DUNGEON_END;

			break;
		}

		m_TickMap[temp_stamp] = temp_status;
	}

	SyncGuildDungeonStatus();
}

void GuildDungeonMgr::ClearSmallBoss(UInt64 guildId, UInt64 playerId, UInt32 occu, const std::string& playerName)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "ClearSmallBoss Not Find Guild Failed !!! guildId:" << guildId << LogStream::eos;
		return;
	}

	const GuildDungeonBattleMap& battle_record = guildDungeon->GetBattleRecordMap();
	for (auto& iter : battle_record)
	{
		if (NULL == iter.second)
			continue;

		const GuildDungeonCfg* dungeon_cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(iter.second->dungeonId);
		if (NULL == dungeon_cfg)
		{
			ErrorLogStream << "ClearSmallBoss Get Dungeon Cfg Failed! DungeonId:" <<iter.second->dungeonId << LogStream::eos;
			continue;
		}

		// 清除小boss
		if (dungeon_cfg->dungeonLvl != GUILD_DUNGEON_MID)
			continue;

		guildDungeon->AddJoinPlayer(playerId, true);
		RecordDungeonDamage(guildId, iter.second->dungeonId, playerId, occu, 2000000000, 10, playerName);
	}
}

void GuildDungeonMgr::ClearBigBoss(UInt64 guildId, UInt64 playerId, UInt32 occu, UInt64 damage, const std::string& playerName)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	InfoLogStream <<"ClearBigBoss PlayerName:"<<playerName<<" GuildId:"<<guildId<<" PlayerId:"<<playerId<<" Damage:"<<damage << LogStream::eos;

	ClearSmallBoss(guildId, playerId, occu, playerName);

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "ClearBigBoss Not Find Guild Failed !!! guildId:" << guildId << LogStream::eos;
		return;
	}

	guildDungeon->AddJoinPlayer(playerId, true);

	auto dungeonRecord = guildDungeon->GetFinalGuildDungeonBattleRecord();
	if (dungeonRecord)
	{
		RecordDungeonDamage(guildId, dungeonRecord->dungeonId, playerId, occu, damage, 10, playerName);
	}
}

void GuildDungeonMgr::GMAddBuffCnt(UInt64 guildId, UInt32 buffItemId, UInt32 cnt)
{
	if (!SwitchIsOpen())
	{
		return;
	}

	InfoLogStream << "GMAddBuffCnt GuildId:" << guildId << " buffItemId:" << buffItemId <<" cnt:" << cnt << LogStream::eos;

	GuildDungeon* guildDungeon = FindGuildDungeon(guildId);
	if (NULL == guildDungeon)
	{
		ErrorLogStream << "Not Find Guild Failed !!!" << LogStream::eos;
		return;
	}

	std::map<UInt32, std::set<UInt32>> tempMap;
	{
		std::set<UInt32> tempSet1 = { 7011000, 7012000, 7013000 };
		tempMap[830000001] = tempSet1;

		std::set<UInt32> tempSet2 = { 7021000, 7022000, 7023000 };
		tempMap[830000002] = tempSet2;
	}

	auto iter = tempMap.find(buffItemId);
	if (iter == tempMap.end())
	{
		ErrorLogStream << "Not Find buff" << LogStream::eos;
		return;
	}

	const GuildDungeonBattleMap& battle_record = guildDungeon->GetBattleRecordMap();
	for (auto& dungeonId : iter->second)
	{
		for (auto& it : battle_record)
		{
			if (it.second->dungeonId != dungeonId)
			{
				continue;
			}

			guildDungeon->AddBattleCnt(dungeonId, cnt);

			while (cnt--)
			{
				guildDungeon->AddDropbuff(dungeonId, buffItemId);
			}

			guildDungeon->UpdateDropBuffDB(dungeonId);
			return;
		}
	}
}
