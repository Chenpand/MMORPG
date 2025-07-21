#ifndef __CHAMPION_PVP_RACE_H__
#define __CHAMPION_PVP_RACE_H__

#include "PvPRace.h"
#include <CLMatchDefine.h>
#include <CLReplayDefine.h>

struct ReplayInfo;
class ChampionPvPRace : public PvPRace
{
    AVALON_DEFINE_OBJECTPOOL(ChampionPvPRace, Avalon::NullMutex)
public:
	ChampionPvPRace(RaceType raceType, RaceInstanceType subType = RACE_PVP);
    virtual ~ChampionPvPRace();

	/**
	*@brief 比赛最多持续时间(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 10 * 60; }

public:
	virtual void OnRecvInitPacket();

protected:
	/**
	*@brief 收到Observe请求开始同步帧
	*/
	virtual void OnReceiveObserveStartSync();

protected:

	/**
	*@brief 当发送帧数据给所有玩家时
	*/
	virtual void _OnSyncFrames(std::vector<CLProtocol::Frame>& unSyncFrame);


protected:
	// 是否需要向Observe同步帧数据
	bool						m_BoSyncObserve;
};

#endif