#include "TransferSysHandler.h"

#include "GameZoneMgr.h"

void HandleProtocol(CLProtocol::SysTransferProtocol& sync)
{
	TransferHeader& header = sync.tHeader;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(header.destZone);
	if (NULL == zone)
	{
		ErrorLogStream << "not find game zone:" << sync.LogStr() << LogStream::eos;
		return;
	}

	switch (header.destServer)
	{
	case ST_WORLD:zone->TransmitToWorld(sync); break;
	case ST_SCENE:zone->TransmitToScene(sync); break;
	default:
		return;
	}
}