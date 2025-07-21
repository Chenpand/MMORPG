#include "CLMallPersonalTailorDataEntry.h"


MallPersonalTailorDataEntry::MallPersonalTailorDataEntry()
{

}

MallPersonalTailorDataEntry::~MallPersonalTailorDataEntry()
{

}

bool MallPersonalTailorDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	activateCond = row.ReadUInt8();
	activateSubCond = row.ReadUInt8();

	std::vector<std::string> strs;
	std::string str = row.ReadString();
	Avalon::StringUtil::Split(str, strs, "|");
	for (auto param : strs)
	{
		UInt32 elem = Avalon::StringUtil::ConvertToValue<UInt32>(param);
		mallGoodsIds.push_back(elem);
	}

	activateSubType = row.ReadUInt8();
	playerLevelLimit = row.ReadUInt16();
	limitInterval = row.ReadUInt16();
	roleActivateTimesLimit = row.ReadUInt16();
	
	return true;
}

MallPersonalTailorDataEntryMgr::MallPersonalTailorDataEntryMgr()
{

}

MallPersonalTailorDataEntryMgr::~MallPersonalTailorDataEntryMgr()
{

}

bool MallPersonalTailorDataEntryMgr::AddEntry(MallPersonalTailorDataEntry* entry)
{
	if (entry->activateCond == 0)
	{
		ErrorLogStream << "PersonalTailorTriggerTable's format(activateCond: 0) is error!";
		return false;
	}

	if (!Avalon::DataEntryMgrBase<MallPersonalTailorDataEntry>::AddEntry(entry)) return false;

	return true;
}