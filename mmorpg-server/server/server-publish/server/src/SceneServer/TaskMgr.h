#ifndef _TASK_MGR_H_
#define _TASK_MGR_H_

#include <map>
#include <set>
#include <CLObjectProperty.h>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include "Task.h"
#include "BaseTaskMgr.h"


class Player;
class Npc;
class PlayerShareData;

/**
 *@brief 任务管理器
 */
class TaskMgr : public BaseTaskMgr
{
	//任务完成标识大小
#define TASK_FINISHEDFLAG_SIZE ((MAX_MAINSIDE_TASKID - 1) / 8 + 1)

	//typedef std::map<UInt32, Task*> TaskMap;
	//typedef std::set<UInt32>		TaskIdSet;

public:
	TaskMgr();
	~TaskMgr();

public:

	/**
	 *@brief 接受，提交，删除一个任务
	 */
	virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce, bool bNotifyErr = true);
	virtual bool SubmitTask(BaseTask* task, bool force = false);
	virtual bool AbandonTask(BaseTask* task);
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false, bool skipPreTask = false);

	/**
	 *@brief 接受需要本id为前置任务的任务
	 */
	void AcceptWithOutPreTask(UInt32 id);

	/**
	*@brief 获取最大任务数
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief 获取当前任务数量
	*/
	virtual UInt32 GetTaskNum() const;

	/**
	*@brief 获取协议使用的任务列表
	*/
	void GetTaskBriefList(std::vector<TaskBriefInfo>& taskList);

	/**
	*@brief 强制完成所有任务
	*/
	UInt32 FinishAllTask(TaskType taskType = TaskType::TASK_MAX , UInt32 taskId = 0);

	/**
	*@brief 转职
	*/
	virtual void OnChangeOccu();

public://事件
	/**
	 *@brief tick事件
	 */
	void OnHeartbeat(const Avalon::Time& now, int tickType);

	/**
	 *@brief 玩家上线
	 */
	void OnOnline(bool bDayChanged);

	/**
	 *@brief 玩家升级
	 */
	void OnLevelup();

private:
	//可接任务集
	TaskIdSet	m_AcceptableTasks;
	//拥有者
	Player*		m_pOwner;
};


class DailyTaskMgr : public BaseTaskMgr
{
public:
	DailyTaskMgr();
	virtual ~DailyTaskMgr() {};

public:
	/**
	*@brief 接受一个任务
	*/
	virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief 提交所有日常
	*/
	bool SubmitAllTask();

	/**
	*@brief 修改所有日常任务状态
	*/
	void SetAllTaskStatus();

	/**
	*@brief 是否能够接取一个任务
	*/
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false);

	/**
	*@brief 获取最大任务数
	*/
	virtual UInt32 GetMaxTaskNum();

public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief 玩家升级
	*/
	void OnLevelup();

public:

	/**
	*@brief 刷新日常任务 Add by huchenhui
	*/
	void RefreshDailyTask();

private:

	/**
	*@brief 同步日常任务 added by huchenhui 2016.06.23
	*/
	void SyncDailyTask();


	void RandomPoolIndex(UInt32 maxIndex, UInt32 size, std::vector<UInt32>& indexs);

	/**
	*@brief 随机日常任务 Add by huchenhui
	*/
	void RandomDailyTask(TaskDataEntryMgr::TaskDataVec& tasks);

	/**
	*@brief 获取未完成日常  Add by huchenhui
	*/
	std::vector<DailyTask*> GetUnfinishedTasks();

	/**
	*@brief 接取升级以后能够接取的日常
	*/
	void UplevelAcceptTask(UInt16 level);

private:
	UInt32 m_TaskPoint;

};

class LegendTaskMgr : public BaseTaskMgr
{
public:
	LegendTaskMgr() {};
	virtual ~LegendTaskMgr() {};

	static bool Init();

public:
	virtual bool AddTask(BaseTask* task);

public:
	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, bool isSync = true);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief 重置一个任务
	*/
	void ResetTask(BaseTask* task);

	/**
	*@brief 是否能够接取一个任务
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskData);

	/**
	*@brief 获取传奇之路任务列表
	*/
	void GetLegendTasks(std::vector<TaskBriefInfo>& tasks);

	/**
	*@brief 获取传奇之路任务最大数量
	*/
	virtual UInt32 GetMaxTaskNum() { return MAX_ACCEPT_LEGEND_NUM; }

	/**
	*@brief 检查是否完成任务
	*/
	bool CheckFinishTask(UInt32 taskId);

public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief 逻辑跨天
	*/
	void OnLogicDayChanged();

	/**
	*@brief 玩家升级
	*/
	void OnLevelup();

private:
	void InitLegendTasks(bool isSync = true);
};


class AchievementTaskMgr : public BaseTaskMgr
{
public:
	AchievementTaskMgr() {};
	virtual ~AchievementTaskMgr() {};

public:
	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, Task::QuestVarMap questVar, bool isSync = true);

	bool AcceptTask(TaskDataEntry* taskDataEntry, bool isSync = true);

	//virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

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
	virtual bool AddTask(BaseTask* task);

	/**
	*@brief 检查是否完成任务
	*/
	bool CheckFinishTask(UInt32 taskId);

public:

	/**
	*@brief 初始化成就  added by huchenhui 2016.06.27
	*/
	void InitAchievementTask(bool isSync = true);

	/**
	*@brief 获取奖励SP点的数量  added by huchenhui 2017.10.23
	*/
	UInt32 GetRewardSPNum();

public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

	void OnLevelup();

	void OnTaskSucced(BaseTask* task);

private:

	/**
	*@brief 同步成就列表 added by huchenhui 2016.06.25
	*/
	void SyncAchievementTask();

};

class CycleTaskMgr : public BaseTaskMgr
{
public:
	CycleTaskMgr();
	virtual ~CycleTaskMgr();

public:

	UInt32 GetMaxTaskNum() { return 1; }

	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief 是否能够接取一个任务
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskDataEntry);

	/**
	*@brief 刷新循环任务
	*/
	//UInt32 RefreshCycleTask();

	/**
	*@brief 刷新循环任务
	*/
	UInt32 RefreshCycleTask(UInt32 taskId = 0);

	/**
	*@brief 刷新循环任务 循环次数
	*/
	void RefreshCycleTaskDailyCount();

	/**
	*@brief 下发任务列表
	*/
	//void SyncCycleTask();

	/**
	*@brief 获取可以生成的任务类型
	*/
	void GetCanGenTaskType(std::vector<UInt8>& types, UInt8 excludeType = CTT_INVALID);

	/**
	*@brief 完成任务
	*/
	void OnCompleteTaskSucc(BaseTask* task);

	/**
	*@brief 清空任务类型完成次数计数
	*/
	void ClearTypeCounter();

public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief 玩家升级
	*/
	void OnLevelup();

private:
	/**
	*@brief 随机循环任务类型
	*/
	CycleTaskType RandomCycleType(bool force = false);

	/**
	*@brief 初始化任务
	*/
	void InitCycle();

	/**
	*@brief 刷新循环任务
	*/
	UInt32 RefreshTask(UInt32 taskId = 0, bool force = false);

	/**
	*@brief 放弃循环任务
	*/
	void AbandonAllTask();

private:
	UInt32 m_CycleTaskMax;
	UInt32 m_CycleTaskDailyMax;
	UInt32 m_CycleTaskRefreshFreeCount;
	UInt32 m_CycleTaskRefreshConsume;

	UInt32 m_CycleTaskTypeMax[CTT_MAX];
};

class RedPacketTaskMgr : public AchievementTaskMgr
{
public:
	RedPacketTaskMgr() {};
	virtual ~RedPacketTaskMgr() {};

public:
	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, Task::QuestVarMap questVar);

	//virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief 是否能够接取一个任务
	*/
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false);

	/**
	*@brief 初始化任务  added by huchenhui 2016.12.16
	*/
	void InitRedPacketTask();

public:
	void OnHeartbeat(const Avalon::Time& now, int tickType);

public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

};

class MasterDailyTaskMgr : public BaseTaskMgr
{
public:
	MasterDailyTaskMgr();
	~MasterDailyTaskMgr() {}
public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief 玩家下线
	*/
	void OnOffline();

	/**
	*@brief 领取全部任务
	*/
	void AcceptAllTask();

	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief 是否能够接取一个任务
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskData);

	/**
	*@brief 逻辑跨天
	*/
	void OnLogicDayChanged();

	/**
	*@brief 放弃所有任务
	*/
	void AbandonAllTask(bool notify = true, bool checkAcceptTime = false);

	/**
	*@brief 获取最大任务数
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief 获取离线数据
	*/
	void GetShareData(MasterTaskShareData& sharedata);

	/**
	*@brief 检查师门日常任务是否都完成
	*/
	bool CheckAllTasksFinished();

	/**
	*@brief 解除了和师傅的关系
	*/
	void OnRelationRelieveMaster(ObjID_t masterId);

	/**
	*@brief 徒弟出师
	*/
	void OnRelationDiscipleFinishSchool(ObjID_t masterId);

public:
	/**
	*@brief 直接操作数据库清空任务,玩家离线的时候操作
	*/
	void static ClearOfflinePlayerAllTask(ObjID_t ownerId);
private:
	/*
	*@brief 同步到客户端
	*/
	void SyncToClient();

	/**
	*@brief 同步到world
	*/
	void SyncToWorld();
};

class MasterAcademicGrowthTaskMgr : public BaseTaskMgr
{
public:
	MasterAcademicGrowthTaskMgr();
	~MasterAcademicGrowthTaskMgr() {}
public:
	/**
	*@brief 玩家上线
	*/
	void OnOnline(bool bDayChanged);

	/*
	*@brief 玩家下线
	*/
	void OnOffline();

	/*
	*@brief 领取全部任务
	*/
	void AcceptAllTask(bool force = false);

	/*
	*@brief 放弃所有任务
	*/
	void AbandonAllTask(bool notify = true);

	/**
	*@brief 接受一个任务
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry);

	/**
	*@brief 是否能够接取一个任务
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskData);

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief 获取最大任务数
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief 拜师成功后接取任务
	*/
	void OnAddMaster();

	/**
	*@brief 获取离线数据
	*/
	void GetShareData(MasterTaskShareData& sharedata);

	/**
	*@brief 解除了和师傅的关系
	*/
	void OnRelationRelieveMaster(ObjID_t masterId);

	/**
	*@brief 徒弟出师
	*/
	void OnRelationDiscipleFinishSchool(ObjID_t masterId);

	/**
	*@brief 升级
	*/
	void OnLevelup();
	
	/**
	*@brief pvp
	*/
	void OnTeamPVP(PkType type, PkRaceResult status, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 怪物攻城
	*/
	void OnTeamKillCityMonster(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds);
public:
	/**
	*@brief 直接操作数据库清空任务
	*/
	void static ClearOfflinePlayerAllTask(ObjID_t ownerId);
private:
	/*
	*@brief 同步到客户端
	*/
	void SyncToClient();

	/**
	*@brief 同步到world
	*/
	void SyncToWorld();

};

#endif
