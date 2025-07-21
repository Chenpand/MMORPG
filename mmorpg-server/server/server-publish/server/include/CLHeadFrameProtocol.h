#ifndef _CL_HEAD_FRAME_PROTOOL_H_
#define _CL_HEAD_FRAME_PROTOOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->server 头像框请求
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

		// 头像框id
		UInt32 headFrameId;
		// 过期时间
		UInt32 expireTime;
	};

	/**
	*@brief client->server 头像框返回
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

		// 头像框列表
		std::vector<HeadFrame> headFrameList;
	};

	/**
	*@brief client->server 使用头像框请求
	*/
	class SceneHeadFrameUseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_USE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & headFrameId;
		}

		// 头像框id
		UInt32 headFrameId;
	};

	/**
	*@brief server->client 使用头像框返回
	*/
	class SceneHeadFrameUseRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_USE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		// 返回值
		UInt32 retCode;
	};

	/**
	*@brief server->client 头像框通知
	*/
	class SceneHeadFrameNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HEAD_FRAME_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isGet & headFrame;
		}

		// 1是获得，0删除
		UInt32 isGet;
		// 头像框
		HeadFrame headFrame;
	};
};


#endif // !_CL_HEAD_FRAME_PROTOOL_H_
