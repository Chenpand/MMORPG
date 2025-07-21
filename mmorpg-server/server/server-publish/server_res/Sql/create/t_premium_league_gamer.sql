CREATE TABLE `t_premium_league_gamer` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`name`  char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`occu`  tinyint(10) UNSIGNED NOT NULL ,
`level`  smallint(10) UNSIGNED NOT NULL ,
`enroll_time`  int(10) UNSIGNED NOT NULL ,
`enroll_count`  tinyint(10) UNSIGNED NOT NULL ,
`remain_hp`  int(10) UNSIGNED NOT NULL ,
`remain_mp`  int(10) UNSIGNED NOT NULL ,
`win_num`  tinyint(10) UNSIGNED NOT NULL ,
`win_time`  int(10) UNSIGNED NOT NULL ,
`final_battle_win_num`  tinyint(10) UNSIGNED NOT NULL ,
`ranking`  int(10) UNSIGNED NOT NULL ,
`next_roll_pos`  int(10) UNSIGNED NOT NULL ,
`battle_flag`  tinyint(10) UNSIGNED NOT NULL ,
`is_finish_roll_battle`  tinyint(10) UNSIGNED NOT NULL ,
`score`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;