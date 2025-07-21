#include "CDKeyMgr.h"

#include <sstream>
#include <AvalonStringUtil.h>
#ifdef AVALON_OS_WINDOWS
#include "FakeMemcachedApi.h"
#else
#include <libmemcached/memcached.h>
#endif
#include "OSNetwork.h"

CDKeyMgr::CDKeyMgr()
{
	m_pMemc = NULL;
}

CDKeyMgr::~CDKeyMgr()
{

}

bool CDKeyMgr::Init(Avalon::PropertySet* properties)
{
	if(properties == NULL) return false;
	
	std::string db_ip1 = properties->GetProperty("Database", "cdkeydb_ip1").GetString();
	UInt32		db_port1 = properties->GetProperty("Database", "cdkeydb_port1");

	std::string db_ip2 = properties->GetProperty("Database", "cdkeydb_ip2").GetString();
	UInt32		db_port2 = properties->GetProperty("Database", "cdkeydb_port2");

	m_pMemc = memcached_create(NULL);
	if(m_pMemc == NULL)
	{
		ErrorLogStream << "GiftBagMgr::Init() create memcached_st failed!" << LogStream::eos;
		return false;
	}

	memcached_behavior_set(m_pMemc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);

	memcached_server_list_st servers;
	memcached_return_t result = MEMCACHED_SUCCESS;
	servers = memcached_server_list_append(NULL, db_ip1.c_str(), db_port1, &result);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "GiftBagMgr::Init() append server1 failed!" << memcached_strerror(m_pMemc, result) << LogStream::eos;
		return false;
	}

	servers = memcached_server_list_append(servers, db_ip2.c_str(), db_port2, &result);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "GiftBagMgr::Init() append server2 failed!" << memcached_strerror(m_pMemc, result) << LogStream::eos;
		memcached_server_list_free(servers);
		return false;
	}

	result = memcached_server_push(m_pMemc, servers);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "GiftBagMgr::Init() push servers failed!" << memcached_strerror(m_pMemc, result) << LogStream::eos;
		memcached_server_list_free(servers);
		return false;
	}

	memcached_server_list_free(servers);
	return true;
}

void CDKeyMgr::Final()
{
	memcached_free(m_pMemc);
	m_pMemc = NULL;
}

void CDKeyMgr::OnTick(const Avalon::Time& now)
{
	for(SessionMap::iterator iter = m_Sessions.begin();
		iter != m_Sessions.end();)
	{
		if(now.Sec() > iter->second + EXCHANGE_CDKEY_TIMEOUT)
		{
			m_Sessions.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void CDKeyMgr::SendProtocol(UInt32 connId, Avalon::Protocol& protocol)
{
	Avalon::NetConnectionPtr conn = OSNetwork::Instance()->FindConnection(connId);
	if( conn != NULL )
	{
		CL_SEND_PROTOCOL(conn, protocol);
	}
}

UInt32 CDKeyMgr::BeginExchangeCDKey(const std::string& cdkey)
{
	if(m_Sessions.find(cdkey) != m_Sessions.end()) return 0;

	CDKeyData data;
	if(!GetCDKeyDataByKey(cdkey, data)) return 0;

	if(data.openId.size() > 1) return 0;	//“—¡Ï»°
	
	m_Sessions.insert(std::make_pair(cdkey, UInt32(CURRENT_TIME.Sec())));
	return data.itemId;
}

void CDKeyMgr::EndExchangeCDKey(const std::string& cdkey, const std::string& openid)
{
	SessionMap::iterator iter = m_Sessions.find(cdkey);
	if(iter == m_Sessions.end()) return;

	CDKeyData data;
	if(!GetCDKeyDataByKey(cdkey, data)) return;

	data.openId = openid;
	SetCDKeyDataByKey(data);

	m_Sessions.erase(iter);
}

bool CDKeyMgr::GetCDKeyDataByKey(const std::string& cdkey, CDKeyData& data)
{
	size_t value_length = 0;
	memcached_return_t result;
	char* buf = memcached_get(m_pMemc, cdkey.c_str(), cdkey.length(), &value_length, 0, &result);
	if(buf == NULL) return false;

	std::string str(buf, buf+value_length);
	std::vector<std::string>	tokens;
	Avalon::StringUtil::Split(str, tokens, "|");
	free(buf);
	
	if(tokens.size() < 3) return false;

	data.cdkey = tokens[0];
	data.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(tokens[1]);
	data.openId = tokens[2];

	return true;
}

bool CDKeyMgr::SetCDKeyDataByKey(const CDKeyData& data)
{
	std::ostringstream oss;
	oss << data.cdkey << "|" << data.itemId << "|" << data.openId;

	memcached_return_t result = memcached_set(m_pMemc, data.cdkey.c_str(), data.cdkey.length(), oss.str().c_str(), oss.str().length(), 0, 0);
	if(result != MEMCACHED_SUCCESS)
	{
		ErrorLogStream << "set cdkey data failed: " << memcached_strerror(m_pMemc, result) << LogStream::eos;
		return false;
	}
	return true;
}
