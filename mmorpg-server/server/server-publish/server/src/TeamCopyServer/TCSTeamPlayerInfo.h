#ifndef _TCS_TEAM_PLAYER_INFO_H_
#define _TCS_TEAM_PLAYER_INFO_H_

#include <CLDefine.h>
#include <CLMatchDefine.h>

namespace CLProtocol{
	class DungeonFighter;
};

/**
*@brief �����³Ǹ�����Ϣ��ѯ
*/
class TCSTeamPlayerInfo
{
public:
	TCSTeamPlayerInfo();
	~TCSTeamPlayerInfo();

	/**
	*@brief ����С��Id
	*/
	inline void SetTeamSquadId(UInt32 id) { m_TeamSquadId = id; }
	inline UInt32 GetTeamSquadId() { return m_TeamSquadId; }

	/**
	*@brief ���³�Id
	*/
	inline void SetDungeonId(UInt32 id) { m_DungeonId = id; }
	inline UInt32 GetDungeonId() { return m_DungeonId; }

	/**
	*@brief �ݵ�Id
	*/
	inline void SetFieldId(UInt32 id) { m_FieldId = id; }
	inline UInt32 GetFieldId() { return m_FieldId; }

	/**
	*@brief β���Ƿ�ͨ��
	*/
	inline void SetTailDungeonId(UInt32 id) { m_TailDungeonId = id; }
	inline UInt32 GetTailDungeonId() { return m_TailDungeonId; }

	/**
	*@brief ��ʱʱ���
	*/
	inline void SetTimeOut(UInt64 stamp) { m_TimeOut = stamp; }
	inline UInt64 GetTimeOut() { return m_TimeOut; }

	/**
	*@brief �������
	*/
	void AddTeamPlayer(UInt64 playerId);

	/**
	*@brief ����Ƿ��ڱ�����
	*/
	bool IsHasPlayer(UInt64 playerId);

	/**
	*@brief ����playerInfo
	*/
	void SetDungeonRacePlayerInfo(UInt64 playerId, DungeonRacePlayerInfo& info);

	/**
	*@brief �Ƿ���ȫ��playerInfo
	*/
	bool IsAllHasInfo();

	/**
	*@brief ��ȡplayerInfos
	*/
	UInt32 GetPlayerInfos(std::vector<CLProtocol::DungeonFighter>& fighters, std::vector<DungeonRacePlayerInfo>& playerInfos);

private:

	// ����С��Id
	UInt32 m_TeamSquadId;
	// ���³�Id
	UInt32 m_DungeonId;
	// �ݵ�id
	UInt32 m_FieldId;
	// β���Ƿ�ͨ��
	UInt32 m_TailDungeonId;
	// ��ʱʱ��
	UInt64 m_TimeOut;
	// ������������
	std::map<UInt64, DungeonRacePlayerInfo> m_RacePlayerInfoMap;
};

#endif
