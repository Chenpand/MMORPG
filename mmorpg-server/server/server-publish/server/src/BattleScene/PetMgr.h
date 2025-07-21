#ifndef _PET_MGR_H_
#define _PET_MGR_H_

#include <CLPetDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include "Pet.h"

class BattlePetMap : public CLComplexProperty
{
public:
	BattlePetMap();
	~BattlePetMap();

public:
	UInt32 SetBattlePet(PetType petType, UInt64 petId);

	std::vector<UInt64> GetBattlePetList();

	UInt64 GetBattlePetId(PetType petType);

public:
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

public:
	std::map<PetType, UInt64> m_BattlePetMap;

};

class PetMgr
{
public:
	typedef std::map<ObjID_t, Pet*> PetMap;

public:
	PetMgr();
	~PetMgr();

public:
	void SetOwner(Player* player) { m_pOwner = player; }
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief ����ID����һ������
	*/
	Pet* FindPet(ObjID_t id);

	/**
	*@brief ��ȡ�������
	*/
	UInt32 GetPetSize() { return m_PetMap.size(); }

	/**
	*@brief ��ȡ��ս����
	*/
	BattlePetMap& GetBattlePetMap() { return m_BattlePetMap; };

	/**
	*@brief ��ȡ����ĳ���ID
	*/
	ObjUInt64& GetFollowPetId() { return m_FollowPetId; }

	/**
	*@brief ��ȡ����ĳ���
	*/
	//ObjScenePet& GetFollowPet(){ return m_ScenePet; }
	ObjUInt32& GetFollowPetDataId() { return m_FollowPetDataId; }

	/**
	*@brief ��ȡս��������Ϣ
	*/
	void GetRacePets(std::vector<RacePet>& pets);

	/**
	*@brief ��ȡ���������Ϣ
	*/
	void GetPetBaseInfos(std::vector<PetBaseInfo>& pets);

public:
	/**
	*@brief ���ӳ���
	*/
	UInt32 AddPet(UInt32 itemId);

	/**
	*@brief ���ó����λ
	*/
	UInt32 SetPetSlot(PetType petType, UInt64 petId);

	/**
	*@brief ��ȡ��������
	*/
	UInt32 GetPetSize(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief ��ȡ�����λ
	*/
	std::vector<UInt64> GetBattlePets() { return m_BattlePetMap.GetBattlePetList(); }

	/**
	*@brief ιʳ����
	*/
	UInt32 FeedPet(ObjID_t id, PetFeedType petFeedType, UInt8& isCritical, UInt32& value);

	/**
	*@brief ���ɳ���
	*/
	UInt32 DevourPet(ObjID_t id, std::vector<ObjID_t> petIds, UInt32& exp);

	/**
	*@brief ���۳���
	*/
	UInt32 SellPet(ObjID_t id);

	/**
	*@brief ������＼��
	*/
	UInt32 ChangePetSkill(ObjID_t id, UInt8 skillIndex);

	/**
	*@brief ���ø���ĳ���
	*/
	UInt32 SetPetFollow(ObjID_t id);

public:	//�¼�
	/**
	*@brief �����¼�
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief �����¼�
	*/
	void OnOffline();

	/**
	*@brief �����߼��ڶ���(6��)
	*/
	void OnLogicDayChanged();

	/**
	*@brief ��ѯ�ص�
	*/
	void OnSelectDataRet(CLRecordCallback* callback);

	/**
	*@brief ƣ�������¼�
	*/
	void OnRemoveFatigue(UInt16 fatigue);

	/**
	*@brief �л�����
	*/
	void OnEnterScene(UInt32 type);

	/**
	*@brief ������³ǳ���
	*/
	void OnEnterDungeonRace();

	/**
	*@brief Tick
	*/
	void OnTick(Avalon::Time now);

	/**
	*@brief ��ս������Ϣ
	*/
	void OnBattlePetRest();

	/**
	*@brief ��ս����ս��
	*/
	void OnBattlePetBattle();

	/**
	*@brief ������Ϣ
	*/
	void OnPetRest();

public:
	/**
	*@brief �۳����Ｂ��ֵ
	*/
	void RemovePetHunger(UInt16 hunger);

	/**
	*@brief ���ӳ��Ｂ��ֵ
	*/
	void AddPetHunger(UInt16 hunger);

	/**
	*@brief �������
	*/
	void Clear();

private:

	/**
	*@brief ���س���
	*/
	UInt32 LoadPet(Pet* pet);

	/**
	*@brief ɾ������
	*/
	UInt32 DeletePet(ObjID_t id);

	/**
	*@brief ��ȡ����б�
	*/
	std::vector<PetInfo> GetPetList();

	/**
	*@brief �·�����б�
	*/
	void SyncPetList();

	/**
	*@brief �·�ɾ������
	*/
	void SyncDeletePet(Pet* pet);

	/**
	*@brief ͬ���������
	*/
	void SyncScenePet(Pet* pet);

	/**
	*@brief �Ƿ������Ϣ����
	*/
	bool IsPetRestScene(UInt32 type);

	/**
	*@brief ����ιʳ����
	*/
	void ResetFeedCount();

	/**
	*@brief ��ʼ����ս����
	*/
	void InitBattlePet();

	/**
	*@brief �жϳ����Ƿ��ս
	*/
	bool IsBattlePet(UInt64 petId);

	/**
	*@brief �����½��־
	*/
	void SendPetLoginUdpLog();

private:
	//ӵ����
	Player*	m_pOwner;

	//����MAP
	PetMap m_PetMap;

	//��ս����
	BattlePetMap m_BattlePetMap;

	//��������
	ObjUInt64 m_FollowPetId;

	//����ĳ���
	//ObjScenePet m_ScenePet;
	ObjUInt32 m_FollowPetDataId;

	//������Ϣʱ��
	UInt32 m_BattlePetRestSec;

	//����ս��ʱ��
	UInt32 m_BattlePetBattleSec;

	//�ǳ�ս������Ϣ��ʼʱ��
	UInt32 m_PetStartTime;

	//������Ϣ��ʼʱ��
	UInt32 m_BattlePetRestStartTime;

	//����ս����ʼʱ��
	UInt32 m_BattlePetBattleStartTime;

	//������Ϣ���ʱ��
	UInt32 m_BattlePetRestIntervalSec;

	//����ս�����ʱ��
	UInt32 m_BattlePetBattleIntervalSec;

	//��Ϣ���ʱ��
	UInt32 m_PetIntervalSec;

};

#endif //_PET_MGR_H_