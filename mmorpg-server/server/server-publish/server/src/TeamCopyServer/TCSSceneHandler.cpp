#include "TCSSceneHandler.h"

#include <CLErrorCode.h>

#include "TCSNetwork.h"
#include "TCSceneMgr.h"
#include "TCSPlayerMgr.h"
#include "TCSTeamMgr.h"
#include "TCSDungeonMgr.h"
#include "CLSceneDataEntry.h"

void HandleProtocol(CLProtocol::TeamCopyExitSceneReq& protocol)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.roleId);
	if (player != NULL)
	{
		TCSceneMgr::Instance()->RemoveToPrepareScene(player->GetTeamType(), protocol.roleId);
	}

	TCSceneMgr::Instance()->RemoveSyncPacket(protocol.roleId);
	TCSTeamMgr::Instance()->OnPlayerQuit(protocol.roleId);
	TCSPlayerMgr::Instance()->DelExpire(protocol.roleId);
	TCSPlayerMgr::Instance()->RemoveTCSPlayer(protocol.roleId);

	DebugLogStream << "team copy player return town:" << protocol.roleId << LogStream::eos;
}

void HandleProtocol(CLProtocol::TeamCopyPlayerReconnect& protocol)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.roleId);
	if (NULL == player)
	{
		return;
	}

	TCSceneMgr::Instance()->RemoveToPrepareScene(player->GetTeamType(), protocol.roleId);
	TCSceneMgr::Instance()->RemoveSyncPacket(protocol.roleId);
	player->SetSceneType(0);
	player->SetSceneSubType(0);

	UInt32 teamId = TCSTeamMgr::Instance()->GetPlayerTeamId(protocol.roleId);
	if (teamId != 0)
	{
		DebugLogStream << "player reconnect id:" << protocol.roleId << LogStream::eos;

		if (TCSTeamMgr::Instance()->IsReconnect(teamId))
		{
			CLProtocol::TeamCopyReconnectNotify notify;
			notify.sceneId = GetTCAttackScene(player->GetTeamType());
			notify.expireTime = TCSPlayerMgr::Instance()->PlayerExpireTime(protocol.roleId);
			player->SendProtocol(notify);

			TCSDungeonMgr::Instance()->ReconnectEndRace(protocol.roleId);

			if (protocol.isSendData != 0)
			{
				TCSTeamMgr::Instance()->HandleTeamDataReq(player, player->GetTeamType());
			}
		}
		else
		{
			TCSTeamMgr::Instance()->OnPlayerQuit(protocol.roleId);
		}
	}
}

void TeamCopyEnterSceneReqFunction::operator()(Avalon::Packet* packet) const
{
	UInt32 sceneNode = 0;
	UInt32 worldNode = 0;
	UInt32 accId = 0;
	ObjID_t roleId = 0;
	std::string name;
	UInt8 occu = 0;
	UInt16 level = 0;
	UInt8 vipLvl = 0;
	UInt64 guildId = 0;
	UInt8 awaken = 0;
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	try
	{
		stream & sceneNode & worldNode & accId & roleId & name & occu & level & vipLvl & guildId & awaken;
	}
	catch (Avalon::NetSerializeException* e)
	{
		ErrorLogStream << "parse player info error!" << LogStream::eos;
		return;
	}

	TCSPlayer* tcsPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(roleId);
	if (NULL == tcsPlayer)
	{
		tcsPlayer = new TCSPlayer();
		tcsPlayer->SetID(roleId);
		tcsPlayer->SetAccID(accId);
		tcsPlayer->SetZoneID(CL_GET_ZONEID_FROM_NODEID(sceneNode));
		tcsPlayer->SetConnId(packet->GetConnID());
		TCSPlayerMgr::Instance()->AddTCSPlayer(tcsPlayer);
	}

	tcsPlayer->SetName(name);
	tcsPlayer->SetOccu(occu);
	tcsPlayer->SetLevel(level);
	tcsPlayer->SetVipLevel(vipLvl);
	tcsPlayer->SetSceneNodeId(sceneNode);
	tcsPlayer->SetWorldNodeId(worldNode);
	tcsPlayer->SetGuildId(guildId);
	tcsPlayer->SetAwaken(awaken);
}

void SceneSyncSceneObjectFunction::operator()(Avalon::Packet* packet) const
{
	UInt8 isEnter = 0;
	UInt32 destSceneId = 0;
	ObjID_t roleId = 0;

	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	try
	{
		stream & isEnter & destSceneId & roleId;
	}
	catch (Avalon::NetSerializeException* e)
	{
		ErrorLogStream << "parse player info error!" << LogStream::eos;
		return;
	}

	TCSPlayer* tcsPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(roleId);
	if (NULL == tcsPlayer)
	{
		return;
	}

	SceneDataEntry* sceneEntry = SceneDataEntryMgr::Instance()->FindEntry(destSceneId);
	if (NULL == sceneEntry)
	{
		ErrorLogStream << "not find scene config id:" << destSceneId << LogStream::eos;
		return;
	}

	TCSceneMgr::Instance()->AddSyncPacket(roleId, packet);

	UInt32 teamType = GetTCType(destSceneId);
	if (sceneEntry->IsTeamCopyPrepareScene())
	{
		TCSceneMgr::Instance()->BroadcastPacket(packet, teamType);
		TCSceneMgr::Instance()->SyncAllObjectToPlayer(tcsPlayer, teamType);
		TCSceneMgr::Instance()->AddToPrepareScene(teamType, roleId);

		DebugLogStream << "player:" << roleId << " enter scene:" << destSceneId << LogStream::eos;
	}
	else if (sceneEntry->IsTeamCopyAttackScene())
	{
		TCSTeamMgr::Instance()->BroadcastToTeam(packet, TCSTeamMgr::Instance()->GetPlayerTeamId(roleId), roleId);
		TCSTeamMgr::Instance()->SyncAllObjectToPlayer(tcsPlayer);
		TCSceneMgr::Instance()->RemoveToPrepareScene(teamType, roleId);

		DebugLogStream << "player:" << roleId << " enter scene:" << destSceneId << LogStream::eos;
	}
	else
	{
		tcsPlayer->SetSceneType(sceneEntry->type);
		tcsPlayer->SetSceneSubType(sceneEntry->subType);
		return;
	}

	TCSPlayerMgr::Instance()->DelExpire(roleId);
	tcsPlayer->SetTeamType(teamType);
	if (sceneEntry->IsTeamCopyPrepareScene())
	{
		tcsPlayer->PlayerInfoToClient(teamType);
	}

	tcsPlayer->SetSceneType(sceneEntry->type);
	tcsPlayer->SetSceneSubType(sceneEntry->subType);
}

void SceneDeleteSceneObjectFunction::operator()(Avalon::Packet* packet) const
{
	UInt64 roleId = 0;
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	try
	{
		stream & roleId;
	}
	catch (Avalon::NetSerializeException* e)
	{
		ErrorLogStream << "parse player info error!" << LogStream::eos;
		return;
	}

	TCSPlayer* tcsPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(roleId);
	if (NULL == tcsPlayer)
	{
		return;
	}

	if (tcsPlayer->GetSceneSubType() == SCENE_SUB_TEAM_COPY_PREPARE)
	{
		TCSceneMgr::Instance()->BroadcastPacket(packet, tcsPlayer->GetTeamType());
	}
	else if (tcsPlayer->GetSceneSubType() == SCENE_SUB_TEAM_COPY_ATTACK)
	{
		TCSTeamMgr::Instance()->BroadcastToTeam(packet, TCSTeamMgr::Instance()->GetPlayerTeamId(roleId), roleId);
	}
}

void SceneSyncPlayerMoveFunction::operator()(Avalon::Packet* packet) const
{
	UInt64 roleId = 0;
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	try
	{
		stream & roleId;
	}
	catch (Avalon::NetSerializeException* e)
	{
		ErrorLogStream << "parse player info error!" << LogStream::eos;
		return;
	}

	TCSPlayer* tcsPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(roleId);
	if (NULL == tcsPlayer)
	{
		return;
	}

	if (tcsPlayer->GetSceneSubType() == SCENE_SUB_TEAM_COPY_PREPARE)
	{
		TCSceneMgr::Instance()->BroadcastPacket(packet, tcsPlayer->GetTeamType());
	}
	else if (tcsPlayer->GetSceneSubType() == SCENE_SUB_TEAM_COPY_ATTACK)
	{
		TCSTeamMgr::Instance()->BroadcastToTeam(packet, TCSTeamMgr::Instance()->GetPlayerTeamId(roleId), roleId);
	}
}

