#include "ObservedRace.h"

#include <CLGameSessionProtocol.h>
#include <CLReplayProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLObserveProtocol.h>
#include <CLErrorCode.h>

#include "RelaySvrRouter.h"
#include "RLPlayerMgr.h"
#include "RaceMgr.h"
#include "RLApplication.h"

ObservedRace::ObservedRace(RaceType raceType, RaceInstanceType subType)
: Race(raceType, subType)
{
	m_pkType = PK_CHAMPION_BATTLE;
	m_pkDungeonId = 0;
}

ObservedRace::~ObservedRace()
{

}

void ObservedRace::Init()
{
	Race::Init();
}

void ObservedRace::OnTick(const Avalon::Time & now)
{
	if (GetStatus() == RACE_STATUS_RACING)
	{

	}
	else if (GetStatus() == RACE_STATUS_WAIT_RACE_END)
	{
		OnRaceEnd();
	}

	//处理Observe观战负载定时器
	{
		std::vector<UInt64> removeTimerVec;
		for (auto& iter : m_ObserveTimerMap)
		{
			if (iter.second.IsTimeout(now))
			{
				CancelObserveOverload();
				removeTimerVec.push_back(iter.first);
			}
		}

		for (auto& i : removeTimerVec)
		{
			m_ObserveTimerMap.erase(i);
		}
	}
	

	OnTickObserving(now);

	if (GetStatus() != RACE_STATUS_DESTORY && (UInt32)now.Sec() - m_createTime > GetRaceMaxTime())
	{
		OnRaceTimeout();
	}
}

void ObservedRace::OnRaceEnd()
{
	m_raceEndTime = CURRENT_TIME;
	ChangeStatus(RACE_STATUS_DESTORY);
}

UInt32 ObservedRace::GetRaceMaxTime()
{
	return 10 * 60;
}

void ObservedRace::CancelObserveOverload()
{
	CLProtocol::ObserveCancelOverload req;
	Router::SendToObserve(req);
}

void ObservedRace::OnObserveRealyAddObservePlayer(UInt64 roleID)
{
	auto iter = m_ObserveTimerMap.find(roleID);
	if (iter != m_ObserveTimerMap.end())
	{
		ErrorLogStream << "Player already Observe" << LogStream::eos;
		return;
	}
	m_ObserveTimerMap[roleID].Reset();
	m_ObserveTimerMap[roleID].SetInterval(RLApplication::Instance()->GetObserveWaitSec() * Avalon::Time::MSECS_OF_SEC);
}

UInt32 ObservedRace::OnObserverLogin(RLPlayer * player, UInt32 startFrame)
{
	if (!player)
	{
		return ErrorCode::SYS_ERROR;
	}

	//玩家已经开始观战，取消负载定时器
	auto iter = m_ObserveTimerMap.find(player->GetID());
	if (iter == m_ObserveTimerMap.end())
	{
		return ErrorCode::RELAY_OBSERVE_RACE_CANNOT_OBSERVE;
	}
	m_ObserveTimerMap.erase(player->GetID());


	DebugLogStream << "race(" << GetID() << ") observer(" << player->GetID() << ") login, current ability(" << RLPlayerMgr::Instance()->get_ablity() << ")." << LogStream::eos;

	auto gamer = GetObserverByRoleID(player->GetID());
	if (gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") repeat observer(" << player->GetID() << ") wants to login." << LogStream::eos;
		return ErrorCode::RELAY_OBSERVE_REPEAT_PLAYER;
	}

	if (!CanObserve())
	{
		return ErrorCode::RELAY_OBSERVE_RACE_CANNOT_OBSERVE;
	}

	// 开始观战

	RaceGamer observer;
	observer.SetAccountID(player->get_accId());
	observer.SetID(player->GetID());
	observer.SetPlayer(player);
	observer.SetLastObserveFrame(startFrame);
	observer.SetLoadProgress(0);
	m_observers.push_back(observer);

	return ErrorCode::SUCCESS;
}

void ObservedRace::OnReceiveObserveFrames(std::vector<CLProtocol::Frame>& frames)
{
	m_allFrames.insert(m_allFrames.end(), frames.begin(), frames.end());
	m_framesData.insert(m_framesData.end(), frames.begin(), frames.end());
}

UInt32 ObservedRace::GetObserveLoad()
{
	return m_ObserveTimerMap.size();
}
