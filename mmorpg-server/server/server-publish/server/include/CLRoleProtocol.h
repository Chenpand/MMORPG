#ifndef _CL_ROLE_PROTOCOL_H_
#define _CL_ROLE_PROTOCOL_H_

#include "CLProtocolDefine.h"


namespace CLProtocol
{
	/**
	*@brief client->world 角色扩展栏位解锁请求
	*/
	class WorldExtensibleRoleFieldUnlockReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_EXTENSIBLE_ROLE_FIELD_UNLOCK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & costItemUId & costItemDataId;
		}

	public:
		// 需要消耗的道具
		ObjID_t costItemUId;
		UInt32 costItemDataId;
	};

	/**
	*@brief world->client 角色扩展栏位解锁返回
	*/
	class WorldExtensibleRoleFieldUnlockRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_EXTENSIBLE_ROLE_FIELD_UNLOCK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode;
		}

	public:
		// 错误码
		UInt32 resCode;
	};
};


#endif