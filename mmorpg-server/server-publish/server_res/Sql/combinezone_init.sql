#清空公告表
DELETE FROM `t_announcement`;

#重置通天塔数据
UPDATE `t_babelinfo` SET `mintime`=0,`minlevel`=0,`firstkillplayerid`=0,`mintimeplayerid`=0,`minlevelplayerid`=0,`firstkillplayername`="",`mintimeplayername`="",`minlevelplayername`="";
DELETE FROM `t_merchant_rec`;

#清空广播邮件
DELETE FROM `t_broadcastmail`;

#全局参数表的处理


#清空祈福日志
DELETE FROM `t_lottery_rec`;

#部落相关
DELETE FROM `t_tribe_territory`;

#清空名人堂数据
DELETE FROM `t_halloffame`;

#清空结婚数据
DELETE FROM `t_marriage`;
DELETE FROM `t_marriage_guest`;

#记录合区标志和合区时间
REPLACE INTO `t_gameparam`(`guid`,`name`,`value`) VALUES(1, "new_combinezone", 1);
REPLACE INTO `t_gameparam`(`guid`,`name`,`value`) VALUES(2, "combinezone_time", unix_timestamp());
