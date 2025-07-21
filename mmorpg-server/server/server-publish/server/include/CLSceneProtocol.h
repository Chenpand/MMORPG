#ifndef _CL_SCENE_PROTOCOL_H_
#define _CL_SCENE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"

namespace CLProtocol
{
	/**
	 *@brief world->scene 通知场景加载地图列表
	 */
	class SceneNotifyLoadmap : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOADMAP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & idSeed & maps;
		}

		//临时id种子
		ObjID_t	idSeed;
		//要加载的地图列表
		std::vector<UInt32> maps;
	};

    /**
    *@brief world->scene 通知场景加载地图列表
    */
    class SceneNotifyLoadDungeonEntryMap : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOAD_DUNGEON_ENTRY_MAP)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & idSeed & maps;
        }

        //临时id种子
        ObjID_t	idSeed;
        //要加载的地图列表
        std::vector<UInt32> maps;
    };

	/**
	 *@brief scene->world 同步id生成种子到world
	 */
	class SceneSyncIdSeed : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_IDSEED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & idSeed;
		}

		//临时id种子
		ObjID_t	idSeed;
	};

	/**
	*@brief scene->world 通知召唤npc
	*/
	class SceneSummonNpc : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUMMON_NPC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};
	
	
	/**
	 *@brief world->scene 通知场景加载动态地图
	 */
	class SceneNotifyLoadDynMap : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOAD_DYNMAP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & type & creater & owner & level & lasttime;
		}

		//地图id
		UInt32	mapId;
		//地图类型
		UInt8	type;
		//动态场景创建者
		ObjID_t	creater;
		//动态场景归属者 玩家、部落或队伍等
		ObjID_t	owner;
		//等级
		UInt16	level;
		//持续时间
		UInt32	lasttime;
	};

	/**
	 *@brief world->scene 通知场景加载战场
	 */
	class SceneNotifyLoadBattleScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOAD_BATTLESCENE)
	public:
		SceneNotifyLoadBattleScene():mapid(0),battleSceneType(0),beginTime(0),lasttime(0),leaveTimeout(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapid & battleSceneType & beginTime & lasttime & leaveTimeout;
			stream.SeriaArray(uParams, BATTLE_PARAM_NUM);
		}

		//地图id
		UInt32	mapid;
		//战场类型
		UInt8	battleSceneType;
		//开始时间
		UInt32	beginTime;
		//持续时间
		UInt32	lasttime;
		//离开超时
		UInt32	leaveTimeout;
		// 创建参数
		UInt64	uParams[BATTLE_PARAM_NUM];
	};

	/**
	 *@brief scene->world 场景地图加载配置后注册到world
	 */
	class WorldRegisterSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_SCENE_REQ)
	public:
		WorldRegisterSceneReq()
		{
			sceneid = 0;
			mapid = 0;
			type = 0;
			level = 0;
			creater = 0;
			owner = 0;
			leaveTimeout = 0;
			battleSceneType = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & mapid & name & type & level & creater & owner & pos & leaveTimeout & battleSceneType;
			for(int i = 0; i < BATTLE_PARAM_NUM; i++)
				stream & params[i];
		}

		//场景id
		UInt32 sceneid;
		//地图id
		UInt32 mapid;
		//场景名
		std::string name;
		//类型
		UInt8	type;

		//副本等级
		UInt16	level;
		//创建者
		ObjID_t	creater;
		//归属者
		ObjID_t	owner;
		//复活点
		CLPosition	pos;

		//离开战场超时时间
		UInt32	leaveTimeout;
		//战场类型
		UInt8	battleSceneType;
		// 战场参数
		UInt64	params[BATTLE_PARAM_NUM];
	};

	/**
	 *@brief world->scene world注册场景后返回
	 */
	class WorldRegisterSceneRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_SCENE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & result;
		}

		//场景id
		UInt32 sceneid;
		//结果
		UInt32 result;
	};

	/**
	 *@brief scene->world 注销地图到world
	 */
	class WorldUnregisterSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UNREGISTER_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid;
		}

		UInt32 sceneid;
	};

	/**
	 *@brief world->scene 注销地图返回
	 */
	class WorldUnregisterSceneRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UNREGISTER_SCENE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid;
		}

		UInt32 sceneid;
	};

	/**
	 *@brief scene->gate 场景服务器将场景同步到gate
	 */
	class GateRegisterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_REGISTER_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & name & row & col & syncRange & sceneDataId & sceneType;
		}

		//场景id
		UInt32 sceneid;
		//场景名
		std::string name;
		//行
		UInt32 row;
		//列
		UInt32 col;
		//同步范围
		UInt32 syncRange;
		//场景表id
		UInt32 sceneDataId;
		//场景类型
		UInt8 sceneType;
	};

	/**
	 *@brief scene->gate 注销场景
	 */
	class GateUnregisterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_UNREGISTER_SCENE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid;
		}

		UInt32 sceneid;
	};

	/**
	 *@brief scene->world 进入副本，如果副本不存在并且有创建副本权限则创建
	 */
	class WorldEnterDynSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ENTER_DYNSCENE_REQ)
	public:
		WorldEnterDynSceneReq():id(0), mapId(0), type(0), enterType(0), level(0),
			bTimesFull(0), bVipTimesFull(0), itemId(0), itemNum(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & mapId & type & enterType & level & bTimesFull & bVipTimesFull & itemId & itemNum;
		}

		//玩家
		ObjID_t	id;
		//地图id
		UInt32	mapId;
		//地图类型
		UInt8	type;
		//副本进入类型
		UInt8	enterType;
		//副本等级
		UInt16	level;
		//副本次数是否已满
		UInt8	bTimesFull;
		//vip副本次数是否已满
		UInt8	bVipTimesFull;
		//需要道具
		UInt32	itemId;
		//需要道具数量
		UInt32	itemNum;
	};

	/**
	 *@brief scene->world 注册npc位置
	 */
	class WorldRegisterNpcPos : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_NPC_POS)
	public:
		WorldRegisterNpcPos():mapId(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & npcs;
		}

		//地图id
		UInt32	mapId;
		//npc位置列表
		std::map<UInt32, CLPosition>	npcs;
	};

	/**
	 *@brief scene->client 同步场景名
	 */
	class SceneSyncName : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_NAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

		//场景名
		std::string name;
	};

	/**
	 *@brief world->scene 设置场景参数
	 */
	class SceneSetParam : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PARAM)
	
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneId & name & value;
		}

		//场景id
		UInt32	sceneId;
		//参数名
		std::string	name;
		//值
		UInt64	value;
	};

	
	/** 
	 *@brief client<->server测试网络延迟
	 */
	class SceneTestNetDelay : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TEST_NETDELAY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & timeStamp;
		}

		UInt32	timeStamp;
	};

	/**
	 *@brief scene->world 同步场景人数到world
	 */
	class SceneSyncPlayerNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PLAYERNUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneId & playerNum;
		}

		UInt32	sceneId;
		UInt32	playerNum;
	};

	/**
	*@brief scene->world 通知world玩家进入场景
	*/
	class SceneNotifyWorldEnterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_WORLD_ENTERSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & sceneId;
		}

		ObjID_t	roleId;
		UInt32	sceneId;
	};

	/**
	*@brief scene->world 通知world玩家加载开始
	*/
	class SceneNotifyWorldLoadStart : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_WORLD_LOAD_START)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t	roleId;
	};
	
	/**
	*@brief scene->world 通知world玩家加载完成
	*/
	class SceneNotifyWorldLoadFinish : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_WORLD_LOAD_FINISH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t	roleId;
	};
	
	/**
	*@brief scene->world 通知world玩家离开SceneServer
	*/
	class SceneNotifyWorldLeave : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_WORLD_LEAVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t	roleId;
	};

	/**
	*@brief client->scene 通知客户端loading情况
	*/
	class SceneNotifyLoadingInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOADING_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isLoading;
		}

		UInt8	isLoading;
	};

	/**
	*@brief scene->client 请求客户端loading情况
	*/
	class SceneQueryLoadingInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_LOADING_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	

	/**
	*@brief scene->world 将推送通知发到world
	*/
	class ScenePushNotifyToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PUSH_NOTIFY)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & accid & roleid & title & content;
		}

		std::string platform;
		UInt32 accid;
		UInt64 roleid;
		std::string title;
		std::string content;
	};
}

#endif
