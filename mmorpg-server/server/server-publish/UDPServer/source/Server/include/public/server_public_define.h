#ifndef __SERVER_PUBLIC_DEFINE_H__
#define __SERVER_PUBLIC_DEFINE_H__

#include "gameplatform_types.h"

namespace KingNet { namespace Server {

//用于CRouterHead
enum enmServerEntity
{
	server_entity_logic		= 0,
	server_entity_connector = 1,
	server_entity_router	= 2,
	server_entity_gamedb	= 3,
	server_entity_catalog	= 4,
	server_entity_itemdb	= 5,
	server_entity_market	= 6,
	server_entity_profile	= 7,
	server_entity_state		= 8,
	server_entity_hall		= 9,
	server_entity_frienddb	= 10,
	server_entity_webmarket = 11,
	server_entity_config	= 12,
    server_entity_monitor	= 13,
	server_entity_gm		= 14,
	server_entity_score		= 15,
	server_entity_51pay		= 16,  //pay.51.com
	server_entity_miscserver= 17,  //game.51.com
	server_entity_hotnews   = 18 ,
	server_entity_chest     = 19 , //开宝箱
	server_entity_antibot   = 20 ,
	server_entity_action    = 21 ,
	server_entity_grouprank = 22 , //群排名服务器
	server_entity_udpconnector = 23,	//udp connector server
	server_entity_relay     = 24,	//relay server
	server_entity_vip		= 25,	//vip server
	server_entity_recommend = 26,   //recommend server
	server_entity_magic		= 27,	//magic server
	server_entity_ad		= 28,	//AD Server
	server_entity_compete	= 29,	//比赛DB
	server_entity_agent = 30,      //agent server
	//continue ...

	//max_entity_type,    //modified by aprilliu. 2009-04-14
	
	max_server_type_count,
};

enum
{
	//如果修改，必须要注意不要超过common库中定义的socket缓冲区
	max_ss_package_size = 0xffff,		//64k SS通讯协议包的最大长度
	invalid_message_id  = -1,
	invalid_message_type = -1,
};

enum
{
	int32_min_value = 0x80000000,
	int32_max_value = 0x7fffffff,
	money_min_value = 0,
	money_max_value = int32_max_value,
	point_min_value = int32_min_value,
	point_max_value = int32_max_value,
	charming_min_value = int32_min_value,
	charming_max_value = int32_max_value,
	item_min_value = int32_min_value,
	item_max_value = int32_max_value,

	int64_min_value = 0x8000000000000000,
	int64_max_value = 0x7fffffffffffff,
	happybean_min_value =0,
	happybean_max_value =int32_max_value,
};

//用于CMessageHead
enum enEntityType //平台系统中的实体类型
{
	entity_type_invalid = 0xff,

	entity_type_player		= 0x01,
	entity_type_room		= 0x02,
	entity_type_table		= 0x03,
	entity_type_server		= 0x04,
	entity_type_client		= 0x05,
	entity_type_gamedb		= 0x06,
	entity_type_itemdb		= 0x07,
	entity_type_connector	= 0x08,
	entity_type_catalog		= 0x09,
	entity_type_p2p			= 0x0A,
	entity_type_buyer		= 0x0B,
	entity_type_router		= 0x0C,
	entity_type_hallserver	= 0x0D,		//hall server entity
	entity_type_stateserver = 0x0F,		//state server entity
	entity_type_frienddb	= 0x10,
	entity_type_hander		= 0x11,		// market server's handler object
	entity_type_config		= 0x12,
	entity_type_gm			= 0x13,		//gm tools entity
	entity_type_score		= 0x14,		//积分服务器
	entity_type_platformkey	= 0x15,		//proxy服务器
	entity_type_hotnews     = 0x16,
	entity_type_chest       = 0x17,
	entity_type_antibot		= 0x18,		//反外挂服务器
	entity_type_action		= 0x19,		//活动服务器
	entity_type_app_handler = 0x1A,		//专门用于hall系统定时
	entity_type_group_rank  = 0x1B,		//群排名服务器
	entity_type_vip			= 0x1C,		//
	entity_type_recommend   = 0x1D,
	entity_type_magic		= 0x1E,
	entity_type_ad			= 0x1F,
	entity_type_compete		= 0x20,		//
	//add by liujj@2009-09-22===>begin
	entity_type_rankcompete	= 0x21,		//
	//add by liujj@2009-09-22===>end
	entity_type_agent		= 0x22,

	// 	entity_type_

	entity_type_chat		= 0x23,
	entity_type_log			 = 0x24,
	max_entity_type_count,				//added by aprilliu, 2009-04-14
};

enum
{
	logic_state_keepalive_interval	 =	30,			//logic server与state server之间的保活时间间隔 30s
};

//service_tag:4个字节,高两个字节为service_tag_type,低两个字节为service_tag_id
/*
enum
{
service_tag_game =1,
service_tag_buyitem =2,
};
*/

enum enmMoneyLockDefine
{
	platform_money_lock_none				= 0x00000000,	//无
	platform_money_lock_game				= 0x00000001,	//game
	platform_money_lock_buy_item			= 0x00000002,	//道具购买.
	platform_money_lock_exchange_chare_item = 0x00000004,	//道具回兑.
	platform_money_lock_charge				= 0x00000008,	//充值...
	platform_money_lock_exchange			= 0x00000010,	//回兑成游戏币...
};

//欢乐豆赠送方式
enum enmHappyBeanPresentMode
{
	enmHappyBeanPresentMode_first_play = 1,				//首次进入欢乐游戏赠送
	enmHappyBeanPresentMode_daily_present = 2,			//每天日常赠送
	enmHappyBeanPresentMode_game_present = 3,			//游戏逻辑决定的赠送
	enmHappyBeanPresentMode_first_login_platform = 4,	//首次登陆游戏平台赠送, hall server使用
	//to be added ...

};

//帐单的业务掩码
enum enmServiceTagMask
{
	service_tag_type_mask		= 0xffff0000,	//类型掩码
	service_tag_id_mask			= 0x0000ffff,	//ID掩码
};

//帐单的业务类型
enum enmServiceTagType
{
	service_tag_type_game		= (int32_t)0x00000000,	//游戏.游戏业务的子业务就是具体游戏ID
	service_tag_type_item		= (int32_t)0x00010000,	//道具
	service_tag_type_charge		= (int32_t)0x00020000,	//游戏币充值
	service_tag_type_exchange	= (int32_t)0x00030000,	//游戏币回兑 
	service_tag_type_gm			= (int32_t)0x00040000,	//gm
	service_tag_type_service	= (int32_t)0x00050000,	//用户服务（包括平台服务类的续费等）
	
	//added by aprilliu, 2009-1-7
	//service_tag_type_happybean_present = (int32_t)0x00060000,		//欢乐豆赠送
	service_tag_type_happybean  = (int32_t)0x00060000,	//欢乐豆业务。凡是由其他业务引发的欢乐豆改变，欢乐豆账单中的service tag要使用其他业务的tag, 否则使用该service tag
														//账单里增加game id字段

	service_tag_type_promotion  = (int32_t)0x00070000,  //(促销)活动

	service_tag_type_game_item	= (int32_t)0x00080000,	//游戏内道具操作

};

//service_tag_type_item 子业务
enum enmServiceTagItemID
{
	service_tag_id_item_default_use	= 0x0000,	//现有的道具使用(不会产生其它道具)
	service_tag_id_item_buy			= 0x0001,	//购买给自已拥用
	service_tag_id_item_present		= 0x0002,	//购买给他人,即赠送
	service_tag_id_item_atonce_use	= 0x0003,	//购买后立即使用,即买即用
	service_tag_id_item_exchange	= 0x0004,	//魅力物品回兑 
	service_tag_id_item_system		= 0x0005,	//系统产生(根据不同业务规则而产生的，如在线时长赠送百宝箱)
	service_tag_id_item_self		= 0x0006,	//使用道具A,产生其它道具（如使用百宝箱产生其它道具)
	service_tag_id_item_expire		= 0x0007,	//道具过期(系统自动清理)

	//
	service_tag_id_game_define_begin = 0x2000,	//留给game自定义id的起始id
	service_tag_id_game_buy_item	 = 0x2001,	//游戏内购买道具
	service_tag_id_game_spawn_item	 = 0x2002,	//游戏内生成道具（比如：活动道具）

};

//游戏币充值业务的业务ID
enum enmServiceTagChargeID
{
	service_tag_id_charge_rmb		=0x0001,	//rmb充值
	service_tag_id_charge_51		=0x0002,	//51币充值
};

//游戏币回兑的业务ID
enum enmServiceTagExchangeID
{
	service_tag_id_exchange_51		=0x0001,	//回兑成51币
};

//用户服务
enum enmServiceTagServiceID
{
	service_tag_id_service_renew	= 0x0001,	//续费(服务)
};

//欢乐豆业务的具体业务ID
enum enmServiceTagHappyBeanID
{	
	enmServiceTagHappyBeanID_present_first_play		= (int32_t)0x0001,		//首次登陆游戏发生的赠送
	enmServiceTagHappyBeanID_present_enter_room		= (int32_t)0x0002,		//进入欢乐房间发生的赠送
	enmServiceTagHappyBeanID_present_set_gameresult = (int32_t)0x0003,		//欢乐游戏局结算时发生的赠送
	enmServiceTagHappyBeanID_present_first_login_platform = (int32_t)0x0004,//首次进入游戏平台发生的赠送
//	enmServiceTagHappyBeanID_present_buy_goods		= (int32_t)0x0004,		//购买商品赠送. 由其他业务引起的欢乐豆业务直接使用其他业务的tag
//	enmServiceTagHappyBeanID_present_register_service = (int32_t)0x0005,	//购买服务赠送 由其他业务引起的欢乐豆业务直接使用其他业务的
    
	enmServiceTagHappyBeanID_present_by_game		= (int32_t)0x0100,		//游戏逻辑进行赠送。最低1btye标识游戏内赠送的具体原因
																			//(具体原因由各个游戏自己定义, 取值范围：1 -- 127)
	


};

//活动的具体业务ID
enum enmServiceTagPromotionID
{
	enmServiceTagPromotionID_game_online_time = (int32_t)0x0001,      //游戏在线时长赠送物品(包括欢乐豆,道具...等等)
	enmServiceTagPromotionID_first_login_platform = (int32_t)0x0004,      //游戏在线时长赠送物品(包括欢乐豆,道具...等等)
    
};
enum
{
    item_reserved_bean = 0x117fffff ,
    
} ;

//
#define GET_SERVICE_TAG_TYPE(nTag)  ((int32_t)((nTag) & service_tag_type_mask))
#define GET_SERVICE_TAG_ID(nTag)	((int32_t)((nTag) & service_tag_id_mask))
#define GET_SERVICE_TAG(nType, nId)	((int32_t)((nType) | (nId)))

//对于具体游戏币变化的业务有如下类型：
//小喇叭，大喇叭，炸弹，送魅力物品等即买即用的道具购买业务
#define GET_SERVICE_TAG_ITEM_ATONCE()	GET_SERVICE_TAG(service_tag_type_item, service_tag_id_item_atonce_use)
#define GET_SERVICE_TAG_ITEM_PRESENT() 	GET_SERVICE_TAG(service_tag_type_item, service_tag_id_item_present)
//TODO... 其他业务tag

//得到游戏内赠送欢乐豆的service tag. presentReason 是指游戏自定义的赠送原因
#define GAME_PRESENT_HAPPYBEAN_SERVICETAG(presentReason)	((int32_t)((service_tag_type_happybean | enmServiceTagHappyBeanID_present_by_game)| (int32_t)presentReason ))

//道具更新的来源(高2个字节)
enum enmUpdateItemInfoSourceType
{
	update_item_source_type_game		= 0x00000000,			//游戏内(低2个字节是具体游戏id)
	update_item_source_type_market		= 0x00010000,			//游戏道具商城
	update_item_source_type_webcenter	= 0x00020000,			//网站中心
	update_item_source_type_gm          = 0x00030000,           //GM管理工具
	update_item_source_type_hall        = 0x00040000,           //Hall中使用
};

#define GET_UPDATE_ITEM_SOURCE_TYPE(type, subtype)   int32_t((type) | (subtype))

//对于更新道具的来源tag，对于赠送魅力物品，扔炸弹，大喇叭需要client上报所在的 游戏id, 作为source tag的低2字节 
//不在任何游戏中，游戏id为0xffff

//对于使用小喇叭，由于协议中有roomid，则可以根据roomid得到所在的游戏id

//用户发言途径
enum enmSpeakSourceType
{
	speak_source_type_hall_loudspeaker	= 0x0001,	//大厅喇叭的发言
	speak_source_type_game_loudspeaker	= 0x0002,	//游戏喇叭的发言
	speak_source_type_room_loudspeaker	= 0x0003,	//房间喇叭的发言
	speak_source_type_private_talk		= 0x0004,	//经过大厅的私聊发言
	speak_source_type_game_talk			= 0x0005,	//游戏内聊天发言
	speak_source_type_room_talk			= 0x0006,	//房间内聊天发言
};
 
}}//namespace KingNet { namespace Server {

#endif
