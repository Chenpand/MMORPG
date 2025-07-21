#ifndef _CL_PET_DEFINE_H_
#define _CL_PET_DEFINE_H_

#include "CLDefine.h"

enum PetObjectAttr
{
	POA_INVALID,			//��Ч
	POA_LEVEL,				//�ȼ�	UInt16
	POA_EXP,				//����	UInt32
	POA_HUNGER,				//����ֵ	UInt16
	POA_SKILL_INDEX,		//��������	UInt8
	POA_GOLD_FEED_COUNT,	//���ιʳ����	UInt8
	POA_POINT_FEED_COUNT,	//���ιʳ����	UInt8
	POA_SELECT_SKILL_COUNT,	//������ѡ����	UInt8
	POA_PET_SCORE,			//��������	UInt32
};

enum PetQuality
{
	PET_QUALITY_INVALID = 0,	//��Ч

	PET_QUALITY_WHITE = 1,		//��ͨ
	PET_QUALITY_BLUE,			//�߼�
	PET_QUALITY_PURPLE,			//ϡ��
	PET_QUALITY_GREEN,			//����
	PET_QUALITY_PINK,			//����
	PET_QUALITY_YELLOW,			//ʫʷ

	PET_QUALITY_NUM

};

enum PetType
{
	PET_TYPE_INVALID = 0,		//��Ч

	PET_TYPE_ATTACK = 1,		//������
	PET_TYPE_PROPERTY = 2,		//������
	PET_TYPE_SUPPORT = 3,		//֧Ԯ��

	PET_TYPE_NUM
};

enum PetFeedType
{
	PET_FEED_INVALID = 0,		//��Ч
	PET_FEED_GOLD = 1,			//���
	PET_FEED_POINT = 2,			//���
	PET_FEED_ITEM = 3,			//��Ʒ
};

struct PetSlot
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & petType & id;
	}

public:
	UInt8 petType;
	UInt64 id;
};

struct FeedItem
{
	FeedItem() :itemId(0), itemNum(0), value(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & itemNum & value;
	}
public:
	//��Ʒid
	UInt32 itemId;
	//��Ʒ����
	UInt32 itemNum;
	//��ֵ
	UInt32 value;
};

struct PetInfo
{
public:
	PetInfo() :id(0), dataId(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & dataId & level & exp & hunger & skillIndex & pointFeedCount & goldFeedCount & selectSkillCount & petScore;
	}

public:
	//id
	ObjID_t id;

	//����ID
	UInt32 dataId;

	//����ȼ�
	UInt16 level;

	//���ﾭ��
	UInt32 exp;

	//���Ｂ��ֵ
	UInt16 hunger;

	//��������
	UInt8 skillIndex;

	//���ιʳ����
	UInt8 pointFeedCount;

	//���ιʳ����
	UInt8 goldFeedCount;

	//������ѡ����
	UInt8 selectSkillCount;

	//����
	UInt32 petScore;
};

//���������Ϣ
struct PetBaseInfo
{
public:
	PetBaseInfo() :m_DataId(0), m_Level(1), m_Hunger(0), m_SkillIndex(0), m_petScore(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level & m_Hunger & m_SkillIndex & m_petScore;
	}

	//�����ID
	UInt32 m_DataId;

	//�ȼ�
	UInt16 m_Level;

	//����ֵ
	UInt16 m_Hunger;

	//��������
	UInt8 m_SkillIndex;

	//��������
	UInt32 m_petScore;
};

//���ֳ���
struct RacePet
{
public:
	RacePet() :m_DataId(0), m_Level(1), m_Hunger(0), m_SkillIndex(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level & m_Hunger & m_SkillIndex;
	}

	//�����ID
	UInt32 m_DataId;

	//�ȼ�
	UInt16 m_Level;

	//����ֵ
	UInt16 m_Hunger;

	//��������
	UInt8 m_SkillIndex;

};

#endif