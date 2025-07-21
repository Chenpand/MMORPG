#ifndef _TCS_DUNGEON_MGR_H_
#define _TCS_DUNGEON_MGR_H_

#include <set>
#include <CLDefine.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>

class TCSDungeonRace;

/**
*@brief 团本地下城管理
*/
class TCSDungeonMgr : public Avalon::Singleton<TCSDungeonMgr>
{
public:
	TCSDungeonMgr();
	~TCSDungeonMgr();

public:

	/**
	*@brief 地下城服务连接
	*/
	void OnConnected(UInt32 id);

	/**
	*@brief 地下城服务断开
	*/
	void OnDisconnected(UInt32 id);

	/**
	*@brief 发送到地下城服务
	*/
	void SendToDungeonSvr(Avalon::Packet* packet);
	void SendToDungeonSvr(Avalon::Protocol& protocol);

	/**
	*@brief 比赛
	*/
	void AddRace(UInt64 raceId, UInt32 teamSquadId, UInt32 fieldId);
	void RemoveRace(UInt64 raceId);

	/**
	*@brief 处理boss阶段
	*/
	void HandlerBossPhase(UInt32 bossPhase, UInt64 raceSessionId);

	/**
	*@brief 结束比赛
	*/
	void DungeonRaceEnd(UInt64 raceSessionId, UInt32 reason);

	/**
	*@brief 据点被歼灭
	*/
	void DungeonRaceDestroy(UInt64 raceSessionId, UInt32 dungeonId);

	/**
	*@brief 增加复活次数
	*/
	void AddRaceReviveCnt(UInt64 raceSessionId, UInt32 reviveCnt);

	/**
	*@brief 重连结束比赛
	*/
	void ReconnectEndRace(UInt64 roldeId);

	/**
	*@brief 查找比赛
	*/
	TCSDungeonRace* FindRaceByTeamSquadId(UInt32 teamSquadId);

	/**
	*@brief 查找比赛
	*/
	TCSDungeonRace* FindRaceByRaceId(UInt64 raceId);

private:

	// 地下城服务器连接
	Avalon::NetConnectionPtr m_DungeonSvrPtr;

	// 比赛(key->比赛id)
	std::map<UInt64, TCSDungeonRace*> m_DungeonRaceMap;

	// 小队比赛(key->队伍小队)
	std::map<UInt32, TCSDungeonRace*> m_TeamSquadRaceMap;

	// boss阶段(key->比赛id, val->阶段)
	std::map<UInt64, std::set<UInt32>> m_RacePhaseMap;
};

#endif
