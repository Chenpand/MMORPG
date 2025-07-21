#include "AvalonDBConnection.h"
#include <cstdarg>
#include "AvalonMysqlConnection.h"


namespace Avalon
{
	DBConnection* DBConnection::Create(const std::string &dbtype)
	{
		if(dbtype == "mysql")
		{
			return new MysqlConnection();
		}

		return new MysqlConnection();
	}

	void DBConnection::Destroy(DBConnection* conn)
	{
		delete conn;
	}

	ResultSet* DBConnection::QueryFmt(const char *sql, ...)
	{
		char buf[5120];

		va_list ap;
		va_start(ap, sql);
		int ret = avalon_vsnprintf(buf, 5100, sql, ap);
		va_end(ap);

		if(ret < 0) return NULL;
		return Query(buf, size_t(ret));
	}

	Int64 DBConnection::UpdateFmt(const char *sql, ...)
	{
		char buf[5120];

		va_list ap;
		va_start(ap, sql);
		int ret = avalon_vsnprintf(buf, 5100, sql, ap);
		va_end(ap);

		if(ret < 0) return -1;
		return Update(buf, size_t(ret));
	}
}
