#include "BSTeam.h"
#include "BSTeamMgr.h"
#include "GameZoneMgr.h"
#include "BSApplication.h"

BSTeamMember::BSTeamMember()
{
	m_AccId = 0;
	m_PlayerId = 0;
	m_PlayerOccu = 0;
	m_ConnId = 0;
	m_hasRaceInfo = false;
	m_RaceId = 0;
	m_robot = false;
	m_playerStatus = BPS_ALIVE;
	m_kills = 0;
	m_protectTimestamp = 0;
	m_matchTimestamp = 0;
	m_IsPk = false;
}

BSTeamMember::~BSTeamMember()
{

}

void BSTeamMember::AddNoWarCD(UInt64 roleId)
{
	m_noWarCDMap[roleId] = CURRENT_TIME.Sec();
}

bool BSTeamMember::IsNoWarCD(UInt64 roleId)
{
	auto iter = m_noWarCDMap.find(roleId);
	if (iter == m_noWarCDMap.end())
	{
		return false;
	}

	// ÃâÕ½CD30Ãë
	return CURRENT_TIME.Sec() - iter->second < BSApplication::Instance()->GetNoWarCD();
}

void BSTeamMember::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_robot)
	{
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(m_ConnId);
	if (zone != NULL)
	{
		zone->SendToPlayer(m_PlayerId, protocol);
		InfoLogStream << "send msg to player:" << m_PlayerId << ", player name:" << m_PlayerName << ", msg id:" << protocol.GetProtocolID() << LogStream::eos;
		return;
	}

	InfoLogStream << "Not Find Zone Player:" << m_PlayerId << " PlayerName:" << m_PlayerName << " ConnId:" << m_ConnId << LogStream::eos;
}

void BSTeamMember::SendToSelfScene(Avalon::Protocol& protocol)
{
	if (m_robot)
	{
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(m_ConnId);
	if (zone != NULL)
	{
		zone->TransmitToScene(protocol);
		InfoLogStream << "send msg to self scene:" << m_PlayerId << ", player name:" << m_PlayerName << ", msg id:" << protocol.GetProtocolID() << LogStream::eos;
		return;
	}

	InfoLogStream << "Not Find Zone Player:" << m_PlayerId << " PlayerName:" << m_PlayerName << " ConnId:" << m_ConnId << LogStream::eos;
}

BSTeam::BSTeam()
{
	m_Id = 0;
	m_TeamStatus = BTS_ALIVE;
}

BSTeam::~BSTeam()
{

}

void BSTeam::AddMember(const BSTeamMember& member)
{
	m_Members.push_back(member);
}

void BSTeam::DelMember(UInt64 playerId)
{
	for (VBSTeamMember::iterator iter = m_Members.begin(); iter != m_Members.end(); ++iter)
	{
		if (playerId == iter->m_PlayerId)
		{
			m_Members.erase(iter);
			break;
		}
	}
}

BSTeamMember* BSTeam::FindMember(UInt64 playerId)
{
	for (VBSTeamMember::iterator iter = m_Members.begin(); iter != m_Members.end(); ++iter)
	{
		if (playerId == iter->m_PlayerId)
		{
			return &(*iter);
		}
	}

	return NULL;
}

void BSTeam::Dismiss()
{
	for (auto& member : m_Members)
	{
		BSTeamMgr::Instance()->DelPlayerMap(member.m_PlayerId);
	}

	m_Id = 0;
	m_Members.clear();
}

void BSTeam::OnMatch()
{
	for (auto& member : m_Members)
	{
		BSTeamMgr::Instance()->DelPlayerMap(member.m_PlayerId);
	}
}

void BSTeam::NotifyMsgToAll(Avalon::Protocol& protocol)
{
	for (auto& member : m_Members)
	{
		if (member.BeRobot())
		{
			continue;
		}

		//InfoLogStream << "notify msg to player:" << member.m_PlayerId << ", msg id:" << protocol.GetProtocolID() << LogStream::eos;

		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(member.m_ConnId);
		if (NULL != zone)
		{
			InfoLogStream << "find it and notify msg to player:" << member.m_PlayerId << ", msg id:" << protocol.GetProtocolID() << LogStream::eos;
			zone->SendToPlayer(member.m_PlayerId, protocol);
		}
	}
}

