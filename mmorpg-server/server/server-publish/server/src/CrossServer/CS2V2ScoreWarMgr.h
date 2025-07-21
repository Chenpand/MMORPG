#ifndef _CS_2V2_SCORE_WAR_MGR_H_
#define _CS_2V2_SCORE_WAR_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <CLGameSessionProtocol.h>
#include "CL2V2ScoreWarDefine.h"
#include "GameZoneMgr.h"

class CS2V2ScoreWarSortListMgr
{
public:
	typedef std::list<ScoreWar2V2SortListEntry> ScoreWarSortEntryList;

	CS2V2ScoreWarSortListMgr() {}
	~CS2V2ScoreWarSortListMgr() {}

	void Init();

	//更新排行榜
	void UpdateSortList(ScoreWar2V2SortListEntry& entry);

	//下发排行榜
	void ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count);

	//获取排名
	UInt32 GetRanking(SortListType sortType, ObjID_t playerId);

public:
	void AddToList(ScoreWar2V2SortListEntry& entry);
	ScoreWarSortEntryList& GetSortEntryList(SortListType sortType);

	void ClearSortList();

	void SendRankingReward();

private:
	void InsertEntry(ScoreWar2V2SortListEntry entry);
	void DeleteEntry(ScoreWarSortEntryList::iterator iter);


private:
	ScoreWarSortEntryList m_ScoreWarList;
};


class CS2V2ScoreWarMgr : public Avalon::Singleton<CS2V2ScoreWarMgr>
{
public:
	CS2V2ScoreWarMgr() :m_Status(ScoreWar2V2Status::SWS_2V2_INVALID),m_IsByTime(true){}
	~CS2V2ScoreWarMgr(){}

	bool Init();

public:
	inline ScoreWar2V2Status GetScoreWarStatus() { return m_Status; };
	inline void SetScoreWarStatus(ScoreWar2V2Status status) { m_Status = status; }

	inline void SetIsByTime(bool isByTime) { m_IsByTime = isByTime; }

	void SetScoreWarTimeInfo(ScoreWar2V2TimeInfo info);

	bool IsJoinRoom() { return m_Status == ScoreWar2V2Status::SWS_2V2_BATTLE || m_Status == ScoreWar2V2Status::SWS_2V2_PREPARE; }

public:

	void ChangeTargetStatus(ScoreWar2V2Status targetStatus);

	void RemoveWaitResultConnId(UInt32 id);

	void ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count);

	void UpdateSortList(ScoreWar2V2SortListEntry& entry);

public:
	void OnTick(Avalon::Time now);

	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	void OnScoreWarPrepare();

	void OnScoreWarBattle();

	void OnScoreWarWaitEnd();

	void OnScoreWarEnd();

	void SyncScoreWarStatus(UInt32 zoneId = 0);

private:
	void _SetScoreWarStatus(ScoreWar2V2Status status);

	void _InitScoreWarStatus();

	void _ChangeToTargetStatus();

	void _SaveScoreWarStatus();

	void _ClearScoreWarSortList();

private:
	//积分赛状态
	ScoreWar2V2Status			m_Status;

	//目标状态
	ScoreWar2V2Status			m_TargetStatus;

	//排行榜
	CS2V2ScoreWarSortListMgr	m_SortListMgr;

	//积分赛时间
	ScoreWar2V2TimeInfo		m_ScoreWarTimeInfo;

	//等待回包的连接
	std::vector<UInt32>		m_WaitResultConnIds;

	//最后Tick时间
	UInt32					m_CheckWaitTime;

	//是否根据自然时间
	bool					m_IsByTime;
};


#endif  //_CS_SCORE_WAR_MGR_H_