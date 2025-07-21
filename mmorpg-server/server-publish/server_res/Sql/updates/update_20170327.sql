ALTER TABLE `t_charge_order` ADD COLUMN `mall_type`  tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '商城类型' AFTER `roleid`;

CREATE INDEX `duetime` ON `t_auction_new`(`duetime`) USING BTREE ;
CREATE INDEX `itemid` ON `t_auction_new`(`itemid`) USING BTREE ;
CREATE INDEX `item_sub_type` ON `t_auction_new`(`item_sub_type`) USING BTREE ;
CREATE INDEX `item_main_type` ON `t_auction_new`(`item_main_type`) USING BTREE ;
CREATE INDEX `max` ON `t_auction_new`(`item_main_type`, `item_sub_type`, `item_level`, `quality`, `strengthen`) USING BTREE ;
CREATE INDEX `orderid` ON `t_charge_order`(`orderid`) USING BTREE ;
CREATE INDEX `accid` ON `t_charge_record`(`accid`) USING BTREE ;
CREATE INDEX `roleid` ON `t_charge_record`(`roleid`) USING BTREE ;
CREATE INDEX `guildid` ON `t_guild_member`(`guildid`) USING BTREE ;
CREATE INDEX `accid` ON `t_punishment`(`accid`) USING BTREE ;
CREATE INDEX `owner_id` ON `t_red_packet`(`owner_id`) USING BTREE ;
CREATE INDEX `receiver_id` ON `t_red_packet_receiver`(`receiver_id`) USING BTREE ;