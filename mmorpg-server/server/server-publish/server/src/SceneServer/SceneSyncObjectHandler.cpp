#include "SceneSyncObjectHandler.h"

#include <CLGlobalActivityProtocol.h>

#include "PlayerMgr.h"
#include "TeamMgr.h"
#include "Scene.h"
#include "SSFigureStatueMgr.h"
#include "PlayerLoadCallback.h"
#include "GameParamMgr.h"
#include "SSServiceSwitchMgr.h"
#include "SSApplication.h"
#include "HonorMgr.h"
#include "SSRouter.h"

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

//废弃
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
	if (protocol.type == SET_RELATION_RELIEVE_MASTER) //解除师徒关系
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
		if (!player)
		{
			//Player::HandleOfflineRelieveMaster(protocol.playerId);
			PlayerMgr::Instance()->LoadOfflinePlayerCb(protocol.playerId, 0, new PlayerLoadingCallbackOnRelieveMaster(protocol.param1));
		}
		else if (player->GetGameStatus() != PS_INIT)
		{
			player->OnRelationRelieveMaster(protocol.param1);
		}
		else
		{
			player->AddLoadingCallback(new PlayerLoadingCallbackOnRelieveMaster(protocol.param1));
		}
		return;
	}
	else if (protocol.type == SET_RELATION_DISCIPLE_APPRENTICE) //徒弟拜师成功
	{
		Player* disciple = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
		if (!disciple)
		{
			PlayerMgr::Instance()->LoadOfflinePlayerCb(protocol.playerId, 0, new PlayerLoadingForApprenticeMaster(protocol.param1));
		}
		else if (disciple->GetGameStatus() != PS_INIT)
		{
			disciple->OnRelationApprenticeMaster(protocol.param1);
		}
		else
		{
			disciple->AddLoadingCallback(new PlayerLoadingForApprenticeMaster(protocol.param1));
		}

		Player* master = PlayerMgr::Instance()->FindPlayer(protocol.param1);
		if (master && master->GetGameStatus() != PS_INIT)
		{
			master->GetAchievementTaskMgr().OnApprentDisciple();
		}
		return;
	}
	else if (protocol.type == SET_RELATION_DISCIPLE_FINISHSCHOOL) //徒弟出师
	{
		Player* master = PlayerMgr::Instance()->FindPlayer(protocol.param1);
		if (master && master->GetGameStatus() != PS_INIT)
		{
			master->SetFinSchDiscipleSum(protocol.param2);
		}
		
		Player* disciple = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
		if (!disciple)
		{
			PlayerMgr::Instance()->LoadOfflinePlayerCb(protocol.playerId, 0, new PlayerLoadingForDiscipleFinishSchool(protocol.param1));
		}
		else if (disciple->GetGameStatus() != PS_INIT)
		{
			disciple->OnRelationDiscipleFinishSchool(protocol.param1);
		}
		else
		{
			disciple->AddLoadingCallback(new PlayerLoadingForDiscipleFinishSchool(protocol.param1));
		}
		return;
	}
	else if (protocol.type == SET_TEAM_COPY_CLEAR_DUNGEON)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
		if (player != NULL)
		{
			player->GetTeamCopyMgr().ClearDungeonEvent(protocol.param1, protocol.param2, protocol.param3);
			Router::SendToWorld(protocol);
		}

		return;
	}

	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
	if (player == NULL) return;
	
	player->WorldEventProcess((SyncEventType)protocol.type, protocol.param1, protocol.param2);
}

void HandleProtocol(Player* player, CLProtocol::SceneClearRedPoint& req)
{
	if (!player)
	{
		return;
	}

	player->ClearRedPoint((RedPointFlag)req.flag);
}

void HandleProtocol(CLProtocol::WorldFigureStatueSync& protocol)
{
	SSFigureStatueMgr::Instance()->SetFigureStatue(protocol.figureStatue);
}

void HandleProtocol(CLProtocol::WorldAccountCounterSync& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
		return;

	player->SetAccountCounter(protocol.accCounter.type, protocol.accCounter.num);

	if (protocol.accCounter.type == ACC_COUNTER_ENERGY_COIN)
	{
		player->GetActiveTaskMgr().ReturnEnergyShop();
	}
}

void HandleProtocol(CLProtocol::WorldAccountDataSync& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
		return;

	if (protocol.type == OFFLINE_TIME)
	{
		UInt64 offTime = Avalon::StringUtil::ConvertToValue<UInt64>(protocol.val);
		Avalon::Time offlineTime(offTime * 1000);
		UInt64 offlineDay = GetDayBeginTime2(offlineTime);
		UInt64 currentDay = GetDayBeginTime2(CURRENT_TIME);
		if (offTime != 0 && offlineDay != currentDay)
		{
			player->GetActiveTaskMgr().RefreshDailyChallengeHell(1);
		}

		if (offTime != 0)
		{
			player->SetAccOfflineTime((UInt32)offTime);
		}
	}
	else if (protocol.type == MAX_ACC_ROLE_LEVEL)
	{
		UInt32 maxLevel = Avalon::StringUtil::ConvertToValue<UInt32>(protocol.val);
		player->SetAccMaxRoleLevel(maxLevel);
	}
}

void HandleProtocol(CLProtocol::SceneGameParamSync& protocol)
{
	for (auto& iter : protocol.gameParamMap)
	{
		GameParamMgr::Instance()->SetValue(iter.first.c_str(), iter.second, false);

		if (iter.first.compare(HONOR_RANK_TIME) == 0)
		{
			HonorMgr::Instance()->SetSyncRankTime();
			InfoLogStream << "honor rank game param sync val:" << iter.second << LogStream::eos;
		}

		if (iter.first.compare(WHOLE_BARGAIN_DISCOUNT) == 0)
		{
			InfoLogStream << "whole bargain game param sync val:" << iter.second << LogStream::eos;

			class DiscountSyncVisit : public PlayerVisitor
			{
			public:
				DiscountSyncVisit(UInt32 discount)
				{
					sync.discount = discount;
				}

				bool operator()(Player* player)
				{
					if (NULL == player) return true;
					if (player->GetActiveTaskMgr().IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_WHOLE_BARGAIN_SHOP))
					{
						player->SendProtocol(sync);
					}

					return true;
				}
			private:
				CLProtocol::GASWholeBargainDiscountSync sync;
			};
			
			DiscountSyncVisit visitor(iter.second);
			PlayerMgr::Instance()->VisitPlayers(visitor);
		}
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneClientLog& req)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_CLIENT_LOG))
	{
		return;
	}

	// 检查参数合法性
	// 1. 必须都是可显字符
	// 2. 不能用|
	// 3. 长度不能超过40个字符
	std::vector<std::string> strs = {req.name, req.param1, req.param2, req.param3};
	for (const auto& str : strs)
	{
		if (str.length() >= SSApplication::Instance()->GetClientLogMaxLength())
		{
			return;
		}

		for (UInt32 i = 0; i < str.length(); i++)
		{
			auto ch = str[i];
			if (ch < 32 || ch > 126 || ch =='|')
			{
				return;
			}
		}
	}

	player->SendUdpLog("client_log", LOG_CLIENT_LOG, req.name.c_str(), req.param1.c_str(), req.param2.c_str(), req.param3.c_str());
}