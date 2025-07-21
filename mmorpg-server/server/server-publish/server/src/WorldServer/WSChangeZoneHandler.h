#ifndef _WS_CHANGEZONE_HANDLER_H_
#define _WS_CHANGEZONE_HANDLER_H_

#include <CLChangeZoneProtocol.h>

void HandleProtocol(CLProtocol::GateChangeZoneCheckRet& protocol);

void HandleProtocol(CLProtocol::WorldChangeZoneNotify& protocol);

#endif
