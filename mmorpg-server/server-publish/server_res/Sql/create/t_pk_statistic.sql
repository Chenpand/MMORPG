CREATE TABLE `t_pk_statistic` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`owner`  bigint(20) UNSIGNED NOT NULL COMMENT '玩家角色ID' ,
`type`  tinyint(10) UNSIGNED NOT NULL COMMENT 'pk类型' ,
`total_win_num`  int(10) UNSIGNED NOT NULL COMMENT '总胜场' ,
`total_lose_num`  int(10) UNSIGNED NOT NULL COMMENT '总负场' ,
`total_num`  int(10) UNSIGNED NOT NULL COMMENT '总场数' ,
`detail_record_info`  varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '对战不同职业的胜负信息，编码方式：occu1:win:lose:total|occu2:win:lose:total' ,
`recent_record`  char(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '近期战绩，记录最近10场的结果，编码格式：01001，0代表输，1代表赢，2代表平局，最前面的最早的那场' ,
`max_win_streak`  int(10) UNSIGNED NOT NULL COMMENT '最多连胜场数' ,
`cur_win_streak`  int(10) UNSIGNED NOT NULL COMMENT '当前连胜场数' ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;