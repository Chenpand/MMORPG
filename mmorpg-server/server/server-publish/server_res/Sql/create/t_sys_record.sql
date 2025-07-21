CREATE TABLE `t_sys_record` (
`guid`  bigint(10) UNSIGNED NOT NULL ,
`behavior`  int(10) UNSIGNED NOT NULL ,
`owner`  bigint(10) UNSIGNED NOT NULL ,
`param1`  int(10) UNSIGNED NOT NULL ,
`num`  int(10) UNSIGNED NOT NULL ,
`record_time`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;