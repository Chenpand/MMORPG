#ifndef _CITY_MONSTER_H_
#define _CITY_MONSTER_H_

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLObject.h>
#include <CLCityMonsterDataEntry.h>
#include <CLObjectDefine.h>

class Player;
class CityMonster : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(CityMonster, Avalon::NullMutex);
public:
	CityMonster();
	~CityMonster();

public:
	// �¼�

	/**
	*@brief �����ɹ�
	*/
	virtual void OnCreated(bool notify = true);

	/**
	*@brief �����ɱ�������ʱͬһ�ֻᱻ���ö�Σ�
	*/
	virtual void OnKilled(ObjID_t raceId);

	/**
	*@brief ���ﱻɾ��
	*/
	virtual void OnRemoved(bool notify = true);

public:

	/**
	*@brief ת�����ַ����������ӡ��־
	*/
	std::string ToString();

	/**
	*@brief ���û�ȡӵ����ID
	*/
	void SetOwnerID(ObjID_t id) { m_Owner = id; }
	ObjID_t GetOwnerID() const { return m_Owner; }

	/**
	*@brief ���û�ȡ����ID
	*/
	void SetSceneID(UInt32 id) { m_SceneId = id; }
	UInt32 GetSceneID() const { return m_SceneId; }

	/**
	*@brief ��������
	*/
	void SetMonsterType(CityMonsterType type) { m_Type = type; }
	CityMonsterType GetMonsterType() const { return m_Type; }
	

	/**
	*@brief ���û�ȡ����ID
	*/
	void SetDataID(UInt32 id) { m_DataId = id; }
	UInt32 GetDataID() const { return m_DataId; }

	/**
	*@brief ���û�ȡλ��
	*/
	void SetPos(NpcPos pos) { m_Pos = pos; }
	NpcPos GetPos() const { return m_Pos; }

	/**
	*@brief ���ö�Ӧ���³�ID
	*/
	void SetDungeonID(UInt32 dungeonId) { m_DungeonId = dungeonId; }
	UInt32 GetDungeonID() const { return m_DungeonId; }

	/**
	*@brief �ܴ���
	*/
	void SetTotalTimes(UInt32 times) { m_TotalTimes = times; }
	UInt32 GetTotalTimes() const { return m_TotalTimes; }

	/**
	*@brief ʣ��ɴ����
	*/
	void SetRemainTimes(UInt32 times) { m_RemainTimes = times; }
	UInt32 GetRemainTimes() const { return m_RemainTimes; }
	void DecRemainTimes();

	/**
	*@brief �ܷ��
	*/
	UInt32 CanFight();

	/**
	*@brief ���ڴ�Ķ����б�
	*/
	void AddRacingRace(ObjID_t raceId);
	void DelRacingRace(ObjID_t raceId);
	UInt32 GetRacingRaceNum() { return m_RacingRaceList.size(); }

	/**
	*@brief npc��Ϣ
	*/
	SceneNpc GetInfo();

	/**
	*@��������ʱ����
	*/
	inline void SetReviveTimeSpace(UInt32 timeSpace) {	m_reviveTimeSpace = timeSpace;}
	inline UInt32 GetReviveTimeSpace() { return m_reviveTimeSpace; }

	inline void SetReviveTimeStamp(UInt32 timeStamp) { m_reviveTimeStamp = timeStamp; }
	inline UInt32 GetReviveTimeStamp() { return m_reviveTimeStamp; }

	//ս��id
	inline void SetBattleId(UInt32 battleId) { m_battleId = battleId; }
	inline UInt32 GetBattleId() { return m_battleId; }

	//ս����̬����id
	inline void SetBattleDyncSceneId(UInt32 battleDyncSceneId) { m_battleDyncSceneId = battleDyncSceneId; }
	inline UInt32 GetBattleDyncSceneId() { return m_battleDyncSceneId; }

	inline UInt8 GetInBattle() { return m_inBattle; }
private:
	// ӵ����(�����ӵ���߾�˵��������ĳ����ҵ�)
	ObjID_t		m_Owner;
	// ����ID
	UInt32		m_SceneId;
	// ��������
	CityMonsterType	m_Type;
	// ����ID
	UInt32		m_DataId;
	// λ��
	NpcPos		m_Pos;
	// ���³�ID
	UInt32		m_DungeonId;
	// ʣ��ɴ����
	UInt32		m_RemainTimes;
	// �ܴ���
	UInt32		m_TotalTimes;
	// �Ѿ�����ı����б�
	std::set<ObjID_t>	m_RaceIds;
	// ���ڴ��ս���б�
	std::set<ObjID_t>	m_RacingRaceList;
	// ����ս��id
	UInt32	m_battleId;
	// ����ս����̬����id
	UInt32	m_battleDyncSceneId;
	// �������ʱ��
	UInt32	m_reviveTimeSpace;
	// ����ʱ���
	UInt32	m_reviveTimeStamp;
	// �Ƿ���ս����
	UInt8	m_inBattle;
};

#endif