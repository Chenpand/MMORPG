local ServiceType = 
{
	-- 无效值
	SERVICE_INVALID = 0,

	-- 赛季匹配
	SERVICE_1V1_SEASON = 1,
	-- 工会战报名
	SERVICE_GUILD_BATTLE_ENROLL = 2,
	-- 工会战
	SERVICE_GUILD_BATTLE = 3,
	-- 自动战斗
	SERVICE_AUTO_BATTLE = 4,
	-- 场景等级限制
	SERVICE_SCENE_LEVEL_LIMIT = 5,
	-- 在线时间控制
	--SERVICE_DAY_ONLINE_TIME = 7,
	-------------装备系统---------------
	-- 装备品级调整
	EQUIP_SET_QUALITY_LV = 10,
	-- 时装选属性
	FASHION_SEL_ATTR = 11,
	-- 金罐积分
	GOLD_JAR_POINT = 12,
	-- 魔罐积分
	MAGIC_JAR_POINT = 13,
	-- 时装合成
	FASHION_COMPOSE = 14,

	-- 公会系统
	-- 圆桌会议
	SERVICE_GUILD_TABLE = 20,
	-- 公会技能
	SERVICE_GUILD_SKILL = 21,
	-- 公会捐赠
	SERVICE_GUILD_DONATE = 22,
	-- 公会膜拜
	SERVICE_GUILD_ORZ = 23,
	-- 公会红包
	SERVICE_GUILD_RED_PACKET = 24,
	-- 跨服公会战
	SERVICE_GUILD_CROSS_BATTLE = 25,

	-- 随从系统
	SERVICE_RETINUE = 40,

	-- 次元石系统
	SERVICE_WARP_STONE = 60,
		
	-- 房间
	SERVICE_ROOM = 70;

	-- 每日充值福利
	SERVEICE_DAY_CHARGE_WELFARE = 91,
	
	-- VIP在线客服
	SERVICE_VIP_CUSTOM = 82;
	-- 在线客服
	SERVICE_CUSTOM = 83;
	
	-- 拍卖行复制功能
	SERVICE_AUCTION_COPY = 90;
	
	-- 每日充值福利
	SERVEICE_DAY_CHARGE_WELFARE = 91;
	
	-- 新的队伍状态计算方式
	SERVICE_NEW_TEAM_STATUS = 92;
	
	--师徒系统
	SERVICE_MASTER = 110,
	
	--安全锁
	SERVICE_SECURITY_LOCK = 111,
	
	--拍卖行珍品开关 
	SERVICE_AUCTION_TREAS = 210,
	
	--拍卖行翻页开关
	SERVICE_AUCTION_PAGE = 211,

	--拍卖行交易冷却时间开关
	SERVICE_AUCTION_COOLTIME = 212,

	--冒险队(佣兵团)
	SERVICE_ADVENTURE_TEAM = 215,

	--新回归活动
	SERVICE_NEW_RETURN = 216,

	--冒险队(佣兵团)排行榜
	SERVICE_ADVENTURE_TEAM_SORTLIST = 217,
	
	--跨天判断同一天
	SERVICE_CHECK_SAME_DAY_ON_DAY_CHANGE = 219,
	
	--周常深渊失败门票返还
	SERVICE_WEEK_HELL_FAIL_RETURN_TICKETS = 221, 
	
	--公会拍卖
	SERVICE_GUILD_AUCTION = 222,
	
	--运营活动上线同步
	SERVICE_OPACT_ONLINE_SYNC = 223,
	
	--公会副本奖励截图
	SERVICE_GUILD_DUNGEON_SCREEN_SHOT = 224,
	
	--单局结算ID判断
	SERVICE_RACE_ID_CHECK = 225,
	
	--每日必做
	SERVICE_DAILY_TODO = 226,
	
	--当删除账号玩家时检查玩家ID
	SERVICE_CHECK_ROLEID_WHEN_REMOVE_ACCOUNT_PLAYER = 227,

	--装备强化保护开关
	SERVICE_EQUIP_STRENGTHEN_PROTECT = 228,

	--装备强化装备分解失败移除装备开关
	SERVICE_EQUIP_STRENG_DESC_FAIL_REMOVE = 229,	

	--辅助装备开关
	SERVICE_ASSIST_EQUIP = 230,

	--公会兼并开关
	SERVICE_GUILD_MERGER = 231,	
	
	--终极试炼爬塔
	SERVICE_ZJSL_TOWER = 232,
	
	--技能页开关
	SERVICE_SKILL_PAGE = 237,
	
	--装备方案开关
	SERVICE_EQUIP_SCHEME = 238,
	
	--金币寄售
	SERVICE_GOLD_CONSIGNMENT = 239,
	
	SERVICE_NUM = 255,
}

-- 初始化
function Init(script)
	Sys:SetAutoBattleMode(2);
	Sys:CloseService(ServiceType.GOLD_JAR_POINT);
	Sys:CloseService(ServiceType.SERVICE_RETINUE);
	Sys:CloseService(ServiceType.SERVICE_WARP_STONE);
	Sys:CloseService(ServiceType.SERVICE_AUCTION_COPY);
	Sys:CloseService(ServiceType.SERVICE_CHECK_SAME_DAY_ON_DAY_CHANGE);
	--Sys:CloseService(ServiceType.SERVICE_EQUIP_SCHEME);
	--Sys:CloseService(ServiceType.SERVICE_ADVENTURE_TEAM_SORTLIST);
	
	if( Sys:GetPlatform() == "test" )
	then
		-- 测试平台
		Sys:OpenService(ServiceType.SERVICE_VIP_CUSTOM);
		Sys:OpenService(ServiceType.SERVICE_CUSTOM);
	elseif( Sys:GetPlatform() == "android" or Sys:GetPlatform() == "ios" or Sys:GetPlatform() == "ios-other")
	then
		-- mg、ios、越狱开启客服系统
		Sys:OpenService(ServiceType.SERVICE_VIP_CUSTOM);
		Sys:OpenService(ServiceType.SERVICE_CUSTOM);
	else
		-- 其它平台关闭客服系统
		Sys:CloseService(ServiceType.SERVICE_VIP_CUSTOM);
		Sys:CloseService(ServiceType.SERVICE_CUSTOM);
	end
	
	-- 拍卖行相关
	if( Sys:GetPlatform() == "test" )
	then
		-- 测试平台：开启珍品机制、浏览权限控制、交易冷却时间
		Sys:OpenService(ServiceType.SERVICE_AUCTION_TREAS);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_PAGE);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_COOLTIME);
	elseif( Sys:GetPlatform() == "android")
	then
		-- mg：开启珍品机制、浏览权限控制、交易冷却时间
		Sys:OpenService(ServiceType.SERVICE_AUCTION_TREAS);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_PAGE);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_COOLTIME);
	elseif( Sys:GetPlatform() == "android-hc")
	then
		-- hc：开启珍品机制、浏览权限控制、交易冷却时间
		Sys:OpenService(ServiceType.SERVICE_AUCTION_TREAS);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_PAGE);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_COOLTIME);
	elseif( Sys:GetPlatform() == "ios")
	then
		-- ios：开启珍品机制、浏览权限控制、交易冷却时间
		Sys:OpenService(ServiceType.SERVICE_AUCTION_TREAS);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_PAGE);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_COOLTIME);
	else
		-- 其它平台：关闭珍品机制、浏览权限控制、交易冷却时间
		Sys:OpenService(ServiceType.SERVICE_AUCTION_TREAS);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_PAGE);
		Sys:OpenService(ServiceType.SERVICE_AUCTION_COOLTIME);
	end

	--Sys:CloseService(ServiceType.SERVICE_OPACT_ONLINE_SYNC);
	Sys:CloseService(ServiceType.SERVICE_ASSIST_EQUIP);
	
	Sys:OpenService(ServiceType.SERVICE_AUCTION_TREAS);
	Sys:OpenService(ServiceType.SERVICE_AUCTION_PAGE);
	Sys:OpenService(ServiceType.SERVICE_AUCTION_COOLTIME);
	--Sys:OpenService(ServiceType.SERVICE_DAY_ONLINE_TIME);
end