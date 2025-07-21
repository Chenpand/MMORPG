#ifndef _CL_GOLD_CONSIGNMENT_PROTOCOL_H_
#define _CL_GOLD_CONSIGNMENT_PROTOCOL_H_

#include <AvalonPacket.h>
#include <CLProtocolDefine.h>
#include <CLGoldConsignmentDefine.h>

namespace CLProtocol
{
	/**
	 *@brief 请求订单页面
	 */
	class UnionGoldConsignmentOrderPageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_ORDER_PAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION(){}
	};

	class UnionGoldConsignmentOrderPageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_ORDER_PAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & startCloseTime & endCloseTime & startCloseStamp & lastAveragePrice & curAveragePrice & recentAveragePrice
				& sellAccNum & buyAccNum & estimateIncome & isHasUnitAbnormalOrder & sellOrderList & buyOrderList;
		}

		// 开始休市时间
		UInt32 startCloseTime;
		// 结束休市时间
		UInt32 endCloseTime;
		// 开始休市时间戳
		UInt32 startCloseStamp;
		// 上一个交易日均价
		UInt32 lastAveragePrice;
		// 当前平均成交价
		UInt32 curAveragePrice;
		// 最近平均成交价
		UInt32 recentAveragePrice;
		// 卖金账号挂单数量
		UInt32 sellAccNum;
		// 买金账号挂单数量
		UInt32 buyAccNum;
		// 账号信用点券预计收入
		UInt32 estimateIncome;
		// 是否有单价异常的订单(不符合当日单价范围的)
		UInt32 isHasUnitAbnormalOrder;
		// 卖金订单
		std::vector<OrderGradeInfo> sellOrderList;
		// 买金订单
		std::vector<OrderGradeInfo> buyOrderList;
	};

	/**
	 *@brief 上架订单
	 */
	class SceneGoldConsignmentSubmitOrderReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GOLD_CONSIGNMENT_SUBMIT_ORDER_REQ)
	public:
		SceneGoldConsignmentSubmitOrderReq()
		{
			param = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & submitId & tradeType & orderType & unitPrice & tradeNum & param;
		}

		// 提交序号
		UInt32 submitId;
		// 交易类型（GoldConsignmentTradeType）
		UInt32 tradeType;
		// 订单类型（GoldConsignmentOrderType）
		UInt32 orderType;
		// 单价
		UInt32 unitPrice;
		// 交易数量
		UInt32 tradeNum;
		// 参数
		UInt32 param;
	};

	class SceneGoldConsignmentSubmitOrderRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GOLD_CONSIGNMENT_SUBMIT_ORDER_RES)
	public:
		SceneGoldConsignmentSubmitOrderRes() : retCode(0), orderType(0), dealNum(0), hangListNum(0),
			unitPrice(0), param1(0), param2(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & submitId & retCode & orderType & dealNum & hangListNum & unitPrice & param1 & param2;
		}
		
		// 提交序号
		UInt32 submitId;
		// 返回值
		UInt32 retCode;
		// 订单类型
		UInt32 orderType;

		// 成交数量
		UInt32 dealNum;
		// 挂单数量
		UInt32 hangListNum;
		// 单价
		UInt32 unitPrice;
		// 参数1（支出或收益）
		UInt32 param1;
		// 参数2（退还或手续费）
		UInt32 param2;
	};

	/**
	 *@brief 请求成交记录
	 */
	class SceneGoldConsignmentDealNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GOLD_CONSIGNMENT_DEAL_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & redPointTime;
		}
		UInt64 roleId;
		UInt32 redPointTime;
	};

	/**
	 *@brief 请求成交记录
	 */
	class UnionGoldConsignmentDealRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_DEAL_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	class UnionGoldConsignmentDealRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_DEAL_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sellRecord & buyRecord;
		}

		// 卖金记录
		std::vector<GoldConsignmentTradeRecord> sellRecord;
		// 买金记录
		std::vector<GoldConsignmentTradeRecord> buyRecord;
	};

	/**
	 *@brief 请求我的订单
	 */
	class UnionGoldConsignmentOwnerOrderReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_OWNER_ORDER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	class UnionGoldConsignmentOwnerOrderRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_OWNER_ORDER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sellAccNum & buyAccNum & sellOrderList & buyOrderList;
		}

		// 卖金账号挂单数量
		UInt32 sellAccNum;
		// 买金账号挂单数量
		UInt32 buyAccNum;
		// 卖金订单
		std::vector<GoldConsignmentOrder> sellOrderList;
		// 买金订单
		std::vector<GoldConsignmentOrder> buyOrderList;
	};

	/**
	 *@brief 下架订单
	 */
	class UnionGoldConsignmentCancelOrderReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_CANCEL_ORDER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderType & orderId;
		}
	
		// 订单类型
		UInt32 orderType;
		// 订单Id
		UInt64 orderId;
	};

	class UnionGoldConsignmentCancelOrderRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_CANCEL_ORDER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & orderId;
		}

		// 返回码
		UInt32 retCode;
		// 订单Id
		UInt64 orderId;
	};

	class UnionGoldConsignmentActivityClose : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_ACTIVITY_CLOSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & endTime;
		}

		UInt32 zoneId;
		UInt32 endTime;
	};
	
}

#endif // !_CL_GOLD_CONSIGNMENT_PROTOCOL_H_

