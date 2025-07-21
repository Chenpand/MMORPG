#ifndef _CL_RECORD_PROTOCOL_H_
#define _CL_RECORD_PROTOCOL_H_

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"
#include "CLDatabaseDefine.h"

namespace CLProtocol
{
	/**
	 *@brief ����guid����
	 */
	class RecordGuidseedReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_GUIDSEED_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodetype & nodeid;
		}

		//�ڵ�����
		UInt32 nodetype;
		//�ڵ�id
		UInt32 nodeid;
	};

	/**
	 *@brief ����guid����
	 */
	class RecordGuidseedRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_GUIDSEED_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seed;
		}

		//����
		UInt64 seed;
	};

	/**
	 *@brief ������¼����
	 */
	class RecordInsertRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_INSERT_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid & result;
		}

		//����id
		UInt32	requestid;
		//������
		UInt32  result;
	};

	/**
	 *@brief �����¼����
	 */
	class RecordUpdateRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_UPDATE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid;
		}

		//����id
		UInt32	requestid;
	};

	/**
	 *@brief ɾ����¼����
	 */
	class RecordDeleteRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_DELETE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid;
		}

		//����id
		UInt32 requestid;
	};

	/**
	 *@brief ��д���ݷ���
	 */
	class RecordFlushRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_FLUSH_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid;
		}

		//����id
		UInt32 requestid;
	};

	/**
	 *@brief ��������
	 */
	class RecordChangeNameRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_CHANGENAME_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid & result;
		}

		//����id
		UInt32	requestid;
		//������
		UInt32	result;
	};

	/**
	 *@brief ת������                                                                     
	 */
	class RecordTransmitRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_TRANSMIT_REQUEST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & nodeId & (*packet);
		}

		RecordTransmitRequest(){ packet = Avalon::Packet::Create(); }
		RecordTransmitRequest(Avalon::Packet* pack){ packet = pack; }
		~RecordTransmitRequest(){ Avalon::Packet::Destroy(packet); }

		void Detach() { packet = NULL; }

		//ԭ��id
		UInt32	zoneId;
		//ԭ���ڵ�id
		UInt32	nodeId;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief ת���ظ�                                                                     
	 */
	class RecordTransmitResponse : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_TRANSMIT_RESPONSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & (*packet);
		}

		RecordTransmitResponse(){ packet = Avalon::Packet::Create(); }
		RecordTransmitResponse(Avalon::Packet* pack){ packet = pack; }
		~RecordTransmitResponse(){ Avalon::Packet::Destroy(packet); }

		void Detach() { packet = NULL; }

		//�ڵ�id
		UInt32	nodeId;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief ֪ͨ������record�������ж�                                                                     
	 */
	class RecordNotifyOtherDisconnected : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_NOTIFY_OTHER_DISCONNECTED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId;
		}

	public:
		//��id
		UInt32	zoneId;
	};
}

#endif
