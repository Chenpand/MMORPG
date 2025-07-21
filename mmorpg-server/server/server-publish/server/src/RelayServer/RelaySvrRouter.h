#ifndef _RELAYSVR_ROUTER_H_
#define _RELAYSVR_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>


namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	//send to match server
	void SendToMatchSvr(Avalon::Protocol& protocol);
	void SendToMatchSvr(Avalon::Packet* packet);

	//send to world server
	void SendToWorldSvr(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToWorldSvr(UInt32 nodeId, Avalon::Packet* packet);

	//send to replay server
	void SendToReplaySvr(Avalon::Protocol& protocol);
	void SendToReplaySvr(Avalon::Packet* packet);

	//send to battle server
	void SendToBattleSvr(Avalon::Protocol& protocol);
	void SendToBattleSvr(Avalon::Packet* packet);

	//send to dungeon server
	void SendToDungeonSvr(Avalon::Protocol& protocol);
	void SendToDungeonSvr(Avalon::Packet* packet);	
	
	//send to Observe server
	void SendToObserve(Avalon::Protocol& protocol);
	void SendToObserve(Avalon::Packet* packet);
}//end of namespace Router

#endif
