#ifndef _CL_CHANGEZONE_PROTOCOL_H_
#define _CL_CHANGEZONE_PROTOCOL_H_

#include <AvalonSocket.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief zone->gate 向另一个区的网关同步基本信息                                                                     
	 */
	class GateChangeZoneCheckReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CHANGEZONE_CHECK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcZoneId & accId & playerId & name & mapId & srcGateId & bReturn;
			stream & gmAuthority  & openid & qqvipinfo & source1 & source2;
			stream & pf & level;
		}

	public:
		//原区
		UInt32		srcZoneId;
		//账号id
		UInt32		accId;
		//玩家id
		ObjID_t		playerId;
		//玩家名
		std::string	name;
		//目标地图
		UInt32		mapId;
		//源gateid
		UInt32		srcGateId;
		
		//是否是返回
		UInt8		bReturn;
		//gm权限
		UInt8		gmAuthority;
		//openid
		std::string	openid;
		//qq特权信息
		QQVipInfo	qqvipinfo;
		//来源1
		std::string	source1;
		//来源2
		std::string	source2;
		//pf
		std::string	pf;
		//等级
		UInt16		level;
	};

	/**
	 *@brief gate->zone                                                                     
	 */
	class GateChangeZoneCheckRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CHANGEZONE_CHECK_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & zoneId & mapId & gateAddr & srcGateId & result;
			stream.SeriaArray(key, 20);
		}

		//玩家id
		ObjID_t playerId;
		//目标区id
		UInt32	zoneId;
		//目标地图id
		UInt32	mapId;
		//目标区gate地址
		Avalon::SockAddr gateAddr;
		//原gateid
		UInt32	srcGateId;
		//结果
		UInt32	result;
		//口令
		UInt8	key[20];
	};

	/**
	 *@brief scene->gate 通知网关开始跨服                                                                     
	 */
	class GateChangeZoneNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CHANGEZONE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstZoneId & dstMapId;
		}

		//玩家id
		ObjID_t	playerId;
		//目标区id
		UInt32	dstZoneId;
		//目标区地图
		UInt32	dstMapId;
	};

	/**
	 *@brief scene->world 通知world跨服                                                                     
	 */
	class WorldChangeZoneNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGEZONE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstZoneId & attr;
			stream.SeriaArray(key, 20);
		}

		ObjID_t		playerId;
		UInt32		dstZoneId;
		Avalon::SockAddr	attr;
		UInt8		key[20];
	};

	/**
	 *@brief world->client 通知客户端跨服                                                                     
	 */
	class WorldChangeZoneNotifyClient : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGEZONE_NOTIFY_CLIENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstZoneId & attr;
			stream.SeriaArray(key, 20);
		}

		ObjID_t	playerId;
		UInt32	dstZoneId;
		Avalon::SockAddr	attr;
		UInt8	key[20];
	};

	/**
	*@brief client->server 请求返回原区
	*/
	class SceneChangeZoneReturn : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGEZONE_RETURN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
}

#endif
