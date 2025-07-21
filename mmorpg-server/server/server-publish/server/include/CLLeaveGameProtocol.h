#ifndef _CL_LEAVEGAME_PROTOCOL_H_
#define _CL_LEAVEGAME_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLPlayerDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->gate �����뿪��Ϸ
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
	 *@brief scene��world��admin->gate ������Ự�����ߵ����
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

		//�˺�id
		UInt32	accid;
		//���id
		ObjID_t id;
		//������
		UInt32	errorCode;
		//��Ϣ
		std::string	msg;
	};

	/**
	 *@brief gate->client ֪ͨ�ͻ��˱���
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
		//������
		UInt32	errorCode;
		//��Ϣ
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
		// ��ɫid
		UInt64 roleId;
		// �뿪ʱ��
		UInt64 leaveTime;
	};
	
	/**
	 *@brief gate->world ֪ͨ�������
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

		//�˺�ID
		UInt32 accid;
		//���id
		ObjID_t	id;
		//�Ƿ����߳�ʱ
		UInt8 isOfflineTimeOut;
	};

	/**
	*@brief gate->world ֪ͨWorld��ҶϿ����ӣ�����ֻ�����ϣ�
	*/
	class GateNotifyDisconnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DISCONNECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//���id
		ObjID_t	id;
	};

	/**
	*@brief gate->world ֪ͨWorld��������ɹ�
	*/
	class GateNotifyReconnectSuccess : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_RECONN_SUCCESS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//���id
		ObjID_t	id;
	};
	

	/**
	 *@brief world->scene ֪ͨ���������
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

		//���
		ObjID_t id;
		//ǿ��ɾ�����
		UInt8	forceLeave;
		//�Ƿ����߳�ʱ
		UInt8	isOfflineTimeOut;
	};

	/**
	*@brief world->scene ֪ͨ����߼�����(�Ͽ�����)
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

		//���
		ObjID_t id;
	};
	

	/**
	 *@brief scene->world ֪ͨworld���ע��
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

		//���id
		ObjID_t id;
		//����id
		UInt32	sceneId;
		//λ��
		CLPosition pos;
	};

	/**
	*@brief scene->world �������ʱ��ͬ��sharedata
	*/
	class SceneSyncShareDataToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SHAREDATA_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_Guid & sharedata;
		}
		ObjID_t m_Guid;    //��ɫid
		PlayerSenceShareData sharedata;
	};
}

#endif
