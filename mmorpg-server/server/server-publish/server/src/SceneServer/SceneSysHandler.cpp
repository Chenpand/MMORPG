#include "SceneSysHandler.h"
#include "PlayerMgr.h"
#include "SceneGMSystem.h"
#include "SSServiceSwitchMgr.h"
#include "SSDungeonConfigMgr.h"
#include <CLCounterDefine.h>

#include "SSNetwork.h"
#include "SSRouter.h"
#include "Player.h"
#include "SceneChampionMgr.h"

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol)
{
	GMSystem::Instance()->ExecuteCommand(protocol.playerid, protocol.cmdname, protocol.params);
}

void HandleProtocol(CLProtocol::WorldNotifyWorldLevelToS &protocol)
{
	WORLD_LEVEL = protocol.uWorldLevel;
}

void HandleProtocol(CLProtocol::SysServiceSwitchChangeStatus& protocol)
{
	if (protocol.open)
	{
		SSServiceSwitchMgr::Instance()->OpenService((ServiceType)protocol.type);
	}
	else
	{
		SSServiceSwitchMgr::Instance()->CloseService((ServiceType)protocol.type);
	}
	
}

void HandleProtocol(CLProtocol::SysDungeonConfigUpdate& protocol)
{
}

void HandleProtocol(CLProtocol::SysDungeonOpenStatusChange& protocol)
{
	
}

void HandleProtocol(CLProtocol::SysPlayerForeverDeleted& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (player)
	{
		player->GetMasterDailyTaskMgr().AbandonAllTask(false);
		player->GetMasterAcademicGrowthTaskMgr().AbandonAllTask(false);
	}
	else
	{
		MasterDailyTaskMgr::ClearOfflinePlayerAllTask(protocol.roleId);
		MasterAcademicGrowthTaskMgr::ClearOfflinePlayerAllTask(protocol.roleId);
	}
}

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
	if (player)
	{
		player->SendPacket(protocol.packet);
	}
}

void HandleProtocol(CLProtocol::SysTransferProtocol& protocol)
{
	SSNetwork::Instance()->ExecuteTransferPacket(protocol.tHeader, protocol.packet);
}

void HandleProtocol(CLProtocol::GateNotifyLeaveConnect& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (NULL == player)
		return;

	if (player->GetTeamCopyMgr().IsTeamCopy())
	{
		Router::SendToTeamCopy(protocol);
	}

	if (SceneChampionMgr::Instance()->IsPlayerInChampion(player->GetID()))
	{
		//SceneChampionMgr::Instance()->OnPlayerDisconnect(player, protocol);
	}
}

