#ifndef _CL_GUILD_DEFINE_H_
#define _CL_GUILD_DEFINE_H_

#include <CLDefine.h>
#include <AvalonNetStream.h>
#include <CLGameDefine.h>
#include <CLSortListDefine.h>
#include <CLItemDefine.h>

#define GUILD_BATTLE_REWARD_SIZE 64

#define GUILD_BATTLE_ACTIVITY_NAME "GuildBattle"
#define GUILD_BATTLE_ENROLL_ACTIVITY_NAME "GuildBattleEnroll"
#define GUILD_BATTLE_STATUS_KEY "guild_battle_status"
#define GUILD_BATTLE_CHALLENGE_PRICE "guild_battle_challenge_price"
#define GUILD_BATTLE_CHALLENGE_PLAYER "guild_battle_challenge_player"

//公会战抽奖几率最大最小值
const static UInt8 GUILD_BATTLE_REWARD_PROBABILITY_MIN = 0;
const static UInt8 GUILD_BATTLE_REWARD_PROBABILITY_MAX = 100;

//主动退出公会下次加入公会的冷却时间
const static UInt32	GUILD_ENTER_COLDDOWN_LEAVE = 2 * 60 * 60;
//被踢出公会下次加入公会的冷却时间
const static UInt32	GUILD_ENTER_COLDDOWN_KICK = 2 * 60 * 60;
//公会兑换CD
const static UInt32	GUILD_EXCHANGE_COLDDOWN = 30 * 60;
//公会解散等待时间
const static UInt32	GUILD_DISSMISS_WAIT_TIME = 24 * 60 * 60;
//公会名长度(宽度)
const static UInt32 GUILD_NAME_MAX_WIDTH = 14;
//公会公告长度(宽度)
const static UInt32 GUILD_ANNOUNCEMENT_MAX_WIDTH = 200;
//公会宣言长度(宽度)
const static UInt32 GUILD_DECLARATION_MAX_WIDTH = 100;
//公会邮件长度(宽度)
const static UInt32 GUILD_MAIL_MAX_WIDTH = 200;
// 公会拍卖货币
const static UInt32 GUILD_AUCTION_COIN = 600000002;


//公会事件: 装备强化成功等级 >= 12
const static UInt32  GUILD_EVENT_STRENGTH_LEVEL_SUCCESS = 12;
//公会事件: 装备强化失败等级 >= 13
const static UInt32  GUILD_EVENT_STRENGTH_LEVEL_FAIL = 13;

//公会事件: 装备激化成功等级 >= 12
const static UInt32  GUILD_EVENT_ENHANCE_LEVEL_SUCCESS = 12;
//公会事件: 装备激化失败等级 >= 13
const static UInt32  GUILD_EVENT_ENHANCE_LEVEL_FAIL = 12;

//公会事件: 荣誉等级发送等级
const static UInt32  GUILD_EVENT_HONOR_LEVEL = 9;

/**
*@brief 公会属性
*/
enum GuildAttr
{
	GA_INVALID,				// 无效属性
	GA_NAME,				// 名字	string
	GA_LEVEL,				// 等级	UInt8
	GA_DECLARATION,			// 宣言 string
	GA_FUND,				// 部落资金 UInt32
	GA_ANNOUNCEMENT,		// 公告 string
	GA_BUILDING,			// 公会建筑 GuildBuilding
	GA_DISMISS_TIME,		// 解散时间 UInt32
	GA_MEMBER_NUM,			// 成员数量 UInt16
	GA_LEADER_NAME,			// 会长名字 string
	GA_ENROLL_TERRID,		// 报名领地ID UInt8
	GA_BATTLE_SCORE,		// 公会战分数 UInt32
	GA_OCCUPY_TERRID,		// 公会占领领地 UInt8
	GA_INSPIRE,				// 公会战鼓舞次数 UInt8
	GA_WIN_PROBABILITY,		// 公会战胜利抽奖几率 UInt8
	GA_LOSE_PROBABILITY,	// 公会战失败抽奖几率 UInt8
	GA_STORAGE_ADD_POST,	// 公会战仓库放入物品 UInt8
	GA_STORAGE_DEL_POST,	// 公会战仓库删除物品 UInt8
	GA_OCCUPY_CROSS_TERRID,	// 公会占领跨服领地 UInt8
	GA_HISTORY_TERRID,		// 工会历史占领领地 UInt8
	GA_JOIN_LEVEL,			// 加入公会等级 UInt32
	GA_DUNGEON_TYPE,		// 公会副本难度 UInt32
	GA_LAST_WEEK_ADD_FUND,  // 上周增加的繁荣度 UInt32
	GA_THIS_WEEK_ADD_FUND,  // 本周增加的繁荣度 UInt32
};

// 公会职务
enum GuildPost
{
	GUILD_POST_INVALID = 0,
	// 成员
	GUILD_MEMBER_BEGIN = 0,	
	GUILD_POST_NORMAL = 1,		// 普通成员
	GUILD_POST_ELITE = 2,		// 精英
	GUILD_MEMBER_END,
	// 管理层
	GUILD_MANAGER_BEGIN = 10,			
	GUILD_POST_ELDER = 11,		// 长老
	GUILD_POST_ASSISTANT = 12,	// 副会长
	GUILD_POST_LEADER = 13,		// 会长
	GUILD_MANAGER_END,

};

/**
*@brief 公会权限掩码
*/
enum GuildPowerFlag
{
	GUILD_POWER_SETMEMBER					= 1 << 0,		// 审批成员
	GUILD_POWER_SETPOST						= 1 << 1,		// 设置职位
	GUILD_POWER_KICK						= 1 << 2,		// 开除成员
	GUILD_POWER_CHANGE_DECLAR				= 1 << 3,		// 修改宣言
	GUILD_POWER_UPDATE_BUILDING				= 1 << 4,		// 升级建筑
	GUILD_POWER_CHANGE_NAME					= 1 << 5,		// 修改公会名
	GUILD_POWER_CHANGE_ANNOUNCE				= 1 << 6,		// 修改公会公告
	GUILD_POWER_MAIL						= 1 << 7,		// 公会邮件
	GUILD_POWER_DISMISS						= 1 << 8,		// 解散帮会
	GUILD_SET_JOIN_LEVEL					= 1 << 9,		// 设置加入等级
	GUILD_SET_DUNGEON_BOSS_TYPE				= 1 << 10,		// 设置公会副本难度

	// 不同职务的权限
	GUILD_POWER_NORMAL	= 0,		// 普通成员
	GUILD_POWER_ELITE = 0,			// 精英
	GUILD_POWER_ELDER = GUILD_POWER_SETMEMBER | GUILD_POWER_KICK | GUILD_POWER_MAIL | GUILD_SET_JOIN_LEVEL,
	// 副会长
	GUILD_POWER_ASSISTANT = GUILD_POWER_ELDER | GUILD_POWER_CHANGE_DECLAR | GUILD_POWER_SETPOST | GUILD_POWER_CHANGE_NAME | GUILD_POWER_CHANGE_ANNOUNCE | GUILD_POWER_MAIL | GUILD_SET_JOIN_LEVEL | GUILD_SET_DUNGEON_BOSS_TYPE,
	// 会长
	GUILD_POWER_LEADER = GUILD_POWER_ASSISTANT | GUILD_POWER_DISMISS | GUILD_POWER_UPDATE_BUILDING | GUILD_SET_JOIN_LEVEL | GUILD_SET_DUNGEON_BOSS_TYPE,
};

//公会仓库设置类型
enum GuildStorageSetting
{
	GUILD_STORAGE_SETTING_INVALID = 0,
	//胜利抽奖几率
	GSS_WIN_PROBABILITY = 1,
	//失败抽奖几率
	GSS_LOSE_PROBABILITY = 2,
	//仓库增加权限
	GSS_STORAGE_ADD_POST = 3,
	//仓库删除权限
	GSS_STORAGE_DEL_POST = 4,
	GSS_MAX,
};

//公会成员抽奖状态
enum GuildBattleLotteryStatus
{
	// 无效
	GBLS_INVALID = 0,
	// 不能抽奖
	GBLS_NOT = 1,
	// 可以抽奖
	GBLS_CAN = 2,
	// 已经抽奖
	GBLS_FIN = 3,

	GBLS_MAX,
};

/**
*@brief 获取职位的权限
*/
inline UInt32 GetGuildPostPower(GuildPost post)
{
	switch (post)
	{
	case GUILD_POST_INVALID:
	case GUILD_POST_NORMAL:
		return GUILD_POWER_NORMAL;
	case GUILD_POST_ELITE:
		return GUILD_POWER_ELITE;
	case GUILD_POST_ELDER:
		return GUILD_POWER_ELDER;
	case GUILD_POST_ASSISTANT:
		return GUILD_POWER_ASSISTANT;
	case GUILD_POST_LEADER:
		return GUILD_POWER_LEADER;
	default:
		return GUILD_POWER_NORMAL;
	}
	return GUILD_POWER_NORMAL;
}

inline bool IsEnrollGuildBattlePost(GuildPost post)
{
	return (post == GUILD_POST_ASSISTANT) || (post == GUILD_POST_LEADER);
}

inline bool HasGuildPower(GuildPost post, GuildPowerFlag power)
{
	return (GetGuildPostPower(post) & power) != 0;
}

struct GuildEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & memberNum & leaderName & declaration & isRequested 
			& occupyCrossTerrId & occupyTerrId & joinLevel & leaderId;
	}

	// id
	ObjID_t			id;
	// name
	std::string		name;
	// 公会等级
	UInt8			level;
	// 公会人数
	UInt8			memberNum;
	// 会长名字
	std::string		leaderName;
	// 宣言
	std::string		declaration;
	// 是否已经申请
	UInt8			isRequested;
	// 跨服领地
	UInt8			occupyCrossTerrId;
	// 本服领地
	UInt8			occupyTerrId;
	// 入会等级
	UInt32			joinLevel;
	// 会长id
	UInt64			leaderId;
};

// 公会成员
struct GuildMemberEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & post & contribution & logoutTime & activeDegree & vipLevel 
			& seasonLevel  & playerLabelInfo;
	}

	// id
	ObjID_t			id;
	// name
	std::string		name;
	// 等级
	UInt16			level;
	// 职业
	UInt8			occu;
	// 职务(对应枚举GuildPost)
	UInt8			post;
	// 历史贡献
	UInt32			contribution;
	// 离线时间(0代表在线)
	UInt32			logoutTime;
	// 活跃度
	UInt32			activeDegree;
	// vip等级
	UInt8			vipLevel;
	// 玩家段位
	UInt32			seasonLevel;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

struct GuildRequesterInfo
{
public:
	GuildRequesterInfo() : id(0), level(0), occu(0), requestTime(0), seasonLevel(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & vipLevel & requestTime & seasonLevel & playerLabelInfo;
	}
	//id
	ObjID_t		id;
	//名字
	std::string	name;
	//等级
	UInt16		level;
	//职业
	UInt8		occu;
	//vip等级
	UInt8		vipLevel;
	//申请时间
	UInt32		requestTime;
	// 玩家段位
	UInt32		seasonLevel;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};
typedef std::map<ObjID_t, GuildRequesterInfo> MapGuildRequesterInfo;

// 公会战类型
enum GuildBattleType
{
	//无效
	GBT_INVALID,

	//普通
	GBT_NORMAL,
	//宣战
	GBT_CHALLENGE,
	//跨服
	GBT_CROSS,

	GBT_MAX,
};

//工会战领地类型
enum GuildTerritoryType
{
	//无效
	GTT_INVALID,

	//普通
	GTT_NORMAL,
	//跨服
	GTT_CROSS,
	
	GTT_MAX
};

// 公会战状态
enum GuildBattleStatus
{
	//无效
	GBS_INVALID,
	//报名
	GBS_ENROLL,
	//准备
	GBS_PREPARE,
	//战斗
	GBS_BATTLE,
	//领奖
	GBS_REWARD,

	GBS_MAX,
};

// 公会建筑类型
enum GuildBuildingType
{
	GUILD_BUILDING_MAIN = 0,	// 主城
	GUILD_BUILDING_SHOP = 1,	// 商店
	GUILD_BUILDING_TABLE = 2,	// 圆桌会议
	GUILD_BUILDING_DUNGEON = 3,	// 地下城
	GUILD_BUILDING_STATUE = 4,	// 雕像
	GUILD_BUILDING_BATTLE = 5,	// 战争坊
	GUILD_BUILDING_WELFARE = 6,	// 福利社
	GUILD_BUILDING_HONOUR = 7,	// 荣耀圣殿
	GUILD_BUILDING_ALTAR = 8,	// 战争祭坛
	GUILD_BUILDING_HONOR = 9,	// 公会荣誉
	GUILD_BUILDING_NUM,
};

// 公会建筑
struct GuildBuilding
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & level;
	}

	// 建筑类型(对应枚举GuildBuildingType)
	UInt8				type;
	// 等级
	UInt8				level;
};

// 圆桌会议成员信息
struct GuildTableMember
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & level & occu & name & seat & type & playerLabelInfo;
	}
	// 角色ID
	ObjID_t		id;
	// 等级
	UInt16		level;
	// 职业
	UInt8		occu;
	// 名字
	std::string	name;
	// 位置
	UInt8		seat;
	// 参与类型
	UInt8		type;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};


// 公会战成员
struct GuildBattleMember
{
	GuildBattleMember() :id(0), winStreak(0), gotScore(0), totalScore(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & serverName & guildName & winStreak & gotScore & totalScore;
	}

	// ID
	ObjID_t			id;
	// 名字
	std::string		name;
	// 服务器名字
	std::string		serverName;
	// 公会名字
	std::string		guildName;
	// 输:战斗前的连胜数 赢:战斗后连胜
	UInt8			winStreak;
	// 获得积分
	UInt16			gotScore;
	//总积分
	UInt16			totalScore;
};

// 公会战记录
struct GuildBattleRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & winner & loser & time;
	}

	UInt32				index;
	// 胜利者
	GuildBattleMember	winner;
	// 失败者
	GuildBattleMember	loser;
	// 时间
	UInt32				time;
};

//领地信息
struct GuildTerritoryBaseInfo
{
	GuildTerritoryBaseInfo() :terrId(0), enrollSize(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & terrId & serverName & guildName & enrollSize;
	}

	//领地ID
	UInt8 terrId;
	//服务器名称
	std::string serverName;
	//公会名称
	std::string guildName;
	//报名数量
	UInt32 enrollSize;

};

struct GuildBattleBaseInfo
{
	GuildBattleBaseInfo()
	{
		enrollTerrId = 0;
		guildBattleScore = 0;
		occupyTerrId = 0;
		occupyCrossTerrId = 0;
		historyTerrId = 0;
		inspire = 0;
		guildBattleType = (UInt8)GuildBattleType::GBT_INVALID;
		guildBattleStatus = (UInt8)GuildBattleStatus::GBS_INVALID;
		guildBattleStatusEndTime = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & enrollTerrId & guildBattleScore & occupyTerrId & occupyCrossTerrId & historyTerrId & inspire
			& selfGuildBattleRecord & terrInfos & guildBattleType & guildBattleStatus & guildBattleStatusEndTime;
	}

	// 报名领地ID
	UInt8			enrollTerrId;
	// 公会战积分
	UInt32			guildBattleScore;
	// 已经占领的领地ID
	UInt8			occupyTerrId;
	// 已经占领的跨服领地ID
	UInt8			occupyCrossTerrId;
	// 历史占领的领地ID
	UInt8			historyTerrId;
	// 鼓舞次数
	UInt8			inspire;

	//自己的公会战记录
	std::vector<GuildBattleRecord> selfGuildBattleRecord;
	//公会战领地信息
	std::vector<GuildTerritoryBaseInfo> terrInfos;
	//公会战类型
	UInt8 guildBattleType;
	//公会战状态
	UInt8 guildBattleStatus;
	//公会战状态结束时间
	UInt32 guildBattleStatusEndTime;
};

// 公会基础信息
struct GuildBaseInfo
{
	GuildBaseInfo()
	{
		id = 0;
		level = 0;
		fund = 0;
		dismissTime = 0;
		memberNum = 0;
		joinLevel = 0;
		emblemLevel = 0;
		guildDungeonType = 0;
		weekAddedFund = 0;
		lastWeekAddedFund = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & fund & declaration & announcement & dismissTime & memberNum 
			& leaderName & winProbability & loseProbability & storageAddPost & storageDelPost & buildings & hasRequester & tableMembers & guildBattleInfo & joinLevel & emblemLevel & guildDungeonType & weekAddedFund & lastWeekAddedFund;
	}

	// 公会ID
	ObjID_t			id;
	// 公会名
	std::string		name;
	// 公会等级
	UInt8			level;
	// 公会资金
	UInt32			fund;
	// 公会宣言
	std::string		declaration;
	// 公告
	std::string		announcement;
	// 解散时间
	UInt32			dismissTime;
	// 成员数量
	UInt16			memberNum;
	// 会长名字
	std::string		leaderName;
	// 公会战胜利抽奖几率
	UInt8			winProbability;
	// 公会战失败抽奖几率
	UInt8			loseProbability;
	// 公会仓库放入权限
	UInt8			storageAddPost;
	// 公会仓库放入权限
	UInt8			storageDelPost;
	// 建筑信息
	std::vector<GuildBuilding> buildings;
	// 有没有申请加入公会的人
	UInt8			hasRequester;
	// 圆桌会议成员信息
	std::vector<GuildTableMember> tableMembers;
	//公会战相关信息
	GuildBattleBaseInfo guildBattleInfo;
	// 入会限制等级
	UInt32			joinLevel;
	// 徽记
	UInt32			emblemLevel;
	// 公会副本boss难度
	UInt32			guildDungeonType;
	//公会本周增加的繁荣度
	UInt32       weekAddedFund;
	//公会上周增加的繁荣度
	UInt32       lastWeekAddedFund;
};

// 公会操作类型
enum GuildOperation
{
	GUILD_OP_MODIFY_DECLAR,		// 修改公会宣言
	GUILD_OP_MODIFY_NAME,		// 修改公会名
	GUILD_OP_MODIFY_ANNOUNCE,	// 修改公会公告
	GUILD_OP_SEND_MAIL,			// 发送公会邮件
	GUILD_OP_UPGRADE_BUILDING,	// 升级建筑
	GUILD_OP_DONATE,			// 捐献
	GUILD_OP_EXCHANGE,			// 兑换
	GUILD_OP_UPGRADE_SKILL,		// 升级技能
	GUILD_OP_DISMISS,			// 解散工会
	GUILD_OP_CANCEL_DISMISS,	// 取消解散工会
	GUILD_OP_ORZ,				// 膜拜
	GUILD_OP_TABLE,				// 圆桌会议
	GUILD_OP_PAY_REDPACKET,		// 公会自费红包
};

// 捐献
enum GuildDonateType
{
	GUILD_DONATE_GOLD,		// 金币捐献
	GUILD_DONATE_POINT,		// 点劵捐献
	GUILD_DONATE_NUM,
};

enum GuildBattleMemberStatus
{
	GBS_NORMAL,		//正常状态
	GBS_START,		//在匹配或者战斗
};

// 捐献日志
struct GuildDonateLog
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & type & num & contri;
	}
	// id
	ObjID_t		id;
	// 名字
	std::string name;
	// 捐献类型（对应枚举GuildDonateType）
	UInt8		type;
	// 次数
	UInt8		num;
	// 获得贡献
	UInt32		contri;
};

// 公会会长信息
struct GuildLeaderInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu & avatar & popularoty & playerLabelInfo;
	}

	// ID
	ObjID_t			id;
	// 名字
	std::string		name;
	// 职业
	UInt8			occu;
	// 外观
	PlayerAvatar	avatar;
	// 人气
	UInt32			popularoty;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

// 膜拜类型
enum GuildOrzType
{
	// 普通膜拜
	GUILD_ORZ_LOW,
	// 中级膜拜
	GUILD_ORZ_MID,
	// 高级膜拜
	GUILD_ORZ_HIGH,
	GUILD_ROZ_NUM,
};

struct GuildBattleEndInfo
{
	GuildBattleEndInfo() :terrId(0), terrName(""), guildId(0), guildName(""), guildLeaderName(""), serverName(""){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & terrId & terrName & guildId & guildName & guildLeaderName & serverName;
	}

	UInt8 terrId;
	std::string terrName;
	ObjID_t guildId;
	std::string guildName;
	std::string guildLeaderName;
	std::string serverName;
};

//跨服工会战报名信息
struct CSEnrollInfo
{
	CSEnrollInfo() :terrId(0), guildId(0), guildName(""), guildLeaderName(""), serverName(""){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & terrId & guildId & guildName & guildLeaderName & serverName;
	}

	inline bool operator==(const CSEnrollInfo &info)
	{
		return guildId == info.guildId;
	}

	UInt8 terrId;
	ObjID_t guildId;
	std::string guildName;
	std::string guildLeaderName;
	std::string serverName;
};

//公会战鼓舞信息
struct GuildBattleInspireInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerId & playerName & inspireNum;
	}

	//玩家ID
	ObjID_t playerId;
	//玩家名字
	std::string playerName;
	//鼓舞次数
	UInt32 inspireNum;

};

struct GuildBattleTimeInfo
{
	GuildBattleTimeInfo() :type(GuildBattleType::GBT_INVALID), enrollTime(0), preTime(0), battleTime(0), rewardTime(0), endTime(0){}

	GuildBattleType type;
	UInt32 enrollTime;
	UInt32 preTime;
	UInt32 battleTime;
	UInt32 rewardTime;
	UInt32 endTime;
};

struct GuildStorageItemInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & dataId & num & str & equipType;
	}

	void DecodeString(std::istringstream& is)
	{
		char split = 0;
		UInt16 str16;
		is >> dataId >> split >> num >> split >> str16;
		str = (UInt8)str16;
	}
	void EncodeString(std::ostringstream& os, char split = '_') const
	{
		os << dataId << split << num << split << (UInt32)str;
	}

	UInt64	uid;
	UInt32	dataId;
	UInt16	num;
	UInt8	str;
	UInt8   equipType;
};

struct GuildStorageDelItemInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & num;
	}

	UInt64	uid;
	UInt16	num;
};

struct GuildStorageOpRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & name & opType & items & time;
	}

	UInt64				guid;
	std::string			name;
	UInt32				opType;					// GuildStorageOpType
	std::vector<GuildStorageItemInfo>	items;
	UInt32				time;
};


struct TerritorySortListEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sortType & serverName & playerId & playerName & guildId & guildName & score;
	}

	UInt8			sortType;
	std::string		serverName;
	UInt64			playerId;
	std::string		playerName;
	UInt64			guildId;
	std::string		guildName;
	UInt32			score;

	void SerializeForClient(Avalon::NetOutputStream& stream)
	{
		if (IsGuildSortList(sortType))
			stream & guildId & guildName & serverName & score;
		else
			stream & playerId & playerName & serverName & score & guildName;
	}

	UInt64 GetSortFactor() const
	{
		return GetSortFactor(sortType, playerId, score);
	}

	static UInt64 GetSortFactor(UInt8 sortType, UInt64 id, UInt32 score)
	{
		switch ((SortListType)sortType)
		{
		case SORTLIST_GUILD_BATTLE_SCORE:return score;
		case SORTLIST_GUILD_BATTLE_MEMBER:return score;
		case SORTLIST_GUILD_MEMBER_SCORE:return score;
		default: return score;
		}
	}
};

//********************公会地下城*********************

// 掉落的buff信息
struct GuildDungeonBuff
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & buffId & buffLvl;
	}
	// id
	UInt32 buffId;
	// 等级
	UInt32 buffLvl;

	GuildDungeonBuff() : buffId(0), buffLvl(0) {}
};


struct GuildDungeonBattleRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dungeonId & battleCnt & oddBlood & buffVec;
	}

	// 地下城id
	UInt32 dungeonId;
	// 战斗次数
	UInt32 battleCnt;
	// 剩余血量
	UInt64 oddBlood;
	// buff列表
	std::vector<GuildDungeonBuff> buffVec;

	GuildDungeonBattleRecord() : dungeonId(0), battleCnt(0), oddBlood(0){}
};

typedef std::map<UInt32, GuildDungeonBattleRecord*> GuildDungeonBattleMap;

// 伤害
struct GuildDungeonPlayerDamage
{
	// 玩家ID
	UInt64 playerId;
	// 伤害值
	UInt64 damagePoint;
	// 玩家名字
	std::string playerName;

	GuildDungeonPlayerDamage() : playerId(0), damagePoint(0) {}

	bool operator < (const GuildDungeonPlayerDamage& info) const
	{
		return damagePoint < info.damagePoint;
	}
};

typedef std::vector<GuildDungeonPlayerDamage*> GuildDungeonDamageVec;

// 公会地下城奖励
struct GuildDungeonReward
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & itemNum & isHighVal;
	}

	// 道具Id
	UInt32 itemId;
	// 道具数量
	UInt32 itemNum;
	// 是否高价值
	UInt32 isHighVal;

	GuildDungeonReward() : itemId(0), itemNum(0), isHighVal(0) {}
};

typedef std::vector<GuildDungeonReward> GuildDungeonRewardVec;
typedef std::map<UInt32, GuildDungeonReward> GuildDungeonRewardMap;

struct GuildDungeonGateTime
{
	GuildDungeonGateTime() : guildId(0), spendTime(0) {}

	// 公会名字
	UInt64 guildId;
	// 通关用时
	UInt64 spendTime;
};

enum GuildAuctionType
{
	// 无效值
	G_AUCTION_INVALID = 0,
	// 公会拍
	G_AUCTION_GUILD = 1,
	// 世界拍
	G_AUCTION_WORLD = 2,
};

enum GuildAuctionItemState
{
	// 无效值
	GAI_STATE_INVALID = 0,
	// 拍卖准备
	GAI_STATE_PREPARE = 1,
	// 拍卖中
	GAI_STATE_NORMAL = 2,
	// 成交
	GAI_STATE_DEAL = 3,
	// 流拍
	GAI_STATE_ABORATION = 4,
};

struct GuildAuctionItem
{
	GuildAuctionItem() : guildId(0), rewardGroup(0), bidNum(0), guid(0), curPrice(0), bidPrice(0), fixPrice(0),
		addPrice(0), state(0), endTime(0), auctionType(0), bidRoleId(0), bidTime(0), vipLevel(0), level(0), occu(0), isBonus(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & bidRoleId & curPrice & bidPrice & fixPrice & endTime & state & auctionRewardVec;
	}

	std::string SerializeToString()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << ", rewardGroup:" << rewardGroup;
		ss << ", bidNum:" << bidNum;
		ss << ", guildId:" << guildId;
		ss << ", curPrice:" << curPrice;
		ss << ", bidPrice:" << bidPrice;
		ss << ", fixPrice:" << fixPrice;
		ss << ", state:" << state;
		ss << ", endTime:" << endTime;
		ss << ", auctionType:" << auctionType;
		ss << ", bidRoleId:" << bidRoleId;
		ss << ", bidTime:" << bidTime;
		ss << ", vipLevel:" << vipLevel;
		ss << ", level:" << level;
		ss << ", occu:" << occu;
		ss << ", isBonus:" << isBonus;

		ss << ", Reward( ";
		for (auto& iter : auctionRewardVec)
		{
			ss << " id:" << iter.id << ", num:" << iter.num;
		}
		ss << " )";
		return ss.str();
	};

	// 公会id
	UInt64 guildId;
	// 奖励组
	UInt32 rewardGroup;
	// 竞价次数
	UInt32 bidNum;
	// guid
	UInt64 guid;
	// 当前出价
	UInt32 curPrice;
	// 竞拍价
	UInt32 bidPrice;
	// 一口价
	UInt32 fixPrice;
	// 加价
	UInt32 addPrice;
	// 物品状态
	UInt32 state;
	// 拍卖结束时间
	UInt32 endTime;
	// 拍卖类型
	UInt32 auctionType;
	// 出价人
	UInt64 bidRoleId;
	// 出价时间
	UInt32 bidTime;
	// vip
	UInt32 vipLevel;
	// 等级
	UInt32 level;
	// 职业
	UInt32 occu;
	// 是否分成
	UInt32 isBonus;
	// 拍卖物品
	ItemRewardVec auctionRewardVec;
};

/**
 *@brief  公会事件信息
 */
struct GuildEvent
{

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & eventInfo & addTime;
	}

	//唯一id
	UInt64			guid;

	//事件信息
	std::string		eventInfo;

	//发生时间
	UInt32			addTime;
};
struct GuildHonor
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & groupId & cnt & time;
	}
	//荣誉id
	UInt32 id;
	//荣誉组id
	UInt32 groupId;
	//完成次数
	UInt32 cnt;
	//完成时间
	UInt32 time;
};

enum GuildProsperityType
{
	GUILD_PROSPERITY_NULL = 0,
	GUILD_PROSPERITY_DISMISS = 1,
	GUILD_PROSPERITY_LOW = 2,
	GUILD_PROSPERITY_MID = 3,
	GUILD_PROSPERITY_HIGH = 4,
	GUILD_PROSPERITY_IN_DISMISS = 5,
	GUILD_PROSPERITY_IN_MERGER = 6,
	GUILD_PROSPERITY_IN_BY_MERGER = 7,
};

enum GuildMergerRequestStatus
{
	GUILD_MERGER_REQUEST_NULL = 0,
	GUILD_MERGER_REQUEST_SEND = 1,
	GUILD_MERGER_REQUEST_ACCEPTED = 2,
	GUILD_MERGER_REQUEST_REFUSED = 3,
};

enum GuildDismissType
{
	GUILD_DISMISS_NORMAL = 0,
	GUILD_DISMISS_LOW = 1,
	GUILD_DISMISS_MERGER = 2,
};

//*****************************************


#endif //_CL_GUILD_DEFINE_H_