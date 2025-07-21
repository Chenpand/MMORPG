#ifndef _CL_GIFTBAG_PROTOCOL_H_
#define _CL_GIFTBAG_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	 *@brief server->client ���������������
	 */
	class SceneSyncOnlineGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ONLINE_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time;
		}
		//��ȡʱ��(��)��0��ʾû�������
		UInt32	time;
	};

	/**
	 *@brief server->client ���͵ȼ��������
	 */
	class SceneSyncLevelGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_LEVEL_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & level;
		}
		//����ȡ�ȼ���0��ʾû�������
		UInt16	level;
	};

	/**
	 *@brief client->server ��ȡ�������
	 */
	class SceneGetGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GET_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		//������� 1Ϊ������� 2Ϊ�ȼ����
		UInt8	type;
	};

	/**
	*@brief server->client ͬ������
	*/
	class SceneSyncActiveGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ACTIVEGIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapGiftBags;
		}
	public:
		// �����Ϣ��id + ״̬��״̬��1 ���죬2 ����
		std::map<UInt32, UInt8>	mapGiftBags;
	};

	/**
	*@brief client->server ��ȡ����
	*/
	class SceneGetActiveGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GET_ACTIVEGIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
	public:
		// ���id
		UInt32	id;
	};

	/**
	*@brief server->client ͬ��Q��������
	*/
	class SceneSyncPointUsed : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_POINTUSED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uPoint;
		}
	public:
		//Q��
		UInt32	uPoint;
	};
	
	/**
	*@brief server->client ͬ�����ռ���
	*/
	class SceneSyncFestivalCount : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_FESTIVAL_COUNT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapInfo;
		}
	public:
		// �������� + �����������Ϊ"mid_autumn"��"mid_autumn_sum"
		std::map<std::string, UInt32>	mapInfo;
	};

	/** 
	 *@brief client->server ���ջ��������
	 */
	class SceneFestivalCounterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FESTIVAL_COUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

	public:
		//������
		std::string	name;
	};

	/**
	 *@brief server->client ���ؽ��ջ����
	 */
	class SceneFestivalCounterRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FESTIVAL_COUNT_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & value;
		}

	public:
		//������
		std::string	name;
		//��ֵ
		UInt32	value;
	};

	/**
	*@brief client -> server ���������
	*/
	class SceneBuyGiftBag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BUY_GIFTBAG)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//���ID
		UInt32 id;
	};

	struct GiftSyncInfo
	{
		GiftSyncInfo() : id(0), itemId(0), itemNum(0), weight(0), equipType(0), strengthen(0), isTimeLimit(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & itemId & itemNum & recommendJobs & weight & validLevels & equipType & strengthen & isTimeLimit;
		}

		// ��Ʒid
		UInt32 id;
		// ����id
		UInt32 itemId;
		// ��������
		UInt32 itemNum;
		// �Ƽ�ְҵ
		std::vector<UInt8> recommendJobs;
		// ���Ȩ��
		UInt32 weight;
		// ��Ч�ȼ�
		std::vector<UInt32> validLevels;
		// װ������
		UInt8	equipType;
		// ǿ���ȼ�
		UInt32	strengthen;
		// �Ƿ�ʱ�޵�
		UInt8	isTimeLimit;
	};

	struct GiftPackSyncInfo
	{
		GiftPackSyncInfo() : id(0), filterType(0), filterCount(-1), uiType(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & filterType & filterCount & gifts & uiType & description;
		}

		// ���ID
		UInt32 id;
		// ɸѡ����
		UInt32 filterType;
		// ���˳�����Ʒ����
		Int32  filterCount;
		// ��Ʒ
		std::vector<GiftSyncInfo> gifts;
		// UI ����
		UInt8 uiType;
		// ����
		std::string description;
	};

	/**
	*@brief client -> scene �����Ϣ����
	*/
	class SceneGiftPackInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GIFT_PACK_INFO_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftPackIds;
		}

	public:
		// ���ID
		std::vector<UInt32> giftPackIds;
	};

	/**
	*@brief scene -> client �����Ϣ����
	*/
	class SceneGiftPackInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GIFT_PACK_INFO_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftPacks;
		}

	public:
		std::vector<GiftPackSyncInfo> giftPacks;
	};
}

#endif
