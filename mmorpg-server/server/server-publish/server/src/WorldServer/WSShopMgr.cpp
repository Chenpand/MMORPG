#include "WSShopMgr.h"

#include <CLAccountShopItemDataEntry.h>
#include <CLShopItemDataEntry.h>
#include <CLAdventurePassDataEntry.h>
#include "AccountLoadDataCallback.h"
#include "AdventurePassMgr.h"

#include "WSPlayer.h"
#include "WSActivityMgr.h"
#include "WSItemTransaction.h"


/**
*@brief 商店购买扣除消耗检查
*/
class ShopBuyRemoveCostTransaction : public ItemTransaction
{
public:
	ShopBuyRemoveCostTransaction(WSPlayer *player, const CLProtocol::WorldAccountShopItemBuyReq& req, ShopType shopType)
		: ItemTransaction(player), m_BuyReq(req), m_ShopType(shopType) {}
	~ShopBuyRemoveCostTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		if (!player)
		{
			UnLock();
			return;
		}

		if (m_ShopType == ST_ACCOUNT_BIND_GOLD)
		{
			if (WSShopMgr::Instance()->OnCheckShopBuyRemoveCostFromSceneCallback(player, m_BuyReq))
			{
				RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ACCOUNT_SHOP_BUY));
			}
			else
			{
				UnLock();
			}
		}
		else if (m_ShopType == ST_ADVENTURE_PASS)
		{
			if (WSShopMgr::Instance()->OnCheckShopBuyRemoveCostFromSceneCallback(player, m_BuyReq))
			{
				RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ACCOUNT_SHOP_BUY));
			}
			else
			{
				UnLock();
			}
		}
		else if (m_ShopType == ST_SECRET_SELL)
		{
			if (WSShopMgr::Instance()->OnCheckShopBuyRemoveCostFromSceneCallback(player, m_BuyReq))
			{
				RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ACCOUNT_SHOP_BUY));
			}
			else
			{
				UnLock();
			}
		}
		else if (m_ShopType == ST_CHAMPION)
		{
			if (WSShopMgr::Instance()->OnCheckShopBuyRemoveCostFromSceneCallback(player, m_BuyReq))
			{
				RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ACCOUNT_SHOP_BUY));
			}
			else
			{
				UnLock();
			}
		}

	}

	void OnCheckFailed(WSPlayer *player)
	{
		UnLock();
		ErrorLogStream << PLAYERINFO(player) << " check failed(" << m_BuyReq.buyShopItemId << ") with index(" << (UInt32)m_BuyReq.queryIndex.tabType << "," << (UInt32)m_BuyReq.queryIndex.jobType << ")!" << LogStream::eos;
	}

private:
	CLProtocol::WorldAccountShopItemBuyReq m_BuyReq;
	ShopType m_ShopType;
};

WSShopMgr::WSShopMgr()
{

}

WSShopMgr::~WSShopMgr()
{

}

bool WSShopMgr::Init()
{
	return true;
}

void WSShopMgr::LoadShopItemFromDB(CLRecordCallback* callback)
{
	if (!callback) return;

	if (!callback->NextRow())
	{
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	do
	{
		UInt32 shopItemId = callback->GetData("shop_itemid");

		// 找到商品表
		auto shopItemData = AccountShopItemDataEntryMgr::Instance()->FindEntry(shopItemId);
		if (!shopItemData)
		{
			ErrorLogStream << "Can't find shop item(" << shopItemId << ")!" << LogStream::eos;
			continue;
		}

		if (shopItemData->offSaleTime > 0 && shopItemData->offSaleTime < curTime)
		{
			continue;
		}

		UInt32 owner = callback->GetData("owner");

		// 商店
		auto shop = FindShop(owner, shopItemData->shopId);
		if (!shop)
		{
			shop = new WSShop();
			if (!shop->Init(shopItemData->shopId, owner))
			{
				ErrorLogStream << "Player(" << owner << ") init shop(" << shopItemData->shopId << ") failed!" << LogStream::eos;
				delete shop;
				continue;
			}
			AddShop(owner, shop);
		}

		WSShopItemIndex shopItemIndex(shopItemData->tabType, shopItemData->jobType);

		// 商品
		auto shopItem = shop->FindShopItem(shopItemIndex, shopItemId);
		if (!shopItem)
		{
			shopItem = new WSShopItem();
			shopItem->SetID(callback->GetKey());
			shopItem->SetDBData(callback);
			shop->AddShopItem(shopItemIndex, shopItem);
		}

	} while (callback->NextRow());
}

void WSShopMgr::InitPlayerAccountShopByTableData(WSPlayer* player)
{
	if (!player) return;

	std::vector<ShopDataEntry*> shopDatas;
	ShopDataEntryMgr::Instance()->GetAccountShops(shopDatas);
	if (shopDatas.empty())
	{
		ErrorLogStream << "Account shop datas are empty!" << LogStream::eos;
		return;
	}

	for (auto shopData : shopDatas)
	{
		if (!shopData) continue;

		if (!InitPlayerAccountShopByTableData(player, shopData->shopId))
		{
			ErrorLogStream << PLAYERINFO(player) << " init account shop(" << shopData->shopId << ") failed!" << LogStream::eos;
		}
	}
}

bool WSShopMgr::InitPlayerAccountShopByTableData(WSPlayer* player, UInt32 shopId)
{
	if (!player) return false;

	auto shop = FindShop(player->GetAccId(), shopId);
	if (!shop)
	{
		shop = new WSShop();
		if (!shop->Init(shopId, player->GetAccId()))
		{
			ErrorLogStream << PLAYERINFO(player) << " init shop(" << shopId << ") failed!" << LogStream::eos;
			delete shop;
			return false;
		}
		AddShop(player->GetAccId(), shop);
	}

	AccountShopItemDataVec shopItemDatas;
	AccountShopItemDataEntryMgr::Instance()->GetAccountShopItemDatas(shopId, shopItemDatas);
	if (shopItemDatas.empty())
	{
		ErrorLogStream << PLAYERINFO(player) << " shop(" << shopId << ")'s items are empty!" << LogStream::eos;
		return false;
	}

	for (auto shopItemData : shopItemDatas)
	{
		if (!shopItemData) continue;

		WSShopItemIndex shopItemIndex(shopItemData->tabType, shopItemData->jobType);
		auto shopItem = shop->FindShopItem(shopItemIndex, shopItemData->id);
		if (!shopItem)
		{
			shopItem = new WSShopItem();
			if (!shopItem->InitByShopItemData(player, shopItemData))
			{
				ErrorLogStream << PLAYERINFO(player) << " init shop item(" << shopItemData->id << ") failed!" << LogStream::eos;
				delete shopItem;
				continue;
			}
			shop->AddShopItem(shopItemIndex, shopItem);
			//shopItem->SaveToDB();
		}
	}

	return true;
}

WSShop* WSShopMgr::FindShop(UInt32 accid, UInt32 shopid)
{
	auto itr = m_ShopMap.find(accid);
	if (itr == m_ShopMap.end())
	{
		return NULL;
	}

	auto shopVec = itr->second;
	for (auto shop : shopVec)
	{
		if (!shop) continue;
		if (shop->GetShopId() == shopid)
		{
			return shop;
		}
	}

	return NULL;
}

void WSShopMgr::AddShop(UInt32 accid, WSShop* shop)
{
	m_ShopMap[accid].push_back(shop);
}

void WSShopMgr::LoadShopItemAccountBuyRecordFromDB(CLRecordCallback* callback)
{
	if (!callback) return;

	if (!callback->NextRow())
	{
		return;
	}

	do
	{
		UInt32 accid = callback->GetData("owner");
		UInt32 shopItemId = callback->GetData("shop_item_id");

		WSShopItemAccountBuyIndex index(accid, shopItemId);
		WSShopItemAccountBuyRecord* record = FindShopItemAccountBuyRecord(index);
		if (!record)
		{
			record = new WSShopItemAccountBuyRecord(accid);
			record->SetID(callback->GetKey());
			record->SetDBData(callback);
			AddShopItemAccountBuyRecord(index, record);
		}

	} while (callback->NextRow());
}

WSShopItemAccountBuyRecord* WSShopMgr::FindShopItemAccountBuyRecord(UInt32 accid, UInt32 shopItemId)
{
	WSShopItemAccountBuyIndex index(accid, shopItemId);
	return FindShopItemAccountBuyRecord(index);
}

WSShopItemAccountBuyRecord* WSShopMgr::FindShopItemAccountBuyRecord(const WSShopItemAccountBuyIndex& index)
{
	auto itr = m_AccountShopItemBuyRecords.find(index);
	if (itr == m_AccountShopItemBuyRecords.end())
	{
		return NULL;
	}
	return itr->second;
}

void WSShopMgr::AddShopItemAccountBuyRecord(const WSShopItemAccountBuyIndex& index, WSShopItemAccountBuyRecord* record)
{
	m_AccountShopItemBuyRecords.insert(std::make_pair(index, record));
}

void WSShopMgr::AddShopItemAccountBuyRecord(UInt32 accid, UInt32 shopItemId, UInt32 buyNum)
{
	WSShopItemAccountBuyIndex index(accid, shopItemId);
	auto record = FindShopItemAccountBuyRecord(index);
	if (!record)
	{
		record = new WSShopItemAccountBuyRecord(accid);

		auto account = AccountMgr::Instance()->FindAccount(accid);
		if (!account)
		{
			ErrorLogStream << "Can't find account(" << accid << ")!" << LogStream::eos;
		}
		else
		{
			record->SetAccGUID(account->GetID());
		}

		auto shopItemData = AccountShopItemDataEntryMgr::Instance()->FindEntry(shopItemId);
		if (!shopItemData)
		{
			ErrorLogStream << "Account(" << accid << ") can't find shop item data(" << shopItemId << ")!" << LogStream::eos;
		}
		else
		{
			record->SetShopId(shopItemData->shopId);
		}

		record->SetShopItemId(shopItemId);
		record->SetBuyedNum(buyNum);
		AddShopItemAccountBuyRecord(index, record);
	}
	else
	{
		record->IncBuyedNum(buyNum);
	}

	record->SaveToDB();
}

void WSShopMgr::LoadShopItemRoleBuyRecordFromDB(CLRecordCallback* callback)
{
	if (!callback) return;

	if (!callback->NextRow())
	{
		return;
	}

	do
	{
		ObjID_t roleid = callback->GetData("owner");
		UInt32 shopItemId = callback->GetData("shop_item_id");

		WSShopItemRoleBuyIndex index(roleid, shopItemId);
		WSShopItemRoleBuyRecord* record = FindShopItemRoleBuyRecord(index);
		if (!record)
		{
			record = new WSShopItemRoleBuyRecord(roleid);
			record->SetID(callback->GetKey());
			record->SetDBData(callback);
			AddShopItemRoleBuyRecord(index, record);
		}

	} while (callback->NextRow());
}

WSShopItemRoleBuyRecord* WSShopMgr::FindShopItemRoleBuyRecord(ObjID_t roleid, UInt32 shopItemId)
{
	WSShopItemRoleBuyIndex index(roleid, shopItemId);
	return FindShopItemRoleBuyRecord(index);
}

WSShopItemRoleBuyRecord* WSShopMgr::FindShopItemRoleBuyRecord(const WSShopItemRoleBuyIndex& index)
{
	auto itr = m_RoleShopItemBuyRecords.find(index);
	if (itr == m_RoleShopItemBuyRecords.end())
	{
		return NULL;
	}
	return itr->second;
}

void WSShopMgr::AddShopItemRoleBuyRecord(const WSShopItemRoleBuyIndex& index, WSShopItemRoleBuyRecord* record)
{
	m_RoleShopItemBuyRecords.insert(std::make_pair(index, record));
}

void WSShopMgr::AddShopItemRoleBuyRecord(ObjID_t roleid, UInt32 shopItemId, UInt32 buyNum)
{
	WSShopItemRoleBuyIndex index(roleid, shopItemId);
	auto record = FindShopItemRoleBuyRecord(index);
	if (!record)
	{
		record = new WSShopItemRoleBuyRecord(roleid);
		record->SetShopItemId(shopItemId);
		record->SetBuyedNum(buyNum);
		AddShopItemRoleBuyRecord(index, record);
	}
	else
	{
		record->IncBuyedNum(buyNum);
	}

	record->SaveToDB();
}

void WSShopMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player) return;

	InitPlayerAccountShopByTableData(player);
}

void WSShopMgr::OnPlayerQueryShopItem(WSPlayer* player, const CLProtocol::WorldAccountShopItemQueryReq& req)
{
	if (!player) return;

	CLProtocol::WorldAccountShopItemQueryRes res;
	res.resCode = ErrorCode::SUCCESS;
	res.queryIndex = req.queryIndex;

	auto shopData = ShopDataEntryMgr::Instance()->FindEntry(req.queryIndex.shopId);
	if (!shopData)
	{
		res.resCode = ErrorCode::SHOP_QUERY_ERR;
		player->SendProtocol(res);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop data(" << req.queryIndex.shopId << ")!" << LogStream::eos;
		return;
	}

	UInt32 checkCode = ErrorCode::SUCCESS;

	switch (shopData->shopType)
	{
	case ST_OP_ACTIVITY:
		checkCode = _CheckOpActivityShopCond(player, shopData->helpId);
		break;

	case ST_ADVENTURE_TEAM:
		player->SyncAdventureTeamMoney();
		break;

	case ST_HIRE:
		player->SyncAccountCounterToClient(ACC_COUNTER_HIRE_COIN);
		break;

	default:
		break;
	}

	if (checkCode != ErrorCode::SUCCESS)
	{
		res.resCode = checkCode;
		player->SendProtocol(res);
		return;
	}

	auto shop = FindShop(player->GetAccId(), req.queryIndex.shopId);
	if (!shop)
	{
		res.resCode = ErrorCode::SHOP_QUERY_ERR;
		player->SendProtocol(res);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop(" << req.queryIndex.shopId << ")!" << LogStream::eos;
		return;
	}

	WSShopItemVec shopItems;
	if (!shop->FindShopItems(req.queryIndex.tabType, req.queryIndex.jobType, shopItems))
	{
		res.resCode = ErrorCode::SHOP_QUERY_ERR;
		player->SendProtocol(res);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop items by index(" << (UInt32)req.queryIndex.tabType << "," << (UInt32)req.queryIndex.jobType << ")!" << LogStream::eos;
		return;
	}

	if (shopItems.empty())
	{
		res.resCode = ErrorCode::SHOP_QUERY_ERR;
		player->SendProtocol(res);
		ErrorLogStream << PLAYERINFO(player) << shop->LogStr() << "'s items are empty!" << LogStream::eos;
		return;
	}

	// 下一波商品上架时间
	UInt32 nextGroupOnSaleTime = 0;
	// 当前时间
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto shopItem : shopItems)
	{
		if (!shopItem) continue;

		auto shopItemData = AccountShopItemDataEntryMgr::Instance()->FindEntry(shopItem->GetShopItemId());
		if (!shopItemData)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can't find shop(" << shop->GetShopId() << ")'s item(" << shopItem->GetShopItemId() << ")'s data!" << LogStream::eos;
			continue;
		}

		if (curTime < shopItemData->onSaleTime)
		{
			if (nextGroupOnSaleTime == 0 || nextGroupOnSaleTime > shopItemData->onSaleTime)
			{
				nextGroupOnSaleTime = shopItemData->onSaleTime;
			}
		}

		if (!shopItemData->IsOnSale(player->GetLevel()))
		{
			continue;
		}
		
		CLProtocol::AccountShopItemInfo shopItemInfo;
		TransToProtocolInfo(player, shopItemData, shopItemInfo);
		res.shopItems.push_back(shopItemInfo);
	}
	
	res.nextGroupOnSaleTime = nextGroupOnSaleTime;
	player->SendProtocol(res);
}

void WSShopMgr::OnPlayerBuyShopItem(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req)
{
	if (!player) return;

	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(req.queryIndex.shopId);
	if (!shopData)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop(" << req.queryIndex.shopId << ")'s data!" << LogStream::eos;
		return;
	}

	auto shopItemData = AccountShopItemDataEntryMgr::Instance()->FindEntry(req.buyShopItemId);
	if (!shopItemData)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop item data(" << req.buyShopItemId << ")!" << LogStream::eos;
		return;
	}

	auto shop = FindShop(player->GetAccId(), req.queryIndex.shopId);
	if (!shop)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop(" << req.queryIndex.shopId << ")!" << LogStream::eos;
		return;
	}

	auto shopItem = shop->FindShopItem(req.queryIndex.tabType, req.queryIndex.jobType, req.buyShopItemId);
	if (!shopItem)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop item(" << req.buyShopItemId << ") with index(" << (UInt32)req.queryIndex.tabType << "," << (UInt32)req.queryIndex.jobType << ")!" << LogStream::eos;
		return;
	}

	UInt32 resCode = _CheckBuy(player, req, shopData, shopItemData, shop, shopItem);
	if (resCode != ErrorCode::SUCCESS)
	{
		SendPlayerBuyShopItemResponse(player, req, resCode);
		return;
	}

	switch (shopData->shopType)
	{
	case ST_ADVENTURE_TEAM:
		resCode = shop->OnBuyAdventureTeamShopItem(player, shopData, shopItemData, req.buyShopItemNum);
		break;

	case ST_OP_ACTIVITY:
		resCode = _OnPlayerBuyOpActivityShopItem(player, shop, shopData, shopItemData, req.buyShopItemNum);
		break;

	case ST_ADVENTURE_PASS:
	{
		auto pass = AdventurePassMgr::Instance()->GetAdventurePass(player->GetAccId());
		if (pass == nullptr)
		{
			resCode = ErrorCode::SHOP_BUY_ERR;
			break;
		}

		if (shopItemData->extensibleCondition > pass->GetLevel())
		{
			ErrorLogStream << PLAYERINFO(player) << " shop item(" << shopItemData->id << ")'s level cond(" << shopItemData->extensibleCondition << ") is less than pass level(" << pass->GetLevel() << ")!" << LogStream::eos;
			resCode = ErrorCode::SHOP_BUY_ERR;
			break;
		}

		auto trans = new ShopBuyRemoveCostTransaction(player, req, ST_ADVENTURE_PASS);
		for (const auto& costItem : shopItemData->costItemVec)
		{
			if (MAX_MONEY / req.buyShopItemNum < costItem.num)
			{
				SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
				return;
			}
			trans->SetItem(costItem.id, costItem.num * req.buyShopItemNum);
		}
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << PLAYERINFO(player) << " begin transaction failed!" << LogStream::eos;
			SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		}
		return;
	}	
	
	case ST_SECRET_SELL:
	{
		auto trans = new ShopBuyRemoveCostTransaction(player, req, ST_SECRET_SELL);
		for (const auto& costItem : shopItemData->costItemVec)
		{
			if (MAX_MONEY / req.buyShopItemNum < costItem.num)
			{
				SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
				return;
			}
			trans->SetItem(costItem.id, costItem.num * req.buyShopItemNum);
		}
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << PLAYERINFO(player) << " begin transaction failed!" << LogStream::eos;
			SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		}
		return;
	}

	case ST_ACCOUNT_BIND_GOLD:
	{
		auto trans = new ShopBuyRemoveCostTransaction(player, req, ST_ACCOUNT_BIND_GOLD);
		for (const auto& costItem : shopItemData->costItemVec)
		{
			trans->SetItem(costItem.id, costItem.num * req.buyShopItemNum);
		}
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << PLAYERINFO(player) << " begin transaction failed!" << LogStream::eos;
			SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		}
		return;
	}
	case ST_CHAMPION:
	{
		auto trans = new ShopBuyRemoveCostTransaction(player, req, ST_CHAMPION);
		for (const auto& costItem : shopItemData->costItemVec)
		{
			trans->SetItem(costItem.id, costItem.num * req.buyShopItemNum);
		}
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << PLAYERINFO(player) << " begin transaction failed!" << LogStream::eos;
			SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		}
		return;
	}


	case ST_MONOPOLY:
	{
		resCode = shop->OnBuyMonopolyShopItem(player, shopData, shopItemData, req.buyShopItemNum);
		break;
	}



	case ST_HIRE:
	{
		resCode = shop->OnBuyHireShopItem(player, shopData, shopItemData, req.buyShopItemNum);
		break;
		
	}

	default:
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		return;
	}

	if (resCode == ErrorCode::SUCCESS)
	{
		OnPlayerBuyShopItemSuccess(player, shopData->shopType, shopData->shopId, shopItem->GetShopItemId(), req.buyShopItemNum);

		CLProtocol::WorldAccountShopItemBuyRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.queryIndex = req.queryIndex;
		res.buyShopItemId = req.buyShopItemId;
		res.buyShopItemNum = req.buyShopItemNum;
		res.accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
		res.roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);
		player->SendProtocol(res);
	}
	else
	{
		SendPlayerBuyShopItemResponse(player, req, resCode);
	}

	InfoLogStream << PLAYERINFO(player) << "buy shop(" << req.queryIndex.shopId << ")'s item(" << req.buyShopItemId << "), buy num(" << req.buyShopItemNum << "), error code(" << (UInt32)resCode << ")." << LogStream::eos;
}

bool WSShopMgr::IsRestBuyNumEnough(WSPlayer* player, WSShopItem* shopItem, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum)
{
	if (!player || !shopItem || !shopItemData) return false;

	if (shopItemData->accountLimitBuyNum == 0 && shopItemData->roleLimitBuyNum == 0)
	{
		return true;
	}

	UInt32 accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
	UInt32 roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);

	if (accountRestBuyNum < buyNum)
	{
		return false;
	}

	if (roleRestBuyNum < buyNum)
	{
		return false;
	}

	return true;
}

void WSShopMgr::SendPlayerBuyShopItemResponse(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req, UInt32 errorCode)
{
	if (!player) return;

	CLProtocol::WorldAccountShopItemBuyRes res;
	res.resCode = errorCode;
	res.queryIndex = req.queryIndex;
	res.buyShopItemId = req.buyShopItemId;
	res.buyShopItemNum = req.buyShopItemNum;
	res.accountRestBuyNum = 0;
	res.roleRestBuyNum = 0;
	player->SendProtocol(res);
}

void WSShopMgr::OnPlayerBuyShopItemSuccess(WSPlayer* player, ShopType shopType, UInt32 shopId, UInt32 shopItemId, UInt32 buyNum)
{
	if (!player) return;
	
	AddShopItemAccountBuyRecord(player->GetAccId(), shopItemId, buyNum);
	AddShopItemRoleBuyRecord(player->GetID(), shopItemId, buyNum);

	if (shopType == ST_ACCOUNT_BIND_GOLD)
	{
		CLProtocol::WorldAccountShopItemBuyedNotify notify;
		notify.playerId = player->GetID();
		notify.shopId = shopId;
		notify.shopItemId = shopItemId;
		notify.shopItemNum = buyNum;
		player->SendToScene(notify);
	}
	else if (shopType == ST_CHAMPION)
	{
		//暂时没用
	}
}

bool WSShopMgr::OnCheckShopBuyRemoveCostFromSceneCallback(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req)
{
	if (!player) return false;

	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(req.queryIndex.shopId);
	if (!shopData)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop(" << req.queryIndex.shopId << ")'s data!" << LogStream::eos;
		return false;
	}

	auto shopItemData = AccountShopItemDataEntryMgr::Instance()->FindEntry(req.buyShopItemId);
	if (!shopItemData)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop item data(" << req.buyShopItemId << ")!" << LogStream::eos;
		return false;
	}

	auto shop = WSShopMgr::Instance()->FindShop(player->GetAccId(), req.queryIndex.shopId);
	if (!shop)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop(" << req.queryIndex.shopId << ")!" << LogStream::eos;
		return false;
	}

	auto shopItem = shop->FindShopItem(req.queryIndex.tabType, req.queryIndex.jobType, req.buyShopItemId);
	if (!shopItem)
	{
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		ErrorLogStream << PLAYERINFO(player) << " can't find shop item(" << req.buyShopItemId << ") with index(" << (UInt32)req.queryIndex.tabType << "," << (UInt32)req.queryIndex.jobType << ")!" << LogStream::eos;
		return false;
	}

	UInt32 resCode = _CheckBuy(player, req, shopData, shopItemData, shop, shopItem);
	if (resCode != ErrorCode::SUCCESS)
	{
		SendPlayerBuyShopItemResponse(player, req, resCode);
		return false;
	}

	switch (shopData->shopType)
	{
	case ST_ACCOUNT_BIND_GOLD:
	{
		std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, req.buyShopItemNum);
		ItemRewardVec rewards;
		rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * req.buyShopItemNum, 0, 0));
		player->AddItem(reason, rewards);

		OnPlayerBuyShopItemSuccess(player, shopData->shopType, shopData->shopId, shopItem->GetShopItemId(), req.buyShopItemNum);

		CLProtocol::WorldAccountShopItemBuyRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.queryIndex = req.queryIndex;
		res.buyShopItemId = req.buyShopItemId;
		res.buyShopItemNum = req.buyShopItemNum;
		res.accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
		res.roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);
		player->SendProtocol(res);
		InfoLogStream << PLAYERINFO(player) << "buy shop(" << req.queryIndex.shopId << ")'s item(" << req.buyShopItemId << "), buy num(" << req.buyShopItemNum << "), error code(" << (UInt32)resCode << ")." << LogStream::eos;
		return true;
	}	
	case ST_CHAMPION:
	{
		std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, req.buyShopItemNum);
		ItemRewardVec rewards;
		rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * req.buyShopItemNum, 0, 0));
		player->AddItem(reason, rewards);

		OnPlayerBuyShopItemSuccess(player, shopData->shopType, shopData->shopId, shopItem->GetShopItemId(), req.buyShopItemNum);

		CLProtocol::WorldAccountShopItemBuyRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.queryIndex = req.queryIndex;
		res.buyShopItemId = req.buyShopItemId;
		res.buyShopItemNum = req.buyShopItemNum;
		res.accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
		res.roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);
		player->SendProtocol(res);
		InfoLogStream << PLAYERINFO(player) << "buy shop(" << req.queryIndex.shopId << ")'s item(" << req.buyShopItemId << "), buy num(" << req.buyShopItemNum << "), error code(" << (UInt32)resCode << ")." << LogStream::eos;
		return true;
	}
	case ST_ADVENTURE_PASS:
	{
		std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, req.buyShopItemNum);
		ItemRewardVec rewards;
		rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * req.buyShopItemNum, 0, 0));
		player->AddItem(reason, rewards);

		OnPlayerBuyShopItemSuccess(player, shopData->shopType, shopData->shopId, shopItem->GetShopItemId(), req.buyShopItemNum);

		CLProtocol::WorldAccountShopItemBuyRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.queryIndex = req.queryIndex;
		res.buyShopItemId = req.buyShopItemId;
		res.buyShopItemNum = req.buyShopItemNum;
		res.accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
		res.roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);
		player->SendProtocol(res);
		InfoLogStream << PLAYERINFO(player) << "buy shop(" << req.queryIndex.shopId << ")'s item(" << req.buyShopItemId << "), buy num(" << req.buyShopItemNum << "), error code(" << (UInt32)resCode << ")." << LogStream::eos;
		return true;
	}	
	case ST_SECRET_SELL:
	{
		std::string reason = GetReason(SOURCE_TYPE_ACCOUNT_SHOP_BUY, shopItemData->id, req.buyShopItemNum);
		ItemRewardVec rewards;
		rewards.push_back(ItemReward(shopItemData->itemDatId, shopItemData->itemNum * req.buyShopItemNum, 0, 0));
		player->AddItem(reason, rewards);

		OnPlayerBuyShopItemSuccess(player, shopData->shopType, shopData->shopId, shopItem->GetShopItemId(), req.buyShopItemNum);

		CLProtocol::WorldAccountShopItemBuyRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.queryIndex = req.queryIndex;
		res.buyShopItemId = req.buyShopItemId;
		res.buyShopItemNum = req.buyShopItemNum;
		res.accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
		res.roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);
		player->SendProtocol(res);
		InfoLogStream << PLAYERINFO(player) << "buy shop(" << req.queryIndex.shopId << ")'s item(" << req.buyShopItemId << "), buy num(" << req.buyShopItemNum << "), error code(" << (UInt32)resCode << ")." << LogStream::eos;
		return true;
	}

	default:
		SendPlayerBuyShopItemResponse(player, req, ErrorCode::SHOP_BUY_ERR);
		return false;
	}
}

void WSShopMgr::TransToProtocolInfo(WSPlayer* player, const AccountShopItemDataEntry* shopItemData, CLProtocol::AccountShopItemInfo& shopItemInfo)
{
	if (!player || !shopItemData) return;

	shopItemInfo.shopItemId = shopItemData->id;
	shopItemInfo.shopItemName = shopItemData->shopItemName;
	shopItemInfo.tabType = shopItemData->tabType;
	shopItemInfo.jobType = shopItemData->jobType;
	shopItemInfo.itemDataId = shopItemData->itemDatId;
	shopItemInfo.itemNum = shopItemData->itemNum;
	shopItemInfo.costItems = shopItemData->costItemVec;

	shopItemInfo.accountRefreshType = shopItemData->accountRefreshType;
	shopItemInfo.accountRefreshTimePoint = shopItemData->accountRefreshTimePoint;

	shopItemInfo.accountLimitBuyNum = shopItemData->accountLimitBuyNum;
	shopItemInfo.accountRestBuyNum = _GetAccountRestBuyNum(player->GetAccId(), shopItemData);
	shopItemInfo.accountBuyRecordNextRefreshTimestamp = _GetAccountBuyRecordNextRefrshTime(player->GetAccId(), shopItemData);

	shopItemInfo.roleRefreshType = shopItemData->roleRefreshType;
	shopItemInfo.roleRefreshTimePoint = shopItemData->roleRefreshTimePoint;

	shopItemInfo.roleLimitBuyNum = shopItemData->roleLimitBuyNum;
	shopItemInfo.roleRestBuyNum = _GetRoleRestBuyNum(player->GetID(), shopItemData);
	shopItemInfo.roleBuyRecordNextRefreshTimestamp = _GetRoleBuyRecordNextRefreshTime(player->GetID(), shopItemData);

	shopItemInfo.extensibleCondition = shopItemData->extensibleCondition;
	shopItemInfo.needPreviewFunc = shopItemData->needPreviewFunc;
}

UInt32 WSShopMgr::_OnPlayerBuyOpActivityShopItem(WSPlayer* player, WSShop* shop, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum)
{
	if (!player || !shop || !shopData || !shopItemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt32 opActType = shopData->helpId;
	OpActivityRegInfo openOpAct;

	WSActivityMgr::Instance()->VisitOpActivity([&](const OpActivityRegInfo& opAct){
		if (opAct.tmpType != opActType)
		{
			return true;
		}

		if (opAct.state != AS_IN)
		{
			return true;
		}

		openOpAct = opAct;
		return false;
	});

	if (openOpAct.state != AS_IN)
	{
		ErrorLogStream << PLAYERINFO(player) << " opActivity(" << openOpAct.dataId << "," << openOpAct.tmpType << ") is not open, query type(" << opActType << ")!" << LogStream::eos;
		return ErrorCode::SHOP_ACTIVITY_CLOSE;
	}

	if (openOpAct.playerLevelLimit > 0 && openOpAct.playerLevelLimit > player->GetLevel())
	{
		ErrorLogStream << PLAYERINFO(player) << " level(" << player->GetLevel() << ") is not reach opActivity(" << openOpAct.dataId << ") level limit(" << openOpAct.playerLevelLimit << ")!" << LogStream::eos;
		return ErrorCode::SHOP_ACTIVITY_CLOSE;
	}

	UInt32 resCode = ErrorCode::SUCCESS;

	switch ((OpActivityType)openOpAct.tmpType)
	{
	case OAT_MALL_BUY_GOT:
		switch ((MallBuyGotType)openOpAct.parm)
		{
		case MALL_BUY_GOT_TYPE_GNOME_COIN:
			resCode = shop->OnBuyGnomeCoinShopItem(player, shopItemData, buyNum);
			break;

		default:
			ErrorLogStream << PLAYERINFO(player) << openOpAct.LogStr() << " mall buy got type(" << openOpAct.parm << ") is error!" << LogStream::eos;
			return ErrorCode::SHOP_ACTIVITY_CLOSE;
		}
		break;

	default:
		ErrorLogStream << PLAYERINFO(player) << " shop(" << shopData->shopId << ")'s helpId(" << shopData->helpId << ") is error!" << LogStream::eos;
		return ErrorCode::SHOP_ACTIVITY_CLOSE;
	}

	return resCode;
}

UInt32 WSShopMgr::_CheckOpActivityShopCond(WSPlayer* player, UInt32 opActType)
{
	if (!player) return ErrorCode::SHOP_ACTIVITY_CLOSE;

	OpActivityRegInfo openOpAct;

	WSActivityMgr::Instance()->VisitOpActivity([&](const OpActivityRegInfo& opAct){
		if (opAct.tmpType != opActType)
		{
			return true;
		}

		if (opAct.state != AS_IN)
		{
			return true;
		}

		openOpAct = opAct;
		return false;
	});

	if (openOpAct.state != AS_IN)
	{
		ErrorLogStream << PLAYERINFO(player) << " opActivity(" << openOpAct.dataId << "," << openOpAct.tmpType << ") is not open, query type(" << opActType << ")!" << LogStream::eos;
		return ErrorCode::SHOP_ACTIVITY_CLOSE;
	}

	if (openOpAct.playerLevelLimit > 0 && openOpAct.playerLevelLimit > player->GetLevel())
	{
		ErrorLogStream << PLAYERINFO(player) << " level(" << player->GetLevel() << ") is not reach opActivity(" << openOpAct.dataId << ") level limit(" << openOpAct.playerLevelLimit << ")!" << LogStream::eos;
		return ErrorCode::SHOP_ACTIVITY_CLOSE;
	}

	return ErrorCode::SUCCESS;
}

UInt32 WSShopMgr::_GetAccountRestBuyNum(UInt32 accid, const AccountShopItemDataEntry* shopItemData)
{
	if (!shopItemData) return 0;

	// 不限购
	if (shopItemData->accountLimitBuyNum == 0)
	{
		return UINT32_MAX;
	}

	auto record = FindShopItemAccountBuyRecord(accid, shopItemData->id);
	if (!record)
	{
		return shopItemData->accountLimitBuyNum;
	}

	record->TryRefresh((TimeUtil::RefreshType)shopItemData->accountRefreshType, shopItemData->accountRefreshDate);
	record->SaveToDB();

	// 账号下已购买数量
	UInt32 buyedNum = record->GetBuyedNum();
	if (buyedNum >= shopItemData->accountLimitBuyNum)
	{
		return 0;
	}

	return shopItemData->accountLimitBuyNum - buyedNum;
}

UInt32 WSShopMgr::_GetAccountBuyRecordNextRefrshTime(UInt32 accid, const AccountShopItemDataEntry* shopItemData)
{
	if (!shopItemData) return 0;

	UInt64 nextRefreshTimestamp = 0;

	auto record = FindShopItemAccountBuyRecord(accid, shopItemData->id);
	if (!record)
	{

		if ((TimeUtil::RefreshType)shopItemData->accountRefreshType == TimeUtil::RefreshType::REFRESH_TYPE_SEASON)
		{
			auto data = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(CURRENT_TIME);
			if (data == nullptr)
			{
				return 0;
			}
			nextRefreshTimestamp = static_cast<UInt64>(data->endTime) * Avalon::Time::MSECS_OF_SEC;
		}
		else
		{
			if (!TimeUtil::CalculateNextRefreshTimestamp((TimeUtil::RefreshType)shopItemData->accountRefreshType, shopItemData->accountRefreshDate, nextRefreshTimestamp))
			{
				ErrorLogStream << "Shop item(" << shopItemData->id << ") calculate next account refresh timestamp failed by refresh type("
					<< shopItemData->accountRefreshType << ") and (" << shopItemData->accountRefreshDate.LogStr() << ")!" << LogStream::eos;
				return 0;
			}
		}
	}
	else
	{
		nextRefreshTimestamp = record->GetRefreshTime();
	}

	return nextRefreshTimestamp / SEC_TO_MSEC;
}

UInt32 WSShopMgr::_GetRoleRestBuyNum(ObjID_t roleid, const AccountShopItemDataEntry* shopItemData)
{
	if (!shopItemData) return 0;

	// 不限购
	if (shopItemData->roleLimitBuyNum == 0)
	{
		return UINT32_MAX;
	}

	auto record = FindShopItemRoleBuyRecord(roleid, shopItemData->id);
	if (!record)
	{
		return shopItemData->roleLimitBuyNum;
	}

	record->TryRefresh((TimeUtil::RefreshType)shopItemData->roleRefreshType, shopItemData->roleRefreshDate);
	record->SaveToDB();

	// 账号下已购买数量
	UInt32 buyedNum = record->GetBuyedNum();
	if (buyedNum >= shopItemData->roleLimitBuyNum)
	{
		return 0;
	}

	return shopItemData->roleLimitBuyNum - buyedNum;
}

UInt32 WSShopMgr::_GetRoleBuyRecordNextRefreshTime(ObjID_t roleid, const AccountShopItemDataEntry* shopItemData)
{
	if (!shopItemData) return 0;

	UInt64 nextRefreshTimestamp = 0;

	auto record = FindShopItemRoleBuyRecord(roleid, shopItemData->id);
	if (!record)
	{
		if (!TimeUtil::CalculateNextRefreshTimestamp((TimeUtil::RefreshType)shopItemData->roleRefreshType, shopItemData->roleRefreshDate, nextRefreshTimestamp))
		{
			ErrorLogStream << "Shop item(" << shopItemData->id << ") calculate next account refresh timestamp failed by refresh type("
				<< shopItemData->roleRefreshType << ") and (" << shopItemData->roleRefreshDate.LogStr() << ")!" << LogStream::eos;
			return 0;
		}
	}
	else
	{
		nextRefreshTimestamp = record->GetRefreshTime();
	}

	return nextRefreshTimestamp / SEC_TO_MSEC;
}

UInt32 WSShopMgr::_CheckBuy(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req, const ShopDataEntry* shopData, AccountShopItemDataEntry* shopItemData, const WSShop* shop, WSShopItem* shopItem)
{
	if (!player || !shopData || !shopItemData || !shop || !shopItem) return ErrorCode::SHOP_BUY_ERR;

	if (req.buyShopItemNum > shopItemData->buyLimit && shopItemData->buyLimit != 0)
	{
		return ErrorCode::SHOP_BUY_ERR;
	}

	if (req.buyShopItemNum == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << " buy num is zero!" << LogStream::eos;
		return ErrorCode::SHOP_BUY_ERR;
	}

	if (shopData->bindType != 1)
	{
		ErrorLogStream << PLAYERINFO(player) << " shop(" << shopData->shopId << ")'s bind type(" << (UInt32)shopData->bindType << ") is error!" << LogStream::eos;
		return ErrorCode::SHOP_BUY_ERR;
	}

	if (!shopItemData->IsOnSale(player->GetLevel()))
	{
		return ErrorCode::SHOP_ITEM_IS_OFF_SALE;
	}

	if (!IsRestBuyNumEnough(player, shopItem, shopItemData, req.buyShopItemNum))
	{
		ErrorLogStream << PLAYERINFO(player) << shopItem->LogStr() << " rest buy num is not enough!" << LogStream::eos;
		return ErrorCode::SHOP_BUY_NOT_ENOUGH_ITEM;
	}

	return ErrorCode::SUCCESS;
}