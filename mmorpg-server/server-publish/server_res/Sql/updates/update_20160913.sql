ALTER TABLE `t_player_info`
CHANGE COLUMN `ironstone` `bindpoint`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `bindgold`,
CHANGE COLUMN `silver` `gold`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `hp`,
CHANGE COLUMN `gold` `point`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `gold`;