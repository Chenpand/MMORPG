#pragma once
#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief	 地牢宝箱表
*/

//礼包随机权重
struct GiftWeight
{
	UInt32	dataId;		//礼包id
	UInt32	weight;		//权重
};

class LostDungeonTreasureDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonTreasureDataEntry();
	~LostDungeonTreasureDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	//唯一id
	UInt32		id;
	//类型
	UInt8		type;
	//参数
	std::vector<GiftWeight>  gifts;
};

class LostDungeonTreasureDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonTreasureDataEntry>
	, public Avalon::Singleton<LostDungeonTreasureDataEntryMgr>
{
public:
	LostDungeonTreasureDataEntryMgr();
	~LostDungeonTreasureDataEntryMgr();
};