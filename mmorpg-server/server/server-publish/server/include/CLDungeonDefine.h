#ifndef __CL_LEVEL_DEFINE_H__
#define __CL_LEVEL_DEFINE_H__

#include <AvalonProtocol.h>
#include <CLDungeonDataEntry.h>
#include <CLDropItemDefine.h>

// 地下城属性
enum DungeonAttr
{
    DA_INVALID,
    DA_ID,          // 地下城ID
    DA_NUM,         // 战斗次数
    DA_BEST_SCORE,  // 最佳评分
    DA_BEST_RECORD_TIME,  // 最快通关时间
};

// 地下城难度信息属性
enum DungeonHardAttr
{
    DHA_INVALID,            
    DHA_ID,                     // 地下城ID
    DHA_UNLOCKED_HARD_TYPE,     // 已解锁的最高难度类型
};

/**
*@brief 关卡评分
*/
enum LevelScore
{
	LS_C,
    LS_B,
    LS_A,
    LS_S,
    LS_SS,
    LS_SSS,
	LS_NUM,
};

struct SceneDungeonInfo
{
    SceneDungeonInfo()
    {
        memset(this, 0, sizeof(*this));
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & id & bestScore & bestRecordTime;
    }

    UInt32	                id;
    UInt8                   bestScore;
    UInt32                  bestRecordTime;
};

struct SceneDungeonHardInfo
{
    SceneDungeonHardInfo()
    {
        memset(this, 0, sizeof(*this));
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & id & unlockedHardType;
    }

    UInt32	                id;
    UInt8                   unlockedHardType;
};

// 地下城中每个玩家的结算信息
struct DungeonPlayerRaceEndInfo
{
    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & roleId & pos & score;
		stream.SeriaArray(md5, sizeof(md5));
		stream & beHitCount & bossDamage;
		stream & playerRemainHp & playerRemainMp & boss1ID & boss2ID & boss1RemainHp & boss2RemainHp;
    }

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "roleid:" << roleId << ", pos:" << (UInt32)pos << ", score:" << (UInt32)score << ", beHitCount:" << beHitCount << ", bossDamage:" << bossDamage;
		ss << ", playerRemainHp:" << playerRemainHp << ", playerRemainMp:" << playerRemainMp << ", boss1ID:" << boss1ID << ", boss2ID:" << boss2ID << ", boss1RemainHp:" << boss1RemainHp << ", boss2RemainHp:" << boss2RemainHp;
		return ss.str();
	}

	bool operator ==(const DungeonPlayerRaceEndInfo& other) const
	{
		return memcmp(this, &other, sizeof(*this)) == 0;
	}

	bool operator !=(const DungeonPlayerRaceEndInfo& other) const
	{
		return !(*this == other);
	}


    // 玩家信息
    ObjID_t         roleId;
    UInt8           pos;

    // 战斗结果
    UInt8           score;
	// md5
	UInt8			md5[16];
    UInt16          beHitCount;

	// 对boss的伤害
	UInt64			bossDamage;

	// 极限试炼的数据
	// 玩家剩余血量
	UInt32 playerRemainHp;
	// 玩家剩余蓝量
	UInt32 playerRemainMp;
	// BOSS1的ID
	UInt32 boss1ID;
	// BOSS2的ID
	UInt32 boss2ID;
	// BOSS1的剩余血量
	UInt32 boss1RemainHp;
	// BOSS2的剩余血量
	UInt32 boss2RemainHp;
};

struct DungeonRaceEndInfo
{
    DungeonRaceEndInfo()
    {
        gamesessionId = 0;
        dungeonId = 0;
        usedTime = 0;
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & gamesessionId & dungeonId & usedTime & infoes;
    }

	bool operator ==(const DungeonRaceEndInfo& other) const
	{
		return (gamesessionId == other.gamesessionId) && (dungeonId == other.dungeonId) && (usedTime == other.usedTime)
			&& (infoes == other.infoes);
	}

	bool operator !=(const DungeonRaceEndInfo& other) const
	{
		return !(*this == other);
	}

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "session:" << gamesessionId << ", dungeon:" << dungeonId << ", usedTime:" << usedTime << ", infos:";
		for (UInt32 i = 0; i < infoes.size(); i++)
		{
			auto& info = infoes[i];
			if (i != 0)
			{
				ss << "|";
			}
			ss << info.ToString();
		}

		return ss.str();
	}

    ObjID_t                                     gamesessionId;
    UInt32                                      dungeonId;
    UInt32                                      usedTime;

    // 各玩家的结算信息
    std::vector<DungeonPlayerRaceEndInfo>       infoes;
};

// 地下城开放信息
struct DungeonOpenInfo
{
    DungeonOpenInfo()
    {
        id = 0;
        hellMode = 0;
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & id & hellMode;
    }

    UInt32      id;
    UInt8       hellMode;
};

// 各种经验加成类型
enum DungeonAdditionType
{
	// 经验药水
	DUNGEON_EXP_ADD_BUFF,
	// VIP
	DUNGEON_EXP_ADD_VIP,
	// 结算经营
	DUNGEON_EXP_ADD_SCORE,
	// 难度
	DUNGEON_EXP_ADD_HARD,
	// 公会技能
	DUNGEON_EXP_ADD_GUILD,
	// VIP金币加成
	DUNGEON_GOLD_ADD_VIP,
	// 师徒经验加成
	DUNGEON_EXP_ADD_MASTER,
	// 好友经验加成
	DUNGEON_EXP_ADD_FRIEND,
	// 冒险队经验加成
	DUNGEON_EXP_ADD_ADVENTURE_TEAM,
	// 基础经验
	DUNGEON_EXP_BASE,
	// 数量
	DUNGEON_ADD_NUM
};

struct DungeonAdditionInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(addition, DUNGEON_ADD_NUM);
	}

	DungeonAdditionInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	UInt32			addition[DUNGEON_ADD_NUM];
};

#define HELL_TICKET_ITEM 200000002


struct DungeonStaticValue
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & values;
	}

	bool IsValid(int s_values[], UInt32 num) const
	{
		if (values.size() != num)
		{
			return false;
		}

		for (UInt32 i = 0; i < num; i++) 
		{
			if (s_values[i] != values[i])
			{
				return false;
			}
		}

		return true;
	}

	std::string ToString() const
	{
		std::stringstream ss;
		for (UInt32 i = 0; i < values.size(); i++)
		{
			if (i)
			{
				ss << ",";
			}
			ss << values[i];
		}

		return ss.str();
	}

	std::vector<int> values;
};

struct DungeonDamageInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & skillId & num;
	}

	UInt32		skillId;
	UInt8		num;
};

struct DungeonKilledMonsterInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & monsterId & damageInfoes;
	}

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "monster(" << monsterId << ")";
		return ss.str();
	}

	// 怪物ID
	UInt32 monsterId;

	// 伤害信息
	std::vector<DungeonDamageInfo>	damageInfoes;
};

// 疲劳燃烧类型
enum FatigueBurnType
{
	// 无
	FBT_NONE,
	// 普通
	FBT_COMMON,
	// 高级
	FBT_ADVANCED,
};

// roll操作类型
enum RollOpTypeEnum
{
	RIE_INVALID = 0, //无效
	RIE_NEED = 1,   //需要
	RIE_MODEST = 2, //谦让
};

//roll物品项目信息
struct RollItemInfo
{
	RollItemInfo()
	: rollIndex(0){}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & rollIndex & dropItem;
	}

public:
	UInt8		rollIndex; //roll物品索引
	DropItem    dropItem;  //掉落物品
};

//rool物品结果信息
struct RollDropResultItem
{
	RollDropResultItem()
		: rollIndex(0), opType(0), point(0), playerId(0), playerName("") {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & rollIndex  & opType & point & playerId & playerName;
	}
public:
	UInt8			rollIndex;	//roll物品索引
	UInt8			opType;		//请求类型
	UInt32			point;		//roll点数
	UInt64			playerId;	//玩家id
	std::string		playerName; //玩家名字
};


#define ROLL_DROP_WAITE_TIEM  15

#endif
