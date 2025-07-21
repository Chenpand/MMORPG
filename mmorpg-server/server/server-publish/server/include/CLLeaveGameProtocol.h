#ifndef _CL_LEAVEGAME_PROTOCOL_H_
#define _CL_LEAVEGAME_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLPlayerDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->gate 请求离开游戏
	 */
	class GateLeaveGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_LEAVEGAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene、world、admin->gate 场景或会话请求踢掉玩家
	 */
	class GateKickOffPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_KICKOFF_REQ)
	public:
		GateKickOffPlayer():accid(0), id(0), errorCode(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & id & errorCode & msg;
		}

		//账号id
		UInt32	accid;
		//玩家id
		ObjID_t id;
		//错误码
		UInt32	errorCode;
		//信息
		std::string	msg;
	};

	/**
	 *@brief gate->client 通知客户端被踢
	 */
	class GateNotifyKickoff : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_KICKOFF)
	public:
		GateNotifyKickoff():errorCode(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & msg;
		}

	public:
		//错误码
		UInt32	errorCode;
		//信息
		std::string	msg;
	};

	/**
	*@brief gate->node 
	*/
	class GateNotifyLeaveConnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_LEAVE_TIME)
	public:
		GateNotifyLeaveConnect() :roleId(0), leaveTime(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & leaveTime;
		}

	public:
		// 角色id
		UInt64 roleId;
		// 离开时间
		UInt64 leaveTime;
	};
	
	/**
	 *@brief gate->world 通知玩家下线
	 */
	class WorldLeaveGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_LEAVEGAME_REQ)
	public:
		WorldLeaveGameReq()
		{
			isOfflineTimeOut = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & id & isOfflineTimeOut;
		}

		//账号ID
		UInt32 accid;
		//玩家id
		ObjID_t	id;
		//是否离线超时
		UInt8 isOfflineTimeOut;
	};

	/**
	*@brief gate->world 通知World玩家断开连接（这里只算闪断）
	*/
	class GateNotifyDisconnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DISCONNECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//玩家id
		ObjID_t	id;
	};

	/**
	*@brief gate->world 通知World玩家重连成功
	*/
	class GateNotifyReconnectSuccess : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_RECONN_SUCCESS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//玩家id
		ObjID_t	id;
	};
	

	/**
	 *@brief world->scene 通知玩家下线了
	 */
	class SceneLeaveGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LEAVEGAME_REQ)
	public:
		SceneLeaveGameReq() : id(0), forceLeave(0), isOfflineTimeOut(0){ }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & forceLeave & isOfflineTimeOut;
		}

		//玩家
		ObjID_t id;
		//强制删除玩家
		UInt8	forceLeave;
		//是否离线超时
		UInt8	isOfflineTimeOut;
	};

	/**
	*@brief world->scene 通知玩家逻辑下线(断开链接)
	*/
	class SceneLogicLeaveGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOGIC_LEAVEGAME_REQ)
	public:
		SceneLogicLeaveGameReq() : id(0) { }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//玩家
		ObjID_t id;
	};
	

	/**
	 *@brief scene->world 通知world玩家注销
	 */
	class WorldUnregisterPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UNREGISTER_PLAYER_REQ)
	public:
		WorldUnregisterPlayerReq():id(0), sceneId(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & sceneId;
			if(sceneId != 0)
			{
				stream & pos;
			}
		}

		//玩家id
		ObjID_t id;
		//场景id
		UInt32	sceneId;
		//位置
		CLPosition pos;
	};

	/**
	*@brief scene->world 玩家下线时候同步sharedata
	*/
	class SceneSyncShareDataToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SHAREDATA_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_Guid & sharedata;
		}
		ObjID_t m_Guid;    //角色id
		PlayerSenceShareData sharedata;
	};
}

#endif
