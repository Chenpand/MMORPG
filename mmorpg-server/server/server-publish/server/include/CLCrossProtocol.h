#ifndef _CL_CROSS_PROTOCOL_H_
#define _CL_CROSS_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{

	/**
	*@brief scene->wscene 请求进入跨服场景
	*/
	class CrossEnterSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_CROSS_ENTER_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief wscene->scene 进入跨服场景返回
	*/
	class CrossEnterSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_CROSS_ENTER_SCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & crossSceneNodeId;
		}

		ObjID_t roleId;
		UInt32 result;
		UInt32 crossSceneNodeId;
	};

	/**
	*@brief wscene->scene 退出跨服场景
	*/
	class CrossExitSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_CROSS_EXIT_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t roleId;
	};

	/**
	*@brief scene->wscene 退出跨服场景
	*/
	class CrossExitSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_CROSS_EXIT_SCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result;
		}

		ObjID_t roleId;
		UInt32 result;
	};



}

#endif
