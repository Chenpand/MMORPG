#include "ShopMgr.h"
#include "Player.h"
#include "CLRecordClient.h"
#include "CLShopItemDataEntry.h"
#include "ItemMaker.h"
#include "CLItemProtocol.h"
#include "CLErrorCode.h"
#include "CLSystemValueDataEntry.h"
#include "CLEventMgr.h"
#include "SSRouter.h"
#include "ActivityMgr.h"
#include "CLEquipInitAttriOnMakeDataEntry.h"
#include "SSApplication.h"

using namespace ErrorCode;
class DeleteShopItemCallback : public CLRecordCallback
{
public:
	DeleteShopItemCallback(Player* player, Shop* shop)
		:m_pOwner(player), m_shop(shop) {
		m_refreshTime = shop->GetRefreshTime().MSec();
	}
	~DeleteShopItemCallback() {}

private:

	void OnFinished()override
	{
		if (m_shop->GetRefreshTime().MSec() != m_refreshTime)
		{
			return;
		}

		if (m_shop->GetSyncDBState() == SSDS_NONE)
		{
			ErrorLogStream << "player(" << m_pOwner->GetID() << ") shop(" << m_shop->GetID() << ") nothing sync to db..." << LogStream::eos;
			return;
		}

		std::vector<ShopItem*> items;
		m_shop->GetShopItems(items);
		for (int i = 0; i < (int)items.size(); ++i)
		{
			ShopItem* item = items[i];
			if (!item)
				continue;

			CLSaveCommand* shopItemCmd = NULL;
			shopItemCmd = CLRecordClient::Instance()->PrepareInsertCommand(m_pOwner->GetSrcZoneId(), "t_shopitem", item->GetID());
			item->GetDBData(shopItemCmd, false);
			CLRecordClient::Instance()->SendCommand(shopItemCmd, NULL, m_pOwner->GetID());
		}

		m_shop->SetSyncDBState(SSDS_NONE);
	}

	void OnFailed(UInt32 errorcode)override
	{
		ErrorLogStream << "player(id = " << m_pOwner->GetID() << ") delete shopItem fail! errorcode=" << errorcode << LogStream::eos;
	}

private:
	//玩家id
	Player* m_pOwner;
	//商店
	Shop* m_shop;
	//商店刷新时间(保证异步回调使用的商店指针对象是同一个)
	UInt64 m_refreshTime;
};

class SelectShopItemCallback : public CLRecordCallback
{
public:
	SelectShopItemCallback(Player* player, Shop* shop)
		:m_pOwner(player), m_shop(shop) {}
	~SelectShopItemCallback() {}

private:

	void OnFinished()override
	{
		while (NextRow())
		{
			ShopItem* item = ShopItem::Create(GetData("shopitemid"));
			item->SetDBData(this);
			item->SetID(GetKey());
			m_shop->AddShopItem(item);
		}
	}

	void OnFailed(UInt32 errorcode)override
	{
		ErrorLogStream << "player(id = " << m_pOwner->GetID() << ") select shopItem fail! errorcode = " << errorcode << LogStream::eos;
	}

private:
	//玩家id
	Player* m_pOwner;
	//商店
	Shop* m_shop;
};

ShopMgr::ShopMgr()
{
	m_pOwner = NULL;
	m_shopList.clear();
}

ShopMgr::~ShopMgr()
{
	std::map<ObjID_t, Shop*>::iterator it = m_shopList.begin();
	for (; it != m_shopList.end(); ++it)
	{
		CL_SAFE_DELETE(it->second);
	}
	m_shopList.clear();
}

UInt32 ShopMgr::OnQueryItem(UInt8 shopId, UInt8 cache)
{
	auto shopData = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!shopData)
	{
		return SHOP_QUERY_ERR;
	}

	if (shopData->shopType == ST_DUNGEON)
	{
		Shop* shop = GetShop(shopId);
		if (!shop)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Query shop(" << shopId << ") failed!" << LogStream::eos;
			return SHOP_QUERY_ERR;
		}

		SyncClient(shop);

		return SUCCESS;
	}

	// 如果是请求公会商店，强制改成当前的公会商店ID
	if (shopData->isGuildShop)
	{
		if (GetOwner()->GetGuildPost() == 0)
		{
			return SHOP_GUIlD_SHOP_NOT_EXIST;
		}

		shopId = GetOwner()->GetGuildShopID();
	}

	bool isRefresh = false;
	Shop* shop = GetShop(shopId);
	if (!shop)
	{
		shop = RefreshShop(shopId, true, shopData->version);
		if (!shop)
			return SHOP_REFRESH_ERR;

		DebugLogStream << PLAYERINFO(GetOwner()) << " no shopData refresh shop suc!" << LogStream::eos;
		m_shopList[shop->GetID()] = shop;
		isRefresh = true;
	}
	else if (IsNeedRefresh(shop) ||
		shop->GetVersion() != shopData->version)
	{
		shop = RefreshShop(shopId, false, shopData->version);
		if (!shop)
			return SHOP_REFRESH_ERR;

		if (shop->GetVersion() != shopData->version)
		{
			DebugLogStream << PLAYERINFO(GetOwner()) << " version change refresh shop suc!" << LogStream::eos;
		}
		else
		{
			DebugLogStream << PLAYERINFO(GetOwner()) << " time's up refresh shop suc!" << LogStream::eos;
		}
		
		isRefresh = true;
	}

	if (isRefresh || !cache)
	{
		SyncClient(shop, false);	
	}
	else
	{
		SyncClient(shop);
	}

	return SUCCESS;
}

UInt32 ShopMgr::OnBuy(const CLProtocol::SceneShopBuy& protocol, UInt16& newNum, UInt32& leaseEndTimeStamp)
{
	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(protocol.shopId);
	if (!shopData)
		return SHOP_BUY_ERR;

	if (shopData->shopType == ST_ACTIVITY)
	{
		UInt32 activityId = shopData->helpId;
		if (!ActivityMgr::Instance()->IsInOpActivity(activityId))
		{
			return SHOP_ACTIVITY_CLOSE;
		}
	}

	UInt32 ret = ErrorCode::SHOP_BUY_ERR;

	for (const auto& costItem : protocol.costExtraItems)
	{
		Item* item = GetOwner()->GetItemMgr().FindItem(costItem.uid);
		if (!item)
		{
			continue;
		}

		UInt32 itemDataId = item->GetDataID();
		std::string reason = GetReason(SOURCE_TYPE_SHOP_BUY, protocol.shopId, itemDataId);

		if (GetOwner()->ReduceItemNum(reason.c_str(), item, costItem.num) != SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Reduce item(" << itemDataId << ") num(" << costItem.num << ") failed!" << LogStream::eos;
			return ret;
		}
	}

	UInt32 dungeonId = 0;
	
	//公会仓库商店生成物品后直接放入公会仓库
	if (ShopType::ST_GUILD_STORAGE == shopData->shopType)
	{
		ret = BuyGuildStorage(protocol.shopId, protocol.shopItemId, protocol.num);
		if (ret == ErrorCode::SUCCESS)
		{
			return ErrorCode::SHOP_WAIT_WORLD_RESPONSE;
		}
		else
		{
			return ret;
		}
	}
	else if (ShopType::ST_DUNGEON == shopData->shopType)
	{
		ret = BuyDungeonMarket(protocol.shopId, protocol.shopItemId, protocol.num, newNum);

		Shop* shop = GetShop(protocol.shopId);
		if (shop != NULL)
		{
			dungeonId = shop->GetDungeonId();
		}
	}
	else if (ShopType::ST_LEASE == shopData->shopType) //租赁商店
	{
		ret = BuyLeaseMarket(protocol.shopId, protocol.shopItemId, protocol.num, newNum, leaseEndTimeStamp);
	}
	//其他商店购买后进入背包
	else
	{
		if (0 != shopData->refresh)
		{
			ret = BuyBlackMarket(protocol.shopId, protocol.shopItemId, protocol.num, newNum);
		}
		else
		{
			ret = BuyNormalMarket(protocol.shopItemId, protocol.num);
			newNum = -1;
		}
	}
	
	if (ret == ErrorCode::SUCCESS)
	{
		GetOwner()->OnShopBuy(protocol.shopId, protocol.shopItemId, protocol.num, newNum, dungeonId);
	}

	return ret;
}

bool ShopMgr::LoadShop(Shop* shop)
{
	if (!shop)
		return false;

	auto itr = m_shopList.find(shop->GetID());
	if (itr != m_shopList.end())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Again add the same shop(" << shop->GetID() << ", " << shop->GetShopId() << ")!" << LogStream::eos;
		return false;
	}

	m_shopList[shop->GetID()] = shop;
	return true;
}

void ShopMgr::LoadShopItem(ShopItem* shopItem)
{
	if (!shopItem)
		return;

	Shop* shop = m_shopList[shopItem->GetShopId()];
	if (!shop)
	{
		CL_SAFE_DELETE(shopItem);
		return;
	}
		
	shop->AddShopItem(shopItem);
}

Shop* ShopMgr::GetShop(UInt8 shopId)
{
	std::map<ObjID_t, Shop*>::iterator it = m_shopList.begin();
	for (; it != m_shopList.end(); ++it)
	{
		if (it->second->GetShopId() == shopId)
			return it->second;
	}

	return NULL;
}

void ShopMgr::DeleteShop(UInt8 shopId)
{
	for (auto itr = m_shopList.begin(); itr != m_shopList.end();)
	{
		auto shop = itr->second;

		if (shop != NULL && shop->GetShopId() == shopId)
		{
			delete shop;
			itr = m_shopList.erase(itr);
			continue;
		}

		++itr;
	}
}

ShopItem* ShopMgr::GetShopItem(UInt8 shopId, UInt32 shopItemId)
{
	Shop* shop = GetShop(shopId);
	if (!shop)
		return NULL;

	return shop->GetShopItem(shopItemId);
}

void ShopMgr::OnQueryRefreshNum(UInt8 shopId, UInt8& restNum, UInt8& maxNum)
{
	auto shopData = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!shopData)
		return;

	UInt32 vipVal = 0;
	GetOwner()->GetVipValue(VIP_PRIVILEGE_MYSTERIOUS_SHOP_REFRESH, vipVal);

	UInt32 maxRefreshNum = shopData->refreshCost.size() + vipVal;
	UInt32 shopRefreshNum = GetOwner()->GetCounter(SHOP_REFRESH_NUM);
	restNum = shopRefreshNum < maxRefreshNum ? maxRefreshNum - shopRefreshNum : 0;
	maxNum = (UInt8)maxRefreshNum;
}

UInt32 ShopMgr::BuyNormalMarket(UInt32 shopItemId, UInt16 num)
{
	ShopItemDataEntry* shopItemData = ShopItemDataEntryMgr::Instance()->FindEntry(shopItemId);
	if (!shopItemData)
		return SHOP_BUY_ERR;

	// 安全锁开启，不允许消耗点券购买
	if (shopItemData->costItemId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	ShopItem shopItem;
	shopItem.SetDataEntry(shopItemData);
	shopItem.SetVipLv(0);

	UInt32 cost = 0;
	ItemDataEntry* costItem = NULL;
	//判断能买不
	UInt32 ck = CheckBuy(&shopItem, num, cost, costItem);
	if (ck != SUCCESS)
		return ck;

	//随从不走道具流程,直接由随从系统添加
	if (SIT_RETINUE == shopItemData->subType)
	{
		// check随从是否可添加
		UInt32 nRes = m_pOwner->CheckAddRetinueByItemId(shopItemData->itemId);
		if (nRes != ErrorCode::SUCCESS)
			return nRes;
			
	}
	else
	{
		if (!m_pOwner->CheckAddItem(shopItemData->itemId, num))
			return SHOP_BUY_NOT_ENOUGH_PACKSIZE;
	}
	
	std::string reason = GetReason(SOURCE_TYPE_SHOP_BUY, shopItemData->shopId, shopItemId);

	//扣钱
	if (shopItemData->useEqualItem)
	{
		if (m_pOwner->RemoveItemWithEqualItem(reason.c_str(), costItem->id, cost) == -1)
		{
			return SHOP_BUY_ERR;
		}
	}
	else
	{
		if (!m_pOwner->RemoveItem(reason.c_str(), costItem->id, cost))
		{
			return SHOP_BUY_ERR;
		}
	}

	if (SIT_RETINUE == shopItemData->subType)
	{
		m_pOwner->AddRetinueByItemId(shopItemData->itemId);
	}
	else
	{
		m_pOwner->AddItem(reason.c_str(), shopItemData->itemId, num);
	}

	return SUCCESS;
}

UInt32 ShopMgr::BuyBlackMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum)
{
	ShopItem* shopItem = GetShopItem(shopId, shopItemId);
	if (!shopItem || !shopItem->GetDataEntry())
		return SHOP_BUY_ERR;

	// 安全锁开启，不允许消耗点券购买
	if (shopItem->GetDataEntry()->costItemId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	UInt32 cost = 0;
	ItemDataEntry* costItem = NULL;

	//判断能买不
	UInt32 ck = CheckBuy(shopItem, num, cost, costItem);
	if (ck != SUCCESS)
	{
		return ck;
	}

	ck = CheckOtherCost(shopItem, num);
	if (ck != SUCCESS)
	{
		return ck;
	}

	UInt32 addNum = num * shopItem->GetDataEntry()->grpNum;

	//随从不走道具流程,直接由随从系统添加
	if (SIT_RETINUE == shopItem->GetDataEntry()->subType)
	{
		// check随从是否可添加
		UInt32 nRes = m_pOwner->CheckAddRetinueByItemId(shopItem->GetDataEntry()->itemId);
		if (nRes != ErrorCode::SUCCESS)
			return nRes;
	}
	else
	{
		if (!m_pOwner->CheckAddItem(shopItem->GetDataEntry()->itemId, addNum))
			return SHOP_BUY_NOT_ENOUGH_PACKSIZE;
	}
	
	std::string reason = GetReason(SOURCE_TYPE_SHOP_BUY, shopId, shopItemId);

	//扣钱
	if (shopItem->GetDataEntry()->useEqualItem)
	{
		if (m_pOwner->RemoveItemWithEqualItem(reason.c_str(), costItem->id, cost) == -1)
		{
			return SHOP_BUY_ERR;
		}
	}
	else
	{
		if (!m_pOwner->RemoveItem(reason.c_str(), costItem->id, cost))
		{
			return SHOP_BUY_ERR;
		}
	}

	if (RemoveOtherCost(shopItem, reason, num) != ErrorCode::SUCCESS)
	{
		return ErrorCode::SHOP_BUY_ERR;
	}

	if (SIT_RETINUE == shopItem->GetDataEntry()->subType)
	{
		m_pOwner->AddRetinueByItemId(shopItem->GetDataEntry()->itemId);
	}
	else
	{
		m_pOwner->AddItem(reason.c_str(), shopItem->GetDataEntry()->itemId, addNum);
	}

	shopItem->ReduceRestNum(num);

	//同步
	shopItem->SyncProperty(m_pOwner);
	newNum = shopItem->GetRestNum();
	
	GetOwner()->GetActiveTaskMgr().OnBuyBlackShop();

	return SUCCESS;
}

UInt32 ShopMgr::BuyGuildStorage(UInt8 shopId, UInt32 shopItemId, UInt16 num)
{
	ShopItem* shopItem = GetShopItem(shopId, shopItemId);
	if (!shopItem || !shopItem->GetDataEntry())
		return SHOP_BUY_ERR;

	// 安全锁开启，不允许消耗点券购买
	if (shopItem->GetDataEntry()->costItemId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	UInt32 cost = 0;
	ItemDataEntry* costItem = NULL;

	//判断能买不
	UInt32 ck = CheckBuy(shopItem, num, cost, costItem);
	if (ck != SUCCESS)
		return ck;

	UInt32 addNum = num * shopItem->GetDataEntry()->grpNum;

	//生成道具发送到world放入仓库
	Item* item = ItemMaker::Instance()->MakeItem(GetOwner(), shopItem->GetDataEntry()->itemId, addNum);
	if (!item)
	{
		return ErrorCode::SHOP_BUY_ERR;
	}

	Avalon::Packet* pPacket = Avalon::Packet::Create();
	pPacket->SetID(CLProtocol::SW_GUILDSTORAGE_ADD_REQ);
	Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());

	stream & GetOwner()->GetID();

	try
	{
		stream & item->GetID() & item->GetDataID();
		item->Encode(stream, SST_WORLD, false);
	}
	catch (Avalon::NetException& exception)
	{
		Avalon::Packet::Destroy(pPacket);
		ErrorLogStream << "shop buy add guild storage item encode error failed:" << exception.what() << "!" << LogStream::eos;
		CL_SAFE_DELETE(item);
		return ErrorCode::SHOP_BUY_ERR;
	}

	CL_SAFE_DELETE(item);
	
	stream & shopId & shopItemId & num & costItem->id & cost;
	pPacket->SetSize(stream.GetPos());
	Router::SendToWorld(pPacket);
	Avalon::Packet::Destroy(pPacket);

	return SUCCESS;
}

UInt32 ShopMgr::BuyDungeonMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum)
{
	auto shopData = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!shopData)
	{
		return SHOP_BUY_ERR;
	}

	ShopItem* shopItem = GetShopItem(shopId, shopItemId);
	if (!shopItem || !shopItem->GetDataEntry())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Shop item or data ptr is null!" << LogStream::eos;
		return SHOP_BUY_ERR;
	}

	// 安全锁开启，不允许消耗点券购买
	if (shopItem->GetDataEntry()->costItemId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	UInt32 cost = 0;
	ItemDataEntry* costItem = NULL;

	//判断能买不
	UInt32 ck = CheckBuy(shopItem, num, cost, costItem);
	if (ck != SUCCESS)
	{
		WarnLogStream << PLAYERINFO(GetOwner()) << "Check buy shop(" << shopId << ")'s item(" << shopItemId << "), error code=" << ck << LogStream::eos;
		return ck;
	}

	UInt32 addNum = num * shopItem->GetDataEntry()->grpNum;

	bool needSendMail = false;

	//随从不走道具流程,直接由随从系统添加
	if (SIT_RETINUE == shopItem->GetDataEntry()->subType)
	{
		// check随从是否可添加
		UInt32 nRes = m_pOwner->CheckAddRetinueByItemId(shopItem->GetDataEntry()->itemId);
		if (nRes != ErrorCode::SUCCESS)
		{
			WarnLogStream << PLAYERINFO(GetOwner()) << "Check add retinue shop(" << shopId << ")'s item(" << shopItemId << "), error code=" << nRes << LogStream::eos;
			return nRes;
		}
	}
	else
	{
		if (!m_pOwner->CheckAddItem(shopItem->GetDataEntry()->itemId, addNum))
			needSendMail = true;
	}

	shopItem->ReduceRestNum(num);

	GetOwner()->SyncSysRecordData(shopData->rareControlType, shopItem->GetDataEntry()->itemId, num);

	newNum = shopItem->GetRestNum();

	std::string reason = GetReason(SOURCE_TYPE_SHOP_BUY, shopId, shopItemId);

	//扣钱
	if (!m_pOwner->RemoveItem(reason.c_str(), costItem->id, cost))
	{
		return SHOP_BUY_ERR;
	}

	if (SIT_RETINUE == shopItem->GetDataEntry()->subType)
	{
		m_pOwner->AddRetinueByItemId(shopItem->GetDataEntry()->itemId);
	}
	else
	{
		RewardGroup* grp = GetOwner()->GetRewardGroup();
		grp->AddReward(shopItem->GetDataEntry()->itemId, addNum);
		GetOwner()->AddRewards(reason.c_str(), grp, true);
	}

	if (needSendMail)
	{
		GetOwner()->SendNotifyById(1245);
	}
	else
	{
		GetOwner()->SendNotifyById(1201);
	}

	DebugLogStream << PLAYERINFO(GetOwner()) << "Buy shop item(" << shopItem->GetShopItemId() << ", " << num << ") in shop(" << shopData->shopId << "), new num=" << newNum << "." << LogStream::eos;

	return SUCCESS;
}

UInt32 ShopMgr::BuyLeaseMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum, UInt32& leaseEndTimeStamp)
{
	ShopItem* shopItem = GetShopItem(shopId, shopItemId);
	if (!shopItem || !shopItem->GetDataEntry())
		return SHOP_BUY_ERR;

	// 安全锁开启，不允许消耗点券购买
	if (shopItem->GetDataEntry()->costItemId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	UInt32 cost = 0;
	ItemDataEntry* costItem = NULL;

	//判断能买不
	UInt32 ck = CheckBuy(shopItem, num, cost, costItem);
	if (ck != SUCCESS)
		return ck;

	UInt32 addNum = num * shopItem->GetDataEntry()->grpNum;

	//随从不走道具流程,直接由随从系统添加
	if (SIT_RETINUE == shopItem->GetDataEntry()->subType)
	{
		// check随从是否可添加
		UInt32 nRes = m_pOwner->CheckAddRetinueByItemId(shopItem->GetDataEntry()->itemId);
		if (nRes != ErrorCode::SUCCESS)
			return nRes;
	}
	else
	{
		if (!m_pOwner->CheckAddItem(shopItem->GetDataEntry()->itemId, addNum))
			return SHOP_BUY_NOT_ENOUGH_PACKSIZE;
	}

	std::string reason = GetReason(SOURCE_TYPE_SHOP_BUY, shopId, shopItemId);

	//扣钱
	if (!m_pOwner->RemoveItem(reason.c_str(), costItem->id, cost))
	{
		return SHOP_BUY_ERR;
	}

	if (SIT_RETINUE == shopItem->GetDataEntry()->subType)
	{
		m_pOwner->AddRetinueByItemId(shopItem->GetDataEntry()->itemId);
	}
	else
	{
		m_pOwner->AddItemWithCallBack(reason.c_str(), shopItem->GetDataEntry()->itemId, num,
			[&shopItem](Item& item){
			if (shopItem->GetDataEntry()->attriFit == 1)
			{
				class Visitor : public EquipInitAttriOnMakeDataEntryVisitor
				{
				public:
					Visitor(UInt32 shopId_, UInt32 itemId_)
						: shopId(shopId_), itemId(itemId_), dataEntry(NULL){};
					bool operator()(EquipInitAttriOnMakeDataEntry* dataEntry_)
					{
						if (dataEntry_ && dataEntry_->sourceId == shopId
							&& dataEntry_->itemId == itemId)
						{
							dataEntry = dataEntry_;
							return false;
						}
						return true;
					}

				public:
					UInt32 shopId;
					UInt32 itemId;
					EquipInitAttriOnMakeDataEntry* dataEntry;
				};

				Visitor visitor(shopItem->GetDataEntry()->shopId, shopItem->GetDataEntry()->itemId);
				EquipInitAttriOnMakeDataEntryMgr::Instance()->Visit(visitor);

				if (visitor.dataEntry)
				{
					item.SetStrengthenValue(visitor.dataEntry->strenthLv);
					UInt32 qualityLv = visitor.dataEntry->qualityLv;
					if (qualityLv > 0)
					{
						if (qualityLv > 100)	qualityLv = 100;
						item.SetQualityLv(qualityLv);
					}
					if (visitor.dataEntry->isLease == 1)
					{
						item.SetItemLock(ILT_LEASE_LOCK);
					}
					return true;
				}

			}
			return false;
		},

			[&shopItem, &shopId, this](Item& item){
			Shop* shop = GetShop(shopId);
			if (shop == NULL || shop->GetDataEntry() == NULL) return;
			if (shop->GetDataEntry()->shopType == ST_LEASE)
			{
				ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(shopItem->GetDataEntry()->itemId);
				if (itemData && itemData->type == ITEM_EQUIP)
				{
					shopItem->SetLeaseEndTimeStamp(item.GetDeadLine());
				}
			}
		}
		);

	}

	//同步
	shopItem->SyncProperty(m_pOwner);
	newNum = shopItem->GetRestNum();
	leaseEndTimeStamp = shopItem->GetLeaseEndTimeStamp();

	m_pOwner->SendUdpLog("item_lease", LOG_ITEM_LEASE, m_pOwner->GetItemNum(costItem->id) + cost, m_pOwner->GetItemNum(costItem->id), cost);

	return SUCCESS;
}

bool ShopMgr::OnRemoveExtraItemOnBuy(ShopItemDataEntry* shopItemDataEntry, const std::string& reason)
{
	if (!shopItemDataEntry) return false;

	for (const auto& costExtraItem : shopItemDataEntry->extraItemCost)
	{
		if (!GetOwner()->RemoveLessScoreItem(reason.c_str(), costExtraItem.id, costExtraItem.num))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove less score item(" << costExtraItem.id << "), " << costExtraItem.num << ") failed!" << LogStream::eos;
			return false;
		}
	}

	return true;
}

bool ShopMgr::IsNeedRefresh(Shop* shop)
{
	if (!shop)
		return false;

	auto shopDataEntry = shop->GetDataEntry();
	if (!shopDataEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "ShopDataEntry("<< shop->GetShopId() <<") ptr is null" << LogStream::eos;
		return false;
	}

	if (shopDataEntry->shopType == ST_LEASE)
	{
		if (GetOwner()->GetCounter(REFRESH_LEASE_SHOP) == 1)
		{
			GetOwner()->SetCounter(REFRESH_LEASE_SHOP, 0);
			return true;
		}
	}

	UInt8 cnt = 0;
	switch ((ShopRefreshType)shopDataEntry->refresh)
	{
	case SRT_NO_REFRESH: return false;
	case SRT_BLACK:
	case SRT_NORMAL:
	case SRT_NORMAL_MANUAL:
		for (auto refreshCycleType : shopDataEntry->refreshCycle)
		{
			if (_IsNeedRefreshOnRefreshCycle(shop, refreshCycleType))
			{
				shop->SetRefreshCycleTypeState(refreshCycleType, (UInt32)REFRESH_CYCLE_ACTIVATION);
				cnt++;
			}
			else
			{
				shop->SetRefreshCycleTypeState(refreshCycleType, (UInt32)REFRESH_CYCLE_QUIET);
			}
		}

		if (cnt > 0)
		{
			return true;
		}
		return false;

	default:
		ErrorLogStream << "In ShopTable.txt, refresh(" << shopDataEntry->refresh << ") is error!" << LogStream::eos;
		return false;
	}
 }

bool ShopMgr::_IsNeedRefreshOnRefreshCycle(Shop* shop, UInt32 refreshCycleType)
{
	if (!shop)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Shop ptr is null" << LogStream::eos;
		return false;
	}

	switch (refreshCycleType)
	{
	case (UInt32)REFRESH_CYCLE_NONE:
		break;

	case (UInt32)REFRESH_CYCLE_DAILY:
		if (CURRENT_TIME > shop->GetRefreshTime())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << ") need refresh shop(" << shop->GetShopId() << "), refreshCycleType(" << refreshCycleType
				<< "), currentTime(" << CURRENT_TIME.MSec() << "), dayRefreshTime(" << shop->GetRefreshTime().MSec() << ")." << LogStream::eos;
			return true;
		}
		break;

	case (UInt32)REFRESH_CYCLE_WEEK:
		if (CURRENT_TIME > shop->GetWeekRefreshTime())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << ") need refresh shop(" << shop->GetShopId() << "), refreshCycleType(" << refreshCycleType
				<< "), currentTime(" << CURRENT_TIME.MSec() << "), weekRefreshTime(" << shop->GetWeekRefreshTime().MSec() << ")." << LogStream::eos;
			return true;
		}
		break;

	case (UInt32)REFRESH_CYCLE_MONTH:
		if (CURRENT_TIME > shop->GetMonthRefreshTime())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << ") need refresh shop(" << shop->GetShopId() << "), refreshCycleType(" << refreshCycleType
				<< "), currentTime(" << CURRENT_TIME.MSec() << "), monthRefreshTime(" << shop->GetMonthRefreshTime().MSec() << ")." << LogStream::eos;
			return true;
		}
		break;

	default:
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Error refreshCycleType(" << refreshCycleType << ")!" << LogStream::eos;
		break;
	}

	return false;
}

Shop* ShopMgr::RefreshShop(UInt32 shopId, bool first, UInt32 version)
{
	if (GetOwner())
	{
		DebugLogStream << "player(" << GetOwner()->GetID() << ") start to refresh shop(" << shopId << ", " << first << ", " << version << ")." << LogStream::eos;
	}

	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!shopData)
		return NULL;

	//权重随机得到普通商品
	std::vector<ShopItemDataEntry*> commonItemDatas;
	if (shopData->shopType == ST_LEASE)
	{
		ShopItemDataEntryMgr::Instance()->GetShopItemsByOccu(shopId, GetOwner()->GetOccu(), commonItemDatas);
	}
	else
	{
		ShopItemDataEntryMgr::Instance()->GetShopCommonItems(shopId, commonItemDatas);
	}

	std::vector<UInt32> commonWts;
	std::vector<UInt32> commonVals;
	std::vector<UInt32> commonOut;
	for (int i = 0; i < (int)commonItemDatas.size(); ++i)
	{
		commonWts.push_back(commonItemDatas[i]->weight);
		commonVals.push_back(commonItemDatas[i]->shopItemId);
	}
	ItemMaker::Instance()->randByWeightCplx(commonVals, commonWts, commonOut, shopData->onSaleNum - shopData->vipInfos.size());

	//权重随机得到vip商品
	std::vector<ShopItemDataEntry*> vipItemDatas;
	ShopItemDataEntryMgr::Instance()->GetShopVipItems(shopId, vipItemDatas);
	std::vector<UInt32> vipWts;
	std::vector<UInt32> vipVals;
	std::vector<UInt32> vipOut;
	for (int i = 0; i < (int)vipItemDatas.size(); ++i)
	{
		vipWts.push_back(vipItemDatas[i]->weight);
		vipVals.push_back(vipItemDatas[i]->shopItemId);
	}
	ItemMaker::Instance()->randByWeightCplx(vipVals, vipWts, vipOut, shopData->vipInfos.size());

	UInt32 ttSz = (UInt32)commonOut.size() + (UInt32)vipOut.size();
	if (0 == ttSz)
		return NULL;

	//添加商品
	bool newShop = false;
	Shop* shop = GetShop(shopId);
	if (!shop)
	{
		shop = Shop::Create(shopId);
		shop->SetOwner(m_pOwner->GetID());
		shop->SetID(CLRecordClient::Instance()->GenGuid());
		shop->SetSyncDBState(SSDS_INSERT);
		newShop = true;
	}
	else
	{
		shop->SetDataEntry(shopData);
		shop->SetSyncDBState(SSDS_UPDATE);
	}
	
	//剔除已经无法重置的限次物品
	if (!first)
	{
		std::vector<ShopItem*> shopitems;
		shop->GetShopItems(shopitems);

		std::vector<UInt32>::iterator itr = commonOut.begin();

		for (; itr != commonOut.end();)
		{
			bool bErase = false;
			for (size_t i = 0; i < shopitems.size(); ++i)
			{
				ShopItem* item = shopitems[i];
				if (!item)
					continue;

				if (item->GetDataEntry()->limiteOnce == 0 &&
					item->GetRestNum() == 0 &&
					item->GetDataEntry()->shopItemId == *itr)
				{
					itr = commonOut.erase(itr);
					bErase = true;
					break;
				}
			}

			if (!bErase)
			{
				itr++;
			}
		}
	}

	shop->ClearUnRefreshShopItem();
	for (auto refreshCycleType : shopData->refreshCycle)
	{
		if (first) //第一次刷新商店
		{
			shop->SetRefreshCycleTypeState(refreshCycleType, REFRESH_CYCLE_ACTIVATION);
		}

		if (shop->GetRefreshCycleState(refreshCycleType) == (UInt32)REFRESH_CYCLE_QUIET)
		{
			if (!_KeepUnRefreshShopItemNum(shop, refreshCycleType))
			{
				if (newShop)
				{
					CL_SAFE_DELETE(shop);
				}
				return NULL;
			}
		}
	}

	shop->ClearItem();
	for (int i = 0; i < (int)commonOut.size(); ++i)
	{
		shop->AddShopItem(commonOut[i]);
	}

	if (shop->HasUnRefreshShopItem())
	{
		shop->SetUnRefreshShopItemRestNum();
	}

	for (int i = 0; i < (int)vipOut.size(); ++i)
	{
		shop->AddShopItem(vipOut[i]);
	}
	
	shop->SetRefreshTime();
	shop->SetVersion(version);
	//同步DB
	SyncDB(shop);

	return shop;
}

bool ShopMgr::_KeepUnRefreshShopItemNum(Shop* shop, UInt32 unRefreshCycleType)
{
	if (!shop)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Shop ptr is null" << LogStream::eos;
		return false;
	}

	auto shopDataEntry = shop->GetDataEntry();
	if (!shopDataEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "ShopDataEntry ptr is null" << LogStream::eos;
		return false;
	}

	std::vector<ShopItem*> shopItems;
	shop->GetShopItems(shopItems);

	for (auto shopItem : shopItems)
	{
		if (!shopItem)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "ShopItem ptr is null" << LogStream::eos;
			continue;
		}

		auto shopItemDataEntry = shopItem->GetDataEntry();
		if (!shopItemDataEntry)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "ShopItem ptr is null" << LogStream::eos;
			continue;
		}

		UInt32 subType = shopItemDataEntry->subType;

		auto iter = shopDataEntry->refreshCycleSub.find(subType);
		if (iter != shopDataEntry->refreshCycleSub.end())
		{
			UInt32 refreshCycleType = iter->second;
			if (unRefreshCycleType == refreshCycleType) //不刷新页签与商品所属页签对应
			{
				shop->KeepUnRefreshShopItemRestNum(shopItem->GetShopItemId(), shopItem->GetRestNum());
			}
		}
		else
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " In shop(" << shop->GetShopId() << "), can not find refreshCycle(" << subType << ")!" << LogStream::eos;
			return false;
		}
	}
	return true;
}

UInt32 ShopMgr::OnRefreshShop(UInt8 shopId)
{
	Shop* shop = GetShop(shopId);
	if (!shop || !shop->GetDataEntry())
		return SHOP_REFRESH_ERR;

	ShopDataEntry* shopData = shop->GetDataEntry();
	if (!shopData)
		return ITEM_DATA_INVALID;

	//能否手动刷新
	if (!_CanManualRefresh(shopData->refresh))
		return SHOP_REFRESH_ERR;

	//黑市刷新判断刷新次数
	if (shopData->refresh == ShopRefreshType::SRT_BLACK)
	{
		UInt32 shopRefreshNum = GetOwner()->GetCounter(SHOP_REFRESH_NUM);

		//刷新次数判断
		UInt32 vipVal = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_MYSTERIOUS_SHOP_REFRESH, vipVal);
		if (shopRefreshNum >= shop->GetDataEntry()->refreshCost.size() + vipVal)
		{
			return SHOP_REFRESH_COUNT;
		}
	}
	

	UInt32 cost = GetRefreshCost(shop);

	// 刷新扣钱
	if (cost > m_pOwner->GetPointBoth())
		return ITEM_NOT_ENOUGH_POINT;

	for (auto refreshCycleType : shopData->refreshCycle)
	{
		shop->SetRefreshCycleTypeState(refreshCycleType, (UInt32)REFRESH_CYCLE_ACTIVE);
	}

	shop = RefreshShop(shopId, false, shopData->version);
	if (!shop)
		return SHOP_REFRESH_ERR;

	//黑市刷新增加刷新次数
	if (shopData->refresh == ShopRefreshType::SRT_BLACK)
	{
		GetOwner()->IncCounter(SHOP_REFRESH_NUM, 1);
	}
	

	m_pOwner->RemovePointBoth(GetReason(LogSourceType::SOURCE_TYPE_SHOP_REFRESH, shopId).c_str(), cost);

	SyncClient(shop);

	m_pOwner->OnRefreshShop();

	return SUCCESS;
}

void ShopMgr::SyncClient(Shop* shop, bool cache)
{
	if (!m_pOwner || !shop)
		return;

	std::vector<ShopItem*> shopItems;

	if (!cache)
	{
		shop->GetShopItems(shopItems);
	}
	
	CLProtocol::SceneShopSync	protocol;
	protocol.shopId = shop->GetShopId();
	protocol.refreshCost = GetRefreshCost(shop);

	for (int i = 0; i < (int)shopItems.size(); ++i)
	{
// 		if (!shopItems[i] || shopItems[i]->GetRestNum() == 0)
// 			continue;
		
		if (shopItems[i]->GetDataEntry()->limiteOnce == 0 && 
			shopItems[i]->GetRestNum() == 0)
		{
			continue;
		}

		if (shop->GetDataEntry()->shopType == ST_LEASE
			&& shopItems[i]->GetDataEntry()->playerLevelLimit.size() == 2
			&& (GetOwner()->GetLevel() > shopItems[i]->GetDataEntry()->playerLevelLimit[1]
			|| GetOwner()->GetLevel() < shopItems[i]->GetDataEntry()->playerLevelLimit[0])
			)
		{
			continue;
		}

		CLProtocol::ProtoShopItem protoItem;
		ShopItem* shopItem = shopItems[i];
		protoItem.shopItemId = shopItem->GetDataEntry()->shopItemId;
		protoItem.grid = shopItem->GetGrid();
		protoItem.restNum = shopItem->GetRestNum();
		protoItem.vipLv = shopItem->GetVipLv();
		protoItem.vipDiscount = shopItem->GetVipDisCount();
		protoItem.discountRate = shopItem->GetDiscountRate();
		protoItem.leaseEndTimeStamp = shopItem->GetLeaseEndTimeStamp();

		protocol.shopItemList.push_back(protoItem);
	}
	
	protocol.restRefreshTime = (UInt32)(shop->GetRefreshTime().Sec() - CURRENT_DATE.ToTime().Sec());
	protocol.restWeekRefreshTime = (UInt32)(shop->GetWeekRefreshTime().Sec() - CURRENT_DATE.ToTime().Sec());
	protocol.restMonthRefreshTime = (UInt32)(shop->GetMonthRefreshTime().Sec() - CURRENT_DATE.ToTime().Sec());

	UInt32 vipVal = 0;
	GetOwner()->GetVipValue(VIP_PRIVILEGE_MYSTERIOUS_SHOP_REFRESH, vipVal);

	UInt32 maxRefreshNum = shop->GetDataEntry()->refreshCost.size() + vipVal;
	UInt32 shopRefreshNum = GetOwner()->GetCounter(SHOP_REFRESH_NUM);
	protocol.restRefreshNum = shopRefreshNum < maxRefreshNum ? maxRefreshNum - shopRefreshNum : 0;
	protocol.maxRefreshNum = (UInt8)maxRefreshNum;

	m_pOwner->SendProtocol(protocol);
}

void ShopMgr::SyncDB(Shop* shop)
{
	if (!m_pOwner || !shop)
		return;

	ShopSyncDBState syncDBState = shop->GetSyncDBState();
	//同步shop表
	CLSaveCommand* shopCmd = NULL;

	if (SSDS_INSERT == syncDBState)
	{
		shopCmd = CLRecordClient::Instance()->PrepareInsertCommand(m_pOwner->GetSrcZoneId(), "t_shop", shop->GetID());
	}
	else if (SSDS_UPDATE == syncDBState)
	{
		shopCmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_pOwner->GetSrcZoneId(), "t_shop", shop->GetID(), false);
	}

	shop->GetDBData(shopCmd, false);
	CLRecordClient::Instance()->SendCommand(shopCmd, NULL, m_pOwner->GetID());

	DebugLogStream << "player(" << m_pOwner->GetID() << ") start to delete shopitems(" << shop->GetID() << ")." << LogStream::eos;

	std::ostringstream cond;
	cond << "shopid = " << shop->GetID() << " AND owner = " << m_pOwner->GetID();
	CLDeleteCommand* delCmd = CLRecordClient::Instance()->PrepareDeleteCommand(m_pOwner->GetSrcZoneId(), "t_shopitem", cond.str());

	if (!SSApplication::Instance()->IsUseNewShopRefresh())
	{
		// 更新shopitem表
		CLRecordClient::Instance()->SendCommand(delCmd, new DeleteShopItemCallback(m_pOwner, shop), m_pOwner->GetID());
		return;
	}
	
	// 新的商店刷新方式
		// 1. 删除商店道具
		// 2. 立马添加新的道具

	CLRecordClient::Instance()->SendCommand(delCmd, NULL, m_pOwner->GetID());

	do
	{
		if (shop->GetSyncDBState() == SSDS_NONE)
		{
			ErrorLogStream << "player(" << m_pOwner->GetID() << ") shop(" << shop->GetID() << ") nothing sync to db..." << LogStream::eos;
			break;
		}

		std::vector<ShopItem*> items;
		shop->GetShopItems(items);
		for (int i = 0; i < (int)items.size(); ++i)
		{
			ShopItem* item = items[i];
			if (!item)
				continue;

			DebugLogStream << "player(" << m_pOwner->GetID() << ")insert shop item(" << item->GetID() << ")." << LogStream::eos;

			CLSaveCommand* shopItemCmd = NULL;
			shopItemCmd = CLRecordClient::Instance()->PrepareInsertCommand(m_pOwner->GetSrcZoneId(), "t_shopitem", item->GetID());
			item->GetDBData(shopItemCmd, false);
			CLRecordClient::Instance()->SendCommand(shopItemCmd, NULL, m_pOwner->GetID());
		}

		shop->SetSyncDBState(SSDS_NONE);
	} while (0);
}

UInt32 ShopMgr::CheckBuy(ShopItem* shopItem, UInt16 num, UInt32& cost, ItemDataEntry*& costItem)
{
	UInt32 costItemId = shopItem->GetDataEntry()->costItemId;
	UInt32 costNum = shopItem->GetDataEntry()->costNum;
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(costItemId);
	if (itemData == NULL) return SHOP_BUY_ERR;

	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(shopItem->GetDataEntry()->shopId);
	if (shopData == NULL) return SHOP_BUY_ERR;

	if (shopData->shopType == ST_LEASE)
	{
		if (shopItem->GetLeaseEndTimeStamp() >= CURRENT_TIME.Sec())
		{
			return	SHOP_IN_LEASEING;
		}
	}

	if (shopItem->GetRestNum() >= 0 && num > shopItem->GetRestNum())
	{
		return SHOP_BUY_SALE_OUT;	
	}
	else
	{
		if (itemData->type != ITEM_INCOME)
		{
			if (shopItem->GetDataEntry()->grpNum * num > itemData->maxNum)
			{
				return SHOP_BUY_ERR;
			}
		}
	}

	if (shopItem->GetDiscountRate() > 0)
	{
		costNum = (UInt32)(costNum * ((float)shopItem->GetDiscountRate() / DISCOUNT_RATE_DENOMINATOR));
	}

	//vip权限
	if (shopItem->GetDataEntry()->vipLimite > 0)
	{
		bool canBuyVipLimite = false;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_SHOP_LIMIT, canBuyVipLimite);
		if (!canBuyVipLimite)
		{
			return PLAYER_VIPLV_NOT_ENOUGH;
		}
	}
	
	//有特殊条件的商品
	switch (shopItem->GetDataEntry()->exLimite)
	{
		case SIE_NONE:
			break;
		case SIE_DUEL_POINT:
		{
			SeasonLevelDataEntry* seasonData = SeasonLevelDataEntryMgr::Instance()->FindEntry(shopItem->GetDataEntry()->exLimiteVal);
			if (!seasonData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "seasondata err! key:" << shopItem->GetDataEntry()->exLimiteVal << LogStream::eos;
				return SHOP_BUY_ERR;
			}
			if (shopItem->GetDataEntry()->exLimiteVal > GetOwner()->GetSeasonLevel())
				return SHOP_NOT_ENOUGH_DUEL_POINT;
			break;
		}
		case SIE_TOWER_LEVEL:
		{
			if (shopItem->GetDataEntry()->exLimiteVal > GetOwner()->GetDeathTowerTopFloor())
				return SHOP_NOT_ENOUGH_TOWER_LEVEL;
			break;
		}
		case SIE_GUILD_SHOP_LV:
		{
			if (shopItem->GetDataEntry()->exLimiteVal > GetOwner()->GetGuildBuildingLevel(GUILD_BUILDING_SHOP))
				return SHOP_GUIlD_NOT_ENOUGH_LV;
			break;
		}
		case SIE_HONOR_LEVEL:
		{
			if (shopItem->GetDataEntry()->exLimiteVal > GetOwner()->GetHonorLevel())
				return SHOP_BUY_HONOR_LVL_NOT_ENOUGH;
			break;
		}
		default:
		{
			GetOwner()->SendNotify("shop data error! exLimite = {0}", shopItem->GetDataEntry()->exLimite); 
			return SHOP_BUY_ERR;
		}
	}
	
	//vip商品
	UInt8 itemVipLv = shopItem->GetVipLv();
	if (itemVipLv > 0)
	{
		if (m_pOwner->GetVipLvl() < itemVipLv)
			return PLAYER_VIPLV_NOT_ENOUGH;

		float disPrice = costNum * (float)(shopItem->GetVipDisCount() / 100);
		costNum = (UInt32)(disPrice + 0.5);
	}
	
	cost = costNum * num;
	costItem = itemData;

	//花费钱购买
	if (ITEM_INCOME == itemData->type)
	{
		if (!GetOwner()->CheckRmMoney((ItemSubType)itemData->subType, cost))
		{
			if (ST_ITEM_GOLD == itemData->subType)			return ITEM_NOT_ENOUGH_GOLD;
			else if (ST_BIND_GOLD == itemData->subType)		return ITEM_NOT_ENOUGH_GOLD;
			else if (ST_ITEM_POINT == itemData->subType)	return ITEM_NOT_ENOUGH_POINT;
			else if (ST_BIND_POINT == itemData->subType)	return ITEM_NOT_ENOUGH_POINT;
			else if (ST_WARRIOR_SOUL == itemData->subType)	return ITEM_NOT_ENOUGH_WARRIOR_SOUL;
			else if (ST_PK_COIN == itemData->subType)		return ITEM_NOT_ENOUGH_PKCOIN;
			else if (ST_GUILD_CONTRI == itemData->subType)	return ITEM_NOT_ENOUGH_GUILD_CONTRI;
			else if (ST_MASTER_GOODTEACH_VALUE == itemData->subType) return ITEM_NOT_ENOUGH_TEACHVALUE;
			else											return ITEM_NOT_ENOUGH_MONEY;
		}
	}
	else	//花费道具购买
	{
		UInt32 itemNum = 0;
		if (shopItem->GetDataEntry()->useEqualItem)
		{
			itemNum = m_pOwner->GetItemNumWithEqualItem(costItemId);
		}
		else
		{
			itemNum = m_pOwner->GetItemNum(costItemId);
		}

		if (itemNum < cost)
			return SHOP_BUY_NOT_ENOUGH_ITEM;
	}

	return SUCCESS;
}

UInt16 ShopMgr::GetRefreshCost(Shop* shop)
{
	UInt16 cost = 0;

	if (!shop)
	{
		return cost;
	}
		
	if (!shop->GetDataEntry())
	{
		return cost;
	}
	
	if (shop->GetDataEntry()->refreshCost.size() == 0)
	{
		return cost;
	}

	//黑市
	if (shop->GetDataEntry()->refresh == ShopRefreshType::SRT_BLACK)
	{
		UInt32 costIdx = GetOwner()->GetCounter(SHOP_REFRESH_NUM);
		if (costIdx >= shop->GetDataEntry()->refreshCost.size())
		{
			costIdx = shop->GetDataEntry()->refreshCost.size() - 1;
		}

		cost = shop->GetDataEntry()->refreshCost[costIdx];
	}
	else
	{
		cost = shop->GetDataEntry()->refreshCost[0];
	}
	
	return cost;
}

bool ShopMgr::_CanManualRefresh(ShopRefreshType type)
{
	if (type == ShopRefreshType::SRT_BLACK ||
		type == ShopRefreshType::SRT_NORMAL_MANUAL)
	{
		return true;
	}

	return false;
}



UInt32 ShopMgr::CheckOtherCost(ShopItem* shopItem, UInt16 num)
{
	auto shopItemData = shopItem->GetDataEntry();
	if (shopItemData == NULL)
	{
		return ErrorCode::SHOP_BUY_ERR;
	}

	for (auto costItem : shopItemData->otherItemCost)
	{
		UInt32 itemNum = 0;
		if (shopItem->GetDataEntry()->useEqualItem)
		{
			itemNum = m_pOwner->GetItemNumWithEqualItem(costItem.id);
		}
		else
		{
			itemNum = m_pOwner->GetItemNum(costItem.id);
		}

		UInt32 needNum = costItem.num * num;
		if (itemNum < needNum)
		{
			return ErrorCode::SHOP_BUY_NOT_ENOUGH_ITEM;
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 ShopMgr::RemoveOtherCost(ShopItem* shopItem, const std::string& reason, UInt16 num)
{
	auto shopItemData = shopItem->GetDataEntry();
	if (shopItemData == NULL)
	{
		return ErrorCode::SHOP_BUY_ERR;
	}

	for (auto costItem : shopItemData->otherItemCost)
	{
		UInt32 needNum = costItem.num * num;
		if (shopItemData->useEqualItem)
		{
			if (m_pOwner->RemoveItemWithEqualItem(reason.c_str(), costItem.id, needNum) < 0)
			{
				ErrorLogStream << "player(" << m_pOwner->GetID() << ") request buy shop item(" << shopItemData->shopItemId << ") failed, remove cost(" << costItem.id << ", " << needNum << ") failed." << LogStream::eos;
				return ErrorCode::SHOP_BUY_ERR;
			}
		}
		else
		{
			if (!m_pOwner->RemoveItem(reason.c_str(), costItem.id, needNum))
			{
				ErrorLogStream << "player(" << m_pOwner->GetID() << ") request buy shop item(" << shopItemData->shopItemId << ") failed, remove cost(" << costItem.id << ", " << needNum << ") failed." << LogStream::eos;
				return ErrorCode::SHOP_BUY_ERR;
			}
		}
	}

	return ErrorCode::SUCCESS;
}
void ShopMgr::OnDayChange()
{
// 	for (std::map<ObjID_t, Shop*>::iterator itr = m_shopList.begin();
// 		itr != m_shopList.end(); ++itr)
// 	{
// 		Shop* sp = itr->second;
// 		if (!sp)
// 			continue;
// 
// 		sp->SetRefreshNum(0);
// 	}
}

void ShopMgr::OnOnline(bool bDayChange)
{
// 	if (bDayChange)
// 	{
// 		for (std::map<ObjID_t, Shop*>::iterator itr = m_shopList.begin();
// 			itr != m_shopList.end(); ++itr)
// 		{
// 			Shop* sp = itr->second;
// 			if (!sp)
// 				continue;
// 
// 			sp->SetRefreshNum(0);
// 		}
// 	}
}

void ShopMgr::OnDungeonRaceEnd(UInt32 dungeonId, UInt32 mysticalMerchantId)
{
	// 随机出神秘商人
	//UInt32 mysticalMerchantId = _RandMysticalMerchant();

	if (mysticalMerchantId == 0) return;

	MysticalMerchantDataEntry* merchantData = MysticalMerchantDataEntryMgr::Instance()->FindEntry(mysticalMerchantId);
	if (!merchantData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find mystical merchant data(" << mysticalMerchantId << ")." << LogStream::eos;
		return;
	}

	DebugLogStream << PLAYERINFO(GetOwner()) << "Dungeon race(" << dungeonId << ") end, mystical merchant(" << mysticalMerchantId << ")." << LogStream::eos;

	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(merchantData->shopId);
	if (!shopData)
	{
		return;
	}

	class ShopItemDataVistor : public Avalon::DataEntryVisitor<ShopItemDataEntry>
	{
	public:
		ShopItemDataVistor(UInt32 shopId)
			: m_ShopId(shopId) {}

	public:
		bool operator()(ShopItemDataEntry* data)
		{
			if (data != NULL && data->shopId == m_ShopId)
			{
				datas.insert(data->itemId);
			}

			return true;
		}

		std::set<UInt32> datas;

	private:
		UInt32 m_ShopId;
	};

	ShopItemDataVistor vistor(merchantData->shopId);
	ShopItemDataEntryMgr::Instance()->Visit(vistor);

	// sysrecord 参数
	std::vector<UInt32> params;

	for (auto shopItemId : vistor.datas)
	{
		params.push_back(shopItemId);
	}

	if (!params.empty())
	{
		std::vector<UInt32> extraParam = { mysticalMerchantId, dungeonId };

		GetOwner()->RequestSysRecordData(SYS_RECORD_EVENT_MYSTICAL_MERCHANT, shopData->rareControlType, params, extraParam);

		InfoLogStream << PLAYERINFO(GetOwner()) << "Start request sys record data..." << LogStream::eos;
	}
}

void ShopMgr::BuyGuildStorageCb(UInt32 retCode, UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt32 costId, UInt32 costNum)
{
	CLProtocol::SceneShopBuyRet ret;
	UInt32 newNum = 0;

	if (retCode != ErrorCode::SUCCESS)
	{
		ret.code = retCode;
		GetOwner()->SendProtocol(ret);
		return;
	}

	std::string reason = GetReason(SOURCE_TYPE_SHOP_BUY, shopId, shopItemId);

	//world成功就扣钱
	if (!m_pOwner->RemoveItem(reason.c_str(), costId, costNum))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " BuyGuildStorageCb remove("<< costId << "," << costNum <<") cost failed!" << LogStream::eos;
		return;
	}

	ShopItem* shopItem = GetShopItem(shopId, shopItemId);
	if (!shopItem || !shopItem->GetDataEntry())
	{
		ret.code = ErrorCode::SHOP_BUY_ERR;
		GetOwner()->SendProtocol(ret);
		return;
	}
	
	// 更新商品可购数量
	shopItem->ReduceRestNum(num);
	shopItem->SyncProperty(m_pOwner);
	newNum = shopItem->GetRestNum();

	ret.code = ErrorCode::SUCCESS;
	ret.shopItemId = shopItemId;
	ret.newNum = newNum;
	GetOwner()->SendProtocol(ret);
}

void ShopMgr::OnSysRecordDataRes(const CLProtocol::SceneSysRecordDataRes& res)
{
	switch ((SysRecordEvent)res.sysRecordEvent)
	{
	case SYS_RECORD_EVENT_NULL:
		break;
	case SYS_RECORD_EVENT_MYSTICAL_MERCHANT:
		_UpdateMysticalMerchantShopItemNum(res);
		break;
	default:
		break;
	}
}

UInt32 ShopMgr::_RandMysticalMerchant()
{
	UInt32 id = 0;

	class MysticalMerchantDataVistor : public Avalon::DataEntryVisitor<MysticalMerchantDataEntry>
	{
	public:
		bool operator()(MysticalMerchantDataEntry* dataEntry)
		{
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<MysticalMerchantDataEntry*> datas;
	};

	MysticalMerchantDataVistor vistor;
	MysticalMerchantDataEntryMgr::Instance()->Visit(vistor);

	// 权重集
	std::vector<UInt32> wts;
	// 随机阈值
	Int32 randThreshold = vistor.datas.size();

	for (auto data : vistor.datas)
	{
		if (!data) continue;

		wts.push_back(data->weight);
	}

	if (wts.empty())
	{
		ErrorLogStream << "Weights are empty!" << LogStream::eos;
		return id;
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= randThreshold || hitIdx < 0)
	{
		ErrorLogStream << "Rand weight failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return id;
	}

	auto hitData = vistor.datas[hitIdx];
	if (!hitData)
	{
		ErrorLogStream << "Hit data is null, hitIdx=" << hitIdx << LogStream::eos;
		return id;
	}

	id = hitData->id;

	return id;
}

void ShopMgr::_UpdateMysticalMerchantShopItemNum(const CLProtocol::SceneSysRecordDataRes& res)
{
	if (res.extraParam.size() != 2)
	{
		return;
	}

	UInt32 mysticalMerchantId = res.extraParam[0];
	UInt32 dungeonId = res.extraParam[1];

	MysticalMerchantDataEntry* merchantData = MysticalMerchantDataEntryMgr::Instance()->FindEntry(mysticalMerchantId);
	if (!merchantData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find mystical merchant data(" << mysticalMerchantId << ")." << LogStream::eos;
		return;
	}

	ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry(merchantData->shopId);
	if (!shopData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find shop data(" << merchantData->shopId << ")." << LogStream::eos;
		return;
	}

	DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find dungeon data(" << dungeonId << ")." << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "Dungeon race(" << dungeonId << "), mystical merchant(" << mysticalMerchantId << "), shop(" << shopData->shopId << ")." << LogStream::eos;

	class ShopItemDataVistor : public Avalon::DataEntryVisitor<ShopItemDataEntry>
	{
	public:
		ShopItemDataVistor(UInt32 shopId, Player* owner, DungeonDataEntry* dungeon)
			: m_ShopId(shopId), m_Owner(owner), m_DungeonData(dungeon){}

	public:
		bool operator()(ShopItemDataEntry* data)
		{
			if (!m_Owner || !m_DungeonData || !data) return true;

			if (data->shopId != m_ShopId) return true;

			// 按照商品出现的条件筛选出符合的商品

			// 角色等级
			if (data->playerLevelLimit.size() == 2)
			{
				UInt16 minLevelLimit = data->playerLevelLimit[0];
				UInt16 maxLevelLimit = data->playerLevelLimit[1];

				if (m_Owner->GetLevel() < minLevelLimit || m_Owner->GetLevel() > maxLevelLimit)
				{
					return true;
				}
			}

			UInt16 vipLevel = m_Owner->GetVipLvl();

			// vip等级
			if (data->vipLevelLimit.size() == 1)
			{
				if (vipLevel != data->vipLevelLimit[0])
				{
					return true;
				}
			}
			if (data->vipLevelLimit.size() == 2)
			{
				UInt8 minLevelLimit = data->vipLevelLimit[0];
				UInt8 maxLevelLimit = data->vipLevelLimit[1];

				if (vipLevel < minLevelLimit || vipLevel > maxLevelLimit)
				{
					return true;
				}
			}

			// 地下城id
			if (!data->dungeonIdLimit.empty())
			{
				if (std::find(data->dungeonIdLimit.begin(), data->dungeonIdLimit.end(),
					m_DungeonData->id) == data->dungeonIdLimit.end())
				{
					return true;
				}
			}

			if (!data->dungeonHardLimit.empty())
			{
				// 地下城难度
				if (std::find(data->dungeonHardLimit.begin(), data->dungeonHardLimit.end(),
					(UInt8)m_DungeonData->hardType) == data->dungeonHardLimit.end())
				{
					return true;
				}
			}
			if (!data->dungeonSubTypeLimit.empty())
			{
				// 地下城子类型
				if (std::find(data->dungeonSubTypeLimit.begin(), data->dungeonSubTypeLimit.end(),
					(UInt8)m_DungeonData->subType) == data->dungeonSubTypeLimit.end())
				{
					return true;
				}
			}

			DebugLogStream << PLAYERINFO(m_Owner) << "Dungeon(id=" << m_DungeonData->id << ", hard=" << m_DungeonData->hardType << "), subType=" << m_DungeonData->subType 
				<< "), Shop(" << m_ShopId << ") add item(" << data->shopId << ")." << LogStream::eos;

			datas[data->itemId].push_back(data);

			return true;
		}

		std::map<UInt32, std::vector<ShopItemDataEntry*> > datas;

	private:
		UInt32 m_ShopId;
		Player* m_Owner;
		DungeonDataEntry* m_DungeonData;
	};

	ShopItemDataVistor vistor(merchantData->shopId, GetOwner(), dungeonData);
	ShopItemDataEntryMgr::Instance()->Visit(vistor);

	if (vistor.datas.empty()) return;

	// 按权重随机商品
	std::vector<UInt32> commonWts;
	std::vector<UInt32> commonVals;
	std::vector<UInt32> commonOut;
	for (auto elem : vistor.datas)
	{
		auto dataVec = elem.second;

		if (dataVec.empty()) continue;

		std::vector<UInt32> wts;

		for (auto data : dataVec)
		{
			if (!data) continue;
			wts.push_back(data->weight);
		}

		Int32 hitIdx = WeightRander::RandWeight(wts);
		if (hitIdx >= (Int32)dataVec.size() || hitIdx < 0)
		{
			ErrorLogStream << "Rand failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
			continue;
		}

		auto data = dataVec[hitIdx];
		if (!data) continue;

		bool findZeroRemainNumData = false;

		// 稀有控制
		for (auto record : res.records)
		{
			// 稀有控制中的参数是道具id
			if (record.param == data->itemId)
			{
				// 如果稀有控制剩余数量为0,则不再随机
				if (record.remainNum == 0)
				{
					findZeroRemainNumData = true;
					continue;
				}
			}
		}

		if (findZeroRemainNumData) continue;

		commonWts.push_back(data->weight);
		commonVals.push_back(data->shopItemId);
	}
	ItemMaker::Instance()->randByWeightCplx(commonVals, commonWts, commonOut, shopData->onSaleNum - shopData->vipInfos.size());

	if (commonOut.empty()) return;

	InfoLogStream << PLAYERINFO(GetOwner()) << "Success rand shop items, shopid=" << shopData->shopId << ", items size=" << (UInt32)commonOut.size() << LogStream::eos;

	// 确保回收掉
	DeleteShop(shopData->shopId);

	auto shop = Shop::Create(shopData->shopId);

	if (!shop)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " create shop(" << shopData->shopId << ") failed!" << LogStream::eos;
		return;
	}

	shop->SetOwner(m_pOwner->GetID());
	shop->SetID(CLRecordClient::Instance()->GenGuid());
	shop->SetDungeonId(dungeonId);

	// 回收所有的商品
	shop->ClearAllItems();

	// 添加到内存
	if (!LoadShop(shop))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " load shop(" << shopData->shopId << ") failed!" << LogStream::eos;
		delete shop;
		return;
	}

	for (auto shopItemId : commonOut)
	{
		shop->AddShopItem(shopItemId);

		auto shopItem = shop->GetShopItem(shopItemId);
		if (shopItem != NULL && shopItem->GetDataEntry() != NULL)
		{
			UInt32 costNum = shopItem->GetDataEntry()->costNum;
			UInt32 discRate = shopItem->GetDiscountRate();
			if (discRate > 0)
			{
				costNum = (UInt32)(((float)discRate / DISCOUNT_RATE_DENOMINATOR) * costNum);
			}
			GetOwner()->SendShopItemUdpLog(shop->GetShopId(), shopItemId, costNum);
		}
	}

	std::vector<ShopItem*> shopItems;
	shop->GetShopItems(shopItems);

	for (auto record : res.records)
	{
		ShopItem* shopItem = NULL;

		for (auto elem : shopItems)
		{
			if (!elem || !elem->GetDataEntry()) continue;

			if (elem->GetDataEntry()->itemId == record.param)
			{
				shopItem = elem;
			}
		}

		if (!shopItem)
		{
			continue;
		}

		shopItem->SetRestNum(record.remainNum);

		DebugLogStream << PLAYERINFO(GetOwner()) << "Shop item(" << shopItem->GetShopItemId() << ")'s rest num=" << shopItem->GetRestNum() << LogStream::eos;
	}

	CLProtocol::SceneSyncMysticalMerchant protocol;
	protocol.id = mysticalMerchantId;
	GetOwner()->SendProtocol(protocol);

	InfoLogStream << PLAYERINFO(GetOwner()) << "Update shop items success, and notify client mystical merchant(" << mysticalMerchantId << ")." << LogStream::eos;
}
