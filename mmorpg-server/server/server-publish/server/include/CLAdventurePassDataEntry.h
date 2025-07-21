#ifndef __CL_ADVENTURE_PASS_DATA_ENTRY_H__
#define __CL_ADVENTURE_PASS_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemDefine.h>

struct PassRewardKey
{
	//赛季id
	UInt32											season;
	// 战令等级
	UInt32                                          lv;
	PassRewardKey() = default;
	PassRewardKey(UInt32 Season, UInt32 Lv) :season(Season), lv(Lv) {};
	bool operator<(const PassRewardKey& rsh) const;
};

class AdventurePassRewardDataEntry : public Avalon::DataEntry
{
public:

	AdventurePassRewardDataEntry();
	virtual ~AdventurePassRewardDataEntry();

	bool Read(Avalon::DataRow &row);

	ItemRewardVec& GetNormalReward();
	ItemRewardVec& GetHighReward();

	UInt32 GetKey() const { return 0; }

public:
	//Key
	PassRewardKey									key;
	// 普通战令奖励
	std::string                                     normal;
	// 高级战令奖励
	std::string                                     high;
	// 本级升下一级的经验
	UInt32                                          exp;

private:
	ItemRewardVec m_NormalReward;
	ItemRewardVec m_HighReward;

};

class AdventurePassRewardDataEntryMgr : public Avalon::DataEntryMgrBase<AdventurePassRewardDataEntry>
	, public Avalon::Singleton<AdventurePassRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(AdventurePassRewardDataEntry* entry);
	virtual bool ReloadData(const std::string& filename);

	AdventurePassRewardDataEntry* FindEntry(UInt32 season, UInt32 lv) const;
private:
	std::map<PassRewardKey, AdventurePassRewardDataEntry*> m_PassReward;
};


class AdventurePassActivityDataEntry : public Avalon::DataEntry
{
public:
	AdventurePassActivityDataEntry();
	virtual ~AdventurePassActivityDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32                                          id;
	// 活跃度
	UInt32                                          activity;
	// 给与战令经验
	UInt32                                          exp;

};

class AdventurePassActivityDataEntryMgr : public Avalon::DataEntryMgrBase<AdventurePassActivityDataEntry>
	, public Avalon::Singleton<AdventurePassActivityDataEntryMgr>
{
public:
	virtual bool AddEntry(AdventurePassActivityDataEntry* entry);
	UInt32 GetAddExp(UInt32 oldActivity, UInt32 newActivity);
};


class AdventurePassBuyLevelDataEntry : public Avalon::DataEntry
{
public:
	AdventurePassBuyLevelDataEntry();
	virtual ~AdventurePassBuyLevelDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// 购买等级
	UInt32                                          buyLv;
	// 点券消耗
	UInt32                                          pointCost;

};

class AdventurePassBuyLevelDataEntryMgr : public Avalon::DataEntryMgrBase<AdventurePassBuyLevelDataEntry>
	, public Avalon::Singleton<AdventurePassBuyLevelDataEntryMgr>
{
public:
	virtual bool AddEntry(AdventurePassBuyLevelDataEntry* entry);

};

enum AdventurePassBuyRewardType
{
	APBRT_POINT = 0,
	APBRT_RMB = 1,
};

class AdventurePassBuyRewardDataEntry : public Avalon::DataEntry
{
public:
	AdventurePassBuyRewardDataEntry();
	virtual ~AdventurePassBuyRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

	ItemRewardVec& GetReward(UInt8 type);
	UInt32 GetPrice(UInt8 type);

public:
	// 赛季ID
	UInt32                                          season;
	// 王者版增加等级
	UInt32                                          kingLevel;
	// 王者版额外奖励
	std::string                                     kingRewardStr;
	// 巅峰王者版增加等级
	UInt32                                          topKingLevel;
	// 巅峰王者版额外奖励
	std::string                                     topKingRewardStr;
	// 王者版价格
	UInt32                                          kingPrice;
	// 巅峰王者版价格
	UInt32                                          topKingPrice;
	// 王者版购买途径
	UInt32											buyType;

	ItemRewardVec kingReward;
	ItemRewardVec topKingReward;
	ItemRewardVec nullReward;

private:

};

class AdventurePassBuyRewardDataEntryMgr : public Avalon::DataEntryMgrBase<AdventurePassBuyRewardDataEntry>
	, public Avalon::Singleton<AdventurePassBuyRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(AdventurePassBuyRewardDataEntry* entry);
	ItemRewardVec& GetReward(UInt32 season, UInt8 type);
	UInt32 GetPrice(UInt32 season, UInt8 type);
	UInt32 GetLevel(UInt32 season, UInt8 type);
	AdventurePassBuyRewardType GetBuyType(UInt32 season);
	ItemRewardVec nullReward;
};



class AdventurePassSeasonDataEntry : public Avalon::DataEntry
{
public:
	AdventurePassSeasonDataEntry();
	virtual ~AdventurePassSeasonDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// 赛季ID
	UInt32                                          id;
	// 开始时间
	UInt32                                     startTime;
	// 结束时间
	UInt32                                     endTime;
	// 赛季最大等级
	UInt32									   maxLevel;

};

class AdventurePassSeasonDataEntryMgr : public Avalon::DataEntryMgrBase<AdventurePassSeasonDataEntry>
	, public Avalon::Singleton<AdventurePassSeasonDataEntryMgr>
{
public:
	using TimePair = std::pair<UInt32, UInt32>;
	virtual bool AddEntry(AdventurePassSeasonDataEntry* entry);
	AdventurePassSeasonDataEntry* GetSeasonData(Avalon::Time time);
};




#endif
