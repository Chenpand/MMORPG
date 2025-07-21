#ifndef _GAS_ACTIVITY_HANDLER_H_
#define _GAS_ACTIVITY_HANDLER_H_

#include <CLGlobalActivityProtocol.h>
#include <CLItemProtocol.h>
#include <CLOpActiveProtocol.h>

class GASPlayer;

void HandleProtocol(CLProtocol::GASActivityInfoReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::GASArtifactJarLotterySignReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::GASArtifactJarLotteryRecordReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::GASArtifactJarLotteryCfgReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::GASWeekSignRecordReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::GASWeekSignLotteryReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::GASWholeBargainJoinNumSync& sync);

void HandleProtocol(GASPlayer* player, CLProtocol::GASWholeBargainReq& req);

void HandleProtocol(CLProtocol::GASWholeBargainDiscountQuery& protocol);

#endif