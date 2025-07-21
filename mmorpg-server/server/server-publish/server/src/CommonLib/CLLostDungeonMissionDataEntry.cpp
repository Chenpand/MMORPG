/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLLostDungeonMissionDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

LostDungeonMissionDataEntry::LostDungeonMissionDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

LostDungeonMissionDataEntry::~LostDungeonMissionDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 LostDungeonMissionDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool LostDungeonMissionDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	value = row.ReadString();
	acceptLimit = row.ReadUInt32();
	frontCond1 = row.ReadUInt32();
	score = row.ReadUInt32();
	grade = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__

	if (type == LDTT_TASK_CLEAR_FIX_DUNGEON || type == LDTT_TASK_KILL_FIX_MONSTER || type == LDTT_TASK_COLLECT_ITEM)
	{
		std::vector<std::string> param_1;
		Avalon::StringUtil::Split(value, param_1, "|");
		for (auto& it : param_1)
		{
			std::vector<std::string> param_2;
			Avalon::StringUtil::Split(it, param_2, ",");
			if (param_2.size() != 2)
			{
				ErrorLogStream << "lost dungeon task error! id:" << id << LogStream::eos;
				return false;
			}

			UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
			UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
			valMap[id] = num;
		}
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool LostDungeonMissionDataEntryMgr::AddEntry(LostDungeonMissionDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<LostDungeonMissionDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

