#ifndef _CL_ROLE_PROTOCOL_H_
#define _CL_ROLE_PROTOCOL_H_

#include "CLProtocolDefine.h"


namespace CLProtocol
{
	/**
	*@brief client->world ��ɫ��չ��λ��������
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
		// ��Ҫ���ĵĵ���
		ObjID_t costItemUId;
		UInt32 costItemDataId;
	};

	/**
	*@brief world->client ��ɫ��չ��λ��������
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
		// ������
		UInt32 resCode;
	};
};


#endif