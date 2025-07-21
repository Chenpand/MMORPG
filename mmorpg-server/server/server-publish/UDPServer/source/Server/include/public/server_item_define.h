#ifndef __SERVER_ITEM_DEFINE_H__
#define __SERVER_ITEM_DEFINE_H__

#include "gameplatform_types.h"

#include "Common/common_base.h"

/************************************************************************/
/*				道具&商品系统的定义	April.Lau 2008-07-11				*/                                                                     
/************************************************************************/

namespace KingNet { namespace Server {


enum enmGoodsMacro 
{
	invalid_item_none        = -1,
	invalid_item_id			= 0,

	max_item_name_length	 = 32,
	max_item_count_in_goods	 = 8,					//商品中所能包含的最大道具数目

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



// 	//ID Macro相关
// 	item_id_base			 = 100000000,			//道具ID起始
// 	goods_id_base			 = 200000000,			//商品ID起始
// 
// 	item_id_total_scope		 = 100000000,			//道具ID的总值域 
// 	goods_id_total_scope	 = item_id_total_scope,	//商品ID的总值域
// 
// 	item_id_scope			 = 10000,				//不同类型的道具ID值域			
// 	goods_id_scope			 = item_id_scope,		//不同类型的商品ID值域
// 
// 	item_id_subscope		 = 1000,				//不同子类型的道具ID值域。特别注意:具体游戏道具ID不遵循此规律
// 	goods_id_subscope		 = item_id_subscope,	//不同子类型的商品ID值域

	/***************************************************************************************/
	/* 道具(商品)ID的位划分策略:														   */
	/* 对于非游戏类道具 id = supertype(4bit) + type(4bit) + subtype(4bit) + subid(20bit)   */
	/* 对于游戏类道具   id = supertype(4bit) + type(4bit) + gameid(16bit) + subid(8bit)    */
	/***************************************************************************************/

	//
	id_supertype_mask			 = 0xf0000000,			//4bit表示 道具/商品两种类型
	id_supertype_shift_bits		 = 28,					//supertype =  ((id & id_supertype_mask) >> id_supertype_shift_bits)
	
	item_id_type_mask		 = 0x0f000000,				//4bit表示 道具/商品的具体类型(比如: 魅力物品类，普通物品类等)
	
	goods_id_type_mask		 = item_id_type_mask,		//4bit表示 道具/商品的具体类型
	id_type_shift_bits		 = 24,						//type = ((id&item_id_type_mask) >> id_type_shift_bits)


	item_id_subtype_mask	 = 0x00f00000,				//4bit表示 道具/商品的子类型(比如: 魅力物品类中的鲜花子类)
	item_id_subid_mask		 = 0x000fffff,				//20bit表示具体的子道具id, 子道具id =  (id & item_id_subid_mask)

	goods_id_subtype_mask	 = item_id_subtype_mask,	//4bit表示 道具/商品的子类型(比如: 魅力物品类中的鲜花子类)
	id_subtype_shift_bits	 = 20,						//subtype = ((id&item_id_subtype_mask) >> id_subtype_shift_bits)

	//游戏特殊处理
	item_id_game_subtype_gameid_mask	= 0x00ffff00,	//对与道具类型为"具体游戏道具"时，2Byte表示具体游戏ID
	id_game_subtype_gameid_shift_bits	= 8,			//gameid = ((id&item_id_game_subtype_gameid_mask) >> id_game_subtype_gameid_shift_bits)
	item_game_subid_mask				= 0x000000ff,	//gameitem id = (id & item_game_subid_mask)

	goods_id_subid_mask = 0x000fffff,					//goods

	common_id_mask = 0xfff00000,						//通用id的类型的mask

};

enum enmMarketItemSuperType		//id的最高4bit的取值
{
	item_super_type_invalid			= 0,
	item_super_type_item			= 1,			//道具类
	item_super_type_goods			= 2,			//商品类
	item_super_type_charmitem		= 3,			//魅力物品类
	item_super_type_chest			= 4,			//活动类
	item_super_type_magic                   = 5,                    //魔法表情
};



enum enmMagicEvent
{
	magic_event_enter = 1,
	magic_event_leave = 2,
	magic_event_victory =3,
	magic_event_fail =4,
	magic_event_kickplayer =5,
};

enum enmGoodsType		//商品类型
{
	goods_type_invalid		= 0,			//
	//goods_type_charm		= 1,			//魅力商品
	goods_type_common		= 1,			//普通（通用）商品
	goods_type_service		= 2,			//服务类商品
	goods_type_vip			= 3,			//VIP服务商品（比如：各种VIP月卡，年卡等）
	goods_type_game			= 4,			//游戏类商品			
	goods_type_package		= 5,			//打包类商品

	max_goods_type,
};

//商品表结构

struct stGoodsItem
{
	int16_t m_nCount;									//数目
	int32_t m_iItemID;									//道具ID
};

struct stGoodsEntry
{
	int32_t	m_iID;										//商品ID
	char	m_szName[max_item_name_length];				//商品名字
	enmGoodsType m_nType;								//商品类型 enmGoodsType
	int32_t m_iCommonPrice;								//商品普通价
	int32_t m_iVIPPrice;								//商品VIP价
	int32_t m_iDiscountPrice;							//商品折扣价
	int32_t m_iRenewPrice;								//商品续费价(只对计时类道具) 
	int32_t m_iIcon;									//商品图标
	int16_t m_nItemCount;								//商品包含道具数目
	int32_t m_iPresentBeanCount;						//购买该商品时，赠送欢乐豆的数量

	stGoodsItem m_astItems[max_item_count_in_goods];	//商品包含的道具信息
};

enum enmStackMode
{
	stack_mode_count		= 1,			//计次
	stack_mode_time			= 2,			//计时
};


enum enmItemType		//道具类型
{
	item_type_invalid		= 0,
	//item_type_charm		= 1,			//魅力道具(比如：鲜花类，宝石类)
	item_type_common		= 1,			//普通(通用)道具(比如：炸弹，大小喇叭等)
	item_type_service		= 2,			//服务类道具(比如: 双倍积分卡，护身符等)
	item_type_new_service	= 3,			//服务类道具(比如: VIP服务，比赛通行证等)
	item_type_game			= 4,			//具体游戏内道具

	max_item_type,
};

enum enmCommonItemSubType		//普通道具 子类型
{
	common_item_subtype_invalid			= 0,		//
	common_item_subtype_bomb			= 1,		//普通道具:炸弹类
	common_item_subtype_loudspeaker		= 2,		//普通道具:小喇叭类
	common_item_subtype_hall_speaker	= 3,		//普通道具:大厅喇叭类
	common_item_subtype_game_speaker	= 4,		//普通道具:游戏喇叭类
	common_item_subtype_reset_negative_point = 5,	//普通道具:负分清零类

	max_common_item_subtype,
};

enum enmServiceItemSubType		//服务类道具 子类型
{
	service_item_subtype_invalid		= 0,	
	service_item_subtype_double_point	= 1,		//服务类道具:双倍积分卡类
	service_item_subtype_shield			= 2,		//服务类道具:护身符类

	//added by aprilliu, 2009-06-06
	service_item_subtype_charming_passport = 3,		//服务类道具：魅力通行证
	service_item_subtype_race_passport = 4,			//服务类道具：比赛通行证   后面的20bit标识具体的比赛
   

	max_service_item_subtype,
};

enum enmNewServiceItemSubType
{
	new_service_item_subtype_invalid	= 0,
	new_service_item_subtype_vip		= 1,		//VIP服务道具:VIP 目前只有一种vip服务
    new_service_item_subtype_race_passport = 2,			//服务类道具：比赛通行证   后面的20bit标识具体的比赛
   
	max_new_service_item_subtype,
};

//道具表结构
struct stItemEntry
{
	int32_t m_iID;										//道具ID
	char m_szName[max_item_name_length];				//道具名字
	enmItemType m_nType;								//道具类型
	int32_t m_iIcon;									//道具图标
	
	int32_t m_iExpiredTime;								//道具过期时间
	int32_t m_iEffectTime;								//道具作用时间
	int16_t m_nGameID;									//道具适用的游戏ID, -1 表示通用道具(使用所有游戏)
	
	int8_t m_cStack;									//道具堆叠类型，也即计时，计次类型 enmStackMode
	int8_t m_cRenewFlag;								//道具可否续费
	int8_t m_cEncashFlag;								//道具可否回兑

	int16_t m_nServiceID;								//道具对应的服务ID
	int32_t m_iDeltaCharm;								//道具对魅力的影响值，可取正负值
};

enum enmCharmItemType	//魅力物品类型
{
	charm_item_type_invalid			= 0,				//
	charm_item_type_common_flower	= 1,				//魅力物品:普通鲜花
	charm_item_type_special_flower	= 2,				//魅力物品:特殊鲜花
	charm_item_type_gem				= 3,				//魅力物品:宝石
	charm_item_type_misc			= 4,				//魅力物品:其他

	max_charm_item_type,		
};

//魅力物品表结构
struct stCharmItemEntry
{
	int32_t m_iID;										//魅力物品ID
	char m_szName[max_item_name_length];				//魅力物品名字
	enmCharmItemType m_nType;							//魅力物品子类型
	int32_t m_iIcon;									//魅力物品图标
	int32_t m_iDeltaCharm;								//魅力物品对魅力值的影响
	int32_t m_iCommonPrice;								//魅力物品普通价
	int32_t m_iDiscountPrice;							//魅力物品折扣价
	int32_t m_iVIPPrice;								//魅力物品VIP价
	int32_t m_iEncashPrice;								//魅力物品回兑价
	int32_t m_iPresentBeanCount;						//购买该魅力物品时，赠送欢乐豆的数量

	int8_t m_cNoteFlag;									//魅力物品是否有便签

};


enum enmChestType	//宝箱类型
{
	chest_type_invalid	= 0,	//
	chest_type_common	= 1,	//

	max_chest_type,
};

//TODO 
//宝箱表结构
struct stChestEntry
{
	int32_t m_iID;							//宝箱ID
	char m_szName[max_item_name_length];	//宝箱名字
	enmChestType m_nType ;                  //宝箱类型
	int32_t m_iExpiredTime;					//宝箱过期时间
	int32_t m_iIcon;
};


//几种道具和对应商品的ID
enum enmItem
{
	//普通道具ID
	id_of_item_bomb				= 0x111FFFFF,		//287309823, 炸弹
	id_of_item_small_speaker	= 0x112FFFFF,		//288358399, 小喇叭(房间内喇叭)
	id_of_item_big_speaker		= 0x113FFFFF,		//289406975, 大厅喇叭
	id_of_item_game_speaker		= 0x114FFFFF,		//290455551, 游戏喇叭
	id_of_item_zero_negative_point = 0x115FFFFF,	//291504127, 负分清零


	//作为商品的普通道具ID
	id_of_goods_bomb			= 0x211FFFFF,		//555745279, 炸弹
	id_of_goods_small_speaker	= 0x212FFFFF,		//556793855, 小喇叭(房间内喇叭)
	id_of_goods_big_speaker		= 0x213FFFFF,		//557842431, 大厅喇叭
	id_of_goods_game_speaker	= 0x214FFFFF,		//558891007, 游戏喇叭
	id_of_goods_zero_negative_point = 0x215FFFFF,	//559939583, 负分清零

};

//根据上面的枚举类型定义得到的服务id
enum enmPlatformService
{
	enmPlatformService_null			= 0x00000000,
	enmPlatformService_double_point	= 0x12100000,	//303038464, 双倍积分服务
	enmPlatformService_shield		= 0x12200000,	//304087040, 护身符服务
	enmPlatformService_vip			= 0x13100000,	//319815680, VIP服务

	//added by aprilliu, 2009-06-06
	enmPlatformService_charming_passport = 0x12300000, //   魅力通行证服务
};

#define id_of_generic_item_small_speaker	(id_of_item_small_speaker & (~item_id_subid_mask))
#define id_of_generic_goods_small_speaker	(id_of_goods_small_speaker & (~goods_id_subid_mask))

#define id_of_generic_item_zero_negative_point  (id_of_item_zero_negative_point & (~item_id_subid_mask))
#define id_of_generic_goods_zero_negative_point	(id_of_goods_zero_negative_point & (~goods_id_subid_mask))

//some macro definition

//得到物品类型(suptertype)enmMarketItemSuperType
#define GET_ITEM_SUPER_TYPE(id)	enmMarketItemSuperType( ((id)&id_supertype_mask) >> id_supertype_shift_bits  )

//得到道具类型enmItemType
#define GET_ITEM_TYPE(id)	enmItemType(((id)&item_id_type_mask) >> id_type_shift_bits)	

//得到商品类型enmGoodsType
#define GET_GOODS_TYPE(id)	enmGoodsType(((id)&goods_id_type_mask) >> id_type_shift_bits)	


//得到普通道具类的子类型
#define GET_COMMON_ITEM_SUBTYPE(id) enmCommonItemSubType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)	

//得到魅力道具类的子类型
#define GET_CHARM_ITEM_SUBTYPE(id) enmCharmItemType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)	

//得到服务道具类的服务类型
#define GET_SERVICE_ITEM_SUBTYPE(id) enmServiceItemSubType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)

//得到VIP服务道具类的子类型
#define GET_NEW_SERVICE_ITEM_SUBTYPE(id) enmNewServiceItemSubType(((id)&item_id_subtype_mask) >> id_subtype_shift_bits)

//对于非具体游戏道具，得到道具的子ID(20bit)
#define GET_ITEM_SUBID(id)	int32_t((id)&item_id_subid_mask)

//对于具体游戏道具，得到游戏ID
#define GET_GAME_ITEM_GAMEID(id) int16_t(((id)&item_id_game_subtype_gameid_mask) >> id_game_subtype_gameid_shift_bits)
//得到具体游戏道具的子ID(8bit)
#define GET_GAME_ITEM_SUB_ITEMID(id) uint8_t((id)&item_game_subid_mask)

//获取商品或道具的前12bit的mask，忽略后20bit值
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
