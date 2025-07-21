#include "CSScoreWarHandler.h"
#include "CSScoreWarMgr.h"
#include "CSRoomMgr.h"
#include "CLErrorCode.h"

void HandleProtocol(CLProtocol::SceneCrossSyncScoreWarInfoRes& sync)
{
	CSScoreWarMgr::Instance()->RemoveWaitResultConnId(sync.GetConnID());
}

void HandleProtocol(CLProtocol::CrossScoreWarInfoReq& req)
{
	CSScoreWarMgr::Instance()->SyncScoreWarStatus(req.GetConnID());
}

void HandleProtocol(CLProtocol::CrossScoreWarSortListReq& req)
{
	InfoLogStream << "HandleProtocol CrossScoreWarSortListReq(type=" << req.type << ", playerId=" << req.playerId << ")" << LogStream::eos;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	CSScoreWarMgr::Instance()->ListSortEntries(zone, req.playerId, (SortListType)req.type, req.start, req.num);
}

void HandleProtocol(CLProtocol::CrossScoreWarSortListSync& sync)
{
	InfoLogStream << "HandleProtocol CrossScoreWarSortListSync(playerId=" << sync.entry.playerId << ")" << LogStream::eos;
	sync.entry.connId = sync.GetConnID();
	CSScoreWarMgr::Instance()->UpdateSortList(sync.entry);
}

void HandleProtocol(CLProtocol::SceneCrossGMSync& sync)
{
	InfoLogStream << "HandleProtocol SceneCrossGMSync(isByTime=" << sync.isByTime << ", targetStatus=" << sync.targetStatus << ")" << LogStream::eos;

	CSScoreWarMgr::Instance()->SetIsByTime(sync.isByTime == 1 ? true : false);
	CSScoreWarMgr::Instance()->ChangeTargetStatus((ScoreWarStatus)sync.targetStatus);
}

void HandleProtocol(CLProtocol::CrossScoreWarBattleInfoSync& sync)
{
	InfoLogStream << "HandleProtocol CrossScoreWarBattleInfoSync(playerId=" << sync.playerId << " | " << sync.roomId << ")" << LogStream::eos;

	UInt32 nRes = CSRoomMgr::Instance()->UpdateRoomMatchInfo(sync.roomId, sync.playerId, sync.loseStreak, sync.winStreak, sync.matchScore, sync.battleCount);
	
	if (nRes == ErrorCode::SUCCESS)
	{
		ErrorLogStream << "HandleProtocol CrossScoreWarBattleInfoSync(playerId=" << sync.playerId << " | " << sync.roomId << ") resut(" << nRes << ")" << LogStream::eos;
	}
}


