#ifndef _CL_RETINUE_DATAENTRY_H_
#define _CL_RETINUE_DATAENTRY_H_

#include "CLDefine.h"
#include "CLRetinueDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#define CL_MAKE_RETINUE_LEVEL_KEY(Level, Quality) ((UInt32(Level) << 16) + (UInt8)(Quality))

/**
*@brief 随从配置表
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
	//随从ID
	UInt32	id;

	//洗练库ID
	UInt32 groupId;

	//名字
	std::string name;

	//品质
	RetinueQuality quality;

	//携带等级
	UInt8	level;

	//最大星级
	UInt8	maxStar;

	//物品ID
	UInt32	itemId;

	//解锁与升星 碎片消耗数量
	std::vector<UInt32> consumeNum;

	//洗练消耗
	std::vector<std::vector<UnlockItem>> changeSkillConsume;

	//无法锁定技能个数
	UInt32 notLockSkillCount;

	//洗练解锁等级
	UInt16	changeSkillUnlock;
};

/**
*@brief 随从配置表管理器
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
*@brief 随从等级配置表
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

	//品质
	RetinueQuality quality;

	//消耗随从碎片
	UInt32	itemNum;

	//消耗
	UnlockItemVec unlockItems;

	//玩家等级
	UInt16 playerLevel;

};

/**
*@brief 随从等级配置表管理器
*/
typedef Avalon::DataEntryMgr<RetinueLevelDataEntry> RetinueLevelDataEntryMgr;



/**
*@brief 随从洗练表
*/
class RetinueSkillDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;

	//洗练库ID
	UInt32 groupId;

	//库类型
	RetinueGroupType groupType;

	//技能ID
	UInt32 dataId;

	//星级限定
	UInt8 starLevel;

	//职业限定
	std::vector<Occupation> occus;

	//锁限定
	UInt32 lockSkillCount;

	//数量
	UInt32 dataNum;

	//权重
	UInt32 weight;

	//修正1
	UInt32 fix1;

	//修正2
	UInt32 fix2;

	//修正2次数
	UInt32 fix2Num;

	//修正上限
	UInt32 fixMax;

	//修正下限
	UInt32 fixMin;

};


/**
*@brief 随从洗练表管理器
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