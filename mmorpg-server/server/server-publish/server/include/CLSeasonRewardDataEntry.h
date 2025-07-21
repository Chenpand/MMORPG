#ifndef _CL_SEASON_REWARD_DATAENTRY_H_
#define _CL_SEASON_REWARD_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemDefine.h>

/**
*@brief 活跃度事件数据项
*/
class SeasonRewardDataEntry : public Avalon::DataEntry
{
public:
	SeasonRewardDataEntry();
	~SeasonRewardDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32				id;
	//起始排名
	UInt32				startRank;
	//结束排名
	UInt32				endRank;
	//奖励
	ItemRewardVec		rewards;
};

typedef Avalon::DataEntryMgr<SeasonRewardDataEntry> SeasonRewardDataEntryMgr;

class SeasonDailyDataEntry : public Avalon::DataEntry
{
public:
	SeasonDailyDataEntry();
	~SeasonDailyDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;
	//起始积分
	UInt32 startScore;
	//结束积分
	UInt32 endScore;
	//奖励
	ItemRewardVec rewards;

};

class SeasonDailyDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonDailyDataEntry>, public Avalon::Singleton<SeasonDailyDataEntryMgr>
{
public:
	SeasonDailyDataEntryMgr();
	~SeasonDailyDataEntryMgr();

public:
	bool AddEntry(SeasonDailyDataEntry* entry);

	UInt32 GetMinValue() { return minValue; };

	ItemRewardVec GetItemRewardVec(UInt32 matchScore);

private:
	UInt32 minValue;
};

#endif
