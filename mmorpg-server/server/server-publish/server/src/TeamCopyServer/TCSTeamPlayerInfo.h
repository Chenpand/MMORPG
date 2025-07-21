#ifndef _TCS_TEAM_PLAYER_INFO_H_
#define _TCS_TEAM_PLAYER_INFO_H_

#include <CLDefine.h>
#include <CLMatchDefine.h>

namespace CLProtocol{
	class DungeonFighter;
};

/**
*@brief 进地下城个人信息查询
*/
class TCSTeamPlayerInfo
{
public:
	TCSTeamPlayerInfo();
	~TCSTeamPlayerInfo();

	/**
	*@brief 队伍小队Id
	*/
	inline void SetTeamSquadId(UInt32 id) { m_TeamSquadId = id; }
	inline UInt32 GetTeamSquadId() { return m_TeamSquadId; }

	/**
	*@brief 地下城Id
	*/
	inline void SetDungeonId(UInt32 id) { m_DungeonId = id; }
	inline UInt32 GetDungeonId() { return m_DungeonId; }

	/**
	*@brief 据点Id
	*/
	inline void SetFieldId(UInt32 id) { m_FieldId = id; }
	inline UInt32 GetFieldId() { return m_FieldId; }

	/**
	*@brief 尾部是否通关
	*/
	inline void SetTailDungeonId(UInt32 id) { m_TailDungeonId = id; }
	inline UInt32 GetTailDungeonId() { return m_TailDungeonId; }

	/**
	*@brief 超时时间点
	*/
	inline void SetTimeOut(UInt64 stamp) { m_TimeOut = stamp; }
	inline UInt64 GetTimeOut() { return m_TimeOut; }

	/**
	*@brief 增加玩家
	*/
	void AddTeamPlayer(UInt64 playerId);

	/**
	*@brief 玩家是否在比赛中
	*/
	bool IsHasPlayer(UInt64 playerId);

	/**
	*@brief 设置playerInfo
	*/
	void SetDungeonRacePlayerInfo(UInt64 playerId, DungeonRacePlayerInfo& info);

	/**
	*@brief 是否有全部playerInfo
	*/
	bool IsAllHasInfo();

	/**
	*@brief 获取playerInfos
	*/
	UInt32 GetPlayerInfos(std::vector<CLProtocol::DungeonFighter>& fighters, std::vector<DungeonRacePlayerInfo>& playerInfos);

private:

	// 队伍小队Id
	UInt32 m_TeamSquadId;
	// 地下城Id
	UInt32 m_DungeonId;
	// 据点id
	UInt32 m_FieldId;
	// 尾部是否通关
	UInt32 m_TailDungeonId;
	// 超时时间
	UInt64 m_TimeOut;
	// 比赛个人数据
	std::map<UInt64, DungeonRacePlayerInfo> m_RacePlayerInfoMap;
};

#endif
