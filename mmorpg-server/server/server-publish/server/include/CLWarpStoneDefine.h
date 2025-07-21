#ifndef _CL_WARP_STONE_DEFINE_H_
#define _CL_WARP_STONE_DEFINE_H_

#include "CLDefine.h"

/**
*@brief ����ʯ����
*/
enum WarpStoneType
{
	STONE_INVALID = 0,

	STONE_RED = 1,
	STONE_GREEN = 2,
	STONE_YELLOW = 3,
	STONE_BLUE = 4,
	STONE_BLACK = 5,
	STONE_COLORLESS = 6,	//��ɫ

	STONE_NUM,
};

inline bool IsWarpStone(WarpStoneType type)
{
	return (type < STONE_NUM && type != STONE_INVALID);
}

/**
*@brief ������Ϣ
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


//��������ʯ
struct RaceWarpStone
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Id & m_Level;
	}

	//id
	UInt32 m_Id;

	//�ȼ�
	UInt8 m_Level;
};

#endif //_CL_WARP_STONE_DEFINE_H_
