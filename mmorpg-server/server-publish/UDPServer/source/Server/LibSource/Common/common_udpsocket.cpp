#include "Common/common_udpsocket.h"

SERVER_BEGIN

CUDPSocket::CUDPSocket()
{
	m_iSocketfd = -1;
	m_unSectionSequence = 0;

	m_bmSlotUsed.clear();
	m_stReadySlotIdxSet.clear();
	m_stUsedSlotIdxsSet.clear();
	
}

//
CUDPSocket::~CUDPSocket()
{
	close_socket();
}

//
int32_t CUDPSocket::close_socket()
{
	if( m_iSocketfd >= 0 )
	{
		close(m_iSocketfd);
		m_iSocketfd = -1;
	}

	m_unSectionSequence = 0;

	m_stUsedSlotIdxsSet.clear();
	m_stReadySlotIdxSet.clear();
	m_bmSlotUsed.clear();

	return 0;
}

int32_t CUDPSocket::clear_slot_of(int32_t slot)
{
	//
	if( slot >= max_slot_count || slot < 0)
	{
		return -1;
	}
	m_bmSlotUsed.unset_bit(slot);

	memset(&m_astUDPSlots[slot], 0x0, sizeof(m_astUDPSlots[slot]));

	return 0;
}

//
int32_t CUDPSocket::set_fd(int32_t fd)
{
	if( fd < 0 )
	{
		return -1;
	}

	//
	m_iSocketfd = fd;

	//
	m_unSectionSequence = 0;
	m_bmSlotUsed.clear();
	m_stUsedSlotIdxsSet.clear();
	m_stReadySlotIdxSet.clear();
	
	return 0;
}

int32_t CUDPSocket::add_fd_set(fd_set* pfd_set)
{
	int32_t result = 0;
	
	if( NULL == pfd_set )
	{
		return -1;
	}
	if( m_iSocketfd > 0 )
	{
		FD_SET( m_iSocketfd, pfd_set );
	}

	time_t tmNow = 0;
	tmNow = time(NULL);
	
	for(int32_t i = 0; i < m_stUsedSlotIdxsSet.last(); ++i)
	{
		if( !m_bmSlotUsed.is_bit_setted(m_stUsedSlotIdxsSet[i]) )
		{
			m_stUsedSlotIdxsSet.del_item(i);
			i--;
			continue;
		}
		if( tmNow - m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_tmLastRcvTime >= max_timeout )
		{
			clear_slot_of(m_stUsedSlotIdxsSet[i]);
			m_stUsedSlotIdxsSet.del_item(i);
			i--;
		}
	}

	return result;
}

//
int32_t CUDPSocket::is_fd_be_setted(fd_set *pfd_set)
{
	int32_t result = 0;

	if( NULL == pfd_set )
	{
		return 0;
	}

	if( m_iSocketfd > 0 )
	{
		result = FD_ISSET( m_iSocketfd, pfd_set );
	}
	

	return result;
}

//从网络接收数据,并完成组包
int32_t CUDPSocket::receive_data()
{

	struct sockaddr_in stTempSocketInAddr;
	socklen_t iTempAddrLen = 0;
		
	
	u_long unlTmpSectionID = 0; //UDP包的unique id
	short nTmpSeq = -1;

	short nTotalSection = 0;

	if( m_iSocketfd < 0 )
	{
		return -1;
	}

	int32_t  iTempIdx = -1;
	int32_t  iSetIdx = -1;

	int32_t i = 0;
	int32_t idle_idx = -1;
	char szData[2*max_unit_size];

	ssize_t iRecvBytes = 0; //接收到的数据

	size_t  iDataSize = 0; 
	char *pszTmpData = NULL; 

	for(/**/; /**/;)
	{
	
		//have some free slot ?
		for( i = 0; i < max_slot_count; i++ )
		{
			if( !m_bmSlotUsed.is_bit_setted(i) )
			{
				break;
			}
		}
		if( i >= max_slot_count )
		{
			//can't find any free slot,just break 
			break;
		}
		else
		{

			idle_idx = i; //record the  idle slot index
		}

		memset(&stTempSocketInAddr, 0x0, sizeof(stTempSocketInAddr) );
		iTempAddrLen = sizeof(stTempSocketInAddr);

		pszTmpData = &szData[0];
		iDataSize = sizeof(szData);
		iRecvBytes = recvfrom( m_iSocketfd, pszTmpData, iDataSize, 0, (struct sockaddr *)&stTempSocketInAddr, (socklen_t *)&iTempAddrLen );

		if( iRecvBytes < 0 )
		{
			if( errno != EAGAIN )
			{
				//some really error
			}
			break;
		}

		//一个section至少要有 其所属的udp包，以及分片总书目，该片的序号信息
		if( iRecvBytes < (int32_t )(sizeof(u_long) + 2*sizeof(short)))
		{
			//此处的处理有些不妥：因为一次recvfrom调用是可能收到任意数量的数据。
			//所以此处的逻辑是有问题的,考虑到内网环境不会太差,暂时这样先
			continue;
		}

		//得到udp包sequence
		unlTmpSectionID = ntohl(*(u_long *)pszTmpData);
		pszTmpData += sizeof(unlTmpSectionID);
		iRecvBytes -= sizeof(unlTmpSectionID);

		//得到udp包的分片总数
		memcpy(&nTotalSection, (const void *)pszTmpData, sizeof(nTotalSection));
		nTotalSection = (short)ntohs(u_short(nTotalSection));
		pszTmpData += sizeof(nTotalSection);
		iRecvBytes -= sizeof(nTotalSection);

		//得到本分片的序号
		memcpy((void *)&nTmpSeq, (const void *)pszTmpData, sizeof(nTmpSeq));
		nTmpSeq = (short)ntohs((u_short)nTmpSeq);
		pszTmpData += sizeof(nTmpSeq);
		iRecvBytes -= sizeof(nTmpSeq);

		//检查分片序号是否合法
		if( nTmpSeq < 0 && nTmpSeq >= nTotalSection )
		{
			continue;
		}

		iTempIdx = -1;
		iSetIdx = -1;

		//查找是否正从对端收到数据包(还没收全)
		for( i = 0; i < m_stUsedSlotIdxsSet.last(); ++i)
		{
			if( m_bmSlotUsed.is_bit_setted(m_stUsedSlotIdxsSet[i]) &&
				m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_ulSrcIP == stTempSocketInAddr.sin_addr.s_addr &&
				m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_unSrcPort == stTempSocketInAddr.sin_port &&
				m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_unlSectionID == unlTmpSectionID )
			{
				if( nTotalSection != m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_nUnitCount )
				{
					//some error, just clear the slot
					clear_slot_of(m_stUsedSlotIdxsSet[i]);
					m_stUsedSlotIdxsSet.del_item(i);
				}
				else
				{
					memcpy(m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_astUnits[nTmpSeq].m_szUnit,
						(const void *)pszTmpData, iRecvBytes);
					m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_astUnits[nTmpSeq].m_nUnitSize = iRecvBytes;
					time(&m_astUDPSlots[m_stUsedSlotIdxsSet[i]].m_tmLastRcvTime);
					iTempIdx = m_stUsedSlotIdxsSet[i];
					iSetIdx = i;
				}
				break;
			}
		}

		//若是一个新的开始，使用上面找到的那一个空闲slot
		if( i >= m_stUsedSlotIdxsSet.last() )
		{
			
			i = idle_idx; //空闲slot idx
			m_bmSlotUsed.set_bit(i);
			iSetIdx = m_stUsedSlotIdxsSet.add_item(i);

			//保持数据
			m_astUDPSlots[i].m_ulSrcIP = stTempSocketInAddr.sin_addr.s_addr;
			m_astUDPSlots[i].m_unSrcPort = stTempSocketInAddr.sin_port;
			m_astUDPSlots[i].m_nUnitCount = nTotalSection;
			m_astUDPSlots[i].m_unlSectionID = unlTmpSectionID;

			memcpy(m_astUDPSlots[i].m_astUnits[nTmpSeq].m_szUnit,
				(const void *)pszTmpData, (ssize_t)iRecvBytes);

			m_astUDPSlots[i].m_astUnits[nTmpSeq].m_nUnitSize = iRecvBytes;
			time(&m_astUDPSlots[i].m_tmLastRcvTime);

			iTempIdx = i;
			
		}

		//added by aprilliu 
		if (iTempIdx < 0)
		{
			continue;
		}

		//received everything ?
		for( i = 0; i < m_astUDPSlots[iTempIdx].m_nUnitCount; i++ )
		{
			if( m_astUDPSlots[iTempIdx].m_astUnits[i].m_nUnitSize <= 0 )
			{
				break;
			}
		}


		if( i >= m_astUDPSlots[iTempIdx].m_nUnitCount )
		{
			//if some slot ready, then return it!
			m_stReadySlotIdxSet.add_item(iTempIdx);
			m_stUsedSlotIdxsSet.del_item(iSetIdx);
		}

	}//for(/**/; /**/;)

	return m_stReadySlotIdxSet.last();
}

//从cache得到一个完整的udp包
int32_t CUDPSocket::get_one_code(char* pszBuffer, int32_t iBufferSize, char *pszSrcIP, uint16_t* punSrcPort)
{
	if( m_iSocketfd < 0 || NULL == pszBuffer || NULL == pszSrcIP  || iBufferSize <= 0 || NULL == punSrcPort )
	{
		return -1;
	}

	//若还没有任何准备好的udp包
	if( m_stReadySlotIdxSet.last() <= 0 )
	{
		return 0;
	}

	//
	int32_t i = 0;
	int32_t iTmpidx = -1;
	for( i = 0; i < m_stReadySlotIdxSet.last(); i++ )
	{
		iTmpidx = m_stReadySlotIdxSet[i];
		if( iTmpidx < 0 || iTmpidx >= max_slot_count ||
			!m_bmSlotUsed.is_bit_setted(iTmpidx) )
		{
			m_stReadySlotIdxSet.del_item(i);
			i--;
			continue;
		}

		break;
	}
	if( i >= m_stReadySlotIdxSet.last() )
	{
		return 0;
	}

	//getted !
	int32_t iSetIdx = i;

	//sum size
	int32_t iPackageSize = 0;
	for( i = 0; i < m_astUDPSlots[iTmpidx].m_nUnitCount; i++ )
	{
		iPackageSize += m_astUDPSlots[iTmpidx].m_astUnits[i].m_nUnitSize;
	}
	if( iPackageSize > iBufferSize )
	{
		//overflow, then return error
		return -2;
	}

	//get the package 
	char* pszTmp = pszBuffer;
	iPackageSize = 0;

	for( i = 0; i < m_astUDPSlots[iTmpidx].m_nUnitCount; i++ )
	{
		memcpy( pszTmp,	(const void *)m_astUDPSlots[iTmpidx].m_astUnits[i].m_szUnit,
			(size_t)m_astUDPSlots[iTmpidx].m_astUnits[i].m_nUnitSize );

		iPackageSize += m_astUDPSlots[iTmpidx].m_astUnits[i].m_nUnitSize;
		pszTmp += m_astUDPSlots[iTmpidx].m_astUnits[i].m_nUnitSize;
	}

	//record the source peer
	struct in_addr inAddrSrc;
	inAddrSrc.s_addr = m_astUDPSlots[iTmpidx].m_ulSrcIP;
	strcpy(pszSrcIP, inet_ntoa(inAddrSrc));
	*punSrcPort = ntohs(m_astUDPSlots[iTmpidx].m_unSrcPort);

	//clear the slot
	m_stReadySlotIdxSet.del_item(iSetIdx);

	clear_slot_of(iTmpidx);

	return iPackageSize;
}

//一个udp包可能会分为多片进行发送，每一片的格式如下:
/*
 ******************************************************************
 *packageID   * totalUnit * Unit NO. * Unit						   *
 *******************************************************************
*/
int32_t CUDPSocket::send_one_code(char *pszBuffer, int32_t iBufferSize, char *pszDstIP, uint16_t unDstPort)
{
	if (m_iSocketfd < 0)
	{
		return -1;
	}

	if (NULL == pszBuffer || iBufferSize <= 0 || NULL == pszDstIP)
	{
		return -1;
	}

	struct sockaddr_in stTempSockAddr;
	int32_t iSentBytes = 0;
	
	int32_t iSendLength = 0;
	ssize_t iTempRet = 0;
	short nTmpSeq = 0;
	char *pszTemp = NULL;

	char abyBuf[2*max_unit_size];

	
	memset((void *)&stTempSockAddr, 0, sizeof( stTempSockAddr ) );
	stTempSockAddr.sin_family = AF_INET;
	stTempSockAddr.sin_addr.s_addr = inet_addr(pszDstIP);
	stTempSockAddr.sin_port = htons( unDstPort);

	//总片数
	int32_t iTotalUnit = 0;

	iTotalUnit = iBufferSize/max_unit_size + ((iBufferSize%max_unit_size) ? 1 : 0);

	int32_t iUnitSize = 0;

	//依次发生各个分片
	int32_t i = 0;
	for( i = 0; i < iTotalUnit; ++i)
	{
		iSendLength = 0;
		nTmpSeq = (short)i;
		pszTemp = &abyBuf[0];
		*((u_long *)pszTemp) = htonl(m_unSectionSequence); //打包该udp包的unique id
		pszTemp += sizeof(m_unSectionSequence);
		iSendLength += sizeof(m_unSectionSequence);

		*((short *)pszTemp) = htons((u_short)iTotalUnit);//打包该udp包的分片总数
		pszTemp += sizeof(short);
		iSendLength += sizeof(short);

		*((short *)pszTemp) = htons(nTmpSeq);//打包该udp包的各个分片的序号
		pszTemp += sizeof(short);
		iSendLength += sizeof(short);

		if( i == iTotalUnit - 1 )
		{
			iUnitSize = (iBufferSize -1)%max_unit_size + 1;
		}
		else
		{
			iUnitSize = max_unit_size;
		}
		memcpy((void *)pszTemp, (const void *)(pszBuffer+ i*max_unit_size), size_t(iUnitSize));
		iSendLength += iUnitSize;
		iTempRet = sendto(m_iSocketfd, (const void *)abyBuf, (size_t)iSendLength, 0,
			(const struct sockaddr *)&stTempSockAddr,
			sizeof(stTempSockAddr));

		if( iTempRet < 0 )
		{
			return iTempRet;
		}
		else
		{
			iSentBytes += iTempRet;
		}
	}//for( i = 0; i < iTotalUnit; ++i )

	++m_unSectionSequence;  //单调递增sequence

	return iSentBytes;
}


SERVER_END
