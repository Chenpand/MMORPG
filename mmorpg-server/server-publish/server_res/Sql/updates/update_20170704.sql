CREATE TABLE `t_activity_op` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`tmptype`  smallint(5) UNSIGNED NOT NULL ,
`name`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`tag`  tinyint(3) UNSIGNED NOT NULL ,
`tagendtime`  int(10) UNSIGNED NOT NULL ,
`preparetime`  int(10) UNSIGNED NOT NULL ,
`starttime`  int(10) UNSIGNED NOT NULL ,
`endtime`  int(10) UNSIGNED NOT NULL ,
`desc`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`ruledesc`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`circletype`  tinyint(3) UNSIGNED NOT NULL ,
`tasks`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`taskdesc`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`taskrewards`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=latin1 COLLATE=latin1_swedish_ci
ROW_FORMAT=Compact
;
CREATE TABLE `t_activity_op_task` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`task_info_str`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=latin1 COLLATE=latin1_swedish_ci
ROW_FORMAT=Compact
;
CREATE TABLE `t_jar_record` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`jar_id`  int(10) UNSIGNED NOT NULL ,
`name`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`item_id`  int(10) UNSIGNED NOT NULL ,
`num`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`),
INDEX `jar_id` (`jar_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=latin1 COLLATE=latin1_swedish_ci
ROW_FORMAT=Compact
;