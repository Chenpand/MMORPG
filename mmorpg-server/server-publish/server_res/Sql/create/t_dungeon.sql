CREATE TABLE `t_dungeon` (
`guid`  bigint(20) NOT NULL ,
`owner`  bigint(20) NOT NULL ,
`dungeon_id`  int(20) UNSIGNED NOT NULL ,
`num`  int(20) UNSIGNED NOT NULL ,
`best_score`  tinyint(20) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;