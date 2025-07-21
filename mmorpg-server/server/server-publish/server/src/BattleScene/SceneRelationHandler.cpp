#include "SceneRelationHandler.h"

#include <CLChatProtocol.h>
#include <CLSysNotify.h>

#include "PlayerMgr.h"
#include "Scene.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"
#include "ActivityMgr.h"
#include "SortListMgr.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "PlayerLoadCallback.h"

void HandleProtocol(CLProtocol::SceneRelationSyncList& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(player == NULL) return;

	for (size_t i = 0; i < req.relationList.size(); ++i)
	{
		player->SetRelationList((RelationType)i, req.relationList[i]);
	}
	if (player->HasRelation(RELATION_MASTER))
	{
		player->GetMasterAcademicGrowthTaskMgr().AcceptAllTask();
	}
	if (player->HasRelation(RELATION_DISCIPLE))
	{
		player->GetAchievementTaskMgr().OnApprentDisciple();
	}
}

void HandleProtocol(CLProtocol::SceneRelationNotifyNew& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(player == NULL) return;

	player->AddRelation((RelationType)req.relationType, req.tarId);

	if (RelationType::RELATION_FRIEND == req.relationType)
	{
		player->GetActionCounterMgr().IncCounter(ACTION_COUNTER_MAKEFRIEND);
		player->OnAddRelation((RelationType)req.relationType);
	}
	else if (RelationType::RELATION_MASTER == req.relationType ||
		RelationType::RELATION_DISCIPLE == req.relationType)
	{
		player->OnAddRelation((RelationType)req.relationType);
	}
}

void HandleProtocol(CLProtocol::SceneRelationNotifyDel& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(player == NULL) return;

	player->RemoveRelation((RelationType)req.relationType, req.tarId);
}

void HandleProtocol(CLProtocol::SceneRelationSyncIntimacy& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(player == NULL) return;

	player->OnSyncIntimacy(req.friendId, req.intimacy, req.incIntimacy);
}

void HandleProtocol(CLProtocol::SceneRelationFindPlayerReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.target);
	if(player == NULL) return;

	CLProtocol::SceneRelationFindPlayerRet ret;
	ret.killer = req.killer;
	ret.target = req.target;
	ret.sceneId = player->GetScene()->GetMapID();
	if(!player->GetScene()->FindRandPos(ret.pos, SCENE_BLOCK_PHYSICS, 5)) return;

	Router::SendToWorld(ret);
}

class PlayerLoadingForQuery : public PlayerLoadingCallback
{
public:
	PlayerLoadingForQuery(const CLProtocol::SceneQueryPlayerRet &ret) : m_Ret(ret) {}
	virtual void OnFinish(Player* player)
	{
		DebugLogStream << PLAYERINFO(player) << "loading for query finish!" << LogStream::eos;

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}

		m_Ret.success = 1;
		player->OnOhterPlayerWatch(m_Ret.info);
		Router::SendToWorld(m_Ret);
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "loading failed for query!" << LogStream::eos;

		m_Ret.success = 0;
		Router::SendToWorld(m_Ret);

		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	CLProtocol::SceneQueryPlayerRet		m_Ret;
};


class PlayerLoadingForQueryDetails : public PlayerLoadingCallback
{
public:
	PlayerLoadingForQueryDetails(const CLProtocol::SceneQueryPlayerDetailsRet &ret) : m_Ret(ret) {}
	virtual void OnFinish(Player* player)
	{
		DebugLogStream << PLAYERINFO(player) << "loading for query finish!" << LogStream::eos;

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}

		m_Ret.success = 1;
		//player->OnOhterPlayerWatch(m_Ret.info);
		m_Ret.info = player->GetMatchRacePlayerInfo(false);
		Router::SendToWorld(m_Ret);
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "loading failed for query!" << LogStream::eos;

		m_Ret.success = 0;
		Router::SendToWorld(m_Ret);

		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	CLProtocol::SceneQueryPlayerDetailsRet		m_Ret;
};

void HandleProtocol(CLProtocol::SceneQueryPlayerReq& req)
{
	CLProtocol::SceneQueryPlayerRet ret;
	ret.roldId = req.srcRoleId;

	Player* player = PlayerMgr::Instance()->FindPlayer(req.targetRoleId);
	
	if (!player)
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(req.targetRoleId, req.tarAccId, new PlayerLoadingForQuery(ret));
	}
	else
	{
		ret.success = 1;
		player->OnOhterPlayerWatch(ret.info);
		Router::SendToWorld(ret);
	}
	
}

void HandleProtocol(CLProtocol::SceneQueryPlayerDetailsReq& req)
{
	CLProtocol::SceneQueryPlayerDetailsRet ret;
	ret.roldId = req.srcRoleId;

	Player* player = PlayerMgr::Instance()->FindPlayer(req.targetRoleId);

	if (!player)
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(req.targetRoleId, 0, new PlayerLoadingForQueryDetails(ret));
	}
	else
	{
		ret.success = 1;
		ret.info = player->GetMatchRacePlayerInfo(false);
		Router::SendToWorld(ret);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneRelationPresentInfoReq& req)
{
	UInt32 days = GetGameStartDays();
	if(days > FRIEND_PRESENT_DAYS) return;	//非活动时间

	player->SendFriendPresentInfo();
}

void HandleProtocol(Player* player, CLProtocol::SceneRelationPresentDraw& req)
{
	UInt32 days = GetGameStartDays();
	if(days > FRIEND_PRESENT_DAYS) return; //非活动时间

	UInt32 itemId = 0;
	UInt32 presentFlag = 0;
	if(req.type == 1)
	{
		UInt32 onlineDays = player->GetContinuousOnlineDays();
		if(onlineDays == 0 || onlineDays > 8) return;
		itemId = FRIEND_PRESENT_ITEMS[onlineDays];
		presentFlag = RELATION_PRESENT_DRAW;
	}
	else
	{ 
		if(days != FRIEND_PRESENT_DAYS) return;
		itemId = FRIEND_PRESENT_ITEM15;
		presentFlag = RELATION_PRESENT_DRAW15;
	}

	UInt32 flag = player->GetCounter("present_flag");
	if(flag & presentFlag) return; 

	if(!player->CheckAddItem(itemId, 1))
	{
		player->SendNotify(21);
		return;
	}

	player->AddItem(GetReason(LogSourceType::SOURCE_TYPE_INVALID).c_str(), itemId, 1);
	flag |= presentFlag;
	player->SetCounter("present_flag", flag);

	player->SendFriendPresentInfo();
}

void HandleProtocol(Player* player, CLProtocol::SceneRelationPresentGive& req)
{
	UInt32 days = GetGameStartDays();
	if(days > FRIEND_PRESENT_DAYS) return; //非活动时间

	UInt32 itemId = FRIEND_GIVE_GIFT;
	UInt32 presentFlag = 0;

 

	if(!player->IsFriend(req.friendId)) return;

	UInt32 flag = player->GetCounter("present_flag");
	if(flag & presentFlag) return;
	flag |= presentFlag;
	player->SetCounter("present_flag", flag);

	Player* pFriend = PlayerMgr::Instance()->FindNormalPlayer(req.friendId);
	if(pFriend != NULL)
	{
		UInt8 result = 0;
		if(!pFriend->FriendGiveGift(player->GetName(), itemId)) result = 2;

		player->OnGiveFriendGiftRet(days, req.type, result);
	}
	else
	{
		CLProtocol::SceneRelationPresentToFriendReq protocol;
		protocol.playerId = player->GetID();
		protocol.playerName = player->GetName();
		protocol.friendId = req.friendId;
		protocol.type = req.type;
		protocol.itemId = itemId;
		protocol.gamestartDay = days;
		Router::SendToWorld(protocol);
	}
}

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendReq& req)
{		
	CLProtocol::SceneRelationPresentToFriendRet ret;
	ret.playerId = req.playerId;
	ret.type = req.type;
	ret.gamestartDay = req.gamestartDay;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.friendId);
	if(player == NULL)
	{
		ret.result = 1;
		Router::SendToWorld(ret);
		return;
	}

	if(!player->FriendGiveGift(req.playerName, req.itemId))
	{
		ret.result = 2;
		Router::SendToWorld(ret);
		return;
	}

	ret.result = 0;
	Router::SendToWorld(ret);
}

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendRet& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player == NULL) return;

	player->OnGiveFriendGiftRet(req.gamestartDay, req.type, req.result);
}

class PlayerLoadingForMasterGive : public PlayerLoadingCallback
{
public:
	PlayerLoadingForMasterGive(const std::vector<ObjID_t>& items, Player* master) : m_Master(master), m_Items(items) {}
	virtual void OnFinish(Player* player)
	{
		if (!m_Master || !player)
		{
			return;
		}

		InfoLogStream << "player(" << player->GetID() << ") loading finish for given by master." << LogStream::eos;

		m_Master->GetItemMgr().MasterGiveEquip(m_Items, player);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	Player*								m_Master;
	std::vector<ObjID_t>				m_Items;
};

void HandleProtocol(Player* player, CLProtocol::SceneMasterGiveEquip& req)
{
	if (!player)
	{
		return;
	}

	for (size_t i = 0; i < req.itemuids.size(); ++i)
	{
		Item* item = player->FindItem(req.itemuids[i]);
		if (!item)
		{
			ErrorLogStream << PLAYERINFO(player) << " master give item is null! uid:" << req.itemuids[i] << LogStream::eos;
			return;
		}

		if (item->IsItemLock(ILT_LEASE_LOCK))
		{
			ErrorLogStream << PLAYERINFO(player) << " master give item is in lease! uid:" << req.itemuids[i] << LogStream::eos;
			return;
		}

		if (item->IsMountedPrecBead())
		{
			ErrorLogStream << PLAYERINFO(player) << " master give item is in mounted precbead uid:" << req.itemuids[i] << LogStream::eos;
			player->SendNotify(1000093);
			return;
		}
	}

	CLProtocol::WorldCheckMasterGive checkReq;
	checkReq.masterId = player->GetID();
	checkReq.disciple = req.disciple;
	checkReq.items = req.itemuids;

	Router::SendToWorld(checkReq);
}

class PlayerLoadingForReqAddonPay : public PlayerLoadingCallback
{
public:
	PlayerLoadingForReqAddonPay(const CLProtocol::SceneAddonPayReq& req, Player* disciple) : m_Disciple(disciple), m_Req(req) {}
	virtual void OnFinish(Player* player)
	{
		if (!m_Disciple || !player)
		{
			return;
		}

		InfoLogStream << "player(" << player->GetID() << ") loading finish for request addon pay." << LogStream::eos;

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
		
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << "player(" << 0 << ", " << m_Req.tarId << ") loading failed!" << LogStream::eos;

		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	Player*								m_Disciple;
	CLProtocol::SceneAddonPayReq		m_Req;
};

void HandleProtocol(Player* player, CLProtocol::SceneAddonPayReq& req)
{
	if (!player)
	{
		return;
	}

	if (!player->HasRelation(RelationType::RELATION_MASTER, req.tarId))
	{
		return;
	}

	// 师傅离线了
	Player* master = PlayerMgr::Instance()->FindNormalPlayer(req.tarId);
	if (!master)
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(req.tarId, 0, new PlayerLoadingForReqAddonPay(req, player));
	}
}

void HandleProtocol(CLProtocol::WorldCheckMasterGiveRes& res)
{
	Player* master = PlayerMgr::Instance()->FindNormalPlayer(res.masterId);
	if (!master)
	{
		return;
	}

	Player* disciple = PlayerMgr::Instance()->FindPlayer(res.disciple);

	// 徒弟离线了
	if (!disciple)
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(res.disciple, 0, new PlayerLoadingForMasterGive(res.items, master));
	}
	else
	{
		master->GetItemMgr().MasterGiveEquip(res.items, disciple);
	}
}