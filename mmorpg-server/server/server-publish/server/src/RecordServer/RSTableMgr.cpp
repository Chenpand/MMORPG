#include "RSTableMgr.h"

#include <AvalonLogger.h>
#include <string>
#include "RSApplication.h"

/**
 *@brief 唯一名验证数据表
 */
const static char* CREATE_UNIQUENAME_TABLE_STATEMENT = "CREATE TABLE IF NOT EXISTS `%s_name`("
	"`name` VARCHAR(32) BINARY NOT NULL,"
	"`" RECORD_ID "` BIGINT	UNSIGNED NOT NULL,"
	"PRIMARY KEY(`name`),"
	"INDEX(`" RECORD_ID "`)"
	");";


RSTableMgr::RSTableMgr()
{
	m_pLocalConn = NULL;
	m_pUniqueNameConn = NULL;

	m_CheckFlushTimer.SetInterval(1000);
	m_LogTimer.SetInterval(5 * 60 * 1000);
}

RSTableMgr::~RSTableMgr()
{
	Final();
}

bool RSTableMgr::Init(Avalon::PropertySet *properties)
{
	if(properties == NULL) return false;

	m_CacheTime = properties->GetProperty("Cache", "cache_time");
	
	std::string cfg = properties->GetProperty("Database", "database_config").GetString();
	Avalon::XmlDocument doc(cfg);
	if(!doc.LoadFile()) return false;

	Avalon::XmlElement* root = doc.RootElement();
	if(root == NULL) return false;
	
	//加载唯一名验证配置
	Avalon::XmlElement* uniqueNameRoot = doc.FirstChildElement(root, "uniquenamedb");
	if(uniqueNameRoot != NULL)
	{
		m_pUniqueNameConn = CreateDBConn(doc, uniqueNameRoot);
		if(m_pUniqueNameConn == NULL){
			ErrorLogStream << "connect to unique name verify database failed!" << LogStream::eos;
			return false;
		}

		Avalon::XmlElement* uniqueNameNode = doc.FirstChildElement(uniqueNameRoot, "uniquename");
		while(uniqueNameNode != NULL)
		{
			std::string tableName;
			std::string columnName;
			doc.GetAttribute(uniqueNameNode, "table", tableName);
			doc.GetAttribute(uniqueNameNode, "namecolumn", columnName);

			if(m_pUniqueNameConn->UpdateFmt(CREATE_UNIQUENAME_TABLE_STATEMENT, tableName.c_str()) < 0)
			{
				ErrorLogStream << "create unique name table " << tableName << "_name failed!" << LogStream::eos;
				return false;
			}

			m_UniqueNames.insert(std::make_pair(tableName, columnName));

			uniqueNameNode = doc.NextElement(uniqueNameNode, "uniquename");
		}
	}

	//加载本地数据库表
	Avalon::XmlElement* localDBNode = doc.FirstChildElement(root, "localdb");
	m_pLocalConn = CreateDBConn(doc, localDBNode);
	if(m_pLocalConn == NULL) return false;

	if(!ShowTables(m_pLocalConn)) return false;

	//加载唯一键值生成种子
	Avalon::ResultSet* guidResult = m_pLocalConn->Query("SELECT * FROM `t_guid`");
	if(guidResult == NULL)
	{
		ErrorLogStream << "can not select guid seeds!" << LogStream::eos;
		return false;
	}
	while(guidResult->NextRow())
	{
		GuidSeed seed;
		seed.nodetype = guidResult->GetField("nodetype").GetUInt32();
		seed.nodeid = guidResult->GetField("nodeid").GetUInt32();
		seed.seed = guidResult->GetField("seed").GetUInt64();
		m_GuidSeeds.push_back(seed);
	}
	m_pLocalConn->FreeResultSet(guidResult);

	return true;
}

void RSTableMgr::Final()
{
	for(TableMap::iterator iter = m_Tables.begin();iter != m_Tables.end();++iter)
	{
		iter->second->Final();
		delete iter->second;
	}
	m_Tables.clear();

	if(m_pLocalConn != NULL)
	{
		m_pLocalConn->Close();
		Avalon::RetryDBConnection::Destroy(m_pLocalConn);
		m_pLocalConn = NULL;
	}

	if(m_pUniqueNameConn != NULL)
	{
		m_pUniqueNameConn->Close();
		Avalon::RetryDBConnection::Destroy(m_pUniqueNameConn);
		m_pUniqueNameConn = NULL;
	}
}

bool RSTableMgr::AddTable(RSTable *table)
{
	if(table == NULL) return false;

	return m_Tables.insert(std::make_pair(table->GetName(),table)).second;
}

RSTable* RSTableMgr::FindTable(const std::string &name)
{
	TableMap::iterator iter = m_Tables.find(name);
	if(iter != m_Tables.end()){
		return iter->second;
	}
	return NULL;
}

bool RSTableMgr::SetGuidSeed(UInt32 nodetype, UInt32 nodeid, UInt64 seed)
{
	if(nodetype == 0 && nodeid == 0) return true;

	bool bFind = false;
	for(GuidSeedVec::iterator iter = m_GuidSeeds.begin();
		iter != m_GuidSeeds.end(); ++iter)
	{
		if(iter->nodetype == nodetype && iter->nodeid == nodeid)
		{
			bFind = true;
			if(seed > iter->seed)
			{
				seed += 100;
				iter->seed = seed;
				break;
			}
			else return true;
		}
	}

	std::ostringstream guidsql;
	if(bFind)
	{
		guidsql << "UPDATE `t_guid` SET `seed`=" << seed << " WHERE `seed` <" << seed << " AND `nodetype`=" << nodetype << " AND `nodeid`=" << nodeid;
	}
	else
	{
		GuidSeed guidSeed;
		guidSeed.nodetype = nodetype;
		guidSeed.nodeid = nodeid;
		guidSeed.seed = seed;
		m_GuidSeeds.push_back(guidSeed);

		guidsql << "INSERT INTO `t_guid`(`nodetype`,`nodeid`,`seed`) VALUES('"
			<< nodetype <<"','" << nodeid << "','" << seed << "') on duplicate key update seed=seed";
	}

	DebugLogStream << guidsql.str() << LogStream::eos;
	return m_pLocalConn->Update(guidsql.str().c_str()) >= 0;
}

UInt64 RSTableMgr::GetGuidSeed(UInt32 nodetype,UInt32 nodeid) const
{
	for(GuidSeedVec::const_iterator iter = m_GuidSeeds.begin();
		iter != m_GuidSeeds.end(); ++iter)
	{
		if(iter->nodetype == nodetype && iter->nodeid == nodeid){
			return iter->seed;
		}
	}
	return 0;
}

void RSTableMgr::Flush()
{
	for(TableMap::iterator iter = m_Tables.begin(); iter != m_Tables.end(); ++iter)
	{
		iter->second->Flush();
	}
}

void RSTableMgr::OnTick(const Avalon::Time &now)
{
	if(!m_CheckFlushTimer.IsTimeout(now)) return;

	bool bLog = m_LogTimer.IsTimeout(now);
	for(TableMap::iterator iter = m_Tables.begin(); iter != m_Tables.end(); ++iter)
	{
		if(!iter->second->OnTick(now, m_CacheTime, bLog))
		{
			return;
		}
	}
}

Avalon::RetryDBConnection* RSTableMgr::CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node)
{
	if(node == NULL) return NULL;

	std::string host;
	UInt32 port = 0;
	std::string user;
	std::string passwd;
	std::string dbname;

	doc.GetAttribute(node, "host", host);
	doc.GetAttribute(node, "port", port);
	doc.GetAttribute(node, "user", user);
	doc.GetAttribute(node, "passwd", passwd);
	doc.GetAttribute(node, "dbname", dbname);

	Avalon::RetryDBConnection* conn = Avalon::RetryDBConnection::Create("mysql", 60, 500);
	if(!conn->Open(host.c_str(), port, user.c_str(), passwd.c_str(), dbname.c_str()))
	{
		ErrorLogStream << "can not connect database " << dbname << "! host="
			<< host << ", port=" << port << "." << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(conn);
		return NULL;
	}

	return conn;
}

bool RSTableMgr::ShowTables(Avalon::RetryDBConnection* dbconn)
{
	if(dbconn == NULL) return false;

	Avalon::ResultSet* result = dbconn->Query("SHOW TABLES");
	if(result == NULL)
	{
		ErrorLogStream << "can not select tables!" << LogStream::eos;
		return false;
	}

	bool ret = true;
	while(result->NextRow())
	{
		std::string tablename = result->GetField(0).GetString();
		RSTable* table = new RSTable();

		std::string uniqueNameCol;
		UniqueNameMap::iterator iter = m_UniqueNames.find(tablename);
		if(iter != m_UniqueNames.end()) uniqueNameCol = iter->second;

		if(!table->Init(tablename, uniqueNameCol))
		{
			ErrorLogStream << "init table " << tablename << " failed!" << LogStream::eos;
			CL_SAFE_DELETE(table);
			ret = false;
			break;
		}

		if(!AddTable(table))
		{
			ErrorLogStream << "duplicate table " << table->GetName() << "!" << LogStream::eos;
			CL_SAFE_DELETE(table);
			ret = false;
			continue;
		}
	}
	dbconn->FreeResultSet(result);
	return ret;
}

