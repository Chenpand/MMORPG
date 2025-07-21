#ifndef _OBSERVE_HANDLER_H_
#define _OBSERVE_HANDLER_H_

#include <CLMatchProtocol.h>
#include <CLSysProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLObserveProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolInNode& protocol);


void HandleProtocol(CLProtocol::UnionChampionObserveReq& protocol);

void HandleProtocol(CLProtocol::ObserveRealyAddObservePlayerRes& protocol);

void HandleProtocol(CLProtocol::ObserveRealyCreateRaceRes& protocol);

void HandleProtocol(CLProtocol::ObserveStartRace& protocol);

void HandleProtocol(CLProtocol::ObserveSyncFrame& protocol);
void HandleProtocol(CLProtocol::ObserveCancelOverload& protocol);

void HandleProtocol(CLProtocol::RelayAbalitySync& protocol);


#endif