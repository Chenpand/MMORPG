#ifndef __OBSERVED_RACE_H__
#define __OBSERVED_RACE_H__

#include "Race.h"
#include <CLMatchDefine.h>
#include <CLReplayDefine.h>
#include <AvalonSimpleTimer.h>

struct ReplayInfo;


class ObservedRace : public Race
{
	AVALON_DEFINE_OBJECTPOOL(ObservedRace, Avalon::NullMutex)
public:
	ObservedRace(RaceType raceType, RaceInstanceType subType = RACE_PVP);
	virtual ~ObservedRace();


	virtual void Init();

	virtual void OnTick(const Avalon::Time& now);

	/**
	*@brief pk类型
	*/
	PkType GetPkType() const { return m_pkType; }
	void SetPkType(PkType type) { m_pkType = type; }

	/**
	*@brief pk场景
	*/
	UInt32 GetPkDungeonID() const { return m_pkDungeonId; }
	void SetPkDungeonID(UInt32 id) { m_pkDungeonId = id; }

	/**
	*@brief 战斗结算
	*/
	virtual void OnRaceEnd();

	/**
	*@brief 比赛最多持续时间
	*/
	virtual UInt32 GetRaceMaxTime();

	/**
	*@brief 发送结算消息到MatchServer
	*/
	virtual void SendRaceEndToMatchServer(EGameSessionDismissType reason) {};

	/**
	*@brief 检测结算合法性
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason) {};

	/**
	*@brief 减少Observe一个负载
	*/
	void CancelObserveOverload();

	/**
	*@brief 收到Observe请求开始同步帧
	*/
	virtual void OnObserveRealyAddObservePlayer(UInt64 roleID);

	/**
	*@brief 玩家开始观战
	*/
	virtual UInt32 OnObserverLogin(RLPlayer* player, UInt32 startFrame);

	/**
	*@brief 收到observe同步数据帧
	*/
	virtual void OnReceiveObserveFrames(std::vector<CLProtocol::Frame>& frames);

	/**
	*@brief 获取本场比赛观战定时器负载数量
	*/
	virtual UInt32 GetObserveLoad();

protected:
	// PK类型
	PkType						m_pkType;
	// pk场景id
	UInt32						m_pkDungeonId;
	// 所有玩家的战斗信息
	std::vector<RacePlayerInfo>	m_racePlayerInfoes;
	// 等待观战玩家连接的定时器
	std::map<UInt64, Avalon::SimpleTimer> m_ObserveTimerMap;

};

#endif