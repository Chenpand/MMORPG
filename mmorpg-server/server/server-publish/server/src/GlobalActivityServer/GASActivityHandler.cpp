#include "GASActivityHandler.h"
#include "GASGameZoneMgr.h"
#include "ActivityMgr.h"
#include "ArtifactJarMgr.h"
#include "WeekSignMgr.h"
#include "WholeBargainMgr.h"
#include "GASPlayer.h"

void HandleProtocol(CLProtocol::GASActivityInfoReq& req)
{
	InfoLogStream << "Server(" << req.adminNodeId << ") request activities." << LogStream::eos;

	GASGameZone* zone = GASGameZoneMgr::Instance()->FindGASGameZone(req.GetConnID());
	if (!zone)
	{
		ErrorLogStream << "Can not find connection(" << req.GetConnID() << ")!" << LogStream::eos;
		return;
	}

	CLProtocol::GASActivitySync protocol;
	ActivityMgr::Instance()->VisitOpActivities([&](OpActivityInfo* opAct)->bool{
		protocol.opActs.push_back(*opAct);
		return true;
	});
	
	if (protocol.opActs.empty())
	{
		ErrorLogStream << "Activties are empty!" << LogStream::eos;
	}
	else
	{
		zone->TransmitToWorld(req.adminNodeId, protocol);
	}
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASArtifactJarLotterySignReq& req)
{
	ArtifactJarMgr::Instance()->HandleGASArtifactJarLotterySignReq(player, req.jarIdVec);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASArtifactJarLotteryRecordReq& req)
{
	ArtifactJarMgr::Instance()->HandleGASArtifactJarLotteryRecordReq(player, req.jarId);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASArtifactJarLotteryCfgReq& req)
{
	ArtifactJarMgr::Instance()->HandleGASArtifactJarLotteryCfgReq(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASWeekSignRecordReq& req)
{
	WeekSignMgr::Instance()->HandleWeekSignRecord(player, req.opActType);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASWeekSignLotteryReq& req)
{
	WeekSignMgr::Instance()->HandleWeekSignReward(player, req.actType, req.actId, req.vipLvl);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASWholeBargainJoinNumSync& sync)
{
	WholeBargainMgr::Instance()->HandleAddJoinNum(player->GetUId(), sync.joinNum);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GASWholeBargainReq& req)
{
	WholeBargainMgr::Instance()->HandleWholeBargainReq(player);
}

void HandleProtocol(CLProtocol::GASWholeBargainDiscountQuery& protocol)
{
	WholeBargainMgr::Instance()->ZoneQueryDiscount(protocol.GetConnID(), protocol.actId);
}