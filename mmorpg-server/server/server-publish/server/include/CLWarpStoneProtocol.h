#ifndef _CL_WARP_STONE_PROTOCOL_H_
#define _CL_WARP_STONE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLWarpStoneDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->server �������
	*/
	class SceneWarpStoneUnlockReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WARP_STONE_UNLOCK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		UInt32 id;
	};

	/**
	*@brief server->client ��Ӧ����
	*/
	class SceneWarpStoneUnlockRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WARP_STONE_UNLOCK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};


	/**
	*@brief client->server �������
	*/
	class SceneWarpStoneChargeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WARP_STONE_CHARGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & info;
		}

	public:
		UInt32 id;
		std::vector<ChargeInfo> info;
	};

	/**
	*@brief server->client ��Ӧ����
	*/
	class SceneWarpStoneChargeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WARP_STONE_CHARGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

}



#endif //_CL_WARP_STONE_PROTOCOL_H_
