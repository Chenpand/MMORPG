#include "WSConsoleHandler.h"
#include "WSConsoleAgent.h"

void HandleProtocol(CLProtocol::AdminConsoleRequest& req)
{
	WSConsoleAgent::Instance()->HandleRequest(req.nodeid, req.cmd, req.subcmd, req.body);
}
