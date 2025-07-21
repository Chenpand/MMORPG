#ifndef _SHOP_H_
#define _SHOP_H_

#include "ShopItem.h"

class Player;

enum ShopSyncDBState
{
	SSDS_NONE = 0,				
	SSDS_INSERT,				//插入
	SSDS_UPDATE,				//更新
};

class Shop : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Shop, Avalon::NullMutex)
public:
	Shop();
	~Shop();

public:
	static Shop* Create(UInt32 shopId);
	void SetDataEntry(ShopDataEntry* dataEntry){ 
		m_pDataEntry = dataEntry; 
		m_shopid = dataEntry->shopId;
	}
	ShopDataEntry* GetDataEntry() const { return m_pDataEntry; }

	
public:	
	inline UInt32 GetShopId() { return m_shopid; }

	inline UInt32 GetRefreshNum() { return m_refreshNum; }
	inline void SetRefreshNum(UInt32 value) { m_refreshNum = value; }
	inline ShopSyncDBState GetSyncDBState() { return m_syncDBState; }
	inline void SetSyncDBState(ShopSyncDBState state) { m_syncDBState = state; }
	inline UInt8 GetItemNum() { return (UInt8)m_shopItems.size(); }
	inline void SetOwner(ObjID_t owner) { m_owner = owner; }
	inline void SetVersion(UInt32 ver)	{ m_version = ver; }
	inline UInt32 GetVersion()	{ return m_version; }

	void AddShopItem(UInt32 itemId);
	void AddShopItem(ShopItem* item);

	void GetShopItems(std::vector<ShopItem*>& items) { items = m_shopItems; }

	void ClearItem();
	void ClearAllItems();

	ShopItem* GetShopItem(UInt32 shopItemId);

	void SetDungeonId(UInt32 dungeonId) { m_DungeonId = dungeonId; }
	UInt32 GetDungeonId() const { return m_DungeonId; }

public:// 商店周期刷新
	/**
	*@brief 清空未刷新的商品数量
	*/
	void ClearUnRefreshShopItem();

	/**
	*@brief 保存商店中未刷新的商品数量
	*/
	void KeepUnRefreshShopItemRestNum(UInt32 shopItemId, Int16 shopItemRestNum);

	/**
	*@brief 商店是否有未刷新的商品
	*/
	bool HasUnRefreshShopItem();

	/**
	*@brief 设置未刷新商品剩余数量
	*/
	void SetUnRefreshShopItemRestNum();

	/**
	*@brief 设置获取刷新周期的刷新状态
	*/
	void SetRefreshCycleTypeState(UInt32 refreshCycleType, UInt32 refreshCycleState);
	UInt32 GetRefreshCycleState(UInt32 refreshCycleType);

	/**
	*@brief 设置刷新时间
	*/
	void SetRefreshTime();

	/**
	*@brief 获取天刷新周期时间戳
	*/
	Avalon::Time GetRefreshTime() { return Avalon::Time(m_refreshTime); }

	/**
	*@brief 获取周刷新周期时间戳
	*/
	Avalon::Time GetWeekRefreshTime() { return Avalon::Time(m_weekRefreshTime); }

	/**
	*@brief 获取月刷新周期时间戳
	*/
	Avalon::Time GetMonthRefreshTime() { return Avalon::Time(m_monthRefreshTime); }

private:
	/**
	*@brief 判断刷新方式状态是否是激活状态
	*/
	bool _CheckRefreshCycleStateActivated(RefreshCycleType refreshCycleType);

	/**
	*@brief 计算下一次刷新时间戳(毫秒级)
	*/
	UInt64 CalculateNextRefreshTimestamp(RefreshCycleType refreshCycleType, UInt32 refreshHour);

public:	//编解码相关
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner", m_owner)
	CL_DBPROPERTY("shopid", m_shopid)
	CL_DBPROPERTY("refreshtime", m_refreshTime)
	CL_DBPROPERTY("week_refreshtime", m_weekRefreshTime)
	CL_DBPROPERTY("month_refreshtime", m_monthRefreshTime)
	CL_DBPROPERTY("refreshnum", m_refreshNum)
	CL_DBPROPERTY("version", m_version)

	CL_DBPROPERTY_END()
	
private:
	//商店id
	ObjUInt32				m_shopid;
	//玩家uid
	ObjUInt64				m_owner;
	//每天刷新时间点			
	ObjUInt64				m_refreshTime;
	//每周刷新时间点		
	ObjUInt64				m_weekRefreshTime;
	//每月刷新时间点		
	ObjUInt64				m_monthRefreshTime;
	//刷新次数
	ObjUInt32				m_refreshNum;
	//商品
	std::vector<ShopItem*>  m_shopItems;
	//同步db状态
	ShopSyncDBState			m_syncDBState;
	//表格项
	ShopDataEntry*			m_pDataEntry;
	//版本
	ObjUInt32				m_version;

	// 刷新方式-->刷新状态
	HashMap<UInt32, UInt32> m_RefreshCycleTypeWithState;

	// 商店进行周期刷新时未刷新页签中的商品的剩余数量 商品id-->商品数量
	HashMap<UInt32, Int16> m_UnRefreshShopItemRestNum;

	// 地下城id
	UInt32 m_DungeonId;
};

#endif
