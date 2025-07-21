#ifndef _AS_EXTERNAL_NETWORK_H_
#define _AS_EXTERNAL_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLExternalNetwork.h>

/**
 *@brief 外网连接
 */
class ASExternalConnection : public CLExternalConnection
{
public:
	ASExternalConnection(CLNetwork* network);
	~ASExternalConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);

	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief 验证账号
	 */
	bool VerifyAccount(const UInt8* _hashval, const std::string& uparams, const std::string& key1, const std::string& key2);

private:
	//验证通过时间
	UInt32	m_VerifyTime;

	//账号id
	UInt32		m_AccId;
	//openid
	std::string	m_OpenId;
	//qq特权信息
	QQVipInfo	m_qqVipInfo;
	//hash时间戳
	UInt32		m_HashTimestamp;
	//gm权限
	UInt8		m_GmAuthority;
	//平台
	std::string	m_Pf;
	//设备ID
	std::string m_DeviceId;
	//绑定手机的角色ID
	ObjID_t		m_PhoneBindRoleId;
	//是否认证身份(未认证是游客)
	UInt32		m_IsIDAuth;
	//沉迷时间
	UInt32		m_AddictSecTime;
	//年龄
	UInt8		m_Age;
	//最后登录时间
	UInt32		m_LastLoginTime;
	//设备型号
	std::string	m_Model;
	//设备版本
	std::string m_DeviceVersion;
};

/**
 *@brief 外网
 */
class ASExternalNetwork : public CLExternalNetwork, public Avalon::Singleton<ASExternalNetwork>
{
public:
	ASExternalNetwork();
	~ASExternalNetwork();

public:

	void RegisterProtocols();

	/**
	 *@brief 创建一个连接
	 */
	Avalon::NetConnection* CreateConnection();

public://事件
	/**
	 *@brief 一个连接建立
	 */
	virtual void OnConnected(UInt32 id);

	/**
	 *@brief 一个连接断开
	 */
	virtual void OnDisconnected(UInt32 id);
};

#endif
