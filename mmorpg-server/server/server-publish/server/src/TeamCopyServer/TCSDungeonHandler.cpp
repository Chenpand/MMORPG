#include "TCSDungeonHandler.h"

#include "TCSPlayer.h"
#include "TCSTeamMgr.h"
#include "TCSDungeonRace.h"
#include "TCSDungeonMgr.h"

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceRes& protocol)
{
	TCSTeamMgr::Instance()->HandlerStartRaceRes(protocol);
}

void HandleProtocol(CLProtocol::DungeonSvrDungeonRaceEnd& protocol)
{
	TCSTeamMgr::Instance()->HandlerRaceEnd(protocol);
}

void HandleProtocol(CLProtocol::RelayDungeonBossPhaseChange& protocol)
{
	// 暂时去掉这个功能
	//TCSDungeonMgr::Instance()->HandlerBossPhase(protocol.bossphase, protocol.raceSessionId);
}

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol)
{
	TCSDungeonMgr::Instance()->SendToDungeonSvr(protocol);
}

void HandleProtocol(CLProtocol::TeamCopyDungeonRevive& protocol)
{
	UInt32 teamSquadId = TCSTeamMgr::Instance()->GetPlayerTeamSquadId(protocol.playerId);
	TCSDungeonRace* race = TCSDungeonMgr::Instance()->FindRaceByTeamSquadId(teamSquadId);
	if (NULL == race)
	{
		ErrorLogStream << "revive not find race playerId:" << protocol.playerId << 
			" targetId:" << protocol.targetId <<
			" teamSquadId:" << teamSquadId << LogStream::eos;
		return;
	}

	CLProtocol::DungeonSvrReviveReq req;
	req.raceSessionId = race->GetRaceId();
	req.playerId = protocol.playerId;
	req.targetId = protocol.targetId;
	req.reviveId = protocol.reviveId;
	req.reviveCoinNum = protocol.reviveCoinNum;

	TCSDungeonMgr::Instance()->SendToDungeonSvr(req);
}

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol)
{
	TCSDungeonMgr::Instance()->SendToDungeonSvr(protocol);
}

void HandleProtocol(CLProtocol::SceneToDungeonQuickBuyTransactionReq& protocol)
{
	TCSDungeonMgr::Instance()->SendToDungeonSvr(protocol);
}

void HandleProtocol(CLProtocol::TeamCopyPhaseBossInfo& protocol, TCSPlayer* player)
{
	/*DebugLogStream << "phase boss raceId:" << protocol.raceId <<
		" roleId:" << protocol.roleId << " curFrame:" << protocol.curFrame <<
		" phase:" << protocol.phase << " bossBloodRate:" << protocol.bossBloodRate << LogStream::eos;*/

	TCSDungeonRace* race = TCSDungeonMgr::Instance()->FindRaceByRaceId(protocol.raceId);
	if (NULL == race)
	{
		ErrorLogStream << "phase boss not find raceId:" << protocol.raceId << LogStream::eos;
		return;
	}

	TCSTeamMgr::Instance()->HandlerPhaseBoss(protocol.roleId, protocol.phase, protocol.curFrame, protocol.bossBloodRate);
}

void HandleProtocol(CLProtocol::SceneSyncEvent& protocol)
{
	TCSDungeonMgr::Instance()->SendToDungeonSvr(protocol);
}