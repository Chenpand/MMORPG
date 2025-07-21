#include "Shop.h"
#include "CLItemProtocol.h"
#include "Player.h"

Shop::Shop()
{
	m_refreshNum = 0;
	m_shopItems.clear();
	m_syncDBState = SSDS_NONE;
	m_pDataEntry = NULL;
	m_refreshTime = 0;
	m_weekRefreshTime = 0;
	m_monthRefreshTime = 0;
}

Shop::~Shop()
{
	//ClearItem();
	for (auto item : m_shopItems)
	{
		CL_SAFE_DELETE(item);
	}
	m_shopItems.clear();
}

Shop* Shop::Create(UInt32 shopId)
{
	ShopDataEntry* data = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!data)
		return NULL;

	Shop* shop = new Shop;
	shop->SetDataEntry(data);
	shop->SetVersion(data->version);
	return shop;
}

void Shop::AddShopItem(UInt32 itemId)
{
	if (m_shopItems.size() >= m_pDataEntry->onSaleNum)
		return;
	if (m_pDataEntry->vipInfos.size() > m_pDataEntry->onSaleNum)
		return;

	if (m_pDataEntry && m_pDataEntry->shopType == ST_LEASE)
	{
		for (UInt32 i = 0; i < m_shopItems.size(); ++i)
		{
			if (m_shopItems[i] && m_shopItems[i]->GetShopItemId() == itemId)
			{
				return;
			}
		}
	}

	ShopItem* item = ShopItem::Create(itemId);
	item->SetID(CLRecordClient::Instance()->GenGuid());
	item->SetShopId(GetID());
	item->SetOwner(m_owner);
	item->CalculateDiscountRate();

	if (item->GetDataEntry()->vip)
	{
		int idx = (int)(m_pDataEntry->vipInfos.size() - (m_pDataEntry->onSaleNum - m_shopItems.size()));
		if (idx >= (int)m_pDataEntry->vipInfos.size() || idx < 0)
		{
			CL_SAFE_DELETE(item);
			return;
		}

		item->SetVipLv(m_pDataEntry->vipInfos[idx].lv);
		item->SetVipDisCount(m_pDataEntry->vipInfos[idx].discount);
	}
	
	m_shopItems.push_back(item);
	item->SetGrid(m_shopItems.size() - 1);
}

void Shop::AddShopItem(ShopItem* item)
{
	m_shopItems.push_back(item);
}

void Shop::ClearItem()
{
	UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
	std::vector<ShopItem*>::iterator itr = m_shopItems.begin();
	for (; itr != m_shopItems.end();)
	{
		ShopItem* item = *itr;
		if (item->GetDataEntry()->limiteOnce == 0 &&
			item->GetRestNum() == 0)
		{
			itr++;
			continue;
		}

		ShopDataEntry* shopData = this->GetDataEntry();
		if (shopData && ShopType::ST_LEASE == shopData->shopType
			&& item->GetLeaseEndTimeStamp() > curr_time)
		{
			itr++;
			continue;
		}

		CL_SAFE_DELETE(item);
		itr = m_shopItems.erase(itr);
	}
}

void Shop::ClearAllItems()
{
	if (m_shopItems.empty()) return;

	for (auto shopItem : m_shopItems)
	{
		if (shopItem)
			CL_SAFE_DELETE(shopItem);
	}

	m_shopItems.clear();
}

ShopItem* Shop::GetShopItem(UInt32 shopItemId)
{
	for (int i = 0; i < (int)m_shopItems.size(); ++i)
	{
		if (m_shopItems[i]->GetDataEntry()->shopItemId == shopItemId)
		{
			return m_shopItems[i];
		}
	}
	
	return NULL;
}

void Shop::ClearUnRefreshShopItem()
{
	m_UnRefreshShopItemRestNum.clear();
}

void Shop::KeepUnRefreshShopItemRestNum(UInt32 shopItemId, Int16 shopItemRestNum)
{
	m_UnRefreshShopItemRestNum[shopItemId] = shopItemRestNum;
}

bool Shop::HasUnRefreshShopItem()
{
	return m_UnRefreshShopItemRestNum.size();
}

void Shop::SetUnRefreshShopItemRestNum()
{
	for (auto shopItem : m_shopItems)
	{
		if (!shopItem)
		{
			ErrorLogStream << "player(" << m_owner << ") shopItem ptr is null!" << LogStream::eos;
			continue;
		}

		auto iter = m_UnRefreshShopItemRestNum.find(shopItem->GetShopItemId());
		if (iter != m_UnRefreshShopItemRestNum.end())
		{
			shopItem->SetRestNum(iter->second);
		}
	}
}

void Shop::SetRefreshCycleTypeState(UInt32 refreshCycleType, UInt32 refreshCycleState)
{
	auto iter = m_RefreshCycleTypeWithState.find(refreshCycleType);
	if (iter != m_RefreshCycleTypeWithState.end())
	{
		if (iter->second == refreshCycleState)
		{
			return;
		}
	}

	m_RefreshCycleTypeWithState[refreshCycleType] = refreshCycleState;
}

UInt32 Shop::GetRefreshCycleState(UInt32 refreshCycleType)
{
	auto iter = m_RefreshCycleTypeWithState.find(refreshCycleType);
	if (iter != m_RefreshCycleTypeWithState.end())
	{
		return iter->second;
	}
	else
	{
		ErrorLogStream << "Can not find refreshCycle(" << refreshCycleType << ")!" << LogStream::eos;
		return (UInt32)REFRESH_CYCLE_INVAILD;
	}
}

bool Shop::_CheckRefreshCycleStateActivated(RefreshCycleType refreshCycleType)
{
	auto iter = m_RefreshCycleTypeWithState.find((UInt32)refreshCycleType);
	if (iter != m_RefreshCycleTypeWithState.end())
	{
		if (iter->second == (UInt32)REFRESH_CYCLE_ACTIVATION)
			return true;
	}
	return false;
}

void Shop::SetRefreshTime()
{
	if (m_pDataEntry->refreshTimes.empty())
	{
		ErrorLogStream << "In shop.txt, refreshTime is empty!" << LogStream::eos;
		return;
	}
	UInt32 refreshHour = m_pDataEntry->refreshTimes[0];

	if (_CheckRefreshCycleStateActivated(REFRESH_CYCLE_DAILY))
	{
		m_refreshTime = CalculateNextRefreshTimestamp(REFRESH_CYCLE_DAILY, refreshHour);
		InfoLogStream << "player(" << m_owner << ") has refreshed shop(" << m_shopid << "), refreshCycleType(" << (UInt32)REFRESH_CYCLE_DAILY 
			<< "), dayRefreshTime(" << m_refreshTime << ")." << LogStream::eos;
	}
	
	if (_CheckRefreshCycleStateActivated(REFRESH_CYCLE_WEEK))
	{
		m_weekRefreshTime = CalculateNextRefreshTimestamp(REFRESH_CYCLE_WEEK, refreshHour);
		InfoLogStream << "player(" << m_owner << ") has refreshed shop(" << m_shopid << "), refreshCycleType(" << (UInt32)REFRESH_CYCLE_WEEK
			<< "), weekRefreshTime(" << m_weekRefreshTime << ")." << LogStream::eos;
	}

	if (_CheckRefreshCycleStateActivated(REFRESH_CYCLE_MONTH))
	{
		m_monthRefreshTime = CalculateNextRefreshTimestamp(REFRESH_CYCLE_MONTH, refreshHour);
		InfoLogStream << "player(" << m_owner << ") has refreshed shop(" << m_shopid << "), refreshCycleType(" << (UInt32)REFRESH_CYCLE_MONTH
			<< "), monthRefreshTime(" << m_monthRefreshTime << ")." << LogStream::eos;
	}
}

UInt64 Shop::CalculateNextRefreshTimestamp(RefreshCycleType refreshCycleType, UInt32 refreshHour)
{
	TimeUtil::RefreshType refreshType;
	TimeUtil::RefreshDate refreshDate;

	switch (refreshCycleType)
	{
	case REFRESH_CYCLE_DAILY:
		refreshType = TimeUtil::REFRESH_TYPE_PER_DAY;
		refreshDate.hour = refreshHour;
		break;

	case REFRESH_CYCLE_WEEK:
		refreshType = TimeUtil::REFRESH_TYPE_PER_WEEK;
		refreshDate.weekDay = 1;
		refreshDate.hour = refreshHour;
		break;

	case REFRESH_CYCLE_MONTH:
		refreshType = TimeUtil::REFRESH_TYPE_PER_MONTH;
		refreshDate.day = 1;
		refreshDate.hour = refreshHour;
		break;

	default:
		ErrorLogStream << "player(" << m_owner << ") shop(" << m_shopid << "), refreshCycleType(" << refreshCycleType << ") is error." << LogStream::eos;
		return 0;
	}

	UInt64 nextRefreshTimestamp = 0;
	if (!TimeUtil::CalculateNextRefreshTimestamp(refreshType, refreshDate, nextRefreshTimestamp))
	{
		ErrorLogStream << "player(" << m_owner << ") shop(" << GetID() << "," << m_shopid << ") calculate next refresh timestamp failed by ("
			<< (UInt32)refreshType << ") and (" << refreshDate.LogStr() << ")!" << LogStream::eos;
	}

	return nextRefreshTimestamp;
}