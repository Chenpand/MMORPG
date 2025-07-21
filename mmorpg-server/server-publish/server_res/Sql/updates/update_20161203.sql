ALTER TABLE `t_retinue` DROP COLUMN `luckexp`;
ALTER TABLE `t_retinue` DROP COLUMN `lucklevel`;
ALTER TABLE `t_retinue` ADD COLUMN `starlevel`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0;
ALTER TABLE `t_retinue` ADD COLUMN `weights`  varchar(1024) NOT NULL DEFAULT '';