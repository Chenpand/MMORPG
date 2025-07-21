#include "WorldDigHandler.h"
#include "DigMapMgr.h"
#include "WSPlayer.h"
#include "WSServiceSwitchMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigMapOpenReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldDigMapOpenReq(" << protocol.mapId << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
	{
		return;
	}

	CLProtocol::WorldDigMapOpenRes res;
	res.mapId = protocol.mapId;
	res.result = DigMapMgr::Instance()->OpenMap(player, protocol.mapId, res.infos, res.playerSize);
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " WorldDigMapOpenRes(" << res.result << ")." << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigMapCloseReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldDigMapCloseReq(" << protocol.mapId << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
	{
		return;
	}

	CLProtocol::WorldDigMapCloseRes res;
	res.result = DigMapMgr::Instance()->CloseMap(player, protocol.mapId);
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " WorldDigMapCloseRes(" << res.result << ")." << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigWatchReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldDigMapCloseReq(" << protocol.mapId << "|" << protocol.index << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
	{
		return;
	}

	CLProtocol::WorldDigWatchRes res;
	res.mapId = protocol.mapId;
	res.index = protocol.index;
	res.result = DigMapMgr::Instance()->WatchDig(protocol.mapId, protocol.index, res.info);
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " WorldDigWatchReq(" << res.result << ")." << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigOpenReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldDigOpenReq(" << protocol.mapId << "|" << protocol.index << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
	{
		return;
	}

	UInt32 nRes = DigMapMgr::Instance()->OpenDig(player, protocol.mapId, protocol.index);
	if (nRes != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " WorldDigOpenReq(" << nRes << ")." << LogStream::eos;
		CLProtocol::WorldDigOpenRes res;
		res.result = nRes;
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigMapInfoReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldDigMapInfoReq()." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
	{
		return;
	}

	CLProtocol::WorldDigMapInfoRes res;
	res.result = DigMapMgr::Instance()->GetDigMapInfo(res.digMapInfos);
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " WorldDigMapInfoRes(" << res.result << ")." << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigRecordsReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldDigRecordsReq()." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
	{
		return;
	}

	CLProtocol::WorldDigRecordsRes res;
	res.result = DigMapMgr::Instance()->GetDigRecords(res.infos);
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " WorldDigRecordsReq(" << res.result << ")." << LogStream::eos;
	}
}

