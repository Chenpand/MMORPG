#ifndef __CHARGE_ORDER_CONTEXT_H__
#define __CHARGE_ORDER_CONTEXT_H__

#include <sstream>
#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLSceneObject.h>
#include <CLItemDefine.h>

// ����״̬
enum ChargeOrderStatus
{
	// ��ʼ״̬
	ORDER_STATUS_INIT,
	// δ����
	ORDER_STATUS_NOT_CREATE = 0,
	// ��֤��
	ORDER_STATUS_VERIFYING = 1,
	// ������
	ORDER_STATUS_SENDING_ITEM = 2,
	// �����쳣
	ORDER_STATUS_ERROR = 3,
	// �������
	ORDER_STATUS_FINISH = 4,
};

class ChargeMallDataEntry;
// ��ֵ����������
struct ChargeOrderContext : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(ChargeOrderContext, Avalon::NullMutex)

	ChargeOrderContext();

	/**
	*@brief ���붩�������ݿ�
	*/
	void InsertToDB();

	/**
	*@brief ���붩�������ݿ�
	*/
	void SetDBData(CLRecordCallback* callback);

	/**
	*@brief ���µ����ݿ�
	*/
	void SaveToDB();

	/**
	*@brief ToString
	*/
	std::string ToString() const;

	// ����״̬
	ChargeOrderStatus	status;
	// ����ID
	UInt32				connid;
	// ����
	std::string			channel;
	// ������
	std::string			orderId;
	// �˺�ID
	UInt32				accid;
	// ��ɫID
	ObjID_t				roleId;
	// �̳�����
	ObjUInt8			mallType;
	// ��ֵ��ƷID
	UInt32				chargeGoodsId;
	// ��ֵ����ID
	UInt32				chargeItemId;
	// ��������
	UInt32				chargeItemNum;
	// VIP����
	UInt32				vipScore;
	// ��ֵ���
	UInt32				chargeMoney;
	// ʱ���
	UInt32				time;
	// ������ʱʱ��
	UInt32				sendGoodTimeoutTime;
	// ���е���
	ItemRewardVec		rewards;
};


#endif