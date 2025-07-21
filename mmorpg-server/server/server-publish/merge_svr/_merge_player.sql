delimiter $$
drop procedure if exists merge_player $$
create procedure merge_player(IN zid INT)
begin
		-- 声明变量
		declare _name varchar(30);
		DECLARE _zoneid INT UNSIGNED DEFAULT 0;
		declare _lastName varchar(30);
		declare _guid bigint default 0;
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
				UPDATE `t_player_info` SET name=CONCAT(_name,'_',i) WHERE guid=_guid;
				-- 更新公会成员表
				UPDATE `t_guild_member` SET name=CONCAT(_name,'_',i) WHERE guid=_guid;
				-- 更新关系表
				UPDATE `t_relation` SET name=CONCAT(_name,'_',i) WHERE id=_guid;
				-- 更新排行榜
				UPDATE `t_sortlist` SET name=CONCAT(_name,'_',i) WHERE id=_guid;
				UPDATE `t_sortlist` SET ownername=CONCAT(_name,'_',i) WHERE ownerid=_guid;

				-- 插入要发改名卡
				INSERT INTO `t_merge_give_namecard`(guid,name,type) VALUES(_guid,CONCAT(_name,'_',i),1);

				SET i = i + 1;
			END IF;

			

			SET _lastName = _name;
			SET _curCnt = _curCnt + 1;
		END WHILE;
	
end $$

delimiter ;

-- CALL merge_player(23);