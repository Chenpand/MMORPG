#ifndef _CL_TRADE_PROTOCOL_H_
#define _CL_TRADE_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief server->client ֪ͨ���׿�ʼ
	 */
	class SceneTradeNotifyBegin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_NOTIFY_BEGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & otherName;
		}

	public:
		//�Է�����
		std::string	otherName;
	};

	/**
	 *@brief client->server ������Ӻ��Ƴ�����
	 */
	class SceneTradeUpdateItemsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_UPDATEITEMS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num & type;
		}

	public:
		//����Ψһid
		ObjID_t	itemid;
		//���������Ƴ�ʱ�����Ƴ�
		UInt16	num;
		//0Ϊ��� 1Ϊ�Ƴ�
		UInt8	type;
	};

	/**
	 *@brief client->server �����������
	 */
	class SceneTradeUpdateSilverReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_UPDATESILVER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & silver;
		}

	public:
		//������
		UInt32 silver;
	};

	/**
	 *@brief server->client ͬ���Լ����׵���
	 */
	class SceneTradeSyncSelfItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SYNC_SELFITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

	public:
		//�����б� id������
		std::map<ObjID_t,UInt16> items;
	};

	/**
	 *@brief server->client ֪ͨ�Է����ӵ��� ��ʽ:objid + dataid(UInt32) + num(UInt16) + data(��CLItemProtocol.h) 
	 */
	class SceneTradeNotifyOtherAddItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_NOTIFY_OTHER_ADDITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/** 
	 *@brief server->client ֪ͨ�Է��Ƴ�����
	 */
	class SceneTradeNotifyOtherRemoveItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_NOTIFY_OTHER_REMOVEITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid;
		}

	public:
		//����Ψһid
		ObjID_t	itemid;
	};

	/**
	 *@brier server->client ͬ��������
	 */
	class SceneTradeSyncSilver : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SYNC_SILVER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & selfSilver & otherSilver;
		}

	public:
		//��������
		UInt32	selfSilver;
		//�Է�����
		UInt32	otherSilver;
	};

	/**
	 *@brief client->server ���ý���״̬
	 */
	class SceneTradeSetStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SET_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}

	public:
		//״̬
		UInt8	status;
	};

	/**
	 *@brief server->client ͬ������״̬
	 */
	class SceneTradeSyncStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SYNC_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & selfStatus & otherStatus;
		}

	public:
		//����״̬
		UInt8	selfStatus;
		//�Է�״̬
		UInt8	otherStatus;
	};
};

#endif
