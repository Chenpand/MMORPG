#include "WSTeamMgr.h"
#include <CLTeamProtocol.h>
#include "WSPlayer.h"
#include "TeamMatchMgr.h"
#include "WSPlayerMgr.h"

WSTeamMgr::WSTeamMgr()
{
	m_TeamIdSerial = 0;

	for (UInt16 number = 1; number <= MAX_TEAM_NUMBER; number++)
	{
		m_TeamNumbers.insert(number);
	}

	m_DailyTimer.SetTime(6);
}

WSTeamMgr::~WSTeamMgr()
{
	for(TeamMap::iterator iter = m_Teams.begin();
		iter != m_Teams.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_Teams.clear();
}

bool WSTeamMgr::AddTeam(WSTeam* team)
{
	if(team == NULL) return false;

	while(++m_TeamIdSerial == 0 || m_Teams.find(m_TeamIdSerial) != m_Teams.end())
		++m_TeamIdSerial;
	team->SetId(m_TeamIdSerial);

	auto teamNumber = GenTeamNumber();
	if (teamNumber == 0)
	{
		// 没有编号了，说明队伍已满
		return false;
	}
	team->SetNumber(teamNumber);
	return m_Teams.insert(std::make_pair(team->GetId(), team)).second;
}

void WSTeamMgr::RemoveTeam(WSTeam* team)
{
	if(team == NULL) return;

    DebugLogStream << "destory team(" << team->GetId() << ")." << LogStream::eos;

	TeamMatchMgr::Instance()->OnTeamCancelMatch(team);

	// 回收队伍编号
	m_TeamNumbers.insert(team->GetNumber());

	TeamMap::iterator iter = m_Teams.find(team->GetId());
	if(iter != m_Teams.end())
	{
		team->OnDismissed();
		m_Teams.erase(iter);
	}
}

WSTeam* WSTeamMgr::FindTeam(UInt32 id)
{
	TeamMap::iterator iter = m_Teams.find(id);
	if(iter != m_Teams.end()) return iter->second;
	return NULL;
}

void WSTeamMgr::VisitTeams(WSTeamVisitor& visitor)
{
	for(TeamMap::iterator iter = m_Teams.begin();
		iter != m_Teams.end(); ++iter)
	{
		if(!visitor(iter->second)) return;
	}
}

class TeamCompair
{
public:
	bool operator()(const WSTeam* team1, const WSTeam* team2)
	{
		if (!team1 || !team2)
		{
			return false;
		}

		return team1->GetTeamTarget() < team2->GetTeamTarget();
	}
};

void WSTeamMgr::OnQueryTeamList(WSPlayer* player, const CLProtocol::WorldQueryTeamList& req)
{
    if (!player || !player->IsOpenTeamFunction())
    {
        return;
    }

    CLProtocol::WorldQueryTeamListRet ret;
	ret.targetId = req.targetId;
	ret.param = req.param;

    std::vector<WSTeam*> allTeams;
    for (auto itr : m_Teams)
    {
        auto team = itr.second;
        if (IsMatchQueryCond(player, team, req))
        {
            allTeams.push_back(team);
        }
    }
	std::sort(allTeams.begin(), allTeams.end(), TeamCompair());

    ret.startPos = req.startPos;
    ret.maxNum = allTeams.size();

    UInt32 maxPos = req.startPos + (req.num > QUERY_MAX_TEAM_NUM_ONCE ? QUERY_MAX_TEAM_NUM_ONCE : req.num);
    for (UInt32 pos = req.startPos; pos < allTeams.size() && pos < maxPos; pos++)
    {
        auto team = allTeams[pos];
        if (team)
        {
            ret.teamList.push_back(team->GetBaseInfo());
        }
    }

    player->SendProtocol(ret);
}

WSTeam* WSTeamMgr::OnQueryTeamByTargetAndNum(WSTeam* srcTeam, UInt32 target, UInt32 num)
{
	for (auto itr : m_Teams)
	{
		auto team = itr.second;
		if (team != srcTeam && team->GetTeamTarget() == target && team->GetSize() == num && team->IsAutoAgree() && team->GetStatus() == TEAM_STATUS_IDEL)
		{
			if (WSApplication::Instance()->IsCloseFixTeamMatch())
			{
				return team;
			}
			else if (!team->GetIsMatched())
			{
				return team;
			}
		}
	}

	return NULL;
}

WSTeam* WSTeamMgr::CombineTeam(std::vector<WSTeam*> teams)
{
	if (teams.size() == 1)
	{
		return teams[0];
	}

	// 合并队伍
	// 找一只目标队伍(人数最多的)最为主队
	WSTeam* masterTeam = NULL;
	for (auto team : teams)
	{
		if (masterTeam == NULL || masterTeam->GetSize() < team->GetSize())
		{
			masterTeam = team;
		}
	}

	if (!masterTeam)
	{
		return NULL;
	}

	// 把其它队伍都解散，然后加到这个队伍
	for (auto team : teams)
	{
		if (team == masterTeam)
		{
			continue;
		}

		InfoLogStream << "combine team(" << team->GetId() << ", " << team->GetSize() << ") to team(" << masterTeam->GetId() << ", " << masterTeam->GetSize() << ")." << LogStream::eos;
		
		auto members = team->GetAllMembers();
		team->Dismiss();

		for (auto member : members)
		{
			if (!member)
			{
				continue;
			}

			// 加入目标队伍
			masterTeam->AddMember(member, true, false);
		}
	}

	return masterTeam;
}

void WSTeamMgr::OnPlayerOnline(WSPlayer* player)
{
	if(player == NULL) return;

	class OnlineVisitor : public WSTeamVisitor
	{
	public:
		OnlineVisitor(WSPlayer* player):m_pPlayer(player), m_sendInfo(false){}

		bool operator()(WSTeam* team)
		{
			if (team->OnPlayerOnline(m_pPlayer))
			{
				m_sendInfo = true;
				return false;
			}

			return true;
		}

	public:
		WSPlayer* m_pPlayer;
		bool m_sendInfo;
	};
	OnlineVisitor visitor(player);
	VisitTeams(visitor);

	if (!visitor.m_sendInfo)
	{
		CLProtocol::WorldSyncTeamInfo info;
		player->SendProtocol(info);
	}
}

void WSTeamMgr::OnTick(const Avalon::Time& now)
{
	for(TeamMap::iterator iter = m_Teams.begin();
		iter != m_Teams.end();)
	{
		WSTeam* team = iter->second;
		++iter;

		if (team)
		{
			team->OnTick(now);
		}

		if(team->GetDismissTime() != 0
			&& CURRENT_TIME.Sec() > team->GetDismissTime())
		{
			RemoveTeam(team);
			CL_SAFE_DELETE(team);
		}
		else if(team->GetMaster() == 0)
		{
			RemoveTeam(team);
			CL_SAFE_DELETE(team);
		}
	}

	if (m_DailyTimer.IsTimeout(now))
	{
		OnDayChanged();
	}
}

void WSTeamMgr::OnDayChanged()
{
	for (auto itr : m_Teams)
	{
		WSTeam* team = itr.second;
		if (team)
		{
			team->OnDayChanged();
		}
	}
}

bool WSTeamMgr::IsMatchQueryCond(WSPlayer* player, WSTeam* team, const CLProtocol::WorldQueryTeamList& req)
{
	if (!player || !team || team->GetSize() == 0 || team->GetStatus() != TEAM_STATUS_IDEL)
    {
        return false;
    }

    do {
		if (req.targetId <= 0)
		{
			if (!GuildDungeonCond(player, team))
			{
				return false;
			}

			return true;
		}

        if (req.teamId > 0)
        {
            if (team->GetNumber() != req.teamId)
            {
                return false;
            }
            break;
        }

		if (std::find(req.targetList.begin(), req.targetList.end(), team->GetTeamTarget()) == req.targetList.end())
        {
            return false;
        }

		if (!GuildDungeonCond(player, team))
		{
			return false;
		}

    } while (0);
    
    return true;
}

bool WSTeamMgr::GuildDungeonCond(WSPlayer* player, WSTeam* team)
{
	if (!player || !team)
	{
		return false;
	}

	// 如果是公会副本组队，只能看见本公会的，用队长的公会和自己的公会判断
	DungeonDataEntry* dungeonEntry = DungeonDataEntryMgr::Instance()->FindEntry(team->GetTargetDungeonID());
	if (dungeonEntry && dungeonEntry->subType == DST_GUILD_DUNGEON)
	{
		WSPlayer* masterPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(team->GetMaster());
		if (masterPlayer && masterPlayer->GetGuildID() != player->GetGuildID())
		{
			return false;
		}
	}

	return true;
}

UInt16 WSTeamMgr::GenTeamNumber()
{
	auto itr = m_TeamNumbers.begin();
	if (itr == m_TeamNumbers.end())
	{
		return 0;
	}

	auto number = *itr;
	m_TeamNumbers.erase(itr);
	return number;
}
