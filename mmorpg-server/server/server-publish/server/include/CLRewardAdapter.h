#ifndef _CL_REWARD_ADAPTER_H_
#define _CL_REWARD_ADAPTER_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"
#include <map>

struct RewardAdapterDataEntry : public Avalon::DataEntry
{
public:
	RewardAdapterDataEntry();
	~RewardAdapterDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32 id;

	//√˚≥∆
	std::string name;

	//µ»º∂Ω±¿¯  ≈‰
	std::map< UInt16 , std::vector<ItemReward> > levelRewards;

};

typedef Avalon::DataEntryMgr<RewardAdapterDataEntry>	RewardAdapterDataEntryMgr;

#endif //_CL_REWARD_ADAPTER_H_