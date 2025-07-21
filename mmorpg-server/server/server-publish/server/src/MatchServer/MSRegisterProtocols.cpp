#include "MSInternalNetwork.h"
#include "MatchHandler.h"

template<typename ProtocolT>
class MSPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

#define MS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new MSPacketFunction<ProtocolT>)

void MSInternalNetwork::RegisterProtocols()
{
    MS_REGISTER_PROTOCOL(CLProtocol::SysTransmitRaceProtocolInNode);
    MS_REGISTER_PROTOCOL(CLProtocol::SysTransmitRaceProtocolToScenePlayer);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchStartReq);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchDungeonReq);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerGuildBattleMatchReq);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerPremiumLeagueMatchReq);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchCancelReq);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchSvrStartGameRes);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchSvrEndGameReq);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchSvrDungeonRaceEnd);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchServerQueryPlayerInfoRes);
    MS_REGISTER_PROTOCOL(CLProtocol::MatchSvrRelayRegister);
    MS_REGISTER_PROTOCOL(CLProtocol::WorldServerTeamCreateRaceReq);
    MS_REGISTER_PROTOCOL(CLProtocol::WorldMatchPracticeReq);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerPremiumLeagueBattleReq);
	MS_REGISTER_PROTOCOL(CLProtocol::SceneServerGiveupReconn);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomReq);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerSpecifyPkRobot);
	MS_REGISTER_PROTOCOL(CLProtocol::MatchServerChampionBattleReq);
	MS_REGISTER_PROTOCOL(CLProtocol::RelayAbalitySync);
}