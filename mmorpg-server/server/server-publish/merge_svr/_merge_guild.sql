delimiter $$
drop procedure if exists merge_guild $$
create procedure merge_guild()
begin
		-- 声明变量
		declare _name varchar(32);
		declare _lastName varchar(32);
		declare _guid bigint default 0;
		declare i int default 1;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		declare _playerName varchar(30);
		declare _playerGuid bigint default 0;

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

    		IF _lastName <> _name THEN 
				SET i = 1;
			END IF;

    		IF i = 1 THEN
    			-- 更新公会名字
				UPDATE `t_guild` SET name=CONCAT(_name,'_',i) WHERE guid=_guid;

				SELECT guid,name INTO _playerGuid,_playerName FROM `t_guild_member` WHERE guildid=_guid AND post=13;
				 -- 插入要发改名卡
				INSERT INTO `t_merge_give_namecard`(guid,name,type) VALUES(_playerGuid,_playerName,2);
				
				SET i = i + 1;
			END IF;

			SET _lastName = _name;
			SET _curCnt = _curCnt + 1;
		END WHILE;
	
		-- 重置工会战
		UPDATE `t_guild` SET enroll_terrid=0,battle_score=0,occupy_terrid=0,inspire=0;

end $$

delimiter ;

-- CALL merge_player(23);