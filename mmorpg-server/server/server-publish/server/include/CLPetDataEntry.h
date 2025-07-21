#ifndef _CL_PET_DATAENTRY_H_
#define _CL_PET_DATAENTRY_H_

#include "CLPetDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#define CL_MAKE_PET_LEVEL_KEY(Level, Quality) ((UInt32(Level) << 16) + (UInt8)(Quality))

/**
*@brief 宠物配置表
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
	//宠物ID
	UInt32 id;

	//名称
	std::string name;

	//宠物类型
	PetType type;

	//宠物品质
	PetQuality quality;

	//最大等级
	UInt16 maxLevel;

	//宠物蛋id
	UInt32 itemId;

	//被吞噬的经验值
	UInt32 devourExp;

	//满级评分
	UInt32 maxLevelScore;
};

/**
*@brief 宠物配置表管理器
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
*@brief 宠物等级配置表
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
	//等级
	UInt8 level;

	//宠物品质
	PetQuality quality;

	//升级经验
	UInt32 upLevelExp;

	//每点疲劳增加的经验
	UInt32 fatigueExp;

	//每点疲劳消耗的疲劳
	UInt32 fatigueHunger;

	//出售价格
	std::vector<ItemReward> sellRewards;

};

/**
*@brief 宠物等级配置表管理器
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
*@brief 宠物喂食配置表
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

	//喂食类型
	PetFeedType type;

	//消耗
	std::vector<FeedItem> feedItems;
};

/**
*@brief 宠物喂食表管理器
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