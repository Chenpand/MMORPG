#ifndef _CL_CURRENCY_COST_DATAENTRY_H_
#define _CL_CURRENCY_COST_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// »õ±ÒÏûºÄ
class CurrencyCostDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32		id;
	UInt32		itemId;
	UInt32		num;
};

typedef Avalon::DataEntryMgr<CurrencyCostDataEntry> CurrencyCostDataEntryMgr;

#endif
