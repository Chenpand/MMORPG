#ifndef _CL_SECURITY_LOCK_PROTOCOL_H_
#define _CL_SECURITY_LOCK_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "AvalonProtocol.h"

namespace CLProtocol
{
	/**
	*@brief world->scene 同步安全锁状态
	*/
	class WorldSecurityStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & state;
		}

		// 角色id
		ObjID_t playerId;
		// 状态
		UInt32 state;
	};

	/**
	*@brief client->server 请求安全锁信息
	*/
	class WorldSecurityLockDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & deviceID;
		}
		// 设备id
		std::string deviceID;
	};

	/**
	*@brief client->server 请求安全锁信息返回
	*/
	class WorldSecurityLockDataRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_DATA_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lockState & isCommonDev & isUseLock & freezeTime & unFreezeTime;
		}

		// 锁状态
		UInt32 lockState; 
		// 是否常用设备
		UInt32 isCommonDev;
		// 是否上过锁
		UInt32 isUseLock;
		// 冻结时间
		UInt32 freezeTime;
		// 解冻时间
		UInt32 unFreezeTime;
	};

	/**
	*@brief client->server 请求设置安全锁密码
	*/
	class WorldSecurityLockOpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_OP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lockOpType & passwd;
		}
		
		// 锁类型
		UInt32 lockOpType;
		// 密码
		std::string passwd;
	};

	/**
	*@brief server->client 请求设置安全锁密码返回
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

		// 返回值
		UInt32 ret;
		// 锁类型
		UInt32 lockOpType;
		// 锁状态
		UInt32 lockState;
		// 冻结时间
		UInt32 freezeTime;
		// 解冻时间
		UInt32 unFreezeTime;
	};


	/**
	*@brief client->server 请求修改安全锁密码
	*/
	class WorldChangeSecurityPasswdReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_CHANGE_PASSWD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oldPasswd & newPasswd;
		}

		// 旧密码
		std::string oldPasswd;
		// 新密码
		std::string newPasswd;
	};
	
	/**
	*@brief server->client 请求修改安全锁密码返回
	*/
	class WorldChangeSecurityPasswdRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_CHANGE_PASSWD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & errNum;
		}

		// 返回值
		UInt32 ret;
		// 错误次数
		UInt32 errNum;
	};

	/**
	*@brief client->server 请求绑定或解绑设备
	*/
	class WorldBindDeviceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BIND_DEVICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bindType & deviceID;
		}

		// 类型0是解绑，否则就是绑定
		UInt32 bindType;
		// 设备ID
		std::string deviceID;
	};

	/**
	*@brief server->client 设备是否绑定返回
	*/
	class WorldBindDeviceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BIND_DEVICE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & bindState & deviceID;
		}

		// 返回值
		UInt32 ret;
		// 绑定状态，0没绑，否则绑定
		UInt32 bindState;
		// 设备ID
		std::string deviceID;
	};

	/**
	*@brief server->client 通知客户端安全锁禁止操作
	*/
	class WorldSecurityLockForbid : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SECURITY_LOCK_FORBID)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret;
		}

		// 返回值
		UInt32 ret;
	};

	/**
	*@brief server->client 安全锁密码错误次数
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

		// 密码错误次数
		UInt32 error_num;
	};

	/**
	*@brief client->gate 请求网关解除安全锁
	*/
	class GateSecurityLockRemoveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_REMOVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & passwd;
		}

		// 密码
		std::string passwd;
	};

	/**
	*@brief gate->client 网关解除安全锁返回
	*/
	class GateSecurityLockRemoveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_REMOVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret;
		}

		// 返回值
		UInt32 ret;
	};

	/**
	*@brief gate->world 请求网关解除安全锁
	*/
	class GateToWorldSecurityLockRemoveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_TO_WORLD_SECURITY_LOCK_REMOVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & passwd;
		}

		// 账户id
		UInt32 accId;
		// 密码
		std::string passwd;
	};

	/**
	*@brief world->gate 网关解除安全锁返回
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

		// 账户id
		UInt32 accId;
		// 密码错误次数
		UInt32 errorNum;
		// 返回值
		UInt32 ret;
	};

	/**
	*@brief gate->world 网关向world检查安全锁
	*/
	class GateSecurityLockCheckReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SECURITY_LOCK_CHECK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & roleId & deviceId;
		}

		// 账号id
		UInt32		accId;
		// 角色id
		ObjID_t		roleId;
		// 设备ID
		std::string	deviceId;
	};

	/**
	*@brief world->gate 网关向world检查安全锁返回
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

		// 账号id
		UInt32		accId;
		// 角色id
		ObjID_t		roleId;
		// 密码错误次数
		UInt32		errorNum;
		// 返回值
		UInt32		ret;
	};
}

#endif