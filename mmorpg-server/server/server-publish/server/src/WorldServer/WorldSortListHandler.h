#ifndef _WORLD_SORTLIST_HANDLER_H_
#define _WORLD_SORTLIST_HANDLER_H_

#include <CLPacketExecutor.h>
#include <CLSortListProtocol.h>
#include <CLScoreWarProtocol.h>
#include <CL2V2ScoreWarProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSortListReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSortListNearReq& protocol);

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldSortListSelfInfoReq &protocol);

void HandleProtocol(CLProtocol::WorldSortListUpdateReq& protocol);

void HandleProtocol(CLProtocol::WorldSortListOwnerChangedReq& protocol);

void HandleProtocol(CLProtocol::WorldSortListDeleteEntryReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSortListWatchDataReq& protocol);

class SortListWatchDataFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::WorldSortListVisitRet& protocol);

void HandleProtocol(CLProtocol::CrossScoreWarSortListRes& protocol);

void HandleProtocol(CLProtocol::Cross2V2ScoreWarSortListRes& protocol);

void HandleProtocol(CLProtocol::WorldScoreWarRankingReward& protocol);

void HandleProtocol(CLProtocol::World2V2ScoreWarRankingReward& protocol);

#endif
