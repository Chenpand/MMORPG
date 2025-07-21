CREATE TABLE `t_red_packet` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`owner_id`  bigint(20) UNSIGNED NOT NULL ,
`owner_name`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`owner_level`  smallint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`owner_occu`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`type`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`status`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`reason`  smallint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`cond1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`total_money`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`remain_money`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`num`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`status_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;