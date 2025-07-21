#ifndef __UDPCONNECTOR_HEAD_H__
#define __UDPCONNECTOR_HEAD_H__

#include "Common/common_base.h"
#include "Common/common_codeengine.h"
#include "gameplatform_message.h"

namespace KingNet { namespace Server {

typedef enum enmUdpCtrMsgType
{
	udp_head_type_invalid =					(int8_t)-1,

	//方向: client --> Service
	udp_head_type_client_request =			(int8_t)0x01,

	//方向: Service --> client
	udp_head_type_service_notify =			(int8_t)0x02,

}UdpCtrMsgType;


class CUdpNetHead
{
public:
	CUdpNetHead();
	~CUdpNetHead();

	int32_t encode(char* pszCode, int32_t& iCodeSize);

	int32_t decode(const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen=NULL);

	void dump();

	int32_t size();

	int32_t m_iPackageSize;		//整个包的长度
	int8_t m_cControlType;
	uint16_t m_unSocketIndex;

	union __U
	{
		struct stSourceIPInfo
		{
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;
		}m_stSourceIPInfo;				//head_type_client_request	

		struct stDestIPInfo
		{
			int32_t m_iDestIP;
			int16_t m_nDestPort;
		}m_stDestIPInfo;				//head_type_sevice_notify
	}m_stControlInfo;

protected:
private:
};

}}//namespace KingNet { namespace Server {

#endif



