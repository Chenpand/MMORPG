CREATE TABLE IF NOT EXISTS `t_auction` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#拥有者
	`type`		TINYINT		UNSIGNED NOT NULL,	#拍卖类型
	`price`	INT		UNSIGNED NOT NULL,			#价格
	`pricetype`		TINYINT		UNSIGNED NOT NULL,	#价格的货币类型
	`duetime`	INT		UNSIGNED NOT NULL,		#拍卖到期时间
	`auctionmoney`	INT		UNSIGNED NOT NULL DEFAULT 0,		#拍卖的货币数量
	`itemid`	INT		UNSIGNED NOT NULL DEFAULT 0,		#道具id
	`num`		SMALLINT	UNSIGNED NOT NULL DEFAULT 0,	#叠加数
	`bind`		TINYINT		UNSIGNED NOT NULL DEFAULT 0,	#是否绑定
	`quality`		TINYINT	UNSIGNED NOT NULL DEFAULT 0,	#品质
	`strengthen`	TINYINT UNSIGNED NOT NULL DEFAULT 0,	#强化等级
	`randattr`		VARCHAR(255)	NOT NULL DEFAULT "",	#随机属性
	`holenum`		TINYINT	UNSIGNED NOT NULL DEFAULT 0,	#孔数
	`gemstone`		VARCHAR(64)	NOT NULL DEFAULT "",		#宝石
	`rune`			VARCHAR(32)	NOT NULL,		#符文
	`param1`	INT		UNSIGNED NOT NULL DEFAULT 0,		#参数1
	`param2`	INT		UNSIGNED NOT NULL DEFAULT 0,		#参数2
	`deadline`	INT		UNSIGNED NOT NULL DEFAULT 0,		#道具的失效时间
	`strfailed`		SMALLINT	UNSIGNED NOT NULL,	#强化失败次数
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);