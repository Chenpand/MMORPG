#ifndef _GOLD_CONSIGNMENT_DEFINE_H_
#define _GOLD_CONSIGNMENT_DEFINE_H_

#include <set>
#include <CLDefine.h>

// 比例
#define GOLD_CONSIGNMENT_RATE	1000


// 订单类型
enum GoldConsignmentOrderType
{
	// 卖金订单
	GCOT_SELL = 1,
	// 买金订单
	GCOT_BUY = 2,
};

// 交易类型
enum GoldConsignmentTradeType
{
	// 一次性
	GCTT_ONCE = 1,
	// 挂单
	GCOT_HANG_LIST = 2,
};

// 收益类型
enum GoldConsignmentProfitType
{
	// 活动结束返还
	GCPT_ACTIVITY_END = 1,
	// 关闭区服
	GCPT_CLOSE_ZONE = 2,
	// 关闭角色订单
	GCPT_CLOSE_ROLE = 3,
	// 订单超时
	GCPT_TIME_OUT = 4,
	// 卖金
	GCPT_SELL_GOLD= 5,
	// 买金
	GCPT_BUY_GOLD = 6,
	// 手动关闭
	GCPT_ADMIN_CLOSE = 7,
};

// 订单挡位信息
struct OrderGradeInfo
{
	OrderGradeInfo() : unitPrice(0), tradeNum(0) {}
	OrderGradeInfo(UInt32 _up, UInt64 _tn) : unitPrice(_up), tradeNum(_tn) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & unitPrice & tradeNum;
	}

	UInt32 unitPrice;
	UInt64 tradeNum;
};

// 订单
struct OrderSubmitUnit
{
	OrderSubmitUnit() : sumbitTime(0), orderId(0) {}
	OrderSubmitUnit(UInt64 _st, UInt64 _oi) : sumbitTime(_st), orderId(_oi) {}

	bool operator < (const OrderSubmitUnit& oq) const
	{
		if (sumbitTime != oq.sumbitTime)
		{
			return sumbitTime < oq.sumbitTime;
		}

		return orderId < oq.orderId;
	}

	bool operator== (const OrderSubmitUnit& oq) const
	{
		return sumbitTime == oq.sumbitTime && orderId == oq.orderId;
	}

	UInt64 sumbitTime;
	UInt64 orderId;
};

typedef std::list<OrderSubmitUnit> OrderSubmitUnitList;

// 订单挡位信息
struct OrderGradeData
{
	OrderGradeData() : tradeTotalNum(0) {}
	OrderGradeData(UInt64 _ttn) : tradeTotalNum(_ttn) {}

	void AddOrder(UInt64 sumbitTime, UInt64 orderId, UInt32 accId, UInt64 tradeNum)
	{
		tradeTotalNum += tradeNum;

		// 按上架时间排序
		OrderSubmitUnit st(sumbitTime, orderId);
		orderList.emplace_back(st);
		orderList.sort();

		// 增加账号上架数量
		//accTradeNumMap[accId] += tradeNum;
	}

	OrderSubmitUnitList& GetOrderQueue() { return orderList; }

// 	UInt64 GetAccTradeNum(UInt32 accId)
// 	{
// 		auto iter = accTradeNumMap.find(accId);
// 		return iter != accTradeNumMap.end() ? iter->second : 0;
// 	}

	UInt64 GetAllowTradeNum(UInt32 accId)
	{
		return tradeTotalNum;
// 		UInt64 accTradeNum = GetAccTradeNum(accId);
// 		if (tradeTotalNum > accTradeNum)
// 			return tradeTotalNum - accTradeNum;
// 		else
// 		{
// 			ErrorLogStream << "trade num error:" << accId << LogStream::eos;
// 		}
// 
// 		return 0;
	}

	void CostAccTradeNum(UInt32 accId, UInt64 num)
	{
// 		auto iter = accTradeNumMap.find(accId);
// 		if (iter != accTradeNumMap.end())
// 		{
// 			iter->second -= num;
// 		}
	}

	// 挡位上架总量
	UInt64 tradeTotalNum;

private:

	// 该挡位每个账号有多少订单量(此处不区分是账号是哪个区服的，该数据结构是用来检查同账号不能交易)
	//std::map<UInt32, UInt64>  accTradeNumMap;
	// 订单列表
	OrderSubmitUnitList		orderList;
};

// 订单
struct GoldConsignmentOrder
{
	GoldConsignmentOrder() : orderId(0), orderType(0), zoneId(0), accId(0), roleId(0), unitPrice(0), 
		tradeNum(0), sumbitTime(0), expireTime(0), totalPoint(0), costPoint(0), returnPoint(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & orderId & unitPrice & tradeNum & sumbitTime & expireTime;
	}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " orderId:" << orderId;
		ss << " orderType:" << orderType;
		ss << " zoneId:" << zoneId;
		ss << " accId:" << accId;
		ss << " roleId:" << roleId;
		ss << " unitPrice:" << unitPrice;
		ss << " tradeNum:" << tradeNum;
		ss << " sumbitTime:" << sumbitTime;
		ss << " totalPoint:" << totalPoint;
		ss << " costPoint:" << costPoint;
		ss << " returnPoint:" << returnPoint;

		return ss.str();
	}

	UInt32 remainPoint() 
	{ 
		if (totalPoint > (costPoint + returnPoint))
		{
			return totalPoint - (costPoint + returnPoint);
		}

		return 0; 
	}

	// 订单id
	UInt64 orderId;
	// 订单类型（买金还是卖金）
	UInt32 orderType;
	// 区id
	UInt32 zoneId;
	// 账号id
	UInt32 accId;
	// 角色id
	UInt64 roleId;
	// 单价
	UInt32 unitPrice;
	// 交易数量
	UInt32 tradeNum;
	// 提交时间
	UInt64 sumbitTime;
	// 过期时间
	UInt64 expireTime;
	// 总点券(买金)
	UInt32 totalPoint;
	// 消耗点券(买金)
	UInt32 costPoint;
	// 退还点券(买金)
	UInt32 returnPoint;
};

// 成交记录
struct GoldConsignmentTradeRecord
{
	GoldConsignmentTradeRecord() : guid(0), roleId(0), zoneId(0), accId(0), tradeSerialId(0), orderId(0), orderType(0), unitPrice(0),
		tradeNum(0), param(0), tradeTime(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & orderType & unitPrice & tradeNum & param & tradeTime;
	}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << " roleId:" << roleId;
		ss << " zoneId:" << zoneId;
		ss << " accId:" << accId;
		ss << " tradeSerialId:" << tradeSerialId;
		ss << " orderId:" << orderId;
		ss << " orderType:" << orderType;
		ss << " unitPrice:" << unitPrice;
		ss << " tradeNum:" << tradeNum;
		ss << " param:" << param;
		ss << " tradeTime:" << tradeTime;
		return ss.str();
	}

	UInt64 guid;
	UInt64 roleId;
	UInt32 zoneId;
	UInt32 accId;

	// 交易流水号
	UInt64 tradeSerialId;
	// 关联订单
	UInt64 orderId;
	// 订单类型（买金还是卖金）
	UInt32 orderType;
	// 单价
	UInt32 unitPrice;
	// 交易量
	UInt32 tradeNum;
	// param(支出或收益)
	UInt32 param;
	// 交易时间
	UInt64 tradeTime;
};


// 账号订单key
struct GCAccOrderKey
{
	GCAccOrderKey() : accId(0), zoneId(0), orderType(0) {}
	GCAccOrderKey(UInt32 _accId, UInt32 _zoneId, UInt32 _orderType) 
		: accId(_accId), zoneId(_zoneId), orderType(_orderType) {}

	bool operator < (const GCAccOrderKey& aok) const
	{
		if (accId != aok.accId)
			return accId < aok.accId;

		if (zoneId != aok.zoneId)
			return zoneId < aok.zoneId;

		return orderType < aok.orderType;
	}

	// 账号id
	UInt32 accId;
	// 区id
	UInt32 zoneId;
	// 订单类型
	UInt32 orderType;
};

// 收益
struct GoldConsignmentProfit
{
	GoldConsignmentProfit() : guid(0), tradeSerial(0), orderId(0), orderType(0), zoneId(0), accId(0), roleId(0), profitType(0), isHandle(0) {}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << " tradeSerial:" << tradeSerial;
		ss << " orderId:" << orderId;
		ss << " orderType:" << orderType;
		ss << " zoneId:" << zoneId;
		ss << " accId:" << accId;
		ss << " roleId:" << roleId;
		ss << " profit:" << profit;
		//ss << " content:" << content;
		ss << " profitType:" << profitType;
		ss << " isHandle:" << isHandle;
		return ss.str();
	}

	// guid
	UInt64 guid;
	// 关联交易号
	UInt64 tradeSerial;
	// 关联订单
	UInt64 orderId;
	// 订单类型
	UInt32 orderType;

	UInt32 zoneId;
	UInt32 accId;
	UInt64 roleId;

	std::string profit;
	std::string content;
	
	// 收益类型
	UInt32 profitType;
	// 是否已经处理
	UInt32 isHandle;
};

// 信用点券收益
struct GoldConsignmentCreditPoint
{
	GoldConsignmentCreditPoint() : guid(0), tradeSerial(0), orderId(0), zoneId(0), accId(0), roleId(0), creditPoint(0), isHandle(0) {}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << " tradeSerial:" << tradeSerial;
		ss << " orderId:" << orderId;
		ss << " zoneId:" << zoneId;
		ss << " accId:" << accId;
		ss << " roleId:" << roleId;
		ss << " creditPoint:" << creditPoint;
		ss << " isHandle:" << isHandle;
		return ss.str();
	}

	// guid
	UInt64 guid;
	// 关联交易号
	UInt64 tradeSerial;
	// 关联订单
	UInt64 orderId;

	UInt32 zoneId;
	UInt32 accId;
	UInt64 roleId;

	// 信用点券
	UInt32 creditPoint;
	// 是否已经处理
	UInt32 isHandle;

};

#endif // !_GOLD_CONSIGNMENT_DEFINE_H_
