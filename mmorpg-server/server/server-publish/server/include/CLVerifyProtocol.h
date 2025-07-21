#ifndef __CL_VERIFY_PROTOCOL_H__
#define __CL_VERIFY_PROTOCOL_H__

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"
#include "CLReplayProtocol.h"
#include "CLVerifyDefine.h"
#include "CLVerifyNode.h"

namespace CLProtocol
{
	/**
	*@brief 请求创建验证任务
	*/
	class VerifyCreateTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CREATE_TASK_REQ)
	public:
		VerifyCreateTaskReq(Avalon::Packet* packet = NULL)
		{
			initPacket = packet;
			if (!initPacket)
			{
				initPacket = Avalon::Packet::Create();
			}
		}

		virtual ~VerifyCreateTaskReq()
		{
			if (initPacket)
			{
				Avalon::Packet::Destroy(initPacket);
			}
		}

		void Detach()
		{
			initPacket = NULL;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & raceType & *initPacket;
		}

		// 比赛ID
		ObjID_t			raceId;
		// 类型
		UInt8			raceType;
		// 开始包
		Avalon::Packet*	initPacket;
	};

	/**
	*@brief 创建验证任务返回
	*/
	class VerifyCreateTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CREATE_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & result;
		}

		ObjID_t			raceId;
		UInt32			result;
	};

	/**
	*@brief 放弃验证
	*/
	class VerifyGiveupTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_GIVEUP_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId;
		}

		ObjID_t			raceId;
	};

	/**
	*@brief 上报帧数据
	*/
	class VerifyReportFrame : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_REPORT_FRAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & frames;
		}

		ObjID_t				raceId;
		std::vector<Frame>	frames;
	};

	/**
	*@brief 上报校验数据
	*/
	class VerifyReportChecksum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_REPORT_CHECKSUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & playerChecksums;
		}

		ObjID_t							raceId;
		std::vector<PlayerChecksumInfo>	playerChecksums;
	};

	/**
	*@brief 上报结算
	*/
	class VerifyReportRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_REPORT_RACE_END)
	public:
		VerifyReportRaceEnd()
		{
			packet = Avalon::Packet::Create();
		}

		virtual ~VerifyReportRaceEnd()
		{
			if (packet)
			{
				Avalon::Packet::Destroy(packet);
			}
		}

		void Detach()
		{
			packet = NULL;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & roleId & *packet;
		}

		// 比赛ID
		ObjID_t			raceId;
		// 玩家ID
		ObjID_t			roleId;
		// 结算包
		Avalon::Packet*	packet;
	};

	/** 
	*@brief 比赛已经解散
	*/
	class VerifyReportRaceDismiss : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_REPORT_RACE_DISMISS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId;
		}

		ObjID_t							raceId;
	};

	/**
	*@brief 上报验证结果（玩家作弊）
	*/
	class VerifyReportTaskResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_TASK_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & roleId & dungeonId & result;
		}

		ObjID_t				raceId;
		ObjID_t				roleId;
		UInt32				dungeonId;
		// 对应枚举 PlayerCheatReason
		UInt32				result;
	};

	/**
	*@brief 验证服务器返回验证结算
	*/
	class VerifyReportTaskRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_TASK_RACE_END)
	public:
		VerifyReportTaskRaceEnd(Avalon::Packet* packet_ = NULL)
		{
			packet = packet_;
			if (packet == NULL)
			{
				packet = Avalon::Packet::Create();
			}
		}

		virtual ~VerifyReportTaskRaceEnd()
		{
			if (packet)
			{
				Avalon::Packet::Destroy(packet);
			}
		}

		void Detach()
		{
			packet = NULL;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceType & raceId & *packet;
		}

		// 类型(对应枚举RaceType)
		UInt8			raceType;
		// 比赛ID
		ObjID_t			raceId;
		// 结算包
		Avalon::Packet*	packet;
	};

	/**
	*@brief 验证服务器请求注册
	*/
	class VerifyCenterRegisterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_REGISTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & addr & ability;
		}

		Avalon::SockAddr	addr;
		UInt32				ability;
	};

	/**
	*@brief 验证服务器请求注册
	*/
	class VerifyCenterRegisterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_REGISTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief 上报负载情况
	*/
	class VerifyCenterReportAbility : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_REPORT_ABILITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ability;
		}

		UInt32 ability;
	};

	/**
	*@brief 通知各个节点的负载情况
	*/
	class VerifyCenterNotifyAbility : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_NOTIFY_ABILITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & ability;
		}

		UInt32 nodeId;
		UInt32 ability;
	};

	/**
	*@brief 通知验证服务器的信息
	*/
	class VerifyCenterNotifyVSInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_NOTIFY_VS_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodes;
		}

		std::vector<CLVerifyNode>			nodes;
	};
	
	/**
	*@brief 通知新增验证服务器
	*/
	class VerifyCenterNotifyVSOnline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_NOTIFY_VS_ONLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & node;
		}

		CLVerifyNode		node;
	};

	/**
	*@brief 通知新增验证服务器
	*/
	class VerifyCenterNotifyVSOffline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(VERIFY_CENTER_NOTIFY_VS_OFFLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		UInt32 id;
	};
};

#endif