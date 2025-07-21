ALTER TABLE `t_mall` ADD COLUMN `is_recommend`  tinyint(3) UNSIGNED NOT NULL AFTER `goods_subtype`;
ALTER TABLE `t_mall_gift_pack` MODIFY COLUMN `rest_num`  smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `activate_end_time`;
ALTER TABLE `t_mall_gift_pack` MODIFY COLUMN `activate_counter`  smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `activate_cond`;
ALTER TABLE `t_mall_gift_pack` ADD COLUMN `activate_sub_cond`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `activate_counter`;
ALTER TABLE `t_mall_gift_pack` ADD COLUMN `activate_type`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `activate_sub_cond`;