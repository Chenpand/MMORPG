#部落表
CREATE TABLE IF NOT EXISTS `t_tribe` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`name`		VARCHAR(32)	 NOT NULL,			#名字
	`level`		TINYINT UNSIGNED NOT NULL DEFAULT 1,	#等级
	`setuptime`	INT	UNSIGNED NOT NULL,					#成立时间
	`impeachtime`	INT UNSIGNED NOT NULL DEFAULT 0,	#弹劾时间
	`impeachid`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#弹劾人
	`color`		TINYINT UNSIGNED NOT NULL DEFAULT 0,	#颜色
	`fund`		INT	NOT NULL DEFAULT 0,					#资金
	`wood`		INT	UNSIGNED NOT NULL DEFAULT 0,		#木材
	`beastSoul`	INT	UNSIGNED NOT NULL DEFAULT 0,		#兽魂
	`metal`		INT	UNSIGNED NOT NULL DEFAULT 0,		#金属
	`ranking`	SMALLINT UNSIGNED NOT NULL DEFAULT 0,	#排名
	`icon`		TINYINT UNSIGNED NOT NULL DEFAULT 0,	#图标
	`qq`		INT	UNSIGNED NOT NULL DEFAULT 0,		#QQ群
	`yy`		INT	UNSIGNED NOT NULL DEFAULT 0,		#语音频道	
	`announce`	VARCHAR(256)	 NOT NULL DEFAULT "",	#公告
	`totem`		TINYINT UNSIGNED NOT NULL DEFAULT 1,	#图腾等级
	`folkhouse`	TINYINT UNSIGNED NOT NULL DEFAULT 1,	#民居等级
	`college`	TINYINT UNSIGNED NOT NULL DEFAULT 1,	#书院等级
	`workshop`	TINYINT UNSIGNED NOT NULL DEFAULT 1,	#工坊等级
	`treasure`	TINYINT UNSIGNED NOT NULL DEFAULT 1,	#宝库等级
	`tribeshop`	VARCHAR(256)	NOT NULL DEFAULT "",	#部落商店信息
	`bosslist`	VARCHAR(128)	NOT NULL DEFAULT "",	#已召唤部落BOSS列表
	`transtime`	INT	UNSIGNED NOT NULL DEFAULT 0,		#开启部落运送的时间
	`transstate`	TINYINT UNSIGNED NOT NULL DEFAULT 0, #部落运送状态
	`transinfo`	VARCHAR(128)	NOT NULL DEFAULT "",	#部落运送信息 格式：品质;地图id;x,y;护送者
	`dismisstime`	INT	UNSIGNED NOT NULL DEFAULT 0,	#欠费解散时间
	`robsouldaily`	SMALLINT UNSIGNED NOT NULL DEFAULT 0,	#每日截取兽魂数	
	`uniontribe`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#结盟部落ID
	`unioncd`	INT	UNSIGNED NOT NULL DEFAULT 0,	#结盟CD时间
	`animal`	VARCHAR(32)	 NOT NULL DEFAULT "",		#部落神兽
	`moneytree`		VARCHAR(64)	 NOT NULL DEFAULT "",	#部落摇钱树
	`pickchest`	VARCHAR(32)	 NOT NULL DEFAULT "",		#部落摘宝箱	
	PRIMARY KEY(`guid`)
);

#部落成员表
CREATE TABLE IF NOT EXISTS `t_tribe_member` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#成员id
	`name`		VARCHAR(32)	 NOT NULL,			#名字
	`sex`		TINYINT UNSIGNED NOT NULL,		#性别
	`occu`		TINYINT UNSIGNED NOT NULL,		#职业
	`tribe`		BIGINT	UNSIGNED NOT NULL,		#所属部落
	`post`		TINYINT	UNSIGNED NOT NULL,		#职位
	`level`		SMALLINT UNSIGNED NOT NULL,		#等级
	`vip`		TINYINT	UNSIGNED NOT NULL DEFAULT 0,	#vip等级
	`contribution`	INT	UNSIGNED NOT NULL DEFAULT 0,	#贡献度
	`totalcontri`	INT	UNSIGNED NOT NULL DEFAULT 0,	#累计贡献度
	`power`		INT	UNSIGNED NOT NULL DEFAULT 0,		#战斗力
	`entertime`	INT		UNSIGNED NOT NULL,				#加入时间
	`leavetime`	INT		UNSIGNED NOT NULL DEFAULT 0,	#离开时间， 离开24小时之类保留数据
	`offlinetime`	INT	UNSIGNED NOT NULL DEFAULT 0,	#离线时间
	`tribeshopbuy`	VARCHAR(128) NOT NULL DEFAULT "",	#部落商店商品购买信息
	`translot`	TINYINT	UNSIGNED NOT NULL DEFAULT 0,	#运送份额
	`feedanimalstate` TINYINT UNSIGNED NOT NULL DEFAULT 0,#饲养部落神兽状态
	`tribemoneytree` VARCHAR(32) NOT NULL DEFAULT "",	#摇钱树成员信息
	PRIMARY KEY(`guid`)
);

#部落领土表
CREATE TABLE IF NOT EXISTS `t_tribe_territory` (
	`guid`		BIGINT 	UNSIGNED NOT NULL,		#主键
	`territory`		TINYINT UNSIGNED NOT NULL,		#领土id
	`owner`		BIGINT	UNSIGNED NOT NULL DEFAULT 0,		#所属部落id
	`secowner`		BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#第二所有者部落id
	`challenger`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#挑战部落id
	`secChal`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#出价第二的挑战部落id
	`aassist`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#进攻方协助部落id
	`dassist`	BIGINT	UNSIGNED NOT NULL DEFAULT 0,	#防守方协助部落id
	`warstarttime`	INT	UNSIGNED NOT NULL DEFAULT 0,		#部落战开始时间
	`warstatus`	TINYINT	UNSIGNED NOT NULL DEFAULT 0,		#部落状态
	PRIMARY KEY(`guid`)
);

#部落战竞标表
CREATE TABLE IF NOT EXISTS `t_tribewar_bid` (
	`guid`		BIGINT 	UNSIGNED NOT NULL,		#主键
	`territory`	TINYINT	UNSIGNED NOT NULL,		#领土id
	`tribe`		BIGINT 	UNSIGNED NOT NULL,		#部落id
	`player`	BIGINT	UNSIGNED NOT NULL,		#玩家id
	`num`		INT		UNSIGNED NOT NULL,		#出资数量
	`bidtime`	INT	UNSIGNED NOT NULL,			#竞标时间
	PRIMARY KEY(`guid`)
);

#部落道具表
CREATE TABLE IF NOT EXISTS `t_tribe_item` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#拥有者
	`itemid`	INT		UNSIGNED NOT NULL,		#道具id
	`num`		SMALLINT	UNSIGNED NOT NULL,	#叠加数
	`bind`		TINYINT		UNSIGNED NOT NULL,	#是否绑定
	`pack`		TINYINT		UNSIGNED NOT NULL,	#所在包裹
	`quality`		TINYINT	UNSIGNED NOT NULL,	#品质
	`strengthen`	TINYINT UNSIGNED NOT NULL,	#强化等级
	`randattr`		CHAR(255)	NOT NULL,		#随机属性
	`holenum`		TINYINT	UNSIGNED NOT NULL,	#孔数
	`gemstone`		CHAR(64)	NOT NULL,		#宝石
	`rune`			VARCHAR(32)	NOT NULL,		#符文
	`param1`	INT		UNSIGNED NOT NULL,		#参数1
	`param2`	INT		UNSIGNED NOT NULL,		#参数2
	`deadline`	INT		UNSIGNED NOT NULL,		#道具的失效时间
	`price`	INT		UNSIGNED NOT NULL,		#道具的定价
	`strfailed`		SMALLINT	UNSIGNED NOT NULL,	#强化失败次数
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);

#部落日志表
CREATE TABLE IF NOT EXISTS `t_tribe_log` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#部落
	`logtype`	SMALLINT	UNSIGNED NOT NULL,		#日志类型
	`content`	VARCHAR(255)	NOT NULL,		#日志内容
	`logtime`	INT		UNSIGNED NOT NULL,		#记录时间
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);

##删除20天前的部落日志
CREATE EVENT IF NOT EXISTS event_cleartribelog ON SCHEDULE EVERY 600 SECOND
ON COMPLETION PRESERVE ENABLE
DO DELETE FROM `t_tribe_log` WHERE UNIX_TIMESTAMP() > `logtime` + 1728000;
