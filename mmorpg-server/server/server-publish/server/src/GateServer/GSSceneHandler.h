#ifndef _GS_SCENE_HANDLER_H_
#define _GS_SCENE_HANDLER_H_

#include <CLSceneProtocol.h>
#include <CLItemProtocol.h>

void HandleProtocol(CLProtocol::GateRegisterScene& protocol);

void HandleProtocol(CLProtocol::GateUnregisterScene& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol);

#endif
