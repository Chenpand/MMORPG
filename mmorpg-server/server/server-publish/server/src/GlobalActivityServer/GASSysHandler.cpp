#include "GASSysHandler.h"
#include "GASPlayerMgr.h"
#include "GASGMSystem.h"

void HandleProtocol(CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol)
{
	GASPlayerMgr::Instance()->ExecuteProtocol(protocol);
}

void HandleProtocol(CLProtocol::GASGMCommand& protocol)
{
	GASGMSystem::Instance()->ExecuteCommand(protocol.cmdName, protocol.param);
}
