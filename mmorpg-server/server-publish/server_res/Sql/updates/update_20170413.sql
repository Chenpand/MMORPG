ALTER TABLE `t_player_info`
CHANGE COLUMN `jadestone` `totleonlinetime`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `honour`;
ALTER TABLE `t_player_info`
CHANGE COLUMN `spiritstone` `totlechargenum`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `totleonlinetime`;