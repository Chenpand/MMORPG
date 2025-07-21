CREATE TABLE `t_charge_record` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`mall_type`  tinyint(10) UNSIGNED NOT NULL ,
`goodsid`  int(10) UNSIGNED NOT NULL ,
`accid`  int(10) UNSIGNED NOT NULL ,
`roleid`  bigint(20) UNSIGNED NOT NULL ,
`today_times`  int(10) UNSIGNED NOT NULL ,
`total_times`  int(10) UNSIGNED NOT NULL ,
`update_time`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;