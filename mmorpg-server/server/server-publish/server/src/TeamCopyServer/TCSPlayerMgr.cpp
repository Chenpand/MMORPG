#include "TCSPlayerMgr.h"

#include <CLSysProtocol.h>
#include "TCSTeamMgr.h"
#include "TCSceneMgr.h"
#include "GameZoneMgr.h"

// 玩家过期时间
#define PLAYER_EXPIRE_INTERVAL  (10 * 60)

TCSPlayerMgr::TCSPlayerMgr()
{
	m_60sTimer.SetInterval(60 * 1000);
}

TCSPlayerMgr::~TCSPlayerMgr()
{
	for (auto& iter : m_PlayerVec)
	{
		if (iter)
		{
			CL_SAFE_DELETE(iter);
		}
	}

	m_PlayerVec.clear();
	m_PlayerMap.clear();
}

void TCSPlayerMgr::Init()
{
	RegisterProtocols();
}

void TCSPlayerMgr::OnTick(const Avalon::Time& now)
{
	// 玩家过期，在团本服上析构玩家，在队伍中退出
	std::map<UInt64, UInt64>::iterator iter = m_PlayerExpireMap.begin();
	while (iter != m_PlayerExpireMap.end())
	{
		// 过期时间x分钟
		if (now.Sec() >= iter->second + PLAYER_EXPIRE_INTERVAL)
		{
			TCSPlayer* player = FindTCSPlayer(iter->first);
			if (player != NULL)
			{
				TCSceneMgr::Instance()->RemoveToPrepareScene(player->GetTeamType(), iter->first);
			}
			TCSTeamMgr::Instance()->OnPlayerQuit(iter->first);
			TCSceneMgr::Instance()->RemoveSyncPacket(iter->first);
			RemoveTCSPlayer(iter->first);

			iter = m_PlayerExpireMap.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	if (m_60sTimer.IsTimeout(now))
	{
		UInt32 playerNum = 0;
		for (auto& iter : m_PlayerVec)
		{
			if (NULL == iter || PlayerIsExpire(iter->GetID()))
			{
				continue;
			}

			// 检查玩家的区服是否断开，如果区服直接断开，则将玩家置成离开
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iter->GetConnId());
			if (NULL == zone)
			{
				AddExpire(iter->GetID(), CURRENT_TIME.Sec());

				InfoLogStream << "zone:" << iter->GetZoneID() << " disconnect player:" << iter->GetID() << " expire" << LogStream::eos;

				continue;
			}

			++playerNum;
		}

		InfoLogStream << "CURRENT PLAYER NUM:" << playerNum << LogStream::eos;
	}
}

TCSPlayer* TCSPlayerMgr::FindTCSPlayer(UInt64 id)
{
	auto iter = m_PlayerMap.find(id);
	if (iter == m_PlayerMap.end())
	{
		return NULL;
	}

	return iter->second;
}

TCSPlayer* TCSPlayerMgr::RandTCSPlayer()
{
	if (m_PlayerVec.empty()) return NULL;

	UInt32 randNum = Avalon::Random::RandBetween(0, m_PlayerVec.size() - 1);
	return m_PlayerVec[randNum];
}

void TCSPlayerMgr::RandTCSPlayer(UInt32 num, std::vector<TCSPlayer*>& playerList, UInt32 param, bool isTeam)
{
	if (num == 0 || m_PlayerVec.empty()) return;

	if (num > 10)	// 最多允许10个
		num = 10;

	UInt32 range = 0;
	if (m_PlayerVec.size() > num)
	{
		range = Avalon::Random::RandBetween(0, m_PlayerVec.size() - num);
	}

	for (UInt32 k = range; k < m_PlayerVec.size(); ++k)
	{
		TCSPlayer* player = m_PlayerVec[k];
		if (NULL == player || player->GetTeamType() != param)
		{
			continue;
		}

		if (!isTeam)
		{
			UInt32 teamId = TCSTeamMgr::Instance()->GetPlayerTeamId(player->GetID());
			if (teamId != 0)
			{
				continue;
			}
		}

		num--;
		playerList.push_back(player);

		if (num == 0)
			break;
	}
}

void TCSPlayerMgr::AddTCSPlayer(TCSPlayer* tcsPlayer)
{
	if (NULL == tcsPlayer) return;

	auto iter = m_PlayerMap.find(tcsPlayer->GetID());
	if (iter != m_PlayerMap.end())
	{
		ErrorLogStream << " TCSPlayer Exist! id:"<< tcsPlayer->GetID() << LogStream::eos;
		RemoveTCSPlayer(tcsPlayer->GetID());
	}

	m_PlayerMap.insert(std::make_pair(tcsPlayer->GetID(), tcsPlayer));
	m_PlayerVec.push_back(tcsPlayer);

	InfoLogStream << "add player id:" << tcsPlayer->GetID() << LogStream::eos;
}

void TCSPlayerMgr::RemoveTCSPlayer(UInt64 playerId)
{
	m_PlayerMap.erase(playerId);

	TCSPlayerVec::iterator iter = m_PlayerVec.begin();
	for (; iter != m_PlayerVec.end(); ++iter)
	{
		if ((*iter)->GetID() == playerId)
		{
			CL_SAFE_DELETE(*iter);
			m_PlayerVec.erase(iter);
			InfoLogStream << "remove player id:" << playerId << LogStream::eos;
			return;
		}
	}
}

void TCSPlayerMgr::AddExpire(UInt64 playerId, UInt64 leaveTime)
{
	// 如果玩家不存在，则无需加入过期列表
	TCSPlayer* player = FindTCSPlayer(playerId);
	if (NULL == player)
	{
		return;
	}

	DebugLogStream << "add expire player id:" << playerId << " leaveTime:" << leaveTime << LogStream::eos;

	if (leaveTime == 0)
	{
		m_PlayerExpireMap.erase(playerId);
		TCSTeamMgr::Instance()->PlayerExpire(playerId, 0);
	}
	else
	{
		player->SetIsLoading(false);
		m_PlayerExpireMap[playerId] = leaveTime;
		TCSTeamMgr::Instance()->PlayerExpire(playerId, leaveTime + PLAYER_EXPIRE_INTERVAL);
	}
}

void TCSPlayerMgr::DelExpire(UInt64 playerId)
{
	auto iter = m_PlayerExpireMap.find(playerId);
	if(iter == m_PlayerExpireMap.end())
	{
		return;
	}

	DebugLogStream << "del expire player id:" << playerId << LogStream::eos;

	m_PlayerExpireMap.erase(playerId);
	TCSTeamMgr::Instance()->PlayerExpire(playerId, 0);
}

bool TCSPlayerMgr::PlayerIsExpire(UInt64 playerId)
{
	auto iter = m_PlayerExpireMap.find(playerId);
	return iter != m_PlayerExpireMap.end();
}

UInt64 TCSPlayerMgr::PlayerExpireTime(UInt64 playerId)
{
	auto iter = m_PlayerExpireMap.find(playerId);
	if (iter == m_PlayerExpireMap.end())
	{
		return (UInt32)CURRENT_TIME.Sec() + PLAYER_EXPIRE_INTERVAL;
	}

	return iter->second + PLAYER_EXPIRE_INTERVAL;
}

UInt64 TCSPlayerMgr::PlayerLeaveTime(UInt64 playerId)
{
	auto iter = m_PlayerExpireMap.find(playerId);
	return iter != m_PlayerExpireMap.end() ? iter->second : 0;
}

void TCSPlayerMgr::BroadcastToPlayer(Avalon::Packet* packet)
{
	for (auto& iter : m_PlayerVec)
	{
		if (NULL == iter)
		{
			continue;
		}

		iter->SendPacket(packet);
	}
}

void TCSPlayerMgr::BroadcastToPlayer(Avalon::Protocol& protocol)
{
	for (auto& iter : m_PlayerVec)
	{
		if (NULL == iter)
		{
			continue;
		}

		iter->SendProtocol(protocol);
	}
}

void TCSPlayerMgr::ExecuteProtocol(const CLProtocol::SysTransmitPlayerToTeamCopy& protocol)
{
	TCSPlayer* player = FindTCSPlayer(protocol.playerId);
	if (NULL == player)
	{
		return;
	}

	DelExpire(player->GetID());
	m_Executor.ExecutePacket(protocol.packet, player);
}

