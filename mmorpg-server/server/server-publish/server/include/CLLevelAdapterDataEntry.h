#ifndef _CL_LEVEL_ADAPTER_H_
#define _CL_LEVEL_ADAPTER_H_

#include "CLDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct LevelAdapterDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32	id;
	//µ»º∂  ≈‰
	UInt32	values[MAX_LEVEL];
};

class LevelAdapterDataEntryMgr : public Avalon::DataEntryMgrBase<LevelAdapterDataEntry>
	, public Avalon::Singleton<LevelAdapterDataEntryMgr>
{
public:
	UInt32 GetValue(UInt32 id, UInt16 level);
};

#endif //_CL_REWARD_ADAPTER_H_