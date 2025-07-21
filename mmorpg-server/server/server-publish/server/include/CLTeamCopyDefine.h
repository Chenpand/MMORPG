#ifndef _CL_TEAM_COPY_DEFINE_H_
#define _CL_TEAM_COPY_DEFINE_H_

#include <set>
#include <AvalonObjectPool.h>
#include <AvalonNetStream.h>
#include <CLDefine.h>
#include <CLTeamCopyValueDataEntry.h>

struct TeamCopyTargetCfg;
class TeamCopyFieldDataEntry;

// 入队时间限制(分钟)
#define TEAM_JOIN_TIME_LIMIT (60)

// 查询玩家作战信息超时时间
#define QUERY_DUNGEON_PLAYER_TIME_OUT  (5)

// 查找格子对多的圈数
#define SEARCH_GRID_MAX_ROUND  (2)

// 队伍ID偏移
#define TEAM_ID_OFFSET (1000)

// 获得队伍小队唯一ID
#define GET_TEAM_SQUAD_ID(TEAMID, SQUADID) \
	(TEAMID * TEAM_ID_OFFSET + SQUADID)

// 获得队伍ID根据唯一ID
#define GET_TEAM_ID(ID) \
	(ID / TEAM_ID_OFFSET)

// 获得小队ID根据唯一ID
#define GET_SQUAD_ID(ID) \
	(ID % TEAM_ID_OFFSET)

enum TeamCopyTeamModel
{
	TEAM_COPY_TEAM_MODEL_DEFAULT = 0,	// 默认全部模式
	TEAM_COPY_TEAM_MODEL_CHALLENGE = 1,	// 挑战模式
	TEAM_COPY_TEAM_MODEL_GOLD = 2,		// 金团模式
};

enum TeamCopyTeamStatus
{
	TEAM_COPY_TEAM_STATUS_PARPARE = 0,	// 备战
	TEAM_COPY_TEAM_STATUS_BATTLE = 1,	// 战斗
	TEAM_COPY_TEAM_STATUS_VICTORY = 2,	// 胜利
	TEAM_COPY_TEAM_STATUS_FAILED = 3,	// 失败
};

enum TeamCopySquadStatus
{
	TEAM_COPY_SQUAD_STATUS_INIT = 0,	// 待命中
	TEAM_COPY_SQUAD_STATUS_PREPARE = 1,	// 备战中
	TEAM_COPY_SQUAD_STATUS_BATTLE = 2,	// 战斗中
};

enum TeamCopyPost
{
	TEAM_COPY_POST_NORMAL = 1 << 0,		// 队员
	TEAM_COPY_POST_GOLD = 1 << 1,		// 金主
	TEAM_COPY_POST_CAPTAIN = 1 << 2,	// 队长
	TEAM_COPY_POST_CHIEF = 1 << 3,		// 团长
};

enum TeamCopyStage
{
	TEAM_COPY_STAGE_NULL = 0,	// 空阶段
	TEAM_COPY_STAGE_ONE = 1,	// 第一阶段
	TEAM_COPY_STAGE_TWO = 2,	// 第二阶段
	TEAM_COPY_STAGE_FINAL = 3,	// 最终阶段
};

enum TeamCopyTargetType
{
	TEAM_COPY_TARGET_TYPE_TEAM = 1,	 // 团队目标
	TEAM_COPY_TARGET_TYPE_SQUAD = 2, // 小队目标
};

enum TeamCopyGrade
{
	TEAM_COPY_GRADE_TEAM = 0,	// 团队难度
	TEAM_COPY_GRADE_A = 1,	// A难度
	TEAM_COPY_GRADE_B = 2,	// B难度
	TEAM_COPY_GRADE_C = 3,	// C难度
	TEAM_COPY_GRADE_D = 4,	// D难度
};

enum TeamCopyPlanModel
{
	TEAM_COPY_PLAN_MODEL_INVALID = 0, // 无效
	TEAM_COPY_PLAN_MODEL_FREE = 1,	// 自由模式
	TEAM_COPY_PLAN_MODEL_GUIDE = 2,	// 引导模式
};

enum TeamCopyAppear
{
	TEAM_COPY_APPEAR_START = 1,  // 开始出现
	TEAM_COPY_APPEAR_UNLOCK = 2, // 解锁出现
	TEAM_COPY_APPEAR_BOSS = 3, // boss据点
	TEAM_COPY_APPEAR_BOSS_ATTACH = 4, // boss附属据点
};

enum TeamCopyFieldStatus
{
	TEAM_COPY_FIELD_STATUS_INVALID = 0, // 无效状态
	TEAM_COPY_FIELD_STATUS_INIT = 1,	// 初始状态
	TEAM_COPY_FIELD_STATUS_REBORN = 2,	// 重生
	TEAM_COPY_FIELD_STATUS_DEFEAT= 3,	// 歼灭
	TEAM_COPY_FIELD_STATUS_URGENT = 4,	// 紧急
	TEAM_COPY_FIELD_STATUS_UNLOCKING = 5,// 解锁中
	TEAM_COPY_FIELD_STATUS_ENERGY_REVIVE = 6,// 能量恢复中
};

enum TeamCopyCheckCode
{
	TEAM_COPY_CHECK_CODE_SUCCESS = 0,	// 成功
	TEAM_COPY_CHECK_CODE_COUNT = 1,		// 次数不足
	TEAM_COPY_CHECK_CODE_TICKET = 2,	// 门票不足
	TEAM_COPY_CHECK_CODE_COMMISSION = 3,// 佣金不足
	TEAM_COPY_CHECK_CODE_UNKNOWN = 4,	// 未知错误
};

// 翻牌限制类型
enum TeamCopyFlopLimit
{
	TEAM_COPY_FLOP_LIMIT_NULL = 0,	// 不限制
	TEAM_COPY_FLOP_LIMIT_DAY = 1,	// 日限制
	TEAM_COPY_FLOP_LIMIT_WEEK = 2,	// 周限制
	TEAM_COPY_FLOP_LIMIT_PASS_GATE = 3,	// 通关限制
};

// 团本消耗类型
enum TeamCopyConsumeType
{
	TEAM_COPY_CONSUME_TYPE_TICKET = 0,	// 门票
	TEAM_COPY_CONSUME_TYPE_COMMISSION = 1,	// 佣金
};

// 团本难度
enum TeamCopyTeamGrade
{
	TEAM_COPY_TEAM_GRADE_COMMON = 1,	// 普通难度
	TEAM_COPY_TEAM_GRADE_DIFF = 2,	// 困难难度
	TEAM_COPY_TEAM_GRADE_NUM = 3,
};

// 团本局跨日期类型
enum TeamCopyCrossDateType
{
	TEAM_COPY_CROSS_NULL = 0,
	TEAM_COPY_CROSS_WEEK = 1,	// 跨周
	TEAM_COPY_CROSS_DAY = 2,	// 跨天
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
*@brief 团本成员
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

	// 账号ID
	UInt32 accId;
	// 玩家ID
	UInt64 playerId;
	// 玩家名字
	std::string playerName;
	// 玩家职业
	UInt32 playerOccu;
	// 觉醒
	UInt32 playerAwaken;
	// 等级
	UInt32 playerLevel;
	// 职位
	UInt32 post;
	// 装备评分
	UInt32 equipScore;
	// 座位ID
	UInt32 seatId;
	// 门票是否足够
	UInt32 ticketIsEnough;
	// zoneId
	UInt32 zoneId;
	// 连接ID
	UInt32 connId;
	// 过期时间
	UInt64 expireTime;
	// 换小队次数
	UInt32 changeSquadCnt;
	// 消耗(key->guid)
	std::map<UInt64, TCConsumeItem> consumeMap;
};

struct TeamCopyTargetDetail
{
	TeamCopyTargetDetail() : fieldId(0), curNum(0), totalNum(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & fieldId & curNum & totalNum;
	}

	// 据点id
	UInt32 fieldId;
	// 当前次数
	UInt32 curNum;
	// 总次数
	UInt32 totalNum;
}; 

// 目标数据
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

	// 目标Id
	UInt32 targetId;
	// 配置
	const TeamCopyTargetCfg* config;
	// 目标详情
	std::vector<TeamCopyTargetDetail> targetDetailVec;
};

// 据点
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

	// 据点id
	UInt32 fieldId;
	// 剩余次数
	UInt32 oddNum;
	// 状态
	UInt32 state;
	// 重生时间点
	UInt32 rebornTime;
	// 能量恢复时间点
	UInt32 energyReviveTime;
	// 攻打小队
	std::vector<UInt32> attackSquadList;
	// 据点比赛
	std::set<UInt64> fieldRaceList;
	// 配置
	const TeamCopyFieldDataEntry* config;
};

/**
*@brief 小队数据
*/
struct TeamCopySquad
{
	TeamCopySquad() : squadId(0), squadStatus(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & squadId & squadStatus & teamMemberList;
	}

	// 小队ID
	UInt32 squadId;
	// 小队状态
	UInt32 squadStatus;
	// 小队目标
	TeamCopyTarget squadTarget;
	// 成员列表
	std::vector<TeamCopyMember> teamMemberList;

	// 分配一个座位
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

// 战前配置
struct TeamCopyBattlePlan
{
	TeamCopyBattlePlan() : grade(0), squadId(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & grade & squadId;
	}

	// 难度
	UInt32 grade;
	// 小队id
	UInt32 squadId;
};

// 翻牌奖励
struct TeamCopyFlop
{
	TeamCopyFlop() : playerId(0), rewardId(0), rewardNum(0), number(0), goldFlop(0), isLimit(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerName & playerId & rewardId & rewardNum & number & goldFlop & isLimit;
	}

	// 玩家name
	std::string playerName;
	// 玩家Id
	UInt64 playerId;
	// 奖励id
	UInt32 rewardId;
	// 奖励数量
	UInt32 rewardNum;
	// 序号
	UInt32 number;
	// 是否金牌
	UInt32 goldFlop;
	// 是否限制(0:不限制，1:日限制，2:周限制)
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

	// 玩家ID
	UInt64 playerId;
	// 玩家名字
	std::string playerName;
	// 玩家职业
	UInt32 playerOccu;
	// 觉醒
	UInt32 playerAwaken;
	// 等级
	UInt32 playerLevel;
	// 装备评分
	UInt32 equipScore;
	// 是否金主(非0是金主)
	UInt32 isGold;
	// 公会id
	UInt64 guildId;
	// 区服id
	UInt32 zoneId;
};

struct TeamCopyCommission
{
	TeamCopyCommission() : connId(0), frozenOrder(0) {}

	// 连接id，当玩家离开时，通过连接发回本服
	UInt32 connId;
	// 冻结订单号
	UInt64 frozenOrder;
};

// 邀请信息
struct TeamCopyInviteInfo
{
	TeamCopyInviteInfo() : teamId(0), teamType(0), teamModel(0), teamGrade(0), occu(0), awaken(0), level(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & teamId & teamType & teamModel & teamGrade & name & occu & awaken & level;
	}

	// 队伍ID
	UInt32 teamId;
	// 队伍类型
	UInt32 teamType;
	// 队伍模式
	UInt32 teamModel;
	// 队伍难度
	UInt32 teamGrade;
	// 名字
	std::string name;
	// 职业
	UInt32 occu;
	// 觉醒
	UInt32 awaken;
	// 等级
	UInt32 level;
};

// boss衍生据点解锁信息
struct TeamCopyBossPhase
{
	TeamCopyBossPhase() { Clear(); }
	// 阶段
	UInt32 phase;
	// 解锁玩家
	UInt64 roleId;
	// 据点id
	UInt32 fieldId;
	// 解锁帧
	UInt32 unlockFrame;
	// boss血量比例
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

//**************************** 团本2 ****************************

// 团本类型
enum TeamCopyType
{
	// 克拉西
	TC_TYPE_ONE = 1,
	// 群风岛
	TC_TYPE_TWO = 2,

	TC_TYPE_MAX,
};

// 格子对象类型
enum TCGridObjType
{
	// 空格子
	TC_GRID_OBJ_NULL = 0,
	// 传送门
	TC_GRID_OBJ_TRANSFER = 1,
	// 前哨战（固定点）
	TC_GRID_OBJ_FIX_POINT = 2,
	// 能量基站
	TC_GRID_OBJ_ENERGY_POINT = 3,
	// 灵魂祭坛
	TC_GRID_OBJ_ALTAR = 4,
	// 恐怖的灵魂祭坛
	TC_GRID_OBJ_TERROR_ALTAR = 5,
	// boss
	TC_GRID_OBJ_BOSS = 6,
	// 出生点
	TC_GRID_OBJ_BORN_POINT = 7,
	// 主城
	TC_GRID_OBJ_MAIN_CITY = 8,
	// 物资补给（复活次数）
	TC_GRID_OBJ_REVIVE_CNT = 9,
	// 机动补给（CD缩短）
	TC_GRID_OBJ_CD_DOWN = 10,
	// 实验室(制造怪物)
	TC_GRID_OBJ_LABORATORY = 11,
	// 小队
	TC_GRID_OBJ_SQUAD = 12,
	// 怪物
	TC_GRID_OBJ_MONSTER = 13,
	// 怪物传送门
	TC_GRID_OBJ_MONSTER_TRANSFER = 14,
};

// 格子据点状态
enum TCGridObjStatus
{
	// 正常状态
	TC_GRID_OBJ_STATUS_NORMAL = 0,
	// 复活
	TC_GRID_OBJ_STATUS_REVIVE = 1,
	// 废墟
	TC_GRID_OBJ_STATUS_RUINS = 2,
	// 封印
	TC_GRID_OBJ_STATUS_SEAL = 3,
	// 战斗中
	TC_GRID_OBJ_STATUS_BATTLE = 4,
	// 未激活
	TC_GRID_OBJ_STATUS_NO_ACTIVE = 5,
	// 死亡
	TC_GRID_OBJ_STATUS_DEAD = 6,
	// 战斗准备
	TC_GRID_OBJ_STATUS_PRE_BATTLE = 7,
	// 销毁
	TC_GRID_OBJ_STATUS_DESTORY = 8,
};

// 小队状态
enum TCGridSquadStatus
{
	// 静止
	TC_GRID_SQUAD_STATUS_NORMAL = 0,
	// 寻路
	TC_GRID_SQUAD_STATUS_MOVE = 1,
	// 战斗
	TC_GRID_SQUAD_STATUS_BATTLE = 2,
};

// 格子属性类型
enum TCGridPropretyType
{
	// cd冷却时间
	TC_GRID_PRO_CD_DOWN_TIME = 1,
	// cd结束时间戳
	TC_GRID_PRO_CD_END_STAMP = 2,
	// 剩余血量
	TC_GRID_PRO_ODD_BLOOD = 3,
	// 复活cd结束时间
	TC_GRID_PRO_REVIVE_CD_STAMP= 4,
	// 怪物属性id
	TC_GRID_PRO_MONSTER_PRO_ID = 5,
	// 死亡原因
	TC_GRID_PRO_DEAD_REASON = 6,
};

// 游戏结束原因
enum TCGameOverReason
{
	// 胜利
	TC_GAME_OVER_REASON_VICTORY = 1,
	// 时间到
	TC_GAME_OVER_REASON_TIME_OVER = 2,
	// 主城死亡
	TC_GAME_OVER_REASON_MAIN_CITY_DEAD = 3,
};

// 怪物类型
enum TCGridMonsterType
{
	// 初始怪物
	TC_GRID_MONSTER_TYPE_INIT = 1,
	// 兵营制造的怪物
	TC_GRID_MONSTER_TYPE_MAKE = 2,
	// 传送门刷出
	TC_GRID_MONSTER_TYPE_TRANSFER = 3,
};

// 团本2翻牌类型
enum TC2PassType
{
	// 普通
	TC_2_PASS_TYPE_COMMON = 0,
	// 减弱
	TC_2_PASS_TYPE_WEAKEN = 1,
	// 加强
	TC_2_PASS_TYPE_ENHANCE = 2,
};

// 团本2据点死亡原因
enum TCGridObjDeadReason
{
	// 活着
	TC_OBJ_DEAD_REASON_LIVE = 0,
	// 小队摧毁
	TC_OBJ_DEAD_REASON_SQUAD_DESTORY = 1,
	// 关卡联动
	TC_OBJ_DEAD_REASON_GATE_LINK = 2,
	// 进入主城
	TC_OBJ_DEAD_REASON_ENTER_MAIN_CITY = 3,
};

// 格子方向
enum TCGridDir
{
	// 方向无效
	TC_GRID_DIR_INVALID = 0,
	// 左
	TC_GRID_DIR_LEFT = 1,
	// 右
	TC_GRID_DIR_RIGHT = 2,
	// 左上
	TC_GRID_DIR_LEFT_UP = 3,
	// 右上
	TC_GRID_DIR_RIGHT_UP = 4,
	// 左下
	TC_GRID_DIR_LEFT_DOWN = 5,
	// 右下
	TC_GRID_DIR_RIGHT_DOWN = 6,

	TC_GRID_DIR_MAX = 7
};

// roll选项
enum TCRollOption
{
	// 正常
	TC_ROLL_NORMAL = 0,
	// 谦让
	TC_ROLL_NO_NEED = 1,
	// 无次数
	TC_ROLL_NO_NUM = 2,
	// 无权限
	TC_ROLL_NO_POWER = 3,
};

inline bool TCTypeIsValid(UInt32 type)
{
	return type >= TC_TYPE_ONE && type < TC_TYPE_MAX;
}

// 团本数据周几刷新
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

// 团本场景
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

// 获取团本准备场景
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

// 获取团本攻坚场景
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

// 是否有效的格子对象类型
inline bool IsValidGridObjType(UInt32 type)
{
	return type > TC_GRID_OBJ_NULL && type <= TC_GRID_OBJ_MONSTER;
}


// 团本格子位置
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

	// 对象id
	UInt32 objId;
	// 格子id
	UInt32 gridId;
	// 类型
	UInt32 gridType;
	// 状态
	UInt32 gridStatus;
	// 数据
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
	// roll点数
	UInt32 rollPoint;
	// TCRollOption
	UInt32 rollOption;
};

#endif