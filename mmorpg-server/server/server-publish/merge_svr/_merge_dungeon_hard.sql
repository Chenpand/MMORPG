delimiter $$
drop procedure if exists merge_dungeon_hard $$
create procedure merge_dungeon_hard()
begin
		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _lastDungeonId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		declare _dungeonId INT UNSIGNED DEFAULT 0;
		declare _guid bigint default 0;
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

-- CALL merge_player(23);