/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLHonorLevelDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

HonorLevelDataEntry::HonorLevelDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

HonorLevelDataEntry::~HonorLevelDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 HonorLevelDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool HonorLevelDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	honorLvlGrade = row.ReadUInt32();
	needExp = row.ReadUInt32();
	weekAddLimit = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				shopDiscount.push_back(param);
			}
		}
	}
	lvlNeedRankRate = row.ReadUInt32();
	rankLeastNum = row.ReadUInt32();
	title = row.ReadUInt32();
	titleDueTime = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__

	{
		for (auto& iter : shopDiscount)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(iter, strs, "_");
			if (strs.size() != 2)
			{
				ErrorLogStream << "honor shop discount error:" << id << LogStream::eos;
				return false;
			}

			UInt32 shopId = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			UInt32 discount = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
			shopDiscountMap[shopId] = discount;
		}
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool HonorLevelDataEntryMgr::AddEntry(HonorLevelDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<HonorLevelDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->needExp == 0)
	{
		return true;
	}

	m_LvlNeedExpMap[entry->needExp] = entry->id;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 HonorLevelDataEntryMgr::GetHonorLvl(UInt32 exp)
{
	auto iter = m_LvlNeedExpMap.upper_bound(exp);
	if (iter == m_LvlNeedExpMap.end())
	{
		return (UInt32)m_LvlNeedExpMap.size();
	}

	if (iter == m_LvlNeedExpMap.begin())
	{
		return 0;
	}

	--iter;

	return iter->second;
}

UInt32 HonorLevelDataEntryMgr::GetShopDiscount(UInt32 honorLvl, UInt32 shopId)
{
	const HonorLevelDataEntry* honorLvlData = FindEntry(honorLvl);
	if (NULL == honorLvlData)
	{
		ErrorLogStream << "not find honor lvl config:" << honorLvl << LogStream::eos;
		return 0;
	}

	auto iter = honorLvlData->shopDiscountMap.find(shopId);
	return iter != honorLvlData->shopDiscountMap.end() ? iter->second : 0;
}

const HonorLevelDataEntry* HonorLevelDataEntryMgr::FindEntry(UInt32 honorLvl)
{
	// 0在配置表转换成1000
	if (honorLvl == 0)
		honorLvl = 1000;

	return Avalon::DataEntryMgrBase<HonorLevelDataEntry>::FindEntry(honorLvl);
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

