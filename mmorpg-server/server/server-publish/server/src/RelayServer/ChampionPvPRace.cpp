#include "ChampionPvPRace.h"
#include <CLGameSessionProtocol.h>
#include <CLReplayProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLObserveProtocol.h>

#include "RelaySvrRouter.h"
#include "RLPlayerMgr.h"
#include "RaceMgr.h"
#include "RLApplication.h"

ChampionPvPRace::ChampionPvPRace(RaceType raceType, RaceInstanceType subType)
: PvPRace(raceType, subType)
{
	m_pkType = PK_CHAMPION_BATTLE;
	m_pkDungeonId = 0;
	m_replayScore = 0;
	m_BoSyncObserve = false;
}

ChampionPvPRace::~ChampionPvPRace()
{

}

void ChampionPvPRace::OnRecvInitPacket()
{
	CLProtocol::ObserveStartRace req;
	req.raceID = GetID();
	req.packet = GetInitPacket();
	Router::SendToObserve(req);
	req.Detach();
}

void ChampionPvPRace::OnReceiveObserveStartSync()
{
	m_BoSyncObserve = true;
	// 开始同步之前的帧
	InfoLogStream << "race(" << GetID() << ") ObserveStartSync..." << LogStream::eos;

	// 先把之前产生的帧发到Observe
	{
		CLProtocol::ObserveSyncFrame sync;
		sync.raceId = GetID();

		for (auto& frame : m_framesData)
		{
			sync.frames.push_back(frame);

			// 一次只发部分帧
			//if (sync.frames.size() == 1000)
			//{
			//	Router::SendToObserve(sync);
			//	sync.frames.clear();
			//}
		}

		if (!sync.frames.empty())
		{
			Router::SendToObserve(sync);
		}
	}
}

void ChampionPvPRace::_OnSyncFrames(std::vector<CLProtocol::Frame>& unSyncFrame)
{
	if (m_BoSyncObserve)
	{
		CLProtocol::ObserveSyncFrame sync;
		sync.raceId = GetID();
		sync.frames = unSyncFrame;
		Router::SendToObserve(sync);
	}
}
