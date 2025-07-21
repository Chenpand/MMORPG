#pragma once
#include <CLDefine.h>
#include <CLGameSessionDefine.h>
#include <AvalonSocket.h>
#include <CLMatchDefine.h>
#include <AvalonProtocol.h>
#include <CLObjectDefine.h>
#include <CLDungeonDefine.h>
#include <CLErrorCode.h>

enum BaseRaceStatus
{
	BRCS_INIT,           // 初始状态
	BRCS_CREATING,		 // 创建中
	BRCS_RACING,         // 比赛中
	BRCS_RACE_END,       // 结算中
	BRCS_WAIT_CLOSE,     // 等待关闭
};

class BSPlayer;
class BSBaseBattle;

class BSBaseRace
{
public:
	BSBaseRace(UInt32 battleId, UInt8 raceType);
	BSBaseRace(UInt8 raceType);
	virtual ~BSBaseRace();

	inline void SetRaceType(UInt8 type) { m_raceType = type; }
	inline UInt8 GetRaceType() { return m_raceType; }

	inline void SetRaceID(UInt64 id) { m_raceID = id; }
	inline UInt64 GetRaceID() const { return m_raceID; }

	inline void SetBattleID(UInt32 id) { m_battleID = id; }
	inline UInt32 GetBattleID() const { return m_battleID; }

	inline void SetDungeonID(UInt32 id) { m_dungeonID = id; }
	inline UInt32 GetDungeonID() const { return m_dungeonID; }

	inline void SetRaceAddr(const Avalon::SockAddr& addr) { m_raceAddr = addr; }
	inline const Avalon::SockAddr& GetRaceAddr() const { return m_raceAddr; }

	inline void SetStatus(BaseRaceStatus status) { m_status = status; }
	inline BaseRaceStatus GetStatus() const { return m_status; }

	inline void SetDestoryTime(Avalon::Time closeTime) { m_destoryTime = closeTime; }
	bool IsTimeToDestory(const Avalon::Time& now);

	/**
	*@brief 设置获取realy服务器id
	*/
	inline void SetRelayServerID(UInt32 id) { m_RelayServerID = id; }
	inline UInt32 GetRelayServerID() const { return m_RelayServerID; }

	bool SendToRelay(Avalon::Protocol& protocol);

	void SendToBScene(Avalon::Protocol& protocol);

	/**
	*@brief 玩家战斗信息
	*/
	void SetPlayerInfos(std::vector<RacePlayerInfo>& playerInfos) { m_playerInfoes = playerInfos; }
	std::vector<RacePlayerInfo>& GetRacePlayerInfo() { return m_playerInfoes; }

	virtual UInt32 StartRace() { return ErrorCode::SUCCESS; };
	virtual void OnStartRaceFail() {};
	virtual void OnRelayStartRaceRes(UInt32 result, UInt64 gamesessionID, UInt32 abality, const Avalon::SockAddr& address) {}
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo) { return true; }
	virtual BSBaseBattle* GetBattle() { return NULL; }
	virtual void OnTick(const Avalon::Time& now) {}

	virtual void OnStartDungeonRes(UInt32 result, UInt64 gamesessionID) {}
	virtual void OnDungeonEnd(UInt8 result, DungeonRaceEndInfo& endInfo) {}

	virtual void OnPlayerLeave(UInt64 roleId, bool isDisconnect);

	virtual void OnPlayerJoin(BSPlayer* player);

	virtual void OnAllPlayerLeave();
protected:
	UInt8			m_raceType;
	UInt64			m_raceID;
	UInt32			m_battleID;
	UInt32			m_dungeonID;

	Avalon::SockAddr m_raceAddr;
	//relay服务器ID
	UInt32 m_RelayServerID;
	// 玩家战斗信息
	std::vector<RacePlayerInfo>	m_playerInfoes;
	// 状态
	BaseRaceStatus       m_status;
	// 比赛被摧毁时间
	Avalon::Time            m_destoryTime;
	// 开始匹配时间
	UInt32		m_start_match_time;
	// 玩家集合
	std::set<UInt64> m_players;
};

