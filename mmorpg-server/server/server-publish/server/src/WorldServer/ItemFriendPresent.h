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

//�������͵��߼�¼(A���id < B���id)
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
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("item_id", itemId)
	CL_DBPROPERTY("playerA_id", playerAId)
	CL_DBPROPERTY("send_num_A", sendNumA)
	CL_DBPROPERTY("playerB_id", playerBId)
	CL_DBPROPERTY("send_num_B", sendNumB)
	CL_DBPROPERTY_END()
public:
	//���͵���id
	ObjUInt32	itemId;
	//A���id
	ObjUInt64   playerAId;
	//A�����������
	ObjUInt32	sendNumA;
	//B���id
	ObjUInt64	playerBId;
	//B�����������
	ObjUInt32	sendNumB;
};

//�������͵��߼�¼��ֵ(A���id < B���id)
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

	//����id
	UInt32	itemId;
	//A���id
	UInt64  playerA;
	//B���id
	UInt64  playerB;
};

//�������͵���ϵͳ
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

	//��ȡ������ͼ���
	ItemPresMap& GetPlayerPresMap(UInt64 playerId);
	
	//���ӱ����͵����ܴ���
	void	IncPlayerItemRecvedNum(UInt64 playerId, UInt32 itemid, UInt32 inc);

	//��ȡ��ҵ��߱���������
	UInt32	GetPlayerSpecItemRecvedNum(UInt64 playerId, UInt32 itemId);

	//���͵���
	UInt32	PlayerSendItem(WSPlayer*player, UInt64 itemId, UInt32 itemTypeId, ObjID_t friendId);
	UInt32  PlayerSendItemImp(WSPlayer*player, UInt64 itemId, UInt32 itemTypeId, ObjID_t friendId, FriendPresentInfo& presentInfo);
	
	
private:
	ItemPresMap		   itemPresedSet;
	//����������key
	PlayerItemPresMap  playerItemPres;
	//��ұ����͵�����������
	std::map<UInt64, std::map<UInt32, UInt32>>  playerRecvItemNum;
};

#endif
