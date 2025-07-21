#ifndef __CL_MONTHSIGNAWARD_DATA_ENTRY_H__
#define __CL_MONTHSIGNAWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <map>

struct MonthSignKey
{
	MonthSignKey() = default;
	MonthSignKey(UInt8 month, UInt8 day){ this->month = month; this->day = day; }
	bool operator<(const MonthSignKey &rsh) const;
	// 月份
	UInt8                                          month;
	// 天数
	UInt8                                          day;
};

class MonthSignAwardDataEntry : public Avalon::DataEntry
{
public:
	MonthSignAwardDataEntry();
	virtual ~MonthSignAwardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	//月份天数组成的key
	MonthSignKey                                   key;
	// 贵几开始奖励翻倍
	UInt8                                          vipDouble;
	// 奖励道具ID
	UInt32                                         itemID;
	// 奖励道具数量
	UInt32                                         itemNum;

};

class MonthSignAwardDataEntryMgr : public Avalon::DataEntryMgrBase<MonthSignAwardDataEntry>
	, public Avalon::Singleton<MonthSignAwardDataEntryMgr>
{
public:
	virtual bool AddEntry(MonthSignAwardDataEntry* entry);
	MonthSignAwardDataEntry* FindEntry(UInt8 month,UInt8 day) const;
private:
	std::map<MonthSignKey, MonthSignAwardDataEntry*> m_MonthSignMap;
};


class MonthSignCollectAwardDataEntry : public Avalon::DataEntry
{
public:
	MonthSignCollectAwardDataEntry();
	virtual ~MonthSignCollectAwardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

public:
	// 累计几天
	UInt8                                          collectDay;
	// 奖励道具ID
	UInt32                                          itemID;
	// 奖励道具数量
	UInt32                                          itemNum;

};

class MonthSignCollectAwardDataEntryMgr : public Avalon::DataEntryMgrBase<MonthSignCollectAwardDataEntry>
	, public Avalon::Singleton<MonthSignCollectAwardDataEntryMgr>
{
public:
	virtual bool AddEntry(MonthSignCollectAwardDataEntry* entry);
};

#endif
