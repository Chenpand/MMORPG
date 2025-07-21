#include "OnlineStateMgr.h"

#include <CLGameDefine.h>
#include "InviteDataMgr.h"

OnlineStateMgr::OnlineStateMgr()
{
	m_MinTimer.SetInterval(60 * 1000);
}

OnlineStateMgr::~OnlineStateMgr()
{
}

void OnlineStateMgr::ReportOnline(UInt32 id)
{
	OnlineStateMap::iterator iter = m_OnlineStates.find(id);
	if(iter != m_OnlineStates.end())
	{
		iter->second = UInt32(CURRENT_TIME.Sec());
	}
	else
	{
		m_OnlineStates.insert(std::make_pair(id, UInt32(CURRENT_TIME.Sec())));
		OnOnline(id);
	}
}

UInt32 OnlineStateMgr::GetOnlineTime(UInt32 id) const
{
	HashMap<UInt32, UInt32>::const_iterator iter = m_OnlineStates.find(id);
	if(iter != m_OnlineStates.end())
	{
		return iter->second;
	}
	return 0;
}

void OnlineStateMgr::OnTick(const Avalon::Time& now)
{
	if(m_MinTimer.IsTimeout(now))
	{
		for(OnlineStateMap::iterator iter = m_OnlineStates.begin();
			iter != m_OnlineStates.end();)
		{
			if(now.Sec() > iter->second + REPORT_ONLINE_STATE_INTERVAL)
			{
				UInt32 id = iter->first;
				OnOffline(id);
				m_OnlineStates.erase(iter++);
			}
			else
			{
				++iter;
			}
		}
	}
}

void OnlineStateMgr::OnOnline(UInt32 id)
{
	InviteDataMgr::Instance()->OnPlayerOnline(id);
}

void OnlineStateMgr::OnOffline(UInt32 id)
{
	InviteDataMgr::Instance()->OnPlayerOffline(id);
}

