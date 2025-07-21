#ifndef _RS_ROUTER_H_
#define _RS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	*@brief ��û�б��÷�����
	*/
	bool HasSlaverRecord();

	/**
	 *@brief ���͵�dbproxy
	 */
	void SendToDBProxy(Avalon::Protocol& protocol);
	void SendToDBProxy(Avalon::Packet* packet);

	/**
	 *@brief ת������                                                                     
	 */
	void TransToRecord(UInt32 zoneId, Avalon::Protocol& protocol);
	void TransToRecord(UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief ת������                                                                     
	 */
	void TransRequest(UInt32 destZoneId, UInt32 nodeId, Avalon::Packet* packet);

	/**
	 *@brief ת���ظ�                                                                     
	 */
	void TransResponse(UInt32 destZoneId, UInt32 destNodeId, Avalon::Protocol& protocol);
	void TransResponse(UInt32 destZoneId, UInt32 destNodeId, Avalon::Packet* packet);

	/**
	 *@brief �㲥������                                                                     
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/**
	*@brief ת������
	*/
	void SendToMasterRecord(Avalon::Protocol& protocol);
	void SendToMasterRecord(Avalon::Packet* packet);

	/**
	*@brief ת������
	*/
	void SendToSlaverRecord(Avalon::Protocol& protocol);
	void SendToSlaverRecord(Avalon::Packet* packet);

	/**
	*@brief ת������
	*/
	void TransRequestToSlaver(UInt32 nodeId, Avalon::Packet* packet);

	/**
	*@brief ת���ظ�
	*/
	void TransResponseFromSlaver(UInt32 destNodeId, Avalon::Protocol& protocol);
	void TransResponseFromSlaver(UInt32 destNodeId, Avalon::Packet* packet);
}

#endif
