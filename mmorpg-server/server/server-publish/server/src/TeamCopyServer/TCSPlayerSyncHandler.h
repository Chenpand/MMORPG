#ifndef _TCS_PLAYER_SYNC_HANDLER_H_
#define _TCS_PLAYER_SYNC_HANDLER_H_

#include <CLTeamCopyProtocol.h>

void HandleProtocol(CLProtocol::TeamCopyPlayerInfoSync& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerTicketSync& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerEquipScoreSync& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerLoadingSync& protocol);

#endif
