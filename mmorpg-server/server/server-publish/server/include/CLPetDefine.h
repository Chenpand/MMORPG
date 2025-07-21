#ifndef _CL_PET_DEFINE_H_
#define _CL_PET_DEFINE_H_

#include "CLDefine.h"

enum PetObjectAttr
{
	POA_INVALID,			//无效
	POA_LEVEL,				//等级	UInt16
	POA_EXP,				//经验	UInt32
	POA_HUNGER,				//饥饿值	UInt16
	POA_SKILL_INDEX,		//技能索引	UInt8
	POA_GOLD_FEED_COUNT,	//金币喂食次数	UInt8
	POA_POINT_FEED_COUNT,	//点卷喂食次数	UInt8
	POA_SELECT_SKILL_COUNT,	//技能重选次数	UInt8
	POA_PET_SCORE,			//宠物评分	UInt32
};

enum PetQuality
{
	PET_QUALITY_INVALID = 0,	//无效

	PET_QUALITY_WHITE = 1,		//普通
	PET_QUALITY_BLUE,			//高级
	PET_QUALITY_PURPLE,			//稀有
	PET_QUALITY_GREEN,			//传承
	PET_QUALITY_PINK,			//神器
	PET_QUALITY_YELLOW,			//诗史

	PET_QUALITY_NUM

};

enum PetType
{
	PET_TYPE_INVALID = 0,		//无效

	PET_TYPE_ATTACK = 1,		//攻击型
	PET_TYPE_PROPERTY = 2,		//属性型
	PET_TYPE_SUPPORT = 3,		//支援型

	PET_TYPE_NUM
};

enum PetFeedType
{
	PET_FEED_INVALID = 0,		//无效
	PET_FEED_GOLD = 1,			//金币
	PET_FEED_POINT = 2,			//点卷
	PET_FEED_ITEM = 3,			//物品
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
	//物品id
	UInt32 itemId;
	//物品数量
	UInt32 itemNum;
	//数值
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

	//数据ID
	UInt32 dataId;

	//宠物等级
	UInt16 level;

	//宠物经验
	UInt32 exp;

	//宠物饥饿值
	UInt16 hunger;

	//技能索引
	UInt8 skillIndex;

	//点卷喂食次数
	UInt8 pointFeedCount;

	//金币喂食次数
	UInt8 goldFeedCount;

	//技能重选次数
	UInt8 selectSkillCount;

	//评分
	UInt32 petScore;
};

//宠物基本信息
struct PetBaseInfo
{
public:
	PetBaseInfo() :m_DataId(0), m_Level(1), m_Hunger(0), m_SkillIndex(0), m_petScore(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level & m_Hunger & m_SkillIndex & m_petScore;
	}

	//宠物表ID
	UInt32 m_DataId;

	//等级
	UInt16 m_Level;

	//饥饿值
	UInt16 m_Hunger;

	//技能索引
	UInt8 m_SkillIndex;

	//宠物评分
	UInt32 m_petScore;
};

//单局宠物
struct RacePet
{
public:
	RacePet() :m_DataId(0), m_Level(1), m_Hunger(0), m_SkillIndex(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level & m_Hunger & m_SkillIndex;
	}

	//宠物表ID
	UInt32 m_DataId;

	//等级
	UInt16 m_Level;

	//饥饿值
	UInt16 m_Hunger;

	//技能索引
	UInt8 m_SkillIndex;

};

#endif