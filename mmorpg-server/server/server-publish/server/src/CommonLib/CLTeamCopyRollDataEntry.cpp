/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyRollDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

TeamCopyRollDataEntry::TeamCopyRollDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyRollDataEntry::~TeamCopyRollDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyRollDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyRollDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	teamType = row.ReadUInt32();
	teamGrade = row.ReadUInt32();
	rewardNum = row.ReadUInt32();
	triggerRate = row.ReadUInt32();
	dropId = row.ReadUInt32();
	convertItem = row.ReadString();

// __CUSTOM_READ_BEGIN__

	{
		std::vector<std::string> rewards;
		Avalon::StringUtil::Split(convertItem, rewards, "|");
		for (auto& it : rewards)
		{
			std::vector<std::string> items;
			Avalon::StringUtil::Split(it, items, "_");
			if (items.size() != 2)
			{
				ErrorLogStream << "roll convert item format error!" << convertItem << LogStream::eos;
				return false;
			}

			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(items[0]);
			UInt32 itemNum = Avalon::StringUtil::ConvertToValue<UInt32>(items[1]);
			convertItemVec.push_back({ itemId, itemNum });
		}
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyRollDataEntryMgr::AddEntry(TeamCopyRollDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyRollDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	m_RollMap[{entry->teamType, entry->teamGrade}] = entry;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

TeamCopyRollDataEntryMgr::TeamCopyRollDataEntryMgr()
{
	for (UInt32 i = 1; i <= 100; ++i)
	{
		m_RollPointVec.push_back(i);
	}
}

TeamCopyRollDataEntry* TeamCopyRollDataEntryMgr::FindEntry(UInt32 teamType, UInt32 teamGrade)
{
	auto iter = m_RollMap.find({ teamType, teamGrade });
	return iter != m_RollMap.end() ? iter->second : NULL;
}

UInt32 TeamCopyRollDataEntryMgr::GetRollRewardNum(UInt32 teamType, UInt32 teamGrade)
{
	TeamCopyRollDataEntry* dataEntry = FindEntry(teamType, teamGrade);
	return dataEntry != NULL ? dataEntry->rewardNum : 0;
}

void TeamCopyRollDataEntryMgr::OnReload()
{
	class TeamCopyRollEntryVisitor :public Avalon::DataEntryVisitor<TeamCopyRollDataEntry>
	{
	public:
		TeamCopyRollEntryVisitor() {}

		bool operator()(TeamCopyRollDataEntry* entry)
		{
			valueMap[{entry->teamType, entry->teamGrade}] = entry;
			return true;
		}

		std::map<TeamTypeGrade, TeamCopyRollDataEntry*>& GetValueMap() { return valueMap; }

	private:
		std::map<TeamTypeGrade, TeamCopyRollDataEntry*> valueMap;
	};

	TeamCopyRollEntryVisitor visitor;
	TeamCopyRollDataEntryMgr::Instance()->Visit(visitor);

	m_RollMap.clear();
	m_RollMap = visitor.GetValueMap();
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

