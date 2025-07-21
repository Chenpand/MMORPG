delimiter $$
drop procedure if exists clear_alltables $$
create procedure clear_alltables()
begin
	TRUNCATE TABLE `t_account`;
	TRUNCATE TABLE `t_active_task`;
	TRUNCATE TABLE `t_activity_op`;
	TRUNCATE TABLE `t_activity_op_task`;
	TRUNCATE TABLE `t_charge_order`;
	TRUNCATE TABLE `t_charge_record`;
	TRUNCATE TABLE `t_counter`;
	TRUNCATE TABLE `t_dungeon`;
	TRUNCATE TABLE `t_g_dungeon_hard`;
	TRUNCATE TABLE `t_guild`;
	TRUNCATE TABLE `t_guild_member`;
	TRUNCATE TABLE `t_item`;
	TRUNCATE TABLE `t_jar_record`;
	TRUNCATE TABLE `t_jaritem_pool`;
	TRUNCATE TABLE `t_mail`;
	TRUNCATE TABLE `t_mailitem`;
	TRUNCATE TABLE `t_mall_gift_pack`;
	TRUNCATE TABLE `t_offline_event`;
	TRUNCATE TABLE `t_pk_statistic`;
	TRUNCATE TABLE `t_player_info`;
	TRUNCATE TABLE `t_player_info_name`;
	TRUNCATE TABLE `t_punishment`;
	TRUNCATE TABLE `t_red_packet`;
	TRUNCATE TABLE `t_red_packet_receiver`;
	TRUNCATE TABLE `t_relation`;
	TRUNCATE TABLE `t_retinue`;
	TRUNCATE TABLE `t_shop`;
	TRUNCATE TABLE `t_shopitem`;
	TRUNCATE TABLE `t_sortlist`;
	TRUNCATE TABLE `t_sys_record`;
	TRUNCATE TABLE `t_task`;
end $$

delimiter ;