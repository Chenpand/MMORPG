#计数表
CREATE TABLE IF NOT EXISTS `t_counter` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#计数拥有者
	`name`		VARCHAR(16) NOT NULL,			#计数名
	`updatetime`	INT	UNSIGNED NOT NULL,		#更新时间
	`value`		INT	UNSIGNED NOT NULL,			#值
	PRIMARY KEY(`guid`),
	INDEX(`owner`),
	INDEX(`name`)
);
