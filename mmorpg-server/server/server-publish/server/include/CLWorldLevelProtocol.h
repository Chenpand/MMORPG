#ifndef __CL_WORLD_LEVEL_PROTOCOL_H__
#define __CL_WORLD_LEVEL_PROTOCOL_H__

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief server->client 通知玩家当前世界等级
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
	*@brief World->scene 通知场景服当前世界等级
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