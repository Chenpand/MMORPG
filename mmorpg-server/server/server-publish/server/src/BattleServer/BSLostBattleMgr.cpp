#include "BSLostBattleMgr.h"
#include "BattleSceneMgr.h"
#include "CLErrorCode.h"
#include "RelaySessionMgr.h"
#include "BSLostPKRace.h"
#include "BSRaceMgr.h"
#include "BSPlayerMgr.h"

#include <CLMatchProtocol.h>
#include <CLLostDungeonDataEntry.h>
#include <CLLostDungeonBattleDataEntry.h>

BSLostBattleMgr::BSLostBattleMgr()
{
	m_isOpen = false;
	//m_tseForOpenActTimer.SetInterval(30*1000);
	//m_Timer.SetInterval(1000);
	m_CheckTimer.SetInterval(1000);
}

BSLostBattleMgr::~BSLostBattleMgr()
{}

bool BSLostBattleMgr::AddBattle(BSBaseBattle* battle)
{
	if (!battle)	return false;
	BSLostBattle* lostBattle = dynamic_cast<BSLostBattle*> (battle);
	if (!lostBattle) return false;

	if (!BSBaseBattleMgr::AddBattle(battle)) return false;
	m_lostBattles[lostBattle->GetFloor()].push_back(lostBattle);
	return true;
}

BSLostBattle* BSLostBattleMgr::FindLostBattle(UInt32 battleId)
{
	BSBaseBattle* battle = FindBattle(battleId);
	if (!battle) return NULL;

	BSLostBattle* lostBattle = dynamic_cast<BSLostBattle*> (battle);
	return lostBattle;
}

void BSLostBattleMgr::OnActOpen()
{
	InfoLogStream << "lost dungeon battle active begin open!!!" << LogStream::eos;
	UInt32 maxFloor = LostDungeonFloorDataEntryMgr::Instance()->GetMaxFloor();
	for (UInt32 floor = 1; floor <= maxFloor; ++floor)
	{
		CreateOneBattleDynamic(floor);
	}
}

void BSLostBattleMgr::OnActClose()
{
	InfoLogStream << "lost dungeon battle active begin close!!!" << LogStream::eos;
	for (auto it : m_lostBattles)
	{
		auto battles = it.second;
		for (auto battle : battles)
		{
			if (!battle) continue;
			battle->NotifyCloseSceneBattle(LDCBR_ACT_CLOSE);
		}
	}
}

void BSLostBattleMgr::DestoryBattleImp(BSBaseBattle* battle)
{
	if (!battle)	return;
	BSLostBattle*  lostBattle = dynamic_cast<BSLostBattle*>(battle);
	if (!lostBattle)	return;

	auto it = m_lostBattles.find(lostBattle->GetFloor());
	if (it != m_lostBattles.end())
	{
		std::vector<BSLostBattle*>& battles = it->second;
		for (std::vector<BSLostBattle*>::iterator it = battles.begin(); it != battles.end(); ++it)
		{
			BSLostBattle* bt = *it;
			if (bt && bt->GetId() == battle->GetId())
			{
				battles.erase(it);
				InfoLogStream << "destory Battle id : " << lostBattle->GetId() << ", floor : " << lostBattle->GetFloor() << LogStream::eos;
				break;
			}
		}
	}
}

void BSLostBattleMgr::OnTickImp(const Avalon::Time& now)
{
	//if (m_tseForOpenActTimer.IsTimeout(now) && !m_isOpen)
	//{
	//	m_isOpen = true;
	//	OnActOpen();
	//}

	if (!m_isOpen)
	{
		UInt32 currTm = (UInt32)CURRENT_TIME.Sec();
		if (m_PreTime > 0 && m_EndTime > 0
			&& currTm >= m_PreTime && currTm < m_EndTime)
		{
			m_isOpen = true;
			OnActOpen();
		}
	}

	if (m_isOpen)
	{
		UInt32 currTm = (UInt32)CURRENT_TIME.Sec();
		if (currTm >= m_EndTime)
		{
			m_isOpen = false;
			OnActClose();
		}
	}

	if (m_isOpen)
	{
		if (m_CheckTimer.IsTimeout(now))
		{
			for (auto it : m_lostBattles)
			{
				UInt32 floor = it.first;
				//判断是否动态生成储备战场
				BSLostBattle* battle = GetPlayerLeastBattle(floor);
				if (battle && battle->GetPlayerNum() > battle->GetPlayerLimit() / 2)
				{
					InfoLogStream << "dynamic create one Reserved lost dungeon battle, floor : " << floor
						<< ", least battle id : " << battle->GetId() << ", battle player num : " << battle->GetPlayerNum()
						<< ", player limit : " << battle->GetPlayerLimit() << LogStream::eos;
					CreateOneBattleDynamic(floor);
				}
				//消耗多余0人战场
				std::vector<BSLostBattle*> battles;
				for (auto bt : it.second)
				{
					if (bt->GetState() != LDBS_NORMAL)
					{
						continue;
					}
					if (bt->GetPlayerNum() == 0)
					{
						battles.push_back(bt);
					}
				}

				//销毁空战场，留一个做储备
				if (battles.size() > 1) 
				{
					for (UInt32 i = 0; i < battles.size() - 1; ++i)
					{
						BSLostBattle* bt = battles[i];
						bt->NotifyCloseSceneBattle(LDCBR_BAT_DYNC_ADMIN);
					}
				}
			}
		}
	}
}

BSLostBattle*	BSLostBattleMgr::SelectOneBattle(UInt32 floor)
{
	auto it = m_lostBattles.find(floor);
	if (it == m_lostBattles.end())
	{
		return NULL;
	}
	BSLostBattle* battle = NULL;
	UInt32	playerNum = 0;
	for (auto bt : it->second)
	{
		if (bt->IsFull())
		{
			continue;
		}
		if (bt->GetState() != LDBS_NORMAL)
		{
			continue;
		}
		if (bt->GetFloor() != floor)
		{
			ErrorLogStream << "battle id : " << bt->GetId() << ", floor : " << bt->GetFloor()
				<< ", is in error battle vector, floor of vector : " << floor << LogStream::eos;
			continue;
		}
		if (bt->GetPlayerNum() > playerNum || battle == NULL)
		{
			playerNum = bt->GetPlayerNum();
			battle = bt;
		}
	}

	return battle;
}

BSLostBattle*	BSLostBattleMgr::GetPlayerLeastBattle(UInt32 floor)
{
	auto it = m_lostBattles.find(floor);
	if (it == m_lostBattles.end())
	{
		return NULL;
	}

	UInt32 minNum = 0;
	BSLostBattle* battle = NULL;

	for (auto bt : it->second)
	{
		if (bt->GetPlayerNum() < minNum || minNum == 0)
		{
			minNum = bt->GetPlayerNum();
			battle = bt;
		}
	}
	
	return battle;
}

void BSLostBattleMgr::CreateOneBattleDynamic(UInt32 floor)
{
	InfoLogStream << "begin create lost dungeon battles floor : " << floor << LogStream::eos;
	LostDungeonFloorDataEntry* floorData = LostDungeonFloorDataEntryMgr::Instance()->GetDataEntry(floor);
	if (!floorData)
	{
		ErrorLogStream << "create lost dungeon battle fail, not find LostDungeonFloorDataEntry floor : " << floor << LogStream::eos;
		return;
	}
	//随机战场
	UInt32 totalWeight = 0;
	UInt32 battleDataId = 0;
	for (auto wt : floorData->battlesWt)
	{
		totalWeight += wt.weight;
	}
	UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);

	for (auto wt : floorData->battlesWt)
	{
		if (wt.weight >= randWeight)
		{
			battleDataId = wt.battleDataId;
			break;
		}
		else
		{
			randWeight -= wt.weight;
		}
	}
	if (0 == battleDataId)
	{
		ErrorLogStream << "create lost dungeon battle fail, battleDataId == 0 floor : " << floor << LogStream::eos;
		return;
	}
	LostDungeonBattleDataEntry* battleData = LostDungeonBattleDataEntryMgr::Instance()->FindEntry(battleDataId);
	if (!battleData)
	{
		ErrorLogStream << "create lost dungeon battle fail, not find LostDungeonBattleDataEntry floor : " << floor 
			<< ", battleDataId : " << battleDataId << LogStream::eos;
		return;
	}

	UInt32 sceneNodeID = BattleSceneMgr::Instance()->SelectSceneServer();
	if (sceneNodeID <= 0)
	{
		ErrorLogStream << "create lost dungeon battle fail, SelectSceneServer fail sceneNodeID : " << sceneNodeID << ", floor : " << floor
			<< ", battleDataId : " << battleDataId << LogStream::eos;
		return;
	}
	BSLostBattle* battle = new BSLostBattle();
	if (!battle)
	{
		ErrorLogStream << "create lost dungeon battle fail, new BSLostBattle fail, floor : " << floor
			<< ", battleDataId : " << battleDataId << LogStream::eos;
		return;
	}
	battle->SetFloor(floorData->floor);
	battle->InitBattle(battleData);
	battle->SetSceneNodeID(sceneNodeID);
	battle->SetId(GenBattleID());
	AddBattle(battle);
	InfoLogStream << "BattleServer created lost dungeon battle success floor : " << floor << ", battleDataId : " << battleDataId
		<< ", battle id : " << battle->GetId() << ", sceneId : " << battle->GetSceneId() << LogStream::eos;
	battle->NotifyCreateSceneBattle();
}

void BSLostBattleMgr::SetActivityTime(UInt32 preTime, UInt32 startTime, UInt32 endTime)
{
	DebugLogStream << "LostBattle preTime : " << preTime <<", startTime : " << startTime << ", endTime : " << endTime << LogStream::eos;
	m_PreTime = preTime;
	m_StartTime = startTime;
	m_EndTime = endTime;
}
