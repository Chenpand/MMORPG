#ifndef _CL_SYSTEM_VALUE_DATAENTRY_H_
#define _CL_SYSTEM_VALUE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum SystemValueType
{
	SVT_INVALID = 0,
	SVT_FATIGUE_MAX = 1,						//每日疲劳值
	SVT_PKCOIN_MAX  = 2,						//每日决斗币上限
	SVT_RETINUE_SKILL = 3,						//随从洗练次数
	SVT_SHOP_REFRESH = 4,						//黑市默认刷新次数
	SVT_SOUTH_VALLEY = 5,						//南部溪谷默认次数
	SVT_RETINUE_SOULS = 6,						//1点卷可以换取的魂
	SVT_VIPLEVEL_MAX = 7,						//VIP最高等级
	SVT_DAILY_TASK_POINT = 8,					//每个日常的点卷
	SVT_WARPSTONE_MAXLEVEL = 9,					//次元石最高等级
	SVT_PKCOIN_BASENUM = 10,					//决斗币基础值
	SVT_MAIL_ITEM_MAXNUM = 11,					//邮件附带物品数量上限
	SVT_ONEKEY_DECOMPOSE_LV = 12,				//一键分解需要的等级
	SVT_GOLDAUCTION_DAILY_MAX_ONSALENUM = 13,	//金币拍卖行每日最高拍卖次数
	SVT_SKILL_DOWNLEVEL_COSTGOLD = 14,			//技能降级消耗金币基本数值
	SVT_CYCLE_TASK_MAX = 15,					//每轮循环任务个数
	SVT_CYCLE_TASK_DAILY_MAX = 16,				//循环任务每日最大循环数
	SVT_CYCLE_TASK_REFRESH_FREE_COUNT = 17,		//循环任务免费刷新次数
	SVT_CYCLE_TASK_REFRESH_CONSUME = 18,		//循环任务刷新消耗
	SVT_DEATH_TOWER_WIPEOUT_SEC = 19,			//死亡之塔每层扫荡耗时
	SVT_DEATH_TOWER_FINISH_WIPEOUT_COST = 20,	//死亡之塔每层扫荡加速消耗点劵
	SVT_STRENGTH_WP_COST_MOD = 21,				//武器强化消耗系数
	SVT_STRENGTH_DF_COST_MOD = 22,				//防具强化消耗系数
	SVT_STRENGTH_JW_COST_MOD = 23,				//首饰强化消耗系数
	SVT_ITEMAUCTION_MAX_ONSALENUM = 24,			//物品拍卖行同时上架个数上限
	SVT_STRCOLOR_ID = 25,						//有色晶体ID
	SVT_STRCOLORLESS_ID = 26,					//无色晶体ID
	SVT_STRPROTECT_ID = 27,						//强化保护券ID
	SVT_STRTOTEN_ID = 28,						//强10券ID
	SVT_BINDGOLD_ID = 29,						//绑金ID
	SVT_VIP_RESET_TOWER_COST = 30,				//vip购买勇者之塔重置次数消耗点劵
	SVT_SP_ITEM_ID = 31,						//技能点物品ID
	SVT_CHANGE_OCCU_SP_NUM = 32,				//转职获得技能点
	SVT_AWAKEN_SP_NUM = 33,						//觉醒获得技能点
	SVT_PACK_40 = 40,							//第50格背包消耗
	SVT_PACK_40_COSTITEMID = 41,
	SVT_PACK_50 = 50,							//第50格背包消耗
	SVT_PACK_50_COSTITEMID = 51,
	SVT_PACK_60 = 60,							//第60格背包消耗
	SVT_PACK_60_COSTITEMID = 61,
	SVT_PACK_70 = 70,							//第70格背包消耗
	SVT_PACK_70_COSTITEMID = 71,
	SVT_PACK_80 = 80,							//第80格背包消耗
	SVT_PACK_80_COSTITEMID = 81,
	SVT_PACK_90 = 90,							//第90格背包消耗
	SVT_PACK_90_COSTITEMID = 91,
	SVT_PACK_100 = 100,							//第100格背包消耗
	SVT_PACK_100_COSTITEMID = 101,
	SVT_GUILD_DONATE_GOLD_COST = 110,			//金币捐赠消耗
	SVT_GUILD_DONATE_GOLD_CONTRI = 111,			//金币捐赠获得贡献
	SVT_GUILD_DONATE_POINT_COST = 112,			//点劵捐赠消耗
	SVT_GUILD_DONATE_POINT_CONTRI = 113,		//点劵捐赠获得贡献
	SVT_GUILD_EXCHANGE_COST = 114,				//福利社兑换消耗贡献
	SVT_GUILD_CHANGE_NAME_COST = 115,			//修改公会名消耗点劵
	SVT_GUILD_MAIL_MAX_TIMES = 116,				//每日发送公会邮件次数
	SVT_GUILD_CREATE_COST = 117,				//创建公会消耗点劵
	SVT_GUILD_NOTICE_MAX_WORDS = 118,			//公告最大字数
	SVT_GUILD_DECLARATION_MAX_WORDS = 119,		//宣言最大字数
	SVT_GUILD_NAME_MAX_WORDS = 120,				//公会名最大字数
	SVT_GUILD_MAIL_MAX_WORLDS = 121,			//公会邮件最大字数
	SVT_GUILD_EXCHANGE_MAX_TIME = 122,			//公会福利社每日最大兑换次数
	SVT_GUILD_WORSHIP_TIME = 123,				//公会膜拜每日次数
	SVT_GUILD_PAY_RED_PACKET = 124,				//公会自费红包每日次数
	SVT_FRIEND_GIVE_TAKE_NUM = 125,				//好友赠送每日领取上限
	SVT_GUILD_MEMBER_BATTLE_COUNT = 126,		//公会战成员战斗次数
	SVT_GUILD_INSPIRE_SKILL_ID = 127,			//公会战鼓舞技能ID
	SVT_GUILD_INSPIRE_ENTER_TIME = 128,			//公会战新成员鼓舞时间
	SVT_GUILD_EVENT_SHOW_MAX = 129,				//公会日志最大显示条目
	SVT_WUDAO_MAX_WIN_NUM = 130,				//武道大会一轮最多胜利场数
	SVT_WUDAO_MAX_LOSE_NUM = 131,				//武道大会一轮最多失败场数
	SVT_WUDAO_TICKET_ID = 132,					//武道大会门票ID
	SVT_WUDAO_ACTIVITY_ID = 133,				//武道大会活动ID
	SVT_FASHION_COMPOSE_MATID = 134,
	SVT_GUILD_DONATE_GOLD_NUM = 135,			//公会金币捐献每日次数
	SVT_GUILD_CONTRI_FROM_FATIGUE = 136,		//消耗一点体力值获得的公会贡献
	SVT_GUILD_AUTO_CHAGE_LEADER_DAY = 137,		//会长连续几天不登录服务器后开启自动转让
	SVT_MONTH_CARD_VALID_DAY = 140,				//月卡有效时间
	SVT_MONTH_CARD_ADD_FATIGUE = 141,			//月卡添加的体力
	SVT_MONTH_CARD_ADD_POINT = 142,				//月卡添加的点劵
	SVT_MAKEUP_NEED_LV = 143,					//疲劳&副本找回奖励所需等级
	SVT_MONEY_MANAGE_CARD_ADD_POINT = 144,		//理财卡添加的点劵
	SVT_TEAM_INVITE_INTERVAL_SEC = 150,			//组队邀请间隔时间
	SVT_SEASON_WIN_STREAK_COUNT = 160,			//赛季连胜奖励场次
	SVT_SEASON_WIN_STREAK_EXP = 161,			//赛季连胜奖励经验
	SVT_AUCTION_MAX_PRICE_PERCENT = 170,        //拍卖行价格上限比例
	SVT_AUCTION_MIN_PRICE_PERCENT = 171,		//拍卖行价格下限比例	
	SVT_WORLD_CHAT_LV_LIMIT = 172,				//世界聊天等级限制
	SVT_AROUND_CHAT_LV_LIMIT = 173,				//附近聊天等级限制
	SVT_TEAM_CHAT_LV_LIMIT = 174,				//组队聊天等级限制
	SVT_EQUIPQL_MATID = 175,					//品级调整需要的材料ID
	SVT_EQUIPQL_MATNUM = 176,					//品级调整需要的材料数量
	SVT_EQUIPQL_MODE = 177,						//品级调整模式1.共用调整百分比,2.单条单独调整
	SVT_WORLDCHAT_ACT_NEED = 178,				//世界频道聊天需要活跃度值
	SVT_JAR_RECORD_QUALITY = 184,				//开罐子需要记录的物品品质
	SVT_JAR_RECORD_MAXNUM = 185,				//开罐子记录最大个数
	SVT_JAR_FIRST_OPEN_GET_ITEM = 186,			//首次开魔罐获得的道具
	SVT_JAR_FIRST_OPEN_GET_ITEM_NUM = 187,			//首次开魔罐获得的道具数量

	SVT_AUCTION_AUTO_REFRESH_SEC = 190,			//拍卖行自动刷新间隔秒数
	SVT_AUCTION_ON_SALE_WAIT_SEC = 191,			//拍卖行上架等待时间
	SVT_AUCTION_DEPOSIT_ITEM_ID = 192,			//拍卖行押金道具
	SVT_AUCTION_DEPOSIT_COST = 193,				//拍卖行押金

	SVT_FASHION_ATTRIBUTE_RESET_COST_ITEM_ID = 195, //时装属性重置石ID
	SVT_GOLDJARPOINT_RESET_DAY = 196,				//金罐子积分重置日
	SVT_GOLDJARPOINT_RESET_INTERVAL_TYPE = 197,		//金罐子积分重置间隔类型(1.月，2.周)
	SVT_MAGJARPOINT_RESET_DAY = 198,				//魔罐子积分重置日
	SVT_MAGJARPOINT_RESET_INTERVAL_TYPE = 199,		//魔罐子积分重置间隔类型(1.月，2.周)

	SVT_WUDAO_ACTIVITY_ID_BEGIN = 200,			//武道大会活动开始ID
	SVT_WUDAO_ACTIVITY_ID_END = 201,			//武道大会活动结束ID

	SVT_HORN_MIN_SEC = 205,						// 喇叭最短持续时间（保护时间）
	SVT_HORN_MAX_SEC = 206,						// 喇叭最长持续时间
	SVT_HORN_COMBO_WAIT_SEC = 207,				// 喇叭连击等待时间（秒）
	SVT_HORN_CONTENT_MAX_WIDTH = 208,			// 喇叭内容最大宽度
	SVT_HORN_ID = 209,							// 喇叭道具ID
	SVT_HORN_10_MIN_SEC = 210,					// 十连发喇叭最短持续时间（保护时间）
	SVT_HORN_10_MAX_SEC = 211,					// 十连发喇叭最长持续时间
	SVT_ROLE_DELETE_LIMIT_TIME = 212,			// 角色删除限制时间(天)
	SVT_ROLE_RECOVER_LIMIT_TIME = 213,			// 角色恢复限制时间(天)
	SVT_FATIGUE_MAKEUP_STEP = 214,				// 疲劳找回每次找回的疲劳步长

	SVT_PET_MAX_NUM = 220,						// 宠物上限
	SVT_PET_HUNGER_MAX_NUM = 221,				// 宠物饥饿度上限
	SVT_PET_POINT_FEED_CRITICAL_CHANCE = 222,	// 宠物点卷喂食暴击几率
	SVT_PET_POINT_FEED_CRITICAL_NUM = 223,		// 宠物点卷喂食暴击倍数
	SVT_PET_DEVOUR_GOLD_RATIO = 224,			// 吞食宠物金币消耗系数
	SVT_PET_DEVOUR_EXP_RATIO = 225,				// 吞食宠物经验获得系数
	SVT_PET_REST_INTERVAL_SEC = 226,			// 增加饱食度时间间隔
	SVT_PET_POINT_FEED_MAX = 227,				// 宠物点卷喂养最大次数
	SVT_PET_GOLD_FEED_MAX = 228,				// 宠物金币喂养最大次数
	SVT_PET_BATTLE_INTERVAL_SEC = 229,			// 消耗饱食度时间间隔


	SVT_MONTH_CARD_RACE_GOLD_RATE = 230,		// 月卡金币获得比例
	SVT_TIME_ITEM_DELETE_INTERVAL = 231,		// 时限道具删除时间
	SVT_PL_LMI_ID = 232,							//疲劳找回低级ID
	SVT_PL_HMI_ID = 233,							//疲劳找回高级ID
	SVT_SIGN_MONEY_ID = 234,						//签到货币ID
	SVT_CHANGE_PLAYERNAME_CARD = 235,					// 角色改名卡
	SVT_CHANGE_GUILDNAME_CARD = 236,					// 公会改名卡
	SVT_PET_HUNGRY_LIMIT = 240,							//宠物饥饿临界值
	SVT_AUTO_SETPOTION = 241,					// 自动设置药品
	SVT_RP_SIGHIN_COST_ITEMID = 242,			// 补签消耗得道具id
	SVT_RP_SIGHIN_COST_NUM	= 243,				// 补签消耗得数量
	SVT_SIGHIN_FREE_NUM = 244,					// 免费签到次数
	SVT_MASTER_EXP_ADDITION = 245,				// 师徒经验加成

	SVT_GUILD_BATTLE_CHALLENGE_ITEM_ID = 260,	//公会宣战物品ID
	SVT_GUILD_BATTLE_CHALLENGE_ITEM_NUM = 261,	// 公会宣战物品数量
	SVT_GUILD_BATTLE_CHALLENGE_TERR_ID = 262,	// 公会宣战领地ID
	SVT_GUILD_BATTLE_STATUE_SKILL_ID = 263,		// 公会战雕像技能ID
	SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID = 264,	// 公会战雕像技能ID

	SVT_CHEAT_FIRST_TIME_PUNISH_SEC = 270,		// 第一次作弊惩罚时间
	SVT_CHEAT_FOREVER_PUNISH_SEC = 271,			// 作弊永久封停时间

	SVT_GUILD_STORAGE_INIT_SIZE = 272,			// 公会仓库初始大小
	SVT_GUILD_STORAGE_OP_RECORD_MAXNUM = 273,	// 公会仓库操作记录最大数量
	SVT_NOTIFY_ITEM_ALREAY_DUE = 274,		// 通知时限道具即将到期时间(s)
	SVT_GUILD_POINT_FUND_NUM = 275,			// 每点券换取的公会资金

	SVT_TAP_SYSTEM_OPEN_LEVEL = 276,		//师徒系统开放等级
	SVT_TAP_SYSTEM_TEACHER_MIN_LEVEL = 277,		//玩家收徒弟最低等级
	SVT_TAP_SYSTEM_PUPIL_MAX_COUNT = 278,		//玩家最大徒弟个数
	SVT_TAP_SYSTEM_ANNOUNCE_MAX_LENGTH = 279,	//收徒公告最大字数
	SVT_TAP_SYSTEM_MASTER_MAX_COUNT = 280,		//玩家最大师傅个数
	SVT_TAP_SYSTEM_FRIEND_MAX_COUNT = 281,		//玩家最大好友个数
	SVT_TAP_SYSTEM_ADDONPAY_OVERDUE_TIME = 282,		//代付订单过期时间 (s)
	SVT_TAP_SYSTEM_DISCIPLE_USED_FATIGUE = 283,		//徒弟已使用的疲劳
	SVT_TAP_SYSTEM_MASTER_GET_FATIGUE = 284,		//师傅一次能获得的疲劳
	SVT_TAP_SYSTEM_MASTER_GET_FATIGUE_MAX = 285,		//师傅每日最大能获得的疲劳
	SVT_TAP_SYSTEM_MASTER_GIVE_MAX = 286,		//师傅赠送每日最大数量

	SVT_PREMIUM_LEAGUE_LEVEL = 290,				// 赏金联赛开放等级
	SVT_PREMIUM_LEAGUE_ENROLL_COST_ITEM = 291,	// 赏金联赛报名消耗的道具ID
	SVT_PREMIUM_LEAGUE_ENROLL_COST = 292,		// 赏金联赛报名消耗的道具数量
	SVT_PREMIUM_LEAGUE_REENROLL_COST = 293,		// 赏金联赛重新报名消耗点劵
	SVT_PREMIUM_LEAGUE_ENROLL_MAX_COUNT = 294,	// 赏金联赛最多报名次数
	SVT_PREMIUM_LEAGUE_REWARD_ITEM = 295,		// 赏金联赛奖励道具
	SVT_PREMIUM_LEAGUE_CHAMP_DEF_VALUE = 296,	// 赏金武道大会冠军默认奖励
	SVT_MGCBOX_FIRST_OPEN_GET_ITEM = 297,			//首次开魔盒获得的道具

	SVT_SIGN_IN_SEVEN_TASK_RE_SUBMIT_COST_ITEM = 298, //七日签到补签消耗道具
	SVT_SIGN_IN_SEVEN_TASK_RE_SUBMIT_COST_NUM = 299, //七日签到补签消耗数量

	SVT_OPEN_MAGIC_BOX_NEED_LV = 300, //开魔盒需要的等级

	SVT_ROOM_SLOT_MAX_SIZE = 310,				//房间队伍最大人数
	SVT_ROOM_ID_MIN = 311,						//最小房间号
	SVT_ROOM_ID_MAX = 312,						//最大房间号
	SVT_ROOM_REJECT_ENTER_TIME = 313,			//自动拒绝进入房间时间
	SVT_ROOM_REJECT_CHANGEPOS_TIME = 314,		//自动拒绝交换位置时间
	SVT_ROOM_CHANGEPOS_PROTECT_TIME = 315,		//发起交换位置间隔时间
	SVT_ROOM_REJECT_BATTLE_READY_TIME = 316,	//自动拒绝接受战斗准备时间
	SVT_MALL_VALID_DISCOUNT_NUM = 317,			//商城时装有效打折数量
	SVT_ROOM_KICKOUT_JOIN_TIME = 318,			//自动拒绝接受战斗准备时间
	SVT_REPLAY_MASTER_SCORE_LOWER_LIMIT = 319,	//高手对决录像分数下限
	SVT_3V3_ROOM_LIMIT_NUM = 320,				//3v3默认一页显示多少个房间
	SVT_DUELREDSHOW = 321,						//决斗场装备推荐显示最大等级装备

	SVT_NEW_YEAR_RED_PACKET_REWARD_NEED_NUM = 330,		//新年红包奖励需要个数
	SVT_NEW_YEAR_RED_PACKET_REWARD_ID = 331,			//新年红包奖励ID
	SVT_NEW_YEAR_RED_PACKET_TITLE_NEED_NUM = 332,		//新年红包称号奖励需要个数
	SVT_NEW_YEAR_RED_PACKET_TITLE_ID = 333,				//新年红包称号ID


	SVT_NEW_YEAR_RED_PACKET_ACTIVITY_ID = 334,	// 新年红包活动ID
	SVT_GOLD_TITLE_DUNGEON_GOLD_RATE = 335,		// 金币称号转换比例
	SVT_NEW_YEAR_RED_PACKET_SORT_LIST_ACTIVITY_ID = 336,	// 新年红包排行活动ID

	SVT_PREMIUM_LEAGUE_ENROLL_REWARD_ITEM = 340,		// 赏金联赛报名奖励道具
	SVT_PREMIUM_LEAGUE_ENROLL_REWARD_ITEM_NUM = 341,	// 赏金联赛报名奖励道具数量
	SVT_PREMIUM_LEAGUE_RACE_REWARD_ITEM = 342,			// 赏金联赛每局奖励道具
	SVT_PREMIUM_LEAGUE_EVERY_RACE_MAX_REWARD_NUM = 343,	// 赏金联赛每局最多奖励数量
	SVT_PREMIUM_LEAGUE_TOTAL_RACE_MAX_REWARD_NUM = 344,	// 赏金联赛比赛最多奖励数量
	SVT_PREMIUM_LEAGUE_REENROLL_COST_ITEM = 345,		// 赏金联赛重新报名消耗的道具ID

	SVT_APPOINTMENT_MAX_ROLE = 350,						//最多预约角色数量
	SVT_ONE_TOKEN_COIN_TO_FATIGUE_NUM = 351,			// 1代币可转换疲劳数
	SVT_REPLAY_LIST_TYPE_MASTER_WAIT_LIMIT = 352,		// 高手对决等待列表容量限制
	SVT_REPLAY_LIST_TYPE_MASTER_LIMIT = 353,			// 高手对决列表容量限制
	SVT_REPLAY_MASTER_SCORE_UPPER_LIMIT = 354,			// 高手对决录像分数上限
	SVT_PK_USE_ROBOT_WAIT_SEC = 355,					// pk时等待机器人时长(s)
	SVT_PK_GOLD_USE_ROBOT_WAIT_SEC = 356,				// 黄金段位pk时等待机器人时长(s)

	SVT_PET_SELECT_SKILL_COUNT_MAX = 360,				// 宠物技能重选次数上限
	SVT_PET_SELECT_SKILL_ITEM = 361,					// 宠物技能重选消耗物品ID
	SVT_PET_SELECT_SKILL_ITEM_NUM = 362,				// 宠物技能重选消耗物品数量

	SVT_EQUIPQL_PERF_MATID = 370,						//完美洗练品级调整需要的材料ID
	SVT_EQUIPQL_PERF_MATNUM = 371,						//完美洗练品级调整需要的材料数量

	SVT_EQUIP_TRANSFER_COLOR = 373,					//装备转移要求装备颜色

	SVT_EQUIP_RECO_TIMES = 374,						//准备回收一周最大次数
	SVT_EQUIP_RECO_UPSHB = 375,						//装备回收积分提升消耗货币类型
	SVT_EQ_DOUBLESWORD_OCC = 376,					//可使用双剑职业
	SVT_GAMBING_PARTICIPANT_LIST_CAPACITY = 378,		// 夺宝参与者列表容量
	SVT_GAMBING_BEGIN_ANNOUNCE_INTERVAL = 379,			// 夺宝发布公告距结束的时间段
	SVT_GAMBING_ANNOUNCE_INTERVAL = 380,				// 夺宝发布公告间隔
	SVT_EQUIP_RECO_SHUHUI_HBTYID = 383,						//装备回收装备赎回消耗货币类型id
	SVT_EQUIP_RECO_SHUHUI_HBNUM = 384,						//装备回收装备赎回消耗货币最低数量
	SVT_EQUIP_RECO_SHUHUI_PRICERATION = 385,			//装备回收装备赎回消耗货币数量计算比例

	SVT_AUCTION_RECENT_PRICE_NUM_MIN = 386,					//拍卖行近期平均价格计算最小次数
	SVT_AUCTION_RECENT_PRICE_NUM_MAX = 387,					//拍卖行近期平均价格计算最大次数
	SVT_AUCTION_RECENT_PRICE_TIME_SPAN = 388,				//拍卖行近期平均价格计算时间跨度
	SVT_GOLD_ITEM_DATA_ID = 389,							//金币ID
	SVT_CHARGE_UN_FREEZE_MONEY_RATE = 390,					//充值解冻金额倍率
	SVT_DAILY_TASK_SCORE_UN_FREEZE_REQUIREMENT = 391,		//日常任务活跃度解冻需求值
	SVT_DAILL_TASK_SCORE_UN_FREEZE_DAYS = 392,				//日常任务活跃度解冻天数
	SVT_FREEZE_TAIL_BASE_DAYS = 393,						//冻结保释期基础天数
	SVT_FREEZE_TAIL_ACCUMULATE_DAYS = 394,					//冻结保释期累加天数
	SVT_FREEZE_TAIL_MAX_DAYS = 395,							//冻结保释期最大天数

	SVT_CITY_MONSTER_DAILY_COUNT = 400,						//城镇怪物每日次数
	SVT_ONE_FATIGUE_TO_MAGPIE_BRIDGE_PROGRESS = 401,		//1疲劳可转鹊桥进度

	SVT_RELATION_RECOMMAN_COOLTIEM = 402,					//师傅徒弟推荐冷却时间
	SVT_RELATION_RECOMMAN_PLAYERNUM = 403,					//师傅徒弟推荐人数
	SVT_RELATION_FINISHLEVEL = 404,							//徒弟出师等级
	SVT_RELATION_MASTERDAILYTASK_NUM = 405,					//师门日常任务个数
	SVT_RELATION_REQMASTER_PUNISHTIME = 406,				//拜师惩罚时间
	SVT_RELATION_REQDISCIP_PUNISHTIME = 407,				//收徒惩罚时间
	SVT_RELATION_NOPUNISH_OFFLINETIME = 408,				//解除师徒关系不受惩罚离线时间
	SVT_MASTERSYS_AUTOFINISHSCH_OFFLINETIEM = 409,			//自动出师需要对方离线时间
	SVT_SCORE_WAR_BATTLE_COUNT = 410,						//积分赛战斗次数
	SVT_CLEARDUNGEN_ADDGROWTH_MAX = 411,				    //师徒组队通关地下城成长值每日增加上限值
	SVT_MASTER_GIVEEQ_ADDGROWTH_MAX = 412,				    //师傅装备赠送成长值每日增加上限值
	SVT_TAP_SYSTEM_APPRENT_LEVEL_MIN = 413,					//拜师最低等级
	SVT_TAP_SYSTEM_APPRENT_LEVEL_MAX = 414,					//拜师最高等级
	SVT_VETERANT_DEFINE_SERVICE_DAYS = 415,					//老兵定义开服天数
	SVT_VETERANT_DEFINE_ROLE_LEVEL = 416,					//老兵定义角色等级
	SVT_VETERANT_DEFINE_OFFLINE_DAYS = 417,					//老兵定义下线天数
	SVT_VETERANT_DEFINE_RETURN_COOLDOWN = 418,				//老兵定义回归cd
	SVT_VETERANT_RETURN_DAYS = 419,							//老兵回归天数

	SVT_TPSKYFASHION_PART_NOTCREATE_TIMES = 420,			//同一部位合出透明天空套时,接下来该部位不会再出透明天空套次数
	SVT_TPSKYFASHION_BASICS_RATE = 421,                     //透明天空套的合出基础概率，百分比
	SVT_TPSKYFASHION_REDUCE_RATE = 422,						//合出一件透明天空套,透明天空套的触发概率降低值
	SVT_TPSKYFASHION_INCRE_RATE = 423,						//合出一件正常天空套,透明天空套的触发概率增加值
	SVT_TPSKYFASHION_RATE_LOWERLIMIT = 424,				    //合出一件透明天空套,概率下线
	SVT_TPSKYFASHION_INCRE_UPPERLIMIT = 425,				//合出一件正常天空套,概率上线

	SVT_DIG_GRID_NUM = 430,									//挖宝格子数
	SVT_DIG_GOLD_ITEM_ID = 431,								//金铲子物品ID
	SVT_DIG_SILVER_ITEM_ID = 432,							//银铲子物品ID
	SVT_DIG_GOLD_BUY_ITEM_ID = 433,							//金挖宝点送的物品ID
	SVT_DIG_GOLD_BUY_ITEM_NUM = 434,						//金挖宝点送的物品数量
	SVT_DIG_SILVER_BUY_ITEM_ID = 435,						//银挖宝点送的物品ID
	SVT_DIG_SILVER_BUY_ITEM_NUM = 436,						//银挖宝点送的物品数量
	SVT_DIG_REFRESH_START_HOUR = 437,						//挖宝刷新开始时间
	SVT_DIG_REFRESH_END_HOUR = 438,							//挖宝刷新结束时间


	SVT_MASTERSYS_ANNOUNCE_SPAN = 440,						//师徒公告世界频道时间间隔
	SVT_MASTERSYS_DISCIPLE_TITLEID = 441,					//师徒拜师徒弟获得称号
	SVT_MASTERSYS_FINSCH_TITLEID = 442,						//师徒徒弟出师获得称号


	SVT_SCORE_WAR_LEVEL = 450,								//33活动等级

	SVT_BIND_DEVICE_MAX_NUM	= 455,							//绑定设备最大数量
	SVT_SECURITY_LOCK_FORCE_TIME = 456,						//安全锁强制解锁时间
	SVT_SECURITY_LOCK_PASSWD_ERROR_NUM = 457,				//安全锁密码错误次数

	SVT_CHFASH_ACTSCORE_SHOPMALL = 460,                     //换装积分 商城购买时装获取1点卷获取积分 百分百
	SVT_CHFASH_ACTSCORE_FASHCOMP = 461,                     //换装积分 时装合成1次天空套获取积分
	SVT_CHFASH_ACTSCORE_ACTFASHCOMP = 462,					//换装积分 活动时装合成（合成进击的勇士）时获取，合成1次获取积分
	SVT_3V3_TUMBLE_PLAYER_NUM	= 463,						// 3V3乱斗每队最少人数
	SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LIMIT = 466,		// 强化券融合强化券等级上限
	SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LOWER_LIMIT = 467,//强化券融合强化券等级下限
	SVT_STRENGTHEN_TICKET_FUSE_TOTAL_VALUE_FIX = 468,		//强化券融合总价值修正值
	
	SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT = 469,				// 公会副本地图开启公会等级限制
	SVT_GUILD_DUNGEON_PLAYER_LEVEL_LIMIT = 470,				// 公会副本地图挑战玩家等级限制
	
	SVT_SECURITY_LOCK_PASSWD_MIN_LENGTH	= 471,				//安全锁密码最小长度
	SVT_SECURITY_LOCK_PASSWD_MAX_LENGTH = 472,				//安全锁密码最大长度
	SVT_ROLE_BASE_FIELD = 473,								//角色基础栏位
	SVT_ROLE_EXTENSIBLE_BASE_FIELD = 474,					//角色可扩展栏位基础数量
	SVT_ROLE_EXTEND_PER_INC_NUM = 475,						//角色扩展单次增加数量

	SVT_GUILD_DUNGEON_VICTORY_CHAT_INTERVAL = 476,			// 公会地下城胜利公会频道消息间隔(秒)
	SVT_GUILD_DUNGEON_VICTORY_CHAT_CNT = 477,				// 公会地下城胜利公会频道消息次数
	SVT_GUILD_DUNGEON_LOTTERY_CHAT_COLOR = 478,				// 公会地下城抽奖高级道具消息

	SVT_WEAPON_LEASE_TICKETS_MAXBUN = 482,				// 武器好运符最大数量
	SVT_BLESS_CRYSTAL_OWN_MAX_NUM = 483,					// 赐福水晶拥有数量最大值
	SVT_BLESS_CRYSTAL_OWN_MAX_EXP = 484,					// 赐福水晶拥有经验最大值
	SVT_INHERIT_BLESS_OWN_MAX_NUM = 485,					// 传承祝福拥有数量最大值
	SVT_INHERIT_BLESS_OWN_MAX_EXP = 486,					// 传承祝福拥有经验最大值
	SVT_INHERIT_BLESS_UNIT_EXP_FOR_USE = 487,				// 传承祝福单元经验使用值
	SVT_INHERIT_BLESS_USE_REWARD_PLAYER_EXP = 488,			// 传承祝福使用奖励玩家经验值
	SVT_INHERIT_BLESS_USE_UNIT_EXP_REWARD_PLAYER_EXP = 489, // 传承祝福使用单元经验奖励玩家经验值
	SVT_BLESS_CRYSTAL_ITEM_DATA_ID = 496,					// 赐福水晶道具ID
	SVT_INHERIT_BLESS_ITEM_DATA_ID = 497,					// 传承祝福道具ID
	SVT_GNOME_COIN_ITEM_DATA_ID = 501,						// 地精纪念币道具ID

	SVT_AUCTION_TREAS_PUBLIC_TIME = 490,				// 拍卖行珍品公示时间长度(单位小时)
	SVT_AUCTION_ON_SALE_WAIT_SEC_LOWER = 491,			// 拍卖行非珍品上架等待时间下限(单位秒)
	SVT_AUCTION_ON_SALE_WAIT_SEC_UPPER = 492,			// 拍卖行非珍品上架等待时间上线(单位秒)
	SVT_AUCTION_RUSH_BUY_TIME = 493,					// 拍卖行珍品抢购时间长度(单位秒)
	SVT_AUCTION_QUERY_PAGE_MAXNUM = 498,				// 拍卖行一页最大显示数量
	SVT_AUCTION_TREAS_COOLTIME = 499,				// 拍卖行珍品交易冷却时间(单位天)
	SVT_AUCTION_NOT_TREAS_COOLTIME = 500,				// 拍卖行非珍品交易冷却时间(单位天)
	SVT_AUCTION_NOT_TREAS_ONSALE_TIME = 502,		//拍卖行非珍品上架时间长度(单位小时)
	SVT_AUCTION_TREAS_ONSALE_TIME = 503,			//拍卖行珍品上架时间长度(单位小时)
	SVT_AUCTION_CALC_AVERPRICE_MINTIMES = 504,			//拍卖行物品计算平均交易价格最低交易次数
	SVT_AUCTION_NOTTREAN_CALC_AVERPRICE_MAXTIMES = 505,			//拍卖行非珍品物品计算平均交易价格最大交易次数
	SVT_AUCTION_TREAN_CALC_AVERPRICE_MAXTIMES = 506,		//拍卖行珍品物品计算平均交易价格最大交易次数
	SVT_AUCTION_QUERY_TIME_INTERVAL = 507,				//拍卖行查询间隔时间
	SVT_AUCTION_DEPOSIT_COST_UPLIMIT = 508,				//拍卖行上架费上限
	SVT_CHARGE_UN_FREEZE_MONEY_RATE_TWO = 509,			//充值解冻金额倍率2
	SVT_TUIJIANJIAGE_BEISHU = 510,						//珍品道具上架价格倍数

	SVT_ARTIFACT_JAR_COUNT_20 = 512,					//20级神器罐派奖需要次数
	SVT_ARTIFACT_JAR_COUNT_30 = 513,					//30级神器罐派奖需要次数
	SVT_ARTIFACT_JAR_COUNT_40 = 514,					//40级神器罐派奖需要次数
	SVT_ARTIFACT_JAR_COUNT_50 = 515,					//50级神器罐派奖需要次数
	SVT_ARTIFACT_JAR_COUNT_60 = 516,					//60级神器罐派奖需要次数
	SVT_ARTIFACT_JAR_COUNT_65 = 517,					//65级神器罐派奖需要次数

	SVT_RELATION_REMOVEDISP_TIME = 519,					//师徒师傅剔徒弟保护时间

	SVT_GUILD_DUNGEON_COLD_TME = 520,					//公会地下城冷却时间
	SVT_GUILD_DUNGEON_SEC_MAX_DAMAGE = 521,				//公会地下城每秒最大伤害
	SVT_GUILD_DUNGEON_VERIFY_TIME = 522,				//公会地下城验证时间

	SVT_AUCTION_ATTENT_MAXNUM = 523,					//拍卖行关注多数量
	SVT_GUILD_SET_MAX_JOIN_LEVEL = 524,					//公会设置最大加入等级
	SVT_CTT_DUNGEON_TIMES_MAX = 525,					//循环任务副本相关最大次数
	SVT_CTT_SET_ITEM_TIMES_MAX = 526,					//循环任务上交物资最大次数
	SVT_CTT_STORY_TIMES_MAX = 527,						//循环任务对话最大次数

	SVT_RETURN_CONTINUE_LOGIN_DAY =528,					// 回归玩家连续为登录天数
	SVT_RETURN_NEED_ROLE_LEVEL = 529,					// 回归玩家要求角色等级
	SVT_ENERGY_COIN_ITEM_DATA_ID = 530,					// 精力商店消耗货币道具ID
	SVT_BLACKMARKET_FIXEDPRICE_VIP_RATE = 531,			//黑市商人固定价格收购vip分配额度
	SVT_BLACKMARKET_FIXEDPRICE_NOTVIP_RATE = 532,		//黑市商人固定价格收购非vip分配额度
	SVT_BLACKMARKET_BACKBUY_GOLD_UPPER_BOUND = 533,     //黑市商人收购金币上限上界
	SVT_BLACKMARKET_BACKBUY_GOLD_LOWER_BOUND = 534,     //黑市商人收购金币上限下界
	SVT_RETURN_HELL_DROP_GIFT = 535,					// 回归活动深渊掉落礼包
	SVT_RETURN_HELL_DROP_DAILY_MAX_NUM = 536,			// 回归活动深渊掉落礼包
	SVT_MAX_QUALITY_LEVEL_SCORE_COEFFICIENT = 537,		// 最大品级评分系数

	SVT_BLACKMARKET_BUYLIMIT_PROP	= 538,				// 黑市商人收购最大金币权重(占拍卖行税金)
	SVT_BLACKMARKET_PLAYER_LEV  = 539,					// 黑市商人玩家申请交易最低等级
	SVT_MERCENARY_FOUND_GIVE_RENAME_CARD = 545,			// 佣兵团创建赠送限时改名卡

	SVT_USE_INHERIT_BLESS_MIN_LV = 546,					// 传承祝福最小角色使用等级
	SVT_USE_INHERIT_BLESS_MAX_LV = 547,					// 传承祝福最大角色使用等级
	SVT_MERGE_SERVER_GIVE_RENAME_CARD = 548,			// 合服重名赠送限时佣兵团改名卡

	SVT_GUILD_AUCTION_TIME = 549,						// 公会拍卖时间(秒)
	SVT_GUILD_WORLD_AUCTION_TIME = 550,					// 公会世界拍卖时间(秒)
	SVT_GUILD_AUCTION_BID_TIME = 551,					// 公会拍卖竞价时间(秒)

	SVT_BLACKMARKET_PROP_GREEN = 552,					// 黑市商人绿装收购额度
	SVT_BLACKMARKET_PROP_PINK = 553,					// 黑市商人粉色收购额度

	SVT_GUILD_EMBLEM_ROLE_LEVEL = 556,					// 公会徽记角色等级
	SVT_GUILD_EMBLEM_GUILD_LEVEL = 557,					// 公会徽记公会等级
	SVT_GUILD_EMBLEM_HONOUR_LEVEL = 558,				// 公会徽记荣耀殿堂等级
	SVT_GUILD_EMBLEM_SKILL_PVP = 559,					// 公会徽记技能pvp

	SVT_AUCTION_PRICESLIST_NUM = 561,					// 拍卖行售价列表数量
	SVT_GUILD_AUCTION_PREPARE_TIME = 562,				// 公会拍卖准备时间(秒)
	SVT_GUILD_WORLD_AUCTION_LVL = 563,					// 公会世界拍卖等级
	SVT_GUILD_EMBLEM_SKILL_PVE = 565,					// 公会徽记技能pve

	SVT_AUCTION_RECOVE_TREANS_NUM = 566,				// 拍卖行珍品扫货每日上限
	SVT_AUCTION_RECOVE_NOTTREANS_NUM = 567,				// 拍卖行非珍品扫货每日上限

	SVT_WEEK_HELL_FAIL_RETURN_TICKETS = 568,			//周常深渊调整失败返回门票数量

	SVT_GUILD_AUCTION_BONUS_NUM = 569,					// 公会拍卖分成数量
	SVT_MONTH_CARD_DEPOSIT_TIME = 572,					// 月卡暂存箱有效时间(分)
	SVT_MONTH_CARD_DEPOSIT_GIRD = 573,					// 月卡暂存箱格子

	SVT_MAGIC_COMP_ONES_MAXTIMES = 575,					// 附魔卡一键合成最大次数

	SVT_ENHANCE_PROTECT_ID = 579,                       //增幅保护券ID

	SVT_EQUAL_PVP_LIMIT_LEVEL = 580,                    //公平竞技场等级限制

	SVT_STRONG_TEMP = 581,                              //强烈的痕迹ID

	SVT_SIGN_NEW_NOFREE = 582,							// 收费补签次数总数
	SVT_SIGN_NEW_FREE = 583,							// 免费补签次数总数
	SVT_SIGN_NEW_ACTIVITE = 584,						// 活跃度补签次数总数
	SVT_ADVENT_TEAM_SKILL_PVP = 585,					// 冒险团技能pvp
	SVT_ADVENT_TEAM_SKILL_PVE = 586,					// 冒险团技能pve

	SVT_ACTIVITE_SIGN_LIMIT = 587,						// 活跃度可补签下限
	SVT_TRIGGER_DUNGEON_HIDDEN_ROOM_PROB = 591,         // 触发隐藏房间概率

	SVT_BATTLE_NPC_EXCHANGE_EQUIP_NUM = 589,	// 吃鸡npc兑换装备数
	SVT_BATTLE_NPC_SURVIVAL_TIME = 590,			// 吃鸡npc存活时间

	SVT_BATTLE_NPC_TRADER_NUM = 592,			// 吃鸡npc商人数量
	SVT_BATTLE_NPC_MONSTER_NUM = 593,			// 吃鸡npc怪物数量

	SVT_GUILD_EMBLEM_SCORE_RATE = 594,			//公会徽记评分系数
	SVT_ADVENT_EMBLEM_SCORE_RATE = 595,			//佣兵团徽记评分系数	SVT_MAX,

	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID1 = 598,       //公平竞技场星期1开启控制活动ID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID2 = 599,       //公平竞技场星期2开启控制活动ID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID3 = 600,       //公平竞技场星期3开启控制活动ID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID4 = 601,       //公平竞技场星期4开启控制活动ID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID5 = 602,       //公平竞技场星期5开启控制活动ID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID6 = 603,       //公平竞技场星期6开启控制活动ID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID7 = 604,       //公平竞技场星期7开启控制活动ID

	SVT_AUCTION_ONSALE_DUETIME_REMAIN = 605,      //拍卖行期限非珍品物品上架至少剩余时间   
	SVT_STRENGTH_ASSIST_COST_MOD = 606,			  //辅助装备强化消耗系数

	SVT_GUILD_REDPACKET_ACC_RECV_DAY_MAX = 607,	  //公会红包账号日领取上限	 

	SVT_GUILD_MERGER_SPACE_DAY = 608,             //公会兼并间隔（天数）
	SVT_ZJSL_TOWER_FLOOR_OPEN_DAILY	= 609,		//终极试炼爬塔每天开放层数
	SVT_ZJSL_DUNGEON_TIMES_DAILY	= 610,		//终极试炼每天打同一关地下城的次数上限

	SVT_MONTH_CARD_DEPOSIT_TIPS_TIME = 611,       //月卡暂存提示时间(小时)

	SVT_MALL_INTEGRAL_NUM = 612,                  //商城积分数量上限
	SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE = 613,  //1点券转换商城积分系数

	SVT_12_SKILL_SLOT_TASK_ID = 614,			//开12技能槽完成的任务
	SVT_ADVENTURE_PASS_COIN = 616,              //冒险通行证道具ID

	SVT_ADVENTURE_PASS_UNLOCK = 617,            //冒险通行证截止前N天不解锁功能(天数)

	SVT_ADVENTURE_PASS_EXP = 624,               //冒险通行证额外经验包经验值

	SVT_RETURN_ANNIVERSARY_TITLE = 625,         //回归周年称号
	SVT_MALL_POINT_ITEM_ID = 626,               //商城积分道具ID

	SVT_REPORT_CONSUME = 628,					//举报消耗的活跃度值

	SVT_SKILL_PAGE_OPEN_CONSUME_NUM = 629,      //新技能标签页解锁道具数量
	SVT_SKILL_PAGE_OPEN_CONSUME_CURRENCY = 630, // 新技能标签页解锁消耗道具id

	SVT_DUNGEON_ROLL_TIME = 631,				//地下城roll掉落roll时间
	SVT_DUNGEON_ROLL_RESULT_TIME = 632,			//地下城roll掉落结果展示时间


	SVT_CHALLENGER_INTEGRAL_ITEM_ID = 634,      //挑战者积分物品ID

	SVT_ADVENTURE_PASS_EXP_ITME = 635,          //冒险通行证经验道具ID
	SVT_ADVENTURE_PASS_EXP_COIN = 636,          //冒险通行证经验转化货币比例
	SVT_INDPEND_ATK_PVP_RATIO = 637,			//独立攻击力PVP系数

	SVT_2V2_SCORE_WAR_LEVEL = 638,           //2v2乱斗积分赛等级限制
	SVT_2V2_SCORE_WAR_PARTAKE_NUM = 639,        //2v2乱斗积分赛参与次数
	SVT_2V2_SCORE_WAR_INITIAL_MATCH_TIME = 640, //2v2乱斗积分赛初始阶段匹配时间（秒）
	SVT_2V2_SCORE_WAR_MIDDLE_MATCH_TIME = 641,  //2v2乱斗积分赛中间阶段匹配时间（秒）
	SVT_2V2_SCORE_WAR_START_SCORE = 642,        //2v2乱斗积分赛初始分
	SVT_2V2_SCORE_WAR_WIN_SCORE = 643,        //2v2乱斗积分赛胜利分
	SVT_2V2_SCORE_WAR_LOSE_SCORE = 644,        //2v2乱斗积分赛失败分
	SVT_2V2_SCORE_WAR_MATCH_READY_TIME = 645,  // 2v2乱斗积分赛匹配计算间隔
	SVT_2V2_SCORE_WAR_TITLE_ID = 646,        //2v2乱斗积分赛头衔id
	SVT_2V2_SCORE_WAR_TITLE_TIME = 647,  // 2v2乱斗积分赛头衔持续时间

	SVT_PK_SEASON_1V1_HONOR_MAX = 648,  // 决斗场周荣誉上限

	SVT_CHALLENGER_SPRING_ITEM_ID = 649,      //春季挑战积分物品ID

	SVT_WEEK_SIGN_SPRING_DUNGEON_LV = 651,      //2020春季周签到地下城等级范围
	
	SVT_PLANT_PROF_MAX_VALUE = 652, //植树 熟练度最大值

	SVT_HONOR_EXP_RATE_1 = 653,		//荣耀经验比例1
	SVT_HONOR_EXP_RATE_2 = 654,		//荣耀经验比例2

	SVT_PK_CHIJI_HONOR_MAX = 659,  // 吃鸡周荣誉上限

	SVT_ADVENTURE_PASS_KING_ITEM = 660,  // 冒险通行证王者版解锁道具
	
	SVT_DL_MYSTERIOUS_SHOP_PROBABILITY = 661, //地牢神秘商店概率

	SVT_ROLE_STORAGE_UNLOCK_BEGIN	= 662,	//角色仓库解锁相关，起始
	SVT_ROLE_STORAGE_UNLOCK_END		= 678,	//角色仓库解锁相关，结束

	SVT_LOSTDUNG_DUNG_TEAM_MAX = 679,  // 爬塔地下城关卡队伍上限
	SVT_LOSTDUNG_DUNG_TEAM_PLAYER_MAX = 680,  // 爬塔地下城关卡队伍成员上限

	SVT_CAPTAIN_LEAVE_TEAM_TIME = 681, //队长自动离队时间(单位:秒)
	
	SVT_LOSTDUNG_DUNG_BATTLE_PROTECT_TIME = 682, //爬塔地下城战斗返回保护时间
	SVT_LOSTDUNG_DUNG_BIRTH_PROTECT_TIME = 684,  //爬塔地下城出生保护时间
	SVT_LOSTDUNG_DUNG_DEDUCTING_LIFE = 689,		//爬塔中途退出关卡扣除血量(当前血量百分比)
	SVT_LOSTDUNG_DUNG_TASK_PROTECT_TIME = 690, //爬塔地下城重领任务保护时间
	SVT_LOSTDUNG_DUNG_FADIAN_ITEM = 692, //爬塔上古法典道具id
	SVT_LOSTDUNG_DUNG_FADIAN_SCORE_ADD = 693, //爬塔上古法典积分加成比例
	SVT_LOSTDUNG_DUNG_REVIVE_STONE_ITEM = 694, //爬塔神器复活宝石id
	SVT_LOSTDUNG_DUNG_REVIVE_LUNHUI_ITEM = 695, //爬塔神器轮回之石id

	SVT_CHAMPION_RELIVE_ITEM_ID = 696, //冠军赛复活道具id
	SVT_CHAMPION_RELIVE_DIANQUAN_NUM = 697, //冠军赛复活点券数量

	SVT_CHAMPION_RELIVE_BETLIMIT_NUM = 698, //冠军赛竞猜上限
	
	SVT_LOSTDUNG_CANT_ENTER_TIME = 699, //爬塔结束前几秒不能再进入战场秒数
	SVT_LOSTDUNG_DUNG_DELAY_TIME = 700, //爬塔战场延迟结束时间
	SVT_LOSTDUNG_DUNG_DESTRUCTION_TIME = 701, //爬塔战场强制提出玩家时间
	SVT_LOSTDUNG_DUNG_QIBAO_ITEM = 702, //爬塔神器查看情报id

	SVT_ADVENTURE_TEAM_RENAME_COST = 704, //佣兵团改名消耗
	SVT_WEAPON_BUILD_UNIQUE_NOTIFY_SCORE = 705, //武器打造极品装备公告分数线

	SVT_PREFERENCES_ROLE_NUM = 706,//偏好角色个数上限

	SVT_ABYSS_TICKET_WAIVER = 707, //深渊票减免比列（当前深渊票百分比）
};

struct SystemValueDataEntry : public Avalon::DataEntry
{
public:
	SystemValueDataEntry();
	~SystemValueDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32 id;

	//数据类型
	SystemValueType type;

	//数据类型2
	SystemValueType type2;

	//数据类型3
	SystemValueType type3;

	//数值
	UInt32 value;

};

class SystemValueDataEntryMgr : public Avalon::DataEntryMgrBase<SystemValueDataEntry>, public Avalon::Singleton<SystemValueDataEntryMgr>
{
public:
	typedef std::map<SystemValueType, SystemValueDataEntry*> SystemValueMap;
public:
	SystemValueDataEntryMgr();
	~SystemValueDataEntryMgr();

public:
	bool AddEntry(SystemValueDataEntry* entry);

	void OnReload();

	void OnAddEntry(SystemValueDataEntry* entry);

	UInt32 GetSystemValue(SystemValueType type);

private:
	SystemValueMap	m_SystemValues;
};

// 系统开关表
struct SystemSwitchData : public Avalon::DataEntry
{
public:
	SystemSwitchData();
	~SystemSwitchData();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//系统枚举
	UInt32 id;

	//开关值
	UInt8 value;


};

class SystemSwitchDataMgr : public Avalon::DataEntryMgrBase<SystemSwitchData>
	, public Avalon::Singleton<SystemSwitchDataMgr >
{
public:
	SystemSwitchDataMgr();
	~SystemSwitchDataMgr();

	void GetCloseSysInfo(std::vector<SystemSwitchData*>& infos);
};

#endif //_CL_SYSTEM_VALUE_DATAENTRY_H_

