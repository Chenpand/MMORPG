#include "WSPlayerMgr.h"

#include <CLInviteGameProtocol.h>
#include <CLGSActivityProtocol.h>
#include <CLGameDefine.h>
#include <CLEnterGameProtocol.h>
#include <CLSysProtocol.h>
#include <CLLoginProtocol.h>
#include <PlayerHelper.h>

#include "WSRouter.h"
#include "WSScene.h"
#include "kingnet_analyzer.h"
#include "WorldSysNotify.h"
#include "AccountMgr.h"
#include "WSApplication.h"
#include "GuildMgr.h"
#include "WSSortListMgr.h"
#include "CLRecordClient.h"
#include "GameParamMgr.h"
#include "WSRelationMgr.h"
#include "WSServiceSwitchMgr.h"

WSPlayerMgr::WSPlayerMgr()
{
	m_OnlineNum = 0;

	m_LogOnlineMin = UInt8(CURRENT_DATE.Min());
	m_ReportOnlineMin = m_LogOnlineMin;
	m_ReportSocialTime = UInt32(CURRENT_TIME.Sec()) + REPORT_SOCIAL_INTERVAL;
	m_TodayMaxOnlineTime = CURRENT_TIME;
	m_TodayMaxOnlineNum = 0;
	m_TodayMinOnlineTime = CURRENT_TIME;
	m_TodayMinOnlineNum = 0;
	m_waitQueueTimer.SetInterval(Avalon::Time::MSECS_OF_SEC);
	m_CheckDeletePlayerTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);
	m_needSyncWaitQueue = false;
	m_loadingPlayerNum = 0;
	m_SystemOpenedExtensibleRoleNum = 0;
}

WSPlayerMgr::~WSPlayerMgr()
{
}

bool WSPlayerMgr::Init()
{
	RegisterProtocols();

	std::string configpath = CONFIG_PATH;
	if(!m_InitAttrs.LoadFromFile(configpath + "PlayerInit.cfg"))
	{
		ErrorLogStream << "load player init attribute config failed!" << LogStream::eos;
		return false;
	}

	if (!PlayerHelper::CalculateSystemOpenedExtensibleRoleNum(m_SystemOpenedExtensibleRoleNum))
	{
		ErrorLogStream << "Calculate extensible role num failed!" << LogStream::eos;
		return false;
	}

	return true;
}

void WSPlayerMgr::Final()
{	
	class DeleteVisitor : public WSPlayerVisitor
	{
	public:
		bool operator()(WSPlayer* player)
		{
			player->OnOffline(true);
			player->Saveback();
			CL_SAFE_DELETE(player);
			return true;
		}
	};
	DeleteVisitor deleteVisitor;
	m_Players.VisitObjects(deleteVisitor);
	m_Players.Clear();

	m_NamePlayers.Clear();
	m_AccidPlayers.clear();
}

bool WSPlayerMgr::AddPlayer(WSPlayer *player)
{
	if(m_Players.AddObject(player))
	{
		if(m_AccidPlayers.insert(std::make_pair(player->GetAccId(), player)).second)
		{
			InfoLogStream << "player(" << player->GetAccId() << ", " << player->GetID() << ") add to account map." << LogStream::eos;
			if(player->GetSrcZoneId() != 0 || m_NamePlayers.AddObject(player))
			{
				return true;
			}
			else
			{
				InfoLogStream << "player(" << player->GetAccId() << ", " << player->GetID() << ") remove from account map." << LogStream::eos;

				m_Players.RemoveObject(player);
				m_AccidPlayers.erase(player->GetAccId());
			}
		}
		else
		{
			m_Players.RemoveObject(player);
		}
	}
	return false;
}

void WSPlayerMgr::RemovePlayer(WSPlayer *player)
{
	InfoLogStream << "player(" << player->GetAccId() << ") remove from account map." << LogStream::eos;

	m_Players.RemoveObject(player);
	_RemovePlayerInAccountMap(player);
	m_NamePlayers.RemoveObject(player);
}

bool WSPlayerMgr::AddPlayerByAccId(WSPlayer* player)
{
	InfoLogStream << "player(" << player->GetAccId() << ", " << player->GetID() << ") add to account map." << LogStream::eos;

	return m_AccidPlayers.insert(std::make_pair(player->GetAccId(), player)).second;
}

void WSPlayerMgr::RemovePlayerByAccId(WSPlayer* player)
{
	_RemovePlayerInAccountMap(player);
}

WSPlayer* WSPlayerMgr::FindPlayer(ObjID_t id)
{
	return m_Players.FindObject(id);
}

WSPlayer* WSPlayerMgr::FindPlayerByAccid(UInt32 accid)
{
	AccidPlayerMap::iterator iter = m_AccidPlayers.find(accid);
	if(iter != m_AccidPlayers.end()) return iter->second;
	return NULL;
}

WSPlayer* WSPlayerMgr::FindNormalPlayer(ObjID_t id)
{
	WSPlayer* player = FindPlayer(id);
	if(player != NULL && player->GetGameStatus() == PS_NORMAL) return player;
	return NULL;
}

WSPlayer* WSPlayerMgr::FindNormalPlayerByName(const std::string& name)
{
	WSPlayer* player = m_NamePlayers.FindObject(name);
	if(player != NULL && player->GetGameStatus() == PS_NORMAL) return player;
	return NULL;
}

void WSPlayerMgr::VisitPlayers(WSPlayerVisitor& visitor)
{
	m_Players.VisitObjects(visitor);
}

/*
void WSPlayerMgr::SendUdpLog(const char* str1, const char* str2, const char* str3
	, const char* str4, const char* str5, const char* str6, const char* strType, Int32 nCount)
{
	CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_WORLD_UID);
	if(NULL == pLogger)
		return;

	std::ostringstream	os;
	os << "|||3|||||||||||||" << ZONE_ID << "|cn|";

	pLogger->SetUserMsg(os.str().c_str());
	pLogger->LogMsg(str1, str2, str3, str4, str5, str6, strType, nCount);
}
*/

void WSPlayerMgr::VisitNormalPlayers(WSPlayerVisitor& visitor)
{
	class NormalPlayerVisitor : public WSPlayerVisitor
	{
	public:
		NormalPlayerVisitor(WSPlayerVisitor& visitor)
			:m_Visitor(visitor){}

		bool operator()(WSPlayer* player)
		{
			if(player->GetGameStatus() == PS_NORMAL) return m_Visitor(player);
			return true;
		}

	private:
		WSPlayerVisitor& m_Visitor;
	};
	NormalPlayerVisitor normalVisitor(visitor);
	VisitPlayers(normalVisitor);
}

void WSPlayerMgr::OnSceneDisconnected(UInt32 nodeid)
{
	class OnSceneUnLoadVisitor : public WSPlayerVisitor
	{
	public:
		OnSceneUnLoadVisitor(UInt32 nodeid):m_NodeID(nodeid){}

		bool operator()(WSPlayer* player)
		{
			if(player->GetSceneNode() == m_NodeID)
			{
				player->LeaveGame();
			}
			return true;
		}

	private:
		UInt32 m_NodeID;
	};
	OnSceneUnLoadVisitor visitor(nodeid);
	m_Players.VisitObjects(visitor);
}

void WSPlayerMgr::OnTick(const Avalon::Time& now)
{
	class HeartbeatVisitor : public WSPlayerVisitor
	{
	public:
		HeartbeatVisitor(const Avalon::Time& now) :m_Now(now), m_PlayerNum(0), m_qqVipPlayerNum(0), m_IwanPlayerNum(0), m_IwanQQvipPlayerNum(0), m_LoadingPlayerNum(0){
			WSPlayerMgr::Instance()->ClearPfPlayerNum();
		}

		bool operator()(WSPlayer* player)
		{
			UInt64 baseTime = player->GetRefTime() > player->GetOfflineTime() ? player->GetRefTime() : player->GetOfflineTime();
			if(player->GetGameStatus() == PS_FINAL)
			{
				if (m_Now.Sec() > baseTime / 1000 + DESTROY_TIMEOUT)
				{
					WSPlayerMgr::Instance()->RemovePlayer(player);
					CL_SAFE_DELETE(player);
					return true;
				}
			}
			else if(player->GetGameStatus() == PS_NORMAL)
			{
				player->OnHeartbeat(m_Now);

				++m_PlayerNum;
				if(player->GetQQVipInfo().level > 0) ++m_qqVipPlayerNum;

				if(player->GetPf().compare("iwan") == 0)
				{
					++m_IwanPlayerNum;
					if(player->GetQQVipInfo().level > 0) ++m_IwanQQvipPlayerNum;
				}

				if (player->GetPf() != "")
				{
					WSPlayerMgr::Instance()->AddPfPlayerNum(player->GetPf());
				}
			}
			else if (player->GetGameStatus() == PS_INIT)
			{
				m_LoadingPlayerNum++;
			}

			if (player->GetGameStatus() == PS_INIT || player->GetGameStatus() == PS_LEAVEGAME)
			{
				if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_PLAYER_STATUS_TIMEOUT) && m_Now.Sec() > player->GetStatusTime() + 5 * Avalon::Time::SECS_OF_MIN)
				{
					ErrorLogStream << "player(" << player->GetID() << ") status(" << player->GetGameStatus() << ") too long..." << LogStream::eos;
					player->SetGameStatus(PS_FINAL);
				}
			}

			return true;
		}

		UInt32 GetPlayerNum() const{ return m_PlayerNum; }
		UInt32 GetLoadingPlayerNum() const { return m_LoadingPlayerNum; }
		UInt32 GetQQVipPlayerNum() const{ return m_qqVipPlayerNum; }

		UInt32 GetIwanPlayerNum() const{ return m_IwanPlayerNum; }
		UInt32 GetIwanQQVipPlayerNum() const{ return m_IwanQQvipPlayerNum; }
	private:
		Avalon::Time m_Now;
		UInt32	m_PlayerNum;
		UInt32	m_qqVipPlayerNum;

		UInt32	m_IwanPlayerNum;
		UInt32	m_IwanQQvipPlayerNum;
		UInt32	m_LoadingPlayerNum;

	};
	HeartbeatVisitor visitor(now);
	VisitPlayers(visitor);

	if (m_OnlineNum != visitor.GetPlayerNum())
	{
		CLProtocol::AdminSyncWorldPlayerNum sync;
		sync.playerNum = visitor.GetPlayerNum();
		sync.maxPlayerNum = WSApplication::Instance()->GetMaxPlayerNum();
		Router::SendToAdmin(sync);
	}

	m_OnlineNum = visitor.GetPlayerNum();
	m_loadingPlayerNum = visitor.GetLoadingPlayerNum();
	UInt8 min = UInt8(CURRENT_DATE.Min());
	if(m_LogOnlineMin != min)
	{
		InfoLogStream << "CURRENT PLAYER NUM: " << m_OnlineNum << "!" << LogStream::eos;
		m_LogOnlineMin = min;
	}

	if (m_OnlineNum > m_TodayMaxOnlineNum)
	{
		m_TodayMaxOnlineNum = m_OnlineNum;
		m_TodayMaxOnlineTime = CURRENT_TIME;
	}

	if (m_OnlineNum < m_TodayMinOnlineNum)
	{
		m_TodayMinOnlineNum = m_OnlineNum;
		m_TodayMinOnlineTime = CURRENT_TIME;
	}

	if(m_ReportOnlineMin != min && min % 5 == 0)
	{
		//上报
		std::string zoneidstr = Avalon::StringUtil::ConvertToString<UInt32>(ZONE_ID);
		std::string timestr = Avalon::StringUtil::ConvertToString<UInt32>(UInt32(CURRENT_TIME.Sec()));

		Sys::SendUdpLog("pcu", LOG_PCU_FORMAT, GAME_VERSION.c_str(), zoneidstr.c_str(), timestr.c_str(), "", m_OnlineNum, m_TodayMaxOnlineNum, m_TodayMaxOnlineTime.Sec(), m_TodayMinOnlineNum, m_TodayMinOnlineTime.Sec());

		std::map<std::string, UInt32>::iterator iter = m_Pf2PlayerNum.begin();
		while (iter != m_Pf2PlayerNum.end())
		{
			Sys::SendUdpLog("pcu", LOG_PCU_FORMAT, GAME_VERSION.c_str(), zoneidstr.c_str(), timestr.c_str(), iter->first.c_str(), iter->second, m_TodayMaxOnlineNum, m_TodayMaxOnlineTime.Sec(), m_TodayMinOnlineNum, m_TodayMinOnlineTime.Sec());
			++iter;
		}

		//SendUdpLog("pcu", zoneidstr.c_str(), timestr.c_str(), Avalon::StringUtil::ConvertToString<UInt32>(visitor.GetPlayerNum()).c_str(), "", "", "ser");
		//SendUdpLog("vippcu", zoneidstr.c_str(), timestr.c_str() , Avalon::StringUtil::ConvertToString<UInt32>(visitor.GetQQVipPlayerNum()).c_str(), "", "", "ser");

		//SendUdpLog("pcu", zoneidstr.c_str(), timestr.c_str(), Avalon::StringUtil::ConvertToString<UInt32>(visitor.GetIwanPlayerNum()).c_str(), "iwan", "", "ser");
		//SendUdpLog("vippcu", zoneidstr.c_str(), timestr.c_str() , Avalon::StringUtil::ConvertToString<UInt32>(visitor.GetIwanQQVipPlayerNum()).c_str(), "iwan", "", "ser");
		m_ReportOnlineMin = min;
	}

	if(CURRENT_TIME.Sec() > m_ReportSocialTime)
	{
		class ReportSocialVisitor : public WSPlayerVisitor
		{
		public:
			ReportSocialVisitor(){}
			~ReportSocialVisitor(){}

		public:
			bool operator()(WSPlayer* player)
			{
				req.players.push_back(player->GetAccId());
				if(req.players.size() >= 1000)
				{
					Router::SendToSocial(req);
					req.players.clear();
				}
				return true;
			}

			void Flush()
			{
				if(!req.players.empty())
				{
					Router::SendToSocial(req);
					req.players.clear();
				}
			}

		private:
			CLProtocol::SocialInviteReportOnline req;
		};

		ReportSocialVisitor visitor;
		VisitNormalPlayers(visitor);
		visitor.Flush();

		m_ReportSocialTime = m_ReportSocialTime
			+ (UInt32)Avalon::Random::RandBetween(Int32(REPORT_SOCIAL_INTERVAL * 0.9f), Int32(REPORT_SOCIAL_INTERVAL * 1.1f));
	}

	if (m_waitQueueTimer.IsTimeout(now))
	{
		while (TryLetFirstPlayerLogin());

		if (m_needSyncWaitQueue)
		{
			SyncWaitLoginPlayerInfo();
		}
	}

	if (m_CheckDeletePlayerTimer.IsTimeout(now))
	{
		QueryForeverDeletedPlayer();
	}
}

void WSPlayerMgr::OnDayChanged()
{
	class DayChangeVisitor : public WSPlayerVisitor
	{
	public:
		DayChangeVisitor(UInt32 days):m_Days(days){}
		~DayChangeVisitor(){}

		bool operator()(WSPlayer *pPlayer)
		{
			pPlayer->OnFirstLogin();
			
			CLProtocol::WorldNotifyGameStartTime notify;
			notify.time = GAMESTART_TIME;
			notify.days = m_Days;
			pPlayer->SendProtocol(notify);
			return true;
		}

	private:
		//开服天数
		UInt32	m_Days;
	};

	DayChangeVisitor	visitor(GetGameStartDays());
	VisitNormalPlayers(visitor);

	m_TodayMaxOnlineNum = 0;
	m_TodayMaxOnlineTime = CURRENT_TIME.Sec();
	m_TodayMinOnlineNum = m_OnlineNum;
	m_TodayMinOnlineTime = CURRENT_TIME.Sec();
}

void WSPlayerMgr::OnNameChanged(ObjID_t playerId, const std::string& newname)
{
	WSPlayer* player = FindPlayer(playerId);
	if(player != NULL)
	{
		m_NamePlayers.RemoveObject(player);
		player->SetName(newname);
		m_NamePlayers.AddObject(player);
	}
}

void WSPlayerMgr::OnSexChanged(ObjID_t playerId, UInt8 sex)
{
	WSPlayer* player = FindPlayer(playerId);
	if(player != NULL)
	{
		player->SetSex(sex);
	}
}

UInt32 WSPlayerMgr::GetPlayerSceneServerID(ObjID_t roleId)
{
	auto itr = m_player2SceneServer.find(roleId);
	return itr == m_player2SceneServer.end() ? 0 : itr->second;
}

void WSPlayerMgr::OnPlayerLoadFinishOnSceneServer(ObjID_t roleId, UInt32 sceneNodeId)
{
	m_player2SceneServer[roleId] = sceneNodeId;
	DelLoadingPlayer(roleId);
}

void WSPlayerMgr::OnPlayerLeaveSceneServer(ObjID_t roleId)
{
	m_player2SceneServer.erase(roleId);
	DelLoadingPlayer(roleId);
}

void WSPlayerMgr::OnPlayerForeverDeleted(ObjID_t roleId, UInt8 type)
{
	InfoLogStream << "player(" << roleId << ") forever deleted..." << LogStream::eos;
	// 清除公会信息
	GuildMgr::Instance()->OnPlayerForeverDeleted(roleId);

	// 清除排行榜信息
	WSSortListMgr::Instance()->DeleteAllEntry(roleId);

	if (type == 1)
	{
		// 清除好友信息
		GlobalRelationMgr::Instance()->RemoveAllRelation(roleId);

		// 清除同门关系
		WSMasterSystem::OnPlayerForeverDeleted(roleId);
	}
	
	// 通知到Scene
	CLProtocol::SysPlayerForeverDeleted sync;
	sync.roleId = roleId;
	Router::BroadcastToScene(sync);
}

class QueryForeverDeletedPlayerCallback : public CLRecordCallback
{
public:
	QueryForeverDeletedPlayerCallback(UInt8 type, UInt32 now) : m_Type(type), m_NowSec(now) {}

	virtual void OnFinished()
	{
		while (NextRow())
		{
			ObjID_t roleId = GetKey();
			WSPlayerMgr::Instance()->OnPlayerForeverDeleted(roleId, m_Type);
		}

		InfoLogStream << "query forever deleted player finish, type(" << m_Type << ")time(" << m_NowSec << ")." << LogStream::eos;
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "query forever deleted player failed, type(" << m_Type << ")time(" << m_NowSec << ") errorcode(" << errorcode << ")." << LogStream::eos;
	}

	UInt8  m_Type;
	UInt32 m_NowSec;
};

void WSPlayerMgr::QueryForeverDeletedPlayer()
{
	if (!GuildMgr::Instance()->IsInitFinish())
	{
		return;
	}

	UInt32 lastCheckTime = GameParamMgr::Instance()->GetValue("check_delete_player_time");
	UInt32 nowTime = (UInt32)CURRENT_TIME.Sec();

	InfoLogStream << "start to query deleted player time(" << nowTime << ")." << LogStream::eos;

	// 十级以下的玩家
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		cmd->PutColumn("accid");
		cmd->PutCond("deletetime", lastCheckTime, ">");
		cmd->PutCond("deletetime", nowTime, "<=");
		cmd->PutCond("level", DIRECT_DELETE_PLAYER_LEVEL, "<");
		CLRecordClient::Instance()->SendCommand(cmd, new QueryForeverDeletedPlayerCallback(0, nowTime));
	}

	// 十级以上的玩家
	{
		UInt32 saveTime = DELETE_PLAYER_SAVE_TIME;
		UInt32 startTime = lastCheckTime <= saveTime ? 0 : lastCheckTime - saveTime;
		UInt32 endTime = nowTime <= saveTime ? 0 : nowTime - saveTime;

		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		cmd->PutColumn("accid");
		cmd->PutCond("deletetime", startTime, ">");
		cmd->PutCond("deletetime", endTime, "<=");
		cmd->PutCond("level", DIRECT_DELETE_PLAYER_LEVEL, ">=");
		CLRecordClient::Instance()->SendCommand(cmd, new QueryForeverDeletedPlayerCallback(1, nowTime));
	}

	GameParamMgr::Instance()->SetValue("check_delete_player_time", nowTime);
}

void WSPlayerMgr::ExecutePacket(ObjID_t id, Avalon::Packet *packet)
{
	WSPlayer* player = FindNormalPlayer(id);
	if(player != NULL)
	{
		m_Executor.ExecutePacket(packet,player);
	}
}

bool WSPlayerMgr::IsNeedWaitOhters()
{
	if (!WSApplication::Instance()->IsOpenLoginQueue())
	{
		return false;
	}

	UInt32 maxLoadingPlayerNum = WSApplication::Instance()->GetMaxLoadingPlayerNum();
	UInt32 maxPlayerNum = WSApplication::Instance()->GetMaxPlayerNum();
	if (GetLoadingPlayerNum() >= maxLoadingPlayerNum)
	{
		return true;
	}

	if (m_OnlineNum >= maxPlayerNum)
	{
		return true;
	}
	
	return false;
}

bool WSPlayerMgr::TryLetFirstPlayerLogin()
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

	CLProtocol::WorldAllowPlayerLogin protocol;
	protocol.accid = accid;
	Router::BroadcastToGate(protocol);

	InfoLogStream << "allow player(" << accid << ") login..." << LogStream::eos;

	DelFromWaitList(accid);

	return true;
}

UInt32 WSPlayerMgr::AddPlayerToWaitList(UInt32 accid)
{
	auto index = GetIndexInWaitList(accid);
	if (index >= 0)
	{
		return index + 1;
	}

	InfoLogStream << "player(" << accid << ") add to wait list." << LogStream::eos;

	m_waitPlayers.push_back(accid);
	return m_waitPlayers.size();
}

void WSPlayerMgr::DelFromWaitList(UInt32 accid)
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

int WSPlayerMgr::GetIndexInWaitList(UInt32 accid)
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

void WSPlayerMgr::SyncWaitLoginPlayerInfo()
{
	UInt32 waitPlayerNum = 1;
	for (auto itr = m_waitPlayers.begin(); itr != m_waitPlayers.end(); ++itr)
	{
		CLProtocol::WorldSyncPlayerLoginWaitInfo sync;
		sync.accid = *itr;
		sync.waitPlayerNum = waitPlayerNum++;
		Router::BroadcastToGate(sync);
	}
	m_needSyncWaitQueue = false;
}

bool WSPlayerMgr::AddOfflinePlayerCache(WSPlayer* player)
{
	return m_Players.AddObject(player);
}

void WSPlayerMgr::AddPfPlayerNum(std::string pf)
{
	UInt32& refNum = m_Pf2PlayerNum[pf];
	refNum++;
}

UInt32 WSPlayerMgr::GetPfPlayerNum(std::string pf)
{
	auto itr = m_Pf2PlayerNum.find(pf);
	if (itr != m_Pf2PlayerNum.end())
	{
		return itr->second;
	}

	return 0;
}

void WSPlayerMgr::ClearPfPlayerNum()
{
	m_Pf2PlayerNum.clear();
}

bool WSPlayerMgr::_RemovePlayerInAccountMap(WSPlayer* player)
{
	InfoLogStream << "player(" << player->GetAccId() << ", " << player->GetID() << ") remove from account map." << LogStream::eos;

	bool removeAccountPlayer = true;
	auto accPlayer = FindPlayerByAccid(player->GetAccId());
	if (accPlayer != NULL)
	{
		DebugLogStream << "current player(" << player->GetAccId() << ", " << player->GetID() << ") in account map." << LogStream::eos;

		if (accPlayer->GetID() != player->GetID())
		{
			DebugLogStream << "current player(" << player->GetAccId() << ", " << player->GetID() << ") not equal to remove player(" << player->GetID() << ", " << player->GetAccId() << ")." << LogStream::eos;
			if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_CHECK_ROLEID_WHEN_REMOVE_ACCOUNT_PLAYER))
			{
				removeAccountPlayer = false;
			}
		}
	}

	if (removeAccountPlayer)
	{
		m_AccidPlayers.erase(player->GetAccId());
		return true;
	}
	else
	{
		InfoLogStream << "current player(" << player->GetAccId() << ", " << player->GetID() << ") not equal to player in account map, don't remove player in account map." << LogStream::eos;
	}

	return false;
}