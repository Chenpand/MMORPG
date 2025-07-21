#include "TCSNetwork.h"

#include <CLProtocolDefine.h>
#include <CLGameSessionProtocol.h>

#include <CLSceneProtocol.h>
#include <CLMoveProtocol.h>
#include "TCSSceneHandler.h"
#include "TCSSysHandler.h"
#include "TCSTeamHandler.h"
#include "TCSDungeonHandler.h"
#include "TCSPlayerSyncHandler.h"

template<typename ProtocolT>
class TCSPacketFunction : public CLPacketFunction
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

#define TCS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new TCSPacketFunction<ProtocolT>());


void TCSNetwork::RegisterProtocols()
{
	TCS_REGISTER_PROTOCOL(CLProtocol::SysTransmitGmCommand);
	TCS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerProtocolInNode);
	TCS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes);
	TCS_REGISTER_PROTOCOL(CLProtocol::SysTransmitToPlayer);
	TCS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerToTeamCopy);
	TCS_REGISTER_PROTOCOL(CLProtocol::GateNotifyLeaveConnect);
	TCS_REGISTER_PROTOCOL(CLProtocol::SceneSyncChat);
	TCS_REGISTER_PROTOCOL(CLProtocol::SceneSyncEvent);

	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyExitSceneReq);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyPlayerReconnect);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyPlayerCheckRes);

	RegisterProtocol(CLProtocol::TeamCopyEnterSceneReq::__ID, new TeamCopyEnterSceneReqFunction());
	RegisterProtocol(CLProtocol::SceneSyncSceneObject::__ID, new SceneSyncSceneObjectFunction());
	RegisterProtocol(CLProtocol::SceneDeleteSceneObject::__ID, new SceneDeleteSceneObjectFunction());
	RegisterProtocol(CLProtocol::SceneSyncPlayerMove::__ID, new SceneSyncPlayerMoveFunction());
	RegisterProtocol(CLProtocol::SceneSyncObjectProperty::__ID, new SceneSyncPlayerMoveFunction());

	TCS_REGISTER_PROTOCOL(CLProtocol::DungenServerTeamStartRaceRes);
	TCS_REGISTER_PROTOCOL(CLProtocol::DungeonSvrDungeonRaceEnd);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyQueryPlayerInfoRes);
	TCS_REGISTER_PROTOCOL(CLProtocol::RelayDungeonBossPhaseChange);

	TCS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonLeaveRace);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyDungeonRevive);
	TCS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumRet);
	TCS_REGISTER_PROTOCOL(CLProtocol::SceneToDungeonQuickBuyTransactionReq);
	
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyPlayerInfoSync);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyPlayerTicketSync);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyPlayerEquipScoreSync);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyPlayerLoadingSync);
	TCS_REGISTER_PROTOCOL(CLProtocol::TeamCopyCfgValReq);
}


