#ifndef _US_PLAYER_HANDLER_H_
#define _US_PLAYER_HANDLER_H_

#include <CLTeamCopyProtocol.h>
#include <CLUnionServerProtocol.h>

void HandleProtocol(CLProtocol::UnionServerAddPlayerReq& protocol);

void HandleProtocol(CLProtocol::UnionServerDelPlayerReq& protocol);

#endif
