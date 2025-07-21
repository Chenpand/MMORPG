/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLMallFashionLimitedSuitDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

MallFashionLimitedSuitDataEntry::MallFashionLimitedSuitDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

MallFashionLimitedSuitDataEntry::~MallFashionLimitedSuitDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 MallFashionLimitedSuitDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return occu;
// __CUSTOM_GETKEY_END__
}

bool MallFashionLimitedSuitDataEntry::Read(Avalon::DataRow &row)
{
	occu = row.ReadUInt32();
	limitedSuit = row.ReadString();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool MallFashionLimitedSuitDataEntryMgr::AddEntry(MallFashionLimitedSuitDataEntry* entry)
{
	//if (!Avalon::DataEntryMgrBase<MallFashionLimitedSuitDataEntry>::AddEntry(entry))
	//{
	//	return false;
	//}
// __CUSTOM_ADDENTRY_BEGIN__
	std::string split1 = "|";
	std::string split2 = "_";
	std::vector<std::string> suitItem;
	Avalon::StringUtil::Split(entry->limitedSuit, suitItem, split1);
	std::vector<UInt32> itemIDVec;
	for (UInt32 i = 0; i < suitItem.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(suitItem[i], strTmp, split2);

		MallFashionLimitedSuitKey key;
		UInt32 suitID = 0;
		if (strTmp.size() == 2)
		{
			suitID = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			key.itemID = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			key.occu = entry->occu;
		}
		else
		{
			return false;
		}
		m_Data.insert(std::make_pair(key, suitID));
		itemIDVec.push_back(key.itemID);
	}
	m_OccuSuit.insert(std::make_pair(entry->occu, itemIDVec));
// __CUSTOM_ADDENTRY_END__
	return true;
}

UInt32 MallFashionLimitedSuitDataEntryMgr::GetSuitID(UInt32 occu, UInt32 itemID)
{
	auto data = m_Data.find({ occu,itemID });
	if (data != m_Data.cend())
	{
		return data->second;
	}
	else
	{
		return 0;
	}
}

std::vector<UInt32> MallFashionLimitedSuitDataEntryMgr::GetAllSuitItemID(UInt32 occu)
{
	auto data = m_OccuSuit.find(occu);
	if (data != m_OccuSuit.cend())
	{
		return data->second;
	}
	return std::vector<UInt32>();
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

