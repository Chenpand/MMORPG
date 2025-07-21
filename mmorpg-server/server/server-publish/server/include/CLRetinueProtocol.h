#ifndef _CL_RETINUE_PROTOCOL_H_
#define _CL_RETINUE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLRetinueDefine.h"

namespace CLProtocol
{
	enum RetinueUpType
	{
		RUT_UPLEVEL = 1,	//随从升级
		RUT_UPSTAR = 2		//随从升星
	};


	/**
	*@brief server->client 下发随从列表
	*/
	class SceneSyncRetinueList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_RETINUE_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & offRetinueIds & retinueList;
		}

	public:
		ObjID_t id;
		std::vector<UInt64> offRetinueIds;
		std::vector<RetinueInfo> retinueList;
	};

	/**
	*@brief server->client 下发随从
	*/
	class SceneSyncRetinue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_RETINUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		RetinueInfo info;
	};

	/**
	*@brief client->server 请求 变更跟随的随从
	*/
	class SceneChanageRetinueReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_RETINUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & index;
		}

	public:
		//随从ID
		ObjID_t id;
		//位置
		UInt8 index;
	};

	/**
	*@brief server->client 响应 变更跟随的随从
	*/
	class SceneChanageRetinueRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_RETINUE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & mainId & offRetinueIds;
		}

	public:
		//返回值
		UInt32 result;
		//主随从ID
		ObjID_t mainId;
		//副随从ID列表
		std::vector<UInt64> offRetinueIds;
	};

	/**
	*@brief client->server 请求 洗练支援技能
	*/
	class SceneRetinueChangeSkillReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RETINUE_CHANGE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & skillIds;
		}

	public:

		//随从ID
		ObjID_t id;

		//洗练库ID列表
		std::vector<UInt32> skillIds;
	};

	/**
	*@brief server->client 响应 洗练支援技能
	*/
	class SceneRetinueChangeSkillRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RETINUE_CHANGE_SKILL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求 随从解锁
	*/
	class SceneRetinueUnlockReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RETINUE_UNLOCK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		UInt32 id;
	};

	/**
	*@brief server->client 响应 随从解锁
	*/
	class SceneRetinueUnlockRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RETINUE_UNLOCK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dataId;
		}

	public:
		UInt32 result;
		UInt32 dataId;
	};


	/**
	*@brief client->server 请求 随从升级
	*/
	class SceneRetinueUpLevelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RETINUE_LEVEL_UP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

	public:
		UInt8 type; //RetinueUpType的枚举
		UInt64 id;
	};

	/**
	*@brief server->client 响应 随从升级
	*/
	class SceneRetinueUpLevelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RETINUE_LEVEL_UP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & type & retinueId;
		}

	public:
		UInt32 result;
		UInt8 type;
		UInt64 retinueId;
	};
}



#endif  //_CL_RETINUE_PROTOCOL_H_