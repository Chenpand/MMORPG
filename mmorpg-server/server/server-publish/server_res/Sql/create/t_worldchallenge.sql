CREATE TABLE IF NOT EXISTS `t_worldchallenger`(
	`guid`	BIGINT		UNSIGNED NOT NULL,				#主键，玩家id
	`name`	VARCHAR(32)	NOT NULL,						#名字
	`level`	SMALLINT	UNSIGNED NOT NULL DEFAULT 1,	#等级
	`occu`	TINYINT		UNSIGNED NOT NULL DEFAULT 0,	#职业
	`score`	INT			UNSIGNED NOT NULL DEFAULT 0,	#积分
	`zoneId` SMALLINT 	UNSIGNED NOT NULL DEFAULT 0,	#区号
	`power`	INT			UNSIGNED NOT NULL DEFAULT 0,	#战力
	`group`	TINYINT		UNSIGNED NOT NULL DEFAULT 0,	#分组
	`awardFlag`	INT	UNSIGNED NOT NULL DEFAULT 0,		#积分奖励标志位
	PRIMARY KEY(`guid`)
);

CREATE TABLE IF NOT EXISTS `t_worldchallenger_ranking`(
	`guid`	BIGINT		UNSIGNED NOT NULL,				#主键，玩家id
	`ranking`	INT		UNSIGNED NOT NULL,				#名次
	PRIMARY KEY(`guid`)
);

#全局游戏参数表
CREATE TABLE IF NOT EXISTS `t_c_gameparam`(
	`name`	VARCHAR(32)	NOT NULL,				#名字 
	`value`	INT	UNSIGNED NOT NULL DEFAULT 0, 	#值
	PRIMARY KEY(`name`)
);
