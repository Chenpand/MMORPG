ALTER TABLE `t_relation` MODIFY COLUMN `accid`  int(10) UNSIGNED NOT NULL AFTER `id`;
ALTER TABLE `t_relation` ADD COLUMN `seasonlv`  int(10) UNSIGNED NOT NULL AFTER `accid`;
ALTER TABLE `t_relation` MODIFY COLUMN `level`  smallint(5) UNSIGNED NOT NULL AFTER `seasonlv`;
ALTER TABLE `t_relation` MODIFY COLUMN `occu`  tinyint(3) UNSIGNED NOT NULL AFTER `level`;
ALTER TABLE `t_relation` DROP COLUMN `sex`;