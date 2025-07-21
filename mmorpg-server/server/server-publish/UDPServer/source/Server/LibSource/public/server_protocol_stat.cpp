#include "public/server_protocol_stat.h"
#include "Common/common_datetime.h"
#include "gameplatform_message.h"

namespace KingNet { namespace Server { 

	CProtocolInfo::CProtocolInfo()
	{
		m_nMessageID	= 0;
		m_cMessageDirection	= -1;
		m_szName[0]		= '\0';
		m_iCount		= 0;
		m_TotalSize		= 0;
	}

	CProtocolInfo::~CProtocolInfo()
	{
		//do nothing
	}

	void CProtocolInfo::clear()
	{
		m_iCount	= 0;
		m_TotalSize	= 0;
	}

	int protocol_compare(const void* v1, const void* v2)
	{
		CProtocolInfo* p1 = (CProtocolInfo*)v1;
		CProtocolInfo* p2 = (CProtocolInfo*)v2;

		if (p1->m_nMessageID == p2->m_nMessageID)
		{
			return int(p1->m_cMessageDirection - p2->m_cMessageDirection);
		}
		else
		{
			return int(p1->m_nMessageID - p2->m_nMessageID);
		}
	}

	CProtocolStat::CProtocolStat()
	{
		m_nCount = 0;
		m_iDumpInterval = default_dump_stat_info_interval;
		m_iPrintDelay = 0;
		STRNCPY(m_szFileNamePrefix, "protocal_stat", sizeof(m_szFileNamePrefix));
	}

	CProtocolStat::~CProtocolStat()
	{
		//do nothing
	}

	int32_t CProtocolStat::Init(const char* pszFileNamePrefix)
	{
		if ((NULL == pszFileNamePrefix) || (strlen(pszFileNamePrefix) == 0))
		{
			return fail;
		}
		else
		{
			STRNCPY(m_szFileNamePrefix, pszFileNamePrefix, sizeof(m_szFileNamePrefix));
			return success;
		}
	}

	int32_t CProtocolStat::Register(int16_t nMessageID, enmMsgDirection iDirection, const char* pszMessageName)
	{
		if ((m_nCount >= max_protocol_stat_count) || (m_nCount < 0))
		{
			m_nCount = 0;
			return fail;
		}

		CProtocolInfo* pstProtoalInfo = find(nMessageID, iDirection);
		if (NULL == pstProtoalInfo)
		{
			m_stProtoalInfos[m_nCount].m_nMessageID		= nMessageID;
			m_stProtoalInfos[m_nCount].m_cMessageDirection	= (int8_t)iDirection;
			STRNCPY(m_stProtoalInfos[m_nCount].m_szName, pszMessageName, sizeof(m_stProtoalInfos[m_nCount].m_szName));
			m_stProtoalInfos[m_nCount].m_iCount			= 0;
			m_stProtoalInfos[m_nCount].m_TotalSize		= 0;

			++m_nCount;

			sort();
			return success;
		}

		return fail;
	}

	int32_t CProtocolStat::Refresh(int16_t nMessageID, enmMsgDirection iDirection, size_t MessageSize)
	{
		if (MessageSize > max_protocol_body_size)
		{
			return fail;
		}

		CProtocolInfo* pstProtoalInfo = find(nMessageID, iDirection);
		if (NULL != pstProtoalInfo)
		{
			++pstProtoalInfo->m_iCount;
			pstProtoalInfo->m_TotalSize += MessageSize;
			return success;
		}

		return fail;
	}

	CProtocolInfo* CProtocolStat::find(int16_t nMessageID, enmMsgDirection iDirection)
	{
		if ((m_nCount >= max_protocol_stat_count) || (m_nCount < 0))
		{
			m_nCount = 0;
			return NULL;
		}

		void* result = NULL;
		CProtocolInfo stProtoalInfo;
		stProtoalInfo.m_nMessageID   = nMessageID;
		stProtoalInfo.m_cMessageDirection = (int8_t)iDirection;

		result = bsearch((const void*)&stProtoalInfo, (const void*)m_stProtoalInfos, (size_t)m_nCount, sizeof(CProtocolInfo), protocol_compare);

		return (CProtocolInfo*)result;
	}

	int32_t CProtocolStat::sort()
	{
		if ((m_nCount >= max_protocol_stat_count) || (m_nCount < 0))
		{
			return fail;
		}

		qsort((void*)m_stProtoalInfos, (size_t)m_nCount, sizeof(CProtocolInfo), protocol_compare);

		return success;
	}

	void CProtocolStat::Dump(int32_t iInterval)
	{
		m_iPrintDelay -= iInterval;
		if (m_iPrintDelay <= 0)
		{
			m_iPrintDelay = m_iDumpInterval;

			for (int16_t i=0; i<m_nCount; ++i)
			{
				log("[0x%x][%d][%s]COUNT=%d;DATASIZE=%d;\n", 
					m_stProtoalInfos[i].m_nMessageID, 
					m_stProtoalInfos[i].m_cMessageDirection,
					m_stProtoalInfos[i].m_szName, 
					m_stProtoalInfos[i].m_iCount, 
					m_stProtoalInfos[i].m_TotalSize);

				//每次打印完后清理统计的信息
				m_stProtoalInfos[i].clear();
			}
		}
	}

	int32_t CProtocolStat::log(const char* szContent, ...)
	{
		int iResult = success;
		char szFileName[256] = {0};
		char szCurDate[32]	 = {0};

		get_current_string_date((char *)szCurDate);
		snprintf(szFileName, (sizeof(szFileName) - 1), "./log/%s_%s", m_szFileNamePrefix, szCurDate);

		char* title = "PROTO";

		va_list ap;

		va_start(ap, szContent);
		iResult = m_stLogEngine.log(szFileName, (int32_t)max_stat_file_size, (int32_t)max_stat_file_count, title, szContent, ap);
		va_end(ap);

		return iResult;
	};

	void CProtocolStat::SetDumpInterval( int32_t iInterval )
	{
		m_iDumpInterval = iInterval;

		if (m_iDumpInterval < default_dump_stat_info_interval)
		{
			m_iDumpInterval = default_dump_stat_info_interval;
		}
		
	}
}}

