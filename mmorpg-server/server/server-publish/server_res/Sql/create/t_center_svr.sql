#惩罚表
CREATE TABLE IF NOT EXISTS `t_g_punishment` (
	`accid`		INT		UNSIGNED NOT NULL,		#账号id
	`duetime`	INT 	UNSIGNED NOT NULL,		#到期时间
	PRIMARY KEY(`accid`)
);

#全局游戏参数表
CREATE TABLE IF NOT EXISTS `t_g_gameparam`(
	`name`	VARCHAR(32)	NOT NULL,		#名字 
	`value`	INT	UNSIGNED NOT NULL DEFAULT 0, #值
	PRIMARY KEY(`name`)
);