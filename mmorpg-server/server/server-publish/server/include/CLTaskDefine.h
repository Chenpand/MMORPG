#ifndef _CL_TASK_DEFINE_H_
#define _CL_TASK_DEFINE_H_

#include <AvalonNetStream.h>
#include "CLDefine.h"
#include "CLItemDefine.h"

#define DAILY_TASK_REWARD_SIZE 64
#define ACHIEVEMENT_TASK_REWARD_SIZE 64

/**
 *@brief ���ɽ�������
 */
#define MAX_ACCEPT_TASK_NUM 25
#define MAX_ACCEPT_ACHIEVEMENT_NUM 1000
#define MAX_ACCEPT_LEGEND_NUM 1000

/**
 *@brief ���ɽ�����������
 */
#define	MAX_ACCEPT_XUANSHANG_TASK_NUM 2


 /**
  *@brief ��������
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
 *@brief ��������
 */
enum TaskType
{
	//TASK_INVALID = 0,
	TASK_DAILY = 0,						//ÿ������
	TASK_MAIN = 1,						//��������
	TASK_BRANCH = 2,					//֧������
	TASK_ACHIEVEMENT = 3,				//�ɾ�
	TASK_SYSTEM = 4,					//ϵͳ
	TASK_ACTIVITY = 5,					//�
	TASK_EXTENTION = 6,					//�⴫
	TASK_TRANSFER = 7,					//תְ����
	TASK_AWAKEN = 8,					//��������
	TASK_CYCLE = 9,						//ѭ������
	TASK_RED_PACKET = 10,				//�������
	TASK_TITLE = 11,					//�ƺ�����
	TASK_LEGEND = 12,					//��������
	TASK_MASTER_DAILY = 13,				//ʦ���ճ�����
	TASK_MASTER_ACADEMIC = 14,			//ʦ��ѧҵ�ɳ�����
	TASK_ACCOUNT_ACHIEVEMENT = 15,		//�˺ųɾ�
	TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY = 16, //ð�ն�(Ӷ����)�˺�ÿ������

	//TASK_ZHUXIE = 3,	//��а������
	//TASK_XUANSHANG = 4,	//��������
	//TASK_ESCORT = 5,	//��������
	//TASK_CHALLENGE = 6,	//������ս
	//TASK_TRIBE = 7,		//��������
	//TASK_LOOP = 8,		//�����ķ�
	//TASK_INNWORK = 9,	//��ջ��
	//TASK_QUESTION = 10,	//��������
	//TASK_DARKTOWER = 11,	//�ڰ�������

	TASK_MAX,
};

enum TaskSubType
{
	TST_INVALID = 0,	//
	TST_DAILY_TASK,		//ÿ������
	TST_DAILY_PROVE,	//ÿ������
	TST_DUNGEON_MISSION,	//�½�����
	TST_DUNGEON_CHEST,		//�½ڱ���
	TST_CHAPTER_CHANGE,		// �½ڹ���
	TST_COMBO_GUIDE,		// ���н�ѧ
	TST_SUMMON_NPC_TASK,	// �ٻ�npc����
	TST_CLEAR_DUNGEON,		// ͨ������
	TST_CLEAR_CITYMONSTER,	// ���﹥������
	TST_COND_ROLE_NUM_CHANGE,// ������ɫ�����ı�����
};

//�ճ��������������		//added by huchenhui 2016.09.02
enum DailyTaskPoolType
{
	DAILY_POOL_INVALID = 0,		//��Ч

	DAILY_POOL_SURE = 1,		//�س�
	DAILY_POOL_DUNGEON = 2,		//�������
	DAILY_POOL_FUNCTION = 3,	//�������
	DAILY_POOL_ACTIVITY = 4,	//����

	DAILY_POOL_NUM,
};

enum CycleTaskType
{
	CTT_INVALID = 0,	//��Ч
	CTT_DUNGEON = 1,	//�������
	CTT_SET_ITEM = 2,	//�Ͻ�����
	CTT_STORY = 3,		//�Ի�

	CTT_MAX
};

inline bool IsValidDailyTask(DailyTaskPoolType poolType)
{
	return (poolType > DAILY_POOL_INVALID) && (poolType < DAILY_POOL_NUM);
}

//�ճ��������ѡ����.
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
*@brief �ж��Ƿ�Ϊ�ɾ�
*/
inline bool IsAchievementTask(TaskType type)
{
	return type == TASK_ACHIEVEMENT;
}

/**
*@brief �ж��Ƿ�Ϊ�˺�����
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
*@brief �ж��Ƿ�Ϊ�˺�ÿ������
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
*@brief �ж��Ƿ�Ϊ�˺ųɾ�
*/
inline bool IsAccountAchievementTask(TaskType type)
{
	return type == TASK_ACCOUNT_ACHIEVEMENT;
}

/**
*@brief �ж��Ƿ�ΪӶ��������
*/
inline bool IsAdventureTeamTask(TaskType type)
{
	return type == TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY;
}

/**
*@brief �ж��Ƿ�Ϊ�������
*/
inline bool IsRedPacketTask(TaskType type)
{
	return type == TASK_RED_PACKET;
}

/**
 *@brief �ж��Ƿ�Ϊ�ճ�����
 */
inline bool IsDailyTask(TaskType type)
{
	return (type == TASK_DAILY);
}

/**
*@brief �ж��Ƿ�Ϊѭ������
*/
inline bool IsCycleTask(TaskType type)
{
	return (type == TASK_CYCLE);
}

/**
*@brief �ж��Ƿ�Ϊ֧������
*/
inline bool IsBranchTask(TaskType type)
{
	return (type == TASK_BRANCH);
}

/**
*@brief �ж��Ƿ�Ϊ�ƺ�����
*/
inline bool IsTitleTask(TaskType type)
{
	return (type == TASK_TITLE);
}

/**
*@brief �ж��Ƿ�Ϊ����֮·
*/
inline bool IsLegendTask(TaskType type)
{
	return (type == TASK_LEGEND);
}

/**
*@brief �ж��Ƿ�Ϊʦ������
*/
inline bool IsMasterTask(TaskType type)
{
	return (type == TASK_MASTER_DAILY || type == TASK_MASTER_ACADEMIC);
}

/**
*@brief �ж��Ƿ�Ϊʦ���ճ�����
*/
inline bool IsMasterDailyTask(TaskType type)
{
	return (type == TASK_MASTER_DAILY);
}

/**
*@brief �ж��Ƿ�Ϊʦ�ųɳ�����
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
*@brief �ж��Ƿ���Ҫ��������
*/
inline bool IsLimitNumberTask(TaskType type)
{
	return (type == TASK_MAIN || type == TASK_BRANCH);
}

/**
 *@brief �ж��Ƿ���������
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
 *@brief һ�������������������
 */
#define MAX_TYPE_TASKNUM 1000

/**
 *@brief ����״̬
 */
enum TaskStatus
{
	TASK_INIT = 0,		//��ʼ״̬
	TASK_UNFINISH = 1,	//�Ѿ�������
	TASK_FINISHED = 2,  //����ɣ�δ�ύ
	TASK_FAILED = 3,	//ʧ��

	//added by aprilliu, 2016.04.19  ���������ύ��״̬��Ϊ�˽���������ύ����ʱ ɾ�������¼�������İ�����״̬���ܴӡ������δ�ύ���ı�Ϊ��δ���״̬��
	TASK_SUBMITTING = 4,	//�����ύ�У�����ɲ��������ύ��)

	//added by huchenhui, 2016.06.22 ���������,���ύ״̬, �ճ���������Ժ󲢲���ɾ��,���Ǳ�ʶ�Ѿ����.
	TASK_OVER = 5,		//�����,���ύ
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
 *@brief npc����״̬
 */
enum NpcTaskStatus
{
	NPC_TASK_NONE = 0,		//������
	NPC_TASK_PUBLISH = 1,	//�пɽ�����
	NPC_TASK_UNFINISH = 2,	//���ѽ�����
	NPC_TASK_SUBMIT = 3,	//�пɽ�����
};

/**
 *@brief npc��������
 */
enum TaskPublishType
{
	TASK_PUBLISH_AUTO = 0,		//�Զ���ȡ
	TASK_PUBLISH_NPC =  1,		//npc����
	TASK_PUBLISH_UI	=	2,		//�����ȡ


	TASK_PUBLISH_ZONE = 3,	//���򴥷�, noted by aprilliu, ���� 2016.4.9
};

//added by aprilliu, 2016.4.9
enum TaskSubmitType
{
	TASK_SUBMIT_AUTO			=		0,		// �Զ��ύ��û���ύnpc
	TASK_SUBMIT_NPC				=		1,		// npc�ύ
	TASK_SUBMIT_UI				=		2,		// �����ύ������û���ύnpc������������������ύ�����npc������npc�ύ�������ǽ����ύ
	TASK_SUBMIT_RIGHTNOW        =       3,      // �����ύ�����Կ۵���ֱ���������
};

enum DeleteTaskReason
{
	DELETE_TASK_REASON_SUBMIT		=		1,			//�ύ�������
	DELETE_TASK_REASON_ABANDON		=		2,			//��������
	DELETE_TASK_REASON_SYSTEM		=		3,			//ϵͳɾ��
	DELETE_TASK_REASON_OTHER		=		4,			//����ԭ��
};

/**
 *@brief ������Ϣ
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
	UInt32	id;									//������Դid
	UInt8	status;								//����״̬ enum TaskStatus					
	std::map<std::string, std::string> vars;	//�������
	UInt32	finTime;							//���ʱ��
	UInt8	submitCount;						//���������ύ����
};

struct TaskInfo  //noted by aprilliu: ע���Ѿ��¶�����TaskBriefInfo,�ýṹ����ʹ�����������񣬺����Ƿ�����������ȷ��
{
public:
	TaskInfo(){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & quality & status  & name & describe & target;
		stream & rewards & vars;
	}

public:
	//����id
	UInt32	id;
	//��������
	UInt8	type;
	//����Ʒ��
	UInt8	quality;
	//����״̬
	UInt8	status;
	//������
	std::string name;
	//��������
	std::string describe;
	//��ǰ״̬����Ŀ��
	std::string target;
	//����
	std::vector<ItemReward>  rewards;
	//�������
	std::map<std::string, std::string> vars;
};


/**
 *@brief ����Ʒ������
 */
struct TaskQualityCfg
{
	//���ˢ�¼���, �ٷֱ�
	UInt32	freeRefreshRate;
	//����ˢ�¼��ʣ��ٷֱ�
	UInt32	itemRefreshRate;
	//���齱������
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
 *@brief �ճ����������Ϣ
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

//������Ʒ

struct TaskItem
{
public:
	TaskItem() : itemId(0), num(0){}

public:
	UInt64 itemId;
	UInt32 num;
};

/**
 *@brief ����ľ������
 */
#define TRANS_WOOD_TASKID 4000

/**
 *@brief ��������id
 */
#define LOOP_TASKID 4600

/**
 *@brief ������������id
 */
#define TRIBE_TRANS_TASKID 4002



//��ʱ������ʱ�����
#define TASK_DEADLINE_VAR		"DEADLINE"
 //���������ȡʱ�ȼ��������
#define TASK_ACCEPT_LEVEL_VAR	"ACCEPTLEVEL"
//�ɷ��������ʱ��
#define TASK_ABANDOM_TIME_VAR	"ABANDONTIME"
//�Ƿ�������
#define TASK_ISESCORT_VAR		"ISESCORT"
//�������ʱ��
#define TASK_ACCEPTTIME_VAL		"ACCEPTTIME"

#define TASK_DAILY_LEVEL_VAR	"DAILYLEVEL"

/**
 *@brief ����id���ͻ����滻Ϊ����
 */
#define TASK_SCENE_ID "SCENEID"

/**
 *@brief npc id���ͻ����滻Ϊ����
 */
#define TASK_NPC_ID "NPCID"


/**
 *@brief �ٳɷ�
 */
#define QUICK_SUBMIT_TASK_ITEM 58

/**
 *@brief ������
 */
#define XUANSHANG_TOKEN 59

/**
*@brief ˢ���������������
*/
#define REFRESH_XUANSHANG_COST 2000

/**
*@brief ���ˢ�����͵ļ��(��)
*/
#define FREE_REFRESH_XUANSHANG_INTERVAL	600

/**
*@brief ѭ��������
*/
#define CYCLE_SUM_ODDS			10000

/**
*@brief ѭ��������³�������
*/
#define CYCLE_DUNGEON_ODDS		2000

/**
*@brief ѭ�������Ͻ�����������
*/
#define CYCLE_SET_ITEM_ODDS		4000

/**
*@brief ѭ������Ի�������
*/
#define CYCLE_STORY_ODDS		4000

/**
*@brief ʦ��������������
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
	UInt32 m_AcademicTotalGrowth;       //ʦ�ųɳ���ֵ
	UInt32 m_MasterDailyTaskGrowth;     //ʦ���ճ�����ɳ���ֵ
	UInt32 m_MasterAcademicTaskGrowth;  //ʦ��ѧҵ�ɳ���ֵ
	UInt32 m_MasterUplevelGrowth;		//ʦ�������ɳ���ֵ
	UInt32 m_MasterGiveEquipGrowth;     //ʦ������װ���ɳ���ֵ
	UInt32 m_MasterGiveGiftGrowth;		//ʦ����������ɳ���ֵ
	UInt32 m_MasterTeamClearDungeonGrowth; //ʦͽ���ͨ�����³ǳɳ�ֵ
	UInt32 m_GoodTeachValue;			//ʦ����ʦֵ

	std::vector<TaskBriefInfo> m_MasterDailyTasks;
	std::vector<TaskBriefInfo> m_MasterAcademicTasks;
};

// ð�ն�(Ӷ����)���񼶱�
enum AdventureTeamTaskGrade
{
	// C��
	ADTT_GRADE_C = 0,
	// B��
	ADTT_GRADE_A = 1,
	// A��
	ADTT_GRADE_B = 2,
	// S��
	ADTT_GRADE_S = 3,
	// SS��
	ADTT_GRADE_SS = 4,
	// SSS��
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
