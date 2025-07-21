/*
 Navicat Premium Data Transfer

 Source Server         : 决战
 Source Server Type    : MySQL
 Source Server Version : 50734
 Source Host           : localhost:3306
 Source Schema         : aldzn_10

 Target Server Type    : MySQL
 Target Server Version : 50734
 File Encoding         : 65001

 Date: 19/02/2022 14:16:59
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for t_abnormal_transaction
-- ----------------------------
DROP TABLE IF EXISTS `t_abnormal_transaction`;
CREATE TABLE `t_abnormal_transaction`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `handle_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '状态',
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `buyer` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '买家',
  `seller` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '卖家',
  `transaction_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '交易时间',
  `transaction_id` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `item_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '物品实例id',
  `item_data_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '物品id',
  `item_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '物品数量',
  `money_type` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '货币类型:\r\n0-无效,\r\n1-金币,\r\n2-点卷,\r\n3-经验\r\n11-勇者之魂\r\n12-决斗币\r\n13-复活币\r\n14-公会贡献\r\n21-以物换物',
  `transaction_amount` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '交易金额',
  `frozen_amount` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '冻结金额',
  `back_amount` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '保释金',
  `pre_frozen_record_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '前一个冻结记录id',
  `freeze_object` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `back_way` tinyint(3) UNSIGNED NOT NULL DEFAULT 3,
  `frozen_permanent` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `treasure_flag` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `reason` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `mail_notify_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `unfrozen_amount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `rmplayer_transmoney_flag` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‰£é™¤çŽ©å®¶äº¤æ˜“é‡‘æ ‡å¿—',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `status+buyer+freeze_obj`(`handle_status`, `buyer`, `freeze_object`) USING BTREE,
  INDEX `status+seller+freeze_obj`(`handle_status`, `seller`, `freeze_object`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account
-- ----------------------------
DROP TABLE IF EXISTS `t_account`;
CREATE TABLE `t_account`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL COMMENT '账号',
  `point` int(10) UNSIGNED NOT NULL COMMENT '点劵',
  `credit_point` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `reg_time` int(10) UNSIGNED NOT NULL,
  `gmauthority` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `charge_record` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `viplvl` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `vipexp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `total_charge_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `month_card_expire_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `storage_size` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `role_delete_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '��ɫɾ��ʱ���',
  `role_recover_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '��ɫ�ָ�ʱ���',
  `money_manage_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `veteran_return_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `veteran_return_role` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `security_passwd` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å®‰å…¨é”å¯†ç ',
  `security_freeze_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®‰å…¨è´¦æˆ·å†»ç»“æ—¶é—´',
  `security_unfreeze_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®‰å…¨è´¦æˆ·è§£å†»æ—¶é—´',
  `common_device` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å¸¸ç”¨è®¾å¤‡',
  `security_passwd_error_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®‰å…¨é”å¯†ç é”™è¯¯æ¬¡æ•°',
  `acc_offline_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è´¦å·ç¦»çº¿æ—¶é—´',
  `gnome_coin_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `gnome_coin_refresh_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `weapon_lease_tickets` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `unlock_extensible_role_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¯æ‰©å±•è§’è‰²è§£é”æ ä½',
  `adventure_team_name` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å†’é™©é˜Ÿå',
  `adventure_team_level` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `adventure_team_exp` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å†’é™©é˜Ÿç»éªŒ',
  `adventure_team_grade_id` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å†’é™©é˜Ÿè¯„çº§id',
  `all_role_value_score` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è§’è‰²æ€»è¯„åˆ†',
  `unlocked_new_occus` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'è§£é”çš„æ–°èŒä¸š',
  `query_new_occus_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æŸ¥è¯¢æ–°èŒä¸šçš„æ—¶é—´',
  `offline_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `account_achievement_score` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è´¦å·æˆå°±ç‚¹',
  `adventure_team_grade` varchar(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å†’é™©é˜Ÿè¯„çº§',
  `mall_point` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `adventure_coin` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE,
  INDEX `rds_idx_0`(`adventure_team_name`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_adventure
-- ----------------------------
DROP TABLE IF EXISTS `t_account_adventure`;
CREATE TABLE `t_account_adventure`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `level` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `exp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `passtype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `normalreward` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `highreward` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `activity` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `unlock` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `seasonid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `extraexp` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_counter
-- ----------------------------
DROP TABLE IF EXISTS `t_account_counter`;
CREATE TABLE `t_account_counter`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `counter_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `counter_num` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `refresh_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `acc_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_monopoly
-- ----------------------------
DROP TABLE IF EXISTS `t_account_monopoly`;
CREATE TABLE `t_account_monopoly`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `vsersion` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `turn` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `step` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `remain_roll` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `roll_times` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `coin` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `buff` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `cards` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `exchanged` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for t_account_shop_acc_buy_record
-- ----------------------------
DROP TABLE IF EXISTS `t_account_shop_acc_buy_record`;
CREATE TABLE `t_account_shop_acc_buy_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `shop_item_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `buyed_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `refresh_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `update_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `acc_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `shop_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_shop_item
-- ----------------------------
DROP TABLE IF EXISTS `t_account_shop_item`;
CREATE TABLE `t_account_shop_item`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `shop_itemid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `owner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `update_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_shop_role_buy_record
-- ----------------------------
DROP TABLE IF EXISTS `t_account_shop_role_buy_record`;
CREATE TABLE `t_account_shop_role_buy_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `shop_item_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `buyed_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `refresh_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `update_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `roleid`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_task
-- ----------------------------
DROP TABLE IF EXISTS `t_account_task`;
CREATE TABLE `t_account_task`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `dataid` int(10) UNSIGNED NOT NULL,
  `status` tinyint(3) UNSIGNED NOT NULL,
  `parameter` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `finished_time` int(10) UNSIGNED NOT NULL,
  `submit_count` tinyint(3) UNSIGNED NOT NULL,
  `acc_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_account_trans
-- ----------------------------
DROP TABLE IF EXISTS `t_account_trans`;
CREATE TABLE `t_account_trans`  (
  `cc_accid` int(10) UNSIGNED NOT NULL,
  `zy_accid` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`cc_accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_active_task
-- ----------------------------
DROP TABLE IF EXISTS `t_active_task`;
CREATE TABLE `t_active_task`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `dataid` int(10) UNSIGNED NOT NULL,
  `status` tinyint(3) UNSIGNED NOT NULL,
  `parameter` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `accepttime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_acc_task
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_acc_task`;
CREATE TABLE `t_activity_acc_task`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `activity_id` int(10) UNSIGNED NOT NULL,
  `task_id` int(10) UNSIGNED NOT NULL,
  `status` tinyint(3) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `accept_time` int(10) UNSIGNED NOT NULL,
  `update_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `day_change_time` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_account_record
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_account_record`;
CREATE TABLE `t_activity_account_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `acc_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `activity_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `cur_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `states` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `acc_index`(`acc_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_op
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_op`;
CREATE TABLE `t_activity_op`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `tmptype` smallint(5) UNSIGNED NOT NULL,
  `name` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `tag` tinyint(3) UNSIGNED NOT NULL,
  `tagendtime` int(10) UNSIGNED NOT NULL,
  `preparetime` int(10) UNSIGNED NOT NULL,
  `starttime` int(10) UNSIGNED NOT NULL,
  `endtime` int(10) UNSIGNED NOT NULL,
  `desc` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `ruledesc` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `circletype` tinyint(3) UNSIGNED NOT NULL,
  `tasks` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `taskdesc` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `taskrewards` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `parm` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `logdesc` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_op_account_task
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_op_account_task`;
CREATE TABLE `t_activity_op_account_task`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `task_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `parameter` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner_id`(`owner_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_op_attribute
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_op_attribute`;
CREATE TABLE `t_activity_op_attribute`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `activity_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `parameter` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `create_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `update_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `refresh_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `roleid`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_op_record
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_op_record`;
CREATE TABLE `t_activity_op_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `behavior` int(10) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `num` int(10) UNSIGNED NOT NULL,
  `record_time` int(10) UNSIGNED NOT NULL,
  `owner_type` int(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for t_activity_op_task
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_op_task`;
CREATE TABLE `t_activity_op_task`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `task_info_str` varchar(20480) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activity_op_task_new
-- ----------------------------
DROP TABLE IF EXISTS `t_activity_op_task_new`;
CREATE TABLE `t_activity_op_task_new`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `task_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `parameter` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `roleid`(`owner_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_activitygiftbag
-- ----------------------------
DROP TABLE IF EXISTS `t_activitygiftbag`;
CREATE TABLE `t_activitygiftbag`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `dataid` int(10) UNSIGNED NOT NULL,
  `state` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_announcement
-- ----------------------------
DROP TABLE IF EXISTS `t_announcement`;
CREATE TABLE `t_announcement`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `color` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `dataid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `content` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `begintime` int(10) UNSIGNED NOT NULL,
  `interval` int(10) UNSIGNED NOT NULL,
  `repeattimes` int(10) UNSIGNED NOT NULL,
  `showzone` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `gmid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_auction
-- ----------------------------
DROP TABLE IF EXISTS `t_auction`;
CREATE TABLE `t_auction`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL,
  `price` int(10) UNSIGNED NOT NULL,
  `pricetype` tinyint(3) UNSIGNED NOT NULL,
  `duetime` int(10) UNSIGNED NOT NULL,
  `auctionmoney` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `itemid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `num` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `bind` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `quality` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `strengthen` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `holenum` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `gemstone` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `rune` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `param2` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `deadline` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strfailed` smallint(5) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_auction_attention
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_attention`;
CREATE TABLE `t_auction_attention`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `auction_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `attent_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å…³æ³¨æ—¶é—´',
  `delete_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_auction_new
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_new`;
CREATE TABLE `t_auction_new`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `account` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `type` tinyint(10) UNSIGNED NOT NULL,
  `pricetype` tinyint(10) UNSIGNED NOT NULL,
  `price` int(10) UNSIGNED NOT NULL,
  `counter_fee` int(10) UNSIGNED NOT NULL COMMENT '手续费',
  `duetime` int(20) UNSIGNED NOT NULL,
  `on_sale_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '上架时间',
  `item_main_type` tinyint(10) UNSIGNED NULL DEFAULT NULL,
  `item_sub_type` int(10) UNSIGNED NULL DEFAULT NULL,
  `item_level` tinyint(10) UNSIGNED NULL DEFAULT NULL,
  `item_src_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `offsale_handled` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦åšè¿‡ä¸‹æž¶å¤„ç†',
  `recove_handled` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦åšè¿‡æ‰«è´§å¤„ç†',
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `quality` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `strengthen` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `holenum` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `gemstone` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `addmagic` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `param1` int(10) UNSIGNED NULL DEFAULT NULL,
  `param2` int(10) UNSIGNED NULL DEFAULT NULL,
  `deadline` int(10) UNSIGNED NULL DEFAULT NULL,
  `strfailed` smallint(5) UNSIGNED NULL DEFAULT NULL,
  `phyatk` int(10) UNSIGNED NULL DEFAULT 0,
  `magatk` int(10) UNSIGNED NULL DEFAULT 0,
  `phydef` int(10) UNSIGNED NULL DEFAULT 0,
  `magdef` int(10) UNSIGNED NULL DEFAULT NULL,
  `strenth` int(10) UNSIGNED NULL DEFAULT 0,
  `stamina` int(10) UNSIGNED NULL DEFAULT 0,
  `intellect` int(10) UNSIGNED NULL DEFAULT 0,
  `spirit` int(10) UNSIGNED NULL DEFAULT 0,
  `qualitylv` tinyint(3) UNSIGNED NULL DEFAULT 0,
  `sealstate` tinyint(3) UNSIGNED NULL DEFAULT 0,
  `sealcount` int(10) UNSIGNED NULL DEFAULT 0,
  `accid` int(10) UNSIGNED NULL DEFAULT 0,
  `disphyatk` int(10) UNSIGNED NULL DEFAULT 0,
  `dismagatk` int(10) UNSIGNED NULL DEFAULT 0,
  `disphydef` int(10) UNSIGNED NULL DEFAULT 0,
  `dismagdef` int(10) UNSIGNED NULL DEFAULT 0,
  `pvpdisphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strenthnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `valuedscore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `setqlnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addpreciousbead` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `transferstone` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `recoScore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ip` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `device_id` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `auction_cool_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“å†·å´æ—¶é—´æˆ³',
  `on_public_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç‰©å“æ‹å–å…¬ç¤ºå¼€å§‹æ—¶é—´æˆ³',
  `ownerVipLev` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ä¸Šæž¶è€…vipç­‰çº§',
  `coupon_strengthen_to` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¼ºåŒ–å·å¼ºåŒ–åŠ ç­‰çº§',
  `bead_extripe_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç æ‘˜é™¤æ¬¡æ•°',
  `bead_replace_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç ç½®æ¢æ¬¡æ•°',
  `equiptype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancetype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancefailed` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `enhancecount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auctionTransNum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `inscriptionHoles` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE,
  INDEX `duetime`(`duetime`) USING BTREE,
  INDEX `itemid`(`itemid`) USING BTREE,
  INDEX `item_sub_type`(`item_sub_type`) USING BTREE,
  INDEX `item_main_type`(`item_main_type`) USING BTREE,
  INDEX `max`(`item_main_type`, `item_sub_type`, `item_level`, `quality`, `strengthen`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_auction_recommend_price
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_recommend_price`;
CREATE TABLE `t_auction_recommend_price`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `item_type_id` int(10) UNSIGNED NOT NULL,
  `strengthen` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `price` int(10) UNSIGNED NOT NULL,
  `recent_price` varchar(5120) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `bead_buff_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç é™„åŠ å±žæ€§id',
  `enhance_type` tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'çº¢å­—è£…å¤‡å¢žå¹…ç±»åž‹',
  `price_update_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¹³å‡ä»·æ ¼åˆ·æ–°æ—¶é—´',
  `visibility_price` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'çŽ©å®¶å¯è§å¹³å‡ä»·æ ¼',
  `vis_price_update_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¯è§å¹³å‡ä»·æ ¼æ›´æ–°æ—¶é—´æˆ³',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_auction_transaction
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_transaction`;
CREATE TABLE `t_auction_transaction`  (
  `guid` bigint(20) UNSIGNED NOT NULL COMMENT 'æ‹å–è¡Œçå“äº¤æ˜“è®¢å•ä¸»é”®id',
  `auction_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 't_auction_newä¸»é”®id',
  `is_treas` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `seller` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `buyer` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `transaction_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'äº¤æ˜“æ—¶é—´',
  `item_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“ç‰©å“å¯¹è±¡id',
  `item_data_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“ç‰©å“dataid',
  `item_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“ç‰©å“æ•°é‡',
  `item_qualitylv` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å“çº§',
  `item_strengthen` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¼ºåŒ–ç­‰çº§',
  `item_bead_buffId` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'é“å…·æ˜¯å®ç çš„æ—¶å€™è®°å½•é™„åŠ buffid',
  `item_equip_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `item_enhance_type` tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'çº¢å­—è£…å¤‡å¢žå¹…ç±»åž‹',
  `item_enhance_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_mount_beadId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_bead_averPrice` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'äº¤æ˜“è£…å¤‡æ—¶é™„åŠ å®ç å¹³å‡äº¤æ˜“ä»·æ ¼',
  `item_mount_magicId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_magic_averPrice` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'äº¤æ˜“è£…å¤‡æ—¶é™„é­”å¡å¹³å‡äº¤æ˜“ä»·æ ¼',
  `item_name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `buy_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è´­ä¹°æ•°é‡',
  `item_score` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'äº¤æ˜“ç‰©å“è¯„åˆ†',
  `money_type` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è´§å¸ç±»åž‹',
  `transaction_amount` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'äº¤æ˜“é‡‘é¢',
  `counter_fee` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‰‹ç»­è´¹',
  `transaction_extra_profit` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'äº¤æ˜“é¢å¤–èŽ·åˆ©',
  `deposit` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ä¸Šæž¶æŠ¼é‡‘',
  `unit_price` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å•ä»·',
  `mail_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ä¹°å®¶é¢†å–çš„é‚®ä»¶id',
  `verify_end_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®¡æ ¸ç»“æŸæ—¶é—´',
  `program_audit_result` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç¨‹åºå®¡æ ¸ç»“æžœ',
  `program_audit_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç¨‹åºå®¡æ ¸æ—¶é—´',
  `punishA_id` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®¡æ ¸ç»“æŸæ—¶ï¼Œå¯¹åº”è£…å¤‡æˆ–é“å…·æ²¡æœ‰è¢«ä½¿ç”¨',
  `punishB_id` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®¡æ ¸ç»“æŸæ—¶ï¼Œå¯¹åº”è£…å¤‡æˆ–é“å…·è¢«ä½¿ç”¨æ¶ˆè€—æˆ–é”€æ¯æˆ–å±žæ€§å‘ç”Ÿæ”¹å˜',
  `punishC_id` tinyint(3) UNSIGNED NOT NULL DEFAULT 100 COMMENT 'äººå·¥å®¡æ ¸å¤„ç†',
  `punished_id` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æœ€ç»ˆé€‰æ‹©å¤„ç½šid',
  `item_returned` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦å½’è¿˜é“å…·',
  `state` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'çŠ¶æ€0æœªå¤„ç†1å¤„ç†ä¸­2æ— å¼‚å¸¸å¤„ç†3å¼‚å¸¸è¿è§„å¤„ç†4å¤„ç†è¶…æ—¶',
  `min_price_onsale` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `isSysRecove` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `isNotRecAveprice` int(3) UNSIGNED NOT NULL DEFAULT 0,
  `mprice` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_mount_beadBuffId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_mount_magicLv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `verify_end_time`(`verify_end_time`) USING BTREE,
  INDEX `state`(`state`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_babelinfo
-- ----------------------------
DROP TABLE IF EXISTS `t_babelinfo`;
CREATE TABLE `t_babelinfo`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `mintime` int(10) UNSIGNED NOT NULL,
  `minlevel` smallint(5) UNSIGNED NOT NULL,
  `firstkillplayerid` bigint(20) UNSIGNED NOT NULL,
  `firstkillplayername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `mintimeplayerid` bigint(20) UNSIGNED NOT NULL,
  `mintimeplayername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `minlevelplayerid` bigint(20) UNSIGNED NOT NULL,
  `minlevelplayername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_blackmarket_auction
-- ----------------------------
DROP TABLE IF EXISTS `t_blackmarket_auction`;
CREATE TABLE `t_blackmarket_auction`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `op_activity_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‰€å±žè¿è¥æ´»åŠ¨id',
  `back_buy_item_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å›žè´­çš„ç‰©å“id',
  `back_buy_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ”¶è´­ç±»åž‹',
  `back_buy_subtype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `back_buy_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ”¶è´­æ•°é‡',
  `fixed_price` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å›ºå®šæ”¶è´­ä»·æ ¼',
  `recommend_price` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æŽ¨èä»·æ ¼',
  `begin_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹å¼€å§‹æ—¶é—´',
  `end_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹ç»“æŸæ—¶é—´',
  `off_sale` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦ä¸‹æž¶0:å¦ï¼Œ1:æ˜¯',
  `is_settled` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦å·²ç»ç»“ç®—',
  `is_sys_cancel` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦ç³»ç»Ÿè‡ªåŠ¨å–æ¶ˆçš„',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `is_settled`(`is_settled`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_blackmarket_auction_price
-- ----------------------------
DROP TABLE IF EXISTS `t_blackmarket_auction_price`;
CREATE TABLE `t_blackmarket_auction_price`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `item_type_id` int(10) UNSIGNED NOT NULL,
  `price` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹å¹³å‡ä»·æ ¼',
  `auction_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹é¡¹id',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_blackmarket_transaction
-- ----------------------------
DROP TABLE IF EXISTS `t_blackmarket_transaction`;
CREATE TABLE `t_blackmarket_transaction`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'åˆ›å»ºæ—¶é—´',
  `opact_main_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‰€å±žä¸»æ´»åŠ¨id',
  `opact_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‰€å±žæ´»åŠ¨id',
  `auction_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹é¡¹guid',
  `item_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹çš„ç‰©å“id',
  `auctioner_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹çŽ©å®¶guid',
  `auctioner_name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `auctioner_accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auctioner_viplv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'çŽ©å®¶vipç­‰çº§',
  `auction_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹ç±»åž‹',
  `auction_money_type` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹è´§å¸ç±»åž‹',
  `auction_price` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹ä»·æ ¼',
  `off_sale` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦ä¸‹æž¶',
  `auction_result` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç«žæ‹ç»“æžœ, 0:å¤±è´¥,1:æˆåŠŸ',
  `state` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è®¢å•å¤„ç†çŠ¶æ€ 0:æœªå¤„ç†, 1:å·²ç»å¤„ç†',
  `money_sended` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å‘æ”¾é‡‘å¸æ ‡å¿— 0:æœªå‘,1:å·²å‘æ”¾',
  `item_returned` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'é“å…·é€€è¿˜æ ‡å¿— 0:æœªé€€è¿˜,1:å·²é€€è¿˜',
  `item_src_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `item_name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `delete_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'åˆ é™¤æ—¶é—´',
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `phyatk` int(10) UNSIGNED NULL DEFAULT 0,
  `magatk` int(10) UNSIGNED NULL DEFAULT 0,
  `magdef` int(10) UNSIGNED NULL DEFAULT NULL,
  `phydef` int(10) UNSIGNED NULL DEFAULT 0,
  `strenth` int(10) UNSIGNED NULL DEFAULT 0,
  `stamina` int(10) UNSIGNED NULL DEFAULT 0,
  `intellect` int(10) UNSIGNED NULL DEFAULT 0,
  `spirit` int(10) UNSIGNED NULL DEFAULT 0,
  `qualitylv` tinyint(3) UNSIGNED NULL DEFAULT 0,
  `quality` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `strengthen` tinyint(3) UNSIGNED NULL DEFAULT NULL,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `dayusenum` int(10) UNSIGNED NOT NULL,
  `strenthnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addmagic` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `param1` int(10) UNSIGNED NULL DEFAULT NULL,
  `param2` int(10) UNSIGNED NULL DEFAULT NULL,
  `deadline` int(10) UNSIGNED NULL DEFAULT NULL,
  `strfailed` smallint(5) UNSIGNED NULL DEFAULT NULL,
  `sealstate` tinyint(3) UNSIGNED NULL DEFAULT 0,
  `sealcount` int(10) UNSIGNED NULL DEFAULT 0,
  `disphyatk` int(10) UNSIGNED NULL DEFAULT 0,
  `dismagatk` int(10) UNSIGNED NULL DEFAULT 0,
  `disphydef` int(10) UNSIGNED NULL DEFAULT 0,
  `dismagdef` int(10) UNSIGNED NULL DEFAULT 0,
  `pvpdisphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `valuedscore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `setqlnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `gemstone` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `addpreciousbead` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `transferstone` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `recoScore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auction_cool_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“å†·å´æ—¶é—´æˆ³',
  `bead_extripe_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç æ‘˜é™¤æ¬¡æ•°',
  `bead_replace_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç ç½®æ¢æ¬¡æ•°',
  `equiptype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancetype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancefailed` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `enhancecount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auctionTransNum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `inscriptionHoles` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_broadcastmail
-- ----------------------------
DROP TABLE IF EXISTS `t_broadcastmail`;
CREATE TABLE `t_broadcastmail`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `sendtime` int(10) UNSIGNED NOT NULL,
  `title` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `content` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `items` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL,
  `sendername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `reason` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `platform` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `occu` smallint(5) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'èŒä¸š',
  `playerChargeNum` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è§’è‰²å……å€¼æ€»é‡‘é¢',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_charge_order
-- ----------------------------
DROP TABLE IF EXISTS `t_charge_order`;
CREATE TABLE `t_charge_order`  (
  `guid` bigint(10) UNSIGNED NOT NULL,
  `channel` varchar(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `orderid` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `status` tinyint(10) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `roleid` bigint(20) UNSIGNED NOT NULL,
  `mall_type` tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '商城类型',
  `charge_goods_id` int(10) UNSIGNED NOT NULL,
  `charge_item_id` int(10) UNSIGNED NOT NULL,
  `charge_item_num` int(10) UNSIGNED NOT NULL,
  `vip_score` int(10) UNSIGNED NOT NULL,
  `charge_money` int(10) UNSIGNED NOT NULL,
  `time` int(10) UNSIGNED NOT NULL,
  INDEX `orderid`(`orderid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_charge_record
-- ----------------------------
DROP TABLE IF EXISTS `t_charge_record`;
CREATE TABLE `t_charge_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `mall_type` tinyint(10) UNSIGNED NOT NULL,
  `goodsid` int(10) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `roleid` bigint(20) UNSIGNED NOT NULL,
  `today_times` int(10) UNSIGNED NOT NULL,
  `total_times` int(10) UNSIGNED NOT NULL,
  `update_time` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE,
  INDEX `roleid`(`roleid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_counter
-- ----------------------------
DROP TABLE IF EXISTS `t_counter`;
CREATE TABLE `t_counter`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `updatetime` int(10) UNSIGNED NOT NULL,
  `value` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE,
  INDEX `name`(`name`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_credit_point_record
-- ----------------------------
DROP TABLE IF EXISTS `t_credit_point_record`;
CREATE TABLE `t_credit_point_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `acc_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `get_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `total_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `today_tansfer` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `order_list` varchar(8192) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `acc_index`(`acc_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_currency_frozen
-- ----------------------------
DROP TABLE IF EXISTS `t_currency_frozen`;
CREATE TABLE `t_currency_frozen`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `item_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `frozen_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `reason` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_daily_todo
-- ----------------------------
DROP TABLE IF EXISTS `t_daily_todo`;
CREATE TABLE `t_daily_todo`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `data_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `day_progress` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `day_prog_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `week_progress` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `week_prog_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_dungeon
-- ----------------------------
DROP TABLE IF EXISTS `t_dungeon`;
CREATE TABLE `t_dungeon`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `dungeon_id` int(20) UNSIGNED NOT NULL,
  `num` int(20) UNSIGNED NOT NULL,
  `best_score` tinyint(20) UNSIGNED NOT NULL,
  `best_record_time` int(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_equip_scheme
-- ----------------------------
DROP TABLE IF EXISTS `t_equip_scheme`;
CREATE TABLE `t_equip_scheme`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `ownerId` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `schemeId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `weared` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `items` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_expedition_map
-- ----------------------------
DROP TABLE IF EXISTS `t_expedition_map`;
CREATE TABLE `t_expedition_map`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `mapid` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `start_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `duration` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `acc_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_expedition_member
-- ----------------------------
DROP TABLE IF EXISTS `t_expedition_member`;
CREATE TABLE `t_expedition_member`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `roleid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `mapid` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `occu` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `avatar` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `acc_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_figure_statue
-- ----------------------------
DROP TABLE IF EXISTS `t_figure_statue`;
CREATE TABLE `t_figure_statue`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `roleid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL,
  `avatar` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `statuetype` tinyint(3) UNSIGNED NOT NULL,
  `guildname` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `guildid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_fish
-- ----------------------------
DROP TABLE IF EXISTS `t_fish`;
CREATE TABLE `t_fish`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `herring` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `carp` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `doll` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `goldcarp` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_g_dungeon_hard
-- ----------------------------
DROP TABLE IF EXISTS `t_g_dungeon_hard`;
CREATE TABLE `t_g_dungeon_hard`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `account` int(10) UNSIGNED NOT NULL,
  `dungeon_id` int(10) UNSIGNED NOT NULL,
  `unlocked_hard_type` tinyint(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `account`(`account`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_g_gameparam
-- ----------------------------
DROP TABLE IF EXISTS `t_g_gameparam`;
CREATE TABLE `t_g_gameparam`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `value` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_g_punishment_copy
-- ----------------------------
DROP TABLE IF EXISTS `t_g_punishment_copy`;
CREATE TABLE `t_g_punishment_copy`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL,
  `duetime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_game_account
-- ----------------------------
DROP TABLE IF EXISTS `t_game_account`;
CREATE TABLE `t_game_account`  (
  `accid` int(10) UNSIGNED NOT NULL,
  `storagesize` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`accid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_gameparam
-- ----------------------------
DROP TABLE IF EXISTS `t_gameparam`;
CREATE TABLE `t_gameparam`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `value` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guid
-- ----------------------------
DROP TABLE IF EXISTS `t_guid`;
CREATE TABLE `t_guid`  (
  `nodetype` int(10) UNSIGNED NOT NULL,
  `nodeid` int(10) UNSIGNED NOT NULL,
  `seed` bigint(20) UNSIGNED NOT NULL,
  PRIMARY KEY (`nodetype`, `nodeid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild
-- ----------------------------
DROP TABLE IF EXISTS `t_guild`;
CREATE TABLE `t_guild`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `level` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `setuptime` int(10) UNSIGNED NOT NULL,
  `fund` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `declaration` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '0',
  `announcement` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `buildings` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `donate_logs` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `shopid` tinyint(10) UNSIGNED NOT NULL COMMENT '商店ID',
  `shop_refresh_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismiss_time` int(10) UNSIGNED NOT NULL,
  `popularoty` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `table` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `enroll_terrid` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `battle_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `occupy_terrid` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `inspire` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `win_probability` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `lose_probability` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `storage_add_post` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `storage_del_post` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enroll_player` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `occupy_cross_terrid` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `history_terrid` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `join_level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dungeon_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `prosperity_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `week_added_fund` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `merger_request_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `goal_guild_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `merger_request_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `merger_accept_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `merger_other_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `last_week_added_fund` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `last_update_guild_building_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_auction_item
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_auction_item`;
CREATE TABLE `t_guild_auction_item`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `reward_group` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `bid_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auction_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `guild_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `bid_role_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `vip_level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `occu` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `bid_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `state` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `cur_price` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `bid_price` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fix_price` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `add_price` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `end_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `is_bonus` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auction_item` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon`;
CREATE TABLE `t_guild_dungeon`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `dungeon_status` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地下城的状态',
  `start_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '状态结束时间戳',
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `week_status_index`(`dungeon_status`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon_battle
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon_battle`;
CREATE TABLE `t_guild_dungeon_battle`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `guild_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '公会ID',
  `dungeon_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地下城ID',
  `battle_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '战斗次数',
  `boss_odd_blood` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'boss剩余血量',
  `drop_buff` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '掉落的buff',
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `guild_dungeon_index`(`guild_id`, `dungeon_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon_info
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon_info`;
CREATE TABLE `t_guild_dungeon_info`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `guild_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '公会ID',
  `dungeon_type` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地下城类型',
  `total_damage` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '总伤害',
  `spend_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `guild_index`(`guild_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon_player_damage
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon_player_damage`;
CREATE TABLE `t_guild_dungeon_player_damage`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `guild_id` bigint(20) UNSIGNED NOT NULL,
  `player_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家Id',
  `player_name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '玩家名字',
  `damage_val` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家的伤害值',
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `guild_player_index`(`guild_id`, `player_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon_player_join
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon_player_join`;
CREATE TABLE `t_guild_dungeon_player_join`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `guild_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `player_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `guild_player_index`(`guild_id`, `player_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon_rand
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon_rand`;
CREATE TABLE `t_guild_dungeon_rand`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `dungeon_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dungeon_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_dungeon_reward
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_dungeon_reward`;
CREATE TABLE `t_guild_dungeon_reward`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 9,
  `player_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '已经领取奖励的玩家',
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `guild_player_index`(`player_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_member
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_member`;
CREATE TABLE `t_guild_member`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `guildid` bigint(20) UNSIGNED NOT NULL,
  `post` tinyint(3) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL,
  `vip_level` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `totalcontri` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `entertime` int(10) UNSIGNED NOT NULL,
  `offlinetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `leavetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `avatar` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `battle_number` int(3) UNSIGNED NOT NULL DEFAULT 0,
  `battle_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `battle_winstreak` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `battle_reward_mask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `battle_hp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `battle_mp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `battle_inspire` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `battle_lottery_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `active_degree` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `season_level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `emblem_level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `battle_terr_day_reward` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `guildid`(`guildid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guild_storage
-- ----------------------------
DROP TABLE IF EXISTS `t_guild_storage`;
CREATE TABLE `t_guild_storage`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `guildid` bigint(20) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL,
  `strengthen` tinyint(3) UNSIGNED NOT NULL,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `holenum` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `gemstone` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `param2` int(10) UNSIGNED NOT NULL,
  `deadline` int(10) UNSIGNED NOT NULL,
  `strfailed` smallint(5) UNSIGNED NOT NULL,
  `disphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `disphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addmagic` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `phyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `phydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strenth` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `stamina` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `intellect` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `spirit` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `qualitylv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealstate` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealcount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strenthnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `valuedscore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `setqlnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addpreciousbead` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `auction_cool_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“å†·å´æ—¶é—´æˆ³',
  `bead_extripe_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç æ‘˜é™¤æ¬¡æ•°',
  `bead_replace_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç ç½®æ¢æ¬¡æ•°',
  `equiptype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancetype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancefailed` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `enhancecount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auctionTransNum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `inscriptionHoles` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_guildstorage_oprecord
-- ----------------------------
DROP TABLE IF EXISTS `t_guildstorage_oprecord`;
CREATE TABLE `t_guildstorage_oprecord`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `guildid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `optype` int(10) UNSIGNED NOT NULL,
  `iteminfo` varchar(2048) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `time` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_halloffame
-- ----------------------------
DROP TABLE IF EXISTS `t_halloffame`;
CREATE TABLE `t_halloffame`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `hoftype` tinyint(3) UNSIGNED NOT NULL,
  `ranking` tinyint(3) UNSIGNED NOT NULL,
  `id` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sex` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `vip` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_head_frame
-- ----------------------------
DROP TABLE IF EXISTS `t_head_frame`;
CREATE TABLE `t_head_frame`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `head_frame_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `expire_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `owner_head_index`(`owner_id`, `head_frame_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_hire
-- ----------------------------
DROP TABLE IF EXISTS `t_hire`;
CREATE TABLE `t_hire`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `code` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `bindaccid` int(10) UNSIGNED NOT NULL,
  `identify` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `tasks` varchar(8192) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `bindtime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_honor_history
-- ----------------------------
DROP TABLE IF EXISTS `t_honor_history`;
CREATE TABLE `t_honor_history`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `date_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `total_honor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvp_info` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_honor_role
-- ----------------------------
DROP TABLE IF EXISTS `t_honor_role`;
CREATE TABLE `t_honor_role`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `honor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `honor_time` bigint(10) UNSIGNED NOT NULL DEFAULT 0,
  `honor_lvl` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `honor_exp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `last_week_rank` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `history_rank` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `highest_honor_lvl` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `is_use_guard_card` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_item
-- ----------------------------
DROP TABLE IF EXISTS `t_item`;
CREATE TABLE `t_item`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NOT NULL,
  `pack` tinyint(3) UNSIGNED NOT NULL,
  `grid` int(10) UNSIGNED NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL,
  `strengthen` tinyint(3) UNSIGNED NOT NULL,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `dayusenum` int(10) UNSIGNED NOT NULL,
  `addmagic` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `param2` int(10) UNSIGNED NOT NULL,
  `deadline` int(10) UNSIGNED NOT NULL,
  `strfailed` smallint(5) UNSIGNED NOT NULL,
  `phyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `phydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magdef` int(10) UNSIGNED NOT NULL,
  `strenth` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `stamina` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `intellect` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `spirit` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `qualitylv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealstate` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealcount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `disphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `disphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strenthnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `valuedscore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `setqlnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addpreciousbead` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `transferstone` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'é•¶åµŒè½¬ç§»çŸ³itemid',
  `recoScore` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '????????',
  `isLock` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è£…å¤‡é”',
  `auction_cool_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“å†·å´æ—¶é—´æˆ³',
  `bead_extripe_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç æ‘˜é™¤æ¬¡æ•°',
  `bead_replace_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç ç½®æ¢æ¬¡æ•°',
  `equiptype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancetype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancefailed` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `enhancecount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auctionTransNum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `inscriptionHoles` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`, `accid`, `pack`) USING BTREE,
  INDEX `accid,pack`(`accid`, `pack`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_item_deposit
-- ----------------------------
DROP TABLE IF EXISTS `t_item_deposit`;
CREATE TABLE `t_item_deposit`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `data_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strengthen` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `quality_lv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `equip_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dungeon_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner_index`(`owner_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_item_presents
-- ----------------------------
DROP TABLE IF EXISTS `t_item_presents`;
CREATE TABLE `t_item_presents`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `item_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `playerA_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `send_num_A` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `playerB_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `send_num_B` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_jar_record
-- ----------------------------
DROP TABLE IF EXISTS `t_jar_record`;
CREATE TABLE `t_jar_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `jar_id` int(10) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `item_id` int(10) UNSIGNED NOT NULL,
  `num` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `jar_id`(`jar_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_jaritem_pool
-- ----------------------------
DROP TABLE IF EXISTS `t_jaritem_pool`;
CREATE TABLE `t_jaritem_pool`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `jartype` int(10) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `occus` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `missnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `getnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `minusfixnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_bindgoldconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_bindgoldconsume`;
CREATE TABLE `t_log_bindgoldconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_goldalarm
-- ----------------------------
DROP TABLE IF EXISTS `t_log_goldalarm`;
CREATE TABLE `t_log_goldalarm`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_goldconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_goldconsume`;
CREATE TABLE `t_log_goldconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_ironstoneconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_ironstoneconsume`;
CREATE TABLE `t_log_ironstoneconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_itemconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_itemconsume`;
CREATE TABLE `t_log_itemconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_itemobtain
-- ----------------------------
DROP TABLE IF EXISTS `t_log_itemobtain`;
CREATE TABLE `t_log_itemobtain`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_loginout
-- ----------------------------
DROP TABLE IF EXISTS `t_log_loginout`;
CREATE TABLE `t_log_loginout`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_mail
-- ----------------------------
DROP TABLE IF EXISTS `t_log_mail`;
CREATE TABLE `t_log_mail`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_petconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_petconsume`;
CREATE TABLE `t_log_petconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_pointconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_pointconsume`;
CREATE TABLE `t_log_pointconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_log_silverconsume
-- ----------------------------
DROP TABLE IF EXISTS `t_log_silverconsume`;
CREATE TABLE `t_log_silverconsume`  (
  `guid` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `accid` int(10) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `playername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `timestamp` int(10) UNSIGNED NOT NULL,
  `params` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `playerid`(`playerid`) USING BTREE,
  INDEX `timestamp`(`timestamp`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_lottery_rec
-- ----------------------------
DROP TABLE IF EXISTS `t_lottery_rec`;
CREATE TABLE `t_lottery_rec`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `price` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `gettime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mail
-- ----------------------------
DROP TABLE IF EXISTS `t_mail`;
CREATE TABLE `t_mail`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `senderid` bigint(20) UNSIGNED NOT NULL,
  `senderaccid` int(10) UNSIGNED NOT NULL,
  `sendername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `sendtime` int(10) UNSIGNED NOT NULL,
  `deadline` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '截至日期',
  `title` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `content` varchar(2048) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `mailtype` tinyint(3) UNSIGNED NOT NULL,
  `status` tinyint(3) UNSIGNED NOT NULL,
  `replytimes` tinyint(3) UNSIGNED NOT NULL,
  `silver` int(10) UNSIGNED NOT NULL,
  `items` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `reason` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mailitem
-- ----------------------------
DROP TABLE IF EXISTS `t_mailitem`;
CREATE TABLE `t_mailitem`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `mailid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL,
  `strengthen` tinyint(3) UNSIGNED NOT NULL,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `holenum` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `gemstone` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `param2` int(10) UNSIGNED NOT NULL,
  `deadline` int(10) UNSIGNED NOT NULL,
  `strfailed` smallint(5) UNSIGNED NOT NULL,
  `disphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `disphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addmagic` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `phyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `phydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strenth` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `stamina` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `intellect` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `spirit` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `qualitylv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealstate` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealcount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdisphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pvpdismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `strenthnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `valuedscore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `setqlnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fashionattrnum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `addpreciousbead` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `transferstone` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `recoScore` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auction_cool_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹å–è¡Œäº¤æ˜“å†·å´æ—¶é—´æˆ³',
  `bead_extripe_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç æ‘˜é™¤æ¬¡æ•°',
  `bead_replace_cnt` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å®ç ç½®æ¢æ¬¡æ•°',
  `equiptype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancetype` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `enhancefailed` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `enhancecount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auctionTransNum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `inscriptionHoles` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mall
-- ----------------------------
DROP TABLE IF EXISTS `t_mall`;
CREATE TABLE `t_mall`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL,
  `subtype` tinyint(3) UNSIGNED NOT NULL,
  `jobtype` tinyint(3) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `groupnum` int(10) UNSIGNED NOT NULL,
  `price` int(10) UNSIGNED NOT NULL,
  `disprice` int(10) UNSIGNED NOT NULL,
  `moneytype` tinyint(3) UNSIGNED NOT NULL,
  `limittype` tinyint(3) UNSIGNED NOT NULL,
  `limitnum` smallint(5) UNSIGNED NOT NULL,
  `giftpackitems` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `giftpackicon` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `limtstarttime` int(10) UNSIGNED NOT NULL,
  `limitendtime` int(10) UNSIGNED NOT NULL,
  `limitetotlenum` smallint(5) UNSIGNED NOT NULL,
  `vipscore` smallint(5) UNSIGNED NOT NULL,
  `giftpackname` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `tagtype` tinyint(3) UNSIGNED NOT NULL,
  `sort` int(10) UNSIGNED NOT NULL,
  `hotsort` int(10) UNSIGNED NOT NULL,
  `goods_type` tinyint(3) UNSIGNED NOT NULL,
  `goods_subtype` tinyint(3) UNSIGNED NOT NULL,
  `is_recommend` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mall_acc_buy_record
-- ----------------------------
DROP TABLE IF EXISTS `t_mall_acc_buy_record`;
CREATE TABLE `t_mall_acc_buy_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `mall_item_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `buyed_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `refresh_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `update_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mall_gift_pack
-- ----------------------------
DROP TABLE IF EXISTS `t_mall_gift_pack`;
CREATE TABLE `t_mall_gift_pack`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `roleid` bigint(20) UNSIGNED NOT NULL,
  `gift_pack_id` int(10) UNSIGNED NOT NULL,
  `activate_start_time` int(20) UNSIGNED NOT NULL,
  `activate_end_time` int(20) UNSIGNED NOT NULL,
  `rest_num` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `activate_state` tinyint(3) UNSIGNED NOT NULL,
  `activate_cond` tinyint(3) UNSIGNED NOT NULL,
  `activate_counter` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `activate_sub_cond` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `activate_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mall_item
-- ----------------------------
DROP TABLE IF EXISTS `t_mall_item`;
CREATE TABLE `t_mall_item`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `mall_itemid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `roleid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `start_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `end_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `roleid`(`roleid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_marriage
-- ----------------------------
DROP TABLE IF EXISTS `t_marriage`;
CREATE TABLE `t_marriage`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL,
  `sex` tinyint(3) UNSIGNED NOT NULL,
  `gift` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `loverId` bigint(20) UNSIGNED NOT NULL,
  `loverName` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `loverOccu` tinyint(3) UNSIGNED NOT NULL,
  `loverSex` tinyint(3) UNSIGNED NOT NULL,
  `loverGift` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `serial` tinyint(3) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL,
  `hascruised` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `hasrited` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `haddinner` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_marriage_guest
-- ----------------------------
DROP TABLE IF EXISTS `t_marriage_guest`;
CREATE TABLE `t_marriage_guest`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `guestId` bigint(20) UNSIGNED NOT NULL,
  `guestName` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `serial` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_mastersect_relation
-- ----------------------------
DROP TABLE IF EXISTS `t_mastersect_relation`;
CREATE TABLE `t_mastersect_relation`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL COMMENT 'å…³ç³»æ‹¥æœ‰è€…',
  `type` tinyint(3) UNSIGNED NOT NULL COMMENT 'ç±»åž‹(1:å¸ˆå‚…, 2:å¸ˆå…„å¼Ÿ, 3:å¾’å¼Ÿ)',
  `id` bigint(20) UNSIGNED NOT NULL COMMENT 'å¯¹æ–¹id',
  `accid` int(10) UNSIGNED NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL,
  `viplv` tinyint(3) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `addtime` int(10) UNSIGNED NOT NULL,
  `isFinSch` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ˜¯å¦å‡ºå¸ˆ',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_merchant_rec
-- ----------------------------
DROP TABLE IF EXISTS `t_merchant_rec`;
CREATE TABLE `t_merchant_rec`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `playerid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `itemnum` int(10) UNSIGNED NOT NULL,
  `merchantlv` tinyint(3) UNSIGNED NOT NULL,
  `gettime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_merge_give_namecard
-- ----------------------------
DROP TABLE IF EXISTS `t_merge_give_namecard`;
CREATE TABLE `t_merge_give_namecard`  (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_new_title
-- ----------------------------
DROP TABLE IF EXISTS `t_new_title`;
CREATE TABLE `t_new_title`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `title_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `duetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_new_title_accout
-- ----------------------------
DROP TABLE IF EXISTS `t_new_title_accout`;
CREATE TABLE `t_new_title_accout`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `title_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `duetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_offline_event
-- ----------------------------
DROP TABLE IF EXISTS `t_offline_event`;
CREATE TABLE `t_offline_event`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `type` smallint(5) UNSIGNED NOT NULL,
  `time` int(10) UNSIGNED NOT NULL,
  `data` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_pasture
-- ----------------------------
DROP TABLE IF EXISTS `t_pasture`;
CREATE TABLE `t_pasture`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL DEFAULT 1,
  `exp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dailyexp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fetchWaterList` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `wateringTimes` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `unloadTime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `events` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `grid1` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid2` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid3` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid4` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid5` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid6` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid7` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid8` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `grid9` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_pet
-- ----------------------------
DROP TABLE IF EXISTS `t_pet`;
CREATE TABLE `t_pet`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `dataid` int(10) UNSIGNED NOT NULL,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `level` smallint(5) UNSIGNED NOT NULL DEFAULT 1,
  `exp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `hunger` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `skill_index` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `gold_feed_count` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `point_feed_count` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `select_skill_count` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_pk_statistic
-- ----------------------------
DROP TABLE IF EXISTS `t_pk_statistic`;
CREATE TABLE `t_pk_statistic`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL COMMENT '玩家角色ID',
  `type` tinyint(10) UNSIGNED NOT NULL COMMENT 'pk类型',
  `total_win_num` int(10) UNSIGNED NOT NULL COMMENT '总胜场',
  `total_lose_num` int(10) UNSIGNED NOT NULL COMMENT '总负场',
  `total_num` int(10) UNSIGNED NOT NULL COMMENT '总场数',
  `detail_record_info` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '对战不同职业的胜负信息，编码方式：occu1:win:lose:total|occu2:win:lose:total',
  `recent_record` char(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '近期战绩，记录最近10场的结果，编码格式：01001，0代表输，1代表赢，2代表平局，最前面的最早的那场',
  `max_win_streak` int(10) UNSIGNED NOT NULL COMMENT '最多连胜场数',
  `cur_win_streak` int(10) UNSIGNED NOT NULL COMMENT '当前连胜场数',
  `cur_lose_streak` int(10) NOT NULL DEFAULT 0 COMMENT '当前连败场数',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_player_info
-- ----------------------------
DROP TABLE IF EXISTS `t_player_info`;
CREATE TABLE `t_player_info`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL DEFAULT 1,
  `occu` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `pre_occu` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `exp` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `blessexp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `sex` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `zoneid` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `tourzone` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `viplvl` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `vipexp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `vipduetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `createtime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `savetime` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `deletetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `localsavetime` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `onlinetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pkmode` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `evil` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `hp` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `gold` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `point` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `bindgold` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `bindpoint` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `totlechargenum` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `totleonlinetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `honour` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `sceneid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `mapid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `pos` varchar(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `dir` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `skills` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `pvp_skills` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `buffs` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `taskmask` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `storymask` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `story` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `pettranslist` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `petsoulval` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `petsoul` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `petexplore` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `petfighting` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `options` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `itemcd` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `copytimes` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `packsize` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `storagesize` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `routine` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `giftbag` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `autohook` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `escortinfo` varchar(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `titlemask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `titletime` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `title` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `tribeskill` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `actionflag` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `mallbuy` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `signin` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `babelinfo` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `updatebulletin` int(10) UNSIGNED NOT NULL DEFAULT 100,
  `twsk` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `darktower` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `revivecost` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `revivetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `continuousdays` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `funcflag` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `mount` varchar(320) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `feed` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `actioncount` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `itemlock` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `expretrieve` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `wing` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `dujie` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `equipshop` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `dragonmark` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `fashion` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `growthscene` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `sp` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `pvp_sp` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `awaken` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `skillbar` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `pvp_skillbar` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `itembar` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `pkvalue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fatigue` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `warrior_soul` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `warpstone` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '次元石',
  `match_score` int(10) UNSIGNED NOT NULL DEFAULT 1000 COMMENT '匹配积分',
  `retinuefighting` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '跟随随从',
  `pk_coin` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `avatar` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `revive_coin` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `offretinuelist` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `newboot` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `tower_wipeout_end_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `guild_contri` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `bootflag` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `redpoint` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `offlinemailtime` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `daily_task_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `daily_task_reward_mask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `wudao_status` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `forbid_talk_duetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `login_ip` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `custom_field` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `season_level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `season_star` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `season_exp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'èµ›å­£ç»éªŒ',
  `season_play_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `season_id` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'èµ›å­£æ®µä½',
  `season_change_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'èµ›å­£æ®µä½',
  `season_uplevel_records` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `season_attr` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `season_king_mark_count` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `goldjar_point` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magjar_point` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auction_refresh_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `auction_addition_booth` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `platform` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `openid` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `season_attr_end_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'èµ›å­£å±žæ€§ç»“æŸæ—¶é—´',
  `last_reset_gjarpoint` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `last_reset_mjarpoint` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `month_card_expire_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `read_notify` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `potion_set` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `pet_battle` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å‡ºæˆ˜å® ç‰©',
  `pet_follow` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è·Ÿéšå® ç‰©',
  `born_zoneid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `is_recv_disciple` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `master_note` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `is_check_sp` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sky_fashion_get` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'èŽ·å¾—å¤©ç©ºå¥—æœ‰æŸè®°å½•bitå­—æ®µ',
  `daychargenum` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ—¥å……å€¼è®¡æ•°',
  `achievement_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `achievement_task_reward_mask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `weapon_bar` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `is_appointment_occu` tinyint(10) UNSIGNED NOT NULL,
  `money_manage_status` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `money_manage_reward_mask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `score_war_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `score_war_battle_count` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `score_war_reward_mask` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `score_war_last_battle_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `return_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `return_level` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `return_first_login` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `academic_total_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¾’å¼Ÿå­¦ä¸šæˆé•¿æ€»å€¼',
  `master_dailytask_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆé—¨æ—¥å¸¸ä»»åŠ¡æˆé•¿æ€»å€¼',
  `master_academictask_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆé—¨å­¦ä¸šæˆé•¿ä»»åŠ¡æˆé•¿æ€»å€¼',
  `master_uplevel_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆé—¨å‡çº§æˆé•¿æ€»å€¼',
  `master_giveequip_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆé—¨èµ é€è£…å¤‡æˆé•¿æ€»å€¼',
  `master_givegift_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆé—¨èµ é€ç¤¼åŒ…æˆé•¿æ€»å€¼',
  `master_teamcleardungeon_growth` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆå¾’ç»„é˜Ÿé€šè¿‡åœ°ä¸‹åŸŽæˆé•¿å€¼',
  `good_teacher_value` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'è‰¯å¸ˆå€¼',
  `apprenticmaster_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ‹œå¸ˆæƒ©ç½šæ—¶é—´æˆ³',
  `recruitdisciple_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ”¶å¾’æƒ©ç½šæ—¶é—´æˆ³',
  `fin_sch_disciple_sum` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å‡ºå¸ˆå¾’å¼Ÿæ•°é‡',
  `score_war_win_battle_count` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `show_fashion_weapon` tinyint(3) UNSIGNED NOT NULL DEFAULT 1 COMMENT 'æ˜¯å¦æ˜¾ç¤ºæ—¶è£…æ­¦å™¨',
  `role_value_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `game_set` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `novice_guide_choose_flag` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ–°æ‰‹å¼•å¯¼é€‰æ‹©æ ‡å¿—',
  `head_frame_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `new_title_guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `chiji_score` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `chiji_score_reset_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `equal_pvp_skills` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å…¬å¹³ç«žæŠ€åœºæŠ€èƒ½',
  `equal_pvp_skillbar` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'å…¬å¹³ç«žæŠ€åœºæŠ€èƒ½æ ',
  `add_preferences_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `del_preferences_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE,
  INDEX `name`(`name`) USING BTREE,
  INDEX `openid`(`openid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_player_info_name
-- ----------------------------
DROP TABLE IF EXISTS `t_player_info_name`;
CREATE TABLE `t_player_info_name`  (
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `guid` bigint(20) UNSIGNED NOT NULL,
  `time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`name`) USING BTREE,
  INDEX `guid`(`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_player_joins
-- ----------------------------
DROP TABLE IF EXISTS `t_player_joins`;
CREATE TABLE `t_player_joins`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `param1` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `param2` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `join_players` varchar(4200) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_premium_league_battle
-- ----------------------------
DROP TABLE IF EXISTS `t_premium_league_battle`;
CREATE TABLE `t_premium_league_battle`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `race_id` bigint(20) UNSIGNED NOT NULL,
  `type` tinyint(10) UNSIGNED NOT NULL,
  `role_id1` bigint(20) UNSIGNED NOT NULL,
  `role_name1` char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `role_occu1` tinyint(10) UNSIGNED NOT NULL,
  `role_id2` bigint(20) UNSIGNED NOT NULL,
  `role_name2` char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `role_occu2` tinyint(10) UNSIGNED NOT NULL,
  `is_end` tinyint(10) UNSIGNED NOT NULL,
  `winner` bigint(20) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_premium_league_gamer
-- ----------------------------
DROP TABLE IF EXISTS `t_premium_league_gamer`;
CREATE TABLE `t_premium_league_gamer`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `occu` tinyint(10) UNSIGNED NOT NULL,
  `level` smallint(10) UNSIGNED NOT NULL,
  `enroll_time` int(10) UNSIGNED NOT NULL,
  `enroll_count` tinyint(10) UNSIGNED NOT NULL,
  `remain_hp` int(10) UNSIGNED NOT NULL,
  `remain_mp` int(10) UNSIGNED NOT NULL,
  `win_num` tinyint(10) UNSIGNED NOT NULL,
  `win_time` int(10) UNSIGNED NOT NULL,
  `final_battle_win_num` tinyint(10) UNSIGNED NOT NULL,
  `ranking` int(10) UNSIGNED NOT NULL,
  `next_roll_pos` int(10) UNSIGNED NOT NULL,
  `battle_flag` tinyint(10) UNSIGNED NOT NULL,
  `is_finish_roll_battle` tinyint(10) UNSIGNED NOT NULL,
  `score` int(10) UNSIGNED NOT NULL,
  `preliminay_reward` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_punishment
-- ----------------------------
DROP TABLE IF EXISTS `t_punishment`;
CREATE TABLE `t_punishment`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL,
  `duetime` int(10) UNSIGNED NOT NULL,
  `param` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `accid`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_questionnaire
-- ----------------------------
DROP TABLE IF EXISTS `t_questionnaire`;
CREATE TABLE `t_questionnaire`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `activeTimeType` tinyint(3) UNSIGNED NOT NULL,
  `masterType` tinyint(3) UNSIGNED NOT NULL,
  `regionId` tinyint(3) UNSIGNED NOT NULL,
  `declaration` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_red_packet
-- ----------------------------
DROP TABLE IF EXISTS `t_red_packet`;
CREATE TABLE `t_red_packet`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `owner_id` bigint(20) UNSIGNED NOT NULL,
  `owner_name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `owner_level` smallint(10) UNSIGNED NOT NULL DEFAULT 0,
  `owner_occu` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `type` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `status` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `reason` smallint(10) UNSIGNED NOT NULL DEFAULT 0,
  `cond1` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `total_money` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `remain_money` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `num` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `status_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `cond2` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner_id`(`owner_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_red_packet_receiver
-- ----------------------------
DROP TABLE IF EXISTS `t_red_packet_receiver`;
CREATE TABLE `t_red_packet_receiver`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `receiver_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `reciver_accid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `red_packet_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `occu` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `level` smallint(10) UNSIGNED NOT NULL DEFAULT 0,
  `money_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `money` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `red_packet_owner_name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `is_best` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `receiver_id`(`receiver_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_relation
-- ----------------------------
DROP TABLE IF EXISTS `t_relation`;
CREATE TABLE `t_relation`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `type` tinyint(3) UNSIGNED NOT NULL,
  `id` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `seasonlv` int(10) UNSIGNED NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL,
  `giveresttime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `daygiftnum` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `viplv` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `addtime` int(10) UNSIGNED NOT NULL,
  `intimacy` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `onlinenotify` tinyint(3) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `master_giveresttime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `day_master_giftnum` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `remark` varchar(48) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT 'çŽ©å®¶å¤‡æ³¨',
  `offline_time` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'çŽ©å®¶ç¦»çº¿æ—¶é—´',
  `daily_mastertask_state` tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å¸ˆé—¨æ—¥å¸¸ä»»åŠ¡çŠ¶æ€',
  `mark` smallint(5) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'å…³ç³»çŠ¶æ€æ ‡å¿—',
  `head_frame` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `guild_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `return_year_title` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE,
  INDEX `id`(`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_replay_fighter
-- ----------------------------
DROP TABLE IF EXISTS `t_replay_fighter`;
CREATE TABLE `t_replay_fighter`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `raceid` bigint(20) UNSIGNED NOT NULL,
  `roleid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `level` smallint(10) UNSIGNED NOT NULL,
  `occu` tinyint(10) UNSIGNED NOT NULL,
  `season_level` int(10) UNSIGNED NOT NULL,
  `season_stars` tinyint(10) UNSIGNED NOT NULL,
  `pos` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `raceid`(`raceid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_replay_file
-- ----------------------------
DROP TABLE IF EXISTS `t_replay_file`;
CREATE TABLE `t_replay_file`  (
  `guid` bigint(20) NOT NULL,
  `data` mediumblob NULL
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_replay_info
-- ----------------------------
DROP TABLE IF EXISTS `t_replay_info`;
CREATE TABLE `t_replay_info`  (
  `guid` bigint(20) UNSIGNED NOT NULL COMMENT '比赛ID',
  `race_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `version` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `record_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `evaluation` tinyint(10) UNSIGNED NOT NULL DEFAULT 0,
  `result` tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '战斗结果（1对于2）',
  `score` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '积分',
  `view_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '观看次数',
  `ref_num` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '被引用次数（被不同的列表所引用）',
  `race_type` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `raceid_index`(`race_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_replay_list
-- ----------------------------
DROP TABLE IF EXISTS `t_replay_list`;
CREATE TABLE `t_replay_list`  (
  `guid` bigint(20) NOT NULL,
  `ownerid` bigint(20) UNSIGNED NOT NULL,
  `type` tinyint(10) UNSIGNED NOT NULL COMMENT '列表类型（1：自己的对战，2：高手对决，3：我的收藏）',
  `raceid` bigint(20) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `type_owner`(`type`, `ownerid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_retinue
-- ----------------------------
DROP TABLE IF EXISTS `t_retinue`;
CREATE TABLE `t_retinue`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `dataid` int(10) UNSIGNED NOT NULL,
  `skills` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `createtime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `level` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `starlevel` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `weights` varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_shop
-- ----------------------------
DROP TABLE IF EXISTS `t_shop`;
CREATE TABLE `t_shop`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `shopid` int(10) UNSIGNED NOT NULL,
  `refreshtime` bigint(20) UNSIGNED NOT NULL,
  `week_refreshtime` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ¯å‘¨åˆ·æ–°æ—¶é—´ç‚¹',
  `month_refreshtime` bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'æ¯æœˆåˆ·æ–°æ—¶é—´ç‚¹',
  `refreshnum` int(10) UNSIGNED NOT NULL,
  `version` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `guid`(`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_shopitem
-- ----------------------------
DROP TABLE IF EXISTS `t_shopitem`;
CREATE TABLE `t_shopitem`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `shopid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `shopitemid` int(10) UNSIGNED NOT NULL,
  `grid` tinyint(3) UNSIGNED NOT NULL,
  `restnum` smallint(5) NOT NULL,
  `viplv` tinyint(3) UNSIGNED NOT NULL,
  `vipdiscount` int(10) UNSIGNED NOT NULL,
  `lease_end_timestamp` int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ç§Ÿèµç»“æŸæ—¶é—´æˆ³',
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `guid`(`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_shortcut_key
-- ----------------------------
DROP TABLE IF EXISTS `t_shortcut_key`;
CREATE TABLE `t_shortcut_key`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `type` int(10) UNSIGNED NOT NULL,
  `value` varchar(2048) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`, `type`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_sortlist
-- ----------------------------
DROP TABLE IF EXISTS `t_sortlist`;
CREATE TABLE `t_sortlist`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `id` bigint(20) UNSIGNED NOT NULL,
  `sorttype` tinyint(3) UNSIGNED NOT NULL,
  `name` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `level` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `occu` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sex` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `vip` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `ownerid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owneraccid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ownername` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `value1` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `value2` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `value3` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `update_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_storage
-- ----------------------------
DROP TABLE IF EXISTS `t_storage`;
CREATE TABLE `t_storage`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `accid` int(10) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NOT NULL,
  `pack` tinyint(3) UNSIGNED NOT NULL,
  `grid` tinyint(3) UNSIGNED NOT NULL,
  `phyatk` int(10) UNSIGNED NOT NULL,
  `magatk` int(10) UNSIGNED NOT NULL,
  `phydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `magdef` int(10) UNSIGNED NOT NULL,
  `strenth` int(10) UNSIGNED NOT NULL,
  `stamina` int(10) UNSIGNED NOT NULL,
  `intellect` int(10) UNSIGNED NOT NULL,
  `spirit` int(10) UNSIGNED NOT NULL,
  `qualitylv` tinyint(3) UNSIGNED NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL,
  `strengthen` tinyint(3) UNSIGNED NOT NULL,
  `randattr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `holenum` tinyint(3) UNSIGNED NOT NULL,
  `gemstone` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `rune` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `param2` int(10) UNSIGNED NOT NULL,
  `deadline` int(10) UNSIGNED NOT NULL,
  `strfailed` smallint(5) UNSIGNED NOT NULL,
  `sealstate` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `sealcount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `disphyatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagatk` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `disphydef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `dismagdef` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`accid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_sys_record
-- ----------------------------
DROP TABLE IF EXISTS `t_sys_record`;
CREATE TABLE `t_sys_record`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `behavior` int(10) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `num` int(10) UNSIGNED NOT NULL,
  `record_time` int(10) UNSIGNED NOT NULL,
  `owner_type` int(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_task
-- ----------------------------
DROP TABLE IF EXISTS `t_task`;
CREATE TABLE `t_task`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `dataid` int(10) UNSIGNED NOT NULL,
  `status` tinyint(3) UNSIGNED NOT NULL,
  `parameter` varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `finished_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `submit_count` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_team_copy
-- ----------------------------
DROP TABLE IF EXISTS `t_team_copy`;
CREATE TABLE `t_team_copy`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `day_num` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `team_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `day_quit_team_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `day_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `week_num` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `week_quit_team_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `week_time` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `model_pass` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `day_flop` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `week_flop` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `extra_num` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner_index`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_team_copy_consume
-- ----------------------------
DROP TABLE IF EXISTS `t_team_copy_consume`;
CREATE TABLE `t_team_copy_consume`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `zone_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `team_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `item_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `consume_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `create_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner_index`(`owner_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tmp
-- ----------------------------
DROP TABLE IF EXISTS `t_tmp`;
CREATE TABLE `t_tmp`  (
  `roleid` bigint(20) UNSIGNED NOT NULL,
  `openid` char(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `old_accid` int(10) UNSIGNED NOT NULL,
  `new_accid` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`roleid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribe
-- ----------------------------
DROP TABLE IF EXISTS `t_tribe`;
CREATE TABLE `t_tribe`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `level` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `setuptime` int(10) UNSIGNED NOT NULL,
  `impeachtime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `impeachid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `color` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `fund` int(11) NOT NULL DEFAULT 0,
  `wood` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `beastSoul` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `metal` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ranking` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `icon` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `qq` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `yy` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `announce` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `totem` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `folkhouse` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `college` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `workshop` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `treasure` tinyint(3) UNSIGNED NOT NULL DEFAULT 1,
  `tribeshop` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `bosslist` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `transtime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `transstate` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `transinfo` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `dismisstime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `robsouldaily` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `uniontribe` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `unioncd` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `animal` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `moneytree` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `pickchest` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribe_item
-- ----------------------------
DROP TABLE IF EXISTS `t_tribe_item`;
CREATE TABLE `t_tribe_item`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `itemid` int(10) UNSIGNED NOT NULL,
  `num` smallint(5) UNSIGNED NOT NULL,
  `bind` tinyint(3) UNSIGNED NOT NULL,
  `pack` tinyint(3) UNSIGNED NOT NULL,
  `quality` tinyint(3) UNSIGNED NOT NULL,
  `strengthen` tinyint(3) UNSIGNED NOT NULL,
  `randattr` char(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `holenum` tinyint(3) UNSIGNED NOT NULL,
  `gemstone` char(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `rune` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `param1` int(10) UNSIGNED NOT NULL,
  `param2` int(10) UNSIGNED NOT NULL,
  `deadline` int(10) UNSIGNED NOT NULL,
  `price` int(10) UNSIGNED NOT NULL,
  `strfailed` smallint(5) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribe_log
-- ----------------------------
DROP TABLE IF EXISTS `t_tribe_log`;
CREATE TABLE `t_tribe_log`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL,
  `logtype` smallint(5) UNSIGNED NOT NULL,
  `content` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `logtime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE,
  INDEX `owner`(`owner`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribe_member
-- ----------------------------
DROP TABLE IF EXISTS `t_tribe_member`;
CREATE TABLE `t_tribe_member`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `sex` tinyint(3) UNSIGNED NOT NULL,
  `occu` tinyint(3) UNSIGNED NOT NULL,
  `tribe` bigint(20) UNSIGNED NOT NULL,
  `post` tinyint(3) UNSIGNED NOT NULL,
  `level` smallint(5) UNSIGNED NOT NULL,
  `vip` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `contribution` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `totalcontri` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `power` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `entertime` int(10) UNSIGNED NOT NULL,
  `leavetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `offlinetime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `tribeshopbuy` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  `translot` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `feedanimalstate` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  `tribemoneytree` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribe_name
-- ----------------------------
DROP TABLE IF EXISTS `t_tribe_name`;
CREATE TABLE `t_tribe_name`  (
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `guid` bigint(20) UNSIGNED NOT NULL,
  PRIMARY KEY (`name`) USING BTREE,
  INDEX `guid`(`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribe_territory
-- ----------------------------
DROP TABLE IF EXISTS `t_tribe_territory`;
CREATE TABLE `t_tribe_territory`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `territory` tinyint(3) UNSIGNED NOT NULL,
  `owner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `secowner` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `challenger` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `secChal` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `aassist` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `dassist` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `warstarttime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `warstatus` tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_tribewar_bid
-- ----------------------------
DROP TABLE IF EXISTS `t_tribewar_bid`;
CREATE TABLE `t_tribewar_bid`  (
  `guid` bigint(20) UNSIGNED NOT NULL,
  `territory` tinyint(3) UNSIGNED NOT NULL,
  `tribe` bigint(20) UNSIGNED NOT NULL,
  `player` bigint(20) UNSIGNED NOT NULL,
  `num` int(10) UNSIGNED NOT NULL,
  `bidtime` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Table structure for t_week_sign
-- ----------------------------
DROP TABLE IF EXISTS `t_week_sign`;
CREATE TABLE `t_week_sign`  (
  `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  `act_type` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `act_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `week_time` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `week_num` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `week_id` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  `week_box` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`) USING BTREE,
  UNIQUE INDEX `owner_act_type_index`(`owner_id`, `act_type`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Event structure for event_clearbmmails
-- ----------------------------
DROP EVENT IF EXISTS `event_clearbmmails`;
delimiter ;;
CREATE EVENT `event_clearbmmails`
ON SCHEDULE
EVERY '600' SECOND STARTS '2021-12-18 17:30:09'
ON COMPLETION PRESERVE
DO DELETE FROM `t_broadcastmail` WHERE UNIX_TIMESTAMP() > `sendtime` + 3456000
;
;;
delimiter ;

-- ----------------------------
-- Event structure for event_clearmails
-- ----------------------------
DROP EVENT IF EXISTS `event_clearmails`;
delimiter ;;
CREATE EVENT `event_clearmails`
ON SCHEDULE
EVERY '600' SECOND STARTS '2021-12-18 17:30:09'
ON COMPLETION PRESERVE
DO DELETE FROM `t_mail` WHERE UNIX_TIMESTAMP() > `sendtime` + 3456000
;
;;
delimiter ;

-- ----------------------------
-- Event structure for event_cleartribelog
-- ----------------------------
DROP EVENT IF EXISTS `event_cleartribelog`;
delimiter ;;
CREATE EVENT `event_cleartribelog`
ON SCHEDULE
EVERY '600' SECOND STARTS '2021-12-19 20:40:52'
ON COMPLETION PRESERVE
DO DELETE FROM `t_tribe_log` WHERE UNIX_TIMESTAMP() > `logtime` + 1728000
;
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
