#ifndef _SCENE_TRANSFER_HANDLER_H_
#define _SCENE_TRANSFER_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLTransferProtocol.h>
#include <CLRelationProtocol.h>
#include "PlayerMgr.h"

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerOne& protocol);

void HandleProtocol(Player* player, TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTwo& protocol);

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerDetailOne& protocol);


#endif

