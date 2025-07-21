#ifndef _GS_ROUTER_H_
#define _GS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief 发送消息到Admin
	 */
	void SendToAdmin(Avalon::Protocol& protocol);

	/**
	 *@brief 发送到world
	 */
	void SendToWorld(Avalon::Protocol& protocol);

	/**
	*@brief 发送到match
	*/
	void SendToMatch(Avalon::Protocol& protocol);

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
	*@brief 发送到globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	/**
	*@brief 发送到battleServer
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);
}

#endif
