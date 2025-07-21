#ifndef _WORLD_TRANSFER_HANDLER_H_
#define _WORLD_TRANSFER_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLTransferProtocol.h>
#include "WSPlayerMgr.h"

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTwo& protocol);

void HandleProtocol(WSPlayer* player, TransferHeader& tHeader, CLProtocol::TransferQueryPlayerDetailTwo& protocol);

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTipsOne& protocol);

void HandleProtocol(WSPlayer* player, TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTipsTwo& protocol);

#endif // !_WORLD_TRANSFER_HANDLER_H_

