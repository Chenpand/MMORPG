#ifndef _CL_HEAD_FRAME_PROTOOL_H_
#define _CL_HEAD_FRAME_PROTOOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->server ͷ�������
	*/
	class SceneHeadFrameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct HeadFrame
	{
		HeadFrame() : headFrameId(0), expireTime(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & headFrameId & expireTime;
		}

		// ͷ���id
		UInt32 headFrameId;
		// ����ʱ��
		UInt32 expireTime;
	};

	/**
	*@brief client->server ͷ��򷵻�
	*/
	class SceneHeadFrameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & headFrameList;
		}

	public:

		// ͷ����б�
		std::vector<HeadFrame> headFrameList;
	};

	/**
	*@brief client->server ʹ��ͷ�������
	*/
	class SceneHeadFrameUseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_USE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & headFrameId;
		}

		// ͷ���id
		UInt32 headFrameId;
	};

	/**
	*@brief server->client ʹ��ͷ��򷵻�
	*/
	class SceneHeadFrameUseRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_USE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		// ����ֵ
		UInt32 retCode;
	};

	/**
	*@brief server->client ͷ���֪ͨ
	*/
	class SceneHeadFrameNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isGet & headFrame;
		}

		// 1�ǻ�ã�0ɾ��
		UInt32 isGet;
		// ͷ���
		HeadFrame headFrame;
	};
};


#endif // !_CL_HEAD_FRAME_PROTOOL_H_
