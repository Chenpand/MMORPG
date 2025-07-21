#ifndef _AS_ROUTER_H_
#define _AS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief ��û��world������
	 */
	bool IsWorldConnected();

	/**
	 *@brief ������Ϣ����¼��
	 */
	void SendToLogin(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵�world
	 */
	void SendToWorld(Avalon::Packet* packet);
	void SendToWorld(Avalon::Protocol& protocol);

	/**
	 *@brief �㲥������
	 */
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief �㲥������
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵���Ӧ��                                                                     
	 */
	void SendToZone(UInt32 zoneId, Avalon::Protocol& protocol);
	void SendToZone(UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief �㲥��������                                                                     
	 */
	void BroadcastToZones(Avalon::Protocol& protocol);
	void BroadcastToZones(Avalon::Packet* packet);
}

#endif
