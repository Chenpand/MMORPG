CREATE TABLE IF NOT EXISTS `t_merchant_rec` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`playerid`	BIGINT	UNSIGNED NOT NULL,		#玩家id
	`name`		VARCHAR(32)	NOT NULL,			#玩家名字
	`itemid`	INT	UNSIGNED NOT NULL,			#道具id
	`itemnum`	INT	UNSIGNED NOT NULL,			#道具数量
	`merchantlv`	TINYINT	UNSIGNED NOT NULL,			#商店等级
	`gettime`	INT	UNSIGNED NOT NULL,			#获得的时间

	PRIMARY KEY(`guid`)
);

##删除20天前的购买记录
CREATE EVENT IF NOT EXISTS event_clearmerchantrec ON SCHEDULE EVERY 600 SECOND
ON COMPLETION PRESERVE ENABLE
DO DELETE FROM `t_merchant_rec` WHERE UNIX_TIMESTAMP() > `gettime` + 1728000;
