#ifndef __CL_DUNGEON_CHEST_DATA_ENTRY_H__
#define __CL_DUNGEON_CHEST_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLDropItemDefine.h>

/**
*@brief 结算宝箱数据项
*/
struct DungeonChestDataEntry : public Avalon::DataEntry
{
public:
    DungeonChestDataEntry();
    ~DungeonChestDataEntry();

public:
    UInt32 GetKey() const { return dungeonId; }

    bool Read(Avalon::DataRow& row);


public:
    // 地下城ID
    UInt32      dungeonId;
	// 评分
	UInt32		score;
    // 金币方案ID
    UInt32      goldDropSetId;
    // 各宝箱方案ID
    UInt32      chestDropoSetIds[DCT_NUM];
	// 付费宝箱消耗货币类型
	UInt32		payChestCostItemId;
    // 付费宝箱消耗货币数量
    UInt32      payChestCost;
};




class DungeonChestDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonChestDataEntry>
	, public Avalon::Singleton<DungeonChestDataEntryMgr>
{
public:
	DungeonChestDataEntry* FindData(UInt32 dungeonId, UInt32 score);
	virtual bool AddEntry(DungeonChestDataEntry* entry);
	virtual ~DungeonChestDataEntryMgr();
private:
	std::map<UInt64, DungeonChestDataEntry*> m_Datas;

};

#endif