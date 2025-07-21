#ifndef _AS_ROUTER_H_
#define _AS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonDefine.h>
#include <AvalonProtocol.h>
#include <AvalonSingleton.h>
#include <AvalonNetConnection.h>

class MSRouter : public Avalon::Singleton<MSRouter>
{
	typedef HashMap<UInt32, Avalon::NetConnectionPtr> MapConnection;
public:
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	*@brief ���͵�world
	*/
	bool SendToWorld(UInt32 id,Avalon::Packet* packet);
	bool SendToWorld(UInt32 id,Avalon::Protocol& protocol);

	/**
	*@brief ���͵�Union
	*/
	bool SendToUnion(Avalon::Packet* packet);
	bool SendToUnion(Avalon::Protocol& protocol);

    /**
    *@brief ���͸����
    */
    bool SendToPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Protocol& protocol);
    bool SendToPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Packet* packet);

    /**
    *@brief ���͸�WS�ϵ����
    */
    bool SendToWorldPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Protocol& protocol);
    bool SendToWorldPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief �㲥��world
	*/
	void BroadcastToWorld(Avalon::Protocol& protocol);

	/**
	*@brief �㲥�����е�RelayServer
	*/
	void BroadcastToRelay(Avalon::Protocol& protocol);

	/**
	*@brief ���͵�RelayServer
	*/
	bool SendToRelay(UInt32 nodeid, Avalon::Protocol& protocol);

protected:
	Avalon::NetConnectionPtr FindConnection(UInt32 id);

private:
	MapConnection					m_worldConnections;
	Avalon::NetConnectionPtr        m_UnionConnections;
};

#endif
