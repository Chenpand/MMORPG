#include "RLObserveHandler.h"

#include <CLErrorCode.h>

#include "RelaySvrRouter.h"
#include "RaceMgr.h"
#include "Race.h"
#include "RLApplication.h"

void HandleProtocol(CLProtocol::ObserveRealyCreateRaceReq & req)
{
	InfoLogStream << "request create observe race(" << req.raceID << ")." << LogStream::eos;
	auto race = RaceMgr::Instance()->FindRace(req.raceID);
	if (race == nullptr)
	{
		race = RaceMgr::Instance()->CreateRace(RT_OBSERVE);
		if (race == nullptr)
		{
			ErrorLogStream << "race(" << req.raceID << ") create instance failed." << LogStream::eos;
			return;
		}

		race->SetID(req.raceID);
		race->Init();
		race->SetInitPacket(req.packet);
		race->SetAllFrames(req.frames);
		req.Detach();
		race->ChangeStatus(RACE_STATUS_RACING);
		RaceMgr::Instance()->AddRace(race);
	}
	CLProtocol::ObserveRealyCreateRaceRes res;
	res.raceID = req.raceID;
	res.result = ErrorCode::SUCCESS;
	Router::SendToObserve(res);
}

void HandleProtocol(CLProtocol::ObserveRealyAddObservePlayerReq & req)
{
	InfoLogStream << "player(" << req.roleID << ") request observer race(" << req.raceID << ")." << LogStream::eos;

	//observeRelay对该玩家的观战增加定时器并创建对应比赛，等待玩家连接，并从Observe同步帧数据
	auto race = RaceMgr::Instance()->FindRace(req.raceID);
	if (race == nullptr)
	{
		ErrorLogStream << "ObserveRealyAddObservePlayer but Race not exists: " << req.raceID << "role id is: " << LogStream::eos;
		return;
	}
	race->OnObserveRealyAddObservePlayer(req.roleID);
	CLProtocol::ObserveRealyAddObservePlayerRes res;
	res.raceID = req.raceID;
	res.roleID = req.roleID;
	res.addr = RLApplication::Instance()->GetAddr();
	Router::SendToObserve(res);
}

void HandleProtocol(CLProtocol::RelayStartReceiveRaceFrame & req)
{
	auto race = RaceMgr::Instance()->FindRace(req.raceID);
	if (race == nullptr)
	{
		return;
	}
	race->OnReceiveObserveStartSync();
}

void HandleProtocol(CLProtocol::ObserveSyncFrame& req)
{
	auto race = RaceMgr::Instance()->FindRace(req.raceId);
	if (race == nullptr)
	{
		return;
	}
	race->OnReceiveObserveFrames(req.frames);
}
