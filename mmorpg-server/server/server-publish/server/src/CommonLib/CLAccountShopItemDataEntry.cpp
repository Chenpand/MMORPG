/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLAccountShopItemDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLShopItemDataEntry.h"
#include "CLItemDataEntry.h"
// __CUSTOM_HEADER_END__

AccountShopItemDataEntry::AccountShopItemDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	rank = 0;
	buyLimit = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

AccountShopItemDataEntry::~AccountShopItemDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 AccountShopItemDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool AccountShopItemDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	shopItemName = row.ReadString();
	shopId = row.ReadUInt32();
	tabType = row.ReadUInt32();
	jobType = row.ReadUInt32();
	itemDatId = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	costItems = row.ReadString();
	timeCalcType = row.ReadUInt32();
	startTime = row.ReadString();
	endTime = row.ReadString();
	accountRefreshType = row.ReadUInt32();
	accountRefreshTimePoint = row.ReadString();
	accountLimitBuyType = row.ReadUInt32();
	accountLimitBuyNum = row.ReadUInt32();
	roleRefreshType = row.ReadUInt32();
	roleRefreshTimePoint = row.ReadString();
	roleLimitBuyType = row.ReadUInt32();
	roleLimitBuyNum = row.ReadUInt32();
	extensibleCondition = row.ReadUInt32();
	needPreviewFunc = row.ReadUInt32();
	showLevelLimit = row.ReadUInt32();
	rank = row.ReadUInt32();
	buyLimit = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
	auto shopData = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!shopData)
	{
		ErrorLogStream << "Can't find shop(" << shopId << ")!" << LogStream::eos;
		//return false;
	}

	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemDatId);
	if (!itemData)
	{
		ErrorLogStream << "Can't find item(" << itemDatId << ")!" << LogStream::eos;
		//return false;
	}

	if (!TimeUtil::StringConvertToRefreshDate((TimeUtil::RefreshType)accountRefreshType, accountRefreshTimePoint, accountRefreshDate))
	{
		ErrorLogStream << "Account Refresh type(" << accountRefreshType << ") is not match refresh tiempoint(" << accountRefreshTimePoint << ")!" << LogStream::eos;
		return false;
	}

	if (!TimeUtil::StringConvertToRefreshDate((TimeUtil::RefreshType)roleRefreshType, roleRefreshTimePoint, roleRefreshDate))
	{
		ErrorLogStream << "Role Refresh type(" << accountRefreshType << ") is not match refresh tiempoint(" << accountRefreshTimePoint << ")!" << LogStream::eos;
		return false;
	}

	if (accountLimitBuyType > 0 && accountLimitBuyNum == 0)
	{
		ErrorLogStream << "Account limit buy type(" << accountLimitBuyType << ") is not match limit buy num(" << accountLimitBuyNum << ")!" << LogStream::eos;
		return false;
	}

	if (roleLimitBuyType > 0 && roleLimitBuyNum == 0)
	{
		ErrorLogStream << "Role limit buy type(" << accountLimitBuyType << ") is not match limit buy num(" << accountLimitBuyNum << ")!" << LogStream::eos;
		return false;
	}
	
	std::vector<std::string> tmpVec;
	Avalon::StringUtil::Split(costItems, tmpVec, "|");
	if (tmpVec.empty())
	{
		ErrorLogStream << "Cost items are empty!" << LogStream::eos;
		return false;
	}
	for (const auto& elem : tmpVec)
	{
		std::vector<std::string> tmpVec2;
		Avalon::StringUtil::Split(elem, tmpVec2, ":");
		if (tmpVec2.size() != 2)
		{
			ErrorLogStream << "Cost items format is error!" << LogStream::eos;
			return false;
		}
		ItemReward costItemInfo;
		costItemInfo.id = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec2[0]);
		costItemInfo.num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec2[1]);
		costItemVec.push_back(costItemInfo);
	}

	if (!CalculateTime())
	{
		return false;
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
bool AccountShopItemDataEntry::CalculateTime()
{
	switch ((TimeUtil::TimestampCalcType)timeCalcType)
	{
	case TimeUtil::TIMESTAMP_CALC_TYPE_COMMON:
		if (!TimeUtil::TimeStrConvertToTimestamp(onSaleTime, startTime))
		{
			ErrorLogStream << "Account shop item data(" << GetKey() << ")'s start time(" << startTime << ") format is error!" << LogStream::eos;
			return false;
		}
		if (!TimeUtil::TimeStrConvertToTimestamp(offSaleTime, endTime))
		{
			ErrorLogStream << "Account shop item data(" << GetKey() << ")'s end time(" << endTime << ") format is error!" << LogStream::eos;
			return false;
		}
		break;

	default:
		ErrorLogStream << "Account shop item data(" << GetKey() << ")'s time calc type(" << timeCalcType << ") is error!" << LogStream::eos;
		return false;
	}

	if ((onSaleTime == 0 && offSaleTime != 0) || (onSaleTime != 0 && offSaleTime == 0))
	{
		ErrorLogStream << "Account shop item data(" << GetKey() << ")'s on sale time(" << onSaleTime << ") and off sale time are error!" << LogStream::eos;
		return false;
	}

	return true;
}

bool AccountShopItemDataEntry::IsOnSale(UInt16 playerLevel)
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if ((onSaleTime > 0 && onSaleTime > curTime) ||
		(offSaleTime > 0 && offSaleTime < curTime))
	{
		return false;
	}

	if (showLevelLimit > (UInt32)playerLevel)
	{
		return false;
	}

	return true;
}

bool AccountShopItemDataEntry::operator<(const AccountShopItemDataEntry &other) const
{
	return rank < other.rank;
}

// __CUSTOM_ENTRYFUNCTION_END__

bool AccountShopItemDataEntryMgr::AddEntry(AccountShopItemDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AccountShopItemDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	OnAddEntry(entry);
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
bool AccountShopItemDataEntryMgr::GetAccountShopItemDatas(UInt32 shopid, AccountShopItemDataVec& datas)
{
	auto itr = m_ShopItemsMap.find(shopid);
	if (itr == m_ShopItemsMap.end())
	{
		return false;
	}
	
	datas = itr->second;
	return true;
}

void AccountShopItemDataEntryMgr::OnAddEntry(AccountShopItemDataEntry* entry)
{
	m_ShopItemsMap[entry->shopId].push_back(entry);
	std::sort(m_ShopItemsMap[entry->shopId].begin(), m_ShopItemsMap[entry->shopId].end(), [](AccountShopItemDataEntry* a, AccountShopItemDataEntry* b)->bool {return *a < *b; });
}

void AccountShopItemDataEntryMgr::OnReload()
{
	m_ShopItemsMap.clear();

	class AccountShopItemDataEntryVisitor :public Avalon::DataEntryVisitor<AccountShopItemDataEntry>
	{
	public:
		AccountShopItemDataEntryVisitor(){}

		bool operator()(AccountShopItemDataEntry* entry)
		{
			AccountShopItemDataEntryMgr::Instance()->OnAddEntry(entry);
			return true;
		}
	};

	AccountShopItemDataEntryVisitor visitor;
	AccountShopItemDataEntryMgr::Instance()->Visit(visitor);
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

