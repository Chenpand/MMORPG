#include "SysHandler.h"

#include "DPNetwork.h"

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	Avalon::NetConnectionPtr node = Network::Instance()->FindConnection(protocol.destnode);
	if(node == NULL)
	{
		ErrorLogStream << "can not find connection(" << protocol.destnode << ") when transmit packet." << LogStream::eos;
		return;
	}

	node->SendPacket(protocol.packet);
}
