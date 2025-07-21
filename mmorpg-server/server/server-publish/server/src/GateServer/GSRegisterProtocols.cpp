#include "GSInternalNetwork.h"
#include "GSExternalNetwork.h"
#include "GSPlayerMgr.h"

#include "GSLoginHandler.h"
#include "GSEnterGameHandler.h"
#include "GSLeaveGameHandler.h"
#include "GSSceneHandler.h"
#include "GSGateHandler.h"
#include "GSMoveHandler.h"
#include "GSRelationHandler.h"
#include "GSChangeZoneHandler.h"
#include "GSActivityHandler.h"
#include "GSSecurityLockHandler.h"

template<typename ProtocolT>
class GSPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		// 统计时间
		auto _startTime = GSPlayerMgr::Instance()->GetUSecsNow();

		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}

		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);
	}
};

template<typename ProtocolT>
class GSSelectPlayerPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if(player == NULL || (player->GetStatus() != GPS_SELECTROLE && player->GetStatus() != GPS_QUEUE)) return;
	
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(player,protocol);
		}
	}
};

template<typename ProtocolT>
class GSPlayerPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if(player == NULL || player->GetStatus() != GPS_NORMAL) return;
	
		player->RecordRecord(packet, true);
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(player,protocol);
		}
	}
};

#define GS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new GSPacketFunction<ProtocolT>());

#define GS_REGISTER_SELECTPLAYER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new GSSelectPlayerPacketFunction<ProtocolT>());

#define GS_REGISTER_PLAYER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new GSPlayerPacketFunction<ProtocolT>());

void GSInternalNetwork::RegisterProtocols()
{
	GS_REGISTER_PROTOCOL(CLProtocol::GateSendAccountInfo)
	GS_REGISTER_PROTOCOL(CLProtocol::GateRegisterScene)
	GS_REGISTER_PROTOCOL(CLProtocol::GateUnregisterScene)

	GS_REGISTER_PROTOCOL(CLProtocol::GateTransmitPacket)
	GS_REGISTER_PROTOCOL(CLProtocol::GateBroadcastAround)
	GS_REGISTER_PROTOCOL(CLProtocol::GateBroadcastExcept)
	GS_REGISTER_PROTOCOL(CLProtocol::GateBroadcastAll)
	GS_REGISTER_PROTOCOL(CLProtocol::GateSyncPlayerBaseInfo)

	GS_REGISTER_PROTOCOL(CLProtocol::SceneEnterGameRet)
	GS_REGISTER_PROTOCOL(CLProtocol::WorldVerifyUniqueRet)
	GS_REGISTER_PROTOCOL(CLProtocol::WorldAllowPlayerLogin)
	GS_REGISTER_PROTOCOL(CLProtocol::WorldSyncPlayerLoginWaitInfo)
	GS_REGISTER_PROTOCOL(CLProtocol::GateKickOffPlayer)
	GS_REGISTER_PROTOCOL(CLProtocol::GateReconnectGameReq)
	GS_REGISTER_PROTOCOL(CLProtocol::CenterQueryConvertAccountRes)
	GS_REGISTER_PROTOCOL(CLProtocol::WorldGetLoginPushInfoRet)
	GS_REGISTER_PROTOCOL(CLProtocol::CenterPlayerAddictTimeSync)

	GS_REGISTER_PROTOCOL(CLProtocol::GateNotifyEnterScene)
	GS_REGISTER_PROTOCOL(CLProtocol::GateNotifyLeaveScene)

	GS_REGISTER_PROTOCOL(CLProtocol::GateSyncGridIndex);

	GS_REGISTER_PROTOCOL(CLProtocol::GateSyncBlackList)
	GS_REGISTER_PROTOCOL(CLProtocol::GateNotifyNewBlackList)
	GS_REGISTER_PROTOCOL(CLProtocol::GateNotifyDelBlackList)

	GS_REGISTER_PROTOCOL(CLProtocol::GateChangeZoneCheckReq)
	GS_REGISTER_PROTOCOL(CLProtocol::GateChangeZoneNotify)

	GS_REGISTER_PROTOCOL(CLProtocol::WSSyncOpActivityData)
	GS_REGISTER_PROTOCOL(CLProtocol::WSOpActivityChange)
	GS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumRet)
	GS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
	GS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyGameStartTimeToS)

	GS_REGISTER_PROTOCOL(CLProtocol::GateSecurityLockCheckRes)
	GS_REGISTER_PROTOCOL(CLProtocol::GateToWorldSecurityLockRemoveRes)
	GS_REGISTER_PROTOCOL(CLProtocol::SysServiceSwitchChangeStatus)
}

void GSExternalNetwork::RegisterProtocols()
{
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateCreateRoleReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateDeleteRoleReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateRecoverRoleReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateEnterGameReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateFinishNewbeeGuide)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateLeaveLoginQueue)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateConvertAccountReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateSecurityLockRemoveReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateNoviceGuideChooseReq)
	GS_REGISTER_SELECTPLAYER_PROTOCOL(CLProtocol::GateSetRolePreferencesReq)

	GS_REGISTER_PLAYER_PROTOCOL(CLProtocol::GateLeaveGameReq)
	GS_REGISTER_PLAYER_PROTOCOL(CLProtocol::RoleSwitchReq)
}
