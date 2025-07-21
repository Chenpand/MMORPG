#ifndef _CL_GOLD_CONSIGNMENT_PROTOCOL_H_
#define _CL_GOLD_CONSIGNMENT_PROTOCOL_H_

#include <AvalonPacket.h>
#include <CLProtocolDefine.h>
#include <CLGoldConsignmentDefine.h>

namespace CLProtocol
{
	/**
	 *@brief ���󶩵�ҳ��
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

		// ��ʼ����ʱ��
		UInt32 startCloseTime;
		// ��������ʱ��
		UInt32 endCloseTime;
		// ��ʼ����ʱ���
		UInt32 startCloseStamp;
		// ��һ�������վ���
		UInt32 lastAveragePrice;
		// ��ǰƽ���ɽ���
		UInt32 curAveragePrice;
		// ���ƽ���ɽ���
		UInt32 recentAveragePrice;
		// �����˺Źҵ�����
		UInt32 sellAccNum;
		// ����˺Źҵ�����
		UInt32 buyAccNum;
		// �˺����õ�ȯԤ������
		UInt32 estimateIncome;
		// �Ƿ��е����쳣�Ķ���(�����ϵ��յ��۷�Χ��)
		UInt32 isHasUnitAbnormalOrder;
		// ���𶩵�
		std::vector<OrderGradeInfo> sellOrderList;
		// ��𶩵�
		std::vector<OrderGradeInfo> buyOrderList;
	};

	/**
	 *@brief �ϼܶ���
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

		// �ύ���
		UInt32 submitId;
		// �������ͣ�GoldConsignmentTradeType��
		UInt32 tradeType;
		// �������ͣ�GoldConsignmentOrderType��
		UInt32 orderType;
		// ����
		UInt32 unitPrice;
		// ��������
		UInt32 tradeNum;
		// ����
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
		
		// �ύ���
		UInt32 submitId;
		// ����ֵ
		UInt32 retCode;
		// ��������
		UInt32 orderType;

		// �ɽ�����
		UInt32 dealNum;
		// �ҵ�����
		UInt32 hangListNum;
		// ����
		UInt32 unitPrice;
		// ����1��֧�������棩
		UInt32 param1;
		// ����2���˻��������ѣ�
		UInt32 param2;
	};

	/**
	 *@brief ����ɽ���¼
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
	 *@brief ����ɽ���¼
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

		// �����¼
		std::vector<GoldConsignmentTradeRecord> sellRecord;
		// ����¼
		std::vector<GoldConsignmentTradeRecord> buyRecord;
	};

	/**
	 *@brief �����ҵĶ���
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

		// �����˺Źҵ�����
		UInt32 sellAccNum;
		// ����˺Źҵ�����
		UInt32 buyAccNum;
		// ���𶩵�
		std::vector<GoldConsignmentOrder> sellOrderList;
		// ��𶩵�
		std::vector<GoldConsignmentOrder> buyOrderList;
	};

	/**
	 *@brief �¼ܶ���
	 */
	class UnionGoldConsignmentCancelOrderReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_GOLD_CONSIGNMENT_CANCEL_ORDER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderType & orderId;
		}
	
		// ��������
		UInt32 orderType;
		// ����Id
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

		// ������
		UInt32 retCode;
		// ����Id
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

