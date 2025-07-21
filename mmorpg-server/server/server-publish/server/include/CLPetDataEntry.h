#ifndef _CL_PET_DATAENTRY_H_
#define _CL_PET_DATAENTRY_H_

#include "CLPetDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#define CL_MAKE_PET_LEVEL_KEY(Level, Quality) ((UInt32(Level) << 16) + (UInt8)(Quality))

/**
*@brief �������ñ�
*/
struct PetDataEntry : public Avalon::DataEntry
{
public:
	PetDataEntry();
	~PetDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 8; };

public:
	//����ID
	UInt32 id;

	//����
	std::string name;

	//��������
	PetType type;

	//����Ʒ��
	PetQuality quality;

	//���ȼ�
	UInt16 maxLevel;

	//���ﵰid
	UInt32 itemId;

	//�����ɵľ���ֵ
	UInt32 devourExp;

	//��������
	UInt32 maxLevelScore;
};

/**
*@brief �������ñ������
*/
class PetDataEntryMgr : public Avalon::DataEntryMgrBase<PetDataEntry>, public Avalon::Singleton<PetDataEntryMgr>
{
public:
	typedef std::map<UInt32, PetDataEntry*> PetEggMap;

	PetDataEntryMgr();
	~PetDataEntryMgr();

public:
	bool AddEntry(PetDataEntry* entry);

	PetDataEntry* GetPetByEgg(UInt32 itemId);

private:
	PetEggMap m_PetEggMap;

};

/**
*@brief ����ȼ����ñ�
*/
class PetLevelDataEntry : public Avalon::DataEntry
{
public:
	PetLevelDataEntry();
	~PetLevelDataEntry();

public:
	UInt32 GetKey() const { return CL_MAKE_PET_LEVEL_KEY(level, quality); }

	bool Read(Avalon::DataRow& row);

public:
	//�ȼ�
	UInt8 level;

	//����Ʒ��
	PetQuality quality;

	//��������
	UInt32 upLevelExp;

	//ÿ��ƣ�����ӵľ���
	UInt32 fatigueExp;

	//ÿ��ƣ�����ĵ�ƣ��
	UInt32 fatigueHunger;

	//���ۼ۸�
	std::vector<ItemReward> sellRewards;

};

/**
*@brief ����ȼ����ñ������
*/
//typedef Avalon::DataEntryMgr<PetLevelDataEntry> PetLevelDataEntryMgr;

class PetLevelDataEntryMgr : public Avalon::DataEntryMgrBase<PetLevelDataEntry>, public Avalon::Singleton<PetLevelDataEntryMgr>
{
public:
	PetLevelDataEntryMgr();
	~PetLevelDataEntryMgr();

public:
	bool AddEntry(PetLevelDataEntry* entry);

	UInt32 GetSumExp(UInt16 level, PetQuality quality);

private:
	std::map<UInt32, UInt32> m_SumExp;
};

/**
*@brief ����ιʳ���ñ�
*/
class PetFeedDataEntry : public Avalon::DataEntry
{
public:
	PetFeedDataEntry();
	~PetFeedDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32 id;

	//ιʳ����
	PetFeedType type;

	//����
	std::vector<FeedItem> feedItems;
};

/**
*@brief ����ιʳ�������
*/
class PetFeedDataEntryMgr : public Avalon::DataEntryMgrBase<PetFeedDataEntry>, public Avalon::Singleton<PetFeedDataEntryMgr>
{
public:
	PetFeedDataEntryMgr();
	~PetFeedDataEntryMgr();

public:
	bool AddEntry(PetFeedDataEntry* entry);

	bool GetFeedItem(PetFeedType type, UInt32 index, FeedItem& result);

private:

	std::map<PetFeedType, std::vector<FeedItem>> m_FeedMap;

};

#endif