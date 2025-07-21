#include "ChampionRaceMgr.h"

#include <CLRecordClient.h>
#include <CLGameSessionDefine.h>
#include <CLSceneObject.h>

#include "USApplication.h"
#include "ChampionPlayer.h"
#include "GameZoneMgr.h"



ChampionRace::ChampionRace()
{
	m_Data.groupID = 0;
	m_Data.raceID = 0;
	m_Data.championStatus = ChampionStatus::CS_NULL;
	m_Data.roleID1 = 0;
	m_Data.roleID2 = 0;
	m_Data.isEnd = 0;
	m_Data.winnerID = false;
	m_Data.result = _resultBitmask::resultBitmask_invalid;
}

void ChampionRace::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_champion_battle_record", m_Data.GetID(), false);
	m_Data.GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}
}

void ChampionRace::InsterToDB()
{
	m_Data.SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_champion_battle_record", m_Data.GetID());
	m_Data.GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
}

void ChampionRace::LoadDBData(CLRecordCallback* callback)
{
	m_Data.SetID(callback->GetKey());
	m_Data.SetDBData(callback);
}

class SelectChampionRecordCallBack : public CLRecordCallback
{
public:
	SelectChampionRecordCallBack(ChampionRaceMgr& mgr) : m_Mgr(mgr) {}
	virtual void OnFinished()
	{
		m_Mgr.OnLoadData(this);
	}
	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load ChampionRecord Faild,ErrorCode is" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}

private:
	ChampionRaceMgr& m_Mgr;
};

const bool ChampionRaceMgr::Init()
{
	m_SaveTimer.SetInterval(1000);
	SelectChampionRecordCallBack* callback = new SelectChampionRecordCallBack(*this);
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_champion_battle_record");
	CLRecordClient::Instance()->SendCommand(selectCmd, callback);
	return true;
}

void ChampionRaceMgr::Clear()
{
	InfoLogStream << "ChampionRace is clear" << LogStream::eos;
	for (auto& recordMapIter : m_BattleRecordMap)
	{
		for (auto& recordIter : recordMapIter.second)
		{
			CL_SAFE_DELETE(recordIter.second);
		}
	}
	m_BattleRecordMap.clear();
	m_RaceMap.clear();
}

void ChampionRaceMgr::OnTick(const Avalon::Time & now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		for (auto& recordMapIter : m_BattleRecordMap)
		{
			for (auto& recordIter : recordMapIter.second)
			{
				recordIter.second->SaveToDB();
			}
		}
		//SyncAllRaceInfo();
	}
}

void ChampionRaceMgr::OnLoadData(CLRecordCallback* cb)
{
	while (cb->NextRow())
	{
		UInt32 groupID = cb->GetData("group_id");
		UInt32 order = cb->GetData("order");
		if (m_BattleRecordMap[groupID][order] == nullptr)
		{
			m_BattleRecordMap[groupID][order] = new ChampionRace;
		}
		m_BattleRecordMap[groupID][order]->LoadDBData(cb);
	}
}

ChampionRace* ChampionRaceMgr::AddRace(UInt32 groupID, UInt64 raceID)
{
	//m_BattleRecordMap[groupID][raceID]->SetGroupID(groupID);
	//m_BattleRecordMap[groupID][raceID]->SetRaceID(raceID);
	//return m_BattleRecordMap[groupID][raceID];
	return nullptr;
}

ChampionRace* ChampionRaceMgr::FindRace(UInt32 groupID, const UInt32 order)
{

	auto recordMapIter = m_BattleRecordMap.find(groupID);
	if (recordMapIter != m_BattleRecordMap.end())
	{
		auto recordIter = recordMapIter->second.find(order);
		if (recordIter != recordMapIter->second.end())
		{
			return recordIter->second;
		}
	}
	return nullptr;
}

std::map<UInt32, ChampionRace*> ChampionRaceMgr::FindRaces(UInt32 groupID)
{

	auto recordMapIter = m_BattleRecordMap.find(groupID);
	if (recordMapIter != m_BattleRecordMap.end())
	{
		return recordMapIter->second;
	}
	return std::map<UInt32, ChampionRace*>{};
}

void ChampionRaceMgr::OnRaceBegin(ChampionPlayer * aPlayer, ChampionPlayer * bPlayer, const ChampionStatus status, const UInt32 groupID, const UInt32 order)
{
	if (aPlayer == nullptr || bPlayer == nullptr)
	{
		ErrorLogStream << "Race(" << static_cast<UInt32>(status) << "|" << groupID << "|" << order << ") begin error, player is null" << LogStream::eos;
		return;
	}
	InfoLogStream << "Race("<< PLAYERINFO(aPlayer) << "|"<< PLAYERINFO(bPlayer) << "|" << static_cast<UInt32>(status) << "|" << groupID << "|"<< order <<") is begin" << LogStream::eos;
	auto race = FindRace(groupID, order);
	if (race != nullptr)
	{
		ErrorLogStream << "race already create (" << static_cast<UInt32>(status) << "|" << groupID << "|" << order << ")" << LogStream::eos;
		return;
	}

	race = new ChampionRace;
	race->SetChampionStatus(status);
	race->SetGroupID(groupID);
	race->SetOrder(order);
	race->SetPlayer(aPlayer, bPlayer);
	race->SetRoleID1(aPlayer->GetID());
	race->SetRoleID2(bPlayer->GetID());
	race->InsterToDB();
	m_BattleRecordMap[groupID][order] = race;

	aPlayer->SetIsInRace(true);
	bPlayer->SetIsInRace(true);
}

void ChampionRaceMgr::OnRaceCreateRet(const UInt32 groupID, UInt32 order, UInt64 raceID)
{
	InfoLogStream << "OnRaceCreateRet(" << groupID << " | " << order << " | " << raceID << ")" << LogStream::eos;
	if (GetRace(raceID) != nullptr)
	{
		ErrorLogStream << "race already create,raceid id " << raceID << LogStream::eos;
		return;
	}
	m_BattleRecordMap[groupID][order]->SetRaceID(raceID);
	m_RaceMap[raceID] = m_BattleRecordMap[groupID][order];
}

void ChampionRaceMgr::OnRaceEnd(UInt64 raceID, UInt64 winner, UInt64 loser, UInt32 result)
{
	InfoLogStream << "OnRaceEnd by raceid(" << raceID << " | " << winner << " | " << loser << "|" << result << ")" << LogStream::eos;
	auto race = ChampionRaceMgr::Instance()->GetRace(raceID);
	if (race == nullptr)
	{
		ErrorLogStream << "cant find battle race,raceid is" << raceID << LogStream::eos;
		return;
	}
	race->SetRoleID1(winner);
	race->SetWinnerID(winner);
	race->SetRoleID2(loser);
	race->SetIsEnd(true);
	race->SetResult(result);
	if (race->GetPlayerA() != nullptr)
	{
		race->GetPlayerA()->SetIsInRace(false);
	}	
	if (race->GetPlayerB() != nullptr)
	{
		race->GetPlayerB()->SetIsInRace(false);
	}
}

void ChampionRaceMgr::OnRaceEnd(const UInt32 groupID, const UInt32 order, UInt64 winner, UInt64 loser, UInt32 result)
{
	InfoLogStream << "OnRaceEnd by group and order(" << groupID << " | " << order << "|" << winner << " | " << loser << "|" << result << ")" << LogStream::eos;
	auto race = ChampionRaceMgr::Instance()->FindRace(groupID, order);
	if (race == nullptr)
	{
		ErrorLogStream << "cant find battle race" << LogStream::eos;
		return;
	}
	//OnRaceEnd(race->GetID(), winner, loser, result);
	race->SetRoleID1(winner);
	race->SetWinnerID(winner);
	race->SetRoleID2(loser);
	race->SetIsEnd(true);
	race->SetResult(result);
	if (race->GetPlayerA() != nullptr)
	{
		race->GetPlayerA()->SetIsInRace(false);
	}
	if (race->GetPlayerB() != nullptr)
	{
		race->GetPlayerB()->SetIsInRace(false);
	}
}

ChampionRace* ChampionRaceMgr::GetRace(UInt64 raceID)
{
	if (m_RaceMap.find(raceID) == m_RaceMap.end())
	{
		return nullptr;
	}
	return m_RaceMap[raceID];
}

void ChampionRaceMgr::SyncAllRaceInfo()
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_CHAMPION_ALL_RACE_INFO_SYNC);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	bool hasaddr = false;
	for (auto& i : m_RaceMap)
	{
		try
		{
			stream & i.second->GetGroupID() & i.second->GetOrder();
			if (i.second->Encode(stream, SST_TEST, true))
			{
				hasaddr = true;
			}
		}
		catch (Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);
			return;
		}
	}

	stream & ObjID_t(0);

	if (hasaddr)
	{
		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
		GameZoneMgr::Instance()->BroadcastToGameZoneScene(packet);
	}
	Avalon::Packet::Destroy(packet);
}

UInt32 ChampionRaceMgr::GetRaceNum(UInt32 groupID)
{
	if (m_BattleRecordMap.count(groupID) == 0)
	{
		return 0;
	}
	return m_BattleRecordMap[groupID].size();
}
