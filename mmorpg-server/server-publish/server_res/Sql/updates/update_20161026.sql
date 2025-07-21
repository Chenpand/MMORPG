ALTER TABLE `t_player_info` ADD COLUMN `warrior_soul`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `fatigue`;
ALTER TABLE `t_player_info` DROP COLUMN `death_coin`;