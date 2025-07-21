#ifndef _CL_DUNGEON_MATCH_DATAENTRY_H_
#define _CL_DUNGEON_MATCH_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 地下城匹配数据项
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
	// 地下城ID
	UInt32		dungeonId;
	// 匹配目标人数
	UInt32		fullPlayerNum;
};

typedef Avalon::DataEntryMgr<DungeonMatchDataEntry> DungeonMatchDataEntryMgr;

#endif
