#ifndef __SERVER_ITEM_DEFINE_H__
#define __SERVER_ITEM_DEFINE_H__

#include "gameplatform_types.h"

#include "Common/common_base.h"

/************************************************************************/
/*				����&��Ʒϵͳ�Ķ���	April.Lau 2008-07-11				*/                                                                     
/************************************************************************/

namespace KingNet { namespace Server {


enum enmGoodsMacro 
{
	invalid_item_none        = -1,
	invalid_item_id			= 0,

	max_item_name_length	 = 32,
	max_item_count_in_goods	 = 8,					//��Ʒ�����ܰ�������������Ŀ

#ifdef _DEBUG
	max_item_entry_count	= 500,
	max_goods_entry_count	= 100,
	max_charmitem_entry_count = 100,
	max_chest_entry_count	= 100,
#else
	max_item_entry_count	= 5000,
	max_goods_entry_count	= 4000,
	max_charmitem_entry_count = 2000,
	max_chest_entry_count	= 5000,
#endif // _DEBUG



// 	//ID Macro���
// 	item_id_base			 = 100000000,			//����ID��ʼ
// 	goods_id_base			 = 200000000,			//��ƷID��ʼ
// 
// 	item_id_total_scope		 = 100000000,			//����ID����ֵ�� 
// 	goods_id_total_scope	 = item_id_total_scope,	//��ƷID����ֵ��
// 
// 	item_id_scope			 = 10000,				//��ͬ���͵ĵ���IDֵ��			
// 	goods_id_scope			 = item_id_scope,		//��ͬ���͵���ƷIDֵ��
// 
// 	item_id_subscope		 = 1000,				//��ͬ�����͵ĵ���IDֵ���ر�ע��:������Ϸ����ID����ѭ�˹���
// 	goods_id_subscope		 = item_id_subscope,	//��ͬ�����͵���ƷIDֵ��

	/***************************************************************************************/
	/* ����(��Ʒ)ID��λ���ֲ���:														   */
	/* ���ڷ���Ϸ����� id = supertype(4bit) + type(4bit) + subtype(4bit) + subid(20bit)   */
	/* ������Ϸ�����   id = supertype(4bit) + type(4bit) + gameid(16bit) + subid(8bit)    */
	/***************************************************************************************/

	//
	id_supertype_mask			 = 0xf0000000,			//4bit��ʾ ����/��Ʒ��������
	id_supertype_shift_bits		 = 28,					//supertype =  ((id & id_supertype_mask) >> id_supertype_shift_bits)
	
	item_id_type_mask		 = 0x0f000000,				//4bit��ʾ ����/��Ʒ�ľ�������(����: ������Ʒ�࣬��ͨ��Ʒ���)
	
	goods_id_type_mask		 = item_id_type_mask,		//4bit��ʾ ����/��Ʒ�ľ�������
	id_type_shift_bits		 = 24,						//type = ((id&item_id_type_mask) >> id_type_shift_bits)


	item_id_subtype_mask	 = 0x00f00000,				//4bit��ʾ ����/��Ʒ��������(����: ������Ʒ���е��ʻ�����)
	item_id_subid_mask		 = 0x000fffff,				//20bit��ʾ������ӵ���id, �ӵ���id =  (id & item_id_subid_mask)

	goods_id_subtype_mask	 = item_id_subtype_mask,	//4bit��ʾ ����/��Ʒ��������(����: ������Ʒ���е��ʻ�����)
	id_subtype_shift_bits	 = 20,						//subtype = ((id&item_id_subtype_mask) >> id_subtype_shift_bits)

	//��Ϸ���⴦��
	item_id_game_subtype_gameid_mask	= 0x00ffff00,	//�����������Ϊ"������Ϸ����"ʱ��2Byte��ʾ������ϷID
	id_game_subtype_gameid_shift_bits	= 8,			//gameid = ((id&item_id_game_subtype_gameid_mask) >> id_game_subtype_gameid_shift_bits)
	item_game_subid_mask				= 0x000000ff,	//gameitem id = (id & item_game_subid_mask)

	goods_id_subid_mask = 0x000fffff,					//goods

	common_id_mask = 0xfff00000,						//ͨ��id�����͵�mask

};

enum enmMarketItemSuperType		//id�����4bit��ȡֵ
{
	item_super_type_invalid			= 0,
	item_super_type_item			= 1,			//������
	item_super_type_goods			= 2,			//��Ʒ��
	item_super_type_charmitem		= 3,			//������Ʒ��
	item_super_type_chest			= 4,			//���
	item_super_type_magic                   = 5,                    //ħ������
};



enum enmMagicEvent
{
	magic_event_enter = 1,
	magic_event_leave = 2,
	magic_event_victory =3,
	magic_event_fail =4,
	magic_event_kickplayer =5,
};

enum enmGoodsType		//��Ʒ����
{
	goods_type_invalid		= 0,			//
	//goods_type_charm		= 1,			//������Ʒ
	goods_type_common		= 1,			//��ͨ��ͨ�ã���Ʒ
	goods_type_service		= 2,			//��������Ʒ
	goods_type_vip			= 3,			//VIP������Ʒ�����磺����VIP�¿����꿨�ȣ�
	goods_type_game			= 4,			//��Ϸ����Ʒ			
	goods_type_package		= 5,			//�������Ʒ

	max_goods_type,
};

//��Ʒ��ṹ

struct stGoodsItem
{
	int16_t m_nCount;									//��Ŀ
	int32_t m_iItemID;									//����ID
};

struct stGoodsEntry
{
	int32_t	m_iID;										//��ƷID
	char	m_szName[max_item_name_length];				//��Ʒ����
	enmGoodsType m_nType;								//��Ʒ���� enmGoodsType
	int32_t m_iCommonPrice;								//��Ʒ��ͨ��
	int32_t m_iVIPPrice;								//��ƷVIP��
	int32_t m_iDiscountPrice;							//��Ʒ�ۿۼ�
	int32_t m_iRenewPrice;								//��Ʒ���Ѽ�(ֻ�Լ�ʱ�����) 
	int32_t m_iIcon;									//��Ʒͼ��
	int16_t m_nItemCount;								//��Ʒ����������Ŀ
	int32_t m_iPresentBeanCount;						//�������Ʒʱ�����ͻ��ֶ�������

	stGoodsItem m_astItems[max_item_count_in_goods];	//��Ʒ�����ĵ�����Ϣ
};

enum enmStackMode
{
	stack_mode_count		= 1,			//�ƴ�
	stack_mode_time			= 2,			//��ʱ
};


enum enmItemType		//��������
{
	item_type_invalid		= 0,
	//item_type_charm		= 1,			//��������(���磺�ʻ��࣬��ʯ��)
	item_type_common		= 1,			//��ͨ(ͨ��)����(���磺ը������С���ȵ�)
	item_type_service		= 2,			//���������(����: ˫�����ֿ����������)
	item_type_new_service	= 3,			//���������(����: VIP���񣬱���ͨ��֤��)
	item_type_game			= 4,			//������Ϸ�ڵ���

	max_item_type,
};

enum enmCommonItemSubType		//��ͨ���� ������
{
	common_item_subtype_invalid			= 0,		//
	common_item_subtype_bomb			= 1,		//��ͨ����:ը����
	common_item_subtype_loudspeaker		= 2,		//��ͨ����:С������
	common_item_subtype_hall_speaker	= 3,		//��ͨ����:����������
	common_item_subtype_game_speaker	= 4,		//��ͨ����:��Ϸ������
	common_item_subtype_reset_negative_point = 5,	//��ͨ����:����������

	max_common_item_subtype,
};

enum enmServiceItemSubType		//��������� ������
{
	service_item_subtype_invalid		= 0,	
	service_item_subtype_double_point	= 1,		//���������:˫�����ֿ���
	service_item_subtype_shield			= 2,		//���������:�������

	//added by aprilliu, 2009-06-06
	service_item_subtype_charming_passport = 3,		//��������ߣ�����ͨ��֤
	service_item_subtype_race_passport = 4,			//��������ߣ�����ͨ��֤   �����20bit��ʶ����ı���
   

	max_service_item_subtype,
};

enum enmNewServiceItemSubType
{
	new_service_item_subtype_invalid	= 0,
	new_service_item_subtype_vip		= 1,		//VIP�������:VIP Ŀǰֻ��һ��vip����
    new_service_item_subtype_race_passport = 2,			//��������ߣ�����ͨ��֤   �����20bit��ʶ����ı���
   
	max_new_service_item_subtype,
};

//���߱�ṹ
struct stItemEntry
{
	int32_t m_iID;										//����ID
	char m_szName[max_item_name_length];				//��������
	enmItemType m_nType;								//��������
	int32_t m_iIcon;									//����ͼ��
	
	int32_t m_iExpiredTime;								//���߹���ʱ��
	int32_t m_iEffectTime;								//��������ʱ��
	int16_t m_nGameID;									//�������õ���ϷID, -1 ��ʾͨ�õ���(ʹ��������Ϸ)
	
	int8_t m_cStack;									//���߶ѵ����ͣ�Ҳ����ʱ���ƴ����� enmStackMode
	int8_t m_cRenewFlag;								//���߿ɷ�����
	int8_t m_cEncashFlag;								//���߿ɷ�ض�

	int16_t m_nServiceID;								//���߶�Ӧ�ķ���ID
	int32_t m_iDeltaCharm;								//���߶�������Ӱ��ֵ����ȡ����ֵ
};

enum enmCharmItemType	//������Ʒ����
{
	charm_item_type_invalid			= 0,				//
	charm_item_type_common_flower	= 1,				//������Ʒ:��ͨ�ʻ�
	charm_item_type_special_flower	= 2,				//������Ʒ:�����ʻ�
	charm_item_type_gem				= 3,				//������Ʒ:��ʯ
	charm_item_type_misc			= 4,				//������Ʒ:����

	max_charm_item_type,		
};

//������Ʒ��ṹ
struct stCharmItemEntry
{
	int32_t m_iID;										//������ƷID
	char m_szName[max_item_name_length];				//������Ʒ����
	enmCharmItemType m_nType;							//������Ʒ������
	int32_t m_iIcon;									//������Ʒͼ��
	int32_t m_iDeltaCharm;								//������Ʒ������ֵ��Ӱ��
	int32_t m_iCommonPrice;								//������Ʒ��ͨ��
	int32_t m_iDiscountPrice;							//������Ʒ�ۿۼ�
	int32_t m_iVIPPrice;								//������ƷVIP��
	int32_t m_iEncashPrice;								//������Ʒ�ضҼ�
	int32_t m_iPresentBeanCount;						//�����������Ʒʱ�����ͻ��ֶ�������

	int8_t m_cNoteFlag;									//������Ʒ�Ƿ��б�ǩ

};


enum enmChestType	//��������
{
	chest_type_invalid	= 0,	//
	chest_type_common	= 1,	//

	max_chest_type,
};

//TODO 
//�����ṹ
struct stChestEntry
{
	int32_t m_iID;							//����ID
	char m_szName[max_item_name_length];	//��������
	enmChestType m_nType ;                  //��������
	int32_t m_iExpiredTime;					//�������ʱ��
	int32_t m_iIcon;
};


//���ֵ��ߺͶ�Ӧ��Ʒ��ID
enum enmItem
{
	//��ͨ����ID
	id_of_item_bomb				= 0x111FFFFF,		//287309823, ը��
	id_of_item_small_speaker	= 0x112FFFFF,		//288358399, С����(����������)
	id_of_item_big_speaker		= 0x113FFFFF,		//289406975, ��������
	id_of_item_game_speaker		= 0x114FFFFF,		//290455551, ��Ϸ����
	id_of_item_zero_negative_point = 0x115FFFFF,	//291504127, ��������


	//��Ϊ��Ʒ����ͨ����ID
	id_of_goods_bomb			= 0x211FFFFF,		//555745279, ը��
	id_of_goods_small_speaker	= 0x212FFFFF,		//556793855, С����(����������)
	id_of_goods_big_speaker		= 0x213FFFFF,		//557842431, ��������
	id_of_goods_game_speaker	= 0x214FFFFF,		//558891007, ��Ϸ����
	id_of_goods_zero_negative_point = 0x215FFFFF,	//559939583, ��������

};

//���������ö�����Ͷ���õ��ķ���id
enum enmPlatformService
{
	enmPlatformService_null			= 0x00000000,
	enmPlatformService_double_point	= 0x12100000,	//303038464, ˫�����ַ���
	enmPlatformService_shield		= 0x12200000,	//304087040, ���������
	enmPlatformService_vip			= 0x13100000,	//319815680, VIP����

	//added by aprilliu, 2009-06-06
	enmPlatformService_charming_passport = 0x12300000, //   ����ͨ��֤����
};

#define id_of_generic_item_small_speaker	(id_of_item_small_speaker & (~item_id_subid_mask))
#define id_of_generic_goods_small_speaker	(id_of_goods_small_speaker & (~goods_id_subid_mask))

#define id_of_generic_item_zero_negative_point  (id_of_item_zero_negative_point & (~item_id_subid_mask))
#define id_of_generic_goods_zero_negative_point	(id_of_goods_zero_negative_point & (~goods_id_subid_mask))

//some macro definition

//�õ���Ʒ����(suptertype)enmMarketItemSuperType
#define GET_ITEM_SUPER_TYPE(id)	enmMarketItemSuperType( ((id)&id_supertype_mask) >> id_supertype_shift_bits  )

//�õ���������enmItemType
#define GET_ITEM_TYPE(id)	enmItemType(((id)&item_id_type_mask) >> id_type_shift_bits)	

//�õ���Ʒ����enmGoodsType
#define GET_GOODS_TYPE(id)	enmGoodsType(((id)&goods_id_type_mask) >> id_type_shift_bits)	


//�õ���ͨ�������������
#define GET_COMMON_ITEM_SUBTYPE(id) enmCommonItemSubType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)	

//�õ������������������
#define GET_CHARM_ITEM_SUBTYPE(id) enmCharmItemType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)	

//�õ����������ķ�������
#define GET_SERVICE_ITEM_SUBTYPE(id) enmServiceItemSubType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)

//�õ�VIP����������������
#define GET_NEW_SERVICE_ITEM_SUBTYPE(id) enmNewServiceItemSubType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)

//���ڷǾ�����Ϸ���ߣ��õ����ߵ���ID(20bit)
#define GET_ITEM_SUBID(id)	int32_t((id)&item_id_subid_mask)

//���ھ�����Ϸ���ߣ��õ���ϷID
#define GET_GAME_ITEM_GAMEID(id) int16_t(((id)&item_id_game_subtype_gameid_mask) >> id_game_subtype_gameid_shift_bits)
//�õ�������Ϸ���ߵ���ID(8bit)
#define GET_GAME_ITEM_SUB_ITEMID(id) uint8_t((id)&item_game_subid_mask)

//��ȡ��Ʒ����ߵ�ǰ12bit��mask�����Ժ�20bitֵ
#define GET_COMMON_ID_MASK(id) int32_t((id) & common_id_mask)

//some compar function used by qsort & bsearch routine
int goods_entry_compare(const void* v1, const void* v2);
int item_entry_compare(const void* v1, const void* v2);
int charmitem_entry_compare(const void* v1, const void* v2);
int chest_entry_compare(const void* v1, const void* v2);

int32_t sort_goods_entries(stGoodsEntry* entries, int32_t count);
int32_t sort_item_entries(stItemEntry* entries, int32_t count);
int32_t sort_charmitem_entries(stCharmItemEntry* entries, int32_t count);
int32_t sort_chest_entries(stChestEntry* entries, int32_t count);

stGoodsEntry* search_goods_entry_by_id(int32_t id, stGoodsEntry* entries, int32_t count);
stItemEntry* search_item_entry_by_id(int32_t id, stItemEntry* entries, int32_t count);
stCharmItemEntry* search_charmitem_entry_by_id(int32_t id, stCharmItemEntry* entries, int32_t count);
stChestEntry* search_chest_entry_by_id(int32_t id, stChestEntry* entries, int32_t count);


}}//namespace KingNet { namespace Server {


#endif //__SERVER_ITEM_DEFINE_H__
