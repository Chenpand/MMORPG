ALTER TABLE `t_item` ADD COLUMN `addmagic`  varchar(32) NOT NULL DEFAULT '' AFTER `disdef`;
ALTER TABLE `t_item` DROP COLUMN `rune`;
ALTER TABLE `t_mailitem` ADD COLUMN `addmagic`  varchar(32) NOT NULL DEFAULT '' AFTER `disdef`;
ALTER TABLE `t_mailitem` DROP COLUMN `rune`;
ALTER TABLE `t_auction_new` ADD COLUMN `addmagic`  varchar(32) NOT NULL DEFAULT '' AFTER `disdef`;
ALTER TABLE `t_auction_new` DROP COLUMN `rune`;