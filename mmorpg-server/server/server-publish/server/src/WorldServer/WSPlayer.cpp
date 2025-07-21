#include "WSPlayer.h"

#include <CLGateProtocol.h>
#include <CLEnterGameProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLRecordClient.h>
#include <CLRelationProtocol.h>
#include <CLItemProtocol.h>
#include <CLBillingProtocol.h>
#include <CLGSActivityProtocol.h>
#include <CLWorldLevelProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLGuildProtocol.h>
#include <CLDBLogger.h>
#include <CLFunctionDataEntry.h>
#include <CLTeamDungeonDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLSeasonDataEntry.h>
#include <CLPremiumLeagueProtocol.h>
#include <CLCenterProtocol.h>
#include <CLRedPacketProtocol.h>
#include <CLRedPacketDataEntry.h>
#include <CLMsgParser.h>
#include <ActivityUtil.h>
#include <CLSecurityLockProtocol.h>
#include <CLSecurityLockDefine.h>
#include <CLRoleProtocol.h>
#include <CLPlayerDataEntry.h>
#include <CLAdventureTeamDataEntry.h>
#include <CLAdventureTeamProtocol.h>
#include <MallHelper.h>
#include <CLAdventureTeamGradeDataEntry.h>
#include <CLSettingDefine.h>
#include <CLSettingProtocol.h>
#include <CLNewTitleDataEntry.h>
#include <CLBattleProtocol.h>
#include <CLTeamCopyDefine.h>

#include "WSNetwork.h"
#include "WSSceneMgr.h"
#include "WSPlayerMgr.h"
#include "WSTeamMgr.h"
#include "WSItemTransaction.h"
#include "WSRouter.h"
#include "WorldBossMgr.h"
#include "WSSortListMgr.h"
#include "PunishmentMgr.h"
#include "BroadcastMailMgr.h"
#include "WSActivityMgr.h"
#include "GameParamMgr.h"
#include "kingnet_analyzer.h"
#include "MallMgr.h"
#include "WorldSysNotify.h"
#include "OfflineNotifyMgr.h"
#include "DungeonRaceMgr.h"
#include "SysRecordMgr.h"
#include "GuildMgr.h"
#include "GuildBattleMgr.h"
#include "RedPacketMgr.h"
#include "AccountMgr.h"
#include "TeamMatchMgr.h"
#include "WSSceneMgr.h"
#include "WSPlayerLoading.h"
#include "FigureStatueMgr.h"
#include "PremiumLeagueMgr.h"
#include "LoginPushMgr.h"
#include "AccountActivityTaskMgr.h"
#include "RoomMgr.h"
#include "WSApplication.h"
#include "WSPlayerShareDataMgr.h"
#include "WSShareDataLoadCallback.h"
#include "DigMapMgr.h"
#include "ScriptMgr.h"
#include "TimerScript.h"
#include "WSServiceSwitchMgr.h"
#include "GuildDungeonMgr.h"
#include "GuildAuctionMgr.h"
#include "WSItemTransactionUtil.h"
#include "AdventurePassMgr.h"
#include "BlackMarketMgr.h"
#include "HireMgr.h"

void SelectRoleInfoCallback::OnFinished()
{
	DebugLogStream << "player(" << m_PlayerId << ") load(" << m_LoadSerial << ") finish." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_PlayerId);
	if(player == NULL || player->GetGameStatus() != PS_INIT || !player->CheckLoadDataSerial(m_LoadSerial)) return;

	OnFinished(player);
	player->SetLoadDataFinished(m_LoadSerial);
}

void SelectRoleInfoCallback::OnLoadFailed()
{
	ErrorLogStream << "player(" << m_PlayerId << ") load(" << m_LoadSerial << ") failed." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_PlayerId);
	if(player == NULL || player->GetGameStatus() != PS_INIT || !player->CheckLoadDataSerial(m_LoadSerial)) return;

	OnFailed(player);
}

/**
 *@brief 查询玩家全局信息回调
 */
class SelectGlobalInfoCallback : public SelectRoleInfoCallback
{
public:
	SelectGlobalInfoCallback(WSPlayer* player):SelectRoleInfoCallback(player){}
	~SelectGlobalInfoCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->OnLoadGlobalInfo(this);
	}

	void OnFailed(WSPlayer* player)
	{
	}
};

/**
*@brief 查询玩家拥有新职业回调
*/
class QueryOwnNewOccupationsCallback : public CLRecordCallback
{
public:
	QueryOwnNewOccupationsCallback(ObjID_t roleid, bool isOnline)
		: m_RoleId(roleid), m_IsPlayerOnOnline(isOnline){}

public:
	virtual void OnFinished()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		auto account = player->GetAccountInfo();
		if (!account) return;

		std::set<UInt8> occus;
		UInt16 maxLevel = PlayerDataEntryMgr::Instance()->GetMaxLevel();

		while (NextRow())
		{
			UInt16 level = GetData("level");
			UInt8 occu = GetData("occu");
			if (level == maxLevel && occu % 10 != 0)
			{
				occus.insert(occu);
			}
		}

		CLProtocol::WorldQueryOwnNewOccupationsSync protocol;

		// 老玩家第一次上线
		if (m_IsPlayerOnOnline && account->GetQueryUnlockedNewOccusTime() == 0)
		{
			// 如果是合过的服，那么所有职业都不是新职业
			if (GAME_FIRST_START_TIME != GAMESTART_TIME)
			{
				std::map<UInt8, UInt8> ownNewOccus;
				for (auto elem : occus)
				{
					ownNewOccus[elem] = 0;
				}
				account->SetUnlockedNewOccus(ownNewOccus);
			}
			else
			{
				// 不是则将已有职业设为新职业
				account->SetQueryUnlockedNewOccusTime((UInt32)CURRENT_TIME.Sec());
				std::map<UInt8, UInt8> occusVec;
				for (auto elem : occus)
				{
					occusVec[elem] = 1;
					protocol.ownNewOccus.push_back(elem);
				}
				account->SetUnlockedNewOccus(occusVec);
			}
		}

		std::map<UInt8, UInt8> ownNewOccus;
		account->GetUnlockedNewOccus(ownNewOccus);

		for (auto itr = ownNewOccus.begin(); itr != ownNewOccus.end();)
		{
			if (m_IsPlayerOnOnline && occus.find(itr->first) == occus.end())
			{
				// 当玩家再次上线，会检查解锁新职业列表有没有在角色列表中存在
				itr = ownNewOccus.erase(itr);
				continue;
			}

			// 新职业大于0，看过后为0
			if (itr->second > 0)
			{
				protocol.ownNewOccus.push_back(itr->first);
			}
			++itr;
		}

		if (m_IsPlayerOnOnline)
		{
			account->SetUnlockedNewOccus(ownNewOccus);
		}

		account->SetQueryUnlockedNewOccusTime((UInt32)CURRENT_TIME.Sec());
		player->SendProtocol(protocol);
	}

	virtual void OnFailed()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		CLProtocol::WorldQueryOwnNewOccupationsSync protocol;
		player->SendProtocol(protocol);

		ErrorLogStream << PLAYERINFO(player) << "Failed to select table t_player_info!" << LogStream::eos;
	}

private:
	ObjID_t m_RoleId;
	bool m_IsPlayerOnOnline;
};

std::map<UInt32, UInt32> WSPlayer::m_ScenePlayerNum;

WSPlayer::WSPlayer()
{
	m_IsMatching = false;
	m_AccId = 0;
	m_TeamId = 0;
	m_OppoAmberLevel = 0;
	m_IpValue = 0;

	m_ZoneId = 0;
	m_SrcZoneId = 0;
	
	m_Level = 0;
	m_Occu = 0;
	m_Sex = 0;
	m_Power = 0;

	m_AuctionRefreshTime = 0;
	m_AuctionAdditionBooth = 0;

	m_PunishFlag = 0;
	m_ForbitLoginDueTime = 0;
	m_ForbitTalkDueTime = 0;
	m_CreateTime = 0;

	m_MailBox.SetOwner(this);
	m_RelationMgr.SetOwner(this);
	m_MasterSys.SetOwner(this);
	m_OfflineEventMgr.SetOwner(this);
	m_MallGiftPackMgr.SetOwner(this);
	m_CondActivateMgr.SetOwner(this);
	m_PlayerActivityInfo.SetOwner(this);

	m_CreateCopyTimer.SetLastTime(CURRENT_TIME.MSec());
	m_CreateCopyTimer.SetInterval(5000);
	m_RecruitTimer.SetInterval(10000);
	m_RecruitTimer.SetLastTime(CURRENT_TIME.MSec());

	m_Status = PS_INIT;

	m_LoadDataCount = 0;
	m_LoadDataSerial = 0;

	m_pItemTrans = NULL;
	m_ItemTransTimeout = 0;

	m_LeaveTimeout = 0;
	m_OfflineTime = 0;
	m_LocalOfflineTime = 0;
	m_OfflineScene = 0;
	m_OfflineMailTime = 0;

	for(int i = 0; i < LOGICTICK_MAX; ++i)
	{
		m_TickTime[i] = CURRENT_TIME.MSec();
	}

	m_uGSMallBuyInfo = 0;

	m_pPasture = NULL;
	m_SceneId = 0;
	m_SceneType = SCENE_TYPE_INVALID;
	m_OnlineTime = 0;

	m_bDayChanged = 0;
	m_GuildId = 0;
	m_GuildLvl = 0;
	m_AccountInfo = NULL;

	m_LoadPlayerType = 0;
	m_RequestTarget = 0;

	m_SeasonLv = SeasonLevelDataEntryMgr::Instance()->GetMinSeasonLevelValue();
	m_bLogicDayChanged = 0;

	m_MatchStatus = FRIEND_MATCH_STATUS_OFFLINE;
	m_MonthCardExpireTime = 0;

	m_RefTime = 0;
	m_RoomId = 0;
	m_QuitRoomId = 0;
	m_QuitRoomTime = 0;
	m_totalPlayerChargeNum = 0;
	m_loadOfflineDataFail = false;
	m_inCrossScene = false;

	m_ResistMagic = 0;
	m_RaceID = 0;

	m_ScoreWarLoseStreak = 0;
	m_ScoreWarWinStreak = 0;
	m_MatchScore = 0;
	m_ScoreWarBattleCount = 0;
	m_StatusTime = (UInt32)CURRENT_TIME.Sec();
	m_AuctionProhibitTimeStamp = 0;

	m_playerOfflineLoadCb = nullptr;
	m_GuildDungeonCD = 0;
	m_DebugGuildDungeonMaxDamage = 0;

	m_RoleReturnTime = 0;
	m_ReturnFirstLogin = 0;
	m_ReturnYearTitle = 0;
	m_HeadFrame = 0;
	m_curWearTitleGuid = 0;

	m_AuctionProhibitQueryPriceListTimeStamp = 0;
	m_TitleMgr.SetOwner(this);
	m_DailyTodoMgr.SetOwner(this);
}

WSPlayer::~WSPlayer()
{
	CL_SAFE_DELETE(m_pItemTrans);

	auto guildMember = GuildMgr::Instance()->FindGuildMember(GetID());
	if (guildMember && guildMember->GetPlayer() == this)
	{
		ErrorLogStream << "set guild member(" << GetID() << ")'s player pointer to null." << LogStream::eos;
		guildMember->SetPlayer(NULL);
	}
}

UInt32 WSPlayer::GetServerPlayerNum(UInt32 nodeId)
{
	std::map<UInt32, UInt32>::iterator iter = m_ScenePlayerNum.find(nodeId);
	if(iter != m_ScenePlayerNum.end())
	{
		return iter->second;
	}
	return 0;
}

bool WSPlayer::Init(ObjID_t id, UInt32 accid, const std::string &name, UInt32 sceneid, UInt32 gateid)
{
	if(!SetSceneNode(sceneid)) return false;

	m_Conn = WSNetwork::Instance()->FindConnection(gateid);
	if(m_Conn == NULL) return false;

	SetID(id);
	m_AccId = accid;
	SetName(name);
	m_LeaveTimeout = 0;
	return true;
}

UInt32 WSPlayer::GetGateID() const
{
	if(m_Conn != NULL)
	{
		return m_Conn->GetID();
	}
	return 0;
}

void WSPlayer::SetGateID(UInt32 nodeId)
{
	m_Conn = WSNetwork::Instance()->FindConnection(nodeId);
}

AccountInfo* WSPlayer::GetAccountInfo() const
{
	return m_AccountInfo;
}

void WSPlayer::SetAccountInfo(AccountInfo* info)
{
	m_AccountInfo = info;
	if (m_AccountInfo)
	{
		m_AccGuid = m_AccountInfo->GetID();
		m_AccountInfo->SetRoleID(GetID());
		if (m_Conn == NULL)
		{
			ErrorLogStream << "connection null " << PLAYERINFO(this) << LogStream::eos;
		}
		else
		{
			m_AccountInfo->SetGateConn(m_Conn->GetID());
		}
	}
}

void WSPlayer::OnHeadFrameChanged(UInt32 headFrameId)
{
	m_RelationMgr.OnHeadFrameChanged();
}

bool WSPlayer::SetSceneNode(UInt32 nodeid)
{
	m_SceneConn =  WSNetwork::Instance()->FindConnection(nodeid);
	return m_SceneConn != NULL;
}

UInt32 WSPlayer::GetSceneNode() const
{
	if(m_SceneConn != NULL)
	{
		return m_SceneConn->GetID();
	}
	return 0;
}

void WSPlayer::IncExpActive(const std::string& reason, UInt64 incExp)
{
	CLProtocol::WorldIncreasePlayerExpReq req;
	req.playerId = GetID();
	req.reason = reason;
	req.incExp = incExp;
	SendToScene(req);

	InfoLogStream << PLAYERINFO(this) << " increase exp(" << incExp << ") with reason(" << reason << ")." << LogStream::eos;
}

void WSPlayer::NotifyLeaveGame(UInt8 isOfflineTimeOut)
{
	if (GetGameStatus() == PS_FINAL || GetGameStatus() == PS_LEAVEGAME)
	{
		DebugLogStream << "player(" << GetID() << ") is already offline" << LogStream::eos;
		return;
	}

	if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_OFFLINE_PLAYER_MGR) && IsOfflineLoad())
	{
		// 离线加载数据，不需要处理
		InfoLogStream << "player(" << GetID() << ") offline load, just return." << LogStream::eos;
		return;
	}

    SetGameStatus(PS_LEAVEGAME);
	m_LeaveTimeout = LEAVEGAME_TIMEOUT;

	DebugLogStream << "player(" << GetID() << ") start to logout." << LogStream::eos;

	GameInfoLogStream("Player") << "player(" << GetAccId() << "," << GetID() << "," << GetName() << ") notified by gate to leave game!" << LogStream::eos;

	auto team = GetTeam();
	if (team)
	{
		team->OnPlayerOffline(this);
	}

	CLProtocol::SceneLeaveGameReq req;
	req.id = GetID();
	req.isOfflineTimeOut = isOfflineTimeOut;
	SendToScene(req);

	CLProtocol::MatchServerMatchCancelReq msReq;
	msReq.id = GetID();
	Router::SendToMatch(msReq);

	DungeonRaceMgr::Instance()->LeaveDungeonRace(GetID(), true);
}

//void WSPlayer::IncExp(UInt32 exp)
//{
//	if (exp == 0) return;
//
//	CLProtocol::WorldAddExpReq req;
//	req.playerId = GetID();
//	req.exp = exp;
//	SendToScene(req);
//}

void WSPlayer::SetIp(const std::string& ip)
{
	m_Ip = ip;

	if(!m_Ip.empty())
	{
		m_IpValue = 0;
		UInt32 value = 0;
		char split = 0;

		std::istringstream iss(m_Ip);
		iss >> value >> split;
		m_IpValue |= (value << 24);
		iss >> value >> split;
		m_IpValue |= (value << 16);
		iss >> value >> split;
		m_IpValue |= (value << 8);
		iss >> value >> split;
		m_IpValue |= value;
	}
}

void WSPlayer::SetPunishFlag(UInt32 flag)
{
	m_PunishFlag = flag;
	if(CheckPunishFlag(PUNISH_FORBIDLOGIN) && m_ForbitLoginDueTime > CURRENT_TIME.Sec())
	{
		char buf[256];
		buf[0] = 0;
		std::string timestr = SysNotifyMgr::Instance()->TimeToStr(m_ForbitLoginDueTime - UInt32(CURRENT_TIME.Sec()));
		SysNotifyMgr::Instance()->MakeNotify(252, buf, 256, timestr.c_str());
		KickOff(ErrorCode::SYS_FORBIT_LOGIN, buf);
	}
}

/*
//by huchenhui
void WSPlayer::SendNotify(UInt32 id, ...)
{
	va_list args;
	va_start(args, id);
	UInt16 type = 0;
	UInt8 color = 0;
	char buffer[CL_MAX_CHAT_LEN+1];
	bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
	va_end(args);

	if(!ret) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = buffer;
	SendProtocol(notify);
}
*/

void WSPlayer::Notify(UInt16 type, UInt8 color, const char* word)
{
	if(word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;
	SendProtocol(notify);
}

class PutOfflineEventSelectCb : public CLRecordCallback
{
public:
	PutOfflineEventSelectCb(WSPlayer* my, ObjID_t uid, OfflineEvent* offlineevent) : m_my(my), m_TarUid(uid), m_OfflineEvent(offlineevent){}
	~PutOfflineEventSelectCb(){}

	void OnFinished()
	{
		bool find = false;
		while (NextRow())
		{
			UInt16 type = GetData("type");

			OfflineEvent* offline_event = OfflineEvent::Create(type);
			if (offline_event == NULL) continue;

			offline_event->SetDBData(this);

			GlobalOfflineEventMgr::Instance()->AddOfflineEvent(m_TarUid, offline_event);

			find = true;
		}

		if (!find)
		{
			GlobalOfflineEventMgr::Instance()->AddOfflineEvent(m_TarUid, NULL);
		}

		std::vector<OfflineEvent*> events;
		UInt32 ret = GlobalOfflineEventMgr::Instance()->FindOfflineEvent(m_TarUid, m_OfflineEvent->GetType(), events);

		if (ret != GOEQT_NO_CACHE)
		{
			if (!m_my->CanPutOfflineEvent(m_OfflineEvent->GetType(), events))
			{
				return;
			}

			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_TarUid);
			if (player == NULL)
			{
				GlobalOfflineEventMgr::Instance()->AddOfflineEvent(m_TarUid, m_OfflineEvent);
				m_OfflineEvent->InsertToDB(m_TarUid);
			}
			else if (player->GetGameStatus() != PS_NORMAL)
			{
				player->GetOfflineEventMgr().PutEvent(m_OfflineEvent);
				m_OfflineEvent->InsertToDB(m_TarUid);
			}
			else
			{
				m_OfflineEvent->OnTriggered(player);
				CL_SAFE_DELETE(m_OfflineEvent);
			}
		}
	}

private:
	WSPlayer*		m_my;
	ObjID_t			m_TarUid;
	OfflineEvent*	m_OfflineEvent;
};

void WSPlayer::PutOfflineEvent(ObjID_t playerId, OfflineEvent* offlineevent)
{
	if(playerId == 0 || offlineevent == NULL) return;

	std::vector<OfflineEvent*> events;
	UInt32 ret = GlobalOfflineEventMgr::Instance()->FindOfflineEvent(playerId, offlineevent->GetType(), events);
	if (ret == GOEQT_NO_CACHE)
	{
		// 需要查找数据库
		GlobalOfflineEventMgr::Instance()->SelectEvent(playerId, new PutOfflineEventSelectCb(this, playerId, offlineevent));
	}
	else
	{
		if (!CanPutOfflineEvent(offlineevent->GetType(), events))
		{
			CL_SAFE_DELETE(offlineevent);
			return;
		}

		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(playerId);
		if (player == NULL)
		{
			GlobalOfflineEventMgr::Instance()->AddOfflineEvent(playerId, offlineevent);
			offlineevent->InsertToDB(playerId);
		}
		else if (player->GetGameStatus() != PS_NORMAL)
		{
			player->m_OfflineEventMgr.PutEvent(offlineevent);
			offlineevent->InsertToDB(playerId);
		}
		else
		{
			offlineevent->OnTriggered(player);
			CL_SAFE_DELETE(offlineevent);
		}
	}
}

bool WSPlayer::CanPutOfflineEvent(UInt16 type, std::vector<OfflineEvent*>& events)
{
	switch (type)
	{
	case OFFLINE_INVITE_FRIEND:
	{
		//个人最大能接受离线申请好友50
		if (events.size() > 50)
		{
			return false;
		}
		
		//两个玩家间只能离线申请一次
		for (size_t i = 0; i < events.size(); ++i)
		{
			if (!events[i])
			{
				continue;
			}

			if (events[i]->GetTarUid() == GetID())
			{
				return false;
			}
		}

		break;
	}
	case OFFLINE_PRIVATE_CHAT:
	{
		//个人最大能接受的离线消息量100
		if (events.size() > 100)
		{
			 return false;
		}
		
		UInt32 num = 0;
		for (size_t i = 0; i < events.size(); ++i)
		{
			if (!events[i])
			{
				continue;
			}

			if (events[i]->GetTarUid() == GetID())
			{
				num++;
			}
		}

		//两个玩家间最大离线消息量50
		if (num >= 50)
		{
			return false;
		}

		break;
	}
	default:	break;
	}

	return true;
}

void WSPlayer::KickOff(UInt32 errorCode, const char* msg)
{
	CLProtocol::GateKickOffPlayer kickOff;
	kickOff.accid = GetAccId();
	kickOff.errorCode = errorCode;
	if(msg != NULL)
	{
		kickOff.msg = msg;
	}
	SendToGate(kickOff);
	m_Conn = NULL;
}

void WSPlayer::LeaveGame()
{
	if(GetGameStatus() == PS_FINAL) return;

	if(GetGameStatus() == PS_NORMAL || GetGameStatus() == PS_LEAVEGAME)
	{
		OnOffline();
		Saveback();
	}
	else if(GetGameStatus() == PS_INIT)
	{
		WSPlayerMgr::Instance()->RemovePlayer(this);
		delete this;
	}
}

bool WSPlayer::CheckCopySceneRecruit()
{
	if(!m_RecruitTimer.IsTimeout(CURRENT_TIME))
	{
		UInt32 sec = UInt32((m_RecruitTimer.GetLastTime() + m_RecruitTimer.GetInterval() - CURRENT_TIME.MSec()) / 1000);
		if(sec == 0) sec = 1;
		SendNotify(1016, sec);
		return false;
	}
	return true;
}

void WSPlayer::SendBillingResult(UInt32 billingId, UInt8 result)
{
	CLProtocol::WorldBillingGiveItemRet ret;
	ret.nodeId  = billingId;
	ret.accId = GetAccId();
	ret.level = GetLevel();
	ret.isqqvip = m_qqVipInfo.IsVip()?1:0;
	ret.qqviplvl = m_qqVipInfo.level;
	ret.result = result;
	Router::SendToAdmin(ret);
}

void WSPlayer::SyncRequest(UInt8 type, WSPlayer* pRequester, const std::string& param)
{
	if(pRequester == NULL) return;

	CLProtocol::SceneSyncRequest sync;
	sync.type = type;
	sync.requester = pRequester->GetID();
	sync.requesterName = pRequester->GetName();
	sync.requesterOccu = pRequester->GetOccu();
	sync.requesterLevel = pRequester->GetLevel();
	sync.param1 = param;
	sync.vipLv = pRequester->GetVipLvl();
	sync.avatar = pRequester->GetAvatar();
	auto questionnaire = pRequester->GetMasterSystem().GetQuestionnaire();
	sync.activeTimeType = questionnaire.activeTimeType;
	sync.masterType = questionnaire.masterType;
	sync.regionId = questionnaire.regionId;
	sync.declaration = questionnaire.declaration;

	SendProtocol(sync);
}

void WSPlayer::SetOfflineMailTime(UInt64 time)
{
	m_OfflineMailTime = time;
}

void WSPlayer::SaveOfflineMailTime(UInt64 time)
{
	m_OfflineMailTime = time;
	CLUpdateCommand *cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("offlinemailtime", m_OfflineMailTime);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WSPlayer::SyncRelationDataToScene()
{
	CLProtocol::SceneSyncSocialData sync;
	sync.playerId = GetID();
	sync.teamId = GetTeamID();

	SendToScene(sync);
}

bool WSPlayer::BeginItemTransaction(ItemTransaction* transaction)
{
	if(transaction == NULL || transaction->m_Status != ItemTransaction::STATUS_INIT)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	//当前事务没结束
	if(m_pItemTrans != NULL && m_pItemTrans->m_Status != ItemTransaction::STATUS_FINAL)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	CL_SAFE_DELETE(m_pItemTrans);
	m_pItemTrans = transaction;
	m_ItemTransTimeout = ITEM_TRANS_TIMEOUT;

	m_pItemTrans->CheckItem();
	return true;
}

void WSPlayer::UnLockAsserts()
{
	CLProtocol::SceneUnlockItemReq req;
	req.playerId = GetID();
	SendToScene(req);
}

void WSPlayer::RemoveItem(ObjID_t itemId)
{
	CLProtocol::SceneRemoveItemObjReq req;
	req.playerId = GetID();
	req.itemId = itemId;
	SendToScene(req);
}

void WSPlayer::OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes)
{
	if(m_pItemTrans == NULL || m_pItemTrans->m_Status != ItemTransaction::STATUS_CHECK || m_pItemTrans->GetSerial() != serial) return;

	m_pItemTrans->SetCounterTimes(counterTimes);
	if(result == ErrorCode::SUCCESS)
	{
		m_pItemTrans->m_Status = ItemTransaction::STATUS_CHECKED;
		m_pItemTrans->OnCheckSucced(this);
	}
	else
	{
		m_pItemTrans->m_Status = ItemTransaction::STATUS_FINAL;
		m_pItemTrans->SetErrorCode(result);
		m_pItemTrans->OnCheckFailed(this);
	}
}

bool WSPlayer::IsOpenTeamFunction()
{
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_TEAM);
	if (functionData)
	{
		if (GetLevel() < functionData->endLevel)
		{
			return false;
		}
	}

	return true;
}

bool WSPlayer::IsSceneCanJoinTeam()
{
	if (m_SceneType == SCENE_TYPE_PK_PREPARE || m_inCrossScene == true 
		|| m_SceneType == SCENE_TYPE_CHIJI_PREPARE || m_SceneType == SCENE_TYPE_PK
		|| m_SceneType == SCENE_TYPE_TEAM_COPY || m_SceneType == SCENE_TYPE_CHAMPION)
	{
		return false;
	}

	return true;
}

void WSPlayer::SetTeam(WSTeam* team)
{
	if (team)
	{
		m_TeamId = team->GetId();
	}
	else
	{
		m_TeamId = WSTeam::INVALID_TEAM_ID;
	}

	DebugLogStream << "player(" << GetID() << ") teamid(" << m_TeamId << ")." << LogStream::eos;
}

WSTeam* WSPlayer::GetTeam() const
{
	if (m_TeamId != WSTeam::INVALID_TEAM_ID)
	{
		return WSTeamMgr::Instance()->FindTeam(m_TeamId);
	}
	else
	{
		return NULL;
	}
}

void WSPlayer::CreateTeam(UInt32 target, bool cancelMatch)
{
    CLProtocol::WorldCreateTeamRes res;
	if(GetTeam() != NULL)	//已有队伍
	{
        res.result = ErrorCode::TEAM_CREATE_TEAM_HAS_TEAM;
        SendProtocol(res);
		return;
	}

	// 是否开放
	if (!IsOpenTeamFunction())
	{
		res.result = ErrorCode::TEAM_MIN_LEVEL;
		SendProtocol(res);
		return;
	}

	// pk场景不能创建队伍
	if (!IsSceneCanJoinTeam())
	{
		res.result = ErrorCode::TEAM_JOIN_NOT_IN_TOWN;
		SendProtocol(res);
		return;
	}

	auto teamDungeonData = TeamDungeonDataEntryMgr::Instance()->FindEntry(target);
	if (!teamDungeonData || teamDungeonData->type == TEAM_DUNGEON_MENU)
	{
		res.result = ErrorCode::TEAM_CREATE_TEAM_INVALID_TARGET;
		SendProtocol(res);
		return;
	}

	if (cancelMatch)
	{
		TeamMatchMgr::Instance()->OnPlayerCancelMatch(this);
	}

	//创建队伍
	WSTeam* team = new WSTeam(this);
    team->SetTeamTarget(target);
	team->SetMaxTeamMemberNum(teamDungeonData->maxPlayerNum);
	team->SetTargetDungeonID(teamDungeonData->dungeonId);
    
	if(!WSTeamMgr::Instance()->AddTeam(team))
	{
        SetTeam(NULL);
		CL_SAFE_DELETE(team);
        res.result = ErrorCode::TEAM_ERROR;
        SendProtocol(res);
		return;
	}

	SetTeam(team);
	InfoLogStream << "player(" << GetID() << ") create team(" << team->GetId() << ")." << LogStream::eos;

	team->UpdateSceneInfo();
	team->SyncTeamInfo();

	if (team->IsTargetTimesLimit())
	{
		team->QueryAllMemberRemainTimes(team->GetTargetDungeonID());
	}

	team->ControlKickMaster(teamDungeonData->autoKick);

    res.result = ErrorCode::SUCCESS;
    SendProtocol(res);
}

UInt32 WSPlayer::InviteTeam(ObjID_t targetId)
{
	auto leftTime = GetRequestMgr().GetRequestLeftTime(REQUEST_INVITETEAM, targetId);
	if (leftTime > 0)
	{
		ErrorLogStream << "player(" << GetID() << ") invite player(" << targetId << ") frequently." << LogStream::eos;
		SendNotify(3128, leftTime);
		return ErrorCode::TEAM_INVITE_FREQUENTLY;
	}

	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	if(target == NULL)	//目标不在线
	{
		InfoLogStream << "player(" << GetID() << ") invite offline player(" << targetId << ") to join team." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TARGET_OFFLINE;
	}

	// 等级
	if (!IsOpenTeamFunction())
	{
		DebugLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, player level(" << GetLevel() << ")." << LogStream::eos;
		return ErrorCode::TEAM_MIN_LEVEL;
	}

	if (!target->IsOpenTeamFunction())
	{
		DebugLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, target level(" << target->GetLevel() << ")." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TARGET_MIN_LEVEL;
	}

	// 对方已经有队伍了
	if (target->GetTeam())
	{
		InfoLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, target in team." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TARGET_IN_TEAM;
	}

	if (target->GetMatchStatus() != FRIEND_MATCH_STATUS_IDLE)
	{
		InfoLogStream << "player(" << GetID() << ") invite busy player(" << targetId << ") to join team." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TARGET_BUSY;
	}

	if (!target->IsSceneCanJoinTeam())
	{
		InfoLogStream << "player(" << GetID() << ") invalid player(" << targetId << ") to join team, invalid scene(" << m_SceneId << ")." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TARGET_BUSY;
	}

	// 自己还没有队伍
	auto team = GetTeam();
	if(team == NULL)
	{
		// 创建一个无目标的队伍
		CreateTeam(1);
		team = GetTeam();
		if (team == NULL)
		{
			InfoLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, player not in team." << LogStream::eos;
			return ErrorCode::TEAM_INVITE_NO_TEAM;
		}
	}

	// 只有队长能邀请
	/*
	if (team->GetMaster() != GetID())
	{
		InfoLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, player not team master." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_NOT_TEAM_MASTER;
	}
	*/
	
	if (team->GetMaster() != GetID() && !team->IsAutoAgree())
	{
		return ErrorCode::TEAM_INVITE_MASTER_NOT_AGREE;
	}

	// 队伍已满
	if (team->IsFull())
	{
		InfoLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, team is full." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TEAM_FULL;
	}

	// 战斗中不能邀请
	if (team->GetStatus() != TEAM_STATUS_IDEL)
	{
		InfoLogStream << "player(" << GetID() << ") invite player(" << targetId << ") to join team, team is in racing." << LogStream::eos;
		return ErrorCode::TEAM_INVITE_TEAM_IN_RACE;
	}

	if (team->IsInInviteList(targetId))
	{
		return ErrorCode::TEAM_INVITE_REPEAT;
	}

	// 玩家是否屏蔽邀请
	if (target->GetGameSetValue<UInt32>(GST_SECRET, SST_TEAM_INVATE))
	{
		return ErrorCode::SETTING_TARGET_SHILED_INVATE;
	}

	team->AddInvitePlayer(target);

	CLProtocol::WorldTeamInviteNotify notify;
	notify.info = team->GetBaseInfo(GetID());
	target->SendProtocol(notify);

	auto inviteIntervalSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TEAM_INVITE_INTERVAL_SEC);
	GetRequestMgr().AddRequest(REQUEST_INVITETEAM, targetId, inviteIntervalSec);

	return ErrorCode::SUCCESS;
}

UInt32 WSPlayer::ReplyInviteTeam(UInt32 teamId, bool agree)
{
	// 失败通知
	// ToDo...

	WSTeam* team = WSTeamMgr::Instance()->FindTeam(teamId);
	if(team == NULL)
	{
		return ErrorCode::TEAM_JOIN_TEAM_UNEXIST;;
	}

	// 不同意加入队伍
	if (!agree)
	{
		team->DelInvitePlayer(GetID());
		return ErrorCode::SUCCESS;
	}

	// 已经在队伍中了
	if (GetTeam())
	{
		return ErrorCode::TEAM_JOIN_TEAM_HAS_TEAM;
	}

	if (GetMatchStatus() != FRIEND_MATCH_STATUS_IDLE)
	{
		return ErrorCode::TEAM_JOIN_NOT_IN_TOWN;
	}

	if (!IsSceneCanJoinTeam())
	{
		return ErrorCode::TEAM_JOIN_NOT_IN_TOWN;
	}

	// 队伍已满
	if (team->IsFull())
	{
		return ErrorCode::TEAM_JOIN_TEAM_FULL;
	}

	// 队伍邀请已失效
	if (!team->IsInInviteList(GetID()))
	{
		return ErrorCode::TEAM_JOIN_INVITE_TIMEOUT;
	}

	// 队伍在战斗中
	if (team->GetStatus() != TEAM_STATUS_IDEL)
	{
		return ErrorCode::TEAM_JOIN_RACING;
	}

	team->AddMember(this, true);
	return ErrorCode::SUCCESS;
}

void WSPlayer::RequestTeam(ObjID_t targetId, const std::string& passwd)
{
    CLProtocol::WorldJoinTeamRes res;
	res.inTeam = 0;

	// 等级
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_TEAM);
	if (functionData)
	{
		if (GetLevel() < functionData->endLevel)
		{
			res.result = ErrorCode::TEAM_MIN_LEVEL;
			SendProtocol(res);
			return;
		}
	}

	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	if(target == NULL)	//目标不在线
	{
        res.result = ErrorCode::TEAM_JOIN_TEAM_MASTER_OFFLINE;
        SendProtocol(res);
		return;
	}

	WSTeam* team = target->GetTeam();
	//目标没有队伍
	if(team == NULL) 
	{
        res.result = ErrorCode::TEAM_JOIN_TEAM_UNEXIST;
        SendProtocol(res);
		return;
	}

	//已有队伍
	if(GetTeam() != NULL) 
	{
        res.result = ErrorCode::TEAM_JOIN_TEAM_HAS_TEAM;
        SendProtocol(res);
		return;
	}

	if (!IsSceneCanJoinTeam())
	{
		res.result = ErrorCode::TEAM_JOIN_NOT_IN_TOWN;
		SendProtocol(res);
		return;
	}

	if ((res.result = team->RequestJoinTeam(this, passwd)) != ErrorCode::SUCCESS)
	{
		SendProtocol(res);
		return;
	}

	if (team->IsAutoAgree())
	{
		team->AddMember(this, true);
		res.inTeam = 1;
	}
	else
	{
		team->AddRequester(this);
	}

	SendProtocol(res);
}

void WSPlayer::RequestJoinTeam(UInt32 teamId, const std::string& passwd)
{
	CLProtocol::WorldJoinTeamRes res;

	WSTeam* team = WSTeamMgr::Instance()->FindTeam(teamId);
	if (team == NULL)
	{
		res.result = ErrorCode::TEAM_JOIN_TEAM_UNEXIST;
		SendProtocol(res);
		return;
	}

	if ((res.result = team->RequestJoinTeam(this, passwd)) != ErrorCode::SUCCESS)
	{
		SendProtocol(res);
		return;
	}

	team->AddMember(this, false);
}

void WSPlayer::ReplyRequestTeam(ObjID_t requesterId)
{
	WSPlayer* requester = WSPlayerMgr::Instance()->FindNormalPlayer(requesterId);
	if(requester == NULL)
	{
		SendNotify(513);
		return;
	}

    CLProtocol::WorldJoinTeamRes res;
	if(!requester->m_Requests.HasRequest(REQUEST_JOINTEAM, GetID()))
	{
        res.result = ErrorCode::TEAM_JOIN_TIMEOUT;
        requester->SendProtocol(res);
		return;
	}
	requester->m_Requests.RemoveRequest(REQUEST_JOINTEAM, GetID());

	if(requester->GetTeam() != NULL)
	{
		return;
	}

	if (!requester->IsSceneCanJoinTeam())
	{
		res.result = ErrorCode::TEAM_JOIN_NOT_IN_TOWN;
		requester->SendProtocol(res);
		return;
	}

	WSTeam* team = GetTeam();
	if(team == NULL)
	{
        res.result = ErrorCode::TEAM_JOIN_TEAM_UNEXIST;
        requester->SendProtocol(res);
		return;
	}

	if(team->IsFull())
	{
        res.result = ErrorCode::TEAM_JOIN_TEAM_FULL;
        requester->SendProtocol(res);
		return;
	}

	team->AddMember(requester, false);
}

void WSPlayer::InviteJoinGuild(ObjID_t targetId)
{
	if (GetGuildID() == 0)
	{
		// 没有公会
		SendNotify(3112);
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(GetGuildID());
	if (!guild)
	{
		// 没有公会，这里应该是进不来的
		ErrorLogStream << "player(" << GetID() << ") can't find guild(" << GetGuildID() << ")." << LogStream::eos;
		SendNotify(3112);
		return;
	}

	if (guild->IsGuildFull())
	{
		// 公会已满
		SendNotify(3113);
		return;
	}

	auto target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	if (!target)
	{
		// 目标已下线
		SendNotify(3115);
		return;
	}

	if (target->GetGuildID() > 0)
	{
		// 目标已经有公会
		SendNotify(3114);
		return;
	}
	
	if (m_Requests.HasRequest(REQUEST_INVITEGUILD, targetId))
	{
		SendNotify(3120);
		return;
	}

	// 玩家是否屏蔽邀请
	if (target->GetGameSetValue<UInt32>(GST_SECRET, SST_GUILD_INVATE))
	{
		SendNotify(9998);
		return; 
	}

	m_Requests.AddRequest(REQUEST_INVITEGUILD, targetId, 60);

	CLProtocol::WorldGuildInviteNotify notify;
	notify.inviterId = GetID();
	notify.inviterName = GetName();
	notify.inviterOccu = GetOccu();
	notify.inviterLevel = GetLevel();
	notify.inviterVipLevel = GetVipLvl();
	notify.guildId = GetGuildID();
	notify.guildName = guild->GetName();
	GetPlayerLabelInfo(notify.playerLabelInfo);
	target->SendProtocol(notify);

	// 邀请已发送
	SendNotify(3116);
}

void WSPlayer::InviteFriend(ObjID_t targetId, std::string tarName)
{
	FuncNotifyDataEntry* functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
	if (!functionData)
	{
		ErrorLogStream << "can't find friend system func data!" << LogStream::eos;
		return;
	}

	if (GetLevel() < functionData->endLevel)
	{
		ErrorLogStream << PLAYERINFO(this) << "player level isn't enough for inviting friend! lv:" << GetLevel() << "|needLv:" << functionData->beginLevel << LogStream::eos;
		return;
	}

	WSPlayer* target = NULL;
	if (targetId != 0)
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	}
	else
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayerByName(tarName);
	}

	if (target == NULL)	//目标不在线
	{
		SendNotify(9903);
		/*WSPlayer* player = new WSPlayer();
		player->SetOfflineReplyCtx(this, LT_MAKE_FRIEND);
		player->SetID(targetId);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
		player->LoadData();*/
		return;
	}

	//对方等级不满足加好友
	if (target->GetLevel() < functionData->endLevel)
	{
		SendNotify(1500005);
		return;
	}

	//目标是自己
	if (target == this)
	{
		SendNotify(3049);
		return;
	}

	targetId = target->GetID();

	//已经是好友
	if (m_RelationMgr.FindRelation(RELATION_FRIEND, targetId) != NULL)
	{
		SendNotify(3110);
		return;
	}

	// 在黑名单里
	/*if (m_RelationMgr.FindRelation(RELATION_BLACKLIST, targetId) != NULL
		|| target->m_RelationMgr.FindRelation(RELATION_BLACKLIST, GetID()) != NULL)
	{
		SendNotify(3107);
		return;
	}*/

	//是师徒
	if (m_RelationMgr.FindRelation(RELATION_MASTER, targetId) || m_RelationMgr.FindRelation(RELATION_DISCIPLE, targetId))
	{
		SendNotify(3110);
		return;
	}

	//自己好友数满
	if (m_RelationMgr.IsFull(RELATION_FRIEND))
	{
		SendNotify(3108);
		return;
	}

	//屏蔽添加好友
	UInt32 shield_lvl = target->GetGameSetValue<UInt32>(GST_FRIEND_INVATE);
	if (GetLevel() < shield_lvl)
	{
		SendNotify(9998);
		return;
	}

	//添加好友
	if (MakeFriendNew(target))
	{
		SendNotify(3102);
	}
}

void WSPlayer::ReplyInviteFriend(ObjID_t requesterId)
{
	//自己好友数满
	if (m_RelationMgr.IsFull(RELATION_FRIEND))
	{
		SendNotify(3103);
		return;
	}

	WSPlayer* requester = WSPlayerMgr::Instance()->FindPlayer(requesterId);
	if(requester == NULL)
	{
		WSPlayer* player = new WSPlayer();
		player->SetOfflineReplyCtx(this, LT_MAKE_FRIEND);
		player->SetID(requesterId);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
		player->LoadData();
		
		return;
	}

	if (MakeFriend(requester))
	{
		SendNotify(3102);
	}
	
}

bool WSPlayer::MakeFriend(WSPlayer* other)
{
	if (!other)
	{
		return false;
	}

	if (other->GetID() == 0)
	{
		return false;
	}

	//不能加自己为好友
	if (other == this)
	{
		SendNotify(3049);
		return false;
	}

	//已经是好友
	if (m_RelationMgr.FindRelation(RELATION_FRIEND, other->GetID()) != NULL
		|| other->m_RelationMgr.FindRelation(RELATION_FRIEND, GetID()) != NULL)
	{
		SendNotify(3106);
		return false;
	}

	//对方好友数满
	if (other->m_RelationMgr.IsFull(RELATION_FRIEND))
	{
		SendNotify(3105);
		return false;
	}

	//自己有关系更新关系
	WSRelation* myOldRelation = m_RelationMgr.GetRelation(other->GetID());
	if (myOldRelation)
	{
		//通知网关移除黑名单
		if (myOldRelation->GetType() == RELATION_BLACKLIST)
		{
			GetRelationMgr().SendNotifyDelBlackList(GetID(), other->GetID());
		}

		//更新关系
		UpdateRelationDebugLog debugLog("WSPlayer::MakeFriend 1");
		m_RelationMgr.UpdateRelation(other->GetID(), myOldRelation, RELATION_FRIEND);
	}
	else
	{
		//没关系创建
		WSRelation* newRelation = new WSRelation(RELATION_FRIEND, other);
		m_RelationMgr.AddRelation(newRelation);
	}

	//对方有关系更新关系
	WSRelation* tarOldRelation = other->m_RelationMgr.GetRelation(GetID());
	if (tarOldRelation)
	{
		if (tarOldRelation->GetType() == RELATION_BLACKLIST)
		{
			GetRelationMgr().SendNotifyDelBlackList(other->GetID(), GetID());
		}

		//更新关系
		UpdateRelationDebugLog debugLog("WSPlayer::MakeFriend 2");
		other->m_RelationMgr.UpdateRelation(GetID(), tarOldRelation, RELATION_FRIEND);
	}
	else
	{
		//没关系创建
		WSRelation* newRelation = new WSRelation(RELATION_FRIEND, this);
		other->m_RelationMgr.AddRelation(newRelation);
	}

	return true;
}

bool WSPlayer::MakeFriendNew(WSPlayer* other)
{
	if (!other)
	{
		return false;
	}

	if (other->GetID() == 0)
	{
		return false;
	}

	//不能加自己为好友
	if (other == this)
	{
		SendNotify(3049);
		return false;
	}

	//已经是好友
	if (m_RelationMgr.FindRelation(RELATION_FRIEND, other->GetID()) != NULL)
	{
		SendNotify(3106);
		return false;
	}

	//自己有关系更新关系
	WSRelation* myOldRelation = m_RelationMgr.GetRelation(other->GetID());
	if (myOldRelation)
	{
		//通知网关移除黑名单
		if (myOldRelation->GetType() == RELATION_BLACKLIST)
		{
			GetRelationMgr().SendNotifyDelBlackList(GetID(), other->GetID());
		}
		myOldRelation->SetRegressionMark(other->IsVeteranReturn());
		myOldRelation->SetHeadFrame(other->GetHeadFrame());
		myOldRelation->SetGuildId(other->GetGuildID());
		myOldRelation->SetReturnYearTitle(other->GetReturnYearTitle());

		//更新关系
		UpdateRelationDebugLog debugLog("WSPlayer::MakeFriendNew");
		m_RelationMgr.UpdateRelation(other->GetID(), myOldRelation, RELATION_FRIEND);
	}
	else
	{
		//没关系创建
		myOldRelation = new WSRelation(RELATION_FRIEND, other);
		myOldRelation->SetRegressionMark(other->IsVeteranReturn());
		m_RelationMgr.AddRelation(myOldRelation);
	}
	
	if (WSRelationMgr::IsMutualFriend(this, other))
	{
		myOldRelation->SetMutualFriendMark();
		myOldRelation->SyncToClient(this);
		WSRelation* targetRelation = other->GetRelationMgr().FindRelation(RELATION_FRIEND, this->GetID());
		if (targetRelation)
		{
			targetRelation->SetMutualFriendMark();
			targetRelation->SyncToClient(other);
		}
	}

	GetRelationMgr().OnAddFriend(myOldRelation);
	SendAddFriendUdpLog( GetGuildID(), other->GetLevel(), other->GetVipLvl(), other->GetOccu(), other->GetGuildID());
	return true;
}

bool WSPlayer::MakeFriendOfflineCb(WSPlayer* target)
{
	if (!target || target->GetID() == 0)
	{
		return false;
	}
	
	//不能加自己为好友
	if (target == this)
	{
		SendNotify(3049);
		return false;
	}

	FuncNotifyDataEntry* functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
	if (!functionData)
	{
		ErrorLogStream << "can't find friend system func data!" << LogStream::eos;
		return false;
	}

	if (GetLevel() < functionData->endLevel)
	{
		ErrorLogStream << PLAYERINFO(this) << "player level isn't enough for inviting friend! lv:" << GetLevel() << "|needLv:" << functionData->beginLevel << LogStream::eos;
		return false;
	}

	//对方等级不满足加好友
	if (target->GetLevel() < functionData->endLevel)
	{
		SendNotify(1500005);
		return false;
	}

	//目标是自己
	if (target == this)
	{
		SendNotify(3049);
		return false;
	}

	UInt64 targetId = target->GetID();

	//已经是好友
	if (m_RelationMgr.FindRelation(RELATION_FRIEND, targetId) != NULL)
	{
		SendNotify(3106);
		return false;
	}

	// 在黑名单里
	if (m_RelationMgr.FindRelation(RELATION_BLACKLIST, targetId) != NULL
		|| target->m_RelationMgr.FindRelation(RELATION_BLACKLIST, GetID()) != NULL)
	{
		SendNotify(3107);
		return false;
	}

	//是师徒
	if (m_RelationMgr.FindRelation(RELATION_MASTER, targetId) || m_RelationMgr.FindRelation(RELATION_DISCIPLE, targetId))
	{
		SendNotify(3110);
		return false;
	}

	//自己好友数满
	if (m_RelationMgr.IsFull(RELATION_FRIEND))
	{
		SendNotify(3108);
		return false;
	}

	//自己有关系更新关系
	WSRelation* myOldRelation = m_RelationMgr.GetRelation(target->GetID());
	if (myOldRelation)
	{
		//通知网关移除黑名单
		if (myOldRelation->GetType() == RELATION_BLACKLIST)
		{
			GetRelationMgr().SendNotifyDelBlackList(GetID(), target->GetID());
		}
		//更新关系
		UpdateRelationDebugLog debugLog("WSPlayer::MakeFriendOfflineCb");
		m_RelationMgr.UpdateRelation(target->GetID(), myOldRelation, RELATION_FRIEND);
	}
	else
	{
		//没关系创建
		myOldRelation = new WSRelation(RELATION_FRIEND, target);
		m_RelationMgr.AddRelation(myOldRelation);
	}

	GetRelationMgr().OnAddFriend(myOldRelation);
	return true;
}

void WSPlayer::OnRemovedFried(ObjID_t targetId)
{
	WSRelation* relation = GetRelationMgr().FindRelation(RELATION_FRIEND, targetId);
	if (relation)
	{
		relation->ClearMutualFriendMark();
		relation->SyncToDB();
	}
}

void WSPlayer::RemoveFriend(ObjID_t targetId)
{
	WSRelation* relation = GetRelationMgr().FindRelation(RELATION_FRIEND, targetId);
	if (relation && relation->IsMutualFriend())
	{
		relation->ClearMutualFriendMark();
		//更新对方
		WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(targetId);
		if (target)
		{
			WSRelation* targetRelation = target->GetRelationMgr().FindRelation(RELATION_FRIEND, this->GetID());
			if (targetRelation)
			{
				targetRelation->ClearMutualFriendMark();
			}
		}
		else
		{
			WSPlayer* player = new WSPlayer();
			player->SetOfflineReplyCtx(this, LT_ONREMOVE_FRIEND);
			player->SetID(targetId);
			WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
			player->LoadData();
		}
	}

	if (GetRelationMgr().ChangeToStranger(RELATION_FRIEND, targetId))
	{
		//更新自己
		UpdateRelationDebugLog debugLog("WSPlayer::RemoveFriend");
		GetRelationMgr().UpdateRelation();
	}

	//更新对方
	/*WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(targetId);
	if (target != NULL)
	{
		target->GetRelationMgr().ChangeToStranger(RELATION_FRIEND, GetID());
	}
	else
	{
		WSRelationMgr::SyncRelationTypeToDB(targetId, GetID(), RELATION_STRANGER);
	}*/
}

bool WSPlayer::AddToBlackList(WSPlayer* target)
{
	if (!target)
	{
		return false;
	}

	ObjID_t targetId = target->GetID();

	if (targetId == 0)
	{
		return false;
	}

	//已有黑名单
	if (this->GetRelationMgr().FindRelation(RELATION_BLACKLIST, targetId) != NULL)
	{
		SendNotify(3118);
		return false;
	}

	//黑名单满
	if (this->GetRelationMgr().IsFull(RELATION_BLACKLIST))
	{
		SendNotify(3119);
		return false;
	}

	//不能加自己到黑名单
	if (this->GetID() == targetId)
	{
		SendNotify(3049);
		return false;
	}

	if (this->GetZoneId() != target->GetZoneId())
	{
		SendNotify(9880);
		return false;
	}

	WSRelation* masterRelation = this->GetRelationMgr().FindRelation(RELATION_MASTER, targetId);
	WSRelation* discipleRelation = this->GetRelationMgr().FindRelation(RELATION_DISCIPLE, targetId);
	if (masterRelation || discipleRelation)
	{
		this->SendNotify(9850);
		return false;
	}


	//是好友关系移除好友关系
	//target->GetRelationMgr().ChangeToStranger(RELATION_FRIEND, this->GetID());

	//有关系更新关系
	WSRelation* oldRelation = this->GetRelationMgr().GetRelation(targetId);
	if (oldRelation)
	{
		UpdateRelationDebugLog debugLog("WSPlayer::AddToBlackList");
		this->GetRelationMgr().UpdateRelation(targetId, oldRelation, RELATION_BLACKLIST);

		//通知网关黑名单
		if (oldRelation->GetType() == RELATION_BLACKLIST)
		{
			CLProtocol::GateNotifyNewBlackList notify;
			notify.id = this->GetID();
			notify.target = targetId;
			this->SendToGate(notify);

		}
	}
	else
	{
		//没有关系创建
		WSRelation* blackRelation = new WSRelation(RELATION_BLACKLIST, target);
		if (!this->GetRelationMgr().AddRelation(blackRelation))
		{
			CL_SAFE_DELETE(blackRelation);
			return false;
		}
	}
	GameInfoLogStream("Player") << "player(" << this->GetAccId() << "," << this->GetID() << "," << this->GetName() << ") add blacklist "
		<< targetId << "." << LogStream::eos;
	return true;
}

bool WSPlayer::CanReqMaster(ObjID_t target, bool sendNotice)
{
	WSPlayer* tar = WSPlayerMgr::Instance()->FindPlayer(target);
	if (!tar)
	{
		if (sendNotice) SendNotify(9922);
		return false;
	}

	//已经有师傅了
	if (this->GetRelationMgr().HasRelation((UInt8)RELATION_MASTER))
	{
		if (sendNotice) SendNotify(9890);
		return false;
	}

	// 自己不在拜师等级
	if (GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_APPRENT_LEVEL_MIN)
		|| GetLevel() > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_APPRENT_LEVEL_MAX))
	{
		if (sendNotice) SendNotify(9900);
		return false;
	}

	// 对方没到收徒等级
	if (tar->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_TEACHER_MIN_LEVEL))
	{
		if (sendNotice) SendNotify(9901);
		return false;
	}

	//检查对方是否是徒弟
	if (tar->GetRelationMgr().HasRelation((UInt8)RelationType::RELATION_MASTER))
	{
		if (sendNotice) SendNotify(9923);
		return false;
	}

	//徒弟在拜师惩罚时间内
	/*if (this->GetMasterSystem().GetApprenticMasterPunishTime() > Avalon::Time::CurrentTime().Sec())
	{
		if (sendNotice) SendNotify(9892);
		return false;
	}*/

	// 不能拜师自己
	if (GetID() == target)
	{
		if (sendNotice) SendNotify(8912);
		return false;
	}

	// 关系不能覆盖
	WSRelation* oldRelation = GetRelationMgr().GetRelation(target);
	if (oldRelation)
	{
		if (!GetRelationMgr().CheckRelationTypePriority(RelationType::RELATION_MASTER, (RelationType)oldRelation->GetType()))
		{
			return false;
		}
	}

	// 该玩家已经是你的师傅
	if (GetRelationMgr().FindRelation(RelationType::RELATION_MASTER, target) != NULL)
	{
		return false;
	}
	
	// 该玩家的徒弟已满了
	if (tar->GetRelationMgr().IsFull(RelationType::RELATION_DISCIPLE))
	{
		if (sendNotice) SendNotify(8902);
		return false;
	}
		
	// 该玩家等级不大于自己
	if (GetLevel() >= tar->GetLevel())
	{
		if (sendNotice) SendNotify(8901);
		return false;
	}

	// 该玩家不愿收徒
	/*if (!tar->GetRelationMgr().GetIsRecvDisciple())
	{
		if (sendNotice) SendNotify(8923);
		return false;
	}*/

	return true;
}

void WSPlayer::ReqMaster(ObjID_t targetId, std::string tarName)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		this->SendNotify(9927);
		return;
	}

	if (!CanReqMaster(targetId, true))
	{
		return;
	}

	if (!this->GetMasterSystem().CheckApprenticMasterPunishTime())
	{
		this->SendNotify(9892);
		return;
	}

	WSPlayer* target = NULL;
	if (targetId != 0)
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	}
	else
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayerByName(tarName);
	}

	// 发送离线请求
	if (target == NULL)
	{
		if (targetId > 0)
		{
			PutOfflineEvent(targetId, new RequestMasterEvent(GetID(), GetName(), GetOccu(), GetLevel(), GetVipLvl()));
			m_Requests.AddRequest(REQUEST_MASTER, targetId);
			SendNotify(3107);
		}

		return;
	}

	m_Requests.AddRequest(REQUEST_MASTER, targetId, 60);

	//向对方同步请求
	target->SyncRequest(REQUEST_MASTER, this);

	SendNotify(3107);

	GetRelationMgr().OnRequestPlayer(RelationType::RELATION_MASTER, target->GetID());
}

void WSPlayer::ReplyMaster(ObjID_t requesterId)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		this->SendNotify(9927);
		return;
	}
	//自己徒弟数满
	if (m_RelationMgr.IsFull(RELATION_DISCIPLE))
	{
		SendNotify(8905);
		return;
	}

	if (!this->GetMasterSystem().CheckRecruitDisciplePunishTime())
	{
		this->SendNotify(9893);
		return;
	}

	WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(requesterId);

	// 请求者离线
	if (disciple == NULL)
	{
		WSPlayer* player = new WSPlayer();
		player->SetOfflineReplyCtx(this, LoadType::LT_MAKE_MASTER);
		player->SetID(requesterId);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
		player->LoadData();

		return;
	}

	MakeMasterDisciple(this, disciple, MakeMasterDiscipleType::MMDT_MASTER_REPLY);
}

bool WSPlayer::CanReqDisciple(ObjID_t target, bool sendNotice)
{
	WSPlayer* tar = WSPlayerMgr::Instance()->FindPlayer(target);
	if (!tar)
	{
		if (sendNotice) SendNotify(9922);
		return false;
	}

	// 自己没到收徒等级
	if (GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_TEACHER_MIN_LEVEL))
	{
		if (sendNotice) SendNotify(9898);
		return false;
	}

	// 对方不在拜师等级
	if (tar->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_APPRENT_LEVEL_MIN)
		|| tar->GetLevel() > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_APPRENT_LEVEL_MAX))
	{
		if (sendNotice) SendNotify(9899);
		return false;
	}

	// 对方是否已经有师傅了
	if (tar->GetRelationMgr().HasRelation((UInt8)RelationType::RELATION_MASTER))
	{
		if (sendNotice) SendNotify(9924);
		return false;
	}

	//徒弟已经满了
	if (this->GetRelationMgr().IsFull(RELATION_DISCIPLE))
	{
		if (sendNotice)	SendNotify(9891);
		return false;
	}

	//师傅在收徒惩罚时间内 
	/*if (this->GetMasterSystem().GetRecruitDisciplePunishTime() > Avalon::Time::CurrentTime().Sec())
	{
		if (sendNotice) SendNotify(9893);
		return false;
	}*/

	// 不能收徒自己
	if (GetID() == target)
	{
		if (sendNotice)	SendNotify(8911);
		return false;
	}

	// 关系不能覆盖
	WSRelation* oldRelation = GetRelationMgr().GetRelation(target);
	if (oldRelation)
	{
		if (!GetRelationMgr().CheckRelationTypePriority(RelationType::RELATION_DISCIPLE, (RelationType)oldRelation->GetType()))
		{
			return false;
		}
	}

	// 该玩家已经是你的徒弟
	if (GetRelationMgr().FindRelation(RelationType::RELATION_DISCIPLE, target) != NULL)
	{
		return false;
	}
		
	// 该玩家的师傅满了
	if (tar->GetRelationMgr().IsFull(RelationType::RELATION_MASTER))
	{
		if (sendNotice)	SendNotify(8904);
		return false;
	}

	// 该玩家等级不小于自己
	if (GetLevel() <= tar->GetLevel())
	{
		if (sendNotice)	SendNotify(8906);
		return false;
	}
	
	return true;
}

void WSPlayer::ReqDisciple(ObjID_t targetId, std::string tarName)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		this->SendNotify(9927);
		return;
	}
	if (!CanReqDisciple(targetId, true))
	{
		return;
	}

	if (!this->GetMasterSystem().CheckRecruitDisciplePunishTime())
	{
		this->SendNotify(9893);
		return;
	}

	WSPlayer* target = NULL;
	if (targetId != 0)
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	}
	else
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayerByName(tarName);
	}

	// 发送离线请求
	if (target == NULL)
	{
		if (targetId > 0)
		{
			PutOfflineEvent(targetId, new RequestDiscipleEvent(GetID(), GetName(), GetOccu(), GetLevel(), GetVipLvl()));
			m_Requests.AddRequest(REQUEST_DISCIPLE, targetId);
			SendNotify(3107);
		}

		return;
	}

	m_Requests.AddRequest(REQUEST_DISCIPLE, targetId, 60);

	//向对方同步请求
	target->SyncRequest(REQUEST_DISCIPLE, this);

	SendNotify(3107);

	GetRelationMgr().OnRequestPlayer(RelationType::RELATION_DISCIPLE, target->GetID());
}

void WSPlayer::ReplyDisciple(ObjID_t requesterId)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		this->SendNotify(9927);
		return;
	}
	//自己师傅数满
	if (m_RelationMgr.IsFull(RELATION_MASTER))
	{
		SendNotify(8903);
		return;
	}

	if (!this->GetMasterSystem().CheckApprenticMasterPunishTime())
	{
		this->SendNotify(9892);
		return;
	}

	WSPlayer* master = WSPlayerMgr::Instance()->FindNormalPlayer(requesterId);

	// 请求者离线
	if (master == NULL)
	{
		WSPlayer* player = new WSPlayer();
		player->SetOfflineReplyCtx(this, LoadType::LT_MAKE_DISCIPLE);
		player->SetID(requesterId);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
		player->LoadData();

		return;
	}

	MakeMasterDisciple(master, this, MakeMasterDiscipleType::MMDT_DISCIPLE_REPLY);
}

bool WSPlayer::MakeMasterDisciple(WSPlayer* master, WSPlayer* disciple, UInt8 type)
{
	if (!disciple || !master)
	{
		return false;
	}

	if (type == MakeMasterDiscipleType::MMDT_MASTER_REPLY)
	{
		if (!disciple->GetMasterSystem().CheckApprenticMasterPunishTime())
		{
			master->SendNotify(9920);
			return false;
		}
	}
	else if (type == MakeMasterDiscipleType::MMDT_DISCIPLE_REPLY)
	{
		if (!master->GetMasterSystem().CheckRecruitDisciplePunishTime())
		{
			disciple->SendNotify(9921);
			return false;
		}
	}

	if (!disciple->CanReqMaster(master->GetID(), true))
	{
		return false;
	}

	if (!master->CanReqDisciple(disciple->GetID(), true))
	{
		return false;
	}

	//更新师傅的关系
	WSRelation* myOldRelation = master->GetRelationMgr().GetRelation(disciple->GetID());
	if (myOldRelation)
	{
		//通知网关移除黑名单
		if (myOldRelation->GetType() == RELATION_BLACKLIST)
		{
			master->GetRelationMgr().SendNotifyDelBlackList(master->GetID(), disciple->GetID());
		}

		//更新关系
		UpdateRelationDebugLog debugLog("WSPlayer::MakeMasterDisciple 1");
		master->GetRelationMgr().UpdateRelation(disciple->GetID(), myOldRelation, RELATION_DISCIPLE);
	}
	else
	{
		//没关系创建
		WSRelation* newRelation = new WSRelation(RELATION_DISCIPLE, disciple);
		master->GetRelationMgr().AddRelation(newRelation);
	}

	//更新徒弟的关系
	WSRelation* tarOldRelation = disciple->GetRelationMgr().GetRelation(master->GetID());
	if (tarOldRelation)
	{
		if (tarOldRelation->GetType() == RELATION_BLACKLIST)
		{
			disciple->GetRelationMgr().SendNotifyDelBlackList(disciple->GetID(), master->GetID());
		}

		//更新关系
		UpdateRelationDebugLog debugLog("WSPlayer::MakeMasterDisciple 2");
		disciple->GetRelationMgr().UpdateRelation(master->GetID(), tarOldRelation, RELATION_MASTER);
	}
	else
	{
		//没关系创建
		WSRelation* newRelation = new WSRelation(RELATION_MASTER, master);
		disciple->GetRelationMgr().AddRelation(newRelation);
	}

	//同门处理
	WSMasterSystem::OnMakeMasterDisciple(master, disciple);

	//通知Scene拜师事件
	if (disciple->GetGameStatus() == PS_NORMAL)
	{
		disciple->SyncEventToScene(SET_RELATION_DISCIPLE_APPRENTICE, master->GetID());
	}
	else if (master->GetGameStatus() == PS_NORMAL)
	{
		CLProtocol::SceneSyncEvent protocol;
		protocol.playerId = disciple->GetID();
		protocol.type = SET_RELATION_DISCIPLE_APPRENTICE;
		protocol.param1 = master->GetID();
		master->SendToScene(protocol);
	}

	// {徒弟名}成为了你的徒弟
	master->SendNotify(8915, disciple->GetName().c_str(), SysNotifyMgr::Instance()->FindNotify(9003)->format);

	// {师傅名}成为了你的师傅
	disciple->SendNotify(8915, master->GetName().c_str(), SysNotifyMgr::Instance()->FindNotify(9002)->format);
	
	//发日志
	this->SendMakeMasterDiscipleUdpLog(disciple->GetID(), master->GetID(), disciple->GetLevel(), master->GetLevel(),
		disciple->GetVipLvl(), master->GetVipLvl(), (UInt32)Avalon::Time::CurrentTime().Sec(), type, master->GetOccu(), disciple->GetOccu());

	InfoLogStream << "make master disciple,  master : " << master->GetID() << ", disciple : " << disciple->GetID() << LogStream::eos;
	return true;
}


void WSPlayer::RemoveMaster(ObjID_t targetId)
{
	GetRelationMgr().ChangeToStranger(RelationType::RELATION_MASTER, targetId);

	//解除成功
	SendNotify(8918);

	//更新对方
	WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(targetId);
	if (target != NULL)
	{
		target->GetRelationMgr().ChangeToStranger(RelationType::RELATION_DISCIPLE, GetID());
	}
	else
	{
		WSRelationMgr::SyncRelationTypeToDB(targetId, GetID(), RELATION_STRANGER);
	}

	//更新同门关系
	WSMasterSystem::OnRemoveMaster(this, targetId);
	//清空徒弟离线缓存中师门任务数据
	WSPlayerShareDataMgr::Instance()->HandleRelieveMaster(this->GetID());
	this->SyncEventToScene(SET_RELATION_RELIEVE_MASTER, targetId);

	//给对方发邮件
	std::string reason = GetReason(SOURCE_TYPE_DEL_MASTER_DISCIPLE);
	std::string title = SysNotifyMgr::Instance()->MakeString(8920);
	std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
	std::string content = SysNotifyMgr::Instance()->MakeString(8919, GetName().c_str());
	MailBox::SendSysMail(targetId, sender, title, content, reason, std::vector<ItemReward>());

	InfoLogStream << "remove master, disciple : " << this->GetID() << ", master : " << targetId << LogStream::eos;
}

void WSPlayer::RemoveDisciple(ObjID_t targetId)
{
	WSRelation *relation = this->GetRelationMgr().FindRelation(RelationType::RELATION_DISCIPLE, targetId);
	if (relation == NULL) return;

	UInt32 protectTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_REMOVEDISP_TIME);
	if (protectTime == 0) protectTime = 12;

	if (relation->GetAddTime() + protectTime * Avalon::Time::SECS_OF_HOUR > (UInt32)CURRENT_TIME.Sec())
	{
		SendNotify(9970);
		return;
	}

	GetRelationMgr().ChangeToStranger(RelationType::RELATION_DISCIPLE, targetId);

	//解除成功
	SendNotify(8918);

	//更新对方
	WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(targetId);
	if (target != NULL)
	{
		target->GetRelationMgr().ChangeToStranger(RelationType::RELATION_MASTER, GetID());
	}
	else
	{
		WSRelationMgr::SyncRelationTypeToDB(targetId, GetID(), RELATION_STRANGER);
	}

	WSPlayer* disciple = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	if (disciple)
	{
		disciple->SyncEventToScene(SET_RELATION_RELIEVE_MASTER, this->GetID());
	}
	else
	{
		CLProtocol::SceneSyncEvent protocol;
		protocol.playerId = targetId;
		protocol.type = SET_RELATION_RELIEVE_MASTER;
		protocol.param1 = GetID();
		SendToScene(protocol);
	}

	//更新同门关系
	WSMasterSystem::OnRemoveDisciple(this, targetId);
	//清空徒弟离线缓存中师门任务数据
	WSPlayerShareDataMgr::Instance()->HandleRelieveMaster(targetId);

	//给对方发邮件
	std::string reason = GetReason(SOURCE_TYPE_DEL_MASTER_DISCIPLE);
	std::string title = SysNotifyMgr::Instance()->MakeString(8920);
	std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
	std::string content = SysNotifyMgr::Instance()->MakeString(8919, GetName().c_str());
	MailBox::SendSysMail(targetId, sender, title, content, reason, std::vector<ItemReward>());
	
	InfoLogStream << "remove disciple, master : " << this->GetID() << ", disciple : " << targetId << LogStream::eos;
}

void WSPlayer::AddIntimacy(ObjID_t friendId, UInt16 value)
{
	if(value == 0) return;
		
	WSRelation* relation = GetRelationMgr().GetRelation(friendId);
	if(relation == NULL) return;

	if (relation->GetType() != RelationType::RELATION_FRIEND
		&& relation->GetType() != RelationType::RELATION_MASTER
		&& relation->GetType() != RelationType::RELATION_DISCIPLE)
	{
		return;
	}

	if(relation->GetIntimacy() < MAX_INTIMACY)
	{
		UInt32 oldIntimacy = relation->GetIntimacy();
		UInt32 intimacy = oldIntimacy + value;
		if(intimacy > MAX_INTIMACY) intimacy = MAX_INTIMACY;

		relation->SetIntimacy(intimacy);

		//同步到场景
		CLProtocol::SceneRelationSyncIntimacy sync;
		sync.id = GetID();
		sync.friendId = friendId;
		sync.intimacy = intimacy;
		sync.incIntimacy = intimacy - oldIntimacy;
		sync.isMaster = relation->GetType() == RelationType::RELATION_MASTER;
		if (this->GetGameStatus() == PS_NORMAL)
		{
			SendToScene(sync);
		}
		else
		{
			Router::BroadcastToScene(sync);
		}

		if (!WSApplication::Instance()->IsOptimUpdateRelation())
		{
			UpdateRelationDebugLog debugLog("WSPlayer::AddIntimacy");
			m_RelationMgr.UpdateRelation();
		}
	}
}

UInt16 WSPlayer::DecIntimacy(ObjID_t friendId, UInt16 value)
{
	if(value == 0) return 0;

	WSRelation* relation = GetRelationMgr().FindRelation(RELATION_FRIEND, friendId);
	if(relation == NULL || relation->GetIntimacy() == 0) return 0;

	if(relation->GetIntimacy() >= value)
	{
		relation->SetIntimacy(relation->GetIntimacy() - value);
	}
	else
	{
		value = relation->GetIntimacy();
		relation->SetIntimacy(0);
	}

	//同步到场景
	CLProtocol::SceneRelationSyncIntimacy sync;
	sync.id = GetID();
	sync.friendId = friendId;
	sync.intimacy = relation->GetIntimacy();
	sync.incIntimacy = 0;
	SendToScene(sync);
	if (!WSApplication::Instance()->IsOptimUpdateRelation())
	{
		UpdateRelationDebugLog debugLog("WSPlayer::DecIntimacy");
		m_RelationMgr.UpdateRelation();
	}
	
	return value;
}

UInt16 WSPlayer::GetIntimacy(ObjID_t friendId)
{
	WSRelation* relation = GetRelationMgr().FindRelation(RELATION_FRIEND, friendId);
	if(relation == NULL) return 0;

	return relation->GetIntimacy();
}

UInt8 WSPlayer::IncLoadDataCount()
{
	++m_LoadDataCount;
	DebugLogStream << "player(" << GetID() << ") start to load(" << m_LoadDataCount << ")." << LogStream::eos;
	m_LoadDataSerial |= 1 << m_LoadDataCount;
	return m_LoadDataCount;
}

bool WSPlayer::CheckLoadDataSerial(UInt8 serial)
{
	if ((m_LoadDataSerial & (1 << serial)) != 0)
	{
		return true;
	}

	return false;
}

void WSPlayer::SetLoadDataFinished(UInt8 serial)
{
	if (CheckLoadDataSerial(serial))
	{
		m_LoadDataSerial = m_LoadDataSerial & (~(1 << serial));
		if (m_LoadDataSerial == 0)
		{
			OnLoadDataFinished();
		}
	}
}

void WSPlayer::AddMoneyItem(const char* reason, UInt8 type, UInt32 num)
{
	CLProtocol::SceneAddMoneyItemReq req;
	req.playerId = GetID();
	req.reason = reason;
	req.type = type;
	req.num = num;
	SendToScene(req);
}

void WSPlayer::AddItem(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify)
{
	CLProtocol::SceneAddRewardReq addItem;
	addItem.roleId = GetID();
	addItem.reason = reason;
	addItem.rewards = itemVec;
	addItem.notice = isNotify ? 1 : 0;
	SendToScene(addItem);
}

void WSPlayer::NotifyClientAddItem(const std::string& reason, const ItemRewardVec& rewards, UInt8 notifyType)
{
	CLProtocol::SceneNotifyGetItem notify;
	notify.items = rewards;
	notify.sourceType = 0;
	notify.notifyType = notifyType;
	ReasonInfo ri;
	if (ParseReason(reason.c_str(), ri))
	{
		notify.sourceType = ri.type;
	}
	SendProtocol(notify);
}

void WSPlayer::SetAvatar(const PlayerAvatar& avatar)
{
	m_Avatar = avatar;

	auto guildMember = GuildMgr::Instance()->FindGuildMember(GetID());
	if (guildMember)
	{
		guildMember->SetAvatar(m_Avatar);
	}

	if (GetRoomId() != 0)
	{
		RoomMgr::Instance()->UpdateAvatar(this);
	}

}

PlayerIcon WSPlayer::GetIcon() const
{
	PlayerIcon icon;
	icon.id = GetID();
	icon.name = GetName();
	icon.occu = GetOccu();
	icon.level = GetLevel();
	GetPlayerLabelInfo(icon.playerLabelInfo);

	return icon;
}

void WSPlayer::GetPlayerLabelInfo(PlayerLabelInfo& info) const
{
	info.awakenStatus = GetAwaken();
	info.returnStatus = IsVeteranReturn() ? 1 : 0;
	info.headFrame = GetHeadFrame();
	info.guildId = GetGuildID();
	info.returnYearTitle = GetReturnYearTitle();
}

void WSPlayer::NotifySceneQueryWaitHandleAbnormalTrans()
{
	CLProtocol::WorldNotifyQueryWaitHandleAbnormalTrans protocol;
	protocol.roleid = GetID();
	SendToScene(protocol);
}

void WSPlayer::TryCreateAdventureTeam()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM))
	{
		return;
	}

	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}

	if (HasAdventureTeam() && GetAccountInfo()->GetAdventureTeamLevel() == 0)
	{
		GetAccountInfo()->SetAdventureTeamLevel(1);
		InfoLogStream << PLAYERINFO(this) << " has advent team, level is 0, set to 1" << LogStream::eos;
	}
	

	auto funcData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_ADVENTURE_TEAM);
	if (GetAdventureTeamName().empty() && funcData && GetLevel() >= funcData->beginLevel)
	{
		GetAccountInfo()->SetAdventureTeamName(GetName());
		GetAccountInfo()->SetAdventureTeamLevel(1);
		SyncAdventureTeamInfo(true);

		CLProtocol::WorldSyncFuncUnlock protocol;
		protocol.funcId = FUNC_ADVENTURE_TEAM;
		SendProtocol(protocol);

		std::string reason = GetReason(SOURCE_TYPE_ADVENTURE_TEAM);
		ItemRewardVec rewards;
		ItemReward reward;
		reward.id = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MERCENARY_FOUND_GIVE_RENAME_CARD);
		reward.num = 1;
		if (reward.id > 0)
		{
			rewards.push_back(reward);
			std::string sender = SysNotifyMgr::Instance()->MakeString(9400);
			std::string title = SysNotifyMgr::Instance()->MakeString(10000);
			std::string content = SysNotifyMgr::Instance()->MakeString(10001);
			MailBox::SendSysMail(GetID(), sender, title, content, reason, rewards);
		}
		else
		{
			ErrorLogStream << PLAYERINFO(this) << " adventure team rename card id is zero!" << LogStream::eos;
		}

		//添加佣兵团头衔
		this->GetAccountInfo()->GetTitleMgr().OnCreateYbt();
		GetAccountInfo()->OnAdventureTeamLevelChanged();
		//接取周常任务
		//this->GetAccountInfo()->GetWeeklyTaskMgr().RefreshTasks();
		InfoLogStream << PLAYERINFO(this) << " create adventure team(" << GetAdventureTeamName() << "), player level(" << GetLevel() << "), limi level(" << funcData->beginLevel << ")." << LogStream::eos;
	}
	else
	{
		SyncAdventureTeamInfo();
	}
}

bool WSPlayer::HasAdventureTeam()
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->HasAdventureTeam();
}

const std::string WSPlayer::GetAdventureTeamName() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return "";
	}
	return GetAccountInfo()->GetAdventureTeamName();
}

bool WSPlayer::RenameAdventureTeam(const std::string& newName)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null while rename(" << newName << ")!" << LogStream::eos;
		return false;
	}

	if (GetAdventureTeamName() == newName)
	{
		ErrorLogStream << PLAYERINFO(this) << " adventure team name is same(" << newName << ")!" << LogStream::eos;
		return false;
	}

	SendAdventureTeamNameChangeUdpLog(GetAdventureTeamName(), newName);

	GetAccountInfo()->SetAdventureTeamName(newName);

	// 名字改变

	WSSortListMgr::Instance()->OnAdventureTeamNameChanged(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccId(), GetAdventureTeamName());

	SyncAdventureTeamInfo();

	this->GetAccountInfo()->GetTitleMgr().OnYbtNameChange();
	return true;
}

UInt16 WSPlayer::GetAdventureTeamLevel() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetAdventureTeamLevel();
}

UInt64 WSPlayer::GetAdventureTeamExp() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetAdventureTeamExp();
}

const std::string& WSPlayer::GetAdventureTeamGrade() const
{
	if (GetAccountInfo() == NULL)
	{
		static std::string s_EmptyStr = "";
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return s_EmptyStr;
	}
	return GetAccountInfo()->GetAdventureTeamGradeStr();
}

UInt8 WSPlayer::GetAdventureTeamGradeNum() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetAdventureTeamGrade();
}

UInt32 WSPlayer::GetAdventureRoleScore() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetTotalScoreOfAllRoleValue();
}

void WSPlayer::SyncAdventureTeamInfo(bool create)
{
	bool bHas = HasAdventureTeam();

	{
		CLProtocol::AdventureTeamInfoSync protocol;
		if (bHas)
		{
			protocol.info.adventureTeamLevel = GetAdventureTeamLevel();
			protocol.info.adventureTeamName = GetAdventureTeamName();
			protocol.info.adventureTeamGrade = GetAdventureTeamGrade();
			protocol.info.adventureTeamRanking = WSSortListMgr::Instance()->GetRanking(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccId());
			protocol.info.roleTotalScore = GetAdventureRoleScore();
		}
		SendProtocol(protocol);
	}

	{
		CLProtocol::WorldAdventureTeamInfoSync protocol;
		if (bHas)
		{
			protocol.roleId = GetID();
			protocol.info.adventureTeamName = GetAdventureTeamName();
			protocol.info.adventureTeamLevel = GetAdventureTeamLevel();
			protocol.isCreate = create ? 1 : 0;
		}
		SendToScene(protocol);
	}
}

void WSPlayer::TryAddUnlockedNewOccu()
{
	if (!HasAdventureTeam())
	{
		return;
	}

	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}

	UInt16 maxLevel = PlayerDataEntryMgr::Instance()->GetMaxLevel();
	if (GetLevel() == maxLevel)
	{
		if (GetAccountInfo()->AddUnlockedNewOccu(GetOccu()))
		{
			SyncUnlockedNewOccus();
		}
	}
}

void WSPlayer::SyncUnlockedNewOccus(bool isOnOnline)
{
	if (!HasAdventureTeam())
	{
		return;
	}

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_player_info");
	cmd->PutCond("accid", GetAccId());
	cmd->PutColumn("level");
	cmd->PutColumn("occu");
	std::ostringstream limitoss;
	limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DELETE_PLAYER_SAVE_TIME << ")";
	cmd->SetLimit(limitoss.str());
	CLRecordClient::Instance()->SendCommand(cmd, new QueryOwnNewOccupationsCallback(GetID(), isOnOnline));
}

void WSPlayer::GetAdventureTeamDungeonAddition(DungeonAddition& addition, const DungeonDataEntry* dungeonData)
{
	if (!dungeonData) return;

	auto adventureTeamData = AdventureTeamDataEntryMgr::Instance()->FindEntry(GetAdventureTeamLevel());
	if (!adventureTeamData)
	{
		ErrorLogStream << PLAYERINFO(this) << " can't find adventure team data(" << GetAdventureTeamLevel() << ")!" << LogStream::eos;
		return;
	}

	addition.additionRate[DART_ADVENTURE_TEAM_EXP_RATE] += adventureTeamData->clearDungeonExpAddition;
}

UInt64 WSPlayer::GetBlessCrystalExp() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_BLESS_CRYSTAL_EXP);
}

UInt32 WSPlayer::GetBlessCrystalNum() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return (UInt32)GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_BLESS_CRYSTAL);
}

UInt64 WSPlayer::GetInheritBlessExp() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_INHERIT_BLESS_EXP);
}

UInt32 WSPlayer::GetInheritBlessNum() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return (UInt32)GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_INHERIT_BLESS);
}

UInt32 WSPlayer::UseInheritBless(UInt32 num)
{
	if (num == 0)
	{
		return ErrorCode::INHERIT_BLESS_USE_ERROR;
	}

	UInt32 playerLevel = (UInt32)GetLevel();
	UInt32 lowLevelLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_USE_INHERIT_BLESS_MIN_LV);
	UInt32 upperLevelLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_USE_INHERIT_BLESS_MAX_LV);

	if (playerLevel < lowLevelLimit || playerLevel > upperLevelLimit)
	{
		return ErrorCode::INHERIT_BLESS_USE_ERROR;
	}

	std::string reason = GetReason(SOURCE_TYPE_USE_INHERIT_BLESS, num, 1);
	if (RemoveAccountMoney(reason, ST_INHERIT_BLESS, num))
	{
		UInt32 incPlayerExp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INHERIT_BLESS_USE_REWARD_PLAYER_EXP);
		IncExpActive(reason, incPlayerExp);
		InfoLogStream << PLAYERINFO(this) << " remove inherit bless(" << num << "), increase player exp(" << incPlayerExp << ")." << LogStream::eos;
		return ErrorCode::SUCCESS;
	}

	UInt64 useUnitExp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INHERIT_BLESS_UNIT_EXP_FOR_USE);
	UInt64 oldExp = GetInheritBlessExp();
	if (oldExp < useUnitExp)
	{
		return ErrorCode::INHERIT_BLESS_USE_ERROR;
	}

	std::string reason2 = GetReason(SOURCE_TYPE_USE_INHERIT_BLESS, useUnitExp, 2);
	if (RemoveAccountCounterNum(reason2, ACC_COUNTER_INHERIT_BLESS_EXP, useUnitExp))
	{
		UInt32 incPlayerExp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INHERIT_BLESS_USE_UNIT_EXP_REWARD_PLAYER_EXP);
		IncExpActive(reason2, incPlayerExp);
		InfoLogStream << PLAYERINFO(this) << " remove inherit bless exp(" << useUnitExp << "), increase player exp(" << incPlayerExp << ")." << LogStream::eos;
		return ErrorCode::SUCCESS;
	}

	return ErrorCode::INHERIT_BLESS_USE_ERROR;
}

UInt32 WSPlayer::GetBounties() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return (UInt32)GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_BOUNTY);
}

void WSPlayer::SyncAdventureTeamMoney()
{
	std::vector<UInt32> counterTypes = { (UInt32)ACC_COUNTER_BLESS_CRYSTAL, (UInt32)ACC_COUNTER_INHERIT_BLESS, (UInt32)ACC_COUNTER_BOUNTY };
	SyncCustomerAccountCounterToClient(counterTypes);
}

void WSPlayer::OnRoleValueScoreSync(UInt32 roleValueScore)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}

	if (!HasAdventureTeam())
	{
		return;
	}

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
	{
		return;
	}

	if (!GetAccountInfo()->HasChangedOfRoleValueScore(GetID(), roleValueScore))
	{
		return;
	}

	GetAccountInfo()->OnRoleValueScoreChange(this, "", roleValueScore);
}

UInt32 WSPlayer::GetTotalScoreOfAllRoleValue() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetTotalScoreOfAllRoleValue();
}

void WSPlayer::SetUnlockedExtensibleRoleField(UInt32 num)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}
	GetAccountInfo()->SetUnlockedExtensibleRoleField(num);
}

UInt32 WSPlayer::GetUnlockedExtensibleRoleField()
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetUnlockedExtensibleRoleField();
}

bool WSPlayer::IncUnlockedExtensibleRoleField(UInt32 incNum)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->UnlockExtensibleRoleField(incNum);
}

void WSPlayer::UnlockExtensibleRoleField(ObjID_t costItemUId, UInt32 costItemDataId)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ROLE_FILED))
	{
		InfoLogStream << PLAYERINFO(this) << " role filed close!" << LogStream::eos;
		SendUnlockExtensibleRoleFieldRespose(ErrorCode::ENTERGAME_EXTENSIBLE_ROLE_FIELD_UNLOCK_FAILED);
		return;
	}

	// 玩家解锁的可扩展栏位
	UInt32 unlockedExtensibleField = GetUnlockedExtensibleRoleField();
	// 系统开放的可扩展栏位
	UInt32 systemOpenedExtensibleField = WSPlayerMgr::Instance()->GetSystemOpenedExtensibleRoleNum();
	if (unlockedExtensibleField >= systemOpenedExtensibleField)
	{
		InfoLogStream << PLAYERINFO(this) << " extensible field(" << unlockedExtensibleField << ") reached limit(" << systemOpenedExtensibleField << ")." << LogStream::eos;
		SendUnlockExtensibleRoleFieldRespose(ErrorCode::ENTERGAME_ROLE_FIELD_REACN_UPPER_LIMIT);
		return;
	}

	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(costItemDataId);
	if (!itemData)
	{
		ErrorLogStream << PLAYERINFO(this) << " item(" << costItemUId << "," << costItemDataId << ") not exist!" << LogStream::eos;
		SendUnlockExtensibleRoleFieldRespose(ErrorCode::ITEM_DATA_INVALID);
		return;
	}
	else if (itemData->thirdType != IT_ITEM_ROLE_EXTEND)
	{
		ErrorLogStream << PLAYERINFO(this) << " item(" << costItemUId << "," << costItemDataId << ")'s third type is error!" << LogStream::eos;
		SendUnlockExtensibleRoleFieldRespose(ErrorCode::ITEM_TYPE_INVALID);
		return;
	}

	auto trans = new UnlockExtensibleRoleFieldTransaction(this);
	trans->AddDetailItem(costItemUId, costItemDataId, 1);
	if (!BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(this) << " failed begin unlock extensible role field transaction!" << LogStream::eos;
		SendUnlockExtensibleRoleFieldRespose(ErrorCode::ITEM_USE_ADVENTURE_TEAM_RENAME_FAILED);
	}
}

void WSPlayer::SendUnlockExtensibleRoleFieldRespose(UInt32 errorCode)
{
	CLProtocol::WorldExtensibleRoleFieldUnlockRes protocol;
	protocol.resCode = errorCode;
	SendProtocol(protocol);
}

UInt32 WSPlayer::GetRoleFileds()
{
	//系统开放的基础栏位
	UInt32 systemOpenedBaseRoleFields = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_BASE_FIELD);
	return systemOpenedBaseRoleFields + GetUnlockedExtensibleRoleField();
}

void WSPlayer::SetGnomeCoinNum(UInt32 num)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}
	GetAccountInfo()->SetGnomeCoinNum(num);
}

UInt32 WSPlayer::GetGnomeCoinNum() const
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return 0;
	}
	return GetAccountInfo()->GetGnomeCoinNum();
}

bool WSPlayer::CheckRmAccountMoney(ItemSubType moneyType, UInt32 cost)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->CheckRmAccountMoney(moneyType, cost);
}

bool WSPlayer::RemoveAccountMoney(const std::string& reason, ItemSubType moneyType, UInt32 num)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->RemoveAccountMoney(this, reason, moneyType, num);
}

bool WSPlayer::IncAccountMoney(const std::string& reason, ItemSubType moneyType, UInt32 incNum)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->IncAccountMoney(this, reason, moneyType, incNum);
}

bool WSPlayer::CheckRmAccountCounterNum(AccountCounterType type, UInt64 cost)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->CheckRmAccountCounterNum(type, cost);
}

bool WSPlayer::RemoveAccountCounterNum(const std::string& reason, AccountCounterType type, UInt64 num)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return false;
	}
	return GetAccountInfo()->RemoveAccountCounterNum(this, reason, type, num);
}

void WSPlayer::IncGnomeCoinNum(const std::string& reason, UInt32 incNum, bool checkCond)
{
	if (incNum == 0)
	{
		return;
	}

	OpActivityRegInfo gnomeCoinOpAct;

	WSActivityMgr::Instance()->VisitOpActivity([&gnomeCoinOpAct](const OpActivityRegInfo& opAct){
		if (opAct.tmpType != (UInt32)OAT_MALL_BUY_GOT)
		{
			return true;
		}

		if (opAct.parm != (UInt32)MALL_BUY_GOT_TYPE_GNOME_COIN)
		{
			return true;
		}

		if (opAct.state != AS_IN)
		{
			return true;
		}
		
		gnomeCoinOpAct = opAct;

		return true;
	});

	if (gnomeCoinOpAct.state != AS_IN)
	{
		ErrorLogStream << PLAYERINFO(this) << " opActivity(" << gnomeCoinOpAct.dataId << "," << gnomeCoinOpAct.tmpType << ") is not open!" << LogStream::eos;
		return;
	}

	if (checkCond && gnomeCoinOpAct.playerLevelLimit > 0 && gnomeCoinOpAct.playerLevelLimit > GetLevel())
	{
		ErrorLogStream << PLAYERINFO(this) << " level(" << GetLevel() << ") is not reach opActivity(" << gnomeCoinOpAct.dataId << ") level limit(" << gnomeCoinOpAct.playerLevelLimit << ")!" << LogStream::eos;
		return;
	}

	if (gnomeCoinOpAct.parm2.size() != 3)
	{
		ErrorLogStream << PLAYERINFO(this) << gnomeCoinOpAct.LogStr() << " param2 size is error!" << LogStream::eos;
		return;
	}

	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}

	UInt32 oldNum = GetAccountInfo()->GetGnomeCoinNum();
	UInt32 newNum = 0;
	if (oldNum + incNum > MAX_MONEY) newNum = MAX_MONEY;
	else newNum = oldNum + incNum;

	if (newNum > oldNum)
	{
		GameInfoLogStream("Property") << "player(" << GetAccId() << "," << GetID() << "," << GetName() << ") add gnome coin:"
			<< reason << "|" << newNum - oldNum << "|" << newNum << LogStream::eos;
		SendAssetUdpLog(AssetType::ASSET_GNOME_COIN, reason.c_str(), (UInt64)oldNum, (Int64)incNum, (Int64)newNum - (Int64)oldNum, (UInt64)newNum);

		GetAccountInfo()->SetGnomeCoinNum(newNum);
		GetAccountInfo()->SetGnomeCoinRefreshTime(gnomeCoinOpAct.parm2[2]);

		MallBuyGotInfoSync(MallBuyGotType::MALL_BUY_GOT_TYPE_GNOME_COIN);

		{
			// 这里只是用来通知
			ItemRewardVec rewards;
			ItemReward reward;
			reward.id = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GNOME_COIN_ITEM_DATA_ID);
			reward.num = incNum;
			rewards.push_back(reward);
			NotifyClientAddItem(reason, rewards, 0);
			this->AddMoneyItem(GetReason(LogSourceType::SOURCE_TYPE_SHOP_MALL).c_str(), ST_CHANGE_FASHION_ACTIVE_TICKET, incNum);
		}
	}

	InfoLogStream << PLAYERINFO(this) << " increase gnome coin(" << incNum << ") now own coin(" << GetAccountInfo()->GetGnomeCoinNum() << "), refresh time(" 
		<< TimeUtil::TimestampToStandardFormat(GetAccountInfo()->GetGnomeCoinRefreshTime()) << ")." << LogStream::eos;
}

void WSPlayer::MallBuyGotInfoSync(MallBuyGotType type)
{
	CLProtocol::WorldPlayerMallBuyGotInfoSync protocol;
	protocol.mallBuyGotInfo.buyGotType = type;

	switch (type)
	{
	case MALL_BUY_GOT_TYPE_GNOME_COIN:
		protocol.mallBuyGotInfo.itemDataId = MallHelper::GetMallBuyGotItemDataId(MALL_BUY_GOT_TYPE_GNOME_COIN);
		protocol.mallBuyGotInfo.buyGotNum = GetGnomeCoinNum();
		break;

	default:
		break;
	}

	SendProtocol(protocol);
}

void WSPlayer::MallBuyGotInfoSync()
{
	MallBuyGotInfoSync(MallBuyGotType::MALL_BUY_GOT_TYPE_GNOME_COIN);
}

void WSPlayer::OnOpActivityClose(const OpActivityRegInfo& opAct)
{
	switch ((OpActivityType)opAct.tmpType)
	{
	case OAT_MALL_BUY_GOT:
		switch ((MallBuyGotType)opAct.parm)
		{
		case MALL_BUY_GOT_TYPE_GNOME_COIN:
			MallBuyGotInfoSync(MallBuyGotType::MALL_BUY_GOT_TYPE_GNOME_COIN);
			break;

		default:
			break;
		}

	default:
		break;
	}
}

void WSPlayer::OnOnline(bool isReconnect)
{
	if(GetGameStatus() != PS_NORMAL) return;

	InfoLogStream << "player(" << GetID() << ") ononline." << LogStream::eos;

	++m_ScenePlayerNum[GetSceneNode()];

	m_OfflineScene = 0;
	m_OfflinePos = CLPosition(0,0);

	Avalon::Time	offlineTime(m_OfflineTime);
	UInt64 offlineDay = GetDayBeginTime2(offlineTime);
	UInt64 currentDay = GetDayBeginTime2(CURRENT_TIME);
	bool dayChanged = (offlineTime != 0 && (offlineDay != currentDay));

	m_AccountInfo->OnPlayerOnline(this);

	WSTeamMgr::Instance()->OnPlayerOnline(this);
	m_MailBox.OnOnline();
	m_RelationMgr.OnOnline(dayChanged, isReconnect);
	m_MasterSys.OnOnline();
	m_OfflineEventMgr.OnOnline();

	WSSortListMgr::Instance()->OnPlayerOnline(this);
	PunishmentMgr::Instance()->OnPlayerOnline(this);
	BroadcastMailMgr::Instance()->OnPlayerOnline(this);

	WSActivityMgr::Instance()->OnPlayerOnline(this);

	if (WSApplication::Instance()->IsHireTurn())
	{
		HireMgr::Instance()->OnPlayerOnline(this);
	}

	CLProtocol::SyncPlayerLoginStatus syncLoginStatus;
	syncLoginStatus.playerLoginStatus = (UInt8)PLS_NONE;
	
	Avalon::Time	onlineTime(m_OnlineTime * Avalon::Time::MSECS_OF_SEC);
	if (!IsSameDay(CURRENT_TIME, offlineTime) && (m_OnlineTime == 0 || !IsSameDay(CURRENT_TIME, onlineTime)))
	{
		OnFirstLogin();
		syncLoginStatus.playerLoginStatus = (UInt8)PLS_FIRST_LOGIN_DAILY;
	}
	SendProtocol(syncLoginStatus);

	
	GuildMgr::Instance()->OnPlayerOnline(this, dayChanged);

	// 重新刷新一遍公会中的玩家属性
	GuildMgr::Instance()->OnPlayerPropertyChanged(this);

	RedPacketMgr::Instance()->OnPlayerOnline(this);

	CLProtocol::WorldNotifyGameStartTime	notify;
	notify.time = GAMESTART_TIME;
	notify.days = GetGameStartDays();
	SendProtocol(notify);

	if(0 != WORLD_LEVEL)
	{		
		CLProtocol::WorldNotifyWorldLevel	sync_c;
		sync_c.uWorldLevel = WORLD_LEVEL;
		SendProtocol(sync_c);
	}

	MallMgr::Instance()->SyncLimitedItem(this);
	MallMgr::Instance()->SyncMallGiftPackActivityState(this);

	FigureStatueMgr::Instance()->SyncFigureStatues(this);
	m_CondActivateMgr.OnOnline();

	zjy_auction::AuctionMgr::Instance()->OnPlayerLoadSelfAuctionList(this);

	SysRecordMgr::Instance()->LoadRecordFromDB(GetAccId(), GetID());
	m_OnlineTime = CURRENT_TIME.Sec();

	AccountActivityTaskMgr::Instance()->OnOnline(this, dayChanged);

	SetMatchStatus(FRIEND_MATCH_STATUS_IDLE, true);

	// 同步赏金联赛状态
	{
		PremiumLeagueMgr::Instance()->OnPlayerOnline(this);
	}

	// 房间同步状态
	RoomMgr::Instance()->OnPlayerOnline(this);

	// 挖宝管理
	DigMapMgr::Instance()->OnPlayerOnline(this);

	// 公会地下城
	GuildDungeonMgr::Instance()->NotifyGuildDungeonActivity(this);
	// 公会拍卖
	GuildAuctionMgr::Instance()->OnPlayerOnline(this);

	// 同步个人活动数据
	GetPlayerActivityInfo().SyncDatas();

	// 安全锁跨天登录
	m_AccountInfo->CheckSecurityLockCrossDay();

	// 商城购买获得物信息同步
	MallBuyGotInfoSync();

	{
		// 冒险队

		TryCreateAdventureTeam();
		SyncUnlockedNewOccus(true);

		m_AccountInfo->GetExpeditionMgr().OnPlayerOnline(this);

		SendAdventureTeamInfoUdpLog();
	}

	// 账号商店上线处理
	WSShopMgr::Instance()->OnPlayerOnline(this);

	// 拍卖行
	zjy_auction::AuctionMgr::Instance()->OnPlayerOnline(this);
	//黑市商人
	BlackMarketMgr::Instance()->OnPlayerOnline(this);

	// 回归
	if (IsVeteranReturn())
	{
		// 同步回归精力币
		SyncAccountCounterToClient(ACC_COUNTER_ENERGY_COIN);
		SyncAccountCounterToScene(ACC_COUNTER_ENERGY_COIN);

		// 角色首次回归登录
		if (IsReturnFirstLogin())
		{
			ReturnRset();
		}
	}

	//黑市商人
	BlackMarketMgr::Instance()->OnPlayerOnline(this);

	// 同步账号计数数据
	SyncAllAccountCounterToClient();

	// 同步账号离线时间
	if ( !isReconnect)
	{
		SyncAccountDataToScene(OFFLINE_TIME);
	}

	SyncAccountDataToScene(MAX_ACC_ROLE_LEVEL);

	GetTitleMgr().OnOnline();

	AdventurePassMgr::Instance()->OnOnline(this);

	// 同步oppo vip等级
	if (GetPf() == "oppo")
	{
		SyncEventToScene(SET_OPPO_VIP_LEVEL, GetOppoAmberLevel());
	}

	SendMoneyStockUdpLog();

	GetMonopolyInfo().OnOnline();
}

void WSPlayer::OnOffline(bool serverQuit)
{
	if (GetGameStatus() != PS_NORMAL && GetGameStatus() != PS_LEAVEGAME)
	{
		ErrorLogStream << "player(" << GetID() << ") want to offline, invalid status(" << (UInt8)GetGameStatus() << ")." << LogStream::eos;
		return;
	}

	DebugLogStream << "player(" << GetID() << ") offline." << LogStream::eos;

	if (!serverQuit)
	{
		SetMatchStatus(FRIEND_MATCH_STATUS_OFFLINE, true);
	}

	OfflineNotifyMgr::Instance()->OnPlayerOffline(this);
	m_RelationMgr.OnOffline();
	m_MasterSys.OnOffline();
	WorldBossMgr::Instance()->CancelAllTrace(this);
	
	auto team = GetTeam();
	if (!team)
	{
		TeamMatchMgr::Instance()->OnPlayerCancelMatch(this);
	}

    CLProtocol::MatchServerMatchCancelReq msReq;
    msReq.id = GetID();
    Router::SendToMatch(msReq);

	if (team != NULL)
	{
		team->OnPlayerOffline(this);
	}

	--m_ScenePlayerNum[GetSceneNode()];

	m_OfflineTime = CURRENT_TIME.MSec();

	WSPlayerMgr::Instance()->RemovePlayerByAccId(this);
	SetGameStatus(PS_FINAL);

	DungeonRaceMgr::Instance()->LeaveDungeonRace(GetID(), true);
	GuildMgr::Instance()->OnPlayerOffline(this);

	PremiumLeagueMgr::Instance()->OnPlayerOffline(this);

	RoomMgr::Instance()->OnPlayerOffline(this);

	WSPlayerShareDataMgr::Instance()->OnOffline(this);

	DigMapMgr::Instance()->OnPlayerOffline(this);

	if (WSApplication::Instance()->IsHireTurn())
	{
		HireMgr::Instance()->OnPlayerOffline(this);
	}

	m_AccountInfo->OnPlayerOffline(this);
	m_AccountInfo->SetOfflineTime(CURRENT_TIME.Sec());
}

void WSPlayer::OnDisconnect()
{
	CLProtocol::MatchServerMatchCancelReq msReq;
	msReq.id = GetID();
	Router::SendToMatch(msReq);

	CLProtocol::SceneLogicLeaveGameReq req;
	req.id = GetID();
	SendToScene(req);

	//取消吃鸡匹配
	CLProtocol::BattleEnrollReq enrollReq;
	enrollReq.isMatch = 0;
	enrollReq.accId = GetAccId();
	enrollReq.roleId = GetID();
	enrollReq.playerName = GetName();
	enrollReq.playerOccu = GetOccu();
	Router::SendToBattle(enrollReq);

	RoomMgr::Instance()->OnDisconnect(this);
}

void WSPlayer::OnReconnectSuccess()
{
	RoomMgr::Instance()->OnReconnectSuccess(this);
}

void WSPlayer::OnServerChanged(UInt32 oldNodeId)
{
	--m_ScenePlayerNum[oldNodeId];
	++m_ScenePlayerNum[GetSceneNode()];

	auto team = GetTeam();
	if (team != NULL)
	{
		team->OnSceneServerChanged(this);
	}
	m_RelationMgr.OnServerChanged();
	PunishmentMgr::Instance()->OnServerChanged(this);

	//同步社会关系
	if(oldNodeId != GetSceneNode())
	{
		SyncRelationDataToScene();
	}
}

void WSPlayer::OnLevelChanged(UInt16 oldLv)
{
	m_RelationMgr.OnLevelChanged();
	//GetMasterSystem().OnLevelChanged();
	WSSortListMgr::Instance()->OnPlayerLevelChanged(this);

	auto team = GetTeam();
	if(team != NULL)
	{
		team->OnPlayerLevelChanged(this);
	}

	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(GetID());
	if (dungeonRace && dungeonRace->GetRelayServerID() > 0)
	{
		CLProtocol::SceneServerSyncPlayerEvent sync;
		sync.raceId = dungeonRace->GetSession();
		sync.roleId = GetID();
		sync.type = SYNC_PLAYER_EVENT_LEVELUP;
		sync.param1 = GetLevel();
		Router::SendToRelay(dungeonRace->GetRelayServerID(), sync);
	}

	GuildMgr::Instance()->OnPlayerPropertyChanged(this);

	GuildDungeonMgr::Instance()->NotifyGuildDungeonActivity(this);

	GuildAuctionMgr::Instance()->OnLevelUp(this);

	WSActivityMgr::Instance()->OnPlayerLevelUp(this, oldLv);

	GetPlayerActivityInfo().OnLevelUp();
	MallMgr::Instance()->OnPlayerLevelChanged(this, oldLv);

	TryCreateAdventureTeam();
	TryAddUnlockedNewOccu();

	AdventurePassMgr::Instance()->OnLevelChg(this);

	if (WSApplication::Instance()->IsHireTurn())
	{
		HireMgr::Instance()->OnPlayerOnline(this);
	}

	if (GetAccountInfo())
	{
		GetAccountInfo()->OnPlayerLevelChanged(this, oldLv);
	}
}

void WSPlayer::OnOccuChanged()
{
	m_RelationMgr.OnOccuChanged();
	//GetMasterSystem().OnOccuChanged();
	GuildMgr::Instance()->OnPlayerPropertyChanged(this);
	WSSortListMgr::Instance()->OnOccuChanged(this);

	auto team = GetTeam();
	if (team != NULL)
	{
		team->OnPlayerOccuChanged(this);
	}

	TryAddUnlockedNewOccu();

	if (GetAccountInfo())
	{
		GetAccountInfo()->OnPlayerOccuChanged(this);
	}
}

void WSPlayer::OnVipLvlChanged()
{
	m_RelationMgr.OnVipLvlChanged();
	//GetMasterSystem().OnVipLvlChanged();
	WSSortListMgr::Instance()->OnVipLvlChanged(this);
	GuildMgr::Instance()->OnPlayerPropertyChanged(this);

	auto team = GetTeam();
	if (team != NULL)
	{
		team->OnPlayerVipLevelChanged(this);
	}

	if (GetRoomId() != 0)
	{
		RoomMgr::Instance()->UpdateVipLevel(this);
	}
}

void WSPlayer::OnSeasonLvChanged()
{
	m_RelationMgr.OnSeasonLvChanged();
	GuildMgr::Instance()->OnPlayerPropertyChanged(this);
}

void WSPlayer::OnExpIncrease(UInt64 addExp, const std::string& reason)
{
	if (addExp == 0)
	{
		return;
	}

	ReasonInfo param;
	if (!ParseReason(reason.c_str(), param))
	{
		ErrorLogStream << PLAYERINFO(this) << " parse reason(" << reason << ") failed while add exp(" << addExp << ")!" << LogStream::eos;
		return;
	}

#ifdef _DEBUG
	InfoLogStream << PLAYERINFO(this) << GetName() << " on exp increase, addExp=" << addExp << ", reason=" << reason << LogStream::eos;
#endif
	
	if (HasAdventureTeam())
	{
		// 冒险队
		auto adventureTeamData = AdventureTeamDataEntryMgr::Instance()->FindEntry(GetAdventureTeamLevel());
		if (!adventureTeamData)
		{
			ErrorLogStream << PLAYERINFO(this) << " can't find adventure team data(" << GetAdventureTeamLevel() << ")!" << LogStream::eos;
			return;
		}

		if (std::find(adventureTeamData->levelUpExpSourceVec.begin(), adventureTeamData->levelUpExpSourceVec.end(), (UInt32)param.type) == adventureTeamData->levelUpExpSourceVec.end())
		{
			return;
		}

		auto playerData = PlayerDataEntryMgr::Instance()->FindEntry((UInt32)GetLevel());
		if (!playerData)
		{
			ErrorLogStream << PLAYERINFO(this) << " can't find player data(" << GetLevel() << ")!" << LogStream::eos;
			return;
		}

		UInt64 incAdventureTeamExp = addExp * (UInt64)playerData->adventureTeamAdditionRate / 100;

		GetAccountInfo()->TryIncAdventureTeamExp(this, reason, incAdventureTeamExp);
		GetAccountInfo()->TryIncBlessExp(this, reason, incAdventureTeamExp, ACC_COUNTER_BLESS_CRYSTAL_EXP);

		UInt32 maxPlayerLevel = PlayerDataEntryMgr::Instance()->GetMaxLevel();
		if (GetLevel() == maxPlayerLevel)
		{
			GetAccountInfo()->TryIncBlessExp(this, reason, addExp, ACC_COUNTER_INHERIT_BLESS_EXP);
		}
	}
}

void WSPlayer::OnKilled(WSPlayer* attacker)
{
	m_RelationMgr.OnKilled(attacker);
}

void WSPlayer::OnHeartbeat(const Avalon::Time& now)
{
	//tick类型
	int tickType = LOGICTICK_MAX;
	for(int i = 0; i < LOGICTICK_MAX; ++i)
	{
		if(now.MSec() >= m_TickTime[i] + LOGICTICK_INTERVAL[i])
		{
			tickType = i;
			m_TickTime[i] = now.MSec();
			break;
		}
	}

	if (tickType == LOGICTICK_FIVEMIN)
	{
		m_RelationMgr.FiveMinHb();
	}

	if(tickType == LOGICTICK_SECOND)
	{
		m_MailBox.OnHeartbeat(now);
		m_RelationMgr.OnHeartbeat(now);	
		m_MasterSys.OnHeartbeat(now);

		m_Requests.OnTick(now);

		m_CondActivateMgr.OnTick(now);

		GetTitleMgr().OnTick(now);

		if(m_ItemTransTimeout > 0)
		{
			if(--m_ItemTransTimeout == 0)
			{
				m_pItemTrans->OnTimeOut(this);
				CL_SAFE_DELETE(m_pItemTrans);
			}
		}

		if(m_LeaveTimeout > 0)
		{
			--m_LeaveTimeout;
			if(m_LeaveTimeout > 25)
			{
				CLProtocol::SceneLeaveGameReq req;
				req.id = GetID();
				SendToScene(req);
			}
			else if(m_LeaveTimeout == 20)
			{
				CLProtocol::SceneLeaveGameReq req;
				req.id = GetID();
				Router::BroadcastToScene(req);
			}
			else if(m_LeaveTimeout == 0)
			{
				GameInfoLogStream("Player") << "player(" << GetAccId() << "," << GetID() << "," << GetName() << ") leave game timeout!" << LogStream::eos;
				LeaveGame();
				return;
			}
		}

		auto newStatus = CountMatchStatus();
		SetMatchStatus(newStatus);
	}

	if (tickType == LOGICTICK_FIVESEC)
	{
		// 检查强制解锁是否到期
		SecurityLockForceCheck();
	}

	if(m_pItemTrans != NULL && m_pItemTrans->m_Status == ItemTransaction::STATUS_FINAL)
	{
		CL_SAFE_DELETE(m_pItemTrans);
		m_ItemTransTimeout = 0;
	}

	if (m_bDayChanged == 0)
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() == 0 && CURRENT_DATE.Sec() < 5)
		{
			m_bDayChanged = 1;
			WSActivityMgr::Instance()->OnDayChanged(this);
		}
	}
	else
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() > 0)
		{
			m_bDayChanged = 0;
		}
	}

	if (m_bLogicDayChanged == 0)
	{
		if (CURRENT_DATE.Hour() == 6 && CURRENT_DATE.Min() == 0 && CURRENT_DATE.Sec() < 5)
		{
			m_bLogicDayChanged = 1;
			m_RelationMgr.OnLogicDayChange();
			m_AccountInfo->SetPasswdErrorNumResetTime(CURRENT_TIME.MSec());
			m_AccountInfo->ClearSecurityPasswdErrorNum();
			NotifySecurityPasswdErrorNum();
		}
	}
	else
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() > 0)
		{
			m_bLogicDayChanged = 0;
		}
	}

	m_PlayerActivityInfo.OnTick(now, tickType);
	
}

void WSPlayer::OnLoadDataFinished(bool isReconnect)
{
	// 非登陆的查询
	if (IsOfflineLoad())
	{
		InfoLogStream << "player(" << GetID() << ") offline load finish." << LogStream::eos;
		/*if (m_loadOfflineDataFail)
		{
			ClearOfflineReplyCtx();
			return;
		}*/
		OfflineLoadCb();
		ClearOfflineReplyCtx();
		return;
	}

	DebugLogStream << "player(" << GetID() << ") load data finished, isReconnect(" << (UInt8)isReconnect << ")." << LogStream::eos;

	SetGameStatus(PS_NORMAL);

	if (WSApplication::Instance()->IsAdjustOnOnline())
	{
		OnOnline(isReconnect);
#ifdef _DEBUG
		Avalon::thread_sleep(1000);
#endif
	}
	
    if (!isReconnect)
    {
        CLProtocol::WorldRegisterPlayerRet ret;
        ret.id = GetID();
        ret.result = ErrorCode::SUCCESS;

		DebugLogStream << "player(" << GetID() << ") registed!" << LogStream::eos;
        SendToScene(ret);
    }

	if (!WSApplication::Instance()->IsAdjustOnOnline())
	{
		OnOnline(isReconnect);
	}

	//最后同步社会关系数据到场景
	SyncRelationDataToScene();

	// 添加角色和账号ID的对应关系
	AccountMgr::Instance()->SetRoleID2AccID(GetID(), GetAccId());
}

void WSPlayer::OnPowerChanged()
{
	m_RelationMgr.OnPowerChanged();
}

void WSPlayer::HandlerFriendGive(WSPlayer* target, WSRelation* relation)
{
	if (!target || !relation)
	{
		ErrorLogStream << "friend give err player id : " << this->GetID() << LogStream::eos;
		return;
	}

	char szContent[256 + 1];
	memset(szContent, 0, sizeof(szContent));
	SysNotifyMgr::Instance()->MakeNotify(1076, szContent, 256, this->GetName());
	std::string content(szContent);

	std::vector<ItemReward> items;
	ItemReward item;
	item.id = FRIEND_GIVE_GIFT;
	item.num = 1;
	items.push_back(item);

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(2014, buffer, sizeof(buffer));
	std::string strTitle(buffer);
	std::string reason = GetReason(SOURCE_TYPE_FRIEND_GIVE);

	MailBox::SendSysMail(target->GetID(), this->GetName(), strTitle, content, reason, items);
	relation->SetDayGiftNum(relation->GetDayGiftNum() - 1);

	Int32 curH = CURRENT_DATE.Hour();
	Avalon::Date resetDate;
	if (curH < FRIEND_DAY_GIFT)
	{
		resetDate = CURRENT_DATE;
		resetDate.Hour(FRIEND_DAY_GIFT);
		resetDate.Min(0);
		resetDate.Sec(0);
		resetDate.MSec(0);
	}
	else
	{
		Avalon::Time resetTime(CURRENT_TIME.MSec() + DAY_TO_SEC * SEC_TO_MSEC);
		resetDate = Avalon::Date(resetTime);
		resetDate.Hour(FRIEND_DAY_GIFT);
		resetDate.Min(0);
		resetDate.Sec(0);
		resetDate.MSec(0);
	}

	relation->SetGiveResetTime(UInt32(resetDate.ToTime().Sec()));
	UpdateRelationDebugLog debugLog("HandleProtocol WorldRelationPresentGiveReq");
	this->GetRelationMgr().UpdateRelation();
	this->SendNotify(1077);
	this->OnFriendGive(target);
	relation->SyncToDB();
}

void WSPlayer::OnFriendGive(WSPlayer* target)
{
	SyncEventToScene(SyncEventType::SET_FRIEND_GIVE);
	if (target)
	{
		if (target->GetRelationMgr().FindRelation(RELATION_FRIEND, this->GetID())
			|| target->GetRelationMgr().FindRelation(RELATION_MASTER, this->GetID())
			|| target->GetRelationMgr().FindRelation(RELATION_DISCIPLE, this->GetID()))
		{
			target->AddIntimacy(this->GetID(), 2);
			this->AddIntimacy(target->GetID(), 2);
			if (!WSApplication::Instance()->IsOptimUpdateRelation())
			{
				UpdateRelationDebugLog debugLog1("WSPlayer::OnFriendGive 1");
				this->GetRelationMgr().UpdateRelation();
			}
			
			if (!WSApplication::Instance()->IsOptimUpdateRelation())
			{
				UpdateRelationDebugLog debugLog2("WSPlayer::OnFriendGive 2");
				target->GetRelationMgr().UpdateRelation();
			}
		}
	}
}

void WSPlayer::OnAddAuction(zjy_auction::AuctionInfo* auctionInfo)
{
	if (NULL == auctionInfo) return;

	if (auctionInfo->type == AT_ITEM)
	{
		SyncEventToScene(SyncEventType::SET_ADD_AUCTION);
	}

	UInt32 deposit = zjy_auction::AuctionMgr::GetDepositByAuctionPrice(auctionInfo->price);
	UInt64 itemId = 0;
	UInt32 itemDataId = 0;
	if (auctionInfo->item != NULL)
	{
		itemId = auctionInfo->item->GetID();
		itemDataId = auctionInfo->item->m_DataId;
	}
	SendAuctionUpLog((AuctionType)auctionInfo->type, itemId, itemDataId, auctionInfo->num, (MoneyType)auctionInfo->pricetype, auctionInfo->price, deposit, auctionInfo->duetime);
}

void WSPlayer::OnDownAuction(zjy_auction::AuctionSendItemType downType, AuctionType type, UInt64 itemId, UInt32 itemDataId, UInt32 num)
{
	SendAuctionDownLog(downType, type, itemId, itemDataId, num);
}

void WSPlayer::OnSellAuction(UInt32 money)
{
	SyncEventToScene(SyncEventType::SET_SELL_AUCTION, money);
}

void WSPlayer::OnBuyAuction(AuctionType type, bool abnormalDeal, ObjID_t owner, UInt32 ownerVipLev, ObjID_t srcItemId, ObjID_t itemId, zjy_auction::AuctionMainItemType mainType,
	UInt32 itemDataId, UInt32 num, MoneyType priceType, UInt32 price, UInt32 counterFee, UInt8 strengthen, UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality,
	std::string sellerIp, std::string sellerDeviceId, UInt32 ownerAccount, UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid, UInt8 isAttented, UInt32 itemAttentNum,
	UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
	UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice, UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice,
	UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax)
{

	UInt32 money = 0;
	if (priceType == MoneyType::MONEY_GOLD)
	{
		money = price;
	}

	if (owner > 0 && srcItemId > 0)
	{
		PutOfflineEvent(owner, new SellAuctionEvent(money));
	}

	SyncEventToScene(SyncEventType::SET_BUY_AUCTION, money);

	SendAuctionBuyLog(owner, ownerVipLev, type, abnormalDeal, srcItemId, itemId, mainType,
		itemDataId, num, priceType, price, counterFee, strengthen, averPrice,
		onSealTime, itemQuality, sellerIp, sellerDeviceId, ownerAccount,
		isTreas, sysRecomPrice, minPriceOnSale, transGuid, isAttented, itemAttentNum,
		mountMagicCardId, mountMagicCardAverPrice, mountBeadId,
		mountBeadAverPrice, eqQualityLv, remainSealCount,
		minPriceGuid, minPriceOnsalePrice, minPriceQualityLv, minPriceMountBeadId, 
		minPriceMountBeadAverPrice, minPriceMountMagicId, minPriceMountMagicAverPrice,
		auctionPrice, auctionTransNum, auctionTransMax);
}

void WSPlayer::OnSyncSceneEvent(SyncEventType type, ObjID_t param1, ObjID_t param2, ObjID_t param3)
{
	if (type == SET_BUY_DUNGEON_TIMES)
	{
		// 如果玩家在队伍里购买了地下城次数之后，需要重新请求一边目标地下城的剩余次数
		auto team = GetTeam();
		if (!team)
		{
			return;
		}

		if (team->IsTargetTimesLimit())
		{
			team->QueryRemainTimes(GetID(), team->GetTeamTarget());
		}
	}
	else if (type == SET_DUNGEON_USE_POTION)
	{
		auto race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(GetID());
		if (!race)
		{
			return;
		}
		auto dungeonPlayer = race->FindDungeonPlayer(GetID());
		if (!dungeonPlayer)
		{
			return;
		}

		UInt32 itemTypeId = (UInt32)param1;
		dungeonPlayer->IncBufferItemUsedNum(itemTypeId);
		SendDungeonUsePotionUdpLog(race->GetID(), race->GetDungeonID(), dungeonPlayer->GetCurAreaID(), itemTypeId, race->IsTeamMode());
	}
	else if (type == SET_ENTER_CROSS_SCENE)
	{
		m_inCrossScene = true;
	}
	else if (type == SET_EXIT_CROSS_SCENE)
	{
		m_inCrossScene = false;
	}
	else if (type == SET_REMOVE_FATIGUE)
	{
		OnRemoveFatigue((UInt32)param1);
	}
	else if (type == SET_TEAM_COPY_CLEAR_DUNGEON)
	{
		OnTeamCopyEvent(param1, param3);
	}
}

void WSPlayer::OnTeamCopyEvent(ObjID_t param1, ObjID_t param3)
{
	UInt32 dailyToDoSubType = 0;
	switch (param3)
	{
	case TC_TYPE_ONE:
	{
		if (TEAM_COPY_TEAM_GRADE_COMMON == param1)
		{
			dailyToDoSubType = DTSTP_COMMON_TEAM_COPY_ONE;
		}
		else if (TEAM_COPY_TEAM_GRADE_DIFF == param1)
		{
			dailyToDoSubType = DTSTP_DIFF_TEAM_COPY_ONE;
		}
	}
	break;
	case TC_TYPE_TWO:
	{
		if (TEAM_COPY_TEAM_GRADE_COMMON == param1)
		{
			dailyToDoSubType = DTSTP_COMMON_TEAM_COPY_TWO;
		}
		else if (TEAM_COPY_TEAM_GRADE_DIFF == param1)
		{
			dailyToDoSubType = DTSTP_DIFF_TEAM_COPY_TWO;
		}
	}
	break;
	default:return;
	}

	if (dailyToDoSubType != 0)
	{
		GetDailyTodoMgr().OnPlayerTaskUpdate(dailyToDoSubType, 1, true);
	}
}

void WSPlayer::OnGuildChanged()
{
	auto team = GetTeam();
	if (team)
	{
		team->ChangeMemberGuildID(GetID(), GetGuildID());
	}
}

void WSPlayer::OnJoinGuild()
{
	SendGuildJoinExitUdpLog(0);
	SyncEventToScene(SyncEventType::SET_JOIN_GUILD);
}

void WSPlayer::RequestGlobalServerInfo()
{
	if (GetPf() != "oppo")
	{
		return;
	}

	CLProtocol::CenterGlobalServerInfoReq protocol;
	protocol.platform = GetPf();
	protocol.openid = GetOpenId();
	protocol.roleId = GetID();
	Router::SendToCenter(protocol);
}

void WSPlayer::OnIncGuildBattleScore(UInt32 score)
{
	SyncEventToScene(SyncEventType::SET_GUILD_BATTLE_SCORE, score);
}

void WSPlayer::OnGuildOccupyTerritory(UInt8 terrId)
{
	SyncEventToScene(SyncEventType::SET_GUILD_OCCUPY_TERRITORY, terrId);
}

void WSPlayer::OnMallItemBuy(UInt32 mallItemId, UInt32 buyNum)
{
	auto mallItemInfo = MallMgr::Instance()->GetMallItemInfo(mallItemId);
	if (!mallItemInfo)
	{
		ErrorLogStream << PLAYERINFO(this) << " can't find mall item(" << mallItemId << ")!" << LogStream::eos;
		return;
	}

	std::string reason = GetReason(SOURCE_TYPE_SHOP_MALL, mallItemId, buyNum);

	for (const auto& gotInfo : mallItemInfo->buyGotInfos)
	{
		switch ((MallBuyGotType)gotInfo.buyGotType)
		{
		case MallBuyGotType::MALL_BUY_GOT_TYPE_GNOME_COIN:
		{
			UInt32 incNum = gotInfo.buyGotNum * buyNum;
			IncGnomeCoinNum(reason, incNum);
			break;
		}

		default:
			ErrorLogStream << PLAYERINFO(this) << " mall item(" << mallItemId << ")'s buy got type(" << (UInt32)gotInfo.buyGotType << ") is error!" << LogStream::eos;
			break;
		}
	}
}

void WSPlayer::OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon)
{
	m_AccountInfo->OnOwnedNpcDied(npcid, type, dungeon);
}

void WSPlayer::OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime, UInt8 isAsist)
{
	if (!dungeonData)
	{
		return;
	}

	if (dungeonData->threeType == DTT_TEAM_ELITE && isAsist)
	{
		return;
	}

	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}
	GetAccountInfo()->OnPlayerClearDungeon(dungeonData, score, usedTime);
	GetDailyTodoMgr().OnPlayerClearDungeon(dungeonData, score, usedTime);
}

void WSPlayer::OnRemoveFatigue(UInt32 removeFatigue)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}
	GetAccountInfo()->OnRemoveFatigue(removeFatigue);
}

bool WSPlayer::CanStartMatch()
{
    // ToDo...
    // 判断能否匹配

    return true;
}

void WSPlayer::OnStartMatch()
{
	// ToDo...
	// 开始匹配后需要设置一些状态

	SetMatching(true);
	return;
}

void WSPlayer::OnCancleMatch()
{
    // ToDo...
    // 取消匹配后需要设置一些状态

	SetMatching(false);
    return;
}

void WSPlayer::OnStartPvPRace()
{
	SetMatching(false);
}

RacePlayerInfo WSPlayer::GetRacePlayerInfo()
{
	RacePlayerInfo info;
    info.accid = GetAccId();
    info.roleId = GetID();
	info.name = GetName();
	info.occu = GetOccu();
	info.level = GetLevel();

	return info;
}

void WSPlayer::RequestPkChallenge(ObjID_t targetId, RequestType type)
{
	// 先判断自己能不能挑战
	UInt32 ret = CanStartPkChallenge(true, type);
	if (ret != ErrorCode::SUCCESS)
	{
		SendNotify(ret);
		return;
	}

	auto target = WSPlayerMgr::Instance()->FindPlayer(targetId);
	if (!target)
	{
		SendNotify(ErrorCode::PK_CHALLENGE_TARGET_NOT_ONLINE);
		return;
	}
	
	// 决斗场开放等级
	auto pkFunctionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_PK);
	if (pkFunctionData)
	{
		if (target->GetLevel() < pkFunctionData->endLevel)
		{
			SendNotify(ErrorCode::PK_CHALLENGE_LEVEL_LIMIT);
			return;
		}
	}

	ret = target->CanStartPkChallenge(true, type);
	if (ret != ErrorCode::SUCCESS)
	{
		SendNotify(ret);
		return;
	}

	if (m_Requests.HasRequest(type, targetId))
	{
		// 重复挑战
		SendNotify(ErrorCode::PK_CHALLENGE_REPEAT);
		return;
	}

	m_Requests.AddRequest(type, targetId);

	target->SyncRequest(type, this);
}

void WSPlayer::ReplyPkChallenge(ObjID_t requesterId, bool agree, RequestType type)
{
	auto requester = WSPlayerMgr::Instance()->FindPlayer(requesterId);
	if (!requester)
	{
		return;
	}

	if (!requester->GetRequestMgr().HasRequest(type, GetID()))
	{
		return;
	}

	requester->GetRequestMgr().RemoveRequest(type, GetID());

	UInt32 ret = requester->CanStartPkChallenge(true, type);
	if (ret != ErrorCode::SUCCESS)
	{
		return;
	}

	ret = CanStartPkChallenge(false, type);
	if (ret != ErrorCode::SUCCESS)
	{
		return;
	}
	else
	{
		if (type == REQUEST_CHALLENGE_PK)
		{
			SendNotify(ErrorCode::PK_CHALLENGE_SUCCESS);
		}
	}

	if (!agree)
	{
		return;
	}

	// 把其它请求清空
	requester->GetRequestMgr().ClearRequests(type);

	CLProtocol::WorldMatchPracticeReq req;
	req.roleIds.push_back(GetID());
	req.roleIds.push_back(requesterId);
	if (type == REQUEST_CHALLENGE_PK)
	{
		req.pkType = PK_PRACTICE;
	}
	else if (type == REQUEST_EQUAL_PK)
	{
		req.pkType = PK_EQUAL_PRACTICE;
	}
	

	Router::SendToMatch(req);
}

UInt32 WSPlayer::CanStartPkChallenge(bool isChallenger, RequestType type)
{
	if (GetTeam())
	{
		return ErrorCode::PK_CHALLENGE_IN_TEAM;
	}

	if (GetRoomId() != 0)
	{
		return ErrorCode::PK_CHALLENGE_IN_ROOM;
	}

	if (isChallenger)
	{
		// 如果是挑战方，必须在战斗准备区
		auto sceneData = SceneDataEntryMgr::Instance()->FindEntry(GetSceneID());
		if (!sceneData || sceneData->type != SCENE_TYPE_PK_PREPARE)
		{
			return ErrorCode::PK_CHALLENGE_NOT_IN_PK_PREPARE;
		}
		if (!sceneData || (type == REQUEST_EQUAL_PK && sceneData->subType != SCENE_EQUAL_PVP))
		{
			return ErrorCode::PK_CHALLENGE_NOT_IN_PK_PREPARE;
		}
	}
	else
	{
		// 更新下战斗状态
		UpdateMatchStatus();

		// 挑战状态
		if (GetMatchStatus() != FRIEND_MATCH_STATUS_IDLE)
		{
			return ErrorCode::PK_CHALLENGE_TARGET_BUSY;
		}
	}
	
	return ErrorCode::SUCCESS;
}

FriendMatchStatus WSPlayer::GetMatchStatus()
{
	return m_MatchStatus;
}

void WSPlayer::SetMatchStatus(FriendMatchStatus status, bool forceSync)
{
	if (m_MatchStatus == status && !forceSync)
	{
		return;
	}

	m_MatchStatus = status;
	GetRelationMgr().OnStatusChanged(status);
	//GetMasterSystem().OnStatusChanged(status);
}

FriendMatchStatus WSPlayer::CountMatchStatus()
{
	if (GetGameStatus() != PS_NORMAL)
	{
		return FRIEND_MATCH_STATUS_OFFLINE;
	}

	// 只有未组队，并且在城镇中才是空闲状态
	if (GetTeam())
	{
		return FRIEND_MATCH_STATUS_BUSY;
	}

	auto sceneData = SceneDataEntryMgr::Instance()->FindEntry(GetSceneID());
	if (!sceneData)
	{
		return FRIEND_MATCH_STATUS_BUSY;
	}

	if (sceneData->type == SCENE_TYPE_NORMAL || sceneData->type == SCENE_TYPE_SINGLE ||
		(sceneData->type == SCENE_TYPE_PK_PREPARE && !IsMatching()))
	{
		return FRIEND_MATCH_STATUS_IDLE;
	}

	return FRIEND_MATCH_STATUS_BUSY;
}

void WSPlayer::UpdateMatchStatus()
{
	SetMatchStatus(CountMatchStatus());
}

UInt8 WSPlayer::GetAuctionNum(AuctionType type)
{
    if (type >= AT_NUM)
    {
        return 0;
    }

    return (UInt8)m_auctoinList[type].size();
}

void WSPlayer::ClearAuction()
{
	for (int i = 0; i < AT_NUM; i++)
	{
		m_auctoinList[i].clear();
	}
}

void WSPlayer::AddAuction(AuctionType type, ObjID_t id)
{
    if (type >= AT_NUM)
    {
        ErrorLogStream << "player(" << GetID() << ") add invalid auction type(" << (UInt8)type << ") id(" << id << ")." << LogStream::eos;
        return;
    }

    if (std::find(m_auctoinList[type].begin(), m_auctoinList[type].end(), id) == m_auctoinList[type].end())
    {
        m_auctoinList[type].push_back(id);
    }
}

void WSPlayer::DelAuction(AuctionType type, ObjID_t id)
{
    if (type >= AT_NUM)
    {
        ErrorLogStream << "player(" << GetID() << ") del invalid auction type(" << (UInt8)type << ") id(" << id << ")." << LogStream::eos;
        return;
    }

    auto itr = std::find(m_auctoinList[type].begin(), m_auctoinList[type].end(), id);
    if (itr != m_auctoinList[type].end())
    {
        m_auctoinList[type].erase(itr);
    }
    else
    {
        ErrorLogStream << "player(" << GetID() << ") del unexist auction type(" << (UInt8)type << ") id(" << id << ")." << LogStream::eos;
    }
}

std::vector<ObjID_t> WSPlayer::GetAuctionList(AuctionType type)
{
    if (type >= AT_NUM)
    {
        ErrorLogStream << "player(" << GetID() << ") get invalid auction list type(" << (UInt8)type << ")." << LogStream::eos;
        return std::vector<ObjID_t>();
    }

    return m_auctoinList[type];
}

UInt16 WSPlayer::GetRoomLimitLevel()
{
	Room* room = RoomMgr::Instance()->GetRoom(GetRoomId());
	if (room != NULL) return room->GetLimitPlayerLevel();
	return 0;
	
}

UInt32 WSPlayer::GetRoomLimitSeasonLevel()
{
	Room* room = RoomMgr::Instance()->GetRoom(GetRoomId());
	if (room != NULL) return room->GetLimitPlayerSeasonLevel();
	return 0;
}

void WSPlayer::OnStartRoomRace()
{
	RoomMgr::Instance()->OnStartRoomRace(this);
}

void WSPlayer::OnCancleRoomMatch()
{
	RoomMgr::Instance()->OnCancleRoomMatch(this);
}

bool WSPlayer::IsOpenRoomFunction()
{
	FuncNotifyDataEntry* functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_PK);
	if (functionData)
	{
		if (GetLevel() < functionData->endLevel)
		{
			return false;
		}
	}
	return true;
}

UInt32 WSPlayer::GetChargeGoodsTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId)
{
	return m_AccountInfo == NULL ? 0 : m_AccountInfo->GetChargeTimes(mallType, goodsId, roleId);
}

UInt32 WSPlayer::GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId)
{
	return m_AccountInfo == NULL ? 0 : m_AccountInfo->GetTodayChargeTimes(mallType, goodsId, roleId);
}

void WSPlayer::AddChargeGoodsTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	m_AccountInfo->IncChargeTimes(accid, roleId, mallType, goodsId, times);
}

void WSPlayer::ResetChargeGoodsTimes()
{
	/*if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	m_AccountInfo->chargeRecord = AcccountChargeRecord();
	auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", GetAccGuid(), true);
	std::ostringstream ss;
	m_AccountInfo->chargeRecord.EncodeString(ss);
	cmd->PutData("charge_record", ss.str());
	CLRecordClient::Instance()->SendCommand(cmd);*/
}

bool WSPlayer::CanBuyMonthCard()
{
	if (!m_AccountInfo)
	{
		return false;
	}

	return m_AccountInfo->CanBuyMonthCard();
}

bool WSPlayer::CanBuyMoneyManageCard()
{
	if (!m_AccountInfo)
	{
		return false;
	}
	
	return m_AccountInfo->CanBuyMoneyManageCard();
}

void WSPlayer::OnSendRedPacketSuccess(ObjID_t redPacketId, UInt16 reason, UInt8 num, const std::string& name)
{
	auto redpacket = RedPacketMgr::Instance()->FindRedPacketByID(redPacketId);
	if (!redpacket)
	{
		return;
	}
	CLProtocol::WorldSendCustomRedPacketRes res;
	res.result = ErrorCode::SUCCESS;
	res.redPacketId = redPacketId;
	SendProtocol(res);

	auto data = RedPacketDataEntryMgr::Instance()->FindEntry(reason);
	if (data && (data->type == RED_PACKET_TYPE_NEW_YEAR || data->type == RED_PACKET_TYPE_GUILD))
	{
		if (data->type == RED_PACKET_TYPE_NEW_YEAR)
		{
			this->SyncEventToScene(SET_NEW_YEAR_RED_PACKET, data->totalMoney);
		}
		
		CLProtocol::SceneSyncChat chat;
		chat.channel = (data->type == RED_PACKET_TYPE_NEW_YEAR) ? CHAT_CHANNEL_WORLD : CHAT_CHANNEL_TRIBE;
		std::string link;
		//MsgPackage::GetRedPacketMsgTag(link, redPacketId);
		UInt32 notifyId = 0;
		if (data->type == RED_PACKET_TYPE_NEW_YEAR)
		{
			notifyId = 9302;
		}
		else if (data->type == RED_PACKET_TYPE_GUILD)
		{
			if (RPT_THR_TYPE_GUILD_ALL == data->thrType)
			{
				notifyId = 9312;
			}
			else if (RPT_THR_TYPE_GUILD_BATTLE == data->thrType || RPT_THR_TYPE_GUILD_CROSS_BATTLE == data->thrType)
			{
				notifyId = 9313;
			}
			else if (RPT_THR_TYPE_GUILD_DUNGEON == data->thrType)
			{
				notifyId = 9314;
			}
			else
			{
			}
		}
		chat.word = SysNotifyMgr::Instance()->MakeString(notifyId, redPacketId);
		chat.bLink = true;
		chat.objid = GetID();
		chat.objname = GetName();
		chat.level = GetLevel();
		chat.viplvl = GetVipLvl();
		chat.occu = GetOccu();
		chat.isGm = 0;
		chat.voiceKey = "";
		chat.voiceDuration = 0;
		chat.mask = CHAT_MASK_RED_PACKET;
		chat.headFrame = GetHeadFrame();
		if (data->type == RED_PACKET_TYPE_NEW_YEAR)
		{
			Router::BroadcastToPlayer(chat);
		}
		else
		{
			auto guild = GuildMgr::Instance()->FindGuildByID(GetGuildID());
			if (!guild)
			{
				return ;
			}
			guild->Broadcast(chat);
		}
	}

	if (data->type == RED_PACKET_TYPE_GUILD)
	{
		this->SendUdpLog("log_guild_readpacket", LOG_GUILD_REDPACKET, redPacketId, GUILD_REDPACKET_SEND, data->thrType, 
			redpacket->GetTotalMoney(), redpacket->GetNum(), 0, redpacket->GetOwner());
	}

}

void WSPlayer::OnSendRedPacketFailure(UInt32 errorCode)
{
	CLProtocol::WorldSendCustomRedPacketRes res;
	res.result = errorCode;
	res.redPacketId = 0;
	SendProtocol(res);
}

void WSPlayer::SendToGate(Avalon::Protocol &protocol)
{
	if(m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn,protocol);
	}
}

void WSPlayer::SendProtocol(Avalon::Protocol &protocol)
{
	if(m_Conn == NULL) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void WSPlayer::SendPacket(Avalon::Packet* packet)
{
	if(m_Conn == NULL || packet == NULL) return;
	if(GetGameStatus() != PS_NORMAL) return;

	CLProtocol::GateTransmitPacket req(packet);
	req.id = GetID();
	CL_SEND_PROTOCOL(m_Conn,req);
	req.Detach();
}

void WSPlayer::SendToScene(Avalon::Protocol& protocol)
{
	if(m_SceneConn != NULL)
	{
		CL_SEND_PROTOCOL(m_SceneConn,protocol);
	}
}

void WSPlayer::SendToScene(Avalon::Packet* packet)
{
	if(m_SceneConn != NULL)
	{
		m_SceneConn->SendPacket(packet);
	}
}

void WSPlayer::SyncEventToScene(SyncEventType type, ObjID_t param1, ObjID_t param2)
{
	CLProtocol::SceneSyncEvent protocol;
	protocol.playerId = GetID();
	protocol.type = type;
	protocol.param1 = param1;
	protocol.param2 = param2;
	SendToScene(protocol);
}

void WSPlayer::LoadData()
{
	//m_loadOfflineDataFail = false;
	//加载邮箱、关系、离线事件
	GetMailBox().LoadMails();
	GetRelationMgr().LoadRelations();
	GetOfflineEventMgr().LoadEvents();
	LoadGlobalInfo();
	LoadAccountInfo();
	LoadMallGiftPack();
	LoadAccountActivityTask();
	GetMasterSystem().LoadRelations();
	LoadMallItem();
	GetTitleMgr().LoadTitles();
	GetDailyTodoMgr().LoadDailyTodos();
	//AuctionTransRecordMgr::Instance()->LoadRecords(this);
	//LoadAccountChargeRecords();
	LoadAccountShopItemRoleBuyRecord();

	// 加载其他角色基础信息
	if(WSApplication::Instance()->IsLoadOtherRoleInfo())
	{
		CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_player_info");
		cmd->PutCond("accid", GetAccId());
		std::ostringstream limitoss;
		limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DELETE_PLAYER_SAVE_TIME << ")";
		cmd->SetLimit(limitoss.str());
		CLRecordClient::Instance()->SendCommand(cmd, new LoadOtherRoleBaseInfoCallback(this));

		InfoLogStream << "player(" << GetID() << ", " << GetAccId() << ") start load other role base info." << LogStream::eos;
	}
}

void WSPlayer::Saveback()
{

}

void WSPlayer::LoadGlobalInfo()
{
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_player_info");
	selectcmd->PutCond("guid", GetID());
	selectcmd->PutColumn("tribeskill");
	selectcmd->PutColumn("mallbuy");
	selectcmd->PutColumn("offlinemailtime");
	selectcmd->PutColumn("accid");
	selectcmd->PutColumn("name");
	selectcmd->PutColumn("sex");
	selectcmd->PutColumn("level");
	selectcmd->PutColumn("occu");
	selectcmd->PutColumn("season_level");
	selectcmd->PutColumn("totlechargenum");
	selectcmd->PutColumn("zoneid");
	selectcmd->PutColumn("savetime");
	selectcmd->PutColumn("academic_total_growth");
	selectcmd->PutColumn("master_dailytask_growth");
	selectcmd->PutColumn("master_academictask_growth");
	selectcmd->PutColumn("master_uplevel_growth");
	selectcmd->PutColumn("master_giveequip_growth");
	selectcmd->PutColumn("master_givegift_growth");
	selectcmd->PutColumn("good_teacher_value");
	selectcmd->PutColumn("apprenticmaster_timestamp");
	selectcmd->PutColumn("recruitdisciple_timestamp");
	selectcmd->PutColumn("fin_sch_disciple_sum");
	selectcmd->PutColumn("return_first_login");
	selectcmd->PutColumn("new_title_guid");
	CLRecordClient::Instance()->SendCommand(selectcmd ,new SelectGlobalInfoCallback(this));
}

void WSPlayer::LoadMallGiftPack()
{
	CLSelectCommand* selectGiftPackCmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_mall_gift_pack");
	selectGiftPackCmd->PutCond("accid", GetAccId());
	CLRecordClient::Instance()->SendCommand(selectGiftPackCmd, new SelectMallGiftPackCb(this));
}

void WSPlayer::LoadAccountActivityTask()
{
	//CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_activity_acc_task");
	//cmd->PutCond("accid", GetAccId());
	//CLRecordClient::Instance()->SendCommand(cmd, new SelectAccountActivityTaskCallback(this));
}

void WSPlayer::LoadAccountShopItemRoleBuyRecord()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_account_shop_role_buy_record");
	cmd->PutCond("owner", GetID());
	CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountShopItemRoleBuyRecordCallback(this));
}

void WSPlayer::LoadMallItem()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_mall_item");
	cmd->PutCond("roleid", GetID());
	CLRecordClient::Instance()->SendCommand(cmd, new LoadMallItemCallback(this));
}

void WSPlayer::SaveTribeSkill()
{
	std::ostringstream	os;
	for(std::map<UInt16, UInt8>::iterator iter = m_mapTribeSkills.begin(); iter != m_mapTribeSkills.end(); iter++)
	{
		os << iter->first << "," << UInt32(iter->second) << ",";
	}
	os << "0";
	
	CLUpdateCommand *cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("tribeskill", os.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WSPlayer::SaveMallBuyInfo()
{
	std::ostringstream	os;
	os << m_uGSMallBuyInfo << ";";

	for(std::map<UInt32, UInt16>::iterator	iter = m_mapMallBuyInfo.begin(); iter != m_mapMallBuyInfo.end(); iter++)
	{
		os << iter->first << "," << UInt32(iter->second) << ";";
	}
	os << "0";
	
	CLUpdateCommand *cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("mallbuy", os.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WSPlayer::LoadAccountInfo()
{
	auto info = AccountMgr::Instance()->FindAccount(m_AccId);
	if (info)
	{
		SetAccountInfo(info);
	}
	else if (m_AccId > 0 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_LOAD_ACCOUNT_INFO))
	{
		class QueryAccountCb : public AccountCallback
		{
		public:
			QueryAccountCb(UInt32 accid, ObjID_t roleId) : AccountCallback(accid), m_RoleId(roleId) {}

			void OnFinish(AccountInfo* info)
			{
				if (!info)
				{
					return;
				}

				auto player = WSPlayerMgr::Instance()->FindPlayer(m_RoleId);
				if (!player)
				{
					return;
				}

				if (player->GetAccountInfo() == NULL)
				{
					player->SetAccountInfo(info);
					InfoLogStream << "player(" << m_RoleId << ") set account info." << LogStream::eos;
				}
			}

			void OnFailure()
			{
				ErrorLogStream << "role(" << m_RoleId << ") load account(" << m_AccId << ") failed.\n";
			}
		private:
			ObjID_t				m_RoleId;
		};

		InfoLogStream << "player(" << GetAccId() << ", " << GetID() << ") start to load account info." << LogStream::eos;
		AccountMgr::Instance()->LoadAccountInfo(GetAccId(), new QueryAccountCb(GetAccId(), GetID()));
	}
}

void WSPlayer::SetOfflineReplyCtx(WSPlayer* player, LoadType lt, PlayerOfflineLoadCallBack cb)
{
	if (lt == LT_AUCTION_QUERY_ITEM || lt == LT_FRIEND_PRSENT)
	{
		if (cb == nullptr) return;
		m_LoadPlayerType = lt;
		m_playerOfflineLoadCb = cb;
	}
	else
	{
		if (!player)
		{
			return;
		}

		m_LoadPlayerType = lt;
		m_RequestTarget = player->GetID();
	}

}

void WSPlayer::ClearOfflineReplyCtx()
{
	m_LoadPlayerType = 0;
	m_RequestTarget = 0;
	SetRefTime(CURRENT_TIME.MSec());
	SetGameStatus(PS_FINAL);
	m_loadOfflineDataFail = false;
	m_playerOfflineLoadCb = nullptr;
}

void WSPlayer::OfflineLoadCb()
{
	if (m_LoadPlayerType == LT_AUCTION_QUERY_ITEM || m_LoadPlayerType == LT_FRIEND_PRSENT)
	{
		if (m_playerOfflineLoadCb != nullptr)
		{
			m_playerOfflineLoadCb(this);
		}
	}
	else
	{
		if (0 == m_RequestTarget)
		{
			return;
		}

		WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(m_RequestTarget);
		if (!target)
		{
			return;
		}

		if (m_LoadPlayerType == LT_MAKE_FRIEND)
		{
			if (target->MakeFriendOfflineCb(this))
			{
				target->SendNotify(3102);
			}

			return;
		}
		else if (m_LoadPlayerType == LT_MAKE_MASTER)
		{
			MakeMasterDisciple(target, this, MakeMasterDiscipleType::MMDT_MASTER_REPLY);

			return;
		}
		else if (m_LoadPlayerType == LT_MAKE_DISCIPLE)
		{
			MakeMasterDisciple(this, target, MakeMasterDiscipleType::MMDT_DISCIPLE_REPLY);

			return;
		}
		else if (m_LoadPlayerType == LT_ADDTO_BLACKLIST)
		{
			if (target->AddToBlackList(this))
			{
				target->SendNotify(3121);
			}
			return;
		}
		else if (m_LoadPlayerType == LT_MAKE_MASTERSECT_BROTHER)
		{
			WSMasterSystem::MakeBrotherRelation(this, target);
		}
		else if (m_LoadPlayerType == LT_MAKE_MASTERSECT_MASTER)
		{
			WSMasterSystem::MakeMasterAndBrotherRelationByMasterDataLoaded(this, target);
		}
		else if (m_LoadPlayerType == LT_MAKE_MASTERSECT_DISCIPLE)
		{
			WSMasterSystem::MakeDiscipleRelation(target, this);
		}
		else if (m_LoadPlayerType == LT_REMOVE_MASTERSECT_DISCIPLE)
		{
			WSMasterSystem::ClearRelationsOnRemoveDisciple(this);
		}
		else if (m_LoadPlayerType == LT_ONREMOVE_FRIEND)
		{
			this->OnRemovedFried(m_RequestTarget);
		}
		else if (m_LoadPlayerType == LT_2V2_SORTLIST_REWARD)
		{
			this->GetTitleMgr().AddOneTitleByTitleId(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_TITLE_ID), 
				SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_TITLE_TIME) * Avalon::Time::SECS_OF_MIN + CURRENT_TIME.Sec());
		}
	}
}

bool WSPlayer::IsOfflineLoad()
{
	return m_LoadPlayerType > 0;
}

void WSPlayer::SetMallItemBuyedNum(UInt32 uId, UInt16 uNum)
{
	m_mapMallBuyInfo[uId] = uNum;
	SaveMallBuyInfo();
}

UInt16 WSPlayer::GetMallItemBuyedNum(UInt32 uId)
{
	std::map<UInt32, UInt16>::iterator	iter = m_mapMallBuyInfo.find(uId);
	if(iter == m_mapMallBuyInfo.end())
		return 0;

	return iter->second;
}

void WSPlayer::OnLoadGlobalInfo(CLRecordCallback *pCallback)
{
	if (!pCallback->NextRow())
	{
		m_loadOfflineDataFail = true;
		return;
	}
	// 解码部落技能
	std::string	strSkills = pCallback->GetData("tribeskill").GetString();

	UInt16	uSkillId = 0;
	char	split;
	std::istringstream	is(strSkills);

	is >> uSkillId;
	while(uSkillId != 0)
	{
		UInt32	uVal;
		is >> split >> uVal;

		m_mapTribeSkills[uSkillId] = UInt8(uVal);

		uSkillId = 0;
		is >> split >>  uSkillId;
	}

	// 解码商城购买信息
	std::string	strMall = pCallback->GetData("mallbuy").GetString();

	UInt32	uMallItemId = 0;
	std::istringstream	isMall(strMall);

	isMall >> m_uGSMallBuyInfo >> split >> uMallItemId >> split;
	while(uMallItemId != 0)
	{
		UInt16	uNum = 0;
		isMall >> uNum >> split;
		m_mapMallBuyInfo[uMallItemId] = uNum;

		uMallItemId = 0;
		isMall >> uMallItemId >> split;
	}

	SetOfflineMailTime(pCallback->GetData("offlinemailtime"));

	SetLevel(pCallback->GetData("level"));
	SetOccu(pCallback->GetData("occu"));
	SetSex(pCallback->GetData("sex"));
	m_AccId = pCallback->GetData("accid");
	SetName(pCallback->GetData("name").GetString());
	SetSeasonLv(pCallback->GetData("season_level"));
	SetZoneId(pCallback->GetData("zoneid"));
	SetTotalPlayerChargeNum(pCallback->GetData("totlechargenum"));
	SetOfflineTime(pCallback->GetData("savetime"));
	SetReturnFirstLogin(pCallback->GetData("return_first_login"));
	m_curWearTitleGuid = pCallback->GetData("new_title_guid");
	this->GetMasterSystem().OnLoadPlayerDataRet(pCallback);
	
	if (m_AccountInfo == NULL)
	{
		LoadAccountInfo();
	}
	
	
}

void WSPlayer::OnFirstLogin()
{
	for(std::map<UInt32, UInt16>::iterator iter = m_mapMallBuyInfo.begin();
		iter != m_mapMallBuyInfo.end();)
	{
		if(iter->first >= MallMgr::NORMAL_LIMITED_ITEM_ID)
			m_mapMallBuyInfo.erase(iter++);
		else
			iter++;
	}
	SaveMallBuyInfo();

	// 查看账号转移的情况
	if (WSApplication::Instance()->IsNeedConvertAccount(GetPf()))
	{
		CLProtocol::CenterQueryConvertAccountReq req;
		req.roleid = GetID();
		req.platform = GetPf();
		req.openid = GetOpenId();
		Router::SendToCenter(req);
	}
}

void WSPlayer::SetSceneID(UInt32 id)
{
	if (GetSceneID() == id)
	{
		return;
	}

	m_SceneId = id;
	auto scene = WSSceneMgr::Instance()->FindScene(id);
	if (!scene)
	{
		ErrorLogStream << "player(" << GetID() << ") enter unknown scene(" << id << ")." << LogStream::eos;
		m_SceneType = SCENE_TYPE_INVALID;
		return;
	}

	m_SceneType = (SceneType)scene->GetType();

	// 换了场景肯定就不在匹配中
	SetMatching(false);
}

UInt32 WSPlayer::GetAuctionRefreshTime()
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	UInt32 intervalSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_AUTO_REFRESH_SEC);
	if (m_AuctionRefreshTime + intervalSec <= curTime)
	{
		RefreshAuctionRefreshTime();
	}

	return m_AuctionRefreshTime;
}

void WSPlayer::RefreshAuctionRefreshTime()
{
	m_AuctionRefreshTime = (UInt32)CURRENT_TIME.Sec();
	SyncEventToScene(SET_AUCTION_REFRESH_TIME, (UInt32)m_AuctionRefreshTime);
}

bool WSPlayer::IsInTown() const
{
	if (GetSceneType() == SCENE_TYPE_NORMAL || GetSceneType() == SCENE_TYPE_SINGLE)
	{
		return true;
	}

	return false;
}

void WSPlayer::SetVipLvl(UInt8 vip)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	if (vip < m_AccountInfo->GetVipLevel())
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") invalid new viplevel(" << vip << ")." << LogStream::eos;
		return;
	}
	m_AccountInfo->SetVipLevel(vip);
}

UInt8 WSPlayer::GetVipLvl() const
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return 0;
	}

	return m_AccountInfo->GetVipLevel();
}

void WSPlayer::SetVipExp(UInt32 exp)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	m_AccountInfo->SetVipExp(exp);
}

UInt32 WSPlayer::GetVipExp() const
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return 0;
	}

	return m_AccountInfo->GetVipExp();
}

void WSPlayer::SetTotalChargeNum(UInt32 num)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	m_AccountInfo->SetTotalChargeNum(num);
}

UInt32 WSPlayer::GetTotalChargeNum() const
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return 0;
	}

	return m_AccountInfo->GetTotalChargeNum();
}

void WSPlayer::SetMonthCardExpireTime(UInt32 time)
{
	m_MonthCardExpireTime = time;
}

UInt32 WSPlayer::GetMonthCardExpireTime() const
{
	return m_MonthCardExpireTime;
}

bool WSPlayer::IsInMonthCard() const
{
	UInt32 expireTime = GetMonthCardExpireTime();
	if ((UInt32)CURRENT_TIME.Sec() > expireTime)
	{
		return false;
	}

	return true;
}

void WSPlayer::SetMoneyManageStatus(UInt8 status)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}
	m_AccountInfo->SetMoneyManageStatus(status);
}


UInt8 WSPlayer::GetMoneyManageStatus()
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return 0;
	}
	return m_AccountInfo->GetMoneyManageStatus();
}

void WSPlayer::SetPoint(UInt32 point)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	m_AccountInfo->SetPoint(point);
}

UInt32 WSPlayer::GetPoint() const
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return 0;
	}

	return m_AccountInfo->GetPoint();
}

void WSPlayer::SetCreditPoint(UInt32 point)
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return;
	}

	m_AccountInfo->SetCreditPoint(point);
}

UInt32 WSPlayer::GetCreditPoint() const
{
	if (!m_AccountInfo)
	{
		ErrorLogStream << "player(" << GetID() << ", " << GetAccId() << ") can't find account info." << LogStream::eos;
		return 0;
	}

	return m_AccountInfo->GetCreditPoint();
}

void WSPlayer::SetGuildID(ObjID_t id)
{
	if (GetGuildID() == id)
	{
		return;
	}

	m_GuildId = id;
	OnGuildChanged();
}

//获取权限的值
void WSPlayer::GetVipValue(VipPrivilegeType type, UInt32& value)
{
	value = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(type, GetVipLvl());
}

UInt32 WSPlayer::GetVipValue(VipPrivilegeType type)
{
	return VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(type, GetVipLvl());
}

//获取权限的值
void WSPlayer::GetVipValue(VipPrivilegeType type, bool& value)
{
	UInt32 tmpValue = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(type, GetVipLvl());
	value = tmpValue == 0 ? false : true;
}


/*
void WSPlayer::SendUdpLog(const char *str1, const char *str2, const char *str3, const char *str4, const char *str5, const char *str6, const char *strType, Int32 nCount)
{
	CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_WORLD_UID);
	if(NULL == pLogger)
		return;

	std::ostringstream	os;
	os << m_OpenId << "|" << GetID() << "||3|||" << GetLevel() << "|||";
	os << "|||||" << UInt32(GetQQVipInfo().level) << "|" << UInt32(m_CreateTime) << "|" << ( GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID ) << "|cn|" << m_Pf;

	pLogger->SetUserIP(m_Ip.c_str());
	pLogger->SetUserMsg(os.str().c_str());
	pLogger->LogMsg(str1, str2, str3, str4, str5, str6, strType, nCount);
}
*/


void WSPlayer::SendLoginUdpLog()
{
	//SendUdpLog("login", LOG_LOGIN_FORMAT, m_OfflineTime / 1000, GetPoint());
}

void WSPlayer::SendLogoutUdpLog()
{
	//SendUdpLog("logout", LOG_LOGOUT_FORMAT, m_Ip.c_str(), m_OnlineTime);
}

void WSPlayer::SendAuctionUpLog(AuctionType type, UInt64 itemId, UInt32 itemDataId, UInt32 itemNum, MoneyType moneyType, UInt32 price, UInt32 deposit, UInt32 duetime)
{
	SendUdpLog("auction_up", LOG_AUCTION_UP_FORMAT, type, itemId, itemDataId, itemNum, moneyType, price, deposit, duetime);
}

void WSPlayer::SendAuctionDownLog(zjy_auction::AuctionSendItemType type, AuctionType auctionType, UInt64 itemId, UInt32 itemDataId, UInt32 itemNum)
{
	SendUdpLog("auction_down", LOG_AUCTION_DOWN_FORMAT, GetID(), type, auctionType, itemId, itemDataId, itemNum);
	//SendUdpLog("auction_down", LOG_AUCTION_DOWN_FORMAT, m_Level, type, auctionType, itemId, itemDataId, itemNum);
}

void WSPlayer::SendAuctionBuyLog(UInt64 targetId, UInt32 ownerVipLev, AuctionType type, bool abnormalDeal, ObjID_t srcItemId, UInt64 itemId,
	zjy_auction::AuctionMainItemType mainType, UInt32 itemDataId, UInt32 itemNum, MoneyType moneyType, UInt32 price,
	UInt32 tax, UInt8 strengthen, UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp,
	std::string sellerDeviceId, UInt32 targetAccId, UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid,
	UInt8 isAttented, UInt32 itemAttentNum, UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice,
	UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
	UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice,
	UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice, UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax)
{
	SendUdpLog("auction_buy", LOG_AUCTION_BUY_FORMAT, targetId, type, srcItemId, itemId, (UInt8)mainType, itemDataId, itemNum, moneyType,
		price, tax, (UInt8)abnormalDeal, strengthen, averPrice, onSealTime, itemQuality, sellerIp.c_str(), sellerDeviceId.c_str(), targetAccId,
		ownerVipLev, isTreas, sysRecomPrice, minPriceOnSale, transGuid, isAttented, itemAttentNum, mountMagicCardId, mountMagicCardAverPrice,
		mountBeadId, mountBeadAverPrice, eqQualityLv, remainSealCount,
		minPriceGuid, minPriceOnsalePrice, minPriceQualityLv, minPriceMountBeadId, minPriceMountBeadAverPrice, minPriceMountMagicId, minPriceMountMagicAverPrice,
		auctionPrice, auctionTransNum, auctionTransMax);
}

void WSPlayer::SendReviveUdpLog(UInt32 dungeonId, UInt64 targetId, UInt32 areaId)
{
	SendUdpLog("revive", LOG_REVIVE_FORMAT, dungeonId, targetId, areaId);
}

void WSPlayer::SendDungeonStartUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt8 isTeam, UInt8 isHaveFriend, UInt8 hellAutoClose)
{
	SendUdpLog("dungeon_start", LOG_DUNGEON_START_FORMAT, dungeonId, (UInt32)type, dungeonName.c_str(), (UInt32)hellmode, isTeam, isHaveFriend, hellAutoClose);
}

void WSPlayer::SendDungeonClearAreaUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt32 areaId, UInt32 usedTime)
{
	SendUdpLog("dungeon_area", LOG_DUNGEON_CLEAR_AREA_FORMAT, dungeonId, (UInt32)type, dungeonName.c_str(), (UInt32)hellmode, areaId, usedTime);
}

void WSPlayer::SendDungeonUsePotionUdpLog(UInt64 raceId, UInt32 dungeonId, UInt32 areaId, UInt32 itemTypeId, UInt8 isTeam)
{
	SendUdpLog("dungeon_use_potion", LOG_DUNGEON_USE_POTION_FORMAT, raceId, dungeonId, areaId, itemTypeId, isTeam);
}

void WSPlayer::SendDungeonDropUdpLog(UInt32 dungeonId, UInt32 itemId, UInt32 color, UInt32 itemLevel)
{
	SendUdpLog("dungeon_drop", LOG_DUNGEON_DROP_FORMAT, dungeonId, itemId, color, itemLevel);
}

void WSPlayer::SendGuildPvpUdpLog(UInt64 guildId, std::string guildName, UInt64 targetId, std::string targetName, PkType type, GuildBattleType battleType, UInt32 result, UInt32 oldGuildScore, UInt32 newGuildScore)
{
	SendUdpLog("guild_pvp_end", LOG_GUILD_PVP_END_FORMAT, guildId, guildName.c_str(), targetId, targetName.c_str(), type, battleType, result, oldGuildScore, newGuildScore);
}

void WSPlayer::SendMatchSuccessLog(UInt32 zoneId, ObjID_t opponentId, UInt8 pkType, UInt32 seasonLevel, UInt32 matchScore, UInt32 usedTime)
{
	SendUdpLog("match_success", LOG_MATCH_SUCCESS_FORMAT, zoneId, opponentId, pkType, seasonLevel, matchScore, usedTime);
}

void WSPlayer::SendMatchCancelLog(UInt8 pkType, UInt32 seasonLevel, UInt32 matchScore, UInt32 matchTime)
{
	SendUdpLog("match_cancel", LOG_MATCH_CANCEL_FORMAT, pkType, seasonLevel, matchScore, matchTime);
}

void WSPlayer::SendFunctionUdpLog(FunctionLogType type, UInt32 p1, UInt32 p2)
{
	SendUdpLog("function", LOG_FUNCTION_FORMAT, (UInt32)type, p1, p2);
}

void WSPlayer::SendPvpEndUdpLog(UInt32 targetZoneId, UInt64 targetId, PkType type, PkRaceResult result, UInt32 oldMatchScore, UInt32 newMatchScore, UInt32 oldPkValue, UInt32 newPkValue, UInt32 oldSeasonLevel, UInt32 newSeasonLevel, UInt32 oldSeasonStar, UInt32 newSeasonStar, UInt32 oldSeasonExp, UInt32 newSeasonExp, UInt32 seasonId, UInt32 dungeonId, UInt32 usedTime)
{
	SendUdpLog("pvp_end", LOG_PVP_END_FORMAT, targetZoneId, targetId, type, result, oldMatchScore, newMatchScore, oldPkValue, newPkValue, oldSeasonLevel, newSeasonLevel, oldSeasonStar, newSeasonStar, oldSeasonExp, newSeasonExp, seasonId, dungeonId, usedTime);
}

void WSPlayer::SendRoomCreateUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime)
{
	SendUdpLog("room", LOG_ROOM_FORMAT, LogRoomOperationType::ROOM_OPERATION_TYPE_CREATE, roomId, roomType, "", createTime, 1);
}

void WSPlayer::SendRoomDeleteUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime)
{
	SendUdpLog("room", LOG_ROOM_FORMAT, LogRoomOperationType::ROOM_OPERATION_TYPE_DELETE, roomId, roomType, "", createTime, 0);
}

void WSPlayer::SendRoomJoinUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime)
{
	SendUdpLog("room", LOG_ROOM_FORMAT, LogRoomOperationType::ROOM_OPERATION_TYPE_JOIN, roomId, roomType, "", createTime, 0);
}

void WSPlayer::SendRoomQuitUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime)
{
	SendUdpLog("room", LOG_ROOM_FORMAT, LogRoomOperationType::ROOM_OPERATION_TYPE_QUIT, roomId, roomType, "", createTime, 0);
}

void WSPlayer::SendRoomMatchStartUdpLog(UInt32 roomId, UInt32 roomType, UInt32 roomPlayerSize, UInt32 createTime)
{
	SendUdpLog("room", LOG_ROOM_FORMAT, LogRoomOperationType::ROOM_OPERATION_TYPE_MATCH_START, roomId, roomType, "", createTime, roomPlayerSize);
}

void WSPlayer::SendRoomBattleStartUdpLog(UInt32 roomId, UInt32 roomType, UInt32 roomPlayerSize, UInt32 createTime)
{
	SendUdpLog("room", LOG_ROOM_FORMAT, LogRoomOperationType::ROOM_OPERATION_TYPE_BATTLE_START, roomId, roomType, "", createTime, roomPlayerSize);
}

void WSPlayer::SendItemUdpLog(const char* reason, UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, std::string itemName, UInt32 itemQuality, Int32 number, UInt32 oldNumber)
{
	if (reason == NULL)
	{
		return;
	}
	SendUdpLog("item", LOG_ITEM_FORMAT, reason, itemId, itemDataId, itemType, itemSubType, itemName.c_str(), number, oldNumber, oldNumber + number, itemQuality, 0, 0, 0);
}

void WSPlayer::SendMakeMasterDiscipleUdpLog(ObjID_t discipleId, ObjID_t masterId, UInt32 discipleLevel, UInt32 masterLevel, UInt32 discipleViplev, UInt32 masterViplev, UInt32 timestamp, UInt8 eventType, UInt8 masterOccu, UInt8 discipleOccu)
{
	SendUdpLog("master_make_masterdisciple", LOG_MASTERSYS_MAKE_MASTERDISCIPLE, discipleId, masterId, discipleLevel, masterLevel, discipleViplev, masterViplev, timestamp, eventType, masterOccu, discipleOccu);
}

void WSPlayer::SendFinishSchoolUdpLog(UInt8 type, ObjID_t discipleId, ObjID_t masterId, UInt32 discipleLevel, UInt32 masterLevel, UInt32 discipleGrowth, UInt32 apprentTimeSec, UInt32 finishTimeSec)
{
	SendUdpLog("mastersys_finshcool", LOG_MASTERSYS_FINISH_SCHOOL, type, discipleId, masterId, discipleLevel, masterLevel, discipleGrowth, apprentTimeSec, finishTimeSec);
}

void WSPlayer::SendMasterPublishDailyTaskLog(ObjID_t discipleId, ObjID_t masterId, UInt32 timestamp)
{
	SendUdpLog("mastersys_pub_dailytask", LOG_MASTERSYS_PUBISH_DAILYTASK, discipleId, masterId, timestamp);
}



void WSPlayer::SendReceiveDailyTaskGiftLog(UInt32 type, ObjID_t uid, UInt32 timestamp)
{
	SendUdpLog("mastersys_rece_dailytask_gift", LOG_MASTERSYS_RECEIVE_DAILYTASKGIFT, type, uid, timestamp);
}

void WSPlayer::SendMasterDiscipleInteractionLog(ObjID_t discipleId, ObjID_t masterId, UInt32 dungeonId, UInt32 timestamp)
{
	SendUdpLog("mastersys_interaction", LOG_MASTERSYS_INTERACTION, discipleId, masterId, dungeonId, timestamp);
}

void WSPlayer::SendCheatLog(PlayerCheatReason reason, ObjID_t raceId, UInt32 dungeonId)
{
	SendUdpLog("cheat", LOG_PLAYER_CHEAT, (UInt32)reason, raceId, dungeonId);
}

void WSPlayer::SendAssetUdpLog(AssetType type, const char* reason, UInt64 oldAsset, Int64 assetNum, Int64 nowAssetNum, UInt64 nowAsset)
{
	SendUdpLog("asset", LOG_ASSET_FORMAT, reason, type, oldAsset, assetNum, nowAssetNum, nowAsset);
}

void WSPlayer::SendAdventureTeamNameChangeUdpLog(const std::string& oldName, const std::string& newName)
{
	//SendUdpLog("adventure_team_name_change", LOG_ADVENTURE_TEAM_NAME_CHANGE, oldName.c_str(), newName.c_str());
}

void WSPlayer::SendDispatchExpeditionTeamUdpLog(UInt8 mapId, UInt32 duration, UInt32 memberNum)
{
	SendUdpLog("expedition_team", LOG_EXPEDITION_TEAM, 1, (UInt32)mapId, duration, memberNum);
}

void WSPlayer::SendCancelExpeditionTeamUdpLog(UInt8 mapId)
{
	SendUdpLog("expedition_team", LOG_EXPEDITION_TEAM, 2, (UInt32)mapId, 0, 0);
}

void WSPlayer::SendGetExpeditionTeamRewardsUdpLog(UInt8 mapId, UInt32 duration, const std::vector<UInt32>& condIds)
{
	std::vector<UInt32> tmpCondIds = condIds;
	tmpCondIds.resize(5);
	SendUdpLog("get_expedition_team_rewards", LOG_GET_EXPEDITION_TEAM_REWARDS, (UInt32)mapId, duration, tmpCondIds[0], tmpCondIds[1], tmpCondIds[2], tmpCondIds[3], tmpCondIds[4]);
}

void WSPlayer::SendBlackMarketTransUdpLog(UInt8 opType, UInt64 transid, UInt64 auctId, UInt8 auctType, UInt32 auctItemDataId, UInt64 auctItemId, UInt32 auctPrice)
{
	SendUdpLog("black_market_trans", LOG_BLACKMARKET_TRANSCATION, opType, transid, auctId, auctType, auctItemDataId, auctItemId, auctPrice);
}

void WSPlayer::SendAdventureTeamInfoUdpLog()
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}

	std::string adventureTeamName = GetAdventureTeamName();
	if (adventureTeamName.empty())
	{
		return;
	}

	UInt32 blessCrystalNum = (UInt32)GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_BLESS_CRYSTAL);
	UInt32 inheritBlessNum = (UInt32)GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_INHERIT_BLESS);
	UInt32 bountyNum = (UInt32)GetAccountInfo()->GetAccountCounterNum(ACC_COUNTER_BOUNTY);

	SendUdpLog("adventure_team_info", LOG_ADVENTURE_TEAM_INFO, GetAdventureTeamLevel(), adventureTeamName.c_str(), blessCrystalNum, inheritBlessNum, bountyNum, GetAdventureTeamGradeNum(), GetAdventureRoleScore());
}

void WSPlayer::SendRechargePushUdpLog(UInt32 pushId, UInt32 price)
{
	SendUdpLog("recharge_push", LOG_RECHARGE_PUSH, pushId, price);
}

void WSPlayer::SendGuildJoinExitUdpLog(UInt32 opType)
{
	SendUdpLog("guild_join_exit", LOG_GUILD_JOIN_EXIT, opType);
}

void WSPlayer::SendGungeonRollUdpLog(UInt32 dungeonId, UInt32 rollIndex, UInt32 rollItem, UInt32 rollItenNum, UInt32 rollType, UInt32 point)
{
	SendUdpLog("dungeon_roll", LOG_DUNGEON_ROLL, dungeonId, rollIndex, rollItem, rollItenNum, rollType, point);
}

void WSPlayer::SendAdventurePassUdpLog(UInt32 level, UInt32 passType)
{
	SendUdpLog("adventure_pass", LOG_ADVENTURE_PASS_FORMAT, level, passType);
}

void WSPlayer::SendAddFriendUdpLog(UInt32 guildId, UInt32 tLevel, UInt32 tVipLev, UInt32 tOccu, UInt32 tGuildId)
{
	SendUdpLog("add_friend", LOG_ADD_FRIEND, guildId, tLevel, tVipLev, tOccu, tGuildId);
}

void WSPlayer::SendMoneyStockUdpLog()
{
	UInt32 gnomePoint = GetGnomeCoinNum();
	SendUdpLog("money_stock_acc", LOG_MONEY_STOCK_ACC, gnomePoint);
}

UInt8 WSPlayer::GetOpenFlag(UInt8 type) const
{
	std::map<UInt8, UInt8>::const_iterator iter = m_mapOpenFlags.find(type);
	if (iter != m_mapOpenFlags.end())
	{
		return iter->second;
	}
	return 0;
}

void WSPlayer::AddGuildSkill(UInt16 id, UInt8 level)
{
	auto skill = GetGuildSkill(id);
	if (skill)
	{
		skill->level = level;
	}
	else
	{
		SkillBase info;
		info.id = id;
		info.level = level;
		m_GuildSkills.push_back(info);
	}
}

SkillBase* WSPlayer::GetGuildSkill(UInt16 id)
{
	for (UInt32 i = 0; i < m_GuildSkills.size(); i++)
	{
		if (m_GuildSkills[i].id == id)
		{
			return &m_GuildSkills[i];
		}
	}

	return NULL;
}

UInt8 WSPlayer::GetGuildSkillLevel(UInt16 id)
{
	auto skill = GetGuildSkill(id);
	if (!skill)
	{
		return 0;
	}

	return skill->level;
}

void WSPlayer::SetGuildSkillLevel(UInt16 id, UInt8 level, UInt32 skillType)
{
	auto skill = GetGuildSkill(id);
	if (!skill)
	{
		AddGuildSkill(id, level);
	}
	else
	{
		skill->level = level;
	}

	CLProtocol::WorldGuildUpdateSkill sync;
	sync.id = GetID();
	sync.skillType = skillType;
	sync.skill.id = id;
	sync.skill.level = level;
	SendToScene(sync);
}

UInt32 WSPlayer::GetCreateFromNowOnDay()
{
	if (m_CreateTime > CURRENT_TIME.Sec())
	{
		return 0;
	}

	Avalon::Time creatT(Avalon::UInt64(m_CreateTime) * 1000);
	UInt32 deltaT = (UInt32)CURRENT_TIME.Sec() - (UInt32)(GetDayBeginTime(creatT) / 1000);
	return (deltaT / (24 * 3600) + 1);
}

//Avalon::Packet* WSPlayer::MakeChangeZonePacket()

void WSPlayer::HandleSceneSyncResistMagic(UInt32 value)
{
	if (this->GetResistMagic() == value)
	{
		return;
	}

	SetResistMagic(value);

	auto team = this->GetTeam();
	if (!team)
	{
		return;
	}

	team->OnPlayerResistMageicChanged(this);
}

void WSPlayer::ReturnRset()
{
	// 好友师徒，回归消息
	GetRelationMgr().NotifyRelationReturn();

	// 通知scene首次回归
	CLProtocol::WorldFirstReturnSync sync;
	sync.roleId = GetID();
	SendToScene(sync);

	// 清除回归角色首次登陆
	SetReturnFirstLogin(0);
	UpdateReturnFirstLogin();
}

bool WSPlayer::IsVeteranReturn() const
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_NEW_RETURN))
	{
		return false;
	}

	if (!ActivityUtil::IsInVeteranReturnPeriod(GetRoleReturnTime()))
	{
		return false;
	}

	return true;
}

void WSPlayer::OnCheckVeteranReturn()
{
	auto activitySystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_VETERAN_RETURN);
	if (!activitySystemData)
	{
		ErrorLogStream << "Can not find activity system data(" << (UInt32)ACTEM_VETERAN_RETURN << ")!" << LogStream::eos;
		return;
	}

	if (activitySystemData->isClose)
		return;

	if (!IsVeteranReturn())
		return;

	InfoLogStream << PLAYERINFO(this) << " veteran return, time=" << GetRoleReturnTime() << LogStream::eos;

	// 注册运营活动
	GetPlayerActivityInfo().RegisterVeteransReturnOpActivity();
}

void WSPlayer::UpdateReturnFirstLogin()
{
	CLUpdateCommand *cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("return_first_login", m_ReturnFirstLogin);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WSPlayer::HandlerSecurityLockDataReq(const std::string& deviceID)
{
	CLProtocol::WorldSecurityLockDataRes res;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
	{
		SendProtocol(res);
		return;
	}

	// 锁住安全锁
	if (!m_AccountInfo->GetSecurityPasswd().empty() && m_AccountInfo->GetSecurityState() == SECURITY_STATE_UNLOCK)
	{
		m_AccountInfo->SetSecurityState(SECURITY_STATE_LOCK);
	}

	bool isCommon = false;
	if ( !deviceID.empty())
		isCommon = m_AccountInfo->IsCommonDev(deviceID);

	res.isCommonDev = isCommon ? 1 : 0;

	if (isCommon && m_AccountInfo->GetSecurityState() != SECURITY_STATE_FORCE)
		m_AccountInfo->SetSecurityState(SECURITY_STATE_UNLOCK);
		
	res.lockState = m_AccountInfo->GetSecurityState();

	if (m_AccountInfo->GetSecurityPasswd().empty())
		res.isUseLock = 0;
	else
		res.isUseLock = 1;

	res.freezeTime = m_AccountInfo->GetSecurityFreezeTime();
	res.unFreezeTime = m_AccountInfo->GetSecurityUnFreezeTime();

	SendProtocol(res);

	// 密码错误次数
	NotifySecurityPasswdErrorNum();

	// 更新常用设备登录时间
	if (isCommon)
	{
		m_AccountInfo->UpdateCommonDevLoginTime(deviceID);
		m_AccountInfo->SaveToDB();
	}

	// 初始化当前在线的设备
	m_curOnlineDev = deviceID;

	// 同步scene安全锁的状态
	SyncSecurityStatusToScene();
}

void WSPlayer::ChangeSecurityPasswdReq(const std::string& _oldPasswd, const std::string& _newPasswd)
{
	CLProtocol::WorldChangeSecurityPasswdRes res;
	res.errNum = m_AccountInfo->GetSecurityPasswdErrorNum();

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
	{
		res.ret = ErrorCode::SYS_NOT_OPEN;
		SendProtocol(res);
		return;
	}

	UInt32 max_error_num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SECURITY_LOCK_PASSWD_ERROR_NUM);
	if (m_AccountInfo->GetSecurityPasswdErrorNum() >= max_error_num)
	{
		res.ret = ErrorCode::SECURITY_LOCK_PASSWD_ERROR_MAX_NUM;
		SendProtocol(res);
		return;
	}

	// 检查密码的长度
	if (!m_AccountInfo->CheckPasswdLength(_newPasswd))
	{
		res.ret = ErrorCode::SECURITY_LOCK_PASSWD_LENGTH_ERROR;
		SendProtocol(res);
		return;
	}

	if (_oldPasswd.empty() || _oldPasswd != m_AccountInfo->GetSecurityPasswd())
	{
		m_AccountInfo->AddSecurityPasswdErrorNum();
		NotifySecurityPasswdErrorNum();

		res.ret = ErrorCode::SECURITY_LOCK_CHANGE_PASSWD_ERROR;
		res.errNum = m_AccountInfo->GetSecurityPasswdErrorNum();
		SendProtocol(res);
		return;
	}

	m_AccountInfo->SetSecurityPasswd(_newPasswd);
	m_AccountInfo->ClearCommonDev();
	m_AccountInfo->SaveToDB();

	InfoLogStream << "Change Secutiry Passwd Accid:" << m_AccountInfo->GetAccID() << " PlayerId:" << GetID() << LogStream::eos;

	res.ret = ErrorCode::SUCCESS;
	SendProtocol(res);
}

void WSPlayer::HandlerSecurityLockOpReq(UInt32 _type, const std::string& _passwd)
{
	CLProtocol::WorldSecurityLockOpRes res;
	res.lockOpType = _type;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
	{
		res.ret = ErrorCode::SYS_NOT_OPEN;
		SendProtocol(res);
		return;
	}

	switch (_type)
	{
	case LT_LOCK:
	{
		// 第一次上锁，没有密码
		if (m_AccountInfo->GetSecurityPasswd().empty())
		{
			if (_passwd.empty())
			{
				res.ret = ErrorCode::SECURITY_LOCK_FAILED;
				SendProtocol(res);
				return;
			}

			// 检查密码的长度
			if (!m_AccountInfo->CheckPasswdLength(_passwd))
			{
				res.ret = ErrorCode::SECURITY_LOCK_PASSWD_LENGTH_ERROR;
				SendProtocol(res);
				return;
			}

			m_AccountInfo->SetSecurityPasswd(_passwd);
		}

		m_AccountInfo->SetSecurityState(SECURITY_STATE_LOCK);
		m_AccountInfo->SaveToDB();

		// 设置安全锁日志
		SendUdpLog("security_lock", LOG_SECURITY_LOCK, SL_LOG_SET_SECURITY_LOCK);

		SyncSecurityStatusToScene(SECURITY_STATE_LOCK);
	}
		break;
	case LT_UNLOCK:
	{
		if (m_AccountInfo->GetSecurityState() == SECURITY_STATE_FORCE)
		{
			res.ret = ErrorCode::SECURITY_LOCK_FORCE_UNLOCK;
			SendProtocol(res);
			return;
		}

		UInt32 max_error_num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SECURITY_LOCK_PASSWD_ERROR_NUM);
		if (m_AccountInfo->GetSecurityPasswdErrorNum() >= max_error_num)
		{
			res.ret = ErrorCode::SECURITY_LOCK_PASSWD_ERROR_MAX_NUM;
			SendProtocol(res);
			return;
		}

		if (_passwd.empty() || _passwd != m_AccountInfo->GetSecurityPasswd())
		{
			res.ret = ErrorCode::SECURITY_LOCK_PASSWD_ERROR;
			SendProtocol(res);

			m_AccountInfo->AddSecurityPasswdErrorNum();
			NotifySecurityPasswdErrorNum();
			return;
		}

		// 每次解锁只在本次登录有效
		m_AccountInfo->SetSecurityState(SECURITY_STATE_UNLOCK);

		SyncSecurityStatusToScene(SECURITY_STATE_UNLOCK);
	}
		break;
	case LT_FORCE_UNLOCK:
	{
		if (m_AccountInfo->GetSecurityPasswd().empty())
		{
			res.ret = ErrorCode::SECURITY_LOCK_FAILED;
			SendProtocol(res);
			return;
		}

		UInt32 start_freeze = UInt32(CURRENT_TIME.Sec());
		UInt32 end_freeze = UInt32(CURRENT_TIME.Sec()) + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SECURITY_LOCK_FORCE_TIME) * 60;

		m_AccountInfo->SetSecurityFreezeTime(start_freeze);
		m_AccountInfo->SetSecurityUnFreezeTime(end_freeze);
		m_AccountInfo->SetSecurityState(SECURITY_STATE_FORCE);
		m_AccountInfo->SaveToDB();

		res.freezeTime = start_freeze;
		res.unFreezeTime = end_freeze;
	}
		break;
	case LT_CANCAL_APPLY:
	{
		if (m_AccountInfo->GetSecurityUnFreezeTime() == 0)
		{
			res.ret = ErrorCode::SECURITY_LOCK_FAILED;
			SendProtocol(res);
			return;
		}

		// 解除冻结
		m_AccountInfo->SetSecurityFreezeTime(0);
		m_AccountInfo->SetSecurityUnFreezeTime(0);
		m_AccountInfo->SetSecurityState(SECURITY_STATE_LOCK);
		m_AccountInfo->SaveToDB();
	}
		break;
	default:return;
	}

	res.lockState = m_AccountInfo->GetSecurityState();
	res.ret = ErrorCode::SUCCESS;
	SendProtocol(res);
}

void WSPlayer::HandlerCommonDeviceReq(UInt32 type, const std::string& deviceID)
{
	CLProtocol::WorldBindDeviceRes res;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
	{
		res.ret = ErrorCode::SYS_NOT_OPEN;
		SendProtocol(res);
		return;
	}

	// 类型0是解绑，否则就是绑定
	if (0 == type)
	{
		if (deviceID.empty())
			return;

		m_AccountInfo->DropCommonDev(deviceID);
		m_AccountInfo->SaveToDB();
		res.bindState = 0;
		res.deviceID = deviceID;

		InfoLogStream << "UnBind Common Device Accid:" << m_AccountInfo->GetAccID() << " PlayerId:" << GetID() 
			<<" DeviceId:"<<deviceID<< LogStream::eos;
	}
	else
	{
		std::string dev_code;
		if (deviceID.empty())
			m_AccountInfo->GetDeviceCode(dev_code);
		else
			dev_code = deviceID;

		m_AccountInfo->AddCommonDev(dev_code);
		m_AccountInfo->SaveToDB();
		res.bindState = 1;
		res.deviceID = dev_code;

		InfoLogStream << "Bind Common Device Accid:" << m_AccountInfo->GetAccID() << " PlayerId:" << GetID()
			<< " DeviceId:" << dev_code << LogStream::eos;

		// 设置常用设备日志
		SendUdpLog("security_lock", LOG_SECURITY_LOCK, SL_LOG_SET_COMMON_DEVICE);
	}

	res.ret = ErrorCode::SUCCESS;
	SendProtocol(res);
}

void WSPlayer::SyncSecurityStatusToScene()
{
	UInt32 lockState = SECURITY_STATE_UNLOCK;

	do
	{
		if (m_AccountInfo->IsCommonDev(m_curOnlineDev))
			break;

		if (m_AccountInfo->GetSecurityUnFreezeTime() > UInt32(CURRENT_TIME.Sec()))
			lockState = SECURITY_STATE_FORCE;
		else
			lockState = m_AccountInfo->GetSecurityState();

	} while (0);

	SyncSecurityStatusToScene(lockState);
}

void WSPlayer::SyncSecurityStatusToScene(UInt32 _state)
{
	CLProtocol::WorldSecurityStatusSync syncInfo;
	syncInfo.playerId = GetID();
	syncInfo.state = _state;      

	SendToScene(syncInfo);
}

bool WSPlayer::SecurityLockCheck()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
		return true;

	if (m_AccountInfo->GetSecurityState() != SECURITY_STATE_UNLOCK)
	{
		CLProtocol::WorldSecurityLockForbid notify;
		notify.ret = ErrorCode::SECURITY_LOCK_FORBID_OP;
		SendProtocol(notify);
		return false;
	}

	return true;
}

void WSPlayer::SecurityLockForceCheck()
{
	if (m_AccountInfo->GetSecurityUnFreezeTime() == 0)
		return;

	if (m_AccountInfo->GetSecurityState() != SECURITY_STATE_FORCE)
		return;

	if (m_AccountInfo->GetSecurityUnFreezeTime() > CURRENT_TIME.Sec())
		return;

	InfoLogStream <<"SecurityLockForceCheck Accid:"<<m_AccountInfo->GetAccID()<<" PlayerId:"<<GetID() << LogStream::eos;

	// 关闭锁
	ClearSecurityLockNotifyClient();
}

void WSPlayer::ClearSecurityLockNotifyClient()
{
	m_AccountInfo->SetSecurityPasswd("");
	m_AccountInfo->SetSecurityFreezeTime(0);
	m_AccountInfo->SetSecurityUnFreezeTime(0);
	m_AccountInfo->SetSecurityState(SECURITY_STATE_UNLOCK);
	m_AccountInfo->ClearCommonDev();
	m_AccountInfo->ClearSecurityPasswdErrorNum();
	m_AccountInfo->SaveToDB();

	// 同步scene
	SyncSecurityStatusToScene(SECURITY_STATE_UNLOCK);

	// 同步客户端
	CLProtocol::WorldSecurityLockDataRes res;
	res.lockState = SECURITY_STATE_UNLOCK;
	res.isCommonDev = 0;
	res.isUseLock = 0;
	res.freezeTime = 0;
	res.unFreezeTime = 0;
	SendProtocol(res);

	NotifySecurityPasswdErrorNum();
}

void WSPlayer::NotifySecurityPasswdErrorNum()
{
	CLProtocol::WorldSecurityLockPasswdErrorNum res;
	res.error_num = m_AccountInfo->GetSecurityPasswdErrorNum();
	SendProtocol(res);
}

void WSPlayer::SetCanBuyAuctionGuids(const std::vector<ObjID_t>& guids)
{
	this->m_AuctionIds = guids;
}

bool WSPlayer::CheckCanBuyAuction(const ObjID_t& guid)
{
	for (size_t i = 0; i < m_AuctionIds.size(); ++i)
	{
		if (guid == m_AuctionIds[i])
			return true;
	}
	return false;
}

void WSPlayer::SyncAccountDataToScene(UInt32 type)
{
	CLProtocol::WorldAccountDataSync sync;
	sync.roleId = GetID();
	sync.type = type;
	if (type == OFFLINE_TIME)
	{
		sync.val = std::to_string(m_AccountInfo->GetOfflineTime());
	}
	else if (type == MAX_ACC_ROLE_LEVEL)
	{
		sync.val = std::to_string(MaxRoleLevel());
	}

	SendToScene(sync);
}

MonopolyInfo& WSPlayer::GetMonopolyInfo()
{
	static MonopolyInfo info;
	auto account = GetAccountInfo();
	if (account == nullptr)
	{
		ErrorLogStream << PLAYERINFO(this) << "can't find account info" << LogStream::eos;
		return info;
	}
	return account->GetMonopolyInfo();
}

template<typename T>
T WSPlayer::GetGameSetValue(UInt32 type, UInt32 subType)
{
	auto iter = m_GameSetMap.find(type);
	if (iter == m_GameSetMap.end())
		return T(0);

	switch (type)
	{
	case GST_FRIEND_INVATE:
	{
		return Avalon::StringUtil::ConvertToValue<T>(iter->second);
	}
	case GST_SECRET:
	{
		UInt32 val = Avalon::StringUtil::ConvertToValue<T>(iter->second);
		return (val & subType);
	}
	}

	return T(0);
}

void WSPlayer::ParseGameSet(const std::string& gameSet)
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(gameSet, param_1, "|");
	for (auto& iter : param_1)
	{
		if (iter.empty())
			continue;

		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, "_");
		if (param_2.size() != 2)
		{
			ErrorLogStream << "playerId:(" << GetID() << ") game set " << gameSet << "error" << LogStream::eos;
			continue;
		}

		UInt32 type = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		std::string val = param_2[1];

		m_GameSetMap[type] = val;
	}
}

void WSPlayer::SendAdventurePassStatus()
{
	CLProtocol::WorldAventurePassStatusRet msg;
	msg.lv = 0;

	auto season = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(CURRENT_TIME);
	if (season != nullptr)
	{
		msg.startTime = season->startTime;
		msg.endTime = season->endTime;
		msg.seasonID = season->id;
	}

	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(GetAccId());
	if (pass == nullptr)
	{
		SendProtocol(msg);
		return;
	}
	if (!pass->IsUnLock())
	{
		SendProtocol(msg);
		return;
	}
	msg.lv = pass->GetLevel();
	msg.exp = pass->GetExp();
	msg.type = pass->GetPassType();
	msg.activity = pass->GetActivity();
	msg.normalReward = pass->GetNormalRewardStatus();
	msg.highReward = pass->GetHighRewardStatus();
	SendProtocol(msg);
}

void WSPlayer::SyncAccountCounterToClient(UInt32 type)
{
	CLProtocol::WorldAccountCounterNotify sync;
	CLProtocol::AccountCounter accCnt;
	accCnt.type = type;
	accCnt.num = GetAccountInfo()->GetAccountCounterNum(AccountCounterType(type));
	sync.accCounterVec.push_back(accCnt);
	SendProtocol(sync);
}

void WSPlayer::SyncAccountCounterToScene(UInt32 type)
{
	CLProtocol::WorldAccountCounterSync sync;
	sync.roleId = GetID();
	sync.accCounter.type = type;
	sync.accCounter.num = GetAccountInfo()->GetAccountCounterNum(AccountCounterType(type));
	SendToScene(sync);
}

void WSPlayer::SyncAllAccountCounterToClient()
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}
	CLProtocol::WorldAccountCounterNotify sync;
	for (UInt32 type = (UInt32)AccountCounterType::ACC_COUNTER_INVALID + 1; 
		type < (UInt32)AccountCounterType::ACC_COUNTER_TYPE_MAX; ++type)
	{
		if (type == (UInt32)ACC_COUNTER_ENERGY_COIN)
		{
			continue;
		}

		CLProtocol::AccountCounter accCnt;
		accCnt.type = type;
		accCnt.num = GetAccountInfo()->GetAccountCounterNum(AccountCounterType(type));
		sync.accCounterVec.push_back(accCnt);
	}
	SendProtocol(sync);
}

void WSPlayer::SyncCustomerAccountCounterToClient(const std::vector<UInt32>& types)
{
	if (GetAccountInfo() == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << " account info is null!" << LogStream::eos;
		return;
	}
	CLProtocol::WorldAccountCounterNotify sync;
	for (auto type : types)
	{
		CLProtocol::AccountCounter accCnt;
		accCnt.type = type;
		accCnt.num = GetAccountInfo()->GetAccountCounterNum(AccountCounterType(type));
		sync.accCounterVec.push_back(accCnt);
	}
	SendProtocol(sync);
}

bool WSPlayer::HasRoleLevelLimit(UInt16 level) const
{
	if (GetLevel() >= level)
	{
		InfoLogStream << "player(" << GetID() << ") self role level(" << GetLevel() << ")." << LogStream::eos;
		return true;
	}

	for (const auto& info : m_OtherRoleInfo)
	{
		if (info.level >= level)
		{
			InfoLogStream << "player(" << GetID() << ") other role(" << info.roleid << ") level(" << GetLevel() << ")." << LogStream::eos;
			return true;
		}
	}

	return false;
}

UInt32 WSPlayer::MaxRoleLevel() const
{
	UInt32 maxLevel = GetLevel();
	for (const auto& info : m_OtherRoleInfo)
	{
		maxLevel = info.level > maxLevel ? info.level : maxLevel;
	}
	return maxLevel;
}

void WSPlayer::SetPubishTasFlag(UInt64 discpleId)
{
	m_pubishTaskFlag[discpleId] = 1;
}
bool WSPlayer::CheckPublishTaskFlag(UInt64 discpleId)
{
	auto it = m_pubishTaskFlag.find(discpleId);
	if (it == m_pubishTaskFlag.end())
	{
		return false;
	}
	return m_pubishTaskFlag[discpleId] == 1;
}

void WSPlayer::ResetPubishTasFlag(UInt64 discpleId)
{
	m_pubishTaskFlag[discpleId] = 0;
}