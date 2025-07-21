#ifndef _SCENE_SHOP_MGR_H_
#define _SCENE_SHOP_MGR_H_

#include "Shop.h"
#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLItemProtocol.h>
#include <CLSysRecordProtocol.h>
#include <CLSysRecordDefine.h>


class Player;

class ShopMgr
{
public:
	ShopMgr();
	~ShopMgr();

	//设置、获得所有者
	void SetOwner(Player *pPlayer) { m_pOwner = pPlayer; }
	Player* GetOwner() const { return m_pOwner; }

	//查询商店
	UInt32 OnQueryItem(UInt8 shopId, UInt8 cache);
	//购买道具
	UInt32 OnBuy(const CLProtocol::SceneShopBuy& protocol, UInt16& newNum, UInt32& leaseEndTimeStamp);
	//刷新商店
	UInt32 OnRefreshShop(UInt8 shopId);
	//加载商店
	bool LoadShop(Shop* shop);
	//加载商品
	void LoadShopItem(ShopItem* shopItem);
	//获取商店
	Shop* GetShop(UInt8 shopId);
	//删除商店
	void DeleteShop(UInt8 shopId);
	//获取商品
	ShopItem* GetShopItem(UInt8 shopId, UInt32 shopItemId);
	//查询商店剩余刷新次数
	void OnQueryRefreshNum(UInt8 shopId, UInt8& restNum, UInt8& maxNum);

	void OnDayChange();
	void OnOnline(bool bDayChange);
	//购买仓库商店回掉
	void BuyGuildStorageCb(UInt32 retCode, UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt32 costId, UInt32 costNum);

public:
	/**
	*@brief 场景切换
	*/
	void OnSceneChanged();

	/**
	*@brief 地下城结算
	*/
	void OnDungeonRaceEnd(UInt32 dungeonId, UInt32 mysticalMerchantId);

public:
	/**
	*@brief 查询sysrecord数据返回
	*/
	void OnSysRecordDataRes(const CLProtocol::SceneSysRecordDataRes& protocol);

private:
	UInt32 BuyNormalMarket(UInt32 shopItemId, UInt16 num);
	UInt32 BuyBlackMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum);
	UInt32 BuyGuildStorage(UInt8 shopId, UInt32 shopItemId, UInt16 num);
	UInt32 BuyDungeonMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum);
	UInt32 BuyLeaseMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum, UInt32& leaseEndTimeStamp);

	// 购买时额外道具消耗
	bool OnRemoveExtraItemOnBuy(ShopItemDataEntry* shopItemDataEntry, const std::string& reason);

	//是否是过期商店
	bool IsNeedRefresh(Shop* shop);

	//刷新商店道具列表
	Shop* RefreshShop(UInt32 shopId, bool first, UInt32 version);
	//同步客户端
	void SyncClient(Shop* shop, bool cache = false);
	//同步DB
	void SyncDB(Shop* shop);
	//检查能否购买
	UInt32 CheckBuy(ShopItem* shopItem, UInt16 num, UInt32& cost, ItemDataEntry*& costItem);
	//计算刷新所需的货币
	UInt16 GetRefreshCost(Shop* shop);
	//能否手动刷新
	bool _CanManualRefresh(ShopRefreshType type);

	// 检查能否购买
	UInt32 CheckOtherCost(ShopItem* shopItem, UInt16 num);
	// 扣货币
	UInt32 RemoveOtherCost(ShopItem* shopItem, const std::string& reason, UInt16 num);

	//随机神秘商人
	UInt32 _RandMysticalMerchant();

	//神秘商人商品数量更新
	void _UpdateMysticalMerchantShopItemNum(const CLProtocol::SceneSysRecordDataRes& res);

private:// 商店周期刷新
	/**
	*@brief 刷新周期是否过期
	*/
	bool _IsNeedRefreshOnRefreshCycle(Shop* shop, UInt32 refreshCycleType);

	/**
	*@brief 保存商店未刷新页签中的商品数量
	*/
	bool _KeepUnRefreshShopItemNum(Shop* shop, UInt32 unRefreshCycleType);

private:
	// 所有者
	Player		*m_pOwner;
	// 商店列表
	std::map<ObjID_t, Shop*>	m_shopList;
};

#endif

