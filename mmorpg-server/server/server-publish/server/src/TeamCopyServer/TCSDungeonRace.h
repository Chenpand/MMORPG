#ifndef _TCS_DUNGEON_RACE_H_
#define _TCS_DUNGEON_RACE_H_

#include <AvalonObjectPool.h>
#include <CLDefine.h>

/**
*@brief �ű����³Ǳ���
*/
class TCSDungeonRace
{
	AVALON_DEFINE_OBJECTPOOL(TCSDungeonRace, Avalon::NullMutex)

public:
	TCSDungeonRace();
	~TCSDungeonRace();

public:
	/**
	*@brief ����Id
	*/
	inline void SetRaceId(UInt64 id) { m_RaceId = id; }
	inline UInt64 GetRaceId() { return m_RaceId; }

	/**
	*@brief ����С��Id
	*/
	inline void SetTeamSquadId(UInt32 id) { m_TeamSquadId = id; }
	inline UInt32 GetTeamSquadId() { return m_TeamSquadId; }

	/**
	*@brief �ݵ�Id
	*/
	inline void SetFieldId(UInt32 id) { m_FieldId = id; }
	inline UInt32 GetFieldId() { return m_FieldId; }


	/**
	*@brief ��ʼʱ��
	*/
	inline void SetCreateTime(UInt64 time) { m_CreateTime = time; }
	inline UInt64 GetCreateTime() { return m_CreateTime; }
	
private:

	// ����Id
	UInt64 m_RaceId;
	// ����С��Id
	UInt32 m_TeamSquadId;
	// �ݵ�id
	UInt32 m_FieldId;
	// ��ʼʱ��
	UInt64 m_CreateTime;
};

#endif
