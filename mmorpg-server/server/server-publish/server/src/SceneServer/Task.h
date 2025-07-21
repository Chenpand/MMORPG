#ifndef _TASK_H_
#define _TASK_H_

#include <CLTaskDefine.h>
#include <CLTaskDataEntry.h>
#include <CLTaskProtocol.h>
#include <CLObject.h>
#include <map>
#include <sstream>
#include <CLRewardAdapter.h>

#include "BaseTask.h"


/**
 *@brief 任务
 */
class Task :public BaseTask
{
	AVALON_DEFINE_OBJECTPOOL(Task, Avalon::NullMutex)

public: 
	Task(Player* owner, UInt32 id);
	Task(Player* owner, UInt32 id, QuestVarMap questVar);
	Task(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);
	virtual ~Task();

public:

	/**
	 *@brief 设置数据项
	 */
	virtual bool SetupBase();

	/**
	 *@brief 获取数据项
	 */
	TaskDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief 获取基本属性
	 */
	virtual TaskType GetType() const{ return m_pDataEntry->type; }
	virtual TaskSubType GetSubType() const{ return m_pDataEntry->subType; }
	virtual const std::string& GetName() const { return m_pDataEntry->name; }
	virtual UInt32 GetLimitTime() const{ return m_pDataEntry->timeLimit; }
	virtual UInt32 GetAbandonLimitTime() const{ return m_pDataEntry->abandomTimeLimit; }
	virtual bool IsAnnouncement() const { return m_pDataEntry->isAnnouncement; }

	/**
	*@brief 获取任务接取次数
	*/
	virtual UInt32 GetMaxSubmitCount() const { return m_pDataEntry->submitCount; }


	/**
	 *@brief 根据状态获取任务目标描述
	 */
	const std::string& GetTargetByStatus() const;
	const std::string& GetTargetByStatus(UInt8 status) const;
	const std::string& GetDescription() const;

	static const std::string& GetDescription(Player* player, TaskDataEntry* dataEntry);
	static const std::string& GetTargetByStatus(Player* player, TaskDataEntry* dataEntry, UInt8 status);

	/**
	*@brief 获取任务奖励
	*/
	virtual void GiveRewards();

	/**
	 *@brief 获得奖励
	 */
	virtual RewardGroup* GetRewards() const;

	/**
	*@brief 设置脏数据
	*/
	//virtual void SetTaskMgrDirty();

protected:
	//数据项
	TaskDataEntry*	m_pDataEntry;

};

//任务访问器
typedef CLVisitor<Task>	TaskVisitor;


class DailyTask : public Task
{
	AVALON_DEFINE_OBJECTPOOL(DailyTask, Avalon::NullMutex)
public:
	DailyTask(Player* owner, UInt32 id);
	DailyTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);
	virtual ~DailyTask();

public:

	/**
	*@brief 设置数据项
	*/
	virtual bool SetupBase();

	/**
	*@brief 获取任务奖励
	*/
	virtual void GiveRewards();

	/**
	*@brief 获得奖励
	*/
	virtual RewardGroup* GetRewards() const;

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	UInt32 GetPoolId() { return m_pDataEntry->poolId; }

	DailyTaskPoolType GetPoolType() { return m_pDataEntry->poolType; }

	/**
	*@brief 初始化任务变量
	*/
	void InitVar(const char* name, Int64 val);

	/**
	*@brief 设置脏数据
	*/
	//virtual void SetTaskMgrDirty();

protected:
	RewardAdapterDataEntry* m_pRewardDataEntry;

};
typedef CLVisitor<DailyTask>	DailyTaskVisitor;

class LegendTask : public Task
{
	AVALON_DEFINE_OBJECTPOOL(LegendTask, Avalon::NullMutex)
public:
	LegendTask(Player* owner, UInt32 id);
	LegendTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt8 submitCount);

	virtual ~LegendTask();

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	/**
	*@brief 获得奖励
	*/
	virtual RewardGroup* GetRewards() const;
};

class AchievementTask :public Task
{
	AVALON_DEFINE_OBJECTPOOL(AchievementTask, Avalon::NullMutex)
public:
	AchievementTask(Player* owner, UInt32 id);
	AchievementTask(Player* owner, UInt32 id, QuestVarMap questVar);
	AchievementTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime);
	virtual ~AchievementTask();

	/**
	*@brief 获取任务奖励
	*/
	virtual void GiveRewards();

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	//virtual void OnSetStatus();

	//virtual void OnSetVar();

};

class CycleTask : public Task
{
	AVALON_DEFINE_OBJECTPOOL(CycleTask, Avalon::NullMutex)
public:
	CycleTask(Player* owner, UInt32 id);
	CycleTask(Player* owner, UInt32 id, QuestVarMap questVar);
	CycleTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);

	virtual ~CycleTask();

	virtual const char* GetRewardsReason();
};

class RedPacketTask : public AchievementTask
{
	AVALON_DEFINE_OBJECTPOOL(RedPacketTask, Avalon::NullMutex)
public:
	RedPacketTask(Player* owner, UInt32 id);
	RedPacketTask(Player* owner, UInt32 id, QuestVarMap questVar);
	RedPacketTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime);

	virtual ~RedPacketTask();

public:
	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	//virtual void OnSetStatus();

	virtual void GiveRewards();

	virtual void SyncVar(std::string key, std::string value) {}
	virtual void SyncStatus(){}
	
};


class MasterDailyTask : public Task
{
	AVALON_DEFINE_OBJECTPOOL(MasterDailyTask, Avalon::NullMutex)
public:
	MasterDailyTask(Player* owner, UInt32 id);
	MasterDailyTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);
	~MasterDailyTask();

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	/**
	*@brief 获取任务奖励
	*/
	virtual void GiveRewards();

	/**
	*@brief 处理设置状态
	*/
	virtual void OnSetStatus();

	/**
	*@brief 处理完成
	*/
	void OnFinished();

	virtual void OnSetVar();
};

class MasterAcademicGrowthTask : public Task
{
	AVALON_DEFINE_OBJECTPOOL(MasterAcademicGrowthTask, Avalon::NullMutex)
public:
	MasterAcademicGrowthTask(Player* owner, UInt32 id);
	MasterAcademicGrowthTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);
	~MasterAcademicGrowthTask();

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	/**
	*@brief 获取任务奖励
	*/
	virtual void GiveRewards();

	/**
	*@brief 处理设置状态
	*/
	virtual void OnSetStatus();

	/**
	*@brief 处理完成
	*/
	void OnFinished();

	virtual void OnSetVar();
};

#endif

