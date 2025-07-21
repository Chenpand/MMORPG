#include "DSNetwork.h"
#include "DungeonHandler.h"
#include "DSSysHandler.h"

template<typename ProtocolT>
class DSPacketFunction : public CLPacketFunction
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

#define DS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new DSPacketFunction<ProtocolT>());


void DSNetwork::RegisterProtocols()
{
	DS_REGISTER_PROTOCOL(CLProtocol::DungenServerTeamStartRaceReq)
	DS_REGISTER_PROTOCOL(CLProtocol::MatchSvrStartGameRes)
	DS_REGISTER_PROTOCOL(CLProtocol::MatchSvrDungeonRaceEnd)
	DS_REGISTER_PROTOCOL(CLProtocol::MatchSvrRelayRegister)
	DS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonLeaveRace)
	DS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumRet)
	DS_REGISTER_PROTOCOL(CLProtocol::RelayDungeonBossPhaseChange)
	DS_REGISTER_PROTOCOL(CLProtocol::TeamCopyNotifyRaceDestroyDungeon)
	DS_REGISTER_PROTOCOL(CLProtocol::TeamCopyAddRaceReviveCnt)
	DS_REGISTER_PROTOCOL(CLProtocol::DungeonSvrEndRace)
	DS_REGISTER_PROTOCOL(CLProtocol::DungeonSvrReviveReq)
	DS_REGISTER_PROTOCOL(CLProtocol::SceneToDungeonQuickBuyTransactionReq)
	DS_REGISTER_PROTOCOL(CLProtocol::VerifyReportTaskResult)
	DS_REGISTER_PROTOCOL(CLProtocol::SceneSyncEvent)
	DS_REGISTER_PROTOCOL(CLProtocol::TeamCopyNotifyBimsEnergyAccuProgress)
	DS_REGISTER_PROTOCOL(CLProtocol::GateTransmitPacket)
	DS_REGISTER_PROTOCOL(CLProtocol::RelayAbalitySync)
}


