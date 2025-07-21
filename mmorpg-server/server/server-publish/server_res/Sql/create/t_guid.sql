CREATE TABLE IF NOT EXISTS `t_guid`(
	`nodetype`	INT		UNSIGNED NOT NULL,				#节点类型
	`nodeid`	INT		UNSIGNED NOT NULL,				#节点id
	`seed`		BIGINT	UNSIGNED NOT NULL,				#种子
	PRIMARY KEY(`nodetype`,`nodeid`)
);

