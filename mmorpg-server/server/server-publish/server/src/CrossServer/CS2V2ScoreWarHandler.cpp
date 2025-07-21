#include "CS2V2ScoreWarHandler.h"
#include "CS2V2ScoreWarMgr.h"
#include "CSRoomMgr.h"
#include "CLErrorCode.h"

void HandleProtocol(CLProtocol::Scene2V2CrossSyncScoreWarInfoRes& sync)
{
	CS2V2ScoreWarMgr::Instance()->RemoveWaitResultConnId(sync.GetConnID());
}

void HandleProtocol(CLProtocol::Cross2V2ScoreWarInfoReq& req)
{
	CS2V2ScoreWarMgr::Instance()->SyncScoreWarStatus(req.GetConnID());
}

void HandleProtocol(CLProtocol::Cross2V2ScoreWarSortListReq& req)
{
	InfoLogStream << "HandleProtocol Cross2V2ScoreWarSortListReq(type=" << req.type << ", playerId=" << req.playerId << ")" << LogStream::eos;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	CS2V2ScoreWarMgr::Instance()->ListSortEntries(zone, req.playerId, (SortListType)req.type, req.start, req.num);
}

void HandleProtocol(CLProtocol::Cross2V2ScoreWarSortListSync& sync)
{
	InfoLogStream << "HandleProtocol Cross2V2ScoreWarSortListSync(playerId=" << sync.entry.playerId << ")" << LogStream::eos;
	sync.entry.connId = sync.GetConnID();
	CS2V2ScoreWarMgr::Instance()->UpdateSortList(sync.entry);
}


void HandleProtocol(CLProtocol::Cross2V2ScoreWarBattleInfoSync& sync)
{
	InfoLogStream << "HandleProtocol Cross2V2ScoreWarBattleInfoSync(playerId=" << sync.playerId << " | " << sync.roomId << ")" << LogStream::eos;

	UInt32 nRes = CSRoomMgr::Instance()->UpdateRoomMatchInfo(sync.roomId, sync.playerId, sync.loseStreak, sync.winStreak, sync.matchScore, sync.battleCount);
	
	if (nRes == ErrorCode::SUCCESS)
	{
		ErrorLogStream << "HandleProtocol Cross2V2ScoreWarBattleInfoSync(playerId=" << sync.playerId << " | " << sync.roomId << ") resut(" << nRes << ")" << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::SceneCross2V2GMSync& sync)
{
	InfoLogStream << "HandleProtocol SceneCrossGMSync(isByTime=" << sync.isByTime << ", targetStatus=" << sync.targetStatus << ")" << LogStream::eos;

	CS2V2ScoreWarMgr::Instance()->SetIsByTime(sync.isByTime == 1 ? true : false);
	CS2V2ScoreWarMgr::Instance()->ChangeTargetStatus((ScoreWar2V2Status)sync.targetStatus);
}


