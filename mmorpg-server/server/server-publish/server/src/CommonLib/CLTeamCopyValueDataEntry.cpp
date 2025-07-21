/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyValueDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

TeamCopyValueDataEntry::TeamCopyValueDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyValueDataEntry::~TeamCopyValueDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyValueDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyValueDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	value = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyValueDataEntryMgr::AddEntry(TeamCopyValueDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyValueDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	m_ValueMap[entry->id] = entry->value;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

void TeamCopyValueDataEntryMgr::OnReload()
{
	class TeamCopyValueEntryVisitor :public Avalon::DataEntryVisitor<TeamCopyValueDataEntry>
	{
	public:
		TeamCopyValueEntryVisitor() {}

		bool operator()(TeamCopyValueDataEntry* entry)
		{
			valueMap[entry->id] = entry->value;
			return true;
		}

	public:
		std::map<UInt32, UInt32>& GetValueMap() { return valueMap; }

	private:
		std::map<UInt32, UInt32> valueMap;
	};

	TeamCopyValueEntryVisitor visitor;
	TeamCopyValueDataEntryMgr::Instance()->Visit(visitor);

	m_ValueMap.clear();
	m_ValueMap = visitor.GetValueMap();
}

UInt32 TeamCopyValueDataEntryMgr::GetTeamCopyValue(UInt32 type)
{
	auto iter = m_ValueMap.find(type);
	return iter != m_ValueMap.end() ? iter->second : 0;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

