CREATE TABLE `t_guild_member` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`guildid`  bigint(20) UNSIGNED NOT NULL ,
`post`  tinyint(3) UNSIGNED NOT NULL ,
`name`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`occu`  tinyint(3) UNSIGNED NOT NULL ,
`level`  smallint(5) UNSIGNED NOT NULL ,
`totalcontri`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`entertime`  int(10) UNSIGNED NOT NULL ,
`offlinetime`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`leavetime`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;