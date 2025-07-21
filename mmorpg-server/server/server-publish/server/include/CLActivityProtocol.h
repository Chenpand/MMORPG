#ifndef _CL_WORLD_LEVEL_PROTOCOL_H_
#define _CL_WORLD_LEVEL_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace ClProtocol
{
	/**
	*@brief scene->world 同步到world活动变量
	*/
	class WorldActivityCounter : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_WORLD_LEVEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_PlayerId & m_Value;
		}
	public:
		ObjID_t m_PlayerId;
		UInt32 m_Value;
	};
}

#endif

