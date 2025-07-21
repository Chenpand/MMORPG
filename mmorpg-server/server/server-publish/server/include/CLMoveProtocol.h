#ifndef _CL_MOVE_PROTOCOL_H_
#define _CL_MOVE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLObjectDefine.h"

namespace CLProtocol
{
	/**
	 *@brief scene->gate֪ͨ���ؽ��볡��
	 */
	class GateNotifyEnterScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_ENTERSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & sceneid & gridindex;
		}

		//���id
		ObjID_t id;
		//����id
		UInt32 sceneid;
		//���ڸ�������
		UInt32 gridindex;
	};

	/**
	 *@brief scene->gate֪ͨ�����뿪����
	 */
	class GateNotifyLeaveScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_LEAVESCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneId & id;
		}

		//����id
		UInt32	sceneId;
		//���id
		ObjID_t id;
	};

	/**
	 *@brief scene->gate ������������ڸ���ͬ��������
	 */
	class GateSyncGridIndex : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_GRIDINDEX)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & gridIndex;
		}

		//���id
		ObjID_t id;
		//��������
		UInt32 gridIndex;
	};

	/**
	 *@brief scene->client֪ͨ�ͻ��˽��볡��
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

		//���
		UInt32		result;
		//��ͼid
		UInt32		mapid;
		//������
		std::string	name;
		//λ��
        ScenePos	pos;
		//����
        SceneDir	dir;
	};

	/**
	 *@brief client->scene �ƶ�
	 */
	class SceneMoveRequire : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MOVE_REQUIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & clientSceneId & pos & dir;
		}

		// �ͻ������ڵĳ���ID
		UInt32	 clientSceneId;
        ScenePos pos;
        SceneDir dir;
	};

	/**
	 *@brief scene->client  ͬ������ƶ�
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
		//λ��
        ScenePos pos;
        // ����
        SceneDir dir;
	};

    /**
     *@brief clint->scene ��������л�����
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
	 *@brief scene->world->scene ����ʼ�л�����
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
		//��������
		UInt8	objtype;
		//�ȼ�
		UInt16	level;
		//Ŀ��λ��
		CLPosition	pos;
		//Ŀ�귽��
		UInt8	dir;
		//�Ƿ��͵��г���
		UInt8	bTransZone;
        //Ŀ�괫����λ��
        UInt32  dstDoorId;
	};

	/**
	 *@brief scene->world->scene ����ʼ�л���������
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
	 *@brief scene->scene ����object����
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
		//Ŀ��guid
		ObjID_t	destid;
		//����
	};

	/**
	 *@brief �л�����
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
	 *@brief server->client ˲��
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
		//λ��
		CLPosition pos;
		//����
		UInt8   dir;
	};

	/**
	 *@brief world->scene ��̬����ǿ������
	 */
	class SceneEnterScenePullPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENTERSCENE_PULL_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & sceneId & mapId & pos & bFirstEnter & level;
		}

		//Ŀ�����
		ObjID_t	playerId;
		//Ŀ�곡��
		UInt32	sceneId;
		//��ͼid
		UInt32	mapId;
		//�����
		CLPosition	pos;
		//�Ƿ��һ�ν�����
		UInt8	bFirstEnter;
		//�����ȼ�
		UInt16	level;
	};

	/**
	 *@brief client->server �뿪��̬����
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
	 *@brief server->client ������ͬ��npc��pet�ƶ�
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
		//npc�����id
		ObjID_t		id;
		//·����ʼλ��
		CLPosition	pos;
		//����·��
		UInt8		path[2];
	};

	/**
	 *@brief server->client ������֪ͨ�ͻ���ֹͣ�ƶ�
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
		//npc�����id
		ObjID_t	id;
		//ͣ��ʱ��λ��
		CLPosition	pos;
	};

	/**
	*@brief world->scene ��ѯ��ҵ�λ��
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
		// ���ID
		ObjID_t		uPlayerId;
		// ����,1 �����ټ�
		UInt8		uType;
		// ����
		ObjID_t		uParam;
	};

	/**
	*@brief scene->world ��ѯ��ҵ�λ�÷���
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
		// ���ID
		ObjID_t		uPlayerId;
		// ��ͼID
		UInt32		uMapId;
		// λ��
		CLPosition	pos;
		// ����,1 �����ټ�
		UInt8		uType;
		// ����
		ObjID_t		uParam;
	};

    /**
    *@brief clent->scene ����ص�����
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
		//0:���س���(����ʱ������������س���)�� 1:����ս��, 2:�����˳�
		UInt8  tag;
    };
}

#endif
