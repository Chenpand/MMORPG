ALTER TABLE `t_account` ADD COLUMN `viplvl`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `gmauthority`;
ALTER TABLE `t_account` ADD COLUMN `vipexp` int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `viplvl`;
