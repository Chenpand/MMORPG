#include <CLProtocolDefine.h>
#include <CLBattleProtocol.h>
#include <CLGameSessionProtocol.h>
#include "BSNetwork.h"
#include "BSSceneHandler.h"
#include "BSSysHandler.h"
#include "BSLostDungeonHandler.h"
#include "BSDungeonHandler.h"
#include "BSItemHandler.h"

template<typename ProtocolT>
class BSPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

#define BS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new BSPacketFunction<ProtocolT>());


void BSNetwork::RegisterProtocols()
{
	BS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
	BS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerProtocolInNode)
	BS_REGISTER_PROTOCOL(CLProtocol::BattleExitSceneRes)

	BS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterSceneReq)
	BS_REGISTER_PROTOCOL(CLProtocol::WorldUnregisterSceneReq)
	BS_REGISTER_PROTOCOL(CLProtocol::MatchSvrRelayRegister);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleSortListReq)

	RegisterProtocol(CLProtocol::BattleEnterSceneReq::__ID, new BattleEnterSceneReqFunction());

	BS_REGISTER_PROTOCOL(CLProtocol::MatchServerQueryPlayerInfoRes);

	BS_REGISTER_PROTOCOL(CLProtocol::MatchSvrStartGameRes);
	BS_REGISTER_PROTOCOL(CLProtocol::MatchSvrEndGameReq);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleEnrollReq);
	BS_REGISTER_PROTOCOL(CLProtocol::BattlePkSomeOneReq);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleLockSomeOneReq);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleServerSpecifyPkRobot);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleServerAddPkRobot);

	BS_REGISTER_PROTOCOL(CLProtocol::SceneBattleNotifySomeoneDead);
	BS_REGISTER_PROTOCOL(CLProtocol::SceneBattleCreateBattleRes);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleNotifyActivityInfo);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleCheckNoWarRes);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleObserveReq);
	BS_REGISTER_PROTOCOL(CLProtocol::RelayAbalitySync);

	BS_REGISTER_PROTOCOL(CLProtocol::LostDungeonMathBattleReq);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleEnterSceneRes);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleLostDungeonPkReq);
	BS_REGISTER_PROTOCOL(CLProtocol::DungenServerTeamStartRaceReq);
	BS_REGISTER_PROTOCOL(CLProtocol::DungenServerTeamStartRaceRes);
	BS_REGISTER_PROTOCOL(CLProtocol::DungeonSvrDungeonRaceEnd);

	BS_REGISTER_PROTOCOL(CLProtocol::SysTransmitToPlayer);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleTeamReportVoteChoice);
	BS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonLeaveRace);
	BS_REGISTER_PROTOCOL(CLProtocol::BattleExitNotify);

	BS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodesType);

	BS_REGISTER_PROTOCOL(CLProtocol::DungeonSvrReviveReq);
	BS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumRet);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleDestoryRes);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleQuerylayerDetailOne);

	BS_REGISTER_PROTOCOL(CLProtocol::BattleSyncRacePlayerInfo);
}


