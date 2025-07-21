/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLWholeBargainDiscountDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

WholeBargainDiscountDataEntry::WholeBargainDiscountDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

WholeBargainDiscountDataEntry::~WholeBargainDiscountDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 WholeBargainDiscountDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool WholeBargainDiscountDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	joinNum = row.ReadUInt32();
	discount = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool WholeBargainDiscountDataEntryMgr::AddEntry(WholeBargainDiscountDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WholeBargainDiscountDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->joinNum > m_MaxJoinNum)
	{
		m_MaxJoinNum = entry->joinNum;
	}

	m_JoinDiscountMap[entry->joinNum] = entry->discount;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 WholeBargainDiscountDataEntryMgr::GetDiscount(UInt32 joinNum)
{
	// 最大折扣
	if (joinNum >= m_MaxJoinNum)
	{
		auto max = m_JoinDiscountMap.find(m_MaxJoinNum);
		return max != m_JoinDiscountMap.end() ? max->second : 100;
	}

	auto iter = m_JoinDiscountMap.upper_bound(joinNum);
	if (iter == m_JoinDiscountMap.end() || iter == m_JoinDiscountMap.begin())
		return 100;

	--iter;

	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

