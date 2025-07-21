#include "GSGateHandler.h"

#include <AvalonLogger.h>
#include <sstream>

#include "GSPlayerMgr.h"
#include "GSSceneMgr.h"
#include "GSInternalNetwork.h"
#include "GSApplication.h"
#include "GSServiceSwitchMgr.h"

void HandleProtocol(CLProtocol::GateTransmitPacket& protocol)
{	
	UInt32 nodeid = protocol.GetConnID();
	//ServerType nodeType = (ServerType)CL_GET_SERVERTYPE_FROM_NODEID(nodeid);
	UInt32 protocolId = protocol.packet->GetID();

	if (protocolId == 500502)
	{
		return;
	}

	{
		// 移动包就不记日志了
		if (GSApplication::Instance()->IsLogTransProtocol())
		{
			static std::vector<UInt32> unlogProtocols = { CLProtocol::SCENE_SYNC_PLAYER_MOVE, CLProtocol::SCENE_SYNC_SELF, CLProtocol::SCENE_SYNC_SCENEOBJECT, CLProtocol::SCENE_SYNC_OBJECTPROPERTY, CLProtocol::SCENE_DELETE_SCENEOBJECT };
			if (std::find(unlogProtocols.begin(), unlogProtocols.end(), protocolId) == unlogProtocols.end())
			{
				InfoLogStream << "player(" << protocol.id << ") trans protocol(" << protocol.packet->GetID() << ") from node(" << nodeid << ")." << LogStream::eos;
			}
		}
	}

	{
		// 记录团本消息
		if (GSApplication::Instance()->IsLogTeamCopyProtocol())
		{
			UInt32 servType = CL_GET_SERVERTYPE_FROM_PROTOCOLID(protocolId);
			UInt32 funcType = (protocolId % CL_SERVERTYPE_OFFSET_IN_PROTOCOLID) / 100;
			if (servType == ST_TEAMCOPY || servType == ST_DUNGEON_SVR || funcType == CLProtocol::FUNCTION_DUNGEON)
			{
				InfoLogStream << "player(" << protocol.id << ") trans teamcopy protocol(" << protocol.packet->GetID() << ") from node(" << nodeid << ")." << LogStream::eos;
			}
		}
	}
	
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.id);
	if(player != NULL)
	{
		player->SendPacket(protocol.packet);
	}
	else
	{
		ErrorLogStream << "player(" << protocol.id << ") trans protocol(" << protocol.packet->GetID() << ") from node(" << nodeid << "), can't find player." << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::GateBroadcastAround& protocol)
{
	GSScene* scene = GSSceneMgr::Instance()->FindScene(protocol.sceneid);
	if(scene == NULL) return;

	//scene->BroadcastAround(protocol.grid, protocol.packet,protocol.exceptobj);
	for (auto id : protocol.targetObjList)
	{
		auto player = GSPlayerMgr::Instance()->FindPlayerByID(id);
		if (player)
		{
			player->SendPacket(protocol.packet);
		}
	}
}

void HandleProtocol(CLProtocol::GateBroadcastExcept& protocol)
{
	GSScene* scene = GSSceneMgr::Instance()->FindScene(protocol.sceneid);
	if(scene == NULL) return;

	//scene->BroadcastExcept(protocol.grid1,protocol.grid2,protocol.packet,protocol.exceptobj);
	for (auto id : protocol.targetObjList)
	{
		auto player = GSPlayerMgr::Instance()->FindPlayerByID(id);
		if (player)
		{
			player->SendPacket(protocol.packet);
		}
	}
}

void HandleProtocol(CLProtocol::GateBroadcastAll& protocol)
{
#ifdef _DEBUG
	InfoLogStream << "[cross] recv broadcast msg (" << protocol.packet->GetID() << ")." << LogStream::eos;
#endif

	GSPlayerMgr::Instance()->Broadcast(protocol.bZone, protocol.level, protocol.packet);
}

void HandleProtocol(CLProtocol::GateSyncPlayerBaseInfo& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.playerId);
	if(player != NULL)
	{
		player->SetLevel(protocol.level);
	}
	else
	{
		ErrorLogStream << "player(" << protocol.playerId << ") new level(" << protocol.level << ") can't find player..." << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	Avalon::NetConnectionPtr node = GSInternalNetwork::Instance()->FindConnection(protocol.destnode);
	if (node == NULL)
	{
#ifdef _DEBUG
		ErrorLogStream << "can not find connection(" << protocol.destnode << ") when transmit packet." << LogStream::eos;
#endif
		return;
	}

	node->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::SysServiceSwitchChangeStatus& protocol)
{
	if (protocol.open)
	{
		GSServiceSwitchMgr::Instance()->OpenService((ServiceType)protocol.type);
	}
	else
	{
		GSServiceSwitchMgr::Instance()->CloseService((ServiceType)protocol.type);
	}

}

