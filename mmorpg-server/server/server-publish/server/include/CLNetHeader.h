#ifndef _CL_NETHEADER_H_
#define _CL_NETHEADER_H_

#include <AvalonDefine.h>
#include <CLDefine.h>
#include <stdint.h>

typedef enum enmControlId
{
	enmControlId_invalid	= -1,
	enmControlId_notify_start = 0x01,		//client-->Service,client首次发送消息给service.
	enmControlId_request_transmit = 0x02,	//client <--> Service, client与service之间互相传递消息(备:若client第一次发送消息，则用前一条指条发送
	enmControlId_request_disconnect_client = 0x03,	// Service --> client,Service主动关闭client的链接.
	enmControlId_notify_disconnect	= 0x04,		//client --> Service, client主动断线
	enmControlId_notify_ping = 0x05,		// udpserver -> relayserver, 通知与客户端之间的ping值
}NetHeadControlID;

typedef enum enmDisconnectReason
{
	enmDisconnectReason_system				= 1,
	enmDisconnectReason_client_close		= 2,				//client主动关闭(超时，主动关闭等)
	enmDisconnectReason_service				= 3,				//service主动要求关闭
	enmDisconnectReason_invalid_newclient	= 4,				//
}EDisconnectReason;

typedef enum enmForceCloseType
{
	enmForceCloseType_Now = 1,
	enmForceCloseType_Delay //等数据全部发送成功后后关闭该连接.
}ForceCloseType;

class CNetHead
{
public:
	CNetHead();
	~CNetHead();

	/*
	* @method:    encode 编码
	* @fullname:
	* @access:    public
	* @param: char * pszCode 传出编码后的code
	* @param: int32_t & iCodeSize 传入pszCode的最大长度，传出实际code的长度
	* @return:   int
	* - success: 编码成功
	* - 其他: 失败
	* @qualifier:
	* @note
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated
	*/
	int32_t encode(char* pszCode, int32_t& iCodeSize);

	/*
	* @method:    decode 解码
	* @fullname:
	* @access:    public
	* @param: const char * pszCode 传入待解码的code
	* @param: const int32_t iCodeSize 待解码code的长度
	* @return:   int
	* - success : 成功
	* - 其他: 失败
	* @qualifier:
	* @note
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated
	*/
	int32_t decode(const char *pszCode, const int32_t iCodeSize, int16_t *pHeadLen = NULL);

	/*
	* @method:    dump 把头部信息dump出来（到文件，stdout等）
	* @fullname:
	* @access:    public
	* @return:   void
	* @qualifier:
	* @note
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated
	*/
	void dump();

	/*
	* @method:    size 计算赋值后的 head的实际长度
	* @fullname:  
	* @access:    public
	* @return:    返回头部长度()
	* @qualifier:
	* @note
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated
	*/
	int32_t size();
public:
	int32_t		m_iPackageSize;		//包总长度
	int8_t		m_cCtrlId;			//命令id
	uint32_t	m_unAccId;			//帐号id
	uint64_t	m_RoleId;			//角色id

	union __U
	{
		struct stSourceIPInfo
		{
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;
		}m_stSourceIPInfo;				//enmControlId_notify_start

		int32_t m_iPing;				//enmControlId_request_transmit   ( c->s :表示client ping值, s->c: 是控制connect server是否断开连接, 0 表示不断开连接， >0 表示断开连接)

		struct __stDisconnectInfo__
		{
			int8_t m_cDisconnectType;	//取值enmForceCloseType
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;

		}m_stDisconnectInfo;			//enmControlId_request_disconnect_client . 
		

		struct stClientRequestDisconnect
		{
			//int32_t m_iAccId;
			int8_t m_cDisconnectReason;		//取值：enmDisconnectReason
		}m_stClientDisconnect;			//enmControlId_notify_disconnect

	}m_stControlInfo;

private:
};


#endif
