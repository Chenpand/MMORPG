ALTER TABLE `t_player_info` ADD COLUMN `offretinuelist` varchar(128) NOT NULL DEFAULT ''

ALTER TABLE `t_retinue` ADD COLUMN `level` tinyint(3) UNSIGNED NOT NULL DEFAULT 1;
ALTER TABLE `t_retinue` ADD COLUMN `lucklevel` tinyint(3) UNSIGNED NOT NULL DEFAULT 1;
ALTER TABLE `t_retinue` ADD COLUMN `luckexp` int(10) UNSIGNED NOT NULL DEFAULT 0;