#ifndef _ABNORMAL_TRANSACTION_RECORD_H_
#define _ABNORMAL_TRANSACTION_RECORD_H_

#include <AvalonObjectPool.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLAbnormalTransDefine.h>


/*
*@brief 异常交易记录
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

	// 是否永久冻结
	bool IsFrozenPermanent() const;

	// 是否珍品
	bool IsTreasure() const;

	UInt8 GetReason() const { return m_Reason; }

	UInt8 GetMailNotifyType() const { return m_MailNotifyType; }

	UInt32 GetUnfrozenAmount() const { return m_UnfrozenAmount; }

	// 是否需要追缴扣除玩家交易金
	bool NeedRemovePlayerTransMoney() const;

public:
	// 是否可通过充值解冻
	bool CanUnfreezeByCharge();

	// 是否可通过活跃度解冻
	bool CanUnFreezeByDailyTaskScore();

	// 是否只能通过充值解冻
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
	// 状态(AbnormalTransHandleStatus)
	ObjUInt8  m_HandleStatus;
	// 创建时间
	ObjUInt32 m_CreateTime;
	// 买家
	ObjUInt64 m_Buyer;
	// 卖家
	ObjUInt64 m_Seller;
	// 交易时间
	ObjUInt32 m_TransactionTime;
	// 交易id
	ObjString m_TransactionId;
	// 物品实例id
	ObjUInt64 m_ItemUid;
	// 物品数据id
	ObjUInt32 m_ItemDataId;
	// 物品数量
	ObjUInt32 m_ItemNum;
	// 交易金类型
	ObjUInt32 m_MoneyType;
	// 交易金额
	ObjUInt32 m_TransationAmount;

	// 冻结金额
	ObjUInt32 m_FrozenAmount;
	// 返还金额
	ObjUInt32 m_BackAmount;
	// 冻结对象(AbnormalTransFreezeObject)
	ObjUInt8 m_FreezeObject;
	// 冻结金返还途径(AbnormalTransFrozenMoneyBackWay)
	ObjUInt8 m_FrozenMoneyBackWay;
	// 是否永久冻结
	ObjUInt8 m_IsFrozenPermanent;
	// 珍品标志
	ObjUInt8 m_TreasureFlag;
	// 冻结原因
	ObjUInt8 m_Reason;
	// 邮件通知类型
	ObjUInt8 m_MailNotifyType;
	// 未冻结金额
	ObjUInt32 m_UnfrozenAmount;
	// 扣除玩家交易金标志
	ObjUInt8 m_RemovePlayerTransMoneyFlag;
};


#endif