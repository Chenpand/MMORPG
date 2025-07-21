CREATE TABLE IF NOT EXISTS `t_activitygiftbag` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#拥有者
	`dataid`	INT	UNSIGNED NOT NULL,			#礼包id
	`state`		TINYINT	UNSIGNED NOT NULL,		#状态
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);