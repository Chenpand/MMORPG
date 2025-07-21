#ifndef _CL_SKILL_DEFINE_H_
#define _CL_SKILL_DEFINE_H_

#include "CLDefine.h"
#include <AvalonNetStream.h>

/**
 *@brief �������ҳ��
 */
#define CL_MAX_SKILL_PAGE 2


/**
 *@brief �������ȼ�
 */
#define CL_MAX_SKILL_LEVEL 100

 /**
 *@brief �������ü��ܵ�������ȼ�
 */
#define RESET_SKILL_LEVEL	30

/**
*@brief ����ʹ������  by huchenhui
*/
enum UseSkillType
{
	ACTIVE_SKILL = 1,	//����
	PASSIVE_SIILL = 2,	//����
	//TODO
};

/**
*@brief  �Ƿ���������  by huchenhui
*/
inline bool IsActiveSkill(UInt8 type)
{
	return ACTIVE_SKILL == type;
}

/**
*@brief  �Ƿ񱻶����� by huchenhui
*/
inline bool IsPassiveSkill(UInt8 type)
{
	return PASSIVE_SIILL == type;
}

/**
*@brief �������÷���
*/
enum SkillConfigType
{
	SKILL_CONFIG_PVE = 1,
	SKILL_CONFIG_PVP = 2,
	SKILL_CONFIG_EQUAL_PVP = 3,        //��ƽ��������������
};

/**
*@brief ��������
*/
enum SkillType
{
	SKILL_TYPE_NORMAL		= 1,		//��ͨ����
	SKILL_TYPE_GENERAL		= 2,		//ͨ�ü���
	SKILL_TYPE_OCCUPATION	= 3,		//ְҵ����
	SKILL_TYPE_AWAKEN		= 4,		//���Ѽ���
	SKILL_TYPE_GUILD		= 5,		//���Ἴ��
	SKILL_TYPE_ADVENTURE	= 6,		//ð���ż���
	//TODO
};

/**
 *@brief �Ƿ���Ҫͬ��
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
 *@brief �Ƿ���Ҫ��db
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
*@brief  ��ð���ŷ���
*/
inline bool IsAdventureSkill(int type)
{
	return SKILL_TYPE_ADVENTURE == type;
}


/**
*@brief  �Ƿ񹫻Ἴ��
*/
inline bool IsGuildSkill(int type)
{
	return SKILL_TYPE_GUILD == type;
}

/**
*@brief  �Ƿ�ְҵ����
*/
inline bool IsOccupation(int type)
{
	return SKILL_TYPE_OCCUPATION == type;
}

/**
*@brief  �Ƿ�ͨ�ü���
*/
inline bool IsGeneral(int type)
{
	return SKILL_TYPE_GENERAL == type;
}

/**
*@brief  �Ƿ�����Я��
*/
inline bool IsNormal(int type)
{
	return SKILL_TYPE_NORMAL == type;
}

/**
*@brief  �Ƿ���Ѽ���
*/
inline bool IsAwaken(int type)
{
	return SKILL_TYPE_AWAKEN == type;
}


/**
*@brief  �Ƿ������õ����ܲ�
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
 *@brief  ����Ƿ������
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


//BUFF����
enum BuffType
{
	BT_PROPERTY_OP = -1,  // ���Բ���(hp,mp)
	BT_SPEED_ADD = 17,	// �ٶ�buff
	BT_EXP_ADD = 19,
	BT_GOLD_ADD = 20,
	BT_TASK_DROP_NUM = 21,
	BT_PINK_DROP = 22,
	BT_FATIGUE_BURN = 23,	//ƣ��buff
	BT_EXP_ADD_ACTIVITY_AND_COMMON = 25, //�����ӳ�(�Ժ󲹳���³ǵľ���ӳɺ����)
};

//BUFF��������
enum BuffWorkType
{
	BWT_INCREASE,
	BWT_REDUCE,
};

//BUFF��������
enum BuffOverlayType
{
	BOT_NEW_ONE,		//����������
	BOT_VALUE_OL,		//����Ч��ֵ
	BOT_CORVER,			//����
	BOT_CAN_NOT_OL,		//���ܵ���
	BOT_TIME_OL,		//����ʱ��
};

//BUFF��������
enum BuffDurationType
{
	BDT_TIME = 0,
	BDT_DUNGEON = 1,	// ����һ�ε��³�
	BDT_ONLY_DUNGEON = 2,	// ֻ�ڵ��³���Ч
	BDT_DEADLINE_CALC = 3,	// ��ֹʱ�����
	BDT_DEADLINE_TIME_CONTINUE_PVP = 4,	// ��ֹʱ�䲢����һ��PK
};

//����buff�ṹ
struct RaceBuffInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_id & m_overlayNums & m_startTime & m_duration;
	}

	//id
	UInt32	m_id;
	//���Ӳ���
	UInt32	m_overlayNums;
	//��ʼʱ��
	UInt64	m_startTime;
	//����ʱ��
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
	// �ȼ�
	UInt8 level;
};

#endif
