#ifndef _BS_SCENE_HANDLER_H_
#define _BS_SCENE_HANDLER_H_

#include <CLSceneProtocol.h>
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLNetwork.h>
#include <CLBattleProtocol.h>

void HandleProtocol(CLProtocol::WorldRegisterSceneReq& protocol);

void HandleProtocol(CLProtocol::WorldUnregisterSceneReq& protocol);

void HandleProtocol(CLProtocol::MatchSvrRelayRegister& reg);

void HandleProtocol(CLProtocol::BattleSortListReq& protocol);

class BattleEnterSceneReqFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoRes& protocol);

void HandleProtocol(CLProtocol::MatchSvrStartGameRes& protocol);

void HandleProtocol(CLProtocol::MatchSvrEndGameReq& protocol);

void HandleProtocol(CLProtocol::BattleEnrollReq& protocol);

void HandleProtocol(CLProtocol::BattlePkSomeOneReq& protocol);

void HandleProtocol(CLProtocol::BattleLockSomeOneReq& protocol);

void HandleProtocol(CLProtocol::BattleServerSpecifyPkRobot& protocol);

void HandleProtocol(CLProtocol::BattleServerAddPkRobot& protocol);

void HandleProtocol(CLProtocol::SceneBattleNotifySomeoneDead& protocol);

void HandleProtocol(CLProtocol::SceneBattleCreateBattleRes& res);

void HandleProtocol(CLProtocol::BattleNotifyActivityInfo& protocol);

void HandleProtocol(CLProtocol::BattleCheckNoWarRes& protocol);

void HandleProtocol(CLProtocol::BattleObserveReq& protocol);

void HandleProtocol(CLProtocol::RelayAbalitySync& sync);

#endif
