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

	//�������а�
	void UpdateSortList(ScoreWarSortListEntry& entry);

	//�·����а�
	void ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count);

	//��ȡ����
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
	//������״̬
	ScoreWarStatus			m_Status;

	//Ŀ��״̬
	ScoreWarStatus			m_TargetStatus;

	//���а�
	CSScoreWarSortListMgr	m_SortListMgr;

	//������ʱ��
	ScoreWarTimeInfo		m_ScoreWarTimeInfo;

	//�ȴ��ذ�������
	std::vector<UInt32>		m_WaitResultConnIds;

	//���Tickʱ��
	UInt32					m_CheckWaitTime;

	//�Ƿ������Ȼʱ��
	bool					m_IsByTime;
};


#endif  //_CS_SCORE_WAR_MGR_H_