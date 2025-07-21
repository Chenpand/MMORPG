#ifndef _CL_GAMESTART_ACTIVITY_PROTOCOL_H_
#define _CL_GAMESTART_ACTIVITY_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief server->client 通知开服时间
	*/
	class WorldNotifyGameStartTime : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GAME_STARTTIME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time & days;
		}

	public:
		// 开服时间
		UInt32	time;
		//今天是开服第几天
		UInt32	days;
	};

	/**
	*@brief world->scene 通知开服时间到场景服
	*/
	class WorldNotifyGameStartTimeToS : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GAME_STARTTIME_TOS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time & firstTime;
		}

	public:
		// 开服时间
		UInt32	time;
		// 第一次开服时间
		UInt32 firstTime;
	};

	/**
	*@brief client->server 开服存金贝
	*/
	class SceneGSActivityStoreGold : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GSACTIVITY_STORE_GOLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		// 储蓄类型，0 存1000，1 存10000
		UInt8	type;
	};

	/**
	*@brief server->client 同步财神信息
	*/
	class SceneGSActivitySyncCaishen : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GSACTIVITY_SYNC_CAISHEN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & canget & days & gold & bindgold;
		}

	public:
		// 是否可领，0 不可，1 可以
		UInt8	canget;
		// 登录天数
		UInt8	days;
		// 金贝
		UInt32	gold;
		// 绑金
		UInt32	bindgold;
	};
}

#endif

