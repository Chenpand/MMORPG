#ifndef __US_CHAMPION_HANDLER_H__
#define __US_CHAMPION_HANDLER_H__

#include <CLNetwork.h>
#include <CLChampionProtocol.h>
#include <CLObserveProtocol.h>
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLRelationProtocol.h>
#include <CLVerifyProtocol.h>

#include "USPlayer.h"
#include "USPlayerMgr.h"

class USPlayer;

void HandleProtocol(CLProtocol::SceneChampionReliveReq & sync, USPlayer * player);
void HandleProtocol(CLProtocol::UnionChampionReliveReq & req, USPlayer * player);
void HandleProtocol(CLProtocol::UnionChampionPlayerStatusReq& req, USPlayer * player);



void HandleProtocol(CLProtocol::UnionCheckChampionEnrollReq& req);
void HandleProtocol(CLProtocol::UnionChampionJoinPrepareReq& req);

void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoReq& req);

void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoRes& req);

void HandleProtocol(CLProtocol::MatchServerChampionBattleRes& req);


void HandleProtocol(CLProtocol::MatchServerChampionRaceEnd& req);

void HandleProtocol(CLProtocol::MatchServerRaceEnd& req);

void HandleProtocol(CLProtocol::WorldNotifyRaceStart& req);

void HandleProtocol(CLProtocol::SceneChampionGambleReq& req);


void HandleProtocol(CLProtocol::UnionChampionObserveReq& req);
void HandleProtocol(CLProtocol::SceneChampionObserveRes& req);

void HandleProtocol(CLProtocol::VerifyReportTaskResult& req);




void HandleProtocol(CLProtocol::SceneChampionSelfStatusReq& req, RoleIDConnID& info);
void HandleProtocol(CLProtocol::SceneChampionTotalStatusReq& req, RoleIDConnID& info);
void HandleProtocol(CLProtocol::SceneChampionGroupRecordReq& req, RoleIDConnID& info);
void HandleProtocol(CLProtocol::SceneChampionGambleRecordReq& req, RoleIDConnID& info);
void HandleProtocol(CLProtocol::SceneChampionGmableAlreadyBetReq& req, RoleIDConnID& info);


#endif



