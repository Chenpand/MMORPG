#离线事件
CREATE TABLE IF NOT EXISTS `t_offline_event` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#拥有者
	`type`		SMALLINT	UNSIGNED NOT NULL,	#类型
	`time`		INT	UNSIGNED NOT NULL,			#发生事件
	`data`		VARCHAR(256) NOT NULL,			#数据
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);

##删除30天前的事件
CREATE EVENT IF NOT EXISTS event_clear_offlineevents ON SCHEDULE EVERY 600 SECOND
ON COMPLETION PRESERVE ENABLE
DO DELETE FROM `t_offline_event` WHERE UNIX_TIMESTAMP() > `time` + 2592000;

