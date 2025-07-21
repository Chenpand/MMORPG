#ifndef _CL_MONSTER_PREFIX_DATAENTRY_H_
#define _CL_MONSTER_PREFIX_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum MonsterPrefixType
{
    // 低级
    MPT_LOW,
    // 高级
    MPT_SENIOR,
};

/**
*@brief 活跃度事件数据项
*/
class MonsterPrefixDataEntry : public Avalon::DataEntry
{
public:
    MonsterPrefixDataEntry();
    ~MonsterPrefixDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt8	id;
    // 词缀类型
    UInt8   type;
    // BufferID
    UInt32  bufferId;
};

typedef Avalon::DataEntryMgr<MonsterPrefixDataEntry> MonsterPrefixDataEntryMgr;

#endif
