#include "GameParamMgr.h"

#include <CLRecordClient.h>
#include <CLGameDefine.h>
#include <CLWorldLevelProtocol.h>
#include <CLCenterProtocol.h>
#include <CLGSActivityProtocol.h>
#include <CLSyncObjectProtocol.h>

#include "WSSelectInitDataCallback.h"
#include "WSSortListMgr.h"
#include "WSRouter.h"
#include "PremiumLeagueMgr.h"
#include "WSNetwork.h"
#include "WSActivityMgr.h"

class SelectGameParamsCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GameParamMgr::Instance()->OnSelectDataRet(this);
	}
};

GameParamMgr::GameParamMgr()
{
	m_IsLoad = false;
}

GameParamMgr::~GameParamMgr()
{
}

void GameParamMgr::Init()
{
	m_FirstService = false;
	m_IsLoad = false;

	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_gameparam");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectGameParamsCallback());
}

void GameParamMgr::Final()
{
}

UInt32 GameParamMgr::GetGameStartTime() const
{
	return GetValue(PARAM_NAME_GAME_START);
}

void GameParamMgr::CalcWorldLevel(UInt64 now)
{
	UInt32	uGameStartTime = GetValue(PARAM_NAME_GAME_START);

	Avalon::Date	startday(UInt64(uGameStartTime) * 1000);
	startday.Hour(0);
	startday.Min(0);
	startday.Sec(0);

	if((now - startday.ToTime().Sec()) < (10 * DAY_TO_SEC))
		return;

	// 世界等级访问器
	class GetWorldLevelVisitor : public WSSortListVisitor
	{
	public:
		GetWorldLevelVisitor() : m_uWorldLevelSum(0), m_uCount(0) {} 
		~GetWorldLevelVisitor() {}

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			if(ranking > 10)
			{
				return false;
			}
			
			m_uWorldLevelSum += entry.value1;
			m_uCount++;

			return true;
		}

		UInt32 GetWorldLevel() 
		{
			if(0 == m_uCount)
				return 0;

			return ((m_uWorldLevelSum - 1) / m_uCount) + 1;
		}

	private:
		UInt32	m_uWorldLevelSum;
		UInt32	m_uCount;
	};

	GetWorldLevelVisitor	visitor;
	WSSortListMgr::Instance()->VisitSortList(SORTLIST_LEVEL, visitor);
	UInt32	uWorldLevel = visitor.GetWorldLevel();
	if(0 != uWorldLevel)
	{
		SetValue("world_lvl", uWorldLevel);
		WORLD_LEVEL = uWorldLevel;

		CLProtocol::WorldNotifyWorldLevel	sync_c;
		sync_c.uWorldLevel = uWorldLevel;
		Router::BroadcastToPlayer(sync_c);

		CLProtocol::WorldNotifyWorldLevelToS	sync_s;
		sync_s.uWorldLevel = uWorldLevel;
		Router::BroadcastToScene(sync_s);
	}
}

void GameParamMgr::OnAdminConnected(UInt32 id)
{
	if (IsFirstService())
	{
		CLProtocol::CenterPunishInfoReq protocol;
		protocol.zoneid = ZONE_ID;
		protocol.centerPlatform = PLATFORM;
		Router::SendToCenter(protocol);

		DebugLogStream << "start request punish info, zoneid=" << protocol.zoneid << ", platform=" << protocol.centerPlatform << LogStream::eos;
	}
}

void GameParamMgr::OnSceneConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL) return;

	CLProtocol::WorldNotifyGameStartTimeToS syncGameTime;
	syncGameTime.time = GAMESTART_TIME;
	syncGameTime.firstTime = GAME_FIRST_START_TIME;
	CL_SEND_PROTOCOL(conn, syncGameTime);

	// 同步数据到scene
	GameParamSyncScene();
}

void GameParamMgr::OnGateConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL) return;

	CLProtocol::WorldNotifyGameStartTimeToS syncGameTime;
	syncGameTime.time = GAMESTART_TIME;
	syncGameTime.firstTime = GAME_FIRST_START_TIME;
	CL_SEND_PROTOCOL(conn, syncGameTime);
}

void GameParamMgr::SetValue(const char* name, UInt32 value)
{
	if(name == NULL) return;

	GameParamMap::iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end())
	{
		if(iter->second.value == value) return;

		iter->second.value = value;

		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gameparam", iter->second.id, true);
		updateCmd->PutData("value", value);
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else if(value != 0)
	{
		GameParam param;
		param.id = CLRecordClient::Instance()->GenGuid();
		param.value = value;
		m_GameParams.insert(std::make_pair(name, param));

		CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gameparam", param.id);
		insertCmd->PutData("name", name);
		insertCmd->PutData("value", value);
		CLRecordClient::Instance()->SendCommand(insertCmd);
	}
}

UInt32 GameParamMgr::GetValue(const char* name) const
{
	GameParamMap::const_iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end()) return iter->second.value;
	return 0;
}

bool GameParamMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	while(callback->NextRow())
	{
		GameParam param;
		param.id = callback->GetKey();
		param.value = callback->GetData("value");
		m_GameParams.insert(std::make_pair(callback->GetData("name").GetString(), param));
	}

	if(GetValue(PARAM_NAME_GAME_START) == 0)
	{
		SetValue(PARAM_NAME_GAME_START, UInt32(CURRENT_TIME.Sec()));
	}

	if (GetValue(PARAM_NAME_GAME_FIRST_START) == 0)
	{
		SetValue(PARAM_NAME_GAME_FIRST_START, UInt32(CURRENT_TIME.Sec()));
		m_FirstService = true;
	}

	GAMESTART_TIME = GetValue(PARAM_NAME_GAME_START);
	GAME_FIRST_START_TIME = GetValue(PARAM_NAME_GAME_FIRST_START);
	WORLD_LEVEL = GetValue("world_lvl");
	if(0 == WORLD_LEVEL)
	{
		CalcWorldLevel(CURRENT_TIME.Sec());
	}

	UInt8 premiumLeagueStatus = GetValue(PARAM_NAME_PREMIUM_LEAGUE_STATUS);
	PremiumLeagueMgr::Instance()->SetStatus((PremiumLeagueStatus)premiumLeagueStatus);

	// 开服时间同步到其他服务器
	CLProtocol::WorldNotifyGameStartTimeToS syncGameTime;
	syncGameTime.time = GAMESTART_TIME;
	syncGameTime.firstTime = GAME_FIRST_START_TIME;
	Router::BroadcastToScene(syncGameTime);
	Router::BroadcastToGate(syncGameTime);

	m_IsLoad = true;

	InfoLogStream << "game param load finish!" << LogStream::eos;

	// 加载完毕同步数据到Scene
	GameParamSyncScene();

	// 处理开服时间的活动
	WSActivityMgr::Instance()->OpenServerTimeActivity(GAME_FIRST_START_TIME);

	return true;
}

void GameParamMgr::GameParamSyncScene()
{
	if ( !m_IsLoad)
		return;

	InfoLogStream << "game param send to scene!" << LogStream::eos;

	CLProtocol::SceneGameParamSync sync;

	// 荣誉排行时间
	{
		UInt32 honorRank = GetValue(HONOR_RANK_TIME);
		sync.gameParamMap[HONOR_RANK_TIME] = honorRank;
	}

	// 全民砍价折扣
	{
		UInt32 discount = GetValue(WHOLE_BARGAIN_DISCOUNT);
		sync.gameParamMap[WHOLE_BARGAIN_DISCOUNT] = discount;
	}

	Router::BroadcastToScene(sync);
}
