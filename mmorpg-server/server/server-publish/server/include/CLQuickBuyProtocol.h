#ifndef _CL_QUICKBUY_PROTOCOL_H_
#define _CL_QUICKBUY_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	*@brief client -> scene ���ٹ�������
	*/
	class SceneQuickBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & param1 & param2;
		}

		// ����(��Ӧö��QuickBuyTargetType)
		UInt8		type;

		// ����
		ObjID_t		param1;
		ObjID_t		param2;
	};

	/**
	*@brief scene -> client ���ٹ��򷵻�
	*/
	class SceneQuickBuyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32		result;
	};

	/**
	*@brief scene -> world ���ٹ�����������
	*/
	class SceneQuickBuyTransactionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_TRANSACTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & transactionId & type & param1 & param2;
		}

		// ��ɫID
		ObjID_t		roleId;
		// ����ID
		UInt32		transactionId;
		// ����(��Ӧö��QuickBuyTargetType)
		UInt8		type;
		// ����
		ObjID_t		param1;
		ObjID_t		param2;
	};

	/**
	*@brief world -> scene ���ٹ������񷵻�
	*/
	class SceneQuickBuyTransactionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICKBUY_TRANSACTION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & transactionId & result;
		}

		// ��ɫID
		ObjID_t		roleId;
		// ����ID
		UInt32		transactionId;
		// ����
		UInt32		result;
	};
};

#endif