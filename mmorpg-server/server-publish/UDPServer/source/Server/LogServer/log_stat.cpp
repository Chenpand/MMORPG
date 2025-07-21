#include "log_stat.h"
#include "Common/common_datetime.h"
#include <dirent.h>
#include "log_config.h"
namespace KingNet { namespace Server { namespace LogServer {
	CStatLog::CStatLog()
	{
		m_iMaxSystemLogFileSize = 0x4000000;
		m_iMaxSystemLogFileCount = 5;
		m_pstStatFile = NULL;
		memset(m_szFileName, 0, sizeof(m_szFileName));
		m_iProcessID = -1;
		memset(m_szCurrentFileName,0,sizeof(m_szCurrentFileName));
		m_iCount = 0;
	}

	CStatLog::~CStatLog()
	{
		//do nothing
	}

	int32_t CStatLog::InitLog(int32_t filesize, int32_t filenumber, char* pazFileName)
	{
		m_iMaxSystemLogFileSize = filesize;
		m_iMaxSystemLogFileCount = filenumber;

		memcpy(&m_szFileName[0], pazFileName, strlen(pazFileName)+1);
		
		return 0;
	};

	void CStatLog::SetProcessID(int32_t nProcess)
	{
		m_iProcessID = nProcess;
	}

	int32_t CStatLog::WriteLog( const char* szContent,const char* tablename)
	{

		char szFileName[256];
		bool brt=false;
		char szCurDate[32];
		if (success != get_current_struct_datetime(m_stNowDateTime))
		{
			return fail;
		};
		
		get_current_string_bill((char *)szCurDate);
		sprintf(szFileName, "%s%s_%d_%d_%s.txt", m_szFileName,tablename,LOGSVRCONFIG.m_iServerID,m_iProcessID, szCurDate);
		
		if( 0 != strcasecmp((const char*)m_szCurrentFileName,(const char*)szFileName))
		{
			if(NULL != m_pstStatFile)
			{
				fclose(m_pstStatFile);
				m_pstStatFile = NULL;
			}
			m_pstStatFile = fopen(szFileName, "a+");
			if( m_pstStatFile == NULL)
			{
				return fail;
			}
			memcpy(m_szCurrentFileName,szFileName,sizeof(m_szCurrentFileName));
		}
		if( m_pstStatFile == NULL)
		{
			m_pstStatFile = fopen(szFileName, "a+");
		}
		
		brt=WriteContentToFile(m_pstStatFile,szContent);
		if(false == brt)
		{
			fclose(m_pstStatFile);
			m_pstStatFile = NULL;
		}
		else
		{
			/*if(m_iCount > 100)
			{
				fflush(m_pstStatFile);
				m_iCount = 0;
			}
			else
			{
				++m_iCount;
			}*/
			
			fflush(m_pstStatFile);
		}
		
		
		return success;

	}

	bool CStatLog::WriteContentToFile(FILE * fpStat,const char* szContent)
	{
		if( NULL == fpStat || NULL == szContent )
		{
			return false;
		}
		char szCurrentDateTime[max_date_string_length] = {0};
		get_string_from_struct_datetime(m_stNowDateTime,szCurrentDateTime);

		szCurrentDateTime[max_date_string_length -1] = '\0';
		int32_t i=fprintf(fpStat, "%s|%s\n", szCurrentDateTime,szContent);
		if(i<0) return false;
		return true;
	}



}}}


