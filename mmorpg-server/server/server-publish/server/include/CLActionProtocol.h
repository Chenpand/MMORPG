#ifndef _CL_ACTION_PROTOCOL_H_
#define _CL_ACTION_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace Protocol
{
	/**
	 *@brief server->client 通知开始一个持续动作（即读条）
	 */
	class SceneBeginAction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEGIN_ACTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actionType & msg & secs;
		}

	public:
		//读条动作类型 0：无 1：采集 2：钓鱼
		UInt8		actionType;
		//提示
		std::string	msg;
		//剩余秒数
		UInt16		secs;
	};

	/**
	 *@brief server->client 中断当前动作
	 */
	class SceneSuspendAction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUSPEND_ACTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
};

#endif
