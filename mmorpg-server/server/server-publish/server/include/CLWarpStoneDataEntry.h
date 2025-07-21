#ifndef _CL_WARP_STONE_DATAENTRY_H_
#define _CL_WARP_STONE_DATAENTRY_H_

#include "CLDefine.h"
#include "CLWarpStoneDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 能量石配置表管理器
*/
struct EnergyStoneDataEntry : public Avalon::DataEntry
{
public:
	EnergyStoneDataEntry();
	~EnergyStoneDataEntry();
public:
	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

public:
	//能量石ID
	UInt32	id;
	//物品ID
	UInt32	itemId;
	//能量值
	UInt32	energyNum;
	//能量石类型
	WarpStoneType	type;
};

/**
*@brief 能量石配置表管理器
*/
typedef Avalon::DataEntryMgr<EnergyStoneDataEntry> EnergyStoneDataEntryMgr;

/**
*@brief 次元石成长配置表
*/
struct WarpStoneLevelDataEntry : public Avalon::DataEntry
{
public:
	WarpStoneLevelDataEntry();
	~WarpStoneLevelDataEntry();
public:
	UInt32 GetKey() const { return level; }
	bool Read(Avalon::DataRow& row);

public:
	//等级
	UInt32	level;
	//能量
	UInt32	energy;
};

/**
*@brief 次元石成长配置表管理器
*/
class WarpStoneLevelDataEntryMgr : public Avalon::DataEntryMgrBase<WarpStoneLevelDataEntry>, public Avalon::Singleton<WarpStoneLevelDataEntryMgr>
{
public:
	typedef std::map<UInt32, UInt32> SumEnergyMap;

	WarpStoneLevelDataEntryMgr();
	~WarpStoneLevelDataEntryMgr();

public:
	bool AddEntry(WarpStoneLevelDataEntry* entry);
	
	UInt32 GetSumEnergy(UInt32 level);

private:
	SumEnergyMap	m_SumEnergyMap;
};


/**
*@brief 次元石配置表
*/
struct WarpStoneDataEntry : public Avalon::DataEntry
{
public:
	WarpStoneDataEntry();
	~WarpStoneDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32	id;

	//名称
	std::string name;

	//次元石类型
	WarpStoneType	type;
	//解锁等级
	UInt16	level;
	//解锁消耗
	UInt32	gold;
	//是否自动解锁
	bool	isUnlock;

};


/**
*@brief 次元石配置表管理器
*/
class WarpStoneDataEntryMgr : public Avalon::DataEntryMgrBase<WarpStoneDataEntry>, public Avalon::Singleton<WarpStoneDataEntryMgr>
{
public:

	typedef std::vector<WarpStoneDataEntry*> WarpStoneDataVec;
	typedef std::map<UInt16, WarpStoneDataVec> WarpStoneDataMap;

	WarpStoneDataEntryMgr();
	~WarpStoneDataEntryMgr();

public:
	bool AddEntry(WarpStoneDataEntry* entry);

	/**
	*@brief 根据等级获取次元石
	*/
	const WarpStoneDataVec& GetLvlWarpStones(UInt16 level) const;

private:
	WarpStoneDataMap m_LvlWarpStones;

};


#endif

