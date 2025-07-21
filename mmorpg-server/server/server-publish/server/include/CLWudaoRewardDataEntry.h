#ifndef _CL_WUDAO_REWARD_DATAENTRY_H_
#define _CL_WUDAO_REWARD_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


//活动data
class WudaoRewardDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt32		id;
	// 最小胜利场数
	UInt32		minWinNum;
	// 最大胜利场数
	UInt32		maxWinNum;
	// 奖励罐子类型
	std::vector<UInt32> jarRewards;
};

class WudaoRewardDataEntryMgr : public Avalon::DataEntryMgrBase<WudaoRewardDataEntry>
	, public Avalon::Singleton<WudaoRewardDataEntryMgr>
{
public:
	UInt32 GetRewardJar(UInt32 winNum);
};
#endif
