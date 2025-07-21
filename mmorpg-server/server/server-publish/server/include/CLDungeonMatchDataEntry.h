#ifndef _CL_DUNGEON_MATCH_DATAENTRY_H_
#define _CL_DUNGEON_MATCH_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ���³�ƥ��������
*/
class DungeonMatchDataEntry : public Avalon::DataEntry
{
public:
	DungeonMatchDataEntry() {}
	~DungeonMatchDataEntry() {}

public:
	UInt32 GetKey() const{ return dungeonId; }

	bool Read(Avalon::DataRow& row);

public:
	// ���³�ID
	UInt32		dungeonId;
	// ƥ��Ŀ������
	UInt32		fullPlayerNum;
};

typedef Avalon::DataEntryMgr<DungeonMatchDataEntry> DungeonMatchDataEntryMgr;

#endif
