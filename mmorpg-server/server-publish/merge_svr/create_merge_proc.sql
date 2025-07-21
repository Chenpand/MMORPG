delimiter $$
drop procedure if exists delete_overdue_player $$
create procedure delete_overdue_player()
begin

-- 声明变量
		declare _guid bigint UNSIGNED default 0;
			DECLARE _curCnt INT UNSIGNED DEFAULT 0;
			DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
	
			DECLARE _index INT UNSIGNED DEFAULT 0;
			
			declare cal1_palyer_info 					TEXT(220000);
			declare cal1_active_task 					TEXT(220000);
			declare cal1_activity_op_task 		TEXT(220000);
			declare cal1_activity_op_task_new 		TEXT(220000);
			declare cal1_counter 							TEXT(220000);
			declare cal1_dungeon 							TEXT(220000);
			declare cal1_guild_member		  		TEXT(220000);
			declare cal1_item 								TEXT(220000);
			declare cal1_jaritem_pool 				TEXT(220000);
			declare cal1_mail 								TEXT(220000);
			declare cal1_mailitem 						TEXT(220000);
			declare cal1_offline_event 				TEXT(220000);
			declare cal1_pk_statistic 				TEXT(220000);
			declare cal1_red_packet 				  TEXT(220000);
			declare cal1_red_packet_receiver  TEXT(220000);
			declare cal1_relation 					  TEXT(220000);
			declare cal1_retinue 						  TEXT(220000);
			declare cal1_shop 							  TEXT(220000);
			declare cal1_shopitem 					  TEXT(220000);
			declare cal1_sortlist 				    TEXT(220000);
			declare cal1_task 							  TEXT(220000);
			declare cal1_pet 									TEXT(220000);
			declare cal1_mall_gift_pack 			TEXT(220000);
			declare call_questionnaire              TEXT(220000);
			declare call_mastersect_relation        TEXT(220000);
			declare cal1_account_shop_role_buy_record TEXT(220000);
			declare cal1_activity_op_attribute 			TEXT(220000);
			declare cal1_mall_item 						TEXT(220000);
			declare cal1_new_title 						TEXT(220000);
			
			declare del_guids		  		   			TEXT(110000);
			
			-- 创建临时表
			CREATE TEMPORARY TABLE IF NOT EXISTS tmp_del_player (  
			`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
			`guid` BIGINT UNSIGNED NOT NULL     
			);

			-- 清空临时表
			TRUNCATE TABLE tmp_del_player;
			-- 查询要删除的玩家插入到临时表（不要用totleonlinetime）
			INSERT INTO tmp_del_player(guid)
			select guid from t_player_info where deletetime=0 and (savetime/1000 + 30 * 24 * 3600) < UNIX_TIMESTAMP() and level < 30 and totleonlinetime < 24 * 3600 and totlechargenum =0;

			SELECT MIN(`incid`) INTO _curCnt FROM `tmp_del_player`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_del_player`; 
			SELECT _curCnt;
			SELECT _maxCnt;
			SET del_guids = "(";
			IF (_curCnt IS NOT NULL) THEN
					-- 遍历临时表
					WHILE _curCnt <= _maxCnt DO 
							-- delete t_player_info where guid in (select guid from tmp_del_player)
							SELECT guid INTO _guid FROM `tmp_del_player` WHERE incid = _curCnt  LIMIT 1;

							SET _index = _curCnt % 5000;
							-- 执行删除操作
							IF  _index = 0 OR _curCnt = _maxCnt THEN 
									
									SET del_guids = CONCAT(del_guids, _guid, ")");
									-- SELECT del_guids;
									SET cal1_palyer_info = CONCAT("DELETE FROM `t_player_info` WHERE guid IN ", del_guids, ";");
									-- SELECT cal1_palyer_info;
									SET @sql1=cal1_palyer_info;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	

									SET cal1_active_task = CONCAT("DELETE FROM `t_active_task` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_active_task;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_activity_op_task = CONCAT("DELETE FROM `t_activity_op_task` WHERE guid IN ", del_guids, ";");
									SET @sql1=cal1_activity_op_task;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET cal1_activity_op_task_new = CONCAT("DELETE FROM `t_activity_op_task_new` WHERE owner_id IN ", del_guids, ";");
									SET @sql1=cal1_activity_op_task_new;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_counter = CONCAT("DELETE FROM `t_counter` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_counter;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_dungeon = CONCAT("DELETE FROM `t_dungeon` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_dungeon;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_guild_member = CONCAT("DELETE FROM `t_guild_member` WHERE guid IN ", del_guids, ";");
									SET @sql1=cal1_guild_member;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_item = CONCAT("DELETE FROM `t_item` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_item;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_jaritem_pool = CONCAT("DELETE FROM `t_jaritem_pool` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_jaritem_pool;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_mail = CONCAT("DELETE FROM `t_mail` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_mail;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_mailitem = CONCAT("DELETE FROM `t_mailitem` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_mailitem;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_offline_event = CONCAT("DELETE FROM `t_offline_event` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_offline_event;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_pk_statistic = CONCAT("DELETE FROM `t_pk_statistic` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_pk_statistic;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_red_packet = CONCAT("DELETE FROM `t_red_packet` WHERE owner_id IN ", del_guids, ";");
									SET @sql1=cal1_red_packet;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_red_packet_receiver = CONCAT("DELETE FROM `t_red_packet_receiver` WHERE receiver_id IN ", del_guids, ";");
									SET @sql1=cal1_red_packet_receiver;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_relation = CONCAT("DELETE FROM `t_relation` WHERE owner IN ", del_guids, "OR id IN ", del_guids, ";");
									SET @sql1=cal1_relation;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_retinue = CONCAT("DELETE FROM `t_retinue` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_retinue;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_shop = CONCAT("DELETE FROM `t_shop` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_shop;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_shopitem = CONCAT("DELETE FROM `t_shopitem` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_shopitem;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_sortlist = CONCAT("DELETE FROM `t_sortlist` WHERE ownerid IN ", del_guids, "OR id IN ", del_guids, ";");
									SET @sql1=cal1_sortlist;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_task = CONCAT("DELETE FROM `t_task` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_task;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_pet = CONCAT("DELETE FROM `t_pet` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_pet;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
					
									SET cal1_mall_gift_pack = CONCAT("DELETE FROM `t_mall_gift_pack` WHERE roleid IN ", del_guids, ";");
									SET @sql1=cal1_mall_gift_pack;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET call_questionnaire = CONCAT("DELETE FROM `t_questionnaire` WHERE owner IN ", del_guids, ";");
									SET @sql1=call_questionnaire;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET call_mastersect_relation = CONCAT("DELETE FROM `t_mastersect_relation` WHERE owner IN ", del_guids, "OR id IN ", del_guids, ";");
									SET @sql1=call_mastersect_relation;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET cal1_account_shop_role_buy_record = CONCAT("DELETE FROM `t_account_shop_role_buy_record` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_account_shop_role_buy_record;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET cal1_activity_op_attribute = CONCAT("DELETE FROM `t_activity_op_attribute` WHERE owner IN ", del_guids, ";");
									SET @sql1=cal1_activity_op_attribute;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET cal1_mall_item = CONCAT("DELETE FROM `t_mall_item` WHERE roleid IN ", del_guids, ";");
									SET @sql1=cal1_mall_item;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET cal1_new_title = CONCAT("DELETE FROM `t_new_title` WHERE roleid IN ", del_guids, ";");
									SET @sql1=cal1_new_title;
									PREPARE stmt1 FROM @sql1;
									EXECUTE stmt1;	
									
									SET del_guids =  "(";
							ELSE
									SET del_guids = CONCAT(del_guids, _guid, ",");
							END IF;
							DELETE FROM `tmp_del_player` WHERE incid = _curCnt  LIMIT 1;
							SET _curCnt = _curCnt + 1;
					END WHILE;
			END IF;
			-- SELECT 11;  
end $$

delimiter ;


delimiter $$
DROP PROCEDURE IF EXISTS merge_adventure_team_repeative_name $$

CREATE PROCEDURE merge_adventure_team_repeative_name(IN zid INT)
BEGIN

		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		DECLARE _name VARCHAR(64) DEFAULT "";
		DECLARE _zoneid INT UNSIGNED DEFAULT 0;
		
		
		-- 处理佣兵团名字重复问题
		
		-- 创建临时重复名表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_repe_advname (
		`team_name` VARCHAR(64) NOT NULL,
		PRIMARY KEY (`team_name`)
		);
		TRUNCATE TABLE tmp_repe_advname;
		
		-- 找到重复名
		INSERT INTO `tmp_repe_advname`(team_name) SELECT adventure_team_name FROM `t_account` WHERE adventure_team_name <> '' GROUP BY BINARY adventure_team_name HAVING COUNT(adventure_team_name)>1;
		
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_advname_info (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,
		`accid`	INT UNSIGNED NOT NULL,
		`team_name` VARCHAR(64) NOT NULL,
		`zone_id` INT UNSIGNED NOT NULL
		);
		TRUNCATE TABLE tmp_advname_info;

		-- 查询重复的name插入到临时表
		INSERT INTO `tmp_advname_info`(guid,accid,team_name,zone_id) SELECT `t_account`.guid,`t_account`.accid,`t_account`.adventure_team_name,`t_account`.zone_id FROM `t_account` 
		RIGHT JOIN `tmp_repe_advname` ON `t_account`.adventure_team_name = `tmp_repe_advname`.team_name WHERE `t_account`.adventure_team_name IS NOT NULL ORDER BY BINARY `t_account`.adventure_team_name;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_advname_info`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_advname_info`;  

		-- 遍历临时表
    	WHILE _curCnt <= _maxCnt DO 
    		SELECT guid,accid,team_name,zone_id INTO _guid,_accid,_name,_zoneid FROM `tmp_advname_info` WHERE incid = _curCnt;

    		IF _zoneid <> zid THEN
				-- 更新账号表
				UPDATE `t_account` SET adventure_team_name=CONCAT(_name,'_',_zoneid) WHERE guid=_guid;

				-- 插入要发改名卡
				INSERT INTO `t_merge_give_namecard`(accid,name,type) VALUES(_accid,CONCAT(_name,'_',_zoneid),3);

			END IF;

			SET _curCnt = _curCnt + 1;
		END WHILE;
END $$

delimiter ;


delimiter $$
DROP PROCEDURE IF EXISTS merge_adventure_team_data $$

CREATE PROCEDURE merge_adventure_team_data(IN zid INT)
BEGIN
		
		-- 处理冒险队(佣兵团)相关数据
		-- 1.保留等级和经验较高的
		-- 2.等级和经验相同时保留主服数据

		-- 创建临时唯一字段账号表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_guid_account (  
		`guid` BIGINT UNSIGNED NOT NULL,
		PRIMARY KEY (`guid`)
		);
		TRUNCATE TABLE tmp_guid_account;
		
		
		-- 创建临时账号表（计算佣兵团等级和经验相同的账号GUID）
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_accid_account (
		`accid` INT UNSIGNED NOT NULL,
		PRIMARY KEY (`accid`)
		);
		TRUNCATE TABLE tmp_accid_account;
		-- 重复账号下冒险队(佣兵团)等级和经验相同的,保留主服的数据
		INSERT INTO tmp_accid_account(accid) SELECT accid FROM `t_account` GROUP BY accid HAVING COUNT(*) > 1 AND COUNT(DISTINCT adventure_team_level) = 1 AND COUNT(DISTINCT adventure_team_exp) = 1;
		INSERT INTO tmp_guid_account(guid) SELECT `t_account`.guid FROM `t_account` RIGHT JOIN `tmp_accid_account` ON `t_account`.accid = `tmp_accid_account`.accid WHERE `t_account`.accid IS NOT NULL AND `t_account`.zone_id = zid;

		
		-- 创建临时账号表2(其他情况的账号ID)
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_accid_account2 (
		`accid` INT UNSIGNED NOT NULL,
		PRIMARY KEY (`accid`)
		);
		TRUNCATE TABLE tmp_accid_account2;
		-- 不同账号和重复账号下冒险队(佣兵团)等级或经验不同的,保留较高的数据
		INSERT INTO tmp_accid_account2(accid) SELECT accid FROM `t_account` GROUP BY accid HAVING COUNT(*) = 1 OR (COUNT(*) > 1 AND COUNT(DISTINCT adventure_team_level) > 1 OR COUNT(DISTINCT adventure_team_exp) > 1);
		
		
		-- 创建临时冒险队信息相关账号表（统计其他情况账号信息）
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_adventure_info_account (  
		`guid` BIGINT UNSIGNED NOT NULL,
		`accid` INT UNSIGNED NOT NULL,
		`adventure_team_level` SMALLINT UNSIGNED NOT NULL,
		`adventure_team_exp` BIGINT UNSIGNED NOT NULL,
		PRIMARY KEY (`guid`),
		INDEX `accid` (`accid`) USING BTREE 
		);
		TRUNCATE TABLE tmp_adventure_info_account;
		INSERT INTO tmp_adventure_info_account(guid,accid,adventure_team_level,adventure_team_exp) SELECT `t_account`.guid,`t_account`.accid,`t_account`.adventure_team_level,`t_account`.adventure_team_exp FROM `t_account` RIGHT JOIN `tmp_accid_account2` ON `t_account`.accid = `tmp_accid_account2`.accid WHERE `t_account`.accid IS NOT NULL; 
		
		-- 要保留的账号
		INSERT INTO tmp_guid_account(guid) SELECT guid FROM (
		SELECT guid,accid FROM `tmp_adventure_info_account` ORDER BY adventure_team_level DESC,adventure_team_exp DESC
		) AS tmp_t GROUP BY tmp_t.accid;
		
		-- 删除不保留账号的数据
		DELETE `t_account_counter` FROM `t_account_counter` LEFT JOIN `tmp_guid_account` ON `t_account_counter`.acc_guid = `tmp_guid_account`.guid WHERE `tmp_guid_account`.guid IS NULL AND counter_type IN (1,2,3,4,6);
		DELETE `t_account_shop_acc_buy_record` FROM `t_account_shop_acc_buy_record` LEFT JOIN `tmp_guid_account` ON `t_account_shop_acc_buy_record`.acc_guid = `tmp_guid_account`.guid WHERE `tmp_guid_account`.guid IS NULL;
		DELETE `t_expedition_map` FROM `t_expedition_map` LEFT JOIN `tmp_guid_account` ON `t_expedition_map`.acc_guid = `tmp_guid_account`.guid WHERE `tmp_guid_account`.guid IS NULL;
		DELETE `t_expedition_member` FROM `t_expedition_member` LEFT JOIN `tmp_guid_account` ON `t_expedition_member`.acc_guid = `tmp_guid_account`.guid WHERE `tmp_guid_account`.guid IS NULL;
		DELETE `t_account_task` FROM `t_account_task` LEFT JOIN `tmp_guid_account` ON `t_account_task`.acc_guid = `tmp_guid_account`.guid WHERE `tmp_guid_account`.guid IS NULL;
		
END $$

delimiter ;


delimiter $$
DROP PROCEDURE IF EXISTS merge_account $$

CREATE PROCEDURE merge_account(IN zid INT)
BEGIN

		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		DECLARE _point INT UNSIGNED DEFAULT 0;
		DECLARE _credit_point INT UNSIGNED DEFAULT 0;
		DECLARE _reg_time INT UNSIGNED DEFAULT 0;
		DECLARE _vipexp INT UNSIGNED DEFAULT 0;
		DECLARE _viplvl INT UNSIGNED DEFAULT 0;
		DECLARE _total_charge_num INT UNSIGNED DEFAULT 0;
		DECLARE _storage_size INT UNSIGNED DEFAULT 0;
		DECLARE _delete_role_time INT UNSIGNED DEFAULT 0;
		DECLARE _recove_role_time INT UNSIGNED DEFAULT 0;
		DECLARE _money_manage_status INT UNSIGNED DEFAULT 0;
		DECLARE _gnome_coin_num INT UNSIGNED DEFAULT 0;
		DECLARE _gnome_coin_refresh_time INT UNSIGNED DEFAULT 0;
		DECLARE _weapon_lease_tickets INT UNSIGNED DEFAULT 0;
		DECLARE _unlock_extensible_role_num INT UNSIGNED DEFAULT 0;
		DECLARE _adventure_team_name VARCHAR(64) DEFAULT "";
		DECLARE _adventure_team_level SMALLINT UNSIGNED DEFAULT 0;
		DECLARE _adventure_team_exp BIGINT UNSIGNED DEFAULT 0;
		DECLARE _adventure_team_same_data_cnt INT UNSIGNED DEFAULT 0;
		DECLARE _account_achieve_score INT UNSIGNED DEFAULT 0;
		DECLARE _offline_time INT UNSIGNED DEFAULT 0;
		DECLARE _mall_point INT UNSIGNED DEFAULT 0;
		DECLARE _adventure_coin INT UNSIGNED DEFAULT 0;
		
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_account (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,     
		`accid` INT UNSIGNED NOT NULL     
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_account;

		-- 查询重复的aid插入到临时表
		INSERT INTO tmp_account(guid, accid)
		SELECT guid,accid FROM  `t_account` WHERE accid IN
		(SELECT accid FROM `t_account` GROUP BY accid HAVING COUNT(accid)>1) ORDER BY accid;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_account`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_account`;  
  
		-- 遍历临时表处理重复的账号
    	WHILE _curCnt <= _maxCnt DO  
				SELECT guid,accid INTO _guid,_accid FROM `tmp_account` WHERE incid = _curCnt;
				
				IF _lastAccId = _accid THEN
					DELETE FROM `t_account` WHERE guid=_guid;
				ELSE
					SELECT SUM(point) INTO _point FROM `t_account` WHERE accid = _accid;
					SELECT SUM(credit_point) INTO _credit_point FROM `t_account` WHERE accid = _accid;
					SELECT MIN(reg_time) INTO _reg_time FROM `t_account` WHERE accid = _accid;
					SELECT MAX(vipexp) INTO _vipexp FROM `t_account` WHERE accid = _accid;
					SELECT MAX(viplvl) INTO _viplvl FROM `t_account` WHERE accid = _accid;
					SELECT SUM(total_charge_num) INTO _total_charge_num FROM `t_account` WHERE accid = _accid;
					SELECT MAX(storage_size) INTO _storage_size FROM `t_account` WHERE accid = _accid;
					SELECT MAX(role_delete_time) INTO _delete_role_time FROM `t_account` WHERE accid = _accid;
					SELECT MAX(role_recover_time) INTO _recove_role_time FROM `t_account` WHERE accid = _accid;
					SELECT MAX(money_manage_status) INTO _money_manage_status FROM `t_account` WHERE accid = _accid;
					SELECT SUM(gnome_coin_num) INTO _gnome_coin_num FROM `t_account` WHERE accid = _accid;
					SELECT MAX(gnome_coin_refresh_time) INTO _gnome_coin_refresh_time FROM `t_account` WHERE accid = _accid;
					SELECT SUM(weapon_lease_tickets) INTO _weapon_lease_tickets FROM `t_account` WHERE accid = _accid;
					SELECT SUM(unlock_extensible_role_num) INTO _unlock_extensible_role_num FROM `t_account` WHERE accid = _accid;
					SELECT adventure_team_level INTO _adventure_team_level FROM `t_account` WHERE accid = _accid ORDER BY adventure_team_level DESC,adventure_team_exp DESC LIMIT 1;
					SELECT adventure_team_exp INTO _adventure_team_exp FROM `t_account` WHERE accid = _accid ORDER BY adventure_team_level DESC,adventure_team_exp DESC LIMIT 1;
					SELECT MAX(offline_time) INTO _offline_time FROM `t_account` WHERE accid = _accid;
					SELECT SUM(mall_point) INTO _mall_point FROM `t_account` WHERE accid = _accid;
					SELECT SUM(adventure_coin) INTO _adventure_coin FROM `t_account` WHERE accid = _accid;


					-- 佣兵团等级和经验相同
					SELECT COUNT(*) INTO _adventure_team_same_data_cnt FROM `t_account` WHERE accid = _accid HAVING COUNT(*) > 1 AND COUNT(DISTINCT adventure_team_level) = 1 AND COUNT(DISTINCT adventure_team_exp) = 1;
					
					IF _adventure_team_same_data_cnt = 0 THEN
						-- 不同时保留偏高的
						SELECT adventure_team_name, account_achievement_score  INTO _adventure_team_name, _account_achieve_score FROM `t_account` WHERE accid = _accid ORDER BY adventure_team_level DESC,adventure_team_exp DESC LIMIT 1;
					ELSE
						-- 相同时保留主服数据
						SELECT adventure_team_name, account_achievement_score INTO _adventure_team_name, _account_achieve_score FROM `t_account` WHERE accid = _accid AND zone_id = zid;
					END IF;
					-- 清空计数！！！
					SET _adventure_team_same_data_cnt = 0;
					
					IF _weapon_lease_tickets > 200 THEN
						SET _weapon_lease_tickets = 200;
					END IF;
					
					UPDATE `t_account` SET point = _point,
														credit_point = _credit_point,
														reg_time = _reg_time,
														vipexp = _vipexp,
														viplvl = _viplvl,
														total_charge_num = _total_charge_num,
														storage_size = _storage_size,
														role_delete_time = _delete_role_time,
														role_recover_time = _recove_role_time,
														money_manage_status = _money_manage_status,
														gnome_coin_num = _gnome_coin_num,
														gnome_coin_refresh_time = _gnome_coin_refresh_time,
														weapon_lease_tickets = _weapon_lease_tickets,
														unlock_extensible_role_num = _unlock_extensible_role_num,
														adventure_team_name = _adventure_team_name,
														adventure_team_level = _adventure_team_level,
														adventure_team_exp = _adventure_team_exp,
														account_achievement_score = _account_achieve_score,
														offline_time = _offline_time,
														mall_point = _mall_point,
														adventure_coin = _adventure_coin
					WHERE guid = _guid;
					
					SET _lastAccId = _accid;
				
					-- 设置账号相关的数据
					UPDATE `t_account_counter` SET acc_guid = _guid WHERE owner = _accid;
					UPDATE `t_account_shop_acc_buy_record` SET acc_guid = _guid WHERE owner = _accid;
					UPDATE `t_expedition_map` SET acc_guid = _guid WHERE accid = _accid;
					UPDATE `t_expedition_member` SET acc_guid = _guid WHERE accid = _accid;
					
        		END IF;

        		SET _curCnt = _curCnt + 1;  
    	END WHILE;
		
		-- 清空冒险队相关数据
		UPDATE `t_account` SET adventure_team_grade_id = 0;
		UPDATE `t_account` SET all_role_value_score = 0;
		UPDATE `t_account` SET unlocked_new_occus = "";
		UPDATE `t_account` SET query_new_occus_time = 0;
	
		-- 解封只有单服有封号的账号
		DELETE FROM `t_punishment` WHERE accid IN (SELECT * FROM (SELECT accid FROM `t_punishment` GROUP BY accid HAVING COUNT(accid)=1) AS p);
		-- 清空角色偏好设置
		UPDATE `t_player_info` SET add_preferences_time = 0;
		UPDATE `t_player_info` SET del_preferences_time = 0;
END $$

delimiter ;



delimiter $$
DROP PROCEDURE IF EXISTS account_adventure $$

CREATE PROCEDURE account_adventure(IN zid INT)
BEGIN
		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		-- DECLARE _main_guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		DECLARE _adventure_pass_level_cnt INT UNSIGNED DEFAULT 0;
		DECLARE _adventure_pass_season_cnt INT UNSIGNED DEFAULT 0;
		DECLARE _max_level_guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _max_type_guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _min_type_guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _min_type INT UNSIGNED DEFAULT 0;	
		DECLARE _extraexp_flag INT UNSIGNED DEFAULT 0;	
		DECLARE _min_highreward VARCHAR(1024) DEFAULT "";
		DECLARE _max_highreward VARCHAR(1024) DEFAULT "";
		DECLARE _max_season_guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _zone_id INT UNSIGNED DEFAULT 0;
		DECLARE _account_guid BIGINT UNSIGNED DEFAULT 0;

		-- DECLARE _min_char CHAR DEFAULT NULL;
		-- DECLARE _max_char CHAR DEFAULT NULL;
		DECLARE _char VARCHAR(10) DEFAULT "";
		DECLARE _reward_len INT UNSIGNED DEFAULT 0;
		DECLARE _curr_pos INT UNSIGNED DEFAULT 0;

		-- 根据最大level取
		DECLARE _level INT UNSIGNED DEFAULT 0;
		DECLARE _exp INT UNSIGNED DEFAULT 0;
		DECLARE _activity INT UNSIGNED DEFAULT 0;
		DECLARE _unlock INT UNSIGNED DEFAULT 0;
		DECLARE _seasonid INT UNSIGNED DEFAULT 0;
		DECLARE _normalreward VARCHAR(1024) DEFAULT "";
		-- 根据最大passtype取
		DECLARE _passtype INT UNSIGNED DEFAULT 0;
		-- 取逻辑上的并集
		DECLARE _extraexp INT UNSIGNED DEFAULT 0;
		-- 按字符累加
		DECLARE _highreward VARCHAR(1024) DEFAULT "";

		
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_account_adventure (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,     
		`accid` INT UNSIGNED NOT NULL     
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_account_adventure;

		-- 查询重复的accid插入到临时表
		INSERT INTO tmp_account_adventure(guid, accid)
		SELECT guid,accid FROM  `t_account_adventure` WHERE accid IN
		(SELECT accid FROM `t_account_adventure` GROUP BY accid HAVING COUNT(accid)>1) ORDER BY accid;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_account_adventure`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_account_adventure`;  
  
		-- 遍历临时表处理重复的账号
    	WHILE _curCnt <= _maxCnt DO  
			SELECT guid,accid INTO _guid,_accid FROM `tmp_account_adventure` WHERE incid = _curCnt;
				
			IF _lastAccId = _accid THEN
				DELETE FROM `t_account_adventure` WHERE guid=_guid;
			ELSE
				-- 赛季id
				SELECT guid,seasonid INTO _max_season_guid,_seasonid FROM `t_account_adventure` WHERE accid = _accid ORDER BY `seasonid` DESC LIMIT 1;
				SELECT COUNT(*) INTO _adventure_pass_season_cnt FROM `t_account_adventure` WHERE accid = _accid HAVING COUNT(*) > 1 AND COUNT(DISTINCT `seasonid`) = 1;
				IF _adventure_pass_season_cnt = 0 THEN
					-- 不同时保留偏高的
					SELECT `level`, exp, passtype, normalreward, highreward, activity, `unlock`, seasonid, extraexp INTO _level, _exp, _passtype, _normalreward, _max_highreward, _activity, _unlock, _seasonid, _extraexp FROM `t_account_adventure` WHERE guid = _max_season_guid;
				ELSE
					-- 通行证等级
					SELECT guid,`level` INTO _max_level_guid,_level FROM `t_account_adventure` WHERE accid = _accid ORDER BY `level` DESC LIMIT 1;
					SELECT COUNT(*) INTO _adventure_pass_level_cnt FROM `t_account_adventure` WHERE accid = _accid HAVING COUNT(*) > 1 AND COUNT(DISTINCT `level`) = 1;
					IF _adventure_pass_level_cnt = 0 THEN
						-- 不同时保留偏高的
						SELECT exp, normalreward, activity, `unlock`, seasonid  INTO _exp, _normalreward, _activity, _unlock, _seasonid FROM `t_account_adventure` WHERE guid = _max_level_guid;
						-- 下面是处理通行证帐号商店的
						SELECT zone_id INTO _zone_id FROM t_account_adventure WHERE guid = _max_level_guid;
						SELECT guid INTO _account_guid FROM t_account WHERE accid = _accid AND zone_id = _zone_id;
						DELETE FROM t_account_shop_acc_buy_record WHERE owner = _accid AND acc_guid != _account_guid AND shop_id = 70;
					ELSE
						SELECT exp, normalreward, activity, `unlock`, seasonid  INTO _exp, _normalreward, _activity, _unlock, _seasonid FROM `t_account_adventure` WHERE accid = _accid AND zone_id = zid;
						-- 等级相同经验保留最大的
						SELECT exp INTO _exp FROM `t_account_adventure` WHERE accid = _accid ORDER BY `exp` DESC LIMIT 1;
						-- 下面是处理通行证帐号商店的
						SELECT guid INTO _account_guid FROM t_account WHERE accid = _accid AND zone_id = zid;
						DELETE FROM t_account_shop_acc_buy_record WHERE owner = _accid AND acc_guid != _account_guid AND shop_id = 70;
					END IF;
					-- 清空计数！！！
					SET _adventure_pass_level_cnt = 0;
					SET _max_level_guid = 0;
					SET _zone_id = 0;
					SET _account_guid = 0;

					-- 活跃度总是保留最大的
					SELECT activity INTO _activity FROM `t_account_adventure` WHERE accid = _accid ORDER BY `activity` DESC LIMIT 1;


					SELECT guid,passtype,highreward INTO _max_type_guid,_passtype,_max_highreward FROM `t_account_adventure` WHERE accid = _accid ORDER BY passtype DESC LIMIT 1;
					SELECT guid,passtype,highreward INTO _min_type_guid,_min_type,_min_highreward FROM `t_account_adventure` WHERE accid = _accid AND guid <> _max_type_guid ORDER BY passtype ASC LIMIT 1;
					IF _passtype <> 0 THEN
						IF _min_type = 0 THEN
							-- 低级通信证为普通通行证时以最大通行证类型高级奖励为新奖励
							SELECT highreward INTO _max_highreward FROM `t_account_adventure` WHERE guid = _max_type_guid;
						ELSE
							-- 低级通信证为高级通行证时需累加高级奖励
							SET _reward_len = LENGTH(_max_highreward);
							SET _curr_pos = 1;
							-- SELECT _reward_len;
							WHILE _curr_pos <= _reward_len DO 
								SET _char = substring(_max_highreward,_curr_pos,1) + substring(_min_highreward,_curr_pos,1);
								-- SELECT _char;
								SET _max_highreward = INSERT(_max_highreward,_curr_pos,1,_char);
								-- SELECT _max_highreward;
								SET _curr_pos = _curr_pos + 1;
							END WHILE;
						END IF;
					END IF;
					-- 清空计数！！！
					SET _max_type_guid = 0;
					SET _min_type_guid = 0;					
					SET _min_type = 0;	

					-- 经验包的处理
					SELECT COUNT(*) INTO _extraexp_flag FROM `t_account_adventure` WHERE accid = _accid AND extraexp = 1;
					IF _extraexp_flag > 0 THEN
						-- 只要有一个服可领取就设可领取
						SET _extraexp = 1;
					ELSE
						-- 所有服都不能领取切存在已领完的就设已领完，否则就是不可领
						SELECT COUNT(*) INTO _extraexp_flag FROM `t_account_adventure` WHERE accid = _accid AND extraexp = 2;
						IF _extraexp_flag > 0 THEN
							SET _extraexp = 2;
						ELSE
							SET _extraexp = 0;
						END IF;						
					END IF;			
					SET _extraexp_flag = 0;		
				END IF;

				SET _max_season_guid = 0;
				SET _adventure_pass_season_cnt = 0;				

				UPDATE `t_account_adventure` SET 	`level` = _level,
													exp = _exp,
													passtype = _passtype,
													normalreward = _normalreward,
													highreward = _max_highreward,
													activity = _activity,
													`unlock` = _unlock,
													seasonid = _seasonid,
													extraexp = _extraexp
				WHERE guid = _guid;
				
				SET _lastAccId = _accid;
				
			END IF;
        SET _curCnt = _curCnt + 1;  
    	END WHILE;
		-- ALTER TABLE `t_account_adventure` DROP COLUMN `zone_id`;
END $$

delimiter ;


delimiter $$
DROP PROCEDURE IF EXISTS merger_hire $$

CREATE PROCEDURE merger_hire(IN zid INT)
BEGIN
		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		
		DECLARE _identify_cnt INT UNSIGNED DEFAULT 0;

		DECLARE _code VARCHAR(1024) DEFAULT "";
		DECLARE _bindaccid INT UNSIGNED DEFAULT 0;
		DECLARE _identify INT UNSIGNED DEFAULT 0;
		DECLARE _tasks VARCHAR(1024) DEFAULT "";
		DECLARE _bindtime INT UNSIGNED DEFAULT 0;

		DECLARE _sum_coin INT UNSIGNED DEFAULT 0;
		DECLARE _count_coin_record INT UNSIGNED DEFAULT 0;

		DECLARE _zone_id INT UNSIGNED DEFAULT 0;
		DECLARE _account_guid_1 BIGINT UNSIGNED DEFAULT 0;
		DECLARE _count_shop_record_1 INT UNSIGNED DEFAULT 0;
		DECLARE _account_guid_2 BIGINT UNSIGNED DEFAULT 0;
		DECLARE _count_shop_record_2 INT UNSIGNED DEFAULT 0;
		
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_t_hire (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,     
		`accid` INT UNSIGNED NOT NULL     
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_t_hire;

		-- 查询重复的accid插入到临时表
		INSERT INTO tmp_t_hire(guid, accid)
		SELECT guid,accid FROM  `t_hire` WHERE accid IN
		(SELECT accid FROM `t_hire` GROUP BY accid HAVING COUNT(accid)>1) ORDER BY accid;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_t_hire`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_t_hire`;  
  
		-- 遍历临时表处理重复的账号
    	WHILE _curCnt <= _maxCnt DO  
			SELECT guid,accid INTO _guid,_accid FROM `tmp_t_hire` WHERE incid = _curCnt;
				
			IF _lastAccId = _accid THEN
				DELETE FROM `t_hire` WHERE guid=_guid;
			ELSE
				SELECT guid INTO _account_guid_1 FROM t_account WHERE accid = _accid AND zone_id = zid;		
				SELECT count(*) INTO _count_shop_record_1 FROM t_account_shop_acc_buy_record WHERE acc_guid = _account_guid_1;
				SELECT guid INTO _account_guid_2 FROM t_account WHERE accid = _accid AND zone_id != zid;	
				SELECT count(*) INTO _count_shop_record_2 FROM t_account_shop_acc_buy_record WHERE acc_guid = _account_guid_2;
				IF _count_shop_record_1 > _count_shop_record_2 THEN
					DELETE FROM t_account_shop_acc_buy_record WHERE owner = _accid AND acc_guid = _account_guid_2 AND shop_id = 34;	
				ELSE
					DELETE FROM t_account_shop_acc_buy_record WHERE owner = _accid AND acc_guid = _account_guid_1 AND shop_id = 34;
				END IF;		
			END IF;

			SELECT SUM(counter_num) INTO _sum_coin FROM `t_account_counter` WHERE owner = _accid AND counter_type = 10;
			UPDATE `t_account_counter` SET 	counter_num = _sum_coin WHERE owner = _accid AND counter_type = 10;	

			SELECT COUNT(*) INTO _count_coin_record FROM `t_account_counter` WHERE owner = _accid AND counter_type = 10;
			IF _count_coin_record >= 2 THEN
				DELETE FROM t_account_counter WHERE owner = _accid AND counter_type = 10 LIMIT 1;	
			END IF;					
			
			SET _count_coin_record = 0;
			SET _sum_coin = 0;
			SET _account_guid_1 = 0;
			SET _account_guid_2 = 0;
			SET _count_shop_record_1 = 0;
			SET _count_shop_record_2 = 0;
			SET _lastAccId = _accid;
				
       		SET _curCnt = _curCnt + 1;  
    	END WHILE;
END $$

delimiter ;



delimiter $$
DROP PROCEDURE IF EXISTS merger_mall $$

CREATE PROCEDURE merger_mall(IN zid INT)
BEGIN
		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		

		DECLARE _buyed_num INT UNSIGNED DEFAULT 0;

		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_t_mall_acc_buy_record (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,     
		`accid` INT UNSIGNED NOT NULL     
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_t_mall_acc_buy_record;

		-- 查询重复的accid插入到临时表
		INSERT INTO tmp_t_mall_acc_buy_record(guid, accid)
		SELECT guid,owner FROM  `t_mall_acc_buy_record` WHERE owner IN
		(SELECT owner FROM `t_mall_acc_buy_record` GROUP BY owner HAVING COUNT(owner)>1) ORDER BY owner;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_t_mall_acc_buy_record`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_t_mall_acc_buy_record`;  
  
		-- 遍历临时表处理重复的账号
    	WHILE _curCnt <= _maxCnt DO  
			SELECT guid,accid INTO _guid,_accid FROM `tmp_t_mall_acc_buy_record` WHERE incid = _curCnt;
				
			IF _lastAccId = _accid THEN
				DELETE FROM `t_mall_acc_buy_record` WHERE guid=_guid;
			END IF;

			SELECT SUM(buyed_num) INTO _buyed_num FROM `t_mall_acc_buy_record` WHERE owner = _accid;
			UPDATE `t_mall_acc_buy_record` SET 	buyed_num = _buyed_num WHERE owner = _accid;	
	
			
			SET _buyed_num = 0;
			SET _lastAccId = _accid;
				
       		SET _curCnt = _curCnt + 1;  
    	END WHILE;
END $$

delimiter ;




delimiter $$
DROP PROCEDURE IF EXISTS merger_monopoly $$

CREATE PROCEDURE merger_monopoly(IN zid INT)
BEGIN
		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;

		DECLARE _max_version_guid BIGINT UNSIGNED DEFAULT 0;

		DECLARE _max_roll_guid BIGINT UNSIGNED DEFAULT 0;

		DECLARE _roll_times_cnt INT UNSIGNED DEFAULT 0;
		DECLARE _version_cnt INT UNSIGNED DEFAULT 0;

		DECLARE _zone_id INT UNSIGNED DEFAULT 0;

		DECLARE _turn INT UNSIGNED DEFAULT 0;
		DECLARE _step INT UNSIGNED DEFAULT 0;
		DECLARE _remain_roll INT UNSIGNED DEFAULT 0;
		DECLARE _roll_times INT UNSIGNED DEFAULT 0;
		DECLARE _coin INT UNSIGNED DEFAULT 0;
		DECLARE _buff INT UNSIGNED DEFAULT 0;
		DECLARE _cards VARCHAR(512) DEFAULT "";
		DECLARE _exchanged VARCHAR(512) DEFAULT "";
		DECLARE _vsersion INT UNSIGNED DEFAULT 0;

		
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_t_account_monopoly (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,     
		`accid` INT UNSIGNED NOT NULL     
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_t_account_monopoly;

		-- 查询重复的accid插入到临时表
		INSERT INTO tmp_t_account_monopoly(guid, accid)
		SELECT guid,accid FROM  `t_account_monopoly` WHERE accid IN
		(SELECT accid FROM `t_account_monopoly` GROUP BY accid HAVING COUNT(accid)>1) ORDER BY accid;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_t_account_monopoly`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_t_account_monopoly`;  
  
		-- 遍历临时表处理重复的账号
    	WHILE _curCnt <= _maxCnt DO  
			SELECT guid,accid INTO _guid,_accid FROM `tmp_t_account_monopoly` WHERE incid = _curCnt;
				
			IF _lastAccId = _accid THEN
				DELETE FROM `t_account_monopoly` WHERE guid=_guid;
			ELSE
				SELECT guid INTO _max_version_guid FROM `t_account_monopoly` WHERE accid = _accid ORDER BY `vsersion` DESC LIMIT 1;
				SELECT COUNT(*) INTO _version_cnt FROM `t_account_monopoly` WHERE accid = _accid HAVING COUNT(*) > 1 AND COUNT(DISTINCT `vsersion`) = 1;
				IF _version_cnt = 0 THEN	
					-- 不同保留偏高的
					SELECT vsersion, turn, step, remain_roll, roll_times, coin, buff, cards, exchanged INTO _vsersion, _turn, _step, _remain_roll, _roll_times, _coin, _buff, _cards, _exchanged FROM `t_account_monopoly` WHERE accid = _accid AND guid = _max_version_guid;
				ELSE
					SELECT guid INTO _max_roll_guid FROM `t_account_monopoly` WHERE accid = _accid ORDER BY `roll_times` DESC LIMIT 1;
					SELECT COUNT(*) INTO _roll_times_cnt FROM `t_account_monopoly` WHERE accid = _accid HAVING COUNT(*) > 1 AND COUNT(DISTINCT `roll_times`) = 1;
					IF _roll_times_cnt = 0 THEN	
						-- 不同保留偏高的
						SELECT vsersion, turn, step, remain_roll, roll_times, buff, cards, exchanged INTO _vsersion, _turn, _step, _remain_roll, _roll_times, _buff, _cards, _exchanged FROM `t_account_monopoly` WHERE accid = _accid AND guid = _max_roll_guid;
						SELECT SUM(coin) INTO _coin FROM `t_account_monopoly` WHERE accid = _accid;
					ELSE
						SELECT vsersion, turn, step, remain_roll, roll_times, buff, cards, exchanged INTO _vsersion, _turn, _step, _remain_roll, _roll_times, _buff, _cards, _exchanged FROM `t_account_monopoly` WHERE accid = _accid AND zone_id = zid;
						SELECT SUM(coin) INTO _coin FROM `t_account_monopoly` WHERE accid = _accid;
					END IF;
				END IF;
			END IF;
			
			UPDATE `t_account_monopoly` SET 	vsersion = _vsersion,
												turn = _turn,
												step = _step,
												remain_roll = _remain_roll,
												roll_times = _roll_times,
												coin = _coin,
												buff = _buff,
												cards = _cards,
												exchanged = _exchanged
			WHERE guid = _guid;


			SET _max_version_guid = 0;
			SET _max_roll_guid = 0;
			SET _roll_times_cnt  = 0;
			SET _version_cnt  = 0;
			SET _turn = 0;
			SET _step  = 0;
			SET _remain_roll  = 0;
			SET _roll_times  = 0;
			SET _coin = 0;
			SET _buff  = 0;
			SET _cards  = "";
			SET _exchanged  = "";


			SET _lastAccId = _accid;
				
			SET _curCnt = _curCnt + 1;  
    	END WHILE;
END $$

delimiter ;


delimiter $$
DROP PROCEDURE IF EXISTS merge_auction $$

CREATE PROCEDURE merge_auction()
BEGIN
		-- 下架所有得拍卖行物品
		UPDATE `t_auction_new` SET duetime=0;
	
		-- 重置开服时间
		UPDATE `t_gameparam` SET value=0 WHERE name='gamestart_time';

		-- 重置罐子折扣
		update `t_counter` set value=10 where name='jar_buy_dis_remain_501';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_601';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_602';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_603';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_604';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_605';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_606';
		update `t_counter` set value=50 where name='jar_buy_dis_remain_607';

		-- 清空首充奖励
		TRUNCATE TABLE `t_charge_record`;
END $$

delimiter ;

delimiter $$
DROP PROCEDURE IF EXISTS merge_black_market $$
CREATE PROCEDURE merge_black_market()
BEGIN
	-- 下架所有未处理的竞价项
	UPDATE `t_blackmarket_auction` SET off_sale=1 WHERE is_settled=0;	
	UPDATE `t_blackmarket_transaction` SET off_sale=1 WHERE `state`=0;	
END $$

delimiter ;

delimiter $$
DROP PROCEDURE IF EXISTS clear_adventure_team_sortlist $$
CREATE PROCEDURE clear_adventure_team_sortlist()
BEGIN
	-- 清空冒险队排行榜
	DELETE FROM `t_sortlist` WHERE sorttype=17;
	
END $$

delimiter ;


delimiter $$
drop procedure if exists merge_dungeon_hard $$
create procedure merge_dungeon_hard()
begin
		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _lastDungeonId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		declare _dungeonId INT UNSIGNED DEFAULT 0;
		declare _guid bigint UNSIGNED default 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		DECLARE _unlockedHardType INT UNSIGNED DEFAULT 0;
	
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_dungeon (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,
		`account` INT UNSIGNED NOT NULL,
		`dungeon_id` INT UNSIGNED NOT NULL       
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_dungeon;

		-- 查询重复的插入到临时表
		INSERT INTO tmp_dungeon(guid,account,dungeon_id)
		SELECT guid,account,dungeon_id FROM `t_g_dungeon_hard` WHERE account in 
				(SELECT account FROM `t_g_dungeon_hard` group by account,dungeon_id HAVING COUNT(*)>1) and dungeon_id in
				(SELECT dungeon_id FROM `t_g_dungeon_hard` group by account,dungeon_id HAVING COUNT(*)>1) order by account,dungeon_id;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_dungeon`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_dungeon`;  

		-- 遍历临时表
    	WHILE _curCnt <= _maxCnt DO 
    		SELECT guid,account,dungeon_id INTO _guid,_accid,_dungeonId FROM `tmp_dungeon` WHERE incid = _curCnt;

    		IF _lastAccId = _accid and _lastDungeonId = _dungeonId THEN
				DELETE FROM `t_g_dungeon_hard` WHERE guid=_guid;
			ELSE
				SELECT MAX(unlocked_hard_type) INTO _unlockedHardType FROM `t_g_dungeon_hard` WHERE account = _accid and dungeon_id = _dungeonId;
				UPDATE `t_g_dungeon_hard` SET unlocked_hard_type = _unlockedHardType WHERE guid = _guid;
				SET _lastAccId = _accid;
				SET _lastDungeonId = _dungeonId;
			END IF;

			SET _curCnt = _curCnt + 1;
		END WHILE;
	
end $$

delimiter ;


delimiter $$
drop procedure if exists merge_guild $$
create procedure merge_guild(IN zid INT)
begin
		-- 声明变量
		declare _name varchar(32);
		declare _lastName varchar(32);
		declare _guid bigint UNSIGNED default 0;
		declare i int default 1;
		DECLARE _curCnt INT UNSIGNED DEFAULT 1;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		declare _playerName varchar(30);
		declare _playerGuid bigint UNSIGNED default 0;
		declare _zoneid int UNSIGNED default 0;

		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_guild (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,
		`name` varchar(32) NOT NULL        
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_guild;

		-- 查询重复的name插入到临时表
		INSERT INTO tmp_guild(guid,name)
		SELECT guid,name FROM `t_guild` WHERE name in 
		(SELECT name FROM `t_guild` group by BINARY name HAVING COUNT(name)>1) ORDER BY BINARY name;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_guild`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_guild`;  

		-- 遍历临时表
    	WHILE _curCnt <= _maxCnt DO 
    		SELECT guid,name INTO _guid,_name FROM `tmp_guild` WHERE incid = _curCnt;

    		SET _zoneid = _guid >> 54;

    		IF _lastName <> _name THEN 
				SET i = 1;
			END IF;

			select _lastName,_name,i,_zoneid;

    		IF _zoneid <> zid THEN
				-- 更新公会名字
				UPDATE `t_guild` SET name=CONCAT(_name,'_',_zoneid) WHERE guid=_guid;

				SELECT guid,name INTO _playerGuid,_playerName FROM `t_guild_member` WHERE guildid=_guid AND post=13;

				 -- 插入要发改名卡
				INSERT INTO `t_merge_give_namecard`(guid,name,type) VALUES(_playerGuid,_playerName,2);

				-- 更新公会排行榜名字
				UPDATE `t_sortlist` SET name=CONCAT(_name,'_',_zoneid) WHERE id=_guid;

				SET i = i + 1;
			END IF;

			SET _lastName = _name;
			SET _curCnt = _curCnt + 1;
		END WHILE;
	
		-- 重置工会战
		UPDATE `t_guild` SET enroll_terrid=0,battle_score=0,occupy_terrid=0,inspire=0;

		-- 删除公会雕像
		DELETE FROM `t_figure_statue` WHERE statuetype=1 OR statuetype=2 OR statuetype=3;
end $$

delimiter ;


delimiter $$
drop procedure if exists merge_player $$
create procedure merge_player(IN zid INT)
begin
		-- 声明变量
		declare _name varchar(30);
		DECLARE _zoneid INT UNSIGNED DEFAULT 0;
		declare _lastName varchar(30);
		declare _guid bigint UNSIGNED default 0;
		declare i int default 1;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		declare _sql varchar(1024);
	
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_player (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL,
		`name` varchar(30) NOT NULL,
		`zoneid`	INT UNSIGNED NOT NULL        
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_player;

		-- 查询重复的name插入到临时表
		INSERT INTO `tmp_player`(guid,name,zoneid)
		SELECT guid,name,zoneid FROM `t_player_info` WHERE name in 
		(SELECT name FROM `t_player_info` group by BINARY name HAVING COUNT(name)>1) and zoneid <> zid ORDER BY BINARY name;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_player`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_player`;  

		-- 遍历临时表
    	WHILE _curCnt <= _maxCnt DO 
    		SELECT guid,name,zoneid INTO _guid,_name,_zoneid FROM `tmp_player` WHERE incid = _curCnt;

    		IF _lastName <> _name THEN 
				SET i = 1;
			END IF;

    		IF _zoneid <> zid THEN
				-- 更新玩家表
				UPDATE `t_player_info` SET name=CONCAT(_name,'_',_zoneid) WHERE guid=_guid;
				-- 更新公会成员表
				UPDATE `t_guild_member` SET name=CONCAT(_name,'_',_zoneid) WHERE guid=_guid;
				-- 更新关系表
				UPDATE `t_relation` SET name=CONCAT(_name,'_',_zoneid) WHERE id=_guid;
				-- 更新排行榜
				UPDATE `t_sortlist` SET name=CONCAT(_name,'_',_zoneid) WHERE id=_guid;
				UPDATE `t_sortlist` SET ownername=CONCAT(_name,'_',_zoneid) WHERE ownerid=_guid;

				-- 插入要发改名卡
				INSERT INTO `t_merge_give_namecard`(guid,name,type) VALUES(_guid,CONCAT(_name,'_',_zoneid),1);

			SET i = i + 1;
			END IF;

			

			SET _lastName = _name;
			SET _curCnt = _curCnt + 1;
		END WHILE;

		-- 重置时装商城购买次数
		UPDATE t_counter SET `value` = 0 WHERE `name` = 'buy_mall_fashion_num';
end $$

delimiter ;


delimiter $$
drop procedure if exists merge_table $$
create procedure merge_table(IN dest VARCHAR(64), IN src VARCHAR(64))
begin

	DECLARE cal1 VARCHAR(1024);
	DECLARE columns_t_abnormal_transaction varchar(1024);
	
	-- 将dest和src的数据库区id设置到账号表中，以便进行账号相关的合服操作，之后记得运行drop_account_zoneid
	SET cal1 = CONCAT("ALTER TABLE `",dest,"`.t_account ADD COLUMN `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("UPDATE `",dest,"`.t_account SET zone_id = (SELECT zoneid FROM `",dest,"`.t_player_info LIMIT 1);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("ALTER TABLE `",src,"`.t_account ADD COLUMN `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("UPDATE `",src,"`.t_account SET zone_id = (SELECT zoneid FROM `",src,"`.t_player_info LIMIT 1);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
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

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_activity_op_task_new SELECT * FROM `",src,"`.t_activity_op_task_new;");
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

	-- 将dest和src的数据库区id设置到账号表中，以便进行账号相关的合服操作，之后记得运行drop_account_adventure_zoneid
	SET cal1 = CONCAT("ALTER TABLE `",dest,"`.t_account_adventure ADD COLUMN `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("UPDATE `",dest,"`.t_account_adventure SET zone_id = (SELECT zoneid FROM `",dest,"`.t_player_info LIMIT 1);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("ALTER TABLE `",src,"`.t_account_adventure ADD COLUMN `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("UPDATE `",src,"`.t_account_adventure SET zone_id = (SELECT zoneid FROM `",src,"`.t_player_info LIMIT 1);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account_adventure SELECT * FROM `",src,"`.t_account_adventure;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

		-- 将dest和src的数据库区id设置到账号表中，以便进行账号相关的合服操作，之后记得运行drop_account_monopoly_zoneid
	SET cal1 = CONCAT("ALTER TABLE `",dest,"`.t_account_monopoly ADD COLUMN `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("UPDATE `",dest,"`.t_account_monopoly SET zone_id = (SELECT zoneid FROM `",dest,"`.t_player_info LIMIT 1);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("ALTER TABLE `",src,"`.t_account_monopoly ADD COLUMN `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	SET cal1 = CONCAT("UPDATE `",src,"`.t_account_monopoly SET zone_id = (SELECT zoneid FROM `",src,"`.t_player_info LIMIT 1);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account_monopoly SELECT * FROM `",src,"`.t_account_monopoly;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;


	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_hire SELECT * FROM `",src,"`.t_hire;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;	

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_mall_acc_buy_record SELECT * FROM `",src,"`.t_mall_acc_buy_record;");
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
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_player_info_name SELECT * FROM `",src,"`.t_player_info_name WHERE NOT EXISTS(SELECT name FROM `",dest,"`.t_player_info_name WHERE name = `",src,"`.t_player_info_name.name);");
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

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_auction_new SELECT * FROM `",src,"`.t_auction_new;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_pet SELECT * FROM `",src,"`.t_pet;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_guild_storage SELECT * FROM `",src,"`.t_guild_storage;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;	
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_guildstorage_oprecord SELECT * FROM `",src,"`.t_guildstorage_oprecord;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SELECT GROUP_CONCAT(COLUMN_NAME) INTO columns_t_abnormal_transaction FROM information_schema.COLUMNS WHERE table_name = 't_abnormal_transaction' AND TABLE_SCHEMA = dest and COLUMN_NAME != 'guid';
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_abnormal_transaction(", columns_t_abnormal_transaction, ") SELECT ", columns_t_abnormal_transaction, " FROM `",src,"`.t_abnormal_transaction;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_questionnaire SELECT * FROM `",src,"`.t_questionnaire;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_mastersect_relation SELECT * FROM `",src,"`.t_mastersect_relation;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account_shop_role_buy_record SELECT * FROM `",src,"`.t_account_shop_role_buy_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_auction_transaction SELECT * FROM `",src,"`.t_auction_transaction;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_activity_op_attribute SELECT * FROM `",src,"`.t_activity_op_attribute;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;

	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_mall_item SELECT * FROM `",src,"`.t_mall_item;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_blackmarket_auction SELECT * FROM `",src,"`.t_blackmarket_auction;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_blackmarket_transaction SELECT * FROM `",src,"`.t_blackmarket_transaction;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account_counter SELECT * FROM `",src,"`.t_account_counter WHERE counter_type IN (1,2,3,4,5,6,10);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account_shop_acc_buy_record SELECT * FROM `",src,"`.t_account_shop_acc_buy_record WHERE shop_id IN (51,52,70,34);");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_expedition_map SELECT * FROM `",src,"`.t_expedition_map;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_expedition_member SELECT * FROM `",src,"`.t_expedition_member;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_head_frame SELECT * FROM `",src,"`.t_head_frame;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_item_deposit SELECT * FROM `",src,"`.t_item_deposit;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_week_sign SELECT * FROM `",src,"`.t_week_sign;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_activity_account_record SELECT * FROM `",src,"`.t_activity_account_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_activity_op_account_task SELECT * FROM `",src,"`.t_activity_op_account_task;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_activity_op_record SELECT * FROM `",src,"`.t_activity_op_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_account_task SELECT * FROM `",src,"`.t_account_task;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_new_title_accout SELECT * FROM `",src,"`.t_new_title_accout;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_new_title SELECT * FROM `",src,"`.t_new_title;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_team_copy SELECT * FROM `",src,"`.t_team_copy;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_currency_frozen SELECT * FROM `",src,"`.t_currency_frozen;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_honor_role SELECT * FROM `",src,"`.t_honor_role;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_honor_history SELECT * FROM `",src,"`.t_honor_history;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_shortcut_key SELECT * FROM `",src,"`.t_shortcut_key;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("INSERT INTO `",dest,"`.t_credit_point_record SELECT * FROM `",src,"`.t_credit_point_record;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
end $$

delimiter ;


delimiter $$
drop procedure if exists drop_account_zoneid $$
create procedure drop_account_zoneid(IN dest VARCHAR(64), IN src VARCHAR(64))
begin

		
	-- 删除账号表的区id字段
	
	DECLARE cal1 VARCHAR(1024);
	
	SET cal1 = CONCAT("ALTER TABLE `",dest,"`.t_account DROP COLUMN `zone_id`;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("ALTER TABLE `",src,"`.t_account DROP COLUMN `zone_id`;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
end $$

delimiter ;


delimiter $$
drop procedure if exists drop_account_adventure_zoneid $$
create procedure drop_account_adventure_zoneid(IN dest VARCHAR(64), IN src VARCHAR(64))
begin

		
	-- 删除账号表的区id字段
	
	DECLARE cal1 VARCHAR(1024);
	
	SET cal1 = CONCAT("ALTER TABLE `",dest,"`.t_account_adventure DROP COLUMN `zone_id`;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("ALTER TABLE `",src,"`.t_account_adventure DROP COLUMN `zone_id`;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
end $$

delimiter ;

delimiter $$
drop procedure if exists drop_account_monopoly_zoneid $$
create procedure drop_account_monopoly_zoneid(IN dest VARCHAR(64), IN src VARCHAR(64))
begin

		
	-- 删除账号表的区id字段
	
	DECLARE cal1 VARCHAR(1024);
	
	SET cal1 = CONCAT("ALTER TABLE `",dest,"`.t_account_monopoly DROP COLUMN `zone_id`;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
	SET cal1 = CONCAT("ALTER TABLE `",src,"`.t_account_monopoly DROP COLUMN `zone_id`;");
	SET @sql1=cal1;
	PREPARE stmt1 FROM @sql1;
	EXECUTE stmt1;
	
end $$

delimiter ;


delimiter $$
drop procedure if exists clear_alltables $$
create procedure clear_alltables()
begin
	TRUNCATE TABLE `t_account`;
	TRUNCATE TABLE `t_active_task`;
	TRUNCATE TABLE `t_activity_op`;
	TRUNCATE TABLE `t_activity_op_task`;
	TRUNCATE TABLE `t_activity_op_task_new`;
	TRUNCATE TABLE `t_charge_order`;
	TRUNCATE TABLE `t_charge_record`;
	TRUNCATE TABLE `t_counter`;
	TRUNCATE TABLE `t_dungeon`;
	TRUNCATE TABLE `t_g_dungeon_hard`;
	TRUNCATE TABLE `t_guild`;
	TRUNCATE TABLE `t_guild_member`;
	TRUNCATE TABLE `t_item`;
	TRUNCATE TABLE `t_jar_record`;
	TRUNCATE TABLE `t_jaritem_pool`;
	TRUNCATE TABLE `t_mail`;
	TRUNCATE TABLE `t_mailitem`;
	TRUNCATE TABLE `t_mall_gift_pack`;
	TRUNCATE TABLE `t_offline_event`;
	TRUNCATE TABLE `t_pk_statistic`;
	TRUNCATE TABLE `t_player_info`;
	TRUNCATE TABLE `t_player_info_name`;
	TRUNCATE TABLE `t_punishment`;
	TRUNCATE TABLE `t_red_packet`;
	TRUNCATE TABLE `t_red_packet_receiver`;
	TRUNCATE TABLE `t_relation`;
	TRUNCATE TABLE `t_retinue`;
	TRUNCATE TABLE `t_shop`;
	TRUNCATE TABLE `t_shopitem`;
	TRUNCATE TABLE `t_sortlist`;
	TRUNCATE TABLE `t_sys_record`;
	TRUNCATE TABLE `t_task`;
	TRUNCATE TABLE `t_merge_give_namecard`;
	TRUNCATE TABLE `t_pet`;
	TRUNCATE TABLE `t_mall_gift_pack`;
	TRUNCATE TABLE `t_auction_new`;
	TRUNCATE TABLE `t_abnormal_transaction`;
	TRUNCATE TABLE `t_questionnaire`;
	TRUNCATE TABLE `t_mastersect_relation`;
	TRUNCATE TABLE `t_account_shop_role_buy_record`;
	TRUNCATE TABLE `t_auction_transaction`;
	TRUNCATE TABLE `t_activity_op_attribute`;
	TRUNCATE TABLE `t_mall_item`;
	TRUNCATE TABLE `t_auction_attention`;
	TRUNCATE TABLE `t_blackmarket_auction`;
	TRUNCATE TABLE `t_blackmarket_transaction`;
	TRUNCATE TABLE `t_blackmarket_auction_price`;
end $$

delimiter ;


delimiter $$
drop procedure if exists start_merge $$
create procedure start_merge(IN zid INT)
begin
	-- call delete_overdue_player();
	call merge_player(zid);
	
	-- 这几个账号相关数据的处理要放在merge_account之前！！！
	call merge_adventure_team_repeative_name(zid);
	call merge_adventure_team_data(zid);
	call account_adventure(zid);
	call merger_hire(zid);
	call merger_mall(zid);
	call merger_monopoly(zid);
	
	call merge_account(zid);
	call merge_dungeon_hard();
	call merge_guild(zid);
	call merge_auction();
	call merge_black_market();
	call clear_adventure_team_sortlist();
	
end $$

delimiter ;
