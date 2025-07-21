#ifndef _WS_ACTIVITY_HANDLER_H_
#define _WS_ACTIVITY_HANDLER_H_

#include <CLRoutineProtocol.h>
#include <CLOpActiveProtocol.h>
#include <CLTaskProtocol.h>

class WSPlayer;

void HandleProtocol(CLProtocol::WorldSyncActivesCounter &protocol);

void HandleProtocol(WSPlayer *player, CLProtocol::WorldNotifyCounterWindow &protocol);

void HandleProtocol(WSPlayer *player, CLProtocol::WorldActivityMonsterReq &protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneActiveTaskSubmit& req);
void HandleProtocol(WSPlayer* player, CLProtocol::SceneSignInRp& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldActivityDetailReq& req);

#endif
