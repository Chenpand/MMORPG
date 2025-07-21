#ifndef __SERVER_ROUTER_MESSAGE_BODY_H__
#define __SERVER_ROUTER_MESSAGE_BODY_H__

#include "server_message.h"

namespace KingNet { namespace Server {

//SS_MSG_ROUTER_HEARTBEAT 其他服务器与router server之间的heart beat , notify 类型
class CNotifyRouterHeartBeat : public CMessageBody
{
public:
	CNotifyRouterHeartBeat();
	~CNotifyRouterHeartBeat();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iServerAddr;
	int16_t m_nServerPort;

protected:
private:
};

//SS_MSG_ROUTER_REGISTER 其他服务器向router server注册的协议, request类型
//暂时没有参数

// class CRequestRegisterToRouter : public CMessageBody
// {
// public:
// 	CRequestRegisterToRouter();
// 	~CRequestRegisterToRouter();
// 
// 	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
// 	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
// 	virtual void dump();
// 
// protected:
// private:
// };

}}//namespace KingNet { namespace Server {



#endif //__SERVER_ROUTER_MESSAGE_BODY_H__

