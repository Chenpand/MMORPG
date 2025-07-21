#ifndef _CL_QUICK_BUY_DATAENTRY_H_
#define _CL_QUICK_BUY_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 快速购买数据项
*/
class QuickBuyDataEntry : public Avalon::DataEntry
{
public:
	QuickBuyDataEntry() {}
	~QuickBuyDataEntry() {}

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// 道具id
	UInt32	id;
	// 货币ID
	UInt32	costItemId;
	// 货币数量
	UInt32	costNum;
	// 积分倍率
	UInt8 multiple;
};

typedef Avalon::DataEntryMgr<QuickBuyDataEntry> QuickBuyDataEntryMgr;

#endif
