#ifndef __CL_MONSTER_DATA_ENTRY_H__
#define __CL_MONSTER_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum MonsterType
{
    MT_HERO,
    MT_MONSTER,
    MT_ELITE,
    MT_BOSS,
    MT_NPC,
    MT_HELL,
    MT_ACTIVITY,
	// 彩蛋
	MT_EASTER_EGG = 9,

	// 这个不会在表格中，服务器自己用
	// 可破坏物
	MT_BOX = 9999,
	
};

/**
*@brief 怪物数据项
*/
class MonsterDataEntry : public Avalon::DataEntry
{
public:
    MonsterDataEntry();
    ~MonsterDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

    static UInt32 GetMonsterTypID(UInt32 id) { return id / 10000; }

public:
    // id
    UInt32	                id;
	// 怪物模式
	UInt8					mode;
	// type
    MonsterType				type;
    // 经验
    UInt32                  exp;
    // 掉落ID
    std::vector<UInt32>     dropSetIds;
	// 每日最多掉落次数
	UInt32					dailyMaxDropNum;
};

typedef Avalon::DataEntryMgr<MonsterDataEntry> MonsterDataEntryMgr;

#endif