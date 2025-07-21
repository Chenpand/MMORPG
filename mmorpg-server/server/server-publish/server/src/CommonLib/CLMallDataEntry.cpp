#include <CLMallDataEntry.h>
#include <CLItemTypeDefine.h>
#include <TimeUtil.h>
#include <MallHelper.h>

MallItemDataEntry::MallItemDataEntry()
{
	multiple = 0;
}

MallItemDataEntry::~MallItemDataEntry()
{
}

bool MallItemDataEntry::Read(Avalon::DataRow &row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	giftPackName = row.ReadString();
	type = row.ReadUInt8();
	subType = row.ReadUInt8();
	jobType = row.ReadUInt8();
	itemId = row.ReadUInt32();
	groupNum = row.ReadUInt32();

	price = row.ReadUInt32();
	disPrice = row.ReadUInt32();
	//if (price < disPrice)
	//{
	//	ErrorLogStream << "In MallItemTable.txt, item(" << id << ")'s price(" << price << ") is lower than disPrice(" << disPrice << ")!" << LogStream::eos;
	//	return false;
	//}

	moneyType = row.ReadUInt8();
	limitType = row.ReadUInt8();
	limitNum = row.ReadUInt16();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");

		for (auto & elem : strs)
		{
			std::vector<std::string> paramStr;
			Avalon::StringUtil::Split(elem, paramStr, ":");

			if (paramStr.size() != 2)
			{
				ErrorLogStream << "Error reward's format(" << str << ")!" << LogStream::eos;
				return false;
			}

			ItemReward reward;
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());

			giftPackItems.push_back(reward);
		}
	}

	giftPackIcon = row.ReadString();
	timeCalcType = row.ReadUInt8();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(limitStartTime, str))
		{
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}
	
	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(limitEndTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	startTimeFromService = row.ReadString();
	endTimeFromService = row.ReadString();
	startTimeFromFirstService = row.ReadString();
	endTimeFromFirstService = row.ReadString();

	if (!startTimeFromService.empty() && !startTimeFromFirstService.empty())
	{
		ErrorLogStream << "Mall item(" << id << ") error format: startTimeFromService and startTimeFromFirstService are all not empty!" << LogStream::eos;
		return false;
	}

	if (!startTimeFromService.empty() && endTimeFromService.empty())
	{
		ErrorLogStream << "Mall item(" << id << ") error format: startTimeFromService is not empty but endTimeFromService is empty!" << LogStream::eos;
		return false;
	}

	if (!startTimeFromFirstService.empty() && endTimeFromFirstService.empty())
	{
		ErrorLogStream << "Mall item(" << id << ") error format: startTimeFromFirstService is not empty but endTimeFromFirstService is empty!" << LogStream::eos;
		return false;
	}

	openInterval = row.ReadString();
	closeInterval = row.ReadString();
	limitTotalNum = row.ReadUInt16();
	vipScore = row.ReadUInt16();
	tagType = row.ReadUInt8();
	sort = row.ReadUInt32();
	hotSort = row.ReadUInt32();
	goodsType = row.ReadUInt8();
	goodsSubType = row.ReadUInt16();
	goodsState = row.ReadUInt8();
	isRecommend = row.ReadUInt8();
	personalTailorActivateCond = row.ReadUInt8();
	discountRate = row.ReadUInt32();
	loginPushId = row.ReadUInt8();
	fashionImagePath = row.ReadString();
	desc = row.ReadString();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		for (const auto& elem : strs)
		{
			std::vector<std::string> paramStr;
			Avalon::StringUtil::Split(elem, paramStr, ":");
			if (paramStr.size() != 2)
			{
				ErrorLogStream << "Buy got info format(" << str << ") is error!" << LogStream::eos;
				return false;
			}

			MallBuyGotInfo buyGotInfo;
			buyGotInfo.buyGotType = Avalon::StringUtil::ConvertToValue<UInt8>(paramStr[0].c_str());
			buyGotInfo.itemDataId = MallHelper::GetMallBuyGotItemDataId((MallBuyGotType)buyGotInfo.buyGotType);
			buyGotInfo.buyGotNum = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());
			buyGotInfos.push_back(buyGotInfo);

			if (buyGotInfo.buyGotType >= MALL_BUY_GOT_TYPE_MAX)
			{
				ErrorLogStream << "Mall buy got type(" << (UInt32)buyGotInfo.buyGotType << ") is error!" << LogStream::eos;
				return false;
			}
		}
	}

	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		for (const auto& elem : strs)
		{
			params.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	playerLevelLimit = row.ReadUInt16();
	mallPushCond = row.ReadUInt8();
	accountRefreshType = (TimeUtil::RefreshType)row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::StringConvertToRefreshDate(accountRefreshType, str, accountRefreshDate))
		{
			ErrorLogStream << "Account Refresh type(" << accountRefreshType << ") is not match refresh tiempoint(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	accountLimitBuyNum = row.ReadUInt32();
	discountCouponId = row.ReadUInt32();
	multiple = row.ReadUInt8();
	deductionCouponId = row.ReadUInt32();
	creditPointDeduction = row.ReadUInt32();
	return true;
}

MallItemDataEntryMgr::MallItemDataEntryMgr()
{
}

MallItemDataEntryMgr::~MallItemDataEntryMgr()
{
}

bool MallItemDataEntryMgr::AddEntry(MallItemDataEntry* entry)
{
	if (!CheckGiftPack(entry))
	{
		ErrorLogStream << "In MallItemTable.txt, goods(" << entry->id << ") is error format!" << LogStream::eos;
		return false;
	}

	if(!Avalon::DataEntryMgrBase<MallItemDataEntry>::AddEntry(entry)) return false;

	m_MallItems.push_back(entry);
	
	return true;
}

UInt32 MallItemDataEntryMgr::GetMallItemPrice(UInt32 uItemId)
{
	std::map<UInt32, UInt32>::iterator	iter = m_mapMallItemPrice.find(uItemId);
	if(iter == m_mapMallItemPrice.end())
		return 0;

	return iter->second;
}

bool MallItemDataEntryMgr::CheckGiftPack(MallItemDataEntry* entry)
{
	if (entry->limitNum != 0 && entry->limitTotalNum != 0)
	{
		ErrorLogStream << "Mall item(" << entry->id << ") info is error, limitNum(" << entry->limitNum 
			<< "), limitTotalNum(" << entry->limitTotalNum << ")." << LogStream::eos;
		return false;
	}

	//switch ((MallGoodsType)entry->goodsType)
	//{
	//case MGT_GIFT_DAILY_REFRESH:
	//	if (entry->limitTotalNum != 0) return false;
	//	break;

	//case MGT_GIFT_ACTIVATE_ONCE:
	//case MGT_GIFT_ACTIVATE_THREE_TIMES:
	////case MGT_GIFT_CHOOSE_ONE:
	//	if (entry->limitNum != 0) return false;
	//	break;

	//default:
	//	return true;
	//}

	return true;
}