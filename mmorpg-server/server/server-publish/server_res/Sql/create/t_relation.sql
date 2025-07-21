CREATE TABLE IF NOT EXISTS `t_relation` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#拥有者
	`type`		TINYINT	UNSIGNED NOT NULL,		#类型	
	`id`		BIGINT	UNSIGNED NOT NULL,		#对方
	`accid`		INT	UNSIGNED NOT NULL,			#对方账号id
	`name`		VARCHAR(32)	NOT NULL,			#对方名字
	`sex`		TINYINT	UNSIGNED NOT NULL,		#对方性别
	`level`		SMALLINT	UNSIGNED NOT NULL,	#对方等级
	`occu`		TINYINT	UNSIGNED NOT NULL,		#对方职业
	`power`		INT	UNSIGNED NOT NULL DEFAULT 0,	#对方战斗力
	`qqvip`		TINYINT UNSIGNED NOT NULL DEFAULT 0,	#对方qqvip等级
	`issb`		TINYINT UNSIGNED NOT NULL DEFAULT 0,	#对方是否超级蓝钻
	`addtime`	INT	UNSIGNED NOT NULL,			#关系建立时间
	`intimacy`	SMALLINT	UNSIGNED NOT NULL DEFAULT 0,	#亲密度
	`onlinenotify`	TINYINT	UNSIGNED NOT NULL,	#是否上线通知
	PRIMARY KEY(`guid`),
	INDEX(`owner`),
	INDEX(`id`)
);
