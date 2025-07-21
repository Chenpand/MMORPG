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
	*@brief ����쳣��¼
	*/
	bool AddAbnormalRecord(AbnormalTransactionRecord* record);

	/**
	*@brief �����쳣��¼
	*/
	AbnormalTransactionRecord* FindAbnormalRecord(ObjID_t id);

	/**
	*@brief ���ʼ�¼
	*/
	void VisitAbnormalRecord(const std::function<bool(AbnormalTransactionRecord*)>& F);

public:
	/**
	*@brief �������
	*/
	void OnOnline();

	/**
	*@brief ��ҳ�ֵ
	*/
	void OnCharge(UInt32 chargeNum);

	/**
	*@brief ��������ճ��������(��Ծ��)
	*/
	void OnIncDailyTaskScore(UInt32 totalScore);

	/**
	*@brief tick
	*/
	void OnHeartbeat(const Avalon::Time& now, int tickType);

	/**
	*@brief �����л�
	*/
	void OnSceneChanged();

public:
	/**
	*@brief �����ݿ��������
	*/
	bool LoadDataFromDBCallback(CLRecordCallback* callback, bool isLoadWaitRecord = false);

	/**
	*@brief �����ݿ��������
	*/
	bool LoadBuyerDataFromDB(CLRecordCallback* callback);

	/**
	*@brief ��ѯ����Ͳ������쳣���׼�¼���ݿⷵ��
	*/
	void OnQueryFrozenAndNeverBackAbnormalTransactionRecordRet();

	/**
	*@brief ��ѯ�ȴ����ύ�׼�¼���ݿⷵ��
	*/
	void OnQueryWaitFreezeAbnormalTransactionRecordRet(CLRecordCallback* callback);

	/**
	*@brief �۳��ʼ����߷���
	*/
	void OnRemoveMailItemRes(const CLProtocol::SceneAbnormalTransRemoveMailItemRes& res);

	/**
	*@brief �쳣���ײ�ѯ����
	*/
	void OnQueryAbnormalTransactionRecordReq();

	/**
	*@brief �쳣���׽ⶳ����
	*/
	void OnAbnormalTransUnfreezeReq();

	/**
	*@brief ��ѯ��������
	*/
	void QueryWaitFreezeAbnormalTrans();

private:
	/**
	*@brief ����״̬Ϊ�ȴ�������쳣����
	*/
	void _OnHandleWaiteHandleAbnormalTransaction(AbnormalTransactionRecord* abnormalRecord);

	/**
	*@brief ֪ͨ�ͻ����쳣����
	*/
	void _NotifyClientAbnormalTransaction(bool bNotify);

	/**
	*@brief ���֪ͨ�ͻ����쳣����
	*/
	void _OnCheckNotifyAbnormalTransaction();

	/**
	*@brief ����ʱ����֪ͨ�ʼ�
	*/
	void _SendNotifyMailWhileFrozen(const AbnormalTransactionRecord* curFrozenTailRecord, const AbnormalTransactionRecord* preFrozenTailRecord);

	/**
	*@brief �ⶳʱ����֪ͨ�ʼ�
	*/
	void _SendNotifyMailWhileUnFreeze(UInt32 unfreezeAmount, UInt32 restBackAmount = 0);

	/**
	*@brief ���͵���
	*/
	void _OnTailDeadlineExpire();

	/**
	*@brief �����쳣�����������
	*/
	void _TerminateAbnormalTransData();

	/**
	*@brief ��ȡ��󶳽�ʱ��
	*/
	UInt32 _GetMaxFrozenTime();

	/**
	*@brief ����ʣ�ඳ�������
	*/
	UInt32 _BackRestFrozenMoneyToPlayer(AbnormalTransactionRecord* record);

	/**
	*@brief �ϲ��쳣���׼�¼
	*/
	void _MergeAbnormalTransaction(AbnormalTransactionRecord* destRecord, AbnormalTransactionRecord* srcRecord, UInt32 freezeBaseDays);

private:
	Player* m_Owner;

	// �����ڽ���ʱ��
	UInt32  m_TailDeadline;

	AbnormalTransactionRecordVec m_AbnormalTransactionRecords;

	// ���߼������ݿ����
	UInt8 m_PlayerOnlineLoadDBCount;
};

#endif