#ifndef __CL_SEASON_DATA_ENTRY_H__
#define __CL_SEASON_DATA_ENTRY_H__

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLSeasonDefine.h>

/**
*@brief 赛季表
*/
struct SeasonDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;

	SeasonEventType eventType;

	//循环类型
	SeasonCyclesType cyclesType;

	//开始天
	UInt32 day;

	//开始时间
	std::string startTime;
};

/**
*@brief 赛季表管理
*/
class SeasonDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonDataEntry>, public Avalon::Singleton<SeasonDataEntryMgr>
{
public:
	SeasonDataEntryMgr();
	~SeasonDataEntryMgr();

public:
	bool AddEntry(SeasonDataEntry* entry);

public:
	SeasonDataEntry* GetSeason(UInt32 now);

	UInt64 GetNextSeasonTime(SeasonEventType eventType, Avalon::Date nowDate);

	UInt64 GetNowSeasonTime(SeasonEventType eventType, Avalon::Date nowDate);

private:
	SeasonCyclesType m_SeasonCyclesType;

	std::map<UInt32, SeasonDataEntry*> m_SeasonDayMap;
};

/**
*@brief 赛季段位表
*/
struct SeasonLevelDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	bool IsUplevelBattle();

public:
	//段位id
	UInt32 id;

	//上级段位
	UInt32 preId;

	//下级段位
	UInt32 afterId;

	//大段位
	UInt32 mainLevel;

	//小段位
	UInt32 smallLevel;

	//星级
	UInt32 star;

	//最大经验
	UInt32 maxExp;

	//隐藏分数
	UInt32 hideScore;

	//是否匹配机器人
	bool isRobot;

	//是否掉级
	bool isDownLevel;

	//是否晋级模式
	bool isPromotion;

	//晋级赛胜利次数
	UInt32 uplevelBattleWinCount;

	//晋级赛战斗次数
	UInt32 uplevelBattleCount;

	//晋级赛胜利段位
	UInt32 uplevelBattleWinExp;

	//晋级赛失败段位
	UInt32 uplevelBattleLoseExp;

	//结算天数
	UInt32 battleDayNumber;
	//掉落经验
	UInt32 battleCount;

	//清档回归段位等级
	UInt32 downLevel;

	//每日奖励
	std::vector<ItemReward>	dailyRewards;

	//赛季奖励
	std::vector<ItemReward> seasonRewards;

	UInt32 attrId;
};

/**
*@brief 赛季段位表管理
*/
class SeasonLevelDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonLevelDataEntry>, public Avalon::Singleton<SeasonLevelDataEntryMgr>
{

public:
	SeasonLevelDataEntryMgr();
	~SeasonLevelDataEntryMgr();

public:
	bool AddEntry(SeasonLevelDataEntry* entry);

	SeasonLevelDataEntry* GetSeasonLevel(UInt32 score);

	SeasonLevelDataEntry* GetMinSeasonLevel();

	SeasonLevelDataEntry* GetMaxSeasonLevel();

	UInt32 GetMinSeasonLevelValue();

	UInt32 GetMaxSeasonLevelValue();

	//SeasonLevelDataEntry* GetPreSeason(UInt32 id);

	//SeasonLevelDataEntry* GetAfterSeason(UInt32 id);

	ItemRewardVec GetDailyItemRewardVec(UInt32 id);

	UInt32 GetSeasonExp(UInt32 winMainLevel, UInt32 loseMainLevel);

	bool GetSeasonLevelByExp(UInt32& seasonLevel, UInt32& seasonStar, UInt32& seasonExp, Int32 incExp);

	UInt32 GetSeasonLevelSumExp(UInt32 seasonLevel);

	SeasonMainLevel GetSeasonMainLevelBySumExp(UInt32 exp);

	UInt32 GetSeasonLevelBySumExp(UInt32 exp);

private:
	SeasonLevelDataEntry* m_MinSeasonLevel;

	SeasonLevelDataEntry* m_MaxSeasonLevel;

	//std::map<UInt32, SeasonLevelDataEntry*> m_PreSeasonMap;

	//std::map<UInt32, SeasonLevelDataEntry*> m_AfterSeasonMap;

	std::vector<SeasonLevelDataEntry*> m_MatchScoreVec;

	std::vector<SeasonExpEntry> m_SeasonExpVec;

	UInt32 m_SeasonExpVecSize;

	std::map<UInt32, UInt32> m_SeasonLevelExpMap;

	std::map<UInt32, UInt32> m_SeasonExpLevelMap;

};

/*
struct SeasonAttrDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;

	//buff
	std::vector<UInt32> buffIds;

};

class SeasonAttrDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonAttrDataEntry>, public Avalon::Singleton<SeasonAttrDataEntryMgr>
{
public:
	SeasonAttrDataEntryMgr(){}
	~SeasonAttrDataEntryMgr(){}
public:
	bool AddEntry(SeasonAttrDataEntry* entry);

	SeasonAttrDataEntry* GetSeasonSortListAttr();

};
*/

#endif  //__CL_SEASON_DATA_ENTRY_H__