 

#ifndef _UDPLOG_CONFIG_H_
#define _UDPLOG_CONFIG_H_


#include "udplog_typedef.h"
#include "udplog_singleton.h"

#include <AvalonXmlDocument.h>

#include <map>
#include <vector>
#include <string>

struct stHost
{
	char szIP[MAX_IPADDR_LENGTH];
	Int16 nPort;
};

class CUdpLogConfig
{
public:
	CUdpLogConfig();
	virtual ~CUdpLogConfig();

public:
	Int32 Init(const char* szFileName = DEFAULT_CONFIGFILENAME, const char* szServerFileName = DEFAULT_SERVER_CONFIG_FILE);
	
	//���ȡһ��host���أ�����ֵ < 0 ��ʾ��ȡʧ��
	Int32 GetHostRandomly(stHost& host);
	
	//�����ļ����ֻ�ȡ��Ӧ��ID������ֵ < 0 ��ʾ��ȡʧ��
	Int32 GetLogFileID(const char* typeinfo);

private:

	Int32 InitServerInfo(Avalon::XmlDocument *pDoc, Avalon::XmlElement* pRoot);

	Int32 InitFileTypeInfo(Avalon::XmlDocument *pDoc, Avalon::XmlElement* pRoot);

private:
	std::map<std::string, Int32>	m_mLogFileTypeInfo;
	std::vector<stHost>				m_vHosts;
};

#define	CREATE_UDPLOGCONFIG_INSTANCE	CSingleton<CUdpLogConfig>::CreateInstance
#define	GET_UDPLOGCONFIG_INSTANCE		CSingleton<CUdpLogConfig>::GetInstance
#define	DESTROY_UDPLOGCONFIG_INSTANCE	CSingleton<CUdpLogConfig>::DestroyInstance

#define _config GET_UDPLOGCONFIG_INSTANCE()

#endif
