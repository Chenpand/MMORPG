#include "WSShop.h"

#include <CLRecordClient.h>
#include <CLShopItemDataEntry.h>
#include <CLAccountShopItemDataEntry.h>
#include "WSPlayer.h"


WSShop::WSShop()
{
	m_ShopId = 0;
	m_Owner = 0;
}

WSShop::~WSShop()
{

}

bool WSShop::Init(UInt32 shopid, UInt32 accid)
{
	SetShopId(shopid);
	SetOwner(accid);

	return true;
}

WSShopItem* WSShop::FindShopItem(UInt8 tabType, UInt8 jobType, UInt32 shopItemId)
{
	WSShopItemIndex index(tabType, jobType);
	return FindShopItem(index, shopItemId);
}

WSShopItem* WSShop::FindShopItem(const WSShopItemIndex& index, UInt32 shopItemId)
{
	WSShopItemVec shopItems;
	if (!FindShopItems(index, shopItems))
	{
		return NULL;
	}

	for (auto shopItem : shopItems)
	{
		if (!shopItem) continue;

		if (shopItem->GetShopItemId() == shopItemId)
		{
			return shopItem;
		}
	}
	return NULL;
}

bool WSShop::FindShopItems(UInt8 tabType, UInt8 jobType, WSShopItemVec& shopItems)
{
	WSShopItemIndex index(tabType, jobType);
	return FindShopItems(index, shopItems);
}

bool WSShop::FindShopItems(const WSShopItemIndex& index, WSShopItemVec& shopItems)
{
	auto itr = m_ShopItemsMap.find(index);
	if (itr != m_ShopItemsMap.end())
	{
		shopItems = itr->second;
		return true;
	}
	return false;
}

void WSShop::AddShopItem(const WSShopItemIndex& index, WSShopItem* shopItem)
{
	m_ShopItemsMap[index].push_back(shopItem);
}

UInt32 WSShop::OnBuyAdventureTeamShopItem(WSPlayer* player, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum)
{
	if (!player || !shopData || !shopItemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (shopItemData->extensibleCondition > player->GetAdventureTeamLevel())
	{
		ErrorLogStream << PLAYERINFO(player) << " shop item(" << shopItemData->id << ")'s level cond(" << shopItemData->extensibleCondition << ") is less than team level(" << player->GetAdventureTeamLevel() << ")!" << LogStream::eos;
		return ErrorCode::SHOP_BUY_ERR;
	}

	if (shopItemData->costItemVec.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(player) << " cost items size is error!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 costItemId = shopItemData->costItemVec.front().id;
	UInt32 costItemNum = shopItemData->costItemVec.front().num;
	auto costItemData = ItemDataEntryMgr::Instance()->FindEntry(costItemId);
	if (!costItemData)
	{
		ErrorLogStream << PLAYERINFO(player) << " can not find item data(" << costItemId << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 needCostNum = buyNum * costItemNum;
	if (needCostNum == 0)
	{
		ErrorLogStream << PLAYERINFO(player) " need cost num is zero!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (!player->CheckRmAccountMoney((ItemSubType)costItemData->subType, needCostNum))
	{
		return ErrorCode::SHOP_BUY_BLESS_CRYSTAL_NOT_ENOUGH;
	}

	std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, needCostNum);

	if (!player->RemoveAccountMoney(reason, (ItemSubType)costItemData->subType, needCostNum))
	{
		return ErrorCode::SHOP_BUY_BLESS_CRYSTAL_NOT_ENOUGH;
	}

	ItemRewardVec rewards;
	rewards.push_back(ItemReward(shopItemData->itemDatId , shopItemData->itemNum * buyNum, 0 , 0));
	player->AddItem(reason, rewards);

	return ErrorCode::SUCCESS;
}

UInt32 WSShop::OnBuyGnomeCoinShopItem(WSPlayer* player, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum)
{
	if (!player || !shopItemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (shopItemData->costItemVec.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(player) << " cost items size is error!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 ownNum = player->GetGnomeCoinNum();
	UInt32 needCostNum = buyNum * shopItemData->costItemVec.front().num;

	if (needCostNum == 0)
	{
		ErrorLogStream << PLAYERINFO(player) " need cost num is zero!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (ownNum < needCostNum)
	{
		return ErrorCode::SHOP_BUY_GNOME_COIN_NOT_ENOUGH;
	}

	player->SetGnomeCoinNum(ownNum - needCostNum);

	std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, needCostNum);
	ItemRewardVec rewards;
	rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * buyNum, 0, 0));
	player->AddItem(reason, rewards);

	player->MallBuyGotInfoSync(MALL_BUY_GOT_TYPE_GNOME_COIN);
	
	return ErrorCode::SUCCESS;
}

UInt32 WSShop::OnBuyHireShopItem(WSPlayer * player, const ShopDataEntry * shopData, const AccountShopItemDataEntry * shopItemData, UInt32 buyNum)
{
	if (!player || !shopData || !shopItemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (shopItemData->costItemVec.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(player) << " cost items size is error!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 costItemId = shopItemData->costItemVec.front().id;
	UInt32 costItemNum = shopItemData->costItemVec.front().num;
	auto costItemData = ItemDataEntryMgr::Instance()->FindEntry(costItemId);
	if (!costItemData)
	{
		ErrorLogStream << PLAYERINFO(player) << " can not find item data(" << costItemId << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 needCostNum = buyNum * costItemNum;
	if (needCostNum == 0)
	{
		ErrorLogStream << PLAYERINFO(player) " need cost num is zero!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (!player->CheckRmAccountMoney((ItemSubType)costItemData->subType, needCostNum))
	{
		return ErrorCode::SHOP_BUY_BLESS_CRYSTAL_NOT_ENOUGH;
	}

	std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, needCostNum);

	if (!player->RemoveAccountMoney(reason, (ItemSubType)costItemData->subType, needCostNum))
	{
		return ErrorCode::SHOP_BUY_BLESS_CRYSTAL_NOT_ENOUGH;
	}

	ItemRewardVec rewards;
	rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * buyNum, 0, 0));
	player->AddItem(reason, rewards);

	return ErrorCode::SUCCESS;
}

UInt32 WSShop::OnBuyMonopolyShopItem(WSPlayer * player, const ShopDataEntry * shopData, const AccountShopItemDataEntry * shopItemData, UInt32 buyNum)
{
	if (!player || !shopData || !shopItemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (shopItemData->costItemVec.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(player) << " cost items size is error!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 costItemId = shopItemData->costItemVec.front().id;
	UInt32 costItemNum = shopItemData->costItemVec.front().num;

	UInt32 needCostNum = buyNum * costItemNum;
	if (needCostNum == 0)
	{
		ErrorLogStream << PLAYERINFO(player) " need cost num is zero!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (player->GetMonopolyInfo().GetCoin() < needCostNum)
	{
		return ErrorCode::SHOP_BUY_NOT_ENOUGH_ITEM;
	}

	std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, needCostNum);

	player->GetMonopolyInfo().SetCoin(player->GetMonopolyInfo().GetCoin() - needCostNum, reason);

	ItemRewardVec rewards;
	rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * buyNum, 0, 0));
	player->AddItem(reason, rewards);

	return ErrorCode::SUCCESS;
}

const std::string WSShop::LogStr()
{
	std::ostringstream oss;
	oss << " shop(" << GetShopId() << ",owner:" << GetOwner() << ")";
	return oss.str();
}