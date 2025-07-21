#ifndef __CL_DROP_ITEM_DEFINE_H__
#define __CL_DROP_ITEM_DEFINE_H__

#include <CLDefine.h>
#include <AvalonNetStream.h>
#include "CLDropItemDataEntry.h"

struct DropItem
{
    DropItem()
    {
        typeId = 0;
        num = 0;
		strenth = 0;
		equipType = 0;
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & typeId & num & strenth & equipType;
    }

    UInt32  typeId;		// 物品ID
    UInt32  num;		// 物品数量
	UInt8	strenth;	// 物品强化等级
	UInt8	equipType;	// 装备类型(对应枚举EquipType)
};
typedef std::vector<DropItem> VDropItem;

struct SceneDungeonDropItem
{
	SceneDungeonDropItem()
	{
		memset(this, 0, sizeof(*this));
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & typeId & num & isDouble & strenth & equipType;
	}

	UInt32 id;
	UInt32 typeId;
	UInt32 num;
	UInt8  isDouble;
	UInt8  strenth;
	UInt8  equipType;
};
typedef std::vector<SceneDungeonDropItem> VSceneDungeonDropItem;

struct PlayerDropItem : public DropItem
{
    PlayerDropItem()
    {
        roleId = 0;
        isPickUp = false;
        isRareControl = false;
		id = 0;
		taskId = 0;
		notice = 0;
		isDouble = false;
		solutionId = 0;
		monsterNormalHardId = 0;
		monsterDailyMaxDropNum = 0;
		monsterDungeonAreaIndex = 0;
		isDailyDrop = false;
		vipDropLimitId = 0;
    }

    PlayerDropItem& operator=(const DropItem& item)
    {
        typeId = item.typeId;
        num = item.num;
		strenth = item.strenth;
		equipType = item.equipType;
		isDouble = false;
		monsterNormalHardId = 0;
		monsterDailyMaxDropNum = 0;
		monsterDungeonAreaIndex = 0;
		isDailyDrop = false;
		vipDropLimitId = 0;
        return *this;
    }
	
	SceneDungeonDropItem GetSceneDungeonDropItem() const
	{
		SceneDungeonDropItem dropItem;
		dropItem.id = id;
		dropItem.typeId = typeId;
		dropItem.num = num;
		dropItem.isDouble = isDouble ? 1 : 0;
		dropItem.strenth = strenth;
		dropItem.equipType = equipType;
		return dropItem;
	}

//     AVALON_DEFINE_NETSERIALIZATION()
//     {
//         stream & roleId & id & typeId & num & strenth;
//     }

    UInt64      roleId;         // 玩家ID
    bool        isPickUp;       // 是否已经拾取
    bool        isRareControl;  // 是否稀有控制
    UInt32      id;             // 掉落唯一ID
    UInt32      taskId;         // 掉落关联的任务ID
	UInt32		notice;			// 提示
	bool		isDouble;		// 是不是双倍复制出来的
	UInt32		solutionId;		// 掉落方案ID
	UInt32		monsterNormalHardId;	// 普通难度怪物ID
	UInt32		monsterDailyMaxDropNum;	// 怪物每日掉落次数上限
	UInt32		monsterDungeonAreaIndex;// 怪物所在地下城区域索引
	bool		isDailyDrop;			// 是否是每日必然掉落的道具
	UInt32		vipDropLimitId;			// vip掉落上限控制Id
};
typedef std::vector<PlayerDropItem> VPlayerDropItem;


// 结算宝箱类型
enum DungeonChestType
{
    DCT_NORMAL,     // 普通宝箱
    DCT_VIP,        // VIP宝箱
    DCT_PAY,        // 付费宝箱
    DCT_NUM,
};

struct DungeonChest : public DropItem
{
    DungeonChest()
    {
        goldReward = 0;
        isRareControl = 0;
        type = 0;
        isOpened = false;
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & typeId & num & type & goldReward & isRareControl & strenth & equipType;
    }
    
    DungeonChest& operator=(const DropItem& item)
    {
        typeId = item.typeId;
        num = item.num;
		strenth = item.strenth;
		equipType = item.equipType;
        return *this;
    }

    // 宝箱类型，对应枚举（DungeonChestType）
    UInt8       type;    
    UInt8       isRareControl;
    UInt32      goldReward;
    bool        isOpened;
};
typedef std::vector<DungeonChest> VDungeonChest;

// 各种加成类型
/*enum DungeonAdditionType
{
	// 经验加成开始
	DUNGEON_ADD_EXP_BEGIN = 0,
	// buff
	DUNGEON_ADD_EXP_BUFF,
	// VIP
	DUNGEON_ADD_EXP_VIP,
	// 结算经营
	DUNGEON_ADD_EXP_SCORE,
	// 难度
	DUNGEON_ADD_EXP_HARD,
	// 公会技能
	DUNGEON_ADD_EXP_GUILD,
	// 经验加成结束
	DUNGEON_ADD_EXP_END,

	// 金币加成
	DUNGEON_ADD_GOLD_BEGIN,
	// buff
	DUNGEON_ADD_GOLD_BUFF,
	// VIP
	DUNGEON_ADD_GOLD_VIP,
	// 公会技能
	DUNGEON_ADD_GOLD_GUILD_SKILL,
	// 金币加成结束
	DUNGEON_ADD_GOLD_END,

	// 任务道具加成
	DUNGEON_ADD_TASK_ITEM_BEGIN,
	// buff
	DUNGEON_ADD_TASK_ITEM_BUFF,
	// 任务道具加成结束
	DUNGEON_ADD_TASK_ITEM_END,

	// 神器掉落加成开始
	DUNGEON_ADD_PINK_DROP_BEGIN,
	// buff
	DUNGEON_ADD_PINK_DROP_BUFF,
	// 神器掉落加成结束
	DUNGEON_ADD_PINK_DROP_END,

	// 数量
	DUNGEON_ADD_NUM
};*/

// 关卡加成系数类型
enum DungeonAdditionRateType
{
	// 经验加成
	DART_EXP_RATE,
	// 金币加成
	DART_GOLD_RATE,
	// 任务道具掉落数量加成
	DART_TASK_DROP_NUM_RATE,
	// 神器掉率加成
	DART_PINK_DROP_RATE,
	// Buff经验加成
	DART_BUFF_EXP_RATE,
	// 公会技能经验加成
	DART_GUID_SKILL_EXP_RATE,
	// Vip经验加成
	DART_VIP_EXP_RATE,
	// Vip金币加成
	DART_VIP_GOLD_RATE,
	// 师徒经验加成
	DART_MASTER_EXP_RATE,
	// 运营活动经验加成
	DART_OP_ACTVITY_EXP_RATE,
	// 疲劳燃烧活动加成
	DART_FATIGUE_BURNING_RATE,
	// 公会领地翻牌加成标志（这个是一个位运算，对应于枚举DungeonChestType）
	DART_GUILD_TERR_CHEST_DOUBLE_FLAG,
	// 好友经验加成
	DART_FRIEND_EXP_RATE,
	// 绑金加成
	DART_BIND_GOLD_RATE,
	// 水苍玉加成
	DART_BERYL_RATE,
	// 冒险队加成
	DART_ADVENTURE_TEAM_EXP_RATE,
	// 数量
	DART_NUM,
};

// 关卡加成系数
struct DungeonAddition
{
    DungeonAddition()
    {
        memset(this, 0, sizeof(*this));
    }

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(additionRate, DART_NUM);
		stream.SeriaArray(dropAddition, DIT_NUM);
		stream.SeriaArray(dropMultiplyRateAddition, DIT_NUM);
	}

	UInt32 additionRate[DART_NUM];
	// 掉落加成
	UInt16	dropAddition[DIT_NUM];
	// 掉落乘率加成
	UInt32 dropMultiplyRateAddition[DIT_NUM];
};

enum DropItemMarkType
{
	DIMT_INVAL = 0,
	DIMT_TEAM_COPY_GOLD = 1,
	DIMT_ACTIVE_EXTRA_DROP = 2,
};


enum DropItemExtra
{
	DIE_NONE = 0,
	DIE_MONTH_CARD = 1,
	DIE_GIFT_RIGHT_CARD = 2,
};

#endif
