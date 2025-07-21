#ifndef _CL_BLACKMARKET_DEFINE_H_
#define _CL_BLACKMARKET_DEFINE_H_

#include <CLDefine.h>
#include <CLItemDefine.h>
#include <CLSystemValueDataEntry.h>
/**
*@brief 黑市商人类型
*/
enum BLACKMARKET_TYPE
{
	BMT_INVALID		  = 0,
	BMT_FIXED_PRICE   = 1, //固定价格
	BMT_AUCTION_PRICE = 2, //竞拍价格
};

/**
*@brief 黑市商人类型
*/
enum BLACKMARKET_SUBTYPE
{
	BMST_NORNAL = 0,	  //正常
	BMST_FIXED_UPLIMIT = 1,  //固定收购上限
};

/**
*@brief 竞拍结果
*/
enum BLACKMARKET_AUTION_RESULT
{
	BMTAR_INVAIL = 0, //未处理
	BMTAR_FALI   = 1, //失败,退还道具
	BMTAR_SUCC   = 2, //成功,发放金币
};

/**
*@brief 竞拍商品竞拍状态
*/
enum BLACKMARKET_AUCTION_ITEM_STATE
{
	BMAIS_INVALID     = 0,  //无效 
	BMAIS_CAN_AUCTION = 1,  //可以竞拍
	BMAIS_APPLYED	  = 2,  //已申请
	BMAIS_TRANSED	  = 3,  //已交易
	BMAIS_MAX, 
};

enum BLACKMARKET_AUCTION_GROUP_TYPE
{
	BMAGT_NONE	= 0,	
	BMAGT_ONE	= 1,	//1,20级粉装
	BMAGT_TWO	= 2,	//2,30级粉装
	BMAGT_THR	= 3,	//3,40级粉装
	BMAGT_FOUR	= 4,	//4,50级粉装
	BMAGT_FIR	= 5,	//5,55级粉装
	BMAGT_SIX	= 6,	//6,50级紫装
	BMAGT_SVEN	= 7,	//7,55级紫装
};

/**
*@brief 黑市商人收购物品项信息
*/
struct BlackMarketAuctionInfo
{
	BlackMarketAuctionInfo()
	{
		//memset(this, 0, sizeof(*this));
		guid = 0;
		op_activity_id = 0;
		back_buy_item_id = 0;
		back_buy_type = 0;
		price = 0;
		begin_time = 0;
		end_time = 0;
		off_sale = 0;
		recommend_price = 0;
		price_lower_limit = 0;
		price_upper_limit = 0;
		state = 0;
		auctioner_guid = 0;
		auction_price = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & back_buy_item_id & back_buy_type & price & begin_time & end_time & recommend_price 
			& price_lower_limit & price_upper_limit & state & auctioner_guid & auctioner_name & auction_price;
	}

	// 唯一id
	ObjID_t		guid;			
	// 所属运营活动id
	UInt32		op_activity_id;
	// 回购物品id
	UInt32		back_buy_item_id;
	// 回购类型
	UInt8		back_buy_type;
	// 固定收购价格
	UInt32		price;
	// 竞拍开始时间
	UInt32		begin_time;
	// 竞拍结束时间
	UInt32		end_time;
	// 是否下架
	UInt8		off_sale;
	// 推荐价格
	UInt32		recommend_price;
	// 价格下限
	UInt32		price_lower_limit;
	// 价格上限
	UInt32		price_upper_limit;
	// 状态(枚举BLACKMARKET_ITEM_AUCTION_STATE)
	UInt8		state;
	// 竞拍角色id
	ObjID_t		auctioner_guid;
	// 竞拍者名字
	std::string auctioner_name;
	// 报价
	UInt32		auction_price;
};

#define GAME_PARAM_BLACKMARKET_TYPE "game_param_blackmarket_type"

#define GAME_PARAM_BLACKMARKET_BUYLIMIT  "game_param_blackmarket_buylimit"

#define BLACKMARKET_BUYLIMIT_RATE 80

#define CHECK_TIME_FIXED_PRICE 3*24*3600

#define SERVER_OPEN_TIME 2*30*24*3600

#define AUCTION_END_TIME_FIX  5*60

//已经处理的订单保留时间
#define HANDLED_TRANS_PERSISIT_TIME  14*24*3600


#endif