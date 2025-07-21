CREATE TABLE `t_red_packet_receiver` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`receiver_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`name`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`red_packet_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`occu`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`level`  smallint(10) UNSIGNED NOT NULL DEFAULT 0 ,
`money`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;