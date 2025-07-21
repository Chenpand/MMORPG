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
	//����
	bool Connect();
	//�ر�
	void Close();
	//��ѯ���� �����ݷ���true ���򷵻�false
	bool TryQuery(const char* sql);
	//��ɾ,�޸Ĳ��� 
	bool Update(const char* sql);
protected:
private:
	string m_host;
	string m_passwd;
	string m_user;
	string m_db;
	UInt32 m_port;

	//����
	Avalon::DBConnection*	m_pConn;

	//�������ݿ��ַ��Ϣ
	static string m_localDbHost;
	static UInt32 m_localDbPort;
	static string m_localDbUser;
	static string m_localDbPasswd;
	static string m_localDbName;
};
#endif