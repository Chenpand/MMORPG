#include "DungeonMatchAlgorithm.h"
#include <CLDungeonMatchDataEntry.h>

DungeonMatchAlgorithm::DungeonMatchAlgorithm()
{
	m_lastUpdateTime = 0;
}

DungeonMatchAlgorithm::~DungeonMatchAlgorithm()
{

}

bool DungeonMatchAlgorithm::Initialize()
{
	class DungeonMatchVistor : public Avalon::DataEntryVisitor<DungeonMatchDataEntry>
	{
	public:
		bool operator()(DungeonMatchDataEntry *dataEntry)
		{
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<DungeonMatchDataEntry*> datas;
	};

	DungeonMatchVistor vistor;
	DungeonMatchDataEntryMgr::Instance()->Visit(vistor);

	for (auto data : vistor.datas)
	{
		if (!data)
		{
			continue;
		}

		auto algorithm = new Match::SeMatchAlorithm();
		if (!algorithm->Initialize(data->fullPlayerNum, 1, 100, 100, 10, 10, 5))
		{
			return false;
		}
		m_mapAlgorithm[data->dungeonId] = algorithm;
	}

	return true;
}

void DungeonMatchAlgorithm::OnTick(const Avalon::Time& now)
{
	for (auto& itr : m_mapAlgorithm)
	{
		auto algorithm = itr.second;
		algorithm->Update(now.MSec());
	}
}

bool DungeonMatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
	Match::DungeonMatchKey& key2 = static_cast<Match::DungeonMatchKey&>(key);

	auto itr = m_mapAlgorithm.find(key2.id);
	if (itr == m_mapAlgorithm.end())
	{
		return false;
	}

	return itr->second->AddMatchTeam(teamId, playerNum, key2.level);
}

bool DungeonMatchAlgorithm::DelTeam(ObjID_t teamId)
{
	for (auto& itr : m_mapAlgorithm)
	{
		auto algorithm = itr.second;
		algorithm->DelMatchTeam(teamId);
	}
	return true;
}

std::vector<Match::MatchTeamResult> DungeonMatchAlgorithm::GetMatchTeamResult()
{
	std::vector<Match::MatchTeamResult> results;

	for (auto& itr : m_mapAlgorithm)
	{
		auto algorithm = itr.second;
		auto& allResult = algorithm->GetAllMatcheResult();

		for (auto& result : allResult)
		{
			Match::MatchTeamResult tmpResult;

			for (auto& unit : result)
			{
				for (auto& team : unit->GetAllMatchTeams())
				{
					tmpResult.teams.push_back(team);
				}
			}

			results.push_back(tmpResult);
		}

		algorithm->DelAllMatchResult();
	}

	return results;
}