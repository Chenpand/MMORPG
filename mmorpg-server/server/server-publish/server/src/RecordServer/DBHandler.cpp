#include "DBHandler.h"
#include "ScriptMgr.h"

string DBHandler::m_localDbHost = "";
UInt32 DBHandler::m_localDbPort = 0;
string DBHandler::m_localDbUser = "";
string DBHandler::m_localDbPasswd = "";
string DBHandler::m_localDbName = "";

DBHandler::DBHandler(const string& host, UInt32 port, const string& user, const string& passwd, const string& db)
: m_host(host), m_passwd(passwd), m_user(user), m_db(db), m_port(port)
{
	m_pConn = DBConnection::Create("mysql");
}

DBHandler::~DBHandler()
{
	if (m_pConn)
	{
		delete m_pConn;
	}
}

DBHandler* DBHandler::CreateDBHandler(const string& host, UInt32 port, const string& user, const string& passwd, const string& db)
{
	return new DBHandler(host, port, user, passwd, db );
}

void DBHandler::CloseDBHandler(DBHandler* dbHandler)
{
	if (dbHandler)
	{
		dbHandler->Close();
		delete dbHandler;
	}
}

bool DBHandler::InitLocalDbAddress(Avalon::PropertySet* properties)
{
	if (properties == NULL)
	{
		return false;
	}
	std::string cfg = properties->GetProperty("Database", "database_config").GetString();
	Avalon::XmlDocument doc(cfg);
	if (!doc.LoadFile()) return false;

	Avalon::XmlElement* root = doc.RootElement();
	if (root == NULL) return false;

	Avalon::XmlElement* localDBNode = doc.FirstChildElement(root, "localdb");

	doc.GetAttribute(localDBNode, "host", DBHandler::m_localDbHost);
	doc.GetAttribute(localDBNode, "port", DBHandler::m_localDbPort);
	doc.GetAttribute(localDBNode, "user", DBHandler::m_localDbUser);
	doc.GetAttribute(localDBNode, "passwd", DBHandler::m_localDbPasswd);
	doc.GetAttribute(localDBNode, "dbname", DBHandler::m_localDbName);
	return true;
}

bool DBHandler::Connect()
{
	if (m_pConn)
	{
		return m_pConn->Open(m_host.c_str(), m_port, m_user.c_str(), m_passwd.c_str(), m_db.c_str());
	}
	return false;
}

void DBHandler::Close()
{
	if (m_pConn)
	{
		m_pConn->Close();
	}
}

bool DBHandler::TryQuery(const char* sql)
{
	bool haveData = false;
	if (!sql)
	{
		return false;
	}
	UInt32 len = strlen(sql);
	if (len == 0)
	{
		return false;
	}
	
	ResultSet* res = m_pConn->Query(sql, len);
	if (!res)
	{
		return false;
	}
	if (res->NextRow())
	{
		haveData = true;
	}
	m_pConn->FreeResultSet(res);
	return haveData;
}

bool DBHandler::Update(const char* sql)
{
	if (!sql)
	{
		return false;
	}
	UInt32 len = strlen(sql);
	if (len == 0)
	{
		return false;
	}
	if (m_pConn->Update(sql, len) != -1)
	{
		return true;
	}
	return false;
}