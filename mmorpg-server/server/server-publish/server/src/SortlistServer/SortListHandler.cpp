#include "SortListHandler.h"

#include "SortListMgr.h"
#include "GameZoneMgr.h"

void HandleProtocol(CLProtocol::WorldSortListUpdateReq& protocol)
{
	SortListMgr::Instance()->UpdateSortList(protocol.entry);
}

void HandleProtocol(CLProtocol::WorldSortListVisitReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	SortListMgr::Instance()->ListSortEntries(zone, protocol.sortType, protocol.function, protocol.num);
}

void HandleProtocol(ObjID_t playerId, CLProtocol::WorldSortListReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	SortListMgr::Instance()->ListSortEntries(zone, playerId, protocol.type, protocol.start, protocol.num);
}

void HandleProtocol(ObjID_t playerId, CLProtocol::WorldSortListNearReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	SortListMgr::Instance()->ListSortEntriesNear(zone, playerId, protocol.type, protocol.num);
}
