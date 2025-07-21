#ifndef _CL_AUCTION_DEFINE_H_
#define _CL_AUCTION_DEFINE_H_

#include <CLDefine.h>
#include<memory>

/**
*@brief 拍卖类型
*/
enum AUCTION_ITEM_TYPE
{
	AUCTION_ITEM	= 0,	// 道具
	AUCTION_GOLD	= 1,	// 金贝
	AUCTION_SILVER	= 2,	// 银贝
};

/**
*@brief 拍卖类型
*/
enum AuctionType
{
    AT_ITEM,      // 道具
    AT_GOLD,      // 金币
    AT_NUM,
};

// 等级段数量
const static UInt32 LEVLE_STEP_NUM = 8;

// 金币拍卖每日上限
const static UInt32 DAILD_GOLD_AUCTION_SELL_MAX_NUM = 3;

/**
*@brief 拍卖行等级筛选
*/
const static UInt32 AUCTION_FILTER_LEVEL[LEVLE_STEP_NUM + 1] = {0, 29, 39, 49, 59, 69, 79, 89, 99};

/**
*@brief 拍卖行分类
*/
enum AUCTION_TYPE
{
	AUCTION_TYPE_ALL		= 0,	// 全部
	AUCTION_TYPE_WEAPON		= 1,	// 武器
	AUCTION_TYPE_ARMOR		= 2,	// 防具
	AUCTION_TYPE_ORNAMENT	= 3,	// 饰品
	AUCTION_TYPE_PET		= 4,	// 宠物
	AUCTION_TYPE_MOUNT		= 5,	// 坐骑
	AUCTION_TYPE_AEROBAT	= 6,	// 飞行器
	AUCTION_TYPE_GEM		= 7,	// 宝石
	AUCTION_TYPE_MONEY		= 8,	// 金贝&银贝
	AUCTION_TYPE_OTHER		= 9,	// 其他
};

/**
*@brief 拍卖行防具子类型
*/
enum AUCTION_ARMOR_SUBTYPE
{
	AUCTION_ARMOR_ALL		= 0,	// 全部
	AUCTION_ARMOR_CLOTHESE	= 1,	// 衣服
	AUCTION_ARMOR_HANDGUARD	= 2,	// 护手
	AUCTION_ARMOR_BELT		= 3,	// 腰带
	AUCTION_ARMOR_SHOES		= 4,	// 鞋子
	AUCTION_ARMOR_LEGGUARD	= 5,	// 护腿
};

/**
*@biref 拍卖行饰品子类型
*/
enum AUCTION_ORNAMENT_SUBTYPE
{
	AUCTION_ORNAMENT_ALL	= 0,	// 全部
	AUCTION_ORNAMENT_AMULET	= 1,	// 护身符
	AUCTION_ORNAMENT_TORQUE	= 2,	// 项链
	AUCTION_ORNAMENT_RING	= 3,	// 戒指
	AUCTION_ORNAMENT_BANGLE	= 4,	// 手镯
};

/**
*@brief 拍卖行宠物子类型
*/
enum AUCTION_PET_SUBTYPE
{
	AUCTION_PET_ALL			= 0,	// 全部
	AUCTION_PET_SKILL		= 1,	// 宠物技能道具
	AUCTION_PET_GROWTH		= 3,	// 宠物成长强化
	AUCTION_PET_EGG			= 4,	// 宠物蛋
	AUCTION_PET_OTHER		= 5,	// 其他宠物道具
};

// 每个玩家的最大拍卖道具数量
const static UInt32 MAX_AUCTION_NUM = 12;

// 拍卖费用
const static UInt32 AUCTION_COST = 100;

// 拍卖时间
const static UInt32	AUCTION_LAST_TIME = 1 * DAY_TO_SEC;

// 金贝道具ID
const static UInt32 GOLD_ITEM_ID = 600000001;

// 绑定点劵道具ID
const static UInt32 BIND_POINT_ITEM_ID = 600000008;

// 每次查询最多返回的条数
const static UInt32 MAX_AUCTION_PER_PAGE = 20;

// 每笔货币的最高拍卖量
const static UInt32 MAX_SELL_MONEY_NUM = 1000000;

// 最高定价
const static UInt32 MAX_AUCTION_PRICE = 1000000000;

// 道具拍卖行押金
const static UInt32 AUCTION_ITEM_DEPOSIT_COST = 500;

// 金币拍卖行最少金币数量
const static UInt32 AUCTION_GOLD_SELL_MIN_NUM = 100 * 10000;

// 金币拍卖行最多金币数量
const static UInt32 AUCTION_GOLD_SELL_MAX_NUM = 1000 * 10000;

struct AuctionBaseInfo
{
    AuctionBaseInfo()
    {
        memset(this, 0, sizeof(*this));
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & guid & price & pricetype & num & itemTypeId & strengthen 
			& itemScore & publicEndTime & isTreas & owner & attent & attentNum 
			& duetime & onSaleTime & beadBuffId & isAgainOnsale & equipType & enhanceType
			& itemDueTime & enhanceNum & itemTransNum;
    }

    // 唯一id
    ObjID_t		guid;			//这里拍卖的ID直接用物品的ID
    UInt32		price;
    UInt8		pricetype;
    UInt32		duetime;		//拍卖的到期时间(绝对时间)
	UInt32		onSaleTime;		//上架时间（可以被别人看到的时间）(珍品时也为公示结束时间)

    UInt32      num;
    UInt32      itemTypeId;
    UInt32      strengthen;		// 强化等级
	UInt32		itemScore;		// 装备评分

	UInt8		goodState;		// 商品状态
	UInt8		buyState;		// 购买状态
	UInt32		onPublicTime;	// 公示开始时间戳
	UInt32		publicEndTime;	// 公示结束时间
	UInt8		isTreas;		// 是否是珍品
	UInt8		itemtype;		// 物品主类型
	UInt32		subType;		// 物品子类型
	UInt32		couponStrengthenTo;	//强化卷+强化等级
	UInt32		beadBuffId;			//宝珠附加buffid
	ObjID_t		owner;			// 卖家角色id
	UInt8		attent;			// 关注[0]未关注 [1]关注
	UInt32		attentNum;		// 被关注人数
	UInt32      attentTime;		// 关注时间戳
	UInt8		offsaleHanled;  // 是否做过下架处理
	UInt8		isAgainOnsale;	// 是否重新上架
	UInt8		equipType;		// 装备类型(EquipType):普通0/带气息1/红字2
	UInt8		enhanceType;	// 红字装备增幅类型 力量1/智力2/体力3/精神4
	UInt32		itemDueTime;    // 道具过期时间戳
	UInt32		enhanceNum;		// 增幅装备属性增加值
	UInt32		itemTransNum;	// 已经交易次数
	UInt32		auctionSubType;  //拍卖行子类型
	UInt8		quality;		//品质
	UInt16		itemLevel;		//需要等级
};

struct AuctionBaseCmpForItemIdSet
{
	bool operator()(const AuctionBaseInfo* left, const AuctionBaseInfo* right) const
	{
		if (!left || !right)
		{
			return false;
		}
		if (left->guid == right->guid)
		{
			return false;
		}

		if (left->num != 0 && right->num != 0 && (left->price / left->num) != (right->price / right->num))
		{
			return (left->price / left->num) < (right->price / right->num);
		}

		if (left->price != right->price)
		{
			return left->price < right->price;
		}

		return left->guid < right->guid;
	}
};

enum AuctionRefreshReason
{
	SRR_BUY,
	SRR_SELL,
	SRR_CANCEL,
	SRR_RUSY_BUY,
	SRR_SYS_RECOVERY,	//扫货
};

// 拍卖行道具基本信息（类型，数量）
struct AuctionItemBaseInfo
{
	AuctionItemBaseInfo()
	{
		itemTypeId = 0;
		num = 0;
		isTreas = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemTypeId & num & isTreas;
	}

	UInt32		itemTypeId;
	UInt32		num;
	UInt8		isTreas;
};

inline UInt32 GetAuctionStrengthAddition(UInt8 strengthen)
{
	UInt32 addition = 0;
	//UInt32 strengthAddition[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 30, 60, 100, 150, 200, 300, 400, 500, 600, 700 };
	//UInt32 strengthAddition[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 60, 100, 150, 200, 300, 400, 500, 600, 700 };
	UInt32 strengthAddition[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 60, 150, 300, 400, 500, 600, 700, 800, 900 };
	UInt32 additionNum = sizeof(strengthAddition) / sizeof(strengthAddition[0]);
	if (strengthen >= sizeof(strengthAddition) / sizeof(strengthAddition[0]))
	{
		addition = strengthAddition[additionNum - 1];
	}
	else
	{
		addition = strengthAddition[strengthen];
	}

	return addition;
}

//拍卖行物品拍卖状态
enum AuctionGoodState
{
	AGS_INVALID, //无效
	AGS_PUBLIC,  //公示
	AGS_ONSALE,	 //上架
	AGS_NUM,
};

//拍卖行物品购买状态
enum AuctionGoodBuyState
{
	AGBS_INVALID,			//不能购买
	AGBS_RUSH_BUY,			//抢购
	AGBS_RUSH_BUY_DELIVER,  //抢购发货中
	AGBS_NORMAL_BUY,		//正常购买
	AGBS_NUM,
};

enum AuctionTransactionType
{
	ATT_INVAIL = 0,
	ATT_SELL = 1, //寄售
	ATT_BUY = 2, //购买
};

//拍卖行交易记录
struct AuctionTransaction
{
	AuctionTransaction()
	{
		type = 0;
		item_id = 0;
		item_num = 0;
		item_score = 0;
		unit_price = 0;
		verify_end_time = 0;
		enhance_type = 0;
		strength = 0;
		qualitylv = 0;
		trans_time = 0;
		beadbuffId = 0;
		mountBeadId = 0;
		mountMagicCardId = 0;
		mountMagicCardLv = 0;
		equipType = 0;
		enhanceNum = 0;

	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & item_id & item_num & item_score & unit_price & verify_end_time & enhance_type & strength 
			& qualitylv & trans_time & beadbuffId & mountBeadId & mountBeadBuffId & mountMagicCardId & mountMagicCardLv
			& equipType & enhanceNum;
	}
	UInt8		type;		//[1]:寄售,[2]购买
	UInt32		item_id;	//物品id
	UInt32		item_num;	//物品数量
	UInt32		item_score; //物品评分
	UInt32		unit_price; //交易物品单价
	UInt32		verify_end_time; //到账时间
	UInt8       enhance_type;	//enhance类型
	UInt8       strength;		//强化等级
	UInt8		qualitylv;		//品级
	UInt32		trans_time;		//交易时间
	UInt32      beadbuffId;		//附加buffid(宝珠)
	UInt32      mountBeadId;	//镶嵌宝珠id
	UInt32      mountBeadBuffId;  //镶嵌宝珠的附加buffid
	UInt32		mountMagicCardId; //镶嵌附魔卡id
	UInt8		mountMagicCardLv; //镶嵌附魔卡等级
	UInt8		equipType;		  //装备类型(EquipType):普通0/带气息1/红字2
	UInt32		enhanceNum;		  //增幅装备属性增加值
};

//抢购发货结果
enum AuctionRushBuyFailType
{
	ARBFT_INVAIL = 0,       //无效
	ARBFT_OVERTIME = 1,     //超时
	ARBFT_CHECK_FAIL = 2,   //scene检查失败
	ARBFT_DELIVER_FAIL = 3, //发货失败
	ARBFT_TRANSOBJ_DES = 4,	//事务销毁
};

//拍卖行关注操作
enum AuctionAttentOpType
{
	ATOT_ATTENT = 1,		//关注
	ATOT_CANCEL = 2,		//取消关注
};

//拍卖行关注类型
enum AuctionAttentType
{
	ATT_NOTATTENT = 0,	//非关注
	ATT_ATTENT	  = 1,	//关注
};

//拍卖行在售附魔卡数量
struct AuctionMagicOnsale
{
	AuctionMagicOnsale()
	:strength(0), num(0){}
	AuctionMagicOnsale(UInt8 strength_, UInt32 num_)
	:strength(strength_), num(num_){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & strength & num;
	}

	UInt8  strength;
	UInt32 num;
};

#define GAME_PARAM_AUCTION_CURRDAY_COUNTERFEE  "auction_curday_counterfee"
#define GAME_PARAM_AUCTION_YESTERDAY_COUNTERFEE "auction_yesterday_counterfee"

#define PARAM_NAME_AUCTION_RECOVE_TREAS_NUM "auction_rec_treas_num"
#define PARAM_NAME_AUCTION_RECOVE_TREAS_TIMESTAMP "auction_rec_treas_time"

#define PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_NUM "auction_rec_nottreas_num"
#define PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_TIMESTAMP "auction_rec_nottreas_time"

enum AuctionPriceListOpType
{
	APLOP_ON_SALE = 1,	//上架
	APLOP_SWITCH = 2,	//切换在售,公示在售价格列表
};

template <typename T>
inline bool CompareVectorEqual(const std::vector<T>& v1, const std::vector<T>& v2)
{
	if (v1.size() == 0 
		&& v1.size() == v2.size())
	{
		return true;
	}
	if (v1.size() != v2.size())
	{
		return false;
	}
		
	for (UInt32 i = 0; i < v1.size(); ++i)
	{
		if (v1[i] != v2[i])
		{
			return false;
		}
	}

	return true;
}

template <typename T>
inline bool FindInVector(const T& e, const std::vector<T>& v)
{
	for (UInt32 i = 0; i < v.size(); ++i)
	{
		if (v[i] == e)
		{
			return true;
		}
	}
	return false;
}

struct QueryAuctionItemNumSt
{
	QueryAuctionItemNumSt()
	{
		itemTypeId = 0;
		isTreas = 0;
		quality = 0;
		itemLevel = 0;
		num = std::make_shared<UInt32>(0);
	}
	bool operator<(const QueryAuctionItemNumSt& other) const
	{
		if (other.quality != quality)
		{
			return quality > other.quality;
		}
		if (other.itemLevel != itemLevel)
		{
			return itemLevel < other.itemLevel;
		}
		return itemTypeId < other.itemTypeId;
	}
	UInt32					itemTypeId;
	std::shared_ptr<UInt32> num;
	UInt8					isTreas;
	UInt8					quality;
	UInt16					itemLevel;
};

#endif

