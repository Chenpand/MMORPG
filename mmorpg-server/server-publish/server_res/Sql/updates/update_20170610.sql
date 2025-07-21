ALTER TABLE `t_player_info` ADD COLUMN `platform`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `auction_addition_booth`;
ALTER TABLE `t_player_info` ADD COLUMN `openid`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `platform`;
ALTER TABLE `t_player_info` ADD COLUMN `season_uplevel_records` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '';
ALTER TABLE `t_player_info` ADD COLUMN `season_attr` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '';
ALTER TABLE `t_player_info` ADD COLUMN `season_attr_end_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '';
ALTER TABLE `t_player_info` ADD COLUMN `last_reset_gjarpoint`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `season_attr_end_time`;
ALTER TABLE `t_player_info` ADD COLUMN `last_reset_mjarpoint`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `last_reset_gjarpoint`;
ALTER TABLE `t_player_info` ADD COLUMN `season_king_mark_count` smallint(5) UNSIGNED NOT NULL DEFAULT 0 COMMENT '';
ALTER TABLE `t_player_info` ADD COLUMN `goldjar_point`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `season_king_mark_count`;
ALTER TABLE `t_player_info` ADD COLUMN `magjar_point`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `goldjar_point`;

ALTER TABLE `t_sortlist` ADD COLUMN `update_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `value3`;

ALTER TABLE `t_item` ADD COLUMN `fashionattrid`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `setqlnum`;
ALTER TABLE `t_item` ADD COLUMN `fashionattrnum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `fashionattrid`;

ALTER TABLE `t_auction_new` ADD COLUMN `item_src_guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_level`;
ALTER TABLE `t_auction_recommend_price` ADD COLUMN `strengthen`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_type_id`;
ALTER TABLE `t_auction_recommend_price` ADD COLUMN `recent_price`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `price`;

