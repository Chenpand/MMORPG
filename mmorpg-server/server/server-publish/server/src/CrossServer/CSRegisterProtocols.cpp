#include <CLProtocolDefine.h>

#include "CSNetwork.h"

#include "CSSceneHandler.h"
#include "CSChallengeHandler.h"
#include "CSGuildBattleHandler.h"
#include "CSSysHandler.h"
#include "CSScoreWarHandler.h"
#include "CSRoomHandler.h"
#include "CS2V2ScoreWarHandler.h"


template<typename ProtocolT>
class CSPacketFunction : public CLPacketFunction
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

#define CS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new CSPacketFunction<ProtocolT>());


void CSNetwork::RegisterProtocols()
{
	CS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
	CS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerProtocolInNode)
	CS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterSceneReq)
	CS_REGISTER_PROTOCOL(CLProtocol::WorldUnregisterSceneReq)

		/*
		RegisterProtocol(CLProtocol::CHALLENGE_WORLDCHALLENGE_SIGNIN_REQ, new WorldChallengeSigninFunction());
		CS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeSignout)
		CS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeInfoReq)
		CS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeSortListReq)
		CS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeGetAwardReq)
		*/
	RegisterProtocol(CLProtocol::CrossEnterSceneReq::__ID, new WorldChallengeEnterSceneReqFunction());
	CS_REGISTER_PROTOCOL(CLProtocol::CrossExitSceneRes)
	CS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyWorldEnterScene)


	CS_REGISTER_PROTOCOL(CLProtocol::WorldCrossGuildBattleEnrollInfoRes)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleEnrollReq)
	CS_REGISTER_PROTOCOL(CLProtocol::MatchServerGuildBattleRaceEnd)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleRecordReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleSortListReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleSelfSortListReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleSortListSync)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleGMSync)

	//ScoreWar
	CS_REGISTER_PROTOCOL(CLProtocol::SceneCrossSyncScoreWarInfoRes)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossScoreWarSortListReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossScoreWarInfoReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossScoreWarSortListSync)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossScoreWarBattleInfoSync)
	CS_REGISTER_PROTOCOL(CLProtocol::SceneCrossGMSync)

	//ScoreWar
	CS_REGISTER_PROTOCOL(CLProtocol::Scene2V2CrossSyncScoreWarInfoRes)
	CS_REGISTER_PROTOCOL(CLProtocol::Cross2V2ScoreWarSortListReq)
	CS_REGISTER_PROTOCOL(CLProtocol::Cross2V2ScoreWarInfoReq)
	CS_REGISTER_PROTOCOL(CLProtocol::Cross2V2ScoreWarSortListSync)
	CS_REGISTER_PROTOCOL(CLProtocol::Cross2V2ScoreWarBattleInfoSync)
	CS_REGISTER_PROTOCOL(CLProtocol::SceneCross2V2GMSync)
		

	
	//Room
	CS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomStartRace)
	CS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomRes)
	CS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomRaceEnd)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomPlayerOnline)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomPlayerOffline)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomPlayerDisconnect)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomPlayerReconnect)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomPlayerChangeScene)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossWorldRoomListReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossUpdateRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossJoinRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossQuitRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossKickOutRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossDismissRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossInviteJoinRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossChangeRoomOwnerReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossBeInviteRoomReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomCloseSlotReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomSwapSlotReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomResponseSwapSlotReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomBattleStartReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomBattleCancelReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomBattleReadyReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomSendInviteLinkReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossRoomMatchInfoRes)
	CS_REGISTER_PROTOCOL(CLProtocol::CrossWorldInviteRoomRes)

	RegisterProtocol(CLProtocol::CROSS_TRANSMIT_CHAT, new CrossTransChatFunction());

}


