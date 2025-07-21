#ifndef _SS_ROUTER_H_
#define _SS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief 发送到admin
	 */
	void SendToAdmin(Avalon::Protocol& protocol);
	void SendToAdmin(Avalon::Packet* packet);

	/**
	 *@brief 发送到World
	 */
	void SendToWorld(Avalon::Protocol& protocol);
	void SendToWorld(Avalon::Packet* packet);

	/**
	 *@brief 发送到social
	 */
	void SendToSocial(Avalon::Protocol& protocol);
	void SendToSocial(Avalon::Packet* packet);

	/**
	 *@brief 发送到center
	 */
	void SendToCenter(Avalon::Protocol& protocol);
	void SendToCenter(Avalon::Packet* packet);

	/**
	 *@brief 发送到对应游戏区                                                                     
	 */
	void SendToZone(UInt32 zoneId, Avalon::Protocol& protocol);
	void SendToZone(UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief 广播到所有游戏区                                                       
	 */
	void BroadcastToZones(Avalon::Protocol& protocol);
	void BroadcastToZones(Avalon::Packet* packet);

	/**
	 *@brief 发送到跨服服务器
	 */
	void SendToCross(Avalon::Protocol& protocol);
	void SendToCross(Avalon::Packet* packet);

	/**
	*@brief 发送到录像服务器
	*/
	void SendToReplay(Avalon::Protocol& protocol);
	void SendToReplay(Avalon::Packet* packet);

	/**
	 *@brief 发送到sortlist                                                                     
	 */
	void SendToSortList(Avalon::Protocol& protocol);
	void SendToSortList(Avalon::Packet* packet);

	/**
	 *@brief 广播到网关
	 */
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief 发送到另一个场景服务器
	 */
	void SendToScene(UInt32 nodeid,Avalon::Protocol& protocol);
	void SendToScene(UInt32 nodeid,Avalon::Packet* packet);

	/**
	 *@brief 广播到所有场景
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/**
	 *@brief 广播到所有玩家
	 */
	void BroadcastToPlayer(Avalon::Protocol& protocol, UInt8 bZone = 1);
	void BroadcastToPlayer(Avalon::Packet* packet, UInt8 bZone = 1);

	/**
	 *@brief 广播到指定等级以上的玩家
	 */
	void BroadcastToPlayerByLvl(UInt16 level, Avalon::Protocol& protocol, UInt8 bZone = 1);
	void BroadcastToPlayerByLvl(UInt16 level, Avalon::Packet* packet, UInt8 bZone = 1);

	/**
	 *@brief 转发给玩家
	 */
	void TransmitToPlayer(ObjID_t id, Avalon::Protocol& protocol);
	void TransmitToPlayer(ObjID_t id, Avalon::Packet* packet);

    /**
    *@brief 发到MatchServer
    */
    void SendToMatch(Avalon::Protocol& protocol);
    
    /**
    *@brief 发到RelayServer
    */
    void SendToRelay(UInt32 nodeId, Avalon::Protocol& protocol);

	/**
	*@brief 发到WSceneServer
	*/
	void SendToWScene(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToBScene(UInt32 nodeId, Avalon::Protocol& protocol);

	/**
	*@brief WScene发到自己所在的Scene
	*/
	void SendToSelfScene(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToSelfScenePlayer(UInt32 nodeId, ObjID_t id, Avalon::Protocol& protocol);

	/**
	*@brief 发送到战场服务器
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);

	/**
	*@brief 发送到地下城服务器
	*/
	void SendToDungeon(Avalon::Protocol& protocol);
	void SendToDungeon(Avalon::Packet* packet);
}

#endif
