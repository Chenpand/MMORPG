#公告表
CREATE TABLE IF NOT EXISTS `t_announcement` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#
	`color`		TINYINT UNSIGNED NOT NULL,		#颜色
	`content`	VARCHAR(256)	NOT NULL,		#内容
	`begintime`	INT 	UNSIGNED NOT NULL,		#开始时间
	`interval`	INT		UNSIGNED NOT NULL,		#重复间隔
	`repeattimes`	INT	UNSIGNED NOT NULL,		#重复次数
	`showzone`	TINYINT	UNSIGNED NOT NULL,		#显示区域
	PRIMARY KEY(`guid`)
);
