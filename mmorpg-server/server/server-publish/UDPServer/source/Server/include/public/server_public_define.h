#ifndef __SERVER_PUBLIC_DEFINE_H__
#define __SERVER_PUBLIC_DEFINE_H__

#include "gameplatform_types.h"

namespace KingNet { namespace Server {

//����CRouterHead
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
	server_entity_chest     = 19 , //������
	server_entity_antibot   = 20 ,
	server_entity_action    = 21 ,
	server_entity_grouprank = 22 , //Ⱥ����������
	server_entity_udpconnector = 23,	//udp connector server
	server_entity_relay     = 24,	//relay server
	server_entity_vip		= 25,	//vip server
	server_entity_recommend = 26,   //recommend server
	server_entity_magic		= 27,	//magic server
	server_entity_ad		= 28,	//AD Server
	server_entity_compete	= 29,	//����DB
	server_entity_agent = 30,      //agent server
	//continue ...

	//max_entity_type,    //modified by aprilliu. 2009-04-14
	
	max_server_type_count,
};

enum
{
	//����޸ģ�����Ҫע�ⲻҪ����common���ж����socket������
	max_ss_package_size = 0xffff,		//64k SSͨѶЭ�������󳤶�
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

//����CMessageHead
enum enEntityType //ƽ̨ϵͳ�е�ʵ������
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
	entity_type_score		= 0x14,		//���ַ�����
	entity_type_platformkey	= 0x15,		//proxy������
	entity_type_hotnews     = 0x16,
	entity_type_chest       = 0x17,
	entity_type_antibot		= 0x18,		//����ҷ�����
	entity_type_action		= 0x19,		//�������
	entity_type_app_handler = 0x1A,		//ר������hallϵͳ��ʱ
	entity_type_group_rank  = 0x1B,		//Ⱥ����������
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
	logic_state_keepalive_interval	 =	30,			//logic server��state server֮��ı���ʱ���� 30s
};

//service_tag:4���ֽ�,�������ֽ�Ϊservice_tag_type,�������ֽ�Ϊservice_tag_id
/*
enum
{
service_tag_game =1,
service_tag_buyitem =2,
};
*/

enum enmMoneyLockDefine
{
	platform_money_lock_none				= 0x00000000,	//��
	platform_money_lock_game				= 0x00000001,	//game
	platform_money_lock_buy_item			= 0x00000002,	//���߹���.
	platform_money_lock_exchange_chare_item = 0x00000004,	//���߻ض�.
	platform_money_lock_charge				= 0x00000008,	//��ֵ...
	platform_money_lock_exchange			= 0x00000010,	//�ضҳ���Ϸ��...
};

//���ֶ����ͷ�ʽ
enum enmHappyBeanPresentMode
{
	enmHappyBeanPresentMode_first_play = 1,				//�״ν��뻶����Ϸ����
	enmHappyBeanPresentMode_daily_present = 2,			//ÿ���ճ�����
	enmHappyBeanPresentMode_game_present = 3,			//��Ϸ�߼�����������
	enmHappyBeanPresentMode_first_login_platform = 4,	//�״ε�½��Ϸƽ̨����, hall serverʹ��
	//to be added ...

};

//�ʵ���ҵ������
enum enmServiceTagMask
{
	service_tag_type_mask		= 0xffff0000,	//��������
	service_tag_id_mask			= 0x0000ffff,	//ID����
};

//�ʵ���ҵ������
enum enmServiceTagType
{
	service_tag_type_game		= (int32_t)0x00000000,	//��Ϸ.��Ϸҵ�����ҵ����Ǿ�����ϷID
	service_tag_type_item		= (int32_t)0x00010000,	//����
	service_tag_type_charge		= (int32_t)0x00020000,	//��Ϸ�ҳ�ֵ
	service_tag_type_exchange	= (int32_t)0x00030000,	//��Ϸ�һض� 
	service_tag_type_gm			= (int32_t)0x00040000,	//gm
	service_tag_type_service	= (int32_t)0x00050000,	//�û����񣨰���ƽ̨����������ѵȣ�
	
	//added by aprilliu, 2009-1-7
	//service_tag_type_happybean_present = (int32_t)0x00060000,		//���ֶ�����
	service_tag_type_happybean  = (int32_t)0x00060000,	//���ֶ�ҵ�񡣷���������ҵ�������Ļ��ֶ��ı䣬���ֶ��˵��е�service tagҪʹ������ҵ���tag, ����ʹ�ø�service tag
														//�˵�������game id�ֶ�

	service_tag_type_promotion  = (int32_t)0x00070000,  //(����)�

	service_tag_type_game_item	= (int32_t)0x00080000,	//��Ϸ�ڵ��߲���

};

//service_tag_type_item ��ҵ��
enum enmServiceTagItemID
{
	service_tag_id_item_default_use	= 0x0000,	//���еĵ���ʹ��(���������������)
	service_tag_id_item_buy			= 0x0001,	//���������ӵ��
	service_tag_id_item_present		= 0x0002,	//���������,������
	service_tag_id_item_atonce_use	= 0x0003,	//���������ʹ��,������
	service_tag_id_item_exchange	= 0x0004,	//������Ʒ�ض� 
	service_tag_id_item_system		= 0x0005,	//ϵͳ����(���ݲ�ͬҵ�����������ģ�������ʱ�����Ͱٱ���)
	service_tag_id_item_self		= 0x0006,	//ʹ�õ���A,�����������ߣ���ʹ�ðٱ��������������)
	service_tag_id_item_expire		= 0x0007,	//���߹���(ϵͳ�Զ�����)

	//
	service_tag_id_game_define_begin = 0x2000,	//����game�Զ���id����ʼid
	service_tag_id_game_buy_item	 = 0x2001,	//��Ϸ�ڹ������
	service_tag_id_game_spawn_item	 = 0x2002,	//��Ϸ�����ɵ��ߣ����磺����ߣ�

};

//��Ϸ�ҳ�ֵҵ���ҵ��ID
enum enmServiceTagChargeID
{
	service_tag_id_charge_rmb		=0x0001,	//rmb��ֵ
	service_tag_id_charge_51		=0x0002,	//51�ҳ�ֵ
};

//��Ϸ�һضҵ�ҵ��ID
enum enmServiceTagExchangeID
{
	service_tag_id_exchange_51		=0x0001,	//�ضҳ�51��
};

//�û�����
enum enmServiceTagServiceID
{
	service_tag_id_service_renew	= 0x0001,	//����(����)
};

//���ֶ�ҵ��ľ���ҵ��ID
enum enmServiceTagHappyBeanID
{	
	enmServiceTagHappyBeanID_present_first_play		= (int32_t)0x0001,		//�״ε�½��Ϸ����������
	enmServiceTagHappyBeanID_present_enter_room		= (int32_t)0x0002,		//���뻶�ַ��䷢��������
	enmServiceTagHappyBeanID_present_set_gameresult = (int32_t)0x0003,		//������Ϸ�ֽ���ʱ����������
	enmServiceTagHappyBeanID_present_first_login_platform = (int32_t)0x0004,//�״ν�����Ϸƽ̨����������
//	enmServiceTagHappyBeanID_present_buy_goods		= (int32_t)0x0004,		//������Ʒ����. ������ҵ������Ļ��ֶ�ҵ��ֱ��ʹ������ҵ���tag
//	enmServiceTagHappyBeanID_present_register_service = (int32_t)0x0005,	//����������� ������ҵ������Ļ��ֶ�ҵ��ֱ��ʹ������ҵ���
    
	enmServiceTagHappyBeanID_present_by_game		= (int32_t)0x0100,		//��Ϸ�߼��������͡����1btye��ʶ��Ϸ�����͵ľ���ԭ��
																			//(����ԭ���ɸ�����Ϸ�Լ�����, ȡֵ��Χ��1 -- 127)
	


};

//��ľ���ҵ��ID
enum enmServiceTagPromotionID
{
	enmServiceTagPromotionID_game_online_time = (int32_t)0x0001,      //��Ϸ����ʱ��������Ʒ(�������ֶ�,����...�ȵ�)
	enmServiceTagPromotionID_first_login_platform = (int32_t)0x0004,      //��Ϸ����ʱ��������Ʒ(�������ֶ�,����...�ȵ�)
    
};
enum
{
    item_reserved_bean = 0x117fffff ,
    
} ;

//
#define GET_SERVICE_TAG_TYPE(nTag)  ((int32_t)((nTag) & service_tag_type_mask))
#define GET_SERVICE_TAG_ID(nTag)	((int32_t)((nTag) & service_tag_id_mask))
#define GET_SERVICE_TAG(nType, nId)	((int32_t)((nType) | (nId)))

//���ھ�����Ϸ�ұ仯��ҵ�����������ͣ�
//С���ȣ������ȣ�ը������������Ʒ�ȼ����õĵ��߹���ҵ��
#define GET_SERVICE_TAG_ITEM_ATONCE()	GET_SERVICE_TAG(service_tag_type_item, service_tag_id_item_atonce_use)
#define GET_SERVICE_TAG_ITEM_PRESENT() 	GET_SERVICE_TAG(service_tag_type_item, service_tag_id_item_present)
//TODO... ����ҵ��tag

//�õ���Ϸ�����ͻ��ֶ���service tag. presentReason ��ָ��Ϸ�Զ��������ԭ��
#define GAME_PRESENT_HAPPYBEAN_SERVICETAG(presentReason)	((int32_t)((service_tag_type_happybean | enmServiceTagHappyBeanID_present_by_game)| (int32_t)presentReason ))

//���߸��µ���Դ(��2���ֽ�)
enum enmUpdateItemInfoSourceType
{
	update_item_source_type_game		= 0x00000000,			//��Ϸ��(��2���ֽ��Ǿ�����Ϸid)
	update_item_source_type_market		= 0x00010000,			//��Ϸ�����̳�
	update_item_source_type_webcenter	= 0x00020000,			//��վ����
	update_item_source_type_gm          = 0x00030000,           //GM������
	update_item_source_type_hall        = 0x00040000,           //Hall��ʹ��
};

#define GET_UPDATE_ITEM_SOURCE_TYPE(type, subtype)   int32_t((type) | (subtype))

//���ڸ��µ��ߵ���Դtag����������������Ʒ����ը������������Ҫclient�ϱ����ڵ� ��Ϸid, ��Ϊsource tag�ĵ�2�ֽ� 
//�����κ���Ϸ�У���ϷidΪ0xffff

//����ʹ��С���ȣ�����Э������roomid������Ը���roomid�õ����ڵ���Ϸid

//�û�����;��
enum enmSpeakSourceType
{
	speak_source_type_hall_loudspeaker	= 0x0001,	//�������ȵķ���
	speak_source_type_game_loudspeaker	= 0x0002,	//��Ϸ���ȵķ���
	speak_source_type_room_loudspeaker	= 0x0003,	//�������ȵķ���
	speak_source_type_private_talk		= 0x0004,	//����������˽�ķ���
	speak_source_type_game_talk			= 0x0005,	//��Ϸ�����췢��
	speak_source_type_room_talk			= 0x0006,	//���������췢��
};
 
}}//namespace KingNet { namespace Server {

#endif
