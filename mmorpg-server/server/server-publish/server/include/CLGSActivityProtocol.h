#ifndef _CL_GAMESTART_ACTIVITY_PROTOCOL_H_
#define _CL_GAMESTART_ACTIVITY_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief server->client ֪ͨ����ʱ��
	*/
	class WorldNotifyGameStartTime : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GAME_STARTTIME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time & days;
		}

	public:
		// ����ʱ��
		UInt32	time;
		//�����ǿ����ڼ���
		UInt32	days;
	};

	/**
	*@brief world->scene ֪ͨ����ʱ�䵽������
	*/
	class WorldNotifyGameStartTimeToS : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GAME_STARTTIME_TOS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time & firstTime;
		}

	public:
		// ����ʱ��
		UInt32	time;
		// ��һ�ο���ʱ��
		UInt32 firstTime;
	};

	/**
	*@brief client->server �������
	*/
	class SceneGSActivityStoreGold : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GSACTIVITY_STORE_GOLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		// �������ͣ�0 ��1000��1 ��10000
		UInt8	type;
	};

	/**
	*@brief server->client ͬ��������Ϣ
	*/
	class SceneGSActivitySyncCaishen : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GSACTIVITY_SYNC_CAISHEN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & canget & days & gold & bindgold;
		}

	public:
		// �Ƿ���죬0 ���ɣ�1 ����
		UInt8	canget;
		// ��¼����
		UInt8	days;
		// ��
		UInt32	gold;
		// ���
		UInt32	bindgold;
	};
}

#endif

