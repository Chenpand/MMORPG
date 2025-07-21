#ifndef _SCENE_ROUTINE_HANDLER_H_
#define _SCENE_ROUTINE_HANDLER_H_

#include <CLRoutineProtocol.h>
#include <CLTaskProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneActiveDegreeReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneRoutineGetBoxReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneRoutineTodaySigninAwardReq& req);

void HandleProtocol(CLProtocol::WorldNotifySceneActivity& req);

void HandleProtocol(CLProtocol::WorldSyncSceneActivities& req);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneActiveTaskSubmit& req);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneSignInRp& req);
void HandleProtocol(Player* pPlayer, CLProtocol::SceneRestTimeReq& req);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneNewSignIn& req);
void HandleProtocol(Player* pPlayer, CLProtocol::SceneNewSignInQuery& req);
void HandleProtocol(Player* pPlayer, CLProtocol::SceneNewSignInCollect& req);





void HandleProtocol(CLProtocol::WorldActivityTaskSendRewards& protocol);

#endif
