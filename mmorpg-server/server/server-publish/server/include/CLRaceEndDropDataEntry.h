#ifndef __CL_RACE_END_DROP_DATA_ENTRY_H__
#define __CL_RACE_END_DROP_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct RaceEndDrop
{
	RaceEndDrop() : dropSetId(0), num(0) {}
	// 掉落方案
	UInt32		dropSetId;
	// 随机次数
	UInt32		num;
};

/**
*@brief 结算掉落数据
*/
class RaceEndDropDataEntry : public Avalon::DataEntry
{
public:
	RaceEndDropDataEntry();
	~RaceEndDropDataEntry();

	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt16						id;
	UInt32						dungeonId;
	UInt32						minLevel;
	UInt32						maxLevel;
	std::vector<RaceEndDrop>	dropSets;
};

typedef Avalon::DataEntryMgr<RaceEndDropDataEntry> RaceEndDropDataEntryMgr;

#endif