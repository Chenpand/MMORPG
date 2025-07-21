#include "SysInterface.h"
#include "DBHandler.h"

SysInterface sys;

DBHandler*	SysInterface::CreateDBHandler(const char* host, UInt32 port, const char* user, const char* passwd, const char* db)
{
	if (host == NULL || user == NULL || passwd == NULL || db == NULL)
	{
		return NULL;
	}
	return DBHandler::CreateDBHandler(host, port ,user, passwd, db );
}

void	SysInterface::CloseDBHandler(DBHandler* dbHandler)
{
	if (dbHandler)
	{
		DBHandler::CloseDBHandler(dbHandler);
	}
}

const char*  SysInterface::GetLocalDbHost()
{
	return DBHandler::GetLocalDbHost().c_str();
}
UInt32  SysInterface::GetLocalDbPort()
{
	return DBHandler::GetLocalDbPort();
}
const char*  SysInterface::GetLocalDbUser()
{
	return DBHandler::GetLocalDbUser().c_str();
}
const char*  SysInterface::GetLocalDbPasswd()
{
	return DBHandler::GetLocalDbPasswd().c_str();
}
const char*  SysInterface::GetLocalDbName()
{
	return DBHandler::GetLocalDbName().c_str();
}