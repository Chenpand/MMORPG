CREATE TABLE IF NOT EXISTS `t_examination` (
	`guid`			BIGINT UNSIGNED NOT NULL,		
	`questionid`	INT	UNSIGNED NOT NULL,		
	`answer`	    TINYINT UNSIGNED NOT NULL,		
	`socre` 		INT		UNSIGNED NOT NULL,   
	`totaltime`		INT		UNSIGNED  NOT NULL,	 
	PRIMARY KEY(`guid`)
	);