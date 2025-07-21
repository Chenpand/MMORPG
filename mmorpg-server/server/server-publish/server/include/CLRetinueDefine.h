#ifndef _CL_RETINUE_DEFINE_H_
#define _CL_RETINUE_DEFINE_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLPkStatisticDefine.h"
#include "DropItemMgr.h"

#define RAND_MAX_SKILL		10000		//�ܹ��ļ���
#define RAND_COMMON_SKILL	9000		//�����ؼ��ܼ���(���֮N)
#define RAND_ONLY_SKILL		1000		//���Գؼ��ܼ���(���֮N)

#define MAIN_RETINUE_INDEX			1			//�����λ��
#define OFF_RETINUE_COUNT			2			//����Ӹ���
#define OFF_RETINUE_INDEX_OFFSET	2			//����ƫ��

enum RetinueQuality
{
	RETINUE_BLUE = 0,		//�̴�
	RETINUE_PURPLE = 1,		//��ʿ
	RETINUE_PINK = 2,		//����
	RETINUE_ORANGE = 3,		//��˵

	RETINUE_NUM,
};

enum RetinueGroupType
{
	//����
	RGT_BUFF = 0,
	//����
	RGT_SOLUTION = 1
};


struct RetinueSkill
{
public:
	RetinueSkill() :id(0), buffId(0){}
	RetinueSkill(UInt32 _id, UInt32 _buffId) :id(_id), buffId(_buffId){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & buffId;
	}

public:
	//id
	UInt32	id;

	//ϴ����id
	UInt32	buffId;

};


struct RetinueInfo
{
public:
	RetinueInfo() :id(0), dataId(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & dataId & level & starLevel & skills;
	}

public:
	//id
	ObjID_t id;

	//����ID
	UInt32 dataId;

	//��Ӽ��ܵȼ�
	UInt8 level;

	//���ǵȼ�
	UInt8 starLevel;

	//֧Ԯ����
	std::vector<RetinueSkill> skills;
};

//����item
struct UnlockItem
{
public:
	UnlockItem() : itemId(0), num(0){}

public:
	UInt32 itemId;
	UInt32 num;
};

//�������
struct RaceRetinue
{
public:
	RaceRetinue() :m_DataId(0), m_Level(1), m_Star(0), m_IsMain(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level & m_Star & m_IsMain & m_BuffIds;
	}

	//��ӱ�ID
	UInt32 m_DataId;

	//�ȼ�
	UInt8 m_Level;

	//�Ǽ�
	UInt8 m_Star;

	//�Ƿ������
	UInt8 m_IsMain;

	//buffID
	std::vector<UInt32> m_BuffIds;

};

#endif	//_CL_RETINUE_DEFINE_H_