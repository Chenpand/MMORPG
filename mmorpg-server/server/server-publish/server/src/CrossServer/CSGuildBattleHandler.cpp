#include "CSGuildBattleHandler.h"
#include "CLErrorCode.h"
#include "CLItemDefine.h"
#include "CSGuildBattleMgr.h"
#include "GameZoneMgr.h"


void HandleProtocol(CLProtocol::WorldCrossGuildBattleEnrollInfoRes& res)
{
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "HandleProtocol WorldCrossGuildBattleEnrollInfoRes(" << res.result << ")" << LogStream::eos;
		return;
	}
	else
	{
		InfoLogStream << "HandleProtocol WorldCrossGuildBattleEnrollInfoRes(" << res.result << ")" << LogStream::eos;
	}

	CSGuildBattleMgr::Instance()->LoadEnroll(res.terrEnrollInfo);
	CSGuildBattleMgr::Instance()->SetTerritoryBaseInfo(res.terrBaseInfo);
	CSGuildBattleMgr::Instance()->RemoveWaitLoadEnrollConnId(res.GetConnID());

	CLProtocol::WorldCrossGuildBattleTerritoryListInfoSync sync;
	UInt32 nRes = CSGuildBattleMgr::Instance()->GetTerritoryBaseInfos(sync.infos);
	if (nRes == ErrorCode::SUCCESS)
	{
		GameZoneMgr::Instance()->TransmitToWorld(sync);
	}
	else
	{
		ErrorLogStream << "HandleProtocol WorldCrossGuildBattleEnrollInfoRes , WorldCrossGuildBattleTerritoryListInfoSync(" << nRes << ")" << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::CrossGuildBattleEnrollReq& req)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleEnrollReq(" << req.info.guildId << ")" << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "HandleProtocol CrossGuildBattleEnrollReq zone ERROR (connID=" << req.GetConnID() << ")" << LogStream::eos;
		return;
	}

	CLProtocol::CrossGuildBattleEnrollRes res;
	res.result = CSGuildBattleMgr::Instance()->AddEnroll(req.info);

	if (res.result == ErrorCode::SUCCESS)
	{
		CLProtocol::WorldCrossGuildBattleTerritoryInfoSync sync;
		UInt32 nRes = CSGuildBattleMgr::Instance()->GetTerritoryBaseInfo(req.info.terrId, sync.info);
		if (nRes == ErrorCode::SUCCESS)
		{
			GameZoneMgr::Instance()->TransmitToWorld(sync);
		}
		else
		{
			ErrorLogStream << "HandleProtocol WorldCrossGuildBattleEnrollReq , WorldCrossGuildBattleTerritoryInfoSync(" << nRes << ")" << LogStream::eos;
		}
	}

	zone->TransmitToWorld(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "HandleProtocol WorldCrossGuildBattleEnrollReq (" << res.result << ")" << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd)
{
	InfoLogStream << "HandleProtocol MatchServerGuildBattleRaceEnd(terrId=" << raceEnd.terrId << ", " << raceEnd.result << ")" << LogStream::eos;
	CSGuildBattleMgr::Instance()->OnCrossBattleRaceEnd(raceEnd);
}

void HandleProtocol(CLProtocol::CrossGuildBattleRecordReq& req)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleRecordReq(terrId=" << req.terrId << ", playerId = " << req.playerId << ")" << LogStream::eos;
	CLProtocol::CrossGuildBattleRecordRes res;
	res.playerId = req.playerId;
	if (req.isSelf)
	{
		res.result = CSGuildBattleMgr::Instance()->GetBattleSelfRecrod(req.terrId, req.playerId, res.records);
	}
	else
	{
		res.result = CSGuildBattleMgr::Instance()->GetBattleRecord(req.terrId, res.records, req.startIndex, req.count);
	}
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "HandleProtocol CrossGuildBattleRecordReq zone ERROR (connID=" << req.GetConnID() << ")" << LogStream::eos;
		return;
	}

	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossGuildBattleSortListSync& req)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleSortListSync(terrId=" << req.terrId << ")" << LogStream::eos;
	CSGuildBattleMgr::Instance()->UpdateSortList(req.terrId, req.entry);
}

void HandleProtocol(CLProtocol::CrossGuildBattleSortListReq& req)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleSortListReq(terrId=" << req.terrId << ", playerId=" << req.playerId << ")" << LogStream::eos;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	CSGuildBattleMgr::Instance()->GetSortList(zone, req.terrId, req.playerId, req.guildId, (SortListType)req.type, req.start, req.num);
}

void HandleProtocol(CLProtocol::CrossGuildBattleSelfSortListReq& req)
{
	CLProtocol::WorldGuildBattleSelfSortListRes selfRes;
	selfRes.result = ErrorCode::SUCCESS;
	selfRes.memberRanking = CSGuildBattleMgr::Instance()->GetRanking(req.terrId, SortListType::SORTLIST_GUILD_BATTLE_MEMBER, req.playerId, req.guildId);
	selfRes.guildRanking = CSGuildBattleMgr::Instance()->GetRanking(req.terrId, SortListType::SORTLIST_GUILD_BATTLE_SCORE, req.playerId, req.guildId);

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (NULL == zone)
	{
		ErrorLogStream << "HandleProtocol CrossGuildBattleSelfSortListReq zone ERROR (connID=" << req.GetConnID() << ")" << LogStream::eos;
		return;
	}

	CLProtocol::CrossGuildBattleSelfSortListRes res;
	res.playerId = req.playerId;
	selfRes.Encode(res.packet);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossGuildBattleGMSync& sync)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleGMSync(isByTime=" << sync.isByTime << ", status=" << sync.targetStatus << ")" << LogStream::eos;
	CSGuildBattleMgr::Instance()->SetIsByTime(sync.isByTime == 1 ? true : false);
	CSGuildBattleMgr::Instance()->ChangeTargetStatus((GuildBattleStatus)sync.targetStatus);
	CSGuildBattleMgr::Instance()->SetGuildBattleType(GuildBattleType::GBT_CROSS);
}
