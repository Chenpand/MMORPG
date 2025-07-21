#include "PlayerMgr.h"
#include <CLRecordClient.h>
#include <CLSceneProtocol.h>
#include <CLReplayProtocol.h>
#include "Scene.h"
#include "SSRouter.h"
#include "SSApplication.h"


PlayerMgr::PlayerMgr()
{
	m_MinTimer.SetInterval(60 * 1000);
}

PlayerMgr::~PlayerMgr()
{
	Final();
}

bool PlayerMgr::Init()
{
	std::string configpath = CONFIG_PATH;
	if (!m_InitAttrs.LoadFromFile(configpath + "PlayerInit.cfg"))
	{
		ErrorLogStream << "load player init attribute config failed!" << LogStream::eos;
		return false;
	}

	RegisterProtocols();
	return true;
}

void PlayerMgr::Final()
{
	class DeleteVisitor : public PlayerVisitor
	{
	public:
		bool operator()(Player* player)
		{
			if(player->GetScene() != NULL)
			{
				player->GetScene()->RemoveObject(player);
			}
			if(player->GetGameStatus() == PS_NORMAL)
			{
				player->OnLogicOffLine();
				player->OnOffline(true);
				player->Saveback(NULL);
			}
			SceneObject::FreeObject(player);
			return true;
		}
	};
	DeleteVisitor visitor;
	m_Players.VisitObjects(visitor);
	m_Players.Clear();
}

bool PlayerMgr::AddPlayer(Player *player)
{
	// 判断缓存的玩家数量，如果超过一定值，就尝试删除部分玩家
	TryRelaseCachePlayerData();

	return m_Players.AddObject(player);
}

void PlayerMgr::DelPlayer(Player* player)
{
	if (!player)
	{
		return;
	}

	if (player->GetScene() != NULL)
	{
		player->GetScene()->RemoveObject(player);
	}

	PlayerMgr::Instance()->RemovePlayer(player, true);
	SceneObject::FreeObject(player);
}

Player* PlayerMgr::FindPlayer(ObjID_t id)
{
	return m_Players.FindObject(id);
}

Player* PlayerMgr::FindNormalPlayer(ObjID_t id)
{
	Player* player = m_Players.FindObject(id);
	if(player != NULL && player->GetGameStatus() == PS_NORMAL && player->IsInScene()) return player;
	return NULL;
}

Player* PlayerMgr::FindLoadingFinishPlayer(ObjID_t id)
{
	Player* player = m_Players.FindObject(id);
	if (player != NULL && (player->GetGameStatus() == PS_NORMAL || player->GetGameStatus() == PS_FINAL || player->GetGameStatus() == PS_LEAVEGAME))
	{
		return player;
	}

	return NULL;
}

class FindPlayerVisitor : public PlayerVisitor
{
public:
	FindPlayerVisitor(const std::string &strName)
		: m_strName(strName)
		, m_pPlayer(NULL)
	{
	}
	~FindPlayerVisitor() {}

	bool operator()(Player* pPlayer)
	{
		if(pPlayer->GetName() == m_strName)
		{
			m_pPlayer = pPlayer;
			return false;
		}

		return true;
	}

	Player* GetResult() { return m_pPlayer; }
private:
	std::string	m_strName;
	Player		*m_pPlayer;
};

Player* PlayerMgr::FindPlayer(const std::string &strName)
{
	FindPlayerVisitor	visitor(strName);
	VisitPlayers(visitor);

	return visitor.GetResult();
}

Player* PlayerMgr::FindNormalPlayer(const std::string &strName)
{
	FindPlayerVisitor	visitor(strName);
	VisitNormalPlayers(visitor);

	return visitor.GetResult();
}

void PlayerMgr::RemovePlayer(Player *player, bool removeFromMap)
{
	if (removeFromMap)
	{
		if (player)
		{
			InfoLogStream << "remove player(" << player->GetID() << ") from map..." << LogStream::eos;
		}
		m_Players.RemoveObject(player);
	}
}

void PlayerMgr::VisitPlayers(PlayerVisitor& visitor)
{
	m_Players.VisitObjects(visitor);
}

void PlayerMgr::VisitNormalPlayers(PlayerVisitor& visitor)
{
	class NormalPlayerVisitor : public PlayerVisitor
	{
	public:
		NormalPlayerVisitor(PlayerVisitor& visitor):m_Visitor(visitor){}

		bool operator()(Player* player)
		{
			if(player->GetGameStatus() == PS_NORMAL) return m_Visitor(player);
			return true;
		}

	private:
		PlayerVisitor& m_Visitor;
	};
	NormalPlayerVisitor normalVisitor(visitor);
	VisitPlayers(normalVisitor);
}

void PlayerMgr::OnGateDisconnected(UInt32 id)
{
	class OnGateDisconnectedVisitor : public PlayerVisitor
	{
	public:
		OnGateDisconnectedVisitor(){}

		bool operator()(Player* player)
		{
			player->LeaveGame();
			return true;
		}
	};
	OnGateDisconnectedVisitor visitor;
	m_Players.VisitObjects(visitor);
}

void PlayerMgr::OnTick(const Avalon::Time& now)
{
	class DestroyVisitor : public PlayerVisitor
	{
	public:
		DestroyVisitor() : m_CurSec((UInt32)CURRENT_TIME.Sec()) {}

		bool operator()(Player* player)
		{
			if(player->GetGameStatus() == PS_FINAL)
			{
				UInt64 baseTime = player->GetRefTime() > player->GetSavebackTime() ? player->GetRefTime() : player->GetSavebackTime();
				if (CURRENT_TIME.Sec() > baseTime / 1000 + SSApplication::Instance()->GetPlayerBuffCacheTime())
				{
					player->SetGameStatus(PS_DESTORY);
				}
			}

			if (player->GetGameStatus() == PS_INIT || player->GetGameStatus() == PS_RECONNECT)
			{
				if (m_CurSec > player->GetStatusTime() + 5 * Avalon::Time::SECS_OF_MIN)
				{
					ErrorLogStream << "player(" << player->GetID() << ") status(" << player->GetGameStatus() << ") too long..." << LogStream::eos;
					player->SetGameStatus(PS_DESTORY);
				}
			}
			
			if (player->GetGameStatus() == PS_DESTORY)
			{
				CLProtocol::SceneNotifyWorldLeave protocol;
				protocol.roleId = player->GetID();
				Router::SendToWorld(protocol);

				// 通知录像服务器
				CLProtocol::ReplayReportPlayerOffline notify;
				notify.roleId = player->GetID();
				Router::SendToReplay(notify);

				PlayerMgr::Instance()->DelPlayer(player);
			}
			return true;
		}

		UInt32 m_CurSec;
	};
	DestroyVisitor visitor;
	VisitPlayers(visitor);

	class TickVisitor : public PlayerVisitor
	{
	public:
		TickVisitor(const Avalon::Time& now) : m_Now(now) 
		{
			memset(m_PlayerNumByStatus, 0, sizeof(m_PlayerNumByStatus));
		}

		bool operator()(Player* player)
		{
			player->OnTick(m_Now);

			if (player->GetGameStatus() < PS_NUM)
			{
				m_PlayerNumByStatus[player->GetGameStatus()]++;
			}

			return true;
		}

		Avalon::Time m_Now;
		UInt32		m_PlayerNumByStatus[PS_NUM];
	};

	TickVisitor vistor(now);
	VisitPlayers(vistor);

	if(m_MinTimer.IsTimeout(now))
	{
		InfoLogStream << "CURRENT PLAYER NUM: " << m_Players.GetNum() << "." << LogStream::eos;
		for (int i = 0; i < PS_NUM; i++)
		{
			InfoLogStream << "Player Status(" << i << ") Num(" << vistor.m_PlayerNumByStatus[i] << ")." << LogStream::eos;
		}
	}

	m_PacketNumInOneTick.clear();
}

void PlayerMgr::OnPlayerForeverDeleted(ObjID_t roleId)
{

}

void PlayerMgr::ExecutePacket(ObjID_t id, Avalon::Packet *packet)
{
	Player* player =FindNormalPlayer(id);
	if(player != NULL && player->IsInScene())
	{
		m_Executor.ExecutePacket(packet,player);

		m_PacketNumInOneTick[packet->GetID()]++;
	}
}

void PlayerMgr::ShowPacketInOneTick()
{
	for (auto itr : m_PacketNumInOneTick)
	{
		auto msgId = itr.first;
		auto num = itr.second;

		InfoLogStream << "packet(" << msgId << ") num(" << num << ")." << LogStream::eos;
	}
}

void PlayerMgr::TryRelaseCachePlayerData()
{
	if (m_Players.GetNum() < 5000)
	{
		return;
	}

	class LruVisitor : public PlayerVisitor
	{
	public:
		LruVisitor() : lastPlayer(NULL) {}

		bool operator()(Player* player)
		{
			if (player->GetGameStatus() != PS_FINAL)
			{
				return true;
			}

			if (lastPlayer == NULL || player->GetOfflineTime() > lastPlayer->GetOfflineTime())
			{
				lastPlayer = player;
			}

			return true;
		}

		Player* lastPlayer;
	};

	LruVisitor vistor;
	m_Players.VisitObjects(vistor);

	if (vistor.lastPlayer)
	{
		InfoLogStream << "memory out of memory, free player(" << vistor.lastPlayer->GetID() << ", " << vistor.lastPlayer->GetAccID() << ")." << LogStream::eos;

		m_Players.RemoveObject(vistor.lastPlayer);
		SceneObject::FreeObject(vistor.lastPlayer);
	}
}

void PlayerMgr::LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, PlayerLoadingCallback* cb)
{
	Player* player = new Player();
	player->SetGameStatus(PS_INIT);
	player->SetID(roleid);
	player->SetAccID(accid);
	PlayerMgr::Instance()->AddPlayer(player);

	player->StartLoadData(cb);
}
