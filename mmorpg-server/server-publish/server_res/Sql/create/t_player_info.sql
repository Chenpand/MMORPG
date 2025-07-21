CREATE TABLE IF NOT EXISTS `t_player_info`(
	`guid`	BIGINT		UNSIGNED NOT NULL,				#主键
	`accid`	INT			UNSIGNED NOT NULL,				#账号id
	`name`	VARCHAR(32)	NOT NULL,						#名字
	`level`	SMALLINT	UNSIGNED NOT NULL DEFAULT 1,	#等级
	`occu`	TINYINT		UNSIGNED NOT NULL DEFAULT 0,	#职业
	`exp`	BIGINT		UNSIGNED NOT NULL DEFAULT 0,	#经验
	`blessexp`	INT		UNSIGNED NOT NULL DEFAULT 0,	#累计祝福经验
	`sex`	TINYINT		UNSIGNED NOT NULL DEFAULT 0,	#性别
	`zoneid`	SMALLINT		UNSIGNED NOT NULL DEFAULT 0,	#区id
	`tourzone`	SMALLINT		UNSIGNED NOT NULL DEFAULT 0,	#所在旅游区
	`viplvl`	TINYINT UNSIGNED NOT NULL DEFAULT 0,	#vip等级
	`vipduetime`	INT	UNSIGNED NOT NULL DEFAULT 0,	#vip到期时间
	`createtime`	INT UNSIGNED NOT NULL DEFAULT 0,	#角色创建时间
	`savetime`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#存档时间
	`deletetime`	INT UNSIGNED NOT NULL DEFAULT 0,	#删号时间
	`localsavetime` BIGINT UNSIGNED NOT NULL DEFAULT 0,	#本服存档时间	
	`pkmode`	TINYINT	UNSIGNED NOT NULL DEFAULT 0,	#pk模式
	`evil`	SMALLINT	UNSIGNED NOT NULL DEFAULT 0,	#罪恶值
	`hp`	INT			UNSIGNED NOT NULL DEFAULT 0,	#当前生命
	`silver`	INT		UNSIGNED NOT NULL DEFAULT 0,	#银币
	`gold`		INT		UNSIGNED NOT NULL DEFAULT 0,	#金币
	`bindgold`	INT		UNSIGNED NOT NULL DEFAULT 0,	#绑金
	`ironstone`	INT		UNSIGNED NOT NULL DEFAULT 0,	#铁石
	`spiritstone`	INT	UNSIGNED NOT NULL DEFAULT 0,	#灵石
	`jadestone`  	INT	UNSIGNED NOT NULL DEFAULT 0,    #玉石
	`honour`	INT		UNSIGNED NOT NULL DEFAULT 0,	#荣誉
	`sceneid`	INT		UNSIGNED NOT NULL DEFAULT 0,	#场景id
	`mapid`		INT		UNSIGNED NOT NULL DEFAULT 0,	#地图id
	`pos`		VARCHAR(16)	NOT NULL DEFAULT "",		#坐标
	`dir`		TINYINT		UNSIGNED NOT NULL DEFAULT 0,	#方向
	`skills`	VARCHAR(512) NOT NULL DEFAULT "",		#技能
	`buffs`		VARCHAR(512) NOT NULL DEFAULT "",		#技能buff
	`taskmask`	VARCHAR(512) NOT NULL DEFAULT "",		#任务掩码
	`tasks`		VARCHAR(2048) NOT NULL DEFAULT "",		#任务
	`storymask`	VARCHAR(128) NOT NULL DEFAULT "",		#剧情掩码
	`story`		SMALLINT UNSIGNED NOT NULL DEFAULT 0,	#当前剧情
	`pettranslist`	VARCHAR(64)	NOT NULL DEFAULT "",	#宠物幻化列表
	`petsoulval`	INT UNSIGNED NOT NULL DEFAULT 0,	#聚魂值
	`petsoul`	VARCHAR(512) NOT NULL DEFAULT '',		#兽魂
	`petexplore`	VARCHAR(1024) NOT NULL DEFAULT '',	#宠物探险
	`petfighting`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#出战中宠物
	`options`	INT		UNSIGNED NOT NULL DEFAULT 0,	#游戏选项
	`itemcd`	VARCHAR(256) NOT NULL DEFAULT "",		#道具cd
	`copytimes`	VARCHAR(512) NOT NULL DEFAULT "",		#副本次数
	`packsize`	TINYINT	UNSIGNED NOT NULL DEFAULT 0,	#包裹大小
	`storagesize`	TINYINT UNSIGNED NOT NULL DEFAULT 0,#仓库大小 
	`routine`	VARCHAR(256) NOT NULL DEFAULT "",		#日常信息	
	`giftbag`	VARCHAR(32)	NOT NULL DEFAULT "",		#礼包
	`autohook`	VARCHAR(256) NOT NULL DEFAULT "",		#自动挂机
	`escortinfo`	VARCHAR(16)	NOT NULL DEFAULT "",	#护送信息
	`titlemask`	VARCHAR(32)	NOT NULL DEFAULT "",		#称号掩码
	`titletime`	VARCHAR(512) NOT NULL DEFAULT "",		#限时称号获得时间
	`title`		TINYINT UNSIGNED NOT NULL DEFAULT 0,	#当前称号
	`tribeskill`	VARCHAR(128) NOT NULL DEFAULT "",	#部落技能
	`actionflag`	INT	UNSIGNED NOT NULL DEFAULT 0,	#行为标识
	`mallbuy`		VARCHAR(256) NOT NULL DEFAULT "",		#商城限量商品购买信息
	`signin`		VARCHAR(128) NOT NULL DEFAULT "",		#签到
	`babelinfo`		VARCHAR(3500) NOT NULL DEFAULT "",      #通天塔
	`updatebulletin`	INT	UNSIGNED NOT NULL DEFAULT 100,	#更新公告	
	`twsk`			VARCHAR(256) NOT NULL DEFAULT "",      	#天外石库
	`darktower`		VARCHAR(128)	NOT NULL DEFAULT "",	#黑暗塔
	`revivecost`	INT UNSIGNED NOT NULL DEFAULT 1,	#连续原地复活次数
	`revivetime`	INT UNSIGNED NOT NULL DEFAULT 0,	#复活时间
	`continuousdays`	INT UNSIGNED NOT NULL DEFAULT 1,	#创角连续天数
	`funcflag`		VARCHAR(32)	NOT NULL DEFAULT "",	#功能开启标志位
	`mount`			VARCHAR(320) NOT NULL DEFAULT "",   #坐骑
	`feed`			VARCHAR(64)	NOT NULL DEFAULT "",	#feed分享信息
	`actioncount`	VARCHAR(512) NOT NULL DEFAULT "",	#行为计数
	`itemlock`		VARCHAR(256) NOT NULL DEFAULT "",   #道具加锁
	`expretrieve`	VARCHAR(1024)	NOT NULL DEFAULT "",#经验找回
	`wing`			VARCHAR(128)	NOT NULL DEFAULT "",	#翅膀信息
	`dujie`			VARCHAR(512) NOT NULL DEFAULT "",    #渡劫
	`equipshop`		VARCHAR(512) NOT NULL DEFAULT "",    #随机装备商店
	`dragonmark`	VARCHAR(256) NOT NULL DEFAULT "",    #龙纹
	`fashion`		VARCHAR(1024) NOT NULL DEFAULT "",   #时装	
	`growthscene`	VARCHAR(1024) NOT NULL DEFAULT "",   #成长副本
	`sp` int(10) unsigned NOT NULL DEFAULT '0',				#SP值
	`isawaken` binary(3) NOT NULL DEFAULT '0\0\0',			#是否觉醒
	`skillbar` varchar(512) NOT NULL DEFAULT '',			#技能快捷栏
	`itembar` varchar(512) NOT NULL DEFAULT '',				#物品快捷栏
	PRIMARY KEY(`guid`),
	INDEX(`accid`)
);
