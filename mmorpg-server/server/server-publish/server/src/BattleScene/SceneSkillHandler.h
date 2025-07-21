#ifndef _SCENE_SKILL_HANDLER_H_
#define _SCENE_SKILL_HANDLER_H_

#include <CLSkillProtocol.h>

class Player;

void HandleProtocol(Player* player,CLProtocol::SceneChangeSkillsReq& protocol);

void HandleProtocol(Player* player,CLProtocol::SceneUseSkillsReq& protocol);

void HandleProtocol(Player* player,CLProtocol::SceneReviveReq& protocol);

void HandleProtocol(Player* player,CLProtocol::SceneSwitchPkMode& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneAddBuff& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneInitSkillsReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneRecommendSkillsReq& protocol);

#endif
