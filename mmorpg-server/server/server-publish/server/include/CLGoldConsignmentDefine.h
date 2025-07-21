#ifndef _GOLD_CONSIGNMENT_DEFINE_H_
#define _GOLD_CONSIGNMENT_DEFINE_H_

#include <set>
#include <CLDefine.h>

// ����
#define GOLD_CONSIGNMENT_RATE	1000


// ��������
enum GoldConsignmentOrderType
{
	// ���𶩵�
	GCOT_SELL = 1,
	// ��𶩵�
	GCOT_BUY = 2,
};

// ��������
enum GoldConsignmentTradeType
{
	// һ����
	GCTT_ONCE = 1,
	// �ҵ�
	GCOT_HANG_LIST = 2,
};

// ��������
enum GoldConsignmentProfitType
{
	// ���������
	GCPT_ACTIVITY_END = 1,
	// �ر�����
	GCPT_CLOSE_ZONE = 2,
	// �رս�ɫ����
	GCPT_CLOSE_ROLE = 3,
	// ������ʱ
	GCPT_TIME_OUT = 4,
	// ����
	GCPT_SELL_GOLD= 5,
	// ���
	GCPT_BUY_GOLD = 6,
	// �ֶ��ر�
	GCPT_ADMIN_CLOSE = 7,
};

// ������λ��Ϣ
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

// ����
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

// ������λ��Ϣ
struct OrderGradeData
{
	OrderGradeData() : tradeTotalNum(0) {}
	OrderGradeData(UInt64 _ttn) : tradeTotalNum(_ttn) {}

	void AddOrder(UInt64 sumbitTime, UInt64 orderId, UInt32 accId, UInt64 tradeNum)
	{
		tradeTotalNum += tradeNum;

		// ���ϼ�ʱ������
		OrderSubmitUnit st(sumbitTime, orderId);
		orderList.emplace_back(st);
		orderList.sort();

		// �����˺��ϼ�����
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

	// ��λ�ϼ�����
	UInt64 tradeTotalNum;

private:

	// �õ�λÿ���˺��ж��ٶ�����(�˴����������˺����ĸ������ģ������ݽṹ���������ͬ�˺Ų��ܽ���)
	//std::map<UInt32, UInt64>  accTradeNumMap;
	// �����б�
	OrderSubmitUnitList		orderList;
};

// ����
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

	// ����id
	UInt64 orderId;
	// �������ͣ����������
	UInt32 orderType;
	// ��id
	UInt32 zoneId;
	// �˺�id
	UInt32 accId;
	// ��ɫid
	UInt64 roleId;
	// ����
	UInt32 unitPrice;
	// ��������
	UInt32 tradeNum;
	// �ύʱ��
	UInt64 sumbitTime;
	// ����ʱ��
	UInt64 expireTime;
	// �ܵ�ȯ(���)
	UInt32 totalPoint;
	// ���ĵ�ȯ(���)
	UInt32 costPoint;
	// �˻���ȯ(���)
	UInt32 returnPoint;
};

// �ɽ���¼
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

	// ������ˮ��
	UInt64 tradeSerialId;
	// ��������
	UInt64 orderId;
	// �������ͣ����������
	UInt32 orderType;
	// ����
	UInt32 unitPrice;
	// ������
	UInt32 tradeNum;
	// param(֧��������)
	UInt32 param;
	// ����ʱ��
	UInt64 tradeTime;
};


// �˺Ŷ���key
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

	// �˺�id
	UInt32 accId;
	// ��id
	UInt32 zoneId;
	// ��������
	UInt32 orderType;
};

// ����
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
	// �������׺�
	UInt64 tradeSerial;
	// ��������
	UInt64 orderId;
	// ��������
	UInt32 orderType;

	UInt32 zoneId;
	UInt32 accId;
	UInt64 roleId;

	std::string profit;
	std::string content;
	
	// ��������
	UInt32 profitType;
	// �Ƿ��Ѿ�����
	UInt32 isHandle;
};

// ���õ�ȯ����
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
	// �������׺�
	UInt64 tradeSerial;
	// ��������
	UInt64 orderId;

	UInt32 zoneId;
	UInt32 accId;
	UInt64 roleId;

	// ���õ�ȯ
	UInt32 creditPoint;
	// �Ƿ��Ѿ�����
	UInt32 isHandle;

};

#endif // !_GOLD_CONSIGNMENT_DEFINE_H_
