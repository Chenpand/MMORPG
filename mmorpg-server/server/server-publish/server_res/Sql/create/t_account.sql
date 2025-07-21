CREATE TABLE `t_account` (
`guid`  bigint(10) UNSIGNED NOT NULL ,
`accid`  int(10) UNSIGNED NOT NULL COMMENT '’À∫≈' ,
`point`  int(10) UNSIGNED NOT NULL COMMENT 'µ„Ñª' ,
`reg_time`  int(10) NOT NULL ,
PRIMARY KEY (`guid`),
INDEX `accid` (`accid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;