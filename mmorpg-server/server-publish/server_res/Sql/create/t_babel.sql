CREATE TABLE IF NOT EXISTS `t_babelinfo` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		
	`mintime`	INT			UNSIGNED NOT NULL,		
	`minlevel`	SMALLINT	UNSIGNED NOT NULL,		
	`firstkillplayerid` BIGINT	UNSIGNED NOT NULL,
	`firstkillplayername`		varchar(32)  NOT NULL,	   
	`mintimeplayerid` BIGINT	UNSIGNED NOT NULL,
	`mintimeplayername`			varchar(32)	 NOT NULL,	   
	`minlevelplayerid` BIGINT	UNSIGNED NOT NULL,
	`minlevelplayername`		varchar(32)	 NOT NULL,	   
	PRIMARY KEY(`guid`)
	);


DELIMITER $$
CREATE PROCEDURE babelinfo_init()
BEGIN
	DECLARE lvl BIGINT UNSIGNED;
	SET lvl=1;
	WHILE lvl<=100 DO
		INSERT INTO `t_babelinfo`(`guid`,`mintime`,`minlevel`,`firstkillplayerid`,`firstkillplayername`,`mintimeplayerid` ,`mintimeplayername`,`minlevelplayerid`,`minlevelplayername`) VALUES(lvl,0,0,0,"",0,"",0,"");
		SET lvl=lvl+1;
	END WHILE;
END$$
DELIMITER ;

CALL babelinfo_init();

CREATE TABLE IF NOT EXISTS `t_merchant_rec` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		
	`playerid`	BIGINT	UNSIGNED NOT NULL,		
	`name`		VARCHAR(32)	NOT NULL,			
	`itemid`	INT	UNSIGNED NOT NULL,			
	`itemnum`	INT	UNSIGNED NOT NULL,			
	`merchantlv`	TINYINT	UNSIGNED NOT NULL,			
	`gettime`	INT	UNSIGNED NOT NULL,			

	PRIMARY KEY(`guid`)
);

CREATE EVENT IF NOT EXISTS event_clearmerchantrec ON SCHEDULE EVERY 600 SECOND
ON COMPLETION PRESERVE ENABLE
DO DELETE FROM `t_merchant_rec` WHERE UNIX_TIMESTAMP() > `gettime` + 1728000;
