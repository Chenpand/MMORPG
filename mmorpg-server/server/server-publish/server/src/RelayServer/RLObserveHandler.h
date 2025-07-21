#ifndef __RLOBSERVE_HANDLE_H__
#define __RLOBSERVE_HANDLE_H__

#include <CLObserveProtocol.h>
void HandleProtocol(CLProtocol::ObserveRealyCreateRaceReq& req);
void HandleProtocol(CLProtocol::ObserveRealyAddObservePlayerReq& req);
void HandleProtocol(CLProtocol::RelayStartReceiveRaceFrame& req);
void HandleProtocol(CLProtocol::ObserveSyncFrame& req);

#endif
