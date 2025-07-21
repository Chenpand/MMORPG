#ifndef _ABNORMAL_TRANSACTION_MGR_
#define _ABNORMAL_TRANSACTION_MGR_

#include <functional>
#include <CLAuctionProtocol.h>

#include "AbnormalTransactionRecord.h"


class CLRecordCallback;
class Player;


typedef std::vector<AbnormalTransactionRecord*> AbnormalTransactionRecordVec;

class AbnormalTransactionMgr
{
public:
	AbnormalTransactionMgr();
	~AbnormalTransactionMgr();

	void SetOwner(Player* player) { m_Owner = player; }
	Player* GetOwner() { return m_Owner; }

	void SetTailDeadline(UInt32 time) { m_TailDeadline = time; }
	void IncTailDeadline(UInt32 time) { m_TailDeadline += time; }
	void DecTailDeadline(UInt32 time) { m_TailDeadline <= time ? m_TailDeadline = 0 : m_TailDeadline -= time; }
	UInt32 GetTailDeadline() const { return m_TailDeadline; }

	void ResetPlayerOnlineLoadDBCount() { m_PlayerOnlineLoadDBCount = 0; }

public:
	/**
	*@brief 添加异常记录
	*/
	bool AddAbnormalRecord(AbnormalTransactionRecord* record);

	/**
	*@brief 查找异常记录
	*/
	AbnormalTransactionRecord* FindAbnormalRecord(ObjID_t id);

	/**
	*@brief 访问记录
	*/
	void VisitAbnormalRecord(const std::function<bool(AbnormalTransactionRecord*)>& F);

public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline();

	/**
	*@brief 玩家充值
	*/
	void OnCharge(UInt32 chargeNum);

	/**
	*@brief 玩家增加日常任务积分(活跃度)
	*/
	void OnIncDailyTaskScore(UInt32 totalScore);

	/**
	*@brief tick
	*/
	void OnHeartbeat(const Avalon::Time& now, int tickType);

	/**
	*@brief 场景切换
	*/
	void OnSceneChanged();

public:
	/**
	*@brief 从数据库加载数据
	*/
	bool LoadDataFromDBCallback(CLRecordCallback* callback, bool isLoadWaitRecord = false);

	/**
	*@brief 从数据库加载数据
	*/
	bool LoadBuyerDataFromDB(CLRecordCallback* callback);

	/**
	*@brief 查询冻结和不返还异常交易记录数据库返回
	*/
	void OnQueryFrozenAndNeverBackAbnormalTransactionRecordRet();

	/**
	*@brief 查询等待冻结交易记录数据库返回
	*/
	void OnQueryWaitFreezeAbnormalTransactionRecordRet(CLRecordCallback* callback);

	/**
	*@brief 扣除邮件道具返回
	*/
	void OnRemoveMailItemRes(const CLProtocol::SceneAbnormalTransRemoveMailItemRes& res);

	/**
	*@brief 异常交易查询请求
	*/
	void OnQueryAbnormalTransactionRecordReq();

	/**
	*@brief 异常交易解冻请求
	*/
	void OnAbnormalTransUnfreezeReq();

	/**
	*@brief 查询待处理交易
	*/
	void QueryWaitFreezeAbnormalTrans();

private:
	/**
	*@brief 处理状态为等待处理的异常交易
	*/
	void _OnHandleWaiteHandleAbnormalTransaction(AbnormalTransactionRecord* abnormalRecord);

	/**
	*@brief 通知客户端异常交易
	*/
	void _NotifyClientAbnormalTransaction(bool bNotify);

	/**
	*@brief 检测通知客户端异常交易
	*/
	void _OnCheckNotifyAbnormalTransaction();

	/**
	*@brief 冻结时发送通知邮件
	*/
	void _SendNotifyMailWhileFrozen(const AbnormalTransactionRecord* curFrozenTailRecord, const AbnormalTransactionRecord* preFrozenTailRecord);

	/**
	*@brief 解冻时发送通知邮件
	*/
	void _SendNotifyMailWhileUnFreeze(UInt32 unfreezeAmount, UInt32 restBackAmount = 0);

	/**
	*@brief 保释到期
	*/
	void _OnTailDeadlineExpire();

	/**
	*@brief 结束异常交易相关数据
	*/
	void _TerminateAbnormalTransData();

	/**
	*@brief 获取最大冻结时间
	*/
	UInt32 _GetMaxFrozenTime();

	/**
	*@brief 返还剩余冻结金给玩家
	*/
	UInt32 _BackRestFrozenMoneyToPlayer(AbnormalTransactionRecord* record);

	/**
	*@brief 合并异常交易记录
	*/
	void _MergeAbnormalTransaction(AbnormalTransactionRecord* destRecord, AbnormalTransactionRecord* srcRecord, UInt32 freezeBaseDays);

private:
	Player* m_Owner;

	// 保释期结束时间
	UInt32  m_TailDeadline;

	AbnormalTransactionRecordVec m_AbnormalTransactionRecords;

	// 上线加载数据库次数
	UInt8 m_PlayerOnlineLoadDBCount;
};

#endif