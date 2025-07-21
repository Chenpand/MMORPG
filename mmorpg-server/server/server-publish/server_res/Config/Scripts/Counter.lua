--程序启动时调用，注册计数
--计数类型:
--  0:无周期 1:小时为周期 2:自然天为周期 3:自然周为周期 4:自然月为周期 5:自然年为周期
-- RegCounter(name, cycle)
-- name 最大长度为15个字节!!!!!!

function Init()
	--[[
	Sys:RegCounter("signin_flag",	3);		--周签到标志
	Sys:RegCounter("dailybox_flag",	2);		--日常箱子领取标志
	Sys:RegCounter("online_time",	2);		--每天在线时长
	Sys:RegCounter("XS_count",		2);		--每天悬赏任务计数
	Sys:RegCounter("XS_refresh",	2);		--每天悬赏任务刷新次数
	Sys:RegCounter("vip_dailyflag",	2);		--vip每日奖励领取标志
	Sys:RegCounter("flyshoe_times",	2);		--小飞鞋次数
	Sys:RegCounter("transwood_count", 2);	--运送木材次数
	Sys:RegCounter("robwood_count", 2);		--劫取木材次数
	Sys:RegCounter("challengescore", 3);	--擂台赛积分
	Sys:RegCounter("tribetask_count", 2); 	--部落任务计数
	Sys:RegCounter("challengetimes", 2);	--擂台赛次数
	Sys:RegCounter("challengewin",	3);		--每周胜利次数
	Sys:RegCounter("challengelose",	3);		--每周失败次数
	Sys:RegCounter("challenge_task", 2); 	--名人挑战任务
	Sys:RegCounter("get_tribe_exp", 2); 	--每天部落经验
	Sys:RegCounter("innwork_count", 2);		--每天客栈打工
	Sys:RegCounter("point_use", 	0);		--总的Q点消费
	Sys:RegCounter("invite_times", 	2);		--邀请次数
	Sys:RegCounter("invite_gold",	2);		--已领取金贝
	Sys:RegCounter("oncebox_flag",	0);		--一次性的箱子领取标志
	Sys:RegCounter("guangtou_flag", 0);		--光头哥连续登录活动标志
	Sys:RegCounter("guangtou_giftbag", 2);	--光头哥连续登录礼包领取标志
	Sys:RegCounter("guangtou_use", 2);		--光头哥连续登录活动非绑礼包使用次数
	Sys:RegCounter("guangtou_count", 0);	--光头哥连续登录礼包领取计数
	Sys:RegCounter("bless_times", 2);		--每日祝福次数
	Sys:RegCounter("festivalcost", 0);		--节日消费
	Sys:RegCounter("festivalcount", 2);		--节日每日计数
	Sys:RegCounter("jierichongzhi", 0);		--节日充值
	Sys:RegCounter("babel_refresh", 2);		--神秘商店刷新次数
	Sys:RegCounter("dttask_count", 2);		--黑暗塔任务次数
	Sys:RegCounter("ss_killtimes",	2);		--每日截杀神兽次数
	Sys:RegCounter("present_flag",	2);		--好友赠送标志
	Sys:RegCounter("present_times",	2);		--好友获赠次数
	Sys:RegCounter("end_of_year", 0);   	--岁末消费红包
	Sys:RegCounter("chongzhi_d", 2);  		--每日充值
	Sys:RegCounter("xiaofei_d", 2);  		--每日消费
	Sys:RegCounter("wudou_flag", 2);		--每日参加武斗标记

	Sys:RegCounter("cdkey_forever", 0);		--终生领一个
	Sys:RegCounter("cdkey_daily",	2);		--一天领一个
	Sys:RegCounter("cdkey_monthly",	4);		--每月领一个

	Sys:RegCounter("y_login", 2);          	--春节每日登录标记
	Sys:RegCounter("y_oldplayerlogin", 0); 	--春节福神每日登录标记
	Sys:RegCounter("flower_num", 2);	
	Sys:RegCounter("give_flower", 2); 	   	--送花榜计数
	Sys:RegCounter("get_flower", 2)  	   	--护花榜计数
	Sys:RegCounter("question_count", 2)    	--每日发布聊天频道答题计数

	Sys:RegCounter("m_login", 2);         	--三月每日登录礼包
	Sys:RegCounter("m_huikui", 2);		   	--三月每日登录回馈礼包
	Sys:RegCounter("m_chongzhi", 2);	   	--三月每日充值
	Sys:RegCounter("m_consume", 2); 	   	--三月每日消费
	Sys:RegCounter("m_girl", 2);           	--三月女生
	Sys:RegCounter("m_boy", 2);            	--三月男生
	
	Sys:RegCounter("3366charge", 5);		--3366活动充值
	Sys:RegCounter("3366charge_may", 5);    --3366五月活动充值
	Sys:RegCounter("3366charge_day", 2);	--3366活动每日充值
	Sys:RegCounter("3366niudanji", 2);		--3366扭蛋机
	Sys:RegCounter("3366lottery", 2);		--3366抽奖
	
	Sys:RegCounter("fools_card", 0);       	--愚人节卡片
	Sys:RegCounter("trans", 0);            	--愚人节卡片使用次数
	Sys:RegCounter("betrans", 0);          	--愚人节被变身次数
	
	Sys:RegCounter("interservice_gb", 0);  	--激情四月,喜迎跨服送豪礼	
	
	Sys:RegCounter("wchallengewin",	3);		--跨服战每周胜利次数
	Sys:RegCounter("wchallengelose",	3);	--跨服战每周失败次数

	Sys:RegCounter("consume_sortval",	4);	--消费排名活动计数
	
	Sys:RegCounter("world_login", 2);       --跨服登录礼包
	
	Sys:RegCounter("consumegold", 2);    	--冲刺消费排行榜,天下宝石入我囊	
	
	Sys:RegCounter("speak_times", 0);		--发言次数
	
	Sys:RegCounter("jqlb_num",	2);			--金券礼包购买次数

	Sys:RegCounter("first_flag", 5);    	 --第一次充值标记
	Sys:RegCounter("total_flag", 5);     	--充值满一定值 标记
	Sys:RegCounter("total_chongzhi", 5);
	Sys:RegCounter("june3_flag")

	Sys:RegCounter("188_flag", 2);			--188金贝礼包 是否领取
	Sys:RegCounter("988_flag", 2);			--988金贝礼包 是否领取
	Sys:RegCounter("1988_flag", 2);			--1988金贝礼包 是否领取

	Sys:RegCounter("june3_chongzhi", 2); 	--六月金贝礼包充值
	Sys:RegCounter("tribechest", 2); 	 	--每天拾取神兽宝箱次数
	
	Sys:RegCounter("canchall_counts", 2); 	--每日可挑战渡劫 0表示可挑战 1表示已经挑战
	Sys:RegCounter("buyedchall_counts", 2);	--每日购买的挑战次数
	Sys:RegCounter("5clean_flag", 2);       --每日5点重置扫荡次数标记

	Sys:RegCounter("petislandtimes", 2);	--今日进入宠物岛次数
	Sys:RegCounter("petcatchtimes",  2);	--今日使用捆龙索次数
	Sys:RegCounter("pettraptimes",	 2);	--今日使用宠物陷阱次数
	Sys:RegCounter("petislandtime",	 0);	--进入宠物岛时间
	
	]]

	Sys:RegCounter("tower",	 2,	6);	--掉线的死亡之塔
	Sys:RegCounter("tower_top_floor",	 0);		--死亡之塔层数
	Sys:RegCounter("tower_top_floor_total",	 0);	--死亡之塔历史最高层
	Sys:RegCounter("tower_used_time_total", 0);		--死亡之塔历史最高层使用时间
	Sys:RegCounter("tower_reset_remain_times", 0);	--死亡之塔重置次数
	Sys:RegCounter("tower_award_mask", 0);			--死亡之塔首通奖励领取情况
	
	Sys:RegCounter("gold_auction_num",	 2,	6);	--金币拍卖次数
	Sys:RegCounter("best_match_score",	 0);		--匹配积分最高分
	Sys:RegCounter("day_pkcoin",		2,	6);	--今日从决斗场获得决斗币数量
	Sys:RegCounter("day_pkcoin_ac",		2,	6);	--今日从活动获得决斗币数量
	Sys:RegCounter("day_season_pk",		2,	6);	--今日参加赛季PK次数
	
	
	Sys:RegCounter("retinue_skill_num",	2,	6);		--随从洗练次数
	Sys:RegCounter("retinue_skill_total", 0);		--随从洗练次数
	Sys:RegCounter("speak_times",	0);		--世界/附近频道高频说话次数

	Sys:RegCounter("3000_SIRpFree", 0);		--签到每月免费补签次数
	Sys:RegCounter("3000_SIRp", 0);			--签到补签总次数	
	Sys:RegCounter("3000_SITt", 0);			--签到总次数
	Sys:RegCounter("3000_SIRpCost", 0);			--签到补签花费	
	Sys:RegCounter("5000_DayOnline", 2, 6);	--每日在线时间
	Sys:RegCounter("day_signin", 2, 6);		--每日签到次数
	
	Sys:RegCounter("vip_free_hell_times", 2, 6);	--每日Vip深渊免费次数
	Sys:RegCounter("vip_shop_refresh_times", 2, 6);	--每日Vip黑市商店刷新次数
	Sys:RegCounter("vip_revive_times", 2, 6);	--每日Vip免费复活次数
	Sys:RegCounter("vip_gift_buy_bit", 0);			--VIP礼包购买BIT
	Sys:RegCounter("vip_gold_chest_times", 2, 6);	--VIP免费打开黄金宝箱次数
	Sys:RegCounter("vip_tower_pay_times", 2, 6);		--VIP购买死亡之塔额外次数

	Sys:RegCounter("open_jar_t1", 0);			--开罐子类型1数量
	Sys:RegCounter("open_jar_t2", 0);			--开罐子类型2数量
	Sys:RegCounter("open_jar_t3", 0);			--开罐子类型3数量
	Sys:RegCounter("open_jar_day", 0);			--每日开罐子次数
	

	Sys:RegCounter("blue_mc", 0);			--使用蓝色附魔卡次数
	Sys:RegCounter("purple_mc", 0);			--使用紫色附魔卡次数
	Sys:RegCounter("refresh_shop", 0);		--每日刷新黑市商店次数
	
	Sys:RegCounter("strengthen_fail_count", 0);	--连续强化失败次数Strengthen
	Sys:RegCounter("pvp_straight_count", 0);	--PVP连败
	Sys:RegCounter("pvp_season_straight_count", 2, 6);	--赛季PVP每日连胜或连败
	
	Sys:RegCounter("cycle_task_count", 0);			--每轮循环任务计数
	Sys:RegCounter("cycle_task_daily_count", 0);	--累积的循环次数
	Sys:RegCounter("cycle_refresh_count", 2, 6);		--每日循环任务刷新次数
	Sys:RegCounter("last_fatigue", 0);				--剩余疲劳
	
	Sys:RegCounter("guild_donate_point", 2, 6);		--公会点劵捐赠
	Sys:RegCounter("guild_donate_gold", 2, 6);		--公会金币捐赠
	Sys:RegCounter("guild_exchange", 2, 6);			--公会兑换
	Sys:RegCounter("guild_exchange_cold", 0);		--公会兑换冷却时间
	Sys:RegCounter("guild_mail", 2, 6);				--公会邮件
	Sys:RegCounter("guild_dismiss", 2, 6);			--公会解散
	Sys:RegCounter("guild_orz", 2, 6);				--公会膜拜
	Sys:RegCounter("guild_table", 2, 6);			--公会圆桌会议
	Sys:RegCounter("guild_table_help", 2, 6);		--公会圆桌会议援助
	Sys:RegCounter("guild_pay_rp", 2, 6);			--公会自费红包数量
	Sys:RegCounter("get_friend_give", 2, 6);
	
	Sys:RegCounter("wudao_times", 2, 6);			--今日武道大会参加次数
	Sys:RegCounter("wudao_win", 0);					--武道大会胜利场数
	Sys:RegCounter("wudao_lose", 0);				--武道大会失败场数
	
	Sys:RegCounter("is_inc_month_card_fatigue", 2, 6);		--月卡疲劳每日增加
	
	Sys:RegCounter("season_battle_num", 0);		--赛季本阶段战斗次数
	Sys:RegCounter("season_max_level", 0);		--赛季最高段位
	Sys:RegCounter("season_win_streak", 0);		--赛季连胜
	Sys:RegCounter("season_attr_level", 0);		--赛季属性对应段位

	--对应强化等级失败次数
	for i=1, 20 do
		local str = "str_fail_count_"..i;
		Sys:RegCounter(str, 0);
	end
	
	--对应武道大会罐子计数器
	for i=1, 5 do
		local strBase = "wudao_jar_base_"..i;
		local strSenior = "wudao_jar_senior_"..i;
		Sys:RegCounter(strBase, 0);
		Sys:RegCounter(strSenior, 0);
	end

	--时装部位合成计数
	Sys:RegCounter("fashion_cp_11", 0);
	Sys:RegCounter("fashion_cp_12", 0);
	Sys:RegCounter("fashion_cp_13", 0);
	Sys:RegCounter("fashion_cp_14", 0);
	Sys:RegCounter("fashion_cp_15", 0);
	Sys:RegCounter("fashion_cp_16", 0);

	--时装动态权重计数
	Sys:RegCounter("fashion_wt_11", 0);
	Sys:RegCounter("fashion_wt_12", 0);
	Sys:RegCounter("fashion_wt_13", 0);
	Sys:RegCounter("fashion_wt_14", 0);
	Sys:RegCounter("fashion_wt_15", 0);
	Sys:RegCounter("fashion_wt_16", 0);
	
	Sys:RegCounter("shop_refresh_num", 2, 6);		--商店刷新次数
	Sys:RegCounter("daily_act_score", 2, 6);		--每日活跃度
	Sys:RegCounter("daily_free_wchat", 2, 6);		--每日免费聊天次数
	Sys:RegCounter("sel_fa_num", 0);				--选择时装属性次数
	Sys:RegCounter("jar_next_use_free", 0);				--罐子下一次使用免费时间
	Sys:RegCounter("mag_jar_free_num", 0);			--魔罐免费开启次数
	Sys:RegCounter("first_open_magjar", 0);			--是否第一次开魔罐
	Sys:RegCounter("first_open_magbox", 0);			--是否第一次开魔盒
	
	Sys:RegCounter("last_daychange_t", 0);			--上一次自然跨天时间
	Sys:RegCounter("last_lgc_daychange_t", 0);			--上一次逻辑跨天时间
	Sys:RegCounter("next_lgc_weekchange_t", 0);			--下一次逻辑跨周时间

	Sys:RegCounter("disciple_used_fatigue", 0);			-- 师徒返利徒弟疲劳消耗点数
	Sys:RegCounter("master_fatigu_max", 2, 6);			-- 师傅每日疲劳补偿上限
	Sys:RegCounter("get_sky_fashon_t", 0);				-- 上一次获得天空套类型
	Sys:RegCounter("buy_mall_fashion_num", 0);		--购买时装数量
	Sys:RegCounter("l_sky_last_t", 0);					-- L概率下上一次合出天空套的类型
	Sys:RegCounter("l_sky_get_num", 0);					-- L概率下天空套合出的次数
	Sys:RegCounter("sky_normal_combo", 0);				-- L概率下连续天空普通的次数
	Sys:RegCounter("last_cmps_c", 0);					-- 上一次的C
	Sys:RegCounter("l_sky_get_info", 0);				-- L概率下N轮内天空套合成情况
	Sys:RegCounter("record_fashion_merge", 0);			-- 时装合成记录
	
	Sys:RegCounter("new_year_red_packet_num", 0);					-- 新年红包
	Sys:RegCounter("pvp_skill_config", 0)				-- pvp技能设置
	Sys:RegCounter("app_coin", 0);						-- 预约币
	Sys:RegCounter("exchanged_tokens", 0);			--已兑换过道具的代币数
	Sys:RegCounter("fatigue_tokens", 0);			--疲劳代币数量
	
	Sys:RegCounter("equip_reco_times", 0);						-- 装备回收本周次数
	Sys:RegCounter("equip_reco_score", 0);						-- 装备回收积分
	Sys:RegCounter("eqreco_upsc_times", 0);						-- 装备回收本周积分提升次数
	Sys:RegCounter("eqreco_reset_tm", 0);						-- 装备回收状态重置时间戳
	
	Sys:RegCounter("guild_terr_dungeon_num", 2, 6);		--公会战领地加成图次数
	Sys:RegCounter("abnm_trans_daily_tail_days", 2, 6);		--异常交易每日保释天数
	Sys:RegCounter("abnm_trans_total_tail_days", 0);		--异常交易总保释天数
	Sys:RegCounter("abnm_trans_open_never_back_t", 0);		--异常交易开启不返还处理时间
	
	Sys:RegCounter("cm_times", 2, 6);					--城镇怪物今日以打次数
	
	Sys:RegCounter("team_cleardungeon_addgrowth", 2, 6);		--每日师徒组队通过地下城成长值增加值
	Sys:RegCounter("master_giveequip_addgrowth", 2, 6);	--每日徒弟师傅赠送装备增加成长值
	
	Sys:RegCounter("sky_conti_produce_num_2", 0);				-- 正常天空套连续产出次数
	Sys:RegCounter("tpsky_conti_produce_num_2", 0);			-- 透明天空套连续产出次数
	
	Sys:RegCounter("cycle_task_daily_total_count", 2, 6);	--循环任务当天完成次数
	
	Sys:RegCounter("refresh_lease_shop", 0);			--刷新租赁商店
	
	Sys:RegCounter("l_sky_get_num_2", 0);					-- L概率下天空套合出的次数
	Sys:RegCounter("l_sky_get_info_2", 0);					-- L概率下N轮内天空套合成情况
	
	Sys:RegCounter("title_task_id_6124", 0);		--成就称号奖励
	
	Sys:RegCounter("active_login_reward", 2, 6);	--累计登录领奖活动是否计数counter
	
	Sys:RegCounter("head_frame_compensate", 0);		--头像框补发
	Sys:RegCounter("month_card_deposit", 0);		--月卡暂存箱过期时间
	

	Sys:RegCounter("equal_pvp_skill_have_sp", 0);		--是否已给过公平竞技场SP点
	Sys:RegCounter("equal_pvp_skill_config", 0);		--是否配置过公平竞技场技能
	Sys:RegCounter("act_dungeon", 2, 6);			--累计签到相关地下城每日次数
	
	Sys:RegCounter("chi_ji_coin", 0);						-- 吃鸡荣耀币

	Sys:RegCounter("new_sign_flag", 4, 6);						-- 签到记录标志位
	Sys:RegCounter("sign_activite_count", 4, 6);				-- 活跃度已补签次数
	Sys:RegCounter("sign_month_count", 4, 6);					-- 月已补签次数
	Sys:RegCounter("sign_activite_remain_count", 2, 6);		    -- 活跃度剩余补签次数
	Sys:RegCounter("new_collect_flag", 4, 6);					-- 累计签到领取奖励标志位
	Sys:RegCounter("sign_activite_have", 2, 6);				    -- 活跃度当日奖励补签次数是否已增加

	Sys:RegCounter("guild_merger_red_point", 2);			-- 公会兼并
	
	Sys:RegCounter("zjsl_top_floor_total", 0);				-- 终极试炼历史最高层
	Sys:RegCounter("zjsl_top_floor", 6, 1);					-- 终极试炼每周最高层
	Sys:RegCounter("zjsl_challenge_times", 2, 6);			-- 终极试炼每天挑战次数
	Sys:RegCounter("zjsl_dungeon_times", 2, 6);				-- 终极试炼关卡进入次数
	Sys:RegCounter("zjsl_dungeon_buff", 6, 1);				-- 终极试炼关卡效果
	Sys:RegCounter("zjsl_dungeon_buff_target", 6, 1);		-- 终极试炼关卡效果目标（1:玩家，2:怪物）
	Sys:RegCounter("zjsl_dungeon_buff_floor", 6, 1);		-- 终极试炼关卡效果层数
	Sys:RegCounter("zjsl_inspire_buff", 6, 1);				-- 终极试炼鼓舞BUFF
	Sys:RegCounter("zjsl_inspire_buff_day", 6, 1);			-- 终极试炼鼓舞BUFF第几天	
	Sys:RegCounter("zjsl_player_hp", 2, 6);					-- 终极试炼个人生命
	Sys:RegCounter("zjsl_player_mp", 2, 6);					-- 终极试炼个人魔力
	Sys:RegCounter("zjsl_boss1_id", 6, 1);					-- 终极试炼BOSS1的ID
	Sys:RegCounter("zjsl_boss2_id", 6, 1);					-- 终极试炼BOSS2的ID
	Sys:RegCounter("zjsl_boss1_hp", 6, 1);					-- 终极试炼BOSS1生命
	Sys:RegCounter("zjsl_boss2_hp", 6, 1);					-- 终极试炼BOSS2生命
	Sys:RegCounter("zjsl_wzhjjj_coin", 0);					-- 终极试炼代币1
	Sys:RegCounter("zjsl_wzhggg_coin", 0);					-- 终极试炼代币2
	Sys:RegCounter("zjsl_challenge_days", 6, 1);			-- 终极试炼每周挑战天数
	Sys:RegCounter("zjsl_clear_award", 6, 1);				-- 终极试炼每周全通关领奖
	
	Sys:RegCounter("dungeon_hell_times", 2, 6);				-- 深渊地下城每天挑战次数
	Sys:RegCounter("dungeon_yuangu_times", 2, 6);			-- 远古地下城每天挑战次数
	Sys:RegCounter("new_pve_skill_page", 0);			    -- 是否已变成pve新技能配置
	Sys:RegCounter("new_pvp_skill_page", 0);			    -- 是否已变成pvp新技能配置
	Sys:RegCounter("new_pve_skill_sp", 0);			        -- 是否已变成pve新技能点配置
	Sys:RegCounter("new_pvp_skill_sp", 0);			        -- 是否已变成pvp新技能点配置
	Sys:RegCounter("new_equal_pvp_skill_page", 0);			-- 是否已变成公平竞技场新技能点配置

	Sys:RegCounter("_challenge_score", 0);					-- 挑战者积分
	Sys:RegCounter("_challenge_total_score", 0);			-- 挑战者总积分
	Sys:RegCounter("_challenge_day_score", 0);				-- 挑战者当日积分

	Sys:RegCounter("_spring_score", 0);					    -- 春季挑战活动获得的积分
	Sys:RegCounter("_spring_total_score", 0);				-- 春季挑战活动获得的总积分

	Sys:RegCounter("_week_sign_spring_num", 0);				-- 2020春季周签到累计次数

	Sys:RegCounter("_hire_task_guild_dungeon_num", 2);		-- 招募公会地下城当天次数

	Sys:RegCounter("pvp_2v2_score", 0);					    -- 2v2乱斗活动匹配分
	Sys:RegCounter("pvp_2v2_battle_count", 0);				-- 2v2乱斗活动剩余战斗次数
	Sys:RegCounter("pvp_2v2_win_count", 0);				    -- 2v2乱斗活动胜利次数
	Sys:RegCounter("pvp_2v2_last_battle_time", 0);			-- 2v2乱斗活动最后战斗时间
	Sys:RegCounter("pvp_2v2_reward_mask", 0);			    -- 2v2乱斗活动奖励标记
	
	Sys:RegCounter("pk_season_1v1_honor", 3);				-- 决斗场周荣誉
	Sys:RegCounter("chi_ji_honor", 3);						-- 吃鸡周荣誉
	
	Sys:RegCounter("chi_ji_shop_coin", 0);					-- 吃鸡商店货币
	
	Sys:RegCounter("active_online_gift_day", 2, 6);	--在线好礼累计在线天数是否计数counter
	
	Sys:RegCounter("avtive_plant_prof", 0);					-- 种植熟练度
	Sys:RegCounter("avtive_plant_grow_last_time", 0);		-- 种植成长剩余时间
	Sys:RegCounter("active_plant_state", 0);				-- 种植状态
	Sys:RegCounter("active_plant_endtm", 0);				-- 种植成长结束时间戳
	
	Sys:RegCounter("sky_conti_produce_num_3", 0);			-- 正常天空套连续产出次数（天三）
	Sys:RegCounter("tpsky_conti_produce_num_3", 0);			-- 透明天空套连续产出次数（天三）
	Sys:RegCounter("l_sky_get_num_2", 0);					-- L概率下天空套合出的次数（天三）
	Sys:RegCounter("l_sky_get_info_2", 0);					-- L概率下N轮内天空套合成情况（天三）

	Sys:RegCounter("hire_red_point", 0);					-- 招募红点

	Sys:RegCounter("champion_relive", 2);					-- 冠军赛复活

	Sys:RegCounter("champion_coin", 0);					-- 冠军赛硬币


	Sys:RegCounter("_gift_right_card", 0);					-- 礼遇特权卡权限
	Sys:RegCounter("_gift_right_card_buy_time", 0);			-- 礼遇特权卡购买时间
	Sys:RegCounter("_gift_right_card_login", 2);			-- 礼遇特权卡当日登陆
	Sys:RegCounter("_gift_right_card_login_total", 0);		-- 礼遇特权卡累计登陆

	
	Sys:RegCounter("role_storage_open_num", 0);				-- 角色背包解锁
	
	Sys:RegCounter("lost_dung_state", 0);		  	-- 地牢状态
	Sys:RegCounter("lost_dung_remain_num", 0);		  	-- 地牢挑战剩余次数
end
