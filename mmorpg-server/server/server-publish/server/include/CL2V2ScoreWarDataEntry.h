#ifndef _CL_2V2_SCORE_WAR_DATAENTRY_H_
#define _CL_2V2_SCORE_WAR_DATAENTRY_H_

#include "CLDefine.h"
#include "CL2V2ScoreWarDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 积分赛时间配置表
*/
struct ScoreWar2V2TimeDataEntry : public Avalon::DataEntry
{
public:
	ScoreWar2V2TimeDataEntry() { };
	~ScoreWar2V2TimeDataEntry() { };

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 group;
	ScoreWar2V2Status status;
	UInt32 week;
	UInt32 hour;
	UInt32 min;
	UInt32 sec;
	bool isOpen;

};

class ScoreWar2V2TimeDataEntryMgr : public Avalon::DataEntryMgrBase<ScoreWar2V2TimeDataEntry>, public Avalon::Singleton<ScoreWar2V2TimeDataEntryMgr>
{
public:
	typedef std::map<ScoreWar2V2Status, ScoreWar2V2TimeDataEntry*> ScoreWarStatusMap;
	typedef std::map<UInt32, ScoreWarStatusMap> ScoreWarTimeMap;
public:
	ScoreWar2V2TimeDataEntryMgr() {}
	~ScoreWar2V2TimeDataEntryMgr() {}

	ScoreWar2V2TimeInfo GetScoreWarTimeInfo(Avalon::Time now);

public:
	bool AddEntry(ScoreWar2V2TimeDataEntry* entry);

private:
	ScoreWarTimeMap m_ScoreWarTimeMap;
};

/**
*@brief 积分赛奖励配置表
*/
struct ScoreWar2V2RewardDataEntry : public Avalon::DataEntry
{
public:
	ScoreWar2V2RewardDataEntry() {}
	~ScoreWar2V2RewardDataEntry() {}

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 rewardId;
	UInt32 battleCount;
	UInt32 winCount;
	UInt32 rankingBegin;
	UInt32 rankingEnd;
	ItemRewardVec rewards;

};

class ScoreWar2V2RewardDataEntryMgr : public Avalon::DataEntryMgrBase<ScoreWar2V2RewardDataEntry>, public Avalon::Singleton<ScoreWar2V2RewardDataEntryMgr>
{
public:
	ScoreWar2V2RewardDataEntryMgr() {}
	~ScoreWar2V2RewardDataEntryMgr() {}

	bool AddEntry(ScoreWar2V2RewardDataEntry* entry);

	ScoreWar2V2RewardDataEntry* GetBaseDataEntry(UInt32 rewardId);

	ScoreWar2V2RewardDataEntry* GetRankingDataEntry(UInt32 ranking);

private:
	std::map<UInt32, ScoreWar2V2RewardDataEntry*> m_BaseRewords;
	std::vector<ScoreWar2V2RewardDataEntry*> m_RankingRewards;

};


#endif //_CL_SCORE_WAR_DATAENTRY_H_
