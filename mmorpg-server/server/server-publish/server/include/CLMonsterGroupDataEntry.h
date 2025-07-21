#ifndef __CL_MONSTER_GROUP_DATA_ENTRY_H__
#define __CL_MONSTER_GROUP_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 怪物组有损类型
enum MonsterGroupLossyType
{
    // 无
    MGLT_NULL,
    // 房间内有损
    MGLT_AREA,
    // 全局有损
    MGLT_GLOBAL,
};

/**
*@brief 刷怪表
*/
class MonsterGroupDataEntry : public Avalon::DataEntry
{
public:
    MonsterGroupDataEntry() {}
    ~MonsterGroupDataEntry() {}

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id，这是个递增的ID，自动生成
    UInt32                  id;
    // 方案ID
    UInt32                  projID;
    // 组ID
    UInt32                  groupID;
	// 组概率
	UInt32					groupProb;
    // 有损类型
    MonsterGroupLossyType   lossyType;
    // 有损数量
    UInt8                   lossyNum;
    // 概率
    UInt32                  prob;
    // 怪物列表
    std::vector<UInt32>     monsterList;
    
};

typedef Avalon::DataEntryMgr<MonsterGroupDataEntry> MonsterGroupDataEntryMgr;

#endif