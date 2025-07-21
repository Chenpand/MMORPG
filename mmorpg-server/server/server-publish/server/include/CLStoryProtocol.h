#ifndef _CL_STORY_PROTOCOL_H_
#define _CL_STORY_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->server 客户端通知结束一个剧情结束
	 */
	class SceneNotifyStopStory : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_STOP_STORY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//剧情id
		UInt32 id;
	};

	/**
	*@brief client->server 客户端通知结束一个新手引导
	*/
	class SceneNotifyNewBoot : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_NEW_BOOT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//剧情id
		UInt32 id;
	};

	class SceneNotifyBootFlag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_BOOT_FLAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bootId;
		}

	public:
		//剧情id
		UInt32 bootId;
	};

}


#endif
