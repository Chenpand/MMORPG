#ifndef _CL_NEW_TITLE_PROTOCOL_H_
#define _CL_NEW_TITLE_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLNewTitleDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client ͬ��ͷ��,����
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
	*@brief client->world ͷ�δ�������
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
	*@brief world->client ͷ�δ�������
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
	*@brief client->world ͷ���ѵ�����
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
	*@brief world->client ͷ���ѵ�����
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
	*@brief world->client ͬ��������ɾ��ͷ��
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
	*@brief world->client ����ͷ������
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
	*@brief World->Scene ����ͷ��
	*/
	class WorldNewTitleModifyScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NEW_TITLE_MODIFY_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & ownerType & opType & titleGuid & titleName & titleId & style;
		}

		ObjID_t			roleId;		 //��ɫId
		UInt8			ownerType;	 //ӵ������
		UInt8			opType;		 //�������� [NewTitleOpType]
		UInt64			titleGuid;	 //ͷ��guid
		std::string	    titleName;   //ͷ������
		UInt32			titleId;	 //ͷ�α�id
		UInt8			style;		 //���
	};

	/**
	*@brief Scene->World ����ͷ��
	*/
	class SceneNewTitleModifyWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_TITLE_MODIFY_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & ownerType & opType & titleGuid & titleName;
		}

		ObjID_t			roleId;		 //��ɫId
		UInt8			ownerType;	 //ӵ������
		UInt8			opType;		 //�������� [NewTitleOpType]
		UInt64			titleGuid;	 //ͷ��guid
		std::string	    titleName;   //ͷ������
	};

	/**
	*@brief Scene->World ���ͷ��
	*/
	class SceneNewTitleAddWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_TITLE_ADD_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & titleId & dueTime;
		}
		ObjID_t			roleId;	   //��ɫId
		UInt32			titleId;   //ͷ��id
		UInt32			dueTime;   //����ʱ���
	};

	/**
	*@brief client->world ���Ϲ���ְλ��ͷ������
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
	*@brief client->world ���Ϲ���ְλ��ͷ�η���
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

