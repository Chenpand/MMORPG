#ifndef _CL_TASK_DATAENTRY_H_
#define _CL_TASK_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLTaskDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <map>

/**
 *@brief 任务配置表
 * ID	任务名称	任务类别	所属副本	前置任务ID	最小玩家等级  职业要求	任务奖励(物品ID)	接取类型	提交类型:	任务所属NPC	提交npc	 任务参数	任务开关	
 * 任务奖励的配置格式：ItemID1_Counter1,ItemID2_Counter2,ItemID3_Counter3
 */
struct TaskDataEntry : public Avalon::DataEntry
{
public:
	TaskDataEntry();
	~TaskDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);
	UInt8 GetTargetOccupation();

	UInt32 GetDailyTaskScore();

	std::vector<ItemReward> GetOccuItemRewards(Occupation occu);

	bool IsLegendOccuItemCheck(Occupation occu);

	bool CheckOccu(Occupation occu);

public:

	//任务id
	UInt32	id;
	//任务名
	std::string	name;

	//任务参数0
	UInt32 param0;

	//任务类型
	TaskType	type;
	//任务子类型
	TaskSubType subType;
	//循环任务类型
	CycleTaskType cycleType;
	//循环任务权重
	UInt32		cycleWeight;

	//随机池类型
	DailyTaskPoolType poolType;

	// 随机池编号
	UInt32		poolId;

	//所属副本
	UInt32 dungeon;
	//前置任务
	UInt32	preTask;

	//added by aprilliu,2016.05.17  标识服务器是否自动接受该任务
	UInt8 isServerTake;

	//最大等级
	UInt16	maxLevel;
	//触发等级
	UInt16	triggerLevel;
	//职业要求
	//UInt8	occupation;
	std::vector<Occupation> occus;

	//职业奖励
	std::vector<OccuItemReward> occuRewards;

	//奖励
	std::vector<ItemReward>	rewards;

	//奖励适配ID
	UInt32 rewardAdapterId;

	//接取类型
	UInt8 publishType;
	//提交类型
	UInt8 submitType;
	//任务所属NPC，也就是接取NPC
	UInt32 acceptNpc;
	//任务提交NPC
	UInt32 submitNPC;

	//是否公告
	bool isAnnouncement;

	//任务提交次数
	UInt32 submitCount;

	//活力值
	UInt32 vitality;

	//任务参数   added by aprilliu, 2016.05.25
	std::string params;

	//模板id
	UInt32 templateId;

	//脚本参数
	std::string scriptParam;

	//任务开关
	UInt8 open;

	//立即完成所需等级
	UInt16 FinishRightNowLevel;

	//立即完成所需道具类型
	UInt32 FinishRightNowItemType;

	//立即完成所需道具数量
	UInt32 FinishRightNowItemNum;


	//noted by aprilliu, 2016.04.12 , not used 
	//描述
	std::string	describe;
	//未接任务目标
	std::string	initTarget;
	//未完成任务目标
	std::string	unfinishedTarget;
	//完成任务目标
	std::string	finishedTarget;
	//限时
	UInt32	timeLimit;
	//放弃限时
	UInt32	abandomTimeLimit;
};

/**
 *@brief 任务数据项管理器
 */
class TaskDataEntryMgr : public Avalon::DataEntryMgrBase<TaskDataEntry>
	, public Avalon::Singleton<TaskDataEntryMgr >
{
public:
	typedef std::vector<UInt32> TaskIdVec;
	typedef std::map<UInt32, TaskIdVec> TaskIdVecMap;

	typedef std::vector<TaskDataEntry*>	TaskDataVec;
	typedef std::map<UInt32, TaskDataVec> TaskDataVecMap;
	typedef std::map<UInt8, TaskDataVec> DailyTaskDataMap;

	typedef std::map<UInt16, TaskDataVec> LevelTaskDataMap;

	struct DailyTaskSubPool
	{
		DailyTaskSubPool() : pooId(0) { }
		UInt32 pooId;
		std::vector<TaskDataEntry*> dailyTasks;
	};

	struct CycleTypeTasks
	{
		CycleTypeTasks() : sumWeight(0) {}

		TaskDataVec cycleTask;
		UInt32 sumWeight;
	};

	typedef std::map<UInt16, std::map<CycleTaskType, CycleTypeTasks>> CycleTaskDataMap;

	typedef std::vector<TaskDataEntry*> DailyTaskVec;
	typedef std::vector<DailyTaskSubPool> DailyTaskPool;
	typedef std::map<DailyTaskPoolType, DailyTaskPool> DailyTaskPoolMap;
	typedef std::map<UInt16, DailyTaskPoolMap> LevelPoolMap;
	typedef std::map<UInt16, DailyTaskVec> SureDailyTaskMap;


	TaskDataEntryMgr();
	~TaskDataEntryMgr();

public:
	bool AddEntry(TaskDataEntry* entry);

	/**
	 *@brief 根据等级获取可接任务列表
	 */
	const TaskDataVec& GetLvlTasks(UInt16 level) const;
	/**
	 *@brief 根据任务获取后置任务列表
	 */
	const TaskDataVec& GetPostTasks(UInt32 taskid) const;
	/**
	 *@brief 根据npc获取可接任务列表
	 */
	void GetAcceptTasks(UInt32 npcid, TaskIdVec& tasks) const;
	/**
	 *@brief 根据npc获取可提交任务列表
	 */
	void GetSubmitTasks(UInt32 npcid, TaskIdVec& tasks) const;

	//added by aprilliu, 2016.04.20 根据dungeonId得到相应的任务列表
	void GetDungeonTasks(UInt32 dungeonId, TaskIdVec& tasks) const;

	/**
	 *@brief 根据等级获得日常任务列表
	 */
	void GetDailyTasks(TaskType types, UInt16 level, TaskDataVec& tasks);

	/**
	*@brief 获得成就列表   add by huchenhui 2016.06.25
	*/
	void GetAchievementTasks(UInt16 level, TaskDataVec& tasks);

	/**
	*@brief 获得账号成就列表   add by huchenhui 2019.02.15
	*/
	void GetAccountAchievementTasks(TaskDataVec& tasks);

	/**
	*@brief 获得佣兵团任务
	*/
	void GetAllAdventureTeamTasks(TaskDataVec& tasks);
	bool RandAdventureTeamTasks(TaskDataVec& tasks, UInt32 taskGrade, UInt32 taskNum);
	bool RandAdventureTeamTasks(TaskDataVec& tasks, const std::map<UInt32, UInt32>& taskGrades);

	/**
	*@brief 根据成就获取后置任务列表   add by huchenhui 2016.07.14
	*/
	const TaskDataVec& GetPostAchievementTasks(UInt32 taskid) const;

	/**
	*@brief 获得红包任务   add by huchenhui 2016.12.16
	*/
	void GetRedPackTasks(TaskDataVec& tasks);

	/**
	*@brief 根据红包任务获取后置任务列表   add by huchenhui 2016.12.16
	*/
	const TaskDataVec& GetPostRedPackTasks(UInt32 taskId) const;

	const DailyTaskPool& GetDailyTaskPool(UInt16 level, DailyTaskPoolType type);

	const DailyTaskVec& GetSureDailyTasks(UInt16 level);

	/**
	*@brief 根据等级和类型获取循环任务列表   add by huchenhui 2016.10.28
	*/
	const CycleTypeTasks& GetCycleTasks(UInt16 level, CycleTaskType type);

	/**
	*@brief 根据等级和类型获取循环任务列表   add by huchenhui 2016.10.28
	*/
	TaskDataEntry* RandomCycleTask(UInt16 level, CycleTaskType type);

	/**
	*@brief 根据等级获取传奇之路任务列表   add by huchenhui 2017.08.22
	*/
	const TaskDataVec& GetLegendTasks(UInt16 level);

	/**
	*@brief 获取师门日常任务列表
	*/
	const TaskDataVec& GetMasterDailyTasks();

	/**
	*@brief 获取师门日常任务
	*/
	const  TaskDataEntry* GetMasterDailyTask(UInt32 key);

	/**
	*@brief 获取师门学业成长任务列表
	*/
	const TaskDataVec& GetMasterAcademicGrowthTasks();
	
	/**
	*@brief 获取师门学业成长任务
	*/
	const TaskDataEntry* GetMasterAcademicGrowthTask(UInt32 key);
private:
	void OnAddEntry(TaskDataEntry* entry);

private:
	//等级任务map 表示要求某等级的任务列表
	TaskDataVecMap	m_LvlTasks;
	//后置任务map  表示某个任务的后置任务列表（也就是该任务是某些任务的前置任务）
	TaskDataVecMap	m_PostTasks;
	//npc-可接任务map  表示可以在某个NPC身上 可接受的任务列表
	TaskIdVecMap	m_AcceptTasks;
	//npc-可交任务map  可在某个npc上交接的任务列表
	TaskIdVecMap	m_SubmitTasks;

	//副本拥有的任务列表 added by aprilliu, 2016.04.12
	TaskIdVecMap m_DungeonTasks;

	//日常任务map   某类型的日常任务列表
	DailyTaskDataMap	m_DailyTasks;

	//成就map		表示成就列表
	TaskDataVec		m_AchievementTasks;

	//成就后置任务map	表示某个成就的后置任务列表
	TaskDataVecMap	m_PostAchievementTask;

	//随机池
	LevelPoolMap m_PoolMap;

	//必出的日常
	SureDailyTaskMap m_SureDailyTasks;

	//根据等级的循环列表
	CycleTaskDataMap	m_LevelCycleTasks;

	//红包任务
	TaskDataVec			m_RedPacketTasks;
	//红包任务后置任务map
	TaskDataVecMap		m_PostRedPacketTasks;

	//根据等级的传奇之路列表
	LevelTaskDataMap	m_LevelLegendTasks;

	//师门日常任务
	TaskDataVec			m_MasterDailyTasks;
	//师门学业成长任务
	TaskDataVec			m_MasterAcademicGrowthTasks;

	//账号成就
	TaskDataVec			m_AccountAchievementTasks;

	//所有冒险队(佣兵团)任务
	TaskDataVec			m_AllAdventureTeamTasks;
	//冒险队(佣兵团)C级任务
	TaskDataVec			m_AdventureTeamCGTasks;
	//冒险队(佣兵团)B级任务
	TaskDataVec			m_AdventureTeamBGTasks;
	//冒险队(佣兵团)A级任务
	TaskDataVec			m_AdventureTeamAGTasks;
	//冒险队(佣兵团)S级任务
	TaskDataVec			m_AdventureTeamSGTasks;
	//冒险队(佣兵团)SS级任务
	TaskDataVec			m_AdventureTeamSSGTasks;
	//冒险队(佣兵团)SSS级任务
	TaskDataVec			m_AdventureTeamSSSGTasks;
};

/**
 *@brief 日常任务表
 */
struct DailyTaskDataEntry : public Avalon::DataEntry
{
public:
	DailyTaskDataEntry();
	~DailyTaskDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:

	//ID
	UInt32		id;

	//随机池类型
	DailyTaskPoolType type;

	// 随机池编号
	UInt32		poolId;

	//taskDataEntry
	TaskDataEntry* taskDataEntry;

};

/**
 *@brief 日常任务数据项管理器
 */
class DailyTaskDataEntryMgr : public Avalon::DataEntryMgrBase<DailyTaskDataEntry>, public Avalon::Singleton<DailyTaskDataEntryMgr>
{
public:
	DailyTaskDataEntryMgr();
	~DailyTaskDataEntryMgr();

public:
	bool AddEntry(DailyTaskDataEntry* entry);

private:

};

struct DailyTaskScoreRewardDataEntry : public Avalon::DataEntry
{
public:
	DailyTaskScoreRewardDataEntry();
	~DailyTaskScoreRewardDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;

	//积分
	UInt32 score;

	//奖励物品
	std::vector<ItemReward> itemReward;

};

typedef Avalon::DataEntryMgr<DailyTaskScoreRewardDataEntry> DailyTaskScoreRewardDataEntryMgr;


class AchievementLevelDataEntry : public Avalon::DataEntry
{
public:
	AchievementLevelDataEntry();
	~AchievementLevelDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;

	//最小积分
	UInt32 minScore;
	//最大积分
	UInt32 maxScore;

	//奖励
	std::vector<ItemReward> itemReward;
};

typedef Avalon::DataEntryMgr<AchievementLevelDataEntry> AchievementLevelDataEntryMgr;


class WeekSignSpringDataEntry : public Avalon::DataEntry
{
public:
	WeekSignSpringDataEntry();
	virtual ~WeekSignSpringDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// 开始等级
	UInt32                                          startLv;
	// 结束等级
	UInt32                                          endLv;
	// 地下城tag
	std::vector<UInt32>                             dungeonTag;


};

class WeekSignSpringDataEntryMgr : public Avalon::DataEntryMgrBase<WeekSignSpringDataEntry>
	, public Avalon::Singleton<WeekSignSpringDataEntryMgr>
{
public:
	virtual bool AddEntry(WeekSignSpringDataEntry* entry);
	bool IsInScope(UInt32 lv, UInt32 tag);
};

#endif
