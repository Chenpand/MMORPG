#ifndef _CL_SCORE_WAR_DATAENTRY_H_
#define _CL_SCORE_WAR_DATAENTRY_H_

#include "CLDefine.h"
#include "CLScoreWarDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 积分赛时间配置表
*/
struct ScoreWarTimeDataEntry : public Avalon::DataEntry
{
public:
	ScoreWarTimeDataEntry() { };
	~ScoreWarTimeDataEntry() { };

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 group;
	ScoreWarStatus status;
	UInt32 week;
	UInt32 hour;
	UInt32 min;
	UInt32 sec;
	bool isOpen;

};

class ScoreWarTimeDataEntryMgr : public Avalon::DataEntryMgrBase<ScoreWarTimeDataEntry>, public Avalon::Singleton<ScoreWarTimeDataEntryMgr>
{
public:
	typedef std::map<ScoreWarStatus, ScoreWarTimeDataEntry*> ScoreWarStatusMap;
	typedef std::map<UInt32, ScoreWarStatusMap> ScoreWarTimeMap;
public:
	ScoreWarTimeDataEntryMgr() {}
	~ScoreWarTimeDataEntryMgr() {}

	ScoreWarTimeInfo GetScoreWarTimeInfo(Avalon::Time now);

public:
	bool AddEntry(ScoreWarTimeDataEntry* entry);

private:
	ScoreWarTimeMap m_ScoreWarTimeMap;
};

/**
*@brief 积分赛奖励配置表
*/
struct ScoreWarRewardDataEntry : public Avalon::DataEntry
{
public:
	ScoreWarRewardDataEntry() {}
	~ScoreWarRewardDataEntry() {}

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

class ScoreWarRewardDataEntryMgr : public Avalon::DataEntryMgrBase<ScoreWarRewardDataEntry>, public Avalon::Singleton<ScoreWarRewardDataEntryMgr>
{
public:
	ScoreWarRewardDataEntryMgr() {}
	~ScoreWarRewardDataEntryMgr() {}

	bool AddEntry(ScoreWarRewardDataEntry* entry);

	ScoreWarRewardDataEntry* GetBaseDataEntry(UInt32 rewardId);

	ScoreWarRewardDataEntry* GetRankingDataEntry(UInt32 ranking);

private:
	std::map<UInt32, ScoreWarRewardDataEntry*> m_BaseRewords;
	std::vector<ScoreWarRewardDataEntry*> m_RankingRewards;

};


#endif //_CL_SCORE_WAR_DATAENTRY_H_
