#ifndef _SCENE_INVITE_HANDLER_H_
#define _SCENE_INVITE_HANDLER_H_

#include <CLInviteGameProtocol.h>

class Player;

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteDailyInfoReq &protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteInviteeListReq& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteNotifyInvited& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteGetGiftbag& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteGetRecallAwardReq& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteGetConsumeAwardReq& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteBindKLPlayer& protocol);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneInviteExchangeCDKey& protocol);

void HandleProtocol(CLProtocol::SocialInviteInviteeListRet& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckGiftbagRet& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckRecallAwardRet& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckConsumeAwardRet& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckKLActivityRet& protocol);

void HandleProtocol(CLProtocol::SceneInviteBeginExchangeCDKeyRet& protocol);

#endif
