#include "USSceneHandler.h"

#include <CLErrorCode.h>
#include <CLSceneDataEntry.h>

#include "USNetwork.h"
#include "USceneMgr.h"
#include "USPlayerMgr.h"
#include "UnionServiceMgr.h"
#include "USPlayer.h"
#include "USApplication.h"

void HandleProtocol(CLProtocol::TeamCopyExitSceneReq& protocol)
{
	//USceneMgr::Instance()->RemoveScene(1, protocol.roleId);
	//USceneMgr::Instance()->RemoveSyncPacket(protocol.roleId);
	//USPlayerMgr::Instance()->DelExpire(protocol.roleId);
	//USPlayerMgr::Instance()->RemoveUSPlayer(protocol.roleId);

	//DebugLogStream << "team copy player return town:" << protocol.roleId << LogStream::eos;
}

void HandleProtocol(CLProtocol::TeamCopyPlayerReconnect& protocol)
{
	//USPlayer* player = USPlayerMgr::Instance()->FindUSPlayer(protocol.roleId);
	//if (NULL == player)
	//{
	//	return;
	//}

	//USceneMgr::Instance()->RemoveScene(1, protocol.roleId);
	//USceneMgr::Instance()->RemoveSyncPacket(protocol.roleId);
	//player->SetCurSceneType(0);

	//UInt32 teamId = USTeamMgr::Instance()->GetPlayerTeamId(protocol.roleId);
	//if (teamId != 0)
	//{
	//	DebugLogStream << "player reconnect id:" << protocol.roleId << LogStream::eos;

	//	if (USTeamMgr::Instance()->IsReconnect(teamId))
	//	{
	//		CLProtocol::TeamCopyReconnectNotify notify;
	//		notify.sceneId = GetTeamCopyValue(TC_VALUE_ATTACK_SCENE_ID);
	//		notify.expireTime = USPlayerMgr::Instance()->PlayerExpireTime(protocol.roleId);
	//		player->SendProtocol(notify);

	//		USDungeonMgr::Instance()->ReconnectEndRace(protocol.roleId);

	//		if (protocol.isSendData != 0)
	//		{
	//			USTeamMgr::Instance()->HandleTeamDataReq(player);
	//		}
	//	}
	//	else
	//	{
	//		USTeamMgr::Instance()->OnPlayerQuit(protocol.roleId);
	//	}
	//}
}

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol, USPlayer* player)
{
	if (NULL == player) return;
	player->OnCheckItemRet(protocol.serial, protocol.result, protocol.counterTimes);
}

void HandleProtocol(CLProtocol::SceneItemThingStartRes& protocol, USPlayer* player)
{
	if (NULL == player) return;
	player->BeginItemThingRes(protocol.retCode, protocol.serialId, protocol.itemThingGuid);
}

void HandleProtocol(CLProtocol::SceneItemThingCommitRes& protocol, USPlayer* player)
{
	if (NULL == player) return;
	player->CommitItemThingRes(protocol.retCode, protocol.itemThingGuid);
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

	UnionServiceMgr::Instance()->OnPlayerSyncSceneObject(roleId, packet);
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

	UnionServiceMgr::Instance()->OnPlayerDeleteSceneObject(roleId, packet);
}

void SceneSyncPlayerMoveFunction::operator()(Avalon::Packet* packet) const
{
	if (USApplication::Instance()->IsCloseMove())
	{
		return;
	}

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

	UnionServiceMgr::Instance()->OnPlayerMove(roleId, packet);
}

