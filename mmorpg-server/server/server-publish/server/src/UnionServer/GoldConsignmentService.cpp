#include "GoldConsignmentService.h"

#include <math.h>
#include <CLRecordClient.h>
#include <CLGoldConsignmentProtocol.h>
#include <CLItemProtocol.h>
#include <CLErrorCode.h>
#include <CLGoldConsignmentValueDataEntry.h>

#include "USApplication.h"
#include "USPlayer.h"
#include "USItemTransaction.h"
#include "SysUtility.h"
#include "GameZoneMgr.h"
#include "USItemThing.h"

class SelectGoldConsignmentOrderCallback : public CLRecordCallback
{
public:
	SelectGoldConsignmentOrderCallback(GoldConsignmentService* service) : m_Service(service) {}
	virtual void OnFinished()
	{
		m_Service->LoadOrderData(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load t_gold_consignment_order Failed, ErrorCode:" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}

private:
	GoldConsignmentService* m_Service;
};

class SelectGoldConsignmentRecordCallback : public CLRecordCallback
{
public:
	SelectGoldConsignmentRecordCallback(GoldConsignmentService* service) : m_Service(service) {}

	virtual void OnFinished()
	{
		m_Service->LoadTradeRecord(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load t_gold_consignment_record Failed, ErrorCode:" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}
private:
	GoldConsignmentService* m_Service;
};

class SelectGoldConsignmentPriceCallback : public CLRecordCallback
{
public:
	SelectGoldConsignmentPriceCallback(GoldConsignmentService* service) : m_Service(service) {}

	virtual void OnFinished()
	{
		m_Service->LoadTradePrice(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load t_gold_consignment_price Failed, ErrorCode:" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}
private:
	GoldConsignmentService* m_Service;
};

class SelectGoldConsignmentProfitCallback : public CLRecordCallback
{
public:
	SelectGoldConsignmentProfitCallback(GoldConsignmentService* service) : m_Service(service) {}

	virtual void OnFinished()
	{
		m_Service->LoadUntreatProfit(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load t_gold_consignment_profit Failed, ErrorCode:" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}
private:
	GoldConsignmentService* m_Service;
};

class SelectGoldConsignmentCreditPointCallback : public CLRecordCallback
{
public:
	SelectGoldConsignmentCreditPointCallback(GoldConsignmentService* service) : m_Service(service) {}

	virtual void OnFinished()
	{
		m_Service->LoadUntreatCreditPoint(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load t_gold_consignment_credit_point Failed, ErrorCode:" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}
private:
	GoldConsignmentService* m_Service;
};


/**
 *@brief 卖金扣款
 */
class GoldTradeThing : public USItemThing
{
public:
	GoldTradeThing(USPlayer* player, GoldConsignmentService* service, UInt32 ot, UInt32 tt, UInt32 up, UInt32 num, UInt32 si)
		: USItemThing(player), m_Service(service), m_OrderType(ot), m_TradeType(tt), m_UnitPrice(up), m_TradeNum(num), m_SubmitId(si) {}
	~GoldTradeThing() {}

	virtual void OnCheckSucced(USPlayer* player, UInt64 thingId)
	{
		if (NULL == m_Service)
		{
			ErrorLogStream << PLAYERINFO(player) << " sell gold failed! gold:" << m_TradeNum << LogStream::eos;
			return;
		}

		InfoLogStream << PLAYERINFO(player) << " result item thing id:" << thingId << " itemCost:" << GetItemCost().LogStr() << LogStream::eos;

		if (GetOrderType() == GCOT_SELL)
		{
			CommitThing(thingId, m_Service->SellGoldTransResult(player, this));
		}
		else if (GetOrderType() == GCOT_BUY)
		{
			CommitThing(thingId, m_Service->BuyGoldTransResult(player, this));
		}
	}

	virtual void OnCheckFailed(USPlayer* player, UInt32 result)
	{
		if (NULL == player) return;

		ErrorLogStream << PLAYERINFO(player) << "GoldTradeThing Failed! unitPrice:" <<
			m_UnitPrice << " tradeNum:" << m_TradeNum << " tradeType:" << m_TradeType << " orderType:" 
			<< m_OrderType << " result:" << result << LogStream::eos;

		CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
		res.submitId = GetSubmitId();
		res.retCode = result;
		player->SendProtocol(res);
	}

	virtual void OnTimeOut(USPlayer* player)
	{
		if (NULL == player) return;

		USItemThing::OnTimeOut(player);

		ErrorLogStream << PLAYERINFO(player) << "GoldTradeThing Time Out! unitPrice:" << m_UnitPrice << 
			" tradeNum:" << m_TradeNum << " tradeType:" << m_TradeType << " orderType:" << m_OrderType << LogStream::eos;

		CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
		res.submitId = GetSubmitId();
		res.retCode = ErrorCode::GOLD_CONSIGNMENT_ORDER_TIME_OUT;
		player->SendProtocol(res);
	}

	UInt32 GetOrderType() { return m_OrderType; }
	UInt32 GetTradeType() { return m_TradeType; }
	UInt32 GetUnitPrice() { return m_UnitPrice; }
	UInt32 GetTradeNum() { return m_TradeNum; }
	UInt32 GetSubmitId() { return m_SubmitId; }

private:

	GoldConsignmentService* m_Service;
	UInt32 m_OrderType;
	UInt32 m_TradeType;
	UInt32 m_UnitPrice;
	UInt32 m_TradeNum;
	UInt32 m_SubmitId;
};

UInt32 GetGoldBase()
{
	return GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_UNIT_PRICE_BASE);
}

UInt32 GetTradeBase()
{
	UInt32 num = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_GOLD_NUM);
	if (num == 0)
	{
		num = 100000;
	}

	return num;
}

float GetMinPrice(UInt32 unitPrice)
{
	if (unitPrice == 0) return 0;
	
	UInt32 unitPriceBase = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_TRADE_UNIT_BASE);
	UInt32 minTradeNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_GOLD_NUM);

	if (unitPriceBase == 0 || minTradeNum == 0 || minTradeNum > unitPriceBase)
		return 0;

	return float(unitPrice) / (unitPriceBase / minTradeNum);
}

UInt64 GetHangListTime()
{
	UInt32 timeOut = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_HANG_UP_TIME);
	return timeOut * 3600 * 1000;
}

UInt64 GenProfitGuid()
{
	return CLRecordClient::Instance()->GenGuid();
}

GoldConsignmentService::GoldConsignmentService()
{
	m_StartTradeStamp = 0;
	m_LastAveragePrice = 0;
	m_CurAveragePrice = 0;
	m_RecentAveragePrice = 0;
	m_TodayCostPoint = 0;
	m_TodayDealNum = 0;
	UInt32 recentTime = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_RECENT_AVERAGE_PRICE_TIME);
	m_RecentAverageTimer.SetInterval(recentTime * 1000);
	m_Timer3s.SetInterval(3 * 1000);
}

GoldConsignmentService::~GoldConsignmentService()
{
	m_SellOrderMap.clear();
	m_BuyOrderMap.clear();
}

bool GoldConsignmentService::Init()
{
	UnionService::Init();

	// 加载数据
	CLSelectCommand* cmd_1 = CLRecordClient::Instance()->PrepareSelectCommand("t_gold_consignment_price");
	CLRecordClient::Instance()->SendCommand(cmd_1, new SelectGoldConsignmentPriceCallback(this));

	CLSelectCommand* cmd_3 = CLRecordClient::Instance()->PrepareSelectCommand("t_gold_consignment_record");
	CLRecordClient::Instance()->SendCommand(cmd_3, new SelectGoldConsignmentRecordCallback(this));

	CLSelectCommand* cmd_4 = CLRecordClient::Instance()->PrepareSelectCommand("t_gold_consignment_profit");
	cmd_4->PutCond("is_handle", 0);
	CLRecordClient::Instance()->SendCommand(cmd_4, new SelectGoldConsignmentProfitCallback(this));

	CLSelectCommand* cmd_5 = CLRecordClient::Instance()->PrepareSelectCommand("t_gold_consignment_credit_point");
	cmd_5->PutCond("is_handle", 0);
	CLRecordClient::Instance()->SendCommand(cmd_5, new SelectGoldConsignmentCreditPointCallback(this));

	return true;
}

void GoldConsignmentService::OnTick(const Avalon::Time& now)
{
	if (m_LastAveragePrice == 0) return;

	UnionService::OnTick(now);

	// N分钟保存一次最近均价
	if (m_RecentAverageTimer.IsTimeout(now) && m_TodayCostPoint != 0 && m_TodayDealNum != 0)
	{
		_SetRecentAveragePrice(m_CurAveragePrice);
		_SetCurAveragePrice(UInt32(m_TodayCostPoint / m_TodayDealNum));

		_DBUpdatePrice(DB_CUR_PRICE | DB_RECENT_PRICE);

		InfoLogStream << "refresh recentPrice:" << m_RecentAveragePrice << " curPrice:" << m_CurAveragePrice 
			<< " todayCostPoint:" << m_TodayCostPoint << " dealNum:" << m_TodayDealNum << LogStream::eos;
	}

	if (m_Timer3s.IsTimeout(now))
	{
		_CheckDealData();

		// 检查是否休市
		_CheckClose(now);

		// 超时订单处理
		_CheckTimeOutOrder(now);

		// 检查需要下架的服务器订单
		_CheckOrderCloseZone();

		// 检查需要下架的玩家订单
		_CheckOrderCloseRole();

		// 检查需要下架的订单
		_CheckOrderClose();

		// 检查未处理的收益
		_SendUntreatProfit();

		// 检查未发放的信用点券
		_SendUntreatCreditPoint();
	}
}

void GoldConsignmentService::OnDayChanged()
{
	// 6点刷新上一个交易日均价
	InfoLogStream << "day changed old last:" << m_LastAveragePrice << " new last:" << m_CurAveragePrice 
		<< " todayCostPoint:" << m_TodayCostPoint << " todayDealNum:" << m_TodayDealNum << LogStream::eos;
	_SetLastAveragePrice(m_CurAveragePrice);

	UInt32 flag = DB_LAST_PRICE;
	UInt32 minProtectPrice = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_PROTECT_PRICE);
	if (minProtectPrice != 0)
	{
		UInt32 waveRate = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_PRICE_WAVE_RATE);
		UInt32 tempVal = minProtectPrice + minProtectPrice * waveRate / GOLD_CONSIGNMENT_RATE;
		if (m_CurAveragePrice < tempVal)
		{
			_SetLastAveragePrice(tempVal);
			_SetCurAveragePrice(tempVal);
			_SetRecentAveragePrice(tempVal);
			flag |= DB_CUR_PRICE | DB_RECENT_PRICE;
		}
	}

	m_TodayCostPoint = 0;
	m_TodayDealNum = 0;
	flag |= DB_TODAY_COST_POINT | DB_TODAY_DEAL_NUM;

	_DBUpdatePrice(flag);

	// 删除不符合今日单价范围的挡位
	_RemoveAbnormalUnitPrice();
}

USPlayer* GoldConsignmentService::CreatePlayer()
{
	return new USPlayer;
}

UnionServerFunctionType GoldConsignmentService::GetFunctionType()
{
	return USFT_GOLD_CONSIGNMENT;
}

void GoldConsignmentService::HandlerOrderPageReq(RoleIDConnID& info)
{
	USPlayer* player = m_PlayerMgr.AddUSPlayer(info.roleID);
	if (NULL == player)
	{
		ErrorLogStream << "create player failed! id:" << info.roleID << " connId:" << info.connId << LogStream::eos;
		return;
	}

	player->SetAccID(info.accID);
	player->SetConnId(info.connId);
	player->SetZoneID(CL_GET_ZONEID_FROM_NODEID(info.connId));
	player->SetSceneNodeId(info.sourceNode[ST_SCENE]);
	player->SetWorldNodeId(info.sourceNode[ST_WORLD]);

	CLProtocol::UnionGoldConsignmentOrderPageRes res;
	res.startCloseTime = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_CLOSE_START_TIME);
	res.endCloseTime = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_CLOSE_END_TIME);
	res.startCloseStamp = m_StartTradeStamp;
	res.lastAveragePrice = m_LastAveragePrice;
	res.curAveragePrice = m_CurAveragePrice;
	res.recentAveragePrice = m_RecentAveragePrice;
	res.sellAccNum = _FindAccOrderNum(player->GetAccID(), player->GetZoneID(), GCOT_SELL);
	res.buyAccNum = _FindAccOrderNum(player->GetAccID(), player->GetZoneID(), GCOT_BUY);
	res.estimateIncome = _EstimateAccCreditPointProfit(player->GetZoneID(), player->GetAccID());
	res.isHasUnitAbnormalOrder = _IsHasAbnormalOrder(player->GetZoneID(), player->GetAccID(), player->GetID()) ? 1:0;

	UInt32 pageNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_GRADE_PAGE_NUM);
	SellOrderGradeMap::iterator sellIter = m_SellGradeMap.begin();
	for (; sellIter != m_SellGradeMap.end(); ++sellIter)
	{
		OrderGradeInfo info(sellIter->first, sellIter->second.tradeTotalNum);
		res.sellOrderList.push_back(info);
		if (res.sellOrderList.size() >= pageNum)
		{
			break;
		}
	}

	BuyOrderGradeMap::iterator buyIter = m_BuyGradeMap.begin();
	for (; buyIter != m_BuyGradeMap.end(); ++buyIter)
	{
		OrderGradeInfo info(buyIter->first, buyIter->second.tradeTotalNum);
		res.buyOrderList.push_back(info);
		if (res.buyOrderList.size() >= pageNum)
		{
			break;
		}
	}

	player->SendProtocol(res);
}

void GoldConsignmentService::HandlerSumbitOrderReq(USPlayer* player, UInt32 tradeType, UInt32 orderType, 
	UInt32 unitPrice, UInt32 tradeNum, UInt32 param, UInt32 submitId)
{
	if (NULL == player || tradeType == 0 || orderType == 0 || unitPrice == 0 || tradeNum == 0) return;

	CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
	res.submitId = submitId;
	res.retCode = ErrorCode::SUCCESS;

	do 
	{
		if (_IsClose())
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_CLOSE_TRADE;
			break;
		}

		if (_ZoneIsCloseOrder(player->GetZoneID()))
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_SERVER_BUSY;
			break;
		}

		// 检查单价的涨幅
		if ( _CheckWavePrice(unitPrice))
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_OVER_WAVE_PRICE;
			break;
		}

		// 检查保护价
		UInt32 minProtectPrice = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_PROTECT_PRICE);
		UInt32 maxProtectPrice = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MAX_PROTECT_PRICE);
		if (unitPrice < minProtectPrice || unitPrice> maxProtectPrice)
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_OVER_PROTECT_PRICE;
			break;
		}

		// 检查交易数量
		UInt32 minNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_GOLD_NUM);
		UInt32 maxNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MAX_GOLD_NUM);
		if (tradeNum * GetTradeBase() < minNum || tradeNum * GetTradeBase() > maxNum)
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_TRADE_NUM_ERROR;
			break;
		}

		// 按账号，检查上架订单数
		UInt32 accOrderNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_ACC_ORDER_NUM);
		if (_FindAccOrderNum(player->GetAccID(), player->GetZoneID(), orderType) > accOrderNum)
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_ORDER_NUM_FULL;
			break;
		}

		// 检查玩家上架条件
		switch (orderType)
		{
		case GCOT_SELL:
		{
			res.retCode = _CheckSellOrder(player, tradeType, unitPrice, tradeNum, param, submitId);
			break;
		}
		case GCOT_BUY:
		{
			res.retCode = _CheckBuyOrder(player, tradeType, unitPrice, tradeNum, submitId);
			break;
		}
		default: return;
		}
	} while (0);

	// 若检查未出错，后续流程在事务返回后处理
	if (res.retCode != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
		return;
	}
}

void GoldConsignmentService::HandlerDealRecordReq(USPlayer* player)
{
	if (NULL == player) return;
	
	CLProtocol::UnionGoldConsignmentDealRecordRes res;

	do 
	{
		auto iter = m_RoleRecordMap.find(player->GetID());
		if (iter == m_RoleRecordMap.end())
			break;

		UInt32 timeOut = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_TRADE_RECORD_SHOW_TIME);
		UInt32 showTime = timeOut * 3600 * 1000;

		for (auto& it : iter->second)
		{
			if (CURRENT_TIME.MSec() > it.tradeTime + showTime)
				continue;

			if (it.orderType == GCOT_BUY)
			{
				res.buyRecord.push_back(it);
			}
			else if (it.orderType == GCOT_SELL)
			{
				res.sellRecord.push_back(it);
			}
		}

	} while (0);

	player->SendProtocol(res);
}

void GoldConsignmentService::HandlerOwnerOrderReq(USPlayer* player)
{
	CLProtocol::UnionGoldConsignmentOwnerOrderRes res;

	res.sellAccNum = _FindAccOrderNum(player->GetAccID(), player->GetZoneID(), GCOT_SELL);
	res.buyAccNum = _FindAccOrderNum(player->GetAccID(), player->GetZoneID(), GCOT_BUY);

	auto buyIt = m_AccOrderMap.find({ player->GetAccID(), player->GetZoneID(), GCOT_BUY });
	if (buyIt != m_AccOrderMap.end())
	{
		for (auto& it : buyIt->second)
		{
			if (it->roleId != player->GetID())
				continue;

			res.buyOrderList.push_back(*it);
		}
	}

	auto sellIt = m_AccOrderMap.find({ player->GetAccID(), player->GetZoneID(), GCOT_SELL });
	if (sellIt != m_AccOrderMap.end())
	{
		for (auto& it : sellIt->second)
		{
			if (it->roleId != player->GetID())
				continue;

			res.sellOrderList.push_back(*it);
		}
	}

	player->SendProtocol(res);
}

void GoldConsignmentService::HandlerCancelOrderReq(USPlayer* player, UInt32 orderType, UInt64 orderId)
{
	CLProtocol::UnionGoldConsignmentCancelOrderRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.orderId = orderId;

	do
	{
		if (_ZoneIsCloseOrder(player->GetZoneID()))
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_SERVER_BUSY;
			break;
		}

		if (!_RoleIsHasOrder(player, orderType, orderId))
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_NOT_EXIST_ORDER;
			break;
		}

		// 从订单列表移除
		GoldConsignmentOrderPtr orderPtr = _FindOrder(orderType, orderId);
		if (NULL == orderPtr)
		{
			ErrorLogStream << PLAYERINFO(player) << "not find order id:" << orderId << LogStream::eos;
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_NOT_EXIST_ORDER;
			break;
		}

		ItemRewardVec rewardVec;
		if (orderPtr->orderType == GCOT_BUY)
		{
			rewardVec.push_back(ItemReward(ITEM_POINT, orderPtr->remainPoint(), 0, 0));
		}
		else if (orderPtr->orderType == GCOT_SELL)
		{
			rewardVec.push_back(ItemReward(ITEM_GOLD, orderPtr->tradeNum * GetTradeBase(), 0, 0));
		}

		// 更新挡位
		_GradeCancelOrder(orderPtr);

		std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_CANCEL_ORDER, orderPtr->orderId);
		player->AddRewards(reason, rewardVec, false);

		_RemoveOrder(orderPtr->orderType, orderPtr->orderId);
		_DBDelOrder(orderPtr->orderId);

		InfoLogStream << PLAYERINFO(player) << "cancel order:" << orderPtr->LogStr() << LogStream::eos;

	} while (0);

	player->SendProtocol(res);
}

void GoldConsignmentService::ZoneActivityEnd(UInt32 zoneId, UInt32 endTime)
{
	if ((UInt32)CURRENT_TIME.Sec() < endTime)
	{
		return;
	}

	InfoLogStream << "zone activity end:" << zoneId << LogStream::eos;

	std::list<GoldConsignmentOrderPtr> cancelOrderList;

	for (auto& iter : m_SellOrderMap)
	{
		if (iter.second->zoneId == zoneId)
		{
			cancelOrderList.push_back(iter.second);
		}
	}

	for (auto& iter : m_BuyOrderMap)
	{
		if (iter.second->zoneId == zoneId)
		{
			cancelOrderList.push_back(iter.second);
		}
	}

	for (auto& it : cancelOrderList)
	{
		std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_ACTIVITY_END, it->orderId);
		std::string title = SysNotifyMgr::Instance()->MakeString(10087);
		std::string content;
		if (it->orderType == GCOT_SELL)
			content = SysNotifyMgr::Instance()->MakeString(10088, it->tradeNum * GetTradeBase());
		else if (it->orderType == GCOT_BUY)
			content = SysNotifyMgr::Instance()->MakeString(10089, it->tradeNum * GetTradeBase());
		
		_CancelOrderReturn(it, reason, title, content, GCPT_ACTIVITY_END);
		InfoLogStream << "activity end cancel order:" << it->LogStr() << LogStream::eos;
	}
}

bool GoldConsignmentService::LoadOrderData(CLRecordCallback* callback)
{
	if (NULL == callback) return false;

	while (callback->NextRow())
	{
		GoldConsignmentOrderPtr orderPtr = std::make_shared<GoldConsignmentOrder>();
		orderPtr->orderId = callback->GetKey();
		orderPtr->orderType = callback->GetData("order_type");
		orderPtr->zoneId = callback->GetData("zone_id");
		orderPtr->accId = callback->GetData("acc_id");
		orderPtr->roleId = callback->GetData("role_id");
		orderPtr->unitPrice = callback->GetData("unit_price");
		orderPtr->tradeNum = callback->GetData("trade_num");
		orderPtr->sumbitTime = callback->GetData("submit_time");
		orderPtr->expireTime = orderPtr->sumbitTime + GetHangListTime();
		orderPtr->totalPoint = callback->GetData("total_point");
		orderPtr->costPoint = callback->GetData("cost_point");
		orderPtr->returnPoint = callback->GetData("return_point");

		_AddOrder(orderPtr);
	}

	return true;
}

bool GoldConsignmentService::LoadTradeRecord(CLRecordCallback* callback)
{
	if (NULL == callback) return false;

	while (callback->NextRow())
	{
		GoldConsignmentTradeRecord record;
		record.guid = callback->GetKey();
		record.roleId = callback->GetData("role_id");
		record.zoneId = callback->GetData("zone_id");
		record.accId = callback->GetData("acc_id");
		record.tradeSerialId = callback->GetData("tarde_serial");
		record.orderId = callback->GetData("order_id");
		record.orderType = callback->GetData("order_type");
		record.unitPrice = callback->GetData("unit_price");
		record.tradeNum = callback->GetData("trade_num");
		record.param = callback->GetData("param");
		record.tradeTime = callback->GetData("trade_time");
		_AddTradeRecord(record);
	}

	return true;
}

bool GoldConsignmentService::LoadTradePrice(CLRecordCallback* callback)
{
	if (NULL == callback) return false;

	while (callback->NextRow())
	{
		m_LastAveragePrice = callback->GetData("last_average");
		m_CurAveragePrice = callback->GetData("cur_average");
		m_RecentAveragePrice = callback->GetData("recent_average");
		m_TodayCostPoint = callback->GetData("today_cost_point");
		m_TodayDealNum = callback->GetData("today_deal_num");
	}

	// 金币寄售均价
	if (m_LastAveragePrice == 0)
	{
		InitUnitPrice();
		_DBInsertPrice();
	}

	CLSelectCommand* cmd_2 = CLRecordClient::Instance()->PrepareSelectCommand("t_gold_consignment_order");
	CLRecordClient::Instance()->SendCommand(cmd_2, new SelectGoldConsignmentOrderCallback(this));

	return true;
}

bool GoldConsignmentService::LoadUntreatProfit(CLRecordCallback* callback)
{
	if (NULL == callback) return false;

	while (callback->NextRow())
	{
		GoldConsignmentProfit profit;
		profit.guid = callback->GetKey();
		profit.tradeSerial = callback->GetData("trade_serial");
		profit.orderId = callback->GetData("order_id");
		profit.orderType = callback->GetData("order_type");
		profit.zoneId = callback->GetData("zone_id");
		profit.accId = callback->GetData("acc_id");
		profit.roleId = callback->GetData("role_id");
		profit.profit = callback->GetData("profit").GetString();
		profit.content = callback->GetData("content").GetString();
		profit.profitType = callback->GetData("profit_type");
		profit.isHandle = callback->GetData("is_handle");

		m_UntreatProfitMap[profit.guid] = profit;
	}

	return true;
}

bool GoldConsignmentService::LoadUntreatCreditPoint(CLRecordCallback* callback)
{
	if (NULL == callback) return false;

	while (callback->NextRow())
	{
		GoldConsignmentCreditPoint cPoint;
		cPoint.guid = callback->GetKey();
		cPoint.tradeSerial = callback->GetData("trade_serial");
		cPoint.orderId = callback->GetData("order_id");
		cPoint.zoneId = callback->GetData("zone_id");
		cPoint.accId = callback->GetData("acc_id");
		cPoint.roleId = callback->GetData("role_id");
		cPoint.creditPoint = callback->GetData("credit_point");
		cPoint.isHandle = callback->GetData("is_handle");

		m_UntreatCreditPointMap[cPoint.guid] = cPoint;
	}

	return true;
}

void GoldConsignmentService::InitUnitPrice()
{
	// 初始化平均交易价,第一次从配置读取
	UInt32 price = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_AVERAGE_PRICE);
	_SetLastAveragePrice(price);
	_SetCurAveragePrice(price);
	_SetRecentAveragePrice(price);

	m_TodayCostPoint = 0;
	m_TodayDealNum = 0;
}

void GoldConsignmentService::ModifyUnitPrice()
{
	InitUnitPrice();
	_DBUpdatePrice(DB_LAST_PRICE | DB_CUR_PRICE | DB_RECENT_PRICE | DB_TODAY_COST_POINT | DB_TODAY_DEAL_NUM);
}

bool GoldConsignmentService::SellGoldTransResult(USPlayer* player, GoldTradeThing* trans)
{
	if (NULL == player || NULL == trans)
		return false;

	// 先创建一个订单
	GoldConsignmentOrderPtr orderPtr = _CreateOrder(player, trans->GetOrderType(), trans->GetUnitPrice(), trans->GetTradeNum());
	if (NULL == orderPtr)
		return false;

	CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
	res.submitId = trans->GetSubmitId();
	res.retCode = ErrorCode::SUCCESS;
	res.orderType = orderPtr->orderType;

	// 一次性交易，再次检查量够不够
	if (trans->GetTradeType() == GCTT_ONCE)
	{
		UInt32 ret = _CheckMatchUnitPriceBuyOrder(player->GetAccID(), orderPtr->unitPrice, orderPtr->tradeNum);
		if (ret != ErrorCode::SUCCESS)
		{
			res.retCode = ret;
			player->SendProtocol(res);
			return false;
		}
	}
	else
	{
		if (m_BuyGradeMap.empty() || orderPtr->unitPrice > m_BuyGradeMap.begin()->first)
		{
			InfoLogStream << PLAYERINFO(player) << " create sell order! all hang list order:" << orderPtr->LogStr() << LogStream::eos;

			// 撮合交易， 没有买家或者没有匹配的订单， 就直接挂单
			_SumbitOrder(orderPtr);
			res.hangListNum = orderPtr->tradeNum;
			player->SendProtocol(res);
			return true;
		}
	}

	InfoLogStream << PLAYERINFO(player) << " create sell order!" << orderPtr->LogStr() << LogStream::eos;

	// 匹配要买的挂单
	_MatchBuyOrder(player, orderPtr, trans->GetSubmitId());

	InfoLogStream << PLAYERINFO(player) << " match buy order success!" << orderPtr->LogStr() << LogStream::eos;

	return true;
}

bool GoldConsignmentService::BuyGoldTransResult(USPlayer* player, GoldTradeThing* trans)
{
	if (NULL == player || NULL == trans)
		return false;

	// 先创建订单
	GoldConsignmentOrderPtr orderPtr = _CreateOrder(player, trans->GetOrderType(), trans->GetUnitPrice(),
		trans->GetTradeNum(), trans->GetItemCost().point);
	if (NULL == orderPtr)
		return false;

	CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
	res.submitId = trans->GetSubmitId();
	res.retCode = ErrorCode::SUCCESS;
	res.orderType = orderPtr->orderType;

	// 一次性交易，再次检查量够不够
	if (trans->GetTradeType() == GCTT_ONCE)
	{
		UInt32 ret = _CheckMatchUnitPriceSellOrder(player->GetAccID(), orderPtr->unitPrice, orderPtr->tradeNum);
		if (ret != ErrorCode::SUCCESS)
		{
			res.retCode = ret;
			player->SendProtocol(res);
			return false;
		}
	}
	else
	{
		// 撮合交易， 没有卖家或者没有匹配的订单， 就直接挂单
		if (m_SellGradeMap.empty() || orderPtr->unitPrice < m_SellGradeMap.begin()->first)
		{
			InfoLogStream << PLAYERINFO(player) << " create buy order! all hang list order:" << orderPtr->LogStr() << LogStream::eos;

			_SumbitOrder(orderPtr);
			res.hangListNum = orderPtr->tradeNum;
			res.param1 = orderPtr->totalPoint;
			player->SendProtocol(res);
			return true;
		}
	}

	InfoLogStream << PLAYERINFO(player) << " create buy order!" << orderPtr->LogStr() << LogStream::eos;

	// 匹配要卖金的挂单
	_MatchSellOrder(player, orderPtr, trans->GetTradeType(), trans->GetSubmitId());

	InfoLogStream << PLAYERINFO(player) << " match sell order success!" << orderPtr->LogStr() << LogStream::eos;

	return true;
}

std::string GoldConsignmentService::GMSumbitOrder(UInt32 tradeType, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum)
{
	if (tradeType == 0 || orderType == 0 || unitPrice == 0 || tradeNum == 0)
	{
		return "param error!";
	}

	// 检查单价的涨幅
	if (_CheckWavePrice(unitPrice))
	{
		return "price wave error! ";
	}

	// 检查保护价
	UInt32 minProtectPrice = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_PROTECT_PRICE);
	UInt32 maxProtectPrice = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MAX_PROTECT_PRICE);
	if (unitPrice < minProtectPrice || unitPrice> maxProtectPrice)
	{
		return "over protect price!";
	}

	// 检查交易数量
	UInt32 minNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_GOLD_NUM);
	UInt32 maxNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MAX_GOLD_NUM);
	if (tradeNum * GetTradeBase() < minNum || tradeNum * GetTradeBase() > maxNum)
	{
		return "trade num error!";
	}

	// 上架订单，无归属订单属于 系统上架的订单

	switch (orderType)
	{
	case GCOT_SELL:
	{
		// 如果是一次性交易，先检查挡位，买金总量够不够
		if (tradeType == GCTT_ONCE && _CheckMatchUnitPriceBuyOrder(0, unitPrice, tradeNum) != ErrorCode::SUCCESS)
		{
			return "goods not enough!";
		}
		break;
	}
	case GCOT_BUY:
	{
		// 如果是一次性交易，先检查挡位，卖金总量够不够
		if (tradeType == GCTT_ONCE && _CheckMatchUnitPriceSellOrder(0, unitPrice, tradeNum) != ErrorCode::SUCCESS)
		{
			return "goods not enough!";
		}
		break;
	}
	default:return "unknown order type!";
	}




	return "success";
}

bool GoldConsignmentService::_CheckWavePrice(UInt32 unitPrice)
{
	UInt32 waveRate = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_PRICE_WAVE_RATE);
	float wavePrice = (float)waveRate / GOLD_CONSIGNMENT_RATE * m_LastAveragePrice;

	UInt32 minPrice = UInt32((float)m_LastAveragePrice - wavePrice);
	UInt32 maxPrice = m_LastAveragePrice + (UInt32)wavePrice;

	if(unitPrice < minPrice || unitPrice > maxPrice)
	{
		return true;
	}

	return false;
}

bool GoldConsignmentService::_IsClose()
{
	return UInt32(CURRENT_TIME.Sec()) >= m_StartTradeStamp;
}

void GoldConsignmentService::_CheckDealData()
{
	UInt32 dirtyFlag = 0xFFFFFFFF;
	UInt32 flag = 0;
	if (m_TodayCostPoint.IsDirty(dirtyFlag))
	{
		flag |= DB_TODAY_COST_POINT;
		m_TodayCostPoint.ClearDirty(dirtyFlag);
	}

	if (m_TodayDealNum.IsDirty(dirtyFlag))
	{
		flag |= DB_TODAY_DEAL_NUM;
		m_TodayDealNum.ClearDirty(dirtyFlag);
	}

	if (flag != 0)
	{
		_DBUpdatePrice(flag);
	}
}

void GoldConsignmentService::_CheckClose(const Avalon::Time& now)
{
	UInt32 closeStart = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_CLOSE_START_TIME);
	UInt32 closeEnd = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_CLOSE_END_TIME);
	if (closeStart == 0 && closeEnd == 0)
	{
		m_StartTradeStamp = 0;
		return;
	}

	UInt32 todayStartTime = UInt32(GetDayBeginTime(now) / 1000);
	Avalon::Date date(now);
	if (closeEnd < closeStart)
	{
		if (UInt32(date.Hour()) >= closeStart || UInt32(date.Hour()) < closeEnd)
		{
			if (UInt32(date.Hour()) >= closeStart)
				m_StartTradeStamp = todayStartTime + closeStart * 3600;
			else
				m_StartTradeStamp = todayStartTime - DAY_TO_SEC + closeStart * 3600;
		}
		else
		{
			m_StartTradeStamp = todayStartTime + closeStart * 3600;
		}
	}
	else
	{
		if (UInt32(date.Hour()) >= closeStart && UInt32(date.Hour()) < closeEnd)
		{
			m_StartTradeStamp = todayStartTime + closeStart * 3600;
		}
		else
		{
			if (UInt32(date.Hour()) < closeStart)
				m_StartTradeStamp = todayStartTime + closeStart * 3600;
			else
				m_StartTradeStamp = todayStartTime + DAY_TO_SEC + closeStart * 3600;
		}
	}
}

bool GoldConsignmentService::_ZoneIsCloseOrder(UInt32 zoneId)
{
	std::set<UInt32> zoneSet = USApplication::Instance()->GetCloseOrderZone();
	auto iter = zoneSet.find(zoneId);
	return iter != zoneSet.end();
}

void GoldConsignmentService::_CheckOrderCloseZone()
{
	std::set<UInt32>& zoneSet = USApplication::Instance()->GetCloseOrderZone();
	if (zoneSet.empty())
	{
		return;
	}

	std::map<UInt32, std::list<GoldConsignmentOrderPtr>> closeOrderMap;

	for (auto& iter : m_SellOrderMap)
	{
		if (zoneSet.count(iter.second->zoneId))
		{
			closeOrderMap[iter.second->zoneId].push_back(iter.second);
		}
	}

	for (auto& iter : m_BuyOrderMap)
	{
		if (zoneSet.count(iter.second->zoneId))
		{
			closeOrderMap[iter.second->zoneId].push_back(iter.second);
		}
	}

	for (auto& it : closeOrderMap)
	{
		InfoLogStream << "start cancel order zoneId:" << it.first << LogStream::eos;
		for (auto& tt : it.second)
		{
			std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_CLOSE_ZONE, tt->orderId);
			std::string title = SysNotifyMgr::Instance()->MakeString(10087);
			std::string content;
			if (tt->orderType == GCOT_SELL)
				content = SysNotifyMgr::Instance()->MakeString(10088, tt->tradeNum * GetTradeBase());
			else if (tt->orderType == GCOT_BUY)
				content = SysNotifyMgr::Instance()->MakeString(10089, tt->tradeNum * GetTradeBase());

			_CancelOrderReturn(tt, reason, title, content, GCPT_CLOSE_ZONE);

			InfoLogStream << "cancel zoneId order:" << tt->LogStr() << LogStream::eos;
		}
		InfoLogStream << "end cancel order zoneId:" << it.first << LogStream::eos;
	}
}

void GoldConsignmentService::_CheckOrderCloseRole()
{
	std::set<UInt64>& roleSet = USApplication::Instance()->GetCloseOrderRole();
	if (roleSet.empty())
	{
		return;
	}

	std::map<UInt64, std::list<GoldConsignmentOrderPtr>> closeOrderMap;

	for (auto& iter : m_SellOrderMap)
	{
		if (roleSet.count(iter.second->roleId))
		{
			closeOrderMap[iter.second->roleId].push_back(iter.second);
		}
	}

	for (auto& iter : m_BuyOrderMap)
	{
		if (roleSet.count(iter.second->roleId))
		{
			closeOrderMap[iter.second->roleId].push_back(iter.second);
		}
	}

	for (auto& it : closeOrderMap)
	{
		for (auto& tt : it.second)
		{
			std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_CLOSE_ROLE, tt->orderId);
			std::string title;
			std::string content;
			if (tt->orderType == GCOT_SELL)
			{
				title = SysNotifyMgr::Instance()->MakeString(10083);
				content = SysNotifyMgr::Instance()->MakeString(10084, tt->tradeNum * GetTradeBase());
			}
			else if (tt->orderType == GCOT_BUY)
			{
				title = SysNotifyMgr::Instance()->MakeString(10085);
				content = SysNotifyMgr::Instance()->MakeString(10086, tt->tradeNum * GetTradeBase());
			}

			_CancelOrderReturn(tt, reason, title, content, GCPT_CLOSE_ROLE);

			InfoLogStream << "cancel roleId order:" << tt->LogStr() << LogStream::eos;
		}
	}
}

void GoldConsignmentService::_CheckOrderClose()
{
	std::set<UInt64>& orderSet = USApplication::Instance()->GetCloseOrder();
	if (orderSet.empty())
	{
		return;
	}

	std::list<GoldConsignmentOrderPtr> closeOrderList;
	for (auto& iter : orderSet)
	{
		auto sell = m_SellOrderMap.find(iter);
		if (sell != m_SellOrderMap.end())
		{
			closeOrderList.push_back(sell->second);
		}
		else
		{
			auto buy = m_BuyOrderMap.find(iter);
			if (buy != m_BuyOrderMap.end())
			{
				closeOrderList.push_back(buy->second);
			}
		}
	}

	for (auto& order : closeOrderList)
	{
		InfoLogStream << "admin close orderId:" << order->LogStr() << LogStream::eos;
		std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_ADMIN_CLOSE, order->orderId);
		std::string title;
		std::string content;
		if (order->orderType == GCOT_SELL)
		{
			title = SysNotifyMgr::Instance()->MakeString(10083);
			content = SysNotifyMgr::Instance()->MakeString(10084, order->tradeNum * GetTradeBase());
		}
		else if (order->orderType == GCOT_BUY)
		{
			title = SysNotifyMgr::Instance()->MakeString(10085);
			content = SysNotifyMgr::Instance()->MakeString(10086, order->tradeNum * GetTradeBase());
		}

		_CancelOrderReturn(order, reason, title, content, GCPT_ADMIN_CLOSE);
	}
}

void GoldConsignmentService::_CheckTimeOutOrder(const Avalon::Time& now)
{
	// 超时卖金订单
	std::vector<GoldConsignmentOrderPtr> timeOutOrderVec;
	for (auto& iter : m_SellOrderMap)
	{
		if (now.MSec() >= iter.second->sumbitTime + GetHangListTime())
			timeOutOrderVec.push_back(iter.second);
	}

	// 超时买金订单
	for (auto& iter : m_BuyOrderMap)
	{
		if (now.MSec() >= iter.second->sumbitTime + GetHangListTime())
			timeOutOrderVec.push_back(iter.second);
	}

	for (auto& orderPtr : timeOutOrderVec)
	{
		InfoLogStream << "order time out:" << orderPtr->LogStr() << LogStream::eos;
		std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_TIMEOUT_ORDER, orderPtr->orderId);
		std::string title;
		std::string content;
		if (orderPtr->orderType == GCOT_SELL)
		{
			title = SysNotifyMgr::Instance()->MakeString(10083);
			content = SysNotifyMgr::Instance()->MakeString(10084, orderPtr->tradeNum * GetTradeBase());
		}
		else if (orderPtr->orderType == GCOT_BUY)
		{
			title = SysNotifyMgr::Instance()->MakeString(10085);
			content = SysNotifyMgr::Instance()->MakeString(10086, orderPtr->tradeNum * GetTradeBase());
		}
		_CancelOrderReturn(orderPtr, reason, title, content, GCPT_TIME_OUT);
	}
}

void GoldConsignmentService::_CancelOrderReturn(GoldConsignmentOrderPtr orderPtr, const std::string& reason, const std::string& title, 
	const std::string& content, UInt32 profitType)
{
	// 邮件返还
	std::string sender = SysNotifyMgr::Instance()->MakeString(10076);
	ItemRewardVec rewardVec;
	if (orderPtr->orderType == GCOT_BUY)
	{
		rewardVec.push_back(ItemReward(ITEM_POINT, orderPtr->remainPoint(), 0, 0));
	}
	else if (orderPtr->orderType == GCOT_SELL)
	{
		rewardVec.push_back(ItemReward(ITEM_GOLD, orderPtr->tradeNum * GetTradeBase(), 0, 0));
	}

	{
		UInt64 profitGuid = GenProfitGuid();
		UInt32 isHandle = 1;
		if ( !SysUtility::SendMail(orderPtr->zoneId, orderPtr->roleId, sender, title, content, reason, rewardVec, profitGuid))
		{
			ErrorLogStream << "cancel order mail failed!" << orderPtr->LogStr() << LogStream::eos;
			isHandle = 0;
		}

		_AddUntreatProfit(profitGuid, orderPtr, rewardVec, content, 0, profitType, isHandle);
	}

	// 更新挡位
	_GradeCancelOrder(orderPtr);

	InfoLogStream << "order cancel:" << orderPtr->LogStr() << LogStream::eos;

	_RemoveOrder(orderPtr->orderType, orderPtr->orderId);
	_DBDelOrder(orderPtr->orderId);
}

UInt32 GoldConsignmentService::_CheckSellOrder(USPlayer* player, UInt32 tradeType, UInt32 unitPrice, UInt32 tradeNum, UInt32 param, UInt32 submitId)
{
	float minUnitPrice = GetMinPrice(unitPrice);

	UInt32 commissionCfg = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_COMMISSIOM);
	UInt32 commission = (UInt32)ceil(tradeNum * minUnitPrice * commissionCfg / GOLD_CONSIGNMENT_RATE);

	// 检查账号下所有订单的预计信用点券收入，会不会超过上限
	if (_EstimateAccCreditPointProfit(player->GetZoneID(), player->GetAccID()) + ((UInt32)minUnitPrice * tradeNum - commission) > param)
	{
		return ErrorCode::GOLD_CONSIGNMENT_OVER_ACC_PROFIT_LIMIT;
	}

	// 如果是一次性交易，先检查挡位，买金总量够不够
	if (tradeType == GCTT_ONCE)
	{
		UInt32 ret = _CheckMatchUnitPriceBuyOrder(player->GetAccID(), unitPrice, tradeNum);
		if (ret != ErrorCode::SUCCESS)
		{
			return ret;
		}
	}

	// 扣除金币事务
	auto itemThing = new GoldTradeThing(player, this, GCOT_SELL, tradeType, unitPrice, tradeNum, submitId);
	{
		ItemThingCost itemCost;
		itemCost.gold = tradeNum * GetTradeBase();
		std::string reason = GetReason(LogSourceType(SOURCE_GOLD_CONSIGNMENT_SELL), unitPrice, tradeNum);

		itemThing->SetFunType(USFT_GOLD_CONSIGNMENT);
		itemThing->SetReason(reason);
		itemThing->SetItemCost(itemCost);
	}

	if ( !player->BeginItemThing(itemThing))
	{
		CL_SAFE_DELETE(itemThing);
		ErrorLogStream << PLAYERINFO(player) << "order trans failed unitPrice:" << unitPrice << " tradeNum:" << tradeNum << LogStream::eos;
		return ErrorCode::GOLD_CONSIGNMENT_ORDER_BEING_PROCESS;
	}

	return ErrorCode::SUCCESS;
}

UInt32 GoldConsignmentService::_CheckBuyOrder(USPlayer* player, UInt32 tradeType, UInt32 unitPrice, UInt32 tradeNum, UInt32 submitId)
{
	// 如果是一次性交易，先检查挡位，卖金总量够不够
	if (tradeType == GCTT_ONCE)
	{
		UInt32 ret = _CheckMatchUnitPriceSellOrder(player->GetAccID(), unitPrice, tradeNum);
		if (ret != ErrorCode::SUCCESS)
		{
			return ret;
		}
	}

	UInt32 costPoint = UInt32(ceil(GetMinPrice(unitPrice))) * tradeNum;
	if (costPoint == 0)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 扣除点券事务
	auto itemThing = new GoldTradeThing(player, this, GCOT_BUY, tradeType, unitPrice, tradeNum, submitId);
	{
		ItemThingCost itemCost;
		itemCost.point = costPoint;
		std::string reason = GetReason(LogSourceType(SOURCE_GOLD_CONSIGNMENT_BUY), unitPrice, tradeNum);

		itemThing->SetFunType(USFT_GOLD_CONSIGNMENT);
		itemThing->SetReason(reason);
		itemThing->SetItemCost(itemCost);
	}

	if ( !player->BeginItemThing(itemThing))
	{
		CL_SAFE_DELETE(itemThing);
		ErrorLogStream << PLAYERINFO(player) << "order trans failed unitPrice:" << unitPrice 
			<< " tradeNum:" << tradeNum << LogStream::eos;
		return ErrorCode::GOLD_CONSIGNMENT_ORDER_BEING_PROCESS;
	}

	return ErrorCode::SUCCESS;
}

void GoldConsignmentService::_AddOrder(GoldConsignmentOrderPtr orderPtr)
{
	switch (orderPtr->orderType)
	{
	case GCOT_SELL:
	{
		m_SellOrderMap[orderPtr->orderId] = orderPtr;
		break;
	}
	case GCOT_BUY:
	{
		m_BuyOrderMap[orderPtr->orderId] = orderPtr;
		break;
	}
	default: return;
	}

	_AddGradeOrder(orderPtr);
	_AddAccOrder(orderPtr);
}

void GoldConsignmentService::_AddGradeOrder(GoldConsignmentOrderPtr orderPtr)
{
	// 价钱不符合当日的单价范围，不加入订单匹配挡位
	if ( _CheckWavePrice( orderPtr->unitPrice))
	{
		return;
	}

	switch (orderPtr->orderType)
	{
	case GCOT_SELL: 
	{
		auto it = m_SellGradeMap.find(orderPtr->unitPrice);
		if (it == m_SellGradeMap.end())
		{
			OrderGradeData data;
			data.AddOrder(orderPtr->sumbitTime, orderPtr->orderId, orderPtr->accId, orderPtr->tradeNum);
			m_SellGradeMap.insert(std::make_pair(orderPtr->unitPrice, data));
		}
		else
		{
			it->second.AddOrder(orderPtr->sumbitTime, orderPtr->orderId, orderPtr->accId, orderPtr->tradeNum);
		}
	}
	break;
	case GCOT_BUY:
	{
		auto it = m_BuyGradeMap.find(orderPtr->unitPrice);
		if (it == m_BuyGradeMap.end())
		{
			OrderGradeData data;
			data.AddOrder(orderPtr->sumbitTime, orderPtr->orderId, orderPtr->accId, orderPtr->tradeNum);
			m_BuyGradeMap.insert(std::make_pair(orderPtr->unitPrice, data));
		}
		else
		{
			it->second.AddOrder(orderPtr->sumbitTime, orderPtr->orderId, orderPtr->accId, orderPtr->tradeNum);
		}
	}
	break;
	default:
		return;
	}
}

void GoldConsignmentService::_AddAccOrder(GoldConsignmentOrderPtr orderPtr)
{
	GCAccOrderKey key(orderPtr->accId, orderPtr->zoneId, orderPtr->orderType);
	m_AccOrderMap[key].push_back(orderPtr);
}

void GoldConsignmentService::_AddTradeRecord(GoldConsignmentTradeRecord& record)
{
	m_RoleRecordMap[record.roleId].push_back(record);
}

void GoldConsignmentService::_DBInsertOrder(GoldConsignmentOrderPtr orderPtr)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gold_consignment_order", orderPtr->orderId);
	cmd->PutData("order_type", orderPtr->orderType);
	cmd->PutData("zone_id", orderPtr->zoneId);
	cmd->PutData("acc_id", orderPtr->accId);
	cmd->PutData("role_id", orderPtr->roleId);
	cmd->PutData("unit_price", orderPtr->unitPrice);
	cmd->PutData("trade_num", orderPtr->tradeNum);
	cmd->PutData("submit_time", orderPtr->sumbitTime);
	cmd->PutData("total_point", orderPtr->totalPoint);
	cmd->PutData("cost_point", orderPtr->costPoint);
	cmd->PutData("return_point", orderPtr->returnPoint);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBDelOrder(UInt64 orderId)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_gold_consignment_order", orderId);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBUpdateOrder(GoldConsignmentOrderPtr orderPtr)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gold_consignment_order", orderPtr->orderId, true);
	cmd->PutData("trade_num", orderPtr->tradeNum);
	cmd->PutData("cost_point", orderPtr->costPoint);
	cmd->PutData("return_point", orderPtr->returnPoint);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBInsertTradeRecord(GoldConsignmentTradeRecord& record)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gold_consignment_record", record.guid);
	cmd->PutData("role_id", record.roleId);
	cmd->PutData("zone_id", record.zoneId);
	cmd->PutData("acc_id", record.accId);
	cmd->PutData("tarde_serial", record.tradeSerialId);
	cmd->PutData("order_id", record.orderId);
	cmd->PutData("order_type", record.orderType);
	cmd->PutData("unit_price", record.unitPrice);
	cmd->PutData("trade_num", record.tradeNum);
	cmd->PutData("param", record.param);
	cmd->PutData("trade_time", record.tradeTime);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBInsertPrice()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gold_consignment_price", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("last_average", m_LastAveragePrice);
	cmd->PutData("cur_average", m_CurAveragePrice);
	cmd->PutData("recent_average", m_RecentAveragePrice);
	cmd->PutData("today_cost_point", m_TodayCostPoint);
	cmd->PutData("today_deal_num", m_TodayDealNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBUpdatePrice(UInt32 flag)
{
	if (flag == 0)
		return;
	
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gold_consignment_price", "1", true);

	if (flag & DB_LAST_PRICE)
	{
		cmd->PutData("last_average", m_LastAveragePrice);
	}

	if (flag & DB_CUR_PRICE)
	{
		cmd->PutData("cur_average", m_CurAveragePrice);
	}

	if (flag & DB_RECENT_PRICE)
	{
		cmd->PutData("recent_average", m_RecentAveragePrice);
	}

	if (flag & DB_TODAY_COST_POINT)
	{
		cmd->PutData("today_cost_point", m_TodayCostPoint);
	}

	if (flag & DB_TODAY_DEAL_NUM)
	{
		cmd->PutData("today_deal_num", m_TodayDealNum);
	}

	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_MatchBuyOrder(USPlayer* player, GoldConsignmentOrderPtr orderPtr, UInt32 submitId)
{
	CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
	res.submitId = submitId;
	res.retCode = ErrorCode::SUCCESS;
	res.orderType = orderPtr->orderType;

	do
	{
		UInt32 oldNum = orderPtr->tradeNum;
		UInt32 remainNum = orderPtr->tradeNum;
		UInt32 gradeNum = 0;
		float sellProfit = 0;
		UInt64 tradeSerial = CLRecordClient::Instance()->GenGuid();
		bool IsHangList = false;

		std::vector<UInt32> emptyGrade;
		// 匹配买金订单，从最高挡位开始匹配
		for (auto& iter : m_BuyGradeMap)
		{
			if (orderPtr->unitPrice > iter.first)
			{
				// 剩下没撮合成功，直接将剩余要卖的挂单
				IsHangList = true;
				orderPtr->tradeNum = remainNum;
				_SumbitOrder(orderPtr);
				break;
			}
			else
			{
				++gradeNum;

				// 撮合订单
				remainNum = _MatchBuyGrade(player, remainNum, tradeSerial, orderPtr->unitPrice, iter.second, sellProfit);
			}

			// 该挡位全匹配了，空了
			if (iter.second.tradeTotalNum == 0)
			{
				emptyGrade.push_back(iter.first);
			}

			if (remainNum == 0)
				break;
		}

		// 删除空档位
		for (auto unit : emptyGrade)
		{
			m_BuyGradeMap.erase(unit);
		}

		// 量不够，挂单
		if (remainNum != 0 && !IsHangList)
		{
			IsHangList = true;
			orderPtr->tradeNum = remainNum;
			_SumbitOrder(orderPtr);
		}

		// 没有交易，break
		if (gradeNum == 0 || oldNum == remainNum)
		{
			res.hangListNum = remainNum;
			break;
		}

		UInt32 sellNum = (oldNum - remainNum) * GetTradeBase();

		// 计算手续费
		UInt32 commissionCfg = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_COMMISSIOM);
		UInt32 commission = (UInt32)ceil(sellProfit * commissionCfg / GOLD_CONSIGNMENT_RATE);

		// 减去手续费后的真实收益
		UInt32 sellRealProfit = (UInt32)sellProfit - commission;

		InfoLogStream << PLAYERINFO(player) << "sell gold serialId:" << tradeSerial << " dealNum:" << oldNum - remainNum
			<< " remain:" << remainNum << " unitPrice" << orderPtr->unitPrice << " income:" << sellRealProfit
		   << " order:" << orderPtr->LogStr() << LogStream::eos;

		// 卖金交易记录
		GoldConsignmentTradeRecord record;
		record.guid = CLRecordClient::Instance()->GenGuid();
		record.roleId = player->GetID();
		record.zoneId = player->GetZoneID();
		record.accId = player->GetAccID();
		record.tradeSerialId = tradeSerial;
		record.orderId = orderPtr->orderId;
		record.orderType = orderPtr->orderType;
		record.unitPrice = orderPtr->unitPrice;
		record.tradeNum = oldNum - remainNum;
		record.param = sellRealProfit;
		record.tradeTime = CURRENT_TIME.MSec();
		_AddTradeRecord(record);
		_DBInsertTradeRecord(record);

		// 发放收益，邮件只是提示
		{
			std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_SELL_DEAL, orderPtr->orderId);
			std::string title = SysNotifyMgr::Instance()->MakeString(10079);
			std::string sender = SysNotifyMgr::Instance()->MakeString(10076);
			std::string content = SysNotifyMgr::Instance()->MakeString(10080, sellNum, GetGoldBase(), orderPtr->unitPrice,
				(UInt32)sellProfit, commission, sellRealProfit);
			ItemRewardVec rewardVec;

			UInt32 isHandle = 1;
			UInt64 profitGuid = GenProfitGuid();

			if ( !SysUtility::SendMail(player->GetZoneID(), player->GetID(), sender, title, content, reason, rewardVec, profitGuid))
			{
				isHandle = 0;
				ErrorLogStream << "sell gold mail failed!" << orderPtr->LogStr() << LogStream::eos;
			}
			else
			{
				// 卖金获得信用点券
				_SendCreditPointIncome(orderPtr->zoneId, orderPtr->accId, orderPtr->roleId, orderPtr->orderId, sellRealProfit, reason);
			}

			_AddUntreatProfit(profitGuid, orderPtr, rewardVec, content, tradeSerial, GCPT_SELL_GOLD, isHandle);
			_AddUntreatCreditPoint(orderPtr, sellRealProfit, tradeSerial, isHandle);
		}

		// 生成订单回执,返回客户端
		res.dealNum = oldNum - remainNum;
		res.hangListNum = remainNum;
		res.unitPrice = orderPtr->unitPrice;
		res.param1 = sellRealProfit;
		res.param2 = commission;

	} while (0);

	player->SendProtocol(res);
}

void GoldConsignmentService::_MatchSellOrder(USPlayer* player, GoldConsignmentOrderPtr orderPtr, UInt32 tradeType, UInt32 submitId)
{
	CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
	res.submitId = submitId;
	res.retCode = ErrorCode::SUCCESS;
	res.orderType = orderPtr->orderType;

	do
	{
		UInt32 oldNum = orderPtr->tradeNum;
		UInt32 remainNum = orderPtr->tradeNum;
		UInt32 unitPriceSum = 0;
		UInt32 gradeNum = 0;
		UInt32 costPoint = 0;
		UInt32 returnPoint = 0;
		UInt64 tradeSerial = CLRecordClient::Instance()->GenGuid();
		bool IsHangList = false;
		std::vector<UInt32> emptyGrade;

		// 匹配卖金订单，从最低挡位开始匹配
		for (auto& iter : m_SellGradeMap)
		{
			if (orderPtr->unitPrice < iter.first)
			{
				IsHangList = true;

				// 剩下没撮合成功，直接将剩余要卖的挂单
				orderPtr->tradeNum = remainNum;
				_SumbitOrder(orderPtr);
				break;
			}
			else
			{
				unitPriceSum += iter.first;
				++gradeNum;

				// 撮合订单
				remainNum = _MatchSellGrade(player, remainNum, tradeSerial, orderPtr->unitPrice, iter.second, costPoint, returnPoint);
			}

			// 该挡位全匹配了，空了
			if (iter.second.tradeTotalNum == 0)
			{
				emptyGrade.push_back(iter.first);
			}

			if (remainNum == 0)
				break;
		}

		// 删除空档位
		for (auto unit : emptyGrade)
		{
			InfoLogStream << PLAYERINFO(player) << " empty sell grade unit price:" << unit << LogStream::eos;
			m_SellGradeMap.erase(unit);
		}

		// 消耗点券
		UInt32 realCost = costPoint - returnPoint;
		orderPtr->costPoint = realCost;
		orderPtr->returnPoint = returnPoint;
		orderPtr->tradeNum = remainNum;

		// 量不够，挂单
		if (remainNum != 0 && !IsHangList)
		{
			IsHangList = true;
			_SumbitOrder(orderPtr);
		}

		// 没有交易，break
		if (gradeNum == 0 || oldNum == remainNum)
		{
			res.hangListNum = remainNum;
			break;
		}

		UInt32 dealNum = (oldNum - remainNum) * GetTradeBase();
		UInt32 averagrPrice = (UInt32)ceil(float(unitPriceSum) / gradeNum);

		// 买金交易记录
		GoldConsignmentTradeRecord record;
		record.guid = CLRecordClient::Instance()->GenGuid();
		record.roleId = player->GetID();
		record.zoneId = player->GetZoneID();
		record.accId = player->GetAccID();
		record.tradeSerialId = tradeSerial;
		record.orderId = orderPtr->orderId;
		record.orderType = orderPtr->orderType;
		record.unitPrice = averagrPrice;
		record.tradeNum = oldNum - remainNum;
		record.param = realCost;
		record.tradeTime = CURRENT_TIME.MSec();
		_AddTradeRecord(record);
		_DBInsertTradeRecord(record);

		// 增加成交信息
		_AddDealInfo(realCost, oldNum - remainNum);

		// 发放收益，邮件发放金币
		{
			std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_BUY_DEAL, orderPtr->orderId);
			std::string title = SysNotifyMgr::Instance()->MakeString(10081);
			std::string sender = SysNotifyMgr::Instance()->MakeString(10076);
			std::string content = SysNotifyMgr::Instance()->MakeString(10082, dealNum, GetGoldBase(),
				averagrPrice, realCost, returnPoint);

			ItemRewardVec rewards = { ItemReward(ITEM_GOLD, dealNum, 0, 0), ItemReward(ITEM_POINT, returnPoint, 0, 0) };
			UInt32 isHandle = 1;
			UInt64 profitGuid = GenProfitGuid();

			if ( !SysUtility::SendMail(player->GetZoneID(), player->GetID(), sender, title, content, reason, rewards, profitGuid))
			{
				isHandle = 0;
				ErrorLogStream << "buy gold mail failed!" << orderPtr->LogStr() << LogStream::eos;
			}

			_AddUntreatProfit(profitGuid, orderPtr, rewards, content, tradeSerial, GCPT_BUY_GOLD, isHandle);
		}

		// 生成订单回执,返回客户端
		res.dealNum = oldNum - remainNum;
		res.hangListNum = remainNum;
		res.unitPrice = averagrPrice;
		res.param1 = realCost;
		res.param2 = returnPoint;

		InfoLogStream << PLAYERINFO(player) << "buy gold serial:"<< tradeSerial << " dealNum:" << oldNum - remainNum 
			<< " remainNum:" << remainNum << " averagrPrice:" << averagrPrice << " order:" << orderPtr->LogStr() << LogStream::eos;

	} while (0);

	player->SendProtocol(res);
}

UInt32 GoldConsignmentService::_MatchBuyGrade(USPlayer* player, UInt32 tradeNum, UInt64 serialId, UInt32 sellUnitPrice, 
	OrderGradeData& orderGrade, float& sellProfit)
{
	if (tradeNum == 0) return 0;

	OrderSubmitUnitList& orderList = orderGrade.GetOrderQueue();
	OrderSubmitUnitList::iterator iter = orderList.begin();
	while (iter != orderList.end())
	{
		GoldConsignmentOrderPtr ptr = _FindOrder(GCOT_BUY, iter->orderId);
		if (NULL == ptr)
		{
			ErrorLogStream << "not find order type:" << GCOT_BUY << "id:" << iter->orderId << LogStream::eos;
			++iter;
			continue;
		}

		// 同账号不能交易
		//if (ptr->accId == player->GetAccID())
		//{
		//	++iter;
		//	continue;
		//}

		// 撮合的订单进行交易,修改订单信息
		bool isDelOrder = false;
		UInt32 unitDealNum = 0;
		if (ptr->tradeNum > tradeNum)
		{	
			// 匹配完成
			unitDealNum = tradeNum;
			ptr->tradeNum -= tradeNum;
			tradeNum = 0;
			_DBUpdateOrder(ptr);
		}
		else
		{
			unitDealNum = ptr->tradeNum;
			tradeNum -= ptr->tradeNum;
			ptr->tradeNum = 0;
			iter = orderList.erase(iter);
			_DBDelOrder(ptr->orderId);
			isDelOrder = true;
		}

		// 挡位信息修改
		orderGrade.tradeTotalNum = orderGrade.tradeTotalNum >= unitDealNum ? orderGrade.tradeTotalNum - unitDealNum :0;
		orderGrade.CostAccTradeNum(ptr->accId, unitDealNum);

		UInt32 buyMinPrice = (UInt32)ceil(GetMinPrice(ptr->unitPrice));
		float sellMinPrice = GetMinPrice(sellUnitPrice);

		// 卖金收益信用点券
		sellProfit += unitDealNum * sellMinPrice;

		// 买金消耗点券
		UInt32 unitCost = unitDealNum * buyMinPrice;

		// 买金返还点券
		UInt32 returnPoint = buyMinPrice > (UInt32)sellMinPrice ? (buyMinPrice - (UInt32)sellMinPrice) * unitDealNum : 0;
		UInt32 realCost = unitCost - returnPoint;

		ptr->returnPoint += returnPoint;
		ptr->costPoint += realCost;

		_DBUpdateOrder(ptr);

		// 增加成交信息
		_AddDealInfo(realCost, unitDealNum);

		// 通知客户端，有成交
		//_NotifyOrderDeal(ptr->zoneId, ptr->roleId);

		InfoLogStream << "buy gold deal serial:" << serialId << " unitDealNum:" << unitDealNum << " buyMinPrice:" << buyMinPrice 
			 << " realCost:" << realCost << " returnPoint:" << returnPoint << " sellMinPrice:" << sellMinPrice << " sellProfit:" << sellProfit
			<< " order:" << ptr->LogStr() << LogStream::eos;

		// 买金交易记录
		GoldConsignmentTradeRecord record;
		record.guid = CLRecordClient::Instance()->GenGuid();
		record.roleId = ptr->roleId;
		record.zoneId = ptr->zoneId;
		record.accId = ptr->accId;
		record.tradeSerialId = serialId;
		record.orderId = ptr->orderId;
		record.orderType = GCOT_BUY;
		record.unitPrice = sellUnitPrice;
		record.tradeNum = unitDealNum;
		record.param = realCost;
		record.tradeTime = CURRENT_TIME.MSec();
		_AddTradeRecord(record);
		_DBInsertTradeRecord(record);

		// 成功的订单进行修改，发放买金收益
		{
			std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_BUY_DEAL, sellUnitPrice, unitDealNum);
			std::string title = SysNotifyMgr::Instance()->MakeString(10081);
			std::string sender = SysNotifyMgr::Instance()->MakeString(10076);
			std::string content = SysNotifyMgr::Instance()->MakeString(10082, unitDealNum * GetTradeBase(),
				GetGoldBase(), sellUnitPrice, unitCost, returnPoint);

			ItemRewardVec rewards = { ItemReward(ITEM_GOLD ,unitDealNum * GetTradeBase(), 0, 0), ItemReward(ITEM_POINT, returnPoint, 0, 0) };
			UInt32 isHandle = 1;
			UInt64 profitGuid = GenProfitGuid();

			if ( !SysUtility::SendMail(ptr->zoneId, ptr->roleId, sender, title, content, reason, rewards, profitGuid))
			{
				isHandle = 0;
				ErrorLogStream << "buy grade mail failed!" << " serialId:" << serialId << ptr->LogStr() << LogStream::eos;
			}

			_AddUntreatProfit(profitGuid, ptr, rewards, content, serialId, GCPT_BUY_GOLD, isHandle);
		}

		// 账号订单修改
		if (isDelOrder)
		{
			_RemoveOrder(ptr->orderType, ptr->orderId);
		}

		// 订单匹配完成
		if (tradeNum == 0)
			return 0;
	}

	return tradeNum;
}

UInt32 GoldConsignmentService::_MatchSellGrade(USPlayer* player, UInt32 tradeNum, UInt64 serialId, UInt32 buyUnitPrice, OrderGradeData& orderGrade,
	UInt32& costPoint, UInt32& returnPoint)
{
	if (tradeNum == 0) return 0;

	OrderSubmitUnitList& orderList = orderGrade.GetOrderQueue();
	OrderSubmitUnitList::iterator iter = orderList.begin();
	while (iter != orderList.end())
	{
		GoldConsignmentOrderPtr ptr = _FindOrder(GCOT_SELL, iter->orderId);
		if (NULL == ptr)
		{
			ErrorLogStream << "not find order type:" << GCOT_SELL << "id:" << iter->orderId << LogStream::eos;
			++iter;
			continue;
		}

		// 同账号不能交易
// 		if (ptr->accId == player->GetAccID())
// 		{
// 			++iter;
// 			continue;
// 		}

		// 撮合的订单进行交易,修改订单信息
		bool isDel = false;
		UInt32 unitDealNum = 0;
		if (ptr->tradeNum > tradeNum)
		{
			// 匹配完成
			unitDealNum = tradeNum;
			ptr->tradeNum -= tradeNum;
			tradeNum = 0;

			_DBUpdateOrder(ptr);
		}
		else
		{
			unitDealNum = ptr->tradeNum;
			tradeNum -= ptr->tradeNum;
			ptr->tradeNum = 0;
			iter = orderList.erase(iter);
			_DBDelOrder(ptr->orderId);
			isDel = true;
		}

		UInt32 buyMinPrice = (UInt32)ceil(GetMinPrice(buyUnitPrice));
		float sellMinPrice = GetMinPrice(ptr->unitPrice);

		UInt32 unitBuyCost = unitDealNum * buyMinPrice;
		float unitSellProfit = unitDealNum * sellMinPrice;

		// 买家消耗
		costPoint += unitBuyCost;

		// 需要返还
		if (unitBuyCost > UInt32(unitSellProfit))
		{
			returnPoint += UInt32(float(unitBuyCost) - unitSellProfit);
		}

		// 挡位信息修改
		orderGrade.tradeTotalNum = orderGrade.tradeTotalNum >= unitDealNum ? orderGrade.tradeTotalNum - unitDealNum : 0;
		orderGrade.CostAccTradeNum(ptr->accId, unitDealNum);

		// 计算手续费
		UInt32 commissionCfg = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_COMMISSIOM);
		UInt32 commission = (UInt32)ceil(unitSellProfit * commissionCfg / GOLD_CONSIGNMENT_RATE);
		UInt32 sellIncome = (UInt32)unitSellProfit - commission;

		InfoLogStream << "sell gold deal serial:" << serialId << " unitDealNum:" << unitDealNum << " buyMinPrice:" << buyMinPrice 
			 << " realCost:" << costPoint - returnPoint << " returnPoint:" << returnPoint << " sellMinPrice:" << sellMinPrice << " sellIncome:" << sellIncome
			<< " order:" << ptr->LogStr()<< LogStream::eos;

		// 卖金交易记录
		GoldConsignmentTradeRecord record;
		record.guid = CLRecordClient::Instance()->GenGuid();
		record.roleId = ptr->roleId;
		record.zoneId = ptr->zoneId;
		record.accId = ptr->accId;
		record.tradeSerialId = serialId;
		record.orderId = ptr->orderId;
		record.orderType = GCOT_SELL;
		record.unitPrice = ptr->unitPrice;
		record.tradeNum = unitDealNum;
		record.param = sellIncome;
		record.tradeTime = CURRENT_TIME.MSec();
		_AddTradeRecord(record);
		_DBInsertTradeRecord(record);

		// 通知客户端，有成交
		//_NotifyOrderDeal(ptr->zoneId, ptr->roleId);

		// 成功的订单进行修改，发放卖金收益	
		{
			std::string title = SysNotifyMgr::Instance()->MakeString(10079);
			std::string sender = SysNotifyMgr::Instance()->MakeString(10076);
			std::string content = SysNotifyMgr::Instance()->MakeString(10080, unitDealNum * GetTradeBase(), GetGoldBase(), ptr->unitPrice,
				(UInt32)unitSellProfit, commission, sellIncome);
			std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_SELL_DEAL, ptr->orderId);
			ItemRewardVec rewards;
			UInt32 isHandle = 1;
			UInt64 profitGuid = GenProfitGuid();

			if (!SysUtility::SendMail(ptr->zoneId, ptr->roleId, sender, title, content, reason, rewards, profitGuid))
			{
				// 要发的信用点券 也存下来
				isHandle = 0;
				ErrorLogStream << "sell grade mail failed! serialId:" << serialId << ptr->LogStr() << LogStream::eos;
			}
			else
			{
				// 卖金获得信用点券
				_SendCreditPointIncome(ptr->zoneId, ptr->accId, ptr->roleId, ptr->orderId, sellIncome, reason);
			}

			_AddUntreatProfit(profitGuid, ptr, rewards, content, serialId, GCPT_SELL_GOLD, isHandle);
			_AddUntreatCreditPoint(ptr, sellIncome, serialId, isHandle);
		}

		// 删除订单
		if (isDel)
		{
			_RemoveOrder(ptr->orderType, ptr->orderId);
		}

		// 订单匹配完成
		if (tradeNum == 0)
			return 0;
	}

	return tradeNum;
}

OrderGradeData* GoldConsignmentService::_FindOrderGradeData(UInt32 unitPrice, UInt32 orderType)
{
	if (orderType == GCOT_BUY)
	{
		auto iter = m_BuyGradeMap.find(unitPrice);
		return iter != m_BuyGradeMap.end() ? &(iter->second) : NULL;
	}
	else if (orderType == GCOT_SELL)
	{
		auto iter = m_SellGradeMap.find(unitPrice);
		return iter != m_SellGradeMap.end() ? &(iter->second) : NULL;
	}

	return NULL;
}

void GoldConsignmentService::_RemoveOrderGrade(UInt32 unitPrice, UInt32 orderType)
{
	if (orderType == GCOT_BUY)
	{
		auto iter = m_BuyGradeMap.find(unitPrice);
		if (iter != m_BuyGradeMap.end())
		{
			m_BuyGradeMap.erase(iter);
		}
	}
	else if (orderType == GCOT_SELL)
	{
		auto iter = m_SellGradeMap.find(unitPrice);
		if (iter != m_SellGradeMap.end())
		{
			m_SellGradeMap.erase(iter);
		}
	}
}

void GoldConsignmentService::_GradeCancelOrder(GoldConsignmentOrderPtr orderPtr)
{
	// 订单取消、更新对应挡位
	OrderGradeData* gradeData = _FindOrderGradeData(orderPtr->unitPrice, orderPtr->orderType);
	if (NULL == gradeData)
	{
		//ErrorLogStream << "not find order grade:" << orderPtr->LogStr() << LogStream::eos;
		return;
	}

	// 更新单价挡位的总量
	if (gradeData->tradeTotalNum > orderPtr->tradeNum)
		gradeData->tradeTotalNum -= orderPtr->tradeNum;
	else
		gradeData->tradeTotalNum = 0;

	// 更新挡位该账号的数量
	gradeData->CostAccTradeNum(orderPtr->accId, orderPtr->tradeNum);

	// 更新挡位队列
	OrderSubmitUnitList& orderQueue = gradeData->GetOrderQueue();
	auto iter = std::find(orderQueue.begin(), orderQueue.end(), OrderSubmitUnit(orderPtr->sumbitTime, orderPtr->orderId));
	if (iter != orderQueue.end())
	{
		orderQueue.erase(iter);
	}

	// 删除挡位
	if (gradeData->tradeTotalNum == 0)
	{
		_RemoveOrderGrade(orderPtr->unitPrice, orderPtr->orderType);
	}
}

GoldConsignmentOrderPtr GoldConsignmentService::_CreateOrder(USPlayer* player, UInt32 orderType, UInt32 unitPrice,
	UInt32 tradeNum, UInt32 totalPoint)
{
	GoldConsignmentOrderPtr orderPtr = std::make_shared<GoldConsignmentOrder>();
	orderPtr->orderId = CLRecordClient::Instance()->GenGuid();
	orderPtr->orderType = orderType;
	orderPtr->zoneId = player->GetZoneID();
	orderPtr->accId = player->GetAccID();
	orderPtr->roleId = player->GetID();
	orderPtr->unitPrice = unitPrice;
	orderPtr->tradeNum = tradeNum;
	orderPtr->sumbitTime = CURRENT_TIME.MSec();
	orderPtr->expireTime = orderPtr->sumbitTime + GetHangListTime();
	orderPtr->totalPoint = totalPoint;
	return orderPtr;
}

void GoldConsignmentService::_SumbitOrder(GoldConsignmentOrderPtr orderPtr)
{
	_AddOrder(orderPtr);
	_DBInsertOrder(orderPtr);
}

UInt32 GoldConsignmentService::_CheckMatchUnitPriceBuyOrder(UInt32 accId, UInt32 unitPrice, UInt32 tradeNum)
{
	if (m_BuyGradeMap.empty())
	{
		return ErrorCode::GOLD_CONSIGNMENT_NOT_ENOUGH_GOODS;
	}

	UInt64 num = 0;
	for (auto& iter : m_BuyGradeMap)
	{
		if (iter.first >= unitPrice)
		{
			num += iter.second.GetAllowTradeNum(accId);
		}

		if (num >= (UInt64)tradeNum)
			return ErrorCode::SUCCESS;
	}

	return ErrorCode::GOLD_CONSIGNMENT_NOT_ENOUGH_GOODS;
}

UInt32 GoldConsignmentService::_CheckMatchUnitPriceSellOrder(UInt32 accId, UInt32 unitPrice, UInt32 tradeNum)
{
	if (m_SellGradeMap.empty())
	{
		return ErrorCode::GOLD_CONSIGNMENT_NOT_ENOUGH_GOODS;
	}

	UInt64 num = 0;
	for (auto& iter : m_SellGradeMap)
	{
		if (iter.first <= unitPrice)
		{
			num += iter.second.GetAllowTradeNum(accId);
		}

		if (num >= (UInt64)tradeNum)
			return ErrorCode::SUCCESS;
	}

	return ErrorCode::GOLD_CONSIGNMENT_NOT_ENOUGH_GOODS;
}

GoldConsignmentOrderPtr GoldConsignmentService::_FindOrder(UInt32 orderType, UInt64 orderId)
{
	switch (orderType)
	{
	case GCOT_BUY:
	{
		auto iter = m_BuyOrderMap.find(orderId);
		return iter != m_BuyOrderMap.end() ? iter->second : NULL;
	}
	case GCOT_SELL:
	{
		auto iter = m_SellOrderMap.find(orderId);
		return iter != m_SellOrderMap.end() ? iter->second : NULL;
	}
	default:break;
	}
	return NULL;
}

void GoldConsignmentService::_RemoveOrder(UInt32 orderType, UInt64 orderId)
{
	UInt32 zoneId = 0;
	UInt32 accId = 0;
	switch (orderType)
	{
	case GCOT_BUY:
	{
		auto iter = m_BuyOrderMap.find(orderId);
		if (iter != m_BuyOrderMap.end())
		{
			zoneId = iter->second->zoneId;
			accId = iter->second->accId;
			m_BuyOrderMap.erase(iter);
		}
		break;
	}
	case GCOT_SELL:
	{
		auto iter = m_SellOrderMap.find(orderId);
		if (iter != m_SellOrderMap.end())
		{
			zoneId = iter->second->zoneId;
			accId = iter->second->accId;
			m_SellOrderMap.erase(iter);
		}
		break;
	}
	default:return;
	}

	GCAccOrderKey key(accId, zoneId, orderType);
	auto accOrder = m_AccOrderMap.find(key);
	if (accOrder != m_AccOrderMap.end())
	{
		GoldConsignmentOrderList::iterator tt = accOrder->second.begin();
		while (tt != accOrder->second.end())
		{
			if ((*tt)->orderId == orderId)
			{
				accOrder->second.erase(tt);
				break;
			}

			++tt;
		}
	}
}

UInt32 GoldConsignmentService::_FindAccOrderNum(UInt32 accId, UInt32 zoneId, UInt32 orderType)
{
	auto iter = m_AccOrderMap.find({ accId, zoneId, orderType });
	if (iter != m_AccOrderMap.end())
	{
		return iter->second.size();
	}

	return 0;
}

void GoldConsignmentService::_SetLastAveragePrice(UInt32 price)
{
	m_LastAveragePrice = price;
}

void GoldConsignmentService::_SetCurAveragePrice(UInt32 price)
{
	m_CurAveragePrice = price;
}

void GoldConsignmentService::_SetRecentAveragePrice(UInt32 price)
{
	m_RecentAveragePrice = price;
}

UInt32 GoldConsignmentService::_EstimateAccCreditPointProfit(UInt32 zoneId, UInt32 accId)
{
	GCAccOrderKey key(accId, zoneId, GCOT_SELL);
	auto iter = m_AccOrderMap.find(key);
	if (iter == m_AccOrderMap.end())
	{
		return 0;
	}

	// 计算手续费
	UInt32 commissionCfg = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_COMMISSIOM);
	UInt32 estimateProfit = 0;
	for (auto& it : iter->second)
	{
		float minUnitPrice = GetMinPrice(it->unitPrice);

		UInt32 commission = (UInt32)ceil(it->tradeNum * minUnitPrice * commissionCfg / GOLD_CONSIGNMENT_RATE);
		estimateProfit += (it->tradeNum * (UInt32)minUnitPrice - commission);
	}

	return estimateProfit;
}

void GoldConsignmentService::_NotifyOrderDeal(UInt32 zoneId, UInt64 roleId)
{
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneId);
	if (NULL == zone)
	{
		return;
	}

	UInt32 timeOut = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_TRADE_RECORD_SHOW_TIME);
	UInt32 showTime = timeOut * 3600;

	CLProtocol::SceneGoldConsignmentDealNotify dealNotify;
	dealNotify.roleId = roleId;
	dealNotify.redPointTime = (UInt32)CURRENT_TIME.Sec() + showTime;
	zone->TransmitToScene(dealNotify);
}

bool GoldConsignmentService::_RoleIsHasOrder(USPlayer* player, UInt32 orderType, UInt64 orderId)
{
	auto iter = m_AccOrderMap.find({ player->GetAccID(), player->GetZoneID(), orderType });
	if (iter == m_AccOrderMap.end())
	{
		return false;
	}

	for (auto& it : iter->second)
	{
		if (it->roleId == player->GetID() && it->orderId == orderId)
		{
			return true;
		}
	}

	return false;
}

void GoldConsignmentService::_AddDealInfo(UInt32 costPoint, UInt32 dealNum)
{
	m_TodayCostPoint = m_TodayCostPoint + costPoint;
	m_TodayDealNum = m_TodayDealNum + dealNum;
}

bool GoldConsignmentService::_SendCreditPointIncome(UInt32 zoneId, UInt32 accId, UInt64 roleId, UInt64 orderId, UInt32 income, const std::string& reason)
{
	CLProtocol::SceneGetCreditPoint req;
	req.roleId = roleId;
	req.accId = accId;
	req.orderId = orderId;
	req.creditPoint = income;
	req.reason = reason;

	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneId);
	if (zone != NULL)
	{
		zone->TransmitToWorld(req);
		return true;
	}
	else
	{
		ErrorLogStream << "send credit point failed zone:" << zoneId << " accId:" << accId << " roleid:" << roleId << " income:" << income
			<< " orderId:" << orderId << " reason:" << reason << LogStream::eos;
	}

	return false;
}

void GoldConsignmentService::_SendUntreatProfit()
{
	std::set<UInt32>& zoneSet = USApplication::Instance()->GetReturnProfitZone();
	if (zoneSet.empty())
	{
		return;
	}

	std::map<UInt64, GoldConsignmentProfit>::iterator iter = m_UntreatProfitMap.begin();
	while (iter != m_UntreatProfitMap.end())
	{
		GoldConsignmentProfit& profit = iter->second;
		if (!zoneSet.count(profit.zoneId) || profit.isHandle != 0)
		{
			++iter;
			continue;
		}

		auto zone = GameZoneMgr::Instance()->FindGameZoneByID(profit.zoneId);
		if (NULL == zone)
		{
			++iter;
			continue;
		}

		std::string reason;
		std::string title;
		std::string sender;
		_CreateUntreatMail(profit.profitType, profit.orderId, profit.orderType, reason, sender, title);

		ItemRewardVec rewardVec;
		ParseProfitStr(profit.guid, rewardVec, profit.profit);
		if (!SysUtility::SendMail(profit.zoneId, profit.roleId, sender, title, profit.content, reason, rewardVec, profit.guid))
		{
			ErrorLogStream << "untreat profit failed: " << profit.LogStr() << LogStream::eos;
			++iter;
			continue;
		}

		InfoLogStream << "untreat profit success: " << profit.LogStr() << LogStream::eos;
		
		profit.isHandle = 1;
		_DBUpdateUntreatProfit(profit.guid);

		iter = m_UntreatProfitMap.erase(iter);
	}
}

void GoldConsignmentService::_AddUntreatProfit(UInt64 profitGuid, GoldConsignmentOrderPtr orderPtr, const ItemRewardVec& rewardVec, const std::string& content,
	UInt64 serialId, UInt32 profitType, UInt32 isHandle)
{
	GoldConsignmentProfit profit;
	profit.guid = profitGuid;
	profit.tradeSerial = serialId;
	profit.orderId = orderPtr->orderId;
	profit.orderType = orderPtr->orderType;
	profit.zoneId = orderPtr->zoneId;
	profit.accId = orderPtr->accId;
	profit.roleId = orderPtr->roleId;
	CreateProfitStr(rewardVec, profit.profit);
	profit.content = content;
	profit.profitType = profitType;
	profit.isHandle = isHandle;

	// 未处理的才需要放在内存，已处理的只需要存库
	if (isHandle == 0)
	{
		m_UntreatProfitMap[profit.guid] = profit;
	}

	_DBInsertUntreatProfit(profit);
}

void GoldConsignmentService::_DBInsertUntreatProfit(GoldConsignmentProfit& profit)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gold_consignment_profit", profit.guid);
	cmd->PutData("trade_serial", profit.tradeSerial);
	cmd->PutData("order_id", profit.orderId);
	cmd->PutData("order_type", profit.orderType);
	cmd->PutData("zone_id", profit.zoneId);
	cmd->PutData("acc_id", profit.accId);
	cmd->PutData("role_id", profit.roleId);
	cmd->PutData("profit", profit.profit);
	cmd->PutData("content", profit.content);
	cmd->PutData("profit_type", profit.profitType);
	cmd->PutData("is_handle", profit.isHandle);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBUpdateUntreatProfit(UInt64 guid)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gold_consignment_profit", guid, true);
	cmd->PutData("is_handle", 1);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_CreateUntreatMail(UInt32 profitType, UInt64 orderId, UInt32 orderType, std::string& reason, std::string& sender, std::string& title)
{
	sender = SysNotifyMgr::Instance()->MakeString(10076);
	if (orderType == GCOT_SELL)
		title = SysNotifyMgr::Instance()->MakeString(10083);
	else
		title = SysNotifyMgr::Instance()->MakeString(10085);

	switch (profitType)
	{
	case GCPT_ACTIVITY_END:
	{
		reason = GetReason(SOURCE_GOLD_CONSIGNMENT_ACTIVITY_END, orderId);
	}
	break;
	case GCPT_CLOSE_ZONE:
	{
		reason = GetReason(SOURCE_GOLD_CONSIGNMENT_CLOSE_ZONE, orderId);
	}
	break;
	case GCPT_CLOSE_ROLE:
	{
		reason = GetReason(SOURCE_GOLD_CONSIGNMENT_CLOSE_ROLE, orderId);
	}
	break;
	case GCPT_TIME_OUT:
	{
		reason = GetReason(SOURCE_GOLD_CONSIGNMENT_TIMEOUT_ORDER, orderId);
	}
	break;
	case GCPT_SELL_GOLD:
	{
		reason = GetReason(SOURCE_GOLD_CONSIGNMENT_SELL_DEAL, orderId);
		title = SysNotifyMgr::Instance()->MakeString(10079);
	}
	break;
	case GCPT_BUY_GOLD:
	{
		reason = GetReason(SOURCE_GOLD_CONSIGNMENT_BUY_DEAL, orderId);
		title = SysNotifyMgr::Instance()->MakeString(10081);
	}
	break;
	}
}

void GoldConsignmentService::CreateProfitStr(const ItemRewardVec& rewardVec, std::string& str)
{
	if (rewardVec.empty()) return;
	str.clear();

	for (auto& it : rewardVec)
	{
		str.append(std::to_string(it.id)).append(",").append(std::to_string(it.num)).append("|");
	}

	if (!str.empty())
		str.pop_back();
}

void GoldConsignmentService::ParseProfitStr(UInt64 guid, ItemRewardVec& rewardVec, const std::string& str)
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(str, param_1, "|");
	for (auto& it : param_1)
	{
		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(it, param_2, ",");

		if (param_2.size() != 2)
		{
			ErrorLogStream << "profit:" << guid << "rewardVec Error: " << str << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);

		rewardVec.push_back(ItemReward(id, num, 0, 0));
	}
}

void GoldConsignmentService::_SendUntreatCreditPoint()
{
	std::set<UInt32>& zoneSet = USApplication::Instance()->GetReturnProfitZone();
	if (zoneSet.empty())
	{
		return;
	}

	std::map<UInt64, GoldConsignmentCreditPoint>::iterator iter = m_UntreatCreditPointMap.begin();
	while (iter != m_UntreatCreditPointMap.end())
	{
		GoldConsignmentCreditPoint& cPoint = iter->second;
		if (!zoneSet.count(cPoint.zoneId) || cPoint.isHandle != 0)
		{
			++iter;
			continue;
		}

		auto zone = GameZoneMgr::Instance()->FindGameZoneByID(cPoint.zoneId);
		if (NULL == zone)
		{
			++iter;
			continue;
		}

		std::string reason = GetReason(SOURCE_GOLD_CONSIGNMENT_ABNORMAL_REWARD, cPoint.orderId);
		if ( !_SendCreditPointIncome(cPoint.zoneId, cPoint.accId, cPoint.roleId, cPoint.orderId, cPoint.creditPoint, reason))
		{
			++iter;
			continue;
		}

		cPoint.isHandle = 1;
		_DBUpdateUntreatCreditPoint(cPoint.guid);
		InfoLogStream << "untreat credit point success: " << cPoint.LogStr() << LogStream::eos;

		iter = m_UntreatCreditPointMap.erase(iter);
	}
}

void GoldConsignmentService::_AddUntreatCreditPoint(GoldConsignmentOrderPtr orderPtr, UInt32 creditPoint, UInt64 serialId, UInt32 isHandle)
{
	GoldConsignmentCreditPoint cPoint;
	cPoint.guid = CLRecordClient::Instance()->GenGuid();
	cPoint.tradeSerial = serialId;
	cPoint.orderId = orderPtr->orderId;
	cPoint.zoneId = orderPtr->zoneId;
	cPoint.accId = orderPtr->accId;
	cPoint.roleId = orderPtr->roleId;
	cPoint.creditPoint = creditPoint;
	cPoint.isHandle = isHandle;

	// 未处理的才需要放在内存，已处理的只需要存库
	if (isHandle == 0)
	{
		m_UntreatCreditPointMap[cPoint.guid] = cPoint;
	}

	_DBInsertUntreatCreditPoint(cPoint);
}

void GoldConsignmentService::_DBInsertUntreatCreditPoint(GoldConsignmentCreditPoint& creditPoint)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gold_consignment_credit_point", creditPoint.guid);
	cmd->PutData("trade_serial", creditPoint.tradeSerial);
	cmd->PutData("order_id", creditPoint.orderId);
	cmd->PutData("zone_id", creditPoint.zoneId);
	cmd->PutData("acc_id", creditPoint.accId);
	cmd->PutData("role_id", creditPoint.roleId);
	cmd->PutData("credit_point", creditPoint.creditPoint);
	cmd->PutData("is_handle", creditPoint.isHandle);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GoldConsignmentService::_DBUpdateUntreatCreditPoint(UInt64 guid)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gold_consignment_credit_point", guid, true);
	cmd->PutData("is_handle", 1);
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool GoldConsignmentService::_IsHasAbnormalOrder(UInt32 zoneId, UInt32 accId, UInt64 roleId)
{
	UInt32 minUnit = UINT32_MAX;
	UInt32 maxUnit = 0;
	auto buyIt = m_AccOrderMap.find({accId, zoneId, GCOT_BUY });
	if (buyIt != m_AccOrderMap.end())
	{
		for (auto& it : buyIt->second)
		{
			if (it->roleId != roleId)
				continue;

			if (it->unitPrice < minUnit)
				minUnit = it->unitPrice;

			if (it->unitPrice > maxUnit)
				maxUnit = it->unitPrice;
		}
	}

	auto sellIt = m_AccOrderMap.find({ accId, zoneId, GCOT_SELL });
	if (sellIt != m_AccOrderMap.end())
	{
		for (auto& it : sellIt->second)
		{
			if (it->roleId != roleId)
				continue;

			if (it->unitPrice < minUnit)
				minUnit = it->unitPrice;

			if (it->unitPrice > maxUnit)
				maxUnit = it->unitPrice;
		}
	}

	if (minUnit != UINT32_MAX && _CheckWavePrice(minUnit))
	{
		return true;
	}

	if (maxUnit != 0 && _CheckWavePrice(maxUnit))
	{
		return true;
	}

	return false;
}

void GoldConsignmentService::_RemoveAbnormalUnitPrice()
{
	SellOrderGradeMap::iterator sellIter = m_SellGradeMap.begin();
	while(sellIter != m_SellGradeMap.end())
	{
		if (_CheckWavePrice(sellIter->first))
		{
			InfoLogStream << "remove sell abnormal unit price grade:" << sellIter->first<< LogStream::eos;
			sellIter = m_SellGradeMap.erase(sellIter);
		}
		else
			++sellIter;
	}

	BuyOrderGradeMap::iterator buyIter = m_BuyGradeMap.begin();
	while (buyIter != m_BuyGradeMap.end())
	{
		if (_CheckWavePrice(buyIter->first))
		{
			InfoLogStream << "remove buy abnormal unit price grade:" << buyIter->first << LogStream::eos;
			buyIter = m_BuyGradeMap.erase(buyIter);
		}
		else
			++buyIter;
	}
}