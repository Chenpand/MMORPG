#include "DSSysHandler.h"
#include "DSPlayer.h"

void HandleProtocol(CLProtocol::GateTransmitPacket& protocol)
{
	DSPlayer::ExecutePacket(protocol.id, protocol.packet);
}