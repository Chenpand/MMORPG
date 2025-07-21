#ifndef __CL_DUNGEON_PREFIX_LIMIT_ENTRY_H__
#define __CL_DUNGEON_PREFIX_LIMIT_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 地下城词缀数量数据项
*/
class DungeonPrefixLimitDataEntry : public Avalon::DataEntry
{
public:
    DungeonPrefixLimitDataEntry() {}
    ~DungeonPrefixLimitDataEntry() {}

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // 最低等级
    UInt16                  minLevel;
    // 最低等级
    UInt16                  maxLevel;
    // 难度
    UInt8                   hardType;
    // 词缀怪总数
    UInt32                  prefixMonsterTotalNum;
    // 高级词缀数量
    UInt32                  seniorPrefixNum;
    // 低级词缀数量
    UInt32                  lowPrefixNum;
};

typedef Avalon::DataEntryMgr<DungeonPrefixLimitDataEntry> DungeonPrefixLimitDataMgr;

#endif