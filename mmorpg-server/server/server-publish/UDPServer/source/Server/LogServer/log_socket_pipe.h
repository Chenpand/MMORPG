#ifndef _SOCKSTREAM_H_
#define _SOCKSTREAM_H_
#include "log_common_define.h"
namespace KingNet { namespace Server { namespace LogServer {
class CSocketPipe
{
public:
	CSocketPipe();
	~CSocketPipe();

	enum _enParas
	{
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || defined(__DEBUG__)
		RCVBUFSIZE	= 0x50000,
		SNDBUFSIZE	= 0x50000,
#else 
		RCVBUFSIZE	= 0x2000000,
		SNDBUFSIZE	= 0x2000000,

#endif 
		STREAM_CLOSED = 0,
		STREAM_OPENED = 1,
		ERR_STRM_NOSOCK = -1,
		ERR_STRM_AGAIN	= -2,
		ERR_STRM_PEER	= -3,
		ERR_STRM_FAILED	= -4,
	};

	int32_t SetUpStream(int32_t iFD);
	int32_t GetSocketFD();
	int32_t GetStatus();

	int32_t RecvData();
	int32_t GetOneCode(int32_t &nCodeLength, char *pCode);
	int32_t SendOneCode(int32_t nCodeLength, char *pCode);
	int32_t AddToCheckSet(fd_set *pCheckSet);
	bool IsFDSetted(fd_set *pCheckSet);

public:
	
protected:

	int32_t CloseStream();

	int32_t m_iSocketFD;					//SocketÃèÊö×Ó
	int32_t m_iStatus;						//Á÷×´Ì¬

private:
	int32_t  m_iReadBegin;
	int32_t  m_iReadEnd;
	char m_abyRecvBuffer[RCVBUFSIZE];

	int32_t  m_iPostBegin;
	int32_t  m_iPostEnd;
	char m_abyPostBuffer[SNDBUFSIZE];
};

}}}
#endif
