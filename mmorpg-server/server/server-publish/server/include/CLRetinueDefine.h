#ifndef _CL_RETINUE_DEFINE_H_
#define _CL_RETINUE_DEFINE_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLPkStatisticDefine.h"
#include "DropItemMgr.h"

#define RAND_MAX_SKILL		10000		//总共的几率
#define RAND_COMMON_SKILL	9000		//公共池技能几率(万分之N)
#define RAND_ONLY_SKILL		1000		//特性池技能几率(万分之N)

#define MAIN_RETINUE_INDEX			1			//主随从位置
#define OFF_RETINUE_COUNT			2			//副随从个数
#define OFF_RETINUE_INDEX_OFFSET	2			//副随偏移

enum RetinueQuality
{
	RETINUE_BLUE = 0,		//侍从
	RETINUE_PURPLE = 1,		//骑士
	RETINUE_PINK = 2,		//贤者
	RETINUE_ORANGE = 3,		//传说

	RETINUE_NUM,
};

enum RetinueGroupType
{
	//技能
	RGT_BUFF = 0,
	//方案
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

	//洗练项id
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

	//数据ID
	UInt32 dataId;

	//随从技能等级
	UInt8 level;

	//星星等级
	UInt8 starLevel;

	//支援技能
	std::vector<RetinueSkill> skills;
};

//解锁item
struct UnlockItem
{
public:
	UnlockItem() : itemId(0), num(0){}

public:
	UInt32 itemId;
	UInt32 num;
};

//单局随从
struct RaceRetinue
{
public:
	RaceRetinue() :m_DataId(0), m_Level(1), m_Star(0), m_IsMain(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level & m_Star & m_IsMain & m_BuffIds;
	}

	//随从表ID
	UInt32 m_DataId;

	//等级
	UInt8 m_Level;

	//星级
	UInt8 m_Star;

	//是否主随从
	UInt8 m_IsMain;

	//buffID
	std::vector<UInt32> m_BuffIds;

};

#endif	//_CL_RETINUE_DEFINE_H_