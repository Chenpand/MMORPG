#ifndef _TCS_DUNGEON_MGR_H_
#define _TCS_DUNGEON_MGR_H_

#include <set>
#include <CLDefine.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>

class TCSDungeonRace;

/**
*@brief �ű����³ǹ���
*/
class TCSDungeonMgr : public Avalon::Singleton<TCSDungeonMgr>
{
public:
	TCSDungeonMgr();
	~TCSDungeonMgr();

public:

	/**
	*@brief ���³Ƿ�������
	*/
	void OnConnected(UInt32 id);

	/**
	*@brief ���³Ƿ���Ͽ�
	*/
	void OnDisconnected(UInt32 id);

	/**
	*@brief ���͵����³Ƿ���
	*/
	void SendToDungeonSvr(Avalon::Packet* packet);
	void SendToDungeonSvr(Avalon::Protocol& protocol);

	/**
	*@brief ����
	*/
	void AddRace(UInt64 raceId, UInt32 teamSquadId, UInt32 fieldId);
	void RemoveRace(UInt64 raceId);

	/**
	*@brief ����boss�׶�
	*/
	void HandlerBossPhase(UInt32 bossPhase, UInt64 raceSessionId);

	/**
	*@brief ��������
	*/
	void DungeonRaceEnd(UInt64 raceSessionId, UInt32 reason);

	/**
	*@brief �ݵ㱻����
	*/
	void DungeonRaceDestroy(UInt64 raceSessionId, UInt32 dungeonId);

	/**
	*@brief ���Ӹ������
	*/
	void AddRaceReviveCnt(UInt64 raceSessionId, UInt32 reviveCnt);

	/**
	*@brief ������������
	*/
	void ReconnectEndRace(UInt64 roldeId);

	/**
	*@brief ���ұ���
	*/
	TCSDungeonRace* FindRaceByTeamSquadId(UInt32 teamSquadId);

	/**
	*@brief ���ұ���
	*/
	TCSDungeonRace* FindRaceByRaceId(UInt64 raceId);

private:

	// ���³Ƿ���������
	Avalon::NetConnectionPtr m_DungeonSvrPtr;

	// ����(key->����id)
	std::map<UInt64, TCSDungeonRace*> m_DungeonRaceMap;

	// С�ӱ���(key->����С��)
	std::map<UInt32, TCSDungeonRace*> m_TeamSquadRaceMap;

	// boss�׶�(key->����id, val->�׶�)
	std::map<UInt64, std::set<UInt32>> m_RacePhaseMap;
};

#endif
