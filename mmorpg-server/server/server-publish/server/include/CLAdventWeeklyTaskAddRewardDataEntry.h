#ifndef _CL_ADVENT_WEEKLY_TASK_ADD_REWARD_DATAENTRY_H_
#define _CL_ADVENT_WEEKLY_TASK_ADD_REWARD_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"

struct AdventAwardItem 
{
	AdventAwardItem()
	 : weight(0){}
	UInt32		weight;
	ItemReward  item;
};

class AdventWeeklyTaskAddRewardDataEntry : public Avalon::DataEntry
{
public:
	AdventWeeklyTaskAddRewardDataEntry();
	~AdventWeeklyTaskAddRewardDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	// Ψһid
	UInt32							id;
	// �����Ѷ�
	UInt8							hard;
	// Ȩ��
	UInt32							weight;
	// ��������
	std::vector<AdventAwardItem>    itemRewards;
};

typedef std::vector<AdventWeeklyTaskAddRewardDataEntry*> AdventWeeklyTaskAddRewardDataEntryList;

class AdventWeeklyTaskAddRewardDataEntryMgr : public Avalon::DataEntryMgrBase<AdventWeeklyTaskAddRewardDataEntry>
	, public Avalon::Singleton<AdventWeeklyTaskAddRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(AdventWeeklyTaskAddRewardDataEntry* entry);

	bool	GetRewardByTaskHard(UInt8 taskHard, ItemReward& itemReward);

private:
	std::map<UInt8, AdventWeeklyTaskAddRewardDataEntryList>  m_dataEntrys;
};

#endif