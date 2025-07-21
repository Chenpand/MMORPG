#include "GameParamMgr.h"

#include <AvalonResultSet.h>
#include "CSApplication.h"

GameParamMgr::GameParamMgr()
{
}

GameParamMgr::~GameParamMgr()
{
}

bool GameParamMgr::Init()
{
	Avalon::RetryDBConnection* dbconn = CSApplication::Instance()->GetDBConnection();
	Avalon::ResultSet* resultSet = dbconn->Query("SELECT * FROM `t_g_gameparam`");
	if(resultSet == NULL) return false;

	while(resultSet->NextRow())
	{
		std::string name = resultSet->GetField("name").GetString();
		UInt32 value = resultSet->GetField("value").GetUInt32();
		m_GameParams.insert(std::make_pair(name, value));
	}
	dbconn->FreeResultSet(resultSet);
	return true;
}

void GameParamMgr::Final()
{
}

void GameParamMgr::SetValue(const char* name, UInt32 value)
{
	if(name == NULL) return;

	Avalon::RetryDBConnection* dbconn = CSApplication::Instance()->GetDBConnection();
	GameParamMap::iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end())
	{
		if(iter->second == value) return;

		iter->second = value;

		if(dbconn->UpdateFmt("UPDATE `t_g_gameparam` SET `value`=%u WHERE `name`='%s'", iter->second, name) < 0)
		{
			ErrorLogStream << "execute database statement failed!" << dbconn->GetError() << LogStream::eos;
			CSApplication::Instance()->Exit();
			return;
		}
	}
	else if(value != 0)
	{
		m_GameParams.insert(std::make_pair(name, value));

		if(dbconn->UpdateFmt("INSERT INTO `t_g_gameparam`(`name`,`value`) VALUES('%s',%u)", name, value) < 0)
		{
			ErrorLogStream << "execute database statement failed!" << dbconn->GetError() << LogStream::eos;
			CSApplication::Instance()->Exit();
			return;
		}
	}
}

UInt32 GameParamMgr::GetValue(const char* name) const
{
	GameParamMap::const_iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end()) return iter->second;
	return 0;
}

