#ifndef __CL_DUNGEON_HELL_DATA_ENTRY_H__
#define __CL_DUNGEON_HELL_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDungeonDefine.h"

class DungeonHellDataEntry : public Avalon::DataEntry
{
public:
    DungeonHellDataEntry();
    virtual ~DungeonHellDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

private:

public:
    // id
    UInt32	                id;
	// 关卡等级
	UInt16					dungeonLevel;
    // 模式
    DungeonHellMode         mode;
    // 刷怪组
    std::vector<UInt32>     monsterGroups;
	// 概率
	UInt32					prob;
	// 要随机的地下城ID列表
	std::vector<UInt32>		dungeonIDs;
	// 是否先随机地下城ID
	UInt32					choiceType;
};

class DungeonHellDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonHellDataEntry>
	, public Avalon::Singleton<DungeonHellDataEntryMgr>
{
public:
	// 加载数据
	virtual bool AddEntry(DungeonHellDataEntry* entry);

	// 随机出地下城ID
	UInt32 RandDungeonID(UInt16 dungeonLevel, DungeonHardType dungeonHard);
};

#endif