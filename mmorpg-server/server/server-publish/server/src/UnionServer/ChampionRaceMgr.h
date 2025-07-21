#ifndef __CHAMPION_BATTLE_RECORD_MGR_H__
#define __CHAMPION_BATTLE_RECORD_MGR_H__

#include <AvalonSingleton.h>
#include <CLChampionDefine.h>
#include <AvalonSimpleTimer.h>
#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class ChampionPlayer;


class ChampionRace : public CLSyncObject
{
public:
	ChampionRace();
	void SaveToDB();
	void InsterToDB();
	void LoadDBData(CLRecordCallback* callback);
	inline void SetGroupID(const UInt32 id) { m_Data.groupID = id; }
	inline const UInt32 GetGroupID() const { return m_Data.groupID; }
	inline void SetOrder(const UInt32 id) { m_Data.order = id; }
	inline const UInt32 GetOrder() const { return m_Data.order; }
	inline void SetRaceID(const UInt64 id) { m_Data.raceID = id; }
	inline const UInt64 GetRaceID() const { return m_Data.raceID; }
	inline void SetChampionStatus(const UInt8 id) { m_Data.championStatus = id; }
	inline const UInt8 GetChampionStatus() const { return m_Data.championStatus; }
	inline void SetRoleID1(const UInt64 id) { m_Data.roleID1 = id; }
	inline const UInt64 GetRoleID1() const { return m_Data.roleID1; }
	inline void SetRoleID2(const UInt64 id) { m_Data.roleID2 = id; }
	inline const UInt64 GetRoleID2() const { return m_Data.roleID2; }
	inline void SetIsEnd(const bool is) { m_Data.isEnd = is ? 1 : 0; }
	inline const bool IsEnd() const { return m_Data.isEnd > 0 ? true : false; }
	inline void SetWinnerID(const UInt64 id) { m_Data.winnerID = id; }
	inline const UInt64 GetWinnerID() const { return m_Data.winnerID; }
	inline void SetResult(const UInt32 result) { m_Data.result = result; }
	inline const UInt32 GetResult() const { return m_Data.result; }
	inline void SetPlayer(ChampionPlayer* a, ChampionPlayer* b) { m_PlayerA = a; m_PlayerB = b; }
	inline ChampionPlayer* GetPlayerA() { return m_PlayerA; }
	inline ChampionPlayer* GetPlayerB() { return m_PlayerB; }
public:


private:
	ChampionRaceData m_Data;
private:
	ChampionPlayer* m_PlayerA = nullptr;
	ChampionPlayer* m_PlayerB = nullptr;
};


class ChampionRaceMgr : public Avalon::Singleton<ChampionRaceMgr>
{
public:
	const bool Init();
	void Clear();
	void OnTick(const Avalon::Time& now);
	void OnLoadData(CLRecordCallback* cb);
	ChampionRace* AddRace(const UInt32 groupID, const UInt64 raceID);
	ChampionRace* FindRace(const UInt32 groupID, const UInt32 order);
	std::map<UInt32, ChampionRace*> FindRaces(const UInt32 groupID);
	void OnRaceBegin(ChampionPlayer* aPlayer, ChampionPlayer* bPlayer, const ChampionStatus status, const UInt32 groupID, const UInt32 order);
	void OnRaceCreateRet(const UInt32 groupID, UInt32 order, UInt64 raceID);
	void OnRaceEnd(UInt64 raceID, UInt64 winner, UInt64 loser, UInt32 result);
	void OnRaceEnd(const UInt32 groupID, const UInt32 order, UInt64 winner, UInt64 loser, UInt32 result);
	ChampionRace* GetRace(UInt64 raceID);
	void SyncAllRaceInfo();
	UInt32 GetRaceNum(UInt32 groupID);
private:
	//组id->比赛顺序order->比赛
	std::map<UInt32, std::map<UInt32, ChampionRace*>> m_BattleRecordMap;
	//比赛id->比赛
	std::map<UInt64, ChampionRace*> m_RaceMap;
	// 保存数据库定时器
	Avalon::SimpleTimer	m_SaveTimer;
};


#endif
