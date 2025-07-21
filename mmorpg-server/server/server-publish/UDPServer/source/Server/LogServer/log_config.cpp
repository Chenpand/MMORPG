#include "Common/common_string.h"
#include "Common/common_socket.h"
#include "tinyxml/xmlconf.h"
#include "tinyxml/tinyxml.h"
#include "log_config.h"
#include "log_log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


namespace KingNet { namespace Server { namespace LogServer {

//
const char* g_pszConfigFile = "./config/log_server.xml";

CLogServerConfig::CLogServerConfig()
{
	initialize();
}

CLogServerConfig::~CLogServerConfig()
{
	//donothing
}

int32_t CLogServerConfig::initialize()
{
	m_iServerID = -1;

	m_iCoreKey = 0;
	m_iCSShareMemoryKey = 0;
	m_iSCShareMemoryKey = 0;

	m_iLogMask = -1;

	m_iUDPSCShareMemoryKey = 0;
	m_iUDPCSShareMemoryKey = 0;
	m_iStatThreadCount = 0;
	memset(m_pszTableXml,0,sizeof(m_pszTableXml));
	for(int32_t i=0;i<max_table_count;++i)
	{
		memset(m_szTableName[i],0,sizeof(m_szTableName));
	}
	return success;
}

int32_t CLogServerConfig::load_config( const char* pszConfigFile )
{
	if (NULL == pszConfigFile)
	{
		return fail;
	}
	TiXmlDocument config(g_pszConfigFile); 
	if (!config.LoadFile(TIXML_ENCODING_UTF8))
	{
		TRACESVR(log_mask_system_error,"[CLogSvrConfig::%s]config file %s encoding is not utf8\n",__FUNCTION__,g_pszConfigFile);
		return fail;
	}

	TiXmlHandle handle(&config);

	TiXmlElement* pstElement =	handle.FirstChild("logsvrd").ToElement();

	if (NULL == pstElement)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No Xml Element logsvrd\n",__FUNCTION__);
		return fail;
	}

	int32_t value = 0;
	const char* pszvalue = NULL;
	//load log_level
	pszvalue = pstElement->Attribute("log_level", &value);

	if (NULL == pszvalue)
	{
		m_iLogMask = -1;		//log all
	}
	else
	{
		m_iLogMask = (int32_t)value;
	}
	SETTRACELEVEL(m_iLogMask);
	TRACESVR(log_mask_system_config, "[CLogServerConfig::%s] log_level = %d\n", __FUNCTION__, m_iLogMask);
	//load server id
	pszvalue = pstElement->Attribute("id" , &value);
	if (NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No id ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iServerID = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s]id = %d\n",__FUNCTION__,m_iServerID);
	}

	//load core_key
	pszvalue = pstElement->Attribute("core_key" , &value);
	if ( NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No core_key ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iCoreKey = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] core_key = %d\n",__FUNCTION__,m_iCoreKey);
	}

	//load cs_channel_key
	pszvalue = pstElement->Attribute("cs_channel_key" , &value);
	if ( NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No cs_channel_key ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iCSShareMemoryKey = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] cs_channel_key = %d\n",__FUNCTION__,m_iCSShareMemoryKey);
	}

	//load sc_channel_key
	pszvalue = pstElement->Attribute("sc_channel_key",&value);
	if ( NULL == pszvalue )
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No sc_channel_key ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iSCShareMemoryKey = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] sc_channel_key = %d\n",__FUNCTION__,m_iSCShareMemoryKey);
	}

	//load udp_cs_channel_key
	pszvalue = pstElement->Attribute("udp_cs_channel_key" , &value);
	if ( NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No udp_cs_channel_key ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iUDPCSShareMemoryKey = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] cs_channel_key = %d\n",__FUNCTION__,m_iUDPCSShareMemoryKey);
	}

	//load udp_sc_channel_key
	pszvalue = pstElement->Attribute("udp_sc_channel_key",&value);
	if ( NULL == pszvalue )
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No udp_sc_channel_key ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iUDPSCShareMemoryKey = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] udp_sc_channel_key = %d\n",__FUNCTION__,m_iUDPSCShareMemoryKey);
	}

	//load work thread number
	pszvalue = pstElement->Attribute("stat_thread_count", &value);
	if(NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No stat_thread_count ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iStatThreadCount = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] stat_thread_count = %d\n",__FUNCTION__,m_iStatThreadCount);
	}

	pszvalue = pstElement->Attribute("max_message_body_queue_size", &value);
	if(NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No max_message_body_queue_size ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		//config file field max_message_body_queue_size  is MB
		m_iMaxMessageBodyQueueSize = (int32_t)value * 1024 * 1024;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] max_message_body_queue_size = %d\n",__FUNCTION__,m_iMaxMessageBodyQueueSize);
	}

	pszvalue = pstElement->Attribute("max_message_entry_count", &value);
	if(NULL == pszvalue)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No max_message_entry_count ,exit\n",__FUNCTION__);
		return fail;
	}
	else
	{
		m_iMaxMessageEntryCount = (int32_t)value;
		TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] max_message_entry_count = %d\n",__FUNCTION__,m_iMaxMessageEntryCount);
	}

	pszvalue = pstElement->Attribute("log_table");
	if ( NULL == pszvalue )
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No admin_uids ,read fail\n",__FUNCTION__);
	}
	else
	{
		STRNCPY(m_pszTableXml,pszvalue,sizeof(m_pszTableXml));
		if( success == read_table_xml((const char*)m_pszTableXml))
		{
			TRACESVR(log_mask_system_config,"[CLogServerConfig::%s]read read_table_xml = %s success\n",__FUNCTION__,m_pszTableXml);
		}
		else
		{
			TRACESVR(log_mask_system_config,"[CLogServerConfig::%s]read read_table_xml = %s error\n",__FUNCTION__,m_pszTableXml);
		}
	}
	
	TRACESVR(log_mask_system_config,"[CLogServerConfig::%s] load config %s successful\n",__FUNCTION__,g_pszConfigFile);
	return success;
}

int32_t CLogServerConfig::reload_config( const char* pszConfigFile )
{
	if (NULL == pszConfigFile)
	{
		return fail;
	}
	TiXmlDocument config(g_pszConfigFile); 
	if (!config.LoadFile(TIXML_ENCODING_UTF8))
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]reload config file %s encoding is not utf8\n",__FUNCTION__,g_pszConfigFile);
		return fail;
	}

	TiXmlHandle handle(&config);

	TiXmlElement* pstElement =	handle.FirstChild("logsvrd").ToElement();

	if (NULL == pstElement)
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No Xml Element logsvrd\n",__FUNCTION__);
		return fail;
	}

	int32_t value = 0;
	const char* pszvalue = NULL;
	pszvalue = pstElement->Attribute("log_level", &value);

	if (NULL == pszvalue)
	{
		m_iLogMask = -1;		//log all
	}
	else
	{
		m_iLogMask = (int32_t)value;
	}
	SETTRACELEVEL(m_iLogMask);

	TRACESVR(log_mask_system_config, "[CLogServerConfig::%s]reset log_level = %d\n", __FUNCTION__, m_iLogMask);

	pszvalue = pstElement->Attribute("log_table");
	if ( NULL == pszvalue )
	{
		TRACESVR(log_mask_system_error,"[CLogServerConfig::%s]No log_table ,read fail\n",__FUNCTION__);
	}
	else
	{
		STRNCPY(m_pszTableXml,pszvalue,sizeof(m_pszTableXml));
		if( success == read_table_xml((const char*)m_pszTableXml))
		{
			TRACESVR(log_mask_system_config,"[CLogServerConfig::%s]reset read_table_xml = %s success\n",__FUNCTION__,m_pszTableXml);
		}
		else
		{
			TRACESVR(log_mask_system_config,"[CLogServerConfig::%s]reset read_table_xml = %s error\n",__FUNCTION__,m_pszTableXml);
		}
	}
	TRACESVR(log_mask_system_config,"[CLogServerConfig::%s]reload config success !\n",__FUNCTION__);
	return success;
}
int32_t CLogServerConfig::reload_config()
{
	return reload_config(g_pszConfigFile);
}
int32_t CLogServerConfig::load_config()
{
	return load_config(g_pszConfigFile);
}
int32_t CLogServerConfig::read_table_xml(const char* pFile)
{
	if (NULL == pFile)
	{
		return fail;
	}

	TiXmlDocument config(pFile); 
	if (!config.LoadFile(TIXML_ENCODING_UTF8))
	{
		TRACESVR(log_mask_system_error, "[CLogServerConfig::%s] load table xml from (%s) failed!\n", __FUNCTION__, pFile);
		return fail;
	}

	TiXmlHandle handle(&config);

	//TiXmlElement* one_table = handle.FirstChild("tables").FirstChild("table").ToElement();
	TiXmlElement* one_table = handle.FirstChild("config").FirstChild("log_file_type").FirstChild("item").ToElement();
	if (NULL == one_table)
	{
		//TRACESVR(log_mask_system_error, "[CLogServerConfig::%s] load tables resource from (%s), but have none of [table] section!\n", __FUNCTION__, pFile);
		TRACESVR(log_mask_system_error, "[CLogServerConfig::%s] load tables resource from (%s), but have none of [log_file_type] or [item] section!\n", __FUNCTION__, pFile);
		return fail;
	}

	int value = 0;
	const char* pszvalue = NULL;

	memset(m_pszTableXml,0,sizeof(m_pszTableXml));
	int32_t tableid = 0;
	while (NULL != one_table)
	{
	
		pszvalue = one_table->Attribute("id", &value);
		if (NULL == pszvalue)
		{
			TRACESVR(log_mask_system_error, "[CLogServerConfig::%s] load tables resource from (%s), but have not attribute [id]!\n", 
				__FUNCTION__, pFile);
			return fail;
		}
		tableid = (int32_t)value;
		
		TRACESVR(log_mask_system_config, "[CLogServerConfig::%s] tableid=[%X]\n", __FUNCTION__, (int32_t)value);

		if (tableid >= int32_t(sizeof(m_szTableName)/sizeof(m_szTableName[0])))
		{
			//so many tables ,just break;
			break;
		}
		
		//pszvalue = one_table->Attribute("name");
		pszvalue = one_table->Attribute("typeinfo");
		if (NULL == pszvalue)
		{
			//TRACESVR(log_mask_system_error, "[one_table::%s] load tables resource from (%s), but table(%d) have not attribute [name]!\n", __FUNCTION__, pFile, tableid);
			TRACESVR(log_mask_system_error, "[one_table::%s] load tables resource from (%s), but table(%d) have not attribute [typeinfo]!\n", __FUNCTION__, pFile, tableid);
			return fail;
		}
		STRNCPY(m_szTableName[tableid], pszvalue, sizeof(m_szTableName[tableid]));

		//one_table = one_table->NextSiblingElement("table");
		one_table = one_table->NextSiblingElement("item");
	}
	return success;
}

int32_t CLogServerConfig::get_table_name(int32_t id,char* pOutName,int32_t inLen)
{
	if( (size_t)id >= sizeof(m_szTableName)/sizeof(m_szTableName[0]))
	{
		return fail;
	}
	STRNCPY(pOutName,m_szTableName[id],inLen);
	return success;
	
}

}}}

