#include "CLShopItemDataEntry.h"
#include "CLItemDataEntry.h"
#include <AvalonRandom.h>
#include <set>



ShopItemDataEntry::ShopItemDataEntry()
{
	shopItemId = 0;
	itemId = 0;
	shopId = 0;
	costItemId = 0;
	costNum = 0;
	vip = 0;
	vipLimite = 0;
	numLimite = 0;
	grpNum = 0;
	weight = 0;
	limiteOnce = 0;
}

ShopItemDataEntry::~ShopItemDataEntry()
{

}

bool ShopItemDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	shopItemId = row.ReadUInt32();
	shopItemName = row.ReadString();
	shopId = row.ReadUInt32();
	useEqualItem = row.ReadUInt8() != 0;
	itemId = row.ReadUInt32();
	subType = row.ReadUInt32();
	costItemId = row.ReadUInt32();
	costNum = row.ReadUInt32();

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		for (const auto& strElem : strVec)
		{
			std::vector<std::string> strVec2;
			Avalon::StringUtil::Split(strElem, strVec2, "_");
			if (strVec2.size() != 2) return false;

			ItemReward costItem;
			costItem.id = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[0]);
			costItem.num = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
			if (costItem.num == 0) return false;
			otherItemCost.push_back(costItem);
		}
	}

	vip = row.ReadUInt8();
	vipLimite = row.ReadUInt8();
	numLimite = row.ReadInt16();
	limiteOnce = row.ReadUInt8();
	grpNum = row.ReadUInt32();
	weight = row.ReadUInt32();
	exLimite = row.ReadUInt32();
	exLimiteVal = row.ReadUInt32();

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		for (const auto& strElem : strVec)
		{
			std::vector<std::string> strVec2;
			Avalon::StringUtil::Split(strElem, strVec2, "_");
			if (strVec2.size() != 2) return false;

			ItemReward costItem;
			costItem.id = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[0]);
			costItem.num = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
			if (costItem.num == 0) return false;
			extraItemCost.push_back(costItem);
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		if (!strVec.empty())
		{
			if (strVec.size() != 2)
			{
				ErrorLogStream << "Shop item(id=" << shopItemId << ")'s player level format is error!" << LogStream::eos;
				return false;
			}

			for (const auto& strElem : strVec)
			{
				playerLevelLimit.push_back(Avalon::StringUtil::ConvertToValue<UInt16>(strElem));
			}

			if (playerLevelLimit[0] > playerLevelLimit[1])
			{
				ErrorLogStream << "Shop item(id=" << shopItemId << ")'s player level format is error!" << LogStream::eos;
				return false;
			}
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		if (!strVec.empty())
		{
			if (strVec.size() > 2)
			{
				ErrorLogStream << "Shop item(id=" << shopItemId << ")'s vip level format is error!" << LogStream::eos;
				return false;
			}

			for (const auto& strElem : strVec)
			{
				vipLevelLimit.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(strElem));
			}

			if (strVec.size() == 2 && vipLevelLimit[0] > vipLevelLimit[1])
			{
				ErrorLogStream << "Shop item(id=" << shopItemId << ")'s vip level format is error!" << LogStream::eos;
				return false;
			}
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		if (!strVec.empty())
		{
			for (const auto& strElem : strVec)
			{
				dungeonHardLimit.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(strElem));
			}
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		if (!strVec.empty())
		{
			for (const auto& strElem : strVec)
			{
				dungeonSubTypeLimit.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(strElem));
			}
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ",");

		if (!strVec.empty())
		{
			for (const auto& strElem : strVec)
			{
				dungeonIdLimit.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strElem));
			}
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ":");

		if (!strVec.empty())
		{
			for (const auto& strElem : strVec)
			{
				discountRate.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strElem));
			}
		}
	}

	{
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(row.ReadString(), strVec, ":");

		if (!strVec.empty())
		{
			for (const auto& strElem : strVec)
			{
				discountRateWeight.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strElem));
			}
		}
	}

	if (discountRate.size() != discountRateWeight.size())
	{
		ErrorLogStream << "Shop item(id=" << shopItemId << ")'s discount rate format is not meat weight format!" << LogStream::eos;
		return false;
	}
	
	attriFit = row.ReadInt8();

	return true;
}

ShopItemDataEntryMgr::ShopItemDataEntryMgr()
{
	m_commonShopItems.clear();
	m_vipShopItems.clear();
}

ShopItemDataEntryMgr::~ShopItemDataEntryMgr()
{

}


bool ShopItemDataEntryMgr::AddEntry(ShopItemDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<ShopItemDataEntry>::AddEntry(dataEntry)) return false;

	//普通商品
	if (0 == dataEntry->vip)
	{
		std::vector<ShopItemDataEntry*>& dataEntries = m_commonShopItems[dataEntry->shopId];
		dataEntries.push_back(dataEntry);
	}
	else //vip商品
	{
		std::vector<ShopItemDataEntry*>& dataEntries = m_vipShopItems[dataEntry->shopId];
		dataEntries.push_back(dataEntry);
	}
	
	return true;
}

void ShopItemDataEntryMgr::GetShopCommonItems(UInt32 shopID, std::vector<ShopItemDataEntry*>& datas)
{
	ShopItemMap::iterator iter = m_commonShopItems.find(shopID);
	if (iter != m_commonShopItems.end())
	{
		datas = iter->second;
	}
}

void ShopItemDataEntryMgr::GetShopVipItems(UInt32 shopID, std::vector<ShopItemDataEntry*>& datas)
{
	ShopItemMap::iterator iter = m_vipShopItems.find(shopID);
	if (iter != m_vipShopItems.end())
	{
		datas = iter->second;
	}
}

void ShopItemDataEntryMgr::GetShopItemsByOccu(UInt32 shopID, UInt8 oc, std::vector<ShopItemDataEntry*>& datas)
{
	ShopItemMap::iterator iter = m_commonShopItems.find(shopID);
	if (iter != m_commonShopItems.end())
	{
		std::vector<ShopItemDataEntry*>& datas_shop = iter->second;
		for (UInt32 i = 0; i < datas_shop.size(); ++i)
		{
			ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(datas_shop[i]->itemId);
			if (itemData && (itemData->occu.test(oc) || itemData->occu.test(oc - oc % 10)))
			{
				datas.push_back(datas_shop[i]);
			}
		}
	}
}

ShopDataEntry::ShopDataEntry()
{
	shopId = 0;
	refresh = (ShopRefreshType)0;
	refreshCostMax = 0;
	refreshIncRate = 0;
	refreshTimes.clear();
	onSaleNum = 0;
	isLimiteGood1 = 0;
	isLimiteGood2 = 0;
	vipInfos.clear();
}

ShopDataEntry::~ShopDataEntry()
{

}

bool ShopDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Shop data column(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}

	shopId = row.ReadUInt32();
	name = row.ReadString();
	shopType = (ShopType)row.ReadUInt8();
	helpId = row.ReadUInt32();
	isGuildShop = row.ReadUInt32();

	_SplitToVecInt(row, subTypes);
	refresh = (ShopRefreshType)row.ReadUInt8();
	_SplitToVecInt(row, refreshCycle);

	if (subTypes.size() != refreshCycle.size())
	{
		ErrorLogStream << "Sub type size(" << (UInt32)subTypes.size() << ") is not match refresh cycle size(" << (UInt32)refreshCycle.size() << ")!" << LogStream::eos;
		return false;
	}
	else
	{
		_SetRefreshCycleSub();
	}

	_SplitToVecInt(row, refreshCost);
	_SplitToVecInt(row, refreshTimes);

	onSaleNum = row.ReadUInt32();
	isLimiteGood1 = row.ReadUInt8();
	isLimiteGood2 = row.ReadUInt8();
	
	std::vector<ShopVipInfo> tmpVipInfos;
	std::string numstr = row.ReadString();
	std::vector<std::string> numstrs;
	Avalon::StringUtil::Split(numstr, numstrs, "|");
	for (std::vector<std::string>::iterator iter = numstrs.begin();
	iter != numstrs.end(); ++iter)
	{
		ShopVipInfo vipInfo;
		vipInfo.lv = Avalon::StringUtil::ConvertToValue<UInt8>(*iter);
		tmpVipInfos.push_back(vipInfo);
	}

	numstr = row.ReadString();
	numstrs.clear();
	Avalon::StringUtil::Split(numstr, numstrs, "|");
	if (numstrs.size() != tmpVipInfos.size())
	{
		ErrorLogStream << "VipLv size(" << (UInt32)numstrs.size() << ") is not match tmpVipInfos size(" << (UInt32)tmpVipInfos.size() << ")!" << LogStream::eos;
		return false;
	}

	int i = 0;
	for (std::vector<std::string>::iterator iter = numstrs.begin();
	iter != numstrs.end(); ++iter)
	{
		tmpVipInfos[i].discount = Avalon::StringUtil::ConvertToValue<UInt8>(*iter);
		++i;
	}
	
	vipInfos = tmpVipInfos;

	if (onSaleNum < vipInfos.size())
	{
		ErrorLogStream << "On sale num(" << onSaleNum << ") is not match vipInfos size(" << (UInt32)vipInfos.size() << ")!" << LogStream::eos;
		return false;
	}

	version = row.ReadUInt32();
	rareControlType = row.ReadUInt32();
	bindType = row.ReadUInt8();
	isHonorDiscount = row.ReadUInt32();

	return true;
}

void ShopDataEntry::_SplitToVecInt(Avalon::DataRow& row, std::vector<UInt32>& vecInt)
{
	std::string str = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");

	for (UInt32 idx = 0; idx != strs.size(); idx++)
	{
		UInt32 val = Avalon::StringUtil::ConvertToValue<UInt32>(strs[idx]);
		vecInt.push_back(val);
	}
}

void ShopDataEntry::_SetRefreshCycleSub()
{
	auto refreshCycleIter = refreshCycle.begin();
	auto subTypesIter = subTypes.begin();
	for (; refreshCycleIter != refreshCycle.end(); ++refreshCycleIter)
	{
		refreshCycleSub[*subTypesIter] = *refreshCycleIter;
		++subTypesIter;
	}
}

ShopDataEntryMgr::ShopDataEntryMgr()
{
	
}

ShopDataEntryMgr::~ShopDataEntryMgr()
{

}

bool ShopDataEntryMgr::AddEntry(ShopDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<ShopDataEntry>::AddEntry(dataEntry)) return false;

	if (dataEntry->bindType == 1)
	{
		m_AccountShops.push_back(dataEntry);
	}

	return true;
}

void ShopDataEntryMgr::GetAccountShops(std::vector<ShopDataEntry*>& shops)
{
	shops = m_AccountShops;
}

bool MysticalMerchantDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	shopId = row.ReadUInt32();
	weight = row.ReadUInt32();

	return true;
}