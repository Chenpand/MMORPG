/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDBATTLESCORERANKREWARD_DATA_ENTRY_H__
#define __CL_GUILDBATTLESCORERANKREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class GuildBattleScoreRankRewardDataEntry : public Avalon::DataEntry
{
public:
	GuildBattleScoreRankRewardDataEntry();
	virtual ~GuildBattleScoreRankRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 头衔id
	UInt32                                          titleId;
	// 头衔过期时间(分钟)
	UInt32                                          titleDueTime;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildBattleScoreRankRewardDataEntryMgr : public Avalon::DataEntryMgrBase<GuildBattleScoreRankRewardDataEntry>
	, public Avalon::Singleton<GuildBattleScoreRankRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildBattleScoreRankRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获得积分排名奖励
	const GuildBattleScoreRankRewardDataEntry* GetScoreRankReward(UInt32 rank);

private:

	std::map<UInt32, GuildBattleScoreRankRewardDataEntry*> m_ScoreRankMap;

// __CUSTOM_MGR_END__
};

#endif
