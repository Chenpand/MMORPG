CREATE TABLE `t_activity_acc_task` (
  `guid` bigint(20) unsigned NOT NULL,
  `accid` int(10) unsigned NOT NULL,
  `activity_id` int(10) unsigned NOT NULL,
  `task_id` int(10) unsigned NOT NULL,
  `status` tinyint(3) unsigned NOT NULL,
  `params` varchar(512) NOT NULL DEFAULT '',
  `accept_time` int(10) NOT NULL,
  `update_time` int(10) unsigned NOT NULL DEFAULT '0',
  `day_change_time` int(10) unsigned NOT NULL,
  PRIMARY KEY (`guid`),
  KEY `accid` (`accid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;