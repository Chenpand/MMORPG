#ifndef _ABNORMAL_TRANSACTION_RECORD_H_
#define _ABNORMAL_TRANSACTION_RECORD_H_

#include <AvalonObjectPool.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLAbnormalTransDefine.h>


/*
*@brief �쳣���׼�¼
*/
class AbnormalTransactionRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AbnormalTransactionRecord, Avalon::NullMutex)

public:
	AbnormalTransactionRecord();
	~AbnormalTransactionRecord();

public:
	ObjID_t GetBuyer() const { return m_Buyer; }
	ObjID_t GetSeller() const { return m_Seller; }

	void SetHandleStatus(AbnormalTransHandleStatus status);
	AbnormalTransHandleStatus GetHandleStatus() const { return (AbnormalTransHandleStatus)m_HandleStatus.GetDBValue(); }

	void SetCreateTime(UInt32 time) { m_CreateTime = time; }
	UInt32 GetCreateTime() const { return m_CreateTime; }

	UInt32 GetMoneyType() const { return m_MoneyType; }

	UInt32 GetTansactionTime() const { return m_TransactionTime; }

	UInt32 GetTransationAmount() const { return m_TransationAmount; }

	UInt32 GetFrozeAmount() const { return m_FrozenAmount; }
	void SetFrozeAmount(UInt32 amount);
	void IncFrozeAmount(UInt32 amount);

	UInt32 GetBackAmount() const { return m_BackAmount; }
	void IncBackAmount(UInt32 amount);

	AbnormalTransFreezeObject GetFreezeObject() const { return (AbnormalTransFreezeObject)m_FreezeObject.GetDBValue(); }

	void SetFrozenMoneyBackWay(UInt8 backWay);
	UInt8 GetFrozenMoneyBackWay() const { return m_FrozenMoneyBackWay; }

	void SetFrozenPermanentFlag(bool flag) { m_IsFrozenPermanent = flag ? 1 : 0; }

	// �Ƿ����ö���
	bool IsFrozenPermanent() const;

	// �Ƿ���Ʒ
	bool IsTreasure() const;

	UInt8 GetReason() const { return m_Reason; }

	UInt8 GetMailNotifyType() const { return m_MailNotifyType; }

	UInt32 GetUnfrozenAmount() const { return m_UnfrozenAmount; }

	// �Ƿ���Ҫ׷�ɿ۳���ҽ��׽�
	bool NeedRemovePlayerTransMoney() const;

public:
	// �Ƿ��ͨ����ֵ�ⶳ
	bool CanUnfreezeByCharge();

	// �Ƿ��ͨ����Ծ�Ƚⶳ
	bool CanUnFreezeByDailyTaskScore();

	// �Ƿ�ֻ��ͨ����ֵ�ⶳ
	bool IsOnlyUnfreezeByCharge();

public:
	const std::string LogStr() const;

public:
	void SaveToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("handle_status", m_HandleStatus);
		CL_DBPROPERTY("create_time", m_CreateTime);
		CL_DBPROPERTY("buyer", m_Buyer);
		CL_DBPROPERTY("seller", m_Seller);
		CL_DBPROPERTY("transaction_time", m_TransactionTime);
		CL_DBPROPERTY("transction_id", m_TransactionId);
		CL_DBPROPERTY("item_id", m_ItemUid);
		CL_DBPROPERTY("item_data_id", m_ItemDataId);
		CL_DBPROPERTY("item_num", m_ItemNum);
		CL_DBPROPERTY("money_type", m_MoneyType);
		CL_DBPROPERTY("transaction_amount", m_TransationAmount);
		CL_DBPROPERTY("frozen_amount", m_FrozenAmount);
		CL_DBPROPERTY("back_amount", m_BackAmount);
		CL_DBPROPERTY("freeze_object", m_FreezeObject);
		CL_DBPROPERTY("back_way", m_FrozenMoneyBackWay);
		CL_DBPROPERTY("frozen_permanent", m_IsFrozenPermanent);
		CL_DBPROPERTY("treasure_flag", m_TreasureFlag);
		CL_DBPROPERTY("reason", m_Reason);
		CL_DBPROPERTY("mail_notify_type", m_MailNotifyType);
		CL_DBPROPERTY("unfrozen_amount", m_UnfrozenAmount);
		CL_DBPROPERTY("rmplayer_transmoney_flag", m_RemovePlayerTransMoneyFlag);

		CL_DBPROPERTY_END()

private:
	// ״̬(AbnormalTransHandleStatus)
	ObjUInt8  m_HandleStatus;
	// ����ʱ��
	ObjUInt32 m_CreateTime;
	// ���
	ObjUInt64 m_Buyer;
	// ����
	ObjUInt64 m_Seller;
	// ����ʱ��
	ObjUInt32 m_TransactionTime;
	// ����id
	ObjString m_TransactionId;
	// ��Ʒʵ��id
	ObjUInt64 m_ItemUid;
	// ��Ʒ����id
	ObjUInt32 m_ItemDataId;
	// ��Ʒ����
	ObjUInt32 m_ItemNum;
	// ���׽�����
	ObjUInt32 m_MoneyType;
	// ���׽��
	ObjUInt32 m_TransationAmount;

	// ������
	ObjUInt32 m_FrozenAmount;
	// �������
	ObjUInt32 m_BackAmount;
	// �������(AbnormalTransFreezeObject)
	ObjUInt8 m_FreezeObject;
	// ����𷵻�;��(AbnormalTransFrozenMoneyBackWay)
	ObjUInt8 m_FrozenMoneyBackWay;
	// �Ƿ����ö���
	ObjUInt8 m_IsFrozenPermanent;
	// ��Ʒ��־
	ObjUInt8 m_TreasureFlag;
	// ����ԭ��
	ObjUInt8 m_Reason;
	// �ʼ�֪ͨ����
	ObjUInt8 m_MailNotifyType;
	// δ������
	ObjUInt32 m_UnfrozenAmount;
	// �۳���ҽ��׽��־
	ObjUInt8 m_RemovePlayerTransMoneyFlag;
};


#endif