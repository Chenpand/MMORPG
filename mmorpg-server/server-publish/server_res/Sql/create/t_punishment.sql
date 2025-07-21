#惩罚表
CREATE TABLE IF NOT EXISTS `t_punishment` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`accid`		INT		UNSIGNED NOT NULL,		#账号id
	`type`		TINYINT	UNSIGNED NOT NULL,		#惩罚类型
	`duetime`	INT 	UNSIGNED NOT NULL,		#到期时间
	PRIMARY KEY(`guid`)
);
