#ifndef _CS_SCORE_WAR_HANDLER_H_
#define _CS_SCORE_WAR_HANDLER_H_

#include "CLScoreWarProtocol.h"

void HandleProtocol(CLProtocol::SceneCrossSyncScoreWarInfoRes& sync);

void HandleProtocol(CLProtocol::CrossScoreWarInfoReq& req);

void HandleProtocol(CLProtocol::CrossScoreWarSortListReq& sync);

void HandleProtocol(CLProtocol::CrossScoreWarSortListSync& sync);

void HandleProtocol(CLProtocol::SceneCrossGMSync& sync);

void HandleProtocol(CLProtocol::CrossScoreWarBattleInfoSync& sync);

#endif //_CS_SCORE_WAR_HANDLER_H_
