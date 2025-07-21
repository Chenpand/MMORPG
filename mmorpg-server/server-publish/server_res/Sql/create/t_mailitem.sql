CREATE TABLE IF NOT EXISTS `t_mailitem` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`mailid`	BIGINT	UNSIGNED NOT NULL,		#邮件
	`owner`		BIGINT	UNSIGNED NOT NULL,		#邮件拥有者
	`itemid`	INT		UNSIGNED NOT NULL,		#道具id
	`num`		SMALLINT	UNSIGNED NOT NULL,	#叠加数
	`bind`		TINYINT		UNSIGNED NOT NULL,	#是否绑定
	`quality`		TINYINT	UNSIGNED NOT NULL,	#品质
	`strengthen`	TINYINT UNSIGNED NOT NULL,	#强化等级
	`randattr`		VARCHAR(255)	NOT NULL,		#随机属性
	`holenum`		TINYINT	UNSIGNED NOT NULL,	#孔数
	`gemstone`		VARCHAR(64)	NOT NULL,		#宝石
	`rune`			VARCHAR(32)	NOT NULL,		#符文
	`param1`	INT		UNSIGNED NOT NULL,		#参数1
	`param2`	INT		UNSIGNED NOT NULL,		#参数2
	`deadline`	INT		UNSIGNED NOT NULL,		#道具的失效时间
	`strfailed`		SMALLINT	UNSIGNED NOT NULL,	#强化失败次数
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);