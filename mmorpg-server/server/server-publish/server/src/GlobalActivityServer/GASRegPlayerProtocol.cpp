#include "GASPlayerMgr.h"

#include "GASGambingHandler.h"
#include "GASBetHorseHandler.h"
#include "GASActivityHandler.h"

template<typename ProtocolT>
class GASPlayerPacketFunction : public CLParamPacketFunction<GASPlayer*>
{
public:
	void operator()(Avalon::Packet* packet, GASPlayer* player) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(player, protocol);
		}
	}
};

#define GAS_REGISTER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID, new GASPlayerPacketFunction<ProtocolT>);

void GASPlayerMgr::RegisterProtocols()
{
	GAS_REGISTER_PROTOCOL(CLProtocol::PayingGambleReq)

	GAS_REGISTER_PROTOCOL(CLProtocol::GambingItemQueryReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GambingMineQueryReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GambingRecordQueryReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumRet)

	GAS_REGISTER_PROTOCOL(CLProtocol::BetHorseReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::ShooterOddsReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::ShooterHistoryRecordReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::StakeReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::StakeRecordReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::BattleRecordReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::ShooterRankReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GASArtifactJarLotteryRecordReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GASArtifactJarLotterySignReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GASArtifactJarLotteryCfgReq)

	GAS_REGISTER_PROTOCOL(CLProtocol::GASWeekSignRecordReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GASWeekSignLotteryReq)

	GAS_REGISTER_PROTOCOL(CLProtocol::GASWholeBargainReq)
	GAS_REGISTER_PROTOCOL(CLProtocol::GASWholeBargainJoinNumSync)
}