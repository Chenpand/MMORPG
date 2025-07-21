/************************************************************************/
/* 注册内网以及外网的消息                                                 */
/************************************************************************/
#include "RLInternalNetwork.h"
#include "RLChannels.h"

#include "GameSessionHandler.h"
#include "RLPlayerMgr.h"
#include "VerifyHandler.h"
#include "RLObserveHandler.h"
//#include "CLRelayServerProtocol.h"

template<typename ProtocolT>
class RelaySvrPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		if (NULL == packet)
		{
			return;
		}

		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
		else
		{
			DebugLogStream << "in RelaySvrPacketFunction(), protocol(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
		}
	}
};

#define RELAYSVR_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new RelaySvrPacketFunction<ProtocolT>)

void RLInternalNetwork::RegisterProtocols()
{
	//
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::MatchSvrStartGameReq);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::MatchSvrNotifyRacePlayerInfo);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::MatchSvrTerminateGame);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::SceneServerPlayerLeaveRace);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::SceneServerSyncPlayerEvent);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::SceneServerGiveupReconn);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::RelaySvrRaceInitPacket);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::ReplayFileReq);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyCreateTaskRes);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyVSInfo);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyVSOnline);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyVSOffline);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyAbility);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyReportTaskResult);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::VerifyReportTaskRaceEnd);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::ObserveRealyAddObservePlayerReq);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::RelayStartReceiveRaceFrame);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::ObserveRealyCreateRaceReq);
	RELAYSVR_REGISTER_PROTOCOL(CLProtocol::ObserveSyncFrame);
}


template<typename ProtocolT>
class PlayerPacketFunction : public CLParamPacketFunction<ObjID_t>
{
public:
    void operator()(Avalon::Packet* packet, ObjID_t roleId) const
    {
        ProtocolT protocol;
        if (protocol.Decode(packet))
        {
			auto player = RLPlayerMgr::Instance()->find_player(roleId);
			if (player)
			{
				player->RecordMsg(packet, true);
			}

            HandleProtocol(roleId, protocol); //noted by aprilliu, HandleProtocol是全局函数，很多重载形式
        }
    }
};
#define RELAYSVR_REGISTER_PLAYER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID, new PlayerPacketFunction<ProtocolT>)

void RLChannels::RegisterProtocols()
{
	// 心跳包
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::SysHeartbeat);
	
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrLoginReq);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrReconnectReq);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrPlayerInputReq);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrEndGameReq);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrDisconnectNotify);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrFrameChecksumRequest);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrPingNotify);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrDungeonRaceEndReq);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrReportLoadProgress);
	RELAYSVR_REGISTER_PLAYER_PROTOCOL(CLProtocol::RelaySvrObserveReq);
}