#include "SceneSyncObjectHandler.h"

#include "PlayerMgr.h"
#include "TeamMgr.h"
#include "Scene.h"
#include "PlayerLoadCallback.h"
#include "GameParamMgr.h"

class PlayerLoadingCallbackOnRelieveMaster : public PlayerLoadingCallback
{
public:
	PlayerLoadingCallbackOnRelieveMaster(ObjID_t masterId) : m_masterId(masterId){}
	virtual void OnFinish(Player* player)
	{
		if (!player)
		{
			return;
		}

		InfoLogStream << "player(" << player->GetID() << ") loading finish, relieve master" << LogStream::eos;

		player->OnRelationRelieveMaster(m_masterId);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	/**
	*@brief º”‘ÿ ß∞‹
	*/
	virtual void OnFailure(Player* player)
	{
		if (player->GetGameStatus() == PS_INIT)
		{
			// º”‘ÿ ß∞‹£¨…ËŒ™¥˙¥›ªŸ◊¥Ã¨
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief º”‘ÿ≥¨ ±
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	ObjID_t m_masterId;
};

class PlayerLoadingForApprenticeMaster : public PlayerLoadingCallback
{
public:
	PlayerLoadingForApprenticeMaster(ObjID_t masterId) : m_masterId(masterId){}
	virtual void OnFinish(Player* player)
	{
		if (!player)
		{
			return;
		}

		InfoLogStream << "player(" << player->GetID() << ") loading finish, apprentice master" << LogStream::eos;

		player->OnRelationApprenticeMaster(m_masterId);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	/**
	*@brief º”‘ÿ ß∞‹
	*/
	virtual void OnFailure(Player* player)
	{
		if (player->GetGameStatus() == PS_INIT)
		{
			// º”‘ÿ ß∞‹£¨…ËŒ™¥˙¥›ªŸ◊¥Ã¨
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief º”‘ÿ≥¨ ±
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	ObjID_t m_masterId;
};

class PlayerLoadingForDiscipleFinishSchool : public PlayerLoadingCallback
{
public:
	PlayerLoadingForDiscipleFinishSchool(ObjID_t masterId) : m_masterId(masterId){}
	virtual void OnFinish(Player* player)
	{
		if (!player)
		{
			return;
		}

		InfoLogStream << "player(" << player->GetID() << ") loading finish, disciple finish school" << LogStream::eos;

		player->OnRelationDiscipleFinishSchool(m_masterId);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	/**
	*@brief º”‘ÿ ß∞‹
	*/
	virtual void OnFailure(Player* player)
	{
		if (player->GetGameStatus() == PS_INIT)
		{
			// º”‘ÿ ß∞‹£¨…ËŒ™¥˙¥›ªŸ◊¥Ã¨
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief º”‘ÿ≥¨ ±
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	ObjID_t m_masterId;
};

void HandleProtocol(CLProtocol::SceneSyncSocialData& sync)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(sync.playerId);
	if(player == NULL) return;

	Team* team = TeamMgr::Instance()->FindTeam(sync.teamId);
	if(player->GetTeam() != team)
	{
		player->SetTeam(team);
		player->UpdateTeamInfo();
		player->SyncTeamAround(false);
		player->SyncTeamAroundToMe();
	}
}

void HandleProtocol(CLProtocol::WorldAddExpReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player == NULL) return;
	player->IncExp("world_add_exp", req.exp);
}

//∑œ∆˙
void HandleProtocol(CLProtocol::SceneWatchPlayerReq& req)
{
	Player* target = PlayerMgr::Instance()->FindNormalPlayer(req.target);
	if(target == NULL) return;

	target->SendWatchData(req.watcher, req.tribe);
}

void HandleProtocol(CLProtocol::SceneSyncPunishment& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(player == NULL) return;

	player->SetPunishFlag(req.punishflag);
	player->SetForbitTalkDueTime(req.forbitTalkDueTime);
}

void HandleProtocol(Player* player, CLProtocol::SceneObjectDataReq& req)
{
	if(req.id == player->GetID()) return;
	Scene* scene = player->GetScene();
	SceneObject* obj = scene->FindObject(req.id);
	if(obj == NULL || !scene->IsInBroadcastZone(player,obj)) return;

	Avalon::Packet* packet = obj->MakeSyncObjectPacket(false);
	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() <<
		") request object:" << req.id << " data!" << LogStream::eos; 
}

void HandleProtocol(Player* player, CLProtocol::SceneSetCustomData& req)
{
	if (!player)
	{
		return;
	}

	player->SetCustomField(req.data);
}

void HandleProtocol(CLProtocol::ScenePlayerPowerReq& req)
{
}

void HandleProtocol(CLProtocol::SceneSyncEvent& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;
	
	player->EventProcess((SyncEventType)protocol.type, protocol.param1, protocol.param2);
}

void HandleProtocol(Player* player, CLProtocol::SceneClearRedPoint& req)
{
	if (!player)
	{
		return;
	}

	player->ClearRedPoint((RedPointFlag)req.flag);
}

void HandleProtocol(CLProtocol::WorldAccountCounterSync& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
		return;

	if (protocol.accCounter.type == ACC_COUNTER_ENERGY_COIN)
	{
		player->SetEnergyCoin(protocol.accCounter.num);
	}
}

void HandleProtocol(CLProtocol::WorldAccountDataSync& protocol)
{
	/*Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
		return;

	if (protocol.type == OFFLINE_TIME)
	{
		UInt64 offTime = Avalon::StringUtil::ConvertToValue<UInt64>(protocol.val);
		Avalon::Time offlineTime(offTime * 1000);
		UInt64 offlineDay = GetDayBeginTime2(offlineTime);
		UInt64 currentDay = GetDayBeginTime2(CURRENT_TIME);
	}*/
}

void HandleProtocol(CLProtocol::SceneGameParamSync& protocol)
{
	for (auto& iter : protocol.gameParamMap)
	{
		GameParamMgr::Instance()->SetValue(iter.first.c_str(), iter.second, false);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSelectObject& protocol)
{
	player->SyncSelfData(false);
}