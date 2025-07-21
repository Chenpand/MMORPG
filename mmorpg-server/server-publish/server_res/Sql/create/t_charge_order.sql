CREATE TABLE `t_charge_order` (
`guid`  bigint(10) NOT NULL ,
`orderid`  varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`status`  tinyint(10) UNSIGNED NOT NULL ,
`accid`  int(10) UNSIGNED NOT NULL ,
`roleid`  bigint(20) UNSIGNED NOT NULL ,
`charge_goods_id`  int(10) UNSIGNED NOT NULL ,
`charge_item_id`  int(10) UNSIGNED NOT NULL ,
`charge_item_num`  int(10) UNSIGNED NOT NULL ,
`vip_score`  int(10) UNSIGNED NOT NULL ,
`charge_money`  int(10) UNSIGNED NOT NULL ,
`time`  int(10) UNSIGNED NOT NULL 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;