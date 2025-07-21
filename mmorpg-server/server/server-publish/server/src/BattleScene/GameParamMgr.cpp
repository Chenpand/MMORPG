#include "GameParamMgr.h"
#include "SSRouter.h"

#include <CLSyncObjectProtocol.h>

GameParamMgr::GameParamMgr()
{
}

GameParamMgr::~GameParamMgr()
{
}

void GameParamMgr::SetValue(const std::string& name, UInt32 value, bool isNotifyWorld)
{
	if (name.empty()) return;

	GameParamMap::iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end())
	{
		iter->second = value;
	}
	else if (value != 0)
	{
		m_GameParams.insert(std::make_pair(name, value));
	}
	else
	{
		return;
	}

	// Í¬²½µ½world
	if (isNotifyWorld)
	{
		CLProtocol::SceneGameParamSync sync;
		sync.gameParamMap[name] = value;
		Router::SendToWorld(sync);
	}
}

UInt32 GameParamMgr::GetValue(const std::string& name) const
{
	GameParamMap::const_iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end()) return iter->second;
	return 0;
}

