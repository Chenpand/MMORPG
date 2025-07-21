#ifndef _CL_WARP_STONE_DEFINE_H_
#define _CL_WARP_STONE_DEFINE_H_

#include "CLDefine.h"

/**
*@brief 能量石类型
*/
enum WarpStoneType
{
	STONE_INVALID = 0,

	STONE_RED = 1,
	STONE_GREEN = 2,
	STONE_YELLOW = 3,
	STONE_BLUE = 4,
	STONE_BLACK = 5,
	STONE_COLORLESS = 6,	//无色

	STONE_NUM,
};

inline bool IsWarpStone(WarpStoneType type)
{
	return (type < STONE_NUM && type != STONE_INVALID);
}

/**
*@brief 充能信息
*/
struct ChargeInfo
{
public:
	ChargeInfo() {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & num;
	}

public:
	UInt32 itemId;
	UInt32 num;
};


//单局能量石
struct RaceWarpStone
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Id & m_Level;
	}

	//id
	UInt32 m_Id;

	//等级
	UInt8 m_Level;
};

#endif //_CL_WARP_STONE_DEFINE_H_
