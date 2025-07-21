#ifndef _CL_RARE_ITEM_DATAENTRY_H_
#define _CL_RARE_ITEM_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLSysRecordDefine.h"

// 一日内的时间,几点几分
struct DayTime
{
	DayTime()
	{
		memset(this, 0, sizeof(*this));
	}

	UInt32 hour;
	UInt32 minute;
	UInt32 second;
};

// 开放时间
struct DayOpenTime
{
	DayTime		startTime;
	DayTime		endTime;
};

/**
*@brief 稀有控制物品数据项
*/
class RareItemDataEntry : public Avalon::DataEntry
{
public:
	RareItemDataEntry() {}
	~RareItemDataEntry() {}

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	bool IsInOpenTime(const Avalon::Date& now);

public:
	// ID
	UInt32						id;
	// 稀有控制类型
	RareControlType				type;
	// 具体的ID，根据类型决定
	UInt32						valueId;
	// 付费限额
	UInt32						minPayCost;
	// 时段限制
	std::vector<DayOpenTime>	timeLimit;
	// 时段限制总时长(min)
	UInt32						timeLimitTotalMin;
	// 个人每日产出
	UInt32						persionalDailyNum;
	// 个人每周产出
	UInt32						persionalWeeklyNum;
	// 个人每月产出
	UInt32						persionalMonthlyNum;
	// 个人总产出
	UInt32						persionalTotalNum;
	// 服务器每日产出
	UInt32						serverDailyNum;
	// 服务器每周产出
	UInt32						serverWeeklyNum;
	// 服务器每月产出
	UInt32						serverMonthlyNum;
	// 服务器总产出
	UInt32						serverTotalNum;
};

class RareItemDataEntryMgr : public Avalon::DataEntryMgrBase<RareItemDataEntry>, public Avalon::Singleton<RareItemDataEntryMgr>
{
public:
	virtual bool AddEntry(RareItemDataEntry* dataEntry);

	RareItemDataEntry* FindEntryByType(RareControlType type, UInt32 id);

private:
	std::map<std::pair<RareControlType, UInt32>, RareItemDataEntry*> m_rareItems;
};

#endif
