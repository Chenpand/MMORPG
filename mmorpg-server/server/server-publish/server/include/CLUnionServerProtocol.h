#ifndef _CL_UNION_SERVER_PROTOCOL_H_
#define _CL_UNION_SERVER_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLItemDefine.h>

namespace CLProtocol {

	//通知本服跨服开启了哪些功能
	class UnionNotifyOpenFunction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_NOTIFY_OPEN_FUNCTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & openFun;
		}
		std::vector<UInt32> openFun;
	};


	//请求新建跨服Player
	class UnionServerAddPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_ADD_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funType & sceneNode & worldNode & accId & roleId & name & occu & level & vipLvl & guildId & awaken & server;
		}

		UInt32 funType;
		UInt32 sceneNode = 0;
		UInt32 worldNode = 0;
		UInt32 accId = 0;
		ObjID_t roleId = 0;
		std::string name;
		UInt8 occu = 0;
		UInt16 level = 0;
		UInt8 vipLvl = 0;
		UInt64 guildId = 0;
		UInt8 awaken = 0;
		std::string server;
	};

	//请求删除跨服Player
	class UnionServerDelPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_DEL_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funType & roleId;
		}

		UInt32 funType;
		ObjID_t roleId = 0;
	};

	class UnionSceneIsReady : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_SCENE_IS_READY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
		UInt64 id = 0;
	};
	
	class UnionWorldChgZoneID : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_WORLD_CHG_ZONE_ID)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & newZoneID & oldZoneIDs;
		}
		UInt32 newZoneID = 0;
		std::vector<UInt32> oldZoneIDs;
	};

	/**
	 *@brief union->scene 玩家获得奖励
	 */
	class UnionPlayerGetItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_PLAYER_GET_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & notice & reason & rewards;
		}

		UInt64 roleId;
		UInt8 notice;
		std::string reason;
		ItemRewardVec rewards;
	};
	
	/**
	*@brief world->union 通知活动
	*/
	class UnionNotifyActivityInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_NOTIFY_ACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funType & actName & preTime & startTime & endTime;
		}

	public:
		UInt32 funType;
		std::string actName;
		UInt32 preTime;
		UInt32 startTime;
		UInt32 endTime;
	};
}

#endif