#ifndef _CL_FUNCTION_DATAENTRY_H_
#define _CL_FUNCTION_DATAENTRY_H_

#include "CLDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLJobDataEntry.h"
#include "CLItemDefine.h"

enum FuncNotifyType
{
	FUNC_ACHIEVEMENT = 4,
	FUNC_PK = 7,
	FUNC_GUILD = 15,
	FUNC_TEAM = 30,
	FUNC_FRIEND = 31,
	FUNC_JAR = 36,
	FUNC_ADVENTURE_TEAM = 86,
	FUNC_ZJSL = 90,
	FUNC_TEAM_COPY = 91,
	FUNC_ADVENTUREPASSSEASON = 93,
	FUNC_TEAM_COPY_TWO = 102,
};

inline bool IsTeamCoyFun(UInt32 funId)
{
	return funId == FUNC_TEAM_COPY || funId == FUNC_TEAM_COPY_TWO;
}

/**
 *@brief 功能预告表
 */
class FuncNotifyDataEntry : public Avalon::DataEntry
{
public:
	FuncNotifyDataEntry();
	~FuncNotifyDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	bool GetRewards(const Occupation occu, std::vector<ItemReward>& itemRewards);

public:
	//id
	UInt8	id;

	//解锁类型
	UnlockType type;

	//触发任务
	UInt32	beginTask;
	//触发等级
	UInt16	beginLevel;
	//结束任务
	UInt32	endTask;
	//结束等级
	UInt16	endLevel;

	//奖励物品
	std::vector<OccuItemReward> rewards;
};

typedef Avalon::DataEntryMgr<FuncNotifyDataEntry> FuncNotifyDataEntryMgr;

#endif
