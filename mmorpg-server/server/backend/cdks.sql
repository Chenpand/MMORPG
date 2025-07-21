/*
Navicat MySQL Data Transfer

Source Server         : 101.43.22.176_3306
Source Server Version : 50734
Source Host           : 101.43.22.176:3306
Source Database       : cdks

Target Server Type    : MYSQL
Target Server Version : 50734
File Encoding         : 65001

Date: 2022-02-24 21:58:07
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `cdk`
-- ----------------------------
DROP TABLE IF EXISTS `cdk`;
CREATE TABLE `cdk` (
  `cdk` varchar(32) NOT NULL,
  `lv` tinyint(4) DEFAULT '0',
  `qid` int(8) DEFAULT NULL,
  `uid` varchar(32) DEFAULT NULL,
  `pass` varchar(32) DEFAULT NULL,
  `status` tinyint(4) DEFAULT '0',
  PRIMARY KEY (`cdk`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of cdk
-- ----------------------------
