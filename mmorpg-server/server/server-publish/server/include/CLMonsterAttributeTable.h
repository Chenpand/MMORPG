#ifndef __CL_MONSTER_ATTRIBUTE_TABLE_H__
#define __CL_MONSTER_ATTRIBUTE_TABLE_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLMonsterDataEntry.h"
#include "CLDungeonDataEntry.h"

/**
*@brief 怪物数据项
*/
class MonsterAttributeDataEntry : public Avalon::DataEntry
{
public:
    MonsterAttributeDataEntry() {}
    ~MonsterAttributeDataEntry() {}

public:
	UInt32 GetKey() const { return MakeKey(difficulty, type, mode, level); }

	static UInt32 MakeKey(UInt32 diff, UInt32 type, UInt32 mode, UInt32 level)
	{
		return diff * 10000000 + type * 100000 + mode * 1000 + level;
	}

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // 副本难度(与DungeonHardType就差1)
    UInt8                   difficulty;
    // 怪物类型
    MonsterType				type;
	// 怪物模式
	UInt8					mode;
    // 等级
    UInt16                  level;
    // 经验
    UInt32                  exp;
    // 掉落ID
    std::vector<UInt32>     dropSetIds;
};

typedef Avalon::DataEntryMgr<MonsterAttributeDataEntry> MonsterAttributeDataEntryMgr;

#endif