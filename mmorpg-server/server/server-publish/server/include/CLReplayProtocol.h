#ifndef __CL_REPLAY_PROTOCOL_H__
#define __CL_REPLAY_PROTOCOL_H__

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLReplayDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->scene �����ս��¼
	*/
	class SceneReplayListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLAY_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		// ��Ӧö��(ReplayListType)
		UInt8	type;
	};

	/**
	*@brief scene->client ���ض�ս��¼
	*/
	class SceneReplayListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLAY_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & replays;
		}

		// ��Ӧö��(ReplayListType)
		UInt8						type;
		std::vector<ReplayInfo>		replays;
	};

	/**
	*@brief client->scene ����һ��¼��
	*/
	class SceneReplayView : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLAY_VIEW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceid;
		}

		ObjID_t raceid;
	};
	
	/**
	*@brief scene->replay �������
	*/
	class ReplayReportPlayerOnline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_REPORT_PLAYER_ONLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t		roleId;
	};

	/**
	*@brief scene->replay �������
	*/
	class ReplayReportPlayerOffline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_REPORT_PLAYER_OFFLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t		roleId;
	};

	/**
	*@brief scene->replay �ϱ�ս����¼
	*/
	class ReplayReportRace : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_REPORT_RACE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		ReplayInfo			info;
	};
	
	/**
	*@brief scene->replay ����¼���б�
	*/
	class ReplayListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & type;
		}

		ObjID_t		roleid;
		UInt8		type;
	};

	/**
	*@brief replay->scene ��ȡ¼���б�
	*/
	class ReplayListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & type & replays;
		}

		ObjID_t					roleid;
		UInt8					type;
		std::vector<ReplayInfo> replays;
	};

	/**
	*@brief scene->replay scene�ϱ���ҿ���һ��¼��
	*/
	class ReplayReportView : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_REPORT_VIEW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & raceid;
		}

		ObjID_t		roleid;
		ObjID_t		raceid;
	};
	

	/**
	*@brief replay->relay ����¼���ļ�
	*/
	class ReplayFileReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_FILE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId;
		}

		ObjID_t raceId;
	};

	/**
	*@brief relay->replay ����¼���ļ�
	*/
	class ReplayFileRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_FILE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & error & raceId & totalLen & index;
		}

		// �Ƿ����
		UInt8				error;
		// ����ID
		ObjID_t				raceId;
		// �ܳ���
		UInt32				totalLen;
		// ��ʼλ��
		UInt32				index;
	};
};

#endif