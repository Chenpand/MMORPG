#ifndef _CL_DROP_ITEM_VIP_LIMIT_DATAENTRY_H_
#define _CL_DROP_ITEM_VIP_LIMIT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

class DropItemVipLimitDataEntry : public Avalon::DataEntry
{
public:
	DropItemVipLimitDataEntry();
	~DropItemVipLimitDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
public:
	//唯一id
	UInt32					id;
	//vip获取上限
	std::vector<UInt32>		vipLimits;
};

typedef Avalon::DataEntryMgr<DropItemVipLimitDataEntry> DropItemVipLimitDataEntryMgr;

#endif