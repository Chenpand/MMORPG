#�ͷ���
CREATE TABLE IF NOT EXISTS `t_g_punishment` (
	`accid`		INT		UNSIGNED NOT NULL,		#�˺�id
	`duetime`	INT 	UNSIGNED NOT NULL,		#����ʱ��
	PRIMARY KEY(`accid`)
);

#ȫ����Ϸ������
CREATE TABLE IF NOT EXISTS `t_g_gameparam`(
	`name`	VARCHAR(32)	NOT NULL,		#���� 
	`value`	INT	UNSIGNED NOT NULL DEFAULT 0, #ֵ
	PRIMARY KEY(`name`)
);