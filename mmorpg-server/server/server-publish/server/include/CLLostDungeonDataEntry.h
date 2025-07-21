#ifndef __LOST_DUNGEON_DATAENTRY_H__
#define __LOST_DUNGEON_DATAENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//楼层下属战场权重
struct FloorBattleWeight
{
	UInt32	battleDataId;	//战场dataid
	UInt32	weight;		//权重
};

//困难关卡生成信息
struct HardBattleGenInfo
{
	UInt8	num;			//数量
	UInt32	pro;			//概率
	UInt8	groupId;		//组id
	UInt8	groupMaxNum;	//组数量上限
};

//宝箱生成信息
struct TreasChestGenInfo
{
	UInt8   num;			//数量
	UInt32	pro;			//概率
	UInt8	groupId;		//组id
	UInt8	groupMaxNum;	//组数量上限
};

//宝箱随机权重
struct TreasChestWeight
{
	UInt32	dataId;		//宝箱id
	UInt32	weight;		//权重
};

/**
*@breif	 爬塔楼层数据项
*/
class LostDungeonFloorDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonFloorDataEntry();
	~LostDungeonFloorDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
public:
	//唯一id
	UInt32		id;
	//楼层
	UInt32		floor;
	//楼层下属战场
	std::vector<FloorBattleWeight>	battlesWt;
	//楼层挑战失败处理
	UInt8		failHandlerId;
	//失败时额外扣除积分
	UInt32		failDecScore;
	//是否可能存在神秘商店
	UInt8		existMysticShop;
	//节点数量
	UInt8		nodeNum;
	//困难关卡生成信息
	HardBattleGenInfo	hardBattleGenInfo;
	//宝箱生成信息
	TreasChestGenInfo	treasChestGenInfo;
	//宝箱随机概率
	std::vector<TreasChestWeight>	treasChestWt;
	//不会掉层和扣除积分的道具
	UInt32	 notFallItemId;
	//可重新挑战一次本层的道具
	UInt32	againItemId;
};

class LostDungeonFloorDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonFloorDataEntry>
	, public Avalon::Singleton<LostDungeonFloorDataEntryMgr>
{
public:
	LostDungeonFloorDataEntryMgr();
	~LostDungeonFloorDataEntryMgr();

	bool AddEntry(LostDungeonFloorDataEntry* dataEntry);
	UInt32  GetMaxFloor();
	LostDungeonFloorDataEntry* GetDataEntry(UInt32 floor);
public:
	UInt32 m_maxFloor;
	std::map<UInt32, LostDungeonFloorDataEntry*> m_datas;
};


/**
*@breif	 爬塔交互元素数据项
*/
class LostDungeonElementDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonElementDataEntry();
	~LostDungeonElementDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
public:
	//唯一id
	UInt32		id;
	//类型
	UInt8		type;
	//参数
	std::string param;
};

class LostDungeonElementDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonElementDataEntry>
	, public Avalon::Singleton<LostDungeonElementDataEntryMgr>
{
public:
	LostDungeonElementDataEntryMgr();
	~LostDungeonElementDataEntryMgr();
public:
	bool AddEntry(LostDungeonElementDataEntry* dataEntry);
	UInt32 GetElementId(UInt8 elemetType);
public:
	std::map<UInt32, UInt32> m_typeMapId;
};

#endif