#include "TCPlayerMgr.h"
#include "TCPlayer.h"



TCPlayerMgr::TCPlayerMgr()
{

}

TCPlayerMgr::~TCPlayerMgr()
{

}

TCPlayer* TCPlayerMgr::find_player(UInt32 accId)
{
	TCPlayerMap::iterator iter = m_stPlayerMap.find(accId);
	if (iter != m_stPlayerMap.end())
	{
		return iter->second;
	}

	return NULL;
}

bool TCPlayerMgr::add_player(TCPlayer* player)
{
	if (NULL == player)
	{
		return false;
	}

	//
	return (m_stPlayerMap.insert(std::make_pair(player->m_unAccId, player)).second);
}

bool TCPlayerMgr::remove_player(TCPlayer* player)
{
	//
	if (NULL == player)
	{
		return true;
	}

	return remove_player(player->m_unAccId);

}

bool TCPlayerMgr::remove_player(UInt32 accId)
{
	TCPlayerMap::iterator iter = m_stPlayerMap.find(accId);
	if (iter != m_stPlayerMap.end())
	{
		m_stPlayerMap.erase(iter);
	}

	return true;
}

UInt32 TCPlayerMgr::get_player_count()
{
	return m_stPlayerMap.size();
}

void TCPlayerMgr::OnTick(const Avalon::Time& now)
{
	//TODO: check players
}

void TCPlayerMgr::OnQuit()
{
	
	for (TCPlayerMap::iterator iter = m_stPlayerMap.begin(); iter != m_stPlayerMap.end(); ++iter)
	{
		if (iter->second)
		{
			CL_SAFE_DELETE(iter->second);
		}
	}

	m_stPlayerMap.clear();
}
