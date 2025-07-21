#include "DungeonRaceMgr.h"
#include <CLGameSessionProtocol.h>
#include "CLRecordClient.h"
#include "DSRouter.h"

DungeonRaceMgr::DungeonRaceMgr()
{
    m_dungeonIdSeed = 0;
}

DungeonRaceMgr::~DungeonRaceMgr()
{

}

ObjID_t	DungeonRaceMgr::GenRaceId()
{
	// ����Ψһ�ı���ID,������Ϊ��Ӧ�ֶε�λ��
	// NodeID(48-63) +  ʱ��(s)(32-67) + �����(0-32)
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 randNum = (UInt64)rand();
	UInt64 raceID = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
	return raceID;
}

void DungeonRaceMgr::OnTick(const Avalon::Time& now)
{
    std::vector<DungeonRace*> allRaceNeedToDestory;
    for (auto itr : m_dungeones)
    {
        auto race = itr.second;
        if (!race)
        {
            continue;
        }

        race->Tick(now);

        if (race->IsTimeToDestory(now))
        {
            allRaceNeedToDestory.push_back(race);
        }

    }

    for (auto race : allRaceNeedToDestory)
    {
        DestoryDungeonRace(race);
    }
}

DungeonRace* DungeonRaceMgr::CreateDungeonRace(UInt32 dungeonId)
{
    DungeonDataEntry* dataEntry = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
    if (dataEntry == NULL)
    {
        return NULL;
    }

    DungeonRace* race = new DungeonRace();
    if (!race)
    {
        return NULL;
    }

    race->SetID(GenDungeonRaceID());
    race->SetDungeonId(dungeonId);
    race->SetDungeonData(dataEntry);
	race->SetSessionId(DungeonRaceMgr::Instance()->GenRaceId());

    m_dungeones[race->GetID()] = race;
	AddDungeonRaceToSessionMgr(race->GetSessionId(), race);

	InfoLogStream << "create race(" << race->GetID() << ") dungeon(" << dungeonId << "), sessionId(" << race->GetSessionId() << ")." << LogStream::eos;

    return race;
}

void DungeonRaceMgr::DestoryDungeonRace(DungeonRace* dungeon)
{
    if (!dungeon)
    {
        return;
    }

	DebugLogStream << "start to destory dungeon race(" << dungeon->GetID() << "), session(" << dungeon->GetSessionId() << ")." << LogStream::eos;

	dungeon->OnDestory();

    m_dungeones.erase(dungeon->GetID());
	if (dungeon->GetSession() > 0)
	{
		m_session2Dungeon.erase(dungeon->GetSession());
	}

    for (auto itr : dungeon->GetPlayerList())
    {
        auto roleId = itr.first;
        auto dungeonItr = m_roleId2Dungeon.find(roleId);
        if (dungeonItr == m_roleId2Dungeon.end())
        {
            continue;
        }

        if (dungeonItr->second != dungeon)
        {
            InfoLogStream <<"player(" << roleId << ")'s dungeon race is not (" << dungeon->GetID() << ")." << LogStream::eos;
            continue;
        }

        m_roleId2Dungeon.erase(dungeonItr);

        DebugLogStream << "remove player(" << roleId << ") from dungeon race(" << dungeon->GetID() << ")." << LogStream::eos;
    }

    DebugLogStream << "destory dungeon race(" << dungeon->GetID() << ") finish." << LogStream::eos;

    delete dungeon;
}

DungeonRace* DungeonRaceMgr::FindDungeonRaceById(UInt64 id)
{
    auto itr = m_dungeones.find(id);
    return itr == m_dungeones.end() ? NULL : itr->second;
}

DungeonRace* DungeonRaceMgr::FindDungeonRaceByRoleId(UInt64 roleId)
{
    auto itr = m_roleId2Dungeon.find(roleId);
    return itr == m_roleId2Dungeon.end() ? NULL : itr->second;
}

DungeonRace* DungeonRaceMgr::FindDungeonRaceBySession(ObjID_t session)
{
	auto itr = m_session2Dungeon.find(session);
	return itr == m_session2Dungeon.end() ? NULL : itr->second;
}

bool DungeonRaceMgr::IsPlayerInRace(ObjID_t roleid)
{
	auto race = FindDungeonRaceByRoleId(roleid);
	if (!race)
	{
		return false;
	}

	auto dungeonPlayer = race->FindDungeonPlayer(roleid);
	if (!dungeonPlayer || dungeonPlayer->IsLeave())
	{
		return false;
	}

	return true;
}

void DungeonRaceMgr::JoinDungeonRace(DungeonRace* race, const DungeonRacePlayerInfo& info)
{
	auto roleId = info.raceInfo.roleId;

    if (!race)
    {
        ErrorLogStream << "player(" << roleId << ") join dungeon race but it is NULL." << LogStream::eos;
        return;
    }

    race->AddPlayer(info);
    m_roleId2Dungeon[roleId] = race;

    InfoLogStream << "player(" << roleId << ") join dungeon race(" << race->GetID() << ") isAssist(" << info.isAssist << ") dungeon(" << race->GetDungeonID() << ")." << LogStream::eos;
}

void DungeonRaceMgr::LeaveDungeonRace(UInt64 roleId, bool isDisconnect)
{
    DungeonRace* race = FindDungeonRaceByRoleId(roleId);
    if (!race)
    {
        return;
    }

	InfoLogStream << "player(" << roleId << ") leave dungeon race(" << race->GetID() << "), sessionId : " << race->GetSessionId() << ", disconnect(" << isDisconnect << ")" << LogStream::eos;

	if (race->GetTeamID() > 0)
	{
		///*auto team = WSTeamMgr::Instance()->FindTeam(race->GetTeamID());
		//if (team && race->GetID() == team->GetRaceID())
		//{
		//	team->OnChangePlayerRacingStatus(roleId, false);
		//}*/
	}

	race->OnPlayerLeave(roleId, isDisconnect);

    if (!isDisconnect && !race->IsPlayerRaceEnd(roleId))
    {
		// ��������������뿪���³�
   //     auto team = WSTeamMgr::Instance()->FindTeam(race->GetTeamID());
   //     if (team)
   //     {
			//// ֪ͨrelay server����뿪����
			//Protocol::SceneServerPlayerLeaveRace notify;
			//notify.raceId = race->GetSession();
			//notify.roleId = roleId;
			//Router::SendToRelay(race->GetRelayServerID(), notify);

			//do 
			//{
			//	// ������������˲��뿪����
			//	if (WSApplication::Instance()->StayTeamWhenLeaveRace())
			//	{
			//		break;
			//	}

			//	// ��Ԩ�������Զ��˳����ܣ����뿪����
			//	if (race->IsHell() && team->IsSetOption(TO_HELL_AUTO_CLOSE))
			//	{
			//		break;
			//	}

			//	team->RemoveMember(roleId);
			//} while (0);
			//
   //     }

		// ֪ͨrelay server����뿪����
		CLProtocol::SceneServerPlayerLeaveRace notify;
		notify.raceId = race->GetSession();
		notify.roleId = roleId;
		DSRouter::Instance()->SendToRelay(race->GetRelayServerID(), notify);
    }

    if (race->GetStatus() != DRS_WAIT_CLOSE && race->IsAllPlayerLeave())
    {
		race->SendDungeonLogWhenAllPlayerLeave();
        race->SetStatus(DRS_WAIT_CLOSE);

        Avalon::Time closeTime = Avalon::Time::CurrentTime();

        if (!race->HasPlayerDisconnect())
        {
            // ��������˶��������뿪�ģ���ô��ֱ�ӿ��Դݻ���
			race->SetDestoryTime(closeTime);
        }
        else
        {
            closeTime += TIME_TO_WAIT_CLOSE_WHEN_PLAYER_LEAVE;
			race->SetDestoryTime(closeTime);
        }
        
		if (race->GetTeamID())
		{
			//auto team = WSTeamMgr::Instance()->FindTeam(race->GetTeamID());
			//if (team && race->GetID() == team->GetRaceID())
			//{
			//	team->SetStatus(TEAM_STATUS_IDEL);
			//}
		}
    }
}

void DungeonRaceMgr::AddDungeonRaceToSessionMgr(ObjID_t session, DungeonRace* race)
{
	m_session2Dungeon[session] = race;
}

UInt32 DungeonRaceMgr::GenHellDungeonID(UInt32 entryDungeonId)
{
	auto entryDungeonData = DungeonDataEntryMgr::Instance()->FindEntry(entryDungeonId);
	if (!entryDungeonData)
	{
		ErrorLogStream << "can't find hell entry(" << entryDungeonId << ")." << LogStream::eos;
		return 0;
	}

	if (!entryDungeonData->IsHellEntry())
	{
		ErrorLogStream << "invalid hell entry(" << entryDungeonId << ")." << LogStream::eos;
		return 0;
	}

	return DungeonDataEntryMgr::Instance()->GenHellDungeonByLevel(entryDungeonData->hellSplitLevel, entryDungeonData->hardType);
}

UInt32 DungeonRaceMgr::GenDungeonIDByEntry(UInt32 entryDungeonId)
{
	auto entryDungeonData = DungeonDataEntryMgr::Instance()->FindEntry(entryDungeonId);
	if (!entryDungeonData)
	{
		ErrorLogStream << "can't find hell entry(" << entryDungeonId << ")." << LogStream::eos;
		return 0;
	}

	return DungeonDataEntryMgr::Instance()->GetDungeonIdByEntryId(entryDungeonId);
}

void  DungeonRaceMgr::OnRelayServerShutDown(UInt32 serverID)
{
	for (auto iter : m_dungeones)
	{
		DungeonRace* race = iter.second;
		if (!race)
		{
			continue;
		}
		if (race->GetRelayServerID() != serverID)
		{
			continue;
		}
		race->SetStatus(DRS_WAIT_CLOSE);
		race->SetDestoryTime(Avalon::Time::CurrentTime());
	}
}

UInt64 DungeonRaceMgr::GenDungeonRaceID()
{
	//return CLRecordClient::Instance()->GenGuid();
	if (m_dungeonIdSeed == 0xFFFFFFFFFFFFFFFF)
	{
		m_dungeonIdSeed = 0;
	}
	return ++m_dungeonIdSeed;
}