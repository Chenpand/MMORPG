ALTER TABLE `t_charge_order` MODIFY COLUMN `guid`  bigint(10) NOT NULL FIRST ;

ALTER TABLE `t_announcement` ADD COLUMN `gmid`  int(10) UNSIGNED NOT NULL AFTER `showzone`;
