#ifndef _CL_ITEM_THING_PROTOCOL_H_
#define _CL_ITEM_THING_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLItemThing.h>

namespace CLProtocol
{
	/**
	*@brief node->scene 开始事务请求
	*/
	class SceneItemThingStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & serialId & servType & param & reason & itemCost;
		}

		// 角色
		UInt64 roleId;
		// 事务序号
		UInt64 serialId;
		// 请求服务
		UInt32 servType;
		// 参数
		UInt64 param;
		// reason
		std::string reason;
		// 事务
		ItemThingCost itemCost;
	};

	/**
	*@brief scene->node 开始事务返回
	*/
	class SceneItemThingStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & serialId & itemThingGuid;
		}

		SceneItemThingStartRes()
		{
			retCode = 0;
			serialId = 0;
			itemThingGuid = 0;
		}

		// 返回码
		UInt32 retCode;
		// 事务序号
		UInt64 serialId;
		// 事务id
		UInt64 itemThingGuid;
	};

	/**
	*@brief node->scene 开始事务取消
	*/
	class SceneItemThingStartCancel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_START_CANCEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & serialId & servType & param;
		}

		// 角色
		UInt64 roleId;
		// 事务序号
		UInt64 serialId;
		// 请求服务
		UInt32 servType;
		// 参数
		UInt64 param;
	};

	/**
	*@brief node->scene 事务提交请求
	*/
	class SceneItemThingCommitReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_COMMIT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & itemThingGuid & servType & param & isCommit;
		}

		// 角色
		UInt64 roleId;
		// 事务id
		UInt64 itemThingGuid;
		// 请求服务
		UInt32 servType;
		// 参数
		UInt64 param;
		// 是否提交(提交：1， 回滚：0)
		UInt32 isCommit;
	};

	/**
	*@brief scene->node 事务提交返回
	*/
	class SceneItemThingCommitRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_COMMIT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemThingGuid;
		}

		SceneItemThingCommitRes()
		{
			retCode = 0;
			itemThingGuid = 0;
		}

		// 返回码
		UInt32 retCode;
		// 事务id
		UInt64 itemThingGuid;
	};
};

#endif // !_CL_ITEM_THING_PROTOCOL_H_

