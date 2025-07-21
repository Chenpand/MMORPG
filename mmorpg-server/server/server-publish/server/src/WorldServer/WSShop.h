#ifndef _WS_SHOP_H_
#define _WS_SHOP_H_

#include <CLObjectProperty.h>
#include "WSShopItem.h"

class AccountShopItemDataEntry;
class ShopDataEntry;


class WSShop
{
	AVALON_DEFINE_OBJECTPOOL(WSShop, Avalon::NullMutex)

public:
	WSShop();
	~WSShop();

	bool Init(UInt32 shopid, UInt32 accid);

public:
	void SetShopId(UInt32 shopid) { m_ShopId = shopid; }
	UInt32 GetShopId() const { return m_ShopId; }

	void SetOwner(UInt32 accid) { m_Owner = accid; }
	UInt32 GetOwner() const { return m_Owner; }

public:
	/*
	*@brief 查找一个商品
	*/
	WSShopItem* FindShopItem(UInt8 tabType, UInt8 jobType, UInt32 shopItemId);
	WSShopItem* FindShopItem(const WSShopItemIndex& index, UInt32 shopItemId);

	/*
	*@brief 查找索引下的商品
	*/
	bool FindShopItems(UInt8 tabType, UInt8 jobType, WSShopItemVec& shopItems);
	bool FindShopItems(const WSShopItemIndex& index, WSShopItemVec& shopItems);

	/*
	*@brief 添加商品
	*/
	void AddShopItem(const WSShopItemIndex& index, WSShopItem* shopItem);

public:
	UInt32 OnBuyAdventureTeamShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	UInt32 OnBuyGnomeCoinShopItem(WSPlayer* player, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	UInt32 OnBuyHireShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	UInt32 OnBuyMonopolyShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

public:
	/*
	*@brief 日志
	*/
	const std::string LogStr();

private:
	// 商店id
	UInt32 m_ShopId;
	// 拥有者(账号id)
	UInt32 m_Owner;
	
	// 商品集
	std::map<WSShopItemIndex, WSShopItemVec> m_ShopItemsMap;
};

#endif