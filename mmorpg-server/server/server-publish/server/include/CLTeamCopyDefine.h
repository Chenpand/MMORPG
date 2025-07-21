#ifndef _CL_TEAM_COPY_DEFINE_H_
#define _CL_TEAM_COPY_DEFINE_H_

#include <set>
#include <AvalonObjectPool.h>
#include <AvalonNetStream.h>
#include <CLDefine.h>
#include <CLTeamCopyValueDataEntry.h>

struct TeamCopyTargetCfg;
class TeamCopyFieldDataEntry;

// ���ʱ������(����)
#define TEAM_JOIN_TIME_LIMIT (60)

// ��ѯ�����ս��Ϣ��ʱʱ��
#define QUERY_DUNGEON_PLAYER_TIME_OUT  (5)

// ���Ҹ��ӶԶ��Ȧ��
#define SEARCH_GRID_MAX_ROUND  (2)

// ����IDƫ��
#define TEAM_ID_OFFSET (1000)

// ��ö���С��ΨһID
#define GET_TEAM_SQUAD_ID(TEAMID, SQUADID) \
	(TEAMID * TEAM_ID_OFFSET + SQUADID)

// ��ö���ID����ΨһID
#define GET_TEAM_ID(ID) \
	(ID / TEAM_ID_OFFSET)

// ���С��ID����ΨһID
#define GET_SQUAD_ID(ID) \
	(ID % TEAM_ID_OFFSET)

enum TeamCopyTeamModel
{
	TEAM_COPY_TEAM_MODEL_DEFAULT = 0,	// Ĭ��ȫ��ģʽ
	TEAM_COPY_TEAM_MODEL_CHALLENGE = 1,	// ��սģʽ
	TEAM_COPY_TEAM_MODEL_GOLD = 2,		// ����ģʽ
};

enum TeamCopyTeamStatus
{
	TEAM_COPY_TEAM_STATUS_PARPARE = 0,	// ��ս
	TEAM_COPY_TEAM_STATUS_BATTLE = 1,	// ս��
	TEAM_COPY_TEAM_STATUS_VICTORY = 2,	// ʤ��
	TEAM_COPY_TEAM_STATUS_FAILED = 3,	// ʧ��
};

enum TeamCopySquadStatus
{
	TEAM_COPY_SQUAD_STATUS_INIT = 0,	// ������
	TEAM_COPY_SQUAD_STATUS_PREPARE = 1,	// ��ս��
	TEAM_COPY_SQUAD_STATUS_BATTLE = 2,	// ս����
};

enum TeamCopyPost
{
	TEAM_COPY_POST_NORMAL = 1 << 0,		// ��Ա
	TEAM_COPY_POST_GOLD = 1 << 1,		// ����
	TEAM_COPY_POST_CAPTAIN = 1 << 2,	// �ӳ�
	TEAM_COPY_POST_CHIEF = 1 << 3,		// �ų�
};

enum TeamCopyStage
{
	TEAM_COPY_STAGE_NULL = 0,	// �ս׶�
	TEAM_COPY_STAGE_ONE = 1,	// ��һ�׶�
	TEAM_COPY_STAGE_TWO = 2,	// �ڶ��׶�
	TEAM_COPY_STAGE_FINAL = 3,	// ���ս׶�
};

enum TeamCopyTargetType
{
	TEAM_COPY_TARGET_TYPE_TEAM = 1,	 // �Ŷ�Ŀ��
	TEAM_COPY_TARGET_TYPE_SQUAD = 2, // С��Ŀ��
};

enum TeamCopyGrade
{
	TEAM_COPY_GRADE_TEAM = 0,	// �Ŷ��Ѷ�
	TEAM_COPY_GRADE_A = 1,	// A�Ѷ�
	TEAM_COPY_GRADE_B = 2,	// B�Ѷ�
	TEAM_COPY_GRADE_C = 3,	// C�Ѷ�
	TEAM_COPY_GRADE_D = 4,	// D�Ѷ�
};

enum TeamCopyPlanModel
{
	TEAM_COPY_PLAN_MODEL_INVALID = 0, // ��Ч
	TEAM_COPY_PLAN_MODEL_FREE = 1,	// ����ģʽ
	TEAM_COPY_PLAN_MODEL_GUIDE = 2,	// ����ģʽ
};

enum TeamCopyAppear
{
	TEAM_COPY_APPEAR_START = 1,  // ��ʼ����
	TEAM_COPY_APPEAR_UNLOCK = 2, // ��������
	TEAM_COPY_APPEAR_BOSS = 3, // boss�ݵ�
	TEAM_COPY_APPEAR_BOSS_ATTACH = 4, // boss�����ݵ�
};

enum TeamCopyFieldStatus
{
	TEAM_COPY_FIELD_STATUS_INVALID = 0, // ��Ч״̬
	TEAM_COPY_FIELD_STATUS_INIT = 1,	// ��ʼ״̬
	TEAM_COPY_FIELD_STATUS_REBORN = 2,	// ����
	TEAM_COPY_FIELD_STATUS_DEFEAT= 3,	// ����
	TEAM_COPY_FIELD_STATUS_URGENT = 4,	// ����
	TEAM_COPY_FIELD_STATUS_UNLOCKING = 5,// ������
	TEAM_COPY_FIELD_STATUS_ENERGY_REVIVE = 6,// �����ָ���
};

enum TeamCopyCheckCode
{
	TEAM_COPY_CHECK_CODE_SUCCESS = 0,	// �ɹ�
	TEAM_COPY_CHECK_CODE_COUNT = 1,		// ��������
	TEAM_COPY_CHECK_CODE_TICKET = 2,	// ��Ʊ����
	TEAM_COPY_CHECK_CODE_COMMISSION = 3,// Ӷ����
	TEAM_COPY_CHECK_CODE_UNKNOWN = 4,	// δ֪����
};

// ������������
enum TeamCopyFlopLimit
{
	TEAM_COPY_FLOP_LIMIT_NULL = 0,	// ������
	TEAM_COPY_FLOP_LIMIT_DAY = 1,	// ������
	TEAM_COPY_FLOP_LIMIT_WEEK = 2,	// ������
	TEAM_COPY_FLOP_LIMIT_PASS_GATE = 3,	// ͨ������
};

// �ű���������
enum TeamCopyConsumeType
{
	TEAM_COPY_CONSUME_TYPE_TICKET = 0,	// ��Ʊ
	TEAM_COPY_CONSUME_TYPE_COMMISSION = 1,	// Ӷ��
};

// �ű��Ѷ�
enum TeamCopyTeamGrade
{
	TEAM_COPY_TEAM_GRADE_COMMON = 1,	// ��ͨ�Ѷ�
	TEAM_COPY_TEAM_GRADE_DIFF = 2,	// �����Ѷ�
	TEAM_COPY_TEAM_GRADE_NUM = 3,
};

// �ű��ֿ���������
enum TeamCopyCrossDateType
{
	TEAM_COPY_CROSS_NULL = 0,
	TEAM_COPY_CROSS_WEEK = 1,	// ����
	TEAM_COPY_CROSS_DAY = 2,	// ����
};

inline bool IsValidGrade(UInt32 targetGrade)
{
	return targetGrade >= TEAM_COPY_GRADE_TEAM && targetGrade <= TEAM_COPY_GRADE_D;
}

inline UInt32 GetTeamCopyValue(UInt32 type)
{
	return TeamCopyValueDataEntryMgr::Instance()->GetTeamCopyValue(type);
}

inline UInt32 GetSquadId(UInt32 seatId)
{
	return (seatId - 1) / 3 + 1;
}

inline bool IsTCUnlockTask(UInt32 taskId)
{
	switch (taskId)
	{
	case 2024:
	case 2334:
		return true;
	default:break;
	}

	return false;
}

struct TCConsumeItem
{
	TCConsumeItem() : id(0), num(0), consumeType(0) {}
	TCConsumeItem(UInt32 _id, UInt32 _num) : id(_id), num(_num), consumeType(0) {}
	TCConsumeItem(UInt32 _id, UInt32 _num, UInt32 _consumeType) : id(_id), num(_num), consumeType(_consumeType) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & num;
	}

	UInt32 id;
	UInt32 num;
	UInt32 consumeType;
};

/**
*@brief �ű���Ա
*/
struct TeamCopyMember
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & accId & playerId & playerName & playerOccu & playerAwaken &
			playerLevel & post & equipScore & seatId & ticketIsEnough & zoneId & expireTime;
	}

	TeamCopyMember() : accId(0), playerId(0), playerOccu(0), playerAwaken(0),
		playerLevel(0), post(0), equipScore(0), seatId(0), ticketIsEnough(0), 
		zoneId(0), connId(0), expireTime(0), changeSquadCnt(0) {}

	bool operator<(const TeamCopyMember& member) const
	{
		return seatId < member.seatId;
	}

	// �˺�ID
	UInt32 accId;
	// ���ID
	UInt64 playerId;
	// �������
	std::string playerName;
	// ���ְҵ
	UInt32 playerOccu;
	// ����
	UInt32 playerAwaken;
	// �ȼ�
	UInt32 playerLevel;
	// ְλ
	UInt32 post;
	// װ������
	UInt32 equipScore;
	// ��λID
	UInt32 seatId;
	// ��Ʊ�Ƿ��㹻
	UInt32 ticketIsEnough;
	// zoneId
	UInt32 zoneId;
	// ����ID
	UInt32 connId;
	// ����ʱ��
	UInt64 expireTime;
	// ��С�Ӵ���
	UInt32 changeSquadCnt;
	// ����(key->guid)
	std::map<UInt64, TCConsumeItem> consumeMap;
};

struct TeamCopyTargetDetail
{
	TeamCopyTargetDetail() : fieldId(0), curNum(0), totalNum(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & fieldId & curNum & totalNum;
	}

	// �ݵ�id
	UInt32 fieldId;
	// ��ǰ����
	UInt32 curNum;
	// �ܴ���
	UInt32 totalNum;
}; 

// Ŀ������
struct TeamCopyTarget
{
	TeamCopyTarget() : targetId(0), config(NULL){}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & targetId & targetDetailVec;
	}

	bool isFinish()
	{
		for (auto& iter : targetDetailVec)
		{
			if (iter.curNum < iter.totalNum)
				return false;
		}

		return true;
	}

	// Ŀ��Id
	UInt32 targetId;
	// ����
	const TeamCopyTargetCfg* config;
	// Ŀ������
	std::vector<TeamCopyTargetDetail> targetDetailVec;
};

// �ݵ�
struct TeamCopyField
{
	TeamCopyField() : fieldId(0), oddNum(0), state(0), rebornTime(0), energyReviveTime(0), config(NULL){}
	
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & fieldId & oddNum & state & rebornTime & energyReviveTime & attackSquadList;
	}

	bool isFinish()
	{
		return oddNum == 0;
	}

	void DelAttackSquad(UInt32 squadId)
	{
		std::vector<UInt32>::iterator iter = attackSquadList.begin();
		for (; iter != attackSquadList.end(); ++iter)
		{
			if (*iter == squadId)
			{
				attackSquadList.erase(iter);
				return;
			}
		}
	}

	void AddAttackSquad(UInt32 squadId)
	{
		for (auto id : attackSquadList)
		{
			if (id == squadId)
				return;
		}

		attackSquadList.push_back(squadId);
	}

	void AddSquadRace(UInt64 raceId)
	{
		fieldRaceList.insert(raceId);
	}

	void DelSquadRace(UInt64 raceId)
	{
		fieldRaceList.erase(raceId);
	}

	bool isCanChallenge()
	{
		switch (state)
		{
		case TEAM_COPY_FIELD_STATUS_INIT:break;
		case TEAM_COPY_FIELD_STATUS_ENERGY_REVIVE:break;
		default: return false;
		}

		return true;
	}

	// �ݵ�id
	UInt32 fieldId;
	// ʣ�����
	UInt32 oddNum;
	// ״̬
	UInt32 state;
	// ����ʱ���
	UInt32 rebornTime;
	// �����ָ�ʱ���
	UInt32 energyReviveTime;
	// ����С��
	std::vector<UInt32> attackSquadList;
	// �ݵ����
	std::set<UInt64> fieldRaceList;
	// ����
	const TeamCopyFieldDataEntry* config;
};

/**
*@brief С������
*/
struct TeamCopySquad
{
	TeamCopySquad() : squadId(0), squadStatus(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & squadId & squadStatus & teamMemberList;
	}

	// С��ID
	UInt32 squadId;
	// С��״̬
	UInt32 squadStatus;
	// С��Ŀ��
	TeamCopyTarget squadTarget;
	// ��Ա�б�
	std::vector<TeamCopyMember> teamMemberList;

	// ����һ����λ
	UInt32 AllocSeat()
	{
		UInt32 squadCapacity = GetTeamCopyValue(TC_VALUE_SQUAD_CAPACITY);
		if (teamMemberList.size() >= squadCapacity)
		{
			return 0;
		}

		for (UInt32 k = 1; k <= squadCapacity; ++k)
		{
			bool isFind = false;
			UInt32 id = (squadId - 1) * 3 + k;
			for (auto& iter : teamMemberList)
			{
				if (id == iter.seatId)
				{
					isFind = true;
					break;
				}
			}

			if ( !isFind)
				return id;
		}

		return 0;
	}

	void AddMember(TeamCopyMember& member)
	{
		teamMemberList.push_back(member);
		std::sort(teamMemberList.begin(), teamMemberList.end());
	}

	void SetSquadStatus(UInt32 status) { squadStatus = status; }
	UInt32 GetSquadStatus() {  return squadStatus; }
};

// սǰ����
struct TeamCopyBattlePlan
{
	TeamCopyBattlePlan() : grade(0), squadId(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & grade & squadId;
	}

	// �Ѷ�
	UInt32 grade;
	// С��id
	UInt32 squadId;
};

// ���ƽ���
struct TeamCopyFlop
{
	TeamCopyFlop() : playerId(0), rewardId(0), rewardNum(0), number(0), goldFlop(0), isLimit(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerName & playerId & rewardId & rewardNum & number & goldFlop & isLimit;
	}

	// ���name
	std::string playerName;
	// ���Id
	UInt64 playerId;
	// ����id
	UInt32 rewardId;
	// ��������
	UInt32 rewardNum;
	// ���
	UInt32 number;
	// �Ƿ����
	UInt32 goldFlop;
	// �Ƿ�����(0:�����ƣ�1:�����ƣ�2:������)
	UInt32 isLimit;

	std::string SerializeStr()
	{
		std::stringstream ss;
		ss << " playerName:" << playerName
		 << ", playerId:" << playerId
		 << ", rewardId:" << rewardId
		 << ", rewardNum:" << rewardNum
		 << ", number:" << number
		 << ", goldFlop:" << goldFlop
		 << ", isLimit:" << isLimit;
		return ss.str();
	}
};

struct TeamCopyApplyProperty
{
	TeamCopyApplyProperty() : playerId(0), playerOccu(0), playerAwaken(0),
		playerLevel(0), equipScore(0), isGold(0), guildId(0), zoneId(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerId & playerName & playerOccu & playerAwaken & playerLevel & 
			equipScore & isGold & guildId & zoneId;
	}

	// ���ID
	UInt64 playerId;
	// �������
	std::string playerName;
	// ���ְҵ
	UInt32 playerOccu;
	// ����
	UInt32 playerAwaken;
	// �ȼ�
	UInt32 playerLevel;
	// װ������
	UInt32 equipScore;
	// �Ƿ����(��0�ǽ���)
	UInt32 isGold;
	// ����id
	UInt64 guildId;
	// ����id
	UInt32 zoneId;
};

struct TeamCopyCommission
{
	TeamCopyCommission() : connId(0), frozenOrder(0) {}

	// ����id��������뿪ʱ��ͨ�����ӷ��ر���
	UInt32 connId;
	// ���ᶩ����
	UInt64 frozenOrder;
};

// ������Ϣ
struct TeamCopyInviteInfo
{
	TeamCopyInviteInfo() : teamId(0), teamType(0), teamModel(0), teamGrade(0), occu(0), awaken(0), level(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & teamId & teamType & teamModel & teamGrade & name & occu & awaken & level;
	}

	// ����ID
	UInt32 teamId;
	// ��������
	UInt32 teamType;
	// ����ģʽ
	UInt32 teamModel;
	// �����Ѷ�
	UInt32 teamGrade;
	// ����
	std::string name;
	// ְҵ
	UInt32 occu;
	// ����
	UInt32 awaken;
	// �ȼ�
	UInt32 level;
};

// boss�����ݵ������Ϣ
struct TeamCopyBossPhase
{
	TeamCopyBossPhase() { Clear(); }
	// �׶�
	UInt32 phase;
	// �������
	UInt64 roleId;
	// �ݵ�id
	UInt32 fieldId;
	// ����֡
	UInt32 unlockFrame;
	// bossѪ������
	UInt32 bossBloodRate;

	void Clear() 
	{
		phase = 0;
		roleId = 0;
		fieldId = 0;
		unlockFrame = 0;
		bossBloodRate = 0;
	}
};

//**************************** �ű�2 ****************************

// �ű�����
enum TeamCopyType
{
	// ������
	TC_TYPE_ONE = 1,
	// Ⱥ�絺
	TC_TYPE_TWO = 2,

	TC_TYPE_MAX,
};

// ���Ӷ�������
enum TCGridObjType
{
	// �ո���
	TC_GRID_OBJ_NULL = 0,
	// ������
	TC_GRID_OBJ_TRANSFER = 1,
	// ǰ��ս���̶��㣩
	TC_GRID_OBJ_FIX_POINT = 2,
	// ������վ
	TC_GRID_OBJ_ENERGY_POINT = 3,
	// ����̳
	TC_GRID_OBJ_ALTAR = 4,
	// �ֲ�������̳
	TC_GRID_OBJ_TERROR_ALTAR = 5,
	// boss
	TC_GRID_OBJ_BOSS = 6,
	// ������
	TC_GRID_OBJ_BORN_POINT = 7,
	// ����
	TC_GRID_OBJ_MAIN_CITY = 8,
	// ���ʲ��������������
	TC_GRID_OBJ_REVIVE_CNT = 9,
	// ����������CD���̣�
	TC_GRID_OBJ_CD_DOWN = 10,
	// ʵ����(�������)
	TC_GRID_OBJ_LABORATORY = 11,
	// С��
	TC_GRID_OBJ_SQUAD = 12,
	// ����
	TC_GRID_OBJ_MONSTER = 13,
	// ���ﴫ����
	TC_GRID_OBJ_MONSTER_TRANSFER = 14,
};

// ���Ӿݵ�״̬
enum TCGridObjStatus
{
	// ����״̬
	TC_GRID_OBJ_STATUS_NORMAL = 0,
	// ����
	TC_GRID_OBJ_STATUS_REVIVE = 1,
	// ����
	TC_GRID_OBJ_STATUS_RUINS = 2,
	// ��ӡ
	TC_GRID_OBJ_STATUS_SEAL = 3,
	// ս����
	TC_GRID_OBJ_STATUS_BATTLE = 4,
	// δ����
	TC_GRID_OBJ_STATUS_NO_ACTIVE = 5,
	// ����
	TC_GRID_OBJ_STATUS_DEAD = 6,
	// ս��׼��
	TC_GRID_OBJ_STATUS_PRE_BATTLE = 7,
	// ����
	TC_GRID_OBJ_STATUS_DESTORY = 8,
};

// С��״̬
enum TCGridSquadStatus
{
	// ��ֹ
	TC_GRID_SQUAD_STATUS_NORMAL = 0,
	// Ѱ·
	TC_GRID_SQUAD_STATUS_MOVE = 1,
	// ս��
	TC_GRID_SQUAD_STATUS_BATTLE = 2,
};

// ������������
enum TCGridPropretyType
{
	// cd��ȴʱ��
	TC_GRID_PRO_CD_DOWN_TIME = 1,
	// cd����ʱ���
	TC_GRID_PRO_CD_END_STAMP = 2,
	// ʣ��Ѫ��
	TC_GRID_PRO_ODD_BLOOD = 3,
	// ����cd����ʱ��
	TC_GRID_PRO_REVIVE_CD_STAMP= 4,
	// ��������id
	TC_GRID_PRO_MONSTER_PRO_ID = 5,
	// ����ԭ��
	TC_GRID_PRO_DEAD_REASON = 6,
};

// ��Ϸ����ԭ��
enum TCGameOverReason
{
	// ʤ��
	TC_GAME_OVER_REASON_VICTORY = 1,
	// ʱ�䵽
	TC_GAME_OVER_REASON_TIME_OVER = 2,
	// ��������
	TC_GAME_OVER_REASON_MAIN_CITY_DEAD = 3,
};

// ��������
enum TCGridMonsterType
{
	// ��ʼ����
	TC_GRID_MONSTER_TYPE_INIT = 1,
	// ��Ӫ����Ĺ���
	TC_GRID_MONSTER_TYPE_MAKE = 2,
	// ������ˢ��
	TC_GRID_MONSTER_TYPE_TRANSFER = 3,
};

// �ű�2��������
enum TC2PassType
{
	// ��ͨ
	TC_2_PASS_TYPE_COMMON = 0,
	// ����
	TC_2_PASS_TYPE_WEAKEN = 1,
	// ��ǿ
	TC_2_PASS_TYPE_ENHANCE = 2,
};

// �ű�2�ݵ�����ԭ��
enum TCGridObjDeadReason
{
	// ����
	TC_OBJ_DEAD_REASON_LIVE = 0,
	// С�Ӵݻ�
	TC_OBJ_DEAD_REASON_SQUAD_DESTORY = 1,
	// �ؿ�����
	TC_OBJ_DEAD_REASON_GATE_LINK = 2,
	// ��������
	TC_OBJ_DEAD_REASON_ENTER_MAIN_CITY = 3,
};

// ���ӷ���
enum TCGridDir
{
	// ������Ч
	TC_GRID_DIR_INVALID = 0,
	// ��
	TC_GRID_DIR_LEFT = 1,
	// ��
	TC_GRID_DIR_RIGHT = 2,
	// ����
	TC_GRID_DIR_LEFT_UP = 3,
	// ����
	TC_GRID_DIR_RIGHT_UP = 4,
	// ����
	TC_GRID_DIR_LEFT_DOWN = 5,
	// ����
	TC_GRID_DIR_RIGHT_DOWN = 6,

	TC_GRID_DIR_MAX = 7
};

// rollѡ��
enum TCRollOption
{
	// ����
	TC_ROLL_NORMAL = 0,
	// ǫ��
	TC_ROLL_NO_NEED = 1,
	// �޴���
	TC_ROLL_NO_NUM = 2,
	// ��Ȩ��
	TC_ROLL_NO_POWER = 3,
};

inline bool TCTypeIsValid(UInt32 type)
{
	return type >= TC_TYPE_ONE && type < TC_TYPE_MAX;
}

// �ű������ܼ�ˢ��
inline UInt32 GetTCRefreshWeek(UInt32 tcType)
{
	switch (tcType)
	{
	case TC_TYPE_ONE: return 6;
	case TC_TYPE_TWO: return 2;
	default:break;
	}
	return 6;
}

// �ű�����
inline UInt32 GetTCType(UInt32 sceneId)
{
	switch (sceneId)
	{
	case 6035:
	case 6036:
		return TC_TYPE_ONE;
	case 6046:
	case 6047:
		return TC_TYPE_TWO;
	default:break;
	}

	return 0;
};

// ��ȡ�ű�׼������
inline UInt32 GetTCPrepareScene(UInt32 tcType)
{
	switch (tcType)
	{
	case TC_TYPE_ONE:
		return 6035;
	case TC_TYPE_TWO:
		return 6046;
	default:break;
	}

	return 0;
};

// ��ȡ�ű����᳡��
inline UInt32 GetTCAttackScene(UInt32 tcType)
{
	switch (tcType)
	{
	case TC_TYPE_ONE:
		return 6036;
	case TC_TYPE_TWO:
		return 6047;
	default:break;
	}

	return 0;
};

// �Ƿ���Ч�ĸ��Ӷ�������
inline bool IsValidGridObjType(UInt32 type)
{
	return type > TC_GRID_OBJ_NULL && type <= TC_GRID_OBJ_MONSTER;
}


// �ű�����λ��
struct TCGridPos
{
	TCGridPos() : x(0), y(0){}
	TCGridPos(Int32 _x, Int32 _y) : x(_x), y(_y) {}
	TCGridPos(const TCGridPos& pos) : x(pos.x), y(pos.y) {}

	Int32 x;
	Int32 y;

	bool operator <(const TCGridPos& pos) const
	{
		if (pos.x != x)
		{
			return pos.x < x;
		}

		return pos.y < y;
	}

	bool operator ==(const TCGridPos& pos) const
	{
		return (x == pos.x) && (y == pos.y);
	}

	bool operator != (const TCGridPos& pos) const
	{
		return (x != pos.x) || (y != pos.y);
	}
};

struct TCGridProperty
{
	TCGridProperty() : proType(0), proValue(0) {}
	TCGridProperty(UInt32 type, UInt32 val) : proType(type), proValue(val) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & proType & proValue;
	}

	UInt32 proType;
	UInt32 proValue;
};

struct TCGridObjData
{
	TCGridObjData() : objId(0), gridId(0), gridType(0), gridStatus(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & objId & gridId & gridType & gridStatus & gridPro;
	}

	// ����id
	UInt32 objId;
	// ����id
	UInt32 gridId;
	// ����
	UInt32 gridType;
	// ״̬
	UInt32 gridStatus;
	// ����
	std::vector<TCGridProperty> gridPro;
};

struct TCMonsterPath
{
	TCMonsterPath() : monsterObjId(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & monsterObjId & pathVec;
	}

	UInt32 monsterObjId;
	std::vector<UInt32> pathVec;
};

inline TCGridPos GetGridDirPos(TCGridDir dir, Int32 srcX, Int32 srcY, UInt32 roundNum = 1)
{
	TCGridPos dirPos(srcX, srcY);
	switch (dir)
	{
	case TC_GRID_DIR_LEFT:
	{
		dirPos.x = srcX - roundNum;
	}
	break;
	case TC_GRID_DIR_RIGHT:
	{
		dirPos.x = srcX + roundNum;
	}
	break;
	case TC_GRID_DIR_LEFT_UP:
	{
		dirPos.x = srcX - roundNum;
		dirPos.y = srcY + 1;
	}
	break;
	case TC_GRID_DIR_RIGHT_UP:
	{
		dirPos.y = srcY + roundNum;
	}
	break;
	case TC_GRID_DIR_LEFT_DOWN:
	{
		dirPos.y = srcY - roundNum;
	}
	break;
	case TC_GRID_DIR_RIGHT_DOWN:
	{
		dirPos.x = srcX + roundNum;
		dirPos.y = srcY - roundNum;
	}
	break;
	default:break;
	}

	return dirPos;
}

inline TCGridPos GetGridDirPos(TCGridDir dir, const TCGridPos& pos, UInt32 roundNum = 1)
{
	return GetGridDirPos(dir, pos.x, pos.y, roundNum);
}

struct TCPlayerRoll
{
	TCPlayerRoll() : playerId(0), rollPoint(0), rollOption(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerId & rollPoint & rollOption;
	}

	std::string SerializeStr()
	{
		std::stringstream ss;
		ss << " playerId:" << playerId
		   << ", rollPoint:" << rollPoint
		   << ", rollOption:" << rollOption;
		return ss.str();
	}

	UInt64 playerId;
	// roll����
	UInt32 rollPoint;
	// TCRollOption
	UInt32 rollOption;
};

#endif