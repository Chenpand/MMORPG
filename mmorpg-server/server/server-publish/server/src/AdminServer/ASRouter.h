#ifndef _AS_ROUTER_H_
#define _AS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief 有没有world的连接
	 */
	bool IsWorldConnected();

	/**
	 *@brief 发送消息到登录服
	 */
	void SendToLogin(Avalon::Protocol& protocol);

	/**
	 *@brief 发送到world
	 */
	void SendToWorld(Avalon::Packet* packet);
	void SendToWorld(Avalon::Protocol& protocol);

	/**
	 *@brief 广播到网关
	 */
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief 广播到场景
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/**
	 *@brief 发送到对应区                                                                     
	 */
	void SendToZone(UInt32 zoneId, Avalon::Protocol& protocol);
	void SendToZone(UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief 广播到所有区                                                                     
	 */
	void BroadcastToZones(Avalon::Protocol& protocol);
	void BroadcastToZones(Avalon::Packet* packet);
}

#endif
