#ifndef _GS_ENTERGAME_HANDLER_H_
#define _GS_ENTERGAME_HANDLER_H_

#include <CLEnterGameProtocol.h>
#include <CLCenterProtocol.h>

class GSPlayer;

void HandleProtocol(GSPlayer *player,CLProtocol::GateCreateRoleReq& protocol);

void HandleProtocol(GSPlayer *player,CLProtocol::GateDeleteRoleReq& protocol);

void HandleProtocol(GSPlayer *player,CLProtocol::GateRecoverRoleReq& protocol);

void HandleProtocol(GSPlayer *player,CLProtocol::GateEnterGameReq& protocol);

void HandleProtocol(GSPlayer *player, CLProtocol::GateFinishNewbeeGuide& protocol);

void HandleProtocol(GSPlayer* player, CLProtocol::GateLeaveLoginQueue& protocol);

void HandleProtocol(GSPlayer* player, CLProtocol::GateConvertAccountReq& protocol);

void HandleProtocol(CLProtocol::CenterQueryConvertAccountRes& protocol);

void HandleProtocol(CLProtocol::GateReconnectGameReq& protocol);

void HandleProtocol(CLProtocol::SceneEnterGameRet& protocol);

void HandleProtocol(CLProtocol::WorldVerifyUniqueRet& protocol);

void HandleProtocol(CLProtocol::WorldAllowPlayerLogin& protocol);

void HandleProtocol(CLProtocol::WorldSyncPlayerLoginWaitInfo& protocol);

void HandleProtocol(GSPlayer* player, CLProtocol::RoleSwitchReq& protocol);

void HandleProtocol(CLProtocol::WorldGetLoginPushInfoRet& protocol);

void HandleProtocol(GSPlayer *player, CLProtocol::GateNoviceGuideChooseReq& req);

void HandleProtocol(GSPlayer *player, CLProtocol::GateSetRolePreferencesReq& req);

#endif
