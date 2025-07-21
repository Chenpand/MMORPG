#ifndef _WS_RELATION_HANDLER_H_
#define _WS_RELATION_HANDLER_H_

#include <CLRelationProtocol.h>
#include <CLTaskProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryPlayerReq& req);
void HandleProtocol(CLProtocol::SceneQueryPlayerRet& ret);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryPlayerDetailsReq& req);
void HandleProtocol(CLProtocol::SceneQueryPlayerDetailsRet& ret);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationFindPlayersReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationPresentGiveReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdateRelation& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAddToBlackList& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdatePlayerOnlineReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldRemoveRelation& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetMasterNoteReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetMasterIsRecvDiscipleReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryMasterSettingReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAddonPayReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAddonPayReply& req);
void HandleProtocol(CLProtocol::WorldCheckMasterGive& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationReportCheat& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetPlayerRemarkReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationAnnounceReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetDiscipleQuestionnaireReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetMasterQuestionnaireReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdateMasterSectRelationReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldDiscipleFinishSchoolReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldPublishMasterDialyTaskReq& req);
void HandleProtocol(CLProtocol::SceneNotifyPublishMasterTaskResToWorld& protocol);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetDiscipleMasterTasksReq& req);
void HandleProtocol(CLProtocol::WorldGetDiscipleMasterTasksToSceneRes& protocol);
void HandleProtocol(CLProtocol::SceneSyncMasterTaskShareDataToWorld& protocol);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldReportMasterDailyTaskReq& req);
void HandleProtocol(CLProtocol::SceneCheckMasterDailyTaskResToWorld& protocol);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldReceiveMasterDailyTaskRewardReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldReceiveMasterAcademicTaskRewardReq& req);
void HandleProtocol(CLProtocol::SceneNotifyGiveMasterSysGiftResToWorld& protocol);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAutomaticFinishSchoolReq& req);
void HandleProtocol(CLProtocol::SceneSyncMasterSysDataToWorld& protocol);
void HandleProtocol(CLProtocol::SceneSyncMasterDailyTaskComp& protocol);

//------------------------------（록폴）--------------------------------------------------------------------


void HandleProtocol(WSPlayer* player, CLProtocol::WorldChaseKill& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetOnlineNotify& req);



void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationQuickMakeFriends& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationSetClostFriend& req);


void HandleProtocol(CLProtocol::WorldRelationAddIntimacy& req);

void HandleProtocol(CLProtocol::SceneRelationFindPlayerRet& ret);

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendReq& req);

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendRet& req);

//------------------------------（록폴）--------------------------------------------------------------------


#endif
