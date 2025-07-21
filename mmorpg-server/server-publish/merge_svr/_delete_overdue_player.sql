delimiter $$
drop procedure if exists delete_overdue_player $$
create procedure delete_overdue_player()
begin

-- 声明变量
		declare _guid bigint default 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
	
		-- 创建临时表
		CREATE TEMPORARY TABLE IF NOT EXISTS tmp_del_player (  
		`incid` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
		`guid` BIGINT UNSIGNED NOT NULL     
		);

		-- 清空临时表
		TRUNCATE TABLE tmp_del_player;

		-- 查询要删除的玩家插入到临时表
		INSERT INTO tmp_del_player(guid)
		select guid from t_player_info where deletetime=0 and onlinetime + 30 * 24 * 3600 < current_timestamp() and level < 30 and totleonlinetime < 24 * 3600 and totlechargenum =0;

		SELECT MIN(`incid`) INTO _curCnt FROM `tmp_del_player`;  
    	SELECT MAX(`incid`) INTO _maxCnt FROM `tmp_del_player`; 

    	-- 遍历临时表
    	WHILE _curCnt <= _maxCnt DO 
    		SELECT guid INTO _guid FROM `tmp_del_player` WHERE incid = _curCnt;

    		
			-- 删除玩家表
			DELETE FROM `t_player_info` WHERE guid=_guid;
			-- 删除活动任务
			DELETE FROM `t_active_task` WHERE owner=_guid;
			-- 删除运营活动任务
			DELETE FROM `t_activity_op_task` WHERE guid=_guid;
			-- 删除计数表
			DELETE FROM `t_counter` WHERE owner=_guid;
			-- 删除地下城表
			DELETE FROM `t_dungeon` WHERE owner=_guid;
			-- 删除公会成员
			DELETE FROM `t_guild_member` WHERE guid=_guid;
			-- 删除道具
			DELETE FROM `t_item` WHERE owner=_guid;
			-- 删除抽罐记录
			DELETE FROM `t_jaritem_pool` WHERE owner=_guid;
			-- 删除邮件
			DELETE FROM `t_mail` WHERE owner=_guid;
			-- 删除邮件物品
			DELETE FROM `t_mailitem` WHERE owner=_guid;
			-- 删除离线事件
			DELETE FROM `t_offline_event` WHERE owner=_guid;
			-- 删除pk记录
			DELETE FROM `t_pk_statistic` WHERE owner=_guid;
			-- 删除红包
			DELETE FROM `t_red_packet` WHERE owner_id=_guid;
			-- 删除红包接受者
			DELETE FROM `t_red_packet_receiver` WHERE receiver_id=_guid;
			-- 删除社会关系
			DELETE FROM `t_relation` WHERE owner=_guid OR id=_guid;
			-- 删除随从
			DELETE FROM `t_retinue` WHERE owner=_guid;
			-- 删除商店
			DELETE FROM `t_shop` WHERE owner=_guid;
			DELETE FROM `t_shopitem` WHERE owner=_guid;
			-- 删除排行榜
			DELETE FROM `t_sortlist` WHERE ownerid=_guid OR id=_guid;
			-- 删除任务
			DELETE FROM `t_task` WHERE owner=_guid;

			SET _curCnt = _curCnt + 1;
		END WHILE;
end $$

delimiter ;