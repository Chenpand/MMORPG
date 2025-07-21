#ifndef _ACCOUNT_TASK_MGR_H_
#define _ACCOUNT_TASK_MGR_H_

#include <map>
#include <set>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>

#include "AccountTask.h"

class WSPlayer;
class DungeonDataEntry;

class AccountTaskMgr
{
public:
	typedef std::map<UInt32, AccountTask*>	TaskMap;
	typedef std::set<UInt32>			TaskIdSet;

public:
	AccountTaskMgr();
	virtual ~AccountTaskMgr();

public:
	AccountInfo* GetOwner() const { return m_pOwner; }
	void SetOwner(AccountInfo* owner) { m_pOwner = owner; }

public:
	virtual bool AddTask(AccountTask* task);

	/**
	*@brief 加载一个任务
	*/
	bool LoadTask(AccountTask* task);

	/**
	*@brief 查找一个任务
	*/
	AccountTask* FindTask(UInt32 id) const;

	/**
	*@brief 删除一个任务
	*/
	void RemoveTask(AccountTask* task);

	/**
	*@brief 清除所有任务
	*/
	void ClearTask();

	/**
	*@brief 获取任务数
	*/
	virtual UInt32 GetTaskNum() const;

	/**
	*@brief 遍历所有任务
	*/
	void VisitTasks(AccountTaskVisitor& visitor);

	/**
	*@brief 获取任务列表
	*/
	void GetTaskList(std::vector<TaskBriefInfo>& task);

	virtual bool GetInitTaskBriefInfo(UInt32 id, TaskBriefInfo& info);

	virtual void CheckFinish();

	virtual UInt32 SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items);
public:
	/**
	*@brief 检查任务奖励背包返回
	*/
	virtual void OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);

public:
	/**
	*@brief 具有归属权的npc死亡
	*/
	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

public:
	void SaveToDB();

protected:
	void OnAcceptOneTask(AccountTask* pTask);

	void OnSubmitOneTask(AccountTask* pTask);
protected:

	//任务map
	TaskMap		m_Tasks;

	AccountInfo* m_pOwner;

};

class GetAccountTaskInfoVisitor : public AccountTaskVisitor
{
public:
	GetAccountTaskInfoVisitor(std::vector<TaskBriefInfo>& taskList)
		:m_TaskList(taskList) {}

	bool operator()(AccountTask* task)
	{
		m_TaskList.resize(m_TaskList.size() + 1);
		task->GetBriefInfo(*(m_TaskList.end() - 1));
		return true;
	}
private:
	std::vector<TaskBriefInfo>& m_TaskList;
};



class WeeklyTaskMgr : public AccountTaskMgr
{
public:
	WeeklyTaskMgr();
	virtual ~WeeklyTaskMgr() {};

public:
	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(const TaskDataEntry* taskDataEntry, bool isSync = true);

	/**
	*@brief 提交一个任务
	*/
	bool SubmitTask(AccountTask* task, bool force = false);

	/**
	*@brief 是否能够接取一个任务
	*/
	bool IsTaskAcceptable(const TaskDataEntry* dataEntry, bool bNotifyErr = false);

	/**
	*@brief 刷新任务
	*/
	void RefreshTasks();

	/**
	*@brief 获取已提交的冒险队任务数
	*/
	UInt32 GetOverAdventureTeamTasks();

	/**
	*@brief 完成一个任务
	*/
	bool FinishTask(UInt32 id, bool force = false);

	UInt32 SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items);
public:
	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief 逻辑跨周
	*/
	void OnLogicWeekChange();

	/**
	*@brief 检查任务奖励背包返回
	*/
	virtual void OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);

	/**
	*@brief 玩家通关
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);
	void OnPlayerClearDungeonForTask(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime, TaskSubType taskSubType);

	/**
	*@brief 玩家消耗疲劳
	*/
	void OnPlayerRemoveFatigue(UInt32 removeFatigue);

	bool OnCheckItemForSubmitItemTaskRet(UInt32 taskId);
private:
	/**
	*@brief 接受任务
	*/
	void _AcceptTasks();

	/**
	*@brief 根据类型删除任务
	*/
	void _RemoveTasksByType(TaskType taskType);

	/**
	*@brief 是否已接受冒险队任务
	*/
	bool _HasAcceptedAdventureTeamTasks();

	/**
	*@brief 接受冒险队任务
	*/
	void _AcceptAdventureTeamTasks();

};


#endif //_ACCOUNT_TASK_MGR_H_