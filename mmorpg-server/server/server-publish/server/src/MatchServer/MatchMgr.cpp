#include "MatchMgr.h"
#include <CLGameSessionProtocol.h>
#include "CLDefine.h"
#include "RelaySessionMgr.h"
#include "MSRouter.h"
#include "CLErrorCode.h"
#include "MSApplication.h"

MatchMgr::MatchMgr()
{

}

MatchMgr::~MatchMgr()
{

}

bool MatchMgr::Init()
{
    if (!m_tradAlgorithm.Initialize())
    {
        ErrorLogStream << "init tradition match algorithm failed." << LogStream::eos;
        return false;
    }

	return true;
}

void MatchMgr::Final()
{

}

void MatchMgr::OnTick(const Avalon::Time& now)
{
	OnMatching();

    // 检查玩家状态
    {
        Avalon::Time now = Avalon::Time::CurrentTime();
        auto itr = m_MatchPlayers.begin();
        auto next = m_MatchPlayers.begin();
        for (; itr != m_MatchPlayers.end(); itr = next)
        {
            ++next;

            MatchPlayer* player = itr->second;
            if (!player || player->GetMatchState() != MS_MATCHING)
            {
                continue;
            }

            if (player->IsMatchTimeOut(now)) {
                InfoLogStream << "player(" << player->GetID() << ") match timeout." << LogStream::eos;

                CLProtocol::WorldMatchStartRes res;
                res.result = ErrorCode::MATCH_TIMEOUT;
                MSRouter::Instance()->SendToPlayer(player->GetServerID(), player->GetID(), res);

                DelMatchPlayerByID(player->GetID());
            }
        }
    }

    // 检查比赛是否超时
    {
        Avalon::Time now = Avalon::Time::CurrentTime();

        auto itr = m_MatchResults.begin();
        auto next = m_MatchResults.begin();
        for (; itr != m_MatchResults.end(); itr = next)
        {
            ++next;

            auto matchResult = itr->second;
            if (!matchResult)
            {
                continue;
            }

            if (!matchResult->IsTimeOut(now))
            {
                continue;
            }

            if (matchResult->GetState() == MRS_RACING)
            {
                OnRaceTimeOut(matchResult);
            }
            else
            {
                DismissMatchResult(matchResult);
            }
        }
    }
}

void MatchMgr::OnMatching()
{
    // 传统匹配
    m_tradAlgorithm.Update();
    VTraditionMatchResult tradMatchResults = m_tradAlgorithm.GetMatchResult();
    for (auto& tradResult : tradMatchResults)
    {
        VMatchPlayer allRacePlayers;

        for (auto roldId : tradResult.roleIds)
        {
            MatchPlayer* player = FindMatchPlayerByID(roldId);
            if (player)
            {
                allRacePlayers.push_back(player);
            }
        }

        if (!OnMatchSuccess(allRacePlayers))
        {
            // 开始战斗失败，重新放回匹配池
            for (auto racePlayer : allRacePlayers)
            {
                if (!racePlayer)
                {
                    continue;
                }

                PlayerStartMatching(racePlayer);
            }
        }
	}
}

UInt32 MatchMgr::AddMatchPlayer(MatchPlayer* player)
{
	if (!player)
	{
		return ErrorCode::MATCH_START_SYSTEMERROR;
	}

	if (FindMatchPlayerByID(player->GetID()))
	{
        InfoLogStream << "repeat add match player(" << player->GetID() << ")." << LogStream::eos;
		return ErrorCode::MATCH_START_REPEAT;
	}

    if (!PlayerStartMatching(player))
    {
        InfoLogStream << "player(" << player->GetID() << ") start match failed." << LogStream::eos;
        return ErrorCode::MATCH_START_SYSTEMERROR;
    }

	m_MatchPlayers[player->GetID()] = player;

    InfoLogStream << "player(" << player->GetID() << ") start matching." << LogStream::eos;

	return 0;
}

bool MatchMgr::DelMatchPlayerByID(ObjID_t id)
{
    PlayerStopMatching(id);

	MatchPlayer* player = FindMatchPlayerByID(id);
	if (!player)
	{
		return false;
	}

	m_MatchPlayers.erase(id);
	delete player;

	return true;
}

MatchPlayer* MatchMgr::FindMatchPlayerByID(ObjID_t id)
{
	MapMatchPlayer::iterator itr = m_MatchPlayers.find(id);
	return itr == m_MatchPlayers.end() ? NULL : itr->second;
}

bool MatchMgr::PlayerStartMatching(MatchPlayer* player)
{
    if (!player)
    {
        return false;
    }

    if (!m_tradAlgorithm.AddPlayerToPool(player->GetID(), player->GetMatchScore()))
    {
        return false;
    }

    player->SetMatchState(MS_MATCHING);
    return true;
}

void MatchMgr::PlayerStopMatching(ObjID_t roldId)
{
    m_tradAlgorithm.RemovePlayer(roldId);
}

void MatchMgr::AddMatchResult(MatchResult* result)
{
    if (!result)
    {
        return;
    }

    m_MatchResults[result->GetID()] = result;
    
}

void MatchMgr::AddMatchResultToRelayServerSet(MatchResult* result)
{
    if (!result)
    {
        return;
    }

    m_RelayServerMatchResultSet[result->GetRelayServerID()][result->GetID()] = result;
}

MatchResult* MatchMgr::FindMatchResult(ObjID_t id)
{
	MapMatchResult::iterator itr = m_MatchResults.find(id);
	return itr == m_MatchResults.end() ? NULL : itr->second;
}

void MatchMgr::DeleteMatchResult(ObjID_t id)
{
    MatchResult* result = FindMatchResult(id);
    if (!result)
    {
        return;
    }

    m_MatchResults.erase(id);

    m_RelayServerMatchResultSet[result->GetRelayServerID()].erase(id);

    delete result;
}

bool MatchMgr::IsPlayerCanStartMatch(ObjID_t id)
{
	if (FindMatchPlayerByID(id))
	{
        InfoLogStream << "Player(" << id << ") " << "is already matching." << LogStream::eos;
		return false;
	}

	return true;
}

bool MatchMgr::OnMatchSuccess(const VMatchPlayer& allPlayers)
{
    UInt64 gameSession = GenGameSession();
    MatchResult* matchResult = new MatchResult();
    if (!matchResult)
    {
        return false;
    }

    UInt8 seat = 0;
    for (UInt32 i = 0; i < allPlayers.size(); i++)
	{
        MatchPlayer* player = allPlayers[i];
		if (!player)
		{
            ErrorLogStream << "player == NULL" << LogStream::eos;
            delete matchResult;
			return false;
		}

		player->SetMatchState(MS_WAIT_WORLD_REP);
        InfoLogStream << "Session (" << gameSession <<  ") Player(" << player->GetID() << ") waiting for race info." << LogStream::eos;

		MatchFighter mFighter;
		mFighter.SetID(player->GetID());
		mFighter.SetPos(seat);
        seat += 5;

		matchResult->PushFighter(mFighter);
	}

	// 保存匹配结果
    matchResult->SetID(gameSession);
    matchResult->ChangeState(MRS_WAITING_PLAYER_INFO);
    AddMatchResult(matchResult);

    // 向WorldServer查询玩家信息
    CLProtocol::MatchServerQueryPlayerInfoReq req;
	req.type = MATCH_QUERY_RACE_INFO;
    req.sessionId = gameSession;
    for (auto player : allPlayers)
    {
        player->SetSessionID(gameSession);

        req.roleId = player->GetID();

        MSRouter::Instance()->SendToWorld(player->GetServerID(), req);
    }

	return true;
}

void MatchMgr::OnQueryPlayerInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info)
{
    InfoLogStream << "race (" << sessionId << ") recv player( " << roleId << ") race info result(" << result << ")." << LogStream::eos;

    MatchResult* matchResult = FindMatchResult(sessionId);
    if (!matchResult)
    {
        ErrorLogStream << "player(" << roleId << ") can't find match result (" << sessionId << ")." << LogStream::eos;
        return;
    }

    // 获取玩家信息失败
    if (result != 0)
    {
        ErrorLogStream << "player(" << roleId << ") is offline, match result (" << sessionId << ")." << LogStream::eos;

        // 把这个玩家从匹配池中去掉
        DelMatchPlayerByID(roleId);

        // 其它玩家重新开始匹配
        DismissMatchResult(matchResult);
        return;
    }

    matchResult->SetRaceInfo(roleId, info);
    if (!matchResult->IsAllPlayerHasRaceInfo())
    {
        return;
    }

    // 到这里，说明所有玩家的比赛信息都已经取过来了
    CLProtocol::MatchSvrStartGameReq startReq;
    startReq.raceType = RT_PK;
    startReq.gamesessionID = matchResult->GetID();
    VMatchPlayer allMatchPlayer;
    for (auto fighter : matchResult->GetFighters())
    {
        startReq.gamesessionFighter.push_back(fighter.GetFighter());

        // 这里顺便检查是不是所有的MatchPlayer都还存在
        MatchPlayer* matchPlayer = FindMatchPlayerByID(fighter.GetID());
        if (!matchPlayer || matchPlayer->GetMatchState() != MS_WAIT_WORLD_REP)
        {
            ErrorLogStream << "can't find match player(" << roleId << ") or match player state error , match result (" << sessionId << ")." << LogStream::eos;

            // 把这个玩家从匹配池中去掉
            DelMatchPlayerByID(fighter.GetID());

            // 其它玩家重新开始匹配
            DismissMatchResult(matchResult);

            return;
        }

        allMatchPlayer.push_back(matchPlayer);
    }

    RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
    if (!relaySession)
    {
        // 没有有用的RelayServer
        ErrorLogStream << "no useable relayserver." << LogStream::eos;

        // 重新开始匹配
        DismissMatchResult(matchResult, false);
        return;
    }


    InfoLogStream << "race (" << sessionId << ") waiting relay server(" << relaySession->GetNodeID() << ") return." << LogStream::eos;

    relaySession->SendProtocol(startReq);

    matchResult->SetRelayServerID(relaySession->GetNodeID());
    matchResult->ChangeState(MRS_WAITING_DELAY_SERVER_RES);
    AddMatchResultToRelayServerSet(matchResult);

    for (auto matchPlayer : allMatchPlayer)
    {
        matchPlayer->SetMatchState(MS_WAIT_RELAY_REP);
    }
}

void MatchMgr::DismissMatchResult(const MatchResult* matchResult, bool playerRematch)
{
    if (!matchResult)
    {
        return;
    }

    for (auto fighter : matchResult->GetFighters())
    {
        MatchPlayer* matchPlayer = FindMatchPlayerByID(fighter.GetID());
        if (!matchPlayer)
        {
            continue;
        }

        if (!playerRematch)
        {
            InfoLogStream << "race (" << matchResult->GetID() << ") dismissed, player(" << matchPlayer->GetID() << ") deleted." << LogStream::eos;

            DelMatchPlayerByID(fighter.GetID());
            continue;
        }

        InfoLogStream << "race (" << matchResult->GetID() << ") dismissed, player(" << matchPlayer->GetID() << ") restart match." << LogStream::eos;

        matchPlayer->SetMatchState(MS_MATCHING);
        matchPlayer->SetSessionID(0);
    }

    DeleteMatchResult(matchResult->GetID());
}

void MatchMgr::OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const std::vector<FightergResult>& results)
{
    InfoLogStream << "race (" << gamesessionID << ") recv race end, ret(" << ret << ")" << LogStream::eos;

    MatchResult* matchResult = FindMatchResult(gamesessionID);
    if (!matchResult)
    {
        ErrorLogStream << "can't find race (" << gamesessionID << ")" << LogStream::eos;
        return;
    }

    if (matchResult->GetState() != MRS_RACING)
    {
        ErrorLogStream << "race (" << gamesessionID << ") not racing, state(" << matchResult->GetState() << ")" << LogStream::eos;
        return;
    }

    /*for (auto& result : results)
    {
        MatchPlayer* player = FindMatchPlayerByID(result.roleId);
        if (!player)
        {
            InfoLogStream << "can't find player(" << result.roleId << LogStream::eos;
            continue;
        }

        CLProtocol::MatchServerRaceEnd endReq;
        endReq.type = PK_NORMAL_1V1;
        endReq.opponent = matchResult->GetOpponentInfo(result.roleId);
        endReq.result = result;
        MSRouter::Instance()->SendToWorld(player->GetServerID(), endReq);
    }*/

    // 解散队伍，把队伍中的玩家都删掉
    DismissMatchResult(matchResult, false);
}

void MatchMgr::OnRelayServerShutDown(UInt32 serverID)
{
    InfoLogStream << "relay server(" << serverID << ") shutdown." << LogStream::eos;

    auto itrSet = m_RelayServerMatchResultSet.find(serverID);
    if (itrSet == m_RelayServerMatchResultSet.end())
    {
        return;
    }

    auto matchResults = itrSet->second;
    for (auto itr : matchResults)
    {
        MatchResult* result = itr.second;
        if (!result)
        {
            ErrorLogStream << "race(" << itr.first << ") is null." << LogStream::eos;
            continue;
        }

        if (result->GetState() != MRS_RACING)
        {
            DismissMatchResult(result);
        }
        else
        {
            std::vector<FightergResult> fighters;
            OnRaceEnd(result->GetID(), gamesessionDismissType_rs_shutdown, fighters);
        }
    }
}

void MatchMgr::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
    InfoLogStream << "race (" << gamesessionID << ") recv start response, result(" << result << ") abality(" << abality << ")." << LogStream::eos;

	MatchResult* matchResult = FindMatchResult(gamesessionID);
	if (!matchResult)
	{
		ErrorLogStream << "can't find match(" << gamesessionID << ")" << LogStream::eos;
		return;
	}

    if (result != 0)
    {
        ErrorLogStream << "relay server return error(" << result << "), match(" << gamesessionID << ")." << LogStream::eos;
        DismissMatchResult(matchResult);
        return;
    }

    matchResult->ChangeState(MRS_RACING);

	// 更新RelayServer负载
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(matchResult->GetRelayServerID(), abality);

	CLProtocol::WorldNotifyRaceStart raceStartInfo;
    raceStartInfo.raceType = RT_PK;
	raceStartInfo.session = gamesessionID;
	raceStartInfo.addr = address;
	raceStartInfo.battleFlag = MSApplication::Instance()->GetBattleFlag();

	// 获取所有玩家信息
	VMatchPlayer allMatchPlayers;
	
	for (auto fighter : matchResult->GetFighters())
	{
		MatchPlayer* matchPlayer = FindMatchPlayerByID(fighter.GetID());
		if (!matchPlayer)
		{
			GameInfoLogStream("Match") << "(" << gamesessionID << ") can't find match player(" << fighter.GetID() << ")." << LogStream::eos;
			continue;
		}

		allMatchPlayers.push_back(matchPlayer);

		matchPlayer->SetMatchState(MS_RACING);

		raceStartInfo.players.push_back(fighter.GetRaceInfo());
	}

    for (auto matchPlayer : allMatchPlayers)
    {
        raceStartInfo.roleId = matchPlayer->GetID();
        InfoLogStream << "race (" << gamesessionID << ") send raceinfo to player(" << matchPlayer->GetID() << ")." << LogStream::eos;
        MSRouter::Instance()->SendToWorld(matchPlayer->GetServerID(), raceStartInfo);
    }
}

void MatchMgr::OnRaceTimeOut(MatchResult* matchResult)
{
    if (!matchResult)
    {
        return;
    }

	InfoLogStream << "(" << matchResult->GetID() << ") race time out."  << LogStream::eos;
	
    std::vector<FightergResult> fighters;
    OnRaceEnd(matchResult->GetID(), gamesessionDismissType_timeout, fighters);
}

UInt32 MatchMgr::OnCancelMatch(ObjID_t id)
{
	MatchPlayer* player = FindMatchPlayerByID(id);
	if (!player)
	{
		return ErrorCode::MATCH_CANCLE_NOT_MATCHING;
	}

	// 只有在匹配中的玩家能离开匹配
	if (player->GetMatchState() != MS_MATCHING)
	{
		return ErrorCode::MATCH_CANCLE_RACING;
	}

	DelMatchPlayerByID(id);

	InfoLogStream << "Player(" << id << ") Leave Matching." << LogStream::eos;

	return 0;
}

UInt64 MatchMgr::GenGameSession()
{
	// 生成唯一的比赛ID,括号内为相应字段的位数
	// NodeID(48-63) +  时间(s)(32-67) + 随机数(0-32)
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 randNum = (UInt64)rand();
	UInt64 raceID = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
	return raceID;
}