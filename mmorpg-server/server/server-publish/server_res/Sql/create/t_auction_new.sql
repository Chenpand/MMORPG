/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50709
Source Host           : 127.0.0.1:3306
Source Database       : dnf-db

Target Server Type    : MYSQL
Target Server Version : 50709
File Encoding         : 65001

Date: 2016-07-21 10:59:44
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_auction_new`
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_new`;
CREATE TABLE `t_auction_new` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner` bigint(20) unsigned NOT NULL,
  `type` tinyint(10) unsigned NOT NULL,
  `pricetype` tinyint(10) unsigned NOT NULL,
  `price` int(10) unsigned NOT NULL,
  `counter_fee` int(10) unsigned NOT NULL COMMENT '手续费',
  `duetime` int(20) unsigned NOT NULL,
  `item_main_type` tinyint(10) unsigned DEFAULT NULL,
  `item_sub_type` int(10) unsigned DEFAULT NULL,
  `item_level` tinyint(10) unsigned DEFAULT NULL,
  `itemid` int(10) unsigned NOT NULL,
  `num` smallint(5) unsigned NOT NULL,
  `bind` tinyint(3) unsigned DEFAULT NULL,
  `quality` tinyint(3) unsigned DEFAULT NULL,
  `strengthen` tinyint(3) unsigned DEFAULT NULL,
  `randattr` varchar(255) DEFAULT NULL,
  `holenum` tinyint(3) unsigned DEFAULT NULL,
  `gemstone` varchar(64) DEFAULT NULL,
  `rune` varchar(32) DEFAULT NULL,
  `param1` int(10) unsigned DEFAULT NULL,
  `param2` int(10) unsigned DEFAULT NULL,
  `deadline` int(10) unsigned DEFAULT NULL,
  `strfailed` smallint(5) unsigned DEFAULT NULL,
  `phyatk` int(10) unsigned DEFAULT '0',
  `magatk` int(10) unsigned DEFAULT '0',
  `defense` int(10) unsigned DEFAULT '0',
  `strenth` int(10) unsigned DEFAULT '0',
  `stamina` int(10) unsigned DEFAULT '0',
  `intellect` int(10) unsigned DEFAULT '0',
  `spirit` int(10) unsigned DEFAULT '0',
  `qualitylv` tinyint(3) unsigned DEFAULT '0',
  `sealstate` tinyint(3) unsigned DEFAULT '0',
  `sealcount` int(10) unsigned DEFAULT '0',
  `accid` int(10) unsigned DEFAULT '0',
  `disphyatk` int(10) unsigned DEFAULT '0',
  `dismagatk` int(10) unsigned DEFAULT '0',
  `disdef` int(10) unsigned DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `owner` (`owner`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
