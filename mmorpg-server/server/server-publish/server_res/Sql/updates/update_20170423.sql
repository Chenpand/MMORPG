CREATE TABLE `t_mall` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`type`  tinyint(3) UNSIGNED NOT NULL ,
`subtype`  tinyint(3) UNSIGNED NOT NULL ,
`jobtype`  tinyint(3) UNSIGNED NOT NULL ,
`itemid`  int(10) UNSIGNED NOT NULL ,
`groupnum`  int(10) UNSIGNED NOT NULL ,
`price`  int(10) UNSIGNED NOT NULL ,
`disprice`  int(10) UNSIGNED NOT NULL ,
`moneytype`  tinyint(3) UNSIGNED NOT NULL ,
`limittype`  int(10) UNSIGNED NOT NULL ,
`limitnum`  int(10) UNSIGNED NOT NULL ,
`isgiftpack`  tinyint(3) UNSIGNED NOT NULL ,
`giftpackitems`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`giftpackicon`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`limtstarttime`  int(10) UNSIGNED NOT NULL ,
`limitendtime`  int(10) UNSIGNED NOT NULL ,
`limitetotlenum`  int(10) UNSIGNED NOT NULL ,
`vipscore`  int(10) UNSIGNED NOT NULL ,
`giftpackname`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`ishot`  tinyint(3) UNSIGNED NOT NULL ,
`sort`  int(10) UNSIGNED NOT NULL ,
`hotsort`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=latin1 COLLATE=latin1_swedish_ci
ROW_FORMAT=Compact
;

ALTER TABLE `t_offline_event` MODIFY COLUMN `data`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `time`;

ALTER TABLE `t_player_info` MODIFY COLUMN `totlechargenum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `bindpoint`;

ALTER TABLE `t_player_info` MODIFY COLUMN `totleonlinetime`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `totlechargenum`;