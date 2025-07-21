#ifndef _CL_RETINUE_PROTOCOL_H_
#define _CL_RETINUE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLRetinueDefine.h"

namespace CLProtocol
{
	enum RetinueUpType
	{
		RUT_UPLEVEL = 1,	//�������
		RUT_UPSTAR = 2		//�������
	};


	/**
	*@brief server->client �·�����б�
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
	*@brief server->client �·����
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
	*@brief client->server ���� �����������
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
		//���ID
		ObjID_t id;
		//λ��
		UInt8 index;
	};

	/**
	*@brief server->client ��Ӧ �����������
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
		//����ֵ
		UInt32 result;
		//�����ID
		ObjID_t mainId;
		//�����ID�б�
		std::vector<UInt64> offRetinueIds;
	};

	/**
	*@brief client->server ���� ϴ��֧Ԯ����
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

		//���ID
		ObjID_t id;

		//ϴ����ID�б�
		std::vector<UInt32> skillIds;
	};

	/**
	*@brief server->client ��Ӧ ϴ��֧Ԯ����
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
	*@brief client->server ���� ��ӽ���
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
	*@brief server->client ��Ӧ ��ӽ���
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
	*@brief client->server ���� �������
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
		UInt8 type; //RetinueUpType��ö��
		UInt64 id;
	};

	/**
	*@brief server->client ��Ӧ �������
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