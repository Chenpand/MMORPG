#ifndef _SCENE_SORTLIST_HANDLER_H_
#define _SCENE_SORTLIST_HANDLER_H_

#include <CLPacketExecutor.h>
#include <CLSortListProtocol.h>

class SyncSortListFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};


void HandleProtocol(CLProtocol::SceneSyncSortListMinValue& protocol);

void HandleProtocol(CLProtocol::SceneSyncSortListEntry& protocol);

void HandleProtocol(CLProtocol::SceneSortListWatchDataReq& protocol);

void HandleProtocol(CLProtocol::SceneSortListDailyReward& protocol);

void HandleProtocol(CLProtocol::SceneSortListResetSeason& protocol);

#endif
