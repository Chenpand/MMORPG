#ifndef _ITEM_FRIEND_PRESENT_H_
#define _ITEM_FRIEND_PRESENT_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLSyncObject.h>
#include <CLItemDefine.h>
#include <functional>

class WSPlayer;
class WSRelation;
class ItemDataEntry;

//好友赠送道具记录(A玩家id < B玩家id)
class FriendPresentRecoord : public CLSyncObject
{
public:
	FriendPresentRecoord(){
		itemId = 0;
		playerAId = 0;
		sendNumA = 0;
		playerBId = 0;
		sendNumB = 0;
	}
	FriendPresentRecoord(UInt32 itemId_, UInt64 playerAId_, UInt64 playerBId_)
	{
		itemId = itemId_;
		sendNumA = 0;
		sendNumB = 0;
		if (playerAId_ < playerBId_)
		{
			playerAId = playerAId_;
			playerBId = playerBId_;
		}
		else
		{
			playerAId = playerBId_;
			playerBId = playerAId_;
		}
	}
	~FriendPresentRecoord() {}

public:
	bool ToClient(WSPlayer*player, FriendPresentInfo& presentInfo, WSRelation* relation = NULL, ItemDataEntry* itemData =NULL);
	void LoadFromDb(CLRecordCallback *pCallback);
	void Update();
	void Insert();
	void Delete();

	bool IncPlayerSendTime(ObjID_t player, UInt32 inc);
public:
	//编解码
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("item_id", itemId)
	CL_DBPROPERTY("playerA_id", playerAId)
	CL_DBPROPERTY("send_num_A", sendNumA)
	CL_DBPROPERTY("playerB_id", playerBId)
	CL_DBPROPERTY("send_num_B", sendNumB)
	CL_DBPROPERTY_END()
public:
	//赠送道具id
	ObjUInt32	itemId;
	//A玩家id
	ObjUInt64   playerAId;
	//A玩家赠送数量
	ObjUInt32	sendNumA;
	//B玩家id
	ObjUInt64	playerBId;
	//B玩家赠送数量
	ObjUInt32	sendNumB;
};

//好友赠送道具记录键值(A玩家id < B玩家id)
struct  FriendPresentKey
{
	FriendPresentKey(UInt32	itemId_, UInt64  playerA_, UInt64  playerB_) 
	: itemId(itemId_), playerA(playerA_), playerB(playerB_){
		if (playerA_ < playerB_)
		{
			playerA = playerA_;
			playerB = playerB_;
		}
		else
		{
			playerA = playerB_;
			playerB = playerA_;
		}
	}

	bool operator<(const FriendPresentKey& other) const
	{
		if (other.itemId != itemId)
		{
			return itemId < other.itemId;
		}
		if (other.playerA != playerA)
		{
			return playerA < other.playerA;
		}
		return playerB < other.playerB;
	}

	//道具id
	UInt32	itemId;
	//A玩家id
	UInt64  playerA;
	//B玩家id
	UInt64  playerB;
};

//好友赠送道具系统
class FriendPresentMgr : public Avalon::Singleton<FriendPresentMgr>
{
public:
	typedef std::map<FriendPresentKey, FriendPresentRecoord*> ItemPresMap;
	typedef std::map<UInt64, ItemPresMap> PlayerItemPresMap;
public:
	FriendPresentMgr();
	~FriendPresentMgr();

public:
	void Init();

	bool OnLoadItemPresRecordsRet(CLRecordCallback *pCallback);

	FriendPresentRecoord* CreateRecord(UInt32 itemId, UInt64 player1, UInt64 player2);

	FriendPresentRecoord* GetRecord(FriendPresentKey& key);

	bool AddOneRecord(FriendPresentRecoord* record);

	void AddOneRecordToPlayer(FriendPresentRecoord* record, UInt64 palyerId);

	UInt32 GetPlayerItemPresentInfo(WSPlayer* player, UInt32 itemId, std::vector<FriendPresentInfo>& list,
		UInt32& recvedTotal, UInt32& recvedTotalLimit);

	//获取玩家赠送集合
	ItemPresMap& GetPlayerPresMap(UInt64 playerId);
	
	//增加被赠送道具总次数
	void	IncPlayerItemRecvedNum(UInt64 playerId, UInt32 itemid, UInt32 inc);

	//获取玩家道具被赠送数量
	UInt32	GetPlayerSpecItemRecvedNum(UInt64 playerId, UInt32 itemId);

	//赠送道具
	UInt32	PlayerSendItem(WSPlayer*player, UInt64 itemId, UInt32 itemTypeId, ObjID_t friendId);
	UInt32  PlayerSendItemImp(WSPlayer*player, UInt64 itemId, UInt32 itemTypeId, ObjID_t friendId, FriendPresentInfo& presentInfo);
	
	
private:
	ItemPresMap		   itemPresedSet;
	//玩家相关赠送key
	PlayerItemPresMap  playerItemPres;
	//玩家被赠送道具数量集合
	std::map<UInt64, std::map<UInt32, UInt32>>  playerRecvItemNum;
};

#endif
