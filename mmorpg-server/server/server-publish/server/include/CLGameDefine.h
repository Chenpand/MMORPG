#ifndef _CL_GAMEDEFINE_H_
#define _CL_GAMEDEFINE_H_

#include "CLDefine.h"
#include <CLJobDataEntry.h>

//名字长度
#define NAME_LENGTH 32
#define NAME_SHOWWIDTH 14

//开服时间的游戏参数名
#define PARAM_NAME_GAME_START "gamestart_time"
#define PARAM_NAME_GAME_FIRST_START "game_first_start_time"

//荣耀排行时间
#define HONOR_RANK_TIME "honor_rank_time"

//全民砍价折扣
#define WHOLE_BARGAIN_DISCOUNT "whole_bargain_discount"

// 钱最大数
#define MAX_MONEY 1000000000

//最大sp
#define MAX_SP 99999

//最大等级
#define MAX_LEVEL 65

// 最大正在删除的角色数量
const static UInt32 MAX_DELETING_PLAYER_NUM = 3;
// 删除的角色的保存时间
const static UInt32 DELETE_PLAYER_SAVE_TIME = 7 * Avalon::Time::SECS_OF_DAY;
// 小于该等级的角色数直接删除
const static UInt32 DIRECT_DELETE_PLAYER_LEVEL = 10;

/**
 *@brief 性别
 */
enum Sex
{
	SEX_MALE	= 0,	//男
	SEX_FEMALE	= 1,	//女
};

inline bool IsValidSex(int sex)
{
	return sex == SEX_MALE || sex == SEX_FEMALE;
}

/**
*@brief 创建角色时判断是否合法
*/
inline bool IsValidOccu(UInt8 mainOccu, UInt8 preOccu, UInt8 isRobot)
{
	JobDataEntry* mainOccuData = JobDataEntryMgr::Instance()->FindEntry(mainOccu);
	if (mainOccuData == NULL) return false;
	JobDataEntry* preOccuData = JobDataEntryMgr::Instance()->FindEntry(preOccu);
	if (preOccuData == NULL) return false;
	if (!mainOccuData->CheckTargetOccu(preOccu)) return false;

	if (mainOccuData->canCreateRole) return true;
	if (isRobot == 2) return true;
	return false;
}

#define TRANSFORM_OCCU_LEVEL 15
#define AWAKEN_ONE_LEVEL	45


/**
 *@brief 基础经验系数
 */
#define BASEEXP_FACTOR_A 720
#define BASEEXP_FACTOR_B 1440

/**
 *@brief 惩罚类型，按优先级从低到高排列
 */
enum PunishType
{
	PUNISH_FORBIDTALK = 1,		//禁言
	PUNISH_FORBIDLOGIN = 2,		//禁止登陆

	PUNISH_MAX = 3,
};

/**
 *@brief gm权限级别
 */
enum GmAuthorityLevel
{
	GM_AUTHORITY_NONE	= 0,		//没gm权限
	GM_AUTHORITY_NORMAL = 10,		//普通gm权限
	GM_AUTHORITY_SUPER	= 20,		//超级gm权限
	GM_AUTHORITY_DEBUG	= 30,		//调试权限
};

/**
 *@brief 客户端进入游戏选项
 */
enum EnterGameOption
{
	ENTERGAME_OPTION_WALLOW	=	1 << 0,	//防沉迷选项
};

/**
 *@brief 临时逻辑定时类型，最大不超过5分钟
 */
enum LogicTickType
{
	LOGICTICK_SECOND =	0,	//1秒
	LOGICTICK_FIVESEC = 1,	//5秒
	LOGICTICK_HALFMIN = 2,	//半分钟	
	LOGICTICK_MIN	 =	3,	//1分钟
	LOGICTICK_FIVEMIN = 4,	//5分钟
	//TODO
	LOGICTICK_MAX,
};
//逻辑定时间隔
const static UInt64	LOGICTICK_INTERVAL[] = { 1000, 5000, 30000, 60000, 300000 };

/**
 *@brief 复活类型
 */
enum ReviveType
{
	REVIVE_INPOSITION = 0,	//原地复活
	REVIVE_NORMAL = 1,		//回城复活
	REVIVE_SCENE = 2,		//场景复活（在副本和战场有效）
	REVIVE_KICKOUT = 3,		//踢出副本复活
};

/**
 *@brief pk模式
 */
enum PkMode
{
	PK_MODE_PEACE = 0,		//和平
	PK_MODE_TEAM =	1,		//组队
	PK_MODE_TRIBE = 2,		//部落
	PK_MODE_ALL = 3,		//全体
	PK_MODE_EVIL = 4,		//罪恶
	PK_MODE_MAX,
};

/**
 *@brief pk颜色
 */
enum PkColor
{
	PK_COLOR_WHITE = 0,		//白色
	PK_COLOR_GREY = 1,		//灰色
	PK_COLOR_RED = 2,		//红
	PK_COLOR_DARKRED = 3,	//深红
};

/**
 *@brief 交易状态
 */
enum TradeStatus
{
	TRADE_STATUS_UNLOCK = 0,	//未锁定
	TRADE_STATUS_LOCK = 1,		//锁定
	TRADE_STATUS_CONFIRM = 2,	//确认
	TRADE_STATUS_CANCEL = 3,	//取消
};

/**
 *@brief 交易请求有效时间(秒)
 */
#define TRADE_INVITE_VALID_TIME (5*60)
/**
 *@brief 交易格子数
 */
#define TRADE_GRID_NUM	7


// 战场参数数量
#define BATTLE_PARAM_NUM 5


//网关服务器发送的udplog的uid
#define UDPLOG_GATE_UID		"gate"
// 场景服务器发送的udplog的uid
#define UDPLOG_SCENE_UID	"scene"
//世界服务器发送的udplog的uid
#define UDPLOG_WORLD_UID	"world"
//Global活动服发送的udplog的uid
#define UDPLOG_GLOBAL_ACTIVITY_UID	"global_activity"
//Dungeon服务器发送的udplog的uid
#define UDPLOG_DUNGEON_UID  "dungeon"
//团本服务器发送的udplog的uid
#define UDPLOG_TEAM_COPY_UID  "team_copy"

//护送任务奖励
struct EscortAward
{
	EscortAward():exp(0), contribution(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & exp & contribution;
	}

	//经验
	UInt32	exp;
	//帮贡
	UInt32	contribution;
};

/**
 *@brief 护送事件
 */
enum EscortEvent
{
	ESCORTEVENT_KILLED	= 1,	//护送目标被杀
	ESCORTEVENT_FARAWAR = 2,	//远离
	ESCORTEVENT_CLOSETO = 3,	//靠近
	ESCORTEVNET_DIED	= 4,	//死亡
	ESCORTEVENT_ATTACKED = 5,	//被攻击
	ESCORTEVNET_SUCC =	6,		//护送成功
};

//最大称号数
#define MAX_TITLE_NUM 128


//游戏系统事件类型
enum SysEventType
{
	ST_NONE = 0,
	ST_CHARGE,
	ST_LVUP,
	ST_FINISH_DG,
	ST_USE_ITEM,

};


/**
 *@brief 行为标志
 */
enum ActionFlag
{
	ACTION_FLAG_GOLDBUY = 0,			//金贝购买
	ACTION_FLAG_BINDGOLDBUY = 1,		//绑金购买
	ACTION_FLAG_KILL = 2,				//野外地图杀死别人
	ACTION_FLAG_KILLED = 3,				//野外地图被杀死
	ACTION_FLAG_STRENGTHENEQUIP = 4,	//强化装备
	ACTION_FLAG_XIYUAN = 5,
	ACTION_FLAG_NEWQQVIP = 7,			//领取qq黄钻或蓝钻新手礼包
	ACTION_FLAG_POINTBUY = 8,			//点数消费

	ACTION_FLAG_NEWER = 10,				//新玩家
	ACTION_FLAG_COUPON_GIFTBAG1	= 11,	//抵扣券礼包1
	ACTION_FLAG_COUPON_GIFTBAG2 = 12,	//抵扣券礼包2
	ACTION_FLAG_COUPON_GIFTBAG3 = 13,	//抵扣券礼包3
	ACTION_FLAG_FASHION_SHOW = 14,		//时装显示 0:不显示 1:显示
	ACTION_FLAG_TRANSWOOD = 15,			//运送木材
	ACTION_FLAG_VIP_TIANWAIYUNTIE = 16,	//vip赠送天外陨铁
	ACTION_FLAG_VIP_DIXINYUNTIE = 17,	//vip赠送地心陨铁
	ACTION_FLAG_VIP_SHENYUANYUNIIE = 18,//vip赠送深渊陨铁
	ACTION_FLAG_APPONPANEL = 19,		//
	ACTION_FLAG_ROBOT = 20,				//机器人

	ACTION_FLAG_MAX,
};


/**
 *@brief 根据登陆天数获取箱子奖励
 */
inline UInt32 GetSigninBoxByDays(UInt32 days)
{
	if(days >= 1 && days <= 7) return 10 + days;

	return 11;
}

/**
 *@brief 根据登陆天数获取vip箱子奖励
 */
inline UInt32 GetVipSigninBoxByDays(UInt32 days)
{
	if(days >= 1 && days <= 7) return 20 + days;

	return 21;
}

/**
 *@brief 每日礼包标志类型
 */
enum DailyGiftBagFlag
{
	DAILY_GIFTBAG_ACTIVE1 = 1,			//活跃度1
	DAILY_GIFTBAG_ACTIVE2 = 2,			//活跃度2
	DAILY_GIFTBAG_ACTIVE3 = 3,			//活跃度3
	DAILY_GIFTBAG_ACTIVE4 = 4,			//活跃度4
	DAILY_GIFTBAG_SIGNIN	= 11,		//签到
	DAILY_GIFTBAG_VIPGIFTBAG = 12,		//vip礼包
	DAILY_GIFTBAG_QQVIP	= 13,			//qq黄钻
	DAILY_GIFTBAG_QQVIP_RECOMMEND1 = 14, //QQ黄钻推荐位礼包1
	DAILY_GIFTBAG_QQVIP_RECOMMEND2 = 15, //QQ黄钻推荐位礼包2
	DAILY_GIFTBAG_QQVIP_KLWELCOME1 = 16, //恐龙新玩家礼包
	DAILY_GIFTBAG_QQVIP_KLWELCOME2 = 17, //恐龙老玩家礼包
	DAILY_GIFTBAG_3366 = 18,		

	DAILY_GIFTBAG_INVITE = 20,			//邀请礼包
	DAILY_GIFTBAG_INVITEE_ACTIVE1 = 21,	//被邀请者活跃礼包1
	DAILY_GIFTBAG_INVITEE_ACTIVE2 = 22,	//被邀请者活跃礼包2
	DAILY_GIFTBAG_INVITEE_ACTIVE3 = 23,	//被邀请者活跃礼包3
	DAILY_GIFTBAG_INVITEE_ACTIVE4 = 24,	//被邀请者活跃礼包4
	DAILY_GIFTBAG_INVITEE_ACTIVE5 = 25,	//被邀请者活跃礼包5
};


//剧情最大个数
#define MAX_ONCE_STORY_NUM 200

//最大功能开启数
#define MAX_FUNCTION_OPEN_NUM 128

//最大弱引导个数
#define MAX_BOOT_FLAG_NUM 128

//第一场战斗ID
#define NEW_BOOT_FIRST_BATTLE_ID 1


/**
 *@brief 获取抵扣券礼包商品对应的action掩码                                                                     
 */
inline UInt32 GetCouponMallItemActionFlag(UInt32 mallid)
{
	switch(mallid)
	{
	case 1001: return ACTION_FLAG_COUPON_GIFTBAG1;
	case 1002: return ACTION_FLAG_COUPON_GIFTBAG2;
	case 1003: return ACTION_FLAG_COUPON_GIFTBAG3;
	}
	return 0;
}

/**
 *@brief 移动监测间隔                                                                     
 */
#define MOVE_CHECK_INTERVAL 5000

/**
*@brief 读条动作类型
*/
enum LastActionType
{
	LAT_INVALID = 0,	//无效
	LAT_GATHER = 1,		//采集
	LAT_FISH = 2,		//钓鱼
};

// 玩家外观信息
struct PlayerAvatar
{
	PlayerAvatar() : weaponStrengthen(0), isShoWeapon(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & equipItemIDs & weaponStrengthen & isShoWeapon;
	}

	bool operator!=(const PlayerAvatar& other) const
	{
		return other.equipItemIDs != equipItemIDs || other.weaponStrengthen != weaponStrengthen || other.isShoWeapon != isShoWeapon;
	}

	bool operator==(const PlayerAvatar& other) const
	{
		return !(other != *this);
	}

	std::vector<UInt32>		equipItemIDs;
	// 武器强化等级
	UInt8					weaponStrengthen;
	// 是否显示时装武器
	UInt8					isShoWeapon;
};


struct SceneRetinue
{
public:
	SceneRetinue() : m_DataId(0), m_Level(1) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level;
	}

	bool operator!=(const SceneRetinue& other) const
	{
		return m_DataId != other.m_DataId || m_Level != other.m_Level;
	}

	bool operator==(const SceneRetinue& other) const
	{
		return m_DataId == other.m_DataId && m_Level == other.m_Level;
	}

	bool operator=(const SceneRetinue& other)
	{
		m_DataId = other.m_DataId;
		m_Level = other.m_Level;
		return true;
	}

	//随从表ID
	UInt32 m_DataId;

	//等级
	UInt8 m_Level;
};

struct ScenePet
{
public:
	ScenePet() :m_Id(0), m_DataId(0), m_Level(1){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Id & m_DataId & m_Level;
	}

	bool operator!=(const ScenePet& other) const
	{
		return m_Id != other.m_Id || m_DataId != other.m_DataId || m_Level != other.m_Level;
	}

	bool operator==(const ScenePet& other) const
	{
		return m_Id == other.m_Id && m_DataId == other.m_DataId && m_Level == other.m_Level;
	}

	bool operator=(const ScenePet& other)
	{
		m_Id = other.m_Id;
		m_DataId = other.m_DataId;
		m_Level = other.m_Level;
		return true;
	}

	//宠物id
	ObjID_t m_Id;

	//随从表ID
	UInt32 m_DataId;

	//等级
	UInt16 m_Level;
};

/**
*@brief 功能解锁类型
*/
enum UnlockType
{
	UNLOCK_INVALID = 0,
	UNLOCK_FUNCTION = 1,	//功能解锁
	UNLOCK_AREA = 2,		//区域解锁

	UNLOCK_NUM = 3,
};

// 红点标记
enum RedPointFlag
{
	// 请求者
	RED_POINT_GUILD_REQUESTER = 0,
	// 商店
	RED_POINT_GUILD_SHOP = 1,
	// 公会膜拜
	RED_POINT_GUILD_ORZ = 2,
	// 圆桌会议
	RED_POINT_GUILD_TABLE = 3,
	// 公会兼并
	RED_POINT_GUILD_MERGER = 4,
	// 公会领地每日奖励
	RED_POINT_GUILD_TERR_ID = 5,
};

// 消耗物品
struct ConsumeItem
{
public:
	ConsumeItem() : itemId(0), num(0){}

public:
	UInt32 itemId;
	UInt32 num;
};

// 账号信息类型
enum AccountInfoType
{
	// 点劵
	ACCOUNT_POINT,
	// VIP等级
	ACCOUNT_VIP_LEVEL,
	// VIP经验
	ACCOUNT_VIP_EXP,
	// 总的充值金额
	ACCOUNT_TOTAL_CHARGE_NUM,
	// 理财计划
	ACCOUNT_MONEY_MANAGE_STATUS,
	// 武器租赁好运符数量
	ACCOUNT_WEAPON_LEASE_TICKETS,
	// 商城积分
	ACCOUNT_MALL_POINT,
	// 日活跃度
	ACCOUNT_ACTIVITY,
	// 冒险币
	ACCOUNT_ADVENTURE,
	// 冒险通行证经验
	ACCOUNT_ADVENTURE_EXP,
	// 冒险通行证王者版
	ACCOUNT_ADVENTURE_KING,
	// 招募硬币
	ACCOUNT_HIRE_COIN,
	// 礼遇特权
	ACCOUNT_GIFT_RIGHT,
	// 信用点劵
	ACCOUNT_CREDIT_POINT,
	// 大富翁硬币
	ACCOUNT_MONOPOLY_COIN,
	// 大富翁祝福卡
	ACCOUNT_MONOPOLY_CARD,	
	// 大富翁骰子
	ACCOUNT_MONOPOLY_ROLL,
	// 深渊特权
	ACCOUNT_DUNGEON_RIGHT,
};

enum NotifyType
{
	NT_NONE = 0,

	// 公会战
	NT_GUILD_BATTLE = 1,

	// 武道大会 		
	NT_BUDO = 2,

	//罐子开放				
	NT_JAR_OPEN = 3,

	//罐子折扣重置			
	NT_JAR_SALE_RESET = 4,

	//时限道具
	NT_TIME_ITEM = 5,

	//赏金武道大会
	NT_GOLD_BONUS_DAHUI = 6,

	//魔罐积分清空
	NT_MAGJAR_RESET_POINT = 7,
	NT_MAX,
};

struct NotifyInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & param;
	}

	UInt32		type;	//NotifyType
	UInt64		param;
};

/**
*@brief 登录推送信息
*/
struct LoginPushInfo
{
	LoginPushInfo()
	{
		id = 0;
		unlockLevel = 0;
		startTime = 0;
		endTime = 0;
		sortNum = 0;
		isShowTime = 0;
		isSetNative = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & unlockLevel & iconPath & linkInfo & startTime & \
			endTime & loadingIconPath & sortNum & openInterval & closeInterval & \
			isShowTime & isSetNative;
	}

	// id
	UInt8				id;
	// 名称
	std::string			name;
	// 解锁等级
	UInt16				unlockLevel;
	// 图标路径
	std::string			iconPath;
	// 链接位置
	std::string			linkInfo;
	// 生效开始时间
	UInt32				startTime;
	// 生效结束时间
	UInt32				endTime;
	// loading预制体路径
	std::string			loadingIconPath;
	// 排序序号
	UInt8				sortNum;
	// 开启间隔
	std::string			openInterval;
	// 关闭间隔
	std::string			closeInterval;
	// 是否显示时间
	UInt8				isShowTime;
	// 是否设置背景图片原比例大小
	UInt8				isSetNative;
};

/**
*@brief 玩家登陆状态
*/
enum PlayerLoginStatus
{
	PLS_NONE,
	PLS_FIRST_LOGIN_DAILY, // 每天第一次登录
};

/**
*@brief 包统计
*/
struct PacketStatistics
{
	PacketStatistics() : num(0), totalSize(0) {}
	UInt32 num;
	UInt32 totalSize;
};

#endif

