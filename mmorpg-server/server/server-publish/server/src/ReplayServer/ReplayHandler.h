#ifndef __REPLAY_HANDLER_H__
#define __REPLAY_HANDLER_H__

#include <CLReplayProtocol.h>
#include <CLPacketExecutor.h>

void HandleProtocol(CLProtocol::ReplayReportPlayerOnline& protocol);
void HandleProtocol(CLProtocol::ReplayReportPlayerOffline& protocol);
void HandleProtocol(CLProtocol::ReplayListReq& protocol);
void HandleProtocol(CLProtocol::ReplayReportView& protocol);

void HandleProtocol(CLProtocol::ReplayReportRace& protocol);
void HandleProtocol(CLProtocol::ReplayFileRes& res);

class ReplayFileResFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};


#endif