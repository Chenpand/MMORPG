#ifndef _GS_RELATION_HANDLER_H_
#define _GS_RELATION_HANDLER_H_

#include <CLRelationProtocol.h>

void HandleProtocol(CLProtocol::GateSyncBlackList& req);

void HandleProtocol(CLProtocol::GateNotifyNewBlackList& req);

void HandleProtocol(CLProtocol::GateNotifyDelBlackList& req);

#endif
