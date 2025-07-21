CREATE TABLE `t_retinue` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner` bigint(20) unsigned NOT NULL,
  `dataid` int(10) unsigned NOT NULL,
  `skills` varchar(128) NOT NULL DEFAULT '',
  `createtime` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
