#ifndef __CL_REPLAY_PROTOCOL_H__
#define __CL_REPLAY_PROTOCOL_H__

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLReplayDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->scene 请求对战记录
	*/
	class SceneReplayListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLAY_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		// 对应枚举(ReplayListType)
		UInt8	type;
	};

	/**
	*@brief scene->client 返回对战记录
	*/
	class SceneReplayListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLAY_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & replays;
		}

		// 对应枚举(ReplayListType)
		UInt8						type;
		std::vector<ReplayInfo>		replays;
	};

	/**
	*@brief client->scene 看了一场录像
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
	*@brief scene->replay 玩家上线
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
	*@brief scene->replay 玩家下线
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
	*@brief scene->replay 上报战斗记录
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
	*@brief scene->replay 请求录像列表
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
	*@brief replay->scene 获取录像列表
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
	*@brief scene->replay scene上报玩家看了一场录像
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
	*@brief replay->relay 请求录像文件
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
	*@brief relay->replay 返回录像文件
	*/
	class ReplayFileRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(REPLAY_FILE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & error & raceId & totalLen & index;
		}

		// 是否出错
		UInt8				error;
		// 比赛ID
		ObjID_t				raceId;
		// 总长度
		UInt32				totalLen;
		// 起始位置
		UInt32				index;
	};
};

#endif