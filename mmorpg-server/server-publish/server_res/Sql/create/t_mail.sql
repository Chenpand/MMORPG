#邮件表
CREATE TABLE IF NOT EXISTS `t_mail` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`owner`		BIGINT	UNSIGNED NOT NULL,		#邮件拥有者
	`senderid`	BIGINT	UNSIGNED NOT NULL,		#发送者id
	`senderaccid`	INT	UNSIGNED NOT NULL,		#发送者账号id
	`sendername`	VARCHAR(32)	NOT NULL,		#发送者名字
	`sendtime`	INT UNSIGNED NOT NULL,			#发送时间
	`title`		VARCHAR(128) NOT NULL,			#标题
	`content`	VARCHAR(1024) NOT NULL,			#内容
	`mailtype`		TINYINT UNSIGNED NOT NULL,	#邮件类型
	`status`		TINYINT UNSIGNED NOT NULL,	#状态
	`replytimes`	TINYINT UNSIGNED NOT NULL,	#回复次数
	`silver`	INT UNSIGNED NOT NULL,			#银币
	`items`		VARCHAR(128) NOT NULL,			#简单道具
	PRIMARY KEY(`guid`),
	INDEX(`owner`)
);

#删除邮件同时删除邮件附件
CREATE TRIGGER trigger_delmailitems BEFORE DELETE ON `t_mail`
FOR EACH ROW DELETE FROM `t_mailitem` where `mailid`=OLD.`guid`;

##删除40天前的邮件
CREATE EVENT IF NOT EXISTS event_clearmails ON SCHEDULE EVERY 600 SECOND
ON COMPLETION PRESERVE ENABLE
DO DELETE FROM `t_mail` WHERE UNIX_TIMESTAMP() > `sendtime` + 3456000;



#广播邮件
CREATE TABLE IF NOT EXISTS `t_broadcastmail` (
	`guid`		BIGINT	UNSIGNED NOT NULL,		#主键
	`sendtime`	INT UNSIGNED NOT NULL,			#发送时间
	`title`		VARCHAR(128) NOT NULL,			#标题
	`content`	VARCHAR(1024) NOT NULL,			#内容
	`items`		VARCHAR(128) NOT NULL,			#简单道具
	`level`		SMALLINT	UNSIGNED NOT NULL,	#等级
	PRIMARY KEY(`guid`)
);

#删除40天前的广播邮件
CREATE EVENT IF NOT EXISTS event_clearbmmails ON SCHEDULE EVERY 600 SECOND
ON COMPLETION PRESERVE ENABLE
DO DELETE FROM `t_broadcastmail` WHERE UNIX_TIMESTAMP() > `sendtime` + 3456000;



