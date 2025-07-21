#ifndef __GS_ACTIVITY_HANDLER_H__
#define __GS_ACTIVITY_HANDLER_H__

#include <CLOpActiveProtocol.h>
#include <CLGSActivityProtocol.h>

void HandleProtocol(CLProtocol::WSSyncOpActivityData& protocol);

void HandleProtocol(CLProtocol::WSOpActivityChange& protocol);

void HandleProtocol(CLProtocol::WorldNotifyGameStartTimeToS &protocol);

#endif