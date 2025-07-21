#include "SceneChampionMgr.h"

#include <CLChampionProtocol.h>
#include <CLUnionServerProtocol.h>
#include <CLObserveProtocol.h>
#include <CLMoveProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <AvalonTimer.h>

#include "Player.h"
#include "SSRouter.h"
#include "PlayerMgr.h"
#include "SSApplication.h"
#include "SceneSysNotify.h"


const UInt32 ENROLL_LEVEL = 60;

#define REGISTER_STATUS_ACT_FUN(status, act, fun) \
		m_StatusActMap[status][act] = std::bind(fun, this, std::placeholders::_1, std::placeholders::_2);

bool SceneChampionMgr::Init()
{
	REGISTER_STATUS_ACT_FUN(CS_ENROLL, CA_ENROLL, &SceneChampionMgr::Enroll)
	REGISTER_STATUS_ACT_FUN(CS_SEA_SELECT_BATTLE, CA_RELIVE, &SceneChampionMgr::Relive)
	REGISTER_STATUS_ACT_FUN(CS_SEA_SELECT_BATTLE, CA_CHECK_RELIVE, &SceneChampionMgr::CheckRelive)

		
	for (UInt32 i = CS_PREPARE_BEGIN + 1; i < CS_PREPARE_END; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_JOIN_RACE, &SceneChampionMgr::JoinRace)
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_QUERY_SELF_ATTRIBUTES, &SceneChampionMgr::QuerySelfAttributes)
	}

	for (UInt32 i = CS_BATTLE_BEGIN + 1; i < CS_BATTLE_END; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_QUERY_SELF_ATTRIBUTES, &SceneChampionMgr::QuerySelfAttributes)
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_JOIN_RACE, &SceneChampionMgr::JoinRace)
	}

	for (UInt32 i = CS_8_SELECT_BATTLE; i <= CS_1_SELECT_BATTLE; ++i)
	{
		REGISTER_STATUS_ACT_FUN(static_cast<ChampionStatus>(i), CA_OBSERVE, &SceneChampionMgr::ObserveRace)
	}

	return true;
}
																					
const UInt32 STATUS_ANNOUNCEMENT[CS_NUM] = { 0, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 106, 107, 108, 109, 110, 111, 112, 0, 0, 0, 113, 114, 115, 116, 117, 118, 119, 120 };

const UInt32 END_SHOW_STATUS = 29;

const UInt32 CHAMPION_ANNOUNCEMENT_SPEC = Avalon::Time::MSECS_OF_SEC * Avalon::Time::SECS_OF_HOUR * 3;

void SceneChampionMgr::SetStatus(const ChampionStatusInfo& status)
{
	InfoLogStream << "Receive Champion Status,now Status is " << m_Status.status << " receive Status is " << status.status << LogStream::eos;
	if (m_Status != status)
	{
		m_Status = status;
		CLProtocol::SceneChampionStatusRes sync;
		sync.status = m_Status;
		Router::BroadcastToPlayer(sync);


		UInt32 tempStatus = 0;
		if (m_Status.status == CS_ENROLL || m_Status.status == CS_END_SHOW)
		{
			/**
			 *@brief 报名公告定时器
			 */
			class SendAnnouncementTimer : public Avalon::TimerHandler
			{
			public:
				SendAnnouncementTimer(ChampionStatus status, std::string server, std::string name) : m_Status(status), m_Server(server), m_Name(name) {}

				void OnTimeout(const Avalon::Time& now)
				{
					if (m_Status == CS_ENROLL)
					{
						Sys::SendAnnouncement(0, STATUS_ANNOUNCEMENT[m_Status], "");
					}
					else
					{
						Sys::SendAnnouncement(0, STATUS_ANNOUNCEMENT[m_Status], m_Server.c_str(), m_Name.c_str());
					}
					
				}

			private:
				ChampionStatus m_Status;
				std::string m_Server;
				std::string m_Name;
			};


			std::string server;
			std::string name;
			tempStatus = m_Status.status;
			if (m_Status.status == CS_END_SHOW)
			{
				tempStatus = END_SHOW_STATUS;
				ChampionTop16Player topPlayer;
				if (GetChampionTopPlayerByPos(1, topPlayer))
				{
					server = topPlayer.server;
					name = topPlayer.name;
				}
			}

			m_SendAnnouncementTimer = new SendAnnouncementTimer(static_cast<ChampionStatus>(tempStatus), server, name);
			Avalon::TimerReactor::Instance()->ScheduleAt(m_SendAnnouncementTimer, Avalon::Time(CURRENT_TIME.MSec() - CHAMPION_ANNOUNCEMENT_SPEC), CHAMPION_ANNOUNCEMENT_SPEC);
		}
		else
		{
			Avalon::TimerReactor::Instance()->RemoveHandler(m_SendAnnouncementTimer);
			m_SendAnnouncementTimer = nullptr;

			if (m_Status.status > CS_PREPARE_BEGIN && m_Status.status < CS_PREPARE_END)
			{
				tempStatus = m_Status.status;
			}
			else if (m_Status.status > CS_PRE_BEGIN && m_Status.status < CS_PRE_END)
			{
				tempStatus = m_Status.status - 60;
			}
			else
			{
				return;
			}

			Sys::SendAnnouncement(0, STATUS_ANNOUNCEMENT[tempStatus], "");
		}
	}
}

const ChampionStatusInfo& SceneChampionMgr::GetStatus() const
{
	return m_Status;
}

void SceneChampionMgr::BeginAct(ChampionAct act, Player* player, UInt64 param)
{
	auto iter = m_StatusActMap[static_cast<ChampionStatus>(m_Status.status)].find(act);
	if (iter == m_StatusActMap[static_cast<ChampionStatus>(m_Status.status)].end())
	{
		return;
	}
	iter->second(player,param);
}

void SceneChampionMgr::Enroll(Player * player, UInt64 param)
{
	CLProtocol::SceneChampionEnrollRes res;
	//if (player->GetLevel() < ENROLL_LEVEL)
	//{
	//	res.errorCode = ErrorCode::CHAMPION_ENROLL_LEVEL;
	//	player->SendProtocol(res);
	//	return;
	//}

	//auto statistic = player->GetPkStatisticMgr().FindPkStatistic(PK_SEASON_1V1);
	//if ((statistic != nullptr ? statistic->GetTotalNum() : 0) < 10)
	//{
	//	res.errorCode = ErrorCode::CHAMPION_ENROLL_PK;
	//	player->SendProtocol(res);
	//	return;
	//}

	//if (!player->CheckRmMoney(ST_ITEM_GOLD, CHAMPION_ENROLL_GLOD))
	//{
	//	res.errorCode = ErrorCode::CHAMPION_ENROLL_GOLD;
	//	player->SendProtocol(res);
	//	return;
	//}


	InfoLogStream << "player begin enroll " << PLAYERINFO(player) << LogStream::eos;

	CLProtocol::UnionCheckChampionEnrollReq req(player->GetID(), player->GetAccID());
	Router::SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::OnEnrollSuccess(Player * player)
{
	player->RemoveMoney(GetReason(SOURCE_CHAMPION_ENROLL).c_str(), ST_ITEM_GOLD, CHAMPION_ENROLL_GLOD);
	InfoLogStream << "OnEnrollSuccess " << PLAYERINFO(player) << LogStream::eos;
	CLProtocol::UnionServerAddPlayerReq req;
	req.funType = USFT_CHAMPION;
	req.accId = player->GetAccID();
	req.sceneNode = NODE_ID;
	req.worldNode = Router::GetWorldNodeID();
	req.roleId = player->GetID();
	req.name = player->GetName();
	req.occu = player->GetOccu();
	req.level = player->GetLevel();
	req.vipLvl = player->GetVipLvl();
	req.guildId = player->GetGuildId();
	req.awaken = player->GetAwaken();
	req.server = SSApplication::Instance()->GetProperties()->GetProperty("Server", "name").GetString();
	Router::SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::Relive(Player * player, UInt64 param)
{
	InfoLogStream << "player:" << PLAYERINFO(player) << " begin champion relive" << LogStream::eos;
	CLProtocol::SceneChampionReliveRes res;
	if (param != ErrorCode::SUCCESS)
	{
		res.errorCode = param;
		player->SendProtocol(res);
	}

	bool ok = false;
	auto reason = GetReason(SOURCE_CHAMPION_ENROLL);
	if (player->GetItemNum(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_ITEM_ID)) > 0)
	{
		player->RemoveItem(reason.c_str(), SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_ITEM_ID), 1);
		ok = true;
	}
	else
	{
		if (player->CheckRmMoney(ST_ITEM_POINT, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_DIANQUAN_NUM)))
		{
			player->RemoveMoney(reason.c_str(), ST_ITEM_POINT, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_DIANQUAN_NUM));
			ok = true;
		}
	}

	res.errorCode = ErrorCode::ITEM_DATA_INVALID;

	InfoLogStream << "player:" << PLAYERINFO(player) << " reduce champion relive item result is" << ok << LogStream::eos;

	if (ok)
	{
		res.errorCode = ErrorCode::SUCCESS;
		CLProtocol::SceneChampionReliveReq req;
		player->SendToUnion(USFT_CHAMPION, req);
	}
	else
	{
		player->SendProtocol(res);
	}
}

void SceneChampionMgr::CheckRelive(Player * player, UInt64 param)
{
	InfoLogStream << "player:" << PLAYERINFO(player) << " begin check relive" << LogStream::eos;
	if (!IsPlayerInChampion(player->GetID()))
	{
		ErrorLogStream << PLAYERINFO(player) << "request relive but not in champion" << LogStream::eos;
		CLProtocol::SceneChampionReliveRes res;
		res.errorCode = ErrorCode::CHAMPION_NO_INFO;
		player->SendProtocol(res);
		return;
	}

	CLProtocol::UnionChampionReliveReq req;
	player->SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::JoinRace(Player * player, UInt64 param)
{
	InfoLogStream << PLAYERINFO(player) << " begin JoinRace" << LogStream::eos;
	if (IsPlayerInChampion(player->GetID()))
	{
		ErrorLogStream << PLAYERINFO(player) << "already in champion race" << LogStream::eos;
		return;
	}

	CLProtocol::UnionChampionJoinPrepareReq req;
	req.accId = player->GetAccID();
	req.sceneNode = NODE_ID;
	req.worldNode = Router::GetWorldNodeID();
	req.roleId = player->GetID();
	req.name = player->GetName();
	req.occu = player->GetOccu();
	req.level = player->GetLevel();
	req.vipLvl = player->GetVipLvl();
	req.guildId = player->GetGuildId();
	req.awaken = player->GetAwaken();
	req.avatar = player->GetAvatar();
	req.server = SSApplication::Instance()->GetProperties()->GetProperty("Server", "name").GetString();
	Router::SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::QuerySelfAttributes(Player * player, UInt64 param)
{
	CLProtocol::UnionChampionPlayerStatusReq req;
	req.roleID = player->GetID();
	req.param = param;
	player->SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::ObserveRace(Player * player, UInt64 raceID)
{
	if (IsPlayerInChampion(player->GetID()) || raceID == 0 || player == nullptr)
	{
		CLProtocol::SceneChampionObserveRes res;
		res.raceID = 0;
		player->SendProtocol(res);
		return;
	}
	
	CLProtocol::UnionChampionObserveReq req;
	req.raceID = raceID;
	req.roleID = player->GetID();
	Router::SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::OnPlayerJoinRaceSuccess(Player * player, UInt32 dstSceneId, UInt32 dstDoorId)
{
	InfoLogStream << PLAYERINFO(player) << " OnPlayerJoinRaceSuccess" << LogStream::eos;
	CLPosition topos;
	m_PlayeInChampion.insert(player->GetID());
	if (!player->ChangeScene(dstSceneId, topos, player->GetDir(), true, dstDoorId))
	{
		m_PlayeInChampion.erase(player->GetID());
		OnPlayerJoinRaceFail(player, dstSceneId, dstDoorId);
		return;
	}
	InfoLogStream << PLAYERINFO(player) << " OnPlayerJoinRaceSuccess success,set player in champion" << LogStream::eos;
	
}

void SceneChampionMgr::OnPlayerJoinRaceFail(Player * player, UInt32 dstSceneId, UInt32 dstDoorId)
{
	InfoLogStream << "player(" << player->GetID() << ") request change scene to " << dstSceneId
		<< " target door(" << dstDoorId << ") failed." << LogStream::eos;

	CLProtocol::SceneNotifyEnterScene notify;
	notify.result = ErrorCode::SCENE_CHANGE_ERROR;
	player->SendProtocol(notify);

	// 客户端一定要等这个消息，发个空的下去
	CLProtocol::SceneSyncSceneObject a;
	player->SendProtocol(a);

	OnPlayerDisconnect(player);
}

void SceneChampionMgr::OnPlayerDisconnect(Player* player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "player is null" << LogStream::eos;
		return;
	}
	CLProtocol::GateNotifyLeaveConnect req;
	req.roleId = player->GetID();
	InfoLogStream << PLAYERINFO(player) << "request disconnect" << LogStream::eos;
	m_PlayeInChampion.erase(player->GetID());
	Router::SendToUnion(USFT_CHAMPION, req);

	Avalon::Packet* packet = player->MakeDelObjectPacket();
	if (packet)
	{
		Router::SendToUnion(USFT_CHAMPION ,packet);
		Avalon::Packet::Destroy(packet);
	}
}

void SceneChampionMgr::OnPlayerBet(Player* player, CLProtocol::SceneChampionGambleReq& req)
{
	InfoLogStream << PLAYERINFO(player) << " player begin bet,betinfo is " << req.id << "|" << req.option << "|" << req.coin << LogStream::eos;
	if (player->GetChampionCoin() < req.coin)
	{
		InfoLogStream << PLAYERINFO(player) << "player champion coin less then need " << player->GetChampionCoin() << "|" << req.coin << LogStream::eos;
		return;
	}

	bool boFind = false;
	for (auto& info : m_GambleInfoVec)
	{
		if (info.id == req.id)
		{
			if (CURRENT_TIME.Sec() > info.endTime || CURRENT_TIME.Sec() < info.startTime)
			{
				return;
			}
			for (auto& option : info.optionVec)
			{
				if (option.option == req.option)
				{
					boFind = true;
					break;
				}
			}
			if (boFind)
			{
				break;
			}
		}
	}
	if (boFind)
	{
		auto reason = GetReason(SOURCE_CHAMPION_ENROLL);
		player->RemoveMoney(reason.c_str(), ST_CHAMPION_COIN, req.coin);
		req.roleID = player->GetID();
		req.accid = player->GetAccID();
		InfoLogStream << "player:" << PLAYERINFO(player) << " start bet, betinfo is " << req.id << "|" << req.option << "|" << req.coin << LogStream::eos;
		Router::SendToUnion(USFT_CHAMPION, req);
	}
}

void SceneChampionMgr::OnPlayerBetRet(CLProtocol::SceneChampionGambleUnionRes& req)
{
	InfoLogStream << "role:" << req.roleID << "receive union bet ret,betinfo is " << req.res.id << "|" << req.res.option << "|" << req.res.num << "errorcode is " << req.res.errorCode <<  LogStream::eos;
	auto player = PlayerMgr::Instance()->FindNormalPlayer(req.roleID);
	if (player == nullptr)
	{
		ErrorLogStream << "cant find player bet" << LogStream::eos;
		return;
	}
	if (req.res.errorCode != ErrorCode::SUCCESS)
	{
		auto reason = GetReason(SOURCE_CHAMPION_ENROLL);
		player->AddMoneyItem(reason.c_str(), ST_CHAMPION_COIN, req.res.num);
		InfoLogStream << PLAYERINFO(player) << "bet failed,retrurn coin" << LogStream::eos;
	}
	player->SendProtocol(req.res);
}

bool SceneChampionMgr::IsPlayerInChampion(const UInt64 roleId)
{
	return m_PlayeInChampion.count(roleId) > 0;
}

void SceneChampionMgr::OnPlayerEnterChampionScene(Player * player, UInt32 dstSceneId, UInt32 dstDoorId)
{
	InfoLogStream << "player(" << player->GetID() << ") request enter ChampionScene scene start" << LogStream::eos;
	if (IsPlayerInChampion(player->GetID()))
	{
		ErrorLogStream << PLAYERINFO(player) << "OnPlayerEnterChampionScene but already in champion" << LogStream::eos;
		OnPlayerJoinRaceFail(player, dstSceneId, dstDoorId);
		return;
	}

	CLProtocol::UnionChampionJoinPrepareReq req;
	req.accId = player->GetAccID();
	req.sceneNode = NODE_ID;
	req.worldNode = Router::GetWorldNodeID();
	req.roleId = player->GetID();
	req.name = player->GetName();
	req.occu = player->GetOccu();
	req.level = player->GetLevel();
	req.vipLvl = player->GetVipLvl();
	req.guildId = player->GetGuildId();
	req.awaken = player->GetAwaken();
	req.dstSceneId = dstSceneId;
	req.dstDoorId = dstDoorId;
	req.avatar = player->GetAvatar();
	req.server = SSApplication::Instance()->GetProperties()->GetProperty("Server", "name").GetString();
	Router::SendToUnion(USFT_CHAMPION, req);
	InfoLogStream << "player(" << player->GetID() << ") request enter ChampionScene scene success" << LogStream::eos;
	
}

void SceneChampionMgr::OnPlayerLeaveChampionScene(Player * player, UInt32 dstSceneId, UInt32 dstDoorId)
{
	InfoLogStream << PLAYERINFO(player) << ")request leave ChampionScene scene" << LogStream::eos;
	if (!IsPlayerInChampion(player->GetID()))
	{
		ErrorLogStream << PLAYERINFO(player) << "OnPlayerLeaveChampionScene but not in champion" << LogStream::eos;
		return;
	}
	
	OnPlayerDisconnect(player);
}

void SceneChampionMgr::OnUnionSyncTop16Table(std::vector<ChampionTop16Player>& vec)
{
	m_Top16PlayerVec = vec;
}

void SceneChampionMgr::OnUnionSyncGambleInfo(std::vector<ChampionGambleInfo>& vec)
{
	m_GambleInfoVec = vec;
}

void SceneChampionMgr::OnUnionSyncBattleRecord(Avalon::Packet * packet)
{
	return;
	UInt32 groupID = 0;
	UInt32 order = 0;
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	std::vector<UInt32> changedGroup;
	try
	{
		stream & groupID;
		while (groupID != 0)
		{
			stream & order;
			m_BattleRecordMap[groupID][order].Decode(stream);
			changedGroup.push_back(groupID);
		}
	}
	catch (Avalon::NetSerializeException* e)
	{
		ErrorLogStream << "parse SceneChampionRace info error!" << LogStream::eos;
		return;
	}
	for (auto groupID : changedGroup)
	{
		for (auto& recordIter : m_BattleRecordMap[groupID])
		{
			auto record = recordIter.second;
			m_GroupStatus[groupID].groupID = groupID;
			if (m_GroupStatus[groupID].roleA == 0)
			{
				m_GroupStatus[groupID].roleA = record.roleID1;
			}
			if (m_GroupStatus[groupID].roleB == 0)
			{
				m_GroupStatus[groupID].roleB = record.roleID2;
			}
			if (record.winnerID == m_GroupStatus[groupID].roleA)
			{
				++m_GroupStatus[groupID].scoreA;
			}
			if (record.winnerID == m_GroupStatus[groupID].roleB)
			{
				++m_GroupStatus[groupID].scoreB;
			}
		}
		m_GroupStatus[groupID].status = 1;
		if (m_GroupStatus[groupID].scoreA >= 3 || m_GroupStatus[groupID].scoreB >= 3)
		{
			m_GroupStatus[groupID].status = 2;
		}
	}

	//for (auto& groupIter : m_BattleRecordMap)
	//{
	//	for (auto& recordIter : groupIter.second)
	//	{
	//		auto record = recordIter.second;
	//		m_GroupStatus[record.groupID].groupID = record.groupID;
	//		if (m_GroupStatus[record.groupID].roleA == 0)
	//		{
	//			m_GroupStatus[record.groupID].roleA = record.roleID1;
	//		}
	//		if (m_GroupStatus[record.groupID].roleB == 0)
	//		{
	//			m_GroupStatus[record.groupID].roleB = record.roleID2;
	//		}
	//		if (record.winnerID == m_GroupStatus[record.groupID].roleA)
	//		{
	//			++m_GroupStatus[record.groupID].scoreA;
	//		}
	//		if (record.winnerID == m_GroupStatus[record.groupID].roleB)
	//		{
	//			++m_GroupStatus[record.groupID].scoreB;
	//		}
	//	}
	//	m_GroupStatus[groupIter.first].status = 1;
	//	if (m_GroupStatus[groupIter.first].scoreA >= 3 || m_GroupStatus[groupIter.first].scoreB >= 3)
	//	{
	//		m_GroupStatus[groupIter.first].status = 2;
	//	}
	//}
}

void SceneChampionMgr::OnUnionSyncAllGroupInfo(CLProtocol::SceneChampionAllGroupRes & req)
{
	if (m_GroupStatus.size() > 0)
	{
		m_GroupStatus.clear();
	}
	for (auto& group : req.groups)
	{
		if (m_GroupStatus.count(group.groupID) != 0)
		{
			ErrorLogStream << "union sync two same group id:" << group.groupID << LogStream::eos;
			continue;
		}
		m_GroupStatus[group.groupID] = group;
	}
	BroadcastAllGroupInfo();
}

void SceneChampionMgr::OnUnionSyncOneGroupInfo(CLProtocol::SceneChampionGroupStatusSync & sync)
{
	if (m_GroupStatus.count(sync.group.groupID) == 0)
	{
		m_GroupStatus[sync.group.groupID] = sync.group;
		BroadcastOneGroupInfo(sync.group);
		return;
	}

	if (m_GroupStatus[sync.group.groupID].IsChanged(sync.group))
	{
		m_GroupStatus[sync.group.groupID] = sync.group;
		BroadcastOneGroupInfo(sync.group);
	}
}

void SceneChampionMgr::BroadcastOneGroupInfo(ChampionGroupStatus& group)
{
	CLProtocol::SceneChampionGroupStatusSync sync;
	sync.group = group;
	Router::BroadcastToPlayer(sync);
}

void SceneChampionMgr::BroadcastAllGroupInfo()
{
	CLProtocol::SceneChampionAllGroupRes res;
	auto& groups = GetAllGroupInfo();
	for (auto& groupIter : groups)
	{
		res.groups.push_back(groupIter.second);
	}
	Router::BroadcastToPlayer(res);
}

std::vector<ChampionTop16Player>& SceneChampionMgr::GetTop16Table()
{
	return m_Top16PlayerVec;
}

void SceneChampionMgr::OnEnterScene(Player * player)
{
	InfoLogStream << PLAYERINFO(player) << "OnEnterScene" << LogStream::eos;
	if (!IsPlayerInChampion(player->GetID()))
	{
		ErrorLogStream << PLAYERINFO(player) << "OnEnterScene but not in champion" << LogStream::eos;
		return;
	}

	Avalon::Packet* packet = player->MakeSyncObjectPacket(true);
	if (packet)
	{
		Router::SendToUnion(USFT_CHAMPION, packet);
		Avalon::Packet::Destroy(packet);
	}
}

std::map<UInt32, ChampionGroupStatus>& SceneChampionMgr::GetAllGroupInfo()
{
	return m_GroupStatus;
}

std::vector<ChampionGambleInfo>& SceneChampionMgr::GetAllGambleInfo()
{
	return m_GambleInfoVec;
}

void SceneChampionMgr::OnPlayerOnline(Player * player)
{
	CLProtocol::SceneChampionTotalStatusReq req;
	req.accid = player->GetAccID();
	player->SendToUnion(USFT_CHAMPION, req);
}

void SceneChampionMgr::OnPlayerOffline(Player * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "OnPlayerOffline player is null" << LogStream::eos;
		return;
	}
	if (IsPlayerInChampion(player->GetID()))
	{
		OnPlayerDisconnect(player);
	}
}

bool SceneChampionMgr::GetChampionTopPlayerByPos(UInt32 pos, ChampionTop16Player& outPlayer)
{
	for (auto& player : m_Top16PlayerVec)
	{
		if (player.pos == pos)
		{
			outPlayer = player;
			return true;
		}
	}
	return false;
}
