#ifndef _SORTLIST_HANDLER_H_
#define _SORTLIST_HANDLER_H_

#include <CLSortListProtocol.h>

void HandleProtocol(CLProtocol::WorldSortListUpdateReq& protocol);

void HandleProtocol(CLProtocol::WorldSortListVisitReq& Protocol);

void HandleProtocol(ObjID_t playerId, CLProtocol::WorldSortListReq& protocol);

void HandleProtocol(ObjID_t playerId, CLProtocol::WorldSortListNearReq& protocol);

#endif
