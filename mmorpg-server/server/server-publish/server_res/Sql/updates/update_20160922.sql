ALTER TABLE `t_item` ADD COLUMN `phydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `magatk`;
ALTER TABLE `t_item` ADD COLUMN `magdef`  int(10) UNSIGNED NOT NULL AFTER `phydef`;
ALTER TABLE `t_item` ADD COLUMN `disphydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `dismagatk`;
ALTER TABLE `t_item` ADD COLUMN `dismagdef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `disphydef`;
ALTER TABLE `t_item` DROP COLUMN `defense`;
ALTER TABLE `t_item` DROP COLUMN `disdef`;
ALTER TABLE `t_mailitem` ADD COLUMN `phydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `magatk`;
ALTER TABLE `t_mailitem` ADD COLUMN `magdef`  int(10) UNSIGNED NOT NULL AFTER `phydef`;
ALTER TABLE `t_mailitem` ADD COLUMN `disphydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `dismagatk`;
ALTER TABLE `t_mailitem` ADD COLUMN `dismagdef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `disphydef`;
ALTER TABLE `t_mailitem` DROP COLUMN `defense`;
ALTER TABLE `t_mailitem` DROP COLUMN `disdef`;

ALTER TABLE `t_storage` ADD COLUMN `disphyatk`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `sealcount`;
ALTER TABLE `t_storage` ADD COLUMN `dismagatk`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `disphyatk`;

ALTER TABLE `t_storage` ADD COLUMN `phydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `magatk`;
ALTER TABLE `t_storage` ADD COLUMN `magdef`  int(10) UNSIGNED NOT NULL AFTER `phydef`;
ALTER TABLE `t_storage` ADD COLUMN `disphydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `dismagatk`;
ALTER TABLE `t_storage` ADD COLUMN `dismagdef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `disphydef`;
ALTER TABLE `t_storage` DROP COLUMN `defense`;
ALTER TABLE `t_storage` DROP COLUMN `disdef`;
ALTER TABLE `t_auction_new` ADD COLUMN `phydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `magatk`;
ALTER TABLE `t_auction_new` ADD COLUMN `magdef`  int(10) UNSIGNED NOT NULL AFTER `phydef`;
ALTER TABLE `t_auction_new` ADD COLUMN `disphydef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `dismagatk`;
ALTER TABLE `t_auction_new` ADD COLUMN `dismagdef`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `disphydef`;
ALTER TABLE `t_auction_new` DROP COLUMN `defense`;
ALTER TABLE `t_auction_new` DROP COLUMN `disdef`;

ALTER TABLE `t_mailitem` ADD COLUMN `daygiftnum`  tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` DROP COLUMN `qqvip`;
ALTER TABLE `t_storage` ADD COLUMN `daygiftnum`  tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_storage` DROP COLUMN `qqvip`;
ALTER TABLE `t_auction_new` ADD COLUMN `daygiftnum`  tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_auction_new` DROP COLUMN `qqvip`;

ALTER TABLE `t_storage` ADD COLUMN `addmagic`  varchar(32) NOT NULL DEFAULT '' AFTER `disdef`;
ALTER TABLE `t_storage` DROP COLUMN `rune`;

