#ifndef _CL_TASK_DEFINE_H_
#define _CL_TASK_DEFINE_H_

#include <AvalonNetStream.h>
#include "CLDefine.h"
#include "CLItemDefine.h"

#define DAILY_TASK_REWARD_SIZE 64
#define ACHIEVEMENT_TASK_REWARD_SIZE 64

/**
 *@brief 最大可接任务数
 */
#define MAX_ACCEPT_TASK_NUM 25
#define MAX_ACCEPT_ACHIEVEMENT_NUM 1000
#define MAX_ACCEPT_LEGEND_NUM 1000

/**
 *@brief 最大可接悬赏任务数
 */
#define	MAX_ACCEPT_XUANSHANG_TASK_NUM 2


 /**
  *@brief 任务类型
  */
enum TaskEventType
{
	TET_EQUIP_REMAKE_EVENT = 0,
	TET_GET_ITEM_EVENT = 1,
	TET_STKILL_NPC_EVENT = 3,
	TET_KILL_CITY_MONSTER_EVENT = 4,
	TET_STRENGTHEN_EQUIP_EVENT = 5,
	TET_PUT_EQUIP_EVENT = 6,
	TET_PUT_FASHION_EVENT = 7,
	TET_CLEAR_DUNGEON_EVENT = 8,
	TET_DEATH_TOWER_EVENT = 9,
	TET_WARP_STONE_UP_LEVEL_EVENT = 10,
	TET_PVP_EVENT = 11,
	TET_RETINUE_CHANGE_SKILL_EVENT = 12,
	TET_RETINUE_UP_LEVEL_EVENT = 13,
	TET_SET_TASK_ITEM_EVENT = 14,
	TET_ACTIVE_TASK_EVENT = 15,
	TET_SHOP_BUY_EVENT = 16,
	TET_OPEN_MAGIC_JAR_EVENT = 17,
	TET_FEED_PET_EVENT = 18,
	TET_ADD_PET_EVENT = 19,
	TET_PET_UP_LEVEL_EVENT = 20,
	TET_RELATION_EVENT = 21,
	TET_EQUIT_SCORE_EVENT = 22,
	TET_USE_ITEM,
	TET_CHANGE_MONEY,
	TET_MOVED,
	TET_MAX,
};



/**
 *@brief 任务类型
 */
enum TaskType
{
	//TASK_INVALID = 0,
	TASK_DAILY = 0,						//每日任务
	TASK_MAIN = 1,						//主线任务
	TASK_BRANCH = 2,					//支线任务
	TASK_ACHIEVEMENT = 3,				//成就
	TASK_SYSTEM = 4,					//系统
	TASK_ACTIVITY = 5,					//活动
	TASK_EXTENTION = 6,					//外传
	TASK_TRANSFER = 7,					//转职任务
	TASK_AWAKEN = 8,					//觉醒任务
	TASK_CYCLE = 9,						//循环任务
	TASK_RED_PACKET = 10,				//红包任务
	TASK_TITLE = 11,					//称号任务
	TASK_LEGEND = 12,					//传奇任务
	TASK_MASTER_DAILY = 13,				//师门日常任务
	TASK_MASTER_ACADEMIC = 14,			//师门学业成长任务
	TASK_ACCOUNT_ACHIEVEMENT = 15,		//账号成就
	TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY = 16, //冒险队(佣兵团)账号每周任务

	//TASK_ZHUXIE = 3,	//诛邪令任务
	//TASK_XUANSHANG = 4,	//悬赏任务
	//TASK_ESCORT = 5,	//运镖任务
	//TASK_CHALLENGE = 6,	//名人挑战
	//TASK_TRIBE = 7,		//部落任务
	//TASK_LOOP = 8,		//安抚四方
	//TASK_INNWORK = 9,	//客栈打工
	//TASK_QUESTION = 10,	//答题任务
	//TASK_DARKTOWER = 11,	//黑暗塔任务

	TASK_MAX,
};

enum TaskSubType
{
	TST_INVALID = 0,	//
	TST_DAILY_TASK,		//每日任务
	TST_DAILY_PROVE,	//每日试炼
	TST_DUNGEON_MISSION,	//章节任务
	TST_DUNGEON_CHEST,		//章节宝箱
	TST_CHAPTER_CHANGE,		// 章节过场
	TST_COMBO_GUIDE,		// 连招教学
	TST_SUMMON_NPC_TASK,	// 召唤npc任务
	TST_CLEAR_DUNGEON,		// 通关任务
	TST_CLEAR_CITYMONSTER,	// 怪物攻城任务
	TST_COND_ROLE_NUM_CHANGE,// 条件角色数量改变任务
};

//日常任务随机池类型		//added by huchenhui 2016.09.02
enum DailyTaskPoolType
{
	DAILY_POOL_INVALID = 0,		//无效

	DAILY_POOL_SURE = 1,		//必出
	DAILY_POOL_DUNGEON = 2,		//副本相关
	DAILY_POOL_FUNCTION = 3,	//功能相关
	DAILY_POOL_ACTIVITY = 4,	//活动相关

	DAILY_POOL_NUM,
};

enum CycleTaskType
{
	CTT_INVALID = 0,	//无效
	CTT_DUNGEON = 1,	//副本相关
	CTT_SET_ITEM = 2,	//上交物资
	CTT_STORY = 3,		//对话

	CTT_MAX
};

inline bool IsValidDailyTask(DailyTaskPoolType poolType)
{
	return (poolType > DAILY_POOL_INVALID) && (poolType < DAILY_POOL_NUM);
}

//日常任务随机选择器.
struct DailyTaskSelector
{
	DailyTaskSelector() :type(DAILY_POOL_INVALID), num(0) { }
	
	DailyTaskPoolType type;
	UInt32 num;

};

inline bool IsNotAbandonTask(TaskType type)
{
	switch (type)
	{
	case TASK_TRANSFER:
	case TASK_AWAKEN:
	case TASK_DAILY:
	case TASK_ACHIEVEMENT:
	case TASK_RED_PACKET:
	case TASK_MAIN:
		return true;
	default:
		return false;
	}
	return false;
}

/**
*@brief 判断是否为成就
*/
inline bool IsAchievementTask(TaskType type)
{
	return type == TASK_ACHIEVEMENT;
}

/**
*@brief 判断是否为账号任务
*/
inline bool IsAccountTask(TaskType type)
{
	switch (type)
	{
	case TASK_ACCOUNT_ACHIEVEMENT:
	case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
		return true;

	default:
		return false;
	}
}

/**
*@brief 判断是否为账号每周任务
*/
inline bool IsAccountWeeklyTask(TaskType type)
{
	switch (type)
	{
	case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
		return true;

	default:
		return false;
	}
}

/**
*@brief 判断是否为账号成就
*/
inline bool IsAccountAchievementTask(TaskType type)
{
	return type == TASK_ACCOUNT_ACHIEVEMENT;
}

/**
*@brief 判断是否为佣兵团任务
*/
inline bool IsAdventureTeamTask(TaskType type)
{
	return type == TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY;
}

/**
*@brief 判断是否为红包任务
*/
inline bool IsRedPacketTask(TaskType type)
{
	return type == TASK_RED_PACKET;
}

/**
 *@brief 判断是否为日常任务
 */
inline bool IsDailyTask(TaskType type)
{
	return (type == TASK_DAILY);
}

/**
*@brief 判断是否为循环任务
*/
inline bool IsCycleTask(TaskType type)
{
	return (type == TASK_CYCLE);
}

/**
*@brief 判断是否为支线任务
*/
inline bool IsBranchTask(TaskType type)
{
	return (type == TASK_BRANCH);
}

/**
*@brief 判断是否为称号任务
*/
inline bool IsTitleTask(TaskType type)
{
	return (type == TASK_TITLE);
}

/**
*@brief 判断是否为传奇之路
*/
inline bool IsLegendTask(TaskType type)
{
	return (type == TASK_LEGEND);
}

/**
*@brief 判断是否为师门任务
*/
inline bool IsMasterTask(TaskType type)
{
	return (type == TASK_MASTER_DAILY || type == TASK_MASTER_ACADEMIC);
}

/**
*@brief 判断是否为师门日常任务
*/
inline bool IsMasterDailyTask(TaskType type)
{
	return (type == TASK_MASTER_DAILY);
}

/**
*@brief 判断是否为师门成长任务
*/
inline bool IsMasteAcademicTask(TaskType type)
{
	return (type == TASK_MASTER_ACADEMIC);
}


inline bool IsServerTake(TaskType type)
{
	return (type == TASK_MAIN || type == TASK_BRANCH || type == TASK_TITLE);
}

/**
*@brief 判断是否需要限制数量
*/
inline bool IsLimitNumberTask(TaskType type)
{
	return (type == TASK_MAIN || type == TASK_BRANCH);
}

/**
 *@brief 判断是否可立即完成
 */
inline bool IsQuickSubmittable(TaskType type)
{
	/*
	switch(type)
	{
	case TASK_TRIBE:
	case TASK_INNWORK:
	case TASK_LOOP:
	case TASK_CHALLENGE:
		return true;
	}
	*/
	return false;
}

/**
 *@brief 一个类型任务最大任务数
 */
#define MAX_TYPE_TASKNUM 1000

/**
 *@brief 任务状态
 */
enum TaskStatus
{
	TASK_INIT = 0,		//初始状态
	TASK_UNFINISH = 1,	//已经接任务
	TASK_FINISHED = 2,  //已完成，未提交
	TASK_FAILED = 3,	//失败

	//added by aprilliu, 2016.04.19  增加正在提交中状态，为了解决由于在提交任务时 删除道具事件而引起的把任务状态可能从“已完成未提交”改变为“未完成状态”
	TASK_SUBMITTING = 4,	//正在提交中（已完成并且正在提交中)

	//added by huchenhui, 2016.06.22 增加已完成,以提交状态, 日常任务完成以后并不会删除,而是标识已经完成.
	TASK_OVER = 5,		//已完成,已提交
};

inline bool IsAchievementStatus(TaskType type, TaskStatus status)
{
	if (IsAchievementTask(type))
	{
		switch (status)
		{
		case TASK_UNFINISH:
		case TASK_FINISHED:
		case TASK_SUBMITTING:
			return true;
		default:
			return false;
		}
	}
	return false;
}

/**
 *@brief npc任务状态
 */
enum NpcTaskStatus
{
	NPC_TASK_NONE = 0,		//无任务
	NPC_TASK_PUBLISH = 1,	//有可接任务
	NPC_TASK_UNFINISH = 2,	//有已接任务
	NPC_TASK_SUBMIT = 3,	//有可交任务
};

/**
 *@brief npc发布类型
 */
enum TaskPublishType
{
	TASK_PUBLISH_AUTO = 0,		//自动接取
	TASK_PUBLISH_NPC =  1,		//npc触发
	TASK_PUBLISH_UI	=	2,		//界面接取


	TASK_PUBLISH_ZONE = 3,	//区域触发, noted by aprilliu, 废弃 2016.4.9
};

//added by aprilliu, 2016.4.9
enum TaskSubmitType
{
	TASK_SUBMIT_AUTO			=		0,		// 自动提交，没有提交npc
	TASK_SUBMIT_NPC				=		1,		// npc提交
	TASK_SUBMIT_UI				=		2,		// 界面提交，可能没有提交npc，但如果任务配置有提交任务的npc，就是npc提交，而不是界面提交
	TASK_SUBMIT_RIGHTNOW        =       3,      // 立即提交，可以扣道具直接完成任务
};

enum DeleteTaskReason
{
	DELETE_TASK_REASON_SUBMIT		=		1,			//提交完成任务
	DELETE_TASK_REASON_ABANDON		=		2,			//放弃任务
	DELETE_TASK_REASON_SYSTEM		=		3,			//系统删除
	DELETE_TASK_REASON_OTHER		=		4,			//其他原因
};

/**
 *@brief 任务信息
 */
struct TaskBriefInfo    //added by aprilliu, 2016.04.09
{
public:
	TaskBriefInfo()
	{
		finTime = 0;
		submitCount = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & status & vars & finTime & submitCount;
	}

public:
	UInt32	id;									//任务资源id
	UInt8	status;								//任务状态 enum TaskStatus					
	std::map<std::string, std::string> vars;	//任务变量
	UInt32	finTime;							//完成时间
	UInt8	submitCount;						//当日任务提交次数
};

struct TaskInfo  //noted by aprilliu: 注意已经新定义了TaskBriefInfo,该结构还在使用于悬赏任务，后续是否有悬赏任务不确定
{
public:
	TaskInfo(){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & quality & status  & name & describe & target;
		stream & rewards & vars;
	}

public:
	//任务id
	UInt32	id;
	//任务类型
	UInt8	type;
	//任务品质
	UInt8	quality;
	//任务状态
	UInt8	status;
	//任务名
	std::string name;
	//任务描述
	std::string describe;
	//当前状态任务目标
	std::string target;
	//奖励
	std::vector<ItemReward>  rewards;
	//任务变量
	std::map<std::string, std::string> vars;
};


/**
 *@brief 任务品质配置
 */
struct TaskQualityCfg
{
	//免费刷新几率, 百分比
	UInt32	freeRefreshRate;
	//道具刷新几率，百分比
	UInt32	itemRefreshRate;
	//经验奖励基数
	Real32	expFactor;
};

const static TaskQualityCfg TASK_QUALITY_CFG[] = 
	{
		{3675,	1780,	2.f},
		{4000,	4000,	3.f},
		{2000,	3000,	4.5f},
		{300,	1000,	6.f},
		{25,	220,	8.f}
	};

/**
 *@brief 日常任务次数信息
 */
struct DailyTaskTimes
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & times & maxTimes;
	}

	UInt16 times;
	UInt16 maxTimes;
};

//任务物品

struct TaskItem
{
public:
	TaskItem() : itemId(0), num(0){}

public:
	UInt64 itemId;
	UInt32 num;
};

/**
 *@brief 运送木材任务
 */
#define TRANS_WOOD_TASKID 4000

/**
 *@brief 连环任务id
 */
#define LOOP_TASKID 4600

/**
 *@brief 部落运送任务id
 */
#define TRIBE_TRANS_TASKID 4002



//限时任务到期时间变量
#define TASK_DEADLINE_VAR		"DEADLINE"
 //护送任务接取时等级任务变量
#define TASK_ACCEPT_LEVEL_VAR	"ACCEPTLEVEL"
//可放弃任务的时间
#define TASK_ABANDOM_TIME_VAR	"ABANDONTIME"
//是否护送任务
#define TASK_ISESCORT_VAR		"ISESCORT"
//任务接受时间
#define TASK_ACCEPTTIME_VAL		"ACCEPTTIME"

#define TASK_DAILY_LEVEL_VAR	"DAILYLEVEL"

/**
 *@brief 场景id，客户端替换为名字
 */
#define TASK_SCENE_ID "SCENEID"

/**
 *@brief npc id，客户端替换为名字
 */
#define TASK_NPC_ID "NPCID"


/**
 *@brief 速成符
 */
#define QUICK_SUBMIT_TASK_ITEM 58

/**
 *@brief 悬赏令
 */
#define XUANSHANG_TOKEN 59

/**
*@brief 刷新悬赏任务的消耗
*/
#define REFRESH_XUANSHANG_COST 2000

/**
*@brief 免费刷新悬赏的间隔(秒)
*/
#define FREE_REFRESH_XUANSHANG_INTERVAL	600

/**
*@brief 循环任务几率
*/
#define CYCLE_SUM_ODDS			10000

/**
*@brief 循环任务地下城任务几率
*/
#define CYCLE_DUNGEON_ODDS		2000

/**
*@brief 循环任务上交物资任务几率
*/
#define CYCLE_SET_ITEM_ODDS		4000

/**
*@brief 循环任务对话任务几率
*/
#define CYCLE_STORY_ODDS		4000

/**
*@brief 师门任务离线数据
*/
struct  MasterTaskShareData
{
public:
	MasterTaskShareData()
	{
		m_AcademicTotalGrowth = 0;
		m_MasterDailyTaskGrowth = 0;
		m_MasterAcademicTaskGrowth = 0;
		m_MasterUplevelGrowth = 0;
		m_MasterGiveEquipGrowth = 0;
		m_MasterGiveGiftGrowth = 0;
		m_MasterTeamClearDungeonGrowth = 0;
		m_GoodTeachValue = 0;
	}
	
	void clear()
	{
		m_AcademicTotalGrowth = 0;
		m_MasterDailyTaskGrowth = 0;
		m_MasterAcademicTaskGrowth = 0,
		m_MasterUplevelGrowth = 0;
		m_MasterGiveEquipGrowth = 0;
		m_MasterGiveGiftGrowth = 0;
		m_MasterTeamClearDungeonGrowth = 0;
		m_GoodTeachValue = 0;
		m_MasterDailyTasks.clear();
		m_MasterAcademicTasks.clear();
	}
	
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_AcademicTotalGrowth & m_MasterDailyTaskGrowth
			& m_MasterAcademicTaskGrowth & m_MasterUplevelGrowth
			& m_MasterGiveEquipGrowth & m_MasterGiveGiftGrowth
			& m_MasterTeamClearDungeonGrowth & m_GoodTeachValue
			& m_MasterDailyTasks & m_MasterAcademicTasks ;
	}
	UInt32 m_AcademicTotalGrowth;       //师门成长总值
	UInt32 m_MasterDailyTaskGrowth;     //师门日常任务成长总值
	UInt32 m_MasterAcademicTaskGrowth;  //师门学业成长总值
	UInt32 m_MasterUplevelGrowth;		//师门升级成长总值
	UInt32 m_MasterGiveEquipGrowth;     //师门赠送装备成长总值
	UInt32 m_MasterGiveGiftGrowth;		//师门赠送礼包成长总值
	UInt32 m_MasterTeamClearDungeonGrowth; //师徒组队通过地下城成长值
	UInt32 m_GoodTeachValue;			//师门良师值

	std::vector<TaskBriefInfo> m_MasterDailyTasks;
	std::vector<TaskBriefInfo> m_MasterAcademicTasks;
};

// 冒险队(佣兵团)任务级别
enum AdventureTeamTaskGrade
{
	// C级
	ADTT_GRADE_C = 0,
	// B级
	ADTT_GRADE_A = 1,
	// A级
	ADTT_GRADE_B = 2,
	// S级
	ADTT_GRADE_S = 3,
	// SS级
	ADTT_GRADE_SS = 4,
	// SSS级
	ADTT_GRADE_SSS = 5,
};

struct WorldSetTaskItemStruct
{
	WorldSetTaskItemStruct()
		:itemGuid(0), itemId(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemGuid & itemId;
	}
public:
	UInt64	itemGuid;
	UInt32	itemId;
};

#endif
