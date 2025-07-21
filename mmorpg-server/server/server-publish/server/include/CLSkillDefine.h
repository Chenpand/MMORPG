#ifndef _CL_SKILL_DEFINE_H_
#define _CL_SKILL_DEFINE_H_

#include "CLDefine.h"
#include <AvalonNetStream.h>

/**
 *@brief 技能最大页数
 */
#define CL_MAX_SKILL_PAGE 2


/**
 *@brief 技能最大等级
 */
#define CL_MAX_SKILL_LEVEL 100

 /**
 *@brief 允许重置技能的玩家最大等级
 */
#define RESET_SKILL_LEVEL	30

/**
*@brief 技能使用类型  by huchenhui
*/
enum UseSkillType
{
	ACTIVE_SKILL = 1,	//主动
	PASSIVE_SIILL = 2,	//被动
	//TODO
};

/**
*@brief  是否主动技能  by huchenhui
*/
inline bool IsActiveSkill(UInt8 type)
{
	return ACTIVE_SKILL == type;
}

/**
*@brief  是否被动技能 by huchenhui
*/
inline bool IsPassiveSkill(UInt8 type)
{
	return PASSIVE_SIILL == type;
}

/**
*@brief 技能配置方案
*/
enum SkillConfigType
{
	SKILL_CONFIG_PVE = 1,
	SKILL_CONFIG_PVP = 2,
	SKILL_CONFIG_EQUAL_PVP = 3,        //公平竞技场技能配置
};

/**
*@brief 技能类型
*/
enum SkillType
{
	SKILL_TYPE_NORMAL		= 1,		//普通攻击
	SKILL_TYPE_GENERAL		= 2,		//通用技能
	SKILL_TYPE_OCCUPATION	= 3,		//职业技能
	SKILL_TYPE_AWAKEN		= 4,		//觉醒技能
	SKILL_TYPE_GUILD		= 5,		//工会技能
	SKILL_TYPE_ADVENTURE	= 6,		//冒险团技能
	//TODO
};

/**
 *@brief 是否需要同步
 */
inline bool IsSkillNeedSyncNet(int type)
{
	switch (type)
	{
	case SKILL_TYPE_NORMAL:
	case SKILL_TYPE_OCCUPATION:
	case SKILL_TYPE_GENERAL:
	case SKILL_TYPE_AWAKEN:
	case SKILL_TYPE_GUILD:
		return true;
	}
	return false;
}

/**
 *@brief 是否需要存db
 */
inline bool IsSkillNeedSyncDB(int type)
{
	switch (type)
	{
	case SKILL_TYPE_NORMAL:
	case SKILL_TYPE_OCCUPATION:
	case SKILL_TYPE_GENERAL:
	case SKILL_TYPE_AWAKEN:
	case SKILL_TYPE_GUILD:
		return true;
	}
	return false;
}

/**
*@brief  是冒险团否技能
*/
inline bool IsAdventureSkill(int type)
{
	return SKILL_TYPE_ADVENTURE == type;
}


/**
*@brief  是否公会技能
*/
inline bool IsGuildSkill(int type)
{
	return SKILL_TYPE_GUILD == type;
}

/**
*@brief  是否职业技能
*/
inline bool IsOccupation(int type)
{
	return SKILL_TYPE_OCCUPATION == type;
}

/**
*@brief  是否通用技能
*/
inline bool IsGeneral(int type)
{
	return SKILL_TYPE_GENERAL == type;
}

/**
*@brief  是否天生携带
*/
inline bool IsNormal(int type)
{
	return SKILL_TYPE_NORMAL == type;
}

/**
*@brief  是否觉醒技能
*/
inline bool IsAwaken(int type)
{
	return SKILL_TYPE_AWAKEN == type;
}


/**
*@brief  是否能配置到技能槽
*/
inline bool IsNeedSetting(int type)
{
	switch (type)
	{
		case SKILL_TYPE_OCCUPATION:
		return true;
	}
	return false;
}

/**
 *@brief  玩家是否可升级
 */
inline bool IsSkillPlayerUpgrade(int type)
{
	switch (type)
	{
	case SKILL_TYPE_OCCUPATION:
	case SKILL_TYPE_GENERAL:
		return true;
	}
	return false;
}

struct ChangeSkill
{
	ChangeSkill() :id(0), dif(0) {}

	ChangeSkill(UInt16 _id, Int8 _dif) :id(_id), dif(_dif) {};


	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & dif;
	}

	UInt16	id;
	Int8	dif;
};


//BUFF类型
enum BuffType
{
	BT_PROPERTY_OP = -1,  // 属性操作(hp,mp)
	BT_SPEED_ADD = 17,	// 速度buff
	BT_EXP_ADD = 19,
	BT_GOLD_ADD = 20,
	BT_TASK_DROP_NUM = 21,
	BT_PINK_DROP = 22,
	BT_FATIGUE_BURN = 23,	//疲劳buff
	BT_EXP_ADD_ACTIVITY_AND_COMMON = 25, //活动经验加成(以后补充地下城的经验加成后废弃)
};

//BUFF工作类型
enum BuffWorkType
{
	BWT_INCREASE,
	BWT_REDUCE,
};

//BUFF覆盖类型
enum BuffOverlayType
{
	BOT_NEW_ONE,		//不叠加新增
	BOT_VALUE_OL,		//叠加效果值
	BOT_CORVER,			//覆盖
	BOT_CAN_NOT_OL,		//不能叠加
	BOT_TIME_OL,		//叠加时间
};

//BUFF持续类型
enum BuffDurationType
{
	BDT_TIME = 0,
	BDT_DUNGEON = 1,	// 持续一次地下城
	BDT_ONLY_DUNGEON = 2,	// 只在地下城生效
	BDT_DEADLINE_CALC = 3,	// 截止时间计算
	BDT_DEADLINE_TIME_CONTINUE_PVP = 4,	// 截止时间并持续一次PK
};

//单局buff结构
struct RaceBuffInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_id & m_overlayNums & m_startTime & m_duration;
	}

	//id
	UInt32	m_id;
	//叠加层数
	UInt32	m_overlayNums;
	//开始时间
	UInt64	m_startTime;
	//持续时间
	UInt32	m_duration;
};

struct SkillBase
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & level;
	}
	// id
	UInt16 id;
	// 等级
	UInt8 level;
};

#endif
