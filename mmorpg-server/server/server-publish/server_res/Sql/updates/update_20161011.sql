ALTER TABLE `t_player_info` DROP COLUMN `tasks`;
ALTER TABLE `t_player_info` DROP COLUMN `achievementtasks`;
ALTER TABLE `t_player_info` DROP COLUMN `dailytasks`;

DROP TABLE IF EXISTS `t_task`;
CREATE TABLE `t_task` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner` bigint(20) unsigned NOT NULL,
  `dataid` int(10) unsigned NOT NULL,
  `status` tinyint(3) unsigned NOT NULL,
  `parameter` varchar(512) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
