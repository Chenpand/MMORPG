CREATE TABLE `t_game_account` (
`accid`  int(10) UNSIGNED NOT NULL ,
`storagesize`  tinyint(3) UNSIGNED NOT NULL ,
PRIMARY KEY (`accid`),
INDEX `accid` USING BTREE (`accid`) 
);

CREATE TABLE `t_storage` (
  `guid` bigint(20) unsigned NOT NULL,
  `accid` int(10) unsigned NOT NULL,
  `itemid` int(10) unsigned NOT NULL,
  `num` smallint(5) unsigned NOT NULL,
  `bind` tinyint(3) unsigned NOT NULL,
  `pack` tinyint(3) unsigned NOT NULL,
  `grid` tinyint(3) unsigned NOT NULL,
  `phyatk` int(10) unsigned NOT NULL,
  `magatk` int(10) unsigned NOT NULL,
  `defense` int(10) unsigned NOT NULL,
  `strenth` int(10) unsigned NOT NULL,
  `stamina` int(10) unsigned NOT NULL,
  `intellect` int(10) unsigned NOT NULL,
  `spirit` int(10) unsigned NOT NULL,
  `qualitylv` tinyint(3) unsigned NOT NULL,
  `quality` tinyint(3) unsigned NOT NULL,
  `strengthen` tinyint(3) unsigned NOT NULL,
  `randattr` varchar(255) NOT NULL,
  `holenum` tinyint(3) unsigned NOT NULL,
  `gemstone` varchar(64) NOT NULL,
  `rune` varchar(32) NOT NULL,
  `param1` int(10) unsigned NOT NULL,
  `param2` int(10) unsigned NOT NULL,
  `deadline` int(10) unsigned NOT NULL,
  `strfailed` smallint(5) unsigned NOT NULL,
  `sealstate` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `sealcount` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `owner` (`accid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;