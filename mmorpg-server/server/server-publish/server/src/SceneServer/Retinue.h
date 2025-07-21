#ifndef _CL_RETINUE_H_
#define _CL_RETINUE_H_

#include <CLRetinueDefine.h>
#include <CLRetinueDataEntry.h>
#include <CLErrorCode.h>
#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class Player;

class ChangeSkillData
{
public:
	ChangeSkillData() : m_pDataEntry(NULL) { }
	~ChangeSkillData() { }

	AVALON_DEFINE_NETSERIALIZATION()
	{
	}

public:
	UInt32 Init(RetinueSkillDataEntry* dataEntry, UInt32 minusFixNum = 0, UInt32 missNum = 0);

	bool IsCanUsed(UInt8 starLevel, UInt32 lockCount, UInt32 haveUsed);

	RetinueGroupType GetGroupType() { return m_pDataEntry->groupType; }

	UInt32 GetGroupId() { return m_pDataEntry->groupId; }

	UInt32 GetDataId() { return m_pDataEntry->dataId; }

	UInt32 GetEntryId() { return m_pDataEntry->id; }

	FakeWeightBase* GetWeight() { return &m_Weight; }

private:
	//����
	RetinueSkillDataEntry* m_pDataEntry;

	//����
	FakeWeightBase m_Weight;
};

class ChangeSkillFakeWeights : public CLComplexProperty
{
public:
	ChangeSkillFakeWeights();
	~ChangeSkillFakeWeights();

	ChangeSkillData* GetChangeSkillData(UInt32 id);

	ChangeSkillData* GetChangeSkillData(RetinueSkillDataEntry* data);

	void ReadChangeSkillData(UInt32 id, UInt32 minusFixNum, UInt32 missNum);

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_ChangeSkillDatas;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	std::map<UInt32, ChangeSkillData> m_ChangeSkillDatas;
};

class RetinueSkillList : public CLComplexProperty
{
public:
	RetinueSkillList();
	~RetinueSkillList();

public:
	std::vector<RetinueSkill> GetSkills() const { return m_Skills; }

	UInt32 GetSkillCount(UInt32 id, UInt8 level) const ;

	void AddSkill(RetinueSkill skill);

	RetinueSkill GetRetinueSkill(UInt32 skillId);

	void ChangeSkill(std::vector<UInt32> lockSkillIds, std::vector<RetinueSkill>& newSkills, std::vector<RetinueSkill>& oldSkills, std::vector<RetinueSkill>& lockSkills);

	void GetSkillIds(std::vector<UInt32>& skillIds);

	void GetBuffIds(std::vector<UInt32>& buffIds);

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Skills;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	std::vector<RetinueSkill> m_Skills;

};

class Retinue : public CLSyncObject
{
public:
	Retinue(Player* player, UInt32 _dataId, ObjID_t _id, UInt8 _level, UInt8 _starExp, std::string _skillStr);
	Retinue(Player* player ,UInt32 _dataId);
	~Retinue();

public:
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief ��ʼ�����
	*/
	UInt32 Init();

public: //�ӿ�

	/**
	*@brief ��ʼ����Ӽ���
	*/
	UInt32 InitSkill();

	/**
	*@brief ϴ����Ӽ���
	*/
	UInt32 ChangeSkill(std::vector<UInt32> skillIds);

	/**
	*@brief ������Ӽ���
	*/
	UInt32 TestChangeSkill(std::vector<UInt32> skillIds);

	/**
	*@brief �������ľ���
	*/
	UInt32 UnlockConsumeItem();

	/**
	*@brief �Ƿ��ܹ�����
	*/
	UInt32 IsFollow();

	/**
	*@brief ��ȡ��ID
	*/
	UInt32 GetDataId() const { return m_DataId; }

	/**
	*@brief ��ȡ���Ʒ��
	*/
	RetinueQuality GetQuality() const { return m_pDataEntry->quality; }

	/**
	*@brief ��ȡ֧Ԯ����
	*/
	std::vector<RetinueSkill> GetSkills() const { return m_Skills.GetSkills(); }

	/**
	*@brief ��ȡ��ӵȼ�
	*/
	UInt8 GetLevel() const { return m_Level; }

	/**
	*@brief ��ȡ���ǵȼ�
	*/
	UInt8 GetStarLevel() const { return m_StarLevel; }

	/**
	*@brief ��ȡ�����Ϣ
	*/
	void GetRetinueInfo(RetinueInfo& info);

	/**
	*@brief ��ȡ��ӵ�buffId
	*/
	void GetBuffIds(std::vector<UInt32>& buffIds);

	/**
	*@brief ��ȡ֧Ԯ���ܸ���
	*/
	UInt32 GetSkillCount(UInt16 id, UInt8 level) const { return m_Skills.GetSkillCount(id, level); }

	/**
	*@brief ���֧Ԯ������
	*/
	UInt32 GetMaxSkillCount() const { return m_StarLevel; }

	/**
	*@brief ����
	*/
	UInt32 UpLevel();

	/**
	*@brief ����
	*/
	UInt32 UpStar();

public:
	/**
	*@brief ��ȡ��Ӽ�����־�ַ���
	*/
	std::string GetUdpLogString(std::vector<RetinueSkill> skills);

private:


	/**
	*@brief ������ܲ�����
	*/
	UInt32 RandSkills(std::vector<UInt32> skillIds);

	/**
	*@brief ������ܶ���
	*/
	UInt32 RandSkillDatas(std::vector<ChangeSkillData*>& result, UInt32 groupId, std::vector<UInt32>& skillIds, UInt32 randCount = 0);

	/**
	*@brief ��ȡȨ�ض���
	*/
	UInt32 GetFakeWeight(std::vector<FakeWeightBase*>& fakeWeights, UInt32 groupId, std::vector<UInt32>& skillIds, std::vector<ChangeSkillData*>& result);

	/**
	*@brief ����Ƿ��ܹ�ϴ�������м���
	*/
	UInt32 CheckTestChangeSkill(std::vector<UInt32>& skillIds, UInt32 groupId);

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("dataid", m_DataId)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("starlevel", m_StarLevel)
	CL_DBPROPERTY("skills", m_Skills)
	CL_DBPROPERTY("weights", m_ChangeSkillFakeWeights)
	CL_DBPROPERTY_END()

	void InsertRetinue();

	void PackWatchData(Avalon::NetOutputStream& stream);

	void SyncToDB(bool bFlush);

private:

	//ӵ����
	Player*	m_pOwner;

	//����ID
	ObjUInt32 m_DataId;
	
	//��ӵȼ�
	ObjUInt8 m_Level;

	//���ǵȼ�
	ObjUInt8 m_StarLevel;

	//֧Ԯ����
	RetinueSkillList m_Skills;

	//ϴ������
	ChangeSkillFakeWeights m_ChangeSkillFakeWeights;

	//������
	RetinueDataEntry* m_pDataEntry;

	//����������
	RetinueLevelDataEntry* m_pLevelDataEntry;
};


#endif //_CL_RETINUE_H_
