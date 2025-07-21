#ifndef _CL_SECURITY_LOCK_PROTOCOL_H_
#define _CL_SECURITY_LOCK_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "AvalonProtocol.h"

namespace CLProtocol
{
	/**
	*@brief world->scene ͬ����ȫ��״̬
	*/
	class WorldSecurityStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & state;
		}

		// ��ɫid
		ObjID_t playerId;
		// ״̬
		UInt32 state;
	};

	/**
	*@brief client->server ����ȫ����Ϣ
	*/
	class WorldSecurityLockDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & deviceID;
		}
		// �豸id
		std::string deviceID;
	};

	/**
	*@brief client->server ����ȫ����Ϣ����
	*/
	class WorldSecurityLockDataRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_DATA_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lockState & isCommonDev & isUseLock & freezeTime & unFreezeTime;
		}

		// ��״̬
		UInt32 lockState; 
		// �Ƿ����豸
		UInt32 isCommonDev;
		// �Ƿ��Ϲ���
		UInt32 isUseLock;
		// ����ʱ��
		UInt32 freezeTime;
		// �ⶳʱ��
		UInt32 unFreezeTime;
	};

	/**
	*@brief client->server �������ð�ȫ������
	*/
	class WorldSecurityLockOpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_OP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lockOpType & passwd;
		}
		
		// ������
		UInt32 lockOpType;
		// ����
		std::string passwd;
	};

	/**
	*@brief server->client �������ð�ȫ�����뷵��
	*/
	class WorldSecurityLockOpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_OP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & lockOpType & lockState & freezeTime & unFreezeTime;
		}

		WorldSecurityLockOpRes()
		{
			freezeTime = 0;
			unFreezeTime = 0;
		}

		// ����ֵ
		UInt32 ret;
		// ������
		UInt32 lockOpType;
		// ��״̬
		UInt32 lockState;
		// ����ʱ��
		UInt32 freezeTime;
		// �ⶳʱ��
		UInt32 unFreezeTime;
	};


	/**
	*@brief client->server �����޸İ�ȫ������
	*/
	class WorldChangeSecurityPasswdReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_CHANGE_PASSWD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oldPasswd & newPasswd;
		}

		// ������
		std::string oldPasswd;
		// ������
		std::string newPasswd;
	};
	
	/**
	*@brief server->client �����޸İ�ȫ�����뷵��
	*/
	class WorldChangeSecurityPasswdRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_CHANGE_PASSWD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & errNum;
		}

		// ����ֵ
		UInt32 ret;
		// �������
		UInt32 errNum;
	};

	/**
	*@brief client->server ����󶨻����豸
	*/
	class WorldBindDeviceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BIND_DEVICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bindType & deviceID;
		}

		// ����0�ǽ�󣬷�����ǰ�
		UInt32 bindType;
		// �豸ID
		std::string deviceID;
	};

	/**
	*@brief server->client �豸�Ƿ�󶨷���
	*/
	class WorldBindDeviceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BIND_DEVICE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & bindState & deviceID;
		}

		// ����ֵ
		UInt32 ret;
		// ��״̬��0û�󣬷����
		UInt32 bindState;
		// �豸ID
		std::string deviceID;
	};

	/**
	*@brief server->client ֪ͨ�ͻ��˰�ȫ����ֹ����
	*/
	class WorldSecurityLockForbid : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_FORBID)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret;
		}

		// ����ֵ
		UInt32 ret;
	};

	/**
	*@brief server->client ��ȫ������������
	*/
	class WorldSecurityLockPasswdErrorNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_PASSWD_ERROR_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & error_num;
		}

		WorldSecurityLockPasswdErrorNum()
		{
			error_num = 0;
		}

		// ����������
		UInt32 error_num;
	};

	/**
	*@brief client->gate �������ؽ����ȫ��
	*/
	class GateSecurityLockRemoveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_REMOVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & passwd;
		}

		// ����
		std::string passwd;
	};

	/**
	*@brief gate->client ���ؽ����ȫ������
	*/
	class GateSecurityLockRemoveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_REMOVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret;
		}

		// ����ֵ
		UInt32 ret;
	};

	/**
	*@brief gate->world �������ؽ����ȫ��
	*/
	class GateToWorldSecurityLockRemoveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_TO_WORLD_SECURITY_LOCK_REMOVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & passwd;
		}

		// �˻�id
		UInt32 accId;
		// ����
		std::string passwd;
	};

	/**
	*@brief world->gate ���ؽ����ȫ������
	*/
	class GateToWorldSecurityLockRemoveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_TO_WORLD_SECURITY_LOCK_REMOVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & errorNum & ret;
		}

		GateToWorldSecurityLockRemoveRes()
		{
			errorNum = 0;
		}

		// �˻�id
		UInt32 accId;
		// ����������
		UInt32 errorNum;
		// ����ֵ
		UInt32 ret;
	};

	/**
	*@brief gate->world ������world��鰲ȫ��
	*/
	class GateSecurityLockCheckReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_CHECK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & roleId & deviceId;
		}

		// �˺�id
		UInt32		accId;
		// ��ɫid
		ObjID_t		roleId;
		// �豸ID
		std::string	deviceId;
	};

	/**
	*@brief world->gate ������world��鰲ȫ������
	*/
	class GateSecurityLockCheckRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_CHECK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & roleId & errorNum & ret;
		}

		GateSecurityLockCheckRes()
		{
			errorNum = 0;
		}

		// �˺�id
		UInt32		accId;
		// ��ɫid
		ObjID_t		roleId;
		// ����������
		UInt32		errorNum;
		// ����ֵ
		UInt32		ret;
	};
}

#endif