#ifndef _SETTING_HANDLER_H_
#define _SETTING_HANDLER_H_

#include <CLSettingProtocol.h>
#include <CLSceneProtocol.h>

class Player;

//void HandleProtocol(Player* player, CLProtocol::SceneSetQuickBar& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneExchangeSkillBarReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneSetWeaponBarReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneSetPvpSkillConfigReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneSetEqualPvpSkillConfigReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneExchangeItemBarReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneSaveOptionsReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneCheckChangeNameReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneChangeNameReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneChangeSexReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneNotifyAppOnPanel& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneChangeOccu& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneNotifyLoadingInfo& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneGameSetReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneReportNotify& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneShortcutKeySetReq& protocol);

#endif
 