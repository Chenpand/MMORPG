#ifndef _CL_QUICKBUY_PROTOCOL_H_
#define _CL_QUICKBUY_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	*@brief client -> scene 快速购买请求
	*/
	class SceneQuickBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & param1 & param2;
		}

		// 类型(对应枚举QuickBuyTargetType)
		UInt8		type;

		// 参数
		ObjID_t		param1;
		ObjID_t		param2;
	};

	/**
	*@brief scene -> client 快速购买返回
	*/
	class SceneQuickBuyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32		result;
	};

	/**
	*@brief scene -> world 快速购买事务请求
	*/
	class SceneQuickBuyTransactionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_TRANSACTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & transactionId & type & param1 & param2;
		}

		// 角色ID
		ObjID_t		roleId;
		// 事务ID
		UInt32		transactionId;
		// 类型(对应枚举QuickBuyTargetType)
		UInt8		type;
		// 参数
		ObjID_t		param1;
		ObjID_t		param2;
	};

	/**
	*@brief world -> scene 快速购买事务返回
	*/
	class SceneQuickBuyTransactionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_TRANSACTION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & transactionId & result;
		}

		// 角色ID
		ObjID_t		roleId;
		// 事务ID
		UInt32		transactionId;
		// 返回
		UInt32		result;
	};
};

#endif