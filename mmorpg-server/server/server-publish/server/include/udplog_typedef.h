 
#ifndef _COMMON_TYPEDEF_H_
#define _COMMON_TYPEDEF_H_

#include <AvalonDefine.h>
#include <AvalonStringUtil.h>
#include "CLDefine.h"

#ifdef __GNUC__
#include <stdint.h>
#include <inttypes.h>
#endif

#define MAX_LOG_LENGTH  1024
#define MAX_MESSAGE_LENGTH MAX_LOG_LENGTH*5
#define MAX_OPTION_LENGTH  128
#define UL_OK 0
#define UL_FAIL -1
#define MAX_IPADDR_LENGTH 20
#define MAX_USERMSG_LENGTH 512

#define DEFAULT_CONFIGFILENAME			"../Config/UdpLog.xml"
#define DEFAULT_SERVER_CONFIG_FILE		"../Config/UdpServer.xml"
//#define DEFAULT_CONFIGFILENAME			"D:\\udp_log\\udplog_cpp\\udplog_cpp\\udplog_cpp\\Debug\\config\\udplog.xml"

//#if defined(AVALON_OS_WINDOWS) || defined(_AVALON_OS_WINDOWS)
//#pragma warning(disable:4996)
//#pragma warning(disable:4005)
//#endif
//
//#if defined(AVALON_OS_WINDOWS) || defined(_AVALON_OS_WINDOWS)
//#define OS_AVALON_OS_WINDOWS 1;
//#else
//#define OS_LINUX 1;
//#endif


enum LogSourceType
{
	SOURCE_TYPE_INVALID = 0,
	SOURCE_TYPE_GM = 1,

	//玩家相关
	SOURCE_TYPE_PLAYER = 100,
	SOURCE_TYPE_PLAYER_CHANGE_OCCU,
	SOURCE_TYPE_PLAYER_UP_LEVEL,
	SOURCE_TYPE_PLAYER_BIRTH,
	SOURCE_TYPE_PLAYER_ONLINE,
	SOURCE_TYPE_FRIEND_GIVE,
	//师傅获得疲劳
	SOURCE_TYPE_MASTER_GET_FATIGUE,
	//删除师徒关系
	SOURCE_TYPE_DEL_MASTER_DISCIPLE,
	//玩家刷新
	SOURCE_TYPE_PLAYER_REFRESH,
	//解锁可扩展角色栏位
	SOURCE_TYPE_UNLOCK_EXTENSIBLE_ROLE_FIELD,
	//删除角色
	SOURCE_TYPE_DELETE_ROLE,
	//恢复角色
	SOURCE_TYPE_RECOVER_ROLE,
	//解锁技能页
	SOURCE_TYPE_UNLOCK_SKILL_PAGE,


	//地下城相关
	SOURCE_TYPE_DUNGEON = 1000,
	//怪物
	SOURCE_TYPE_DUNGEON_MONSTER,
	//箱子
	SOURCE_TYPE_DUNGEON_BOX,
	//黄金箱子
	SOURCE_TYPE_DUNGEON_GOLD_BOX,
	//地下城结算
	SOURCE_TYPE_DUNGEON_END,
	//死亡之塔
	SOURCE_TYPE_DUNGEON_DEATH_TOWER,
	//扫荡死亡之塔
	SOURCE_TYPE_DEATH_TOWER_WIPEOUT,
	//死亡之塔首通奖励
	SOURCE_TYPE_DEATH_TOWER_PASS_FLOOR_AWARD,
	//任务物品
	SOURCE_TYPE_DUNGEON_TASK_ITEM,
	//购买次数
	SOURCE_TYPE_DUNGEON_BUY_TIMES,
	//翻牌
	SOURCE_TYPE_DUNGEON_CHEST,
	//结算掉落翻倍
	SOURCE_TYPE_DUNGEON_MULTI_RACE_END_DROP,
	//复活
	SOURCE_TYPE_DUNGEON_REVIVE,
	//组队奖励
	SOURCE_TYPE_DUNGEON_TEAM_REWARD,
	//吃Buff药
	SOURCE_TYPE_DUNGEON_BUFF_DRUG,
	//购买并使用Buff药
	SOURCE_TYPE_DUNGEON_BUFF_DRUG_QUICK_BUY,
	//月卡结算金币奖励
	SOURCE_TYPE_DUNGEON_MONTH_CARD_GOLD_REWARD,
	//金币称号金币奖励
	SOURCE_TYPE_DUNGEON_GOLD_TITLE_GOLD_REWARD,
	//疲劳燃烧
	SOURCE_TYPE_DUNGEON_FATIGUE_BURNING,
	//结算奖励
	SOURCE_TYPE_DUNGEON_RACE_END,
	//终极试炼刷新BUFF
	SOURCE_TYPE_ZJSL_REFRESH_BUFF,
	//终极试炼全通关奖励领取
	SOURCE_TYPE_ZJSL_CLEAR_GET_AWARD,

	//物品相关
	SOURCE_TYPE_ITEM = 2000,
	//罐子
	SOURCE_TYPE_ITEM_JAR,
	//装备分解
	SOURCE_TYPE_EQUIP_DECOMPOSE,
	//强化
	SOURCE_TYPE_EQUIP_STRENGTHEN,
	//合成附魔卡
	SOURCE_TYPE_ADD_MAGIC_CARD,
	//附魔
	SOURCE_TYPE_ADD_MAGIC,
	//移动
	SOURCE_TYPE_MOVE,
	//系统
	SOURCE_TYPE_SYSTEM,
	//拓展背包
	SOURCE_TYPE_ENLARGE_PACK,
	//拓展仓库
	SOURCE_TYPE_ENLARGE_STORAGE,
	//物品移动
	SOURCE_TYPE_ITEM_MOVE,
	//整理物品
	SOURCE_TYPE_ITEM_TRIM,
	//使用物品
	SOURCE_TYPE_USE_ITEM,
	//装备封装
	SOURCE_TYPE_SEAL_EQUIP,
	//重置品质等级
	SOURCE_TYPE_REMAKE_EQUIP,
	//武道大会奖励
	SOURCE_TYPE_WUDAO_REWARD,
	//武道大会
	SOURCE_TYPE_WUDAO,
	//时装合成
	SOURCE_TYPE_FASHION_COMPOSE,
	//装备制作
	SOURCE_EQUIP_MAKE,
	//道具到期
	SOURCE_TYPE_ITEM_TIMEOUT,
	//时装更换属性
	SOURCE_TYPE_FASHION_SEL_ATTR,
	//使用月卡道具
	SOURCE_TYPE_GM_MONTH_CARD,
	//金罐重置积分
	SOURCE_GOLDJAR_RESET_POINT,
	//魔罐重置积分
	SOURCE_MAGJAR_RESET_POINT,
	//发送喇叭
	SOURCE_SEND_HORN,
	//使用礼包
	SOURCE_USE_GIFT_PACK,
	//罐子十连抽
	SOURCE_TYPE_ITEM_JAR_TEN,
	//续费时限道具
	SOURCE_TYPE_RENEW_TIME_ITEM,
	//使用改名卡
	SOURCE_TYPE_USE_CHANGE_NAME_CARD,
	//合服仓库溢出
	SOURCE_TYPE_MERGE_SERVER_STORAGE,
	//合服给角色改名卡
	SOURCE_TYPE_MERGE_SERVER_ROLECHANGENAME,
	//合服给公会改名卡
	SOURCE_TYPE_MERGE_SERVER_GUILDCHANGENAME,
	//师傅赠送
	SOURCE_TYPE_MASTER_GIVE,
	//师傅亲密付
	SOURCE_TYPE_MASTER_ADDON_PAY,
	//开魔盒
	SOURCE_TYPE_OPEN_MAG_BOX,
	//添加宝珠
	SOURCE_TYPE_JEWELRY,
	//装备转移
	SOURCE_TYPE_TRANSFER,
	//装备回收积分提升
	SOURCE_TYPE_EQRECO_UPSCORE,
	//装备回收装备赎回
	SOURCE_TYPE_EQRECO_REDEEM,
	//装备回收背包装备自动销毁(周一)
	SOURCE_TYPE_EQRECO_DESTORY_SELF,
	//使用消耗道具的罐子规则礼包
	SOURCE_TYPE_CONSUME_JARGIFT,
	//强化券合成
	SOURCE_TYPE_STRENGTHEN_TICKET_SYNTHESIS,
	//强化券融合
	SOURCE_TYPE_STRENGTHEN_TICKET_FUSE,
	//宝珠镶嵌
	SOURCE_TYPE_PRECBEAD_MOUNT,
	//宝珠摘除
	SOURCE_TYPE_PRECBEAD_EXTIRPE,
	//宝珠置换
	SOURCE_TYPE_PRECBEAD_REPLACE,
	//装备升级
	SOURCE_EQUIE_UPDATE,
	//合服给冒险队(佣兵团)改名卡
	SOURCE_TYPE_MERGE_SERVER_ADVENTURETEAMCHANGENAME,
	// 头像框补发
	SOURCE_TYPE_HEAD_FRAME_COMPENSATE,
	//附魔卡升级
	SOURCE_TYPE_MAGIC_UP,
	//附魔卡一键合成
	SOURCE_TYPE_ADD_MAGIC_CARD_ONEKEY,
	//装备增幅
	SOURCE_TYPE_EQUIP_ENHANCE,
	//商店相关
	SOURCE_TYPE_ENHANCE_MATERIAL_COMBO,
	//气息装备清除
	SOURCE_TYPE_SMELL_EQUIP_CLEAR,
	//气息装备激活
	SOURCE_TYPE_SMELL_EQUIP_RED,
	//红字装备转化
	SOURCE_TYPE_ENHANCE_EQUIP_CHG,
	//活动罐子抽
	SOURCE_TYPE_ITEM_ACT_JAR_ONE,
	//活动罐子十连抽
	SOURCE_TYPE_ITEM_ACT_JAR_TEN,
	//铭文开孔
	SOURCE_TYPE_INSCRIPTION_OPEN,
	//铭文镶嵌
	SOURCE_TYPE_INSCRIPTION_MOUNT,
	//铭文摘取
	SOURCE_TYPE_INSCRIPTION_EXTRACT,
	//铭文合成
	SOURCE_TYPE_INSCRIPTION_SYNTHESIS,
	//铭文碎裂
	SOURCE_TYPE_INSCRIPTION_DESTROY,
	//宝珠转换
	SOURCE_TYPE_PRECBEAD_CONVERT,
	//装备转换
	SOURCE_EQUIE_COVERNT,
	//拓展角色仓库
	SOURCE_TYPE_ENLARGE_ROLE_STORAGE,
	//装备回收
	SOURCE_TYPE_EQRECO,
			
	//商店相关
	SOURCE_TYPE_SHOP = 3000,
	//购买
	SOURCE_TYPE_SHOP_BUY,
	//出售
	SOURCE_TYPE_SHOP_SELL,
	//商城
	SOURCE_TYPE_SHOP_MALL,
	//商店刷新
	SOURCE_TYPE_SHOP_REFRESH,
	//公会仓库购买
	SOURCE_TYPE_GUILD_SHOP_BUY,
	//账号商店购买
	SOURCE_TYPE_ACCOUNT_SHOP_BUY,
	//充值推送购买
	SOURCE_TYPE_RECHARGE_PUSH_BUY,

	//任务相关
	SOURCE_TYPE_TASK = 4000,
	//主线
	SOURCE_TYPE_TASK_MAIN,
	//支线
	SOURCE_TYPE_TASK_BRANCH,
	//每日任务
	SOURCE_TYPE_TASK_DAILY,
	//成就
	SOURCE_TYPE_TASK_ACHIEVEMENT,
	//转职
	SOURCE_TYPE_TASK_CHANGE_OCCU,
	//活动任务
	SOURCE_TYPE_TASK_ACTIVE,
	//循环任务
	SOURCE_TYPE_TASK_CYCLE,
	//循环任务刷新
	SOURCE_TYPE_TASK_CYCLE_REFRESH,
	//红包任务
	SOURCE_TYPE_TASK_RED_PACKET,
	//每日任务积分奖励
	SOURCE_TYPE_TASK_SCORE_REWARD,
	//称号任务
	SOURCE_TYPE_TASK_TITLE,
	//传奇之路
	SOURCE_TYPE_TASK_LEGEND,
	//觉醒任务
	SOURCE_TYPE_TASK_AWAKEN,
	//成就积分奖励
	SOURCE_TYPE_TASK_ACHIEVEMENT_SCORE_REWARD,
	//师门日常任务
	SOURCE_TYPE_TASK_MASTER_DAILY,
	//师门学业成长任务
	SOURCE_TYPE_TASK_MASTER_ACADEMIC,
	//账号成就
	SOURCE_TYPE_ACCOUNT_TASK_ACHIEVEMENT,
	//立即完成任务
	SOURCE_TYPE_TASK_FINISH_RIGHTNOW,
	//周签到周奖励
	SOURCE_TYPE_WEEK_SIGN_WEEK_REWARD,
	//冒险队(佣兵团)账号每周任务
	SOURCE_TYPE_ADVENTURE_TEAM_ACCOUNT_WEEKLY_TASK,
	//自然月签到相关
	SOURCE_TYPE_NEW_SIGN,

	//拍卖行相关
	SOURCE_TYPE_ACTION = 5000,
	//拍卖行上架
	SOURCE_TYPE_ACTION_UP,
	//拍卖行售出
	SOURCE_TYPE_ACTION_SELL,
	//拍卖行购买
	SOURCE_TYPE_ACTION_BUY,
	//拍卖行下架
	SOURCE_TYPE_ACTION_CANCEL,
	//拍卖行超时
	SOURCE_TYPE_ACTION_TIMEOUT,
	//刷新拍卖行
	SOURCE_TYPE_AUCTION_REFRESH,
	//购买栏位
	SOURCE_TYPE_AUCTION_BUY_BOOTH,
	//异常交易充值返还金
	SOURCE_TYPE_ABNORMAL_TRANS_CHARGE_BACK_MONEY,
	//异常交易达到活跃度返还金
	SOURCE_TYPE_DAILY_TASK_SCORE_BACK_MONEY,
	//异常交易冻结金
	SOURCE_TYPE_ABNORMAL_TRANS_FREEZE_MONEY,
	//异常交易冻结邮件
	SOURCE_TYPE_ABNORMAL_TRANS_FREEZE_MAIL,
	//异常交易解冻邮件
	SOURCE_TYPE_ABNORMAL_TRANS_UNFREEZE_MAIL,
	//拍卖行审核通过交易成功
	SOURCE_TYPE_ACTION_ADUIT_TRANS_SUCC,
	//拍卖行审核异常取消交易
	SOURCE_TYPE_ACTION_ADUIT_CANCEL_TRANS,
	//拍卖行审核异常过但交易成功
	SOURCE_TYPE_ACTION_ADUIT_ABNORAML_TRANS_SUCC,
	//异常交易未冻结金
	SOURCE_TYPE_ABNORMAL_TRANS_UNFREEZE_MONEY,
	//拍卖行扫货
	SOURCE_TYPE_ACTION_SYS_RECOVERY,

	//次元石
	SOURCE_TYPE_WARP_STONE = 6000,
	//解锁
	SOURCE_TYPE_WARP_STONE_UNLOCK,
	//充能
	SOURCE_TYPE_WARP_STONE_CHARGE,

	//随从
	SOURCE_TYPE_RETINUE = 7000,
	//随从解锁
	SOURCE_TYPE_RETINUE_UNLOCK,
	//随从洗练
	SOURCE_TYPE_RETINUE_CHANGE_SKILL,
	//随从升级
	SOURCE_TYPE_RETINUE_UPLEVEL,
	//随从升星
	SOURCE_TYPE_RETINUE_UP_STAR,

	//邮件相关
	SOURCE_TYPE_MAIL = 8000,
	//收取附件
	SOURCE_TYPE_MAIL_COLLECTION_ATTACHMENT,
	//附件系统扣除
	SOURCE_TYPE_MAIL_SYSTEM_TAKE,

	//VIP
	SOURCE_TYPE_VIP = 9000,
	//购买VIP礼包
	SOURCE_TYPE_VIP_BUY_GIFT = 9001,
	//勇者之塔充值次数
	SOURCE_TYPE_VIP_BUY_TOWER_RESET = 9002,

	//功能解锁
	SOURCE_TYPE_FUNCTION_UNLOCK = 10000,

	//技能相关
	SOURCE_TYPE_SKILL = 11000,
	//技能升级
	SOURCE_TYPE_SKILL_UP_LEVEL,
	//技能降级
	SOURCE_TYPE_SKILL_DOWN_LEVEL,

	// 公会相关
	SOURCE_TYPE_GUILD = 12000,
	// 创建公会
	SOURCE_TYPE_GUILD_CREATE,
	// 修改公会名
	SOURCE_TYPE_GUILD_CHANGE_NAME,
	// 公会捐赠
	SOURCE_TYPE_GUILD_DONATE,
	// 公会兑换
	SOURCE_TYPE_GUILD_EXCHANGE,
	// 升级公会技能
	SOURCE_TYPE_GUILD_UPGRADE_SKILL,
	// 发送公会邮件
	SOURCE_TYPE_GUILD_MAIL,
	// 解散工会
	SOURCE_TYPE_GUILD_DISMISS,
	// 膜拜
	SOURCE_TYPE_GUILD_ORZ,
	// 圆桌会议
	SOURCE_TYPE_GUILD_TABLE,
	// 公会战报名
	SOURCE_TYPE_GUILD_BATTLE_ENROLL,
	// 公会战鼓舞
	SOURCE_TYPE_GUILD_BATTLE_INSPIRE,
	// 公会战积分奖励
	SOURCE_TYPE_GUILD_BATTLE_SCORE,
	// 公会战匹配
	SOURCE_TYPE_GUILD_BATTLE_MATCH,
	// 加入公会
	SOURCE_TYPE_GUILD_MEMBER_REPLY_JOIN,
	// 公会踢人
	SOURCE_TYPE_GUILD_MEMBER_KICK,
	// 公会战胜利
	SOURCE_TYPE_GUILD_BATTLE_WIN,
	// 公会战失败
	SOURCE_TYPE_GUILD_BATTLE_LOSE,
	// 公会会长自动转让
	SOURCE_TYPE_GUILD_AUTO_CHANGE,
	// 公会捐献仓库
	SOURCE_TYPE_ADD_GUILD_STORAGE,
	// 公会战宣战
	SOURCE_TYPE_GUILD_BATTLE_CHALLENGE,
	// 公会战宣战退回
	SOURCE_TYPE_GUILD_BATTLE_CHALLENGE_RESULT,
	// 公会抽奖
	SOURCE_TYPE_GUILD_LOTTRY,
	// 公会宣战鼓舞
	SOURCE_TYPE_GUILD_BATTLE_CHALLENGE_INSPIRE,
	// 点卷换公会资金
	SOURCE_TYPE_GUILD_POINT_FUND,

	// 公会拍卖退款
	SOURCE_TYPE_GUILD_AUCTION_REFUND,
	// 公会拍卖分成
	SOURCE_TYPE_GUILD_AUCTION_BONUS,
	// 公会拍卖奖励
	SOURCE_TYPE_GUILD_AUCTION_REWARD,
	// 公会拍卖竞价
	SOURCE_TYPE_GUILD_AUCTION_BID,
	// 公会拍卖一口价
	SOURCE_TYPE_GUILD_AUCTION_FIX,
	// 公会徽记升级
	SOURCE_TYPE_GUILD_EMBLEM_UGRADE,
	// 公会领地每日奖励
	SOURCE_TYPE_GUILD_TERR_DAY_REWARD,

	// 公会最大
	SOURCE_TYPE_GUILD_MAX,

	// 红包
	SOURCE_TYPE_RED_PACKET = 13000,
	// 公会自费红包
	SOURCE_TYPE_RED_PACKET_GUILD_PAY,
	// 自费红包
	SOURCE_TYPE_RED_PACKET_CUSTOM,

	// 充值
	SOURCE_TYPE_BILLING = 14000,
	// 发货
	SOURCE_TYPE_BILLING_GOODS,
	//月卡
	SOURCE_TYPE_MONTH_CARD,

	//快速购买
	SOURCE_TYPE_QUICK_BUY = 15000,
	//通用快速购买
	SOURCE_TYPE_QUICK_BUY_COMMON,
	//复活
	SOURCE_TYPE_QUICK_BUY_REVIVE,

	//活动相关
	SOURCE_TYPE_ACTIVE = 16000,
	//日常活动
	SOURCE_TYPE_ROUTINE,
	//七日活动
	SOURCE_TYPE_SEVEN,
	//充值返利
	SOURCE_TYPE_CHARGE,
	//CDK
	SOURCE_CDK_GIVE,
	//道具兑换
	SOURCE_TYPE_ITEM_CHANGE,
	//新服冲级赛
	SOURCE_TYPE_NEW_SERVER_FIGHTING,
	// 七日签到
	SOURCE_TYPE_SIGN_IN_SEVEN,
	// 活动结束
	SOURCE_TYPE_ACTIVITY_CLOSE,
	// OPPO礼包
	SOURCE_OPPO_CDK,
	//夺宝活动购买商品
	SOURCE_TYPE_GAMBING_PAYING,
	// 夺宝活动开奖
	SOURCE_TYPE_GAMBING_LOTTERY,
	// 植树活动
	SOURCE_TYPE_PLANT,

	//赛季相关
	SOURCE_TYPE_SEASON = 17000,
	//赛季奖励
	SOURCE_TYPE_SEASON_REWARD = 17001,



	//PVP相关
	SOURCE_TYPE_PVP = 18000,
	//pvp战斗
	SOURCE_TYPE_PVP_RACE,
	//pvp活动
	SOURCE_TYPE_PVP_ACTIVITY,

	//宠物相关
	SOURCE_TYPE_PET = 19000,
	//开宠物
	SOURCE_TYPE_PET_ADD,
	//出售宠物
	SOURCE_TYPE_PET_SELL,
	//吞噬宠物
	SOURCE_TYPE_PET_DEVOUR,
	//喂养宠物
	SOURCE_TYPE_PET_FEED,
	//重选技能
	SOURCE_TYPE_PET_SELECT_SKILL,

	//赏金联赛
	SOURCE_TYPE_PREMIUM_LEAGUE = 20000,
	// 报名
	SOURCE_TYPE_PREMIUM_LEAGUE_ENROLL,
	// 重新报名
	SOURCE_TYPE_PREMIUM_LEAGUE_REENROLL,
	// 赏金联赛奖励
	SOURCE_TYPE_PREMIUM_LEAGUE_REWARD,
	// 8强通知
	SOURCE_TYPE_PREMIUM_LEAGUE_FINAL_EIGHT_NOTIFY,
	// 赏金联赛预选赛奖励
	SOURCE_TYPE_PREMIUM_LEAGUE_PRELIMINAY_REWARD,

	//抽奖
	SOURCE_TYPE_DRAW_PRIZE = 21000,

	//理财
	SOURCE_TYPE_MONEY_MANAGE = 22000,

	//积分赛
	SOURCE_TYPE_SCORE_WAR = 23000,
	//积分赛
	SOURCE_TYPE_2V2_SCORE_WAR = 23001,

	//师门系统相关
	//领取师门日常任务奖励
	MASTERSYS_RECV_DAILYTASK_RWD = 24000,
	//领取师门成长奖励
	MASTERSYS_RECV_ACADEMIC_RWD = 24001,
	//师门出师奖励
	MASTERSYS_RECV_FINSCH_RWD = 24002,
	//师傅出师获取良师值
	MASTERSYS_ADD_GOODTEACH = 24003,
	//徒弟升级获取成长值
	MASTERSYS_ADDGROWTH_UPLEVEL = 24004,
	//师徒组队通关地下城获取成长值 
	MASTERSYS_ADDGROWTH_DUNGEON_END = 24005,
	//师傅赠送获取成长值
	MASTERSYS_ADDGROWTH_MASTER_GIVE = 24006,
	//师徒关系解除
	MASTERSYS_RELIEVED = 24007,
	//拜师成功
	MASTERSYS_APPRENTICE = 24008,

	SOURCE_TYPE_MAX,
	//宝藏
	SOURCE_TYPE_DIG = 25000,
	//宝藏奖励
	SOURCE_TYPE_DIG_ADD_REWARD = 25001,


	// 赌马开奖
	SOURCE_TYPE_BET_HORSE_REWARD = 26000,
	// 赌马押注
	SOURCE_TYPE_BET_HORSE_STAKE = 26001,

	// 时装活动绝版兑换
	SOURCE_TYPE_CHANGE_FASHION_EXCHANGE = 26002,
	// 时装活动换装福利
	SOURCE_TYPE_CHANGE_FASHION_WELFARE = 26003,
	// 百变时装活动时装合成
	SOURCE_TYPE_CHANGE_FASHION_COMP = 26004,

	// 宝珠升级
	SOURCE_PRECIOUSBEAD_UPLEVEL = 26005,

	// 公会地下城挑战胜利
	SOURCE_TYPE_GUILD_DUNGEON_VICTORY = 26006,
	// 公会地下城参与奖励
	SOURCE_TYPE_GUILD_DUNGEON_ATTEND = 26007,
	
	// 神器罐子派奖
	SOURCE_TYPE_ARTIFACT_JAR_LOTTERY = 27003,

	//黑市商人竞拍成功
	SOURCE_TYPE_BLACKMARKET_AUCTION_SUCC = 28001,
	//黑市商人竞拍失败
	SOURCE_TYPE_BLACKMARKET_AUCTION_FAIL = 28002,
	//黑市商人申请竞拍交易
	SOURCE_TYPE_BLACKMARKET_AUCTION_REQ = 28003,

	// 冒险队
	SOURCE_TYPE_ADVENTURE_TEAM = 29000,
	// 冒险队改名
	SOURCE_TYPE_ADVENTURE_TEAM_RENAME = 29001,
	// 使用传承祝福
	SOURCE_TYPE_USE_INHERIT_BLESS = 29002,
	// 冒险队远征
	SOURCE_TYPE_ADVENTURE_TEAM_EXPEDITION = 29003,

	// 周常深渊失败返回门票
	SOURCE_TYPE_WEEKHELL_FAIL_RETURN_TICKET = 30001,

	// 月卡暂存箱
	SOURCE_TYPE_MONTH_CARD_DEPOSIT = 30002,
	// 月卡暂存箱到期
	SOURCE_TYPE_MONTH_CARD_DEPOSIT_EXPIRE = 30003,

	// 团本翻牌
	SOURCE_TYPE_TEAM_COPY_FLOP = 30010,
	// 团本扣除
	SOURCE_TYPE_TEAM_COPY_DEDUCT = 30011,
	// 团本冻结
	SOURCE_TYPE_TEAM_COPY_FROZEN = 30012,
	// 团本佣金分成
	SOURCE_TYPE_TEAM_COPY_COMMISSION_BONUS = 30013,
	// 团本消耗返还
	SOURCE_TYPE_TEAM_COPY_CONSUME_RETURN = 30014,
	// 强制投票结束返还
	SOURCE_TYPE_TEAM_COPY_FORCE_RETURN = 30015,
	// 团本roll奖励
	SOURCE_TYPE_TEAM_COPY_ROLL_REWARD= 30016,

	//冒险通行证
	SOURCE_TYPE_ADVENTURE_PASS = 30020,
	SOURCE_TYPE_ADVENTURE_PASS_BUY = 30021,
	SOURCE_TYPE_ADVENTURE_PASS_BUY_LEVEL = 30022,
	SOURCE_TYPE_ADVENTURE_PASS_REWARD = 30023,
	
	//飞升秘药
	SOURCE_TYPE_FLAY_UP = 30030,

	//地下城roll公共掉落奖励
	SOURCE_DUNGEON_ROLL_ADD_REWARD = 30040,

	//挑战者活动
	SOURCE_CHALLENGE_SCORE = 30050,

	//道具赠送
	SOURCE_ITEM_PRESENT_SCORE = 30051,

	//春季挑战
	SOURCE_SPRING_SCORE = 30060,

	//勇士招募
	SOURCE_HIRE_TASK = 30070,

	//吃鸡
	SOURCE_CHIJI_REWARD = 30080,

	//冠军赛
	SOURCE_CHAMPION_ENROLL = 30090,

	//迷失地牢
	//开宝箱
	SOURCE_LOSTDUNGEON_OPENBOX = 30100,
	//爬塔失败
	SOURCE_LOSTDUNGEON_FAIL = 30101,
	//爬塔结束领取奖励
	SOURCE_LOSTDUNGEON_REWARD = 30102,
	//爬塔消耗
	SOURCE_LOSTDUNGEON_CONS = 30103,

	//礼遇特权
	SOURCE_GIFT_RIGHT_CARD = 30110,

	//深渊特权
	SOURCE_DUNGEON_RIGHT_CARD = 30111,

	//武器打造
	SOURCE_WEAPON_BUILD = 30120,

	//金币寄售
	//卖金
	SOURCE_GOLD_CONSIGNMENT_SELL = 30200,
	//买金
	SOURCE_GOLD_CONSIGNMENT_BUY = 30201,
	//买金成交
	SOURCE_GOLD_CONSIGNMENT_BUY_DEAL = 30202,
	//卖金成交
	SOURCE_GOLD_CONSIGNMENT_SELL_DEAL = 30203,
	//玩家订单取消
	SOURCE_GOLD_CONSIGNMENT_CANCEL_ORDER = 30205,
	//订单超时
	SOURCE_GOLD_CONSIGNMENT_TIMEOUT_ORDER = 30206,
	//关闭区服务器
	SOURCE_GOLD_CONSIGNMENT_CLOSE_ZONE = 30207,
	//活动结束
	SOURCE_GOLD_CONSIGNMENT_ACTIVITY_END = 30208,
	//下架角色订单
	SOURCE_GOLD_CONSIGNMENT_CLOSE_ROLE = 30209,
	//手动下架订单
	SOURCE_GOLD_CONSIGNMENT_ADMIN_CLOSE = 30210,
	//信用点券到时转化
	SOURCE_CREDIT_POINT_TRANSFER= 30211,
	//异常补发收益
	SOURCE_GOLD_CONSIGNMENT_ABNORMAL_REWARD = 30212,
	//大富翁
	SOURCE_MONOPOLY = 30300,
	//大富翁出售祝福卡
	SOURCE_MONOPOLY_SELL_CARD = 30301,
	//通过道具获得
	SOURCE_MONOPOLY_ITEM = 30302,
};

enum AssetType
{
	ASSET_INVALID = 0,
	ASSET_GOLD = 1,
	ASSET_POINT,
	ASSET_EXP,
	ASSET_LEVEL,
	ASSET_SP,
	ASSET_WARRIOR_SOUL,
	ASSET_PK_COIN,
	ASSET_REVIVE_COIN,
	ASSET_BIND_GOLD,
	ASSET_BIND_POINT,
	ASSET_GUILD_CONTRI,
	ASSET_VIP_EXP,
	ASSET_VIP_LEVEL,
	ASSET_GOLDJAR_POINT,
	ASSET_MAGJAR_POINT,
	ASSET_FATIGUE,
	ASSET_PVP_SP,
	ASSET_APPOINTMENT_COIN,
	ASSET_MASTER_ACADEMIC_GROWTH,
	ASSET_MASTER_GOODTEACH_VALUE,
	ASSET_CHANGEFASHIN_SCORE,
	ASSET_WEAPON_LEASE_TICKETS,
	//地精纪念币
	ASSET_GNOME_COIN,
	//冒险队经验
	ASSET_ADVENTURE_TEAM_EXP,
	//冒险队等级
	ASSET_ADVENTURE_TEAM_LEVEL,
	//冒险队评级
	ASSET_ADVENTURE_TEAM_GRADE,
	//角色总评分
	ASSET_TOTAL_ROLE_SCORE,
	//赐福水晶（佣兵勋章）
	ASSET_BLESS_CRYSTAL,
	//赐福水晶（佣兵勋章）经验
	ASSET_BLESS_CRYSTAL_EXP,
	//传承祝福（成长药剂）
	ASSET_INHERIT_BLESS,
	//传承祝福（成长药剂）经验
	ASSET_INHERIT_BLESS_EXP,
	//赏金
	ASSET_BOUNTY,
	//吃鸡荣耀币
	ASSET_CHI_JI_COIN,
	//终极试炼代币1
	ASSET_ZJSL_WZHJJJ_COIN,
	//终极试炼代币2
	ASSET_ZJSL_WZHGGG_COIN,
	//商城积分
	ASSET_MALL_POINT,
	//冒险币
	ASSET_ADVENTURE_PASS_COIN,
	//招募硬币
	ASSET_HIRE_COIN,
	//荣誉
	ASSET_HONOR,
	//挑战者积分
	ASSET_CHALLENGE_SCORE,
	//冠军赛硬币
	ASSET_CAHMPION_COIN,
	//信用点券
	ASSET_CREDIT_POINT,
	//神秘卖场硬币
	ASSET_SECRET_COIN,
	//大富翁硬币
	ASSET_MONOPOLY_COIN,
	//大富翁骰子
	ASSET_MONOPOLY_ROLL,
	//装备回收积分
	ASSET_EQUIP_REC_SCORE,
};

// 各种功能类型
enum FunctionLogType
{
	// 装备强化
	FUNCTION_EQUIP_STRENGTHEN = 1,
	//随从解锁
	FUNCTION_RETINUE_UNLOCK,
	//随从洗练
	FUNCTION_RETINUE_CHANGE_SKILL,
	//随从升级
	FUNCTION_RETINUE_UPLEVEL,
	//随从升星
	FUNCTION_RETINUE_UPSTAR,
	//次元石解锁
	FUNCTION_WARPSTONE_UNLOCK,
	//次元石充能
	FUNCTION_WARPSTONE_CHARGE,
	//开罐子
	FUNCTION_OPEN_JAR,
	//附魔
	FUNCTION_ADD_MAGIC,
	//打造
	FUNCTION_EQUIP_MAKE,
	//触发礼包
	FUNCTION_ACTIVATE_GIFT,
	//装备增幅
	FUNCTION_EQUIP_ENHANCE,
};

#define REASON_PARAM_COUNT 3
#define REASON_TYPE_OFFSET 0
#define REASON_PARAM_ONE_OFFSET 1
#define REASON_PARAM_TWO_OFFSET 2
#define LOG_SOURCE_TYPE_RATIO	1000

inline std::string GetReason(LogSourceType type = SOURCE_TYPE_INVALID, UInt64 p1 = 0, UInt32 p2 = 0)
{
	char reasonBuff[MAX_LOG_LENGTH] = { 0 };

#ifndef AVALON_OS_WINDOWS
	snprintf(reasonBuff, sizeof(reasonBuff), "%u;%llu;%u", type, p1, p2);
#else
	_snprintf_s(reasonBuff, sizeof(reasonBuff), "%u;%llu;%u", type, p1, p2);
#endif
	reasonBuff[MAX_LOG_LENGTH - 1] = '\0';
	return std::string(reasonBuff);
}

struct ReasonInfo
{
	LogSourceType type;
	UInt64 param1;
	UInt32 param2;
};

inline bool ParseReason(const char* reason, ReasonInfo& param)
{
	std::vector<std::string> tmpVec;
	Avalon::StringUtil::Split(reason, tmpVec, ";");
	if (tmpVec.size() != REASON_PARAM_COUNT)
		return false;
	param.type = (LogSourceType)Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[REASON_TYPE_OFFSET]);
	param.param1 = Avalon::StringUtil::ConvertToValue<UInt64>(tmpVec[REASON_PARAM_ONE_OFFSET]);
	param.param2 = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[REASON_PARAM_TWO_OFFSET]);
	return true;
}

// add by huchenhui
//version|openId|zoneId|pf|did|model|deviceVersion|ip|playerId|accid|level|vipLevel|occu|monthCardTime|createTime|date|bornZoneId|isReturn|roleScore|roleTotalCharge|guildLvl
#define LOG_USERMSG_FORMAT "%s|%s|%u|%s|%s|%s|%s|%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|reason|objectId|dataId|itemType|itemSubType|itemName|number|oldNumber|nowNumber|itemQuality|strenth|equipType|enhanceType
#define LOG_ITEM_FORMAT "%s|%s|%llu|%u|%u|%u|%s|%d|%u|%u|%u|%u|%u|%u"

//userMsg|objectId|dataId|itemNum|itemType|itemSubType|quality|strengthen|qualityLevel|itemName|preciousId|preciousQuality|cardId|cardQuality|slot|equipType|magicId|magicLv|inscriptionId1|inscriptionQuality1|inscriptionId2|inscriptionQuality2|needLevel|equipScheme
#define LOG_EQUIP_LOGIN_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//罐子
//userMsg|jarId|combo|itemId|itemType|itemQuality|itemName|number
#define LOG_OPEN_JAR_FORMAT "%s|%u|%u|%u|%u|%u|%s|%u"

//userMsg|reason|AssetType|oldAsset|assetNum|nowAssetNum|nowAsset
#define LOG_ASSET_FORMAT "%s|%s|%u|%llu|%lld|%lld|%llu"

//userMsg|userName|OfflineTime|point|bindPoint|gold|bindGold|tower|adventureTeamLevel|adventureTeamGrade|roleCreateTime|honorLvl|honorExp
#define LOG_LOGIN_FORMAT "%s|%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|roleFileds
#define LOG_ACCOUNT_LOGIN_FORMAT "%s|%u"

//userMsg|deleteTime
#define LOG_DELETE_FORMAT "%s|%u"

//userMsg|userName|occu|roleId|preOccu|isAppointMentOccu
#define LOG_CREATE_FORMAT "%s|%s|%u|%llu|%u|%u"

//userMsg|onlinetime|point|bindpoint|gold|bindgold|masterType|resistMagic
#define LOG_LOGOUT_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneid|timeStr|pf|onlineNum|todayMaxOnlineNum|todayMaxOnlineTime|todayMinOnlineNum|todayMinOnlineTime
#define LOG_PCU_FORMAT "%s|%s|%s|%s|%s|%d|%d|%d|%d|%d"

//userMsg|taskId|type|dailyScore
#define LOG_TASK_SUBMIT_FORMAT "%s|%u|%u|%u"

//userMsg|taskId|type|subType
#define LOG_TASK_ACCEPT_FORMAT "%s|%u|%u|%u"

#define LOG_TASK_ABANDON_FORMAT "%s|%u|%u"

#define LOG_LEVEL_FORMAT "%s|%u"

//userMsg|gameVer|zoneId|time|pf|ip|roleId|accid|level|vipLevel|occu|raceId|dungeonId|dungeonType|dungeonName|hard|hellMode|usedTime|score|reviveCount|isAssist|shopId|isTeam|hpItemUsedNum|hellAutoClose
#define LOG_DUNGEON_FORMAT "%s|%s|%d|%d|%s|%s|%llu|%u|%u|%u|%u|%llu|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// userMsg|dungeonId|dungeonType|dungeonName|hellHardMode|isTeam|isHaveFriend|hellAutoClose
#define LOG_DUNGEON_START_FORMAT "%s|%u|%u|%s|%u|%u|%u|%u"

// userMsg|dungeonId|dungeonType|dungeonName|hellHardMode|areaId|usedTime
#define LOG_DUNGEON_CLEAR_AREA_FORMAT "%s|%u|%u|%s|%u|%u|%u"

// userMsg|raceId|dungeonId|areaId|itemTypeId|isTeam
#define LOG_DUNGEON_USE_POTION_FORMAT "%s|%llu|%u|%u|%u|%u"

// userMsg|dungeonId|itemId|color|itemLevel
#define LOG_DUNGEON_DROP_FORMAT "%s|%u|%u|%u|%u"

// userMsg|functionType|value1|value2
#define LOG_FUNCTION_FORMAT "%s|%u|%d|%d"

// userMsg|skillid|skillName|changeLevel|sourceLevel|newLevel
#define LOG_SKILL_CHANGE_FORMAT "%s|%u|%s|%d|%u|%u"

// userMsg|dungeonId|target
#define LOG_REVIVE_FORMAT "%s|%u|%llu|%u"

//userMsg|auctionType|itemId|itemDataId|itemNum|moneyType|price|deposit|duetime
#define LOG_AUCTION_UP_FORMAT "%s|%u|%llu|%u|%u|%u|%u|%u|%u"

//userMsg|playerId|downType|auctionType|itemId|itemDataId|itemNum
#define LOG_AUCTION_DOWN_FORMAT "%s|%llu|%u|%u|%llu|%u|%u"

//userMsg|auctionPlayerId|auctionType|srcItemId|itemId|auctionMainType|itemDataId|itemNum|moneyType|price|tax|abnormalDeal|strengthen|averPrice|onSealTime|itemQuality|sellerIp|sellerDeviceId|auctionPlayerAccId| auctionPlayerViplev |isTreas|sysRecomPrice|minPriceOnSale|transGuid|isAttented|itemAttentNum
//|mountMagicCardId|mountMagicCardAverPrice|mountBeadId|mountBeadAverPrice|eqQualityLv|remainSealCount|minPriceGuid|minPriceOnsalePrice|minPriceQualityLv|minPriceMountBeadId|minPriceMountBeadAverPrice|minPriceMountMagicId|minPriceMountMagicAverPrice|auctionPrice|auctionTransNum|auctionTransMax
#define LOG_AUCTION_BUY_FORMAT "%s|%llu|%u|%llu|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%s|%s|%u|%u|%u|%u|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneid|timeStr|pf|transGuid|punishAId|punishBId|punishCId|punishedId|minPriceOnsale|extraProfit|verifyEndTime|programAuditResult|state|isSysRecove
#define LOG_AUCTION_TREAS_TRANSACTION_PUNISH "%s|%s|%u|%u|%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneid|timeStr|pf|isTreas|itemDataid|itemNum|price
#define LOG_AUCTION_SYSRECOVER "%s|%s|%u|%u|%s|%u|%u|%u|%u"

//userMsg|StoneId|money
#define LOG_WARPSTONE_UNLOCK_FORMAT "%s|%u|%u"

//userMsg|StoneId|incEnergy|energy|oldEnergy|nowEnergy|oldLevel|nowLevel
#define LOG_WARPSTONE_CHARGE_FORMAT "%s|%u|%u|%u|%u|%u|%u|%u"

//userMsg|retinueId|dataId|
#define LOG_RETINUE_UNLOCK_FORMAT "%s|%llu|%u"

//userMsg|retinueId|dataId|retinueLevel
#define LOG_RETINUE_UPLEVEL_FORMAT "%s|%llu|%u|%u"

//userMsg|retinueId|dataId|lockbuff|oldbuff|newbuff
#define LOG_RETINUE_CHANGE_SKILL_FORMAT "%s|%llu|%u|%s|%s|%s"

//userMsg|activeId|taskId
#define LOG_ACTIVE_FINISH_FORMAT "%s|%u|%u"

//userMsg|shopId|shopItemId|num|newNum|dungeonId
#define LOG_SHOP_BUY_FORMAT "%s|%u|%u|%u|%u|%u"

//userMsg|shopId|shopItemId|costNum
#define LOG_SHOP_ITEM_FORMAT "%s|%u|%u|%u"

//userMsg|mallItemId|num
#define LOG_MALL_BUY_FORMAT "%s|%u|%u"

//userMsg|bootId
#define LOG_PLAYER_NEW_BOOT "%s|%u"

//userMsg|itemId|itemDataId|oldStrength|newStrength|code|pack|useUnbreak|isTicker|probability
#define LOG_STRENGTH_EQUIP_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|dungeonId|floor|usedSec
#define LOG_ENHANCE_EQUIP_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u"

//userMsg|type|itemId|itemDataId|itemLvel|sit|quality|enhanceLv|oldEnhanceType|newEnhanceType
#define LOG_ENHANCE_EQUIP_CHG_FORMAT "%s|%u|%llu|%u|%u|%u|%u|%u|%u|%u"

//userMsg|dungeonId|floor|usedSec
#define LOG_FIRST_CLEAR_DEATH_TOWER "%s|%u|%u|%u"

//userMsg|pvpType
#define LOG_MATCH_START_FORMAT "%s|%u"

//userMsg|zoneId|opponentId|type|seasonLevel|matchScore|usedTime
#define LOG_MATCH_SUCCESS_FORMAT "%s|%u|%llu|%u|%u|%u|%u"

//userMsg|type|seasonLevel|matchScore|matchTime
#define LOG_MATCH_CANCEL_FORMAT "%s|%u|%u|%u|%u"

//userMsg|targetZoneId|targetId|type|result|oldMatchScore|newMatchScore|oldPkValue|newPkValue|oldSeasonLevel|newSeasonLevel|oldSeasonStar|newSeasonStar|oldSeasonExp|newSeasonExp|seasonId|dungeonID|usedTime
#define LOG_PVP_END_FORMAT "%s|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|guildId|guildName|targetId|targetName|type|result|oldGuildScore|newGuildScore
#define LOG_GUILD_PVP_END_FORMAT "%s|%llu|%s|%llu|%s|%u|%u|%u|%u|%u"

//userMsg|level|passType
#define LOG_ADVENTURE_PASS_FORMAT "%s|%u|%u"

//userMsg|gameVer|zoneId|channge|time|roleId|accid|level|vipLevel|orderId|mallType|goodsId|itemId|itemNum|vipScore|money|timeoutTime
#define LOG_PAY_START_FORMAT "%s|%s|%u|%s|%u|%llu|%u|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneId|time|roleId|accid|level|vipLevel|orderId|mallType|goodsId|itemId|itemNum|vipScore|money|timeoutTime|result
//#define LOG_PAY_END_FORMAT "%s|%s|%u|%u|%llu|%u|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|orderId|mallType|goodsId|itemId|itemNum|vipScore|money|time|timeoutTime
#define LOG_PAY_END_FORMAT "%s|%s|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|itemSubType
#define LOG_WELFARE_CARD_FORMAT "%s|%u"

//合成附魔卡
//userMsg|card1|card2|targetCard|code
#define LOG_COMPOSE_MAGIC_CARD_FORMAT "%s|%u|%u|%u|%u"

//合成时装
//userMsg|fashion1|fashion2|commonFashion|skyFashion
#define LOG_COMPOSE_FASHION_FORMAT "%s|%u|%u|%u|%u"

//品阶调整
//userMsg|itemId|itemDataId|oldLevel|nowLevel|oldMatNum|newMatNum|oldPoint|newPoint|isFrenzy|eqColor|eqPost
#define LOG_REMAKE_EQUIP_LEVEL_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//打造
//#define 

//转职事件
//userMsg|mainOccu|preOccu|Occu
#define LOG_CHANGE_OCCU_FORMAT "%s|%u|%u|%u"

//宠物登陆
//userMsg|objId|dataId|petLevel|petExp|petType|slot
#define LOG_PET_LOGIN_FORMAT "%s|%llu|%u|%u|%u|%u|%u"

//增加宠物
//userMsg|objId|dataId|itemId|petType|petQuality
#define LOG_PET_ADD_FORMAT "%s|%llu|%u|%u|%u|%u"

//上阵宠物
//userMsg|objId|dataId|oldPetId|oldDataId
#define LOG_PET_BATTLE_FORMAT "%s|%llu|%u|%llu|%u"

//吞噬宠物
//userMsg|objId|dataId|oldLevel|newLevel|oldExp|newExp|exp|beDevourObjId|beDevourDataId|beDevourPetLevel|beDevourPetExp
#define LOG_PET_DEVOUR_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%llu|%u|%u|%u"

//宠物属性重选
//userMsg|objId|dataId|oldIndex|newIndex
#define LOG_PET_CHANGE_SKILL_FORMAT "%s|%llu|%u|%u|%u"

//创建房间
//userMsg|operationType|roomId|roomType|reason|createTime|playerSize
#define LOG_ROOM_FORMAT "%s|%u|%u|%u|%s|%u|%u"

//上报实时语音数据
//userMsg|type|voiceRoomId|joinTimeStamp
#define LOG_LIVE_VOICE_DATA_FORMAT "%s|%u|%s|%s"

//上报语音聊天数据
//userMsg|type|voiceId
#define LOG_CHAT_VOICE_DATA_FORMAT "%s|%u|%s"

//解散房间
//userMsg|roomId|roomType|deleteTime
//#define LOG_ROOM_CREATE_FORMAT "%s|%u|%u|%u"

// 勇者之塔开始扫荡
//userMsg|startFloor|endFloor
#define LOG_TOWER_START_WIPEOUT_FORMAT "%s|%u|%u"

// 勇者之塔重置
//userMsg|topFloor
#define LOG_TOWER_RESET_FORMAT "%s|%u"

//userMsg|gameVer|zoneid|timeStr|pf|monster_guid|monster_id|type|dungeon_id|scene_id|total_times|owner
#define LOG_CITY_MONSTER_FORMAT "%s|%s|%u|%u|%s|%llu|%u|%u|%u|%u|%u|%llu"

//拜师/收徒成功
//userMsg|discipleId|masterId|discipleLevel|masterLevel|discipleViplev|masterViplev|timestamp|eventType|masterOccu|discipleOccu
#define LOG_MASTERSYS_MAKE_MASTERDISCIPLE  "%s|%llu|%llu|%u|%u|%u|%u|%u|%u|%u|%u"

//师门出师
//userMsg|type|discipleId|masterId|discipleLevel|masterLevel|discipleGrowth|apprentTimeSec|finishTimeSec
#define LOG_MASTERSYS_FINISH_SCHOOL "%s|%u|%llu|%llu|%u|%u|%u|%u|%u"

//师傅发布师门任务
//userMsg|discipleId|masterId|timestamp
#define LOG_MASTERSYS_PUBISH_DAILYTASK "%s|%llu|%llu|%u"

//徒弟完成师门任务
//userMsg|discipleId|masterId|timestamp
#define LOG_MASTERSYS_COMP_DAILYTASK "%s|%llu|%llu|%u"

//领取师门日常任务礼包
//userMsg|type|uid|timestamp
#define LOG_MASTERSYS_RECEIVE_DAILYTASKGIFT "%s|%u|%llu|%u"

//师徒互动数据
//userMsg|discipleId|masterId|dungeonId|timestamp
#define LOG_MASTERSYS_INTERACTION "%s|%llu|%llu|%u|%u"

// 赌马场次随机射手
//userMsg|courtId|shooterId|timestamp
#define LOG_BET_HORSE_SHOOTER "%s|%u|%u|%u"

// 装备锁
//userMsg|opType
#define LOG_SECURITY_LOCK "%s|%u"

// 玩家作弊
//userMsg|reason|raceId|dungeonId
#define LOG_PLAYER_CHEAT "%s|%u|%llu|%u"

// 公会副本
//userMsg|dungeonId|dungeonName|dungeonLvl|score|totalDamage|bossDamage|usetime|reviveCnt|isTeam|guildName|guildLvl|bossOddBlood
#define LOG_GUILD_DUNGEON "%s|%u|%s|%u|%u|%lu|%lu|%u|%u|%u|%s|%u|%lu"

// 宝珠
//userMsg|beadOpType|objectId|itemId|srcBeadId|newBeadId|costItemId|costItemNum|isSuccess|param1|param2
#define LOG_PRECIOUS_BEAD "%s|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u"

// 物品租赁
//userMsg|itemId|oldAdventurerEmblem|oddAdventurerEmblem|costAdventurerEmblem
#define LOG_ITEM_LEASE "%s|%u|%u|%u|%u"

// 技能推荐
//userMsg|skillConfigType
#define LOG_SKILL_RECOMMEND "%s|%u"

// 技能槽位
//userMsg|skillConfigType|slotNum
#define LOG_SKILL_SLOT "%s|%u|%u"

// 黑市商人回收道具信息
// userMsg|gameVer|zoneid|timeStr|pf|auctId|auctType|itemDataId|recovePrice|recoveNum
#define LOG_BLACKMARKET_RECOVE_AUCTION_INFO "%s|%s|%u|%u|%s|%llu|%u|%u|%u|%u"

// 黑市商人最终回收信息
// userMsg|auctId|auctType|itemDataId|palyerId|itemId|recovedPrice
#define LOG_BLACKMARKET_RECOVED_INFO "%s|%llu|%u|%u|%llu|%llu|%u"

// 黑市商人交易信息
//  userMsg|opType|transid|auctId|auctType|auctItemDataId|auctItemId|auctPrice
enum BlackMarketOpType
{
	BMOT_REQ = 1,      //申请交易
	BMOT_CANCEL = 2,   //取消交易
};
#define LOG_BLACKMARKET_TRANSCATION "%s|%u|%llu|%llu|%u|%u|%llu|%u"

// 冒险队改名日志
//  userMsg|oldName|newName
#define LOG_ADVENTURE_TEAM_NAME_CHANGE "%s|%s|%s"

// 冒险队基本信息
// userMgr|adventureTeamLevel|adventureTeamName|blessCrystalNum|inheritBlessNum|bountyNum
#define LOG_ADVENTURE_TEAM_INFO "%s|%u|%s|%u|%u|%u|%u|%u"

// 派遣(取消)远征日志(falg: 1:派遣，2:取消)
//  userMsg|flag|mapId|duration|memberNum
#define LOG_EXPEDITION_TEAM "%s|%u|%u|%u|%u"

// 领取远征奖励日志
//  userMsg|mapId|duration|cond1|cond2|cond3|cond4|cond5
#define LOG_GET_EXPEDITION_TEAM_REWARDS "%s|%u|%u|%u|%u|%u|%u|%u"

// 抽神器罐子折扣日志
//  userMsg|opActId|discountRate|effectTimes
#define LOG_EXTRACT_ARTIFACT_JAR_DISCOUNT "%s|%u|%u|%u"

// 公会成员登陆
//  userMsg|GuildId|GuildName|GuildLevel|GuildMemberNum|EmblemLevel
#define LOG_GUILD_MEMBER_LOGIN "%s|%llu|%s|%u|%u|%u"

enum 
{
	UDP_GUILD_AUCTION_CREATE = 1,	// 公会拍卖创建
	UDP_GUILD_AUCTION_ABORATION = 2,	// 公会拍卖流拍
};
// 公会拍卖创建
//  userMsg|gameVer|zoneId|time|pf|opType|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum
// 公会拍卖流拍
//  userMsg|gameVer|zoneId|time|pf|opType|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum
#define LOG_GUILD_AUCTION "%s|%s|%d|%d|%s|%u|%u|%lu|%u|%lu|%u|%u|%u"

// 公会拍卖竞价
//  userMsg|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum|bidPrice|bidNum
#define LOG_GUILD_AUCTION_BID "%s|%u|%lu|%u|%lu|%u|%u|%u|%u|%u"

// 公会拍卖成交(dealType, 0代表竞拍, 1一口价)
//  userMsg|gameVer|zoneId|time|pf|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum|bidPrice|bidNum|dealType|playerId|vipLvl|level|Occu
#define LOG_GUILD_AUCTION_DEAL "%s|%s|%d|%d|%s|%u|%lu|%u|%lu|%u|%u|%u|%u|%u|%u|%lu|%u|%u|%u"

enum 
{
	UDP_HEAD_FRAME_LOGIN = 1,	// 登录头像框
	UDP_HEAD_FRAME_USE = 2,	// 穿戴头像框
};
// 头像框
//  userMsg|opType|headFrameId
#define LOG_HEAD_FRAME "%s|%u|%u"

// 充值推送
//  userMsg|pushId|price
#define LOG_RECHARGE_PUSH "%s|%u|%u"

enum
{
	UDP_ITEM_DEPOSIT_OBTAIN = 1,	// 暂存
	UDP_ITEM_DEPOSIT_GET = 2,	// 领取
	UDP_ITEM_DEPOSIT_EXPIRE = 3,	// 过期
	UDP_ITEM_DEPOSIT_REPLACE = 4,	// 替换
};
// 物品暂存箱
//  userMsg|opType|itemId|itemNum|itemColor|dungeonId
#define LOG_ITEM_DEPOSIT "%s|%u|%u|%u|%u|%u"

//	附魔卡升级
//	userMsg|isSuccess|oldId|oldLv|curId|curLv|consumeId|consumeNum|oldColor
#define LOG_ITEM_UP_MAGIC "%s|%u|%u|%u|%u|%u|%u|%u|%u"
// 吃鸡结算
//  userMsg|rank|survivalTime|reason
#define LOG_CHIJI_BALANCE "%s|%u|%u|%u"
// 吃鸡宝箱
// userMsg|battleId|playerId|boxId|param
#define LOG_CHIJI_BOX "%s|%u|%llu|%u|%u"
// 吃鸡陷阱
// userMsg|battleId|playerId|param
#define LOG_CHIJI_TRAPS "%s|%u|%llu|%u"
// 吃鸡角色
// userMsg|battleId|playerId|occu
#define LOG_CHIJI_OCCU "%s|%u|%llu|%u"

enum
{
	CHIJI_NPC_CREATE = 1, // 刷npc
	CHIJI_NPC_EXCHANGE = 2, // 兑换(挑战)
};

// 吃鸡npc刷出
// userMsg|battleId|npcType|opType|createNum
// 吃鸡npc兑换
// userMsg|battleId|npcType|opType|playerId|itemId|param
#define LOG_CHIJI_NPC "%s|%u|%u|%u|%llu|%u|%u"

// 吃鸡技能
// userMsg|battleId|playerId|skillId|skillLvl
#define LOG_CHIJI_SKILL "%s|%u|%llu|%u|%u"

// 吃鸡免战牌
// userMsg|battleId|playerId|oddNum|isUse
#define LOG_CHIJI_NO_WAR_CARD "%s|%u|%llu|%u|%u"

enum  GuildRedPacketOpType
{
	GUILD_REDPACKET_SEND = 1,  //发生红包
	GUILD_REDPACKET_RECV = 2,  //领取红包
};

// 公会红包
//  userMsg|redpacketId|opType|redpacketType|money|num|receiveState|senderId
#define LOG_GUILD_REDPACKET "%s|%llu|%u|%u|%u|%u|%u|%llu"

// 终极试炼爬塔
// userMsg|floor|topFloor|usedTime|score|dungeonId|dungeonBuff|inspireBuff
#define LOG_ZJSL_TOWER "%s|%u|%u|%u|%u|%u|%u|%u"

// 地下城通关时间
// userMsg|dungeonId|score|time|roleValueScore|dungeonValueScore
#define LOG_DUNGEON_CLEAR_TIME "%s|%u|%u|%u|%u|%u"


enum GuildMergerResult
{
	GUILD_MERGER_RESULT_CANCEL       = 0,
	GUILD_MERGER_RESULT_ACCEPT       = 1,
	GUILD_MERGER_RESULT_REFUSE       = 2,
	GUILD_MERGER_RESULT_SUCCESS      = 3,
	GUILD_MERGER_RESULT_SEND         = 4,
	GUILD_MERGER_RESULT_DISMISS_BY   = 5,
	GUILD_MERGER_RESULT_DISMISS_FAILD= 6,
	GUILD_MERGER_RESULT_DISMISS_YES  = 7,
	GUILD_MERGER_RESULT_DISMISS_MERGER=8,
};

//公会兼并
// userMsg|gameVer|zoneId|time|pf|guildID|guildName|guileLevel|memberNum|mermberLoginNum|LastWeekFund|result
#define LOG_GUILD_MERGER "%s|%s|%d|%d|%s|%llu|%s|%d|%d|%d|%d|%d"

//新月签到
// userMsg|signDays|repairSignDays|AwardRemainReapirNum|ActiviteRemainReapirNum
#define LOG_NEW_SIGN "%s|%u|%u|%u|%u"

//公会加入或退出
// userMsg|opType
#define LOG_GUILD_JOIN_EXIT "%s|%u"

// 客户端埋点
//userMsg|name|param1|param2|param3
#define LOG_CLIENT_LOG "%s|%s|%s|%s|%s"

// 套装激活
// userMsg|suitId|suitNum
#define LOG_SUIT_WEAR "%s|%u|%u"

// 团本开战
//userMsg|post|isGold|isOpenGold|teamId|model|playerNum|difficulty|preWarCfg|commission|equipScore|teamGrade|teamType
#define LOG_TEAM_COPY_START_BATTLE "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// 团本据点结算
// userMsg|teamId|squadId|stageId|fieldId|dungeonId|costTime|result|teamGrade|teamType
#define LOG_TEAM_COPY_FIELD_SETTLE "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// 团本阶段结束
// userMsg|teamId|stageId|costTime|result|teamGrade|teamType|changeSquadCnt|moveCnt|bossType|mainCityBlood
#define LOG_TEAM_COPY_STAGE_END "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// 团本翻牌
// userMsg|teamId|stageId|rewardId|rewardNum|limitType|teamGrade|teamType|param1
#define LOG_TEAM_COPY_FLOP "%s|%u|%u|%u|%u|%u|%u|%u|%u"

// 团本强制结束
// userMsg|isAgree|teamId|forceQuitNum|agreeNum|disAgreeNum|waiverNum|oddNum|isSuccess|oddTime|teamGrade|teamType
#define LOG_TEAM_FORCE_END "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// roll掉落信息
// userMsg|dungeonId|rollIndex|rollItem|rollItenNum|rollType|point
#define LOG_DUNGEON_ROLL "%s|%u|%u|%u|%u|%u|%u"

//添加好友
//userMsg|guildId|tlevel|tvipLevel|toccu|tguildId
#define LOG_ADD_FRIEND  "%s|%llu|%u|%u|%u|%llu"

//货币库存
//userMsg|gold|point|pkCoin|reviveCoin|bindGold|bindPoint|guildContri|vipExp|magjarPoint
#define LOG_MONEY_STOCK "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//货币库存（账号）
//userMsg|gnomePoint
#define LOG_MONEY_STOCK_ACC "%s|%u"

//装备转换
//userMsg|epicEquipId|equipDataId|part|equipType|strLev|beadId|magicId|inscriptId|equipNewDataId|newPart|convertType
#define LOG_EQUIP_CONVERT "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//团本作弊
//userMsg|cheatNum|forbidTime
#define LOG_TEAM_COPY_CHEAT "%s|%u|%u"

enum 
{
	ITEM_THING_START = 1,	// 开始事务
	ITEM_THING_CONMIT = 2,	// 开始事务
	ITEM_THING_ROLL_BACK= 3,	// 开始事务
	ITEM_THING_CANCEL = 4,	// 开始取消
};
//道具事务
//userMsg|opType|thingId|serialId|reason|itemCost
#define LOG_ITEM_THING "%s|%u|%llu|%llu|%s|%s"

//userMsg|isPreferences|roleLocation
#define LOG_ROLE_PREFERENCES "%s|%u|%u"
#endif
