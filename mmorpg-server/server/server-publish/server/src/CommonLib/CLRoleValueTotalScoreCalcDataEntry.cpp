/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLRoleValueTotalScoreCalcDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

RoleValueTotalScoreCalcDataEntry::RoleValueTotalScoreCalcDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

RoleValueTotalScoreCalcDataEntry::~RoleValueTotalScoreCalcDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 RoleValueTotalScoreCalcDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool RoleValueTotalScoreCalcDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	roleValueScoreProb = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__

UInt32 RoleValueTotalScoreCalcDataEntry::ConvertToTotalScoreBySingleScore(UInt32 singleScore)
{
	return singleScore * (UInt64)roleValueScoreProb / 100;
}

// __CUSTOM_ENTRYFUNCTION_END__

bool RoleValueTotalScoreCalcDataEntryMgr::AddEntry(RoleValueTotalScoreCalcDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<RoleValueTotalScoreCalcDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (GetMaxKeyInDatas() < entry->GetKey())
	{
		m_MaxKeyInDatas = entry->GetKey();
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

