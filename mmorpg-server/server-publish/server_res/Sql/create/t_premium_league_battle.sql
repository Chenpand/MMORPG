CREATE TABLE `t_premium_league_battle` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`race_id`  bigint(20) UNSIGNED NOT NULL ,
`type`  tinyint(10) UNSIGNED NOT NULL ,
`role_id1`  bigint(20) UNSIGNED NOT NULL ,
`role_name1`  char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`role_occu1`  tinyint(10) UNSIGNED NOT NULL ,
`role_id2`  bigint(20) UNSIGNED NOT NULL ,
`role_name2`  char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`role_occu2`  tinyint(10) UNSIGNED NOT NULL ,
`is_end`  tinyint(10) UNSIGNED NOT NULL ,
`winner`  bigint(20) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;