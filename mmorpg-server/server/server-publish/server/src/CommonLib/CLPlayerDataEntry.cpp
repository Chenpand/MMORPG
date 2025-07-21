#include "CLPlayerDataEntry.h"

PlayerDataEntry::PlayerDataEntry()
{
}

PlayerDataEntry::~PlayerDataEntry()
{
}

bool PlayerDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt16();
	upgradeexp = row.ReadUInt32();
	sp = row.ReadUInt32();

	skillGridSize = row.ReadUInt32();
	itemGridSize = 0;

	std::string strTmp = row.ReadString();
	std::vector<std::string> poolVec;
	std::string split = ";";
	std::string split1 = ",";

	Avalon::StringUtil::Split(strTmp, poolVec, split);
	for (UInt32 i = 0; i < poolVec.size(); ++i)
	{
		std::vector<std::string> tmpVec;
		Avalon::StringUtil::Split(poolVec[i], tmpVec, split1);
		if (tmpVec.size() < 2) continue;
		DailyTaskSelector pool;
		pool.type = (DailyTaskPoolType)Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[0]);
		pool.num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[1]);
		dailyTaskPool.push_back(pool);
	}

	retinueSize = row.ReadUInt32();
	adventureTeamAdditionRate = row.ReadUInt32();
	score = row.ReadUInt32();
	creditPointMonthGetLimit = row.ReadUInt32();
	creditPointHoldLimit = row.ReadUInt32();
	creditPointTransferLimit = row.ReadUInt32();

	return true;
}

PlayerDataEntryMgr::PlayerDataEntryMgr()
{
	m_MaxSkillGridSize = 0;
	m_MaxLevel = 0;
}

PlayerDataEntryMgr::~PlayerDataEntryMgr()
{
}

bool PlayerDataEntryMgr::AddEntry(PlayerDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<PlayerDataEntry>::AddEntry(entry)) return false;

	UInt64 sumExp = GetSumExp(entry->level - 1);
	sumExp += entry->upgradeexp;
	m_SumExpMap.insert(std::make_pair(entry->level, sumExp));

	UInt32 sumSP = GetSumSP(entry->level - 1);
	sumSP += entry->sp;
	m_SumSPMap.insert(std::make_pair(entry->level, sumSP));

	if (m_MaxSkillGridSize < entry->skillGridSize)
		m_MaxSkillGridSize = entry->skillGridSize;

	if (m_MaxLevel < entry->level)
		m_MaxLevel = entry->level;
	return true;
}

UInt64 PlayerDataEntryMgr::GetSumExp(UInt16 level)
{
	SumExpMap::iterator iter = m_SumExpMap.find(level);
	if (iter != m_SumExpMap.end())
	{
		return iter->second;
	}
	return 0;
}

UInt32 PlayerDataEntryMgr::GetSumSP(UInt16 level)
{
	SumSPMap::iterator iter = m_SumSPMap.find(level);
	if (iter != m_SumSPMap.end())
	{
		return iter->second;
	}
	return 0;
}

UInt32 PlayerDataEntryMgr::GetMaxSkillGridSize()
{
	return m_MaxSkillGridSize;
}

UInt32 PlayerDataEntryMgr::GetCreditPointMonthGetLimit(UInt32 level)
{
	auto data = FindEntry(level);
	if (NULL == data)
	{
		return 0;
	}

	return data->creditPointMonthGetLimit;
}

UInt32 PlayerDataEntryMgr::GetCreditPointHoldLimit(UInt32 level)
{
	auto data = FindEntry(level);
	if (NULL == data)
	{
		return 0;
	}

	return data->creditPointHoldLimit;
}

UInt32 PlayerDataEntryMgr::GetCreditPointTransferLimit(UInt32 level)
{
	auto data = FindEntry(level);
	if (NULL == data)
	{
		return 0;
	}

	return data->creditPointTransferLimit;
}