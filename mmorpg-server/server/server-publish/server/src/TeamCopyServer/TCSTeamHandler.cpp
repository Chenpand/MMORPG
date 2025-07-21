#include "TCSTeamHandler.h"

#include "TCSTeamMgr.h"
#include "TCSPlayer.h"

void HandleProtocol(CLProtocol::TeamCopyCreateTeamReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleCreateTeamReq(player, protocol);
}

void HandleProtocol(CLProtocol::TeamCopyTeamDataReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamDataReq(player, protocol.teamType);
}

void HandleProtocol(CLProtocol::TeamCopyTeamListReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamListReq(player, protocol.pageNum, protocol.teamType, protocol.teamModel);
}

void HandleProtocol(CLProtocol::TeamCopyTeamApplyReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamApplyReq(player, protocol.teamId, protocol.isGold);
}

void HandleProtocol(CLProtocol::TeamCopyTeamQuitReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamQuitReq(player);
}

void HandleProtocol(CLProtocol::TeamCopyTeamApplyListReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamApplyListReq(player);
}

void HandleProtocol(CLProtocol::TeamCopyTeamApplyReplyReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamApplyReplyReq(player, protocol.isAgree, protocol.playerIds);
}

void HandleProtocol(CLProtocol::TeamCopyStartBattleReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleStartBattleReq(player, protocol.planModel, protocol.battlePlanList);
}

void HandleProtocol(CLProtocol::TeamCopyStartBattleVote& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleStartBattleVote(player, protocol.vote);
}

void HandleProtocol(CLProtocol::TeamCopyStartChallengeReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleStartChallengeReq(player, protocol.fieldId);
}

void HandleProtocol(CLProtocol::TeamCopyQueryPlayerInfoRes& protocol)
{
	TCSTeamMgr::Instance()->QueryPlayerRaceRes(protocol.teamSquadId, protocol.roleId, protocol.result, protocol.dungeonPlayerInfo);
}

void HandleProtocol(CLProtocol::TeamCopyFindTeamMateReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandlePlayerListReq(player);
}

void HandleProtocol(CLProtocol::TeamCopyInvitePlayer& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleInvitePlayerReq(player, protocol.inviteList);
}

void HandleProtocol(CLProtocol::TeamCopyStageFlopReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleStageFlopReq(player, protocol.stageId);
}

void HandleProtocol(CLProtocol::TeamCopyPlayerCheckRes& protocol)
{
	TCSTeamMgr::Instance()->HandleCheckCondRes(protocol.retCode, protocol.teamId, protocol.roleId, 
		protocol.commissionOrder, protocol.GetConnID(), protocol.forzenCommission, protocol.consumeItemVec);
}

void HandleProtocol(CLProtocol::TeamCopyChangeSeatReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleChangeSeatReq(player, protocol.srcSeat, protocol.destSeat);
}

void HandleProtocol(CLProtocol::TeamCopyTeamDetailReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleTeamDetailReq(player, protocol.teamId);
}

void HandleProtocol(CLProtocol::TeamCopyKickReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleKickReq(player, protocol.playerId);
}

void HandleProtocol(CLProtocol::TeamCopyAppointmentReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleAppointmentReq(player, protocol.playerId, protocol.post);
}

void HandleProtocol(CLProtocol::TeamCopyAutoAgreeGoldReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleAutoAgreeGoldReq(player, protocol.isAutoAgree);
}

void HandleProtocol(CLProtocol::TeamCopyInviteListReq& protocol, TCSPlayer* player)
{
	player->SendInviteList();
}

void HandleProtocol(CLProtocol::TeamCopyInviteChoiceReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleInviteChoiceReq(player, protocol.isAgree, protocol.teamIds);
}

void HandleProtocol(CLProtocol::TeamCopyRecruitReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleRecruitReq(player);
}

void HandleProtocol(CLProtocol::TeamCopyLinkJoinReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleLinkJoinReq(player, protocol.teamId, protocol.isGold);
}

void HandleProtocol(CLProtocol::TeamCopyCfgValReq& protocol)
{
	TCSTeamMgr::Instance()->ValCfgSyncToScene(protocol.GetConnID());
}

void HandleProtocol(CLProtocol::TeamCopyForceEndReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleForceEndReq(player);
}

void HandleProtocol(CLProtocol::TeamCopyForceEndVoteReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleForceEndVoteReq(player, protocol.vote);
}

void HandleProtocol(CLProtocol::TeamCopyModifyEquipScoreReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleModifyEquipScore(player, protocol.equipScore);
}

void HandleProtocol(CLProtocol::TeamCopyGridMoveReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleSquadMoveGrid(player, protocol.targetGridId, protocol.targetObjId);
}

void HandleProtocol(CLProtocol::TeamCopyRollChoiceReq& protocol, TCSPlayer* player)
{
	TCSTeamMgr::Instance()->HandleRollReward(player, protocol.isNeed);
}