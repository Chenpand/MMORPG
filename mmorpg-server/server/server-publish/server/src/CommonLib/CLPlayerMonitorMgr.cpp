#include "CLPlayerMonitorMgr.h"

ClPlayerMonitorMgr::ClPlayerMonitorMgr()
{
}

ClPlayerMonitorMgr::~ClPlayerMonitorMgr()
{

}

void ClPlayerMonitorMgr::AddPlayer(UInt32 accid)
{
	m_AccountList.insert(accid);
	OnPlayerAddToMonitor(accid);
}

void ClPlayerMonitorMgr::DelPlayer(UInt32 accid)
{
	m_AccountList.erase(accid);
	OnPlayerDelFromMonitor(accid);
}

void ClPlayerMonitorMgr::SetMonitorList(std::set<UInt32>& accList)
{
	std::vector<UInt32> newPlayers;
	std::vector<UInt32> needDelPlayers;
	for (auto accid : accList)
	{
		if (!IsPlayerMonitored(accid))
		{
			newPlayers.push_back(accid);
		}
	}

	for (auto accid : m_AccountList)
	{
		if (accList.find(accid) == accList.end())
		{
			needDelPlayers.push_back(accid);
		}
	}

	for (auto accid : needDelPlayers)
	{
		DelPlayer(accid);
	}

	for (auto accid : newPlayers)
	{
		AddPlayer(accid);
	}
}

bool ClPlayerMonitorMgr::IsPlayerMonitored(UInt32 accid)
{
	//return true;
	return m_AccountList.find(accid) != m_AccountList.end();
}