ALTER TABLE `t_player_info` ADD COLUMN `revive_coin`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `avatar`;

ALTER TABLE `t_player_info`
DROP COLUMN `isawaken`,
ADD COLUMN `awaken` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `sp`;