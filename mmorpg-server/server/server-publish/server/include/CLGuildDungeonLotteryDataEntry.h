/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDDUNGEONLOTTERY_DATA_ENTRY_H__
#define __CL_GUILDDUNGEONLOTTERY_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLGuildDefine.h>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct GuildDungeonLotteryCfg
{
	// 总权重
	UInt32 totalWeight;
	// 奖励
	GuildDungeonRewardMap lotteryCfgMap;

	GuildDungeonLotteryCfg() : totalWeight(0) {}
};

// __CUSTOM_STRUCT_END__

class GuildDungeonLotteryDataEntry : public Avalon::DataEntry
{
public:
	GuildDungeonLotteryDataEntry();
	virtual ~GuildDungeonLotteryDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 奖励组
	UInt32                                          rewardGroup;
	// 道具ID
	UInt32                                          itemID;
	// 道具数量
	UInt32                                          itemNum;
	// 权重
	UInt32                                          weight;
	// 是否高价值
	UInt32                                          isHighVal;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildDungeonLotteryDataEntryMgr : public Avalon::DataEntryMgrBase<GuildDungeonLotteryDataEntry>
	, public Avalon::Singleton<GuildDungeonLotteryDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildDungeonLotteryDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 按组抽取奖励单个
	GuildDungeonReward* GetGuildDungeonReward(UInt32 rewardGroup);
	
	// 按组抽取奖励多个
	void GetGuildDungeonReward(UInt32 rewardGroup, UInt32 rewardNum, GuildDungeonRewardVec& rewardVec);

private:
	// 奖励
	std::map<UInt32, GuildDungeonLotteryCfg> m_LotteryCfgMap;

// __CUSTOM_MGR_END__
};

#endif
