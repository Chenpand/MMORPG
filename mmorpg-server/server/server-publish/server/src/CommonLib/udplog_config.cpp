 
#include "udplog_config.h"
#include <cstdio>
#include <ctime>

#define TAGNAME_UDPLOG_SERVER				"udplog_server"
#define TAGNAME_LOGFILE_TYPE				"log_file_type"
#define TAGNAME_ITEM						"item"

#define ATTRIBUTE_ID						"id"
#define ATTRIBUTE_IP						"ip"
#define ATTRIBUTE_PORT						"port"
#define ATTRIBUTE_TYPEINFO					"typeinfo"
#define ATTRIBUTE_PLATFORM					"name"


CUdpLogConfig::CUdpLogConfig()
{
}

CUdpLogConfig::~CUdpLogConfig()
{

}

Int32 CUdpLogConfig::InitServerInfo(Avalon::XmlDocument *pDoc, Avalon::XmlElement* pRoot)
{
	if(NULL == pDoc || NULL == pRoot)
	{
		return UL_FAIL;
	}
	Avalon::XmlElement *pServer = pDoc->FirstChildElement(pRoot, TAGNAME_UDPLOG_SERVER);
	if(NULL == pServer)
	{
		return UL_FAIL;
	}
	
	m_vHosts.clear();

	std::string pszValue;
	Avalon::XmlElement *pItem = pDoc->FirstChildElement(pServer, TAGNAME_ITEM);
	while (NULL != pItem)
	{
		stHost host;
		if (!pDoc->GetAttribute(pItem, std::string(ATTRIBUTE_IP), pszValue))
		{
			return UL_FAIL;
		}
#ifndef AVALON_OS_WINDOWS
		strncpy(host.szIP, pszValue.c_str(), MAX_IPADDR_LENGTH-1);
#else
		strncpy_s(host.szIP, sizeof(host.szIP), pszValue.c_str(), strlen(pszValue.c_str()));
#endif
		Int32 port ;
		if (!pDoc->GetAttribute(pItem, std::string(ATTRIBUTE_PORT), port))
		{
			return UL_FAIL;
		}
		host.nPort = (Int16)port;
		
		m_vHosts.push_back(host);

		pItem = pDoc->NextElement(pItem);
	}
	return UL_OK;
}

Int32 CUdpLogConfig::InitFileTypeInfo(Avalon::XmlDocument *pDoc, Avalon::XmlElement* pRoot)
{
	if(NULL == pDoc || NULL == pRoot)
	{
		return UL_FAIL;
	}

	Avalon::XmlElement	*pServer = pDoc->FirstChildElement(pRoot, TAGNAME_LOGFILE_TYPE);
	if(NULL == pServer)
	{
		return UL_FAIL;
	}
	
	m_mLogFileTypeInfo.clear();

	Avalon::XmlElement	*pItem = pDoc->FirstChildElement(pServer, TAGNAME_ITEM);
	while (NULL != pItem)
	{
		std::string file;
		if (!pDoc->GetAttribute(pItem, ATTRIBUTE_TYPEINFO, file))
		{
			return UL_FAIL;
		}
		
		Int32 id;

		if (!pDoc->GetAttribute(pItem, ATTRIBUTE_ID, id))
		{
			return UL_FAIL;
		}
		m_mLogFileTypeInfo[file] = id;

		pItem = pDoc->NextElement(pItem);
	}
	return UL_OK;
}

Int32 CUdpLogConfig::Init(const char* szFileName/* = DEFAULT_CONFIGFILENAME*/, const char* szServerFileName /* = DEFAULT_SERVER_CONFIG_FILE*/)
{
	srand((UInt32)time(NULL));

	Avalon::XmlDocument doc(szFileName); 
	if (!doc.LoadFile())
	{
		return UL_FAIL;
	}

	Avalon::XmlDocument serverDoc(szServerFileName);
	if (!serverDoc.LoadFile())
	{
		return UL_FAIL;
	}

	//获取根节点
	Avalon::XmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		return UL_FAIL;
	}

	Avalon::XmlElement *pServerRoot = serverDoc.RootElement();
	if (NULL == pServerRoot)
	{
		return UL_FAIL;
	}

	Int32 ret = InitServerInfo(&serverDoc, pServerRoot);
	if(ret < 0)
	{
		return ret;
	}

	ret = InitFileTypeInfo(&doc, pRoot);
	if(ret < 0)
	{
		return ret;
	}

	return UL_OK;
}

Int32 CUdpLogConfig::GetHostRandomly(stHost& host)
{
	Int32 size = (Int32)m_vHosts.size();
	if(0 == size)
	{
		return UL_FAIL;
	}
	
	Int32 index = rand()%size;
	
	host = m_vHosts[index];

	return UL_OK;
}

Int32 CUdpLogConfig::GetLogFileID(const char* typeinfo)
{
	if(NULL == typeinfo)
	{
		return UL_FAIL;
	}
	std::map<std::string, Int32>::iterator it = m_mLogFileTypeInfo.find(typeinfo);
	if(it != m_mLogFileTypeInfo.end())
	{
		return it->second;
	}
	return UL_FAIL;
}

