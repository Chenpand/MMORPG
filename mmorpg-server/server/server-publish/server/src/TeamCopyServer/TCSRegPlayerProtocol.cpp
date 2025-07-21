#include "TCSPlayerMgr.h"

#include "TCSTeamHandler.h"
#include "TCSDungeonHandler.h"

template<typename ProtocolT>
class TCSPlayerPacketFunction : public CLParamPacketFunction<TCSPlayer*>
{
public:
	void operator()(Avalon::Packet* packet, TCSPlayer* player) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(protocol, player);
		}
	}
};

#define TCS_REGISTER_PLAYER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID, new TCSPlayerPacketFunction<ProtocolT>);

void TCSPlayerMgr::RegisterProtocols()
{
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyCreateTeamReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamDataReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamListReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamApplyReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamQuitReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamApplyListReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamApplyReplyReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyStartBattleReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyStartBattleVote);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyStartChallengeReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyFindTeamMateReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyInvitePlayer);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyStageFlopReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyChangeSeatReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyTeamDetailReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyKickReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyAppointmentReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyAutoAgreeGoldReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyInviteListReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyInviteChoiceReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyRecruitReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyLinkJoinReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyForceEndReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyForceEndVoteReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyPhaseBossInfo);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyModifyEquipScoreReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyGridMoveReq);
	TCS_REGISTER_PLAYER_PROTOCOL(CLProtocol::TeamCopyRollChoiceReq);
}