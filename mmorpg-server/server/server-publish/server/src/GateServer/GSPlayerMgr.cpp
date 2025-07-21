#include "GSPlayerMgr.h"

#include <AvalonLogger.h>
#include <CLLoginProtocol.h>
#include <CLErrorCode.h>
#include <CLEnterGameProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <CLGameSessionProtocol.h>
#include <PlayerHelper.h>

#include "GSExternalNetwork.h"
#include "GSAccountMgr.h"
#include "GSRouter.h"
#include "GSScene.h"
#include "GSPlayerMonitorMgr.h"
#include "GSApplication.h"
#include "CLSceneDefine.h"

GSPlayerMgr::GSPlayerMgr()
{
	m_ConnPlayerNum = 0;
	m_loadingPlayerNum = 0;
	m_normalPlayerNum = 0;
	m_roleSelectPlayerNum = 0;
	m_needSyncWaitQueue = false;
	m_SystemOpenedExtensibleRoleFields = 0;

	m_waitQueueTimer.SetInterval(Avalon::Time::MSECS_OF_SEC);
	m_reportWaitQueueTimer.SetInterval(30 * Avalon::Time::MSECS_OF_SEC);
	m_PerfTimer.SetInterval(60 * Avalon::Time::MSECS_OF_SEC);
}

GSPlayerMgr::~GSPlayerMgr()
{
	Final();
}

bool GSPlayerMgr::Init(Avalon::PropertySet* properties)
{
	if(properties == NULL) return false;

	std::string configpath = CONFIG_PATH;
	if(!m_InitAttrs.LoadFromFile(configpath + "PlayerInit.cfg"))
	{
		ErrorLogStream << "load player init attribute config failed!" << LogStream::eos;
		return false;
	}

	m_InitAttrs.LoadFromFile("../PlayerInit.cfg");

	if (!PlayerHelper::CalculateSystemOpenedExtensibleRoleNum(m_SystemOpenedExtensibleRoleFields))
	{
		ErrorLogStream << "Calculate extensible role num failed!" << LogStream::eos;
		return false;
	}

	return true;
}

void GSPlayerMgr::Final()
{	
	for(AccPlayerMap::iterator iter = m_accPlayers.begin()
		;iter != m_accPlayers.end();++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_accPlayers.clear();
	m_Players.Clear();
}

bool GSPlayerMgr::AddPlayer(GSPlayer *player)
{
	//一开始没选角色
	return m_accPlayers.insert(std::make_pair(player->GetAccID(),player)).second;
}

bool GSPlayerMgr::AddGuidPlayer(GSPlayer* player)
{
	return m_Players.AddObject(player);
}

bool GSPlayerMgr::RemoveGuidPlayer(GSPlayer* player)
{
    m_Players.RemoveObject(player);
    return true;
}

void GSPlayerMgr::RemovePlayer(GSPlayer* player)
{
	GSPlayerMgr::Instance()->DelFromWaitList(player->GetAccID());
	m_accPlayers.erase(player->GetAccID());
	m_Players.RemoveObject(player);
}

GSPlayer* GSPlayerMgr::FindPlayerByAccID(UInt32 id)
{
	AccPlayerMap::iterator iter = m_accPlayers.find(id);
	if(iter != m_accPlayers.end())
	{
		return iter->second;
	}
	return NULL;
}

GSPlayer* GSPlayerMgr::FindPlayerByID(ObjID_t id)
{
	return m_Players.FindObject(id);
}

void GSPlayerMgr::OnTick(const Avalon::Time& now)
{
	class TickVisitor : public GSPlayerVisitor
	{
	public:
		TickVisitor(const Avalon::Time& now) :m_Now(now), connPlayerNum(0), roleSelectPlayerNum(0), loadingPlayerNum(0), normalPlayerNum(0) {}

		bool operator()(GSPlayer* player)
		{
			if (player->GetConn() != NULL)
			{
				connPlayerNum++;
			}

			if (player->GetStatus() == GPS_NORMAL || (player->GetStatus() == GPS_OFFLINE && player->CanEnterGame()))
			{
				normalPlayerNum++;
			}
			else if (player->GetStatus() == GPS_CREATEROLE || player->GetStatus() == GPS_SELECTROLE || player->GetStatus() == GPS_WAITROLEINFO || player->GetStatus() == GPS_VERIFY)
			{
				roleSelectPlayerNum++;
			}
			else if (player->GetStatus() == GPS_ENTERGAME)
			{
				loadingPlayerNum++;
			}

			player->OnTick(m_Now);
            if (player->GetStatus() == GPS_OFFLINE && player->IsOfflineTimeout(m_Now))
            {
				if (player->GetScene() != NULL)
				{
					player->GetScene()->RemovePlayer(player);  //从场景中删除player
				}
				player->NotifyLeaveGame(player->GetID(), 1);
                offlinePlayerList.push_back(player->GetAccID());
            }

			return true;
		}
	private:
		Avalon::Time m_Now;

    public:
        std::vector<UInt32> offlinePlayerList;
		UInt32				connPlayerNum;
		UInt32				roleSelectPlayerNum;
		UInt32				loadingPlayerNum;
		UInt32				normalPlayerNum;
	};
	TickVisitor visitor(now);
	VisitPlayers(visitor);
	m_ConnPlayerNum = visitor.connPlayerNum;
	m_loadingPlayerNum = visitor.loadingPlayerNum;
	m_normalPlayerNum = visitor.normalPlayerNum;
	m_roleSelectPlayerNum = visitor.roleSelectPlayerNum;

	TryLetFirstPlayerLogin();
	if (m_waitQueueTimer.IsTimeout(now) && m_needSyncWaitQueue)
	{
		SyncWaitLoginPlayerInfo();
	}

	if (m_reportWaitQueueTimer.IsTimeout(now))
	{
		CLProtocol::AdminSyncLoginQueueInfo report;
		report.inQueuePlayerNum = (UInt32)m_waitPlayers.size();
		Router::SendToAdmin(report);
	}

    for (auto accid : visitor.offlinePlayerList)
    {
		auto player = this->FindPlayerByAccID(accid);
        if (player)
        {
			InfoLogStream << "player(" << accid << ") offline timeout, delete player." << LogStream::eos;
            RemovePlayer(player);
            CL_SAFE_DELETE(player);
        }
    }

	if (m_PerfTimer.IsTimeout(now))
	{
		ShowUsedTime();
		m_UsedTimeInOneTick.clear();
	}
}

void GSPlayerMgr::OnGateConnected(UInt32 id)
{
    DebugLogStream << "player(" << id << ") connected." << LogStream::eos;

	ReconnInfo reconnInfo;
	reconnInfo = GSAccountMgr::Instance()->GetReconnInfo(id);
	bool isReconn = reconnInfo.accid != 0;

	Avalon::NetConnectionPtr conn = GSExternalNetwork::Instance()->FindConnection(id);
	if (conn == NULL)
	{
		ErrorLogStream << "player(" << id << ") can't find connection." << LogStream::eos;
		return;
	}

    GSPlayer* player = FindPlayerByAccID(id); 
    if (player)
    {
        player->SetConn(conn);

		if (player->GetStatus() == GPS_NORMAL && player->GetID() != 0)
		{
			//顶号后一定不能重连，通知匹配服务器放弃比赛
			CLProtocol::SceneServerGiveupReconn giveup;
			giveup.roleId = player->GetID();
			Router::SendToMatch(giveup);
		}

		if (reconnInfo.accid != 0)
		{
			CLProtocol::GateReconnectGameRes res;
			res.lastRecvSequence = player->GetLastRecvSequence();

			res.result = player->BeginReconnect(conn, player->GetSession(), reconnInfo.sequence);
			player->SendProtocol(res);

			if (res.result != ErrorCode::SUCCESS)
			{
				return;
			}

			player->SetStatus(GPS_NORMAL);
			player->NotifyReconnectSuccess();
		}
		else
		{
			player->ClearPacketBuffer();
			player->SetStatus(GPS_VERIFY);
		}

		AccountInfo info;
		if (GSAccountMgr::Instance()->GetAccount(id, info))
		{
			std::vector<UInt8> session;
			for (auto val : info.key)
			{
				session.push_back(val);
			}

			player->SetSession(session);
			player->SetPhoneBindRoleID(info.phoneBindRoleId);
		}
    }
    else
    {
		if (isReconn)
		{
			CLProtocol::GateReconnectGameRes res;
			res.lastRecvSequence = 0;
			res.result = ErrorCode::RECONNECT_PLAYER_DELETED;
			CL_SEND_PROTOCOL(conn, res);
			return;
		}

		AccountInfo info;
		if (!GSAccountMgr::Instance()->GetAccount(id, info))
		{
			DebugLogStream << "player(" << id << ") get account failed." << LogStream::eos;

			Avalon::NetConnectionPtr conn = GSExternalNetwork::Instance()->FindConnection(id);
			if (conn != NULL)
			{
				CLProtocol::GateClientLoginRet ret;
				ret.result = ErrorCode::LOGIN_SERVER_BUSY;
				CL_SEND_PROTOCOL(conn, ret);
			}
			
			return;
		}

		if (GSPlayerMgr::Instance()->GetConnPlayerNum() >= GSApplication::Instance()->GetMaxConnNum())
		{
			CLProtocol::GateClientLoginRet ret;
			ret.result = ErrorCode::LOGIN_BUSY;
			CL_SEND_PROTOCOL(conn, ret);

			InfoLogStream << "player(" << id << ") want to login, but connection is full(" << GSPlayerMgr::Instance()->GetConnPlayerNum() << ")." << LogStream::eos;

			return;
		}

        player = new GSPlayer();
        if (!player->Init(id, info.gmauthority, info.openid, info.qqVipInfo, info.source1, info.source2, info.pf, info.did, info.model, info.deviceVersion))
        {
            DebugLogStream << "player(" << id << ") init failed." << LogStream::eos;

            CL_SAFE_DELETE(player);
            return;
        }
		player->SetMonitored(GSPlayerMonitorMgr::Instance()->IsPlayerMonitored(id));
		player->SetPhoneBindRoleID(info.phoneBindRoleId);

		std::vector<UInt8> session;
		for (auto val : info.key)
		{
			session.push_back(val);
		}
		player->SetSession(session);

        if (!AddPlayer(player))	//一般没这种情况
        {
            DebugLogStream << "add player(" << id << ") failed." << LogStream::eos;

            CLProtocol::GateClientLoginRet ret;
            ret.result = ErrorCode::LOGIN_REPEAT;
            player->SendProtocol(ret);
            CL_SAFE_DELETE(player);
            return;
        }
    }

	CLProtocol::GateSyncServerTime syncTime;
	syncTime.time = UInt32(CURRENT_TIME.Sec());
	player->SendProtocol(syncTime);
	
	CLProtocol::AdminSyncGatePlayerNum sync;
	sync.playerNum = m_Players.GetNum();
	sync.maxPlayerNum = GSExternalNetwork::Instance()->GetNetProperties()->maxnum_of_connection;
	Router::SendToAdmin(sync);

	if (!isReconn)
	{
		CLProtocol::WorldVerifyUniqueReq req;
		req.accid = player->GetAccID(); 
		req.checkWaitQueue = 0;
		Router::SendToWorld(req);
		DebugLogStream << "player(" << id << ") send verify unique request to world." << LogStream::eos;
	}
}

void GSPlayerMgr::OnGateDisconnected(UInt32 id)
{
	GSPlayer* player = FindPlayerByAccID(id);
	if(player == NULL) return;
	
	DebugLogStream << "player(" << id << ") disconnected cur packet buff min sequence(" << 
		player->GetPlayerPacketBuffer().GetMinSequence() << ")." << LogStream::eos;

	player->SetConn(NULL);
	player->NotifyNodeRoleConnect();

	if (player->GetStatus() == GPS_NORMAL || player->GetStatus() == GPS_REPLACE || player->GetStatus() == GPS_QUEUE || player->GetStatus() == GPS_SELECTROLE)
    {
        player->SetStatus(GPS_OFFLINE);
		if (player->GetScene() && CL_GET_SERVERTYPE_FROM_NODEID(player->GetScene()->GetNodeID()) == ST_BSCENE
			&& (player->GetScene()->GetSceneType() == SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC 
				|| player->GetScene()->GetSceneType() == SCENE_TYPE_LOST_DUNGEON))
		{
			player->SetOfflineTimer(2 * 60);
		}
		else
		{
			player->SetOfflineTimer(10 * 60);
		}
		player->NotifyDisconnect();
    }
	else if (player->GetStatus() != GPS_OFFLINE && player->GetStatus() != GPS_RECONNECT)
    {
		if (player->GetScene() != NULL)
		{
			player->GetScene()->RemovePlayer(player);  //从场景中删除player
		}

		player->NotifyLeaveGame(player->GetID());
        RemovePlayer(player);
        CL_SAFE_DELETE(player);
    }
	

	CLProtocol::AdminSyncGatePlayerNum sync;  //noted by aprilliu, 通知admin server最新在线人数， 后续可以优化为 定时push在线人数
	sync.playerNum = m_Players.GetNum();
	sync.maxPlayerNum = GSExternalNetwork::Instance()->GetNetProperties()->maxnum_of_connection;
	Router::SendToAdmin(sync);
}

void GSPlayerMgr::VisitPlayers(GSPlayerVisitor& visitor)
{
	for(AccPlayerMap::iterator iter = m_accPlayers.begin();iter != m_accPlayers.end();)
	{
		GSPlayer* player = iter->second;
		++iter;
		if(!visitor(player)) return;
	}
}

void GSPlayerMgr::Broadcast(UInt8 bZone, UInt16 level, Avalon::Packet* packet)
{
	class BroadcastVisitor : public GSPlayerVisitor
	{
	public:
		BroadcastVisitor(UInt8 bZone, UInt16 level, Avalon::Packet* pack)
			:m_bZone(bZone), m_Level(level), m_pPacket(pack){}

		bool operator()(GSPlayer* player)
		{
			if(player->GetStatus() != GPS_NORMAL) return true;
			if(m_bZone == 1 && player->GetSrcZoneId() != 0) return true;
			if(player->GetLevel() < m_Level) return true;
			
#ifdef _DEBUG
			InfoLogStream << "[cross] send (" << m_pPacket->GetID() << ") to player(" << player->GetAccID() << ", " << player->GetID() << ")." << LogStream::eos;
#endif
			player->SendPacket(m_pPacket);
			return true;
		}
	private:
		UInt8	m_bZone;
		UInt16	m_Level;
		Avalon::Packet* m_pPacket;
	};
	BroadcastVisitor visitor(bZone, level, packet);
	VisitPlayers(visitor);
}

bool GSPlayerMgr::IsNeedWaitOhters()
{
	UInt32 maxLoadingPlayerNum = GSApplication::Instance()->GetMaxLoadingPlayerNum();
	UInt32 maxPlayerNum = GSApplication::Instance()->GetMaxPlayerNum();
	if (GetLoadingPlayerNum() >= maxLoadingPlayerNum)
	{
		return true;
	}

	if (GetNormalPlayerNum() + GetRoleSelectPlayerNum() >= maxPlayerNum)
	{
		return true;
	}

	return false;
}

bool GSPlayerMgr::TryLetFirstPlayerLogin()
{
	if (IsNeedWaitOhters())
	{
		return false;
	}

	if (m_waitPlayers.empty())
	{
		return false;
	}

	// 让排在第一个的玩家登录
	auto accid = *(m_waitPlayers.begin());

	// 通知客户端可以进了，开始加载数据
	InfoLogStream << "allow player(" << accid << ") login..." << LogStream::eos;

	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(accid);
	if (player && player->GetStatus() == GPS_QUEUE)
	{
		CLProtocol::GateNotifyAllowLogin notify;
		player->SendProtocol(notify);

		player->BeginQueryAccountInfo();
		player->SetCanEnterGame(true);

		// 这里先把在选角界面的玩家数加上
		m_roleSelectPlayerNum++;
	}

	DelFromWaitList(accid);
	return true;
}

UInt32 GSPlayerMgr::AddPlayerToWaitList(UInt32 accid)
{
	auto index = GetIndexInWaitList(accid);
	if (index >= 0)
	{
		return index + 1;
	}

	InfoLogStream << "player(" << accid << ") add to wait list index(" << (UInt32)m_waitPlayers.size() + 1 << ") normal(" 
					<< m_normalPlayerNum << ") load(" << m_loadingPlayerNum << ") roleselect(" << m_roleSelectPlayerNum << ")." << LogStream::eos;

	m_waitPlayers.push_back(accid);
	return m_waitPlayers.size();
}

void GSPlayerMgr::DelFromWaitList(UInt32 accid)
{
	for (auto itr = m_waitPlayers.begin(); itr != m_waitPlayers.end(); ++itr)
	{
		if (*itr == accid)
		{
			m_waitPlayers.erase(itr);
			m_needSyncWaitQueue = true;

			InfoLogStream << "player(" << accid << ") del from wait list." << LogStream::eos;

			return;
		}
	}
}

int GSPlayerMgr::GetIndexInWaitList(UInt32 accid)
{
	int i = 0;
	for (auto itr = m_waitPlayers.begin(); itr != m_waitPlayers.end(); ++itr)
	{
		if (*itr == accid)
		{
			return i;
		}
		i++;
	}

	return -1;
}

void GSPlayerMgr::SyncWaitLoginPlayerInfo()
{
	UInt32 waitPlayerNum = 1;
	for (auto accid : m_waitPlayers)
	{
		auto player = GSPlayerMgr::Instance()->FindPlayerByAccID(accid);
		if (player)
		{
			CLProtocol::GateNotifyLoginWaitInfo notify;
			notify.waitPlayerNum = waitPlayerNum++;
			player->SendProtocol(notify);
		}
	}
	m_needSyncWaitQueue = false;
}

UInt64 GSPlayerMgr::GetUSecsNow()
{
	if (!GSApplication::Instance()->OpenPerf())
	{
		return 0;
	}

#if defined(AVALON_OS_WINDOWS)
	return 0;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (UInt64)tv.tv_sec * 1000000 + (UInt64)tv.tv_usec;
#endif
}

void GSPlayerMgr::RecordUsedTime(UInt32 id, UInt32 usec)
{
	if (!GSApplication::Instance()->OpenPerf())
	{
		return;
	}

	static PerfInfo s_info;
	auto itr = m_UsedTimeInOneTick.find(id);
	if (itr == m_UsedTimeInOneTick.end())
	{
		s_info.count = 1;
		s_info.times = usec;
		m_UsedTimeInOneTick[id] = s_info;
	}
	else
	{
		itr->second.count++;
		itr->second.times += usec;
	}
}

void GSPlayerMgr::ShowUsedTime()
{
	for (auto& itr : m_UsedTimeInOneTick)
	{
		auto& info = itr.second;
		UInt32 average = info.count == 0 ? 0 : (info.times / info.count);
		InfoLogStream << "module(" << itr.first << ") count: " << info.count << " used: " << info.times << "(us) average: " << average << "(us)" << LogStream::eos;
	}
}