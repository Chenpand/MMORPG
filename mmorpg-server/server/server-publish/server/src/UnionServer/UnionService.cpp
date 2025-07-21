#include "UnionService.h"
#include "GameZoneMgr.h"
#include <CLSysProtocol.h>


UnionService::UnionService() : m_PlayerMgr(*this), m_SceneMgr(*this)
{
}

UnionService::~UnionService()
{
}

bool UnionService::Init()
{
	m_PlayerMgr.Init();
	m_SceneMgr.Init();
	return true;
}

void UnionService::OnTick(const Avalon::Time & now)
{
	m_PlayerMgr.OnTick(now);
	m_SceneMgr.OnTick(now);
}

void UnionService::OnConnected(GameZone* zone)
{

}

void UnionService::OnAddNewPlayer(USPlayer * player)
{
}

void UnionService::OnRemovePlayer(UInt64 roleId)
{
}

bool UnionService::OnCheckAddNewPlayer(UInt64 roleId, UInt32 accid)
{
	return true;
}

void UnionService::OnPlayerDisconnect(UInt64 roleId)
{

}

void UnionService::OnPlayerMove(UInt64 roleID, Avalon::Packet* packet)
{

}

void UnionService::OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet * packet)
{
}

void UnionService::OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet * packet)
{
}


void UnionService::OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet * packet)
{
}

void UnionService::OnSceneReady(CLProtocol::UnionSceneIsReady & protocol)
{

}

void UnionService::OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID)
{
}

void UnionService::OnActivityNotify(CLProtocol::UnionNotifyActivityInfo& protocol)
{

}

void UnionService::AddNewPlayer(CLProtocol::UnionServerAddPlayerReq & protocol)
{
	if (!OnCheckAddNewPlayer(protocol.roleId, protocol.accId))
	{
		ErrorLogStream << "OnCheckAddNewPlayer faild, accid is " << protocol.accId << " roleID is " << protocol.roleId << LogStream::eos;
		return;
	}
	auto player = m_PlayerMgr.AddUSPlayer(protocol.roleId);
	if (player == nullptr)
	{
		return;
	}

	player->SetID(protocol.roleId);
	player->SetAccID(protocol.accId);
	player->SetZoneID(CL_GET_ZONEID_FROM_NODEID(protocol.sceneNode));
	player->SetConnId(protocol.GetConnID());
	player->SetName(protocol.name);
	player->SetOccu(protocol.occu);
	player->SetLevel(protocol.level);
	player->SetVipLevel(protocol.vipLvl);
	player->SetSceneNodeId(protocol.sceneNode);
	player->SetWorldNodeId(protocol.worldNode);
	player->SetGuildId(protocol.guildId);
	player->SetAwaken(protocol.awaken);
	player->SetServer(protocol.server);

	OnAddNewPlayer(player);
}

void UnionService::RemovePlayer(CLProtocol::UnionServerDelPlayerReq & protocol)
{
	m_PlayerMgr.RemoveUSPlayer(protocol.roleId);
	OnRemovePlayer(protocol.roleId);
}

void UnionService::BroadcastToZone(Avalon::Packet * packet, ServerType type)
{

	switch (type)
	{
	case ST_SCENE:
		break;
	case ST_WORLD:
		GameZoneMgr::Instance()->BroadcastToGameZoneWorld(packet);
		break;
	default:
		GameZoneMgr::Instance()->BroadcastToGameZone(packet);
		break;
	}
}

void UnionService::BroadcastToZone(Avalon::Protocol& protocol, ServerType type)
{
	switch (type)
	{
	case ST_SCENE:
		GameZoneMgr::Instance()->BroadcastToGameZoneScene(protocol);
		break;
	case ST_WORLD:
		GameZoneMgr::Instance()->BroadcastToGameZoneWorld(protocol);
		break;
	default:
		GameZoneMgr::Instance()->BroadcastToGameZone(protocol);
		break;
	}
}