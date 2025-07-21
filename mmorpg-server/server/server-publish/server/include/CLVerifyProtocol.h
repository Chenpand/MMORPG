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
	*@brief ���󴴽���֤����
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

		// ����ID
		ObjID_t			raceId;
		// ����
		UInt8			raceType;
		// ��ʼ��
		Avalon::Packet*	initPacket;
	};

	/**
	*@brief ������֤���񷵻�
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
	*@brief ������֤
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
	*@brief �ϱ�֡����
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
	*@brief �ϱ�У������
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
	*@brief �ϱ�����
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

		// ����ID
		ObjID_t			raceId;
		// ���ID
		ObjID_t			roleId;
		// �����
		Avalon::Packet*	packet;
	};

	/** 
	*@brief �����Ѿ���ɢ
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
	*@brief �ϱ���֤�����������ף�
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
		// ��Ӧö�� PlayerCheatReason
		UInt32				result;
	};

	/**
	*@brief ��֤������������֤����
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

		// ����(��Ӧö��RaceType)
		UInt8			raceType;
		// ����ID
		ObjID_t			raceId;
		// �����
		Avalon::Packet*	packet;
	};

	/**
	*@brief ��֤����������ע��
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
	*@brief ��֤����������ע��
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
	*@brief �ϱ��������
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
	*@brief ֪ͨ�����ڵ�ĸ������
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
	*@brief ֪ͨ��֤����������Ϣ
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
	*@brief ֪ͨ������֤������
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
	*@brief ֪ͨ������֤������
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