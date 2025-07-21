#ifndef _WS_ROUTER_H_
#define _WS_ROUTER_H_

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
    *@brief 发送到match
    */
    void SendToMatch(Avalon::Protocol& protocol);
    void SendToMatch(Avalon::Packet* packet);

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
	 *@brief 发送到cross
	 */
	void SendToCross(Avalon::Protocol& protocol);
	void SendToCross(Avalon::Packet* packet);

	/**
	 *@brief 发送到sortlist
	 */
	void SendToSortList(Avalon::Protocol& protocol);
	void SendToSortList(Avalon::Packet* packet);

	/**
	*@brief 发送到globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	/**
	*@brief 发送到relay
	*/
	void SendToRelay(UInt32 id, Avalon::Protocol& protocol);

	/**
	 *@brief 转发玩家协议到sortlist
	 */
	void TransmitPlayerProtocolToSortList(ObjID_t playerId, Avalon::Protocol& protocol);


	/**
	 *@brief 广播到网关
	 */
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief 广播到场景
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
	*@brief 发送到战场服务器
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);

	/**
	*@brief 发送到团本服务器
	*/
	void SendToTeamCopy(Avalon::Protocol& protocol);
	void SendToTeamCopy(Avalon::Packet* packet);

	/**
	*@brief 发送到中转服务器
	*/
	void SendToTransfer(TransferHeader& tHeader, Avalon::Protocol& protocol);
	void SendToTransfer(TransferHeader& tHeader, Avalon::Packet* packet);

	/**
	*@brief 发送到Union服务器
	*/
	void SendToUnion(UnionServerFunctionType type, Avalon::Protocol& protocol);
	void SendToUnion(UnionServerFunctionType type, Avalon::Packet* packet);
}

#endif
