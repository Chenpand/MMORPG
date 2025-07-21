#ifndef _SCENE_OPERATE_HANDLER_H_
#define _SCENE_OPERATE_HANDLER_H_

#include <CLOpActiveProtocol.h>

class Player;

void HandleProtocol(CLProtocol::WSSyncOpActivityData& req);
void HandleProtocol(CLProtocol::WSOpActivityChange& req);
void HandleProtocol(Player* player, CLProtocol::CSOpActTakeRewardReq& req);
void HandleProtocol(CLProtocol::CSCDKVerifyRes& res);

void HandleProtocol(Player* player, CLProtocol::CSDeleteNotifyList& req);
void HandleProtocol(Player* player, CLProtocol::SceneOpActivityTaskInfoReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarDiscountInfoReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarExtractDiscountReq& req);

void HandleProtocol(CLProtocol::WorldFirstReturnSync& sync);

#endif
