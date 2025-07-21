CREATE TABLE `t_auction_recommend_price` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`item_type_id`  int(10) UNSIGNED NOT NULL ,
`price`  int(10) UNSIGNED NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Dynamic
;