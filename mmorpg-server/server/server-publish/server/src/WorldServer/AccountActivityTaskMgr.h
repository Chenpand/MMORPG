#ifndef _ACCOUNT_ACTIVITY_TASK_MGR_
#define _ACCOUNT_ACTIVITY_TASK_MGR_

#include <CLObject.h>
#include <CLActiveEventDataEntry.h>
#include <AvalonDailyTimer.h>
#include <CLRecordCallback.h>
#include <CLActivityDefine.h>
#include "WSActivityTask.h"

class WSPlayer;


class AccountActivityTaskMgr : public Avalon::Singleton<AccountActivityTaskMgr>
{
public:
	AccountActivityTaskMgr();
	~AccountActivityTaskMgr();

public:
	/**
	*@brief �������ݿ�
	*/
	void OnLoadTaskInfo(CLRecordCallback* callback);

	/**
	*@brief ��������
	*/
	WSActivityTask* CreateActivityTask(WSPlayer* player, ActiveTaskDataEntry* taskData);

	/**
	*@brief ���һ������
	*/
	bool AddActivityTask(WSActivityTask* task);
	void AddActivityTaskNoCheck(WSActivityTask* task);

	/**
	*@brief ��������
	*/
	WSActivityTask* FindActivityTask(UInt32 accid, UInt32 taskId);
	void GetActivityTaskByAccID(UInt32 accid, std::vector<WSActivityTask*>& tasks);

	/**
	*@brief �������
	*/
	bool CheckCond() { return m_OpenCheck; }
	void SetCondCheck(bool open) { m_OpenCheck = open; }
	bool CheckDrawCount() { return m_OpenCheckDrawCount; }
	void SetDrawCountCheck(bool open) { m_OpenCheckDrawCount = open; }

public:
	/**
	*@brief ��������
	*/
	//void AcceptTask(WSPlayer* player);

	/**
	*@brief ���ܻ
	*/
	void AcceptActivities(WSPlayer* player);

	/**
	*@brief ���ܻ����
	*/
	void AcceptActivityTasks(WSPlayer* player, const ActivityInfo& actInfo);

	/**
	*@brief �ύ����
	*/
	void SubmitTask(WSPlayer* player, UInt32 taskId);

	/**
	*@brief ���ݻ��Ϣˢ������
	*/
	void TryRefreshTaskByActivity(WSPlayer* player, const ActivityInfo& actInfo, WSActivityTask* task);
	void TryRefreshTaskByActivityNoSync(WSActivityTask* task);
	void TryRefreshTaskByActivityNoSync(const ActivityInfo& actInfo, WSActivityTask* task);

	/**
	*@brief ���轱��
	*/
	void GiveRewards(WSPlayer* player, WSActivityTask* task, bool notify = true);
	void GiveRewards(WSPlayer* player, WSActivityTask* task, const ItemRewardVec& rewards, bool notify = true);

	/**
	*@brief ��ȡ����
	*/
	void GetRewards(WSActivityTask* task, std::vector<ItemReward>& rewards);

public:
	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �������
	*/
	void OnOnline(WSPlayer* player, bool logicDayChangesd);

	/**
	*@brief ����
	*/
	void OnLogicDayChange();

	/**
	*@brief ����
	*/
	void OnLogicDayChangeForOnePlayer(WSPlayer* player);

	/**
	*@brief �����
	*/
	void OnActivityOpen(const ActivityInfo& actInfo);

	/**
	*@brief ��ֵ
	*/
	void OnCharge(UInt32 accid, ObjID_t roleId, UInt32 num);

public: /* ����ǩ����ǩ */
	/**
	*@brief ��ǩ
	*/
	void ReSignInSevenTask(WSPlayer* player, const std::vector<UInt32>& taskIds);

	/**
	*@brief ��ǩ�����߷���
	*/
	bool OnCheckReSignInSevenTaskRet(WSPlayer* player, const std::vector<UInt32>& taskIds);

public:/* oppo������Ȩ */
	void UpdateAmberPrivilegeTask(WSPlayer* player);
	void UpdateOppoVipLevelTask(WSPlayer* player);
	bool IsOppoPlayer(WSPlayer* player);

public: /* �˺��ۼƳ�ֵ */
	void OnAccountTotalCharge(UInt32 accid, ObjID_t roleId, UInt32 num);

private:
	/**
	*@brief ����ʱ�Ƿ���Ը�������
	*/
	bool _CanChangeOnDayChange(WSActivityTask* task);

	/**
	*@brief �Ƿ�����
	*/
	bool _IsLimit(WSPlayer* player, WSActivityTask* task);

	/**
	*@brief �ύ�����ȡ����������
	*/
	bool _SubmitClickRewardTask(WSPlayer* player, WSActivityTask* task);

	/**
	*@brief �ύ�˺ų�ֵ����
	*/
	bool _SubmitAccountChargeRebateTask(WSPlayer* player, WSActivityTask* task);

	/**
	*@brief ����ʱ���Ƿ���ÿ��ˢ��ʱ��
	*/
	bool _CanRefreshTaskByWeek(UInt64 time, WSActivityTask* task);

	/**
	*@brief ��ȡ����ʱ�������һ��ˢ�µ�ʱ����
	*/
	UInt64 _GetRemainingIntervalOverTime(UInt64 time, RefreshCycleType refreshCycleType);

private:/* ��¼��Ȩ */
	void _AcceptLoginPrivilegeTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _OnLogicDayChangeForLoginPrivilegeTask(WSPlayer* player, WSActivityTask* loginTask);
	bool _SubmitLoginPrivilegeTask(WSPlayer* player, WSActivityTask* loginTask);

private:/* ����ǩ�� */
	void _AcceptSignInSevenTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _UpdateSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask);
	void _UpdateSignInSevenTaskAfterSubmit(WSPlayer* player, WSActivityTask* signInSevenTask);
	void _OnLogicDayChangeForSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask);
	bool _SubmitSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask);

private:/* �����ۼ�ǩ�� */
	void _AcceptSignInSevenFinishTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _UpdateSignInSevenFinishTask(WSPlayer* player);
	bool _SubmitSignInSevenFinishTask(WSPlayer* player, WSActivityTask* signFinishTask);

private:/* �齱 */
	void _AcceptDrawPrizeTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _UpdateDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask);
	void _OnLogicDayChangeForDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask);
	bool _SubmitDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask);

private:/* oppo������Ȩ */
	void _AcceptAmberPrivilegeTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _OnLogicDayChangeForAmberPrivilegeTask(WSPlayer* player, WSActivityTask* task);

private:/* ��ֵ���� */
	void _AcceptChargeRebateTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _OnChargeForChargeRebateTask(UInt32 accid, ObjID_t roleId, UInt32 chargeNum);
	void _OnChargeForChargeRebateTask(ObjID_t roleId, WSActivityTask* task, ActiveTaskDataEntry* taskData, UInt32 chargeNum);

private:
	bool m_OpenCheck;

	bool m_OpenCheckDrawCount;

	// ÿ�ն�ʱ��
	Avalon::DailyTimer	m_DailyTimer;

	HashMap<UInt32, std::vector<WSActivityTask*>> m_AccountActivityTasks;
};

#endif