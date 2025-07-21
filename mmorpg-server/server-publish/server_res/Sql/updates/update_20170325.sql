ALTER TABLE `t_account`
ADD COLUMN `storage_size`  int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `month_card_expire_time`;