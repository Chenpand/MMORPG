CREATE TABLE `t_shop` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`shopid`  int(10) UNSIGNED NOT NULL ,
`owner`  bigint(20) UNSIGNED NOT NULL ,
`refreshtime`  int(10) UNSIGNED NOT NULL ,
`refreshnum`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`),
INDEX `guid` USING BTREE (`guid`) 
)
;