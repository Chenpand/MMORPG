CREATE TABLE `t_figure_statue` (
  `guid` bigint(20) unsigned NOT NULL,
  `accid` int(10) unsigned NOT NULL,
  `roleid` bigint(20) unsigned NOT NULL,
  `name` varchar(32) NOT NULL,
  `occu` tinyint(3) unsigned NOT NULL,
  `avatar` varchar(128) NOT NULL,
  `statuetype` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
