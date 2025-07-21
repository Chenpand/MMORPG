#ifndef __DUNGEON_ROLL_H__
#define __DUNGEON_ROLL_H__
#include <CLDefine.h>
#include <CLDropItemDefine.h>
#include <CLDungeonDefine.h>

//һ�����roll��Ϣ
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

//roll����
class DungeonRollItem
{
public:
	DungeonRollItem();
	~DungeonRollItem();

	/**
	*@brief ��������һ��roll��
	*/
	UInt32 RandomRollPoint();

	/**
	*@brief ����ʱ��ȫ����ǫ�õ�ʱ������roll��
	*/
	void RandomRollPointForAllModest();
	
	/**
	*@brief ���͵���
	*/
	void SendDropItem(UInt64 raceId, UInt32 dungeonId);

	/**
	*@brief 
	*/
	RollItemInfo ToClient();

	/*
	*@brief ��ȡ���roll��Ϣ
	*/
	PlayerRollInfo* GetOnePlayerInfo(ObjID_t roleId);

public:
	//����
	UInt8		index;
	//����
	DropItem	dropItem;
	//���roll��Ϣ
	std::vector<PlayerRollInfo>  playerRollInfos;
	
};


#endif