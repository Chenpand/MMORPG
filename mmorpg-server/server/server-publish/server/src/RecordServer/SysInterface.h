#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_
#include <CLDefine.h>
#include <string>

using namespace std;
class DBHandler;

class SysInterface
{
public:
	DBHandler*	CreateDBHandler(const char* host, UInt32 port, const char* user, const char* passwd, const char* db);
	void	CloseDBHandler(DBHandler* dbHandler);
	const char*  GetLocalDbHost();
	UInt32  GetLocalDbPort();
	const char*  GetLocalDbUser();
	const char*  GetLocalDbPasswd();
	const char*  GetLocalDbName();

};

extern SysInterface sys;

#endif