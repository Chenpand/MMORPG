#include <AvalonPropertySet.h>
#include <CLGameSessionProtocol.h>
#include "RLPlayerMgr.h"
#include "RLPlayer.h"
#include "RelaySvrRouter.h"
#include "RaceMgr.h"

RLPlayerMgr::RLPlayerMgr()
{
	m_stPlayerMap.clear();
	m_unMaxPlayer = relaysvr_max_players;
	m_stWaitFreePlayerSet.clear();
	m_abalityTimers.SetInterval(30 * 1000);
}

RLPlayerMgr::~RLPlayerMgr()
{

}

bool RLPlayerMgr::Init(Avalon::PropertySet* prop)
{
	UInt32 maxPlayer = prop->GetProperty("server", "capacity"); 
	m_unMaxPlayer = maxPlayer;

	//
	//RegisterProtocols();

	return true;
}

void RLPlayerMgr::OnTick(const Avalon::Time& now)
{
	if (m_abalityTimers.IsTimeout(now))
	{
		CLProtocol::RelayAbalitySync sync;
		sync.abality = get_ablity();
		Router::SendToMatchSvr(sync);
		Router::SendToDungeonSvr(sync);
		if (SERVER_TYPE == ST_OBSERVE_RELAY)
		{
			sync.abality = get_ablity() - RaceMgr::Instance()->GetObserveLoad();
			if (get_ablity() <= RaceMgr::Instance()->GetObserveLoad())
			{
				sync.abality = 0;
			}
			Router::SendToObserve(sync);
		}
	}

	//tick players
	class TickPlayerVisitor : public RLPlayerVisitor
	{
	public:
		TickPlayerVisitor(const Avalon::Time& now) :m_Now(now)
		{
		}

		bool operator()(RLPlayer* player)
		{
			if (player)
			{
				player->OnTick(m_Now);
			}

			return true;
		}
	protected:

		Avalon::Time m_Now;
	private:
	};

	TickPlayerVisitor tickVisitor(now);
	
	VisitPlayers(tickVisitor);

	//实际回收RLPlayer对象
	DeallocPlayer();
}

void RLPlayerMgr::OnQuit()
{

}


//执行消息(调用响应的handler处理消息)
void RLPlayerMgr::ExecutePacket(RLPlayer* player, Avalon::Packet* packet)
{
	//find RLPlayer and handle the packet
}

UInt32 RLPlayerMgr::get_player_count()
{
	return m_stPlayerMap.size();
}

UInt32 RLPlayerMgr::get_ablity()
{
	if (get_player_count() >= m_unMaxPlayer)
	{
		return 0;
	}

	return (m_unMaxPlayer - get_player_count());
}

void RLPlayerMgr::set_max_player_num(UInt32 num)
{
	m_unMaxPlayer = num;
}

RLPlayer* RLPlayerMgr::find_player(ObjID_t roleId)
{
	RLPlayerMap::iterator iter = m_stPlayerMap.find(roleId);
	if (iter != m_stPlayerMap.end())
	{
		return iter->second;
	}

	return NULL;
}

RLPlayer* RLPlayerMgr::findPlayerByAccount(UInt32 accid)
{
	for (auto itr : m_stPlayerMap)
	{
		auto player = itr.second;
		if (player && player->get_accId() == accid)
		{
			return player;
		}
	}

	return NULL;
}

//加入player时 没有判断是否达到系统支撑上限，此限制可以在创建player的时候进行
bool RLPlayerMgr::add_player(RLPlayer* player)
{
	if (NULL == player)
	{
		return false;
	}

	//
	return (m_stPlayerMap.insert(std::make_pair(player->GetID(), player)).second);
}

bool RLPlayerMgr::remove_player(RLPlayer* player)
{
	if (NULL == player)
	{
		return false;
	}

	return remove_player(player->GetID());
}

bool RLPlayerMgr::remove_player(ObjID_t roleId)
{
	RLPlayerMap::iterator iter = m_stPlayerMap.find(roleId);
	if (iter != m_stPlayerMap.end())
	{
        DebugLogStream << "remove player(" << roleId << ") from player manager." << LogStream::eos;
		m_stPlayerMap.erase(iter);
	}

	return true;
}

void RLPlayerMgr::FreePlayer(RLPlayer* player)
{
	if (NULL != player)
	{
        DebugLogStream << "player(" << player->GetID() << ") wait to free." << LogStream::eos;
		m_stWaitFreePlayerSet.insert(player);
	}
	
}

void RLPlayerMgr::DeallocPlayer()
{
	for (RLPlayerSet::iterator iter = m_stWaitFreePlayerSet.begin(); iter != m_stWaitFreePlayerSet.end(); ++iter)
	{
		delete *iter;
	}

	m_stWaitFreePlayerSet.clear();
}

void RLPlayerMgr::VisitPlayers(RLPlayerVisitor& visitor)
{
	RLPlayer* player = NULL;
	for (RLPlayerMap::iterator iter = m_stPlayerMap.begin(); iter != m_stPlayerMap.end(); )
	{
		player = iter->second;
		++iter;
		if(!visitor(player))
		{
			return;
		}
	}
}
