/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiScoreDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiScoreDataEntry::ChiJiScoreDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiScoreDataEntry::~ChiJiScoreDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiScoreDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiScoreDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	score = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiScoreDataEntryMgr::AddEntry(ChiJiScoreDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChiJiScoreDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	m_ScoreMap[entry->id] = entry->score;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 ChiJiScoreDataEntryMgr::GetScoreByRank(UInt32 rank)
{
	auto iter = m_ScoreMap.lower_bound(rank);
	if (iter == m_ScoreMap.end())
	{
		return 0;
	}

	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

