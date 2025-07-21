#ifndef _CL_SCENE_PROTOCOL_H_
#define _CL_SCENE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"

namespace CLProtocol
{
	/**
	 *@brief world->scene ֪ͨ�������ص�ͼ�б�
	 */
	class SceneNotifyLoadmap : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOADMAP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & idSeed & maps;
		}

		//��ʱid����
		ObjID_t	idSeed;
		//Ҫ���صĵ�ͼ�б�
		std::vector<UInt32> maps;
	};

    /**
    *@brief world->scene ֪ͨ�������ص�ͼ�б�
    */
    class SceneNotifyLoadDungeonEntryMap : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOAD_DUNGEON_ENTRY_MAP)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & idSeed & maps;
        }

        //��ʱid����
        ObjID_t	idSeed;
        //Ҫ���صĵ�ͼ�б�
        std::vector<UInt32> maps;
    };

	/**
	 *@brief scene->world ͬ��id�������ӵ�world
	 */
	class SceneSyncIdSeed : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_IDSEED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & idSeed;
		}

		//��ʱid����
		ObjID_t	idSeed;
	};

	/**
	*@brief scene->world ֪ͨ�ٻ�npc
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
	 *@brief world->scene ֪ͨ�������ض�̬��ͼ
	 */
	class SceneNotifyLoadDynMap : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_LOAD_DYNMAP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & type & creater & owner & level & lasttime;
		}

		//��ͼid
		UInt32	mapId;
		//��ͼ����
		UInt8	type;
		//��̬����������
		ObjID_t	creater;
		//��̬���������� ��ҡ����������
		ObjID_t	owner;
		//�ȼ�
		UInt16	level;
		//����ʱ��
		UInt32	lasttime;
	};

	/**
	 *@brief world->scene ֪ͨ��������ս��
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

		//��ͼid
		UInt32	mapid;
		//ս������
		UInt8	battleSceneType;
		//��ʼʱ��
		UInt32	beginTime;
		//����ʱ��
		UInt32	lasttime;
		//�뿪��ʱ
		UInt32	leaveTimeout;
		// ��������
		UInt64	uParams[BATTLE_PARAM_NUM];
	};

	/**
	 *@brief scene->world ������ͼ�������ú�ע�ᵽworld
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

		//����id
		UInt32 sceneid;
		//��ͼid
		UInt32 mapid;
		//������
		std::string name;
		//����
		UInt8	type;

		//�����ȼ�
		UInt16	level;
		//������
		ObjID_t	creater;
		//������
		ObjID_t	owner;
		//�����
		CLPosition	pos;

		//�뿪ս����ʱʱ��
		UInt32	leaveTimeout;
		//ս������
		UInt8	battleSceneType;
		// ս������
		UInt64	params[BATTLE_PARAM_NUM];
	};

	/**
	 *@brief world->scene worldע�᳡���󷵻�
	 */
	class WorldRegisterSceneRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_SCENE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & result;
		}

		//����id
		UInt32 sceneid;
		//���
		UInt32 result;
	};

	/**
	 *@brief scene->world ע����ͼ��world
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
	 *@brief world->scene ע����ͼ����
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
	 *@brief scene->gate ����������������ͬ����gate
	 */
	class GateRegisterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_REGISTER_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & name & row & col & syncRange & sceneDataId & sceneType;
		}

		//����id
		UInt32 sceneid;
		//������
		std::string name;
		//��
		UInt32 row;
		//��
		UInt32 col;
		//ͬ����Χ
		UInt32 syncRange;
		//������id
		UInt32 sceneDataId;
		//��������
		UInt8 sceneType;
	};

	/**
	 *@brief scene->gate ע������
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
	 *@brief scene->world ���븱����������������ڲ����д�������Ȩ���򴴽�
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

		//���
		ObjID_t	id;
		//��ͼid
		UInt32	mapId;
		//��ͼ����
		UInt8	type;
		//������������
		UInt8	enterType;
		//�����ȼ�
		UInt16	level;
		//���������Ƿ�����
		UInt8	bTimesFull;
		//vip���������Ƿ�����
		UInt8	bVipTimesFull;
		//��Ҫ����
		UInt32	itemId;
		//��Ҫ��������
		UInt32	itemNum;
	};

	/**
	 *@brief scene->world ע��npcλ��
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

		//��ͼid
		UInt32	mapId;
		//npcλ���б�
		std::map<UInt32, CLPosition>	npcs;
	};

	/**
	 *@brief scene->client ͬ��������
	 */
	class SceneSyncName : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_NAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

		//������
		std::string name;
	};

	/**
	 *@brief world->scene ���ó�������
	 */
	class SceneSetParam : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PARAM)
	
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneId & name & value;
		}

		//����id
		UInt32	sceneId;
		//������
		std::string	name;
		//ֵ
		UInt64	value;
	};

	
	/** 
	 *@brief client<->server���������ӳ�
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
	 *@brief scene->world ͬ������������world
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
	*@brief scene->world ֪ͨworld��ҽ��볡��
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
	*@brief scene->world ֪ͨworld��Ҽ��ؿ�ʼ
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
	*@brief scene->world ֪ͨworld��Ҽ������
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
	*@brief scene->world ֪ͨworld����뿪SceneServer
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
	*@brief client->scene ֪ͨ�ͻ���loading���
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
	*@brief scene->client ����ͻ���loading���
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
	*@brief scene->world ������֪ͨ����world
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
