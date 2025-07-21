#ifndef _GS_ROUTER_H_
#define _GS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief ������Ϣ��Admin
	 */
	void SendToAdmin(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵�world
	 */
	void SendToWorld(Avalon::Protocol& protocol);

	/**
	*@brief ���͵�match
	*/
	void SendToMatch(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵�center
	 */
	void SendToCenter(Avalon::Protocol& protocol);
	void SendToCenter(Avalon::Packet* packet);

	/**
	 *@brief ���͵���Ӧ��Ϸ��                                                                     
	 */
	void SendToZone(UInt32 zoneId, Avalon::Protocol& protocol);
	void SendToZone(UInt32 zoneId, Avalon::Packet* packet);

	/**
	*@brief ���͵�globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	/**
	*@brief ���͵�battleServer
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);
}

#endif
