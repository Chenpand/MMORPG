#ifndef _ACCOUNT_ACHIEVEMENT_TASK_MGR_H_
#define _ACCOUNT_ACHIEVEMENT_TASK_MGR_H_

#include "AccountTaskMgr.h"

struct AccountInfo;

class AccountAchievementTask : public AccountTask
{
	AVALON_DEFINE_OBJECTPOOL(AccountAchievementTask, Avalon::NullMutex)

public:
	AccountAchievementTask(AccountInfo* owner, UInt32 id);
	AccountAchievementTask(AccountInfo* owner, UInt32 id, QuestVarMap questVar);
	AccountAchievementTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime);

};

class AccountAchievementTaskMgr : public AccountTaskMgr
{
public:
	AccountAchievementTaskMgr() {};
	virtual ~AccountAchievementTaskMgr() {};

public:
	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, AccountTask::QuestVarMap questVar, bool isSync = true);

	//virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(AccountTask* task, bool force = false);

	/**
	*@brief 是否能够接取一个任务
	*/
	virtual bool IsTaskAcceptable(UInt32 id);

	/**
	*@brief 获取最大任务数
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief 添加成就
	*/
	//virtual bool AddTask(AccountTask* task);

	/**
	*@brief 检查是否完成任务
	*/
	bool CheckFinishTask(UInt32 taskId);

	/**
	*@brief 检查任务奖励背包返回
	*/
	void OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);
public:
	/**
	*@brief 初始化账号成就
	*/
	void InitAccountAchievementTask(bool isSync = true);

public:
	void OnPlayerOnline(WSPlayer* player);

	void OnCreateAdventTeam();

	/**
	 *@brief 等级改变
	 */
	void OnPlayerLevelChanged(WSPlayer* player, UInt32 oldLvl);

	/**
	*@brief 职业改变
	*/
	void OnPlayerOccuChanged(WSPlayer* player);

	/**
	*@brief 冒险队(佣兵团)等级改变
	*/
	void OnAdventureTeamLevelChanged();

	/**
	*@brief 冒险队(佣兵团)评级改变
	*/
	void OnAdventureTeamGradeChanged();

	/**
	*@brief 完成n个指定等级的周常任务
	*/
	void OnCompleteAdventureWeeklyTask(UInt32 taskLevel);
private:

	/**
	*@brief 同步成就列表
	*/
	void SyncAccountAchievementTask();

	/**
	*@brief 开始查询角色等级和职业信息
	*/
	void _BeginQueryRoleLevelAndOccuInfos();

	/**
	*@brief 检查条件角色数量任务
	*/
	void _OnCheckCondRoleNumTasks(WSPlayer* player, UInt8 reason, UInt32 oldLvl = 0);

};

#endif //_ACCOUNT_ACHIEVEMENT_TASK_MGR_H_