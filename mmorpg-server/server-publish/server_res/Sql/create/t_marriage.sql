#婚礼庆典表
CREATE TABLE IF NOT EXISTS `t_marriage`(
	`guid`		BIGINT	UNSIGNED NOT NULL,		#预约人id
	`name`		VARCHAR(32)	NOT NULL,			#预约人名字
	`occu`		TINYINT	UNSIGNED NOT NULL,		#职业
	`sex`		TINYINT	UNSIGNED NOT NULL,		#性别
	`gift`		VARCHAR(32)	NOT NULL default "",	#贺礼信息
	`loverId`	BIGINT	UNSIGNED NOT NULL,		#伴侣id
	`loverName`	VARCHAR(32)	NOT NULL,			#伴侣名字
	`loverOccu`	TINYINT UNSIGNED NOT NULL,		#伴侣职业
	`loverSex`	TINYINT	UNSIGNED NOT NULL,		#伴侣性别
	`loverGift`	VARCHAR(32)	NOT NULL default "",	#伴侣贺礼信息
	`serial`	TINYINT	UNSIGNED NOT NULL,		#场次
	`type`		TINYINT UNSIGNED NOT NULL,		#类型
	`hascruised`	TINYINT UNSIGNED NOT NULL default 0, #是否巡游了
	`hasrited`	TINYINT	UNSIGNED NOT NULL default 0,	#是否拜过
	`haddinner`	TINYINT	UNSIGNED NOT NULL default 0,	#是否宴请过
	PRIMARY KEY(`guid`)
);

#婚礼宾客表
CREATE TABLE IF NOT EXISTS `t_marriage_guest`(
	`guid`		BIGINT	UNSIGNED NOT NULL,		#guid
	`guestId`	BIGINT	UNSIGNED NOT NULL,		#宾客id
	`guestName`	VARCHAR(32)	NOT NULL,			#宾客名字
	`serial`	TINYINT	UNSIGNED NOT NULL,		#场次
	PRIMARY KEY(`guid`)
);


