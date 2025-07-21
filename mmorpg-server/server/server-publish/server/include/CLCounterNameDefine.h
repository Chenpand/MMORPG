#ifndef __CL_COUNTER_NAME_DEFINE_H__
#define __CL_COUNTER_NAME_DEFINE_H__

// 上一次打的死亡之塔
#define COUNTER_TOWER					"tower"
// 今日死亡之塔最高层
#define COUNTER_TOWER_TOP_FLOOR			"tower_top_floor"
// 死亡之塔历史最高层
#define COUNTER_TOWER_TOP_FLOOR_TOTAL	"tower_top_floor_total"
// 死亡之塔历史最高层使用时间
#define COUNTER_TOWER_USED_TIME_TOTAL	"tower_used_time_total"
// 今天死亡之塔剩余次数
#define COUNTER_TOWER_RESET_REMAIN_TIMES "tower_reset_remain_times"
// 死亡之塔首通奖励领取情况
#define COUNTER_TOWER_AWARD_MASK		"tower_award_mask"


// 匹配积分最高分
#define COUNTER_BEST_MATCH_SCORE		"best_match_score"

// 金币拍卖次数
#define COUNTER_SELL_GOLD_NUM       "gold_auction_num"

//免费洗练
#define COUNTER_RETINUE_FREE_NUM	"retinue_free_num"

//洗练次数
#define COUNTER_RETINUE_SKILL_NUM	"retinue_skill_num"

//总共洗练次数
#define COUNTER_RETINUE_SKILL_TOTAL	"retinue_skill_total"

//世界/附近频道频繁发言计数
#define CHAT_HI_FRQ_COUNT			"speak_times"

//关卡每日次数(前缀)
#define DUNGEON_DAILY_COUNT_PREFIX	"dungeon_"
// 今日赠送次数
#define DAY_GIFT_COUNT				"gift_day"
//本月补签次数
#define SIGN_IN_RP				"signInRp"
//本月累计签到次数
#define SIGN_IN_TT				"signInTt"
// 今日从决斗场获得决斗币数量
#define COUNTER_DAY_PKCOIN_FROM_RACE "day_pkcoin"
// 今日从活动获得决斗币数量
#define COUNTER_DAY_PKCOIN_FROM_ACTIVITY "day_pkcoin_ac"
// 今日参加赛季PK次数
#define COUNTER_DAY_SEASON_PK "day_season_pk"

//积分每日奖励重置KEY
#define COUNTER_REWARD_MATCH_DIALY "reward_match_daily"

//剩余疲劳
#define COUNTER_LAST_FATIGUE	"last_fatigue"

//是否增加过月卡疲劳
#define COUNTER_IS_INC_MONTH_CARD_FATIGUE "is_inc_month_card_fatigue"

//活动相关
#define DAY_ONLINE_TIME			"5000_DayOnline"			//每日在线时长(s)
#define SIGN_IN_RP_COUNT		"3000_SIRp"					//签到补签次数
#define SIGN_IN_RP_FREE_COUNT	"3000_SIRpFree"				//签到免费补签次数
#define SIGN_IN_RP_COST			"3000_SIRpCost"				//签到补签消耗
#define SIGN_IN_TT_COUNT		"3000_SITt"					//签到总次数
#define DAY_SIGN_IN				"day_signin"				//每日签到数

#define SIGN_NEW_FLAG           "new_sign_flag"             //签到记录标志位
#define SIGN_ACTIVITE_COUNT     "sign_activite_count"       //活跃度已补签次数
#define SIGN_MONTH_COUNT        "sign_month_count"          //月已补签次数
#define SIGN_ACTIVITE_REMAIN_COUNT     "sign_activite_remain_count"       //活跃度剩余补签次数
#define SIGN_COLLECT_FLAG           "new_collect_flag"                    //累计签到领取奖励标志位
#define SIGN_ACTIVITE_HAVE     "sign_activite_have"                       //活跃度当日奖励补签次数是否已增加



// 运营活动相关
#define OPACT_COST_HELL_TICKETS "_cost_hell_tickets"		//运营活动内消耗深渊票数量(前缀+运营活动枚举)
#define OPACT_DARW_PRIZE_NUM "_draw_prize_num"				//运营活动内抽奖次数(前缀+运营活动枚举)
#define OPACT_USED_DARW_COUNT "_used_draw_cnt"				//运营活动内用掉的的抽奖次数(前缀+运营活动枚举)
#define OPACT_COST_FATIGUE "_cost_fatigue"					//运营活动内消耗疲劳量(前缀+运营活动id)
#define OPACT_EXCHANGED_TOKENS "_exchanged_tokens"			//已兑换过道具的代币数(前缀+运营活动id)
#define OPACT_FATIGUE_TOKENS "_fatigue_tokens"				//疲劳代币数量(前缀+运营活动id)
#define OPACT_MAGPIE_BRIDGE_DAILY_PROGRESS "_mgp_bdg_daily_prog" //七夕鹊桥每日进度(前缀+运营活动id)
#define OPACT_CHALLENGE_SCORE "_challenge_score"			//挑战者活动获得的积分
#define OPACT_CHALLENGE_TOTAL_SCORE "_challenge_total_score"//挑战者活动获得的总积分
#define OPACT_CHALLENGE_DAY_SCORE "_challenge_day_score"	//挑战者活动获得的当日积分
#define OPACT_SPRING_SCORE "_spring_score"			        //春季挑战活动获得的积分
#define OPACT_SPRING_TOTAL_SCORE "_spring_total_score"		//春季挑战活动获得的总积分
#define OPACT_WEEK_SIGN_SPRING_NUM "_week_sign_spring_num"  //2020春季周签到累计次数
#define OPACT_HIRE_TASK_GUILD_DUNGEON_NUM "_hire_task_guild_dungeon_num"  //招募公会地下城当天次数
#define OPACT_GIFT_RIGHT_CARD "_gift_right_card"			//礼遇特权卡权限
#define OPACT_GIFT_RIGHT_CARD_BUY_TIME "_gift_right_card_buy_time"			//礼遇特权卡购买时间
#define OPACT_GIFT_RIGHT_CARD_LOGIN_TOTAL "_gift_right_card_login_total"	//礼遇特权卡累计登陆


// 每日Vip深渊免费次数
#define COUNTER_VIP_FREE_HELL_TIMES "vip_free_hell_times"

// 每日Vip黑市商店刷新次数
#define COUNTER_VIP_SHOP_FRESH_TIMES "vip_shop_refresh_times"

//每日Vip免费复活次数
#define COUNTER_VIP_REVIVE_TIMES "vip_revive_times"

//VIP购买礼包的计数BIT
#define COUNTER_VIP_GIFT_BUY_BIT "vip_gift_buy_bit"

//VIP免费打开黄金宝箱次数
#define COUNTER_VIP_FREE_GOLD_CHEST_TIMES "vip_gold_chest_times"

//VIP购买死亡之塔额外次数
#define COUNTER_VIP_TOWER_PAY_TIMES "vip_tower_pay_times"

//开罐子数量
#define COUNTER_OPEN_JAR_T1		"open_jar_t1"
#define COUNTER_OPEN_JAR_T2		"open_jar_t2"
#define COUNTER_OPEN_JAR_T3		"open_jar_t3"
#define COUNTER_OPEN_JAR_DAY	"open_jar_day"

//使用蓝色附魔卡次数
#define COUNTER_BLUE_MAGIC_CARD		"blue_mc"
//使用紫色附魔卡次数
#define COUNTER_PP_MAGIC_CARD		"purple_mc"

//连续强化失败次数Strengthen
#define COUNTER_STRENGTHEN_FAIL_COUNT "strengthen_fail_count"

//PVP连败
#define COUNTER_PVP_STRAIGHT_COUNT	"pvp_straight_count"

//每轮循环任务计数
#define COUNTER_CYCLE_TASK_COUNT	"cycle_task_count"

//每轮循环任务各种类型任务次数前缀
#define COUNTER_CYCLE_TASK_TYPETIMES_PER "cycle_task_count_type_"

//循环任务最近任务类型
#define COUNTER_CYCLE_TASK_LATEST_TYPE "cycle_task_latest_type"

//累积的循环次数
#define COUNTER_CYCLE_TASK_DAILY_COUNT "cycle_task_daily_count"

//循环任务刷新次数
#define COUNTER_CYCLE_REFRESH_COUNT "cycle_refresh_count"

//收取好友赠礼次数,日更
#define COUNTER_GET_FRIEND_GIVE "get_friend_give"

//时装合成
#define FASHIONSKY_COMMON_WT	"fs_sky_com_wt"
#define FASHIONSKY_ADVANCE_WT	"fs_sky_ad_wt"
#define FASHION_COMPOSE_NUM		"fs_cps_num"

// 公会兑换次数
#define COUNTER_GUILD_EXCHANGE "guild_exchange"
// 公会兑换冷却时间
#define COUNTER_GUILD_EXCHANGE_COLDTIME "guild_exchange_cold"
// 公会膜拜
#define COUNTER_GUILD_ORZ "guild_orz"
// 公会圆桌会议（首次）
#define COUNTER_GUILD_TABLE "guild_table"
// 公会圆桌会议（有偿协助）
#define COUNTER_GUILD_TABLE_HELP "guild_table_help"
// 公会邮件
#define COUNTER_GUILD_MAIL "guild_mail"
// 公会解散计数
#define COUNTER_GUILD_DISMISS "guild_dismiss"
// 公会点劵捐赠次数
#define COUNTER_GUILD_DONATE_POINT "guild_donate_point"
// 公会金币捐赠次数
#define COUNTER_GUILD_DONATE_GOLD "guild_donate_gold"
// 公会自费红包数量
#define COUNTER_GUILD_PAY_RED_PACKET "guild_pay_rp"
// 公会兼并
#define COUNTER_GUILD_MERGER_RED_POINT "guild_merger_red_point"

// 今日武道大会参加次数
#define COUNTER_WUDAO_TIMES "wudao_times"
// 武道大会胜利场数
#define COUNTER_WUDAO_WIN_NUMS "wudao_win"
// 武道大会失败场数
#define COUNTER_WUDAO_LOSE_NUMS "wudao_lose"

//道具每日使用限制次数 后缀为道具ID
#define ITEM_DAYUSE_PRE	"item_dayuse_"
//道具每周使用限制次数(周六刷新) 后缀为道具ID
#define ITEM_WEEK6USE_PRE	"item_week6use_"
//时装部位权重计数 后缀为部位ID
#define FASHION_WEIGHT_PRE "fashion_wt_"
//时装部位合成次数 后缀为部位ID
#define FASHION_COMPOSE_PRE "fashion_cp_"

//黑市商店刷新次数
#define SHOP_REFRESH_NUM	"shop_refresh_num"

//赛季本阶段战斗次数
#define SEASON_BATTLE_NUM	"season_battle_num"

//赛季最高段位
#define SEASON_MAX_LEVEL	"season_max_level"

//赛季连胜
#define SEASON_WIN_STREAK	"season_win_streak"

//赛季属性对应段位
#define SEASON_ATTR_LEVEL	"season_attr_level"

//强化失败计数
#define STRENGTH_FAIL_NUM "str_fail_count_"
//强化上一次失败修正2系数(*1000)
#define STRENGTH_FAIL_FIX2 "str_fail_fix2_"

//增幅失败计数
#define ENHANCE_FAIL_NUM "enhance_fail_count_"
//增幅上一次失败修正2系数(*1000)
#define ENHANCE_FAIL_FIX2 "enhance_fail_fix2_"

//每日活跃度
#define DAILY_ACTIVE_SCORE "daily_act_score"
//每日世界频道免费聊天次数
#define DAILY_WORLD_CHAT_FREE "daily_free_wchat"

//罐子下一次使用免费时间
#define JAR_NEXT_USE_FREE_TIME "jar_next_use_free"
//罐子下一次免费使用次数
#define OPEN_MAGJAR_FREE_NUM	"mag_jar_free_num"
//是否第一次开魔罐
#define FIRST_OPEN_MAGJAR	"first_open_magjar"
//是否第一次开魔盒
#define FIRST_OPEN_MAGBOX	"first_open_magbox"

//活动罐子折扣次数前缀
#define ACTJAR_DIS_NUM_PRE	"jar_buy_dis_remain_"

//赛季PVP每日连胜或连败
#define COUNTER_SEASON_PVP_STRAIGHT_COUNT	"pvp_season_straight_count"

//33活动每日连胜或连败
#define COUNTER_SCORE_WAR_STRAIGHT_COUNT	"score_war_straight_count"

//地下城每日必掉前缀
#define DUNGEON_DAILY_DROP "dungeon_daily_"

//通知是否读过
#define READ_NOTIFY "read_notify_"

//上次自然天跨天时间戳
#define LAST_DAYCHANGE_TIME "last_daychange_t"

//上次逻辑跨天时间戳
#define LAST_LGC_DAYCHANGE_TIME "last_lgc_daychange_t"

//下次逻辑跨周时间戳
#define NEXT_LGC_WEEKCHANGE_TIME "next_lgc_weekchange_t"

//师徒返利徒弟疲劳消耗点数
#define DISCIPLE_USED_FATIGUE	"disciple_used_fatigue"

//师傅每日疲劳补偿上限
#define MASTER_DAY_FATIGU_MAX	"master_fatigu_max"
//时装部位合成次数
#define FASHION_CMPS_PRE "fashion_cmps_3_"
//上一次合到时装得类型 LastGetSkyFashionType 后缀部位
#define GET_SKY_FASHION_TYPE "get_sky_fashon_t_3_"
//天空套获得的数量 后缀套装id+普通黄金类型
#define SKY_FASHOPN_SUIT_NUM "sky_fs_suit_num_3_"
//天空套获得的id
#define GET_SKY_FASHION_ID "get_sky_fs_3_"

//L概率下天空套合出的次数
#define L_SKY_GET_NUM	"l_sky_get_num_3"
//L概率下N轮内天空套合成情况
#define L_SKY_GET_INFO	"l_sky_get_info_3"

//玩家购买时装数量
#define PLAYER_BUY_MALL_FASHION_NUM "buy_mall_fashion_num"
//时装合成记录
#define PLAYER_FASHION_MERGE_RECORD "record_fashion_merge"

//正常天空套连续产出次数
#define  PLAYER_SKY_CONTI_NUM  "sky_conti_produce_num_3"
//透明天空套连续产出次数
#define  PLATER_TPSKY_CONTI_NUM "tpsky_conti_produce_num_3"
//部位在合出透明天空套影响下合出正常天空套次数
#define  PLAYER_SKY_COMPNUM_IN_PRODUCE_TPSKY "sky_com_num_tpsky_3_"

//新年红包
#define NEW_YEAR_RED_PACKET_NUM "new_year_red_packet_num"

//预约币
#define COUNTER_APPOINTMENT_COIN "app_coin"

//pvp技能设置
#define PVP_SKILL_CONFIG "pvp_skill_config"

#define EQUAL_PVP_SKILL_CONFIG "equal_pvp_skill_config"

#define EQUAL_PVP_SKILL_HAVE_SP "equal_pvp_skill_have_sp"

//装备回收本周次数
#define EQUIP_RECO_TIMES "equip_reco_times"
//装备回收积分
#define EQUIP_RECO_SCORE "equip_reco_score"
//装备回收积分奖励罐子前缀
#define EQUIP_RECO_REWARD_PER "eqreco_jarstate_"
//装备回收本周积分提升次数
#define EQUIP_RECO_UPSC_TMS "eqreco_upsc_times"
//装备回收状态重置时间戳 
#define EQUIP_RECO_RESET_TM "eqreco_reset_tm"

//公会战领地加成图次数
#define COUNTER_GUILD_TERR_DUNGEON_TIMES "guild_terr_dungeon_num"

//异常交易每日保释天数
#define ABNORMAL_TRANS_DAILY_TAIL_DAYS "abnm_trans_daily_tail_days"
//异常交易总保释天数
#define ABNORMAL_TRANS_TOTAL_TAIL_DAYS "abnm_trans_total_tail_days"
//异常交易开启不返还处理时间
#define ABNORMAL_TRANS_OPEN_NEVEN_BACK_HANDLE_TIME "abnm_trans_open_never_back_t"

// 城镇怪物今日以打次数
#define COUNTER_CITY_MONSTER_DAILY_TIMES "cm_times"

// 每日师徒组队通关地下城增加成长值
#define CLEAR_DUNGEON_ADDGROWTH_VALUE "team_cleardungeon_addgrowth"
// 每次徒弟师傅装备赠送成长值
#define MASTER_GIVEEQUIP_ADDGROWTH "master_giveequip_addgrowth"

// 循环任务当天次数
#define COUNTER_CYCLE_TASK_DAILY_TOTAL_COUNT "cycle_task_daily_total_count"

// 活动积分前缀
#define ACTIVE_SCORE_PER "active_score_"

// 活动积分消耗前缀
#define ACTIVE_SCORE_CONSUME_PER "active_consume_score_"

// 百变时装活动时装部分合成前缀
#define CHANGEFASH_ACT_PART_MERGED_PER "changefash_act_part_mergerd_"

// 刷新租赁商店
#define REFRESH_LEASE_SHOP "refresh_lease_shop"

// 神器罐子活动
#define ARTIFACT_JAR_BUY "jarfact_jar_buy_"

// 头像框补发
#define HEAD_FRAME_COMPENSATE	"head_frame_compensate"

// 月卡暂存时间
#define MONTH_CARD_DEPOSIT	"month_card_deposit"

// 关卡进度保存
#define COUNTER_DUNGEON_PROCESS_PREFIX	"dp_"

// 累计签到相关地下城每日次数
#define COUNTER_CUMULATEPASS_DUNGEON "act_dungeon"

// 吃鸡荣耀币
#define COUNTER_CHI_JI_COIN "chi_ji_coin"

// 吃鸡场景商店货币(仅在吃鸡scene使用)
#define CHIJI_SHOP_COIN_NAME	"chi_ji_shop_coin"

// 终极试炼历史最高层
#define COUNTER_ZJSL_TOP_FLOOR_TOTAL	"zjsl_top_floor_total"
// 终极试炼每周最高层
#define COUNTER_ZJSL_TOP_FLOOR			"zjsl_top_floor"
// 终极试炼每天挑战次数
#define COUNTER_ZJSL_CHALLENGE_TIMES	"zjsl_challenge_times"
// 终极试炼关卡进入次数
#define COUNTER_ZJSL_DUNGEON_TIMES		"zjsl_dungeon_times"
// 终极试炼关卡效果
#define COUNTER_ZJSL_DUNGEON_BUFF		"zjsl_dungeon_buff"
// 终极试炼关卡效果目标（玩家OR怪物）
#define COUNTER_ZJSL_DUNGEON_BUFF_TARGET "zjsl_dungeon_buff_target"
// 终极试炼关卡效果层数
#define COUNTER_ZJSL_DUNGEON_BUFF_FLOOR	"zjsl_dungeon_buff_floor"
// 终极试炼鼓舞BUFF
#define COUNTER_ZJSL_INSPIRE_BUFF		"zjsl_inspire_buff"
// 终极试炼鼓舞BUFF第几天
#define COUNTER_ZJSL_INSPIRE_BUFF_DAY	"zjsl_inspire_buff_day"
// 终极试炼个人生命
#define COUNTER_ZJSL_PLAYER_HP			"zjsl_player_hp"
// 终极试炼个人魔力
#define COUNTER_ZJSL_PLAYER_MP			"zjsl_player_mp"
// 终极试炼BOSS1的ID
#define COUNTER_ZJSL_BOSS1_ID			"zjsl_boss1_id"
// 终极试炼BOSS2的ID
#define COUNTER_ZJSL_BOSS2_ID			"zjsl_boss2_id"
// 终极试炼BOSS1生命
#define COUNTER_ZJSL_BOSS1_HP			"zjsl_boss1_hp"
// 终极试炼BOSS2生命
#define COUNTER_ZJSL_BOSS2_HP			"zjsl_boss2_hp"
// 终极试炼代币1
#define COUNTER_ZJSL_WZHJJJ_COIN		"zjsl_wzhjjj_coin"
// 终极试炼代币2
#define COUNTER_ZJSL_WZHGGG_COIN		"zjsl_wzhggg_coin"
// 终极试炼每周挑战天数
#define COUNTER_ZJSL_CHALLENGE_DAYS		"zjsl_challenge_days"
// 终极试炼每周全通关领奖
#define COUNTER_ZJSL_CLEAR_AWARD		"zjsl_clear_award"

//飞升深渊礼盒使用此时前置 后缀为道具ID
#define ITEM_FLY_HELL_GIFT_USE_PRE		"item_fly_hell_gift_"

// 深渊地下城每天挑战次数
#define COUNTER_DUNGEON_HELL_TIMES		"dungeon_hell_times"

// 远古地下城每天挑战次数
#define COUNTER_DUNGEON_YUANGU_TIMES	"dungeon_yuangu_times"

// 是否已变成新技能配置
#define COUNTER_NEW_PVE_SKILL_PAGE	"new_pve_skill_page"
// 是否已变成新技能配置
#define COUNTER_NEW_PVP_SKILL_PAGE	"new_pvp_skill_page"

// 是否已变成新技能点配置
#define COUNTER_NEW_PVE_SKILL_SP	"new_pve_skill_sp"
// 是否已变成新技能点配置
#define COUNTER_NEW_PVP_SKILL_SP	"new_pvp_skill_sp"
// 是否已变成新技能点配置
#define COUNTER_NEW_EQUAL_PVP_SKILL_PAGE	"new_equal_pvp_skill_page"

// 限定商城套装获取记录 + 套装id
#define COUNTER_LIMITED_SUIT	"limited_suit"

//2v2乱斗活动积分
#define COUNTER_PVP_2V2_SCORE	"pvp_2v2_score"

//2v2乱斗活动剩余战斗次数
#define COUNTER_PVP_2V2_BATTLE_COUNT  "pvp_2v2_battle_count"


//2v2乱斗活动胜利次数
#define COUNTER_PVP_2V2_WIN__COUNT  "pvp_2v2_win_count"



//2v2乱斗活动最后战斗时间
#define COUNTER_PVP_2V2_LAST_BATTLE_TIME  "pvp_2v2_last_battle_time"


//2v2乱斗活动奖励标记
#define COUNTER_PVP_2V2_REWARD_MASK  "pvp_2v2_reward_mask"

//决斗场荣誉
#define COUNTER_PK_SEASON_1V1_HONOR  "pk_season_1v1_honor"

//吃鸡荣誉
#define COUNTER_CHI_JI_HONOR  "chi_ji_honor"

// 累计在线天数活动计数counter
#define COUNTER_ACTIVE_ONLINE_GIFT_DAY "active_online_gift_day"

// 团本道具使用次数counter
#define COUNTER_TEAM_COPY_ITEM_USE_CNT  "team_copy_item_use_"


// 种植熟练度
#define COUNTER_PLANT_PROF  "avtive_plant_prof"
// 种植成长剩余时间
#define COUNTER_PLANT_GROW_LASTTIME "avtive_plant_grow_last_time"
// 种植状态
#define COUNTER_PLANT_STATE "active_plant_state"
// 种植成长结束时间戳
#define COUNTER_PLANT_ENDTM "active_plant_endtm"

//角色背包开启数量
#define COUNTER_ROLE_STORAGE_OPEN_NUM "role_storage_open_num"



// 招募红点
#define COUNTER_HIRE_RED_POINT "hire_red_point"


// 冠军赛复活
#define COUNTER_CHAMPION_RELIVE "champion_relive"

// 冠军赛硬币
#define COUNTER_CHAMPION_COIN "champion_coin"

// 迷失地牢状态
#define COUNTER_LOSTDUNG_STATE "lost_dung_state"

// 地牢挑战剩余次数
#define COUNTER_LOSTDUNG_REMAIN_NUM "lost_dung_remain_num"

// 地牢代币
#define COUNTER_LOSTDUNG_COIN "lost_dungeon_coin"

#endif