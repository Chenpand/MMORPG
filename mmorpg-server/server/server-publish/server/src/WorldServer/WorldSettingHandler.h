#ifndef _WORLD_SETTING_HANDLER_H_
#define _WORLD_SETTING_HANDLER_H_

#include <CLSettingProtocol.h>

void HandleProtocol(CLProtocol::SceneNotifyChangeNameToWorld& protocol);

void HandleProtocol(CLProtocol::SceneNotifyChangeSexToWorld& protocol);

void HandleProtocol(CLProtocol::SceneGameSetSync& protocol);

#endif
