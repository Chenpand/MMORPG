/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLAdventureTeamDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

AdventureTeamDataEntry::AdventureTeamDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

AdventureTeamDataEntry::~AdventureTeamDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 AdventureTeamDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool AdventureTeamDataEntry::Read(Avalon::DataRow &row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "column(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();
	exp = row.ReadString();
	clearDungeonExpAddition = row.ReadUInt32();
	expSource = row.ReadString();

// __CUSTOM_READ_BEGIN__
	levelUpExp = Avalon::StringUtil::ConvertToValue<UInt64>(exp);
	if (levelUpExp == 0)
	{
		ErrorLogStream << "Level up exp is zero!" << LogStream::eos;
		return false;
	}

	{
		std::vector<std::string> tmpVec;
		Avalon::StringUtil::Split(expSource, tmpVec, "|");
		for (const auto& elem : tmpVec)
		{
			levelUpExpSourceVec.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool AdventureTeamDataEntryMgr::AddEntry(AdventureTeamDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventureTeamDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	if (m_MaxLevel < entry->id)
	{
		m_MaxLevel = entry->id;
	}
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

