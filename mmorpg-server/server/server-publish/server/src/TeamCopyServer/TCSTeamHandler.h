#ifndef _TCS_TEAM_HANDLER_H_
#define _TCS_TEAM_HANDLER_H_

#include <CLTeamCopyProtocol.h>

class TCSPlayer;

void HandleProtocol(CLProtocol::TeamCopyCreateTeamReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamDataReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamListReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamApplyReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamQuitReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamApplyListReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamApplyReplyReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyStartBattleReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyStartBattleVote& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyStartChallengeReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyQueryPlayerInfoRes& protocol);

void HandleProtocol(CLProtocol::TeamCopyFindTeamMateReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyInvitePlayer& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyStageFlopReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyPlayerCheckRes& protocol);

void HandleProtocol(CLProtocol::TeamCopyChangeSeatReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyTeamDetailReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyKickReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyAppointmentReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyAutoAgreeGoldReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyInviteListReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyInviteChoiceReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyRecruitReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyLinkJoinReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyCfgValReq& protocol);

void HandleProtocol(CLProtocol::TeamCopyForceEndReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyForceEndVoteReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyModifyEquipScoreReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyGridMoveReq& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::TeamCopyRollChoiceReq& protocol, TCSPlayer* player);

#endif
