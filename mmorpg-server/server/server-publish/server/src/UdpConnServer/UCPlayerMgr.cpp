#include "UCPlayerMgr.h"
#include "UCPlayer.h"
#include "UCApplication.h"
#include "enet/enet.h"

UCPlayerMgr::UCPlayerMgr()
{
	m_ReportPingTimer.SetInterval(Avalon::Time::MSECS_OF_SEC);
	m_ReportDelayTimer.SetInterval(10 * Avalon::Time::MSECS_OF_SEC);
}

UCPlayerMgr::~UCPlayerMgr()
{

}

UCPlayer* UCPlayerMgr::find_player(UInt32 accId)
{
	UCPlayerMap::iterator iter = m_stPlayerMap.find(accId);
	if (iter != m_stPlayerMap.end())
	{
		return iter->second;
	}

	return NULL;
}

bool UCPlayerMgr::add_player(UCPlayer* player)
{
	if (NULL == player)
	{
		return false;
	}

	//
	return (m_stPlayerMap.insert(std::make_pair(player->m_unAccId, player)).second);
}

bool UCPlayerMgr::remove_player(UCPlayer* player)
{
	//
	if (NULL == player)
	{
		return true;
	}

	return remove_player(player->m_unAccId);

}

bool UCPlayerMgr::remove_player(UInt32 accId)
{
	UCPlayerMap::iterator iter = m_stPlayerMap.find(accId);
	if (iter != m_stPlayerMap.end())
	{
		m_stPlayerMap.erase(iter);
	}

	return true;
}

UInt32 UCPlayerMgr::get_player_count()
{
	return m_stPlayerMap.size();
}

void UCPlayerMgr::OnTick(const Avalon::Time& now)
{
	if (m_ReportPingTimer.IsTimeout(now))
	{
		auto itr = m_stPlayerMap.begin();
		auto next = m_stPlayerMap.begin();
		for (; itr != m_stPlayerMap.end(); itr = next)
		{
			next++;
			UCApplication::Instance()->NotifyPing(itr->second);
		}
		
	}

	if (m_ReportDelayTimer.IsTimeout(now))
	{
		auto itr = m_stPlayerMap.begin();
		auto next = m_stPlayerMap.begin();
		for (; itr != m_stPlayerMap.end(); itr = next)
		{
			next++;
			auto player = itr->second;
			if (player && player->IsMonitored() && player->m_pstPeer)
			{
				auto peer = player->m_pstPeer;
				DebugLogStream << "player(" << player->m_unAccId << ", " << player->m_unRoleId << ") ping(" << peer->roundTripTime 
								<< ") packetsSent(" << peer->packetsSent << ") packetsLost("
								<< peer->packetsLost << ") packetLoss(" << peer->packetLoss << ")." << LogStream::eos;
			}
		}
	}
}

void UCPlayerMgr::OnQuit()
{
	
	for (UCPlayerMap::iterator iter = m_stPlayerMap.begin(); iter != m_stPlayerMap.end(); ++iter)
	{
		if (iter->second)
		{
			CL_SAFE_DELETE(iter->second);
		}
	}

	m_stPlayerMap.clear();
}
