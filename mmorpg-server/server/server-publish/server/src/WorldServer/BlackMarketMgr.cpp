#include "BlackMarketMgr.h"
#include "CLBlackMarketProtocol.h"
#include "WSSelectInitDataCallback.h"
#include "WSItem.h"
#include "CLBlackMarketItemDataEntry.h"
#include "NewAuctionMgr.h"
#include "WSActivityMgr.h"
#include "CLSysNotify.h"
#include "udplog_typedef.h"
#include "MailBox.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "CLErrorCode.h"
#include "WSItemCheck.h"
#include "GameParamMgr.h"
#include "AnnouncementMgr.h"
#include "CLMsgParser.h"
#include "WSServiceSwitchMgr.h"
#include "WorldSysNotify.h"
#include <CLRecordClient.h>
#include <math.h>

//装备回收上限比例
static UInt32 blackBuyProps[ITEM_QUALITY_MAX] = { 0, 0, 0, 0, SVT_BLACKMARKET_PROP_GREEN, SVT_BLACKMARKET_PROP_PINK, 0 };

using namespace ErrorCode;

BlackMarketAuction::BlackMarketAuction()
{
	m_off_sale = 0;
}

BlackMarketAuction::BlackMarketAuction(UInt32 opActId, UInt32 backBuyItemId, UInt8 backBuyType, UInt8 backBuySubType,
	UInt8 backBuyNum, UInt32 fixedPrice, UInt32 recomPrice, UInt32 beginTm, UInt32 endTm)
{
	m_op_activity_id = opActId;
	m_back_buy_item_id = backBuyItemId;
	m_back_buy_type = backBuyType;
	m_back_buy_subtype = backBuySubType;
	m_back_buy_num = backBuyNum;
	m_fixed_price = fixedPrice;
	m_recommend_price = recomPrice;
	m_begin_time = beginTm;
	m_end_time = endTm;
	m_off_sale = 0;
	m_auction_price_lower = 1;
	m_auction_price_upper = m_fixed_price;
}

BlackMarketAuction::~BlackMarketAuction()
{

}

void BlackMarketAuction::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_blackmarket_auction", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void BlackMarketAuction::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_blackmarket_auction", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BlackMarketAuction::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_blackmarket_auction", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BlackMarketAuction::LoadFromDb(CLRecordCallback *pCallback)
{
	if (!pCallback)
	{
		return;
	}
	this->SetDBData(pCallback);
	this->SetID(pCallback->GetKey());
	m_auction_price_lower = 1;
	m_auction_price_upper = m_fixed_price;
	this->ClearDirty();
}

BlackMarketTransaction::BlackMarketTransaction()
{
	m_item = NULL;
}

BlackMarketTransaction::~BlackMarketTransaction()
{
	if (m_item)
	{
		CL_SAFE_DELETE(m_item);
	}
}

void BlackMarketTransaction::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_blackmarket_transaction", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void BlackMarketTransaction::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_blackmarket_transaction", GetID());
	this->GetDBData(cmd, false);
	this->m_item->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BlackMarketTransaction::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_blackmarket_transaction", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BlackMarketTransaction::LoadFromDb(CLRecordCallback *pCallback)
{
	if (!pCallback)
	{
		return;
	}

	this->SetDBData(pCallback);
	this->SetID(pCallback->GetKey());
	this->m_item = new WSItem();
	this->m_item->m_DataId = pCallback->GetData("item_id");
	this->m_item->SetDBData(pCallback);
	this->m_item->SetID(pCallback->GetData("item_src_guid"));
	this->ClearDirty();
	this->m_item->ClearDirty();
}

/**
*@breif 加载黑市商人竞拍项
*/
class SelectBlackMarketAuctuinCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return BlackMarketMgr::Instance()->OnLoadAuctionRet(this);
	}
};

/**
*@breif 加载黑市商人交易订单
*/
class SelectBlackMarketTransactionCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return BlackMarketMgr::Instance()->OnLoadTranscationRet(this);
	}
};

/**
*@brief 加载竞价平均交易价格
*/
class SelectBlackMarketAuctionAverPriceCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return BlackMarketMgr::Instance()->OnLoadAuctionAverPriceRet(this);
	}
};


BlackMarketMgr::BlackMarketMgr()
{
	m_isOpen = false;
	//m_curOpActId = 0;
	m_beginTm = 0;
	m_beginTm = 0;
	//m_mainOpActId = 0;
	m_auction_type = 0;

	m_curActId = 0;
	m_curActName = "";
}

BlackMarketMgr::~BlackMarketMgr()
{
}

void BlackMarketMgr::Init()
{
	this->LoadRecords();
	m_checkAuctionTimer.SetInterval(60 * 1000);
	m_checkTransDeleteTimer.SetInterval(24 * 3600 * 1000);
}

void BlackMarketMgr::InitOpActInfo()
{
	//if (!CheckCanOpenAct())
	//{
	//	return;
	//}
	//const OpActivityRegInfo* pOpActInfo = NULL;
	//WSActivityMgr::Instance()->VisitOpActivity([&pOpActInfo](const OpActivityRegInfo& opAct){
	//	if (opAct.tmpType != OAT_BLACK_MARKET_SHOP)
	//	{
	//		return true;
	//	}
	//	if (opAct.state != AS_IN)
	//	{
	//		return true;
	//	}
	//	pOpActInfo = &opAct;
	//	return false;
	//}
	//);

	//if (pOpActInfo)
	//{
	//	m_isOpen = true;
	//	m_curOpActId = pOpActInfo->dataId;
	//	m_beginTm = pOpActInfo->startTime;
	//	m_endTm = pOpActInfo->endTime;
	//	m_mainOpActId = pOpActInfo->parm;

	//	std::vector<BlackMarketAuction*> auctions;
	//	GetAuctionsByOpActId(m_curOpActId, auctions);
	//	if (auctions.size() == 0)
	//	{
	//		RandomAuctionType();
	//		GenerateBuyLimit();
	//		GenerateAuctions(*pOpActInfo);
	//		InfoLogStream << "open blackmarket by self check when start server m_curOpActId=" << m_curOpActId << ", m_beginTm=" << m_beginTm
	//			<< ", m_endTm=" << m_endTm << ", m_mainOpActId=" << m_mainOpActId << LogStream::eos;
	//	}
	//	else if (auctions[0])
	//	{
	//		m_auction_type = auctions[0]->m_back_buy_type;
	//	}
	//	else{
	//		ErrorLogStream << "init buy type err" << LogStream::eos;
	//	}
	//}
}

void BlackMarketMgr::LoadRecords()
{
	//加载竞拍项
	CLSelectCommand* selectCmd1 = CLRecordClient::Instance()->PrepareSelectCommand("t_blackmarket_auction");
	selectCmd1->PutCond("is_settled", 0);
	CLRecordClient::Instance()->SendCommand(selectCmd1, new SelectBlackMarketAuctuinCallback());

	//加载竞拍交易订单
	CLSelectCommand* selectCmd2 = CLRecordClient::Instance()->PrepareSelectCommand("t_blackmarket_transaction");
	CLRecordClient::Instance()->SendCommand(selectCmd2, new SelectBlackMarketTransactionCallback());

	//加载竞价平均交易价格
	CLSelectCommand* selectCmd3 = CLRecordClient::Instance()->PrepareSelectCommand("t_blackmarket_auction_price");
	CLRecordClient::Instance()->SendCommand(selectCmd3, new SelectBlackMarketAuctionAverPriceCallback());
}

bool BlackMarketMgr::OnLoadAuctionRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		if (GetAuction(id) != NULL) continue;	//重复

		BlackMarketAuction* record = new BlackMarketAuction();
		record->LoadFromDb(pCallback);
		this->AddOneAuction(record);
	}
	return true;
}

bool BlackMarketMgr::OnLoadTranscationRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		if (GetTransaction(id) != NULL) continue;	//重复

		BlackMarketTransaction* record = new BlackMarketTransaction();
		record->LoadFromDb(pCallback);
		this->AddOneTransaction(record);
	}
	return true;
}

bool BlackMarketMgr::OnLoadAuctionAverPriceRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		auto priceItem = new BlackMarketItemAuctionPrice();
		priceItem->guid = pCallback->GetKey();;
		priceItem->item_type_id = pCallback->GetData("item_type_id");
		priceItem->price = pCallback->GetData("price");
		priceItem->auction_guid = pCallback->GetData("auction_guid");
		m_transAverPrices[priceItem->item_type_id] = priceItem;
	}
	return true;
}

bool BlackMarketMgr::CheckCanOpenAct()
{
	//return (UInt32)CURRENT_TIME.Sec() > GameParamMgr::Instance()->GetValue(PARAM_NAME_GAME_FIRST_START) + SERVER_OPEN_TIME;
	return true;
}

void BlackMarketMgr::NotifyRefresh(WSPlayer& player)
{
	CLProtocol::WorldBlackMarketNotifyRefresh protocol;
	player.SendProtocol(protocol);
}

void BlackMarketMgr::RandomAuctionType()
{
	UInt8 types[] = { BMT_FIXED_PRICE, BMT_AUCTION_PRICE };
	m_auction_type = types[Avalon::Random::RandBetween(0, 1)];
}

void BlackMarketMgr::GenerateBuyLimit()
{
	//if (m_mainOpActId != m_curOpActId)
	//{
	//	return;
	//}

	if (m_curActName != ACTIVITY_BLACK_MARKET_1)
	{
		return;
	}

	UInt32 buylimit_prop = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_BUYLIMIT_PROP);
	buylimit_prop = (buylimit_prop == 0) ? BLACKMARKET_BUYLIMIT_RATE : buylimit_prop;

	UInt32 buy_limit = GameParamMgr::Instance()->GetValue(GAME_PARAM_AUCTION_YESTERDAY_COUNTERFEE) * buylimit_prop / 100;
	GameParamMgr::Instance()->SetValue(GAME_PARAM_BLACKMARKET_BUYLIMIT, buy_limit);
	InfoLogStream << "black market buy_limit : " << buy_limit << LogStream::eos;
}

UInt8 BlackMarketMgr::GetAuctionType()
{
	return m_auction_type;
}

UInt32 BlackMarketMgr::CalcAuctionFixedPrice(BlackMarketItemDataEntry* entry)
{
	if (!entry)
	{
		return 0;
	}
	
	UInt32 fixedPrice = 0;
	std::vector<PriceItem>& priceItems = entry->lowerPrices;
	UInt32 timestamp = zjy_auction::AuctionMgr::Instance()->GetItemLatestDealTime(entry->itemId);
	//3天内拍卖行有成交
	if (timestamp > (UInt32)CURRENT_TIME.Sec() - CHECK_TIME_FIXED_PRICE)
	{
		priceItems = entry->upperPrices;
	}
	UInt32 totalWt = 0;
	for (auto item : priceItems)
	{
		totalWt += item.weight;
	}
	UInt32 wt = Avalon::Random::RandBetween(1, totalWt);
	for (auto item : priceItems)
	{
		if (item.weight >= wt)
		{
			fixedPrice = item.price;
			break;
		}
		else
		{
			wt -= item.weight;
		}
	}

	return fixedPrice;
}

void BlackMarketMgr::GenerateAuctions(const OpActivityRegInfo& opActInfo)
{
	//从黑市商人收购物品库随机选择要收购的物品
	std::map<UInt8, std::vector<BlackMarketItemDataEntry*>> dataEntrys;
	UInt8 buyType = GetAuctionType();
	BlackMarketItemDataEntryMgr::Instance()->GetDataEntrysByBuyType(buyType, BMST_NORNAL, dataEntrys);
	std::vector<BlackMarketItemDataEntry*> selectEntrys;
	std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>::iterator iter = dataEntrys.begin();
	for (; iter != dataEntrys.end(); ++iter)
	{
		std::vector<BlackMarketItemDataEntry*>& entrys = iter->second;
		if (entrys.size() > 0)
		{
			UInt32 index = Avalon::Random::RandBetween(0, entrys.size() - 1);
			selectEntrys.push_back(entrys[index]);
		}
	}
	if (selectEntrys.size() == 0)
	{
		ErrorLogStream << "size of selectEntrys is 0" << LogStream::eos;
		return;
	}

	//计算收购物品的固定收购价格
	std::map<UInt32, UInt32> fixedPrices;
	//UInt32 fixedPricesSum = 0;
	UInt32 fixedPricesSums[ITEM_QUALITY_MAX] = {0};
	UInt32 upperLimits[ITEM_QUALITY_MAX] = {0};
	UInt32 backBuyUpLimits[ITEM_QUALITY_MAX] = { 0 };

	for (auto entry : selectEntrys)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->itemId);
		if (!dataEntry)
		{
			ErrorLogStream << "dataEntry is null itemId : " << entry->itemId << LogStream::eos;
			continue;
		}

		UInt8 color = dataEntry->color;
		if (color <= ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
		{
			ErrorLogStream << "color is invalid  itemId : " << entry->itemId << ", color : " << color << LogStream::eos;
			continue;
		}

		UInt32 fixedPrice =  CalcAuctionFixedPrice(entry);
		fixedPrices[entry->itemId] = fixedPrice;
		//fixedPricesSum += fixedPrice;

		fixedPricesSums[color] = fixedPricesSums[color] + fixedPrice;
	}
	/*if (fixedPricesSum == 0)
	{
		ErrorLogStream << "fixedPricesSum is 0" << LogStream::eos;
		return;
	}*/



	//计算每件装备的收购数量上限
	UInt32 upperLimit = GetBackBuyMoneyUpperLimit() / 3;

	for (UInt8 cl = ITEM_QUALITY_WHITE; cl < ITEM_QUALITY_MAX; ++cl)
	{
		if (fixedPricesSums[cl] == 0) continue;
		if (blackBuyProps[cl] == 0) continue;
		UInt32 uplimitProp = SystemValueDataEntryMgr::Instance()->GetSystemValue((SystemValueType)blackBuyProps[cl]);
		upperLimits[cl] = upperLimit * uplimitProp / 100;
		if (upperLimits[cl] > 0)
		{
			backBuyUpLimits[cl] = upperLimits[cl] / fixedPricesSums[cl];
			InfoLogStream << "backBuyUpLimit = " << backBuyUpLimits[cl] << ", color = " << cl << LogStream::eos;
		}
	}

	/*UInt32 backBuyUpLimit = upperLimit / fixedPricesSum;
	InfoLogStream << "upperLimit = " << upperLimit << "backBuyUpLimit = " << backBuyUpLimit << LogStream::eos;*/

	//生成竞拍项
	for (auto entry : selectEntrys)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->itemId);
		if (!dataEntry)
		{
			ErrorLogStream << "dataEntry is null itemId : " << entry->itemId << LogStream::eos;
			continue;
		}
		UInt8 color = dataEntry->color;
		if (backBuyUpLimits[color] == 0)
		{
			InfoLogStream << "backBuyUpLimit = 0, color = " << color << LogStream::eos;
			continue;
		}
		UInt32 averPrice = GetAuctionTransAverPrice(entry->itemId);
		BlackMarketAuction* auction = new BlackMarketAuction(opActInfo.dataId, entry->itemId, buyType, entry->subType,
			backBuyUpLimits[color], fixedPrices[entry->itemId], averPrice, opActInfo.startTime, opActInfo.endTime);
		if (!auction)
		{
			ErrorLogStream << "new BlackMarketAuction fail" << LogStream::eos;
			continue;
		}
		auction->Update();
		this->AddOneAuction(auction);
		InfoLogStream << "create one auction id=" << auction->GetID() << ", itemId=" << entry->itemId << LogStream::eos;
		
		SendBlackMarketAuctionUdpLog(auction->GetID(), auction->m_back_buy_type, auction->m_back_buy_item_id, 
			(auction->m_back_buy_type == BMT_FIXED_PRICE) ? auction->m_fixed_price : 0, auction->m_back_buy_num);
	}

	//固定活动类型额外生成无上限收购类型
	if (buyType == BMT_FIXED_PRICE)
	{
		std::map<UInt8, std::vector<BlackMarketItemDataEntry*>> dataEntrys;
		BlackMarketItemDataEntryMgr::Instance()->GetDataEntrysByBuyType(buyType, BMST_FIXED_UPLIMIT, dataEntrys);
		std::vector<BlackMarketItemDataEntry*> selectEntrys;
		std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>::iterator iter = dataEntrys.begin();
		for (; iter != dataEntrys.end(); ++iter)
		{
			std::vector<BlackMarketItemDataEntry*>& entrys = iter->second;
			selectEntrys.insert(selectEntrys.end(), entrys.begin(), entrys.end());
		}
		//生成竞拍项
		for (auto entry : selectEntrys)
		{
			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->itemId);
			if (!dataEntry)
			{
				ErrorLogStream << "dataEntry is null itemId : " << entry->itemId << LogStream::eos;
				continue;
			}
			UInt32 averPrice = GetAuctionTransAverPrice(entry->itemId);
			UInt32 fixedPrice = CalcAuctionFixedPrice(entry);
			BlackMarketAuction* auction = new BlackMarketAuction(opActInfo.dataId, entry->itemId, buyType, entry->subType,
				entry->fixBuyLimit, fixedPrice, averPrice, opActInfo.startTime, opActInfo.endTime);
			if (!auction)
			{
				ErrorLogStream << "new BlackMarketAuction fail" << LogStream::eos;
				continue;
			}
			auction->Update();
			this->AddOneAuction(auction);
			InfoLogStream << "create one auction id=" << auction->GetID() << ", itemId=" << entry->itemId << LogStream::eos;

			SendBlackMarketAuctionUdpLog(auction->GetID(), auction->m_back_buy_type, auction->m_back_buy_item_id,
				(auction->m_back_buy_type == BMT_FIXED_PRICE) ? auction->m_fixed_price : 0, auction->m_back_buy_num);
		}
	}
}

void BlackMarketMgr::GenerateAuctions(const ActivityInfo& actInfo)
{
	//从黑市商人收购物品库随机选择要收购的物品
	std::map<UInt8, std::vector<BlackMarketItemDataEntry*>> dataEntrys;
	UInt8 buyType = GetAuctionType();
	BlackMarketItemDataEntryMgr::Instance()->GetDataEntrysByBuyType(buyType, BMST_NORNAL, dataEntrys);
	std::vector<BlackMarketItemDataEntry*> selectEntrys;
	std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>::iterator iter = dataEntrys.begin();
	for (; iter != dataEntrys.end(); ++iter)
	{
		std::vector<BlackMarketItemDataEntry*>& entrys = iter->second;
		if (entrys.size() > 0)
		{
			UInt32 index = Avalon::Random::RandBetween(0, entrys.size() - 1);
			selectEntrys.push_back(entrys[index]);
		}
	}
	if (selectEntrys.size() == 0)
	{
		ErrorLogStream << "size of selectEntrys is 0" << LogStream::eos;
		return;
	}

	//计算收购物品的固定收购价格
	std::map<UInt32, UInt32> fixedPrices;
	//UInt32 fixedPricesSum = 0;
	UInt32 fixedPricesSums[ITEM_QUALITY_MAX] = { 0 };
	UInt32 upperLimits[ITEM_QUALITY_MAX] = { 0 };
	UInt32 backBuyUpLimits[ITEM_QUALITY_MAX] = { 0 };

	for (auto entry : selectEntrys)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->itemId);
		if (!dataEntry)
		{
			ErrorLogStream << "dataEntry is null itemId : " << entry->itemId << LogStream::eos;
			continue;
		}

		UInt8 color = dataEntry->color;
		if (color <= ITEM_QUALITY_INVLID || color >= ITEM_QUALITY_MAX)
		{
			ErrorLogStream << "color is invalid  itemId : " << entry->itemId << ", color : " << color << LogStream::eos;
			continue;
		}

		UInt32 fixedPrice = CalcAuctionFixedPrice(entry);
		fixedPrices[entry->itemId] = fixedPrice;
		//fixedPricesSum += fixedPrice;

		fixedPricesSums[color] = fixedPricesSums[color] + fixedPrice;
	}
	/*if (fixedPricesSum == 0)
	{
		ErrorLogStream << "fixedPricesSum is 0" << LogStream::eos;
		return;
	}*/



	//计算每件装备的收购数量上限
	UInt32 upperLimit = GetBackBuyMoneyUpperLimit() / 3;

	for (UInt8 cl = ITEM_QUALITY_WHITE; cl < ITEM_QUALITY_MAX; ++cl)
	{
		if (fixedPricesSums[cl] == 0) continue;
		if (blackBuyProps[cl] == 0) continue;
		UInt32 uplimitProp = SystemValueDataEntryMgr::Instance()->GetSystemValue((SystemValueType)blackBuyProps[cl]);
		upperLimits[cl] = upperLimit * uplimitProp / 100;
		if (upperLimits[cl] > 0)
		{
			backBuyUpLimits[cl] = upperLimits[cl] / fixedPricesSums[cl];
			InfoLogStream << "backBuyUpLimit = " << backBuyUpLimits[cl] << ", color = " << cl << LogStream::eos;
		}
	}

	/*UInt32 backBuyUpLimit = upperLimit / fixedPricesSum;
	InfoLogStream << "upperLimit = " << upperLimit << "backBuyUpLimit = " << backBuyUpLimit << LogStream::eos;*/

	//生成竞拍项
	for (auto entry : selectEntrys)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->itemId);
		if (!dataEntry)
		{
			ErrorLogStream << "dataEntry is null itemId : " << entry->itemId << LogStream::eos;
			continue;
		}
		UInt8 color = dataEntry->color;
		if (backBuyUpLimits[color] == 0)
		{
			InfoLogStream << "backBuyUpLimit = 0, color = " << color << LogStream::eos;
			continue;
		}
		UInt32 averPrice = GetAuctionTransAverPrice(entry->itemId);
		BlackMarketAuction* auction = new BlackMarketAuction(actInfo.id, entry->itemId, buyType, entry->subType,
			backBuyUpLimits[color], fixedPrices[entry->itemId], averPrice, actInfo.startTime, actInfo.dueTime);
		if (!auction)
		{
			ErrorLogStream << "new BlackMarketAuction fail" << LogStream::eos;
			continue;
		}
		auction->Update();
		this->AddOneAuction(auction);
		InfoLogStream << "create one auction id=" << auction->GetID() << ", itemId=" << entry->itemId << LogStream::eos;

		SendBlackMarketAuctionUdpLog(auction->GetID(), auction->m_back_buy_type, auction->m_back_buy_item_id,
			(auction->m_back_buy_type == BMT_FIXED_PRICE) ? auction->m_fixed_price : 0, auction->m_back_buy_num);
	}

	//固定活动类型额外生成无上限收购类型
	if (buyType == BMT_FIXED_PRICE)
	{
		std::map<UInt8, std::vector<BlackMarketItemDataEntry*>> dataEntrys;
		BlackMarketItemDataEntryMgr::Instance()->GetDataEntrysByBuyType(buyType, BMST_FIXED_UPLIMIT, dataEntrys);
		std::vector<BlackMarketItemDataEntry*> selectEntrys;
		std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>::iterator iter = dataEntrys.begin();
		for (; iter != dataEntrys.end(); ++iter)
		{
			std::vector<BlackMarketItemDataEntry*>& entrys = iter->second;
			selectEntrys.insert(selectEntrys.end(), entrys.begin(), entrys.end());
		}
		//生成竞拍项
		for (auto entry : selectEntrys)
		{
			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->itemId);
			if (!dataEntry)
			{
				ErrorLogStream << "dataEntry is null itemId : " << entry->itemId << LogStream::eos;
				continue;
			}
			UInt32 averPrice = GetAuctionTransAverPrice(entry->itemId);
			UInt32 fixedPrice = CalcAuctionFixedPrice(entry);
			BlackMarketAuction* auction = new BlackMarketAuction(actInfo.id, entry->itemId, buyType, entry->subType,
				entry->fixBuyLimit, fixedPrice, averPrice, actInfo.startTime, actInfo.dueTime);
			if (!auction)
			{
				ErrorLogStream << "new BlackMarketAuction fail" << LogStream::eos;
				continue;
			}
			auction->Update();
			this->AddOneAuction(auction);
			InfoLogStream << "create one auction id=" << auction->GetID() << ", itemId=" << entry->itemId << LogStream::eos;

			SendBlackMarketAuctionUdpLog(auction->GetID(), auction->m_back_buy_type, auction->m_back_buy_item_id,
				(auction->m_back_buy_type == BMT_FIXED_PRICE) ? auction->m_fixed_price : 0, auction->m_back_buy_num);
		}
	}
}

BlackMarketAuction* BlackMarketMgr::GetAuction(UInt64 uid)
{
	AuctionMap::iterator it = m_auctions.find(uid);
	if (it == m_auctions.end()) return NULL;

	return it->second;
}

bool BlackMarketMgr::AddOneAuction(BlackMarketAuction* record)
{
	if (!record) return false;
	AuctionMap::iterator it = m_auctions.find(record->GetID());
	if (it != m_auctions.end()) return false;
	m_auctions.insert(std::pair<UInt64, BlackMarketAuction*>(record->GetID(), record));
	return true;
}

void BlackMarketMgr::RemoveAuction(UInt64 uid)
{
	AuctionMap::iterator it = m_auctions.find(uid);
	if (it == m_auctions.end()) return;

	CL_SAFE_DELETE(it->second);
	m_auctions.erase(uid);
}

BlackMarketTransaction* BlackMarketMgr::GetTransaction(UInt64 uid)
{
	TransactionMap::iterator it = m_transactions.find(uid);
	if (it == m_transactions.end()) return NULL;

	return it->second;
}

bool BlackMarketMgr::AddOneTransaction(BlackMarketTransaction* record)
{
	if (!record) return false;
	TransactionMap::iterator it = m_transactions.find(record->GetID());
	if (it != m_transactions.end()) return false;
	m_transactions.insert(std::pair<UInt64, BlackMarketTransaction*>(record->GetID(), record));
	return true;
}

void BlackMarketMgr::RemoveOneTransaction(UInt64 uid, bool del)
{
	TransactionMap::iterator it = m_transactions.find(uid);
	if (it == m_transactions.end()) return;
	
	if (del)
	{
		it->second->Delete();
		InfoLogStream << "delete trans id : " << uid << ", auctioner : " << it->second->m_auctioner_guid << "auction guid : " << it->second->m_auction_guid
			<< ", state : " << it->second->m_state << ", result : " << it->second->m_auction_result << ", money_send : " << it->second->m_money_sended 
			<< ", item_renturned : " << it->second->m_item_returned << LogStream::eos;
	}

	CL_SAFE_DELETE(it->second);
	m_transactions.erase(uid);
}

MoneyType BlackMarketMgr::GetMoneyType()
{
	return MONEY_GOLD;
}

UInt32 BlackMarketMgr::GetBackBuyMoneyUpperLimit()
{
	UInt32 upperLimit = GameParamMgr::Instance()->GetValue(GAME_PARAM_BLACKMARKET_BUYLIMIT);
	UInt32 upperBound = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_BACKBUY_GOLD_UPPER_BOUND);
	UInt32 lowerBound = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_BACKBUY_GOLD_LOWER_BOUND);
	upperBound = (upperBound == 0) ? 30000000 : upperBound;
	lowerBound = (lowerBound == 0) ? 3000000 : lowerBound;

	upperLimit = (upperLimit > upperBound) ? upperBound : upperLimit;
	upperLimit = (upperLimit < lowerBound) ? lowerBound : upperLimit;
	return upperLimit;
}

void BlackMarketMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player)
	{
		return;
	}
	
	CLProtocol::WorldBlackMarketSyncType protocol;
	protocol.type = this->m_auction_type;
	player->SendProtocol(protocol);
}

void BlackMarketMgr::OnOpActOpen(const OpActivityRegInfo& opActInfo)
{
	//if (!CheckCanOpenAct())
	//{
	//	return;
	//}
	//m_isOpen = true;
	//m_curOpActId = opActInfo.dataId;
	//m_beginTm = opActInfo.startTime;
	//m_endTm = opActInfo.endTime;
	//m_mainOpActId = opActInfo.parm;
	//RandomAuctionType();
	//GenerateBuyLimit();
	//GenerateAuctions(opActInfo);
	//InfoLogStream << "open blackmarket by active m_curOpActId=" << m_curOpActId << ", m_beginTm=" << m_beginTm
	//	<< ", m_endTm=" << m_endTm << ", m_mainOpActId=" << m_mainOpActId << LogStream::eos;

	//class Visitor : public WSPlayerVisitor
	//{
	//public:
	//	Visitor(UInt8 auction_type)
	//	{
	//		protocol.type = auction_type;
	//	}
	//	virtual bool operator()(WSPlayer* player)
	//	{
	//		if (player)
	//		{
	//			player->SendProtocol(protocol);
	//		}
	//		return true;
	//	}

	//	CLProtocol::WorldBlackMarketSyncType protocol;
	//};
	//Visitor visitor(GetAuctionType());
	//WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);

	//std::string systemLinkStr;
	//MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_BLACKMARKET, 0);
	//AnnouncementMgr::Instance()->PublicAnnouncement(94, UInt32(CURRENT_TIME.Sec()), systemLinkStr.c_str());
}

void BlackMarketMgr::OnActOpen(const ActivityInfo& actInfo)
{
	if (actInfo.name != ACTIVITY_BLACK_MARKET_1 && actInfo.name != ACTIVITY_BLACK_MARKET_2
		&& actInfo.name != ACTIVITY_BLACK_MARKET_3)
	{
		return;
	}

	if (!CheckCanOpenAct())
	{
		return;
	}
	if (m_isOpen)
	{
		return;
	}
	m_isOpen = true;
	m_curActId = actInfo.id;
	m_curActName = actInfo.name;
	m_beginTm = actInfo.startTime;
	m_endTm = actInfo.dueTime;
	RandomAuctionType();
	GenerateBuyLimit();
	GenerateAuctions(actInfo);
	InfoLogStream << "open blackmarket by active m_curActId=" << m_curActId << ", m_beginTm=" << m_beginTm
		<< ", m_endTm=" << m_endTm << ", m_curActName=" << m_curActName << ", m_auction_type=" << (UInt32)m_auction_type << LogStream::eos;

	class Visitor : public WSPlayerVisitor
	{
	public:
		Visitor(UInt8 auction_type)
		{
			protocol.type = auction_type;
		}
		virtual bool operator()(WSPlayer* player)
		{
			if (player)
			{
				player->SendProtocol(protocol);
			}
			return true;
		}

		CLProtocol::WorldBlackMarketSyncType protocol;
	};
	Visitor visitor(GetAuctionType());
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);

	std::string systemLinkStr;
	MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_BLACKMARKET, 0);
	AnnouncementMgr::Instance()->PublicAnnouncement(0, 94, UInt32(CURRENT_TIME.Sec()), systemLinkStr.c_str());

}

void BlackMarketMgr::OnOpActClose(const OpActivityRegInfo& opActInfo)
{
	////结算竞拍逻辑
	//SettleCurActAuctions();
	//m_isOpen = false;
	//m_curOpActId = 0;
	//m_beginTm = 0;
	//m_endTm = 0;
	//m_auction_type = 0;

	//class Visitor : public WSPlayerVisitor
	//{
	//public:
	//	Visitor(UInt8 auction_type)
	//	{
	//		protocol.type = auction_type;
	//	}
	//	virtual bool operator()(WSPlayer* player)
	//	{
	//		if (player)
	//		{
	//			player->SendProtocol(protocol);
	//		}
	//		return true;
	//	}

	//	CLProtocol::WorldBlackMarketSyncType protocol;
	//};
	//Visitor visitor(BMT_INVALID);
	//WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void BlackMarketMgr::OnActClose(const ActivityInfo& actInfo)
{
	if (actInfo.name != ACTIVITY_BLACK_MARKET_1 && actInfo.name != ACTIVITY_BLACK_MARKET_2
		&& actInfo.name != ACTIVITY_BLACK_MARKET_3)
	{
		return;
	}
	if (!m_isOpen)
	{
		return;
	}
	//结算竞拍逻辑
	SettleCurActAuctions();
	m_isOpen = false;
	m_curActId = 0;
	m_beginTm = 0;
	m_endTm = 0;
	m_auction_type = 0;

	class Visitor : public WSPlayerVisitor
	{
	public:
		Visitor(UInt8 auction_type)
		{
			protocol.type = auction_type;
		}
		virtual bool operator()(WSPlayer* player)
		{
			if (player)
			{
				player->SendProtocol(protocol);
			}
			return true;
		}

		CLProtocol::WorldBlackMarketSyncType protocol;
	};
	Visitor visitor(BMT_INVALID);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void BlackMarketMgr::OnPlayerQueryAuctionList(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldBlackMarketAuctionListRes res;
	if (!IsActOpen())
	{
		player->SendProtocol(res);
		return;
	}
	std::vector<BlackMarketAuction*> auctions;
	TraversalAuction([&auctions, this](BlackMarketAuction& auction){
		if (auction.m_op_activity_id != m_curActId)
		{
			return true;
		}
		if (auction.m_off_sale)
		{
			return true;
		}

		auctions.push_back(&auction);
		return true;
	});

	for (auto auction : auctions)
	{
		res.items.push_back(GetAuctionInfo(*player, *auction));
	}
	player->SendProtocol(res);
}

void BlackMarketMgr::OnPlayerAuctionReq(WSPlayer* player, UInt64 auction_guid, UInt64 item_guid, UInt32 auction_price)
{
	if (!player)
	{
		return;
	}

	InfoLogStream << "player auction req, player id : " << player->GetID() << ", auction_guid : " << auction_guid
		<< ", item_guid : " << item_guid << ", auction_price : " << auction_price << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_BLACK_MARKET))
	{
		player->SendNotify(9997);
		return;
	}

	CLProtocol::WorldBlackMarketAuctionRes res;
	
	UInt32 playerLv = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_PLAYER_LEV);
	playerLv = (playerLv == 0) ? 40 : playerLv;

	if (player->GetLevel() < playerLv)
	{
		res.code = BLACK_MARKET_PLAYE_LV_FAIL;
		player->SendProtocol(res);
		return;
	}

	if (!IsActOpen())
	{
		res.code = BLACK_MARKET_OVERED;
		player->SendProtocol(res);
		NotifyRefresh(*player);
		return;
	}

	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
	if (currTime < m_beginTm || currTime > m_endTm)
	{
		ErrorLogStream << "m_beginTm : " << m_beginTm << ", m_endTm : " << m_endTm << ", player id : " << player->GetID() << LogStream::eos;
		res.code = BLACK_MARKET_TRANS_REQ_FAIL;
		player->SendProtocol(res);
		return;
	}

	BlackMarketAuction* auction = GetAuction(auction_guid);
	if (!auction || auction->m_op_activity_id != m_curActId)
	{
		if (!auction)
		{
			ErrorLogStream << "auction = NULL, auction_guid : " << auction_guid << ", player id : " << player->GetID() << LogStream::eos;
		}
		else
		{
			ErrorLogStream << "auction->m_op_activity_id != m_curActId, auction->m_op_activity_id : " << auction->m_op_activity_id 
				<< ", m_curActId : " << m_curActId << ", player id : " << player->GetID() << LogStream::eos;
		}
		res.code = BLACK_MARKET_TRANS_REQ_FAIL;
		player->SendProtocol(res);
		NotifyRefresh(*player);
		return;
	}

	if (auction->m_back_buy_type == BMT_AUCTION_PRICE)
	{
		if (auction_price < auction->m_auction_price_lower || auction_price > auction->m_auction_price_upper)
		{
			ErrorLogStream << "auction->m_auction_price_lower : " << auction->m_auction_price_lower << ", auction->m_auction_price_upper : " << auction->m_auction_price_upper
				<< ", auction_price : " << auction_price << ", player id : " << player->GetID() << LogStream::eos;
			res.code = BLACK_MARKET_TRANS_REQ_FAIL_PRICE;
			player->SendProtocol(res);
			return;
		}
	}
	else if (auction->m_back_buy_type == BMT_FIXED_PRICE)
	{
		if (auction_price != auction->m_fixed_price)
		{
			ErrorLogStream << "auction_price != auction->m_fixed_price, auction_price : " << auction_price << ", auction->m_fixed_price : "
				<< auction->m_fixed_price << ", player id : " << player->GetID() << LogStream::eos;
			res.code = BLACK_MARKET_TRANS_REQ_FAIL;
			player->SendProtocol(res);
			return;
		}
	}
	else{
		ErrorLogStream << "auction type is err auction id : " << auction->GetID() << " err type : " << auction->m_back_buy_type 
			<< "player id : " << player->GetID() << LogStream::eos;
		res.code = BLACK_MARKET_TRANS_REQ_FAIL;
		player->SendProtocol(res);
		return;
	}

	BlackMarketTransaction* tran = GetPlayerAuctionTransaction(player->GetID(), auction_guid);
	if (tran)
	{
		ErrorLogStream <<"find BlackMarketTransaction , player id : " << player->GetID() << ", auction_guid : " << auction_guid << LogStream::eos;
		res.code = BLACK_MARKET_TRANS_REQ_FAIL;
		player->SendProtocol(res);
		NotifyRefresh(*player);
		return;
	}

	//secne检查道具callback
	UInt64 auctioner_id = player->GetID();
	//UInt32 opact_main_id = m_mainOpActId;
	UInt32 opact_main_id = 0;
	//UInt32 opact_id = m_curOpActId;
	UInt32 opact_id = m_curActId;
	std::string name = player->GetName();
	UInt32 accid = player->GetAccId();
	UInt32 viplv = player->GetVipLvl();
	UInt32 money_type = GetMoneyType();
	//UInt32 opactId = m_curOpActId;
	UInt32 opactId = m_curActId;
	UInt32 playerLev = player->GetLevel();
	UInt8  playerOcc = player->GetOccu();
	BlackMarketAuction auctiontemp = *auction;

	OBFUNCTION sceneCkeckItemCb = [this, auctioner_id, auctiontemp, item_guid, auction_price,
		opact_main_id, opact_id, name, accid, viplv, money_type, opactId, playerLev, playerOcc](UInt32 reqId, UInt32 res, WSItem* item)
	{
		InfoLogStream << "secne check item cb for blackmarket auction req auction guid : " << auctiontemp.GetID() << ", auctioner id : "
			<< auctioner_id << ", item_guid : " << item_guid << ", auction_price : " << auction_price << LogStream::eos;
		
		CLProtocol::WorldBlackMarketAuctionRes protocol;
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctioner_id);
		if (res != ErrorCode::SUCCESS)  //未找到item或item不能申请竞价交易
		{
			if (player)
			{
				ErrorLogStream << "sceneCkeckItemCb  res : " << res << ", player id : " << player->GetID() << LogStream::eos;
				protocol.code = BLACK_MARKET_TRANS_REQ_FAIL;
				player->SendProtocol(protocol);
			}
		}
		else if (res == ErrorCode::SUCCESS && item) //找到item
		{
			BlackMarketTransaction* tran = CreateOnAuctionTranscation(auctioner_id, item_guid, auctiontemp.GetID(), auctiontemp.m_back_buy_type,
				auction_price,opact_main_id, opact_id, name, accid, viplv, money_type, *item);
			if (!tran)
			{
				ErrorLogStream << "create BlackMarketTransaction fail auctioner_id : " << auctioner_id << " item_guid : " << item_guid << LogStream::eos;
				return;
			}
			
			if (player)
			{
				player->SendBlackMarketTransUdpLog(BMOT_REQ, tran->GetID(), auctiontemp.GetID(), 
					auctiontemp.m_back_buy_type, auctiontemp.m_back_buy_item_id, item_guid, auction_price);
			}

			if (!IsActOpen() || m_curActId != opactId)
			{
				if (player)
				{
					player->SendNotify(9995);
				}
				HandleOneTransaction(*tran, false);
				return;
			}
			if (player)
			{
				protocol.code = SUCCESS;
				protocol.item = GetAuctionInfo(*player, auctiontemp);
				player->SendProtocol(protocol);
				player->SendNotify(9995);
			}
		}
		return;
	};

	WSItemCheck::Instance()->RequestOneCheck(CMRT_BLACKMARKET_TRANSACTION_REQ, auctioner_id, item_guid, auction_guid, sceneCkeckItemCb);
}

void BlackMarketMgr::OnPlayerCancelAuctionReq(WSPlayer* player, UInt64 auction_guid)
{
	if (!player)
	{
		return;
	}

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_BLACK_MARKET))
	{
		player->SendNotify(9997);
		return;
	}	

	CLProtocol::WorldBlackMarketAuctionCancelRes res;

	UInt32 playerLv = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_PLAYER_LEV);
	playerLv = (playerLv == 0) ? 40 : playerLv;

	if (player->GetLevel() < playerLv)
	{
		res.code = BLACK_MARKET_PLAYE_LV_FAIL;
		player->SendProtocol(res);
		return;
	}

	if (!IsActOpen())
	{
		res.code = BLACK_MARKET_OVERED;
		player->SendProtocol(res);
		NotifyRefresh(*player);
		return;
	}

	BlackMarketAuction* auction = GetAuction(auction_guid);
	if (!auction || auction->m_op_activity_id != m_curActId)
	{
		res.code = BLACK_MARKET_TRANS_CANCEL_FAIL;
		player->SendProtocol(res);
		NotifyRefresh(*player);
	}

	BlackMarketTransaction* tran = GetPlayerAuctionTransaction(player->GetID(), auction_guid);
	if (!tran)
	{
		res.code = BLACK_MARKET_TRANS_CANCEL_FAIL;
		player->SendProtocol(res);
		NotifyRefresh(*player);
		return;
	}

	CancelOneTransaction(*player, *tran);
	res.code = SUCCESS;
	res.item = GetAuctionInfo(*player, *auction);
	player->SendProtocol(res);
	player->SendNotify(9996);
}

void BlackMarketMgr::SettleCurActAuctions()
{
	InfoLogStream << "settle auctions of act actid="<< m_curActId << ", curtime=" << (UInt32)CURRENT_TIME.Sec() << LogStream::eos;
	std::vector<BlackMarketAuction*> settleAuctions;
	AuctionMap::iterator iter = m_auctions.begin();
	for (; iter != m_auctions.end(); ++iter)
	{
		BlackMarketAuction* auction = iter->second;
		if (!auction 
			|| auction->m_off_sale 
			|| auction->m_is_settled
			|| auction->m_op_activity_id != m_curActId)
		{
			continue;
		}
		
		settleAuctions.push_back(auction);
	}
	
	for (auto auction : settleAuctions)
	{
		SettleOneAuction(*auction);
		RemoveAuction(auction->GetID());
	}
}

void BlackMarketMgr::SettleOneAuction(BlackMarketAuction& auction)
{
	//黑市商人开关关闭，则取消
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_BLACK_MARKET))
	{
		InfoLogStream << "cancel one black market auction by service is close id = " << auction.GetID() << LogStream::eos;
		CancelOneAuction(auction);
	}
	if (auction.m_back_buy_type == BMT_FIXED_PRICE) //固定价格
	{
		SettleOneFixedPriceAuction(auction);
	}
	else if (auction.m_back_buy_type == BMT_AUCTION_PRICE) //竞拍价格
	{
		SettleOneAuctionPriceAuction(auction);
	}
	else{
		ErrorLogStream << "back buy type is err type=" << auction.m_back_buy_type << ", auction id=" << auction.GetID() << LogStream::eos;
		CancelOneAuction(auction);
	}
	auction.m_is_settled = 1;
	auction.Update();
}

void BlackMarketMgr::SettleOneFixedPriceAuction(BlackMarketAuction& auction)
{
	if (auction.m_back_buy_subtype == BMST_NORNAL)
	{
		//计算vip,非vip收购数量
		UInt32 vipRate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_FIXEDPRICE_VIP_RATE);
		UInt32 notVipRate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLACKMARKET_FIXEDPRICE_NOTVIP_RATE);

		vipRate = (vipRate == 0) ? 90 : vipRate;
		notVipRate = (notVipRate == 0) ? 10 : notVipRate;

		UInt32 vipNum = ceil(float(auction.m_back_buy_num * vipRate) / (vipRate + notVipRate));       //vip等级>0的收购数量
		UInt32 notVipNum = auction.m_back_buy_num * notVipRate / (vipRate + notVipRate); //vip等级=0的收购数量

		std::vector<BlackMarketTransaction*> vipTrans;		//vip等级>0的订单
		std::vector<BlackMarketTransaction*> notVipTrans;	//vip等级=0的订单
		GetTransactionByAuctionId(auction.GetID(), vipTrans, notVipTrans);

		std::random_shuffle(vipTrans.begin(), vipTrans.end());
		std::random_shuffle(notVipTrans.begin(), notVipTrans.end());

		std::vector<BlackMarketTransaction*> selectedTrans; //选中收购装备的订单
		std::vector<BlackMarketTransaction*> notSelectTrans; //未选中收购装备的订单
		for (UInt32 i = 0; i < vipTrans.size(); ++i)
		{
			if (i < vipNum)
			{
				selectedTrans.push_back(vipTrans[i]);
			}
			else
			{
				notSelectTrans.push_back(vipTrans[i]);
			}
		}

		for (UInt32 i = 0; i < notVipTrans.size(); ++i)
		{
			if (i < notVipNum)
			{
				selectedTrans.push_back(notVipTrans[i]);
			}
			else
			{
				notSelectTrans.push_back(notVipTrans[i]);
			}
		}

		for (auto tran : selectedTrans)
		{
			HandleOneTransaction(*tran, true);
		}
		for (auto tran : notSelectTrans)
		{
			HandleOneTransaction(*tran, false);
		}
	}
	else if (auction.m_back_buy_subtype == BMST_FIXED_UPLIMIT)
	{
		std::vector<BlackMarketTransaction*> trans; //收购装备的订单
		GetTransactionByAuctionId(auction.GetID(), trans);
		if (trans.size() > auction.m_back_buy_num)
		{
			std::random_shuffle(trans.begin(), trans.end());
		}
		UInt32 num = 0;
		for (auto tran : trans)
		{
			++num;
			if (num <= auction.m_back_buy_num)
			{
				HandleOneTransaction(*tran, true);
			}
			else
			{
				HandleOneTransaction(*tran, false);
			}
		}
	}
	else
	{
	}
}

void BlackMarketMgr::SettleOneAuctionPriceAuction(BlackMarketAuction& auction)
{
	std::vector<BlackMarketTransaction*> trans;
	GetTransactionByAuctionId(auction.GetID(), trans);
	InfoLogStream << "settle one price type auction id = " << auction.GetID() << ", size = " << (UInt32)trans.size() << LogStream::eos;

	if (trans.size() == 0)
	{
		return;
	}
	//申请交易人数大于收购数量,先排序
	if (trans.size() > auction.m_back_buy_num)
	{
		std::sort(trans.begin(), trans.end(), [](BlackMarketTransaction* tran1, BlackMarketTransaction* tran2){
			return tran1->m_auction_price < tran2->m_auction_price;
		});
	}
	UInt32 selectNum = (trans.size() > auction.m_back_buy_num) ? auction.m_back_buy_num : trans.size();
	UInt32 priceSum = 0;
	for (UInt32 i = 0; i < trans.size(); ++i)
	{
		if (!trans[i])
		{
			continue;
		}
		if (i < selectNum)
		{
			HandleOneTransaction(*trans[i], true);
			priceSum += trans[i]->m_auction_price;
		}
		else{
			HandleOneTransaction(*trans[i], false);
		}
	}
	//本期活动平均收购价格
	if (priceSum > 0 && selectNum > 0)
	{
		UInt32 averPrice = priceSum / selectNum;
		UpdateAuctionTransAverPrice(auction.m_back_buy_item_id, averPrice, auction.GetID());
	}

	InfoLogStream << "settle one price type auction id = " << auction.GetID() << ", trans size = " << (UInt32)trans.size() 
		<< ", m_back_buy_num = " << auction.m_back_buy_num << ", selectNum = " << selectNum << ", priceSum = " << priceSum << LogStream::eos;
}

void BlackMarketMgr::CancelOneAuction(BlackMarketAuction& auction)
{
	std::vector<BlackMarketTransaction*> trans;
	GetTransactionByAuctionId(auction.GetID(), trans);
	for (auto tran : trans)
	{
		HandleOneTransaction(*tran, false);
	}
	auction.m_is_settled = 1;
	auction.m_is_sys_cancel = 1;
	auction.Update();
	InfoLogStream << "sys cancel auction guid=" << auction.GetID() << LogStream::eos;
}

void BlackMarketMgr::CancelOneTransaction(WSPlayer& player, BlackMarketTransaction& trans)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string strReason;

	// 发送者
	SysNotifyMgr::Instance()->MakeNotify(9993, buffer, sizeof(buffer));
	strSenderName = buffer;

	SysNotifyMgr::Instance()->MakeNotify(9992, buffer, sizeof(buffer));
	strTitle = buffer;

	SysNotifyMgr::Instance()->MakeNotify(9994, buffer, sizeof(buffer), trans.m_item_name.c_str());
	strContent = buffer;

	std::vector<WSItem*> vecitems;
	vecitems.push_back(trans.m_item);

	std::vector<ItemReward> depositItems;
	strReason = GetReason(SOURCE_TYPE_BLACKMARKET_AUCTION_FAIL, trans.GetID());
	if (MailBox::SendSysMail(trans.m_auctioner_guid, strSenderName, strTitle, strContent, strReason, depositItems, vecitems) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "send mail for return item fail trans guid=" << trans.GetID() << ", auction guid=" << trans.m_auction_guid << ", player id=" << trans.m_auctioner_guid << LogStream::eos;
		trans.m_item_returned = 0;
	}
	else
	{
		trans.m_item_returned = 1;
		trans.m_item = NULL;
	}
	
	InfoLogStream << "cancel auction transaction guid=" << trans.GetID() << ", auctioner id=" << trans.m_auctioner_guid << LogStream::eos;
	
	//SendBlackMarketCancelUdpLog(trans.m_auction_guid, trans.m_auction_type, trans.GetID(), trans.m_auctioner_guid, trans.m_auctioner_accid, trans.m_item_id, trans.m_item_src_guid);
	player.SendBlackMarketTransUdpLog(BMOT_CANCEL, trans.GetID(), trans.m_auction_guid, trans.m_auction_type, trans.m_item_id, trans.m_item_src_guid, trans.m_auction_price);
	RemoveOneTransaction(trans.GetID(), true);
}

void BlackMarketMgr::HandleOneTransaction(BlackMarketTransaction &trans, bool select)
{
	if (trans.m_state)
	{
		return;
	}

	InfoLogStream << "handle one black market auction id = " << trans.GetID() << ", select = " << select << LogStream::eos;
	trans.m_auction_result = select;
	trans.m_state = 1;
	if (select) //选中竞拍,邮件发放金币
	{
		std::vector<ItemReward>	vecitems;
		UInt32 uGetMoneyItemId = GetItemTypeIDByMoneyType(GetMoneyType());
		vecitems.push_back(ItemReward(uGetMoneyItemId, trans.m_auction_price, false, 0));

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		std::string strReason;

		SysNotifyMgr::Instance()->MakeNotify(9988, buffer, sizeof(buffer));
		strSenderName = buffer;
		SysNotifyMgr::Instance()->MakeNotify(9987, buffer, sizeof(buffer));
		strTitle = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9989, buffer, sizeof(buffer), trans.m_item_name.c_str(), trans.m_auction_price);
		strContent = buffer;

		strReason = GetReason(SOURCE_TYPE_BLACKMARKET_AUCTION_SUCC, trans.m_item_id, trans.m_auction_type);
		if (MailBox::SendSysMail(trans.m_auctioner_guid, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "send mail for send gold fail trans guid=" << trans.GetID() << ", auction guid=" << trans.m_auction_guid 
				<< ", player id=" << trans.m_auctioner_guid << ", gold=" << trans.m_auction_price << LogStream::eos;
			trans.m_money_sended = 0;
		}
		else
		{
			trans.m_money_sended = 1;
		}
		//SendBlackMarketRecovedUdpLog(trans.m_auction_guid, trans.m_auction_type, trans.m_item_id, trans.m_auctioner_guid, trans.m_item_src_guid, trans.m_auction_price);
	}
	else //未选中,邮件退还道具
	{
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		std::string strReason;

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(9985, buffer, sizeof(buffer));
		strSenderName = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9984, buffer, sizeof(buffer));
		strTitle = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9986, buffer, sizeof(buffer), trans.m_item_name.c_str());
		strContent = buffer;

		std::vector<WSItem*> vecitems;
		vecitems.push_back(trans.m_item);
		
		std::vector<ItemReward> depositItems;
		strReason = GetReason(SOURCE_TYPE_BLACKMARKET_AUCTION_FAIL, trans.GetID());
		if (MailBox::SendSysMail(trans.m_auctioner_guid, strSenderName, strTitle, strContent, strReason, depositItems, vecitems) != ErrorCode::SUCCESS)
		{
			ErrorLogStream << "send mail for return item fail trans guid=" << trans.GetID() << ", auction guid=" << trans.m_auction_guid << ", player id=" << trans.m_auctioner_guid << LogStream::eos;
			trans.m_item_returned = 0;
		}
		else
		{
			trans.m_item_returned = 1;
			trans.m_item = NULL;
		}
	}
	
	trans.m_delete_time = (UInt32)CURRENT_TIME.Sec() + HANDLED_TRANS_PERSISIT_TIME;
	trans.Update();
}

void BlackMarketMgr::GetTransactionByAuctionId(UInt64 autionId,
		std::vector<BlackMarketTransaction*>& vipTrans, 
		std::vector<BlackMarketTransaction*>& notVipTrans)
{
	TransactionMap::iterator iter = m_transactions.begin();
	for (; iter != m_transactions.end(); ++iter)
	{
		BlackMarketTransaction* tran = iter->second;
		if (!tran || tran->m_auction_guid != autionId)
		{
			continue;
		}
		if (tran->m_auctioner_viplv > 0)
		{
			vipTrans.push_back(tran);
		}
		else
		{
			notVipTrans.push_back(tran);
		}
	}
}

void BlackMarketMgr::GetTransactionByAuctionId(UInt64 autionId, std::vector<BlackMarketTransaction*>& trans)
{
	TransactionMap::iterator iter = m_transactions.begin();
	for (; iter != m_transactions.end(); ++iter)
	{
		BlackMarketTransaction* tran = iter->second;
		if (!tran || tran->m_auction_guid != autionId)
		{
			continue;
		}
		trans.push_back(tran);
	}
}

UInt32 BlackMarketMgr::GetItemTypeIDByMoneyType(MoneyType type)
{
	if (type == MONEY_GOLD)
	{
		return GOLD_ITEM_ID;
	}

	return 0;
}

void BlackMarketMgr::GetAuctionsByOpActId(UInt32 opActId, std::vector<BlackMarketAuction*>& auctions)
{
	AuctionMap::iterator iter = m_auctions.begin();
	for (; iter != m_auctions.end(); ++iter)
	{
		BlackMarketAuction* auction = iter->second;
		if (auction && auction->m_op_activity_id == opActId && auction->m_off_sale == 0 && auction->m_is_settled == 0)
		{
			auctions.push_back(auction);
		}
	}
}

void BlackMarketMgr::GetAuctionsByOpAct(const ActivityInfo* pActInfo, std::vector<BlackMarketAuction*>& auctions)
{
	if (!pActInfo) return;

	UInt32 actId = pActInfo->id;

	AuctionMap::iterator iter = m_auctions.begin();
	for (; iter != m_auctions.end(); ++iter)
	{
		BlackMarketAuction* auction = iter->second;
		if (!auction) continue;
		if (auction->m_op_activity_id != actId) continue;
		if (auction->m_off_sale != 0) continue;
		if (auction->m_is_settled != 0) continue;
		if (auction->m_begin_time != pActInfo->startTime 
			|| auction->m_end_time != pActInfo->dueTime)
		{
			continue;
		}

		auctions.push_back(auction);

		//if (auction && auction->m_op_activity_id == actId && auction->m_off_sale == 0 && auction->m_is_settled == 0)
		//{
		//	auctions.push_back(auction);
		//}
	}
}

void BlackMarketMgr::UpdateAuctionTransAverPrice(UInt32 itemTypeId, UInt32 averPrice, UInt64 auctionGuid)
{
	AuctionTransAverPriceMap::iterator iter = m_transAverPrices.find(itemTypeId);
	if (iter == m_transAverPrices.end() || !iter->second)
	{
		auto priceItem = new BlackMarketItemAuctionPrice();
		priceItem->guid = CLRecordClient::Instance()->GenGuid();
		priceItem->item_type_id = itemTypeId;
		priceItem->price = averPrice;
		priceItem->auction_guid = auctionGuid;

		CLInsertCommand *pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_blackmarket_auction_price", priceItem->guid);
		pCmd->PutData("item_type_id", priceItem->item_type_id);
		pCmd->PutData("price", priceItem->price);
		pCmd->PutData("auction_guid", priceItem->auction_guid);
		CLRecordClient::Instance()->SendCommand(pCmd);

		m_transAverPrices[itemTypeId] = priceItem;
	}
	else{
		auto priceItem = iter->second;
		priceItem->price = averPrice;
		priceItem->auction_guid = auctionGuid;

		CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_blackmarket_auction_price", priceItem->guid, false);
		pCmd->PutData("price", priceItem->price);
		pCmd->PutData("auction_guid", priceItem->auction_guid);
		CLRecordClient::Instance()->SendCommand(pCmd);
	}
}

UInt32 BlackMarketMgr::GetAuctionTransAverPrice(UInt32 itemTypeId)
{
	AuctionTransAverPriceMap::iterator iter = m_transAverPrices.find(itemTypeId);
	if (iter == m_transAverPrices.end() || !iter->second)
	{
		return 0;
	}
	return iter->second->price;
}

void BlackMarketMgr::TraversalAuction(const std::function<bool(BlackMarketAuction&)>& ff)
{
	for (auto& elem : m_auctions)
	{
		if (!elem.second) continue;
		if (!ff(*elem.second))
		{
			break;
		}
	}
}

void BlackMarketMgr::TraversalTransaction(const std::function<bool(BlackMarketTransaction&)>& ff)
{
	for (auto& elem : m_transactions)
	{
		if (!elem.second) continue;
		if (!ff(*elem.second))
		{
			break;
		}
	}
}

BlackMarketAuctionInfo BlackMarketMgr::GetAuctionInfo(WSPlayer& player, const BlackMarketAuction& auction)
{
	BlackMarketAuctionInfo auctionInfo;
	auctionInfo.guid = auction.GetID();
	auctionInfo.back_buy_item_id = auction.m_back_buy_item_id;
	auctionInfo.back_buy_type = auction.m_back_buy_type;
	auctionInfo.begin_time = auction.m_begin_time;
	auctionInfo.end_time = auction.m_end_time;
	auctionInfo.recommend_price = auction.m_recommend_price;
	if (auction.m_back_buy_type == BMT_FIXED_PRICE)
	{
		auctionInfo.price = auction.m_fixed_price;
	}
	else{
		auctionInfo.price_lower_limit = 1;
		auctionInfo.price_upper_limit = auction.m_fixed_price;
		auctionInfo.recommend_price = GetAuctionTransAverPrice(auction.m_back_buy_item_id);
		//auctionInfo.recommend_price = auctionInfo.recommend_price == 0 ? auction.m_fixed_price : auctionInfo.recommend_price;
	}

	//确定竞拍状态
	UInt8 state = BMAIS_INVALID;
	UInt64 auctioner_guid = 0;
	std::string auctioner_name = "";
	UInt32 accid = player.GetAccId();
	UInt32 itemid = auction.m_back_buy_item_id;
	//UInt32 mainOpActId = m_mainOpActId;
	UInt32 mainOpActId = 0;
	//UInt32 opActId = m_curOpActId;
	UInt32 opActId = m_curActId;
	UInt32 auctionPrice = 0;
	UInt64 auctionGuid = auction.GetID();
	TraversalTransaction([accid, itemid, mainOpActId, opActId, 
		&state, &auctioner_guid, &auctioner_name, &auctionPrice, auctionGuid](BlackMarketTransaction& tran){
		if (tran.m_off_sale == 1
			|| tran.m_item_id != itemid
			|| tran.m_opact_main_id != mainOpActId
			|| tran.m_auctioner_accid != accid)
		{
			return true;
		}
		if (tran.m_auction_result) //本期活动已经交易过该类型物品
		{
			state = BMAIS_TRANSED;
			auctioner_guid = tran.m_auctioner_guid;
			auctioner_name = tran.m_auctioner_name;
			return false;
		}
		if (tran.m_opact_id == opActId) //本期活动该阶段已经申请过交易该物品
		{
			state = BMAIS_APPLYED;
			auctioner_guid = tran.m_auctioner_guid;
			auctioner_name = tran.m_auctioner_name;
			auctionPrice = tran.m_auction_price;
			return false;
		}
		return true;
	}
	);

	if (state == BMAIS_INVALID)
	{
		state = BMAIS_CAN_AUCTION;
	}
	auctionInfo.state = state;
	auctionInfo.auctioner_guid = auctioner_guid;
	auctionInfo.auctioner_name = auctioner_name;
	auctionInfo.auction_price = auctionPrice;
	return auctionInfo;
}

BlackMarketTransaction* BlackMarketMgr::GetPlayerAuctionTransaction(UInt64 playerId, UInt64 auctionId)
{
	BlackMarketTransaction* ptran = NULL; 
	TraversalTransaction([playerId, auctionId, &ptran](BlackMarketTransaction& tran){
		if (tran.m_auction_guid == auctionId && tran.m_auctioner_guid == playerId)
		{
			ptran = &tran;
			return false;
		}
		return true;
	}
	);

	return ptran;
}

BlackMarketTransaction* BlackMarketMgr::CreateOnAuctionTranscation(UInt64 playerId, UInt64 itemGuid, UInt64 auctionGuid, UInt8 auctionType, 
	UInt32 auctionPrice, UInt32 opActMainId, UInt32 opActId, std::string playerName, UInt32 accid, UInt32 viplv, UInt32 moneyType, WSItem& item)
{
	BlackMarketTransaction* tran = new BlackMarketTransaction();
	tran->m_create_time = (UInt32)CURRENT_TIME.Sec();
	tran->m_opact_main_id = opActMainId;
	tran->m_opact_id = opActId;
	tran->m_auction_guid = auctionGuid;
	tran->m_item_id = item.GetDataID();
	tran->m_auctioner_guid = playerId;
	tran->m_auctioner_name = playerName;
	tran->m_auctioner_accid = accid;
	tran->m_auctioner_viplv = viplv;
	tran->m_auction_type = auctionType;
	tran->m_auction_money_type = moneyType;
	tran->m_item_src_guid = itemGuid;
	tran->m_auction_price = auctionPrice;
	ItemDataEntry* dataEntry = item.GetDataEntry();
	if (dataEntry)
	{
		tran->m_item_name = dataEntry->name;
	}
	tran->m_item = &item;
	tran->Update();
	AddOneTransaction(tran);
	return tran;
}

BlackMarketTransaction* CreateOnAuctionTranscation(const WSPlayer& player, const BlackMarketAuction& auction)
{
	BlackMarketTransaction* tran = new BlackMarketTransaction();
	return tran;
}

void BlackMarketMgr::OnTick(const Avalon::Time &now)
{
	if (m_checkAuctionTimer.IsTimeout(now))
	{
		//检查已不在活动时间的未处理auction
		std::vector<UInt64> deleteIds;
		for (auto iter : m_auctions)
		{
			BlackMarketAuction* auction = iter.second;
			if (!auction)
			{
				continue;
			}
			//已经结算过
			if (auction->m_is_settled == 1)
			{
				continue;
			}
			//当前活动且未下架
			if (auction->m_op_activity_id == m_curActId && auction->m_off_sale != 1)
			{
				continue;
			}
			
			CancelOneAuction(*auction);
			deleteIds.push_back(iter.first);
			
		}

		for (auto id : deleteIds)
		{
			RemoveAuction(id);
		}
	}

	if (m_checkTransDeleteTimer.IsTimeout(now))
	{
		std::vector<UInt64> deleteIds;
		for (auto iter : m_transactions)
		{
			BlackMarketTransaction* tran = iter.second;
			if (!tran)
			{
				continue;
			}
			if (tran->m_state && (UInt32)now.Sec() > tran->m_delete_time)
			{
				deleteIds.push_back(iter.first);
			}
		}
		for (auto id : deleteIds)
		{
			RemoveOneTransaction(id, true);
		}
	}
}

void BlackMarketMgr::SendBlackMarketAuctionUdpLog(UInt64 auctId, UInt8 auctType, UInt32  itemDataId, UInt32 recovePrice, UInt32 recoveNum)
{
	Sys::SendUdpLog("black_market_auction", LOG_BLACKMARKET_RECOVE_AUCTION_INFO, GAME_VERSION.c_str(), ZONE_ID, UInt32(CURRENT_TIME.Sec()), PLATFORM.c_str(), auctId, auctType, itemDataId, recovePrice, recoveNum);
}


void BlackMarketMgr::SendBlackMarketRecovedUdpLog(UInt64 auctId, UInt8 auctType, UInt32 itemDataId, UInt64 palyerId, UInt64 itemId, UInt32 recovedPrice)
{
	Sys::SendUdpLog("black_market_recoved", LOG_BLACKMARKET_RECOVED_INFO, auctId, auctType, itemDataId, palyerId, itemId, recovedPrice);
}

void  BlackMarketMgr::InitActInfo()
{
	if (!CheckCanOpenAct())
	{
		return;
	}
	const ActivityInfo* pActInfo = NULL;
	WSActivityMgr::Instance()->VisitActivity([&pActInfo](const ActivityInfo& act) {
		if (act.name != ACTIVITY_BLACK_MARKET_1 && act.name != ACTIVITY_BLACK_MARKET_2
			&& act.name != ACTIVITY_BLACK_MARKET_3)
		{
			return true; 
		}
		if (act.state != AS_IN)
		{
			return true;
		}
		pActInfo = &act;
		return false;
	}
	);

	if (pActInfo)
	{
		m_isOpen = true;
		m_curActId = pActInfo->id;
		m_curActName = pActInfo->name;
		m_beginTm = pActInfo->startTime;
		m_endTm = pActInfo->dueTime;

		std::vector<BlackMarketAuction*> auctions;
		GetAuctionsByOpAct(pActInfo, auctions);

		if (auctions.size() == 0)
		{
			RandomAuctionType();
			GenerateBuyLimit();
			GenerateAuctions(*pActInfo);
			InfoLogStream << "open blackmarket by self check when start server m_curActId=" << m_curActId << ", m_beginTm=" << m_beginTm
				<< ", m_endTm=" << m_endTm << ", m_curActName=" << m_curActName << LogStream::eos;
		}
		else if (auctions[0])
		{
			m_auction_type = auctions[0]->m_back_buy_type;
		}
		else {
			ErrorLogStream << "init buy type err" << LogStream::eos;
		}
	}
}
