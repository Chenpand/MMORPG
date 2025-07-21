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
	*@brief ����һ����Ʒ
	*/
	WSShopItem* FindShopItem(UInt8 tabType, UInt8 jobType, UInt32 shopItemId);
	WSShopItem* FindShopItem(const WSShopItemIndex& index, UInt32 shopItemId);

	/*
	*@brief ���������µ���Ʒ
	*/
	bool FindShopItems(UInt8 tabType, UInt8 jobType, WSShopItemVec& shopItems);
	bool FindShopItems(const WSShopItemIndex& index, WSShopItemVec& shopItems);

	/*
	*@brief �����Ʒ
	*/
	void AddShopItem(const WSShopItemIndex& index, WSShopItem* shopItem);

public:
	UInt32 OnBuyAdventureTeamShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	UInt32 OnBuyGnomeCoinShopItem(WSPlayer* player, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	UInt32 OnBuyHireShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	UInt32 OnBuyMonopolyShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

public:
	/*
	*@brief ��־
	*/
	const std::string LogStr();

private:
	// �̵�id
	UInt32 m_ShopId;
	// ӵ����(�˺�id)
	UInt32 m_Owner;
	
	// ��Ʒ��
	std::map<WSShopItemIndex, WSShopItemVec> m_ShopItemsMap;
};

#endif