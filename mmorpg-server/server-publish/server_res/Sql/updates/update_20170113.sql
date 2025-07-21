ALTER TABLE `t_guild` ADD COLUMN `enroll_terrid`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `table`;
ALTER TABLE `t_guild` ADD COLUMN `battle_score`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enroll_terrid`;
ALTER TABLE `t_guild` ADD COLUMN `occupy_terrid`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `battle_score`;
ALTER TABLE `t_guild` ADD COLUMN `inspire`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `occupy_terrid`;

ALTER TABLE `t_guild_member` ADD COLUMN `battle_number`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `avatar`;
ALTER TABLE `t_guild_member` ADD COLUMN `battle_score`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `battle_number`;
ALTER TABLE `t_guild_member` ADD COLUMN `battle_winstreak`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `battle_score`;
ALTER TABLE `t_guild_member` ADD COLUMN `battle_reward_mask`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `battle_winstreak`;
ALTER TABLE `t_guild_member` ADD COLUMN `battle_hp`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `battle_reward_mask`;
ALTER TABLE `t_guild_member` ADD COLUMN `battle_mp`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `battle_hp`;
