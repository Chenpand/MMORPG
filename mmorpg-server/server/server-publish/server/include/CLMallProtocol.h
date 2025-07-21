#ifndef _CL_MALL_PROTOCOL_H_
#define _CL_MALL_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include <CLMallDefine.h>

namespace CLProtocol
{
	/**
	*@brief client->server �����̳ǵ�������
	*/
	class WorldMallBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_uId & m_uNum;
		}
	public:
		// ����Id
		UInt32	m_uId;
		// ����
		UInt16	m_uNum;
	};

	/**
	*@brief server->client �����̳ǵ��߷���
	*/
	class WorldMallBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_code & m_mallItemId & m_restLimitNum & m_accountRestBuyNum;
		}
	public:
		UInt32	m_code;
		UInt32	m_mallItemId;
		Int32	m_restLimitNum;
		// �˺�ʣ�๺�����
		UInt32  m_accountRestBuyNum;
	};

	class CWMallBatchBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CW_MALL_BATCH_BUY_FASHION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & _buyItems;
		}
	public:
		std::vector<ItemReward>	_buyItems;
	};

	class SCMallBatchBuyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WC_MALL_BATCH_BUY_FASHION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & _code & _itemUids;
		}
	public:
		UInt32				_code;
		std::vector<UInt64>	_itemUids;
	};

	/**
	*@brief world->scene ��鹺���̳ǵ�������
	*/
	class WorldMallCheckBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_CHECK_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_uPlayerId & m_uAccId & m_uCost & m_uMoneyType & m_uItemDataId & m_uItemData & m_uMallItemId & m_uMallItemNum & m_uReceiver & mallLimitBuyType & m_uLimitNum
				& m_uTotalLimitNum & m_uGoodsType & m_uVipScore & m_uItemQuality & m_uBind & billingId & orderId & coins & moneyType & taocanId & m_DiscountRate
				& m_discountCouponId & multiple & mallType & deductionCouponId & tagType & creditPointDeduction;
		}
	public:
		// ���id
		ObjID_t	m_uPlayerId;
		// �˺�id
		UInt32	m_uAccId;
		// �������
		UInt32	m_uCost;
		// ��������
		UInt8	m_uMoneyType;
		// ����id
		std::vector<UInt32>	m_uItemDataId;
		// ����id-->����
		std::map<UInt32, UInt32>	m_uItemData;
		// �̳ǵ���id
		UInt32	m_uMallItemId;
		// ����
		UInt16	m_uMallItemNum;
		// ��õ��ߵ����id
		ObjID_t	m_uReceiver;
		// �޹�ˢ������
		UInt8	mallLimitBuyType;
		// �޹�����
		UInt32 m_uLimitNum;
		// ���޹�����
		UInt32 m_uTotalLimitNum;
		// ��Ʒ����
		UInt8 m_uGoodsType;
		// ������õ�vip����
		UInt16 m_uVipScore;
		// ����Ʒ��
		UInt8	m_uItemQuality;
		// ��
		UInt8	m_uBind;
		// billing����id
		UInt32	billingId;
		// ������
		std::string	orderId;
		// ���
		std::string	coins;
		// ��������
		std::string	moneyType;
		// �ײ�id
		std::string	taocanId;
		// ������
		UInt32 m_DiscountRate;
		// �ۿ�ȯid
		UInt32 m_discountCouponId;
		// ���ֱ���
		UInt8 multiple;
		// �̳����
		UInt8 mallType;
		// �ֿ�ȯ
		UInt32 deductionCouponId;
		// ��ǩ
		UInt8 tagType;
		// ���õ�ȯ�ֿ�
		UInt32 creditPointDeduction;
	};

	/**
	*@brief scene->world ��鹺���̳ǵ��߷���
	*/
	class WorldMallCheckBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_CHECK_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_uPlayerId & m_uRet & m_uMallItemId & m_getItemUid & m_uMallItemNum & m_uReceiver & m_restLimitNum;
		}
	public:
		// ���id
		ObjID_t	m_uPlayerId;
		// ���ؽ��
		UInt32	m_uRet;
		// �̳ǵ���id
		UInt32	m_uMallItemId;
		// ��õ���uid
		std::vector<UInt64>	m_getItemUid;
		// ����
		UInt16	m_uMallItemNum;
		// ��õ��ߵ����id
		ObjID_t	m_uReceiver;
		// ʣ���޹�����
		UInt32  m_restLimitNum;
	};
	
	/**
	*@brief client->server ������Ʒ
	*/
	class WorldMallQueryItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_tagType & m_type & m_subType & m_moneyType & m_job & m_updateTime & m_IsPersonalTailor;
		}

		UInt8 m_tagType;			//��������,1-����
		UInt8 m_type;				//�̳����
		UInt8 m_subType;			//�̳������
		UInt8	m_moneyType;		//��������
		UInt8 m_job;				//ְҵ
		UInt32	m_updateTime;		// ���µ�ʱ��(s)
		UInt8 m_IsPersonalTailor;	//�Ƿ�˽�˶���
	};

	/**
	*@brief client->server ������Ʒ�����ϸ
	*/
	class WorldMallQueryItemDetailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_DETAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_MallItemId;
		}

		UInt32	m_MallItemId;	//�̳���Ʒid
	};

	/**
	*@brief server->client ������Ʒ�����ϸ����
	*/
	class WorldMallQueryItemDetailRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_DETAIL_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & details;
		}

		std::vector<MallGiftDetail>	details;	//�̳������ϸ
	};

	
	
	// server->client ������Ʒ����
	class WorldMallQueryItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallType & m_items;
		}

		//�̳����
		UInt8 mallType;
		std::vector<MallItemInfo>	m_items;
	};
	
	/**
	*@brief server->client ͬ��������Ʒ�����仯
	*/
	class WorldMallLimitedItemChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_LIMITED_ITEM_CHANGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & totalnum & limitnum;
		}

	public:
		// ��Ʒid
		UInt32	id;
		// ʣ������
		UInt16	totalnum;
		// �޹�����
		UInt16	limitnum;
	};	

	struct QueryGood
	{
		QueryGood() :id(0), bLimit(0), discountRate(0) {}
		~QueryGood(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & bLimit & discountRate;
		}

		UInt32	id;
		UInt8	bLimit;
		UInt8	discountRate;
	};
	/**
	*@brief w->s ˢ���̳��޹�����
	*/
	class WSRefreshLimiteBuyNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_REFRESH_LIMITEBUYNUM)
	public:
		WSRefreshLimiteBuyNum()
		{
			playerid = 0;
			isPersonalTailor = 0;
			mallType = 0;
			isQuerySingleGoods = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerid & qgs & isPersonalTailor & mallType & isQuerySingleGoods;
		}

	public:
		ObjID_t playerid;
		// ��Ʒid
		std::vector<QueryGood>	qgs;
		// �Ƿ�˽�˶���
		UInt8 isPersonalTailor;
		//�̳����
		UInt8 mallType;
		// �Ƿ��ѯ������Ʒ
		UInt8 isQuerySingleGoods;
	};
	
	/**
	*@brief s->w ˢ���̳��޹���������
	*/
	class SWRefreshLimiteBuyNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_REFRESH_LIMITEBUYNUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerid & qgs & code & isPersonalTailor & mallType & isQuerySingleGoods;
		}

	public:
		ObjID_t playerid;
		
		// ��Ʒid
		std::vector<QueryGood>	qgs;
		UInt32 code;
		UInt8 isPersonalTailor;
		//�̳����
		UInt8 mallType;
		// �Ƿ��ѯ������Ʒ
		UInt8 isQuerySingleGoods;
	};

	/**
	*@brief client ->server �����̳���ʱ�������
	*/
	class WorldMallGiftPackActivateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_GIFT_PACK_ACTIVATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_GiftPackActCond;
		}

	public:
		UInt8 m_GiftPackActCond;
	};

	/**
	*@brief scene ->server �����̳���ʱ�������
	*/
	class MallGiftPackActivateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MALL_GIFT_PACK_ACTIVATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_PlayerId & m_GiftPackActCond;
		}

	public:
		ObjID_t m_PlayerId;
		UInt8 m_GiftPackActCond;
	};

	/**
	*@brief server->client �����̳���ʱ�������
	*/
	class WorldMallGiftPackActivateRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_GIFT_PACK_ACTIVATE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_Items & m_Code;
		}

	public:
		std::vector<MallItemInfo> m_Items;
		UInt32 m_Code;
	};

	/**
	*@brief world->client ͬ���̳�����״̬
	*/
	class SyncWorldMallGiftPackActivityState : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MALL_GIFT_PACK_ACTIVITY_SYNC_STATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_State;
		}

	public:
		UInt8 m_State;
	};

	/**
	*@brief scene->world �̳Ǵ�����������
	*/
	class MallActivateCondReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_ACTIVATE_COND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_RoleId & m_ActivateCond & m_ActivateType;
		}

	public:
		// ��ɫid
		UInt64		m_RoleId;
		// ��������
		UInt8		m_ActivateCond;
		// ��������
		UInt8		m_ActivateType;
	};

	/**
	*@brief world->scene ͬ���̳�˽�˶���״̬
	*/
	class WorldSyncMallPersonalTailorState : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_PERSONAL_TAILOR_SYNC_STATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_State & m_GoodsId;
		}

	public:
		// ͬ��״̬
		UInt8		m_State;
		// ���´�������Ʒid
		UInt32		m_GoodsId;
	};

	/**
	*@brief world->scene �����̳Ǵ����������ݿ�֪ͨ
	*/
	class WolrdLoadConditionActivateDBNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_LOAD_CONDITION_ACTIVATE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_RoleId & m_IsLoaded;
		}

	public:
		// ��ɫid
		UInt64		m_RoleId;
		// �Ƿ�������
		UInt8		m_IsLoaded;
	};	

	/**
	*@brief client->world ���ݵ���id��ȡ�̳ǵ�������
	*/
	class WorldGetMallItemByItemIdReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_MALL_ITEMS_BY_ITEM_ID_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_ItemId;
		}

		// ����id(�����̳ǵ��ߵ�id)
		UInt32 m_ItemId;
	};

	/**
	*@brief world->client ���ݵ���id��ȡ�̳ǵ��߷���(����Ϊչʾ��)
	*/
	class WorldGetMallItemByItemIdRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_MALL_ITEMS_BY_ITEM_ID_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & m_ItemId & m_MallItem;
		}

		// ������
		UInt32 retCode;
		// ����id
		UInt32 m_ItemId;
		// ӳ����̳ǵ���
		MallItemInfo m_MallItem;
	};

	/**
	*@brief client->world �̳ǲ�ѯ������Ʒ����
	*/
	class WorldMallQuerySingleItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_SINGLE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallItemId;
		}

		// �̳ǵ���id
		UInt32 mallItemId;
	};

	/**
	*@brief world->client �̳ǲ�ѯ������Ʒ����
	*/
	class WorldMallQuerySingleItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_SINGLE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & mallItemInfo;
		}

		// ������
		UInt32 retCode;
		// �̳ǵ���
		MallItemInfo mallItemInfo;
	};

	/**
	*@brief world->client ���ӵ�е��̳ǹ�������ͬ��
	*/
	class WorldPlayerMallBuyGotInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PLAYER_MALL_BUY_GOT_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallBuyGotInfo;
		}

	public:
		MallBuyGotInfo mallBuyGotInfo;
	};

	/**
	*@brief world->client �����̳���Ʒ
	*/
	class WorldPushMallItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PUSH_MALL_ITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallItems;
		}

	public:
		MallItemInfosVec mallItems;
	};

	/**
	*@brief client->world ��ȡ��ֵ���͵����б�����
	*/
	class WorldGetRechargePushItemsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_RECHARGE_PUSH_ITEMS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ��ȡ��ֵ���͵����б���
	*/
	class WorldGetRechargePushItemsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_RECHARGE_PUSH_ITEMS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemVec;
		}

		// ������
		UInt32 retCode;
		// ���͵���
		RechargePushItemVec itemVec;
	};

	/**
	*@brief client->world �����ֵ���͵�������
	*/
	class WorldBuyRechargePushItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BUY_RECHARGE_PUSH_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pushId;
		}

		UInt32 pushId;
	};

	/**
	*@brief world->client �����ֵ���͵��߷���
	*/
	class WorldBuyRechargePushItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BUY_RECHARGE_PUSH_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & pushId & itemVec;
		}

		// ������
		UInt32 retCode;
		// ����ID
		UInt32 pushId;
		// ���͵���
		RechargePushItemVec itemVec;
	};

	/**
	*@brief world->scene or center ����ֵ��������
	*/
	class WorldCheckRechargePushReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_RECHARGE_PUSH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & roleId & pushId & lvLower & lvToplimit & vipLvLower & judgePropIDs & daysBeforMin & daysBeforeMax & recent & parameterType & parameter1 & parameter2;
		}

		UInt32 zoneId;
		UInt64 roleId;
		UInt32 pushId;
		UInt32 lvLower;
		UInt32 lvToplimit;
		UInt32 vipLvLower;
		std::vector<UInt32> judgePropIDs;
		UInt32 daysBeforMin;
		UInt32 daysBeforeMax;
		UInt32 recent;
		UInt32 parameterType;
		UInt32 parameter1;
		UInt32 parameter2;
	};

	/**
	*@brief scene or center->world ����ֵ���ͷ���
	*/
	class WorldCheckRechargePushRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_RECHARGE_PUSH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & pushId & zoneId;
		}

		// ������
		UInt32 retCode;
		UInt64 roleId;
		UInt32 pushId;
		UInt32 zoneId;
	};

	/**
	*@brief world->scene ���������ֵ��������
	*/
	class WorldSceneBuyRechargePushItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCENE_BUY_RECHARGE_PUSH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & pushId & price & item;
		}

		UInt64 roleId;
		UInt32 pushId;
		UInt32 price;
		ItemReward item;
	};

	/**
	*@brief world->scene ���������ֵ���͵��߷���
	*/
	class WorldSceneBuyRechargePushItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCENE_BUY_RECHARGE_PUSH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & pushId;
		}

		// ������
		UInt32 retCode;
		UInt64 roleId;
		UInt32 pushId;
	};


	/**
	*@brief client->scene �޶��̳���װ�����ѯ
	*/
	class SceneMallFashionLimitedSuitStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MALL_FASHION_LIMITED_SUIT_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};

	/**
	*@brief client->scene �޶��̳���װ�������
	*/
	class SceneMallFashionLimitedSuitStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MALL_FASHION_LIMITED_SUIT_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
		//������װ�е���id
		std::vector<UInt32> result;
	};

}

#endif