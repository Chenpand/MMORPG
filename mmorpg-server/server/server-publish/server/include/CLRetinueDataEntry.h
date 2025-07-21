#ifndef _CL_RETINUE_DATAENTRY_H_
#define _CL_RETINUE_DATAENTRY_H_

#include "CLDefine.h"
#include "CLRetinueDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#define CL_MAKE_RETINUE_LEVEL_KEY(Level, Quality) ((UInt32(Level) << 16) + (UInt8)(Quality))

/**
*@brief ������ñ�
*/
struct RetinueDataEntry : public Avalon::DataEntry
{
public:
	RetinueDataEntry();
	~RetinueDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	bool GetUnlockItem(UnlockItem& unlockItem, UInt8 starLevel = 0);

	bool GetChangeSkillConsume(std::vector<UnlockItem>& unlockItems, UInt32 lockSkillCount);

public:
	//���ID
	UInt32	id;

	//ϴ����ID
	UInt32 groupId;

	//����
	std::string name;

	//Ʒ��
	RetinueQuality quality;

	//Я���ȼ�
	UInt8	level;

	//����Ǽ�
	UInt8	maxStar;

	//��ƷID
	UInt32	itemId;

	//���������� ��Ƭ��������
	std::vector<UInt32> consumeNum;

	//ϴ������
	std::vector<std::vector<UnlockItem>> changeSkillConsume;

	//�޷��������ܸ���
	UInt32 notLockSkillCount;

	//ϴ�������ȼ�
	UInt16	changeSkillUnlock;
};

/**
*@brief ������ñ������
*/
//typedef Avalon::DataEntryMgr<RetinueDataEntry> RetinueDataEntryMgr;
class RetinueDataEntryMgr : public Avalon::DataEntryMgrBase<RetinueDataEntry>, public Avalon::Singleton<RetinueDataEntryMgr>
{
public:
	typedef std::map<UInt32, RetinueDataEntry*> RetinueItemIdMap;

	RetinueDataEntryMgr();
	~RetinueDataEntryMgr();

public:
	bool AddEntry(RetinueDataEntry* entry);
	
	RetinueDataEntry* GetRetinueDataEntry(UInt32 itemId);

private:
	RetinueItemIdMap m_RetinueItemId;

};


/**
*@brief ��ӵȼ����ñ�
*/
class RetinueLevelDataEntry : public Avalon::DataEntry
{
public:
	typedef std::vector<UnlockItem> UnlockItemVec;

public:
	RetinueLevelDataEntry();
	~RetinueLevelDataEntry();

public:
	UInt32 GetKey() const { return CL_MAKE_RETINUE_LEVEL_KEY(level, quality); }

	bool Read(Avalon::DataRow& row);

public:
	//level
	UInt8 level;

	//Ʒ��
	RetinueQuality quality;

	//���������Ƭ
	UInt32	itemNum;

	//����
	UnlockItemVec unlockItems;

	//��ҵȼ�
	UInt16 playerLevel;

};

/**
*@brief ��ӵȼ����ñ������
*/
typedef Avalon::DataEntryMgr<RetinueLevelDataEntry> RetinueLevelDataEntryMgr;



/**
*@brief ���ϴ����
*/
class RetinueSkillDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;

	//ϴ����ID
	UInt32 groupId;

	//������
	RetinueGroupType groupType;

	//����ID
	UInt32 dataId;

	//�Ǽ��޶�
	UInt8 starLevel;

	//ְҵ�޶�
	std::vector<Occupation> occus;

	//���޶�
	UInt32 lockSkillCount;

	//����
	UInt32 dataNum;

	//Ȩ��
	UInt32 weight;

	//����1
	UInt32 fix1;

	//����2
	UInt32 fix2;

	//����2����
	UInt32 fix2Num;

	//��������
	UInt32 fixMax;

	//��������
	UInt32 fixMin;

};


/**
*@brief ���ϴ���������
*/
class RetinueSkillDataEntryMgr : public Avalon::DataEntryMgrBase<RetinueSkillDataEntry>, public Avalon::Singleton<RetinueSkillDataEntryMgr>
{
public:
	typedef std::vector<RetinueSkillDataEntry*> RetinueSkillVec;
	typedef std::map<Occupation, RetinueSkillVec> OccuRetinueSkillMap;
	typedef std::map<UInt32, OccuRetinueSkillMap> GroupOccuRetinueSkillMap;
	typedef std::map<UInt32, RetinueSkillVec> GroupRetinueSkillMap;

	struct RetinueSkillPoint
	{
		RetinueSkillPoint() : degree(0) {}
		UInt32 degree;
		std::set<UInt32> subRetinueSkill;
	};

	RetinueSkillDataEntryMgr();
	~RetinueSkillDataEntryMgr();

public:
	bool AddEntry(RetinueSkillDataEntry* entry);

	std::vector<RetinueSkillDataEntry*> GetRetinueSkills(UInt32 groupId, Occupation occu);

	std::vector<RetinueSkillDataEntry*> GetRetinueSkills(UInt32 groupId);

	bool CheckData();

private:
	GroupOccuRetinueSkillMap m_GroupOccuRetinueSkillMap;
	GroupRetinueSkillMap m_GroupRetinueSkillMap;

};

#endif //_CL_RETINUE_DATAENTRY_H_