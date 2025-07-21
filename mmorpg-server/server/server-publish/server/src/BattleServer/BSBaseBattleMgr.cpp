#include "BSBaseBattleMgr.h"
#include "BSBaseBattle.h"
#include "BSBaseRace.h"

UInt32 BSBaseBattleMgr::m_CurrentID = 100000000;

BSBaseBattleMgr::BSBaseBattleMgr()
{
	m_PreTime = 0;
	m_StartTime = 0;
	m_EndTime = 0;
	m_AwardTime = 0;
	m_Timer.SetInterval(500);
}

BSBaseBattleMgr::~BSBaseBattleMgr()
{
	std::map<UInt32, BSBaseBattle*>::iterator iter = m_allBattles.begin();
	while (iter != m_allBattles.end())
	{
		BSBaseBattle* battle = iter->second;
		++iter;
		if (battle == NULL) continue;
		CL_SAFE_DELETE(battle);
	}
	m_allBattles.clear();
}

UInt32 BSBaseBattleMgr::GenBattleID()
{
	++m_CurrentID;
	return m_CurrentID;
}

bool BSBaseBattleMgr::AddBattle(BSBaseBattle* battle)
{
	if (!battle)
	{
		return false;
	}

	auto iter = m_allBattles.find(battle->GetId());
	if (iter != m_allBattles.end())
	{
		return false;
	}

	return m_allBattles.insert(std::make_pair(battle->GetId(), battle)).second;
}

void BSBaseBattleMgr::DelBattle(BSBaseBattle* battle)
{
	auto iter = m_allBattles.find(battle->GetId());
	if (iter != m_allBattles.end())
	{
		m_allBattles.erase(iter);
	}

	CL_SAFE_DELETE(battle);
}

BSBaseBattle* BSBaseBattleMgr::FindBattle(UInt32 battleId)
{
	auto iter = m_allBattles.find(battleId);
	if (iter != m_allBattles.end())
	{
		return iter->second;
	}

	return NULL;
}

void BSBaseBattleMgr::DestoryBattle(BSBaseBattle* battle)
{
	if (!battle)	return;
	DestoryBattleImp(battle);
	m_allBattles.erase(battle->GetId());
	CL_SAFE_DELETE(battle);
}

void BSBaseBattleMgr::OnTick(const Avalon::Time& now)
{
	if (!m_Timer.IsTimeout(now))
	{
		return;
	}

	std::vector<BSBaseBattle*> allBattleNeedToDestory;
	for (auto itr : m_allBattles)
	{
		auto battle = itr.second;
		if (!battle)
		{
			continue;
		}

		battle->OnTick(now);

		if (battle->IsTimeToDestory(now))
		{
			allBattleNeedToDestory.push_back(battle);
		}
	}

	for (auto race : allBattleNeedToDestory)
	{
		DestoryBattle(race);
	}

	OnTickImp(now);
}

void BSBaseBattleMgr::OnSceneServerConnected(UInt32 nodeId)
{
	for (SceneServerVec::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if (*iter == nodeId) return;
	}
	m_SceneServers.push_back(nodeId);
}

void BSBaseBattleMgr::OnSceneServerDisconnected(UInt32 nodeId)
{
	for (SceneServerVec::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if (*iter == nodeId)
		{
			m_SceneServers.erase(iter);
			return;
		}
	}
}

UInt32 BSBaseBattleMgr::SelectSceneServer()
{
	UInt32 ret = 0;
	for (SceneServerVec::const_iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if (*iter == m_SceneServer)
		{
			if (++iter != m_SceneServers.end())
			{
				ret = *iter;
			}
			break;
		}
	}
	if (ret == 0 && !m_SceneServers.empty())
	{
		ret = *(m_SceneServers.begin());
	}
	m_SceneServer = ret;
	return ret;
}

void BSBaseBattleMgr::SetActivityTime(UInt32 preTime, UInt32 startTime, UInt32 endTime)
{
	m_PreTime = preTime;
	m_StartTime = startTime;
	m_EndTime = endTime;
}