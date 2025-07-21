#ifndef _CL_RECORD_PROTOCOL_H_
#define _CL_RECORD_PROTOCOL_H_

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"
#include "CLDatabaseDefine.h"

namespace CLProtocol
{
	/**
	 *@brief 请求guid种子
	 */
	class RecordGuidseedReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_GUIDSEED_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodetype & nodeid;
		}

		//节点类型
		UInt32 nodetype;
		//节点id
		UInt32 nodeid;
	};

	/**
	 *@brief 返回guid种子
	 */
	class RecordGuidseedRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_GUIDSEED_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seed;
		}

		//种子
		UInt64 seed;
	};

	/**
	 *@brief 创建记录返回
	 */
	class RecordInsertRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_INSERT_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid & result;
		}

		//请求id
		UInt32	requestid;
		//错误码
		UInt32  result;
	};

	/**
	 *@brief 保存记录返回
	 */
	class RecordUpdateRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_UPDATE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid;
		}

		//请求id
		UInt32	requestid;
	};

	/**
	 *@brief 删除记录返回
	 */
	class RecordDeleteRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_DELETE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid;
		}

		//请求id
		UInt32 requestid;
	};

	/**
	 *@brief 回写数据返回
	 */
	class RecordFlushRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_FLUSH_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid;
		}

		//请求id
		UInt32 requestid;
	};

	/**
	 *@brief 改名返回
	 */
	class RecordChangeNameRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RECORD_CHANGENAME_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requestid & result;
		}

		//请求id
		UInt32	requestid;
		//错误码
		UInt32	result;
	};

	/**
	 *@brief 转发请求                                                                     
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

		//原区id
		UInt32	zoneId;
		//原区节点id
		UInt32	nodeId;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief 转发回复                                                                     
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

		//节点id
		UInt32	nodeId;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief 通知和其他record的连接中断                                                                     
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
		//区id
		UInt32	zoneId;
	};
}

#endif
