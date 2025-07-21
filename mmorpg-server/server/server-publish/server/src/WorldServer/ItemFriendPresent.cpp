#include "ItemFriendPresent.h"

#include <CLRecordClient.h>
#include <CLItemDataEntry.h>
#include "WSPlayer.h"
#include "WSSelectInitDataCallback.h"
#include "WSRelationMgr.h"
#include "WSItemTransaction.h"
#include "CLItemProtocol.h"

/**
*@breif 查询道具赠送记录
*/
class SelectItemPresentsCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return FriendPresentMgr::Instance()->OnLoadItemPresRecordsRet(this);
	}
};

bool FriendPresentRecoord::ToClient(WSPlayer*player, FriendPresentInfo& presentInfo, WSRelation* relation, ItemDataEntry* itemData)
{
	if (!player)
	{
		return false;
	}
	if (!itemData)
	{
		itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
		if (!itemData || itemData->id != itemId)	return false;
	}
	
	ObjID_t playerId = player->GetID();
	ObjID_t friendId = 0;
	if (playerId == playerAId)
	{
		friendId = playerBId;
	}
	else if (playerId == playerBId)
	{
		friendId = playerAId;
	}
	else
	{
		return false;
	}
	if (relation == NULL)
	{
		relation = player->GetRelationMgr().GetRelation(friendId);
		if (!relation)
		{
			return false;
		}
	}
	if (relation && relation->GetID() != friendId)
	{
		return false;
	}

	presentInfo.guid = GetID();
	presentInfo.friendId = friendId;
	presentInfo.friendOcc = relation->GetOccu();
	presentInfo.friendLev = relation->GetLevel();
	presentInfo.friendname = relation->GetName();
	presentInfo.isOnline = relation->IsOnline();
	presentInfo.beSendedLimit = itemData->sendedLimit;
	presentInfo.sendLimit = itemData->sendLimit;
	presentInfo.sendTimes = 0;
	presentInfo.beSendedTimes = 0;
	presentInfo.sendedTotalTimes = FriendPresentMgr::Instance()->GetPlayerSpecItemRecvedNum(friendId, itemId);
	presentInfo.sendedTotalLimit = itemData->sendedTotleLimit;
	if (playerId == playerAId)
	{
		presentInfo.beSendedTimes = sendNumB;
		presentInfo.sendTimes = sendNumA;
	}
	else
	{
		presentInfo.beSendedTimes = sendNumA;
		presentInfo.sendTimes = sendNumB;
	}
	return true;
}

void FriendPresentRecoord::LoadFromDb(CLRecordCallback *pCallback)
{
	if (!pCallback)
	{
		return;
	}
	this->SetDBData(pCallback);
	this->SetID(pCallback->GetKey());
	this->ClearDirty();
}

void FriendPresentRecoord::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_item_presents", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}
void FriendPresentRecoord::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_item_presents", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}
void FriendPresentRecoord::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_item_presents", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool FriendPresentRecoord::IncPlayerSendTime(ObjID_t player, UInt32 inc)
{
	if (player != playerAId && player != playerBId)
	{
		return false;
	}
	if (player == playerAId)
	{
		sendNumA = sendNumA + 1;
		FriendPresentMgr::Instance()->IncPlayerItemRecvedNum(playerBId, itemId, 1);
	}
	else
	{
		sendNumB = sendNumB + 1;
		FriendPresentMgr::Instance()->IncPlayerItemRecvedNum(playerAId, itemId, 1);
	}

	Update();
	return true;
}

FriendPresentMgr::FriendPresentMgr()
{

}

FriendPresentMgr::~FriendPresentMgr()
{
	for (auto it : itemPresedSet)
	{
		FriendPresentRecoord* record = it.second;
		if (record)
		{
			CL_SAFE_DELETE(record);
		}
	}
	itemPresedSet.clear();
}

void FriendPresentMgr::Init()
{
	CLSelectCommand* selectAuctionTransCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_item_presents");

	CLRecordClient::Instance()->SendCommand(selectAuctionTransCmd, new SelectItemPresentsCallback());
}

bool FriendPresentMgr::OnLoadItemPresRecordsRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL)
	{
		return false;
	}
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		UInt32 itemId = pCallback->GetData("item_id");
		UInt64 playerA = pCallback->GetData("playerA_id");
		UInt64 playerB = pCallback->GetData("playerB_id");

		if (playerA >= playerB)
		{
			ErrorLogStream << "item present record is err playerA >= playerB id : " << id << ", playerA ：" << playerA << ", playerB : " << playerB << LogStream::eos;
			continue;
		}

		FriendPresentKey key(itemId, playerA, playerB);
		auto record = GetRecord(key);
		if (record)	continue;	//重复

		record = new FriendPresentRecoord();
		record->LoadFromDb(pCallback);
		this->AddOneRecord(record);
	}
	return true;
}

FriendPresentRecoord* FriendPresentMgr::CreateRecord(UInt32 itemId, UInt64 player1, UInt64 player2)
{
	FriendPresentRecoord* record = new FriendPresentRecoord(itemId, player1, player2);
	return record;
}

FriendPresentRecoord* FriendPresentMgr::GetRecord(FriendPresentKey& key)
{
	auto iter = itemPresedSet.find(key);
	if (iter == itemPresedSet.end())
	{
		return NULL;
	}
	return iter->second;
}

bool FriendPresentMgr::AddOneRecord(FriendPresentRecoord* record)
{
	if (!record)
	{
		return false;
	}
	FriendPresentKey key(record->itemId, record->playerAId, record->playerBId);

	auto it = itemPresedSet.find(key);
	if (it != itemPresedSet.end())
	{
		return false;
	}
	itemPresedSet.insert(std::pair<FriendPresentKey, FriendPresentRecoord*>(key, record));

	AddOneRecordToPlayer(record, record->playerAId);
	AddOneRecordToPlayer(record, record->playerBId);
	IncPlayerItemRecvedNum(record->playerAId, record->itemId, record->sendNumB);
	IncPlayerItemRecvedNum(record->playerBId, record->itemId, record->sendNumA);
	return true;
}

void FriendPresentMgr::AddOneRecordToPlayer(FriendPresentRecoord* record, UInt64 palyerId)
{
	if (!record)
	{
		return;
	}
	auto iter = playerItemPres.find(palyerId);
	if (iter == playerItemPres.end())
	{
		ItemPresMap map;
		playerItemPres.insert(std::pair<UInt64, ItemPresMap>(palyerId, map));
	}
	ItemPresMap& map = playerItemPres[palyerId];
	FriendPresentKey key(record->itemId, record->playerAId, record->playerBId);
	auto it = map.find(key);
	if (it != map.end())
	{
		return;
	}
	map.insert(std::pair<FriendPresentKey, FriendPresentRecoord*>(key, record));
	return;
}

UInt32 FriendPresentMgr::GetPlayerItemPresentInfo(WSPlayer* player, UInt32 itemId, std::vector<FriendPresentInfo>& list, 
	UInt32& recvedTotal, UInt32& recvedTotalLimit)
{
	if (!player)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!itemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	recvedTotalLimit = itemData->sendedTotleLimit;
	ItemPresMap& presMap = GetPlayerPresMap(player->GetID());

	UInt8 relationTypes[3] = { RELATION_FRIEND, RELATION_MASTER, RELATION_DISCIPLE };
	for (UInt32 i=0; i<3; i++)
	{
		UInt8 rt = relationTypes[i];
		player->GetRelationMgr().VisitRelationsNew(rt,
			[&list, player, itemId, itemData, presMap, this](WSRelation* relation) {
			if (!relation)
			{
				return true;
			}
			ObjID_t playerId = player->GetID();
			ObjID_t friendId = relation->GetID();
			FriendPresentKey key(itemId, playerId, friendId);

			FriendPresentInfo presentInfo;
			auto it = presMap.find(key);
			if (it != presMap.end())
			{
				FriendPresentRecoord* record = it->second;
				if (!record)
				{
					return true;
				}
				if (!record->ToClient(player, presentInfo, relation))
				{
					return true;
				}
			}
			else
			{
				presentInfo.guid = 0;
				presentInfo.friendId = friendId;
				presentInfo.friendOcc = relation->GetOccu();
				presentInfo.friendLev = relation->GetLevel();
				presentInfo.friendname = relation->GetName();
				presentInfo.isOnline = relation->IsOnline();
				presentInfo.beSendedLimit = itemData->sendedLimit;
				presentInfo.sendLimit = itemData->sendLimit;
				presentInfo.sendTimes = 0;
				presentInfo.beSendedTimes = 0;
				presentInfo.sendedTotalTimes = GetPlayerSpecItemRecvedNum(friendId, itemId);
				presentInfo.sendedTotalLimit = itemData->sendedTotleLimit;
			}
			list.push_back(presentInfo);
			return true;
		});
	}
	
	recvedTotal = GetPlayerSpecItemRecvedNum(player->GetID(), itemId);

	return ErrorCode::SUCCESS;
}

FriendPresentMgr::ItemPresMap& FriendPresentMgr::GetPlayerPresMap(UInt64 playerId)
{
	static ItemPresMap map;
	auto iter = playerItemPres.find(playerId);
	if (iter == playerItemPres.end())
	{
		return map;
	}

	return iter->second;
}

void	FriendPresentMgr::IncPlayerItemRecvedNum(UInt64 playerId, UInt32 itemid, UInt32 inc)
{
	if (inc == 0)
	{
		return;
	}
	auto it = playerRecvItemNum.find(playerId);
	if (it == playerRecvItemNum.end())
	{
		std::map<UInt32, UInt32> map;
		playerRecvItemNum.insert(std::pair<UInt64, std::map<UInt32, UInt32>>(playerId, map));
	}
	std::map<UInt32, UInt32>& recvNum = playerRecvItemNum[playerId];
	recvNum[itemid] = recvNum[itemid] + inc;
}

UInt32	FriendPresentMgr::GetPlayerSpecItemRecvedNum(UInt64 playerId, UInt32 itemId)
{
	auto it = playerRecvItemNum.find(playerId);
	if (it == playerRecvItemNum.end())
	{
		return 0;
	}
	auto iter = it->second.find(itemId);
	if (iter == it->second.end())
	{
		return 0;
	}
	return iter->second;
}

class PresentItemTransaction : public ItemTransaction
{
public:
	PresentItemTransaction(WSPlayer *player,  ObjID_t itemId_, UInt32 itemTypeId_, ObjID_t friendId_)
	:ItemTransaction(player), itemId(itemId_), itemTypeId(itemTypeId_), friendId(friendId_) {}

	~PresentItemTransaction() {}
public:
	void OnCheckSucced(WSPlayer *pPlayer)
	{
		CLProtocol::WorldItemFriendPresentRes res;
		res.itemId = itemId;
		res.itemTypeId = itemTypeId;
		res.retCode = FriendPresentMgr::Instance()->PlayerSendItemImp(pPlayer, itemId, itemTypeId, friendId, res.presentInfo);
		if (res.retCode == ErrorCode::SUCCESS)
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_ITEM_PRESENT_SCORE, friendId, itemTypeId));
		}
		pPlayer->SendProtocol(res);
		UnLock();
	}

	void OnCheckFailed(WSPlayer *pPlayer)
	{
		CLProtocol::WorldItemFriendPresentRes res;
		res.itemId = itemId;
		res.itemTypeId = itemTypeId;
		res.retCode = GetErrorCode();
		pPlayer->SendProtocol(res);
		UnLock();
	}

	void OnTimeOut(WSPlayer* pPlayer)
	{
		CLProtocol::WorldItemFriendPresentRes res;
		res.itemId = itemId;
		res.itemTypeId = itemTypeId;
		res.retCode = ErrorCode::ITEM_PRESENT_FAIL;
		pPlayer->SendProtocol(res);
		UnLock();
	}
private:
	ObjID_t		itemId;
	UInt32		itemTypeId;
	ObjID_t		friendId;
};

UInt32	FriendPresentMgr::PlayerSendItem(WSPlayer* player, UInt64 itemId, UInt32 itemTypeId, ObjID_t friendId)
{
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemData)
	{
		return  ErrorCode::ITEM_PRESENT_FAIL;
	}

	//判断对方被赠送上限
	UInt32 sendedNum = GetPlayerSpecItemRecvedNum(friendId, itemTypeId);
	if (sendedNum >= itemData->sendedTotleLimit)
	{
		return  ErrorCode::ITEM_PRESENT_FAIL_UPLIMIT;
	}

	//判断我赠送的数量
	ObjID_t playerId = player->GetID();
	FriendPresentKey key(itemTypeId, playerId, friendId);
	FriendPresentRecoord* record = GetRecord(key);
	if (record)
	{
		UInt32	mySendNum = 0;
		if (playerId == key.playerA)
		{
			mySendNum = record->sendNumA;
		}
		else
		{
			mySendNum = record->sendNumB;
		}
		if (mySendNum >= itemData->sendLimit)
		{
			return ErrorCode::ITEM_PRESENT_FAIL_UPLIMIT_SINGLE;
		}
	}

	PresentItemTransaction *pTransaction = new PresentItemTransaction(player, itemId, itemTypeId, friendId);
	pTransaction->AddDetailItem(itemId, itemTypeId, 1);

	if (player->BeginItemTransaction(pTransaction))
	{
		return ErrorCode::SUCCESS;
	}
	return  ErrorCode::ITEM_PRESENT_FAIL;
}

UInt32  FriendPresentMgr::PlayerSendItemImp(WSPlayer*player, UInt64 itemId, UInt32 itemTypeId, ObjID_t friendId, FriendPresentInfo& presentInfo)
{
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemData)
	{
		return  ErrorCode::ITEM_PRESENT_FAIL;
	}

	//修改次数
	ObjID_t playerId = player->GetID();
	FriendPresentKey key(itemTypeId, playerId, friendId);
	FriendPresentRecoord* record = GetRecord(key);
	if (!record)
	{
		record = CreateRecord(itemTypeId, playerId, friendId);
		if (!record)
		{
			return  ErrorCode::ITEM_PRESENT_FAIL;
		}
		AddOneRecord(record);
	}

	if (!record->IncPlayerSendTime(playerId, 1))
	{
		return ErrorCode::ITEM_PRESENT_FAIL;
	}
	record->ToClient(player, presentInfo);

	//发赠送的东西
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string strReason;

	// 发送者
	SysNotifyMgr::Instance()->MakeNotify(10062, buffer, sizeof(buffer));
	strSenderName = buffer;

	SysNotifyMgr::Instance()->MakeNotify(10061, buffer, sizeof(buffer));
	strTitle = buffer;

	SysNotifyMgr::Instance()->MakeNotify(10063, buffer, sizeof(buffer), player->GetName().c_str());
	strContent = buffer;

	std::vector<WSItem*> vecitems;
	strReason = GetReason(SOURCE_ITEM_PRESENT_SCORE, playerId, friendId);

	if (MailBox::SendSysMail(friendId, strSenderName, strTitle, strContent, strReason, itemData->presentedItems, vecitems) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "send mail for present friend item fail trans playerId=" << playerId << ", friendId=" << friendId << ", item id =" << itemTypeId << LogStream::eos;
	}

	return ErrorCode::SUCCESS;
}