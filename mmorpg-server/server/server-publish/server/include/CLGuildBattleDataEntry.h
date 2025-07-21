#ifndef _CL_GUILD_TERRITORY_DATAENTRY_H_
#define _CL_GUILD_TERRITORY_DATAENTRY_H_

#include "CLDefine.h"
#include "CLGameDefine.h"
#include "CLItemDefine.h"
#include "CLGuildDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct GuildBattleScore
{
public:
	UInt32 winScore;
	UInt32 endScore;
	UInt32 loserScore;
};

/**
*@brief 领地配置表
*/
struct GuildTerritoryDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	std::vector<ConsumeItem> GetMatchConsume(UInt32 index);

	bool IsChestDoubleDungeon(UInt32 dungeonId);

	bool IsDropAdditionDungeon(UInt32 dungeonId);

	void ParseReward(ItemRewardVec& rewardVec, std::string& strReward);

public:
	//领地ID
	UInt8 id;

	//名称
	std::string name;
	
	//领地类型
	GuildTerritoryType type;

	//领地等级
	UInt8 level;

	//需要占领的领地
	UInt8 needTerritoryId;

	//需要的公会等级
	UInt8 needGuildLevel;

	//上级领地
	UInt8 preTerritoryId;

	//报名数量
	UInt32 enroll;

	//消耗道具
	std::vector<ConsumeItem> consumeItem;

	//积分计算
	std::vector<GuildBattleScore> guildBattleScore;

	// 占领积分
	UInt32 occupyScore;

	// 战斗积分系数(百分比)
	UInt32 battleScoreFactor;

	//会长奖励
	ItemRewardVec leaderRewards;

	//成员奖励
	ItemRewardVec memberRewards;

	//会长每日奖励
	ItemRewardVec leaderDayRewards;

	//会员每日奖励
	ItemRewardVec memberDayRewards;

	//失败奖励
	ItemRewardVec loseRewards;

	//匹配消耗
	std::vector<std::vector<ConsumeItem>> matchConsumeItems;

	//地下城ID
	std::vector<UInt32> chestDoubleDungeons;

	// 每日翻牌翻倍次数
	UInt32		dailyChestDoubleTimes;

	// 翻牌加成类型(对应枚举DungeonChestType的位组合)
	UInt8		chestDoubleFlag;

	//地下城ID
	std::vector<UInt32> dropAdditionDungeons;

	//掉率加成
	UInt32		dropAddition;

	// 会长PVE加成技能
	UInt32				leaderPveAddSkill;
	// 会长PVP加成技能
	UInt32				leaderPvpAddSkill;
};

class GuildTerritoryDataEntryMgr :public  Avalon::DataEntryMgrBase<GuildTerritoryDataEntry>, public Avalon::Singleton<GuildTerritoryDataEntryMgr>
{
public:
	GuildTerritoryDataEntryMgr();
	~GuildTerritoryDataEntryMgr();

public:
	bool AddEntry(GuildTerritoryDataEntry* entry);
	
	const UInt32 GetMaxLevel() const { return m_MaxLevel; }

private:
	UInt32 m_MaxLevel;
};



/**
*@brief 公会战鼓舞表
*/
struct GuildInspireDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return level; }

	bool Read(Avalon::DataRow& row);

public:
	//等级
	UInt8 level;

	//需要的公会等级
	UInt8 needGuildLevel;

	//消耗道具
	std::vector<ConsumeItem> consumeItem;

	//跨服消耗道具
	std::vector<ConsumeItem> crossConsumeItem;
};

class GuildInspireDataEntryMgr :public  Avalon::DataEntryMgrBase<GuildInspireDataEntry>, public Avalon::Singleton<GuildInspireDataEntryMgr>
{
public:
	GuildInspireDataEntryMgr();
	~GuildInspireDataEntryMgr();

	UInt8 GetMaxLevel() { return m_MaxLevel; }

public:
	bool AddEntry(GuildInspireDataEntry* entry);

private:
	UInt8 m_MaxLevel;
};

struct GuildRewardDataEntry : public Avalon::DataEntry
{
public:
	GuildRewardDataEntry();
	~GuildRewardDataEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
	
public:
	//ID
	UInt32 id;

	//积分范围
	UInt32 beginScore;

	//奖励物品
	std::vector<ItemReward> itemReward;

};

typedef Avalon::DataEntryMgr<GuildRewardDataEntry> GuildRewardDataEntryMgr;

struct GuildBattleTimeDataEntry : public Avalon::DataEntry
{
public:
	GuildBattleTimeDataEntry() {};
	~GuildBattleTimeDataEntry() {};

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 group;
	GuildBattleType type;
	GuildBattleStatus status;
	UInt32 week;
	UInt32 hour;
	UInt32 min;
	UInt32 sec;
	bool isOpen;
};

class GuildBattleTimeDataEntryMgr : public  Avalon::DataEntryMgrBase<GuildBattleTimeDataEntry>, public Avalon::Singleton<GuildBattleTimeDataEntryMgr>
{
public:
	typedef std::map<GuildBattleStatus, GuildBattleTimeDataEntry*> GuildBattleStatusMap;
	typedef std::map<UInt32, GuildBattleStatusMap> GuildBattleTimeMap;
public:
	GuildBattleTimeDataEntryMgr(){}
	~GuildBattleTimeDataEntryMgr(){}

	//GuildBattleStatus GetGuildBattleStatus(Avalon::Time now);
	GuildBattleTimeInfo GetGuildBattleTimeInfo(Avalon::Time now, GuildBattleType type = GuildBattleType::GBT_INVALID);

	UInt32 GetGuildCrossBattleEndTime(Avalon::Time now);

	/**
	*@breif 获取上一次活动时间戳
	*/
	UInt32 GetLastGuildBattleTim(Avalon::Time now, GuildBattleType type);

public:
	bool AddEntry(GuildBattleTimeDataEntry* entry);

private:
	GuildBattleTimeMap m_GuildBattleTimeMap;
};

struct GuildBattleLotteryNumDataEntry : public Avalon::DataEntry
{
public:
	GuildBattleLotteryNumDataEntry(){}
	~GuildBattleLotteryNumDataEntry(){}

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt16 num;

};

typedef Avalon::DataEntryMgr<GuildBattleLotteryNumDataEntry> GuildBattleLotteryNumDataEntryMgr;

#endif