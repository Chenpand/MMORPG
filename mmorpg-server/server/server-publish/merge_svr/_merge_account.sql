delimiter $$
DROP PROCEDURE IF EXISTS merge_account $$

CREATE PROCEDURE merge_account()
BEGIN

		-- 声明变量
		DECLARE _lastAccId INT UNSIGNED DEFAULT 0;
		DECLARE _accid INT UNSIGNED DEFAULT 0;
		DECLARE _guid BIGINT UNSIGNED DEFAULT 0;
		DECLARE _curCnt INT UNSIGNED DEFAULT 0;
		DECLARE _maxCnt INT UNSIGNED DEFAULT 0;
		DECLARE _point INT UNSIGNED DEFAULT 0;
		DECLARE _reg_time INT UNSIGNED DEFAULT 0;
		DECLARE _vipexp INT UNSIGNED DEFAULT 0;
		DECLARE _viplvl INT UNSIGNED DEFAULT 0;
		DECLARE _total_charge_num INT UNSIGNED DEFAULT 0;
		DECLARE _storage_size INT UNSIGNED DEFAULT 0;

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
					SELECT MIN(reg_time) INTO _reg_time FROM `t_account` WHERE accid = _accid;
					SELECT MAX(vipexp) INTO _vipexp FROM `t_account` WHERE accid = _accid;
					SELECT MAX(viplvl) INTO _viplvl FROM `t_account` WHERE accid = _accid;
					SELECT SUM(total_charge_num) INTO _total_charge_num FROM `t_account` WHERE accid = _accid;
					SELECT MAX(storage_size) INTO _storage_size FROM `t_account` WHERE accid = _accid;

					UPDATE `t_account` SET point = _point,
														reg_time = _reg_time,
														vipexp = _vipexp,
														viplvl = _viplvl,
														total_charge_num = _total_charge_num,
														storage_size = _storage_size
					WHERE guid = _guid;
					
					SET _lastAccId = _accid;
        		END IF;

        		SET _curCnt = _curCnt + 1;  
    	END WHILE;
	
END $$

delimiter ;