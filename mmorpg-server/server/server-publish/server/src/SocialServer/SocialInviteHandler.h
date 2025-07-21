#ifndef _SOCIAL_INVITE_HANDLER_H_
#define _SOCIAL_INVITE_HANDLER_H_

#include <CLInviteGameProtocol.h>

void HandleProtocol(CLProtocol::SocialInviteReportOnline& protocol);

void HandleProtocol(CLProtocol::SocialInviteNotifyLevelup& protocol);

void HandleProtocol(CLProtocol::SocialInviteNotifyConsume& protocol);

void HandleProtocol(CLProtocol::SocialNotifyInviteeEnterGame& protocol);

void HandleProtocol(CLProtocol::SocialInviteInviteeListReq& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckGiftbagReq& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckRecallAwardReq& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckConsumeAwardReq& protocol);

void HandleProtocol(CLProtocol::SocialInviteBindKLActivity& protocol);

void HandleProtocol(CLProtocol::SocialInviteCheckKLActivityReq& protocol);

void HandleProtocol(CLProtocol::SceneInviteBeginExchangeCDKeyReq& protocol);

void HandleProtocol(CLProtocol::SceneInviteEndExchangeCDKeyReq& protocol);

#endif
