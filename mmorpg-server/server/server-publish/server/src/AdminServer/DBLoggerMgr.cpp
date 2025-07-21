#include "DBLoggerMgr.h"

#include <AvalonDBConnection.h>
#include <AvalonXmlDocument.h>
#include "DBLogThread.h"
#include "DBLogQueryThread.h"
#include "ASApplication.h"

const char* CREATE_LOGGERDB_STATEMENT = "CREATE TABLE IF NOT EXISTS `t_log_%s`("
	"`guid`	BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,"
	"`accid` INT UNSIGNED NOT NULL,"
	"`playerid` BIGINT UNSIGNED NOT NULL,"
	"`playername` VARCHAR(32) NOT NULL,"
	"`timestamp` INT UNSIGNED NOT NULL,"
	"`params` VARCHAR(512) NOT NULL,"
	"PRIMARY KEY(`guid`),"
	"INDEX(`playerid`),"
	"INDEX(`timestamp`)"
	")";

DBLoggerMgr::DBLoggerMgr()
{
	m_Port = 0;
	m_LogThreadNum = 0;
	m_pQueryThread = NULL;
	m_ThreadIndex = 0;
}

DBLoggerMgr::~DBLoggerMgr()
{
}

bool DBLoggerMgr::Init(const std::string& config)
{
	//加载日志数据库配置
	if(!LoadDBConfig()) return false;
	
	Avalon::XmlDocument doc(config);
	if(!doc.LoadFile()) return false;

	Avalon::XmlElement* root = doc.RootElement();
	if(root == NULL) return false;
	Avalon::XmlElement* dblog = doc.FirstChildElement(root, "dblog");
	if(dblog == NULL) return false;

	doc.GetAttribute(dblog, "threadnum", m_LogThreadNum);
	Avalon::XmlElement* loggerNode = doc.FirstChildElement(dblog, "logger");
	while(loggerNode != NULL)
	{
		std::string name;
		UInt32	keeptime = 0;
		doc.GetAttribute(loggerNode, "name", name);
		doc.GetAttribute(loggerNode, "keeptime", keeptime);
		m_Logs.insert(std::make_pair(name, keeptime));

		loggerNode = doc.NextElement(loggerNode, "logger");
	}

	//初始化日志表
	if(!InitLogDB()) return false;

	m_pQueryThread = new DBLogQueryThread(m_Logs);
	if(!m_pQueryThread->Start(m_Host, m_Port, m_User, m_Passwd, m_dbName))
	{
		CL_SAFE_DELETE(m_pQueryThread);
		return false;
	}

	for(UInt32 i = 0; i < m_LogThreadNum; ++i)
	{
		DBLogThread* logThread = new DBLogThread(m_Logs);
		if(!logThread->Start(m_Host, m_Port, m_User, m_Passwd, m_dbName))
		{
			CL_SAFE_DELETE(logThread);
			return false;
		}
		m_LogThreads.push_back(logThread);
	}

	return true;
}

void DBLoggerMgr::Final()
{
	if(m_pQueryThread != NULL)
	{
		m_pQueryThread->Stop();
		CL_SAFE_DELETE(m_pQueryThread);
	}

	for(LogThreadVec::iterator iter = m_LogThreads.begin();
		iter != m_LogThreads.end(); ++iter)
	{
		DBLogThread* logThread = *iter;
		logThread->Stop();
		CL_SAFE_DELETE(logThread);
	}
	m_LogThreads.clear();
}

void DBLoggerMgr::OnTick(const Avalon::Time& now)
{
	for(LogThreadVec::iterator iter = m_LogThreads.begin();
		iter != m_LogThreads.end();)
	{
		DBLogThread* thread = *iter;
		if(!thread->IsDBConnAlive())
		{
			thread->Stop();
			iter = m_LogThreads.erase(iter);
			CL_SAFE_DELETE(thread);
		}
		else
		{
			++iter;
		}
	}

	if(m_LogThreads.empty())
	{
		ASApplication::Instance()->Exit();
	}
}

void DBLoggerMgr::ProcessLogPacket(Avalon::Packet* packet)
{
	if(m_LogThreads.empty())
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	m_ThreadIndex = (m_ThreadIndex + 1) % m_LogThreads.size();
	m_LogThreads[m_ThreadIndex]->PutLogPacket(packet);
}

void DBLoggerMgr::QueryLog(UInt32 connid, const std::string& logname, ObjID_t roleid, UInt32 beginTime, UInt32 endTime, UInt16 page, UInt16 pagesize, UInt32 itemId)
{
	DBLogQueryRequest* request = new DBLogQueryRequest();
	request->connid = connid;
	request->type = logname;
	request->roleid = roleid;
	request->beginTime = beginTime;
	request->endTime = endTime;
	request->page = page;
	request->pagesize = pagesize;
	if(logname.compare("itemobtain") == 0 || logname.compare("itemconsume") == 0)
	{
		request->itemId = itemId;
	}
	m_pQueryThread->PutRequest(request);
}

bool DBLoggerMgr::LoadDBConfig()
{
	std::string dbconfig = ROOT_PATH;
	dbconfig = dbconfig + "Database.xml";
	Avalon::XmlDocument doc(dbconfig);
	if(!doc.LoadFile())
	{
		ErrorLogStream << "load database config:" << dbconfig << " failed!" << LogStream::eos;
		return false;
	}

	Avalon::XmlElement* root = doc.RootElement();
	if(root == NULL) return false;
	Avalon::XmlElement* logdbNode = doc.FirstChildElement(root, "localdb");
	if(logdbNode == NULL) return false;

	doc.GetAttribute(logdbNode, "host", m_Host);
	doc.GetAttribute(logdbNode, "port", m_Port);
	doc.GetAttribute(logdbNode, "user",	m_User);
	doc.GetAttribute(logdbNode, "passwd", m_Passwd);
	doc.GetAttribute(logdbNode, "dbname", m_dbName);

	return true;
}

bool DBLoggerMgr::InitLogDB()
{
	Avalon::DBConnection* dbconn = Avalon::DBConnection::Create("mysql");
	if(!dbconn->Open(m_Host.c_str(), m_Port, m_User.c_str(), m_Passwd.c_str(), m_dbName.c_str()))
	{
		ErrorLogStream << "can not connect to log database!" << LogStream::eos;
		Avalon::DBConnection::Destroy(dbconn);
		dbconn = NULL;
		return false;
	}

	for(LogNameMap::iterator iter = m_Logs.begin();
		iter != m_Logs.end(); ++iter)
	{
		if(dbconn->UpdateFmt(CREATE_LOGGERDB_STATEMENT, iter->first.c_str()) < 0)
		{
			ErrorLogStream << "create log table failed!" << LogStream::eos;
			dbconn->Close();
			Avalon::DBConnection::Destroy(dbconn);
			dbconn = NULL;
			return false;
		}
	}

	dbconn->Close();
	Avalon::DBConnection::Destroy(dbconn);
	dbconn = NULL;
	return true;
}

