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

        // ��������
        UInt8                       type;
		// ��Ʒ״̬ [1]:��ʾ [2]:�ϼ�
		UInt8						goodState;
        // ��Ʒ�����ͣ���Ӧö��AuctionMainItemType��
        UInt8                       itemMainType;
        // ��Ʒ������
        std::vector<UInt32>         itemSubTypes;
		// �ų���Ʒ������
		std::vector<UInt32>         excludeItemSubTypes;
        // ��ƷID
        UInt32                      itemTypeID;
        // Ʒ��
        UInt8                       quality;
        // �����Ʒ�ȼ�
        UInt8                       minLevel;
        // �����Ʒ�ȼ�
        UInt8                       maxLevel;
        // ���ǿ���ȼ�
        UInt8                       minStrength;
        // ���ǿ���ȼ�
        UInt8                       maxStrength;
        // ����ʽ����Ӧö��AuctionSortType��
        UInt8                       sortType;
        // ҳ��
        UInt16                      page;
        // ÿҳ���ٸ���Ʒ
        UInt8                       itemNumPerPage;
		// ǿ����ǿ���ȼ�			
		UInt32						couponStrengthToLev;
		// �Ƿ��ע[0]�ǹ�ע, [1]��ע
		UInt8						attent;
		// ְҵ
		std::vector<UInt8>			occus;
    };
	/**
	*@brief client->server ���������б�
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
	*@brief server->client ͬ�������б�
	*		��ʽ����ʼλ��(UInt16) + ������(UInt16) + UInt8(����) + [����(UInt8, 0 ���ߣ�1 �𱴣�2 ����) + �۸�(UInt32) + ��������(UInt8) + ����ʱ��(UInt32) + data] ...
	*		data	0 : Guid(UInt64) + itemid(UInt32) + [ItemAttr(UInt8) + value][ItemAttr(UInt8) + value]...[0]
	*				1��2 : Guid(UInt64) + ��������(UInt32)
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
	*@brief client->server �����Լ��ĳ����б�
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
	*@brief server->client ͬ���Լ��ĳ����б�
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
	*@brief client->server ��������
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
		// ��������
		UInt8	type;
		// ��������id
		ObjID_t	id;
		// ��������id
		UInt32	typeId;
		// ����
		UInt32	num;
		// �۸�
		UInt32	price;
        // ����ʱ��(AuctionSellDuration)
        UInt8   duration;
		// ǿ���ȼ�
		UInt8  strength;
		// ����buffid
		UInt32  buffId;
		// �Ƿ������ϼ�
		UInt8   isAgain;
		// �����ϼ�guid
		UInt64 auctionGuid;
		// ����װ���������� ��Ч0/����1/����2/����3/����4
		UInt8  enhanceType;
	};

	/**
	*@brief server->client ��ҵ������б�����һ��
	*		��ʽ��[������Ϣ����ʽ���ϣ�]
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
	*@brief client->server ������������
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
		//����
		UInt32		num;
	};
	
	/**
	*@brief client->server ȡ������
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
	*@brief server->client ֪ͨ�ͻ���ɾ������
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
	*@brief server->client ֪ͨ�ͻ�����Ҫˢ�������б�
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
	*@brief world->scene �������
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
		// ���id
		ObjID_t	playerId;
		// ��������
		UInt8	type;
		// ��������id
		ObjID_t	id;
		// ������������ID
		UInt32	typeId;
		// ����
		UInt32	num;
		// �۸�
		UInt32	price;
		// �۸�Ļ�������
		UInt8	pricetype;
        // Ѻ��
        UInt32  deposit;
        // ������
        UInt32  counterFee;
        // ����ʱ��(AuctionSellDuration)
        UInt8   duration;
		// ǿ���ȼ�
		UInt8  strength;
		// ����buffid
		UInt32  beadbuffId;
		// �Ƿ������ϼ�
		UInt8   isAgain;
		// �����ϼ�guid
		UInt64 auctionGuid;
		
	};

	/**
	*@brief scene->world �����б��ѯ����
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
		// ��Ʒ״̬(AuctionGoodState)
		UInt8							goodState;
		// �Ƿ��ע(AuctionAttentType)
		UInt8							attent;
	};

	/**
	*@brief client->server ����guid������Ʒ
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
		// Ψһid
		ObjID_t	id;
	};

	/**
	*@brief server->client ������Ʒ����
	*		��ʽ��[������Ϣ����ʽ���ϣ�]
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
    *@brief client->server ������Ʒguid����������Ʒ��ϸ��Ϣ
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
        // Ψһid
        ObjID_t	id;
    };

    /**
    *@brief server->client ����������Ʒ��ϸ��Ϣ����
    *		��ʽ��[WSItem��]
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
    *@brief client->server ��ѯ��Ʒ���Ƽ��۸�
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
    *@brief client->server ������Ʒ���Ƽ��۸�
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
	*@brief client->server ��ѯ�����е�������
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
	*@brief client->server ���������е�������
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
		// ��Ʒ״̬(AuctionGoodState)
		UInt8 goodState;
		std::vector<AuctionItemBaseInfo>		items;
	};

	/**
	*@brief client->scene ˢ��������ʱ������
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
	*@brief scene->client ˢ��������ʱ�䷵��
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
	*@brief client->scene ������������λ����
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
	*@brief scene->client ������������λ����
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
	*@brief	client->world ��ѯ���߼۸����(���ڽ���ƽ����, ����������ڳ��ۼ۸�)
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
	*@brief world->client ���߼۸��������
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
	*@brief scene->client ֪ͨ�쳣����
	*/
	class SceneNotifyAbnormalTransaction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_AUCTION_ABNORMAL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bNotify;
		}

		// boolֵ(false(0):�ر�֪ͨ��true(1):����֪ͨ)
		UInt8 bNotify;
	};

	/*
	*@brief client->scene �쳣���׼�¼��ѯ
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
	*@brief scene->client �쳣���׼�¼����
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

		// ���������
		UInt32 frozenMoneyType;
		// ������
		UInt32 frozenAmount;
		// �쳣����ʱ��
		UInt32 abnormalTransactionTime;
		// ��������ʱ��
		UInt32 backDeadline;
		// �������
		UInt32 backAmount;
		// �ѷ�����
		UInt32 backDays;
	};

	/**
	*@brief scene->world �쳣���׿۳��ʼ��еĵ���
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
		// �쳣����id
		ObjID_t  abnormalTransId;
		// Ҫ�۳������id
		ObjID_t owner;
		// Ҫ�۳��ĵ���id
		UInt32 itemDataId;
		// Ҫ�۳��ĵ�������
		UInt32 needRmNum;
		// ��һ���쳣����id
		ObjID_t preAbnormalTransId;
	};

	/**
	*@brief world->scene �۳��ʼ��еĵ��߷���
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
		// �쳣����id
		ObjID_t  abnormalTransId;
		// ��һ���쳣����id
		ObjID_t preAbnormalTransId;
		// Ҫ�۳������id
		ObjID_t owner;
		// Ҫ�۳��ĵ���id
		UInt32 itemDataId;
		// Ҫ�۳��ĵ�������
		UInt32 needRmNum;

		// �ɹ��۳��ĵ�������
		UInt32 successRmNum;
	};

	/**
	*@brief world->scene ֪ͨ��ѯ�������쳣����
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
	*@brief client->world ��ȡ��Ʒ���׼�¼����
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
	*@brief world->client ��ȡ��Ʒ���׼�¼����
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
	*@brief client->server ����������������
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
		//����
		UInt32		num;
	};

	/**
	*@ world->client ͬ�������й�ʾ��Ʒ��ʾ��ҳǩid
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
		// ���������ε����б�
		std::vector<UInt32>		shieldItemList;
	};

	/**
	*@brief client->world �����й�ע
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
		//��������Ʒid
		ObjID_t		autionGuid;
	};

	/**
	*@brief world->client �����й�ע����
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
		//���
		UInt32 code;
		//��������Ʒ��Ϣ
		AuctionBaseInfo auction;
	};

	/**
	*@brief world->scene �����м�����
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
		//��������Ʒid
		ObjID_t auctionGuid;
		std::vector<ItemReward> items;
	};

	/**
	*@brief scene->world �����м���������
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
		//��������Ʒid
		ObjID_t auctionGuid;
	};

	/*
	*@brief	client->world ��ѯ���߼۸����(��ͳ��ۼ۸��б�)
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
	*@brief world->client ���߼۸��������
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
	*@brief client->world ��ѯ������ָ����ħ�����ȼ���������
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
	*@brief world->client ��ѯ������ָ����ħ�����ȼ���������
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
	*@brief client->world ��ѯ���������н��׼�¼����
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
	*@brief world->client ��ѯ���������н��׼�¼����
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

