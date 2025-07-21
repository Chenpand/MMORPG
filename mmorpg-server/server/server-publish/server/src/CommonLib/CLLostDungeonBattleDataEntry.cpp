/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLLostDungeonBattleDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLLostDungeonDefine.h>
// __CUSTOM_HEADER_END__

LostDungeonBattleDataEntry::LostDungeonBattleDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

LostDungeonBattleDataEntry::~LostDungeonBattleDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 LostDungeonBattleDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool LostDungeonBattleDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	commonTask = row.ReadString();
	diffTask = row.ReadString();
	maxPlayerNum = row.ReadUInt32();
	mapId = row.ReadUInt32();
	passExitDoorId = row.ReadUInt32();
// __CUSTOM_READ_BEGIN__

	if (!ParseTask(commonTask, commonTaskMap))
	{
		return false;
	}

	if (!ParseTask(diffTask, diffTaskMap))
	{
		return false;
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__

bool LostDungeonBattleDataEntry::ParseTask(const std::string& taskStr, std::map<UInt32, UInt32>& taskMap)
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(taskStr, param_1, "|");
	for (auto& iter : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");
		if (param_2.size() != 2)
		{
			ErrorLogStream << "common task error! id:" << id << LogStream::eos;
			return false;
		}

		UInt32 taskId = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		UInt32 weight = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
		taskMap[taskId] = weight;
	}

	return true;
}

std::map<UInt32, UInt32>& LostDungeonBattleDataEntry::GetTaskMap(UInt32 grade)
{
	static std::map<UInt32, UInt32> temp;
	switch (grade)
	{
		case LDET_NORMAL_BATTLE:
			return commonTaskMap;
		case LDET_HARD_BATTLE:
			return diffTaskMap;
	default:
		break;
	}

	return temp;
}

// __CUSTOM_ENTRYFUNCTION_END__

bool LostDungeonBattleDataEntryMgr::AddEntry(LostDungeonBattleDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<LostDungeonBattleDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

