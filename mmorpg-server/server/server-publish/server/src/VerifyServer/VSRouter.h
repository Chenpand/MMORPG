#ifndef _RELAYSVR_ROUTER_H_
#define _RELAYSVR_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	//send to verify center
	void SendToVerifyCenter(Avalon::Protocol& protocol);
	void SendToVerifyCenter(Avalon::Packet* packet);

}//end of namespace Router

#endif
