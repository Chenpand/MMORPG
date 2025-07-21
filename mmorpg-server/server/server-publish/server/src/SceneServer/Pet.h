#ifndef _PET_H_
#define _PET_H_

#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLErrorCode.h>
#include <CLPetDataEntry.h>

class Player;

class Pet : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Pet, Avalon::NullMutex)
public:
	Pet(Player* player, UInt32 _dataId, ObjID_t _id, UInt16 _level, UInt32 _exp, UInt16 _hunger, UInt8 _skillIndex, UInt8 _goldFeedCount, UInt8 _pointFeedCount, UInt8 _selectSkillCount);
	Pet(Player* player, UInt32 _dataId, UInt16 _hungry);
	~Pet();

public:
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief ��ʼ������
	*/
	UInt32 Init();

	/**
	*@brief ��ȡ������
	*/
	PetDataEntry* GetDataEntry() { return m_pDataEntry; }

	/**
	*@brief ��ȡ��ID
	*/
	UInt32 GetDataId() const { return m_DataId; };

	/**
	*@brief ��ȡ����Ʒ��
	*/
	PetQuality GetQuality() const { return m_pDataEntry->quality; }

	/**
	*@brief ��ȡ��������
	*/
	PetType GetType() const { return m_pDataEntry->type; }

	/**
	*@brief ��ȡ���۽���
	*/
	std::vector<ItemReward> GetSellReward() const { return m_pLevelDataEntry->sellRewards; }

	/**
	*@brief ��ȡ���ﵰID
	*/
	UInt32 GetItemId() const { return m_pDataEntry->itemId; }

	/**
	*@brief ��ȡ����ȼ�
	*/
	UInt16 GetLevel() const { return m_Level; }

	/**
	*@brief ��ȡ���ﾭ��
	*/
	UInt32 GetExp() const { return m_Exp; }

	/**
	*@brief ��ȡ���Ｂ��ֵ
	*/
	UInt16 GetHunger() const { return m_Hunger; }

	/**
	*@brief ��ȡ��������
	*/
	UInt8 GetSkillIndex() const { return m_SkillIndex; }

	/**
	*@brief ��ȡ���ι������
	*/
	UInt8 GetGoldFeedCount() const { return m_GoldFeedCount; }

	/**
	*@brief ������ѡ����
	*/
	UInt8 GetSelectSkillCount() const { return m_SelectSkillCount; }

	/**
	*@brief ���ӽ��ι������
	*/
	void IncGoldFeedCount() { m_GoldFeedCount = m_GoldFeedCount + 1; }

	/**
	*@brief ��ȡ���ι������
	*/
	UInt8 GetPointFeedCount() const { return m_PointFeedCount; }

	/**
	*@brief ���ӵ��ι������
	*/
	void IncPointFeedCount() { m_PointFeedCount = m_PointFeedCount + 1; }

	/**
	*@brief ��ȡ������Ϣ
	*/
	void GetPetInfo(PetInfo& info);

	/**
	*@brief �Ƿ�����
	*/
	bool IsFullLevel(){ return m_pLevelDataEntry->upLevelExp == 0; }

	/**
	*@brief �����ɲ����ľ���
	*/
	UInt32 GetDevourExp();

public:
	/**
	*@brief ����ƣ���¼�
	*/
	void OnRemoveFatigue(UInt16 fatigue);

public:
	/**
	*@brief ιʳ����
	*/
	UInt32 Feed();

	/**
	*@brief ���ó��＼������
	*/
	UInt32 SetSkillIndex(UInt8 index);

	/**
	*@brief ���Ӿ���
	*/
	void IncExp(UInt32 exp);

	/**
	*@brief ���ļ�����
	*/
	void ConsumeHunger(UInt16 value);

	/**
	*@brief ���Ӽ�����
	*/
	void AddHunger(UInt16 value);

	/**
	*@brief �Ƿ�����ʳ��
	*/
	bool IsFullHunger();

	/**
	*@brief ����ιʳ����
	*/
	void ResetFeedCount();

private:
	/**
	*@brief ����
	*/
	void UpLevel();

public:// ����

	/**
	*@brief ���û�ȡ����
	*/
	void SetPetScore(UInt32 score) { m_PetScore = score; }
	UInt32 GetPetScore() { return m_PetScore; }

	/**
	*@brief ��������
	*/
	void CalcPetScore();

public:

	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
	CL_SYNC_PROPERTY(m_Level, POA_LEVEL, SST_SELF)
	CL_SYNC_PROPERTY(m_Exp, POA_EXP, SST_SELF)
	CL_SYNC_PROPERTY(m_Hunger, POA_HUNGER, SST_SELF)
	CL_SYNC_PROPERTY(m_SkillIndex, POA_SKILL_INDEX, SST_SELF)
	CL_SYNC_PROPERTY(m_GoldFeedCount, POA_GOLD_FEED_COUNT, SST_SELF)
	CL_SYNC_PROPERTY(m_PointFeedCount, POA_POINT_FEED_COUNT, SST_SELF)
	CL_SYNC_PROPERTY(m_SelectSkillCount, POA_SELECT_SKILL_COUNT, SST_SELF)
	CL_SYNC_PROPERTY(m_PetScore, POA_PET_SCORE, SST_SELF)
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("dataid", m_DataId)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("exp", m_Exp)
	CL_DBPROPERTY("hunger", m_Hunger)
	CL_DBPROPERTY("skill_index", m_SkillIndex)
	CL_DBPROPERTY("gold_feed_count", m_GoldFeedCount)
	CL_DBPROPERTY("point_feed_count", m_PointFeedCount)
	CL_DBPROPERTY("select_skill_count", m_SelectSkillCount)
	CL_DBPROPERTY_END()

	void InsertPet();

	void DeletePet();

	void WatchData(Avalon::NetOutputStream& stream);

	void SyncToDB(bool bFlush);

	void SyncProperty(bool bDirty = true);

private:
	//ӵ����
	Player*	m_pOwner;

	//����ID
	ObjUInt32 m_DataId;

	//����ȼ�
	ObjUInt16 m_Level;

	//���ﾭ��
	ObjUInt32 m_Exp;

	//���＼������
	ObjUInt8 m_SkillIndex;

	//���Ｂ����
	ObjUInt16 m_Hunger;

	//������ιʳ����
	ObjUInt8 m_GoldFeedCount;

	//������ιʳ����
	ObjUInt8 m_PointFeedCount;

	//������ѡ����
	ObjUInt8 m_SelectSkillCount;

	//������
	PetDataEntry* m_pDataEntry;

	//����������
	PetLevelDataEntry* m_pLevelDataEntry;

	//����
	ObjUInt32	m_PetScore;
};

#endif //_PET_H_