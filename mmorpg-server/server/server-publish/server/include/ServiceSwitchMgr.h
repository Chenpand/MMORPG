#ifndef __SERVICE_SWITCH_MGR_H__
#define __SERVICE_SWITCH_MGR_H__

#include <CLDefine.h>

// 服务类型
enum ServiceType
{
	// 无效值
	SERVICE_INVALID = 0,

	// 赛季匹配
	SERVICE_1V1_SEASON = 1,
	// 工会战报名
	SERVICE_GUILD_BATTLE_ENROLL = 2,
	// 工会战
	SERVICE_GUILD_BATTLE = 3,
	// 自动战斗
	SERVICE_AUTO_BATTLE = 4,
	// 场景等级限制
	SERVICE_SCENE_LEVEL_LIMIT = 5,
	// 新的跨天算法
	NEW_DAY_CACL = 6,
	// 在线时间控制
	SERVICE_DAY_ONLINE_TIME = 7,
	// 工会信息数据库脏数据
	SERVICE_GUILD_CLEAR_DIRTY = 8,

	//-----------装备系统---------------
	// 装备品级调整
	EQUIP_SET_QUALITY_LV = 10,
	// 时装选属性
	FASHION_SEL_ATTR = 11,
	// 金罐积分
	GOLD_JAR_POINT = 12,
	// 魔罐积分
	MAGIC_JAR_POINT = 13,
	// 时装合成
	FASHION_COMPOSE = 14,

	/* 公会系统 */
	// 圆桌会议
	SERVICE_GUILD_TABLE = 20,
	// 公会技能
	SERVICE_GUILD_SKILL = 21,
	// 公会捐赠
	SERVICE_GUILD_DONATE = 22,
	// 公会膜拜
	SERVICE_GUILD_ORZ = 23,
	// 公会红包
	SERVICE_GUILD_RED_PACKET = 24,
	// 跨服公会战
	SERVICE_GUILD_CROSS_BATTLE = 25,

	/* 随从系统 */
	SERVICE_RETINUE = 40,

	/* 次元石系统 */
	SERVICE_WARP_STONE = 60,

	/* 房间 */
	SERVICE_ROOM = 70,
	SERVICE_SCORE_WAR = 71,

	/* 理财计划 */
	SERVICE_MONEY_MANAGE = 80,

	/* 拍卖行复制 */
	SERVICE_AUCTION_COPY = 90,

	// 每日充值福利
	SERVEICE_DAY_CHARGE_WELFARE = 91,

	// 新的队伍状态计算方式
	SERVICE_NEW_TEAM_STATUS = 92,

	// 新的加载账号信息方式
	SERVICE_NEW_LOAD_ACCOUNT_INFO = 93,

	// 玩家状态超时判断
	SERVICE_PLAYER_STATUS_TIMEOUT = 94,

	// 离线玩家加载管理
	SERVICE_OFFLINE_PLAYER_MGR = 95,

	/* 随机宝箱 */
	SERVICE_DIG = 100,

	/* 师徒系统 */
	SERVICE_MASTER = 110,

	/* 安全锁 */
	SERVICE_SECURITY_LOCK = 111,

	/* 3v3乱斗 */
	SERVICE_3v3_TUMBLE = 112,

	/* 公会副本 */
	SERVICE_GUILD_DUNGEON = 145,

	// 拍卖行珍品机制开关
	SERVICE_AUCTION_TREAS = 210,

	// 拍卖行翻页开关
	SERVICE_AUCTION_PAGE = 211,

	// 拍卖行交易冷却时间开关
	SERVICE_AUCTION_COOLTIME = 212,

	// 角色栏位
	SERVICE_ROLE_FILED = 213,

	// 黑市商人
	SERVICE_BLACK_MARKET = 214,

	/* 新的场景同步方式 */
	SERVICE_NEW_SCENE_SYNC = 220,

	// 冒险队(佣兵团)
	SERVICE_ADVENTURE_TEAM = 215,

	// 新回归活动
	SERVICE_NEW_RETURN = 216,

	// 冒险队(佣兵团)排行榜
	SERVICE_ADVENTURE_TEAM_SORTLIST = 217,

	// 设置玩家保存时间
	SERVICE_SET_PLAYER_SAVE_BACK_TIME = 218,

	// 跨天判断同一天
	SERVICE_CHECK_SAME_DAY_ON_DAY_CHANGE = 219,

	// 日充活动上线检查跨天
	SERVICE_CHECK_DAY_CHANGE_FOR_CHARGE_DAY_ACT = 220,

	// 周常深渊失败门票返还
	SERVICE_WEEK_HELL_FAIL_RETURN_TICKETS = 221,

	// 公会拍卖
	SERVICE_GUILD_AUCTION = 222,

	// 运营活动上线同步
	SERVICE_OPACT_ONLINE_SYNC = 223,

	//公会副本奖励截图
	SERVICE_GUILD_DUNGEON_SCREEN_SHOT = 224,

	//单局结算ID判断
	SERVICE_RACE_ID_CHECK = 225,

	//每日必做
	SERVICE_DAILY_TODO = 226,

	//当删除账号玩家时检查玩家ID
	SERVICE_CHECK_ROLEID_WHEN_REMOVE_ACCOUNT_PLAYER = 227,

	//装备强化保护开关
	SERVICE_EQUIP_STRENGTHEN_PROTECT = 228,

	//装备强化装备分解失败移除装备开关
	SERVICE_EQUIP_STRENG_DESC_FAIL_REMOVE = 229,

	//辅助装备开关
	SERVICE_ASSIST_EQUIP = 230,

	//公会兼并开关
	SERVICE_GUILD_MERGER = 231,

	//终极试炼爬塔
	SERVICE_ZJSL_TOWER = 232,

	//团本
	SERVICE_TEAM_COPY = 233,

	//飞升药接受活动同步开关
	SERVICE_FLY_UP    = 234,

	// 日志埋点通过游戏服发送
	SERVICE_NEW_CLIENT_LOG = 235,

	// 新的结算面板
	SERVICE_NEW_RACE_END_EXP = 236,

	//技能页开关
	SERVICE_SKILL_PAGE = 237,

	//装备方案
	SERVICE_EQUIP_SCHEME = 238,

	//金币寄售
	SERVICE_GOLD_CONSIGNMENT = 239,

	//团本2
	SERVICE_TEAM_COPY_TWO = 240,

	SERVICE_NUM,
};

struct ServiceConfig
{
	ServiceConfig() : type(SERVICE_INVALID), open(true) {}

	ServiceType type;
	// 是否开放
	bool open;
};

// 控制服务是否开放
class ServiceSwitchMgr
{
public:
	ServiceSwitchMgr();
	virtual ~ServiceSwitchMgr();

public:
	void OpenService(ServiceType type);
	void CloseService(ServiceType type);
	bool IsOpen(ServiceType type);

public:
	virtual void OnOpenService(ServiceType type) {}
	virtual void OnCloseService(ServiceType type) {}

protected:
	ServiceConfig			m_serviceConfigs[SERVICE_NUM];
};

#endif