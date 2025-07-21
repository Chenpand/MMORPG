#ifndef __ACTIVITY_MONSTER_PROB_DATA_ENTRY_H__
#define __ACTIVITY_MONSTER_PROB_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <bitset>

/**
*@brief 活动怪物表
*/
class ActivityMonsterProbDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	UInt32					activityId;
	std::bitset<32>			tags;
	UInt32					prob;
};

class ActivityMonsterProbDataEntryMgr : public Avalon::DataEntryMgrBase<ActivityMonsterProbDataEntry>, public Avalon::Singleton<ActivityMonsterProbDataEntryMgr>
{
public:
	std::vector<ActivityMonsterProbDataEntry*> GetDatasByActivity(UInt32 activityId);
	UInt32 GetProbByActivity(UInt32 activityId, UInt32 tag);
};

#endif