#ifndef _CS_2V2_SCORE_WAR_HANDLER_H_
#define _CS_2V2_SCORE_WAR_HANDLER_H_

#include "CL2V2ScoreWarProtocol.h"

void HandleProtocol(CLProtocol::Scene2V2CrossSyncScoreWarInfoRes& sync);

void HandleProtocol(CLProtocol::Cross2V2ScoreWarInfoReq& req);

void HandleProtocol(CLProtocol::Cross2V2ScoreWarSortListReq& sync);

void HandleProtocol(CLProtocol::Cross2V2ScoreWarSortListSync& sync);

void HandleProtocol(CLProtocol::Cross2V2ScoreWarBattleInfoSync& sync);

void HandleProtocol(CLProtocol::SceneCross2V2GMSync& sync);

#endif //_CS_SCORE_WAR_HANDLER_H_
