/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLExpeditionMapDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ExpeditionMapDataEntry::ExpeditionMapDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ExpeditionMapDataEntry::~ExpeditionMapDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ExpeditionMapDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ExpeditionMapDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	mapName = row.ReadString();
	playerLevelLimit = row.ReadUInt32();
	adventureTeamLevelLimit = row.ReadUInt32();
	rolesCapacity = row.ReadUInt32();
	expeditionTime = row.ReadString();

// __CUSTOM_READ_BEGIN__
	std::vector<std::string> tmpVec;
	Avalon::StringUtil::Split(expeditionTime, tmpVec, "|");
	if (tmpVec.empty())
	{
		ErrorLogStream << "expedition time list is empty!" << LogStream::eos;
		return false;
	}
	for (const auto& elem : tmpVec)
	{
		expeditionDurationList.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
bool ExpeditionMapDataEntry::CheckPlayerLevelLimit(UInt16 level)
{
	if (level == 0) return true;
	// 不限制
	if (playerLevelLimit == 0) return false;
	return level < playerLevelLimit;
}

bool ExpeditionMapDataEntry::CheckAdventureTeamLevelLimit(UInt16 level)
{
	if (level == 0) return true;
	// 不限制
	if (adventureTeamLevelLimit == 0) return false;
	return level < adventureTeamLevelLimit;
}

bool ExpeditionMapDataEntry::CheckExpiditonMemberValid(UInt32 members)
{
	if (members == 0) return false;
	// 不限制
	if (rolesCapacity == 0) return true;
	return members <= rolesCapacity;
}

bool ExpeditionMapDataEntry::CheckExpeditionDurationValid(UInt32 duration)
{
	return (std::find(expeditionDurationList.begin(), expeditionDurationList.end(), duration) != expeditionDurationList.end());
}
// __CUSTOM_ENTRYFUNCTION_END__

bool ExpeditionMapDataEntryMgr::AddEntry(ExpeditionMapDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ExpeditionMapDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

