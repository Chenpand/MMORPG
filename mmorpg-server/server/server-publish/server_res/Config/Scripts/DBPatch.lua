--require "DbConfig"

DBHandler = {}
DBHandler.__index = DBHandler

function DBHandler:new( config )
	local conn = Sys:CreateDBHandler( config.host, config.port, config.user, config.passwd, config.dbname )
	if not conn then
		print("DBHandler:new  Sys:CreateDBHandler fail ")
		return
	end
	
	if not conn:Connect() then
		Sys:CloseDBHandler(conn);
		return 
	end
	
	local o = {}
	setmetatable( o, DBHandler )
	o.conn = conn
	
	return o
end

--查询操作 有数据返回true 否则返回false
function DBHandler:TryQuery( sql )
	return self.conn:TryQuery(sql)
end

--增删,修改操作 
function DBHandler:Update( sql )
	return self.conn:Update(sql)
end

function DBHandler:Close()
	Sys:CloseDBHandler(self.conn);
	self.conn = nil;
end

DBPatch = {
	DBHandlers = {};   --保存各个库对应的sql操作对象
	schedulers = {},	--- 保存表结构
	
}

function DBPatch.init()
	--[[local dbHandler =  DBHandler:new(DbConfig.game)
	if  dbHandler then
		DBPatch.DBHandlers.game = dbHandler
	end]]
	
	--本地数据库
	local local_host = Sys:GetLocalDbHost()
	local local_port =  Sys:GetLocalDbPort()
	local local_user =  Sys:GetLocalDbUser()
	local local_passwd =  Sys:GetLocalDbPasswd()
	local local_dbname =  Sys:GetLocalDbName()
	local dbHandler =  DBHandler:new({host = local_host, port = local_port, user = local_user, passwd = local_passwd, dbname = local_dbname})
	if  dbHandler then
		DBPatch.DBHandlers.localdb = dbHandler
	end
	
	DBPatch.schedulers.t_testpatch = [[CREATE TABLE if not exists t_testpatch  (
		guid int(11) UNSIGNED NOT NULL COMMENT "唯一id",
		PRIMARY KEY (guid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;]]
	
	DBPatch.schedulers.t_abnormal_transaction = [[CREATE TABLE `t_abnormal_transaction` (
  `guid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `handle_status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '状态',
  `create_time` int(10) unsigned NOT NULL DEFAULT '0',
  `buyer` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '买家',
  `seller` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '卖家',
  `transaction_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易时间',
  `transaction_id` varchar(64) NOT NULL DEFAULT '',
  `item_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '物品实例id',
  `item_data_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '物品id',
  `item_num` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '物品数量',
  `money_type` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '货币类型:\r\n0-无效,\r\n1-金币,\r\n2-点卷,\r\n3-经验\r\n11-勇者之魂\r\n12-决斗币\r\n13-复活币\r\n14-公会贡献\r\n21-以物换物',
  `transaction_amount` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易金额',
  `frozen_amount` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '冻结金额',
  `back_amount` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '保释金',
  `pre_frozen_record_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '前一个冻结记录id',
  PRIMARY KEY (`guid`),
  KEY `status+seller` (`handle_status`,`seller`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8 COMMENT='拍卖';]]

	DBPatch.schedulers.t_mastersect_relation = [[CREATE TABLE `t_mastersect_relation` (
		`guid`  bigint(20) UNSIGNED NOT NULL ,
		`owner`  bigint(20) UNSIGNED NOT NULL COMMENT '关系拥有者' ,
		`type`  tinyint(3) UNSIGNED NOT NULL COMMENT '类型(1:师傅, 2:师兄弟, 3:徒弟)' ,
		`id`  bigint(20) UNSIGNED NOT NULL COMMENT '对方id' ,
		`accid`  int(10) UNSIGNED NOT NULL ,
		`level`  smallint(5) UNSIGNED NOT NULL ,
		`occu`  tinyint(3) UNSIGNED NOT NULL ,
		`viplv`  tinyint(3) UNSIGNED NOT NULL ,
		`name`  varchar(32) NOT NULL ,
		`addtime`  int(10) UNSIGNED NOT NULL ,
		PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

	DBPatch.schedulers.t_questionnaire = [[CREATE TABLE `t_questionnaire`  (
		`guid`  bigint(20) UNSIGNED NOT NULL ,
		`owner`  bigint(20) UNSIGNED NOT NULL ,
		`activeTimeType`  tinyint(3) UNSIGNED NOT NULL ,
		`masterType`  tinyint(3) UNSIGNED NOT NULL ,
		`regionId`  tinyint(3) UNSIGNED NOT NULL ,
		`declaration`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
		PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]


	DBPatch.schedulers.t_guild_dungeon = [[CREATE TABLE `t_guild_dungeon` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`dungeon_status`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地下城的状态' ,
`start_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '状态结束时间戳' ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `week_status_index` (`dungeon_status`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact;]]


	DBPatch.schedulers.t_guild_dungeon_battle = [[CREATE TABLE `t_guild_dungeon_battle` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`guild_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '公会ID' ,
`dungeon_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地下城ID' ,
`battle_cnt`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '战斗次数' ,
`boss_odd_blood`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'boss剩余血量' ,
`drop_buff`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '掉落的buff' ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `guild_dungeon_index` (`guild_id`, `dungeon_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8mb4 COLLATE=utf8mb4_general_ci
ROW_FORMAT=Compact;]]


	DBPatch.schedulers.t_guild_dungeon_info = [[CREATE TABLE `t_guild_dungeon_info` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`guild_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '公会ID' ,
`dungeon_type`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地下城类型' ,
`total_damage`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '总伤害' ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `guild_index` (`guild_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact;]]


	DBPatch.schedulers.t_guild_dungeon_player_damage = [[CREATE TABLE `t_guild_dungeon_player_damage` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`guild_id`  bigint(20) UNSIGNED NOT NULL ,
`player_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家Id' ,
`player_name`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '玩家名字' ,
`damage_val`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家的伤害值' ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `guild_player_index` (`guild_id`, `player_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact;]]
	
	
	DBPatch.schedulers.t_guild_dungeon_reward = [[CREATE TABLE `t_guild_dungeon_reward` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 9 ,
`player_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '已经领取奖励的玩家' ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `guild_player_index` (`player_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact;]]

DBPatch.schedulers.t_guild_dungeon_player_join = [[CREATE TABLE `t_guild_dungeon_player_join` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`guild_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`player_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `guild_player_index` (`guild_id`, `player_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact;]]
	
	DBPatch.schedulers.t_account_shop_acc_buy_record = [[CREATE TABLE `t_account_shop_acc_buy_record` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`owner`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`shop_item_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`buyed_num`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`refresh_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`create_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`update_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
INDEX `accid` (`owner`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]
	
	DBPatch.schedulers.t_account_shop_item = [[CREATE TABLE `t_account_shop_item` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`shop_itemid`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`owner`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`create_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`update_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
INDEX `accid` (`owner`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]
	
	DBPatch.schedulers.t_account_shop_role_buy_record = [[CREATE TABLE `t_account_shop_role_buy_record` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`owner`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`shop_item_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`buyed_num`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`refresh_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`create_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`update_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
INDEX `roleid` (`owner`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]
	
	DBPatch.schedulers.t_activity_op_attribute = [[CREATE TABLE `t_activity_op_attribute` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`owner`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`activity_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`parameter`  varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' ,
`create_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`update_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`refresh_time`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
INDEX `roleid` (`owner`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]

DBPatch.schedulers.t_auction_transaction = [[CREATE TABLE `t_auction_transaction` (
`guid` bigint(20) unsigned NOT NULL COMMENT '拍卖行珍品交易订单主键id',
  `auction_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 't_auction_new主键id',
  `is_treas` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `seller` bigint(20) unsigned NOT NULL DEFAULT '0',
  `buyer` bigint(20) unsigned NOT NULL DEFAULT '0',
  `transaction_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易时间',
  `item_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拍卖行交易物品对象id',
  `item_data_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '拍卖行交易物品dataid',
  `item_num` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '拍卖行交易物品数量',
  `item_qualitylv` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '品级',
  `item_strengthen` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '强化等级',
  `item_bead_buffId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '道具是宝珠的时候记录附加buffid',
  `item_bead_averPrice` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易装备时附加宝珠平均交易价格',
  `item_magic_averPrice` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易装备时附魔卡平均交易价格',
  `item_name` varchar(32) NOT NULL,
  `buy_num` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '购买数量',
  `item_score` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易物品评分',
  `money_type` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '货币类型',
  `transaction_amount` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易金额',
  `counter_fee` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '手续费',
  `transaction_extra_profit` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '交易额外获利',
  `deposit` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '上架押金',
  `unit_price` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '单价',
  `mail_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '买家领取的邮件id',
  `verify_end_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '审核结束时间',
  `program_audit_result` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '程序审核结果',
  `program_audit_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '程序审核时间',
  `punishA_id` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '审核结束时',
  `punishB_id` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '审核结束时，对应装备或道具被使用消耗或销毁或属性发生改变',
  `punishC_id` tinyint(3) unsigned NOT NULL DEFAULT '100' COMMENT '人工审核结果',
  `punished_id` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '最终选择处罚id',
  `item_returned` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否归还道具',
  `state` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '状态0未处理1处理中2无异常处理3异常违规处理4处理超时',
  PRIMARY KEY (`guid`),
  INDEX `verify_end_time` (`verify_end_time`) USING BTREE,
  INDEX `state` (`state`) USING BTREE
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]
	
	DBPatch.schedulers.t_mall_item = [[CREATE TABLE `t_mall_item` (
  `guid` bigint(20) unsigned NOT NULL,
  `mall_itemid` int(10) unsigned NOT NULL DEFAULT '0',
  `roleid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `create_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `start_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `end_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]
	
	DBPatch.schedulers.t_mall_acc_buy_record = [[CREATE TABLE `t_mall_acc_buy_record` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner` int(10) unsigned NOT NULL DEFAULT '0',
  `mall_item_id` int(10) unsigned NOT NULL DEFAULT '0',
  `buyed_num` int(10) unsigned NOT NULL DEFAULT '0',
  `refresh_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `create_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `update_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `accid` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_auction_attention = [[CREATE TABLE `t_auction_attention` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`owner_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`auction_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`delete_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_blackmarket_auction = [[CREATE TABLE `t_blackmarket_auction` (
  `guid` bigint(20) unsigned NOT NULL,
  `op_activity_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '所属运营活动id',
  `back_buy_item_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '回购的物品id',
  `back_buy_type` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '收购类型',
  `back_buy_num` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '收购数量',
  `fixed_price` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '固定收购价格',
  `recommend_price` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '推荐价格',
  `begin_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍开始时间',
  `end_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍结束时间',
  `off_sale` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否下架0:否，1:是',
  `is_settled` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否已经结算',
  `is_sys_cancel` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否系统自动取消的',
  PRIMARY KEY (`guid`),
  KEY `is_settled` (`is_settled`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_blackmarket_auction_price = [[CREATE TABLE `t_blackmarket_auction_price` (
  `guid` bigint(20) unsigned NOT NULL,
  `item_type_id` int(10) unsigned NOT NULL,
  `price` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍平均价格',
  `auction_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍项id',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_blackmarket_transaction = [[CREATE TABLE `t_blackmarket_transaction` (
  `guid` bigint(20) unsigned NOT NULL,
  `create_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `opact_main_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '所属主活动id',
  `opact_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '所属活动id',
  `auction_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍项guid',
  `item_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍的物品id',
  `auctioner_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍玩家guid',
  `auctioner_name` varchar(32) NOT NULL,
  `auctioner_accid` int(10) unsigned NOT NULL DEFAULT '0',
  `auctioner_viplv` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '玩家vip等级',
  `auction_type` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍类型',
  `auction_money_type` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍货币类型',
  `auction_price` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍价格',
  `auction_result` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '竞拍结果, 0:失败,1:成功',
  `state` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '订单处理状态 0:未处理, 1:已经处理',
  `money_sended` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '发放金币标志 0:未发,1:已发放',
  `item_returned` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '道具退还标志 0:未退还,1:已退还',
  `item_src_guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `item_name` varchar(32) NOT NULL,
  `delete_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间',
  `itemid` int(10) unsigned NOT NULL,
  `num` smallint(5) unsigned NOT NULL,
  `bind` tinyint(3) unsigned DEFAULT NULL,
  `phyatk` int(10) unsigned DEFAULT '0',
  `magatk` int(10) unsigned DEFAULT '0',
  `magdef` int(10) unsigned DEFAULT NULL,
  `phydef` int(10) unsigned DEFAULT '0',
  `strenth` int(10) unsigned DEFAULT '0',
  `stamina` int(10) unsigned DEFAULT '0',
  `intellect` int(10) unsigned DEFAULT '0',
  `spirit` int(10) unsigned DEFAULT '0',
  `qualitylv` tinyint(3) unsigned DEFAULT '0',
  `quality` tinyint(3) unsigned DEFAULT NULL,
  `strengthen` tinyint(3) unsigned DEFAULT NULL,
  `randattr` varchar(255) DEFAULT NULL,
  `dayusenum` int(10) unsigned NOT NULL,
  `strenthnum` int(10) unsigned NOT NULL DEFAULT '0',
  `addmagic` varchar(32) DEFAULT NULL,
  `param1` int(10) unsigned DEFAULT NULL,
  `param2` int(10) unsigned DEFAULT NULL,
  `deadline` int(10) unsigned DEFAULT NULL,
  `strfailed` smallint(5) unsigned DEFAULT NULL,
  `sealstate` tinyint(3) unsigned DEFAULT '0',
  `sealcount` int(10) unsigned DEFAULT '0',
  `disphyatk` int(10) unsigned DEFAULT '0',
  `dismagatk` int(10) unsigned DEFAULT '0',
  `disphydef` int(10) unsigned DEFAULT '0',
  `dismagdef` int(10) unsigned DEFAULT '0',
  `pvpdisphyatk` int(10) unsigned NOT NULL DEFAULT '0',
  `pvpdismagatk` int(10) unsigned NOT NULL DEFAULT '0',
  `pvpdisphydef` int(10) unsigned NOT NULL DEFAULT '0',
  `pvpdismagdef` int(10) unsigned NOT NULL DEFAULT '0',
  `valuedscore` int(10) unsigned NOT NULL DEFAULT '0',
  `setqlnum` int(10) unsigned NOT NULL DEFAULT '0',
  `fashionattrid` int(10) unsigned NOT NULL DEFAULT '0',
  `fashionattrnum` int(10) unsigned NOT NULL DEFAULT '0',
  `gemstone` varchar(64) DEFAULT NULL,
  `addpreciousbead` varchar(128) NOT NULL DEFAULT '',
  `transferstone` int(10) unsigned NOT NULL DEFAULT '0',
  `recoScore` int(10) unsigned NOT NULL DEFAULT '0',
  `auction_cool_timestamp` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '拍卖行交易冷却时间戳',
  `bead_extripe_cnt` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '宝珠摘除次数',
  `bead_replace_cnt` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '宝珠置换次数',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_activity_account_record = [[CREATE TABLE `t_activity_account_record` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`acc_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`activity_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`cur_num`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`states`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `acc_activity_index` (`acc_id`, `activity_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact;]]

DBPatch.schedulers.t_account_counter = [[CREATE TABLE `t_account_counter` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner` int(10) unsigned NOT NULL DEFAULT '0',
  `counter_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `counter_num` bigint(20) unsigned NOT NULL DEFAULT '0',
  `create_time` int(10) unsigned NOT NULL DEFAULT '0',
  `refresh_time` bigint(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `accid` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_expedition_map = [[CREATE TABLE `t_expedition_map` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`accid`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`mapid`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
`start_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`duration`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`status`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
INDEX `accid` (`accid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]

DBPatch.schedulers.t_expedition_member = [[CREATE TABLE `t_expedition_member` (
`guid`  bigint(20) UNSIGNED NOT NULL ,
`accid`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`roleid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`mapid`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
`occu`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
`avatar`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' ,
`create_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
INDEX `accid` (`accid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]

DBPatch.schedulers.t_account_task = [[CREATE TABLE `t_account_task` (
  `guid` bigint(20) unsigned NOT NULL,
  `accid` int(10) unsigned NOT NULL,
  `dataid` int(10) unsigned NOT NULL,
  `status` tinyint(3) unsigned NOT NULL,
  `parameter` varchar(512) NOT NULL DEFAULT '',
  `finished_time` int(10) unsigned NOT NULL,
  `submit_count` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`guid`),
  INDEX `accid` (`accid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]

DBPatch.schedulers.t_guild_auction_item = [[CREATE TABLE `t_guild_auction_item` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`auction_type`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`guild_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`bid_role_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`bid_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`state`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`cur_price`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`bid_price`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`fix_price`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`add_price`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`end_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`auction_item`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
PRIMARY KEY (`guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]

DBPatch.schedulers.t_head_frame = [[CREATE TABLE `t_head_frame` (
`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`owner_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`head_frame_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`expire_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
PRIMARY KEY (`guid`),
UNIQUE INDEX `owner_head_index` (`owner_id`, `head_frame_id`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
ROW_FORMAT=Compact
;]]

DBPatch.schedulers.t_new_title = [[CREATE TABLE `t_new_title` (
  `guid`  bigint(20) UNSIGNED NOT NULL DEFAULT '0',
  `create_time`  int(10) UNSIGNED NOT NULL DEFAULT '0',
  `owner`  bigint(20) UNSIGNED NOT NULL DEFAULT '0',
  `title_id`  int(10) UNSIGNED NOT NULL DEFAULT '0',
  `duetime`  int(10) UNSIGNED NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  INDEX `owner` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_new_title_accout = [[CREATE TABLE `t_new_title_accout` (
  `guid`  bigint(20) UNSIGNED NOT NULL DEFAULT '0',
  `create_time`  int(10) UNSIGNED NOT NULL DEFAULT '0',
  `owner`  bigint(20) UNSIGNED NOT NULL DEFAULT '0',
  `title_id`  int(10) UNSIGNED NOT NULL DEFAULT '0',
  `duetime`  int(10) UNSIGNED NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  INDEX `owner` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_guild_dungeon_rand = [[CREATE TABLE `t_guild_dungeon_rand` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `dungeon_type` int(10) unsigned NOT NULL DEFAULT '0',
  `dungeon_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_item_deposit = [[CREATE TABLE `t_item_deposit` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `data_id` int(10) unsigned NOT NULL DEFAULT '0',
  `num` int(10) unsigned NOT NULL DEFAULT '0',
  `strengthen` int(10) unsigned NOT NULL DEFAULT '0',
  `quality_lv` int(10) unsigned NOT NULL DEFAULT '0',
  `create_time` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `owner_index` (`owner_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_week_sign = [[CREATE TABLE `t_week_sign` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `act_type` int(10) unsigned NOT NULL DEFAULT '0',
  `act_id` int(10) unsigned NOT NULL DEFAULT '0',
  `week_time` int(10) unsigned NOT NULL DEFAULT '0',
  `week_num` int(10) unsigned NOT NULL DEFAULT '0',
  `week_id` varchar(64) CHARACTER SET utf8mb4 NOT NULL DEFAULT '',
  `week_box` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`),
  UNIQUE KEY `owner_act_type_index` (`owner_id`,`act_type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_player_joins = [[CREATE TABLE `t_player_joins` (
  `guid`  bigint(20) UNSIGNED NOT NULL ,
  `create_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
  `type`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
  `param1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
  `param2`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
  `join_players`  varchar(4200) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_activity_op_account_task = [[CREATE TABLE `t_activity_op_account_task` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `task_id` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `parameter` varchar(512) NOT NULL DEFAULT '',
  `create_time` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `owner_id` (`owner_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;]]

DBPatch.schedulers.t_activity_op_record = [[CREATE TABLE `t_activity_op_record` (
  `guid` bigint(20) unsigned NOT NULL,
  `behavior` int(10) unsigned NOT NULL,
  `owner` bigint(20) unsigned NOT NULL,
  `param1` int(10) unsigned NOT NULL,
  `num` int(10) unsigned NOT NULL,
  `record_time` int(10) unsigned NOT NULL,
  `owner_type` int(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `owner` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;]]

DBPatch.schedulers.t_daily_todo = [[CREATE TABLE `t_daily_todo` (
	`guid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
	`owner_id`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
	`data_id`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
	`day_progress`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
	`day_prog_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
	`week_progress`  int(10) UNSIGNED NOT NULL DEFAULT 0,
	`week_prog_time`  int(10) UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`guid`),
	INDEX `owner` (`owner_id`) USING BTREE 
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_team_copy = [[CREATE TABLE `t_team_copy` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `day_num` int(10) unsigned NOT NULL DEFAULT '0',
  `day_quit_team_num` int(10) unsigned NOT NULL DEFAULT '0',
  `day_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `week_num` int(10) unsigned NOT NULL DEFAULT '0',
  `week_quit_team_num` int(10) unsigned NOT NULL DEFAULT '0',
  `week_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `model_pass` varchar(255) NOT NULL,
  `day_flop` varchar(255) NOT NULL,
  `week_flop` varchar(255) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_account_adventure = [[CREATE TABLE `t_account_adventure` (
	`guid` bigint(20) unsigned NOT NULL,
	`accid` int(10) unsigned NOT NULL,
	`level` int(10) unsigned NOT NULL DEFAULT '1',
	`exp` int(10) unsigned NOT NULL DEFAULT '0',
	`passtype` tinyint(3) unsigned NOT NULL DEFAULT '0',
	`normalreward` varchar(1024) NOT NULL DEFAULT '',
	`highreward` varchar(1024) NOT NULL DEFAULT '',
	`activity` tinyint(3) unsigned NOT NULL DEFAULT '0',
	`unlock` tinyint(3) unsigned NOT NULL DEFAULT '0',
	`seasonid` int(10) unsigned NOT NULL DEFAULT '0',
	`extraexp` tinyint(3) unsigned NOT NULL DEFAULT '0',
	PRIMARY KEY (`guid`)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

  DBPatch.schedulers.t_hire = [[CREATE TABLE `t_hire` (
	`guid` bigint(20) unsigned NOT NULL,
	`accid` int(10) unsigned NOT NULL,
	`code` varchar(128) NOT NULL DEFAULT '',
	`bindaccid` int(10) unsigned NOT NULL,
	`identify` int(10) unsigned NOT NULL DEFAULT '0',
	`tasks` varchar(8192) NOT NULL DEFAULT '',
	`bindtime` int(10) unsigned NOT NULL,
	PRIMARY KEY (`guid`)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]  


  DBPatch.schedulers.t_account_monopoly = [[CREATE TABLE `t_account_monopoly` (
	`guid` bigint(20) unsigned NOT NULL,
	`accid` int(10) unsigned NOT NULL,
	`turn` int(10) unsigned NOT NULL,
	`step` int(10) unsigned NOT NULL,
	`remain_roll` int(10) unsigned NOT NULL,
	`roll_times` int(10) unsigned NOT NULL,
	`coin` int(10) unsigned NOT NULL,
	`buff` int(10) unsigned NOT NULL,
	`cards` varchar(512) NOT NULL DEFAULT '',
	`exchanged` varchar(512) NOT NULL DEFAULT '',
	PRIMARY KEY (`guid`)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]  



DBPatch.schedulers.t_currency_frozen = [[CREATE TABLE `t_currency_frozen` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `item_id` int(10) unsigned NOT NULL DEFAULT '0',
  `num` int(10) unsigned NOT NULL DEFAULT '0',
  `frozen_time` int(10) unsigned NOT NULL DEFAULT '0',
  `reason` varchar(255) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_item_presents = [[CREATE TABLE `t_item_presents` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `item_id` int(10) unsigned NOT NULL DEFAULT '0',
  `playerA_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `send_num_A` int(10) unsigned NOT NULL DEFAULT '0',
  `playerB_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `send_num_B` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_equip_scheme = [[CREATE TABLE `t_equip_scheme` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `ownerId` bigint(20) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `schemeId` int(10) unsigned NOT NULL DEFAULT '0',
  `weared` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `items` varchar(1024) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`),
  INDEX `ownerId` (`ownerId`) USING BTREE 
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_honor_history = [[CREATE TABLE `t_honor_history` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `owner` bigint(20) unsigned NOT NULL DEFAULT '0',
  `date_type` int(10) unsigned NOT NULL DEFAULT '0',
  `total_honor` int(10) unsigned NOT NULL DEFAULT '0',
  `pvp_info` varchar(255) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_honor_role = [[CREATE TABLE `t_honor_role` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `honor` int(10) unsigned NOT NULL DEFAULT '0',
  `honor_time` bigint(10) unsigned NOT NULL DEFAULT '0',
  `honor_lvl` int(10) unsigned NOT NULL DEFAULT '0',
  `honor_exp` int(10) unsigned NOT NULL DEFAULT '0',
  `last_week_rank` int(10) unsigned NOT NULL DEFAULT '0',
  `history_rank` int(10) unsigned NOT NULL DEFAULT '0',
  `highest_honor_lvl` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_shortcut_key = [[CREATE TABLE `t_shortcut_key` (
  `guid` bigint(20) unsigned NOT NULL,
  `owner` bigint(20) unsigned NOT NULL,
  `type` int(10) unsigned NOT NULL,
  `value` varchar(2048) NOT NULL,
  PRIMARY KEY (`guid`),
  KEY `owner` (`owner`,`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_lost_dungeon = [[CREATE TABLE `t_lost_dungeon` (
  `guid`  bigint(20) UNSIGNED NOT NULL ,
  `owner`  bigint(20) UNSIGNED NOT NULL ,
  `create_time`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
  `hp`  int(10) NOT NULL DEFAULT 0 ,
  `mp`  int(10) NOT NULL DEFAULT 0 ,
  `score`  int(10) NOT NULL ,
  `floors`  varchar(1024) NULL ,
  PRIMARY KEY (`guid`),
  INDEX `owner` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_credit_point_record = [[CREATE TABLE `t_credit_point_record` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `acc_id` int(10) unsigned NOT NULL DEFAULT '0',
  `get_time` int(10) unsigned NOT NULL DEFAULT '0',
  `total_num` int(10) unsigned NOT NULL DEFAULT '0',
  `transfer_num` int(10) unsigned NOT NULL DEFAULT '0',
  `cost_num` int(10) unsigned NOT NULL DEFAULT '0',
  `order_list` varchar(1024) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

DBPatch.schedulers.t_guild_event = [[CREATE TABLE `t_guild_event` (
  `guid` bigint(20) unsigned NOT NULL,
  `guild_id` bigint(20) unsigned NOT NULL,
  `add_time` int(10) unsigned NOT NULL,
  `event_info` varchar(1024) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;]]

end

function DBPatch.closeHandlers()
	for _, handler in pairs(DBPatch.DBHandlers) do 
		if handler then
			handler:Close()
		end
	end
end

function DBPatch.getDBHandler( dbName )
	if not dbName then return DBPatch.DBHandlers.localdb end
	
	return DBPatch.DBHandlers[dbName]
end

--- 添加一列
function DBPatch.addColumn( tableName, columnName, restriction, dbName )
	local handler = DBPatch.getDBHandler(dbName)
	if not handler then return end
	
	if DBPatch.isColumnExist( handler, tableName, columnName ) then return true end
	local sql = string.format( "alter table %s add column %s %s;", tableName, columnName, restriction )
	local flag = handler:Update( sql )
	return flag
end

--- 指定表名和其中一列,如果不存在表或者表中不存在此列，则创建或重建表
function DBPatch.addTable( tableName, primaryKey, dbName )
	local handler = DBPatch.getDBHandler(dbName)
	if not handler then return end
	
	if DBPatch.isTableExist( handler, tableName ) and 
		DBPatch.isColumnExist( handler, tableName, primaryKey ) then return true end
	
	DBPatch.dropTableByHandler( handler, tableName )
	
	local scheduler = DBPatch.schedulers[tableName]
	if not scheduler then return false end

	local flag = handler:Update( scheduler )
	return flag
end

function DBPatch.execute( sql, dbName )
	local handler = DBPatch.getDBHandler( dbName )
	if not handler then return end

	local flag = handler:Update( sql )
	return flag 
end

function DBPatch.dropColumn( tableName, columnName, dbName )
	local handler = DBPatch.getDBHandler( dbName )
	if not handler then return end
	
	if not DBPatch.isColumnExist( handler, tableName, columnName ) then return true end

	local sql = string.format( "alter table %s drop column %s;", tableName, columnName )
	local flag = handler:Update( sql )
	return flag
end

function DBPatch.dropTable( tableName, dbName )
	local handler = DBPatch.getDBHandler( dbName )
	if not handler then return end
	
	local sql = string.format( "drop table if exists %s;", tableName )

	handler:Update( sql )
end

function DBPatch.dropTableByHandler( dbHandler, tableName )
	local sql = string.format( "drop table if exists %s;", tableName )

	dbHandler:Update( sql )
end

function DBPatch.isTableExist( dbHandler, tableName )
	local sql = string.format( "show tables like '%s';", tableName )
	local flag = dbHandler:TryQuery( sql )

	return flag
end

function DBPatch.isColumnExist( dbHandler, tableName, columnName )
	local sql = string.format( "DESCRIBE %s %s;", tableName, columnName )
	local flag = dbHandler:TryQuery( sql )

	return flag
end

-- 数据库操作字段
function DBPatch.patch()
	--测试语句
	--[[DBPatch.dropTable("t_testpatch")
	DBPatch.addTable("t_testpatch", "guid")
	DBPatch.addColumn("t_testpatch", "column1", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_testpatch", "column2", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_testpatch", "column3", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_testpatch", "column3", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_testpatch", "column4", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.execute("alter table t_testpatch modify column column2 int(11) unsigned not null DEFAULT 0")
	DBPatch.dropColumn("t_testpatch", "column2")]]
	
	DBPatch.dropColumn("t_account", "money_manage_flag")
	DBPatch.dropColumn("t_player_info", "money_manage_flag")
	DBPatch.dropColumn("t_account", "money_manage_role")
	DBPatch.dropColumn("t_account", "money_manage_reward_mask")
	DBPatch.addColumn("t_account", "money_manage_status", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_player_info", "money_manage_status", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_player_info", "money_manage_reward_mask", "varchar(32) NOT NULL DEFAULT ''")
	DBPatch.addColumn("t_task", "submit_count", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.execute("ALTER TABLE `t_auction_recommend_price` MODIFY COLUMN `recent_price`  varchar(5120) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	DBPatch.addColumn("t_auction_new", "ip", "varchar(32) NOT NULL DEFAULT ''")
	DBPatch.addColumn("t_auction_new", "device_id", "varchar(128) NOT NULL DEFAULT ''")
	DBPatch.addTable("t_abnormal_transaction", "guid")
	DBPatch.addColumn("t_player_info", "academic_total_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '徒弟学业成长总值'")
	DBPatch.addColumn("t_player_info", "master_dailytask_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师门日常任务成长总值' AFTER `academic_total_growth`") 
	DBPatch.addColumn("t_player_info", "master_academictask_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师门学业成长任务成长总值'  AFTER `master_dailytask_growth`")
	DBPatch.addColumn("t_player_info", "master_uplevel_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师门升级成长总值' AFTER `master_academictask_growth`")
	DBPatch.addColumn("t_player_info", "master_giveequip_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师门赠送装备成长总值' AFTER `master_uplevel_growth`")
	DBPatch.addColumn("t_player_info", "master_givegift_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师门赠送礼包成长总值' AFTER `master_giveequip_growth` ")
	DBPatch.addColumn("t_player_info", "master_teamcleardungeon_growth", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师徒组队通过地下城成长值' AFTER `master_givegift_growth` ")
	DBPatch.addColumn("t_player_info", "good_teacher_value", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '良师值' AFTER `master_teamcleardungeon_growth` ")
	DBPatch.addColumn("t_player_info", "apprenticmaster_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '拜师惩罚时间戳' AFTER `good_teacher_value` ")
	DBPatch.addColumn("t_player_info", "recruitdisciple_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '收徒惩罚时间戳' AFTER `apprenticmaster_timestamp` ")
	DBPatch.addColumn("t_player_info", "fin_sch_disciple_sum", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '出师徒弟数量' AFTER `recruitdisciple_timestamp` ")
	DBPatch.addColumn("t_relation", "daily_mastertask_state", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '师门日常任务状态'")
	DBPatch.addTable("t_mastersect_relation", "guid")
	DBPatch.addTable("t_questionnaire", "guid")
	DBPatch.addColumn("t_player_info", "score_war_score", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_player_info", "score_war_battle_count", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_player_info", "score_war_win_battle_count", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_player_info", "score_war_reward_mask", "varchar(32) NOT NULL DEFAULT ''")
	DBPatch.addColumn("t_player_info", "score_war_last_battle_time", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_relation", "remark", "varchar(32) NOT NULL DEFAULT '' COMMENT '玩家备注'")
	--DBPatch.addColumn("t_relation", "isRegress", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否是回归玩家'")
	DBPatch.addColumn("t_relation", "offline_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家离线时间'")
	DBPatch.dropColumn("t_relation", "isRegress")
	DBPatch.addColumn("t_account", "veteran_return_time", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_account", "veteran_return_role", "bigint(20) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_mastersect_relation", "isFinSch", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否出师'")
	DBPatch.addColumn("t_player_info", "return_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家回归时间'")
	DBPatch.addColumn("t_relation", "mark", "smallint(5) UNSIGNED NOT NULL DEFAULT 0 COMMENT '关系状态标志'")

	DBPatch.addColumn("t_account", "security_passwd", "varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '安全锁密码' AFTER `veteran_return_role`")
	DBPatch.addColumn("t_account", "security_freeze_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '安全账户冻结时间' AFTER `security_passwd`")
	DBPatch.addColumn("t_account", "security_unfreeze_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '安全账户解冻时间' AFTER `security_freeze_time`")
	DBPatch.addColumn("t_account", "common_device", "varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '常用设备' AFTER `security_unfreeze_time`")
	DBPatch.addColumn("t_account", "security_passwd_error_num", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '安全锁密码错误次数' AFTER `common_device`")
	DBPatch.addColumn("t_item", "isLock", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '装备锁' AFTER `recoScore`")
	
	DBPatch.addColumn("t_account", "acc_offline_time", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '账号离线时间' AFTER `security_passwd_error_num`")
	
	DBPatch.addColumn("t_player_info", "show_fashion_weapon", "tinyint(3) UNSIGNED NOT NULL DEFAULT 1 COMMENT '是否显示时装武器'")
	
	DBPatch.execute("ALTER TABLE `t_item` MODIFY COLUMN `addpreciousbead`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	DBPatch.execute("ALTER TABLE `t_mailitem` MODIFY COLUMN `addpreciousbead`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	DBPatch.execute("ALTER TABLE `t_auction_new` MODIFY COLUMN `addpreciousbead`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	DBPatch.execute("ALTER TABLE `t_guild_storage` MODIFY COLUMN `addpreciousbead`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")

	DBPatch.execute("ALTER TABLE `t_player_info` MODIFY COLUMN `sp`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	DBPatch.execute("ALTER TABLE `t_player_info` MODIFY COLUMN `pvp_sp`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	
	DBPatch.execute("ALTER TABLE `t_player_info` MODIFY COLUMN `skills`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	DBPatch.execute("ALTER TABLE `t_player_info` MODIFY COLUMN `pvp_skills`  varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	
	DBPatch.addColumn("t_item", "auction_cool_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '拍卖行交易冷却时间戳'")
	DBPatch.addColumn("t_mailitem", "auction_cool_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '拍卖行交易冷却时间戳'")
	DBPatch.addColumn("t_auction_new", "auction_cool_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '拍卖行交易冷却时间戳'")
	DBPatch.addColumn("t_guild_storage", "auction_cool_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '拍卖行交易冷却时间戳'")
	
	DBPatch.addColumn("t_account", "weapon_lease_tickets", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '武器租赁好运符数量'")
	DBPatch.addColumn("t_shopitem", "lease_end_timestamp", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '租赁结束时间戳'")
	
	DBPatch.execute("ALTER TABLE `t_figure_statue` ADD COLUMN `guildid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `guildname`")
	DBPatch.addTable("t_guild_dungeon", "guid")
	DBPatch.addTable("t_guild_dungeon_battle", "guid")
	DBPatch.addTable("t_guild_dungeon_info", "guid")
	DBPatch.addTable("t_guild_dungeon_player_damage", "guid")
	DBPatch.addTable("t_guild_dungeon_reward", "guid")

	DBPatch.addColumn("t_account", "gnome_coin_num", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地精纪念币数量'")
	DBPatch.addColumn("t_account", "gnome_coin_refresh_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '地精纪念币刷新时间'")
	DBPatch.addTable("t_account_shop_acc_buy_record", "guid")
	DBPatch.addTable("t_account_shop_item", "guid")
	DBPatch.addTable("t_account_shop_role_buy_record", "guid")
	
	DBPatch.addColumn("t_auction_new", "on_public_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '物品拍卖公示开始时间戳'")
	DBPatch.addColumn("t_auction_new", "ownerVipLev", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '上架者vip等级'")
	--DBPatch.dropTable("t_auction_transaction");
	DBPatch.addTable("t_auction_transaction", "guid");
	DBPatch.addColumn("t_auction_recommend_price", "bead_buff_id", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠附加属性id'")
	DBPatch.addColumn("t_auction_recommend_price", "price_update_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '平均价格刷新时间'")
	DBPatch.addColumn("t_auction_recommend_price", "visibility_price", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家可见平均价格'")
	DBPatch.addColumn("t_auction_recommend_price", "vis_price_update_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '可见平均价格更新时间戳'")
	DBPatch.addColumn("t_auction_new", "coupon_strengthen_to", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '强化卷强化加等级'")
	
	DBPatch.addColumn("t_auction_new", "bead_extripe_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠摘除次数'")
	DBPatch.addColumn("t_item", "bead_extripe_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠摘除次数'")
	DBPatch.addColumn("t_guild_storage", "bead_extripe_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠摘除次数'")
	DBPatch.addColumn("t_mailitem", "bead_extripe_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠摘除次数'")
	
	DBPatch.addColumn("t_auction_new", "bead_replace_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠置换次数'")
	DBPatch.addColumn("t_item", "bead_replace_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠置换次数'")
	DBPatch.addColumn("t_guild_storage", "bead_replace_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠置换次数'")
	DBPatch.addColumn("t_mailitem", "bead_replace_cnt", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '宝珠置换次数'")
	
	DBPatch.addColumn("t_abnormal_transaction", "freeze_object", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '冻结对象'")
	DBPatch.addColumn("t_abnormal_transaction", "back_way", "tinyint(3) UNSIGNED NOT NULL DEFAULT 3 COMMENT '冻结金返还途径'")
	DBPatch.addColumn("t_abnormal_transaction", "frozen_permanent", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否永久冻结'")
	DBPatch.addColumn("t_abnormal_transaction", "treasure_flag", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否珍品'")
	DBPatch.addColumn("t_abnormal_transaction", "reason", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '冻结原因'")
	DBPatch.addColumn("t_abnormal_transaction", "mail_notify_type", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '邮件通知类型'")
	DBPatch.addColumn("t_abnormal_transaction", "unfrozen_amount", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '未解冻金额'")
	
	DBPatch.execute("ALTER TABLE `t_abnormal_transaction` DROP INDEX `status+seller`")
	DBPatch.execute("CREATE INDEX `status+buyer+freeze_obj` ON `t_abnormal_transaction`(`handle_status`, `buyer`, `freeze_object`) USING BTREE")
	DBPatch.execute("CREATE INDEX `status+seller+freeze_obj` ON `t_abnormal_transaction`(`handle_status`, `seller`, `freeze_object`) USING BTREE")
	DBPatch.addColumn("t_abnormal_transaction", "rmplayer_transmoney_flag", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '扣除玩家交易金标志'")
	DBPatch.addColumn("t_account", "unlock_extensible_role_num", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '可扩展角色解锁栏位'")
	DBPatch.addTable("t_activity_op_attribute", "guid")
	
	DBPatch.addColumn("t_guild_dungeon_player_damage", "dungeon_id", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `guild_id`")
	DBPatch.execute("DROP INDEX `guild_player_index` ON `t_guild_dungeon_player_damage`")
	DBPatch.execute("CREATE UNIQUE INDEX `guild_player_index` ON `t_guild_dungeon_player_damage`(`guild_id`, `player_id`, `dungeon_id`) USING BTREE")
	
	DBPatch.addTable("t_guild_dungeon_player_join", "guid")
	
	DBPatch.addColumn("t_auction_transaction", "item_mount_beadId", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_bead_buffId`")
	DBPatch.addColumn("t_auction_transaction", "item_mount_magicId", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_bead_averPrice`")
	DBPatch.addTable("t_mall_item", "guid")
	DBPatch.addTable("t_mall_acc_buy_record", "guid")
	DBPatch.execute("CREATE INDEX `roleid` ON `t_mall_item`(`roleid`) USING BTREE")
	
	DBPatch.addTable("t_auction_attention", "guid")
	
	DBPatch.addColumn("t_guild", "join_level", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `history_terrid`")
	DBPatch.addColumn("t_guild_member", "season_level", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	
	DBPatch.addColumn("t_guild_dungeon_info", "spend_time", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `total_damage`")
	DBPatch.dropColumn("t_guild_dungeon_player_damage", "dungeon_id")
	DBPatch.execute("DROP INDEX `guild_player_index` ON `t_guild_dungeon_player_damage`")
	DBPatch.execute("CREATE UNIQUE INDEX `guild_player_index` ON `t_guild_dungeon_player_damage`(`guild_id`, `player_id`) USING BTREE")
	
	DBPatch.addColumn("t_player_info", "game_set", "varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''")
	
	DBPatch.addTable("t_blackmarket_auction", "guid")
	DBPatch.addTable("t_blackmarket_auction_price", "guid")
	DBPatch.addTable("t_blackmarket_transaction", "guid")
	
	DBPatch.addColumn("t_player_info", "return_level", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `return_time`")
	DBPatch.addColumn("t_player_info", "return_first_login", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `return_level`")
	
	DBPatch.addTable("t_activity_account_record", "guid")
	DBPatch.addTable("t_account_counter", "guid")
	
	DBPatch.addColumn("t_sys_record", "owner_type", "int(20) UNSIGNED NOT NULL DEFAULT 0")
	
	DBPatch.addColumn("t_account", "adventure_team_name", "varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '冒险队名'")
	DBPatch.addColumn("t_account", "adventure_team_level", "smallint(5) UNSIGNED NOT NULL DEFAULT 1 COMMENT '冒险队等级'")
	DBPatch.addColumn("t_account", "adventure_team_exp", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '冒险队经验'")
	DBPatch.addColumn("t_account", "adventure_team_grade_id", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '冒险队评级id'")
	DBPatch.addColumn("t_account", "all_role_value_score", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '角色总评分'")
	
	DBPatch.addTable("t_expedition_map", "guid")
	DBPatch.addTable("t_expedition_member", "guid")
	DBPatch.addTable("t_account_task", "guid")
	
	DBPatch.addColumn("t_account", "unlocked_new_occus", "varchar(1024) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '解锁的新职业'")
	DBPatch.addColumn("t_account", "query_new_occus_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '查询新职业的时间'")

	DBPatch.addColumn("t_account", "offline_time", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	DBPatch.addColumn("t_player_info", "role_value_score", "int(10) UNSIGNED NOT NULL DEFAULT 0")
	
	DBPatch.addColumn("t_auction_attention", "attent_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '关注时间' AFTER `auction_id`");
	DBPatch.addColumn("t_player_info", "novice_guide_choose_flag", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '新手引导选择标志'")

	DBPatch.addColumn("t_blackmarket_transaction", "off_sale", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否下架' AFTER `auction_price`");
	DBPatch.addColumn("t_account_counter", "acc_guid", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_account_shop_acc_buy_record", "acc_guid", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_account_shop_acc_buy_record", "shop_id", "int(10) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_expedition_map", "acc_guid", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_expedition_member", "acc_guid", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_merge_give_namecard", "accid", "int(10) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.execute("ALTER TABLE `t_merge_give_namecard` MODIFY COLUMN `guid` bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `accid`;");
	DBPatch.execute("ALTER TABLE `t_merge_give_namecard` MODIFY COLUMN `name` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' AFTER `guid`;");
	
	DBPatch.addColumn("t_auction_new", "offsale_handled", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否做过下架处理' AFTER `item_src_guid`")
	
	DBPatch.addTable("t_guild_auction_item", "guid")
	DBPatch.addColumn("t_auction_new", "recove_handled", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否做过扫货处理' AFTER `offsale_handled`");
	DBPatch.addTable("t_head_frame", "guid")
	DBPatch.addColumn("t_player_info", "head_frame_id", "int(10) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_relation", "head_frame", "int(10) UNSIGNED NOT NULL DEFAULT 0");
	
	DBPatch.addColumn("t_guild_member", "emblem_level", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `season_level`")
	DBPatch.addColumn("t_guild", "dungeon_type", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `join_level`");
	
	DBPatch.addTable("t_new_title", "guid")
	DBPatch.addTable("t_new_title_accout", "guid")
	
	DBPatch.addColumn("t_player_info", "new_title_guid", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	
	DBPatch.addColumn("t_auction_transaction", "min_price_onsale", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `state`");
	DBPatch.addColumn("t_auction_transaction", "isSysRecove", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `min_price_onsale`");
	
	DBPatch.addColumn("t_guild_auction_item", "reward_group", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `guid`");
	DBPatch.addColumn("t_guild_auction_item", "bid_num", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `reward_group`");
	
	DBPatch.addColumn("t_guild_auction_item", "vip_level", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `bid_role_id`");
	DBPatch.addColumn("t_guild_auction_item", "level", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `vip_level`");
	DBPatch.addColumn("t_guild_auction_item", "occu", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `level`");
	DBPatch.addColumn("t_guild_auction_item", "is_bonus", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `end_time`");
	
	DBPatch.addColumn("t_player_info", "chiji_score", "int(10) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_player_info", "chiji_score_reset_time", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");

	DBPatch.addColumn("t_player_info", "equal_pvp_skills", "varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '公平竞技场技能' AFTER `chiji_score_reset_time`");
	DBPatch.addColumn("t_player_info", "equal_pvp_skillbar", "varchar(512) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '公平竞技场技能栏' AFTER `equal_pvp_skills`");

	DBPatch.addTable("t_guild_dungeon_rand", "guid")
	
	DBPatch.addColumn("t_auction_transaction", "isNotRecAveprice", "int(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `isSysRecove`");
	
	DBPatch.addTable("t_item_deposit", "guid")
	
	DBPatch.addTable("t_week_sign", "guid")
	
	DBPatch.addTable("t_player_joins", "guid")
	DBPatch.addColumn("t_red_packet_receiver", "money_id", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `level`");
	DBPatch.addColumn("t_red_packet_receiver", "red_packet_owner_name", "varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `time`");
	DBPatch.addColumn("t_red_packet_receiver", "is_best", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `red_packet_owner_name`");
	DBPatch.addColumn("t_red_packet", "cond2", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `status_time`");
	
	DBPatch.addColumn("t_relation", "guild_id", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	DBPatch.addColumn("t_item_deposit", "dungeon_id", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `create_time`");
	DBPatch.addColumn("t_item", "equiptype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `bead_replace_cnt`");
	DBPatch.addColumn("t_item", "enhancetype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `equiptype`");
	DBPatch.addColumn("t_item", "enhancefailed", "smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancetype`");
	DBPatch.addColumn("t_item", "enhancecount", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancefailed`");

	DBPatch.execute("ALTER TABLE `t_red_packet` MODIFY COLUMN `name` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `guid`;");
	DBPatch.addColumn("t_auction_new", "equiptype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `bead_replace_cnt`");
	DBPatch.addColumn("t_auction_new", "enhancetype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `equiptype`");
	DBPatch.addColumn("t_auction_new", "enhancefailed", "smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancetype`");
	DBPatch.addColumn("t_auction_new", "enhancecount", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancefailed`");

	DBPatch.addColumn("t_mailitem", "equiptype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `bead_replace_cnt`");
	DBPatch.addColumn("t_mailitem", "enhancetype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `equiptype`");
	DBPatch.addColumn("t_mailitem", "enhancefailed", "smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancetype`");
	DBPatch.addColumn("t_mailitem", "enhancecount", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancefailed`");

	DBPatch.addColumn("t_guild_storage", "equiptype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `bead_replace_cnt`");
	DBPatch.addColumn("t_guild_storage", "enhancetype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `equiptype`");
	DBPatch.addColumn("t_guild_storage", "enhancefailed", "smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancetype`");
	DBPatch.addColumn("t_guild_storage", "enhancecount", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancefailed`");

	DBPatch.addColumn("t_blackmarket_transaction", "equiptype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `bead_replace_cnt`");
	DBPatch.addColumn("t_blackmarket_transaction", "enhancetype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `equiptype`");
	DBPatch.addColumn("t_blackmarket_transaction", "enhancefailed", "smallint(5) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancetype`");
	DBPatch.addColumn("t_blackmarket_transaction", "enhancecount", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancefailed`");

	
	DBPatch.addColumn("t_item_deposit", "equip_type", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `quality_lv`");
	
	DBPatch.addTable("t_activity_op_account_task", "guid");
	DBPatch.addTable("t_activity_op_record", "guid");
	
	DBPatch.addColumn("t_auction_recommend_price", "enhance_type", "tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '红字装备增幅类型' AFTER `bead_buff_id`");
	DBPatch.addColumn("t_auction_transaction", "item_enhance_type", "tinyint(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '红字装备增幅类型' AFTER `item_bead_buffId`");

	DBPatch.addTable("t_daily_todo", "guid");
	
	DBPatch.addColumn("t_account", "account_achievement_score", "int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '账号成就点' AFTER `offline_time`");
	
	DBPatch.addColumn("t_relation", "guild_id", "bigint(20) UNSIGNED NOT NULL DEFAULT 0");
	
	DBPatch.execute("ALTER TABLE `t_account` MODIFY COLUMN `adventure_team_level` smallint(5) UNSIGNED NOT NULL DEFAULT 0  AFTER `adventure_team_name`;");
	
	DBPatch.addColumn("t_account_task", "acc_guid", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `submit_count`");
	
	DBPatch.execute("CREATE INDEX `acc_index` ON `t_activity_account_record`(`acc_id`) USING BTREE");
	DBPatch.execute("ALTER TABLE `t_activity_account_record` DROP INDEX `acc_activity_index`");
	
	DBPatch.addColumn("t_red_packet_receiver", "reciver_accid", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `receiver_id`");
	
	DBPatch.execute("ALTER TABLE `t_item_deposit` MODIFY COLUMN `strengthen`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `num`");
	DBPatch.execute("ALTER TABLE `t_item_deposit` MODIFY COLUMN `quality_lv`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `strengthen`");
	DBPatch.execute("ALTER TABLE `t_item_deposit` MODIFY COLUMN `equip_type`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `quality_lv`");
	
	
	DBPatch.addColumn("t_item", "auctionTransNum", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancecount`");
	DBPatch.addColumn("t_auction_new", "auctionTransNum", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancecount`");
	DBPatch.addColumn("t_mailitem", "auctionTransNum", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancecount`");
	DBPatch.addColumn("t_guild_storage", "auctionTransNum", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancecount`");
	DBPatch.addColumn("t_blackmarket_transaction", "auctionTransNum", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `enhancecount`");
	
	DBPatch.addColumn("t_auction_transaction", "mprice", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `isNotRecAveprice`");


	DBPatch.addColumn("t_guild", "prosperity_status", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `dungeon_type`");
	DBPatch.addColumn("t_guild", "week_added_fund", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `prosperity_status`");
	DBPatch.addColumn("t_guild", "merger_request_status", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `week_added_fund`");
	DBPatch.addColumn("t_guild", "goal_guild_id", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `merger_request_status`");
	DBPatch.addColumn("t_guild", "merger_request_time", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `goal_guild_id`");
	DBPatch.addColumn("t_guild", "merger_accept_time", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `merger_request_time`");
	DBPatch.addColumn("t_guild", "merger_other_time", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `merger_accept_time`");
	DBPatch.addColumn("t_guild", "last_week_added_fund", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `merger_other_time`");
	DBPatch.addColumn("t_account", "mall_point", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `adventure_team_grade`");
	
	DBPatch.addTable("t_team_copy", "guid");
	DBPatch.addTable("t_currency_frozen", "guid");
	
	DBPatch.addColumn("t_account", "adventure_coin", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `mall_point`");
	
	DBPatch.addColumn("t_relation", "return_year_title", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `guild_id`");
	
	DBPatch.addColumn("t_item", "inscriptionHoles", "varchar(128) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_mailitem", "inscriptionHoles", "varchar(128) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_auction_new", "inscriptionHoles", "varchar(128) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_guild_storage", "inscriptionHoles", "varchar(128) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_blackmarket_transaction", "inscriptionHoles", "varchar(128) NOT NULL DEFAULT ''");


	DBPatch.addTable("t_account_adventure", "guid");

	DBPatch.addTable("t_hire", "guid");
	
	DBPatch.addColumn("t_auction_transaction", "item_mount_beadBuffId", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `mprice`");
	DBPatch.addColumn("t_auction_transaction", "item_mount_magicLv", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_mount_beadBuffId`");
	DBPatch.addColumn("t_auction_transaction", "item_equip_type", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_bead_buffId`");
	DBPatch.addColumn("t_auction_transaction", "item_enhance_num", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `item_enhance_type`");
	
	DBPatch.addColumn("t_blackmarket_auction", "back_buy_subtype", "tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `back_buy_type`");

	DBPatch.execute("ALTER TABLE `t_team_copy` MODIFY COLUMN `day_num`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `guid`");
	DBPatch.execute("ALTER TABLE `t_team_copy` MODIFY COLUMN `week_num`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `day_time`;");

	DBPatch.addTable("t_item_presents", "guid");
	DBPatch.addTable("t_equip_scheme", "guid");
	
	DBPatch.addTable("t_honor_history", "guid");
	DBPatch.addTable("t_honor_role", "guid");
	
	DBPatch.addColumn("t_honor_role", "is_use_guard_card", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `highest_honor_lvl`");
	DBPatch.execute("ALTER TABLE `t_jaritem_pool` MODIFY COLUMN `occus`  varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT ''");
	
	DBPatch.addTable("t_shortcut_key", "guid");
	
	DBPatch.addColumn("t_guild_member", "battle_terr_day_reward", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `emblem_level`");
	
	DBPatch.execute("ALTER TABLE `t_team_copy` ADD COLUMN `extra_num`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `week_flop`");
	
	DBPatch.addTable("t_lost_dungeon", "guid");
	
	DBPatch.addColumn("t_item", "keyValues", "varchar(1024) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_mailitem", "keyValues", "varchar(1024) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_auction_new", "keyValues", "varchar(1024) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_guild_storage", "keyValues", "varchar(1024) NOT NULL DEFAULT ''");
	DBPatch.addColumn("t_blackmarket_transaction", "keyValues", "varchar(1024) NOT NULL DEFAULT ''");
	
	DBPatch.addColumn("t_account", "credit_point", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `point`");
	DBPatch.addTable("t_credit_point_record", "guid");
	DBPatch.execute("CREATE INDEX `acc_index` ON `t_credit_point_record`(`acc_id`) USING BTREE");
	DBPatch.addColumn("t_credit_point_record", "today_tansfer", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `cost_num`");
	
	DBPatch.execute("ALTER TABLE `t_credit_point_record` DROP COLUMN `transfer_num`");
	DBPatch.execute("ALTER TABLE `t_credit_point_record` DROP COLUMN `cost_num`");
	DBPatch.execute("ALTER TABLE `t_credit_point_record` MODIFY COLUMN `order_list`  varchar(8192) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `today_tansfer`");
	
	
	DBPatch.addColumn("t_guild", "last_update_guild_building_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `last_week_added_fund`");
	
	DBPatch.addColumn("t_player_info", "add_preferences_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `equal_pvp_skillbar`");
	DBPatch.addColumn("t_player_info", "del_preferences_time", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `add_preferences_time`");
	
	DBPatch.addTable("t_account_monopoly", "guid");
	

	DBPatch.addColumn("t_team_copy", "owner", "bigint(20) UNSIGNED NOT NULL DEFAULT 0 AFTER `guid`");
	DBPatch.addColumn("t_team_copy", "team_type", "int(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `owner`");
	DBPatch.execute("CREATE INDEX `owner_index` ON `t_team_copy`(`owner`) USING BTREE");

  DBPatch.addTable("t_guild_event", "guid");
	
	DBPatch.execute("ALTER TABLE `t_team_copy` ADD COLUMN `roll_num`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL AFTER `extra_num`");
	
end

function Init()
	DBPatch.init()
	DBPatch.patch()
	DBPatch.closeHandlers()
end