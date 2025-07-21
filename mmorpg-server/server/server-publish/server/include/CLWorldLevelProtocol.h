#ifndef __CL_WORLD_LEVEL_PROTOCOL_H__
#define __CL_WORLD_LEVEL_PROTOCOL_H__

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief server->client ֪ͨ��ҵ�ǰ����ȼ�
	*/
	class WorldNotifyWorldLevel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_WORLD_LEVEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uWorldLevel;
		}
	public:
		UInt32	uWorldLevel;
	};

	/**
	*@brief World->scene ֪ͨ��������ǰ����ȼ�
	*/
	class WorldNotifyWorldLevelToS : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_WORLD_LEVEL_TOS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uWorldLevel;
		}
	public:
		UInt32	uWorldLevel;
	};

}

#endif