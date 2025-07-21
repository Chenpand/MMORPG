#ifndef __NEW_AUCTION_MGR_H__
#define __NEW_AUCTION_MGR_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include "WSItem.h"
#include <CLAuctionDefine.h>
#include <AvalonSimpleTimer.h>
#include <CLAuctionProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <deque>
#include <CLAuctionSysValueDataEntry.h>

class WSPlayer;
class AuctionTransRecord;
// 金币拍卖的最少金币
#define GOLD_BASE_NUM 1000000
// 平均价格需要的最近成交价次数
#define RECENT_PRICE_NUM 10
// 高于这个强化等级的道具额外记录它的平均价格
#define AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL 10
// 异常交易会复制道具的最高等级
#define AUCTION_ABNORMAL_COPY_ITEM_MAX_STRENGTHEN 11

// 红字装备增幅跨度,小于11级计算平均交易几个区间跨度
#define  AUCTION_ZENFU_AVERAGE_SPAN	4

// 增幅道具价格一直上限
#define  AUCTION_ZENFU_AVERAGE_STR_LEV 6

namespace zjy_auction {

struct AuctionNumCache
{
	AuctionNumCache()
	{
		expireTime = 0;
	}
	CLProtocol::AuctionQueryCondition cond;
	CLProtocol::WorldAuctionItemNumRes res;
	UInt32		expireTime;
};


struct AuctionRecommendPriceKey
{
	AuctionRecommendPriceKey(UInt32 id_, UInt8 strengthen_, UInt32 beadBuffId_, UInt8 enhanceType_)
	: typeId(id_), strengthen(strengthen_), beadBuffId(beadBuffId_), enhanceType(enhanceType_){}

	bool operator<(const AuctionRecommendPriceKey& other) const
	{
		if (other.typeId != typeId)
		{
			return typeId < other.typeId;
		}
		if (other.strengthen != strengthen)
		{
			return strengthen < other.strengthen;
		}
		if (other.beadBuffId != beadBuffId)
		{
			return beadBuffId < other.beadBuffId;
		}

		return enhanceType < other.enhanceType;
	}

	UInt32	typeId;
	UInt8	strengthen;
	UInt32	beadBuffId;
	UInt8	enhanceType; //红字装备增幅类型(EnhanceType)
};

// 拍卖物品主类型
enum AuctionMainItemType
{
    // 无效
    AMIT_INVALID,
    // 武器
    AMIT_WEAPON,
    // 防具
    AMIT_ARMOR,
    // 首饰
    AMIT_JEWELRY,
    // 消耗品
    AMIT_COST,
    // 材料
    AMIT_MATERIAL,
    // 其它
    AMIT_OTHER,
    // 数量
    AMIT_NUM
};

struct AuctionRushBuyPlayer 
{
	AuctionRushBuyPlayer()
	:playerId(0), buyNum(0)
	{}
	AuctionRushBuyPlayer(ObjID_t playerId_, UInt32 buyNum_)
	: playerId(playerId_), buyNum(buyNum_)
	{
	}
	ObjID_t playerId;
	UInt32  buyNum;
};

struct AuctionInfo : public AuctionBaseInfo
{
    AVALON_DEFINE_OBJECTPOOL(AuctionInfo, Avalon::NullMutex)

    AuctionInfo()
    {
        type = 0;
		account = 0;
		//owner = 0;
        counterFee = 0;
        item = NULL;
		itemSrcGuid = 0;
		mainType = AMIT_INVALID;
		ownerVipLev = 0;
		auctionAgaining = false;
		isRecoveHandled = 0;
		isRecordInOnsaleNum = 0;
    }

    ~AuctionInfo()
    {
		InfoLogStream << "delete auction object, guid : " << guid << "item type id : " << itemTypeId
			<< ", address : " << (UInt64)this << LogStream::eos;
        if (item)
        {
            delete item;
			item = NULL;
        }
    }
    
	UInt8 GetEnhanceType() const;

    UInt8		type;
	// 卖家账号
	UInt32		account;
	// 卖家角色
    //ObjID_t		owner;
	// 卖家vip等级
	UInt32		ownerVipLev;
    // 手续费
    UInt32      counterFee;
    WSItem*     item;
	ObjID_t		itemSrcGuid;
	AuctionMainItemType	mainType;
	// 卖家IP
	std::string	ip;
	// 卖家设备号
	std::string deviceId;
	// 抢购玩家
	std::vector<AuctionRushBuyPlayer> rushBuyPlayers;
	//  是否在重新上架中
	UInt8  auctionAgaining;
	//  是否做过扫货处理
	UInt8  isRecoveHandled;
	//  是否已经记入在售数量(针对附魔卡)
	UInt8  isRecordInOnsaleNum;
};

enum AuctionSortType
{
    AST_PRICE_ASC,  // 按价格升序
    AST_PRICE_DESC, // 按价格降序
};

enum AuctionSellDuration
{
    ASD_24_HOUR,    // 24小时
    ASD_48_HOUR,    // 48小时
    ASD_NUM,
};

enum AuctionSendItemType
{
    ASIT_BUY,       // 购买
    ASIT_CANCEL,    // 下架
    ASIT_TIMEOUT,   // 超时
};

struct AuctionRecentPrice
{
    AuctionRecentPrice()
    {
        memset(this, 0, sizeof(*this));
    }

    UInt32      itemTypeID;
    UInt32      averPrice;
    UInt32      num;
};

struct AuctionItemRecentPrice
{
	AuctionItemRecentPrice(UInt32 price_, UInt32 num_, UInt32 timestamp_, UInt32 qualitylv_, UInt32 beadAverPrice_, UInt32 magicCardAverPrice_)
	: price(price_), num(num_), timestamp(timestamp_), qualitylv(qualitylv_), beadAverPrice(beadAverPrice_), magicCardAverPrice(magicCardAverPrice_)
	{
	}

	UInt32	price;			//总价
	UInt32  num;			//数量
	UInt32	timestamp;
	//UInt32	mountedBeadItemId;		//交易时镶嵌的宝珠itemid
	//UInt32	mountedBeadItemBuffId;	//交易时镶嵌的宝珠的附加buffid
	//UInt32  mountedMagicCardItemId; //交易时候附魔卡itemid
	UInt32	qualitylv;					//交易时候装备品级
	UInt32	beadAverPrice;				//交易时候镶嵌宝珠的平均交易价格
	UInt32  magicCardAverPrice;			//交易时候附魔卡的平均交易价格
};

struct AuctionRecommendPrice
{
    AVALON_DEFINE_OBJECTPOOL(AuctionRecommendPrice, Avalon::NullMutex)
	AuctionRecommendPrice() : guid(0), itemTypeID(0), strengthen(0), bead_buff_id(0),
	price(0), price_update_time(0), visibility_price(0), vis_price_update_time(0){}

	std::string GetRecentPriceStr()
	{
		std::stringstream ss;
		for (auto recent : recentPrice)
		{
			ss << recent.price << "," << recent.num << "," << recent.timestamp << "," << recent.qualitylv << ","
				<< recent.beadAverPrice << "," << recent.magicCardAverPrice << "|";
		}

		return ss.str();
	}

	bool UpdateVisPrice()
	{
		Avalon::Date curr_date = CURRENT_DATE;
		Avalon::Date vis_price_date((UInt64)this->vis_price_update_time * 1000);
		curr_date.Hour(0);
		curr_date.Min(0);
		curr_date.Sec(0);
		curr_date.MSec(0);
		vis_price_date.Hour(0);
		vis_price_date.Min(0);
		vis_price_date.Sec(0);
		vis_price_date.MSec(0);
		
		if (vis_price_date.ToTime().Sec() != curr_date.ToTime().Sec())
		{
			this->visibility_price = this->price;
			this->vis_price_update_time = (UInt32)curr_date.ToTime().Sec();
			return true;
		}
		return false;
	}

	void ForceUpdateVisPrice(UInt32 price);

	void AddRecentPrice(UInt32 price, UInt32 num, UInt32 timestamp, UInt32 quality, UInt32 beadAverPrice, UInt32 magicCardAverPrice, UInt8 istrea = 0)
	{
		UInt32 max_times = 100;
		if (istrea)
		{
			max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_TREAN_CALC_AVERPRICE_MAXTIMES);
		}
		else
		{
			max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES);
		}
		if (max_times == 0) max_times = 100;
		 
		recentPrice.push_back(AuctionItemRecentPrice(price, num, timestamp, quality, beadAverPrice, magicCardAverPrice));
		if (recentPrice.size() > max_times)
		{
			for (int i = recentPrice.size() - max_times; i > 0; --i)
			{
				recentPrice.pop_front();
			}
		}
	}

	void UpdatePrice(bool isTreas);

	/**
	*@brief 获取计算珍品装备交易价格M系数
	*/
	static void GetCalcTransMPriceRate(UInt32& bead_averprice_rate, UInt32& magiccard_averprice_rate, UInt32& guarantee_rate, UInt32& grade_uplimint);

	/**
	*@brief 获取计算珍品装备交易价格M系数
	*/
	static void GetCalcTransMPriceRateNew(AuctionInfo& auctionInfo, UInt32& bead_averprice_rate, UInt32& magiccard_averprice_rate,
		UInt32& guarantee_rate, UInt32& grade_uplimint,UInt32& cmp_bead_rate, UInt32& cmp_magic_rate);

	/**
	*@brief 计算珍品装备交易价格M
	*/
	/*static UInt32 CalcTransMPrice(UInt32 price, UInt32 num, UInt32 qualitylv, UInt32 beadAverPrice, UInt32 magicCardAverPrice,
		UInt32 bead_averprice_rate, UInt32 magiccard_averprice_rate, UInt32 guarantee_rate, UInt32 grade_uplimint);*/

	/**
	*@brief 计算珍品装备(裸装, 没有镶嵌宝珠,附魔卡)交易价格M
	*/
	static UInt32 CalcTransEqBareMPrice(UInt32 price, UInt32 qualitylv, UInt32 guarantee_rate, UInt32 grade_uplimint);

	/*
	*@brief 计算珍品装备（镶嵌了宝珠或附魔卡）交易价格M
	*/
	static UInt32 CalcTransEqMountedMPrice(AuctionInfo& auctionInfo, UInt32 beadAverPrice, UInt32 magicCardAverPrice,
		UInt32 bead_averprice_rate, UInt32 magiccard_averprice_rate, UInt32 guarantee_rate, UInt32 grade_uplimint,
		UInt32 cmp_bead_rate, UInt32 cmp_magic_rate);

    ObjID_t					guid;
    UInt32					itemTypeID;
	UInt8					strengthen;
	UInt32					bead_buff_id;
	UInt8					enhanceType; //红字装备增幅类型
    UInt32					price;
	UInt32					price_update_time;
	// 最近成交价格
	std::deque<AuctionItemRecentPrice> recentPrice;
	UInt32					visibility_price;
	UInt32					vis_price_update_time;
};

struct AuctionPriceSt
{
	ObjID_t  guid;
	UInt32   unitPrice;
};

struct AuctionPricesKey
{
	AuctionPricesKey(UInt32 id_, UInt8 strengthen_, UInt32 beadBuffId_, UInt8 isRedEquip_, UInt8 goodState_)
	: itemTypeId(id_), strengthen(strengthen_), beadBuffId(beadBuffId_), isRedEquip(isRedEquip_), goodState(goodState_){}

	bool operator<(const AuctionPricesKey& other) const
	{
		if (other.itemTypeId != itemTypeId)
		{
			return itemTypeId < other.itemTypeId;
		}
		if (other.strengthen != strengthen)
		{
			return strengthen < other.strengthen;
		}
		if (other.goodState != goodState)
		{
			return goodState < other.goodState;
		}
		if (other.isRedEquip != isRedEquip)
		{
			return isRedEquip < other.isRedEquip;
		}
		return goodState < other.goodState;
	}

	UInt32	itemTypeId;
	UInt32	strengthen;
	UInt32  beadBuffId;
	UInt8	isRedEquip;
	UInt8   goodState;
};

struct AuctionPricesSt
{
	AuctionPricesSt()
	{
		timeStmap = 0;
	}
	CLProtocol::WorldAuctionQueryItemPricesRes res;
	UInt32 timeStmap;
};

struct AuctionPricesStForSwtich
{
	AuctionPricesStForSwtich()
	{
		timeStmap = 0;
	}
	CLProtocol::WorldAuctionQueryItemPriceListRes res;
	UInt32 timeStmap;
};

struct MagicOnsaleKey
{
	MagicOnsaleKey(UInt32 id_, UInt8 strengthen_)
	: itemTypeId(id_), strengthen(strengthen_){}

	bool operator<(const MagicOnsaleKey& other) const
	{
		if (other.itemTypeId != itemTypeId)
		{
			return itemTypeId < other.itemTypeId;
		}
		
		return strengthen < other.strengthen;
	}

	UInt32	itemTypeId;
	UInt8	strengthen;
};



class AuctionMgr : public Avalon::Singleton<AuctionMgr>
{
    typedef HashMap<ObjID_t, AuctionInfo*> MapAuction;
	typedef std::map<UInt32, UInt32> MapPriceNum;
	typedef std::map<AuctionPricesKey, AuctionPricesSt> MapAuctionPrices;
	typedef std::map<AuctionPricesKey, AuctionPricesStForSwtich> MapAuctionPricesForSwtich;
	
	typedef std::map<MagicOnsaleKey, UInt32>	MapMagicOnsaleNum;
	typedef std::map<UInt8, UInt32> MapMagicMinAverPrices;

	static const UInt32 max_rush_buy_time = 5;
public:
    AuctionMgr();
    ~AuctionMgr();

    void Init();
    void OnTick(const Avalon::Time &now);

	/**
	*@ 更新拍卖物品状态
	*/
	void UpdateTreasState(const Avalon::Time &now);
	
	/**
	*@ 更新单个拍卖物品状态
	*/
	bool UpdateOneTreasState(AuctionInfo* pAuctionInfo, const Avalon::Time &now, bool& earse, bool& recoved, bool canDelete = false);

	/**
	*@ 增加屏蔽道具
	*/
	void AddShieldItem(UInt32 itemId);

	/**
	*@ 清空屏蔽道具
	*/
	void ClearShieldItem();

public:
    /**
    *@brief 拍卖
    * type : AuctionType
    */
	bool OnPlayerAddItemToAuction(WSPlayer* player, AuctionType type, ObjID_t id, UInt32 typeId,
		UInt32 num, UInt32 price, AuctionSellDuration duration, UInt8 strength, UInt32 buffId, UInt8 enhanceType);

	/**
	*@brief 重新上架
	*/
	bool OnPlayerAddItemToAuctionAgain(WSPlayer* player, AuctionType type, ObjID_t auctionGuid, 
		UInt32 num, UInt32 price, UInt8 strength, UInt32 buffId, UInt8 enhanceType);

    /**
    *@brief 查询拍卖的物品
    * type : AuctionType
    */
    bool OnPlayerQueryAuctionList(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);

	/**
	*@brief 根据条件获取拍卖的物品的数量（按照ItemTypeID分组）
	* type : AuctionType
	*/
	bool OnPlayerQueryAuctionItemNum(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);

    /**
    *@brief 查询拍卖物品的详细信息
    */
    bool OnPlayerQueryAuctionItem(WSPlayer* player, ObjID_t id);

    /**
    *@brief 购买拍卖物品
    */
    bool OnPlayerBuyAuctionItem(WSPlayer* player, ObjID_t id, UInt32 num, bool rushBuy = false);

	/**
	*@brief 抢购珍品物品
	*/
	bool OnPlayerRusyBuyAuctionItem(WSPlayer* player, ObjID_t guid, UInt32 num);

    /**
    *@brief 撤销拍卖
    */
    bool OnPlayerCancelAuction(WSPlayer *player, ObjID_t id);
	bool OnSceneAuctionCheckPackageRet(WSPlayer *player, ObjID_t id);

    /**
    *@brief 加载自己的拍卖物品列表
    */
    bool OnPlayerLoadSelfAuctionList(WSPlayer *player);

    /**
    *@brief 查询自己的拍卖物品列表
    */
    bool OnPlayerQuerySelfAuctionList(WSPlayer *player, UInt8 type);

    /**
    *@brief 查询自己的拍卖物品列表
    */
    bool OnPlayerQueryRecommendPrice(WSPlayer *player, AuctionType type, UInt32 itemTypeId);

	/**
	*@brief 查询道具近期交易价格
	*/
	bool OnPlayerQueryItemRecentPrices(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType);

	/**
	*@breid 查询道具最近交易价格
	*/
	bool OnPlayerQueryItemRecentPricesList(WSPlayer *player, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType);
public:
    /**
    *@brief 数据库返回所有拍卖信息
    */
    bool OnSelectAuctionRet(CLRecordCallback *pCallback);

    /**
    *@brief 数据库返回推荐价格信息
    */
    bool OnSelectAuctionRecommendPriceRet(CLRecordCallback *pCallback);

    /**
    *@brief 数据库返回超时的物品
    */
    bool OnSelectTimeoutAuctionRet(CLRecordCallback *pCallback);

    /**
    *@brief 数据库返回所有拍卖信息
    */
    bool OnQueryPlayerAuctionListRet(ObjID_t roleId, std::vector<std::pair<UInt8, ObjID_t> >& auctionList);

    /**
    *@brief 数据库检索返回
    */
	void OnQueryAuctionRet(ObjID_t roleId, UInt8 type, UInt16 curPage, UInt16 maxPage, std::vector<ObjID_t>& auctionIds, UInt8 goodState, UInt8 attent);

    /**
    *@brief SceneServer返回待拍卖的物品信息
    */
    void OnAuctionCheckRet(Avalon::NetInputStream &stream);

    /**
    *@brief SceneServer返回扣除货币请求
    */
    bool OnBuyAuctionCheckRet(WSPlayer *pPlayer, ObjID_t id, UInt32 num);

	/**
	*@brief 数据库返回某类物品在售最低价格列表
	*/
	bool OnQueryAuctionSaleLisRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32   beadbuffid, UInt8 enhanceType);
	
	/**
	*@brief 数据库返回某类物品在售最低价格列表
	*/
	bool OnQueryAuctionSalePriceListRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType);
public:
	/**
	*@brief 根据角色ID获取拍卖行信息
	*/
	//std::vector<AuctionInfo*> GetPlayerAuctionInfoes(ObjID_t roleid);
	
	
	
	/**
	*@brief 获取在售最低价格
	*/
	UInt32 _GetMinPriceOnsale(AuctionInfo& auctionInfo, UInt32 selfPrice);

	/**
	*@brief 获取在售珍品装备最低价格
	*/
	UInt32 _GetTreasEqMinPriceOnSale(AuctionInfo& auctionInfo, UInt64& minPriceGuid, UInt32& minPriceOnsalePrice, UInt32& minPriceQualityLv,
		UInt32& minPriceMountBeadId, UInt32& minPriceMountBeadAverPrice, UInt32& minPriceMountMagicId, UInt32& minPriceMountMagicAverPrice);

	/**
	*@brief 获取货币物品ID
	*/
	UInt32 _GetItemTypeIDByPriceType(MoneyType type);

	/**
	*@brief 珍品物品上架
	*/
	void OnTreasOnSale(AuctionInfo& auctionInfo);

	/**
	*@brief 珍品装备上架
	*/
	void OnTreasEqOnSale(AuctionInfo& auctionInfo);

	/**
	*@brief 记录在售价格
	*/
	void AddTreasOnsalePrice(AuctionInfo& auctionInfo);

	/**
	*@brief 记录在售价格
	*/
	void AddTreasOnsalePrice(AuctionRecommendPriceKey &key, UInt32 price);

	/**
	*@brief 去除在售价格
	*/
	void RemoveTreasOnsalePrice(AuctionRecommendPriceKey &key, UInt32 price);

	/**
	*@brief 珍品下架
	*/
	void OnTreasOffSale(AuctionInfo& auctionInfo);

	/**
	*@brief 珍品装备下架
	*/
	void OnTreasEqOffSale(AuctionInfo& auctionInfo);

	/*
	*@brief 珍品非装备下架
	*/
	void OnTreasNoEqOffSale(AuctionInfo& auctionInfo);

	/**
	*@breif 记录珍品交易价格(审核判定交易成功的或违规但交易未取消)
	*/
	void RecordTreasDealPrice(const AuctionTransRecord& auctionTrans, bool fore = false);

	/**
	*@brief 获取平均价格
	*/
	UInt32 _GetAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	/**
	*@brief 获取实时平均价格
	*/
	UInt32 _GetRealAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	/**
	*@brief 获取推荐价格
	*/
	UInt32 _GetRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, UInt8 enhanceType);

	/**
	*@brief 获取附魔卡推荐价格
	*/
	UInt32 _GetMagicRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, ItemDataEntry* itemData);

	/**
	*@brief 获取某品质附魔卡最低实时平均交易价格
	*/
	UInt32 _GetMagicMinAverPriceByQuality(UInt8 quality);

	/**
	*@brief 计算附魔卡最低实时平均交易价格
	*/
	void _CalcMagicMinAverPriceByQuality();

	/**
	*@brief 查找拍卖信息
	*/
	AuctionInfo* _FindAuction(ObjID_t id);

	/**
	*@brief 获取手续费
	*/
	UInt32 GetCounterFeeNew(UInt8 vipLvl, AuctionType type, UInt32 price);
private:
    /**
    *@brief 获取每个玩家拍卖物品最多数量
    */
    UInt8 _GetAuctionMaxItemPerPlayer(AuctionType type, WSPlayer* player);
    
    /**
    *@brief 通过邮件将发送物品给玩家，isBuy代表是购买还是退回
    */
	bool _SendAuctionToPlayer(AuctionInfo *auctionInfo, UInt32 num, ObjID_t roleId, AuctionSendItemType sendType, bool abnormalDeal = false);

    /**
    *@brief 查找拍卖道具
    */
    WSItem* _FindAuctionItem(ObjID_t id);

    /**
    *@brief 删除拍卖信息
    */
    void _DeleteAuctionInfo(AuctionInfo *auctionInfo);

	/**
	*@brief 获取手续费
	*/
	UInt32 _GetCounterFeeByAuctionType(WSPlayer* player, AuctionType type, UInt32 price);

    /**
    *@brief 获取持续时间
    */
    UInt32 _GetAuctionDuetime(ItemDataEntry* dataEntry);

	/*
	*@brief 获取珍品平均价格
	*/
	UInt32 _GetTreasAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	/*
	*@brief 获取可见平均价格
	*/
	bool _GetVisAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType, UInt32& visAverPrice);
    
	/**
    *@brief 记录成交价格
    */
	void _RecordDealPrice(AuctionInfo* auctionInfo, UInt32 beadBuffId, UInt32 price, UInt32 num);

    /**
    *@brief 更新物品推荐价格
    */
    void _UpdateRecommendPrice();

	/**
	*@brief 发送物品的事件
	*/
	void _OnSendAuctionToPlayer(WSPlayer* player, AuctionSendItemType sendType, AuctionType type, bool abnormalDeal, ObjID_t owner, UInt32 ownerVipLev, ObjID_t srcItemId, ObjID_t itemId,
		AuctionMainItemType mainType, UInt32 itemDataId, UInt32 num, MoneyType priceType, UInt32 price, UInt32 counterFee, UInt8 strengthen,
		UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp, std::string sellerDeviceId, UInt32 ownerAccount,
		UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid, UInt8 isAttented, UInt32 itemAttentNum,
		UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
		UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice, UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice,
		UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax);

	/**
	*@brief 是否是有效的价格
	*/
	bool _IsValidPrice(UInt32 typeId, UInt8 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt32 price);

	/**
	*@brief 获取一个限定价格  by huchenhui 2018.12.18
	*/
	bool _GetLimitPrice(UInt32 typeId, UInt8 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt32& minPrice, UInt32& maxPrice);

public:
	/**
	*@breif 拍卖物品添加一个抢购者
	*/
	bool AddOneRushBuyPlayer(AuctionInfo* pAuctionInfo, WSPlayer* player, UInt32 num);

	/**
	*@brief 一个拍卖商品抢购发货一个选中玩家
	*/
	//void DeliverOneRushBuyPlayer(AuctionInfo* pAuctionInfo);

	/**
	*@brief 抢购发货
	*/
	bool DeliverOneRushBuyPlayerNew(AuctionInfo* pAuctionInfo);

	/**
	*@brief 选择一个购买者
	*/
	bool SelectOneRushBuyer(const AuctionInfo& auctionInfo, AuctionRushBuyPlayer& rush_buyer);

	/**
	*@brief 处理一次抢购发货结果
	*/
	void OnDeliverOneRushBuyResult(ObjID_t playerId, ObjID_t id, UInt32 num, bool succ, UInt8 reason);

	/**
	*@brief 珍品抢购发货结束
	*/
	void HandleTreaseRushDeliverEnd(AuctionInfo* pAuctionInfo);

	/*
	*@brief 设置拍卖行物品交易冷却时间戳
	*/
	void SetAuctionItemCoolTime(WSItem* item);

	/*
	*@brief 计算物品交易冷却时间
	*/
	static UInt32 CalcAuctionItemCoolTime(const std::string& pf, UInt32 itemId, UInt32 beginTime);
	static UInt32 CalcAuctionItemCoolTime(const std::string& pf, ItemDataEntry* dataEntry, UInt32 beginTime);
	
	void AddOneAution(AuctionInfo* auctionInfo);

	void AddOneTreas(AuctionInfo* auctionInfo);
	void RemoveOneTreas(AuctionInfo* auctionInfo);

	void GetBeadAndMagicCardAverPrice(AuctionInfo* auctionInfo, UInt32& beadAverPrice, UInt32& magicCardAverPrice);

	//获取珍品装备镶嵌宝珠，附魔卡平均交易价格用于计算M价格
	void GetBeadAndMagicAverPriceForCalcTransEqMPrice(AuctionInfo* auctionInfo, UInt32& beadAverPrice, UInt32& magicCardAverPrice,
		UInt32& mountedBeadId, UInt32& mountedMagicId, UInt32& mountedBeadBuffId, UInt8& mountedMagicLv);

	bool IsMountedBeadOrMagic(const AuctionInfo& auctionInfo);

	AuctionRecommendPrice* GetRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType);

	bool SetAuctionGoodState(AuctionInfo& auctionInfo, UInt8 st);

	bool SetAuctionBuyState(AuctionInfo& auctionInfo, UInt8 st);

	void PrintAuctionState(AuctionInfo& auctionInfo);

	/**
	*@brief 计算珍品装备交易价格M值
	*/
	UInt32 CalcTransEqMPrice(AuctionInfo& auctionInfo);

	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief 获取拍卖物品信息
	*/
	AuctionInfo* GetAuctionInfo(WSPlayer* player, UInt64 auctionId);

	/**
	*@brief 过滤拍卖行id
	*/
	void FilterAuctions(std::vector<UInt64>& srcIds, std::vector<UInt64>& desIds, UInt8 goodState);

	/**
	*@brief 获取物品的最近交易时间
	*/
	UInt32 GetItemLatestDealTime(UInt32 itemId);
public:
    /**
    *@brief 根据金币数量获取对应的金币物品
    */
    static UInt32 GetGoldItemByNum(UInt32 num);

	/**
	*@brief 获取押金
	*/
	static UInt32 GetDepositByAuctionPrice(UInt32 price);

	/**
	*@brief 根据条件生成sql语句
	*/
	static bool GenSqlByCondition(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond);

	/**
	*@brief 根据条件计算拍卖行道具数量
	*/
	static bool GenAuctionItemNumSql(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond);

    /**
    *@brief 计算sql查询的条件
    */
	static bool GenAuctionQuerySqlCondition(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond);

    /**
    *@brief 计算sql查询的条件
    */
    static MoneyType GetMoneyTypeByAuctionType(AuctionType type);

    /*
    *@brief 获取拍卖行物品主类型
    */
	static AuctionMainItemType GetAuctionMainType(ItemType type, ItemSubType subType);

    /*
    *@brief 获取拍卖行物品子类型
    */
    static UInt32 GetAuctionSubType(ItemDataEntry* entry);

	/**
	*@brief GM设置平均交易价格
	*/
	bool SetAverPriceGM(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType,
		bool bSetAverPrice, UInt32 averPrice, bool bSetAverVisPrice, UInt32 averVisPrice, UInt32 transNum);

	/**
	*@brief 跨天处理
	*/
	void OnDayChanged();

	/**
	*@brief 记录交易手续费
	*/
	void RecordCounterFee(UInt32 counterFee);

	/**
	*brief 获取剩余系统扫货回收数量
	*/
	UInt32 GetSysRecoveSurplus(UInt8 isTreas);

	/**
	*@brief 增加扫货计算
	*/
	void IncRecoveryNum(UInt8 IsTreas, UInt32 num);

	/*
	*@brief 是否应该扫货
	*/
	bool IsShouldRecovery(AuctionInfo& auctionInfo);

	/**
	*@brief 添加待扫货
	*/
	void AddOneShouldRcoAuction(ObjID_t auctionId);

	/**
	*@brief 扫货处理(非珍品)
	*/
	void RecoveryAuctions();

	/**
	*@brief 扫货处理
	*/
	bool HandleForRecovery(AuctionInfo& auctionInfo, bool canDelete, bool& handled);

	/**
	*@brief 处理扫货非珍品拍卖物品
	*/
	bool OnSelectCheckRecovedNoTreasAuctionRet(CLRecordCallback *pCallback);

	AuctionPricesSt* GetAuctionPriceListCache(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	void UpdateAuctionPriceListCache(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, CLProtocol::WorldAuctionQueryItemPricesRes& res, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	AuctionPricesStForSwtich* GetAuctionPriceListCacheForSwitch(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	void UpdateAuctionPriceListCacheForSwitch(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, CLProtocol::WorldAuctionQueryItemPriceListRes& res, ItemDataEntry& itemData, UInt8 goodState = AGS_ONSALE);

	/**
	*@brief 添加待上架附魔卡
	*/
	void AddWaiteOnsaleMagic(ObjID_t auctionId);

	/**
	*@brief 处理待上架附魔卡
	*/
	void HandleWaiteOnsaleMagics();

	/**
	*@brief 附魔卡上架
	*/
	void OnMagicOnsale(AuctionInfo& auctionInfo);

	/**
	*@brief 附魔卡在售数量增加
	*/
	void IncMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen, UInt32 num = 1);

	/**
	*@brief 附魔卡在售数量减少
	*/
	void DecMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen, UInt32 num = 1);

	/*
	*@brief 获取附魔卡在售数量
	*/
	UInt32 GetMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen);

	/**
	*@brief 记录附魔卡在售数量
	*/
	void RecordMagicOnsaleNum(AuctionInfo& auctionInfo);

	/**
	*@brief 附魔卡下架
	*/
	void OnMagicOffsale(AuctionInfo& auctionInfo);

	/**
	*@brief 附魔卡过期
	*/
	void OnMagicTimeOut(AuctionInfo& auctionInfo);

	/**
	*@brief 减少附魔卡在售数量(被购买)
	*/
	void OnMagicBuy(AuctionInfo& auctionInfo, UInt32 num);

	/**
	*@brief 获取指定附魔卡各等级在售数量
	*/
	bool QueryMagicOnsales(WSPlayer *player, UInt32 itemTypeId);

	/**
	*@brief 检查过期超出时间自动下架回调处理
	*/
	bool OnSelectTimeoutOffsaleAuctionRet(CLRecordCallback *pCallback);

	/**
	*@brief 非珍品tick函数
	*/
	void OnTickNoTreasAuctions();

	/**
	*@brief 获取记录平均交易价格的强化等级
	*/
	UInt8 GetStrengthenForAverPrice(const AuctionInfo& auctionInfo);

	UInt8 GetStrengthenForAverPrice(ItemDataEntry& itemData, UInt32 strengthen, UInt8 enhanceType);
	
	//获取红字装备强化等级区间
	void GetStrengthenRangeForHzEquip(UInt8 strengthen, UInt8& min, UInt8& max);

	//更新
	void UpdateAuctionPriceCacheForExplaceHzEquip(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8 realEnhanceType);

	void OnQueryAuctionSaleLisForExplaceHzEquipRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8	realEnhanceType);

	//职业检查
	static bool CheckAuctionOccus(const ItemDataEntry* itemData, std::vector<UInt8>& occus);

	//时限物品上架时间检查
	static bool CheckTimeLimitAuctionOnsaleAgain(WSPlayer* player, const AuctionInfo& auctionInfo);

	//查询物品已交易记录
	UInt32  GetItemTransedRecords(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, std::vector<AuctionTransaction>& itemTrans);

	//查询物品数量缓存
	AuctionNumCache* GetAuctionNumCache(const CLProtocol::AuctionQueryCondition& cond);

	//更新物品数量缓存
	void UpdateAuctionNumCache(const CLProtocol::AuctionQueryCondition& cond, CLProtocol::WorldAuctionItemNumRes& res);

public:	//内存查询物品数量，物品列表
	//
	void AddOneAuctionForMemQuery(AuctionInfo* auctionInfo);
	void RemoveAuctionForMemQuery(AuctionInfo* auctionInfo);
	void QueryAuctionItemNumFromMem(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);
	void QueryAuctionItemListFromMem(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond);
	void QueryItemRecentPricesFormMem(WSPlayer* player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType);
	void UpdateAuctionPriceCacheForExplaceHzEquipFromMem(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8 realEnhanceType);
	void QueryItemRecentPricesFormMemForSwitch(WSPlayer *player, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType);
private:
    MapAuction                                      m_auctiones;

	//需要更新状态的珍品
	MapAuction										m_needUpdateTreasAuctions;

    // 检测拍卖行超时物品定时器
    Avalon::SimpleTimer	                            m_checkTimer;

	// 检测拍卖行珍品状态定时器
	Avalon::SimpleTimer	                            m_checkTreasTimer;

	// 检测扫货,附魔卡上架定时器
	Avalon::SimpleTimer	                            m_checkNoTreasTimer;

	// 计算某品质附魔卡最低实时平均交易价格定时器
	Avalon::SimpleTimer	                            m_clacMagicMinAverPriceTimer;

    // 推荐价格
	std::map<AuctionRecommendPriceKey, AuctionRecommendPrice*>	m_recommendPrice;

	// 在售珍品物品(非装备)价格
	std::map<AuctionRecommendPriceKey, MapPriceNum> m_onSaleTreaPrices;

    // 最近售价
    HashMap<UInt32, AuctionRecentPrice>             m_recentPrice;

    // 更新推荐价格的定时器
    Avalon::SimpleTimer                             m_priceTimer;

	// 上架的珍品装备
	std::map<AuctionRecommendPriceKey, MapAuction> m_onSaleTreaEqAuctions;

	// 检测拍卖行非珍品扫货定时器
	Avalon::SimpleTimer	                            m_checkRecoveNoTreasTimer;

	// 拍卖行物品价格缓存上架
	MapAuctionPrices								m_pircesCacheOfOnSale;
	// 拍卖行物品价格缓存列表切换					
	MapAuctionPricesForSwtich						m_pircesCacheOfSwitch; 

	//附魔卡在售数量(区分等级)
	MapMagicOnsaleNum								m_magicOnsaleNum;

	// 检测下架拍卖行超时物品定时器
	Avalon::SimpleTimer	                            m_checkExpireAuctionTimer;

	// 待扫货非珍品
	std::set<ObjID_t>								m_shouldRecoveryAuctions;

	// 待上架附魔卡
	std::set<ObjID_t>								m_waiteOnsaleMagics;

	// 附魔卡某品质最低实时平均交易价格
	MapMagicMinAverPrices							m_magicMinAverPrice;

	// 拍卖行屏蔽礼包(配置)
	std::vector<UInt32>								m_ShieldItemVec;

	// 拍卖物品数量缓存
	std::vector<AuctionNumCache>					m_auctionNumCaches;

	// 拍卖物品容器key:item_main_type
	std::map<UInt32, MapAuction>				m_AuctionsMainType;
	
	// 拍卖物品容器key:item_sub_type
	std::map<UInt32, MapAuction>				m_AuctionsSubType;

	// 拍卖物品容器key:itemid
	std::map<UInt32, std::set<AuctionInfo*, AuctionBaseCmpForItemIdSet>>	m_auctionesItemId;
};

};

#endif