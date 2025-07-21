#ifndef _GS_CHANGEZONE_HANDLER_H_
#define _GS_CHANGEZONE_HANDLER_H_

#include <CLChangeZoneProtocol.h>

void HandleProtocol(CLProtocol::GateChangeZoneCheckReq& protocol);

void HandleProtocol(CLProtocol::GateChangeZoneNotify& protocol);

#endif
