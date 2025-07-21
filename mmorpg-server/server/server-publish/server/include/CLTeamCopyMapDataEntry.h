#ifndef __CL_TEAMCOPYVALUE_MAP_ENTRY_H__
#define __CL_TEAMCOPYVALUE_MAP_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// �ű�����
class TeamCopyGridDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyGridDataEntry();
	virtual ~TeamCopyGridDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

public:
	// ������Դid
	UInt32											sourceId;
	// ����
	UInt32		                                    gridType;
	// ��ͨ(�赲)
	UInt32											IsLink;
};

class TeamCopyGridDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyGridDataEntry>
	, public Avalon::Singleton<TeamCopyGridDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyGridDataEntry* entry);
};


// �ű���ͼ
class TeamCopyMapDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyMapDataEntry();
	virtual ~TeamCopyMapDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

public:
	// ����ΨһID
	UInt32                                          gridId;
	// ������Դid
	UInt32											sourceId;
	// ����X
	Int32                                           x;
	// ����Y
	Int32											y;
	// �ݵ�id
	UInt32											fieldId;
};

class TeamCopyMapDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyMapDataEntry>
	, public Avalon::Singleton<TeamCopyMapDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyMapDataEntry* entry);
};

// ج���Ѷȵ�ͼ
class TeamCopyMapDataEntryMgr2 : public Avalon::DataEntryMgrBase<TeamCopyMapDataEntry>
	, public Avalon::Singleton<TeamCopyMapDataEntryMgr2>
{
public:
	virtual bool AddEntry(TeamCopyMapDataEntry* entry);
};

#endif