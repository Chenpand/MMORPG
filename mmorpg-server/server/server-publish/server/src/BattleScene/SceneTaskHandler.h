#ifndef _SCENE_TASK_HANDLER_H_
#define _SCENE_TASK_HANDLER_H_

#include <CLTaskProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneVisitTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneAcceptTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneSubmitTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneAbandonTask& req);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneLegendTaskListReq& req);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneSubmitLegendTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneSubmitAllDailyTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneSubmitAchievementTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneSetTaskItemReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneRefreshCycleTask& req);

void HandleProtocol(Player* player, CLProtocol::SceneDailyScoreRewardReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneAchievementScoreRewardReq& req);

void HandleProtocol(CLProtocol::WorldNotifyPublishMasterTaskToScene& req);

void HandleProtocol(CLProtocol::WorldGetDiscipleMasterTasksToSceneReq& req);

void HandleProtocol(CLProtocol::WorldCheckMasterDailyTaskReqToScene& req);

void HandleProtocol(CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene& req);

#endif
