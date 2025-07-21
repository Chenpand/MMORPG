/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDDUNGEONREWARD_DATA_ENTRY_H__
#define __CL_GUILDDUNGEONREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <AvalonObjectPool.h>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

enum EGuildDungeonReward
{
	E_GUILD_DUNGEON_REWARD_INVAILD,

	E_GUILD_DUNGEON_REWARD_VICTORY = 1,	// 胜利奖励
	E_GUILD_DUNGEON_REWARD_ATTEND = 2,	// 参与奖励

	E_GUILD_DUNGEON_REWARD_FEISHI = 3,	// 飞狮奖励
	E_GUILD_DUNGEON_REWARD_SHADIE = 4,	// 沙碟奖励
	E_GUILD_DUNGEON_REWARD_MONV = 5,	// 魔女奖励

	E_GUILD_DUNGEON_REWARD_AUCTION = 6,	// 拍卖奖励

	E_GUILD_DUNGEON_REWARD_TEMP = 7,	// 临时奖励

	E_GUILD_DUNGEON_REWARD_MAX,
};

struct RewardTypeGroup
{
	AVALON_DEFINE_OBJECTPOOL(RewardTypeGroup, Avalon::NullMutex)

	RewardTypeGroup() : rewardType(0), dungeonType(0) {}

	// 奖励类型
	UInt32 rewardType;
	// 地下城类型
	UInt32 dungeonType;

	bool operator < (const RewardTypeGroup& rtg) const
	{
		if (rtg.rewardType < rewardType)
			return true;

		if (rewardType < rtg.rewardType)
			return false;

		if (rtg.dungeonType < dungeonType)
			return true;

		if (dungeonType < rtg.dungeonType)
			return false;

		return false;
	}
};

struct RewardInfo
{
	AVALON_DEFINE_OBJECTPOOL(RewardInfo, Avalon::NullMutex)

	RewardInfo() : rewardGroup(0), rewardCnt(0), fixPrice(0), auctionPrice(0), addPirce(0) {}

	// 奖励组
	UInt32 rewardGroup;
	// 奖励次数
	UInt32 rewardCnt;
	// 一口价
	UInt32 fixPrice;
	// 起拍价
	UInt32 auctionPrice;
	// 每次加价
	UInt32 addPirce;
};

struct RewardDetail
{
	AVALON_DEFINE_OBJECTPOOL(RewardDetail, Avalon::NullMutex)

	RewardDetail() : rewardGroupWeight(0), rewardSchemeWeight(0), ownerBonusLimit(0) {}

	// 奖励信息
	std::map<UInt32, RewardInfo*> rewardInfoMap;
	std::vector<RewardInfo*> rewardInfo;

	// 奖励组总权重
	UInt32 rewardGroupWeight;

	// 奖励方案权重
	std::map<UInt32, UInt32> rewardSchemeMap;
	// 总权重
	UInt32 rewardSchemeWeight;

	// 个人分成限制
	UInt32 ownerBonusLimit;
};

// __CUSTOM_STRUCT_END__

class GuildDungeonRewardDataEntry : public Avalon::DataEntry
{
public:
	GuildDungeonRewardDataEntry();
	virtual ~GuildDungeonRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 奖励类型
	UInt32                                          rewardType;
	// 地下城类型
	UInt32                                          dungeonType;
	// 伤害权重(万分比）
	UInt32                                          damageWeight;
	// 个人分成限制
	UInt32                                          ownerBonusLimit;
	// 奖励组
	std::vector<UInt32>                             rewardGroup;
	// 奖励次数
	std::vector<UInt32>                             rewardCount;
	// 奖励组权重
	std::vector<UInt32>                             rewardWeight;
	// 奖励方案个数
	std::vector<UInt32>                             rewardSchemeCnt;
	// 方案个数权重
	std::vector<UInt32>                             schemeWeight;
	// 一口价
	std::vector<UInt32>                             fixPrice;
	// 起拍价
	std::vector<UInt32>                             auctionPrice;
	// 每次加价
	std::vector<UInt32>                             addPirce;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildDungeonRewardDataEntryMgr : public Avalon::DataEntryMgrBase<GuildDungeonRewardDataEntry>
	, public Avalon::Singleton<GuildDungeonRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildDungeonRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获取奖励组根据奖励类型和伤害比例
	RewardDetail* GetRewardDetail(UInt32 rewardType, UInt32 dungeonType, UInt32 damageRate = 0);

private:

	// 奖励
	std::map<RewardTypeGroup, std::map<UInt32, RewardDetail>> m_RewardMap;

// __CUSTOM_MGR_END__
};

#endif
