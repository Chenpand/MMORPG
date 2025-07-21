#ifndef _WORLD_GLOBAL_ACTIVITY_HANDLER_H_
#define _WORLD_GLOBAL_ACTIVITY_HANDLER_H_

#include <CLGlobalActivityProtocol.h>
#include <CLGambingProtocol.h>


void HandleProtocol(CLProtocol::GASSendAnnounceReq& protocol);

void HandleProtocol(CLProtocol::GASSendMailReq& protocol);

void HandleProtocol(CLProtocol::GASActivitySync& protocol);

void HandleProtocol(CLProtocol::GASWholeBargainDiscountSync& protocol);

#endif