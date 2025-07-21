#ifndef _CS_SCORE_WAR_MGR_H_
#define _CS_SCORE_WAR_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <CLGameSessionProtocol.h>
#include "CLScoreWarDefine.h"
#include "GameZoneMgr.h"

class CSScoreWarSortListMgr
{
public:
	typedef std::list<ScoreWarSortListEntry> ScoreWarSortEntryList;

	CSScoreWarSortListMgr() {}
	~CSScoreWarSortListMgr() {}

	void Init();

	//更新排行榜
	void UpdateSortList(ScoreWarSortListEntry& entry);

	//下发排行榜
	void ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count);

	//获取排名
	UInt32 GetRanking(SortListType sortType, ObjID_t playerId);

public:
	void AddToList(ScoreWarSortListEntry& entry);
	ScoreWarSortEntryList& GetSortEntryList(SortListType sortType);

	void ClearSortList();

	void SendRankingReward();

private:
	void InsertEntry(ScoreWarSortListEntry entry);
	void DeleteEntry(ScoreWarSortEntryList::iterator iter);


private:
	ScoreWarSortEntryList m_ScoreWarList;
};


class CSScoreWarMgr : public Avalon::Singleton<CSScoreWarMgr>
{
public:
	CSScoreWarMgr() :m_Status(ScoreWarStatus::SWS_INVALID),m_IsByTime(true){}
	~CSScoreWarMgr(){}

	bool Init();

public:
	inline ScoreWarStatus GetScoreWarStatus() { return m_Status; };
	inline void SetScoreWarStatus(ScoreWarStatus status) { m_Status = status; }

	inline void SetIsByTime(bool isByTime) { m_IsByTime = isByTime; }

	void SetScoreWarTimeInfo(ScoreWarTimeInfo info);

	bool IsJoinRoom() { return m_Status == ScoreWarStatus::SWS_BATTLE || m_Status == ScoreWarStatus::SWS_PREPARE; }

public:

	void ChangeTargetStatus(ScoreWarStatus targetStatus);

	void RemoveWaitResultConnId(UInt32 id);

	void ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count);

	void UpdateSortList(ScoreWarSortListEntry& entry);

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
	void _SetScoreWarStatus(ScoreWarStatus status);

	void _InitScoreWarStatus();

	void _ChangeToTargetStatus();

	void _SaveScoreWarStatus();

	void _ClearScoreWarSortList();

private:
	//积分赛状态
	ScoreWarStatus			m_Status;

	//目标状态
	ScoreWarStatus			m_TargetStatus;

	//排行榜
	CSScoreWarSortListMgr	m_SortListMgr;

	//积分赛时间
	ScoreWarTimeInfo		m_ScoreWarTimeInfo;

	//等待回包的连接
	std::vector<UInt32>		m_WaitResultConnIds;

	//最后Tick时间
	UInt32					m_CheckWaitTime;

	//是否根据自然时间
	bool					m_IsByTime;
};


#endif  //_CS_SCORE_WAR_MGR_H_