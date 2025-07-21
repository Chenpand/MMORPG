#ifndef _US_ROUTER_H_
#define _US_ROUTER_H_

#include <CLDefine.h>
#include <AvalonDefine.h>
#include <AvalonProtocol.h>
#include <AvalonSingleton.h>
#include <AvalonNetConnection.h>

class USRouter : public Avalon::Singleton<USRouter>
{
public:
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	*@brief 发送到Match
	*/
	bool SendToMatch(Avalon::Packet* packet);
	bool SendToMatch(Avalon::Protocol& protocol);

	/**
	*@brief 发送到Observe
	*/
	bool SendToObserve(Avalon::Packet* packet);
	bool SendToObserve(Avalon::Protocol& protocol);

    ///**
    //*@brief 发送给玩家
    //*/
    //bool SendToPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Protocol& protocol);
    //bool SendToPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Packet* packet);

    ///**
    //*@brief 发送给WS上的玩家
    //*/
    //bool SendToWorldPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Protocol& protocol);
    //bool SendToWorldPlayer(UInt32 wsNodeId, UInt64 roleID, Avalon::Packet* packet);


private:
	Avalon::NetConnectionPtr        m_MatchConnection;
	Avalon::NetConnectionPtr        m_ObserveConnection;
};

#endif
