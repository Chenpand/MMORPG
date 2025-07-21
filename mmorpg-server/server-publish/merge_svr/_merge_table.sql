delimiter $$
drop procedure if exists merge_table $$
create procedure merge_table(IN dest VARCHAR(64), IN src VARCHAR(64))
begin

	DECLARE cal1 VARCHAR(1024);

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account SELECT * FROM `",src,"`.t_account;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_active_task SELECT * FROM `",src,"`.t_active_task;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("DELETE FROM `",dest,"`.t_activity_op;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_activity_op_task SELECT * FROM `",src,"`.t_activity_op_task;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_charge_order SELECT * FROM `",src,"`.t_charge_order;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_charge_record SELECT * FROM `",src,"`.t_charge_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_counter SELECT * FROM `",src,"`.t_counter;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_dungeon SELECT * FROM `",src,"`.t_dungeon;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_g_dungeon_hard SELECT * FROM `",src,"`.t_g_dungeon_hard;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_guild SELECT * FROM `",src,"`.t_guild;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_guild_member SELECT * FROM `",src,"`.t_guild_member;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_item SELECT * FROM `",src,"`.t_item;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("DELETE FROM `",dest,"`.t_jar_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_jaritem_pool SELECT * FROM `",src,"`.t_jaritem_pool;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_mail SELECT * FROM `",src,"`.t_mail;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_mailitem SELECT * FROM `",src,"`.t_mailitem;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_mall_gift_pack SELECT * FROM `",src,"`.t_mall_gift_pack;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_offline_event SELECT * FROM `",src,"`.t_offline_event;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_pk_statistic SELECT * FROM `",src,"`.t_pk_statistic;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_player_info SELECT * FROM `",src,"`.t_player_info;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_player_info_name SELECT * FROM `",src,"`.t_player_info_name;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_punishment SELECT * FROM `",src,"`.t_punishment;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_red_packet SELECT * FROM `",src,"`.t_red_packet;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_red_packet_receiver SELECT * FROM `",src,"`.t_red_packet_receiver;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_relation SELECT * FROM `",src,"`.t_relation;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_retinue SELECT * FROM `",src,"`.t_retinue;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_shop SELECT * FROM `",src,"`.t_shop;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_shopitem SELECT * FROM `",src,"`.t_shopitem;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_sortlist SELECT * FROM `",src,"`.t_sortlist;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_sys_record SELECT * FROM `",src,"`.t_sys_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_task SELECT * FROM `",src,"`.t_task;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

end $$

delimiter ;