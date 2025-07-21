#ifndef __SERVER_ROUTER_MESSAGE_BODY_H__
#define __SERVER_ROUTER_MESSAGE_BODY_H__

#include "server_message.h"

namespace KingNet { namespace Server {

//SS_MSG_ROUTER_HEARTBEAT ������������router server֮���heart beat , notify ����
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

//SS_MSG_ROUTER_REGISTER ������������router serverע���Э��, request����
//��ʱû�в���

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

