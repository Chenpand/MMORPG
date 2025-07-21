#ifndef __COMMON_LOGENGINE_H__
#define __COMMON_LOGENGINE_H__


#include "gameplatform_types.h"
#include "Common/common_base.h"
#include "Common/common_string.h"
#include "common_namespace_definition.h"

SERVER_BEGIN

//���1Byte��ϵͳ���������룬����Ӧ�ý����Զ������־��Ϣ���Ӧ��ʹ�ø�3Byte����
enum enmBasicLogMaskBit
{
	log_mask_none				= int32_t(0x00000000),				//
	log_mask_system_error		= int32_t(0x00000001),				//
	log_mask_system_warning		= int32_t(0x00000002),				//
	log_mask_routine_call		= int32_t(0x00000004),				//��������
	log_mask_system_config		= int32_t(0x00000008),				//������Ϣ
	log_mask_system_performance = int32_t(0x00000010),				//ϵͳ����ͳ����Ϣ
	log_mask_detail				= int32_t(0x00000020),				//��ϸ��Ϣ
	log_mask_package			= int32_t(0x00000040),				//�����Լ���Ӧ�Ķ��������ݰ�

};

enum 
{
	max_log_level_title_len = 16,
};

struct stLogTitleEntry 
{
	int32_t m_iLogLevel;
	char m_szTitle[max_log_level_title_len];
};



class CLogEngine
{
public:
	enum
	{
		max_dump_info_size = 0x400, //
		max_dump_line_size = 0x10,  //
	};

	CLogEngine(int32_t filesize, int32_t filenumber, int32_t logmask, char* logfile):m_iMaxLogFileSize(filesize),
		m_iMaxLogFileNumber(filenumber),m_iLogMask(logmask)
	{
		if (NULL != logfile)
		{
			STRNCPY(m_szFileName, logfile, sizeof(m_szFileName));
		}
	}

	CLogEngine():m_iMaxLogFileSize(0),
		m_iMaxLogFileNumber(0),m_iLogMask(0)
	{
		m_szFileName[0] = '\0';
	}

	virtual ~CLogEngine()
	{
		//do nothing
	}

	int32_t initialize(int32_t filesize, int32_t filenumber, int32_t logmask, char* logfile);


	/*
	* @method:    whether_do_log ������־mask����Ƿ�����Ҫlog����Ϣ
	* @fullname:  Game51::Server::CLogEngine::whether_do_log
	* @access:    public 
	* @param: int32_t mask -- ��Ҫlog����Ϣ��mask
	* @return:   bool
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	bool whether_do_log(int32_t mask);

	void set_logmask(int32_t mask);

	/*
	* @method:    log ��¼��־��Ϣ
	* @fullname:  Game51::Server::CLogEngine::log
	* @access:    public 
	* @param: char * pszFileName
	* @param: int32_t iMaxFileSize
	* @param: int32_t iMaxFileNumber
	* @param: const char * pszLogInfo
	* @param: va_list vaList
	* @return:   Game51::int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t log(char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber, char* title, const char* pszLogInfo, va_list vaList);

	int32_t log(char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber, char* title, const char* pszLogInfo, ...);

	/*
	* @method:    log ����־��Ϣд��Ĭ���ļ�
	* @fullname:  Game51::Server::CLogEngine::log
	* @access:    public 
	* @param: const char * pszLogInfo
	* @param: va_list vaList
	* @return:   Game51::int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t log( char* title, const char* pszLogInfo, va_list vaList);

	int32_t log( char* title, const char* pszLogInfo, ...);

	/*
	* @method:    dumpbin ��¼��������Ϣ��־
	* @fullname:  Game51::Server::CLogEngine::dumpbin
	* @access:    public 
	* @param: char * pszFileName
	* @param: const char * pszBuffer
	* @param: int32_t iSize
	* @return:   Game51::int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t dumpbin(char* pszFileName, const char* pszBuffer, int32_t iSize,const char* pTitle=NULL);

	/*
	* @method:    shiftfile �����ļ���ת
	* @fullname:  Game51::Server::CLogEngine::shiftfile
	* @access:    public 
	* @param: char * pszFileName
	* @param: int32_t iMaxFileSize
	* @param: int32_t iMaxFileNumber
	* @return:   Game51::int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t shiftfile(char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber);

	//
	char* get_log_level_title(int32_t logLevel); 

private:
	char    m_szFileName[max_file_name_length];
	int32_t m_iMaxLogFileSize;
	int32_t m_iMaxLogFileNumber;

	int32_t m_iLogMask;

protected:
private:
};

SERVER_END

#endif //__COMMON_LOGENGINE_H__
