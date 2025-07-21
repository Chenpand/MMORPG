#ifndef __CL_TEAMCOPYVALUE_MAP_ENTRY_H__
#define __CL_TEAMCOPYVALUE_MAP_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 团本格子
class TeamCopyGridDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyGridDataEntry();
	virtual ~TeamCopyGridDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

public:
	// 格子资源id
	UInt32											sourceId;
	// 类型
	UInt32		                                    gridType;
	// 联通(阻挡)
	UInt32											IsLink;
};

class TeamCopyGridDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyGridDataEntry>
	, public Avalon::Singleton<TeamCopyGridDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyGridDataEntry* entry);
};


// 团本地图
class TeamCopyMapDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyMapDataEntry();
	virtual ~TeamCopyMapDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

public:
	// 格子唯一ID
	UInt32                                          gridId;
	// 格子资源id
	UInt32											sourceId;
	// 坐标X
	Int32                                           x;
	// 坐标Y
	Int32											y;
	// 据点id
	UInt32											fieldId;
};

class TeamCopyMapDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyMapDataEntry>
	, public Avalon::Singleton<TeamCopyMapDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyMapDataEntry* entry);
};

// 噩梦难度地图
class TeamCopyMapDataEntryMgr2 : public Avalon::DataEntryMgrBase<TeamCopyMapDataEntry>
	, public Avalon::Singleton<TeamCopyMapDataEntryMgr2>
{
public:
	virtual bool AddEntry(TeamCopyMapDataEntry* entry);
};

#endif