ALTER TABLE `t_guild` ADD COLUMN `popularoty`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `dismiss_time`;
ALTER TABLE `t_guild` ADD COLUMN `table`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `popularoty`;
ALTER TABLE `t_guild_member` ADD COLUMN `avatar`  varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `leavetime`;