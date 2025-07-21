ALTER TABLE `t_auction_new` ADD COLUMN `on_sale_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `duetime`;
ALTER TABLE `t_auction_new` ADD COLUMN `pvpdisphyatk`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `dismagdef`;
ALTER TABLE `t_auction_new` ADD COLUMN `pvpdismagatk`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdisphyatk`;
ALTER TABLE `t_auction_new` ADD COLUMN `pvpdisphydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdismagatk`;
ALTER TABLE `t_auction_new` ADD COLUMN `pvpdismagdef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdisphydef`;
ALTER TABLE `t_auction_new` ADD COLUMN `strenthnum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdismagdef`;
ALTER TABLE `t_auction_new` ADD COLUMN `valuedscore`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `strenthnum`;
ALTER TABLE `t_auction_new` ADD COLUMN `setqlnum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `valuedscore`;

ALTER TABLE `t_guild` MODIFY COLUMN `shop_refresh_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `shopid`;

ALTER TABLE `t_item` ADD COLUMN `setqlnum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `valuedscore`;
ALTER TABLE `t_mailitem` ADD COLUMN `pvpdisphyatk`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `sealcount`;
ALTER TABLE `t_mailitem` ADD COLUMN `pvpdismagatk`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdisphyatk`;
ALTER TABLE `t_mailitem` ADD COLUMN `pvpdisphydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdismagatk`;
ALTER TABLE `t_mailitem` ADD COLUMN `pvpdismagdef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdisphydef`;
ALTER TABLE `t_mailitem` ADD COLUMN `strenthnum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `pvpdismagdef`;
ALTER TABLE `t_mailitem` ADD COLUMN `valuedscore`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `strenthnum`;
ALTER TABLE `t_mailitem` ADD COLUMN `setqlnum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `valuedscore`;

CREATE INDEX `name` ON `t_player_info`(`name`) USING BTREE ;
ALTER TABLE `t_player_info` ADD COLUMN `auction_refresh_time`  int(10) UNSIGNED NOT NULL DEFAULT 0;
ALTER TABLE `t_player_info` ADD COLUMN `auction_addition_booth`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `auction_refresh_time`;

ALTER TABLE `t_offline_event` MODIFY COLUMN `type`  smallint(5) UNSIGNED NOT NULL AFTER `owner`;
ALTER TABLE `t_offline_event` MODIFY COLUMN `time`  int(10) UNSIGNED NOT NULL AFTER `type`;
ALTER TABLE `t_offline_event` MODIFY COLUMN `data`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `time`;

ALTER TABLE `t_sortlist` MODIFY COLUMN `name` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci