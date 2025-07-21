#ifndef _CL_NEW_TITLE_PROTOCOL_H_
#define _CL_NEW_TITLE_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLNewTitleDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client 同步头衔,上线
	*/
	class WorldGetPlayerTitleSyncList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_SYNC_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & titles;
		}

		std::vector<PlayerTitle>	titles;
	};

	/**
	*@brief client->world 头衔穿戴请求
	*/
	class WorldNewTitleTakeUpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_TAKEUP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & titleGuid & titleId;
		}

		UInt64	titleGuid;
		UInt32	titleId;
	};

	/**
	*@brief world->client 头衔穿戴返回
	*/
	class WorldNewTitleTakeUpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_TAKEUP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & res & titleGuid;
		}

		UInt32	res;
		UInt64	titleGuid;
	};

	/**
	*@brief client->world 头衔脱掉请求
	*/
	class WorldNewTitleTakeOffReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_TAKEOFF_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & titleGuid & titleId;
		}

		UInt64	titleGuid;
		UInt32	titleId;
	};

	/**
	*@brief world->client 头衔脱掉返回
	*/
	class WorldNewTitleTakeOffRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_TAKEOFF_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & res & titleGuid;
		}

		UInt32	res;
		UInt64	titleGuid;
	};

	/**
	*@brief world->client 同步新增或删除头衔
	*/
	class WorldNewTitleSyncUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_SYNC_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & addTitles & delGuids;
		}

		std::vector<PlayerTitle> addTitles;
		std::vector<UInt64> delGuids;
	};

	/**
	*@brief world->client 更新头衔数据
	*/
	class WorldNewTitleUpdateData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_UPDATE_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & updates;
		}

		std::vector<PlayerTitle> updates;
	};

	/**
	*@brief World->Scene 设置头衔
	*/
	class WorldNewTitleModifyScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_MODIFY_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & ownerType & opType & titleGuid & titleName & titleId & style;
		}

		ObjID_t			roleId;		 //角色Id
		UInt8			ownerType;	 //拥有类型
		UInt8			opType;		 //操作类型 [NewTitleOpType]
		UInt64			titleGuid;	 //头衔guid
		std::string	    titleName;   //头衔名字
		UInt32			titleId;	 //头衔表id
		UInt8			style;		 //风格
	};

	/**
	*@brief Scene->World 设置头衔
	*/
	class SceneNewTitleModifyWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_TITLE_MODIFY_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & ownerType & opType & titleGuid & titleName;
		}

		ObjID_t			roleId;		 //角色Id
		UInt8			ownerType;	 //拥有类型
		UInt8			opType;		 //操作类型 [NewTitleOpType]
		UInt64			titleGuid;	 //头衔guid
		std::string	    titleName;   //头衔名字
	};

	/**
	*@brief Scene->World 添加头衔
	*/
	class SceneNewTitleAddWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_TITLE_ADD_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & titleId & dueTime;
		}
		ObjID_t			roleId;	   //角色Id
		UInt32			titleId;   //头衔id
		UInt32			dueTime;   //到期时间戳
	};

	/**
	*@brief client->world 穿上公会职位的头衔请求
	*/
	class WorldNewTitleTakeUpGuildPostReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_TITLE_TAKEUP_GUILD_POST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world 穿上公会职位的头衔返回
	*/
	class WorldNewTitleTakeUpGuildPostRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_TITLE_TAKEUP_GUILD_POST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & res;
		}

		UInt32 res;
	};
		
}

#endif

