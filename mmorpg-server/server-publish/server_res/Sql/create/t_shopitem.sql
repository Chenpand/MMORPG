CREATE TABLE `t_shopitem` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`shopid`  bigint(20) UNSIGNED NOT NULL ,
`shopitemid`  int(10) UNSIGNED NOT NULL ,
`owner`  bigint(20) UNSIGNED NOT NULL ,
`grid`  int(10) UNSIGNED NOT NULL ,
`restnum`  int(10) UNSIGNED NOT NULL ,
`viplv`  int(10) UNSIGNED NOT NULL ,
`vipdiscount`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`),
INDEX `guid` USING BTREE (`guid`) 
)
;