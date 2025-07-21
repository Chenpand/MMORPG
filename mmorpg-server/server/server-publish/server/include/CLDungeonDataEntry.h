#ifndef __CL_LEVEL_DATA_ENTRY_H__
#define __CL_LEVEL_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct AreaGroup
{
	bool HasArea(UInt32 areaId) const 
	{
		return std::find(areaIds.begin(), areaIds.end(), areaId) != areaIds.end();
	}

	UInt32 randArea(std::vector<UInt32>& exceptIds) const
	{
		std::vector<UInt32> ids;

		for (auto id : areaIds)
		{
			if (std::find(exceptIds.begin(), exceptIds.end(), id) == exceptIds.end())
			{
				ids.push_back(id);
			}
		}

		if (ids.empty())
		{
			return 0;
		}

		UInt32 index = Avalon::Random::RandBetween(0, ids.size() - 1);
		if (index >= ids.size())
		{
			return 0;
		}

		return ids[index];
	}

	std::vector<UInt32> areaIds;
};

// 地下城类型
enum DungeonType
{
    LT_NORMAL,      // 普通
    LT_STORY,       // 剧情
    LT_ACTIVITY,    // 活动
    LT_TOWER,       // 塔
};

// 地下城子类型
enum DungeonSubType
{
	// 普通关卡
	DST_NORMAL,
	// 远古关卡
	DST_YUANGU,
	// 牛头怪乐园
	DST_NIUTOUGUAI,
	// 南部溪谷
	DST_NANBUXIGU,
	// 死亡之塔
	DST_SIWANGZHITA,
	// 引导
	DST_GUILD,
	// PK战斗
	DST_PK,
	// 金币
	DST_GOLD,
	// 深渊
	DST_HELL,
	// 公会PK
	DST_GUILD_PK,
	// 深渊入口
	DST_HELL_ENTRY,
	// 组队BOSS
	DST_TEAM_BOSS,
	// 武道会关卡
	DST_WUDAOHUI = 13,
	// 绝望之塔
	DST_JUEWANG_TOWER,
	// 连招教学
	DST_COMBO,
	// 怪物攻城
	DST_CITY_MONSTER,
	// 异界
	DST_DEVILDDOM,
	// 公会地下城
	DST_GUILD_DUNGEON,
	// 限时深渊
	DST_LIMIT_TIME_HELL,
	// 周常深渊
	DST_WEEK_HELL, 
	// PVE训练场
	DST_TRAINING_PVE,
	// 周常深渊入口
	DST_WEEK_HELL_ENTRY,
	// 周常深渊前置关卡
	DST_WEEK_HELL_PER,
	// 终极试炼爬塔
	DST_ZJSL_TOWER,
	// 团队副本
	DST_RAID_DUNGEON,
	// 周年活动普通
	DST_ANNIVERSARY_NORMAL,
	// 周年活动困难
	DST_ANNIVERSARY_HARD,
	// 春节随机副本
	DST_TREASUREMAP = 28,
	// 免费限时深渊
	DST_FREE_LIMIT_TIME_HELL = 31,
	// 大富翁小游戏关卡
	DST_PLAYGAME = 32,

	// 数量
	DST_NUM,
};

// 地下城第三类型
enum DungeonThreeType
{
	DTT_NORMAL,			//  普通地下城
	DTT_TEAM_ELITE,		//  组队精英地下城
	DTT_CHIJI_PK,		//  吃鸡PK场景
	DTT_LOST_DUNGEON_MONSTER,		//  地牢小怪地下城
};

// 地下城难度
enum DungeonHardType
{
    LHT_NORMAL,     // 普通
    LHT_RISK,       // 冒险
    LHT_WARRIOR,    // 勇士
    LHT_KING,       // 王者

    LHT_NUM,
};

// 深渊模式
enum DungeonHellMode
{
	// 无
	HELL_MODE_NULL,
	// 普通
	HELL_MODE_NORMAL,
	// 困难
	HELL_MODE_HARD,
	// 终极困难
	HELL_MODE_HARD_ULTIMATE,

	HELL_MODE_MAX,
};

// 地下城门票消耗
struct DungeonTicket
{
    DungeonTicket()
    {
        itemId = 0;
        num = 0;
    }

    UInt32      itemId;
    UInt32       num;
};

// 结算掉落消耗
struct RaceEndDropMultiCost
{
	RaceEndDropMultiCost() : multi(0), cost(0) {}
	UInt8	multi;
	UInt32	cost;
};
typedef std::vector<RaceEndDropMultiCost> VRaceEndDropMultiCost;

// 关卡怪物等级适配类型
enum DungeonMonsterLevelAdaptType
{
	// 怪物本身等级
	DMLAT_MONSTER,
	// 关卡等级
	DMLAT_DUNGEON,
	// 队伍平均等级
	DMLAT_TEAM,
	// 怪物攻城加权平均等级
	DMLAT_CITY_MONSTER
};

struct DungeonChapterKey
{
	DungeonChapterKey(UInt32 dungeonId_, UInt8 hard_)
		: dungeonId(dungeonId_), hard(hard_)
	{}
	bool operator<(const DungeonChapterKey& other) const
	{
		if (other.dungeonId != dungeonId)
		{
			return dungeonId < other.dungeonId;
		}

		return hard < other.hard;
	}

	UInt32	dungeonId;
	UInt8   hard;

};

/**
*@brief 地下城数据项
*/
class DungeonDataEntry : public Avalon::DataEntry
{
public:
    DungeonDataEntry();
    ~DungeonDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

	// 是不是深渊地下城入口
	bool IsHellEntry() const;

	// 是不是深渊地下城
	bool IsHellDungeon() const;

	int GetAreaIndex(UInt32 areaId) const;

	// 是不是周常深渊入口
	bool IsWeekHellEntry() const;
private:
    bool ReadDropIDs(std::string dropIDStrs, UInt32* dropIDs);

public:
    // id
    UInt32	                id;
	// 名字
	std::string				name;
    // 关卡等级
    UInt16                  level;
    // 类型
    DungeonType             type;
	// 子类型
	DungeonSubType			subType;
	// 第三类型
	DungeonThreeType		threeType;
    // 难度
    DungeonHardType         hardType;
	// 标签
	UInt32					tag;
	// 是否难度根据等级
	DungeonMonsterLevelAdaptType monsterLevelAdaptType;
	// 最高难度适配等级
	UInt32					maxHardAdaptLevel;
    // 最低等级
    UInt32                  minLevel;
    // 剧情任务
    UInt32                  storyTaskId;
    // 前置任务
    UInt32                  preTaskId;
    // 前置地下城
    UInt32                  prevDungeonId;
    // 剧情地下城
    std::vector<UInt32>     storyDungeonIds;
	// 经验是否等级适配
	bool					isExpLevelAdapt;
    // 经验奖励
    UInt32                  expReward;
    // 区域ID
	std::vector<AreaGroup>     areaList;
    // 起始区域ID
    UInt32                  startAreaId;
	// BOSS区域
	UInt32					bossAreaId;
    // 深渊区域ID
    std::vector<UInt32>     hellAreaList;
    // 每个区域消耗的疲劳
    UInt16                  costFatiguePerArea;
    // 小怪掉落
    UInt32                  normalDropItem;
    // 精英掉落
    UInt32                  eliteDropItem;
    // BOSS掉落
    UInt32                  bossDropItem;
	// 关卡掉落
	UInt32                  dungeonDropItem;
	// 活动掉落(只在结算时掉落)
	std::vector<UInt32>		activityDropItems;
	// 关卡首次掉落
	UInt32					dungeonFirstDropItem;
	// 可破坏物掉落
	UInt32					destructDropItem;
	// 彩蛋掉落
	UInt32					easterEggDropItem;
    // 任务掉落
    std::vector<UInt32>     taskDropItems;
	// 公告掉落
	UInt32					rollDropId;
    // 门票
    DungeonTicket           ticket;
    // 深渊票
    UInt32                  hellTicketNum;
    // 深渊任务
    UInt32                  hellTask;
    // 深渊掉落
    std::vector<UInt32>     hellDrops[HELL_MODE_MAX];
	// 深渊活动掉落
	std::vector<UInt32>		hellActivityDropItems;
	// 单个可复活次数
	Int32					maxReviveTimes;
	// 团本队伍可复活次数
	UInt32					maxTeamCopyReviveTimes;
	// 结算掉落基础倍率
	std::vector<UInt8>		raceEndDropBaseMulti;
	// 不同倍率结算掉落所需消耗
	VRaceEndDropMultiCost	raceEndDropMultiCost;
	// 活动ID
	UInt32					activityId;
	// 每日次数
	UInt32					dailyMaxTime;
	// 通关最少房间数
	UInt32					clearMinAreaNum;
	// 关卡内可用的道具
	std::vector<UInt32>     quickUseItems;
	// 深渊等级段
	UInt32					hellSplitLevel;
	// 深渊随机概率
	UInt32					hellProb;
	// 是否只有结算收益
	bool					onlyRaceEndProfit;
	// 是否有师徒收益加成
	bool					masterExpAddition;
	// 引导任务, key为引导任务id, value为对应引导地下城id
	std::map<UInt32, UInt32> guideTasks;
	// 是否单人副本
	bool					singleDungeon;
	// 是否通关地下城结算
	bool					onlyRaceEndSettlement;
	// 所属地下城入口
	UInt32					ownerEntryId;
	// 入口选中权重
	UInt32					weightEntry;
	// 精英地下城前置章节
	UInt32					eliteDungeonPrevChapter;
};

struct HellDungeonInfo
{
	UInt32 dungeonId;
	UInt32 prob;
};

class DungeonDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonDataEntry>
	, public Avalon::Singleton<DungeonDataEntryMgr>
{
public:
	/**
	*@brief 将数据项加入列表中
	*/
	virtual bool AddEntry(DungeonDataEntry* entry);

	/**
	*@brief 根据深渊等级随机深渊地下城ID
	*/
	UInt32 GenHellDungeonByLevel(UInt32 level, DungeonHardType hard);

	/**
	*@brief 根据深渊等级随机深渊地下城ID
	*/
	UInt32 GetHellEntry(UInt32 level, DungeonHardType hard);

	/**
	*@brief 根据地下城入口ID随机地下城ID
	*/
	UInt32 GetDungeonIdByEntryId(UInt32 entryId);

	/**
	*@brief 根据地下城id找地下城入口ID
	*/
	UInt32 GetDungeonEntryIdByDungeonId(UInt32 dungeonId);

	/**
	*@brief 根据章节id，难度获取地下城id
	*/
	bool  GetDungeonIdsByChapterHard(UInt32 chapter, UInt8 hard, std::vector<UInt32>& ids);

	std::map<DungeonChapterKey, std::vector<UInt32>>& GetAllChapterIds() {
		return  m_ChapterDungeonIds;
	}
private:
	// 深渊地下城（根据等级和难度）
	std::map<UInt32, std::vector<HellDungeonInfo>>	m_HellDungeonMap;
	// 深渊地下城入口（根据等级和难度）
	std::map<UInt32, UInt32>						m_HellEntryMap;
	// 地下城入口集合
	std::map<UInt32, std::vector<HellDungeonInfo>>  m_DungeonMap;
	// 地下城id映射入口id
	std::map<UInt32, UInt32>						m_DungeonMapEntry;
	// 章节地下城id
	std::map<DungeonChapterKey, std::vector<UInt32>>  m_ChapterDungeonIds;
};

#endif