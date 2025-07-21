ALTER TABLE `t_item` ADD COLUMN `sealstate` tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_player_info` modify COLUMN `exp` BIGINT	UNSIGNED NOT NULL DEFAULT 0;
ALTER TABLE `t_item` ADD COLUMN `sealcount` int(10) unsigned NOT NULL DEFAULT '0';
