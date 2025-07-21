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
	*@brief ����������ʱ��(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 10 * 60; }

public:
	virtual void OnRecvInitPacket();

protected:
	/**
	*@brief �յ�Observe����ʼͬ��֡
	*/
	virtual void OnReceiveObserveStartSync();

protected:

	/**
	*@brief ������֡���ݸ��������ʱ
	*/
	virtual void _OnSyncFrames(std::vector<CLProtocol::Frame>& unSyncFrame);


protected:
	// �Ƿ���Ҫ��Observeͬ��֡����
	bool						m_BoSyncObserve;
};

#endif