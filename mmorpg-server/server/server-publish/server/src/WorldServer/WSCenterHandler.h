#ifndef _WS_CENTER_HANDLER_H_
#define _WS_CENTER_HANDLER_H_

#include <CLCenterProtocol.h>
#include <CLChatProtocol.h>

class WSPlayer;

void HandleProtocol(CLProtocol::CenterForbidTalkListRet& protocol);

void HandleProtocol(CLProtocol::CenterNotifyNewForbidTalk& protocol);

void HandleProtocol(CLProtocol::CenterNotifyDelForbidTalk& protocol);


void HandleProtocol(CLProtocol::CenterCustomServiceSignRes& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldCustomServiceSignReq& req);


void HandleProtocol(CLProtocol::CenterForbidAccountNotify& protocol);

void HandleProtocol(CLProtocol::CenterGlobalServerInfoRet& protocol);

void HandleProtocol(CLProtocol::CenterQueryConvertAccountRes& protocol);

#endif

