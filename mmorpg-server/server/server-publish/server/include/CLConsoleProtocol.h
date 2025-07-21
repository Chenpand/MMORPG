#ifndef _CL_CONSOLE_PROTOCOL_H_
#define _CL_CONSOLE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include <CLItemDefine.h>

namespace CLProtocol
{
	/**
	 *@brief admin->server 向其他服务器转发客服后台请求
	 */
	class AdminConsoleRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_CONSOLE_REQUSET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & cmd & subcmd & timestamp & body;
		}

	public:
		//连接id
		UInt32		nodeid;
		//指令
		std::string	cmd;
		//子命令
		std::string	subcmd;
		//时间戳
		UInt32		timestamp;
		//json
		std::string	body;
	};

	/**
	 *@brief server->admin 其他服务器要求admin转发回复给客服后台
	 */
	class AdminConsoleResponse : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_CONSOLE_RESPONSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & cmd & subcmd & timestamp & body;
		}

	public:
		//连接id
		UInt32		nodeid;
		//指令
		std::string	cmd;
		//子命令
		std::string	subcmd;
		//时间戳
		UInt32		timestamp;
		//json
		std::string	body;
	};

	/**
	 *@brief world->scene 客服后台请求查询玩家信息
	 */
	class SceneConsoleQueryPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_QUERY_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & subcmd & keyType & accId & playerId & name & param & param2;
		}

	public:
		//连接id
		UInt32		nodeid;
		//子命令
		std::string subcmd;
		//key类型	1表示账号为key 否则为name为key
		UInt8		keyType;
		//账号id
		UInt32		accId;
		//玩家id
		ObjID_t		playerId;
		//玩家名
		std::string	name;
		//参数
		UInt32		param;
		//参数2
		UInt32		param2;
	};

	/**
	 *@brief world->scene 重新加载脚本
	 */
	class SceneConsoleReloadScripts : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_RELOAD_SCRIPTS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & scripts;
		}

		std::vector<UInt32>		scripts;
	};


	/**
	*@brief world->scene 客服后台请求操作玩家
	*/
	class SceneConsoleControlPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_PLAYER_CONTROL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & subcmd & keyType & accId & playerId & name & param1 & param2 & items;
		}

	public:
		//连接id
		UInt32		nodeid;
		//子命令
		std::string subcmd;
		//key类型	1表示账号为key 2.palyerUid key 3.name为key
		UInt8		keyType;
		//账号
		UInt32		accId;
		//玩家id
		ObjID_t		playerId;
		//玩家名
		std::string	name;
		//操作参数1
		UInt64		param1;
		//操作参数2
		UInt32		param2;
		//通用操作批量道具
		std::vector<ItemReward>	items;
	};
	
	/**
	*@brief world->scene 手机绑定
	*/
	class SceneConsolePhoneBind : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_PHONE_BIND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & roleid;
		}

	public:
		//连接id
		UInt32		nodeid;
		//角色id
		ObjID_t		roleid;
	};

	/**
	*@brief world->scene 同步玩家gm权限
	*/
	class WorldSyncPlayerGMAuthority : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_GM_AUTHORITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & gmAuth;
		}

	public:
		//角色id
		ObjID_t	roleid;
		// gm权限
		UInt8 gmAuth;
	};

	/**
	*@brief world->scene 处理资产冻结
	*/
	class SceneConsoleCurrencyFrozen : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_CURRENCY_FROZEN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & roleId & orderId & opType;
		}

	public:
		UInt32 nodeId;
		//角色id
		ObjID_t	roleId;
		// 订单号
		UInt64 orderId;
		// 操作
		UInt32 opType;
	};

	/**
	*@brief world->scene 移除信用点券
	*/
	class SceneConsoleRemoveCreditPoint : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_REMOVE_CREDIT_POINT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & onlineId & creditPoint & reason;
		}

	public:

		UInt32 nodeId;
		UInt64 onlineId;
		UInt32 creditPoint;
		std::string reason;
	};
}

#endif
