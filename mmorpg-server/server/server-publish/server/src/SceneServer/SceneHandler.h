#ifndef _SCENE_HANDLER_H_
#define _SCENE_HANDLER_H_

#include <CLSceneProtocol.h>
#include <CLSysProtocol.h>

class Player;

void HandleProtocol(CLProtocol::SceneNotifyLoadmap& protocol);

void HandleProtocol(CLProtocol::SceneNotifyLoadDungeonEntryMap& protocol);

void HandleProtocol(CLProtocol::SceneNotifyLoadDynMap& protocol);

void HandleProtocol(CLProtocol::SceneNotifyLoadBattleScene& protocol);

void HandleProtocol(CLProtocol::WorldRegisterSceneRet& protocol);

void HandleProtocol(CLProtocol::WorldUnregisterSceneRet& protocol);

void HandleProtocol(CLProtocol::SceneSetParam& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneTestNetDelay& protocol);

void HandleProtocol(CLProtocol::SysAsyncWaitMsgRet& protocol);

void HandleProtocol(CLProtocol::SceneSummonNpc& protocol);

#endif
