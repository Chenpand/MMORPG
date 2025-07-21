#include "USNetwork.h"

#include <CLProtocolDefine.h>
#include <CLGameSessionProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLChampionProtocol.h>

#include <CLSceneProtocol.h>
#include <CLMoveProtocol.h>
#include "USSceneHandler.h"
#include "USSysHandler.h"
#include "USPlayerHandler.h"
#include "USChampionHandler.h"
#include "USGoldConsignmentHandler.h"

template<typename ProtocolT>
class USPacketFunction : public CLPacketFunction
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

#define US_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new USPacketFunction<ProtocolT>());


void USNetwork::RegisterProtocols()
{
	US_REGISTER_PROTOCOL(CLProtocol::SysTransmitGmCommand);
	US_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerProtocolInNode);
	US_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes);
	US_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerToUnion);
	US_REGISTER_PROTOCOL(CLProtocol::GateNotifyLeaveConnect);
	US_REGISTER_PROTOCOL(CLProtocol::UnionSceneIsReady);
	US_REGISTER_PROTOCOL(CLProtocol::UnionWorldChgZoneID);
	US_REGISTER_PROTOCOL(CLProtocol::UnionNotifyActivityInfo);
	US_REGISTER_PROTOCOL(CLProtocol::UnionGoldConsignmentActivityClose);

	US_REGISTER_PROTOCOL(CLProtocol::UnionServerAddPlayerReq);
	US_REGISTER_PROTOCOL(CLProtocol::UnionServerDelPlayerReq);

	RegisterProtocol(CLProtocol::SceneSyncSceneObject::__ID, new SceneSyncSceneObjectFunction());
	RegisterProtocol(CLProtocol::SceneDeleteSceneObject::__ID, new SceneDeleteSceneObjectFunction());
	RegisterProtocol(CLProtocol::SceneSyncPlayerMove::__ID, new SceneSyncPlayerMoveFunction());
	RegisterProtocol(CLProtocol::SceneSyncObjectProperty::__ID, new SceneSyncPlayerMoveFunction());

	US_REGISTER_PROTOCOL(CLProtocol::UnionCheckChampionEnrollReq);
	US_REGISTER_PROTOCOL(CLProtocol::UnionMatchServerQueryPlayerInfoReq);
	US_REGISTER_PROTOCOL(CLProtocol::UnionChampionJoinPrepareReq);
	US_REGISTER_PROTOCOL(CLProtocol::WorldNotifyRaceStart);
	US_REGISTER_PROTOCOL(CLProtocol::UnionMatchServerQueryPlayerInfoRes);
	US_REGISTER_PROTOCOL(CLProtocol::MatchServerChampionRaceEnd);
	US_REGISTER_PROTOCOL(CLProtocol::MatchServerChampionBattleRes);
	US_REGISTER_PROTOCOL(CLProtocol::SceneChampionGambleReq);
	US_REGISTER_PROTOCOL(CLProtocol::UnionChampionObserveReq);
	US_REGISTER_PROTOCOL(CLProtocol::SceneChampionObserveRes);
	US_REGISTER_PROTOCOL(CLProtocol::MatchServerRaceEnd);
	US_REGISTER_PROTOCOL(CLProtocol::VerifyReportTaskResult);
}


