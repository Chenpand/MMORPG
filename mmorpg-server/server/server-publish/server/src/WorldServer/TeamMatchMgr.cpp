#include "TeamMatchMgr.h"
#include "CLTeamDungeonDataEntry.h"
#include "WSPlayer.h"
#include "WSTeam.h"
#include "WSTeamMgr.h"

UInt32 TeamMatchEntry::GetSize()
{
	if (type == TEAM_MATCH_ENTRY_PLAYER && player)
	{
		return 1;
	}
	else if (type == TEAM_MATCH_ENTRY_TEAM && team)
	{
		return team->GetSize();
	}

	return 0;
}

TeamMatchMgr::TeamMatchMgr()
{

}

TeamMatchMgr::~TeamMatchMgr()
{

}

bool TeamMatchMgr::OnInit()
{
	m_MatchTimer.SetInterval(Avalon::Time::MSECS_OF_SEC);

	// ����ƥ���
	class TeamDungeonDataVistor : public Avalon::DataEntryVisitor<TeamDungeonDataEntry>
	{
	public:
		bool operator()(TeamDungeonDataEntry *dataEntry)
		{
			if (!dataEntry)
			{
				return false;
			}
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<TeamDungeonDataEntry*>	datas;
	};
	TeamDungeonDataVistor vistor;
	TeamDungeonDataEntryMgr::Instance()->Visit(vistor);

	for (auto data : vistor.datas)
	{
		if (data->type == TEAM_DUNGEON_MENU || data->dungeonId == 0 || data->maxPlayerNum == 0)
		{
			continue;
		}

		TeamDungeonMatchPool pool;
		pool.targetId = data->GetKey();
		pool.fullPlayerNum = data->maxPlayerNum;
		pool.matchType = (TeamMatchType)data->matchType;
		m_MatchPool[data->dungeonId] = pool;
	}

	return true;
}

void TeamMatchMgr::OnTick(const Avalon::Time& now)
{
	if (m_MatchTimer.IsTimeout(now))
	{
		// ��ƥ�����ƥ��
		_DoMatch();

		// ���Լ������ж���
		_DoJoinTeamMatch();
	}

	_DoMatchResult();
}

UInt32 TeamMatchMgr::OnPlayerJoinMatch(WSPlayer* player, UInt32 dungeonId)
{
	DebugLogStream << "player(" << player->GetID() << ") request team match, dungeon(" << dungeonId << ")." << LogStream::eos;

	auto entry = this->_FindEntryByPlayer(player);
	if (entry)
	{
		ErrorLogStream << "player(" << player->GetID() << ") is in matching" << LogStream::eos;
		return ErrorCode::TEAM_MATCH_START_IN_MATCHING;
	}

	// Ѱ��ƥ���
	auto itr = m_MatchPool.find(dungeonId);
	if (itr == m_MatchPool.end())
	{
		ErrorLogStream << "player(" << player->GetID() << ") request team match, dungeon(" << dungeonId << ") can't match." << LogStream::eos;
		return ErrorCode::TEAM_MATCH_START_FAILED;
	}

	auto& pool = itr->second;
	auto team = player->GetTeam();
	TeamMatchEntryType type = team ? TEAM_MATCH_ENTRY_TEAM : TEAM_MATCH_ENTRY_PLAYER;
	entry = new TeamMatchEntry(pool.matchType, dungeonId, type, player, team);
	pool.entrys.push_back(entry);

	CLProtocol::SceneTeamMatchStartRes res;
	res.result = ErrorCode::SUCCESS;
	_Broadcast(entry, res);

	return ErrorCode::SUCCESS;
}

UInt32 TeamMatchMgr::OnPlayerCancelMatch(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::TEAM_MATCH_CANCEL_NOT_IN_MATCHING;
	}

	auto entry = _FindEntryByPlayer(player);
	if (!entry)
	{
		return ErrorCode::TEAM_MATCH_CANCEL_NOT_IN_MATCHING;
	}

	InfoLogStream << "player(" << player->GetID() << ") cancel team match." << LogStream::eos;

	if (entry->dirty && entry->matchType == TEAM_MATCH_TYPE_RACE)
	{
		// ˵����ȡ��ƥ����ɽ���ս��
		// ����ж��飬Ҫ�˳�����
		auto team = entry->team;
		if (team && team->GetSize() > 1)
		{
			team->RemoveMember(player->GetID());
		}
	}

	_BroadMatchCancel(entry);
	_DelEntry(entry, true);
	return ErrorCode::SUCCESS;
}

UInt32 TeamMatchMgr::OnTeamCancelMatch(WSTeam* team)
{
	if (!team)
	{
		return ErrorCode::TEAM_MATCH_CANCEL_NOT_IN_MATCHING;
	}

	auto entry = _FindEntryByTeam(team);
	if (!entry)
	{
		return ErrorCode::TEAM_MATCH_CANCEL_NOT_IN_MATCHING;
	}

	InfoLogStream << "team(" << team->GetId() << ") cancel team match." << LogStream::eos;

	if (entry->dirty && entry->matchType == TEAM_MATCH_TYPE_RACE)
	{
		// ˵����ȡ��ƥ����ɽ���ս��
		// ����ж��飬Ҫ�˳�����
		auto team = entry->team;
		if (team && team->GetSize() >= 0)
		{
			team->Dismiss();
		}
	}

	_BroadMatchCancel(entry);
	_DelEntry(entry, true);
	return ErrorCode::SUCCESS;
}

void TeamMatchMgr::_DoMatchResult()
{
	UInt64 curTime = CURRENT_TIME.MSec();
	// ������
	for (UInt32 i = 0; i < m_Results.size(); i++)
	{
		auto& result = m_Results[i];
		if (result.entrys.empty() || curTime >= result.doResultTime)
		{
			_OnMatchSuccess(result);

			auto entrys = result.entrys;
			for (auto entry : entrys)
			{
				_DelEntry(entry, true);
			}

			m_Results.erase(m_Results.begin() + i);
			i--;
		}
	}
}

void TeamMatchMgr::_DoMatch()
{
	// ���
	std::vector<TeamMatchResult> results;
	// ƥ���������������ֻ��ƥ��ɹ�������������Ҵճ���Ҫ������
	for (auto itr : m_MatchPool)
	{
		auto dungeonId = itr.first;
		auto& dungeonPool = itr.second;

		// ������������
		auto entrysBySize = _GenMatchEntryBySize(dungeonPool);
		if (entrysBySize.size() < 2)
		{
			continue;
		}

		// һ���������ˣ����������(1+1+1��1+2��������ֻ��Ҫ����1���˵�����Ϳ�����
		auto& onePlayerPool = entrysBySize[1];

		// �ȴ���1+1+1
		UInt32 tripleSize = onePlayerPool.size() / dungeonPool.fullPlayerNum;
		for (UInt32 i = 0; i < tripleSize; i++)
		{
			TeamMatchResult result;
			for (UInt32 j = 0; j < dungeonPool.fullPlayerNum; j++)
			{
				auto entry = onePlayerPool[i * dungeonPool.fullPlayerNum + j];
				entry->dirty = true;

				result.target = dungeonPool.targetId;
				result.dungeonId = dungeonId;
				result.matchType = dungeonPool.matchType;
				result.entrys.push_back(entry);
			}
			results.push_back(result);
		}

		// �ٴ���1+2
		for (UInt32 i = tripleSize * dungeonPool.fullPlayerNum; i < onePlayerPool.size(); i++)
		{
			auto entry = onePlayerPool[i];
			UInt32 remainSize = dungeonPool.fullPlayerNum - 1;
			if (entrysBySize.size() <= remainSize || entrysBySize[remainSize].empty())
			{
				break;
			}

			// ��һ��û��õĶ���
			TeamMatchEntry* targetEntry = NULL;
			for (auto tmpEntry : entrysBySize[remainSize])
			{
				if (tmpEntry->dirty)
				{
					continue;
				}

				targetEntry = tmpEntry;
				break;
			}

			if (!targetEntry)
			{
				break;
			}

			TeamMatchResult result;
			result.target = dungeonPool.targetId;
			result.dungeonId = dungeonId;
			result.matchType = dungeonPool.matchType;
			result.entrys.push_back(entry);
			result.entrys.push_back(targetEntry);

			for (auto tmp : result.entrys)
			{
				tmp->dirty = true;
			}
			
			if (entry->type == TEAM_MATCH_ENTRY_TEAM && entry->team)
			{
				InfoLogStream << "team(" << entry->team->GetId() << ") match with team(" << (targetEntry->team ? targetEntry->team->GetId() : 0) << ")." << LogStream::eos;
			}
			else if (entry->type == TEAM_MATCH_ENTRY_PLAYER && entry->player)
			{
				InfoLogStream << "player(" << entry->player->GetID() << ") match with team(" << (targetEntry->team ? targetEntry->team->GetId() : 0) << ")." << LogStream::eos;
			}

			if (targetEntry->team != nullptr)
			{
				targetEntry->team->SetIsMatched(true);
			}

			results.push_back(result);
		}

		// ������û���Ѿ����������Ķ���
		if (entrysBySize.size() <= dungeonPool.fullPlayerNum)
		{
			break;
		}

		for (auto entry : dungeonPool.entrys)
		{
			TeamMatchResult result;
			result.target = dungeonPool.targetId;
			result.dungeonId = dungeonId;
			result.matchType = dungeonPool.matchType;
			entry->dirty = true;
			result.entrys.push_back(entry);

			results.push_back(result);
		}
	}

	for (auto& result : results)
	{
		if (result.matchType == TEAM_MATCH_TYPE_RACE)
		{
			// �㲥����
			_BroadcastNotifyRaceVote(result);
		}

		for (auto entry : result.entrys)
		{
			_DelEntry(entry, false);
		}

		m_Results.push_back(result);
	}
}

void TeamMatchMgr::_DoJoinTeamMatch()
{
	std::vector<TeamMatchEntry*> joinTeamEntrys;

	for (auto itr : m_MatchPool)
	{
		std::vector<TeamMatchEntry*> onePlayerEntrys;
		auto dungeonId = itr.first;
		auto& dungeonPool = itr.second;
		for (auto entry : dungeonPool.entrys)
		{
			if (entry->dirty || !entry->IsStartJoinTeam(CURRENT_TIME.MSec()))
			{
				continue;
			}

			if (_TryJoinOtherTeam(dungeonPool.targetId, dungeonPool.fullPlayerNum, entry))
			{
				joinTeamEntrys.push_back(entry);
			}
			else if (entry->GetSize() == 1)
			{
				onePlayerEntrys.push_back(entry);
			}
		}

		// ʵ�ڲ����ˣ���������������һ��
		// ��ʵֻ��һ�����������ֻ��1+1�����
		if (onePlayerEntrys.size() == 2)
		{
			TeamMatchResult result;
			result.target = dungeonPool.targetId;
			result.dungeonId = dungeonId;
			// ��������¿϶��ǿ������
			result.matchType = TEAM_MATCH_TYPE_TEAM;
			
			for (auto entry : onePlayerEntrys)
			{
				entry->dirty = true;
				result.entrys.push_back(entry);
				_DelEntry(entry, false);
			}

			m_Results.push_back(result);
		}
	}
	
	for (auto entry : joinTeamEntrys)
	{
		_DelEntry(entry, true);
	}
}

bool TeamMatchMgr::_TryJoinOtherTeam(UInt32 target, UInt32 fullPlayerNum, TeamMatchEntry* entry)
{
	if (entry->GetSize() >= fullPlayerNum)
	{
		return true;
	}

	for (UInt32 i = fullPlayerNum - entry->GetSize(); i >= 1; i--)
	{
		// �Ҷ���
		auto team = WSTeamMgr::Instance()->OnQueryTeamByTargetAndNum(entry->team, target, i);
		if (!team)
		{
			continue;
		}

		if (entry->type == TEAM_MATCH_ENTRY_PLAYER && entry->player)
		{
			_BroadMatchCancel(entry);
			team->AddMember(entry->player, true);

			if (team->GetSize() > 3)
			{
				ErrorLogStream << "final team(" << team->GetId() << ") too many member(" << team->GetSize() << ")." << LogStream::eos;
			}
		}
		else if (entry->type == TEAM_MATCH_ENTRY_TEAM && entry->team)
		{
			std::vector<WSTeam*> teams;
			teams.push_back(entry->team);
			teams.push_back(team);
			auto finalTeam = WSTeamMgr::Instance()->CombineTeam(teams);
			if (finalTeam)
			{
				CLProtocol::WorldTeamMatchCancelRes res;
				res.result = ErrorCode::SUCCESS;
				finalTeam->Broadcast(res);
			}

			if (finalTeam->GetSize() > 3)
			{
				ErrorLogStream << "final team(" << finalTeam->GetId() << ") too many member(" << finalTeam->GetSize() << ")." << LogStream::eos;
			}
		}
		else
		{
			// ����
			ErrorLogStream << "error team match entry." << LogStream::eos;
		}

		return true;
	}

	return false;
}

bool TeamMatchMgr::_OnMatchSuccess(TeamMatchResult result)
{
	if (result.entrys.empty())
	{
		return false;
	}

	// ���������һ����
	
	// ���еĶ���͸���
	std::vector<WSTeam*> teams;
	std::vector<WSPlayer*> players;
	for (auto entry : result.entrys)
	{
		if (entry->type == TEAM_MATCH_ENTRY_PLAYER && entry->player)
		{
			players.push_back(entry->player);
		}
		else if (entry->type == TEAM_MATCH_ENTRY_TEAM && entry->team)
		{
			teams.push_back(entry->team);
			entry->team->SetIsMatched(false);
		}
		else
		{
			// ����
			ErrorLogStream << "error team match entry." << LogStream::eos;
			return false;
		}
	}

	// ���û�ж��飬�����һ�������Ϊ�ӳ�����һ������
	if (teams.empty())
	{
		// ȫ�ǵ��������
		if (players.empty())
		{
			return false;
		}

		// ��һ�����Ϊ�ӳ���������
		auto master = players[0];
		master->CreateTeam(result.target, false);
		auto team = master->GetTeam();
		if (!team)
		{
			ErrorLogStream << "player(" << master->GetID() << ") create team(" << result.target << ") failed." << LogStream::eos;
			return false;
		}

		teams.push_back(team);
		players.erase(players.begin());
	}

	// �ѵ�������ң��ŵ�һ�����ж�����
	auto team = teams[0];
	for (auto player : players)
	{
		team->AddMember(player, true);
	}

	auto finalTeam = WSTeamMgr::Instance()->CombineTeam(teams);
	if (!finalTeam)
	{
		FatalLogStream << "combine team failed." << LogStream::eos;
		return false;
	}

	if (finalTeam->GetSize() > 3)
	{
		ErrorLogStream << "final team(" << finalTeam->GetId() << ") too many member(" << finalTeam->GetSize() << ")." << LogStream::eos;
	}

	if (result.matchType == TEAM_MATCH_TYPE_RACE)
	{
		// �������
		finalTeam->SendStartRaceToScene(result.dungeonId, SceneNpc());
	}

	CLProtocol::WorldTeamMatchCancelRes cancel;
	cancel.result = ErrorCode::SUCCESS;
	finalTeam->Broadcast(cancel);

	return true;
}

std::vector<std::vector<TeamMatchEntry*>> TeamMatchMgr::_GenMatchEntryBySize(TeamDungeonMatchPool& pool)
{
	std::vector<std::vector<TeamMatchEntry*>> entrysBySize;

	for (auto entry : pool.entrys)
	{
		if (!entry)
		{
			continue;
		}

		if (entry->GetSize() >= entrysBySize.size())
		{
			entrysBySize.resize(entry->GetSize() + 1);
		}

		entrysBySize[entry->GetSize()].push_back(entry);
	}

	return entrysBySize;
}

TeamMatchEntry* TeamMatchMgr::_FindEntryByPlayer(WSPlayer* player)
{
	if (!player)
	{
		return NULL;
	}

	for (auto itr : m_MatchPool)
	{
		auto& dungeonPool = itr.second;
		for (auto entry : dungeonPool.entrys)
		{
			if (_IsSame(entry, player))
			{
				return entry;
			}
		}
	}

	// �ڽ���в���
	for (auto& result : m_Results)
	{
		for (auto entry : result.entrys)
		{
			if (_IsSame(entry, player))
			{
				entry->dirty = true;
				return entry;
			}
		}
	}

	return NULL;
}

TeamMatchEntry* TeamMatchMgr::_FindEntryByTeam(WSTeam* team)
{
	if (!team)
	{
		return NULL;
	}

	for (auto itr : m_MatchPool)
	{
		auto& dungeonPool = itr.second;
		for (auto entry : dungeonPool.entrys)
		{
			if (_IsSame(entry, team))
			{
				return entry;
			}
		}
	}

	// �ڽ���в���
	for (auto& result : m_Results)
	{
		for (auto entry : result.entrys)
		{
			if (_IsSame(entry, team))
			{
				return entry;
			}
		}
	}

	return NULL;
}

void TeamMatchMgr::_DelEntry(TeamMatchEntry* entry, bool destoryObject)
{
	for (auto& itr : m_MatchPool)
	{
		for (UInt32 i = 0; i < itr.second.entrys.size(); i++)
		{
			if (itr.second.entrys[i] == entry)
			{
				itr.second.entrys.erase(itr.second.entrys.begin() + i);
				if (destoryObject)
				{
					delete entry;
				}
				return;
			}
		}
	}

	// �ڽ���в���
	for (auto& result : m_Results)
	{
		for (UInt32 i = 0; i < result.entrys.size(); i++)
		{
			if (result.entrys[i] == entry)
			{
				result.entrys.erase(result.entrys.begin() + i);
				if (destoryObject)
				{
					delete entry;
				}
			}
		}
	}
}

void TeamMatchMgr::_BroadcastNotifyRaceVote(TeamMatchResult& result)
{
	// �����������
	CLProtocol::WorldTeamMatchResultNotify notify;
	notify.dungeonId = result.dungeonId;

	std::vector<WSPlayer*> allPlayers;
	for (auto entry : result.entrys)
	{
		if (entry->player)
		{
			notify.players.push_back(entry->player->GetIcon());
			allPlayers.push_back(entry->player);
		}
		else if (entry->team)
		{
			auto members = entry->team->GetAllMembers();
			for (auto member : members)
			{
				notify.players.push_back(member->GetIcon());
				allPlayers.push_back(member);
			}
		}
	}

	// �㲥
	for (auto player : allPlayers)
	{
		player->SendProtocol(notify);
	}

	result.doResultTime = CURRENT_TIME.MSec() + 5 * Avalon::Time::MSECS_OF_SEC;
}

bool TeamMatchMgr::_IsSame(TeamMatchEntry* entry, WSPlayer* player)
{
	if (!entry || !player)
	{
		return false;
	}

	if (entry->type == TEAM_MATCH_ENTRY_PLAYER && entry->player == player)
	{
		return true;
	}
	else if (entry->type == TEAM_MATCH_ENTRY_TEAM && entry->team == player->GetTeam())
	{
		return true;
	}

	return false;
}

bool TeamMatchMgr::_IsSame(TeamMatchEntry* entry, WSTeam* team)
{
	if (!entry || !team)
	{
		return false;
	}

	if (entry->type == TEAM_MATCH_ENTRY_TEAM && entry->team == team)
	{
		return true;
	}

	return false;
}

void TeamMatchMgr::_BroadMatchCancel(TeamMatchEntry* entry)
{
	if (!entry)
	{
		return;
	}

	CLProtocol::WorldTeamMatchCancelRes res;
	res.result = ErrorCode::SUCCESS;
	_Broadcast(entry, res);
}

void TeamMatchMgr::_Broadcast(TeamMatchEntry* entry, Avalon::Protocol& protocol)
{
	if (!entry)
	{
		return;
	}

	if (entry->type == TEAM_MATCH_ENTRY_PLAYER && entry->player)
	{
		//InfoLogStream << "player(" << entry->player->GetID() << ") cancel team match." << LogStream::eos;
		entry->player->SendProtocol(protocol);
	}
	else if (entry->type == TEAM_MATCH_ENTRY_TEAM && entry->team)
	{
		//InfoLogStream << "team(" << entry->team->GetId() << ") cancel team match." << LogStream::eos;
		entry->team->Broadcast(protocol);
	}
}