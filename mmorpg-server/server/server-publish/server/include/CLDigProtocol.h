#ifndef _CL_DIG_PROTOCOL_H_
#define _CL_DIG_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLDigDefine.h"

namespace CLProtocol
{
	/**
	*@brief server->client ͬ��������Ϣ
	*/
	class WorldDigInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & info;
		}
	public:
		UInt32 mapId;
		DigInfo info;
	};

	/**
	*@brief server->client ͬ������ˢ����Ϣ
	*/
	class WorldDigRefreshSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_REFRESH_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & infos;
		}

	public:
		UInt32 mapId;
		std::vector<DigInfo> infos;
	};

	/**
	*@brief server->client ͬ�����ص�ͼ�������
	*/
	class WorldDigPlayerSizeSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_PLAYER_SIZE_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & playerSize;
		}

	public :
		UInt32 mapId;
		UInt32 playerSize;
	};

	/**
	*@brief server->client ͬ���ڱ���¼
	*/
	class WorldDigRecordInfoSync :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_RECORD_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		DigRecordInfo info;
	};

	/**
	*@brief client->server ����򿪱��ص�ͼ
	*/
	class WorldDigMapOpenReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_MAP_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		UInt32 mapId;
	};

	/**
	*@brief server->client ����򿪱��ص�ͼ����
	*/
	class WorldDigMapOpenRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_MAP_OPEN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & mapId & infos & playerSize;
		}

	public:
		UInt32 result;
		UInt32 mapId;
		std::vector<DigInfo> infos;
		UInt32 playerSize;
	};

	/**
	*@brief client->server ����رձ��ص�ͼ
	*/
	class WorldDigMapCloseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_MAP_CLOSE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		UInt32 mapId;
	};

	/**
	*@brief server->client ����رձ��ص�ͼ����
	*/
	class WorldDigMapCloseRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_MAP_CLOSE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server ����رձ��ص�ͼ
	*/
	class WorldDigWatchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_WATCH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & index;
		}
	public:
		UInt32 mapId;
		UInt32 index;
	};

	/**
	*@brief server->client ����鿴���ط���
	*/
	class WorldDigWatchRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_WATCH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & mapId & index & info;
		}
	public:
		UInt32 result;
		UInt32 mapId;
		UInt32 index;
		DigDetailInfo info;
	};

	/**
	*@brief client->server ����򿪱���
	*/
	class WorldDigOpenReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & index;
		}
	public:
		UInt32 mapId;
		UInt32 index;

	};

	/**
	*@brief server->client ����򿪱��ط���
	*/
	class WorldDigOpenRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_OPEN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & itemIndex & itemId & itemNum;
		}
	public:
		UInt32 result;
		UInt32 itemIndex;
		UInt32 itemId;
		UInt32 itemNum;
	};

	/**
	*@brief client->server ���󱦲ص�ͼ��Ϣ
	*/
	class WorldDigMapInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_MAP_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	
	/**
	*@brief server->client ���󱦲ص�ͼ��Ϣ����
	*/
	class WorldDigMapInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_MAP_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & digMapInfos;
		}

	public:
		UInt32 result;
		std::vector<DigMapInfo> digMapInfos;
	};

	/**
	*@brief client->server �����ڱ���¼��Ϣ
	*/
	class WorldDigRecordsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_RECORDS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client �����ڱ���¼��Ϣ����
	*/
	class WorldDigRecordsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DIG_RECORDS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & infos;
		}

	public:
		UInt32 result;
		std::vector<DigRecordInfo> infos;
	};

}


#endif //_CL_DIG_PROTOCOL_H_