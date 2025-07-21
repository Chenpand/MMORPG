#ifndef _CL_AUCTION_PROTOCOL_H_
#define _CL_AUCTION_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLAuctionDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
    struct AuctionQueryCondition
    {
		AuctionQueryCondition()
		{
			type = 0;
			goodState = 0;
			itemMainType = 0;
			itemTypeID = 0;
			quality = 0;
			minLevel = 0;
			maxLevel = 0;
			minStrength = 0;
			maxStrength = 0;
			sortType = 0;
			page = 0;
			itemNumPerPage = 0;
			couponStrengthToLev = 0;
			attent = 0;
		}

        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & type & goodState & itemMainType & itemSubTypes & excludeItemSubTypes & itemTypeID & quality
                    & minLevel & maxLevel & minStrength & maxStrength & sortType
					& page & itemNumPerPage & couponStrengthToLev & attent & occus;
        }

		bool operator == (const AuctionQueryCondition& other) const
		{
			if (other.type != type || other.goodState != goodState || other.itemMainType != itemMainType || other.itemTypeID != itemTypeID
				|| other.quality != quality || other.minLevel != minLevel || other.maxLevel != maxLevel || other.minStrength != minStrength
				|| other.maxStrength != maxStrength || other.sortType != sortType || other.page != page || other.itemNumPerPage != itemNumPerPage 
				|| other.couponStrengthToLev != couponStrengthToLev || other.attent != attent)
			{
				return false;
			}

			if (!CompareVectorEqual(other.itemSubTypes, itemSubTypes)
				|| !CompareVectorEqual(other.excludeItemSubTypes, excludeItemSubTypes)
				|| !CompareVectorEqual(other.occus, occus))
			{
				return false;
			}

			return true;
		}

        // 拍卖类型
        UInt8                       type;
		// 商品状态 [1]:公示 [2]:上架
		UInt8						goodState;
        // 物品主类型（对应枚举AuctionMainItemType）
        UInt8                       itemMainType;
        // 物品子类型
        std::vector<UInt32>         itemSubTypes;
		// 排除物品子类型
		std::vector<UInt32>         excludeItemSubTypes;
        // 物品ID
        UInt32                      itemTypeID;
        // 品质
        UInt8                       quality;
        // 最低物品等级
        UInt8                       minLevel;
        // 最高物品等级
        UInt8                       maxLevel;
        // 最低强化等级
        UInt8                       minStrength;
        // 最高强化等级
        UInt8                       maxStrength;
        // 排序方式（对应枚举AuctionSortType）
        UInt8                       sortType;
        // 页数
        UInt16                      page;
        // 每页多少个物品
        UInt8                       itemNumPerPage;
		// 强化卷强化等级			
		UInt32						couponStrengthToLev;
		// 是否关注[0]非关注, [1]关注
		UInt8						attent;
		// 职业
		std::vector<UInt8>			occus;
    };
	/**
	*@brief client->server 请求拍卖列表
	*/
	class WorldAuctionListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cond;
		}

	public:
        AuctionQueryCondition cond;
	};

	/**
	*@brief server->client 同步拍卖列表
	*		格式：开始位置(UInt16) + 总条数(UInt16) + UInt8(数量) + [类型(UInt8, 0 道具，1 金贝，2 银贝) + 价格(UInt32) + 货币类型(UInt8) + 到期时间(UInt32) + data] ...
	*		data	0 : Guid(UInt64) + itemid(UInt32) + [ItemAttr(UInt8) + value][ItemAttr(UInt8) + value]...[0]
	*				1、2 : Guid(UInt64) + 货币数量(UInt32)
	*/
	class WorldAuctionListSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_LIST_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->server 请求自己的出售列表
	*/
	class WorldAuctionSelfListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_SELF_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
            stream & type;
		}

        UInt8   type;
	};

	/**
	*@brief server->client 同步自己的出售列表
	*/
	class WorldAuctionSelfListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_SELF_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
            stream & type & data;
		}

        UInt8                           type;
        std::vector<AuctionBaseInfo>    data;
	};

	/**
	*@brief client->server 拍卖道具
	*/
	class WorldAuctionRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_REQUEST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & typeId & num & price & duration & strength & buffId & isAgain & auctionGuid & enhanceType;
		}

	public:
		// 拍卖类型
		UInt8	type;
		// 拍卖道具id
		ObjID_t	id;
		// 道具类型id
		UInt32	typeId;
		// 数量
		UInt32	num;
		// 价格
		UInt32	price;
        // 持续时间(AuctionSellDuration)
        UInt8   duration;
		// 强化等级
		UInt8  strength;
		// 宝珠buffid
		UInt32  buffId;
		// 是否重新上架
		UInt8   isAgain;
		// 重新上架guid
		UInt64 auctionGuid;
		// 红字装备增幅类型 无效0/力量1/智力2/体力3/精神4
		UInt8  enhanceType;
	};

	/**
	*@brief server->client 玩家的拍卖列表增加一项
	*		格式：[拍卖信息（格式如上）]
	*/
	class WorldAuctionAdd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_ADD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->server 购买拍卖道具
	*/
	class WorldAuctionBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}

	public:
		//guid
		ObjID_t		id;
		//数量
		UInt32		num;
	};
	
	/**
	*@brief client->server 取消拍卖
	*/
	class WorldAuctionCancel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_CANCEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//guid
		ObjID_t		id;
	};

	/**
	*@brief server->client 通知客户端删除拍卖
	*/
	class WorldAuctionDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//guid
		ObjID_t		id;
	};
	
	/**
	*@brief server->client 通知客户端需要刷新拍卖列表
	*/
	class WorldAuctionNotifyRefresh : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_NOTIFY_REFRESH)
	public:
		WorldAuctionNotifyRefresh()
		{
			type = 0;
			reason = 0;
			auctGuid = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & reason & auctGuid;
		}

	public:
        UInt8       type;
        UInt8       reason;
		UInt64		auctGuid;
	};

	/**
	*@brief world->scene 拍卖检查
	*/
	class WorldAuctionCheck : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_CHECK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & id & typeId & num & price & pricetype 
				& deposit & counterFee & duration & strength & beadbuffId & isAgain & auctionGuid;
		}

	public:
		// 玩家id
		ObjID_t	playerId;
		// 拍卖类型
		UInt8	type;
		// 拍卖道具id
		ObjID_t	id;
		// 拍卖道具类型ID
		UInt32	typeId;
		// 数量
		UInt32	num;
		// 价格
		UInt32	price;
		// 价格的货币类型
		UInt8	pricetype;
        // 押金
        UInt32  deposit;
        // 手续费
        UInt32  counterFee;
        // 持续时间(AuctionSellDuration)
        UInt8   duration;
		// 强化等级
		UInt8  strength;
		// 宝珠buffid
		UInt32  beadbuffId;
		// 是否重新上架
		UInt8   isAgain;
		// 重新上架guid
		UInt64 auctionGuid;
		
	};

	/**
	*@brief scene->world 拍卖列表查询返回
	*/
	class WorldAuctionListQueryRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_LIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & data & curPage & maxPage & goodState & attent;
		}

        UInt8                           type;
        std::vector<AuctionBaseInfo>    data;
        UInt32                          curPage;
        UInt32                          maxPage;
		// 商品状态(AuctionGoodState)
		UInt8							goodState;
		// 是否关注(AuctionAttentType)
		UInt8							attent;
	};

	/**
	*@brief client->server 根据guid查找商品
	*/
	class WorldAuctionFindByID : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_FIND_BYID)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		// 唯一id
		ObjID_t	id;
	};

	/**
	*@brief server->client 查找商品返回
	*		格式：[拍卖信息（格式如上）]
	*/
	class WorldAcutionFindRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_FIND_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

    /**
    *@brief client->server 根据物品guid查找拍卖物品详细信息
    */
    class WorldAuctionQueryItemReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & id;
        }

    public:
        // 唯一id
        ObjID_t	id;
    };

    /**
    *@brief server->client 查找拍卖物品详细信息返回
    *		格式：[WSItem流]
    */
    class WorldAuctionQueryItemRet : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_RET)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
        }
    };

    /**
    *@brief client->server 查询物品的推荐价格
    */
    class WorldAuctionRecommendPriceReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_RECOMMEND_PRICE_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & type & itemTypeId;
        }

    public:
        UInt8       type;
        UInt32	    itemTypeId;
    };
    
    /**
    *@brief client->server 返回物品的推荐价格
    */
    class WorldAuctionRecommendPriceRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_RECOMMEND_PRICE_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & type & itemTypeId & price;
        }

    public:
        UInt8       type;
        UInt32	    itemTypeId;
        UInt32      price;
    };

	/**
	*@brief client->server 查询拍卖行道具数量
	*/
	class WorldAuctionItemNumReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_ITEM_NUM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cond;
		}

	public:
		AuctionQueryCondition cond;
	};

	/**
	*@brief client->server 返回拍卖行道具数量
	*/
	class WorldAuctionItemNumRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_ITEM_NUM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodState & items;
		}

	public:
		// 商品状态(AuctionGoodState)
		UInt8 goodState;
		std::vector<AuctionItemBaseInfo>		items;
	};

	/**
	*@brief client->scene 刷新拍卖行时间请求
	*/
	class SceneAuctionRefreshReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_REFRESH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	
	/**
	*@brief scene->client 刷新拍卖行时间返回
	*/
	class SceneAuctionRefreshRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_REFRESH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->scene 购买拍卖行栏位请求
	*/
	class SceneAuctionBuyBoothReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_BUY_BOOTH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client 购买拍卖行栏位返回
	*/
	class SceneAuctionBuyBoothRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_BUY_BOOTH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & boothNum;
		}

		UInt32 result;
		UInt32 boothNum;
	};

	/*
	*@brief	client->world 查询道具价格情况(近期交易平均价, 其它玩家正在出售价格)
	*/
	class WorldAuctionQueryItemPricesReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_PRICES_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & itemTypeId & strengthen & beadbuffid & enhanceType;
		}

		UInt8	type;
		UInt32	itemTypeId;
		UInt32	strengthen;
		UInt32  beadbuffid;
		UInt8	enhanceType;
	};

	/*
	*@brief world->client 道具价格情况返回
	*/
	class WorldAuctionQueryItemPricesRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_PRICES_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & itemTypeId & strengthen & averagePrice & actionItems & visAverPrice & minPrice & maxPrice & recommendPrice;
		}

		UInt8	type;
		UInt32	itemTypeId;
		UInt32	strengthen;
		UInt32  averagePrice;
		std::vector<AuctionBaseInfo>    actionItems;
		UInt32  visAverPrice;
		UInt32	minPrice;
		UInt32	maxPrice;
		UInt32  recommendPrice;
	};

	/*
	*@brief scene->client 通知异常交易
	*/
	class SceneNotifyAbnormalTransaction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_AUCTION_ABNORMAL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bNotify;
		}

		// bool值(false(0):关闭通知，true(1):开启通知)
		UInt8 bNotify;
	};

	/*
	*@brief client->scene 异常交易记录查询
	*/
	class SceneAbnormalTransactionRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_ABNORMAL_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/*
	*@brief scene->client 异常交易记录返回
	*/
	class SceneAbnormalTransactionRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_ABNORMAL_RECORD_RES)
	public:
		SceneAbnormalTransactionRecordRes()
		{
			frozenMoneyType = 0;
			frozenAmount = 0;
			abnormalTransactionTime = 0;
			backDeadline = 0;
			backAmount = 0;
			backDays = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & frozenMoneyType & frozenAmount & abnormalTransactionTime & backDeadline & backAmount & backDays;
		}

		// 冻结金类型
		UInt32 frozenMoneyType;
		// 冻结金额
		UInt32 frozenAmount;
		// 异常交易时间
		UInt32 abnormalTransactionTime;
		// 结束保释时间
		UInt32 backDeadline;
		// 返还金额
		UInt32 backAmount;
		// 已返还日
		UInt32 backDays;
	};

	/**
	*@brief scene->world 异常交易扣除邮件中的道具
	*/
	class SceneAbnormalTransRemoveMailItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ABNORMAL_TRANS_REMOVE_MAIL_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & abnormalTransId & preAbnormalTransId & owner & itemDataId & needRmNum;
		}

	public:
		// 异常交易id
		ObjID_t  abnormalTransId;
		// 要扣除的玩家id
		ObjID_t owner;
		// 要扣除的道具id
		UInt32 itemDataId;
		// 要扣除的道具数量
		UInt32 needRmNum;
		// 上一个异常交易id
		ObjID_t preAbnormalTransId;
	};

	/**
	*@brief world->scene 扣除邮件中的道具返回
	*/
	class SceneAbnormalTransRemoveMailItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ABNORMAL_TRANS_REMOVE_MAIL_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & abnormalTransId & preAbnormalTransId & owner & itemDataId & needRmNum & successRmNum;
		}

	public:
		// 异常交易id
		ObjID_t  abnormalTransId;
		// 上一个异常交易id
		ObjID_t preAbnormalTransId;
		// 要扣除的玩家id
		ObjID_t owner;
		// 要扣除的道具id
		UInt32 itemDataId;
		// 要扣除的道具数量
		UInt32 needRmNum;

		// 成功扣除的道具数量
		UInt32 successRmNum;
	};

	/**
	*@brief world->scene 通知查询待处理异常交易
	*/
	class WorldNotifyQueryWaitHandleAbnormalTrans : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_QUEYR_WAIT_HANDLE_ABNORMAL_TRANS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid;
		}

		ObjID_t roleid;
	};

	/**
	*@brief client->world 拉取珍品交易记录请求
	*/
	class WorldAuctionGetTreasTransactionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_GET_TRANS_TRANSACTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

	/**
	*@brief world->client 拉取珍品交易记录返回
	*/
	class WorldAuctionGetTreasTransactionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_GET_TRANS_TRANSACTION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sales & buys;
		}
	
	public:
		std::vector<AuctionTransaction> sales;
		std::vector<AuctionTransaction> buys;
	};

	/**
	*@brief client->server 抢购购买拍卖道具
	*/
	class WorldAuctionRusyBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_RUSY_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}

	public:
		//guid
		ObjID_t		id;
		//数量
		UInt32		num;
	};

	/**
	*@ world->client 同步拍卖行公示商品显示子页签id
	*/
	class WorldAuctionSyncPubPageIds : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_SYNC_PUBDISPLAY_PAGEIDS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pageIds & shieldItemList;
		}

	public:
		//id
		std::vector<UInt32>		pageIds;
		// 拍卖行屏蔽道具列表
		std::vector<UInt32>		shieldItemList;
	};

	/**
	*@brief client->world 拍卖行关注
	*/
	class WorldAuctionAttentReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_ATTENT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & autionGuid;
		}
	public:
		//拍卖行商品id
		ObjID_t		autionGuid;
	};

	/**
	*@brief world->client 拍卖行关注返回
	*/
	class WorldActionAttentRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_ATTENT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & auction;
		}
	public:
		//结果
		UInt32 code;
		//拍卖行物品信息
		AuctionBaseInfo auction;
	};

	/**
	*@brief world->scene 拍卖行检查包裹
	*/
	class SceneAuctionCheckPackageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_CHECK_PACKAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & auctionGuid & items;
		}

	public:
		ObjID_t	playerId;
		//拍卖行商品id
		ObjID_t auctionGuid;
		std::vector<ItemReward> items;
	};

	/**
	*@brief scene->world 拍卖行检查包裹返回
	*/
	class SceneAuctionCheckPackageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_AUCTION_CHECK_PACKAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & auctionGuid;
		}

	public:
		ObjID_t	playerId;
		//拍卖行商品id
		ObjID_t auctionGuid;
	};

	/*
	*@brief	client->world 查询道具价格情况(最低出售价格列表)
	*/
	class WorldAuctionQueryItemPriceListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_PRICELIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & auctionState & itemTypeId & strengthen & enhanceType;
		}
		UInt8	type;
		UInt8   auctionState;
		UInt32	itemTypeId;
		UInt32	strengthen;
		UInt8	enhanceType;
	};

	/*
	*@brief world->client 道具价格情况返回
	*/
	class WorldAuctionQueryItemPriceListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_PRICELIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & auctionState & itemTypeId & strengthen & actionItems;
		}

		UInt8	type;
		UInt8   auctionState;
		UInt32	itemTypeId;
		UInt32	strengthen;
		std::vector<AuctionBaseInfo>    actionItems;
	};

	/**
	*@brief client->world 查询拍卖行指定附魔卡各等级数量请求
	*/
	class WorldAuctionQueryMagicOnsalesReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_MAGIC_ONSALES_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemTypeId;
		}
		UInt32	itemTypeId;
	};

	/**
	*@brief world->client 查询拍卖行指定附魔卡各等级数量返回
	*/
	class WorldAuctionQueryMagicOnsalesRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_MAGIC_ONSALES_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & magicOnsales;
		}
		UInt32	code;
		std::vector<AuctionMagicOnsale>  magicOnsales;
	};

	/**
	*@brief client->world 查询道具拍卖行交易记录请求
	*/
	class WorldAuctionQueryItemTransListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_TRANS_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemTypeId & strengthen & enhanceType & beadBuffId;
		}
		UInt32	itemTypeId;
		UInt32	strengthen;
		UInt8	enhanceType;
		UInt32  beadBuffId;
	};

	/**
	*@brief world->client 查询道具拍卖行交易记录返回
	*/
	class WorldAuctionQueryItemTransListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUCTION_QUERY_ITEM_TRANS_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & itemTypeId & strengthen & enhanceType & beadBuffId & transList;
		}
		UInt32	code;
		UInt32	itemTypeId;
		UInt32	strengthen;
		UInt8	enhanceType;
		UInt32  beadBuffId;
		std::vector<AuctionTransaction>  transList;
	};

}

#endif

