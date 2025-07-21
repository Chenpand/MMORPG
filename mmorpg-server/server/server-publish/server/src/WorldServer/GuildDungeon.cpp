#include "GuildDungeon.h"
#include "Guild.h"
#include "GuildDungeonMgr.h"
#include "DungeonRaceMgr.h"

#include <CLGuildDungeonLvlDataEntry.h>
#include <CLGuildDungeonRewardDataEntry.h>
#include <CLGuildDungeonLotteryDataEntry.h>
#include <CLItemBuffDataEntry.h>
#include <CLRecordClient.h>

GuildDungeon::GuildDungeon()
{
	m_Guild = NULL;
	m_DungeonType = 0;
	m_TotalDamage = 0;
	m_VictoryPubCnt = 0;
	m_VictoryPubStamp = 0;
}

GuildDungeon::~GuildDungeon()
{

}

bool GuildDungeon::OnInit(Guild* guildPtr)
{
	if (NULL == guildPtr)
		return false;

	m_Guild = guildPtr;
	m_TotalDamage = 0;
	
	m_DungeonType = m_Guild->GetGuildDungeonType(); 

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_info", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("guild_id", m_Guild->GetID());
	cmd->PutData("dungeon_type", m_DungeonType);
	cmd->PutData("total_damage", m_TotalDamage);
	CLRecordClient::Instance()->SendCommand(cmd);

	// 先初始化初级关卡
	GuildDungeonCfgVec dungeonCfgVec;
	GuildDungeonLvlDataEntryMgr::Instance()->GetDungeonCfgVec(m_DungeonType, GUILD_DUNGEON_LOW, dungeonCfgVec);
	if (dungeonCfgVec.empty())
	{
		ErrorLogStream << "GuildDungeon::OnInit dungeonCfgVec Error!" << LogStream::eos;
		return false;
	}

	for (auto& iter : dungeonCfgVec)
	{
		if (NULL == iter)
		{
			ErrorLogStream << "GuildDungeonCfg Null!!" << LogStream::eos;
			continue;
		}

		GuildDungeonBattleRecord* battleRecord = new GuildDungeonBattleRecord();
		battleRecord->dungeonId = iter->dungeonId;
		battleRecord->battleCnt = 0;
		battleRecord->oddBlood = iter->bossBlood;

		m_BattleRecordMap[battleRecord->dungeonId] = battleRecord;

		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_battle", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("guild_id", m_Guild->GetID());
		cmd->PutData("dungeon_id", iter->dungeonId);
		cmd->PutData("battle_cnt", 0);
		cmd->PutData("boss_odd_blood", iter->bossBlood);
		cmd->PutData("drop_buff", "");
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	// 初始化随机关卡（使徒，大boss）
	std::vector<UInt32> randVec;
	GuildDungeonMgr::Instance()->GetRandDungeon(m_DungeonType, randVec);
	if (randVec.empty())
	{
		ErrorLogStream << "randVec null type:"<< m_DungeonType << LogStream::eos;
		return false;
	}

	for (auto& dungeonId : randVec)
	{
		const GuildDungeonCfg* cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(dungeonId);
		if (NULL == cfg)
		{
			ErrorLogStream << "not find GuildDungeonCfg:"<< dungeonId << LogStream::eos;
			continue;
		}

		GuildDungeonBattleRecord* battleRecord = new GuildDungeonBattleRecord();
		battleRecord->dungeonId = cfg->dungeonId;
		battleRecord->battleCnt = 0;
		battleRecord->oddBlood = cfg->bossBlood;

		m_BattleRecordMap[battleRecord->dungeonId] = battleRecord;

		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_battle", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("guild_id", m_Guild->GetID());
		cmd->PutData("dungeon_id", cfg->dungeonId);
		cmd->PutData("battle_cnt", 0);
		cmd->PutData("boss_odd_blood", cfg->bossBlood);
		cmd->PutData("drop_buff", "");
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	InfoLogStream << "GuildDungeon::OnInit guildId:" << guildPtr->GetID() <<" DungeonType:"<<m_DungeonType<< LogStream::eos;

	return true;
}

void GuildDungeon::OnTick(const Avalon::Time& now)
{

}

void GuildDungeon::Final()
{
	m_Guild = NULL;

	for (auto& iter : m_BattleRecordMap)
	{
		delete iter.second;
		iter.second = NULL;
	}

	m_BattleRecordMap.clear();

	for (auto& iter : m_DamageVec)
	{
		delete iter;
		iter = NULL;
	}

	m_DamageVec.clear();
}

void GuildDungeon::AddDropbuff(UInt32 dungeonId, UInt32 buffItemId)
{
	auto iter = m_BattleRecordMap.find(dungeonId);
	if (iter == m_BattleRecordMap.end())
	{
		ErrorLogStream << "AddDropbuff Not Find BattleRecord !!! guildId:" << m_Guild->GetID()
			<< " dungeonId:" << dungeonId << LogStream::eos;
		return;
	}

	if (NULL == iter->second)
		return;

	ItemBuffDataEntry* itemBuffEntry = ItemBuffDataEntryMgr::Instance()->FindEntry(buffItemId);
	if (NULL == itemBuffEntry)
	{
		ErrorLogStream << "AddDropbuff Not Find ItemBuffDataEntry Failed !!! guildId:" <<
			m_Guild->GetID() << " itemBuffId:" << buffItemId << LogStream::eos;
		return;
	}

	bool isAdd = true;
	for (auto& buff : iter->second->buffVec)
	{
		if (buff.buffId != itemBuffEntry->buffId)
			continue;

		if (buff.buffLvl >= itemBuffEntry->buffMaxLvl)
		{
			isAdd = false;
			break;
		}

		buff.buffLvl++;
		isAdd = false;
		break;
	}

	if (isAdd)
	{
		GuildDungeonBuff dungeonBuff;
		dungeonBuff.buffId = itemBuffEntry->buffId;
		dungeonBuff.buffLvl = 1 ;
		iter->second->buffVec.push_back(dungeonBuff);
	}
}

void GuildDungeon::UpdateDropBuffDB(UInt32 dungeonId)
{
	GuildDungeonBattleRecord* battleRecord = GetGuildDungeonBattleRecord(dungeonId);
	if (NULL == battleRecord)
	{
		ErrorLogStream <<"UpdateDropBuffDB not find GuildDungeonBattleRecord guildId:"<<m_Guild->GetID()
			<<" dungeonId:"<<dungeonId<< LogStream::eos;
		return;
	}

	std::string buff_str;
	buff_str.reserve(255);
	for (auto& iter : battleRecord->buffVec)
	{
		buff_str.append(std::to_string(iter.buffId)).append(",").
			append(std::to_string(iter.buffLvl)).append("|");
	}

	std::ostringstream cond;
	cond << "guild_id=" << m_Guild->GetID()<<" and dungeon_id="<<dungeonId;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon_battle", cond.str());
	cmd->PutData("drop_buff", buff_str);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildDungeon::CostBossBlood(UInt32 dungeonId, UInt64 bossDamage)
{
	if (bossDamage == 0) return;

	GuildDungeonBattleRecord* battleRecord = GetGuildDungeonBattleRecord(dungeonId);
	if (NULL == battleRecord)
	{
		ErrorLogStream << "ModifyDungeonBattle not find GuildDungeonBattleRecord guildId:" << m_Guild->GetID()
			<< " dungeonId:" << dungeonId << LogStream::eos;
		return;
	}

	InfoLogStream << "CostBossBlood GuildId:" << m_Guild->GetID() << " DungeonId:" << dungeonId 
		<< " BossOddBlood:" << battleRecord->oddBlood << " Damage:" << bossDamage << LogStream::eos;

	if (battleRecord->oddBlood == 0) return;

	if (bossDamage >= battleRecord->oddBlood)
		battleRecord->oddBlood = 0;
	else
		battleRecord->oddBlood -= bossDamage;

	std::ostringstream cond;
	cond << "guild_id=" << m_Guild->GetID() << " and dungeon_id=" << dungeonId;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon_battle", cond.str());
	cmd->PutData("boss_odd_blood", battleRecord->oddBlood);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildDungeon::AddBattleCnt(UInt32 dungeonId, UInt32 addCnt)
{
	GuildDungeonBattleRecord* battleRecord = GetGuildDungeonBattleRecord(dungeonId);
	if (NULL == battleRecord)
	{
		ErrorLogStream << "AddBattleCnt not find GuildDungeonBattleRecord guildId:" << m_Guild->GetID()
			<< " dungeonId:" << dungeonId << LogStream::eos;
		return;
	}

	battleRecord->battleCnt += addCnt;

	std::ostringstream cond;
	cond << "guild_id=" << m_Guild->GetID() << " and dungeon_id=" << dungeonId;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon_battle", cond.str());
	cmd->PutData("battle_cnt", battleRecord->battleCnt);
	CLRecordClient::Instance()->SendCommand(cmd);
}

UInt64 GuildDungeon::GetBossOddBlood(UInt32 dungeonId)
{
	GuildDungeonBattleRecord* battleRecord = GetGuildDungeonBattleRecord(dungeonId);
	return battleRecord != NULL ? battleRecord->oddBlood : 0;
}

GuildDungeonBattleRecord* GuildDungeon::GetFinalGuildDungeonBattleRecord()
{
	return GetGuildDungeonBattleRecord(GuildDungeonLvlDataEntryMgr::Instance()->GetFinalDungeonId(m_DungeonType));
}

GuildDungeonBattleRecord* GuildDungeon::GetGuildDungeonBattleRecord(UInt32 dungeonId)
{
	auto iter = m_BattleRecordMap.find(dungeonId);
	return iter != m_BattleRecordMap.end() ? iter->second : NULL;
}

bool GuildDungeon::CheckAllBossIsDead()
{
	for (auto& iter : m_BattleRecordMap)
	{
		if (iter.second->oddBlood > 0)
			return false;
	}

	return true;
}

UInt32 GuildDungeon::GetFinalBossBloodRate()
{
	GuildDungeonBattleRecord* battleRecord = GetFinalGuildDungeonBattleRecord();
	if (NULL == battleRecord)
		return 0;

	const GuildDungeonCfg* dungeon_cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(battleRecord->dungeonId);
	if (NULL == dungeon_cfg)
	{
		ErrorLogStream << "GetFinalBossBloodRate Get Dungeon Cfg Failed! DungeonId:" << battleRecord->dungeonId << LogStream::eos;
		return 0;
	}

	UInt64 totalBlood = dungeon_cfg->bossBlood;
	if (totalBlood == 0)
		return 0;

	if (totalBlood <= battleRecord->oddBlood)
		return 0;
	
	// 万分比返回
	return UInt32(ceil((totalBlood - battleRecord->oddBlood) / totalBlood)) * 10000;
}

void GuildDungeon::AddBattleRecord(UInt32 dungeonId, UInt32 battleCnt, UInt64 bossOddBlood, const std::string& dropBuff)
{
	const GuildDungeonCfg* dungeonCfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(dungeonId);
	if (NULL == dungeonCfg)
	{
		ErrorLogStream <<"AddBattleRecord Get Dungeon Cfg Failed! DungeonId:"<< dungeonId << LogStream::eos;
		return;
	}

	GuildDungeonBattleRecord* record = new GuildDungeonBattleRecord();
	record->dungeonId = dungeonId;
	record->battleCnt = battleCnt;
	record->oddBlood = bossOddBlood;

	if (!dropBuff.empty())
	{
		std::vector<std::string> param_1;
		Avalon::StringUtil::Split(dropBuff, param_1, "|");
		for (auto& iter : param_1)
		{
			if (iter.empty())
				continue;

			std::vector<std::string> param_2;
			Avalon::StringUtil::Split(iter, param_2, ",");
			if (param_2.size() != 2)
			{
				ErrorLogStream << "AddBattleRecord Parse Buff Failed! dropBuff:" << dropBuff << LogStream::eos;
				delete record;
				record = NULL;
				return;
			}

			GuildDungeonBuff dungeonBuff;

			try
			{
				dungeonBuff.buffId = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
				dungeonBuff.buffLvl = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
			}
			catch (...)
			{
				ErrorLogStream << "AddBattleRecord StringUtil Failed! dropBuff:" << dropBuff << LogStream::eos;
				continue;
			}

			record->buffVec.push_back(dungeonBuff);
		}
	}

	m_BattleRecordMap[dungeonId] = record;
}

void GuildDungeon::UpdateTotalDamageDB()
{
	std::ostringstream cond;
	cond << "guild_id=" << m_Guild->GetID();
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon_info", cond.str());
	cmd->PutData("total_damage", m_TotalDamage);
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool GuildDungeon::SmallBossIsAllDead()
{
	for (auto& iter : m_BattleRecordMap)
	{
		const GuildDungeonCfg* cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(iter.second->dungeonId);
		if (NULL == cfg)
		{
			ErrorLogStream << "GuildDungeonLvlDataEntryMgr Find Cfg Failed!!! id:" << iter.second->dungeonId << LogStream::eos;
			continue;
		}

		if (cfg->dungeonLvl != GUILD_DUNGEON_MID)
		{
			continue;
		}

		if (iter.second->oddBlood > 0)
		{
			return false;
		}
	}

	return true;
}

UInt32 GuildDungeon::SmallBossDeadNum()
{
	UInt32 num = 0;
	for (auto& iter : m_BattleRecordMap)
	{
		const GuildDungeonCfg* cfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(iter.second->dungeonId);
		if (NULL == cfg)
		{
			ErrorLogStream << "GuildDungeonLvlDataEntryMgr Find Cfg Failed!!! id:" << iter.second->dungeonId << LogStream::eos;
			continue;
		}

		if (cfg->dungeonLvl != GUILD_DUNGEON_MID)
		{
			continue;
		}

		if (iter.second->oddBlood == 0)
		{
			++num;
		}
	}

	return num;
}

bool DamageGreaterSort(GuildDungeonPlayerDamage* a, GuildDungeonPlayerDamage* b)
{
	if (!a || !b)
	{
		return false;
	}

	return (a->damagePoint > b->damagePoint);
}

void GuildDungeon::AddPlayerDamage(UInt64 playerId, UInt64 damagePoint, const std::string& playerName, bool isSave)
{
	for (auto& iter : m_DamageVec)
	{
		if (iter->playerId != playerId)
		{
			continue;
		}

		iter->damagePoint += damagePoint;
		iter->playerName = playerName;

		if (isSave)
		{
			std::ostringstream cond;
			cond << "guild_id=" << m_Guild->GetID() << " and player_id=" << playerId;
			CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_dungeon_player_damage", cond.str());
			cmd->PutData("player_name", iter->playerName);
			cmd->PutData("damage_val", iter->damagePoint);
			CLRecordClient::Instance()->SendCommand(cmd);
		}

		std::sort(m_DamageVec.begin(), m_DamageVec.end(), DamageGreaterSort);

		return;
	}

	GuildDungeonPlayerDamage* playerDamage = new GuildDungeonPlayerDamage();
	playerDamage->playerId = playerId;
	playerDamage->damagePoint = damagePoint;
	playerDamage->playerName = playerName;

	m_DamageVec.push_back(playerDamage);
	std::sort(m_DamageVec.begin(), m_DamageVec.end(), DamageGreaterSort);

	if (isSave)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_player_damage", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("guild_id", m_Guild->GetID());
		cmd->PutData("player_id", playerId);
		cmd->PutData("player_name", playerName);
		cmd->PutData("damage_val", damagePoint);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void GuildDungeon::AddJoinPlayer(UInt64 playerId, bool isSave)
{
	auto iter = m_JoinPlayer.find(playerId);
	if (iter != m_JoinPlayer.end())
	{
		return;
	}

	m_JoinPlayer.insert(playerId);

	if (isSave)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_dungeon_player_join", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("guild_id", m_Guild->GetID());
		cmd->PutData("player_id", playerId);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void GuildDungeon::AddDungeonRace(UInt32 dungeonId, UInt64 dungeonRaceId)
{
	m_DungeonRaceMap[dungeonId].insert(dungeonRaceId);
}

void GuildDungeon::EndDungeonRace(UInt32 dungeonId, UInt64 dungeonRaceId)
{
	if (dungeonId == 0 || dungeonRaceId == 0) return;
	
	auto it = m_DungeonRaceMap.find(dungeonId);
	if (it == m_DungeonRaceMap.end())
		return;

	// 删除当前局
	it->second.erase(dungeonRaceId);

	for (auto& iter : it->second)
	{
		DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceById(iter);
		if (NULL == race)
		{
			ErrorLogStream << "not find dungeon race id:" << iter << LogStream::eos;
			continue;
		}

		race->EndGuildDungeon();
	}

	m_DungeonRaceMap.erase(it);
}

void GuildDungeon::NotifyRaceBossOddBlood(UInt32 dungeonId, UInt64 dungeonRaceId)
{
	if (dungeonId == 0 || dungeonRaceId == 0) return;

	auto it = m_DungeonRaceMap.find(dungeonId);
	if (it == m_DungeonRaceMap.end())
		return;

	UInt64 bossTotalBlood = 0;
	const GuildDungeonCfg* dungeonCfg = GuildDungeonLvlDataEntryMgr::Instance()->GetGuildDungeonCfg(dungeonId);
	if (dungeonCfg != NULL)
	{
		bossTotalBlood = dungeonCfg->bossBlood;
	}

	for (auto& iter : it->second)
	{
		if (iter == dungeonRaceId)
			continue;

		DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceById(iter);
		if (NULL == race)
		{
			ErrorLogStream << "not find dungeon race id:" << iter << LogStream::eos;
			continue;
		}

		race->NotifyGuildDungeonBossOddBlood(GetBossOddBlood(dungeonId), bossTotalBlood);
	}
}