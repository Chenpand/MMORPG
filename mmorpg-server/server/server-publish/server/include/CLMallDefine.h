#ifndef _CL_MALL_DEFINE_H_
#define _CL_MALL_DEFINE_H_

#include "CLItemDefine.h"
#include "TimeUtil.h"

enum TagType
{
	TT_HOT = 1,	// 热门
	TT_WHOLE_BARGAIN = 2,	// 全民砍价活动
};

// 商城子类别
enum MallSubType
{
	MST_INVALID = 0, // 无类别
	MST_HEAD = 1,
	MST_UPWEAR = 2,
	MST_CHEST = 3,
	MST_DOWNWEAR = 4,
	MST_BELT = 5,
	MST_FASHION_LIMIT = 8,    //限定商城
};

// 商城类别
enum MallType
{
	MT_INVALID,
	MT_FASHION = 4,				// 时装
	MT_GOLD = 5,				// 金币
	MT_LIMIT_GIFT_PACK = 6,		// 限时礼包
	MT_GIFT_PACK_ACTIVITY = 7,	// 限时礼包(活动)
	MT_FLYUP_GIFT = 20,			// 飞升礼包
	MT_DEDUCTION_COUPON = 24,	// 抵扣券
};

/**
*@brief 商城购买获得物类型
*/
enum MallBuyGotType
{
	// 无
	MALL_BUY_GOT_TYPE_NONE = 0,
	// 地精纪念币
	MALL_BUY_GOT_TYPE_GNOME_COIN = 1,

	MALL_BUY_GOT_TYPE_MAX,
};

/**
*@brief 商城购买获得物信息
*/
struct MallBuyGotInfo
{
	MallBuyGotInfo() :buyGotType(0), itemDataId(0), buyGotNum(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & buyGotType & itemDataId & buyGotNum;
	}

	// 购买获得物类型(MallBuyGotType)
	UInt8 buyGotType;
	// 购买获得物对应的道具id
	UInt32 itemDataId;
	// 购买获得物数量
	UInt32 buyGotNum;
};

typedef std::vector<MallBuyGotInfo> MallBuyGotInfoVec;

/**
*@brief 商城道具信息
*/
struct MallItemInfo
{
	MallItemInfo()
	{
		id = 0;
		type = 0;
		subType = 0;
		jobType = 0;
		itemid = 0;
		itemnum = 0;
		price = 0;
		discountprice = 0;
		moneytype = 0;
		bLimit = 0;
		limitnum = 0;
		goodsType = 0;
		vipScore = 0;
		timeCalcType = 0;
		startTime = 0;
		endTime = 0;
		limitTotalNum = 0;
		tagType = 0;
		sortIdx = 0;
		hotSortIdx = 0;
		goodsSubType = 0;
		isRecommend = 0;
		personalTailorActivateCond = 0;
		discountRate = 0;
		loginPushId = 0;
		playerLevelLimit = 0;
		mallPushCond = 0;
		accountRefreshType = TimeUtil::REFRESH_TYPE_NONE;
		accountLimitBuyNum = 0;
		accountRestBuyNum = 0;
		discountCouponId = 0;
		multiple = 0;
		multipleEndTime = 0;
		deductionCouponId = 0;
		creditPointDeduction = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & subType & jobType & itemid & itemnum & price & discountprice & \
			moneytype & bLimit & limitnum & goodsType & vipScore & icon & startTime & endTime & \
			limitTotalNum & rewards & giftName & tagType & sortIdx & hotSortIdx & goodsSubType & \
			isRecommend & personalTailorActivateCond & discountRate & loginPushId & fashionImagePath & \
			desc & buyGotInfos & params & accountRefreshType & accountLimitBuyNum & accountRestBuyNum & \
			discountCouponId & multiple & multipleEndTime & deductionCouponId & creditPointDeduction;
	}
	
	const std::string LogStr() const
	{
		std::ostringstream oss;
		oss << "MallItemInfo(" << id << ", " << (UInt32)type << ", " << (UInt32)subType << ", " << (UInt32)jobType << ")";
		return oss.str();
	}

	// id
	UInt32	id;
	//类别
	UInt8	type;
	//子类别
	UInt8	subType;
	//职业类别
	UInt8	jobType;
	// 道具id
	UInt32	itemid;
	// 一组数量
	UInt32	itemnum;
	// 原价
	UInt32	price;
	// 折扣价
	UInt32	discountprice;
	// 货币类型
	UInt8	moneytype;
	// 限购类型
	UInt8	bLimit;
	// 限购数量
	UInt16	limitnum;
	//商品类型
	UInt8	goodsType;
	// 购买后获得的vip积分
	UInt16		vipScore;
	// icon
	std::string	icon;
	// 时间计算类型(对应枚举TimeUtil::TimestampCalcType)
	UInt8		timeCalcType;
	// 开始时间
	UInt32		startTime;
	// 结束时间
	UInt32		endTime;
	// 开启间隔
	std::string	openInterval;
	// 关闭间隔
	std::string	closeInterval;
	// 总限购次数
	UInt16		limitTotalNum;
	// 礼包内容
	ItemRewardVec rewards;
	// 礼包名字
	std::string	giftName;
	// 标签,1.热门
	UInt8	tagType;
	// 排序序号
	UInt32	sortIdx;
	// 热门排序序号
	UInt32	hotSortIdx;
	// 商品子类别
	UInt16 goodsSubType;
	// 是否推荐
	UInt8 isRecommend;
	// 私人订制条件
	UInt8 personalTailorActivateCond;
	// 打折率
	UInt32 discountRate;
	// 登录推送表id
	UInt8 loginPushId;
	// 时装推荐图片路径
	std::string fashionImagePath;
	// 描述
	std::string desc;
	// 购买获得物信息
	MallBuyGotInfoVec buyGotInfos;
	// 扩展参数
	std::vector<UInt32>	params;
	// 玩家等级限制
	UInt16 playerLevelLimit;
	// 商城推送条件
	UInt8 mallPushCond;
	// 账号刷新类型(TimeUtil::RefreshType)
	UInt8 accountRefreshType;
	// 账号刷新日期
	TimeUtil::RefreshDate accountRefreshDate;
	// 账号限购次数
	UInt32 accountLimitBuyNum;
	// 账号剩余购买次数
	UInt32 accountRestBuyNum;
	// 折扣券id
	UInt32 discountCouponId;
	// 多倍积分倍率
	UInt8 multiple;
	// 多倍结束时间
	UInt32 multipleEndTime;
	// 抵扣卷id
	UInt32 deductionCouponId;
	// 是否使用信用点券
	UInt32 creditPointDeduction;
};

typedef std::vector<MallItemInfo> MallItemInfosVec;


/**
*@brief 商城礼包详细
*/
struct MallGiftDetail
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & num;
	}

	UInt32 itemId;
	UInt16 num;
};

/**
*@brief 商城批量购买上下文
*/
struct MallBatchBuyCtx
{
	MallBatchBuyCtx()
	{
		m_itemUids.clear();
		m_totleNum = 0;
	}

	std::vector<UInt64>	m_itemUids;		// 已经购买完的道具uid
	UInt32 m_totleNum;					// 批量购买总数
};

/**
*@brief 条件触发类型
*/
enum ConditionActivateType
{
	// 触发限时礼包
	CAT_MALL_LIMIT_TIME_GIFT_PACK = 0,
	// 触发私人订制
	CAT_MALL_PERSONAL_TAILOR,
};

/**
*@brief 条件触发数据库加载状态
*/
enum ConditionActivateDBLoadState
{
	// 未加载
	CADBLS_NONE = 0,
	// 加载完毕
	CADBLS_DONE,
};

/**
*@brief 商城限时礼包激活条件
*/
enum MallGiftPackActivateCond
{
	MGPAC_INVALID = 0,
	MGPAC_STRENGEN_TEN,					//强化到10
	MGPAC_STRENGEN_NO_RESOURCE,			//强化资源不足
	MGPAC_NO_QUILTY_ADJUST_BOX,			//品级调整箱不足
	MGPAC_NO_FATIGUE,					//疲劳不足，有月卡且背包中无疲劳药水
	MGPAC_NO_HELL_TICKET,				//刷深渊门票不足
	MGPAC_NO_ANCIENT_TICKET,			//刷远古门票不足
	MGPAC_DIE,							//死亡
	MGPAC_STRENGEN_BROKE_TEN,			//强化装备碎掉,推送10级装备
	MGPAC_STRENGEN_BROKE_FIFTEEN,		//强化装备碎掉,推送15级装备
	MGPAC_STRENGEN_BROKE_TWENTY,		//强化装备碎掉,推送20级装备
	MGPAC_STRENGEN_BROKE_TWENTY_FIVE,	//强化装备碎掉,推送25级装备
	MGPAC_STRENGEN_BROKE_THIRTY,		//强化装备碎掉,推送30级装备
	MGPAC_STRENGEN_BROKE_THIRTY_FIVE,	//强化装备碎掉,推送35级装备
	MGPAC_STRENGEN_BROKE_FORTY,			//强化装备碎掉,推送40级装备
	MGPAC_STRENGEN_BROKE_FORTY_FIVE,	//强化装备碎掉,推送45级装备
	MGPAC_STRENGEN_BROKE_FIFTY,			//强化装备碎掉,推送50级装备
	MGPAC_PLAYER_LEVEL_UP_GET_GIFT,		//玩家升级获得礼包
};

/**
*@brief 商城限购类型
*/
enum MallLimitBuyType
{
	MLBT_INVALID = 0,		//不限购
	MLBT_DAILY_REFRESH,		//每日刷新限购
	MLBT_NO_REFRESH,		//不刷新限购
	MLBT_WEEKLY_REFRESH,	//每周刷新限购
};

/**
*@brief 商城限时礼包激活数目限制
*/
enum MallGiftPackLimitNum
{
	MGPLN_INVALID,
	MGPLN_ONCE = 1,			//触发一次
	MGPLN_THREE_TIMES = 3,	//触发三次
};

/**
*@brief 商城商品类型
*/
enum MallGoodsType
{
	MGT_INVALID = 0,					//普通商品
	MGT_GIFT_DAILY_REFRESH,				//人民币礼包：可每日刷新
	MGT_GIFT_ACTIVATE_ONCE,				//礼包：账号激活限制一次
	MGT_GIFT_ACTIVATE_THREE_TIMES,		//礼包：账号激活限制三次
	MGT_COMMON_CHOOSE_ONE,				//普通商品：可多选一
	//MGT_GIFT_CHOOSE_ONE,				//礼包：可多选一
};

/**
*@brief 玩家等级间隔
*/
#define PLAYER_LEVEL_INTERVAL 5

/**
*@brief 玩家等级
*/
enum PlayerLevel
{
	PL_INVALID = 0,
	PL_TEN = 10,
	PL_FIFTEEN = 15,
	PL_TWENTY = 20,
	PL_TWENTY_FIVE = 25,
	PL_THIRTY = 30,
	PL_THIRTY_FIVE = 35,
	PL_FORTY = 40,
	PL_FORTY_FIVE = 45,
	PL_FIFTY = 50,
};

/**
*@brief 打折分母
*/
#define DISCOUNT_RATE_DENOMINATOR 100

//充值推送购买
struct RechargePushItem
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & pushId & itemId & itemCount & buyTimes & maxTimes & price & discountPrice & validTimestamp & lastTimestamp;
	}

	UInt32 pushId;
	UInt32 itemId;
	UInt32 itemCount;
	UInt32 buyTimes;
	UInt32 maxTimes;
	UInt32 price;
	UInt32 discountPrice;
	UInt32 validTimestamp;
	UInt32 lastTimestamp;
};

typedef std::vector<RechargePushItem> RechargePushItemVec;

#endif