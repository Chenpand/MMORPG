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

    UInt32  typeId;		// ��ƷID
    UInt32  num;		// ��Ʒ����
	UInt8	strenth;	// ��Ʒǿ���ȼ�
	UInt8	equipType;	// װ������(��Ӧö��EquipType)
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

    UInt64      roleId;         // ���ID
    bool        isPickUp;       // �Ƿ��Ѿ�ʰȡ
    bool        isRareControl;  // �Ƿ�ϡ�п���
    UInt32      id;             // ����ΨһID
    UInt32      taskId;         // �������������ID
	UInt32		notice;			// ��ʾ
	bool		isDouble;		// �ǲ���˫�����Ƴ�����
	UInt32		solutionId;		// ���䷽��ID
	UInt32		monsterNormalHardId;	// ��ͨ�Ѷȹ���ID
	UInt32		monsterDailyMaxDropNum;	// ����ÿ�յ����������
	UInt32		monsterDungeonAreaIndex;// �������ڵ��³���������
	bool		isDailyDrop;			// �Ƿ���ÿ�ձ�Ȼ����ĵ���
	UInt32		vipDropLimitId;			// vip�������޿���Id
};
typedef std::vector<PlayerDropItem> VPlayerDropItem;


// ���㱦������
enum DungeonChestType
{
    DCT_NORMAL,     // ��ͨ����
    DCT_VIP,        // VIP����
    DCT_PAY,        // ���ѱ���
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

    // �������ͣ���Ӧö�٣�DungeonChestType��
    UInt8       type;    
    UInt8       isRareControl;
    UInt32      goldReward;
    bool        isOpened;
};
typedef std::vector<DungeonChest> VDungeonChest;

// ���ּӳ�����
/*enum DungeonAdditionType
{
	// ����ӳɿ�ʼ
	DUNGEON_ADD_EXP_BEGIN = 0,
	// buff
	DUNGEON_ADD_EXP_BUFF,
	// VIP
	DUNGEON_ADD_EXP_VIP,
	// ���㾭Ӫ
	DUNGEON_ADD_EXP_SCORE,
	// �Ѷ�
	DUNGEON_ADD_EXP_HARD,
	// ���Ἴ��
	DUNGEON_ADD_EXP_GUILD,
	// ����ӳɽ���
	DUNGEON_ADD_EXP_END,

	// ��Ҽӳ�
	DUNGEON_ADD_GOLD_BEGIN,
	// buff
	DUNGEON_ADD_GOLD_BUFF,
	// VIP
	DUNGEON_ADD_GOLD_VIP,
	// ���Ἴ��
	DUNGEON_ADD_GOLD_GUILD_SKILL,
	// ��Ҽӳɽ���
	DUNGEON_ADD_GOLD_END,

	// ������߼ӳ�
	DUNGEON_ADD_TASK_ITEM_BEGIN,
	// buff
	DUNGEON_ADD_TASK_ITEM_BUFF,
	// ������߼ӳɽ���
	DUNGEON_ADD_TASK_ITEM_END,

	// ��������ӳɿ�ʼ
	DUNGEON_ADD_PINK_DROP_BEGIN,
	// buff
	DUNGEON_ADD_PINK_DROP_BUFF,
	// ��������ӳɽ���
	DUNGEON_ADD_PINK_DROP_END,

	// ����
	DUNGEON_ADD_NUM
};*/

// �ؿ��ӳ�ϵ������
enum DungeonAdditionRateType
{
	// ����ӳ�
	DART_EXP_RATE,
	// ��Ҽӳ�
	DART_GOLD_RATE,
	// ������ߵ��������ӳ�
	DART_TASK_DROP_NUM_RATE,
	// �������ʼӳ�
	DART_PINK_DROP_RATE,
	// Buff����ӳ�
	DART_BUFF_EXP_RATE,
	// ���Ἴ�ܾ���ӳ�
	DART_GUID_SKILL_EXP_RATE,
	// Vip����ӳ�
	DART_VIP_EXP_RATE,
	// Vip��Ҽӳ�
	DART_VIP_GOLD_RATE,
	// ʦͽ����ӳ�
	DART_MASTER_EXP_RATE,
	// ��Ӫ�����ӳ�
	DART_OP_ACTVITY_EXP_RATE,
	// ƣ��ȼ�ջ�ӳ�
	DART_FATIGUE_BURNING_RATE,
	// ������ط��Ƽӳɱ�־�������һ��λ���㣬��Ӧ��ö��DungeonChestType��
	DART_GUILD_TERR_CHEST_DOUBLE_FLAG,
	// ���Ѿ���ӳ�
	DART_FRIEND_EXP_RATE,
	// ���ӳ�
	DART_BIND_GOLD_RATE,
	// ˮ����ӳ�
	DART_BERYL_RATE,
	// ð�նӼӳ�
	DART_ADVENTURE_TEAM_EXP_RATE,
	// ����
	DART_NUM,
};

// �ؿ��ӳ�ϵ��
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
	// ����ӳ�
	UInt16	dropAddition[DIT_NUM];
	// ������ʼӳ�
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
