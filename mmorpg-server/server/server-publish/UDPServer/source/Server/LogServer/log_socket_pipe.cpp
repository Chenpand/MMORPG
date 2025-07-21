#include "log_socket_pipe.h"
namespace KingNet { namespace Server { namespace LogServer {
CSocketPipe::CSocketPipe()
{
	m_iSocketFD = -1;
	m_iStatus = STREAM_CLOSED;

	m_iReadBegin = m_iReadEnd = 0;
	m_iPostBegin = m_iPostEnd = 0;
}
CSocketPipe::~CSocketPipe()
{
	if( STREAM_OPENED == m_iStatus && m_iSocketFD > 0 )
	{
		CloseStream();
	}
}
int32_t CSocketPipe::SetUpStream(int32_t iFD)
{
	if( iFD < 0 )
	{
		return -1;
	}

	if( m_iSocketFD > 0 && STREAM_OPENED == m_iStatus )
	{
		CloseStream();
	}

	m_iSocketFD = iFD;
	m_iStatus = STREAM_OPENED;
	m_iReadBegin = m_iReadEnd = 0;
	m_iPostBegin = m_iPostEnd = 0;

	set_non_block(m_iSocketFD);
	return 0;
}
int32_t CSocketPipe::CloseStream()
{
	if( m_iSocketFD > 0 )
	{
		close(m_iSocketFD);
	}

	m_iSocketFD = -1;
	m_iStatus = STREAM_CLOSED;
	return 0;
}

int32_t CSocketPipe::GetSocketFD()
{
	return m_iSocketFD;
}
int32_t CSocketPipe::GetStatus()
{
	return m_iStatus;
}
int32_t CSocketPipe::AddToCheckSet(fd_set *pCheckSet)
{
	int32_t iTempRet = 0;

	if( !pCheckSet )
	{
		return -1;
	}

	if( m_iSocketFD > 0 && m_iStatus == STREAM_OPENED )
	{
		FD_SET( m_iSocketFD+1 , pCheckSet );
	}
	else if( m_iSocketFD > 0 )
	{
		CloseStream();
		iTempRet = ERR_STRM_NOSOCK;
	}

	return iTempRet;
}
bool CSocketPipe::IsFDSetted(fd_set *pCheckSet)
{
 
	if( !pCheckSet )
	{
		return false;
	}

	if( m_iSocketFD > 0 && STREAM_OPENED == m_iStatus )
	{
		return  FD_ISSET( m_iSocketFD, pCheckSet );
	}

	return false;
}
int32_t CSocketPipe::RecvData()
{
	int32_t iRecvedBytes = 0;
	//int32_t iMaxBytes = 0;
	int32_t iTempRet = success;

	if( m_iSocketFD < 0 || STREAM_OPENED != m_iStatus )
	{
		return ERR_STRM_NOSOCK;
	}

	if( m_iReadEnd == m_iReadBegin )
	{
		m_iReadBegin = 0;
		m_iReadEnd = 0;
	}

	do
	{
		//如果接收缓冲区已满，停止接收数据
		if( m_iReadEnd == sizeof(m_abyRecvBuffer) )
		{
			if(m_iReadBegin==0)
			{
				printf("The recv buffer is full now(%d, %d), stop recv data, fd = %d.\n",
					m_iReadBegin, m_iReadEnd, m_iSocketFD);
				iTempRet = ERR_STRM_AGAIN;
				break;
			}
			else
			{
				memmove((void*)&m_abyRecvBuffer[0],(void*)&m_abyRecvBuffer[m_iReadBegin],m_iReadEnd-m_iReadBegin); 
				m_iReadEnd = m_iReadEnd-m_iReadBegin;
				m_iReadBegin = 0;
			}
		}

		iRecvedBytes = recv(m_iSocketFD, &m_abyRecvBuffer[m_iReadEnd],
			sizeof(m_abyRecvBuffer)-m_iReadEnd, 0);
		if( iRecvedBytes > 0 )
		{
			m_iReadEnd += iRecvedBytes;
		}
		else if( iRecvedBytes == 0 )
		{
			printf("The peer side may closed this stream, fd = %d, errno = %d.\n", m_iSocketFD, errno);
			CloseStream();
			iTempRet = ERR_STRM_PEER;
			break;
		}
		else if( errno != EAGAIN )
		{
			CloseStream();
			iTempRet = ERR_STRM_FAILED;
			break;
		}
	} while( iRecvedBytes > 0 );

	return iTempRet;
}

//从接收缓冲区里取出一个消息包出来...
int32_t CSocketPipe::GetOneCode(int32_t &nCodeLength, char *pCode)
{
	int32_t iDataLength = 0;
	int32_t nTempLength;

	if( !pCode )
	{
		return -1;
	}

	iDataLength = m_iReadEnd - m_iReadBegin;

	if( iDataLength <= 0 )
	{
		return 0;
	}

	if( iDataLength < (int32_t)sizeof(int32_t) )
	{
		if( m_iReadEnd == (int32_t)sizeof(m_abyRecvBuffer) )
		{
			memmove((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		return 0;
	}

	memcpy((void *)&nTempLength, (const void *)&m_abyRecvBuffer[m_iReadBegin], sizeof(int32_t));
	nTempLength = ntohl(nTempLength);
	if( iDataLength < nTempLength )
	{
		if( m_iReadEnd == (int32_t)sizeof(m_abyRecvBuffer) )
		{
			memmove((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}
		return 0;
	}

	nCodeLength = nTempLength;
	memcpy((void *)pCode, (const void *)&m_abyRecvBuffer[m_iReadBegin], nCodeLength);

	m_iReadBegin += nTempLength;
	if( m_iReadBegin == m_iReadEnd )
	{
		m_iReadBegin = m_iReadEnd = 0;
	}

	return 1;
}
int32_t CSocketPipe::SendOneCode(int32_t nCodeLength, char *pCode)
{
	int32_t iBytesSent = 0;
	int32_t iBytesLeft = nCodeLength;
	char *pbyTemp = NULL;
	int32_t iTempRet = success;

	if( !pCode )
	{
		return ERR_STRM_NOSOCK;
	}

	if( m_iSocketFD < 0 || STREAM_OPENED != m_iStatus )
	{
		return ERR_STRM_NOSOCK;
	}

	//首先检查是否有滞留数据
	iBytesLeft = m_iPostEnd - m_iPostBegin;
	pbyTemp = &(m_abyPostBuffer[m_iPostBegin]);
	while( iBytesLeft > 0 )
	{
		iBytesSent = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);

		if( iBytesSent > 0 )
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;

			m_iPostBegin +=iBytesSent;
		}
 
		if( iBytesSent < 0 && errno != EAGAIN )
		{
			//出现发送错误，关闭流
			CloseStream();
			iTempRet = ERR_STRM_FAILED;
			//break;
			return iTempRet;
		}
		else if( iBytesSent < 0 )
		{
			//TCP层发送缓冲已满
			//iTempRet = ERR_STRM_AGAIN;
			break;
		}
	}

	if( iBytesLeft > 0 )
	{
		if(iBytesLeft+nCodeLength>=SNDBUFSIZE)
		{
			return  ERR_STRM_AGAIN;
		}

		//还有待发送的数据,将待发送的数据加在后面....
	    if(m_iPostBegin>0)
		{
			memmove((void *)&(m_abyPostBuffer[0]), (const void *)pbyTemp, iBytesLeft);
			m_iPostBegin = 0;
			m_iPostEnd = iBytesLeft;
		}
		memcpy((void *)&(m_abyPostBuffer[m_iPostEnd]), (const void *)pCode, nCodeLength);
		m_iPostEnd +=nCodeLength;
		return success;
	}

	//发送本次数据....
	m_iPostBegin = m_iPostEnd = 0;

	//发送本次提交的数据
	iBytesLeft = nCodeLength;
	pbyTemp = pCode;

	while( iBytesLeft > 0 )
	{
		iBytesSent = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);

		if( iBytesSent > 0 )
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;
		}
		if( iBytesSent < 0 && errno != EAGAIN )
		{
			//出现发送错误，关闭流
			CloseStream();
			return  ERR_STRM_FAILED;
			
		}
		else if( iBytesSent < 0 )
		{
			//Socket发送缓冲区满，则将剩余的数据放到缓存中
			memcpy((void *)&(m_abyPostBuffer[m_iPostEnd]), (const void *)pbyTemp, iBytesLeft);
			m_iPostEnd +=iBytesLeft;
			return success;
		}
	}

	return success;
}


}}}

