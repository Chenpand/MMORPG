#ifndef __DUNGEON_ROLL_H__
#define __DUNGEON_ROLL_H__
#include <CLDefine.h>
#include <CLDropItemDefine.h>
#include <CLDungeonDefine.h>

//一个玩家roll信息
struct PlayerRollInfo
{
	PlayerRollInfo()
		: playerId(0), playerName(""), opType(RIE_INVALID), point(0)
	{}
	UInt64			playerId;
	std::string		playerName;
	UInt8			opType;
	UInt32			point;
};

//roll掉落
class DungeonRollItem
{
public:
	DungeonRollItem();
	~DungeonRollItem();

	/**
	*@brief 正常生产一个roll点
	*/
	UInt32 RandomRollPoint();

	/**
	*@brief 结算时候全部是谦让的时候生成roll点
	*/
	void RandomRollPointForAllModest();
	
	/**
	*@brief 发送掉落
	*/
	void SendDropItem(UInt64 raceId, UInt32 dungeonId);

	/**
	*@brief 
	*/
	RollItemInfo ToClient();

	/*
	*@brief 获取玩家roll信息
	*/
	PlayerRollInfo* GetOnePlayerInfo(ObjID_t roleId);

public:
	//索引
	UInt8		index;
	//掉落
	DropItem	dropItem;
	//玩家roll信息
	std::vector<PlayerRollInfo>  playerRollInfos;
	
};


#endif