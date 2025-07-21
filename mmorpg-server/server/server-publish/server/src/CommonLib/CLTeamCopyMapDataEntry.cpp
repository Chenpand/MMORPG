#include "CLTeamCopyMapDataEntry.h"


TeamCopyGridDataEntry::TeamCopyGridDataEntry()
{

}

TeamCopyGridDataEntry::~TeamCopyGridDataEntry()
{

}

UInt32 TeamCopyGridDataEntry::GetKey() const
{
	return sourceId;
}

bool TeamCopyGridDataEntry::Read(Avalon::DataRow &row)
{
	sourceId = row.ReadUInt32();
	gridType = row.ReadUInt32();
	IsLink = row.ReadUInt32();
	return true;
}

bool TeamCopyGridDataEntryMgr::AddEntry(TeamCopyGridDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyGridDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}


TeamCopyMapDataEntry::TeamCopyMapDataEntry()
{
}

TeamCopyMapDataEntry::~TeamCopyMapDataEntry()
{
}

UInt32 TeamCopyMapDataEntry::GetKey() const
{
	return gridId;
}

bool TeamCopyMapDataEntry::Read(Avalon::DataRow &row)
{
	gridId = row.ReadUInt32();
	sourceId = row.ReadUInt32();
	x = row.ReadInt32();
	y = row.ReadInt32();
	fieldId = row.ReadUInt32();
	return true;
}

bool TeamCopyMapDataEntryMgr::AddEntry(TeamCopyMapDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyMapDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

bool TeamCopyMapDataEntryMgr2::AddEntry(TeamCopyMapDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyMapDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

