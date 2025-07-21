#include "SSGateHandler.h"
#include "PlayerMgr.h"


void HandleProtocol(CLProtocol::GateTransmitPacket& protocol)
{
	PlayerMgr::Instance()->ExecutePacket(protocol.id,protocol.packet);
}
