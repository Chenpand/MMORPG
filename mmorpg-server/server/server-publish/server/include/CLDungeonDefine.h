#ifndef __CL_LEVEL_DEFINE_H__
#define __CL_LEVEL_DEFINE_H__

#include <AvalonProtocol.h>
#include <CLDungeonDataEntry.h>
#include <CLDropItemDefine.h>

// ���³�����
enum DungeonAttr
{
    DA_INVALID,
    DA_ID,          // ���³�ID
    DA_NUM,         // ս������
    DA_BEST_SCORE,  // �������
    DA_BEST_RECORD_TIME,  // ���ͨ��ʱ��
};

// ���³��Ѷ���Ϣ����
enum DungeonHardAttr
{
    DHA_INVALID,            
    DHA_ID,                     // ���³�ID
    DHA_UNLOCKED_HARD_TYPE,     // �ѽ���������Ѷ�����
};

/**
*@brief �ؿ�����
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

// ���³���ÿ����ҵĽ�����Ϣ
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


    // �����Ϣ
    ObjID_t         roleId;
    UInt8           pos;

    // ս�����
    UInt8           score;
	// md5
	UInt8			md5[16];
    UInt16          beHitCount;

	// ��boss���˺�
	UInt64			bossDamage;

	// ��������������
	// ���ʣ��Ѫ��
	UInt32 playerRemainHp;
	// ���ʣ������
	UInt32 playerRemainMp;
	// BOSS1��ID
	UInt32 boss1ID;
	// BOSS2��ID
	UInt32 boss2ID;
	// BOSS1��ʣ��Ѫ��
	UInt32 boss1RemainHp;
	// BOSS2��ʣ��Ѫ��
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

    // ����ҵĽ�����Ϣ
    std::vector<DungeonPlayerRaceEndInfo>       infoes;
};

// ���³ǿ�����Ϣ
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

// ���־���ӳ�����
enum DungeonAdditionType
{
	// ����ҩˮ
	DUNGEON_EXP_ADD_BUFF,
	// VIP
	DUNGEON_EXP_ADD_VIP,
	// ���㾭Ӫ
	DUNGEON_EXP_ADD_SCORE,
	// �Ѷ�
	DUNGEON_EXP_ADD_HARD,
	// ���Ἴ��
	DUNGEON_EXP_ADD_GUILD,
	// VIP��Ҽӳ�
	DUNGEON_GOLD_ADD_VIP,
	// ʦͽ����ӳ�
	DUNGEON_EXP_ADD_MASTER,
	// ���Ѿ���ӳ�
	DUNGEON_EXP_ADD_FRIEND,
	// ð�նӾ���ӳ�
	DUNGEON_EXP_ADD_ADVENTURE_TEAM,
	// ��������
	DUNGEON_EXP_BASE,
	// ����
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

	// ����ID
	UInt32 monsterId;

	// �˺���Ϣ
	std::vector<DungeonDamageInfo>	damageInfoes;
};

// ƣ��ȼ������
enum FatigueBurnType
{
	// ��
	FBT_NONE,
	// ��ͨ
	FBT_COMMON,
	// �߼�
	FBT_ADVANCED,
};

// roll��������
enum RollOpTypeEnum
{
	RIE_INVALID = 0, //��Ч
	RIE_NEED = 1,   //��Ҫ
	RIE_MODEST = 2, //ǫ��
};

//roll��Ʒ��Ŀ��Ϣ
struct RollItemInfo
{
	RollItemInfo()
	: rollIndex(0){}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & rollIndex & dropItem;
	}

public:
	UInt8		rollIndex; //roll��Ʒ����
	DropItem    dropItem;  //������Ʒ
};

//rool��Ʒ�����Ϣ
struct RollDropResultItem
{
	RollDropResultItem()
		: rollIndex(0), opType(0), point(0), playerId(0), playerName("") {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & rollIndex  & opType & point & playerId & playerName;
	}
public:
	UInt8			rollIndex;	//roll��Ʒ����
	UInt8			opType;		//��������
	UInt32			point;		//roll����
	UInt64			playerId;	//���id
	std::string		playerName; //�������
};


#define ROLL_DROP_WAITE_TIEM  15

#endif
