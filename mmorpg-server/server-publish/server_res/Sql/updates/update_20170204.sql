ALTER TABLE `t_player_info` ADD COLUMN `daily_task_score`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `offlinemailtime`;
ALTER TABLE `t_player_info` ADD COLUMN `daily_task_reward_mask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `daily_task_score`;
