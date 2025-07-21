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
	*@brief 加载数据库
	*/
	void OnLoadTaskInfo(CLRecordCallback* callback);

	/**
	*@brief 创建任务
	*/
	WSActivityTask* CreateActivityTask(WSPlayer* player, ActiveTaskDataEntry* taskData);

	/**
	*@brief 添加一项任务
	*/
	bool AddActivityTask(WSActivityTask* task);
	void AddActivityTaskNoCheck(WSActivityTask* task);

	/**
	*@brief 查找任务
	*/
	WSActivityTask* FindActivityTask(UInt32 accid, UInt32 taskId);
	void GetActivityTaskByAccID(UInt32 accid, std::vector<WSActivityTask*>& tasks);

	/**
	*@brief 条件检查
	*/
	bool CheckCond() { return m_OpenCheck; }
	void SetCondCheck(bool open) { m_OpenCheck = open; }
	bool CheckDrawCount() { return m_OpenCheckDrawCount; }
	void SetDrawCountCheck(bool open) { m_OpenCheckDrawCount = open; }

public:
	/**
	*@brief 接受任务
	*/
	//void AcceptTask(WSPlayer* player);

	/**
	*@brief 接受活动
	*/
	void AcceptActivities(WSPlayer* player);

	/**
	*@brief 接受活动任务
	*/
	void AcceptActivityTasks(WSPlayer* player, const ActivityInfo& actInfo);

	/**
	*@brief 提交任务
	*/
	void SubmitTask(WSPlayer* player, UInt32 taskId);

	/**
	*@brief 根据活动信息刷新任务
	*/
	void TryRefreshTaskByActivity(WSPlayer* player, const ActivityInfo& actInfo, WSActivityTask* task);
	void TryRefreshTaskByActivityNoSync(WSActivityTask* task);
	void TryRefreshTaskByActivityNoSync(const ActivityInfo& actInfo, WSActivityTask* task);

	/**
	*@brief 给予奖励
	*/
	void GiveRewards(WSPlayer* player, WSActivityTask* task, bool notify = true);
	void GiveRewards(WSPlayer* player, WSActivityTask* task, const ItemRewardVec& rewards, bool notify = true);

	/**
	*@brief 获取奖励
	*/
	void GetRewards(WSActivityTask* task, std::vector<ItemReward>& rewards);

public:
	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 玩家上线
	*/
	void OnOnline(WSPlayer* player, bool logicDayChangesd);

	/**
	*@brief 跨天
	*/
	void OnLogicDayChange();

	/**
	*@brief 跨天
	*/
	void OnLogicDayChangeForOnePlayer(WSPlayer* player);

	/**
	*@brief 活动开放
	*/
	void OnActivityOpen(const ActivityInfo& actInfo);

	/**
	*@brief 充值
	*/
	void OnCharge(UInt32 accid, ObjID_t roleId, UInt32 num);

public: /* 七日签到补签 */
	/**
	*@brief 补签
	*/
	void ReSignInSevenTask(WSPlayer* player, const std::vector<UInt32>& taskIds);

	/**
	*@brief 补签检查道具返回
	*/
	bool OnCheckReSignInSevenTaskRet(WSPlayer* player, const std::vector<UInt32>& taskIds);

public:/* oppo琥珀特权 */
	void UpdateAmberPrivilegeTask(WSPlayer* player);
	void UpdateOppoVipLevelTask(WSPlayer* player);
	bool IsOppoPlayer(WSPlayer* player);

public: /* 账号累计充值 */
	void OnAccountTotalCharge(UInt32 accid, ObjID_t roleId, UInt32 num);

private:
	/**
	*@brief 跨天时是否可以更改任务
	*/
	bool _CanChangeOnDayChange(WSActivityTask* task);

	/**
	*@brief 是否限制
	*/
	bool _IsLimit(WSPlayer* player, WSActivityTask* task);

	/**
	*@brief 提交点击领取奖励的任务
	*/
	bool _SubmitClickRewardTask(WSPlayer* player, WSActivityTask* task);

	/**
	*@brief 提交账号充值返利
	*/
	bool _SubmitAccountChargeRebateTask(WSPlayer* player, WSActivityTask* task);

	/**
	*@brief 参数时间是否到了每周刷新时刻
	*/
	bool _CanRefreshTaskByWeek(UInt64 time, WSActivityTask* task);

	/**
	*@brief 获取参数时间距离下一次刷新的时间间隔
	*/
	UInt64 _GetRemainingIntervalOverTime(UInt64 time, RefreshCycleType refreshCycleType);

private:/* 登录特权 */
	void _AcceptLoginPrivilegeTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _OnLogicDayChangeForLoginPrivilegeTask(WSPlayer* player, WSActivityTask* loginTask);
	bool _SubmitLoginPrivilegeTask(WSPlayer* player, WSActivityTask* loginTask);

private:/* 七日签到 */
	void _AcceptSignInSevenTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _UpdateSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask);
	void _UpdateSignInSevenTaskAfterSubmit(WSPlayer* player, WSActivityTask* signInSevenTask);
	void _OnLogicDayChangeForSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask);
	bool _SubmitSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask);

private:/* 七日累计签到 */
	void _AcceptSignInSevenFinishTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _UpdateSignInSevenFinishTask(WSPlayer* player);
	bool _SubmitSignInSevenFinishTask(WSPlayer* player, WSActivityTask* signFinishTask);

private:/* 抽奖 */
	void _AcceptDrawPrizeTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _UpdateDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask);
	void _OnLogicDayChangeForDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask);
	bool _SubmitDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask);

private:/* oppo琥珀特权 */
	void _AcceptAmberPrivilegeTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _OnLogicDayChangeForAmberPrivilegeTask(WSPlayer* player, WSActivityTask* task);

private:/* 充值返利 */
	void _AcceptChargeRebateTask(WSPlayer* player, ActiveTaskDataEntry* taskData);
	void _OnChargeForChargeRebateTask(UInt32 accid, ObjID_t roleId, UInt32 chargeNum);
	void _OnChargeForChargeRebateTask(ObjID_t roleId, WSActivityTask* task, ActiveTaskDataEntry* taskData, UInt32 chargeNum);

private:
	bool m_OpenCheck;

	bool m_OpenCheckDrawCount;

	// 每日定时器
	Avalon::DailyTimer	m_DailyTimer;

	HashMap<UInt32, std::vector<WSActivityTask*>> m_AccountActivityTasks;
};

#endif