#ifndef _CL_MOVE_PROTOCOL_H_
#define _CL_MOVE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLObjectDefine.h"

namespace CLProtocol
{
	/**
	 *@brief scene->gate通知网关进入场景
	 */
	class GateNotifyEnterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_ENTERSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & sceneid & gridindex;
		}

		//玩家id
		ObjID_t id;
		//场景id
		UInt32 sceneid;
		//所在格子索引
		UInt32 gridindex;
	};

	/**
	 *@brief scene->gate通知网关离开场景
	 */
	class GateNotifyLeaveScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_LEAVESCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneId & id;
		}

		//场景id
		UInt32	sceneId;
		//玩家id
		ObjID_t id;
	};

	/**
	 *@brief scene->gate 场景将玩家所在格子同步给网关
	 */
	class GateSyncGridIndex : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_GRIDINDEX)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & gridIndex;
		}

		//玩家id
		ObjID_t id;
		//格子索引
		UInt32 gridIndex;
	};

	/**
	 *@brief scene->client通知客户端进入场景
	 */
	class SceneNotifyEnterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_ENTERSCENE)
	public:
		SceneNotifyEnterScene()
		{
			result = 0;
			mapid = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & mapid & name & pos & dir;
		}

		//结果
		UInt32		result;
		//地图id
		UInt32		mapid;
		//场景名
		std::string	name;
		//位置
        ScenePos	pos;
		//方向
        SceneDir	dir;
	};

	/**
	 *@brief client->scene 移动
	 */
	class SceneMoveRequire : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MOVE_REQUIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & clientSceneId & pos & dir;
		}

		// 客户端所在的场景ID
		UInt32	 clientSceneId;
        ScenePos pos;
        SceneDir dir;
	};

	/**
	 *@brief scene->client  同步玩家移动
	 */
	class SceneSyncPlayerMove : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PLAYER_MOVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & pos & dir;
		}

		//obj
		ObjID_t id;
		//位置
        ScenePos pos;
        // 朝向
        SceneDir dir;
	};

    /**
     *@brief clint->scene 玩家请求切换场景
     */
    class ScenePlayerChangeSceneReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_PLAYER_CHANGESCENE_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & curDoorId & dstSceneId & dstDoorId;
        }

        UInt32      curDoorId;
        UInt32      dstSceneId;
        UInt32      dstDoorId;
    };

	/**
	 *@brief scene->world->scene 请求开始切换场景
	 */
	class SceneBeginChangeSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEGIN_CHANGESCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & objtype & level & pos & dir & bTransZone & dstDoorId;
		}
		
		ChangeSceneInfo info;
		//物体类型
		UInt8	objtype;
		//等级
		UInt16	level;
		//目标位置
		CLPosition	pos;
		//目标方向
		UInt8	dir;
		//是否传送点切场景
		UInt8	bTransZone;
        //目标传送门位置
        UInt32  dstDoorId;
	};

	/**
	 *@brief scene->world->scene 请求开始切换场景返回
	 */
	class SceneBeginChangeSceneRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEGIN_CHANGESCENE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & pos & bTransZone;
		}

	public:
		
		ChangeSceneInfo info;
		CLPosition  pos;
		UInt8 bTransZone;
	};

	/**
	 *@brief scene->scene 传输object数据
	 */
	class SceneTransObjectData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRANS_OBJECTDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & destid;
		}

		ChangeSceneInfo info;
		//目标guid
		ObjID_t	destid;
		//数据
	};

	/**
	 *@brief 切换场景
	 */
	class SceneChangeScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGESCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		ChangeSceneInfo info;
	};

	/**
	 *@brief server->client 瞬移
	 */
	class SceneTeleport : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TELEPORT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & pos & dir;
		}

		//obj
		ObjID_t id;
		//位置
		CLPosition pos;
		//方向
		UInt8   dir;
	};

	/**
	 *@brief world->scene 动态场景强制拉人
	 */
	class SceneEnterScenePullPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENTERSCENE_PULL_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & sceneId & mapId & pos & bFirstEnter & level;
		}

		//目标玩家
		ObjID_t	playerId;
		//目标场景
		UInt32	sceneId;
		//地图id
		UInt32	mapId;
		//进入点
		CLPosition	pos;
		//是否第一次进副本
		UInt8	bFirstEnter;
		//副本等级
		UInt16	level;
	};

	/**
	 *@brief client->server 离开动态场景
	 */
	class SceneLeaveDynScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LEAVE_DYNSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief server->client 服务器同步npc和pet移动
	 */
	class SceneSyncMove : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MOVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & pos;
			stream.SeriaArray(path, 2);
		}

	public:
		//npc或宠物id
		ObjID_t		id;
		//路径起始位置
		CLPosition	pos;
		//方向路径
		UInt8		path[2];
	};

	/**
	 *@brief server->client 服务器通知客户端停止移动
	 */
	class SceneStopMove : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_STOP_MOVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & pos;
		}

	public:
		//npc或宠物id
		ObjID_t	id;
		//停下时的位置
		CLPosition	pos;
	};

	/**
	*@brief world->scene 查询玩家的位置
	*/
	class ScenePlayerPosReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PLAYER_POS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uPlayerId & uType & uParam;
		}

	public:
		// 玩家ID
		ObjID_t		uPlayerId;
		// 类型,1 部落召集
		UInt8		uType;
		// 参数
		ObjID_t		uParam;
	};

	/**
	*@brief scene->world 查询玩家的位置返回
	*/
	class ScenePlayerPosRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PLAYER_POS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uPlayerId & uMapId & pos & uType & uParam;
		}

	public:
		// 玩家ID
		ObjID_t		uPlayerId;
		// 地图ID
		UInt32		uMapId;
		// 位置
		CLPosition	pos;
		// 类型,1 部落召集
		UInt8		uType;
		// 参数
		ObjID_t		uParam;
	};

    /**
    *@brief clent->scene 请求回到城镇
    */
    class SceneReturnToTown : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_RETURN_TO_TOWN_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & tag;
        }
    public:
		//0:返回城镇(地牢时候代表死亡返回城镇)， 1:返回战场, 2:主动退出
		UInt8  tag;
    };
}

#endif
