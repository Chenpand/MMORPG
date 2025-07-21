#ifndef _DB_HANDLER_H_
#define _DB_HANDLER_H_

#include <CLDefine.h>
#include <string>
#include <AvalonPropertySet.h>
#include "AvalonDBConnection.h"

using namespace Avalon;
using namespace std;

class DBHandler
{
public:
	DBHandler(const string& host, UInt32 port, const string& user, const string& passwd, const string& db );
	~DBHandler();
public:
	static DBHandler* CreateDBHandler(const string& host, UInt32 port, const string& user, const string& passwd, const string& db);
	static void CloseDBHandler(DBHandler* dbHandler);

	static bool InitLocalDbAddress(Avalon::PropertySet* properties);
	static string& GetLocalDbHost() { return DBHandler::m_localDbHost; };
	static UInt32 GetLocalDbPort() { return DBHandler::m_localDbPort; };
	static string& GetLocalDbUser() { return DBHandler::m_localDbUser; };
	static string& GetLocalDbPasswd() { return DBHandler::m_localDbPasswd; };
	static string& GetLocalDbName() { return DBHandler::m_localDbName; };
public:
	//连接
	bool Connect();
	//关闭
	void Close();
	//查询操作 有数据返回true 否则返回false
	bool TryQuery(const char* sql);
	//增删,修改操作 
	bool Update(const char* sql);
protected:
private:
	string m_host;
	string m_passwd;
	string m_user;
	string m_db;
	UInt32 m_port;

	//连接
	Avalon::DBConnection*	m_pConn;

	//本地数据库地址信息
	static string m_localDbHost;
	static UInt32 m_localDbPort;
	static string m_localDbUser;
	static string m_localDbPasswd;
	static string m_localDbName;
};
#endif