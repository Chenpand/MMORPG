#include "RSTable.h"
#include <AvalonTime.h>
#include <AvalonLogger.h>
#include <AvalonStringUtil.h>

#include <CLRecordProtocol.h>
#include <CLErrorCode.h>

#include "RSTableMgr.h"
#include "RSApplication.h"
#include "RSRouter.h"
#include "RSNetwork.h"

#define CASE_SQL_TO_DATASTREAM(Type) \
			case DBTypeTraits<Type>::ID: \
						{ \
						Type val = result->GetField(column->name.c_str()).Get##Type(); \
						outstream & val; \
						} \
						break;

inline UInt64 GetUSecsNow()
{
#if defined(AVALON_OS_WINDOWS)
	return 0;
#else
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (UInt64)tv.tv_sec * 1000000 + (UInt64)tv.tv_usec;
#endif
}

//删除唯一名语句
const static char* DELETE_UNIQUENAME_STATEMENT = "DELETE FROM `%s_name` WHERE `" RECORD_ID "`= " AVALON_UINT64_FORMAT;


char STRING_BUF[ 64 * 1024 * 2 + 8];

RSTable::RSTable():m_NameIndex(-1)
{
	m_InsertNum = m_UpdateNum = m_DeleteNum = m_SelectNum = 0;
	m_InsertTime = m_UpdateTime = m_DeleteTime = m_SelectTime = 0;
	m_DBThread = NULL;
}

RSTable::~RSTable()
{
	if (m_DBThread)
	{
		m_DBThread->Stop();
	}
}

void RSTable::ConvertToSql(Avalon::RetryDBConnection* conn, std::ostringstream& stream,const CLDatabaseData& data)
{
#define CASE_DBTYPE(Type) \
	case DBTypeTraits<Type>::ID:

	switch(data.GetType())
	{
	CASE_DBTYPE(Int8)
	CASE_DBTYPE(UInt8)
	CASE_DBTYPE(Int16)
	CASE_DBTYPE(UInt16)
	CASE_DBTYPE(Int32)
		{
			stream << Int32(data);
		}
		break;
	CASE_DBTYPE(UInt32)
	CASE_DBTYPE(Int64)
		{
			stream << Int64(data);
		}
		break;
	CASE_DBTYPE(UInt64)
		{
			stream << UInt64(data);
		}
		break;
	CASE_DBTYPE(Real32)
	CASE_DBTYPE(Real64)
		{
			stream << Real64(data);
		}
		break;
	CASE_DBTYPE(std::string)
	CASE_DBTYPE(char*)
		{
			stream << "'";
			size_t size = conn->EscapeString(STRING_BUF, data.GetBlobPtr(), data.GetBlobSize());
			STRING_BUF[size] = 0;
			stream << STRING_BUF;
			stream << "'";
		}
		break;
	}

	return;
}

bool RSTable::Init(const std::string& name, const std::string& uniqueNameCol)
{
	m_Name = name;

	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream sqlstream;
	sqlstream << "SHOW COLUMNS FROM `" << name << "`";
	Avalon::ResultSet* result = dbconn->Query(sqlstream.str());
	if(result == NULL) return false;

	int index = 0;
	char coltype[64];
	while(result->NextRow())
	{
		std::string colname = result->GetField(0).GetString();
		if(colname.compare(RECORD_ID) == 0) continue;

		RSColumn column;
		column.name = colname;

		//获取列类型
		size_t len = result->GetField(1).GetData(coltype,63);
		coltype[len] = 0;
		column.type = GetColumnType(coltype);
		if(column.type == DBT_INVALID) continue;
		column.index = index++;

		//是否唯一名
		if(column.type == DBT_STRING && colname == uniqueNameCol){
			m_NameIndex = column.index;
		}

		m_Columns.insert(std::make_pair(result->GetField(0).GetString(),column));
		m_ColumnVec.push_back(column);
	}
	dbconn->FreeResultSet(result);

#ifdef _NEW_DB_MGR_
	// 创建sql线程
	auto properties = RSApplication::Instance()->GetProperties();
	auto cfg = properties->GetProperty("Database", "database_config").GetString();
	Avalon::XmlDocument doc(cfg);
	if (!doc.LoadFile()) return false;
	Avalon::XmlElement* root = doc.RootElement();
	if (root == NULL) return false;
	Avalon::XmlElement* node = doc.FirstChildElement(root, "localdb");
	if (node == NULL) return false;
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

	m_DBThread = new DBThread();
	if (!m_DBThread->Init(host, port, user, passwd, dbname))
	{
		ErrorLogStream << "table(" << name << ") create thread failed." << LogStream::eos;
		return false;
	}
	m_DBThread->Activate();
#endif

	return true;
}

void RSTable::Final()
{
	Flush();
}

UInt32 RSTable::GenSequence()
{
	static UInt32 s_seed = 0;
	return ++s_seed;
}

const RSColumn* RSTable::FindColumn(const std::string& colname) const
{
	ColumnMap::const_iterator iter = m_Columns.find(colname);
	if(iter != m_Columns.end()){
		return &iter->second;
	}
	return NULL;
}

const RSColumn* RSTable::FindColumn(int index) const
{
	if(index >= (int)m_ColumnVec.size() || index < 0) return NULL;
	return &m_ColumnVec[index];
}

const RSColumn* RSTable::GetUniqueNameColumn() const
{
	if(m_NameIndex < 0) return NULL;
	return FindColumn(m_NameIndex);
}

bool RSTable::UpdateRecordDirect(DBKey_t key, Avalon::NetInputStream& stream)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream updatestream;
	updatestream << "UPDATE `" << m_Name << "` SET ";

	int column_num = 0;
	try
	{
		bool bFirst = true;
		UInt16 colnum = 0;

		stream & colnum;
		if(colnum == 0) return true;

		while(colnum-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if(column == NULL || column->index == m_NameIndex) continue;

			if(bFirst) bFirst = false;
			else updatestream << ",";
			updatestream << "`" << colname << "`=";
			ConvertToSql(dbconn, updatestream, data);
			++column_num;
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::UpdateRecordDirect netexception!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}

	if(column_num == 0) return false;

	updatestream << " WHERE `" RECORD_ID "`=" << key;
	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << updatestream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(updatestream.str());
	++m_UpdateNum;
	m_UpdateTime += GetUSecsNow() - btime;
	if(ret < 0)
	{
		ErrorLogStream << "RSTable::UpdateRecordDirect failed:" << dbconn->GetError() << " statement:" << updatestream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}
	return true;
}

bool RSTable::NewUpdateRecordDirect(UInt32 requestid, UInt32 connid, DBKey_t key, Avalon::NetInputStream& stream)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream updatestream;
	updatestream << "UPDATE `" << m_Name << "` SET ";

	int column_num = 0;
	try
	{
		bool bFirst = true;
		UInt16 colnum = 0;

		stream & colnum;
		if (colnum == 0) return true;

		while (colnum-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if (column == NULL || column->index == m_NameIndex) continue;

			if (bFirst) bFirst = false;
			else updatestream << ",";
			updatestream << "`" << colname << "`=";
			ConvertToSql(dbconn, updatestream, data);
			++column_num;
		}
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::UpdateRecordDirect netexception!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}

	if (column_num == 0) return false;

	updatestream << " WHERE `" RECORD_ID "`=" << key;

	class UpdateCallback : public DBRequestCallback
	{
	public:
		UpdateCallback(UInt32 request, UInt32 conn) : m_requestid(request), m_connid(conn) {}

		virtual void OnSuccess(Avalon::ResultSet* resultSet)
		{
			if (m_requestid > 0)
			{
				CLProtocol::RecordUpdateRet ret;
				ret.requestid = m_requestid;

				Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(m_connid);
				if (conn != NULL)
				{
					CL_SEND_PROTOCOL(conn, ret);
				}
			}
		}

		UInt32 m_requestid;
		UInt32 m_connid;
	};

	UpdateCallback* cb = NULL;
	if (requestid > 0)
	{
		cb = new UpdateCallback(requestid, connid);
	}
	m_DBThread->PushSql(updatestream.str(), SQL_UPDATE, cb);
	return true;
}

bool RSTable::UpdateRecord(DBKey_t key, Avalon::NetInputStream& stream, bool bFlush)
{
	RSRecord *pRecord = NULL;

	RecordMap::iterator recordIter = m_Records.find(key);
	if(bFlush)
	{
		// 如果有缓存，先更新缓存
		if (recordIter != m_Records.end())
		{
			if (!FlushRecord(recordIter->first, recordIter->second))
			{
				ErrorLogStream << "flush table(" << GetName() << ") key(" << recordIter->first << ") failed." << LogStream::eos;
			}

			m_Records.erase(recordIter);
		}
		
		return UpdateRecordDirect(key, stream);
	}
	else if (recordIter != m_Records.end())
	{
		pRecord = &recordIter->second;
	}
	else
	{
		recordIter = m_Records.insert(std::make_pair(key, RSRecord())).first;
		pRecord = &recordIter->second;
		pRecord->SetUpdateTime((UInt32)CURRENT_TIME.Sec());
	}

	try
	{
		UInt16 colnum = 0;
		stream & colnum;

		while(colnum-- > 0)
		{
			std::string colname;
			stream & colname;
			
			const RSColumn* column = FindColumn(colname);
			if(column == NULL || column->index == m_NameIndex) //跳过
			{
				CLDatabaseData data;
				stream & data;
				continue;
			}

			pRecord->PutData(column->index,stream);
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::UpdateRecord1 netexception! tablename=" << GetName() << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}

	if(pRecord->GetDataNum() == 0)
	{
		m_Records.erase(recordIter);
	}
	return true;
}

bool RSTable::UpdateRecord(Avalon::NetInputStream& stream)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream updatestream;
	updatestream << "UPDATE `" << m_Name << "` SET ";
	std::string cond;

	UInt16 colnum = 0;
	int column_num = 0;
	try
	{
		bool bFirst = true;

		stream  & colnum;
		if(colnum == 0) return true;

		while(colnum-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if(column == NULL || column->index == m_NameIndex) continue;

			if(bFirst) bFirst = false;
			else updatestream << ",";
			updatestream << "`" << colname << "`=";
			ConvertToSql(dbconn, updatestream, data);
			++column_num;
		}

		stream & cond;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::UpdateRecord2 netexception!tablename=" << GetName() << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}

	if(column_num == 0) return false;

	if(cond.empty())
	{
		Flush();
	}
	else
	{
		updatestream << " WHERE " << cond;

		std::ostringstream selectstream;
		selectstream << "SELECT `" RECORD_ID "` FROM `" << m_Name << "` WHERE " << cond;
#ifdef __RECORD_SQL__
		DebugLogStream << updatestream.str() << LogStream::eos;
#endif
		Avalon::ResultSet* result = dbconn->Query(selectstream.str());
		if(result == NULL)
		{
			dbconn->FreeResultSet(result);
			ErrorLogStream << "RSTable::UpdateRecord2 select failed:" << dbconn->GetError() << " statement:" << selectstream.str() << "!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return false;
		}
		while(result->NextRow())
		{
			RecordMap::iterator recordIter = m_Records.find(result->GetField(RECORD_ID).GetUInt64());
			if(recordIter != m_Records.end())
			{
				if(!FlushRecord(recordIter->first, recordIter->second))
				{
					dbconn->FreeResultSet(result);
					return false;
				}
			}
		}
		dbconn->FreeResultSet(result);
	}

	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << updatestream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(updatestream.str());
	++m_UpdateNum;
	m_UpdateTime += GetUSecsNow() - btime;
	if(ret < 0)
	{
		ErrorLogStream << "update failed:" << dbconn->GetError() << " statement:" << updatestream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}
	return true;
}

bool RSTable::NewUpdateRecord(UInt32 requestid, UInt32 connid, DBKey_t key, Avalon::NetInputStream& stream, bool bFlush)
{
	return NewUpdateRecordDirect(requestid, connid, key, stream);
}

bool RSTable::NewUpdateRecord(UInt32 requestid, UInt32 connid, Avalon::NetInputStream& stream)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream updatestream;
	updatestream << "UPDATE `" << m_Name << "` SET ";
	std::string cond;

	UInt16 colnum = 0;
	int column_num = 0;
	try
	{
		bool bFirst = true;

		stream  & colnum;
		if (colnum == 0) return true;

		while (colnum-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if (column == NULL || column->index == m_NameIndex) continue;

			if (bFirst) bFirst = false;
			else updatestream << ",";
			updatestream << "`" << colname << "`=";
			ConvertToSql(dbconn, updatestream, data);
			++column_num;
		}

		stream & cond;
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::UpdateRecord2 netexception!tablename=" << GetName() << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}

	if (column_num == 0) return false;

	if (cond.empty())
	{
		Flush();
	}
	else
	{
		updatestream << " WHERE " << cond;

		std::ostringstream selectstream;
		selectstream << "SELECT `" RECORD_ID "` FROM `" << m_Name << "` WHERE " << cond;

		Avalon::ResultSet* result = dbconn->Query(selectstream.str());
		if (result == NULL)
		{
			dbconn->FreeResultSet(result);
			ErrorLogStream << "RSTable::UpdateRecord2 select failed:" << dbconn->GetError() << " statement:" << selectstream.str() << "!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return false;
		}
		while (result->NextRow())
		{
			RecordMap::iterator recordIter = m_Records.find(result->GetField(RECORD_ID).GetUInt64());
			if (recordIter != m_Records.end())
			{
				if (!FlushRecord(recordIter->first, recordIter->second))
				{
					dbconn->FreeResultSet(result);
					return false;
				}
			}
		}
		dbconn->FreeResultSet(result);
	}

	class UpdateCallback : public DBRequestCallback
	{
	public:
		UpdateCallback(UInt32 request, UInt32 conn) : m_requestid(request), m_connid(conn) {}

		virtual void OnSuccess(Avalon::ResultSet* resultSet)
		{
			if (m_requestid > 0)
			{
				CLProtocol::RecordUpdateRet ret;
				ret.requestid = m_requestid;

				Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(m_connid);
				if (conn != NULL)
				{
					CL_SEND_PROTOCOL(conn, ret);
				}
			}
		}

		UInt32 m_requestid;
		UInt32 m_connid;
	};

	UpdateCallback* cb = NULL;
	if (requestid > 0)
	{
		cb = new UpdateCallback(requestid, connid);
	}
	m_DBThread->PushSql(updatestream.str(), SQL_UPDATE, cb);

// 	if (requestid > 0)
// 	{
// 		CLProtocol::RecordUpdateRet ret;
// 		ret.requestid = requestid;
// 		Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(connid);
// 		if (conn != NULL)
// 		{
// 			CL_SEND_PROTOCOL(conn, ret);
// 		}
// 	}

	return true;
}

UInt32 RSTable::InsertRecord(DBKey_t key, Avalon::NetInputStream& stream)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream sqlstream;
	sqlstream << "INSERT INTO `" << m_Name << "`(`" RECORD_ID "`";
	std::ostringstream datastream;
	datastream << " VALUES(" << key;

	try
	{
		UInt16 num = 0;
		stream & num;

		while (num-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if (column == NULL) continue;

			//唯一名列
			if (column->index == m_NameIndex)
			{
				Avalon::RetryDBConnection* uniqueNameDB = RSTableMgr::Instance()->GetUniqueNameDBConn();
				if (uniqueNameDB == NULL) return ErrorCode::RECORD_DUPLICATE_NAME;

				std::ostringstream uniqueNameSql;
				uniqueNameSql << "INSERT INTO `" << GetName() << "_name`(`name`,`" RECORD_ID "`) VALUES(";
				ConvertToSql(uniqueNameDB, uniqueNameSql, data);
				uniqueNameSql << "," << key << ")";

				if (uniqueNameDB->Update(uniqueNameSql.str(), RSApplication::Instance()->IsCheckMysqlErrNo()) < 0)
				{
					return ErrorCode::RECORD_DUPLICATE_NAME;
				}
			}

			sqlstream << ",`" << colname << "`";
			datastream << ",";
			ConvertToSql(dbconn, datastream, data);
		}
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::InsertRecord netexception!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return ErrorCode::RECORD_ERROR;
	}

	sqlstream << ")" << datastream.str() << ")";

	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(sqlstream.str());
	++m_InsertNum;
	m_InsertTime += GetUSecsNow() - btime;
	if (ret < 0)
	{
		ErrorLogStream << "insert failed:" << dbconn->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return ErrorCode::RECORD_ERROR;
	}

	return ErrorCode::SUCCESS;
}

class InsertCallback : public DBRequestCallback
{
public:
	InsertCallback(UInt32 requestid_, UInt32 connid_) : requestid(requestid_), connid(connid_) {}
	virtual void OnSuccess(Avalon::ResultSet* resultSet)
	{
		if (requestid > 0)
		{
			CLProtocol::RecordInsertRet ret;
			ret.requestid = requestid;
			ret.result = ErrorCode::SUCCESS;

			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(connid);
			if (conn != NULL)
			{
				CL_SEND_PROTOCOL(conn, ret);
			}

		}
	}

	UInt32 requestid;
	UInt32 connid;
};

UInt32 RSTable::NewInsertRecord(UInt32 requestid, UInt32 connId, DBKey_t key, Avalon::NetInputStream& stream)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream sqlstream;
	sqlstream << "INSERT INTO `" << m_Name << "`(`" RECORD_ID "`";
	std::ostringstream datastream;
	datastream << " VALUES(" << key;

	try
	{
		UInt16 num = 0;
		stream & num;

		while (num-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if (column == NULL) continue;

			//唯一名列
			if (column->index == m_NameIndex)
			{
				Avalon::RetryDBConnection* uniqueNameDB = RSTableMgr::Instance()->GetUniqueNameDBConn();
				if (uniqueNameDB == NULL) return ErrorCode::RECORD_DUPLICATE_NAME;

				std::ostringstream uniqueNameSql;
				uniqueNameSql << "INSERT INTO `" << GetName() << "_name`(`name`,`" RECORD_ID "`) VALUES(";
				ConvertToSql(uniqueNameDB, uniqueNameSql, data);
				uniqueNameSql << "," << key << ")";

				if (uniqueNameDB->Update(uniqueNameSql.str(), RSApplication::Instance()->IsCheckMysqlErrNo()) < 0)
				{
					return ErrorCode::RECORD_DUPLICATE_NAME;
				}
			}

			sqlstream << ",`" << colname << "`";
			datastream << ",";
			ConvertToSql(dbconn, datastream, data);
		}
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::InsertRecord netexception!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return ErrorCode::RECORD_ERROR;
	}

	sqlstream << ")" << datastream.str() << ")";

	InsertCallback* cb = new InsertCallback(requestid, connId);
	m_DBThread->PushSql(sqlstream.str(), SQL_INSERT, cb);

	return ErrorCode::SUCCESS;
}

bool RSTable::SaveGuidSeed(DBKey_t key)
{
	UInt32 nodetype = CL_GET_NODETYPE_FROM_GUID(key);
	UInt32 nodeid = CL_GET_NODEID_FROM_GUID(key);
	UInt64 seed = CL_GET_SEED_FROM_GUID(key);

	if(!RSTableMgr::Instance()->SetGuidSeed(nodetype, nodeid, seed))
	{
		ErrorLogStream << "RSTable::InsertRecord invalid guid(" << nodetype 
			<< ")(" << nodeid << ")(" << seed << "):" << RSTableMgr::Instance()->GetLocalDBConn()->GetError() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}
	return true;
}

UInt32 RSTable::InsertAutoGuidRecord(Avalon::NetInputStream& stream, UInt32 requestid, UInt32 connId)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream sqlstream;
	std::ostringstream datastream;

	try
	{
		UInt16 num = 0;
		stream & num;

		while (num-- > 0)
		{
			std::string colname;
			CLDatabaseData data;

			stream & colname & data;

			const RSColumn* column = FindColumn(colname);
			if (column == NULL) continue;

			if (!sqlstream.str().empty())
			{
				sqlstream << ",";
			}

			if (!datastream.str().empty())
			{
				datastream << ",";
			}

			sqlstream << "`" << colname << "`";
			ConvertToSql(dbconn, datastream, data);
		}
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::InsertAutoGuidRecord netexception!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return ErrorCode::RECORD_ERROR;
	}

	std::ostringstream ultiSqlOss;
	ultiSqlOss << "INSERT INTO `" << m_Name << "`(" << sqlstream.str() << ") VALUES(" << datastream.str() << ")";

	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << ultiSqlOss.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(ultiSqlOss.str());
	++m_InsertNum;
	m_InsertTime += GetUSecsNow() - btime;
	if (ret < 0)
	{
		ErrorLogStream << "insert failed:" << dbconn->GetError() << " statement:" << ultiSqlOss.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return ErrorCode::RECORD_ERROR;
	}

	return ErrorCode::SUCCESS;
}

bool RSTable::DeleteRecord(DBKey_t key)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::ostringstream sqlstream;
	sqlstream << "DELETE FROM `" << m_Name << "` WHERE `" RECORD_ID "`=" << key;
	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(sqlstream.str());
	++m_DeleteNum;
	m_DeleteTime += GetUSecsNow() - btime;
	if(ret < 0)
	{
		ErrorLogStream << "delete failed:" << dbconn->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}
	m_Records.erase(key);

	//删除唯一名
	if(GetUniqueNameColumn() != NULL)
	{
		Avalon::RetryDBConnection* uniqueNameDB = RSTableMgr::Instance()->GetUniqueNameDBConn();

		if(uniqueNameDB->UpdateFmt(DELETE_UNIQUENAME_STATEMENT, m_Name.c_str(), key) < 0)
		{
			ErrorLogStream << "delete unique name failed:" << uniqueNameDB->GetError() << "! table=" << m_Name
				<< ", guid=" << key << "." << LogStream::eos;
			return false;
		}
	}

	return true;
}

bool RSTable::DeleteRecord(const std::string& cond)
{
	Flush();

	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();
	std::vector<DBKey_t> uniqueIds;
	
	const RSColumn* uniqueNameCol = GetUniqueNameColumn();
	if(uniqueNameCol != NULL) //有唯一名列
	{
		//唯一名值不会修改，故直接从数据库获取。
		std::ostringstream sqlstream;
		sqlstream << "SELECT `" RECORD_ID "` FROM `" << m_Name << "`";
		if(!cond.empty())
		{
			sqlstream << " WHERE " << cond;
		}
		
#ifdef __RECORD_SQL__
		DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
		Avalon::ResultSet* result = dbconn->Query(sqlstream.str());
		if(result == NULL)
		{
			dbconn->FreeResultSet(result);
			ErrorLogStream << "select uniquename failed:" << dbconn->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return false;
		}
		while(result->NextRow())
		{
			uniqueIds.push_back(result->GetField(RECORD_ID).GetUInt64());
		}
		dbconn->FreeResultSet(result);
		if(uniqueIds.empty()) return true;
	}

	std::ostringstream sqlstream;
	sqlstream << "DELETE FROM `" << m_Name << "`";
	if(!cond.empty())
	{
		sqlstream << " WHERE " << cond;
	}
	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(sqlstream.str());
	++m_DeleteNum;
	m_DeleteTime += GetUSecsNow() - btime;
	if(ret < 0)
	{
		ErrorLogStream << "delete failed:" << dbconn->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}

	//删除唯一名
	if(!uniqueIds.empty())
	{

		Avalon::RetryDBConnection* uniqueNameDB = RSTableMgr::Instance()->GetUniqueNameDBConn();

		for(std::vector<DBKey_t>::iterator iter = uniqueIds.begin();
			iter != uniqueIds.end(); ++iter)
		{
			if(uniqueNameDB->UpdateFmt(DELETE_UNIQUENAME_STATEMENT, m_Name.c_str(), *iter) < 0)
			{
				ErrorLogStream << "delete unique name failed:" << uniqueNameDB->GetError() << "! table=" << m_Name
					<< ", guid=" << (*iter) << "." << LogStream::eos;
				return false;
			}
		}
	}

	return true;
}

void RSTable::SelectRecord(UInt32 srcZoneId, UInt32 srcNodeId, UInt32 requestid, DBKey_t key, Avalon::NetInputStream& stream, Avalon::NetConnectionPtr conn)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();
	std::vector<const RSColumn*> columnList;

	std::ostringstream sqlstream;
	sqlstream << "SELECT ";

	RSColumn globalColumn;
	try
	{
		UInt16 colnum = 0;
		stream & colnum;
		
		if(colnum == 0)
		{
			sqlstream << "*";

			//所有列
			for(std::vector<RSColumn>::iterator iter = m_ColumnVec.begin();
				iter != m_ColumnVec.end(); ++iter){
				columnList.push_back(&(*iter));
			}
		}
		else
		{
			sqlstream << "`" RECORD_ID "`";

			while(colnum-- > 0)
			{
				std::string colname;
				stream & colname;

				const RSColumn* column = FindColumn(colname);
				if(column == NULL)
				{
					if(colname.compare("count(*)") == 0 || colname.find("sum(") != std::string::npos)
					{
						globalColumn.index = -1;
						globalColumn.name = colname;
						globalColumn.type = DBTypeTraits<UInt32>::ID;
						columnList.push_back(&globalColumn);

						sqlstream << "," << colname;
					}
					else
					{
						ErrorLogStream << "RSTable::SelectRecord unknow column " << colname << " for table " << GetName() << "!" << LogStream::eos;
						RSApplication::Instance()->Exit();
						return;
					}
				}
				else
				{
					columnList.push_back(column);

					sqlstream << ",`" << colname << "`";
				}
			}
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::SelectRecord netexception 1!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	sqlstream << " FROM	`" << m_Name << "`";

	if(key != 0)
	{
		sqlstream << " WHERE `" RECORD_ID "`=" << key;
	}
	else
	{
		try
		{
			UInt16 condnum = 0;
			stream & condnum;

			bool bFirst = true;
			while(condnum -- > 0)
			{
				if(bFirst){
					sqlstream << " WHERE ";
					bFirst = false;
				}
				else
				{
					sqlstream << " AND ";
				}

				std::string condname;
				CLDatabaseData data;
                std::string op;

				stream & condname & data & op;

                if (data.GetType() == DBT_STRING && op.empty())
                {
                    sqlstream << "BINARY `" << condname << "` LIKE ";
                }
                else if (op == "in")
                {
                    size_t size = dbconn->EscapeString(STRING_BUF, data.GetBlobPtr(), data.GetBlobSize());
                    STRING_BUF[size] = 0;
                    sqlstream << "`" << condname << "` " << op << " " << STRING_BUF;
                    continue;
                }
                else
                {
                    sqlstream << "`" << condname << "` " << op;
                }

				ConvertToSql(dbconn, sqlstream, data);
			}

			
            std::string order;
			stream & order;

            if (!order.empty())
            {
                sqlstream << " " << order;
            }

            std::string limit;
            stream & limit;
			sqlstream << " " << limit;
		}
		catch(const Avalon::NetConnection&)
		{
			ErrorLogStream << "RSTable::SelectRecord netexception 2!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return;
		}
	}

	std::string sql = sqlstream.str();
	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << "request(" << requestid << ") " << sqlstream.str() << LogStream::eos;
#endif
	Avalon::ResultSet* result = dbconn->Query(sql);
	if(result == NULL)
	{
		ErrorLogStream  << "select failed:" << dbconn->GetError() << " statement:" << sql << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}
	++m_SelectNum;
	m_SelectTime += GetUSecsNow() - btime;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::RECORD_SELECT_RET);
	//先发送列信息
	try
	{	
		Avalon::NetOutputStream outstream(packet->GetBuffer(), packet->GetMaxSize());
		
		outstream & requestid & UInt8(1) &  UInt16(columnList.size());	//1	表示是列信息
		for(std::vector<const RSColumn*>::iterator iter = columnList.begin()
			;iter != columnList.end(); ++iter)
		{
			outstream & (*iter)->name;
			outstream & UInt8((*iter)->type);
		}

		packet->SetSize(UInt16(outstream.GetPos()));
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::SelectRecord netexception 3!" << LogStream::eos;
		dbconn->FreeResultSet(result);
		Avalon::Packet::Destroy(packet);
		RSApplication::Instance()->Exit();
		return;
	}

	SendPacket(requestid, conn, packet, srcZoneId, srcNodeId);
	packet->SetSize(0);

	try
	{
		while(result->NextRow())
		{
			Avalon::NetOutputStream outstream(packet->GetBuffer() + packet->GetSize(),
				packet->GetMaxSize() - packet->GetSize());
			if(packet->GetSize() == 0)
			{
				outstream & requestid & UInt8(2);	//2表示行信息
			}

			DBKey_t key = result->GetField(RECORD_ID).GetUInt64();
			outstream & key;

			RSRecord* pRecord = NULL;
			RecordMap::iterator recordIter = m_Records.find(key);
			if(recordIter != m_Records.end())
			{
				pRecord = &recordIter->second;
			}

			for(std::vector<const RSColumn*>::iterator iter = columnList.begin()
				;iter != columnList.end();++iter)
			{
				const RSColumn* column = *iter;
				if(pRecord != NULL)
				{
					CLDatabaseData* pData = pRecord->FindData(column->index);
					if(pData != NULL)
					{
						pData->OutputValue(column->type, outstream);
						continue;
					}
				}

#define CASE_SQL_TO_DATASTREAM(Type) \
			case DBTypeTraits<Type>::ID: \
				{ \
					Type val = result->GetField(column->name.c_str()).Get##Type(); \
					outstream & val; \
				} \
				break;

				switch(column->type)
				{
				CASE_SQL_TO_DATASTREAM(Int8)
				CASE_SQL_TO_DATASTREAM(UInt8)
				CASE_SQL_TO_DATASTREAM(Int16)
				CASE_SQL_TO_DATASTREAM(UInt16)
				CASE_SQL_TO_DATASTREAM(Int32)
				CASE_SQL_TO_DATASTREAM(UInt32)
				CASE_SQL_TO_DATASTREAM(Int64)
				CASE_SQL_TO_DATASTREAM(UInt64)
				CASE_SQL_TO_DATASTREAM(Real32)
				CASE_SQL_TO_DATASTREAM(Real64)
				case DBTypeTraits<std::string>::ID:
				case DBTypeTraits<char*>::ID:
					{
						size_t len = result->GetField(column->name.c_str()).GetData(STRING_BUF, 64 * 1024);
						outstream & UInt16(len);
						outstream.SeriaArray(STRING_BUF, len);
					}
					break;
				};
			}

			packet->SetSize(packet->GetSize() + UInt16(outstream.GetPos()));
			if(packet->GetSize() > packet->GetMaxSize() / 3)
			{
				SendPacket(requestid, conn, packet, srcZoneId, srcNodeId);
				packet->SetSize(0);
			}
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::SelectRecord netexception 4!" << LogStream::eos;
		dbconn->FreeResultSet(result);
		Avalon::Packet::Destroy( packet);
		RSApplication::Instance()->Exit();
		return;
	}

	if(packet->GetSize() > 0)
	{
		SendPacket(requestid, conn, packet, srcZoneId, srcNodeId);
		packet->SetSize(0);
	}
	dbconn->FreeResultSet(result);

	try
	{
		Avalon::NetOutputStream outstream(packet->GetBuffer(), packet->GetMaxSize());
		outstream & requestid & UInt8(0);
		packet->SetSize(UInt16(outstream.GetPos()));
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::SelectRecord netexception 5!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		RSApplication::Instance()->Exit();
		return;
	}

	SendPacket(requestid, conn, packet, srcZoneId, srcNodeId);
	Avalon::Packet::Destroy(packet);
}

class SelectCallback : public DBRequestCallback
{
public:
	SelectCallback(RSTable::RecordMap&	records) : m_Records(records) {}

	virtual void OnSuccess(Avalon::ResultSet* result)
	{
		//DebugLogStream << "execute sql finish:" << GetSql() << LogStream::eos;
		auto conn = RSNetwork::Instance()->FindConnection(connid);
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::RECORD_SELECT_RET);
		//先发送列信息
		try
		{
			Avalon::NetOutputStream outstream(packet->GetBuffer(), packet->GetMaxSize());

			outstream & requestid & UInt8(1) &  UInt16(columnList.size());	//1	表示是列信息
			for (std::vector<const RSColumn*>::iterator iter = columnList.begin()
				; iter != columnList.end(); ++iter)
			{
				outstream & (*iter)->name;
				outstream & UInt8((*iter)->type);
			}

			packet->SetSize(UInt16(outstream.GetPos()));
		}
		catch (const Avalon::NetException&)
		{
			ErrorLogStream << "RSTable::SelectRecord netexception 3!" << LogStream::eos;
			dbconn->FreeResultSet(result);
			Avalon::Packet::Destroy(packet);
			RSApplication::Instance()->Exit();
			return;
		}

		if (conn != NULL)
		{
			conn->SendPacket(packet);
		}

		packet->SetSize(0);

		try
		{
			while (result->NextRow())
			{
				Avalon::NetOutputStream outstream(packet->GetBuffer() + packet->GetSize(),
					packet->GetMaxSize() - packet->GetSize());
				if (packet->GetSize() == 0)
				{
					outstream & requestid & UInt8(2);	//2表示行信息
				}

				DBKey_t key = result->GetField(RECORD_ID).GetUInt64();
				outstream & key;

				RSRecord* pRecord = NULL;
				auto recordIter = m_Records.find(key);
				if (recordIter != m_Records.end())
				{
					pRecord = &recordIter->second;
				}

				for (std::vector<const RSColumn*>::iterator iter = columnList.begin()
					; iter != columnList.end(); ++iter)
				{
					const RSColumn* column = *iter;
					if (pRecord != NULL)
					{
						CLDatabaseData* pData = pRecord->FindData(column->index);
						if (pData != NULL)
						{
							pData->OutputValue(column->type, outstream);
							continue;
						}
					}

					switch (column->type)
					{
						CASE_SQL_TO_DATASTREAM(Int8)
							CASE_SQL_TO_DATASTREAM(UInt8)
							CASE_SQL_TO_DATASTREAM(Int16)
							CASE_SQL_TO_DATASTREAM(UInt16)
							CASE_SQL_TO_DATASTREAM(Int32)
							CASE_SQL_TO_DATASTREAM(UInt32)
							CASE_SQL_TO_DATASTREAM(Int64)
							CASE_SQL_TO_DATASTREAM(UInt64)
							CASE_SQL_TO_DATASTREAM(Real32)
							CASE_SQL_TO_DATASTREAM(Real64)
					case DBTypeTraits<std::string>::ID:
					case DBTypeTraits<char*>::ID:
					{
													size_t len = result->GetField(column->name.c_str()).GetData(STRING_BUF, 64 * 1024);
													outstream & UInt16(len);
													outstream.SeriaArray(STRING_BUF, len);
					}
						break;
					};
				}

				packet->SetSize(packet->GetSize() + UInt16(outstream.GetPos()));
				if (packet->GetSize() > packet->GetMaxSize() / 3)
				{
					if (conn != NULL)
					{
						conn->SendPacket(packet);
					}

					packet->SetSize(0);
				}
			}
		}
		catch (const Avalon::NetException&)
		{
			ErrorLogStream << "RSTable::SelectRecord netexception 4!" << LogStream::eos;
			dbconn->FreeResultSet(result);
			Avalon::Packet::Destroy(packet);
			RSApplication::Instance()->Exit();
			return;
		}

		if (packet->GetSize() > 0)
		{
			if (conn != NULL)
			{
				conn->SendPacket(packet);
			}

			packet->SetSize(0);
		}

		try
		{
			Avalon::NetOutputStream outstream(packet->GetBuffer(), packet->GetMaxSize());
			outstream & requestid & UInt8(0);
			packet->SetSize(UInt16(outstream.GetPos()));
		}
		catch (const Avalon::NetException&)
		{
			ErrorLogStream << "RSTable::SelectRecord netexception 5!" << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			RSApplication::Instance()->Exit();
			return;
		}

		if (conn != NULL)
		{
			conn->SendPacket(packet);
		}

		Avalon::Packet::Destroy(packet);
	}

	virtual void OnFailure()
	{

	}

	std::string sql;
	UInt32 requestid;
	std::vector<const RSColumn*> columnList;
	UInt32 connid;
	Avalon::RetryDBConnection* dbconn;
	RSTable::RecordMap&	m_Records;
};

void RSTable::SelectRecordNew(UInt32 requestid, DBKey_t key, Avalon::NetInputStream& stream, Avalon::NetConnectionPtr conn)
{
	Avalon::RetryDBConnection* dbconn = RSTableMgr::Instance()->GetLocalDBConn();
	std::vector<const RSColumn*> columnList;

	std::ostringstream sqlstream;
	sqlstream << "SELECT ";

	RSColumn globalColumn;
	try
	{
		UInt16 colnum = 0;
		stream & colnum;

		if (colnum == 0)
		{
			sqlstream << "*";

			//所有列
			for (std::vector<RSColumn>::iterator iter = m_ColumnVec.begin();
				iter != m_ColumnVec.end(); ++iter){
				columnList.push_back(&(*iter));
			}
		}
		else
		{
			sqlstream << "`" RECORD_ID "`";

			while (colnum-- > 0)
			{
				std::string colname;
				stream & colname;

				const RSColumn* column = FindColumn(colname);
				if (column == NULL)
				{
					if (colname.compare("count(*)") == 0)
					{
						globalColumn.index = -1;
						globalColumn.name = "count(*)";
						globalColumn.type = DBTypeTraits<UInt32>::ID;
						columnList.push_back(&globalColumn);

						sqlstream << "," << colname;
					}
					else
					{
						ErrorLogStream << "RSTable::SelectRecord unknow column " << colname << " for table " << GetName() << "!" << LogStream::eos;
						RSApplication::Instance()->Exit();
						return;
					}
				}
				else
				{
					columnList.push_back(column);

					sqlstream << ",`" << colname << "`";
				}
			}
		}
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "RSTable::SelectRecord netexception 1!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	sqlstream << " FROM	`" << m_Name << "`";

	if (key != 0)
	{
		sqlstream << " WHERE `" RECORD_ID "`=" << key;
	}
	else
	{
		try
		{
			UInt16 condnum = 0;
			stream & condnum;

			bool bFirst = true;
			while (condnum-- > 0)
			{
				if (bFirst){
					sqlstream << " WHERE ";
					bFirst = false;
				}
				else
				{
					sqlstream << " AND ";
				}

				std::string condname;
				CLDatabaseData data;
				std::string op;

				stream & condname & data & op;

				if (data.GetType() == DBT_STRING && op.empty())
				{
					sqlstream << "BINARY `" << condname << "` LIKE ";
				}
				else if (op == "in")
				{
					size_t size = dbconn->EscapeString(STRING_BUF, data.GetBlobPtr(), data.GetBlobSize());
					STRING_BUF[size] = 0;
					sqlstream << "`" << condname << "` " << op << " " << STRING_BUF;
					continue;
				}
				else
				{
					sqlstream << "`" << condname << "` " << op;
				}

				ConvertToSql(dbconn, sqlstream, data);
			}


			std::string order;
			stream & order;

			if (!order.empty())
			{
				sqlstream << " " << order;
			}

			std::string limit;
			stream & limit;
			sqlstream << " " << limit;
		}
		catch (const Avalon::NetConnection&)
		{
			ErrorLogStream << "RSTable::SelectRecord netexception 2!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return;
		}
	}

	std::string sql = sqlstream.str();

	auto cb = new SelectCallback(m_Records);
	cb->columnList = columnList;
	cb->connid = conn->GetID();
	cb->dbconn = dbconn;
	cb->requestid = requestid;
	cb->sql = sql;
	m_DBThread->PushSql(sql, SQL_SELECT, cb);
}

UInt32 RSTable::ChangeName(DBKey_t key, const std::string& name, UInt8 isInsert)
{
	if(m_NameIndex < 0) return ErrorCode::RECORD_NO_NAMECOLUMN;

	const RSColumn* nameColumn = FindColumn(m_NameIndex);
	if(nameColumn == NULL) return ErrorCode::RECORD_NO_NAMECOLUMN;

	Avalon::RetryDBConnection* uniqueNameDB = RSTableMgr::Instance()->GetUniqueNameDBConn();
	Avalon::RetryDBConnection* localdb = RSTableMgr::Instance()->GetLocalDBConn();

	if(uniqueNameDB == NULL || localdb == NULL) return ErrorCode::RECORD_DUPLICATE_NAME;

	//需求改为改名后原来的名字保留给该玩家,别人无法使用,故把UPDATE改为INSERT
// 	std::ostringstream updateNameSql;
// 	updateNameSql << "UPDATE `" << GetName() << "_name` SET name='";
// 	size_t size = uniqueNameDB->EscapeString(STRING_BUF, name.c_str(), name.length());
// 	STRING_BUF[size] = 0;
// 	updateNameSql << STRING_BUF;
// 	updateNameSql << "' WHERE `" RECORD_ID "`=" << key;
// 	if(uniqueNameDB->Update(updateNameSql.str()) < 0)
// 	{
// 		return ErrorCode::RECORD_DUPLICATE_NAME;
// 	}

	size_t size = uniqueNameDB->EscapeString(STRING_BUF, name.c_str(), name.length());
	STRING_BUF[size] = 0;

	if (isInsert > 0)
	{
		std::ostringstream insertNameSql;
		insertNameSql << "INSERT INTO `" << GetName() << "_name`(`name`,`" RECORD_ID "`,`time`) VALUES('" << STRING_BUF << "'," << key << "," << Avalon::Time::CurrentTime().Sec() << ")";
#ifdef __RECORD_SQL__
		DebugLogStream << insertNameSql.str() << LogStream::eos;
#endif
		if (uniqueNameDB->Update(insertNameSql.str(), RSApplication::Instance()->IsCheckMysqlErrNo()) < 0)
		{
			ErrorLogStream << " ChangeName failed! sql:" << insertNameSql.str() << LogStream::eos;
			return ErrorCode::RECORD_DUPLICATE_NAME;
		}
	}
	
	//更新t_player_info表
	std::ostringstream sqlstream;
	sqlstream << "UPDATE `" << GetName() << "` SET `" << nameColumn->name << "`='" << STRING_BUF << "' WHERE `" RECORD_ID "`=" << key;
#ifdef __RECORD_SQL__
	DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
	if(localdb->Update(sqlstream.str()) < 0)
	{
		ErrorLogStream << "RSTable::ChangeName failed:" << localdb->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return ErrorCode::RECORD_ERROR;
	}

	return ErrorCode::SUCCESS;
}

bool RSTable::OnTick(const Avalon::Time& now,UInt32 cachetime, bool bLog)
{
	if(bLog)
	{
		InfoLogStream << "TABLE " << m_Name << " OPERATOR TIMES(" << " SELECT:(" << m_SelectNum << "," << (m_SelectTime != 0 ? m_SelectTime / m_SelectNum : 0) 
			<< ")INSERT:(" << m_InsertNum << "," << (m_InsertTime != 0 ? m_InsertTime / m_InsertNum : 0)
			<< ")UPDATE:(" << m_UpdateNum << "," << (m_UpdateTime != 0 ? m_UpdateTime / m_UpdateNum : 0) 
			<< ")DELETE:(" << m_DeleteNum << "," <<  (m_DeleteTime != 0 ? m_DeleteTime / m_DeleteNum : 0) <<  ")." << LogStream::eos; 
		m_SelectNum = m_InsertNum = m_UpdateNum = m_DeleteNum = 0;
		m_SelectTime = m_InsertTime = m_UpdateTime = m_DeleteTime = 0;
	}

	UInt32 mergeSize = RSApplication::Instance()->GetMergeFlushSize();
	if (!RSApplication::Instance()->IsTableOpenBatchExecute(m_Name))
	{
		mergeSize = 0;
	}

	std::vector<std::pair<DBKey_t, RSRecord*>> records;
	std::vector<DBKey_t> ids;
	for(RecordMap::iterator iter = m_Records.begin();iter != m_Records.end();)
	{
		if(now.Sec() > iter->second.GetUpdateTime() + cachetime)
		{
			if (mergeSize > 0)
			{
				records.push_back(std::make_pair(iter->first, &(iter->second)));
				ids.push_back(iter->first);
				iter++;
			}
			else
			{
				if (!FlushRecord(iter->first, iter->second)) return false;
				m_Records.erase(iter++);
			}
		}
		else ++iter;
	}

	if (mergeSize > 0)
	{
		BatchFlushRecord(records);

		for (auto id : ids)
		{
			m_Records.erase(id);
		}
	}

	return true;
}

void RSTable::Flush()
{
	UInt32 mergeSize = RSApplication::Instance()->GetMergeFlushSize();
	if (!RSApplication::Instance()->IsTableOpenBatchExecute(m_Name))
	{
		mergeSize = 0;
	}

	if (mergeSize > 0)
	{
		std::vector<std::pair<DBKey_t, RSRecord*>> records;
		for (RecordMap::iterator iter = m_Records.begin(); iter != m_Records.end();
			++iter)
		{
			records.push_back(std::make_pair(iter->first, &(iter->second)));
		}

		BatchFlushRecord(records);
		m_Records.clear();
	}
	else
	{
		for (RecordMap::iterator iter = m_Records.begin(); iter != m_Records.end();
			++iter)
		{
			FlushRecord(iter->first, iter->second);
		}
		m_Records.clear();
	}
}

DBDataType RSTable::GetColumnType(const std::string& typestr)
{
	std::vector<std::string> tokens;
	Avalon::StringUtil::Split(typestr,tokens,"() ");

	if(tokens.size() == 0) return DBT_INVALID;

	bool bUnsigned = false;
	if(tokens.size() > 1)
	{
		const std::string& signstr = *(--tokens.end());
		if(signstr.compare("unsigned") == 0) bUnsigned = true;
	}

	std::string tname = tokens[0];
	if(tname.compare("tinyint") == 0)
	{
		return bUnsigned ? DBT_UINT8 : DBT_INT8;
	}
	else if(tname.compare("smallint") == 0)
	{
		return bUnsigned ? DBT_UINT16 : DBT_INT16;
	}
	else if(tname.compare("int") == 0)
	{
		return bUnsigned ? DBT_UINT32 : DBT_INT32;
	}
	else if(tname.compare("bigint") == 0)
	{
		return bUnsigned ? DBT_UINT64 : DBT_INT64;
	}
	else if(tname.compare("float") == 0)
	{
		return DBT_REAL32;
	}
	else if(tname.compare("double") == 0)
	{
		return DBT_REAL64;
	}
	else if(tname.compare("blob") == 0)
	{
		return DBT_BINARY;
	}
	else
	{
		return DBT_STRING;
	}
}

bool RSTable::FlushRecord(DBKey_t key, RSRecord& record)
{
	Avalon::RetryDBConnection *dbconn = RSTableMgr::Instance()->GetLocalDBConn();
	std::ostringstream sqlstream;
	sqlstream << "UPDATE `" << m_Name << "` SET ";

	bool bFirst = true;
	for(RSRecord::DataIterator iter = record.begin();
		iter != record.end(); ++iter)
	{
		const RSColumn* pColumn = FindColumn(iter->first);
		if(pColumn == NULL) continue;

		if(!bFirst)  sqlstream << ",";
		else bFirst = false;

		sqlstream << "`" << pColumn->name << "`=";
		ConvertToSql(dbconn, sqlstream, iter->second);
	}

	sqlstream << " WHERE `" RECORD_ID "`=" << key;
	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(sqlstream.str());
	++m_UpdateNum;
	m_UpdateTime += GetUSecsNow() - btime;
	if(ret < 0)
	{
		ErrorLogStream << "flush record failed:" << dbconn->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}
	return true;
}

bool RSTable::BatchFlushRecord(const std::vector<std::pair<DBKey_t, RSRecord*>>& records)
{
	if (records.empty())
	{
		return true;
	}

	UInt32 mergeSize = RSApplication::Instance()->GetMergeFlushSize();
	if (mergeSize == 0)
	{
		return false;
	}

	Avalon::RetryDBConnection *dbconn = RSTableMgr::Instance()->GetLocalDBConn();

	std::map<std::string, std::string> columnSql;
	std::ostringstream sqlstream;
	std::ostringstream whereStream;

	UInt32 currentSize = 0;

	for (auto recordIter : records)
	{
		if (currentSize == 0)
		{
			sqlstream.str("");
			sqlstream << "UPDATE `" << m_Name << "` SET ";

			whereStream.str("");
			whereStream << " WHERE `" RECORD_ID "` IN(";

			columnSql.clear();
		}

		DBKey_t key = recordIter.first;
		RSRecord* record = recordIter.second;
		
		for (RSRecord::DataIterator iter = record->begin(); iter != record->end(); ++iter)
		{
			const RSColumn* pColumn = FindColumn(iter->first);
			if (pColumn == NULL) continue;

			std::map<std::string, std::string>::iterator columnIter = columnSql.find(pColumn->name);

			std::ostringstream columnStream;
			if (columnIter == columnSql.end())
			{
				columnStream << "`" << pColumn->name << "`= CASE `" RECORD_ID "`";
				columnIter = columnSql.insert(std::make_pair(pColumn->name, columnStream.str())).first;
			}
			else
			{
				columnStream << columnIter->second;
			}

			columnStream << " WHEN " << key << " THEN ";
			ConvertToSql(dbconn, columnStream, iter->second);
			columnIter->second = columnStream.str();
		}

		if (currentSize != 0)
		{
			whereStream << ",";
		}

		whereStream << key;

		currentSize++;

		if (currentSize >= mergeSize)
		{
			whereStream << ");";
			BatchFlushSql(sqlstream, whereStream, columnSql);
			currentSize = 0;
		}
	}
	if (currentSize > 0)
	{
		whereStream << ");";
		BatchFlushSql(sqlstream, whereStream, columnSql);
	}
	
	return true;
}

bool RSTable::BatchFlushSql(const std::ostringstream& sql, const std::ostringstream& whereSql,
	const std::map<std::string, std::string>& columnSqls)
{
	Avalon::RetryDBConnection *dbconn = RSTableMgr::Instance()->GetLocalDBConn();
	std::ostringstream sqlstream;
	sqlstream << sql.str();
	std::map<std::string, std::string>::const_iterator columnIter = columnSqls.begin();
	bool isFirst = true;
	while (columnIter != columnSqls.end())
	{
		if (!isFirst)
		{
			sqlstream << ",";
		}
		else
		{
			isFirst = false;
		}
		sqlstream << columnIter->second << " ELSE `" << columnIter->first << "` END";
		++columnIter;
	}
	sqlstream << whereSql.str();

	UInt64 btime = GetUSecsNow();
#ifdef __RECORD_SQL__
	DebugLogStream << sqlstream.str() << LogStream::eos;
#endif
	Int64 ret = dbconn->Update(sqlstream.str());
	++m_UpdateNum;
	m_UpdateTime += GetUSecsNow() - btime;
	if (ret < 0)
	{
		ErrorLogStream << "flush record failed:" << dbconn->GetError() << " statement:" << sqlstream.str() << "!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return false;
	}
	return true;
}

void RSTable::SendPacket(UInt32 requestid, Avalon::NetConnectionPtr conn, Avalon::Packet* packet, UInt32 srcZoneId, UInt32 srcNodeId)
{
	// 如果是备用服务器，把返回转发给原服务器
	if (SERVER_TYPE == ST_RECORD_SLAVER)
	{
		DebugLogStream << "requestid(" << requestid << ") trans to node(" << srcNodeId << ")." << LogStream::eos;
		Router::TransResponseFromSlaver(srcNodeId, packet);
		return;
	}

	if (srcZoneId == 0)
	{
		if (conn != NULL)
		{
			packet->SetSequence(GenSequence());
			DebugLogStream << "requestid(" << requestid << ") table(" << GetName() << ") send row info sequence(" << packet->GetSequence() << ")." << LogStream::eos;
			conn->SendPacket(packet);
		}
		else
		{
			ErrorLogStream << "requestid(" << requestid << ") connection is null." << LogStream::eos;
		}
	}
	else
	{
		DebugLogStream << "requestid(" << requestid << ") trans to zone(" << srcZoneId << ")." << LogStream::eos;
		Router::TransResponse(srcZoneId, srcNodeId, packet);
	}
}


