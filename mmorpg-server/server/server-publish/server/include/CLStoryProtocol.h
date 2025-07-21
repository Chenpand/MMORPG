#ifndef _CL_STORY_PROTOCOL_H_
#define _CL_STORY_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->server �ͻ���֪ͨ����һ���������
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
		//����id
		UInt32 id;
	};

	/**
	*@brief client->server �ͻ���֪ͨ����һ����������
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
		//����id
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
		//����id
		UInt32 bootId;
	};

}


#endif
