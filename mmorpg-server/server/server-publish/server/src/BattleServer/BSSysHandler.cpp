#include "BSSysHandler.h"
#include "BSNetwork.h"
#include "GameZoneMgr.h"
#include "BSBattleMgr.h"
#include "BSPlayerMgr.h"
#include "BSRouter.h"
#include "BSLostBattleMgr.h"
#include <CLBattleDefine.h>

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	// 除了要发到wscene，那么需要转发到adminserver 
	auto serverType = CL_GET_SERVERTYPE_FROM_NODEID(protocol.destnode);
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.destnode);
	if (serverType == ST_BSCENE)
	{
		Avalon::NetConnectionPtr node = BSNetwork::Instance()->FindConnection(protocol.destnode);
		if (node == NULL)
		{
			return;
		}

		node->SendPacket(protocol.packet);
		return;
	}

	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		return;
	}

	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol)
{
	auto serverType = CL_GET_SERVERTYPE_FROM_NODEID(protocol.nodeId);
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.nodeId);
	if (serverType == ST_BSCENE)
	{
		Avalon::NetConnectionPtr node = BSNetwork::Instance()->FindConnection(protocol.nodeId);
		if (node == NULL)
		{
			return;
		}

		node->SendPacket(protocol.packet);
		return;
	}

	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		return;
	}

	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::BattleExitSceneRes& protocol)
{
	InfoLogStream << "request exit battle scene battleType : "  << protocol.battleType << ", roleId : " <<
		protocol.roleId << ", battleId : " << protocol.battleId << LogStream::eos;

	if (protocol.battleType == BTY_DILAO)
	{
		BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
		if (!player) return;

		BSLostBattle* battle = BSLostBattleMgr::Instance()->FindLostBattle(protocol.battleId);
		if (!battle) return;
		battle->SendToBattleScene(protocol);
		return;
	}

	BSBattleMgr::Instance()->OnPlayerLeaveBattleScene(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol)
{
	auto player = BSPlayerMgr::Instance()->FindPlayer(protocol.playerId);
	if (!player)
	{
		return;
	}
	BSNetwork::Instance()->SendToBScene(player->GetDstSceneNodeId(), protocol);
}

void HandleProtocol(CLProtocol::GateTransmitPacket& protocol)
{
	BSPlayerMgr::Instance()->ExecuteProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitInNodesType& protocol)
{
	if (protocol.destnodetype == (UInt8)ST_DUNGEON_SVR)
	{
		Router::SendToDungeon(protocol.packet);
		return;
	}
}

