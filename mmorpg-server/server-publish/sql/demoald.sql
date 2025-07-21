/*
 Navicat Premium Data Transfer

 Source Server         : 123131
 Source Server Type    : MySQL
 Source Server Version : 100231
 Source Host           : localhost:3306
 Source Schema         : 4156

 Target Server Type    : MySQL
 Target Server Version : 100231
 File Encoding         : 65001

 Date: 28/03/2022 05:43:39
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for xy_admin
-- ----------------------------
DROP TABLE IF EXISTS `xy_admin`;
CREATE TABLE `xy_admin`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `password` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `rmb` decimal(10, 2) NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 2 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_admin
-- ----------------------------
INSERT INTO `xy_admin` VALUES (1, 'admin', 'f2740df07c3c9beeb7889d4b2293d14a', 0.00);

-- ----------------------------
-- Table structure for xy_agent
-- ----------------------------
DROP TABLE IF EXISTS `xy_agent`;
CREATE TABLE `xy_agent`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `password` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `agent` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `state` int NULL DEFAULT 1,
  `regtime` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `logintime` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `pid` int NOT NULL DEFAULT 0,
  `branch` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '10',
  `type` int NOT NULL,
  `rmb` decimal(10, 2) NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 681 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_agent
-- ----------------------------
INSERT INTO `xy_agent` VALUES (680, 'mygamename', '4520b7f7f4e2e7278601ea35ed93a28f', 'admin', 1, '2022-03-28 04:32:57', '2022-03-28 04:32:57', 0, '10', 0, 0.00);

-- ----------------------------
-- Table structure for xy_baozhan
-- ----------------------------
DROP TABLE IF EXISTS `xy_baozhan`;
CREATE TABLE `xy_baozhan`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `token` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL,
  `type` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT '1' COMMENT '套餐按1234排',
  `paytime` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 105 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_baozhan
-- ----------------------------
INSERT INTO `xy_baozhan` VALUES (104, 'MjM3NTgyNDcyNTI5ODM4Mjg1', '1', '2022-03-14 13:34:32');

-- ----------------------------
-- Table structure for xy_cdk
-- ----------------------------
DROP TABLE IF EXISTS `xy_cdk`;
CREATE TABLE `xy_cdk`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `Name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `account` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `server` varchar(11) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `role` bigint NOT NULL,
  `Type` int NOT NULL,
  `BeginDay` datetime NOT NULL,
  `EndDay` datetime NOT NULL,
  `MailTxt` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `Award` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `state` int NOT NULL,
  `Maxr` int NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 14 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Records of xy_cdk
-- ----------------------------

-- ----------------------------
-- Table structure for xy_cdkorder
-- ----------------------------
DROP TABLE IF EXISTS `xy_cdkorder`;
CREATE TABLE `xy_cdkorder`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `cdkId` int NOT NULL,
  `role` bigint NOT NULL,
  `server` int NOT NULL,
  `account` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `time` datetime NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4490 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Records of xy_cdkorder
-- ----------------------------

-- ----------------------------
-- Table structure for xy_config
-- ----------------------------
DROP TABLE IF EXISTS `xy_config`;
CREATE TABLE `xy_config`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `namet` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `state` int NULL DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 14 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_config
-- ----------------------------
INSERT INTO `xy_config` VALUES (1, 'Site', '后台大陆', '站点名称', 1);
INSERT INTO `xy_config` VALUES (2, 'Site_1', '账号密码注册需.字母+数字组合', '站点标语', 1);
INSERT INTO `xy_config` VALUES (3, 'Site_2', '2022', '站点版权', 1);
INSERT INTO `xy_config` VALUES (4, 'url', 'http://42.192.81.101/', '站点地址', 1);
INSERT INTO `xy_config` VALUES (5, 'gmmailtitle', '3', 'GM邮件标题', 1);
INSERT INTO `xy_config` VALUES (6, 'gmmailsender', '系统', 'GM邮件发件人', 1);
INSERT INTO `xy_config` VALUES (7, 'gmmailcontent', '阿拉德', 'GM邮件内容', 1);
INSERT INTO `xy_config` VALUES (8, 'gmmaillevel', '0', 'GM邮件限制等级', 1);
INSERT INTO `xy_config` VALUES (9, 'gmmami', '912sy.com', 'GM密码', 0);
INSERT INTO `xy_config` VALUES (10, 'clientkey', 'TAnKhxca8CjALz5Pmif2prMomFmJe7ET', '客户端KEY', 1);
INSERT INTO `xy_config` VALUES (11, 'gg', '就要玩手游网- www.912sy.com', '公告', 1);
INSERT INTO `xy_config` VALUES (13, 'novice', 'ald001', '代理封号权限', 1);
INSERT INTO `xy_config` VALUES (12, 'skey', 'bd17f1390d756a354da833cc97401c74', '服务端KEY', 1);

-- ----------------------------
-- Table structure for xy_gameconfig
-- ----------------------------
DROP TABLE IF EXISTS `xy_gameconfig`;
CREATE TABLE `xy_gameconfig`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `server` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `surface` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `state` int NULL DEFAULT NULL,
  `ip` varchar(100) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `port` int NOT NULL,
  `gmport` int NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 42 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_gameconfig
-- ----------------------------
INSERT INTO `xy_gameconfig` VALUES (1, '阿拉德之怒', '10', 'aldzn_1', 1, '42.192.81.101', 9027, 7377);

-- ----------------------------
-- Table structure for xy_gamemail
-- ----------------------------
DROP TABLE IF EXISTS `xy_gamemail`;
CREATE TABLE `xy_gamemail`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `operation` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `server` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `role` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `content` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `sender` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `attach` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `attach_item` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `level` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `return` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `state` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 1353 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_gamemail
-- ----------------------------

-- ----------------------------
-- Table structure for xy_gameorder
-- ----------------------------
DROP TABLE IF EXISTS `xy_gameorder`;
CREATE TABLE `xy_gameorder`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `sorder` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `gorder` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `server` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `role` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `rolename` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `agent` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `state` int NULL DEFAULT NULL,
  `uptime` datetime NULL DEFAULT NULL,
  `payment` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `money` decimal(10, 2) NULL DEFAULT NULL,
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `cmda` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `cmdb` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `merchant` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `servername` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 64975 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_gameorder
-- ----------------------------

-- ----------------------------
-- Table structure for xy_gamepay
-- ----------------------------
DROP TABLE IF EXISTS `xy_gamepay`;
CREATE TABLE `xy_gamepay`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `operation` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `server` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `role` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `rolename` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `agent` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `money` decimal(10, 2) NULL DEFAULT NULL,
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `servername` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `cmda` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `cmdb` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `md5` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `response` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `time` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 93118 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_gamepay
-- ----------------------------

-- ----------------------------
-- Table structure for xy_gmlog
-- ----------------------------
DROP TABLE IF EXISTS `xy_gmlog`;
CREATE TABLE `xy_gmlog`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `server` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `data` varchar(500) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `time` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_gmlog
-- ----------------------------

-- ----------------------------
-- Table structure for xy_houtaigoods
-- ----------------------------
DROP TABLE IF EXISTS `xy_houtaigoods`;
CREATE TABLE `xy_houtaigoods`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `gameid` int NULL DEFAULT NULL,
  `num` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `goodsname` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_houtaigoods
-- ----------------------------
INSERT INTO `xy_houtaigoods` VALUES (1, 800001618, '100', '65史诗防具自选礼盒');
INSERT INTO `xy_houtaigoods` VALUES (2, 167220009, '10', '炽天使：溯源');

-- ----------------------------
-- Table structure for xy_loginlog
-- ----------------------------
DROP TABLE IF EXISTS `xy_loginlog`;
CREATE TABLE `xy_loginlog`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `password` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `client` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `time` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `md5` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `ip` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `response` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `responsea` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_loginlog
-- ----------------------------

-- ----------------------------
-- Table structure for xy_payconfig
-- ----------------------------
DROP TABLE IF EXISTS `xy_payconfig`;
CREATE TABLE `xy_payconfig`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `platform` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `apiurl` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `merchant` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `key` varchar(600) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `state` int NULL DEFAULT NULL,
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `private_key` varchar(2000) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 5 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_payconfig
-- ----------------------------
INSERT INTO `xy_payconfig` VALUES (3, 'lingdu', 'http://api.nandun.cn/apisubmit', '11155', 'b2ffc21b9618e13cc35c3ab1216d4a379c3fd32b', 1, '一号', '');
INSERT INTO `xy_payconfig` VALUES (1, 'zfb', NULL, '2021003103665549', 'MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy804CdErKg2gogEEnGAmu7MjCuIgoCny66OLWbOHGgHSMG8IPDkhctqyJCKRC/Pt5tLBXxiDG3VLXvRwTzERZcdbqaOxjLDBrH7veLMXAyUURC2+dnIZNtiZS2ep+6l5w5N1ky+F3djDOJ3FV/mG7kKyBKOCfBM0bP8svqkBPv5sk3TXov70clyVohV+71sDuq++DMPbkwh425qFTJIXxsDQMakFhLFZ0hapn8w8GDjKNScYg2gijmnhLZ4EwS4lM3NU+t+Zrsuod+Rp5BezpTD8IHRw1lTPeGK1xekJf+Tk8VMeZcQouNkPS4l5iDQ7Riwu+u11KrceXqFA4L1HXwIDAQAB', 1, '4号', 'MIIEowIBAAKCAQEAjmRqEQOqk30yfT1VI6fQEIUWh8f2wMJOBwXddtMMGN5WIP0UEAxNrfVXhrKiAEJek0DKTgNKL8ZGNY+IwBeBbTppex1azLYJ8u+n2WOiR8OD2lWJLOpTwN02f/QMKVWbmN2zAIuMeni/fi6VHh5QQUNZSi1dHxNjdhEYAsnLOZIN3II8vCprqq/S7TvM6IsTj26+rOSTqZ4/ZbuPM8CAt3t0ZtsXiud7i/4vIw8NwbeHuciVhC3q0gRg0R4YNfyOKtWBAT9LZcnEy+wwxfolKfUAcvihiJlxvfN3ELbqVI3QY+FnoGKlWUZOhFJ507LeixjeOD74IoHSxgdw0eMjxQIDAQABAoIBAGF7h03cp7PPe3hYO+hsI7qbkqAkmt6+Z3LRZhvKQrbnyClPeaO++JNGqsRRmSz6QwsmyRl9kRWB9WEHwgRYU0g3PC2IHDiXy9ngjpX6awgcZzFba80vI2WfTIduycCmlh6icMDHyMWz0s2NEJONBz6W4BMzWVREDsyuyTRneFURhgxxWZTu2XNCpoynE68msh8kQdAKciYBoqyyufhE2i3dCm5Ium0ippmjIcekyQ2i9u34X1Y3yNrYeI+Hnrgyj1XZ2KLQPQwTo5CsdzGas/EWodrUTzLQ+1mgSXpBPDwX0ttkUT4J5cxbYR/W9antAttvzHMqPPJmnEjF73wndgUCgYEA9PgwANwi+kIHhrp1q0TxQJAp+eIUeL8Yx7maWc5KslQ3sLznFIdDHJOI9sVBerjgJJ16Abyfdcp+eKo3IhuY3GcQnqiSs82Oe1MF5KjW6jG5/PJBfhyxioE251Oxq3iwNnaDszJ1LkZCH9nKetnWpELlebHikstULgWz1dNPR2MCgYEAlM3MYHVy6J5UsrNQtfbqAuRkcoN6ZeAtlWW/Ob/1y4rN55RpvX8BdjszGXuCeKSdL2wrK9t2kRYw0gIH1KyTLio2/xaUV/CBz+jP4skErzD6BoYymuBdbe3yqa9eRnrYPPfFt5DHDJp+ImFiDMR5MD45LdAGIrpaPdQnHzadNLcCgYEA4ROUT4Xz4Up/zctOSDHNFEwhNUYDFzpFk2S2K+N+AEGe9ZVrIBJzmDL6OfndgtuQnjPOoP/k535JG6cRhxYiKgfMXmp2eBva2PbcSE7VQFK2F3GiuTO18JNulbbxXKJEDZZwxVoMDpthMBCUmLHEJCKO43SMv+FwE+ULaeCATA8CgYBD7zC/WN99E8vXScYvTOrJJzw0i5H1sMMe0bTbyKeOpXAbqcYcnT8tmxlZKOoH6h9Y5ylOfINE+byQJB0vGxqWmO9CxiwzebnKEZ/XCa75zIJZOCsiSgxdg6Slk0qXIr5WF0hM5ev59Q5g49S/v3MUc1PndnFWqX1JfPHfbi+XQQKBgGzOrI0Cesd8jhk4W8nvn6yYonIIvm8yGozXkT8CD2nGZaumCM8ZLdfRHlpWBIxwUhpOFPWFXIWlWZzkDFDAaTQi0qgB/SHy99QalleY0JcWto6JkBRdsTAUrhzDk6xwtQwf7z+/TE49kJkBWPZSLJwJwhRhr+JyHG/jQNcp2J/R');
INSERT INTO `xy_payconfig` VALUES (4, 'cx', 'https://hcxy.91cp1.com/', '943748', '65E378FA2424320684980ED9D7DB39CD', 1, '5号', '');

-- ----------------------------
-- Table structure for xy_recharge
-- ----------------------------
DROP TABLE IF EXISTS `xy_recharge`;
CREATE TABLE `xy_recharge`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `rechargeid` int NOT NULL,
  `state` int NOT NULL,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `rechargew` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `uptime` datetime NOT NULL,
  `money` decimal(10, 2) NOT NULL,
  `szDesc` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `moneya` decimal(10, 2) NOT NULL,
  `roleid` bigint NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 3437 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_recharge
-- ----------------------------

-- ----------------------------
-- Table structure for xy_rechargeguid
-- ----------------------------
DROP TABLE IF EXISTS `xy_rechargeguid`;
CREATE TABLE `xy_rechargeguid`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `rmb` decimal(10, 2) NOT NULL,
  `role` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `time` datetime NOT NULL,
  `state` int NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 27260 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_rechargeguid
-- ----------------------------

-- ----------------------------
-- Table structure for xy_rmbdorder
-- ----------------------------
DROP TABLE IF EXISTS `xy_rmbdorder`;
CREATE TABLE `xy_rmbdorder`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `account1` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `account2` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `rmb1` decimal(10, 2) NOT NULL,
  `rmb2` decimal(10, 2) NOT NULL,
  `rmb3` decimal(10, 2) NOT NULL,
  `rmb4` decimal(10, 2) NOT NULL,
  `time` datetime NOT NULL,
  `state` int NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 2808 CHARACTER SET = utf8 COLLATE = utf8_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_rmbdorder
-- ----------------------------

-- ----------------------------
-- Table structure for xy_rmborder
-- ----------------------------
DROP TABLE IF EXISTS `xy_rmborder`;
CREATE TABLE `xy_rmborder`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `account1` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `account2` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `rmb1` decimal(10, 2) NOT NULL,
  `rmb2` decimal(10, 2) NOT NULL,
  `rmb3` decimal(10, 2) NOT NULL,
  `rmb4` decimal(10, 2) NOT NULL,
  `time` datetime NOT NULL,
  `state` int NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 5038 CHARACTER SET = utf8 COLLATE = utf8_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_rmborder
-- ----------------------------

-- ----------------------------
-- Table structure for xy_user
-- ----------------------------
DROP TABLE IF EXISTS `xy_user`;
CREATE TABLE `xy_user`  (
  `Id` int NOT NULL AUTO_INCREMENT,
  `account` varchar(18) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `password` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `email` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `agent` varchar(18) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `state` int NULL DEFAULT NULL,
  `logintime` datetime NULL DEFAULT NULL,
  `regtime` datetime NULL DEFAULT NULL,
  `recharge` decimal(10, 2) NULL DEFAULT NULL,
  `ip` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `ipd` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `safety` varchar(18) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `lock` int NULL DEFAULT NULL,
  `rmb` decimal(10, 2) NOT NULL DEFAULT 0.00,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 66458 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_user
-- ----------------------------

-- ----------------------------
-- Table structure for xy_yueka
-- ----------------------------
DROP TABLE IF EXISTS `xy_yueka`;
CREATE TABLE `xy_yueka`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `token` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL,
  `type` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT '1' COMMENT '套餐按1234排',
  `endtime` int NULL DEFAULT NULL,
  `linqutime` int NULL DEFAULT NULL,
  `paytime` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 1565 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of xy_yueka
-- ----------------------------
INSERT INTO `xy_yueka` VALUES (1563, 'MTkzMDkzNTkzMjQyMTE0NDg4MQ==', 'yk1', 1732204800, 0, '2022-02-26 02:00:48');
INSERT INTO `xy_yueka` VALUES (1564, 'MjM3NTgyNDcyNTI5ODM4Mjg1', 'yk1', 1733328000, 0, '2022-03-11 21:43:29');

SET FOREIGN_KEY_CHECKS = 1;
