#ifndef __CL_MATCH_DEFINE_H__
#define __CL_MATCH_DEFINE_H__

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLSkillDefine.h"
#include "CLWarpStoneDefine.h"
#include "CLRetinueDefine.h"
#include "CLDropItemDefine.h"
#include "CLPetDefine.h"
#include "CLEquipSchemeDefine.h"

const UInt32 SEASON_MIN_MATCH_SCORE = 1000;

enum PkType
{
    PK_NORMAL_1V1,  // 传统决斗，1V1
    PK_SEASON_1V1,  // 赛季，1V1
	PK_GUILD_BATTLE,// 公会战
	PK_WUDAODAHUI,	// 武道大会
	PK_PRACTICE,	// 好友切磋
	PK_PREMIUM_LEAGUE_PRELIMINAY,	// 赏金联赛预选赛
	PK_PREMIUM_LEAGUE_BATTLE,		// 赏金联赛对决赛（8强以后）
	PK_3V3_ROOM,					// 3v3房间
	PK_3V3_MATCH,					// 3v3匹配
	PK_3V3_ACTIVITY,				// 3v3活动
	PK_3V3_TUMBLE,					// 3v3乱斗
	PK_1V1_CHIJI,					// 1v1吃鸡
	PK_3V3_CHIJI,					// 3v3吃鸡
	PK_EQUAL_1V1,                   // 公平竞技场
	PK_EQUAL_PRACTICE,              // 公平竞技场好友切磋
	PK_2V2_ACTIVITY,                // 2V2乱斗活动
	PK_LOST_DUNGEON,				// 迷失地牢PK
	PK_CHAMPION_BATTLE,				// 冠军赛
	PK_5V5_MATCH,					// 5V5匹配
    PK_NUM,
};

/**
*@brief 决斗信息属性
*/
enum PkStatisticAttr
{
    PKIA_INVALID,
    PKIA_TYPE,		        // 类型	UInt8
    PKIA_TOTAL_WIN_NUM,     // 总胜场数 UInt32
    PKIA_TOTAL_LOSE_NUM,    // 总负场数 UInt32
    PKIA_TOTAL_NUM,         // 总场数 UInt32
    PKIA_DETAIL_RECORD_INFO,// 对战不同职业的战绩
    PKIA_RECENT_RECORD,     // 最近战绩
    PKIA_MAX_WIN_STEAK,     // 最多连胜
    PKIA_CUR_WIN_STEAK,     // 当前连胜

    PKIA_NUM,
};

// PK结果
enum PkRaceResult
{
    PKRR_INVALID,       // 无效
    PKRR_WIN,           // 胜利
    PKRR_LOSE,          // 失败
    PKRR_DOGFALL,       // 平局
    PKRR_NUM
};

// 比赛类型
enum RaceType
{
	RT_DUNGEON,
	RT_PK,
	RT_GUILD_BATTLE,
	// 赏金联赛预选赛
	RT_PREMIUM_LEAGUE_PRELIMINAY,
	// 赏金联赛淘汰赛
	RT_PREMIUM_LEAGUE_BATTLE,
	// 3V3
	RT_PK_3V3,
	// 积分赛
	RT_SCORE_WAR,
	// 3V3乱斗
	RT_3V3_TUMBLE,
	// 吃鸡
	RT_CHIJI,
	// 跨服地下城
	RT_EQULA_PK,
	// 跨服地下城
	RT_DUNGEON_CROSS,
	// 2V2乱斗活动
	RT_2V2_TUMNLE,
	// 迷失地牢pk
	RT_LOST_DUNGEON_PK,
	// 冠军赛
	RT_CHAMPION = 13,
	// 观战用比赛
	RT_OBSERVE = 14,
	// 5V5
	RT_PK_5V5,
};

// 好友状态
enum FriendMatchStatus
{
	// 空闲
	FRIEND_MATCH_STATUS_IDLE,
	// 忙碌
	FRIEND_MATCH_STATUS_BUSY,
	// 下线
	FRIEND_MATCH_STATUS_OFFLINE,
};

// 好友状态信息
struct FriendMatchStatusInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & status;
	}
	ObjID_t		roleId;
	// 状态，对应枚举（FriendMatchStatus）
	UInt8		status;
};

// 匹配服务器查询的类型
enum MatchQueryType
{
	// 竞技
	MATCH_QUERY_RACE_INFO,
	// 帮会战
	MATCH_QUERY_GUILD_RACE_INFO,
	// 赏金联赛预选赛
	MATCH_PREMIUM_LEAGUE_PRELIMINAY_INFO,
	// 地下城
	MATCH_QUERY_DUNGEON_INFO,
	//公平竞技场
	MATCH_QUERY_EQUAL_RACE_INFO,
};

// 匹配服务器查询玩家信息返回
enum MatchQueryPlayerInfoResult
{
	MATCH_QUERY_SUCCESS,
	MATCH_QUERY_PLAYER_OFFLINE,
};

// 机器人AI难度
enum RobotAIHardType
{
	ROBOT_AI_INVALID,	// 无效值
	ROBOT_AI_FISH,		// 新手
	ROBOT_AI_EASY,		// 简单
	ROBOT_AI_NORMAL,	// 普通
	ROBOT_AI_HARD,		// 困难
	ROBOT_AI_MASTER,	// 大师
	ROBOT_AI_GUILD_BATTLE,	// 公会战
};

// 比赛中的技能信息
struct RaceSkillInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & level & slot;
	}

	// 技能ID
	UInt16      id;
	// 技能等级
	UInt8       level;
	// 槽位
	UInt8       slot;
};

struct MatchPlayerBaseInfo
{
	MatchPlayerBaseInfo() : id(0), accid(0), serverCreateTime(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & accid & name & serverCreateTime;
	}

	ObjID_t         id;
	UInt32          accid;
	std::string     name;
	UInt32			serverCreateTime;
};

struct PkRoomMatchPlayerInfo
{
	PkRoomMatchPlayerInfo()
	{
		//memset(this, 0, sizeof(*this));
		id = 0;
		wsId = 0;
		accid = 0;
		score = 0;
		level = 0;
		winStreak = 0;
		loseStreak = 0;
		seasonLevel = 0;
		seasonStar = 0;
		seasonExp = 0;
		seat = 0;
		serverCreateTime = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & wsId & accid & name & score & level & winStreak & loseStreak & seasonLevel & seasonStar & seasonExp & seat & serverCreateTime;
	}
	ObjID_t         id;
	UInt32			wsId;
	UInt32          accid;
	std::string     name;
	UInt32			score;
	UInt16			level;
	// 连胜数
	UInt32			winStreak;
	// 连败数
	UInt32			loseStreak;
	// 段位
	UInt32			seasonLevel;
	// 赛季星星
	UInt32			seasonStar;
	// 赛季经验
	UInt32			seasonExp;
	// 座位号
	UInt8			seat;
	// 开服时间
	UInt32			serverCreateTime;
};

// 比赛中的玩家信息
struct RacePlayerInfo
{
	RacePlayerInfo() : zoneId(0), robotAIType(0), robotHard(0), roleId(0), accid(0), occu(0), level(0), pkValue(0), pos(0), remainHp(HP_MP_FULL_VALUE), remainMp(HP_MP_FULL_VALUE), seasonLevel(0), seasonStar(0), seasonAttr(0), monthcard(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & zoneId & serverName & robotAIType & robotHard & roleId & accid & name & guildName & occu & level & pkValue & matchScore & pos & remainHp & remainMp & seasonLevel & seasonStar & seasonAttr & monthcard & skills &
			equips & raceItems & buffs & warpStones & retinues & pets & potionPos & secondWeapons & avatar & playerLabelInfo & equipScheme & wearingEqIds
			& lostDungArtifacts;
	}
	// 服务器ID
	UInt32 zoneId;
	// 服务器名
	std::string serverName;
	// RobotAI难度（对应枚举RobotAIHardType）,如果是无效值就说明不是机器人
	UInt8			robotAIType;
	// 第几次打(0代表满血，这个字段用在机器人身上，代表是机器人难度（血量不同）)
	UInt8			robotHard;
	//角色id
	ObjID_t			roleId;
	//账号id
	UInt32          accid;
	//名字
	std::string		name;
	//公会名
	std::string		guildName;
	//职业
	UInt8			occu;
	//等级
	UInt16			level;
	//PK值
	UInt32          pkValue;
	//匹配积分
	UInt32			matchScore;
	//位置
	UInt8			pos;
	// 剩余血量
	UInt32			remainHp;
	// 剩余蓝量
	UInt32			remainMp;
	// 赛季段位
	UInt32			seasonLevel;
	// 赛季星星
	UInt32			seasonStar;
	// 赛季属性
	UInt8			seasonAttr;
	// 是不是月卡用户
	UInt8			monthcard;

	// 技能列表
	std::vector<RaceSkillInfo>  skills;

	// 装备列表
	std::vector<RaceEquip>      equips;

	// 单局道具
	std::vector<RaceItem>		raceItems;
	// buff
	std::vector<RaceBuffInfo>	buffs;

	//次元石
	std::vector<RaceWarpStone>	warpStones;
	//随从
	std::vector<RaceRetinue>	retinues;
	//宠物
	std::vector<RacePet>		pets;
	//药品配置
	std::vector<UInt32>			potionPos;
	//武器栏武器
	std::vector<RaceEquip>		secondWeapons;
	//外观
	PlayerAvatar				avatar;
	//玩家标签信息
	PlayerLabelInfo				playerLabelInfo;
	//装备方案信息
	std::vector<RaceEquipScheme>	equipScheme;
	//当前穿戴装备,时装id
	std::vector<UInt64>			wearingEqIds;
	// 地牢神器
	std::vector<RaceItem>		lostDungArtifacts;
};

// 地下城玩家信息
struct DungeonRacePlayerInfo
{
	DungeonRacePlayerInfo()
	{
		accId = 0;
		usedTime = 0;
		remainHp = 0;
		remainMp = 0;
		freeGoldChest = 0;
		isAssist = 0;
		addedMonsterDropNum = 0;
		soureSceneId = 0;
		vipLvl = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & accId & zoneId & raceInfo & taskList & addition & dailyCount & totalCount & freeGoldChest & addedMonsterDropNum
			& usedTime & remainHp & remainMp & isAssist & soureSceneId & vipLvl;
	}

	// accid
	UInt32						accId;
	// zoneid
	UInt32						zoneId;
	// 战斗信息
	RacePlayerInfo				raceInfo;
	// 当前任务列表
	std::vector<UInt32>			taskList;
	// 各种加成
	DungeonAddition				addition;
	// 今日第几次打
	UInt32						dailyCount;
	// 已经打过的次数
	UInt32						totalCount;
	// 能否免费打开黄金宝箱
	UInt8						freeGoldChest;
	// 额外添加的怪物掉落次数
	UInt32						addedMonsterDropNum;

	// 继续上一次的进度时需要的数据
	// 已经用了的时间
	UInt32						usedTime;
	// 剩余血量
	UInt32						remainHp;
	// 剩余蓝量
	UInt32						remainMp;
	// 是否是助战
	UInt8						isAssist;
	// 玩家源sceneId
	UInt32						soureSceneId;
	// 玩家vip
	UInt8						vipLvl;
};

#endif
