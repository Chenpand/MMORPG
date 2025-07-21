#ifndef _RS_ROUTER_H_
#define _RS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	*@brief 有没有备用服务器
	*/
	bool HasSlaverRecord();

	/**
	 *@brief 发送到dbproxy
	 */
	void SendToDBProxy(Avalon::Protocol& protocol);
	void SendToDBProxy(Avalon::Packet* packet);

	/**
	 *@brief 转发请求                                                                     
	 */
	void TransToRecord(UInt32 zoneId, Avalon::Protocol& protocol);
	void TransToRecord(UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief 转发请求                                                                     
	 */
	void TransRequest(UInt32 destZoneId, UInt32 nodeId, Avalon::Packet* packet);

	/**
	 *@brief 转发回复                                                                     
	 */
	void TransResponse(UInt32 destZoneId, UInt32 destNodeId, Avalon::Protocol& protocol);
	void TransResponse(UInt32 destZoneId, UInt32 destNodeId, Avalon::Packet* packet);

	/**
	 *@brief 广播到场景                                                                     
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/**
	*@brief 转发请求
	*/
	void SendToMasterRecord(Avalon::Protocol& protocol);
	void SendToMasterRecord(Avalon::Packet* packet);

	/**
	*@brief 转发请求
	*/
	void SendToSlaverRecord(Avalon::Protocol& protocol);
	void SendToSlaverRecord(Avalon::Packet* packet);

	/**
	*@brief 转发请求
	*/
	void TransRequestToSlaver(UInt32 nodeId, Avalon::Packet* packet);

	/**
	*@brief 转发回复
	*/
	void TransResponseFromSlaver(UInt32 destNodeId, Avalon::Protocol& protocol);
	void TransResponseFromSlaver(UInt32 destNodeId, Avalon::Packet* packet);
}

#endif
