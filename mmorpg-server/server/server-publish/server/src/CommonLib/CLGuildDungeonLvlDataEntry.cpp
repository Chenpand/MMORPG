/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildDungeonLvlDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildDungeonLvlDataEntry::GuildDungeonLvlDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildDungeonLvlDataEntry::~GuildDungeonLvlDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildDungeonLvlDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildDungeonLvlDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	dungeonType = row.ReadUInt32();
	dungeonLvl = row.ReadUInt32();
	dungeonId = row.ReadUInt32();
	bossId = row.ReadUInt32();
	bossBlood = row.ReadString();
	rewardType = row.ReadUInt32();
	dungeonName = row.ReadString();
	dropBuff = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildDungeonLvlDataEntryMgr::AddEntry(GuildDungeonLvlDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildDungeonLvlDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->dungeonType == 0 || entry->dungeonLvl == 0 || entry->dungeonId == 0)
	{
		ErrorLogStream << "GuildDungeonLvlDataEntryMgr Cfg Error Id:"<< entry->id << LogStream::eos;
		return false;
	}

	// 按地下城类型存下 终极地下城的Id
	if (entry->dungeonLvl == GUILD_DUNGEON_HIGH)
	{
		m_GuildDungeonTypeFinalGate[entry->dungeonType] = entry->dungeonId;
	}

	GuildDungeonCfg* dungeonCfg = new GuildDungeonCfg();
	dungeonCfg->dungeonId = entry->dungeonId;
	dungeonCfg->dungeonLvl = entry->dungeonLvl;
	dungeonCfg->bossId = entry->bossId;
	dungeonCfg->bossBlood = Avalon::StringUtil::ConvertToValue<UInt64>(entry->bossBlood);
	dungeonCfg->rewardType = entry->rewardType;
	dungeonCfg->dungeonName = entry->dungeonName;
	dungeonCfg->dropBuff = entry->dropBuff;

	m_GuildDungeonCfg[entry->dungeonId] = dungeonCfg;

	auto iter = m_GuildDungeonTypeCfgMap.find(entry->dungeonType);
	if (iter == m_GuildDungeonTypeCfgMap.end())
	{
		GuildDungeonCfgVec dungeonCfgVec;
		dungeonCfgVec.push_back(dungeonCfg);

		GuildDungeonCfgMap dungeonCfgMap;
		dungeonCfgMap[entry->dungeonLvl] = dungeonCfgVec;

		m_GuildDungeonTypeCfgMap[entry->dungeonType] = dungeonCfgMap;
	}
	else
	{
		GuildDungeonCfgMap& dungeonCfgMap = iter->second;
		auto it = dungeonCfgMap.find(entry->dungeonLvl);
		if (it == dungeonCfgMap.end())
		{
			GuildDungeonCfgVec dungeonCfgVec;
			dungeonCfgVec.push_back(dungeonCfg);

			dungeonCfgMap[entry->dungeonLvl] = dungeonCfgVec;
		}
		else
		{
			it->second.push_back(dungeonCfg);
		}
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

bool GuildDungeonLvlDataEntryMgr::GetDungeonCfgVec(UInt32 dungeonType, GuildDungeonCfgVec& dungeonVec)
{
	auto iter = m_GuildDungeonTypeCfgMap.find(dungeonType);
	if (iter == m_GuildDungeonTypeCfgMap.end())
		return false;

	GuildDungeonCfgMap& dungeonCfgMap = iter->second;
	for (auto& it : dungeonCfgMap)
	{
		dungeonVec.insert(dungeonVec.end(), it.second.begin(), it.second.end());
	}

	return true;
}

bool GuildDungeonLvlDataEntryMgr::GetDungeonCfgVec(UInt32 dungeonType, UInt32 dungeonLvl, GuildDungeonCfgVec& dungeonVec)
{
	auto iter = m_GuildDungeonTypeCfgMap.find(dungeonType);
	if (iter == m_GuildDungeonTypeCfgMap.end())
		return false;

	GuildDungeonCfgMap& dungeonCfgMap = iter->second;
	auto it = dungeonCfgMap.find(dungeonLvl);
	if (it == dungeonCfgMap.end())
		return false;

	dungeonVec.insert(dungeonVec.end(), it->second.begin(), it->second.end());

	return true;
}

UInt32 GuildDungeonLvlDataEntryMgr::GetFinalDungeonId(UInt32 dungeonType)
{
	auto iter = m_GuildDungeonTypeFinalGate.find(dungeonType);
	return iter != m_GuildDungeonTypeFinalGate.end() ? iter->second : 0;
}

const GuildDungeonCfg* GuildDungeonLvlDataEntryMgr::GetGuildDungeonCfg(UInt32 dungeonId)
{
	auto iter = m_GuildDungeonCfg.find(dungeonId);
	return iter != m_GuildDungeonCfg.end() ? iter->second : NULL;
}

bool GuildDungeonLvlDataEntryMgr::IsFinalBossGate(UInt32 dungeonId)
{
	const GuildDungeonCfg* cfg = GetGuildDungeonCfg(dungeonId);
	if (NULL == cfg)
		return false;

	if (cfg->dungeonLvl == GUILD_DUNGEON_HIGH)
	{
		return true;
	}

	return false;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

