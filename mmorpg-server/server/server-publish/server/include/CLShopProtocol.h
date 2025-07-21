#ifndef _CL_SHOP_PROTOCOL_H_
#define _CL_SHOP_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	*@brief �˺��̵��ѯ����
	*/
	struct AccountShopQueryIndex
	{
		AccountShopQueryIndex() : shopId(0), tabType(0), jobType(0) {}
		AccountShopQueryIndex(UInt32 shopid, UInt8 tabtype, UInt8 jobtype)
		: shopId(shopid), tabType(tabtype), jobType(jobtype) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & tabType & jobType;
		}

		// ��ѯ���̵�id
		UInt32 shopId;
		// ��ѯ��ҳǩ���
		UInt8 tabType;
		// ��ѯ��ְҵ���
		UInt8 jobType;
	};

	/**
	*@brief �˺��̵���Ʒ��Ϣ
	*/
	struct AccountShopItemInfo
	{
		AccountShopItemInfo() :shopItemId(0), tabType(0), jobType(0), itemDataId(0), itemNum(0), accountRefreshType(0),
		accountLimitBuyNum(0), accountRestBuyNum(0), roleRefreshType(0), roleLimitBuyNum(0), roleRestBuyNum(0), extensibleCondition(0),
		needPreviewFunc(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopItemId & shopItemName & tabType & jobType & itemDataId & itemNum & costItems & accountRefreshType
				& accountRefreshTimePoint & accountLimitBuyNum & accountRestBuyNum & accountBuyRecordNextRefreshTimestamp
				& roleRefreshType & roleRefreshTimePoint & roleBuyRecordNextRefreshTimestamp & roleLimitBuyNum & roleRestBuyNum 
				& extensibleCondition & needPreviewFunc;
		}

		// ��Ʒid
		UInt32 shopItemId;
		// ��Ʒ����
		std::string shopItemName;
		// ҳǩ���
		UInt8 tabType;
		// ְҵ���
		UInt8 jobType;
		// �ϼܵ���
		UInt32 itemDataId;
		// �ϼ�����
		UInt32 itemNum;
		// �������ĵ���
		ItemRewardVec costItems;

		// �˺�ˢ������(TimeUtil::RefreshType)
		UInt8 accountRefreshType;
		// �˺�ˢ��ʱ���
		std::string accountRefreshTimePoint;

		// �˺��޹�����(0:���޹�)
		UInt32 accountLimitBuyNum;
		// �˺�ʣ�๺������
		UInt32 accountRestBuyNum;
		// �˺Ź����¼�´�ˢ��ʱ��
		UInt32 accountBuyRecordNextRefreshTimestamp;

		// ��ɫˢ������(TimeUtil::RefreshType)
		UInt8 roleRefreshType;
		// ��ɫˢ��ʱ���
		std::string roleRefreshTimePoint;
		// ��ɫ�����¼�´�ˢ��ʱ��
		UInt32 roleBuyRecordNextRefreshTimestamp;

		// ��ɫ�޹�����(0:���޹�)
		UInt32 roleLimitBuyNum;
		// ��ɫʣ�๺������
		UInt32 roleRestBuyNum;

		// ��չ����
		UInt32 extensibleCondition;
		// �Ƿ���ҪԤ����ť
		UInt8 needPreviewFunc;
	};

	/**
	*@brief client->world �˺��̵���Ʒ��ѯ����
	*/
	class WorldAccountShopItemQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_QUERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & queryIndex;
		}

	public:
		// ��ѯ����
		AccountShopQueryIndex queryIndex;
	};

	/**
	*@brief world->client �˺��̵���Ʒ��ѯ����
	*/
	class WorldAccountShopItemQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_QUERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & queryIndex & shopItems & nextGroupOnSaleTime;
		}

	public:
		// ������
		UInt32 resCode;
		// ��ѯ����
		AccountShopQueryIndex queryIndex;
		// ��Ʒ��
		std::vector<AccountShopItemInfo> shopItems;
		// ��һ����Ʒ�ϼ�ʱ��
		UInt32 nextGroupOnSaleTime;
	};

	/**
	*@brief client->world �˺��̵���Ʒ��������
	*/
	class WorldAccountShopItemBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_BUY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & queryIndex & buyShopItemId & buyShopItemNum;
		}

	public:
		// ��ѯ����
		AccountShopQueryIndex queryIndex;
		// �������Ʒid
		UInt32 buyShopItemId;
		// ������Ʒ����
		UInt32 buyShopItemNum;
	};

	/**
	*@brief world->client �˺��̵���Ʒ���򷵻�
	*/
	class WorldAccountShopItemBuyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_BUY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & queryIndex & buyShopItemId & buyShopItemNum 
				& accountRestBuyNum & roleRestBuyNum;
		}

	public:
		// ������
		UInt32 resCode;
		// ��ѯ����
		AccountShopQueryIndex queryIndex;
		// �������Ʒid
		UInt32 buyShopItemId;
		// ��������
		UInt32 buyShopItemNum;
		// �˺�ʣ�๺������
		UInt32 accountRestBuyNum;
		// ��ɫʣ�๺������
		UInt32 roleRestBuyNum;
	};

	/**
	*@brief world->client �˺��̵���Ʒ���򷵻�
	*/
	class WorldAccountShopItemBuyedNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_BUYED_NOTIFY_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & shopId & shopItemId & shopItemNum;
		}

	public:
		//��ɫid
		ObjID_t playerId;
		// �̵�id
		UInt32 shopId;
		// �������Ʒid
		UInt32 shopItemId;
		// ��������
		UInt32 shopItemNum;
	};
};

#endif
