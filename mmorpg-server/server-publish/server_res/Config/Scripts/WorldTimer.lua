                                                        require "GameDefine"

function CityMonsterHandler()
	Sys:SendAnnouncementNow(75, 0, 1);
	Sys:SummonCityMonster();
end

--注册活动时要保证活动名唯一，如果活动名中有@，客户端会忽略@后的全部字符
--初始设置定时器
function Init(timer)
	--首先清理已注册的活动信息
	Sys:ClearActivityRegInfos();
	
	-- Sys:RegConsistActivity(1000, "MinotaurParadise", 19); --牛头怪乐园活动
	Sys:RegConsistActivity(1000,      "MinotaurParadise",      1);
	-- Sys:RegConsistActivity(1001, "SouthernValley", 23);  --南部溪谷活动
	--Sys:RegConsistActivity(1001,      "SouthernValley",        19);  新版本关闭
	-- Sys:RegDailyActivity(1002,      "PVP",                   10,     "20:00:00",        1 * 30 * 60,              2 * 60 * 60);
	-- Sys:RegDailyActivity(1002,      "PVP",                   10,     "00:00:00",        0,              24 * 60 * 60 - 1);
    Sys:RegConsistActivity(1003,     "DungeonDeadTower",       1);   --死亡之塔
	--Sys:RegDailyActivity(1003,      "DungeonDeadTower",      16,      "00:00:00",          0,                         24 * 60 * 60);
	Sys:RegConsistActivity(1022,     "DungeonKingTower",       1);   --王者之塔
	-- Sys:RegConsistActivity(1004, "DungeonSY", 30);      --深渊
	Sys:RegConsistActivity(1004,      "DungeonSY0",            1);
	-- Sys:RegConsistActivity(1005, "DungeonYG", 35);     --远古 试验场
	Sys:RegConsistActivity(1005,      "DungeonYG0",             1);
	-- Sys:RegConsistActivity(1006, "DungeonGoldRush", 26);    --哥布林藏宝室
    Sys:RegConsistActivity(1006,      "DungeonGoldRush",       1)
	-- Sys:RegDailyActivity(1007,      "PVP",   武道会                10,     "20:00:00",        1 * 30 * 60,              2 * 60 * 60);
	--Sys:RegDailyActivity(1007,      "PVPwudaohui",                   16,     "16:00:00",        1 * 30 * 60,              23 * 60 * 60);
     --40级深渊
	Sys:RegConsistActivity(1008,      "DungeonSY1",             1);
	 --远古  机械牛
	Sys:RegConsistActivity(1009,      "DungeonYG1",             1);
		 --远古  悲鸣
	Sys:RegConsistActivity(1101,      "DungeonYG3",             1);
		 --远古  战国		 
	Sys:RegConsistActivity(1010,      "DungeonYG2",             1);
	     --55级深渊
	Sys:RegConsistActivity(1200,      "DungeonSY2",             1);
	     --周常深渊	     
	Sys:RegConsistActivity(25100,      "DungeonSY4",             1);
		 --60级深渊
	Sys:RegConsistActivity(1201,      "DungeonSY5",             1);	
     --组队BOSS  20
	--Sys:RegConsistActivity(1300,      "TeamBOSS",             1);
     --组队 BOSS  30
	--Sys:RegConsistActivity(1301,      "TeamBOSS1",             1);
      --组队BOSS  40
	--Sys:RegConsistActivity(1302,      "TeamBOSS2",             1);
      --深渊  50
	-- Sys:RegDailyActivity(1010,      "DungeonSY2",             50,      "00:00:00",          0,                         24 * 60 * 60);


	-- 怪物攻城活动
	Sys:RegDailyActivity(1012,      "CityMonster",   0,     "12:00:00",        0,              10 * 60 * 60 - 1);
	for hour=12,22 do
		timer:RegisterDailyEvent(hour * 60 * 60, "CityMonsterHandler");
	end
	


	--例子:中秋活动, 活动ID，活动名称，几级解锁，每周几，几点开始, 准备时间,准备时间后的持续时间，周循环
	--Sys:RegWeekActivity(2000, "ZhongQiu", 1, "1|1:00:00", 0, 120 * 60 * 60);
	--武道会  1,3,5,7
  Sys:RegWeekActivity(1500, "PVPwudaohui", 30, "1|11:00:00", 1 * 30 * 60, 10 * 60 * 60);
  --Sys:RegWeekActivity(1501, "PVPwudaohui", 30, "2|19:00:00", 1 * 30 * 60, 1 * 60 * 60);
  Sys:RegWeekActivity(1502, "PVPwudaohui", 30, "3|19:00:00", 1 * 30 * 60, 1 * 60 * 60);  
  --Sys:RegWeekActivity(1503, "PVPwudaohui", 30, "4|19:00:00", 1 * 30 * 60, 1 * 60 * 60);
  Sys:RegWeekActivity(1504, "PVPwudaohui", 30, "5|19:00:00", 1 * 30 * 60, 1 * 60 * 60);
  --Sys:RegWeekActivity(1505, "PVPwudaohui", 30, "6|19:00:00", 1 * 30 * 60, 1 * 60 * 60);
  Sys:RegWeekActivity(1506, "PVPwudaohui", 30, "7|19:00:00", 1 * 30 * 60, 1 * 60 * 60);
  Sys:RegWeekActivity(1604, "PVPshangjin", 30, "5|20:00:00", 0, 2 * 60 * 60);

  --公会副本

	--Sys:RegWeekActivity(1702, "GuildDungeon", 66, "1|20:30:00", 0, 1* 60 * 60);
	--Sys:RegWeekActivity(1702, "GuildDungeon", 66, "2|20:30:00", 0, 1* 60 * 60);
	--Sys:RegWeekActivity(1702, "GuildDungeon", 66, "3|20:30:00", 0, 1* 60 * 60);
	--Sys:RegWeekActivity(1702, "GuildDungeon", 66, "4|20:30:00", 0, 1* 60 * 60);
	--Sys:RegWeekActivity(1702, "GuildDungeon", 66, "5|20:30:00", 0, 1* 60 * 60);
	Sys:RegWeekActivity(1702, "GuildDungeon", 66, "6|19:00:00", 0, 1* 60 * 60);
	--Sys:RegWeekActivity(1702, "GuildDungeon", 66, "7|20:30:00", 0, 1* 60 * 60);
	
	--吃鸡
	if( Sys:GetPlatform() == "android" ) then
		-- mg: 星期天18:30-20:30开吃鸡
		Sys:RegWeekActivity(2025, "chiji", 40, "7|18:30:00", 0, 2 * 60 * 60);
	elseif( Sys:GetPlatform() == "dev") then
		-- 内网: 全天开放
		--Sys:RegWeekActivity(2019, "chiji", 1, "1|00:00:00", 0, 16 * 60 * 60);
		--Sys:RegWeekActivity(2020, "chiji", 1, "2|00:00:00", 0, 16 * 60 * 60);
		--Sys:RegWeekActivity(2021, "chiji", 1, "3|00:00:00", 0, 16 * 60 * 60);
		--Sys:RegWeekActivity(2022, "chiji", 1, "4|00:00:00", 0, 16 * 60 * 60);
		--Sys:RegWeekActivity(2023, "chiji", 1, "5|00:00:00", 0, 16 * 60 * 60);
		--Sys:RegWeekActivity(2024, "chiji", 1, "6|00:00:00", 0, 16 * 60 * 60);
		--Sys:RegWeekActivity(2025, "chiji", 1, "7|00:00:00", 0, 16 * 60 * 60);
		Sys:RegDailyActivity(2019, "chiji", 1, "00:00:00", 0, 4 * 58 * 60);
		Sys:RegDailyActivity(2020, "chiji", 1, "04:00:00", 0, 4 * 58 * 60);
		Sys:RegDailyActivity(2021, "chiji", 1, "08:00:00", 0, 4 * 58 * 60);
		Sys:RegDailyActivity(2022, "chiji", 1, "12:00:00", 0, 4 * 58 * 60);
		Sys:RegDailyActivity(2023, "chiji", 1, "16:00:00", 0, 4 * 58 * 60);
		Sys:RegDailyActivity(2024, "chiji", 1, "20:00:00", 0, 4 * 58 * 60);
		
		
		
	end

	--迷失地牢
	Sys:RegCustomCircleActivity(10088, "lostDungeon", 1, "1|2020-4-12|08:00:00", 0, 16 * 60 * 60);

	--公平竞技场
	Sys:RegWeekActivity(2026, "EqualPVP", 30, "1|00:00:00", 0, 24 * 60 * 60);	
	Sys:RegWeekActivity(2027, "EqualPVP", 30, "2|00:00:00", 0, 24 * 60 * 60);	
	Sys:RegWeekActivity(2028, "EqualPVP", 30, "3|00:00:00", 0, 24 * 60 * 60);	
	Sys:RegWeekActivity(2029, "EqualPVP", 30, "4|00:00:00", 0, 24 * 60 * 60);	
	Sys:RegWeekActivity(2030, "EqualPVP", 30, "5|00:00:00", 0, 24 * 60 * 60);	
	--Sys:RegWeekActivity(2031, "EqualPVP", 30, "6|12:00:00", 0, 6 * 60 * 60);	
	Sys:RegWeekActivity(2031, "EqualPVP", 30, "6|00:00:00", 0, 24 * 60 * 60);
	Sys:RegWeekActivity(2032, "EqualPVP", 30, "7|00:00:00", 0, 24 * 60 * 60);	
	
	
	--签到活动
	--第一个月
	--Sys:RegOnceActivity(3000, "SignInFirstMonth", 1, "2016-9-28 00:00:00", 0, 30 * 24 * 3600);
	--后续
	Sys:RegConsistActivity(3000, "SignInFirstMonth", 1);
	Sys:RegConsistActivity(3001, "SignInTotal", 1);

	--升级活动
	Sys:RegConsistActivity(4000, "LvUp", 1);

	--在线时长活动
	Sys:RegConsistActivity(5000, "OnlineTime", 1);

	--月卡返利活动
	Sys:RegConsistActivity(6000, "MonthCard", 1);

	--7日活动
	Sys:RegConsistActivity(7100, "SevenDayOne", 1);
	Sys:RegConsistActivity(7200, "SevenDayTwo", 1);
	Sys:RegConsistActivity(7300, "SevenDayThree", 1);
	Sys:RegConsistActivity(7400, "SevenDayFour", 1);
	Sys:RegConsistActivity(7500, "SevenDayFive", 1);
	Sys:RegConsistActivity(7600, "SevenDaySix", 1);
	Sys:RegConsistActivity(7700, "SevenDaySeven", 1);
	Sys:RegConsistActivity(7800, "SevenDayCharge", 1);
	
	--补偿福利
	Sys:RegConsistActivity(8100, "MakeupFatigue", 1);
	Sys:RegConsistActivity(8200, "MakeupDungeon", 1);
	--充值活动
	Sys:RegConsistActivity(8300, "FirstCharge", 1);
	Sys:RegConsistActivity(8400, "TotleCharge", 1);
	Sys:RegConsistActivity(8700, "DayChargeNew", 1);
		
	--勇士招募活动
	Sys:RegConsistActivity(8800, "HireTask", 1);
	
	--理财计划
	Sys:RegConsistActivity(8600, "ChargeMoneyManage", 8);
	
	--*@param ID name:活动名称 level:玩家等级 fromDay:开服第几天开服时间点开始 toDay:开服第几天开服时间点结束
	Sys:RegGSActivityIntervalTime(9001, "ActJar1", 8, "1|0|0|0",  "30000|23|59|59");
	Sys:RegGSActivityIntervalTime(9002, "ActJar2", 8, "1|0|0|0",  "30000|23|59|59");
	Sys:RegGSActivityIntervalTime(9003, "ActJar3", 30, "1|0|0|0", "30000|23|59|59");
	Sys:RegGSActivityIntervalTime(9004, "ActJar4", 40, "1|0|0|0", "30000|23|59|59");
	Sys:RegGSActivityIntervalTime(9006, "ActJar6", 50, "1|0|0|0", "30000|23|59|59");
	Sys:RegGSActivityIntervalTime(9007, "ActJar7", 55, "1|0|0|0", "30000|23|59|59");
	Sys:RegGSActivityIntervalTime(10002, "LimitItem2", 1, "15|0|0|0", "21|23|59|59");
	--参数: id, 名字, 等级, 开始时间(开服时间0时0分0秒 + 天|时|分|秒), 结束时间(开服时间0时0分0秒 + 天|时|分|秒) 
	--Sys:RegGSActivityIntervalTime(10000, "test", 10, "1|0|0|0", "7|0|0|0")
	
	--夏日活动
	-- 兑换活动
	--Sys:RegOnceActivity(18300, "ChristmasChange", 1, "2018-7-06 00:00:00", 0, 31 * 24 * 3600-1);
	-- 刷怪活动q
	--Sys:RegOnceActivity(19300, "ChristmasMonster", 1, "2018-7-06 00:00:00", 0, 31 * 24 * 3600-1);
	-- 活动掉落
	--Sys:RegOnceActivity(19400, "SummerDrop", 1, "2018-7-06 00:00:00", 0, 31 * 24 * 3600-1);
	
	--劳动节活动
	-- 兑换活动
	--Sys:RegOnceActivity(18200, "ChristmasChange", 1, "2018-4-28 00:00:00", 0, 14 * 24 * 3600-1);
	-- 刷怪活动q
	--Sys:RegOnceActivity(19200, "ChristmasMonster", 1, "2018-4-28 00:00:00", 0, 14 * 24 * 3600-1);
	
	--国庆节活动
	-- 兑换活动
	Sys:RegOnceActivity(18500, "ChristmasChange", 1, "2018-10-01 00:00:00", 0, 14 * 24 * 3600-1);
	-- 刷怪活动q
	--Sys:RegOnceActivity(19500, "ChristmasMonster", 1, "2018-10-01 00:00:00", 0, 14 * 24 * 3600-1);
	--Sys:RegOnceActivity(19600, "SummerDrop", 1, "2018-10-01 00:00:00", 0, 31 * 24 * 3600-1);
	Sys:RegOnceActivity(19700, "ChristmasMonster", 1, "2018-11-29 00:00:00", 0, 5 * 24 * 3600-1);
	-- 虚空商店兑换活动
	--Sys:RegOnceActivity(18700, "xukongChange", 60, "2018-11-29 00:00:00", 0, 28 * 24 * 3600-1);
	Sys:RegOnceActivity(18700, "xukongChange2", 60, "2019-08-15 00:00:00", 0, 28 * 24 * 3600-1);
	-- 武器时装商店兑换活动
	Sys:RegOnceActivity(18800, "wuqishizhuangChange", 1, "2018-11-10 00:00:00", 0, 51 * 24 * 3600-1);
	-- 强化券活动掉落
	Sys:RegOnceActivity(19800, "SummerDrop", 1, "2018-11-17 00:00:00", 0, 2 * 24 * 3600-1);
	-- 强化券活动掉落
	Sys:RegOnceActivity(19900, "SummerDrop", 1, "2018-12-15 00:00:00", 0, 2 * 24 * 3600-1);
	-- 强化券活动掉落
	Sys:RegOnceActivity(20100, "SummerDrop", 1, "2019-08-15 00:00:00", 0, 7 * 24 * 3600-1);
	-- 强化券活动掉落
	Sys:RegOnceActivity(20200, "SummerDrop", 1, "2019-09-19 00:00:00", 0, 7 * 24 * 3600-1);
	-- 幸运魔方掉落
	Sys:RegOnceActivity(20300, "SummerDrop2", 1, "2019-08-08 00:00:00", 0, 24 * 24 * 3600-1);
	-- 幸运魔方掉落
	Sys:RegOnceActivity(20400, "SummerDrop2", 1, "2019-09-12 00:00:00", 0, 19 * 24 * 3600-1);
	-- 强化券活动掉落
	Sys:RegOnceActivity(20500, "SummerDrop", 1, "2019-10-17 00:00:00", 0, 7 * 24 * 3600-1);
	-- 米亚的宝箱掉落
	Sys:RegOnceActivity(20600, "SummerDrop2", 1, "2019-10-01 00:00:00", 0, 31 * 24 * 3600-1);
	-- 周年小蛋糕掉落
	Sys:RegOnceActivity(20700, "SummerDrop3", 1, "2019-09-19 00:00:00", 0, 12 * 24 * 3600-1);
	-- 强化券活动掉落
	Sys:RegOnceActivity(20800, "SummerDrop4", 1, "2019-11-21 00:00:00", 0, 7 * 24 * 3600-1);
	-- 米亚的宝箱掉落
	Sys:RegOnceActivity(20900, "SummerDrop5", 1, "2019-11-01 00:00:00", 0, 30 * 24 * 3600-1);
	-- 虚无之源月卡掉落
	Sys:RegOnceActivity(19850, "SummerDrop6", 55, "2019-10-04 06:00:00", 0, 2419199);
	-- 宇宙之眼掉落
	Sys:RegOnceActivity(19860, "SummerDrop7", 1, "2020-01-16 00:00:00", 0, 7 * 24 * 3600-1);
	-- 宇宙之眼掉落
	Sys:RegOnceActivity(19870, "SummerDrop8", 1, "2020-01-30 00:00:00", 0, 7 * 24 * 3600-1);
	-- 宇宙之眼掉落
	Sys:RegOnceActivity(19880, "SummerDrop9", 1, "2020-02-13 00:00:00", 0, 7 * 24 * 3600-1);
	-- 宇宙之眼掉落
	Sys:RegOnceActivity(19890, "SummerDrop10", 1, "2020-02-27 00:00:00", 0, 7 * 24 * 3600-1);
	-- 兑换材料月卡掉落
	Sys:RegOnceActivity(19851, "SummerDrop11", 1, "2019-12-17 00:00:00", 0, 14 * 24 * 3600-1);
	
	--oppo活动 登录特权 活动id, 活动名称, 玩家等级, 开始时间, 准备时间, 持续时间
	Sys:RegOnceActivity(12000, "LoginPrivilege", 1, "2017-12-01 00:00:00", 0, 5000 * 24 * 3600);
	--oppo活动 七日签到
	Sys:RegOnceActivity(15000, "SignInSeven", 1, "2017-12-01 00:00:00", 0, 5000 * 24 * 3600);
	--oppo活动 七日签到签满奖励
	Sys:RegOnceActivity(16000, "SignInSevenAll", 1, "2017-12-01 00:00:00", 0, 5000 * 24 * 3600);
	--oppo活动 抽奖
	Sys:RegOnceActivity(17000, "DrawPrize", 1, "2017-12-01 00:00:00", 0, 5000 * 24 * 3600);
	--oppo活动 琥珀特权
	Sys:RegOnceActivity(20000, "AmberPrivilege", 1, "2017-12-01 00:00:00", 0, 5000 * 24 * 3600);
	--oppo活动 累计充值
	Sys:RegMonthActivity(26000, "OppoAccountTotalCharge", 1);
	--oppo活动 琥珀等级
	Sys:RegOnceActivity(27000, "OppoVipLevel", 1, "2018-11-20 00:00:00", 0, 5000 * 24 * 3600);
	
	-- 排行榜
	Sys:RegOnceActivity(21100, "HuaweiYearSortList", 20, "2018-02-15 00:00:00", 0, 7 * 24 * 3600);
	Sys:RegOnceActivity(21200, "OppoYearSortList", 20, "2018-02-15 00:00:00", 0, 7 * 24 * 3600);
	Sys:RegOnceActivity(21300, "VivoYearSortList", 20, "2018-02-15 00:00:00", 0, 7 * 24 * 3600);
	Sys:RegOnceActivity(21400, "XiaomiYearSortList", 20, "2018-02-15 00:00:00", 0, 7 * 24 * 3600);
	Sys:RegOnceActivity(21500, "MeizuYearSortList", 20, "2018-02-15 00:00:00", 0, 7 * 24 * 3600);

	--新年红包活动
	Sys:RegOnceActivity(22000, "NewYearRedPacket", 20, "2019-02-04 00:00:00", 0, 8 * 24 * 3600);
	--新年红包活动排行榜
	Sys:RegOnceActivity(22100, "NewYearRedPacketSortList", 20, "2019-02-04 00:00:00", 0, 9 * 24 * 3600);
	
	--vivo活动 登录特权
	Sys:RegOnceActivity(23000, "VivoLoginPrivilege", 1, "2017-12-01 00:00:00", 0, 5000 * 24 * 3600);
	--账号充值返利
	Sys:RegOnceActivity(24000, "AccountChargeRebate", 10, "2019-11-04 00:00:00", 0, 3 * 24 * 3600-1);	
    --活动深渊
    --Sys:RegOnceActivity(25000, "DungeonSY3", 1, "2019-04-25 00:00:00", 0, 28 * 24 * 3600-1);
	--活动深渊
	Sys:RegOnceActivity(28000, "DungeonSY6", 1, "2020-03-20 00:00:00", 0, 28 * 24 * 3600-1);
	--活动深渊
	Sys:RegOnceActivity(28100, "DungeonSY7", 1, "2020-03-20 00:00:00", 0, 28 * 24 * 3600-1);
	--活动深渊-60-不消耗票
    Sys:RegOnceActivity(25000, "DungeonSY3", 1, "2019-05-30 00:00:00", 0, 35 * 24 * 3600-1);
	-- 春节刷怪活动q
	Sys:RegOnceActivity(22200, "ChristmasMonster", 1, "2019-02-01 00:00:00", 0, 14 * 24 * 3600-1);
	-- 春节兑换活动
	Sys:RegOnceActivity(22300, "chunjieChange", 1, "2019-02-01 00:00:00", 0, 14 * 24 * 3600-1);
	-- 劳动节刷怪活动
	Sys:RegOnceActivity(22400, "ChristmasMonster", 1, "2019-05-01 00:00:00", 0, 14 * 24 * 3600-1);
	-- 劳动节兑换活动
	Sys:RegOnceActivity(22500, "LabordayChange", 1, "2019-05-01 00:00:00", 0, 17 * 24 * 3600-1);
	-- 史诗升级兑换活动
	Sys:RegOnceActivity(22600, "LimitChange", 55, "2019-11-04 06:00:00", 0, 2419199);
	-- 夏日刷怪活动
	Sys:RegOnceActivity(22700, "ChristmasMonster", 1, "2019-07-04 00:00:00", 0, 14 * 24 * 3600-1);
	-- 夏日兑换活动
	Sys:RegOnceActivity(22800, "SummerChange", 1, "2019-07-04 00:00:00", 0, 15 * 24 * 3600-1);
	-- 国庆刷怪活动
	Sys:RegOnceActivity(22810, "ChristmasMonster", 1, "2019-10-01 00:00:00", 0, 14 * 24 * 3600-1);
	-- 国庆兑换活动
	Sys:RegOnceActivity(22820, "SummerChange", 1, "2019-10-01 00:00:00", 0, 15 * 24 * 3600-1);
	-- 钥匙兑换活动
	--Sys:RegOnceActivity(22840, "SummerChange2", 1, "2019-10-01 00:00:00", 0, 31 * 24 * 3600-1);
	-- 周年派对兑换活动
	Sys:RegOnceActivity(22850, "SummerChange3", 1, "2019-10-01 00:00:00", 0, 31 * 24 * 3600-1);
	-- 元旦兑换活动
	Sys:RegOnceActivity(22860, "NewYearChange", 1, "2019-11-14 00:00:00", 0, 7 * 24 * 3600-1);
	-- 挑战者活动
	Sys:RegOnceActivity(22870, "ChallengerChange", 25, "2019-11-14 06:00:00", 0, 1209599);
	-- 春节兑换活动
	Sys:RegOnceActivity(22880, "chunjieChange", 40, "2019-12-17 00:00:00", 0, 14 * 24 * 3600-1);
	-- 春节时装兑换活动
	Sys:RegOnceActivity(22900, "chunjieChange2", 1, "2020-01-06 00:00:00", 0, 83 * 24 * 3600-1);
	-- 春季刷怪活动
	Sys:RegOnceActivity(22910, "ChristmasMonster", 99, "2020-04-01 00:00:00", 0, 28 * 24 * 3600-1);
	-- 春季兑换活动
	Sys:RegOnceActivity(22920, "SummerChange", 99, "2020-04-01 00:00:00", 0, 29 * 24 * 3600-1);
	
	-- 宝珠转换
	Sys:RegOnceActivity(82900, "BeadConvert", 30, "2020-02-09-17:00:00", 0, 1 * 1 * 60-1);
	Sys:RegOnceActivity(82910, "BeadConvert", 30, "2020-02-09-17:03:00", 0, 30 * 24 * 3600-1);	
end


--查询计数数据返回
function OnSelectCounterReturn(key, playerId, name, value)
	
end

--活动准备
function OnActivityPrepare(name)
	if name == "PVP" then
		Sys:SendAnnouncement(4001, "19:30", 15 * 60, 2);
	end
end

--活动开始
function OnActivityStart(name)
	--if name == "MinotaurParadise" then
		--Sys:BroadcastNotify(8, 0, "牛头怪乐园现已开启");
	--elseif name == "ZhongQiu" then
		--Sys:BroadcastNotify(1, 0, "中秋活动现已开启");
	--end
	
	if name == "PVP" then
		Sys:SendAnnouncement(4002, "20:00", 10 * 60, 6);
	elseif name == "LimitItem2" then
		Sys:OnSaleMallWaitGoods(318018);
	end
	
	if name == "ChristmasMonster" then
		Sys:SetActivityMonsterOpen(1);
	end
	
	if name == "chiji" then
		Sys:ChijiOpen(name);
	end
end

--活动结束
function OnActivityEnd(name)
	--if name == "MinotaurParadise" then
		--Sys:BroadcastNotify(9, 0, "牛头怪乐园现已结束");
	--elseif name == "ZhongQiu" then
		--Sys:BroadcastNotify(1, 0, "中秋活动现已关闭");
	--en1
	
	if name == "PVP" then
		Sys:SendAnnouncement(4003, "21:00", 100, 1);
	elseif name == "LimitItem2" then
		Sys:OffSaleMallWaitGoods(318018);
	end

	if name == "ChristmasMonster" then
		Sys:SetActivityMonsterOpen(0);
	end
	
	if name == "NewYearRedPacket" then
		-- Sys:OnNewYearRedPacketEnd();
	end
end


