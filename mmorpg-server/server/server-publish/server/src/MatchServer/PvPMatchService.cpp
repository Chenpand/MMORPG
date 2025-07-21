#include "PvPMatchService.h"
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLErrorCode.h>
#include <CLDungeonDataEntry.h>
#include <CLSeasonDataEntry.h>
#include <deque>
#include <CLRelayServerProtocol.h>
#include <CLSystemValueDataEntry.h>

#include "TraditionMatchAlgorithm.h"
#include "PvPMatchRace.h"
#include "MatchServiceMgr.h"
#include "MSRouter.h"
#include "RelaySessionMgr.h"
#include "MatchServiceMgr.h"
#include "MSApplication.h"

PvPMatchService::PvPMatchService(bool useRobot)
    : MatchService(MST_PVP)
{
	m_useRobot = useRobot;
}

PvPMatchService::~PvPMatchService()
{

}

bool PvPMatchService::Init()
{
    auto algorithm = new TraditionMatchAlgorithm();
    if (!algorithm->Initialize())
    {
        return false;
    }

    SetAlgorithm(algorithm);

	class DungeonVistor : public Avalon::DataEntryVisitor<DungeonDataEntry>
	{
	public:
		bool operator()(DungeonDataEntry *dataEntry)
		{
			if (dataEntry && dataEntry->subType == DST_PK && dataEntry->threeType != DTT_CHIJI_PK)
			{
				pkDungeonIdList.push_back(dataEntry->id);
			}
			return true;
		}

		std::vector<UInt32> pkDungeonIdList;
	};

	DungeonVistor vistor;
	DungeonDataEntryMgr::Instance()->Visit(vistor);
	m_pkDungeonIdList = vistor.pkDungeonIdList;

	if (m_pkDungeonIdList.empty())
	{
		ErrorLogStream << "pk dungeon list is empty." << LogStream::eos;
		return false;
	}

    return true;
}

bool PvPMatchService::OnTick(const Avalon::Time& now)
{
	MatchService::OnTick(now);

	if (!m_useRobot)
	{
		return true;;
	}

	for (auto itr : m_mapTeams)
	{
		auto team = itr.second;
		if (!team || !team->IsMatching())
		{
			continue;
		}

		if (/*team->GetScore() > 1200 || */team->GetWaitMatchTime() == 0 || team->GetWaitMatchTime() >= (UInt32)now.Sec())
		{
			continue;
		}

		if (!OnStartRaceWithRobot(team))
		{
			OnJoinMatchPool(team);
		}
	}

	return true;
}

bool PvPMatchService::OnTeamStartMatch(MatchTeam* team)
{
    if (!team)
    {
        return false;
    }

    if (!AddTeam(team))
    {
        return false;
    }

    CountTeamMatchScore(team);

    DebugLogStream << "team(" << team->GetID() << ") start matching, score(" << team->GetScore() << ")." << LogStream::eos;

    if (!OnJoinMatchPool(team))
    {
        ErrorLogStream << "team(" << team->GetID() << ") add to match pool failed." << LogStream::eos;
        DelTeam(team->GetID());
        return false;
    }

    return true;
}

bool PvPMatchService::OnJoinMatchPool(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	Match::MatchKey key;
	key.score = team->GetScore();
	if (m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
	{
		team->SetMatching(true);
		team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + GetWaitMatchUntilRobotSec());
	}

	for (auto member : team->GetTeamMembers())
	{
		member->SetStartTime(CURRENT_TIME.Sec());
	}

	return true;
}

UInt32 PvPMatchService::OnTeamCancelMatch(ObjID_t teamId)
{
    auto team = FindTeam(teamId);
    if (!team)
    {
        return ErrorCode::MATCH_CANCLE_NOT_MATCHING;
    }

    if (team->GetRaceID())
    {
        return ErrorCode::MATCH_CANCLE_RACING;
    }

    InfoLogStream << "team(" << teamId << ") cancel match." << LogStream::eos;

    DelTeam(teamId);
    return ErrorCode::SUCCESS;
}

bool PvPMatchService::OnMatchSuccess(VMatchTeam allTeams)
{
    auto race = CreateRace();
    race->SetID(MatchServiceMgr::Instance()->GenRaceID());

    race->OnCreate(allTeams);

    AddRace(race);

	auto members = race->GetAllMember();
    CLProtocol::MatchServerQueryPlayerInfoReq req;
    req.sessionId = race->GetID();
	req.type = GetMatchQueryType();
    UInt8 seat = 0;

	UInt32 index = 0;
    for (auto member : members)
    {
        if (!member)
        {
            continue;
        }

		index++;
		member->SetSeat(seat++);

#ifdef _DEBUG
		if (GetRaceType() == RT_SCORE_WAR && index == members.size() / 2)
		{
			seat = 3;
		}
#endif
        
        req.roleId = member->GetID();
        MSRouter::Instance()->SendToWorld(member->GetWsID(), req);

		CLProtocol::MatchServerPlayerMatchSuccessNotify notify;
		notify.zoneId = CL_GET_ZONEID_FROM_NODEID(member->GetWsID());
		notify.roleId = member->GetID();
		notify.type = member->GetPkType();
		notify.seasonLevel = member->GetSeasonLevel();
		notify.matchScore = member->GetScore();
		notify.usedTime = (UInt32)(CURRENT_TIME.Sec() - member->GetStartTime());
		if (seat == 1)
		{
			notify.opponentId = members.size() == 1 ? 0 : members[1]->GetID();
		}
		else
		{
			notify.opponentId = members[0]->GetID();
		}

		MSRouter::Instance()->SendToWorld(member->GetWsID(), notify);
    }

    return true;
}

void PvPMatchService::OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info)
{
    auto race = FindRace(sessionId);
    if (!race)
    {
        return;
    }

    InfoLogStream << "race (" << sessionId << ") recv player( " << roleId << ") race info result(" << result << ")." << LogStream::eos;

    // 获取玩家信息失败
    if (result != 0)
    {
        ErrorLogStream << "player(" << roleId << ") is offline, match result (" << sessionId << ")." << LogStream::eos;

		DismissMatchRace(race, true);
		MatchServiceMgr::Instance()->OnPlayerCancelMatch(roleId);
        return;
    }

    auto member = race->FindMember(roleId);
    if (!member)
    {
        ErrorLogStream << "race(" << sessionId << ") can't find member(" << roleId << ")." << LogStream::eos;
        return;
    }

    member->SetRaceInfo(info);

    if (!race->IsAllPlayerHasRaceInfo())
    {
        return;
    }

	// 只有一个玩家，说明还有一个是机器人
	if (race->GetAllMember().size() == 1)
	{
		auto aiHard = GenRobotAIHard(member, info.level);
		auto attrHard = GenRobotAttrHard(member, info.level);
		Robot* robot = RobotMgr::Instance()->GenRobot(GetType(), attrHard, info.level);
		if (!robot || aiHard == ROBOT_AI_INVALID)
		{
			ErrorLogStream << "player(" << roleId << ") gen robot with level(" << info.level << ") failed." << LogStream::eos;

			// 重新开始匹配
			DismissMatchRace(race, true);
			return;
		}
		race->SetRobot(robot);
		race->SetRobotAI(aiHard);

		if (race->GetRobotMatchScore() == 0)
		{
			race->SetRobotMatchScore(GenRobotMatchScore(info.matchScore));
		}
		
		if (race->GetRobotSeasonLevel() == 0)
		{
			race->SetRobotSeasonLevel(_GenRobotSeasonLevel(info.seasonLevel));
		}
	}

    // 到这里，说明所有玩家的比赛信息都已经取过来了
    CLProtocol::MatchSvrStartGameReq startReq;
    startReq.raceType = GetRaceType();
    startReq.gamesessionID = race->GetID();
    for (auto member : race->GetAllMember())
    {
        if (!member)
        {
            continue;
        }

        startReq.gamesessionFighter.push_back(member->GetFighter());
    }

    RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
    if (!relaySession)
    {
        // 没有有用的RelayServer
        ErrorLogStream << "no useable relayserver." << LogStream::eos;

        // 重新开始匹配
        DismissMatchRace(race, false);
        return;
    }

    InfoLogStream << "race (" << sessionId << ") waiting relay server(" << relaySession->GetNodeID() << ") return." << LogStream::eos;

    relaySession->SendProtocol(startReq);

    race->SetRelayServerID(relaySession->GetNodeID());
    race->ChangeState(MRS_WAITING_RELAY_SERVER_RES);
}

void PvPMatchService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
    auto race = FindRace(gamesessionID);
    if (!race)
    {
        return;
    }

    InfoLogStream << "race (" << gamesessionID << ") recv start response, result(" << result << ") abality(" << abality << ")." << LogStream::eos;

    if (result != 0)
    {
        ErrorLogStream << "relay server return error(" << result << "), match(" << gamesessionID << ")." << LogStream::eos;
        DismissMatchRace(race, true);
        return;
    }

    race->ChangeState(MRS_RACE);

    // 更新RelayServer负载
    RelaySessionMgr::Instance()->UpdateRelayServerAbality(race->GetRelayServerID(), abality);

    CLProtocol::WorldNotifyRaceStart raceStartInfo;
	raceStartInfo.raceType = GetRaceType();
    raceStartInfo.session = gamesessionID;
    raceStartInfo.addr = address;
	raceStartInfo.dungeonID = GenPkDungeon();
	raceStartInfo.battleFlag = MSApplication::Instance()->GetBattleFlag();

	race->SetDungeonID(raceStartInfo.dungeonID);

	CLProtocol::MatchSvrNotifyRacePlayerInfo notify;
	notify.gamesessionID = gamesessionID;
	notify.pkDungeonId = raceStartInfo.dungeonID;
	notify.battleFlag = raceStartInfo.battleFlag;

	auto members = race->GetAllMember();
	if (members.empty())
	{
		ErrorLogStream << "match(" << gamesessionID << ") members is empty." << LogStream::eos;
		DismissMatchRace(race, true);
		return;
	}

	auto leader = members[0];
    for (auto member : race->GetAllMember())
    {
        if (!member)
        {
            continue;
        }

        // 这里要检查玩家是否存在
        raceStartInfo.players.push_back(member->GetRaceInfo());

		notify.pkType = member->GetPkType();
		notify.players.push_back(member->GetRaceInfo());
    }

	if (race->GetRobot())
	{
		RobotMgr::Instance()->GenRobotEquipStrengthen(race->GetRobot());
		auto raceInfo = race->GetRobot()->raceInfo;
		raceInfo.robotAIType = race->GetRobotAI();
		raceInfo.pos = 1;
		raceInfo.matchScore = race->GetRobotMatchScore();
		raceInfo.pkValue = _GenRobotPkValue(leader->GetRaceInfo().pkValue);
		raceInfo.seasonLevel = race->GetRobotSeasonLevel();

		if (GetType() == MST_SEASON_PVP && leader->GetFightCount() < 3)
		{
			raceInfo.robotHard = (UInt8)leader->GetFightCount() + 1;
		}

		raceStartInfo.players.push_back(raceInfo);
		notify.players.push_back(raceInfo);

		InfoLogStream << "race (" << gamesessionID << ") add robot(" << raceInfo.roleId << ")." << LogStream::eos;
	}

    for (auto member : race->GetAllMember())
    {
        if (!member)
        {
            continue;
        }

        raceStartInfo.roleId = member->GetID();
        InfoLogStream << "race (" << gamesessionID << ") send raceinfo to player(" << member->GetID() << ")." << LogStream::eos;
        MSRouter::Instance()->SendToWorld(member->GetWsID(), raceStartInfo);
    }

	OnRaceStarted(gamesessionID);

	MSRouter::Instance()->SendToRelay(race->GetRelayServerID(), notify);

	// 战斗初始包
	{
		CLProtocol::RelaySvrRaceInitPacket init;
		init.raceId = gamesessionID;
		if (raceStartInfo.Encode(init.packet))
		{
			MSRouter::Instance()->SendToRelay(race->GetRelayServerID(), init);
		}
	}
}

bool PvPMatchService::OnStartRaceWithRobot(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	if (m_matchAlgorithm)
	{
		m_matchAlgorithm->DelTeam(team->GetID());
	}
	team->SetMatching(false);

	VMatchTeam teams;
	teams.push_back(team);

	return OnMatchSuccess(teams);
}

RobotAIHardType PvPMatchService::GenRobotAIHard(MatchTeamMember* member, UInt16 level)
{

#ifdef _DEBUG
	// 指定了机器人的ai
	if (RobotMgr::Instance()->GetSepcifyRobotAI() > ROBOT_AI_INVALID)
	{
		return RobotMgr::Instance()->GetSepcifyRobotAI();
	}
#endif

	if (!member)
	{
		return ROBOT_AI_INVALID;
	}

	if (GetType() == MST_GUILD_BATTLE && level > 50)
	{
		return ROBOT_AI_GUILD_BATTLE;
	}

	if (member->GetFightCount() == 0)
	{
		return ROBOT_AI_FISH;
	}
	else if (member->GetFightCount() < 3)
	{
		return ROBOT_AI_EASY;
	}
	else if (member->GetScore() <= 1300)
	{
		return ROBOT_AI_NORMAL;
	}
	else if (member->GetScore() <= 2000)
	{
		return ROBOT_AI_HARD;
	}

	return ROBOT_AI_MASTER;
}

RobotAttrHardType PvPMatchService::GenRobotAttrHard(MatchTeamMember* member, UInt16 level)
{
	if (!member)
	{
		return ROBOT_ATTR_INVALID;
	}

	if (GetType() == MST_GUILD_BATTLE && level > 50)
	{
		return ROBOT_ATTR_HARD;
	}

	auto seasonLevelData = SeasonLevelDataEntryMgr::Instance()->FindEntry(member->GetSeasonLevel());
	if (!seasonLevelData)
	{
		return ROBOT_ATTR_EASY;
	}

	if (seasonLevelData->mainLevel <= SML_SILVER)
	{
		return ROBOT_ATTR_EASY;
	}
	else if (seasonLevelData->mainLevel == SML_GOLD)
	{
		return ROBOT_ATTR_NORMAL;
	}
	else
	{
		return ROBOT_ATTR_HARD;
	}
}

UInt32 PvPMatchService::GenRobotMatchScore(UInt32 score)
{
	int minScore = score - 100;
	minScore = minScore < 1000 ? 1000 : minScore;
	minScore = minScore > 1800 ? 1800 : minScore;
	int maxScore = score + 100;
	maxScore = maxScore > 1800 ? 1800 : maxScore;
	minScore = minScore < 1000 ? 1000 : minScore;
	return (UInt32)Avalon::Random::RandBetween(minScore, maxScore);
}

void PvPMatchService::AddHistory(ObjID_t player1, ObjID_t player2)
{
	if (player1 > 0)
	{
		auto& history = m_mathchHistory[player1];
		if (history.size() >= MAX_PVP_HISTORY_OPPONENT)
		{
			history.pop_front();
		}

		history.push_back(player2);
	}

	if (player2 > 0)
	{
		auto& history = m_mathchHistory[player2];
		if (history.size() >= MAX_PVP_HISTORY_OPPONENT)
		{
			history.pop_front();
		}

		history.push_back(player1);
	}
}

std::vector<ObjID_t> PvPMatchService::GetLastOpponent(ObjID_t id)
{
	std::vector<ObjID_t> opponentList;

	auto itr = m_mathchHistory.find(id);
	if (itr == m_mathchHistory.end())
	{
		return opponentList;
	}

	for (auto id : itr->second)
	{
		opponentList.push_back(id);
	}

	return opponentList;
}

UInt32 PvPMatchService::GenPkDungeon()
{
	auto size = m_pkDungeonIdList.size();
	if (size == 0)
	{
		return 0;
	}

	return m_pkDungeonIdList[Avalon::Random::RandBetween(0, size - 1)];
}

UInt32 PvPMatchService::_GenRobotPkValue(UInt32 pkValue)
{
	return PkLevelDataEntryMgr::Instance()->GenRobotPkValue(pkValue);
}

UInt32 PvPMatchService::_GenRobotSeasonLevel(UInt32 seasonLevel)
{
	UInt32 maxDiffLevel = 15;
	auto data = SeasonLevelDataEntryMgr::Instance()->FindEntry(seasonLevel);
	if (!data)
	{
		return seasonLevel;
	}

	std::deque<UInt32> allPossiableSeasonLevel;
	allPossiableSeasonLevel.push_back(seasonLevel);
	auto tmpLevel = seasonLevel;
	auto prevSeason = SeasonLevelDataEntryMgr::Instance()->FindEntry(tmpLevel);
	auto nextSeason = prevSeason;

	// 先计算向上的等级
	for (UInt32 i = 0; i < maxDiffLevel; i++)
	{
		prevSeason = SeasonLevelDataEntryMgr::Instance()->FindEntry(prevSeason->preId);
		if (!prevSeason)
		{
			break;
		}

		tmpLevel = prevSeason->id;
		allPossiableSeasonLevel.push_front(tmpLevel);
	}

	// 再计算向下的等级
	for (UInt32 i = 0; i < maxDiffLevel; i++)
	{
		nextSeason = SeasonLevelDataEntryMgr::Instance()->FindEntry(nextSeason->afterId);
		if (!nextSeason)
		{
			break;
		}

		tmpLevel = nextSeason->id;
		allPossiableSeasonLevel.push_front(tmpLevel);
	}

	if (allPossiableSeasonLevel.empty())
	{
		return seasonLevel;
	}

	return allPossiableSeasonLevel[Avalon::Random::RandBetween(0, allPossiableSeasonLevel.size() - 1)];
}

UInt32 PvPMatchService::GetWaitMatchUntilRobotSec(SeasonMainLevel level) const
{
	if (level < SML_GOLD)
	{
		return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PK_USE_ROBOT_WAIT_SEC);
	}

	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PK_GOLD_USE_ROBOT_WAIT_SEC);
}