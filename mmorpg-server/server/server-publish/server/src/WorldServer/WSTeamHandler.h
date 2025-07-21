#ifndef _WS_TEAM_HANDLER_H_
#define _WS_TEAM_HANDLER_H_

#include <CLTeamProtocol.h>
#include <CLGameSessionProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldCreateTeam& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldLeaveTeamReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetTeamOption& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTransferTeammaster& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDismissTeam& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryTeamList& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamMasterOperSync& sync);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamRequesterListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamProcessRequesterReq& req);

void HandleProtocol(CLProtocol::WorldTeamBroadcastNotify& req);

void HandleProtocol(CLProtocol::WorldSceneCreateTeamReq& req);

void HandleProtocol(CLProtocol::SceneSyncTeamMemberAvatar& sync);

void HandleProtocol(CLProtocol::WorldServerTeamStartDungeonReq& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamReportVoteChoice& vote);

void HandleProtocol(CLProtocol::WorldTeamMatchStartReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamMatchCancelReq& req);

void HandleProtocol(CLProtocol::WorldTeamQueryTargetCondRes& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChangeAssistModeReq& req);

#endif
