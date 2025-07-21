#include "CLTeamDungeonDataEntry.h"

bool TeamDungeonDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = (TeamDungeonEntryType)row.ReadUInt32();
	dungeonId = row.ReadUInt32();
	matchType = row.ReadUInt8();
	minLevel = row.ReadUInt16();
	minPlayerNum = row.ReadUInt8();
	maxPlayerNum = row.ReadUInt8();

	auto temp = row.ReadUInt8();
	autoKick = temp > 0 ? true : false;
	return true;
}

TeamDungeonDataEntryMgr::TeamDungeonDataEntryMgr()
{
	m_CityMonsterTeamTargetId = 0;
}

bool TeamDungeonDataEntryMgr::AddEntry(TeamDungeonDataEntry* entry)
{
	if (!DataEntryMgrBase::AddEntry(entry))
	{
		return false;
	}

	if (entry->type == TEAM_CITY_MONSTER)
	{
		m_CityMonsterTeamTargetId = entry->id;
	}

	return true;
}

TeamDungeonDataEntry* TeamDungeonDataEntryMgr::FindEntryByDungeonID(UInt32 dungeonId)
{
	class TeamDungeonDataEntryVistor : public Avalon::DataEntryVisitor<TeamDungeonDataEntry>
	{
	public:
		bool operator()(TeamDungeonDataEntry *dataEntry)
		{
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<TeamDungeonDataEntry*> datas;
	};
	TeamDungeonDataEntryVistor vistor;
	this->Visit(vistor);

	for (auto data : vistor.datas)
	{
		if (data && data->dungeonId == dungeonId)
		{
			return data;
		}
	}

	return NULL;
}