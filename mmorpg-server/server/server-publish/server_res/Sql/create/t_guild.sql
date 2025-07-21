CREATE TABLE `t_guild` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`name`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`level`  tinyint(3) UNSIGNED NOT NULL DEFAULT 1 ,
`setuptime`  int(10) UNSIGNED NOT NULL ,
`fund`  int(11) NOT NULL DEFAULT 0 ,
`declaration`  varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '0' ,
`announcement`  varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`buildings`  varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`donate_logs`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`shopid`  tinyint(10) UNSIGNED NOT NULL COMMENT '…ÃµÍID' ,
`shop_refresh_time`  int(10) UNSIGNED NOT NULL ,
`dismiss_time`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;