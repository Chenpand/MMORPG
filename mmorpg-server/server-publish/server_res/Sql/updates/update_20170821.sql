ALTER TABLE `t_mall` CHANGE COLUMN `isgiftpack` `goods_type` tinyint(3) UNSIGNED NOT NULL AFTER `hotsort`;

CREATE TABLE `t_mall_gift_pack` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`accid`  int(10) UNSIGNED NOT NULL ,
`roleid`  bigint(20) UNSIGNED NOT NULL ,
`gift_pack_id`  int(10) UNSIGNED NOT NULL ,
`activate_start_time`  int(20) UNSIGNED NOT NULL ,
`activate_end_time`  int(20) UNSIGNED NOT NULL ,
`rest_num`  smallint(5) UNSIGNED NOT NULL ,
`activate_state`  tinyint(3) UNSIGNED NOT NULL ,
`activate_cond`  tinyint(3) UNSIGNED NOT NULL ,
`activate_counter`  smallint(5) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`),
INDEX `accid` (`accid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;