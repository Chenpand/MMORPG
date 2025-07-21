#include "ChampionService.h"
#include <CLChampionDataEntry.h>
#include <CLChampionProtocol.h>
#include <CLErrorCode.h>
#include "GameZone.h"
#include "ChampionPlayer.h"
#include "ChampionScene.h"
#include "USApplication.h"
#include "ChampionRaceMgr.h"
#include "USRouter.h"
#include "GameZoneMgr.h"
#include "USGamble.h"
#include "USChampionStatus.h"
#include "USGameParamMgr.h"


#define REGISTER_STATUS_ACT_FUN(status, act, fun) \
		m_StatusActMap[status][act] = std::bind(fun, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);



bool IsLogicSameStatus(ChampionStatus a, ChampionStatus b)
{
	if (a >= CS_SEA_SELECT_PREPARE && a <= CS_1_SELECT_PREPARE)
	{
		if (a + 40 == b)
		{
			return true;
		}
	}

	if (b >= CS_SEA_SELECT_PREPARE && b <= CS_1_SELECT_PREPARE)
	{
		if (b + 40 == a)
		{
			return true;
		}
	}

	if (a == b)
	{
		return true;
	}

	if (a == CS_ENROLL && b == CS_SEA_SELECT_BATTLE)
	{
		return true;
	}

	if (b == CS_ENROLL && a == CS_SEA_SELECT_BATTLE)
	{
		return true;
	}

	return false;
}


bool IsFirstBigThenSecondLogicStatus(ChampionStatus a, ChampionStatus b)
{
	if (a >= CS_BATTLE_END || a <= CS_BATTLE_BEGIN)
	{
		if (a >= CS_SEA_SELECT_PREPARE && a <= CS_1_SELECT_PREPARE + 1)
		{
			a = static_cast<ChampionStatus>(a + 40);
		}
		else
		{
			return false;
		}
	}

	if (b >= CS_BATTLE_END || b <= CS_BATTLE_BEGIN)
	{
		if (b >= CS_SEA_SELECT_PREPARE && b <= CS_1_SELECT_PREPARE + 1)
		{
			b = static_cast<ChampionStatus>(b + 40);
		}
		else
		{
			return false;
		}
	}

	if (a >= b)
	{
		return true;
	}

	return false;
}


bool IsLogicSameRace(ChampionStatus a, ChampionStatus b)
{
	if (a >= CS_SEA_SELECT_PREPARE && a <= CS_1_SELECT_PREPARE)
	{
		if (a + 40 == b)
		{
			return true;
		}
	}

	if (b >= CS_SEA_SELECT_PREPARE && b <= CS_1_SELECT_PREPARE)
	{
		if (b + 40 == a)
		{
			return true;
		}
	}

	return false;
}

bool IsPrepareStatus(ChampionStatus status)
{
	if (status > CS_PREPARE_BEGIN && status < CS_PREPARE_END)
	{
		return true;
	}

	return false;
}

bool IsBattleStatus(ChampionStatus status)
{
	if (status > CS_BATTLE_BEGIN && status < CS_BATTLE_END)
	{
		return true;
	}

	return false;
}


ChampionService::ChampionService()
{
	m_Status = CS_NULL;
	m_LoadFinish = false;
}

ChampionService::~ChampionService()
{
	if (m_StatusObj != nullptr)
	{
		CL_SAFE_DELETE(m_StatusObj);
	}
}

void ChampionService::Clear()
{
	m_PlayerMgr.ClearUSPlayer();
	m_SceneMgr.ClearScene();
	m_EnrollAccidSet.clear();
	m_Top16TableVec.clear();
	m_UserIDConnIDMap.clear();
}

USPlayer* ChampionService::CreatePlayer()
{
	return new ChampionPlayer;
}

UScene* ChampionService::CreateScene()
{
	return new ChampionScene;
}


bool ChampionService::Init()
{
	UnionService::Init();

	m_StatusObj =nullptr;

	LoadPlayerData();

	// 注册行为回调
	REGISTER_STATUS_ACT_FUN(CS_ENROLL,CA_ENROLL,&ChampionService::OnPlayerCheckEnroll)
	REGISTER_STATUS_ACT_FUN(CS_SEA_SELECT_BATTLE, CA_RELIVE, &ChampionService::OnPlayerRelive)
	REGISTER_STATUS_ACT_FUN(CS_SEA_SELECT_BATTLE, CA_CHECK_RELIVE, &ChampionService::OnPlayerCheckRelive)
	for (UInt32 i = CS_PREPARE_BEGIN + 1; i < CS_PREPARE_END; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_JOIN_RACE, &ChampionService::OnPlayerJoinRace)
	}

	for (UInt32 i = CS_BATTLE_BEGIN + 1; i < CS_BATTLE_END; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_RACE_END, &ChampionService::OnPlayerRaceEnd)
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_JOIN_RACE, &ChampionService::OnPlayerJoinRace)
	}

	for (UInt32 i = CS_NULL + 1; i <= CS_NUM; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_RACE_END, &ChampionService::OnPlayerRaceEnd)
	}

	for (UInt32 i = CS_8_SELECT_BATTLE; i <= CS_1_SELECT_BATTLE; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_OBSERVE, &ChampionService::OnPlayerObserve)
	}

	LoadTimePoint();

	return true;
}

void ChampionService::OnTick(const Avalon::Time & now)
{
	UnionService::OnTick(now);

	if (!BoLoadFinish())
	{
		return;
	}

	if (!m_IsGMControlStatus)
	{
		CalcStatus(now);
	}

	if (!IsBattleStatus(m_Status))
	{
		return;
	}

	m_StatusObj->OnTick(now);

}

void ChampionService::OnConnected(GameZone* zone)
{
	if (zone == nullptr)
	{
		return;
	}
}

UnionServerFunctionType ChampionService::GetFunctionType()
{
	return USFT_CHAMPION;
}

void ChampionService::OnAddNewPlayer(USPlayer * player)
{
	InfoLogStream << "AddNewPlayer success: " << player->GetID() << LogStream::eos;
	m_EnrollAccidSet.insert(player->GetAccID());
	static_cast<ChampionPlayer*>(player)->SetStatus(CS_SEA_SELECT_PREPARE);
	static_cast<ChampionPlayer*>(player)->InsterToDB();
}

void ChampionService::GMChangeStatus(const ChampionStatus status)
{
	InfoLogStream << "GMChangeStatus is " << static_cast<UInt32>(status) << LogStream::eos;
	m_IsGMControlStatus = true;
	ChangeToStatus(status);
}

void ChampionService::GMChangePlayerStatus(UInt64 roleID, std::function<void(ChampionPlayer*)> changer)
{
	if (roleID == 0)
	{
		auto vec = m_PlayerMgr.GetAllUSPlayer();
		for (auto& player : vec)
		{
			changer(static_cast<ChampionPlayer*>(player));
		}
		return;
	}
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleID));
	if (player == nullptr)
	{
		return;
	}
	changer(player);
}

void ChampionService::SetTop16Table(std::vector<ChampionPlayer*> vec)
{
	m_Top16TableVec = vec;
}

const std::vector<ChampionTop16Player> ChampionService::GetTop16Table()
{
	std::vector<ChampionTop16Player> vec;
	for (auto& player : m_Top16TableVec)
	{
		ChampionTop16Player topPlayer;
		topPlayer.roleID = player->GetID();
		topPlayer.pos = player->GetPos();
		topPlayer.name = player->GetName();
		topPlayer.occu = player->GetOccu();
		topPlayer.avatar = player->GetAvatar();
		topPlayer.server = player->GetServer();
		topPlayer.zoneID = player->GetZoneID();
		vec.push_back(topPlayer);
	}
	return vec;
}

void ChampionService::Sync16Table(GameZone* zone)
{
	CLProtocol::SceneChampion16TableSync syncTable;
	syncTable.playerVec = GetTop16Table();
	if (zone == nullptr)
	{
		BroadcastToZone(syncTable, ST_SCENE);
	}
	else
	{
		zone->TransmitToScene(syncTable);
	}
}

const UInt32 ChampionService::GetConnID(const UInt64 roleID) const
{
	auto iter = m_UserIDConnIDMap.find(roleID);
	if (iter == m_UserIDConnIDMap.end())
	{
		return 0;
	}
	return iter->second;
}

void ChampionService::GetPlayerAttributes(UInt64 roleID, std::function<void(ChampionPlayer*)> geter)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleID));
	if (player == nullptr)
	{
		return;
	}
	geter(player);
}

ChampionPlayer * ChampionService::GetPlayer(UInt64 roleID)
{
	return static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleID));
}

void ChampionService::OnPlayerScoreChanged(ChampionPlayer* player)
{
	if (GetStatus() == CS_RE_SEA_SELECT_BATTLE || GetStatus() == CS_64_SELECT_BATTLE || GetStatus() == CS_16_SELECT_BATTLE || 
		GetStatus() == CS_RE_SEA_SELECT_PREPARE || GetStatus() == CS_64_SELECT_PREPARE || GetStatus() == CS_16_SELECT_PREPARE)
	{
		if (m_StatusObj == nullptr)
		{
			return;
		}
		m_StatusObj->OnPlayerScoreChanged(player);
	}
}

const std::list<ChampionPlayer*>& ChampionService::OnPlayerGetGroupScoreRank(ChampionPlayer * player)
{
	if (m_StatusObj == nullptr)
	{
		return m_NullGroupScoreRank;
	}
	return m_StatusObj->OnPlayerGetGroupScoreRank(player);
}

const std::vector<ChampionScoreBattleRecord>& ChampionService::OnPlayerGetGroupScoreRecord(ChampionPlayer* player)
{
	if (m_StatusObj == nullptr)
	{
		return m_NullScoreBattleRecord;
	}
	return m_StatusObj->OnPlayerGetGroupScoreRecord(player);
}

UInt32 ChampionService::BeginAct(ChampionAct act, UInt32 accid, UInt64 roleId, ChampionActInfoBase* info)
{
	InfoLogStream << "accid(" << accid << ") roleid(" << roleId << ") is begin act(" << static_cast<UInt32>(act) << ")" << LogStream::eos;
	auto iter = m_StatusActMap[static_cast<ChampionStatus>(m_Status)].find(act);
	if (iter == m_StatusActMap[static_cast<ChampionStatus>(m_Status)].end())
	{
		InfoLogStream << "accid(" << accid << ") roleid(" << roleId << ") is begin act(" << static_cast<UInt32>(act) << ") but can fin function." << LogStream::eos;
		return ErrorCode::CHAMPION_STATUS_ERROR;
	}
	UInt32 result = iter->second(accid, roleId, info);
	InfoLogStream << "accid(" << accid << ") roleid(" << roleId << ") is begin act(" << static_cast<UInt32>(act) << ") result is" << result << LogStream::eos;
	return result;
}

bool ChampionService::IsEnroll(UInt32 accid)
{
	auto iter = m_EnrollAccidSet.find(accid);
	if (iter == m_EnrollAccidSet.end())
	{
		return false;
	}
	return true;
}

void ChampionService::SetDBStatus(ChampionStatus status)
{
	m_DBStatus = status;
}


UInt32 ChampionService::OnPlayerCheckEnroll(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info)
{
	CLProtocol::UnionCheckChampionEnrollRes res;
	if (IsEnroll(accid))
	{
		return ErrorCode::CHAMPION_ALREADY_ENROLL;
	}
	return ErrorCode::SUCCESS;
}

void ChampionService::LoadTimePoint()
{

	class ChampionTimeVistor : public Avalon::DataEntryVisitor<ChampionTimeDataEntry>
	{
	public:
		bool operator()(ChampionTimeDataEntry* dataEntry)
		{
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<ChampionTimeDataEntry*> datas;
	};

	memset(m_TimePoints, 0, sizeof(m_TimePoints));

	ChampionTimeVistor vistor;
	ChampionTimeDataEntryMgr::Instance()->Visit(vistor);

	for (auto data : vistor.datas)
	{
		if (!data)
		{
			return;
		}

		ChampionStatusInfo point;
		point.status = data->status;
		point.startTime = data->startTime;
		point.endTime = data->endTime;
		point.preStartTime = data->preStartTime;
		point.prepareStartTime = data->prepareStartTime;
		point.battleStartTime = data->battleStartTime;
		point.battleEndTime = data->battleEndTime;


		if (m_TimePoints[point.status].endTime == 0 || point.endTime < m_TimePoints[point.status].endTime)
		{
			m_TimePoints[point.status] = point;
		}
	}


	for (int i = CS_NULL; i < CS_NUM; i++)
	{
		auto& point = m_TimePoints[i];
		InfoLogStream << "status(" << (UInt8)point.status << ") start(" << point.startTime << ") end(" << point.endTime << ")." << LogStream::eos;
	}

	InfoLogStream << "success." << LogStream::eos;
}


class SelectChampionPlayerCallBack : public CLRecordCallback
{
public:
	SelectChampionPlayerCallBack(ChampionService& service) : m_Service(service) {}
	virtual void OnFinished()
	{
		m_Service.OnLoadPlayerData(this);
		m_Service.SetLoadFinish(true);
	}
	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load ChampionPlayer Faild,ErrorCode is" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}

private:
	ChampionService& m_Service;
};


void ChampionService::LoadPlayerData()
{
	SelectChampionPlayerCallBack* callback = new SelectChampionPlayerCallBack(*this);
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_champion_role_list");
	CLRecordClient::Instance()->SendCommand(selectCmd, callback);
}

void ChampionService::OnLoadPlayerData(CLRecordCallback * callback)
{
	while (callback->NextRow())
	{
		auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.AddUSPlayer(callback->GetKey()));
		player->LoadDBData(callback);
		m_EnrollAccidSet.insert(player->GetAccID());
	}

	{
		//生成16强表
		auto vec = m_PlayerMgr.GetSuitUSPlayer([&](USPlayer* player)->bool {
			auto newPlayer = static_cast<ChampionPlayer*>(player);
			if (newPlayer->GetPos() > 0)
			{
				return true;
			}
			return false;
		});
		for (auto& player : vec)
		{
			auto newPlayer = static_cast<ChampionPlayer*>(player);
			m_Top16TableVec.push_back(newPlayer);
		}

		//生成Group
		std::map<UInt32, ChampionPlayer*> tempPosMap;
		for (auto& player : m_Top16TableVec)
		{
			tempPosMap[player->GetPos()] = player;
		}
		for (UInt32 pos = 31; pos >= 1; --pos)
		{
			UInt32 calcPos = pos;
			while (tempPosMap.count(calcPos) == 0)
			{
				calcPos /= 2;
				if (calcPos == 0)
				{
					break;
				}
			}
			if (calcPos != pos && calcPos != 0)
			{
				tempPosMap[pos] = tempPosMap[calcPos];
			}
		}

		for (UInt32 pos = 2; pos <= 30; pos+=2)
		{
			if (tempPosMap.count(pos) > 0 && tempPosMap.count(pos + 1) > 0)
			{
				ChampionPlayer* winner = nullptr;
				ChampionPlayer* loser = nullptr;
				if (tempPosMap[pos]->GetGroup() > tempPosMap[pos + 1]->GetGroup())
				{
					winner = tempPosMap[pos + 1];
					loser = tempPosMap[pos];
				}
				else
				{
					winner = tempPosMap[pos];
					loser = tempPosMap[pos + 1];
				}
				
				if (winner == nullptr || loser == nullptr)
				{
					continue;
				}

				ChampionGroupStatus info;
				info.groupID = loser->GetGroup();
				info.roleA = winner->GetID();
				info.scoreA = loser->GetLoseCount();
				info.roleB = loser->GetID();
				info.scoreB = loser->GetWinCount();
				info.status = winner->GetStatus() == loser->GetStatus() ? ChampionGroupBattleStatus ::CGBS_NOT_START : CGBS_END;
				ChampionGroupInfoMgr::Instance()->Push(info);
			}
		}

	}

}

void ChampionService::OnPlayerDisconnect(UInt64 roleId)
{
	InfoLogStream << "received player disconnect,role id is " << roleId << LogStream::eos;
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleId));
	if (player == nullptr)
	{
		InfoLogStream << "can't find player,id is" << roleId << LogStream::eos;
		return;
	}
	player->SetOnline(false);
	--m_InPlayerNum;
}

void ChampionService::OnPlayerMove(UInt64 roleID, Avalon::Packet* packet)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleID));
	if (player == nullptr)
	{
		return;
	}
	auto scene = m_SceneMgr.FindUScene(player->GetSceneID());
	if (scene == nullptr)
	{
		return;
	}
	scene->BroadcastPacket(packet);
}

bool ChampionService::OnCheckAddNewPlayer(UInt64 roleId, UInt32 accid)
{
	if (IsEnroll(accid))
	{
		return false;
	}
	return true;
}

void ChampionService::OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet * packet)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleID));
	if (player == nullptr)
	{
		return;
	}

	m_SceneMgr.AddSyncPacket(roleID, packet);
	m_SceneMgr.SyncAllObjectToPlayer(player, player->GetSceneID());

	auto scene = m_SceneMgr.FindUScene(player->GetSceneID());
	if (scene == nullptr)
	{
		return;
	}
	scene->BroadcastPacket(packet);

}

void ChampionService::OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet * packet)
{
	OnPlayerMove(roleID, packet);
}

void ChampionService::OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet * packet)
{
	OnPlayerMove(roleID, packet);
}

void ChampionService::OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID)
{
	auto& players = m_PlayerMgr.GetAllUSPlayer();
	for (auto player : players)
	{
		if (player->GetZoneID() == oldZoneID)
		{
			player->SetZoneID(newZoneID);
		}
	}


	auto gambleRecordVec = USGambleMgr::Instance()->GetAllRecord();
	for (auto record : gambleRecordVec)
	{
		if (record->GetZoneID() == oldZoneID)
		{
			record->SetZoneID(newZoneID);
		}
	}

	Sync16Table(nullptr);
}

void ChampionService::OnSceneReady(CLProtocol::UnionSceneIsReady & protocol)
{
	auto zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == nullptr)
	{
		return;
	}
	CLProtocol::SceneReceiveSyncChampionStatus sync;
	sync.status = m_TimePoints[m_Status];
	zone->TransmitToScene(sync);

	Sync16Table(zone);


	ChampionGroupInfoMgr::Instance()->SyncAllGroupInfo();

}

UInt32 ChampionService::OnMatchQueryPlayerInfo(CLProtocol::UnionMatchServerQueryPlayerInfoReq & req)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(req.msg.roleId));
	if (player == nullptr)
	{
		return MATCH_QUERY_PLAYER_OFFLINE;
	}
	if (!player->GetOnline())
	{
		return MATCH_QUERY_PLAYER_OFFLINE;
	}
	player->SendToWorld(req);
	return ErrorCode::SUCCESS;
}

void ChampionService::OnNotifyRaceStart(CLProtocol::WorldNotifyRaceStart & req)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(req.roleId));
	if (player == nullptr)
	{
		return;
	}
	if (!player->GetOnline())
	{
		return;
	}
	CLProtocol::WorldMatchRaceStart start;
	start.roleId = player->GetID();
	player->SendToScene(start);
	player->SetIsInRace(true);
	player->SendToWorld(req);
}

void ChampionService::OnMatchBattleRes(CLProtocol::MatchServerChampionBattleRes & req)
{
	if (m_Status == CS_1_SELECT_BATTLE || m_Status == CS_2_SELECT_BATTLE || m_Status == CS_4_SELECT_BATTLE || m_Status == CS_8_SELECT_BATTLE)
	{
		ChampionRaceMgr::Instance()->OnRaceCreateRet(req.groupID, req.order, req.sessionId);
	}
}

void ChampionService::OnMatchOnPVPRes(CLProtocol::MatchServerRaceEnd & req)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(req.result.roleId));
	if (player == nullptr)
	{
		return;
	}
	player->SendToWorld(req);
}

void ChampionService::OnMatchRaceEnd(CLProtocol::MatchServerChampionRaceEnd & req)
{
	BattleResultActInfo info;
	info.winnerID = req.winner.id;
	info.loseID = req.loser.id;
	info.result = static_cast<_resultBitmask>(req.result);
	BeginAct(CA_RACE_END, 0, 0, &info);

	if (m_Status == CS_1_SELECT_BATTLE || m_Status == CS_2_SELECT_BATTLE || m_Status == CS_4_SELECT_BATTLE || m_Status == CS_8_SELECT_BATTLE)
	{
		ChampionRaceMgr::Instance()->OnRaceEnd(req.raceId, info.trueWinner, info.trueLoser, req.result);
	}
}

void ChampionService::OnEnrollStart()
{
	Clear();
	auto delCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_champion_role_list","");
	CLRecordClient::Instance()->SendCommand(delCmd);
	delCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_champion_battle_record", "");
	CLRecordClient::Instance()->SendCommand(delCmd);
	delCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_champion_gamble", "");
	CLRecordClient::Instance()->SendCommand(delCmd);
	delCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_champion_gamble_record", "");
	CLRecordClient::Instance()->SendCommand(delCmd);


	ChampionRaceMgr::Instance()->Clear();
	USGambleMgr::Instance()->Clear();
	ChampionGroupInfoMgr::Instance()->Clear();
}

void ChampionService::OnEnrollEnd()
{

}

//new fun


UInt32 ChampionService::OnPlayerJoinRace(UInt32 accid, UInt64 roleId, ChampionActInfoBase* info)
{
	if (m_InPlayerNum >= USApplication::Instance()->GetPlayerLimit())
	{
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}

	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleId));
	if (player == nullptr)
	{
		ErrorLogStream << "player (" << accid << "," << roleId << ")not exists" << LogStream::eos;
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}

	InfoLogStream << PLAYERINFO(player) << " status is " << static_cast<UInt32>(player->GetStatus()) << " champion status is " << static_cast<UInt32>(m_Status) << LogStream::eos;

	if (!IsLogicSameStatus(player->GetStatus(), m_Status))
	{
		if (!IsFirstBigThenSecondLogicStatus(player->GetStatus(), m_Status))
		{
			if (player->GetStatus() != CS_END_SHOW)
			{

				return ErrorCode::CHAMPION_PERMISSION_DENIED;
			}
		}
	}

	auto newInfo = static_cast<JoinRaceActInfo*>(info);

	player->SetID(newInfo->msg->roleId);
	player->SetAccID(newInfo->msg->accId);
	player->SetZoneID(CL_GET_ZONEID_FROM_NODEID(newInfo->msg->sceneNode));
	player->SetConnId(newInfo->msg->GetConnID());
	player->SetName(newInfo->msg->name);
	player->SetOccu(newInfo->msg->occu);
	player->SetLevel(newInfo->msg->level);
	player->SetVipLevel(newInfo->msg->vipLvl);
	player->SetSceneNodeId(newInfo->msg->sceneNode);
	player->SetWorldNodeId(newInfo->msg->worldNode);
	player->SetGuildId(newInfo->msg->guildId);
	player->SetAwaken(newInfo->msg->awaken);
	player->SetAvatar(newInfo->msg->avatar);
	player->SetServer(newInfo->msg->server);

	InfoLogStream << PLAYERINFO(player) << " union join race Success " << LogStream::eos;

	if (m_StatusObj == nullptr)
	{
		ErrorLogStream << PLAYERINFO(player) << " request join race but obj is null" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}

	auto result = m_StatusObj->OnPlayerJoinRace(player);
	if (result == ErrorCode::SUCCESS)
	{
		++m_InPlayerNum;
	}
	return result;
}

UInt32 ChampionService::OnPlayerRaceEnd(UInt32 accid, UInt64 roleId, ChampionActInfoBase * info)
{
	if (info == nullptr)
	{
		ErrorLogStream << "info is null(" << accid << "," << roleId << ")" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}

	auto newInfo = static_cast<BattleResultActInfo*>(info);

	auto winner = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(newInfo->winnerID));
	if (winner == nullptr)
	{
		ErrorLogStream << "winner is null(" << accid << "," << roleId << ") info is(" << newInfo->winnerID << "," << newInfo->loseID << "," << newInfo->result << ")"  << LogStream::eos;
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}
	winner->SetIsInRace(false);


	auto loser = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(newInfo->loseID));
	if (loser == nullptr)
	{
		ErrorLogStream << "loser is null(" << accid << "," << roleId << ") info is(" << newInfo->winnerID << "," << newInfo->loseID << "," << newInfo->result << ")" << LogStream::eos;
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}
	loser->SetIsInRace(false);

	if (m_StatusObj == nullptr)
	{
		ErrorLogStream << "m_StatusObj is null(" << accid << "," << roleId << ") info is(" << newInfo->winnerID << "," << newInfo->loseID << "," << newInfo->result << ")" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}

	InfoLogStream << "OnPlayerRaceEnd(" <<PLAYERINFO(winner) << "," << PLAYERINFO(loser) << ") info is(" << newInfo->winnerID 
		<< "," << newInfo->loseID << "," << newInfo->result << ")" << LogStream::eos;
	m_StatusObj->OnPlayerRaceEnd(winner, loser, newInfo->result, newInfo);

	return ErrorCode::SUCCESS;
}

UInt32 ChampionService::OnPlayerRelive(UInt32 accid, UInt64 roleId, ChampionActInfoBase * info)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleId));
	if (player == nullptr)
	{
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}
	if (player->GetStatus() != CS_ENROLL)
	{
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}
	InfoLogStream << PLAYERINFO(player) << "relive success" << LogStream::eos;
	player->SetStatus(CS_SEA_SELECT_PREPARE);
	player->SyncBattleCount();
	return ErrorCode::SUCCESS;
}

UInt32 ChampionService::OnPlayerCheckRelive(UInt32 accid, UInt64 roleId, ChampionActInfoBase * info)
{
	auto player = static_cast<ChampionPlayer*>(m_PlayerMgr.FindUSPlayer(roleId));
	if (player == nullptr)
	{
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}
	if (player->GetStatus() != CS_ENROLL)
	{
		return ErrorCode::CHAMPION_PERMISSION_DENIED;
	}
	return ErrorCode::SUCCESS;
}

UInt32 ChampionService::OnPlayerObserve(UInt32 accid, UInt64 roleId, ChampionActInfoBase * info)
{
	if (info == nullptr)
	{
		return ErrorCode::CHAMPION_NO_INFO;
	}

	auto newInfo = static_cast<ObserveActInfo*>(info);

	auto race = ChampionRaceMgr::Instance()->GetRace(newInfo->msg->raceID);
	if (race == nullptr)
	{
		return ErrorCode::CHAMPION_NO_INFO;
	}
	if (race->IsEnd())
	{
		return ErrorCode::CHAMPION_RACE_ALREADY_END;
	}

	m_UserIDConnIDMap[roleId] = newInfo->connID;

	USRouter::Instance()->SendToObserve(*newInfo->msg);
	return ErrorCode::SUCCESS;
}

void ChampionService::SetStatusRace(ChampionStatus status)
{
	InfoLogStream << "set new status race,status is " << static_cast<UInt32>(status) << LogStream::eos;
#define NEW_RACE(name) new name(*this, m_PlayerMgr, m_SceneMgr);
	if (m_StatusObj != nullptr)
	{
		CL_SAFE_DELETE(m_StatusObj);
	}
	switch (status)
	{
	case CS_NULL:
	case CS_ENROLL:
	case CS_PREPARE_END:
	case CS_PREPARE_BEGIN:
	case CS_BATTLE_BEGIN:
	case CS_BATTLE_END:
	case CS_NUM:
	case CS_SEA_SELECT_PRE:
	case CS_RE_SEA_SELECT_PRE:
	case CS_64_SELECT_PRE:
	case CS_16_SELECT_PRE:
	case CS_8_SELECT_PRE:
	case CS_4_SELECT_PRE:
	case CS_2_SELECT_PRE:
	case CS_1_SELECT_PRE:
	case CS_END_SHOW:
		m_StatusObj = nullptr;
		break;
	case CS_SEA_SELECT_PREPARE:
	case CS_SEA_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(SeaStatus);
		break;
	case CS_RE_SEA_SELECT_PREPARE:
	case CS_RE_SEA_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(ReSeaStatus);
		break;
	case CS_64_SELECT_PREPARE:
	case CS_64_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(Select64Status);
		break;
	case CS_16_SELECT_PREPARE:
	case CS_16_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(Select16Status);
		break;
	case CS_8_SELECT_PREPARE:
	case CS_8_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(Select8Status);
		break;
	case CS_4_SELECT_PREPARE:
	case CS_4_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(Select4Status);
		break;
	case CS_2_SELECT_PREPARE:
	case CS_2_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(Select2Status);
		break;
	case CS_1_SELECT_PREPARE:
	case CS_1_SELECT_BATTLE:
		m_StatusObj = NEW_RACE(Select1Status);
		break;
	default:
		break;
	}
}


void ChampionService::CalcStatus(const Avalon::Time & now)
{
	bool haveStatus = false;
	for (auto& i : m_TimePoints)
	{
		if (i.startTime <= now.Sec() && i.endTime >= now.Sec())
		{
			if (m_Status != i.status)
			{
				ChangeToStatus(static_cast<ChampionStatus>(i.status));
			}
			haveStatus = true;
		}
	}

	if (!haveStatus && m_Status != CS_NULL)
	{
		//没有状态了
		ChangeToStatus(CS_NULL);
	}
}

void ChampionService::ChangeToStatus(ChampionStatus status)
{
	InfoLogStream << "champion status(" << static_cast<UInt32>(m_Status) << ") change to(" << static_cast<UInt32>(status) << ")" << LogStream::eos;
	if (status == m_Status)
	{
		ErrorLogStream << "change to same status" << LogStream::eos;
		return;
	}
	// 上一个状态结束了
	if (IsPrepareStatus(m_Status))
	{
		InfoLogStream << static_cast<UInt32>(m_Status) << " OnPrepareEnd" << LogStream::eos;
		m_StatusObj->OnPrepareEnd();
	}
	else if (IsBattleStatus(m_Status))
	{
		InfoLogStream << static_cast<UInt32>(m_Status) << " OnBattleEnd" << LogStream::eos;
		m_StatusObj->OnBattleEnd();
	}

	if (!IsLogicSameRace(m_Status, status))
	{
		SetStatusRace(status);
	}

	m_Status = status;

	// 新的状态开始了
	if (IsPrepareStatus(m_Status))
	{
		InfoLogStream << static_cast<UInt32>(m_Status) << " OnPrepareStart" << LogStream::eos;
		m_StatusObj->OnPrepareStart();
	}
	else if (IsBattleStatus(m_Status))
	{
		InfoLogStream << static_cast<UInt32>(m_Status) << " OnBattleStart" << LogStream::eos;
		m_StatusObj->OnBattleStart();
	}
	else if (m_Status == CS_ENROLL)
	{
		if (m_DBStatus != CS_ENROLL)
		{
			InfoLogStream << static_cast<UInt32>(m_Status) << " OnEnrollStart" << LogStream::eos;
			OnEnrollStart();
		}
	}

	USGameParamMgr::Instance()->SetValue(CHAMPION_STATUS_PARAM, m_Status);

	//向所有区广播状态
	CLProtocol::SceneReceiveSyncChampionStatus sync;
	sync.status = m_TimePoints[m_Status];
	BroadcastToZone(sync, ST_SCENE);
}


std::vector<ChampionScoreBattleRecord> ChampionService::m_NullScoreBattleRecord;
std::list<ChampionPlayer*>             ChampionService::m_NullGroupScoreRank;