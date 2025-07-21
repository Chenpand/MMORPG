#include "CLMallGiftPackDataEntry.h"


MallGiftPackDataEntry::MallGiftPackDataEntry()
{
	giftPackId = 0;
	activateCond = 0;
}

MallGiftPackDataEntry::~MallGiftPackDataEntry()
{

}

bool MallGiftPackDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	giftPackId = row.ReadUInt32();
	activateCond = row.ReadUInt8();
	limitInterval = row.ReadInt32();

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, "|");
		for (const auto& elem : strVec)
		{
			params.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	return true;
}

MallGiftPackDataEntryMgr::MallGiftPackDataEntryMgr()
{

}

MallGiftPackDataEntryMgr::~MallGiftPackDataEntryMgr()
{

}

bool MallGiftPackDataEntryMgr::AddEntry(MallGiftPackDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<MallGiftPackDataEntry>::AddEntry(dataEntry)) return false;
	
	m_ActivateCondGiftsMap[dataEntry->activateCond].push_back(dataEntry->giftPackId);
	return true;
}

void MallGiftPackDataEntryMgr::GetGiftsByCondition(MallGiftPackActivateCond cond, std::vector<UInt32>& gifts)
{
	auto iter = m_ActivateCondGiftsMap.find((UInt8)cond);
	if (iter != m_ActivateCondGiftsMap.end())
	{
		gifts = iter->second;
	}
}