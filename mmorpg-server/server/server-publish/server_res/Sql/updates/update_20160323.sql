ALTER TABLE `t_player_info` DROP COLUMN `quickbar`;
ALTER TABLE `t_player_info` ADD COLUMN `sp` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_player_info` ADD COLUMN `isawaken` binary(3) NOT NULL DEFAULT '0\0\0';
ALTER TABLE `t_player_info` ADD COLUMN `skillbar` varchar(512) NOT NULL DEFAULT '';
ALTER TABLE `t_player_info` ADD COLUMN `itembar` varchar(512) NOT NULL DEFAULT '';