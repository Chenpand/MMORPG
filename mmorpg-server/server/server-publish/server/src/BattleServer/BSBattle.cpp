#include "BSBattle.h"
#include "BSNetwork.h"
#include "BSBattleMgr.h"
#include "BattleSceneMgr.h"
#include "CLErrorCode.h"
#include "BSSortListMgr.h"

#include <CLObjectDefine.h>
#include <CLChiJiTimeDataEntry.h>
#include <CLChiJiScoreDataEntry.h>
#include <CLChiJiShopTimeDataEntry.h>
#include <CLHonor.h>
#include <CLSceneDefine.h>
#include <CLBattleDefine.h>

BSBattle::BSBattle()
{
	m_ID = 0;
	m_nodeID = 0;
	m_waveID = 0;
	m_shopWave = 0;
	m_beginTime = Avalon::Time::CurrentTime();
	m_canPK = false;
}

BSBattle::~BSBattle()
{
	for (BSBattleTeamList::iterator iter = m_BattleTeams.begin(); iter != m_BattleTeams.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	m_BattleTeams.clear();
}

void BSBattle::OnTick(const Avalon::Time& now)
{
	if (m_beginTime >= now)
	{
		return;
	}

	if ((now - m_beginTime).Sec() > BSBattleMgr::Instance()->GetBattleTimeOut())
	{
		InfoLogStream << "battle:" << m_ID << ", time out" << LogStream::eos;
		
		// 战场超时，把所有人存活的踢出
		OnBattleTimeOut();

		CLProtocol::SceneBattleDestroyBattle req;
		req.battleType = BTY_CHIJI;
		req.battleID = GetId();
		BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), req);

		BSBattleMgr::Instance()->DelBattle(this);
		return;
	}

	//设置开始时间
	if (m_waveID <= 0)
	{
		m_beginTime = now;
	}

	class ChijiTimeDataVisitor : public Avalon::DataEntryVisitor<ChiJiTimeDataEntry>
	{
	public:
		ChijiTimeDataVisitor(UInt32 waveID, const Avalon::Time& beginTime, const Avalon::Time& now) : m_waveID(waveID), m_beginTime(beginTime), m_nowTime(now) {}

		bool operator()(ChiJiTimeDataEntry* dataEntry)
		{
			if (m_waveID >= dataEntry->id)
			{
				return true;
			}

			Avalon::Time intervalTime = m_nowTime - m_beginTime;
			UInt32 interval = intervalTime.Sec();
			if (interval >= dataEntry->startTime)
			{
				m_waveID = dataEntry->id;
				return true;
			}

			return true;
		}

	public:
		UInt32 m_waveID;
		Avalon::Time m_beginTime;
		Avalon::Time m_nowTime;
	};

	ChijiTimeDataVisitor visitor(m_waveID, m_beginTime, now);
	ChiJiTimeDataEntryMgr::Instance()->Visit(visitor);

	for (UInt32 key = m_waveID + 1; key <= visitor.m_waveID; ++key)
	{
		ChiJiTimeDataEntry* dataEntry = ChiJiTimeDataEntryMgr::Instance()->FindEntry(key);
		if (!dataEntry)
		{
			break;
		}

		if (dataEntry->chickenType == CT_START_PK)
		{
			m_canPK = true;
		}

		CLProtocol::SceneBattleNotifyWaveInfo notify;
		notify.battleID = GetId();
		notify.waveID = key;
		BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), notify);
	}
	m_waveID = visitor.m_waveID;

	// 刷吃鸡商店
	UInt32 interval = (now - m_beginTime).Sec();
	UInt32 shopWave = ChiJiShopTimeDataEntryMgr::Instance()->GetShop(m_shopWave, interval);
	if (shopWave != 0 && shopWave != m_shopWave)
	{
		m_shopWave = shopWave;

		CLProtocol::BattleShopNotify notify;
		notify.battleId = GetId();
		notify.shopWave = m_shopWave;
		BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), notify);
	}

	UInt32 count = GetSurvivalNum();
	if (count <= 1)
	{
		for (auto iter : m_BattleTeams)
		{
			if (iter->GetStatus() != BTS_OUT)
			{
				const VBSTeamMember& members = iter->GetTeamMembers();
				for (auto member : members)
				{
					if (member.GetStatus() != BPS_OUT)
					{
						UInt32 score = ChiJiScoreDataEntryMgr::Instance()->GetScoreByRank(1);
						ScoreSortList(&member, score);

						UInt32 honorRank = member.m_IsPk ? 1 : 0;
						UInt32 honor = CLHonor::PvpHonorReward(PK_3V3_CHIJI, 1, honorRank, member.m_kills);

						CLProtocol::SceneBattleBalanceEnd notify;
						notify.roleId = member.m_PlayerId;
						notify.rank = 1;
						notify.playerNum = GetPlayerNum();
						notify.kills = member.m_kills;
						notify.survivalTime = (Avalon::Time::CurrentTime() - GetBeginTime()).Sec();
						notify.score = score;
						notify.battleID = GetId();
						notify.getHonor = honor;
						member.SendToSelfScene(notify);

						//发回battle scene用于些UDP埋点日志
						SendBalanceLog(notify, member.m_PlayerId, CHIJI_DEAD_REASON_BALANCE);

						//统计最佳排名
						BSSortListMgr::Instance()->SetPlayerBestRank(member.m_PlayerId, member.m_ConnId, notify.rank);
						break;
					}
				}

				break;
			}
		}

		CLProtocol::SceneBattleDestroyBattle req;
		req.battleType = BTY_CHIJI;
		req.battleID = GetId();
		BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), req);

		BSBattleMgr::Instance()->DelBattle(this);
		return;
	}

	//如果30秒了还没从MATCH切换到PK状态，那么就解散
	for (auto iter : m_BattleTeams)
	{
		for (auto member : iter->m_Members)
		{
			if (member.GetStatus() == BPS_MATCH && (now.Sec() - member.m_matchTimestamp) > 30)
			{
				member.SetStatus(BPS_ALIVE);
				member.m_matchTimestamp = 0;
			}
		}
	}
}

void BSBattle::AddTeam(BSTeam* team)
{
	if (!team)
	{
		return;
	}

	m_BattleTeams.push_back(team);
	const VBSTeamMember& members = team->GetTeamMembers();
	for (auto member : members)
	{
		if (member.m_robot)
		{
			continue;
		}

		BSBattleMgr::Instance()->OnPlayerEnterBattleScene(member.m_PlayerId, m_nodeID);
	}
}

UInt32 BSBattle::PkSomeOne(UInt64 roleId, UInt64 dstId, UInt32 dungeonId)
{
	if (!m_canPK)
	{
		return ErrorCode::BATTLE_TARGET_FAILED;
	}

	BSTeamMember* myPlayer = NULL;
	BSTeamMember* dstPlayer = NULL;

	BSTeam* myTeam = NULL;
	BSTeam* dstTeam = NULL;
	UInt32 now = Avalon::Time::CurrentTime().Sec();

	for (auto iter : m_BattleTeams)
	{
		if (!myPlayer)
		{
			myPlayer = iter->FindMember(roleId);
			if (myPlayer != NULL)
			{
				InfoLogStream << "player:" << roleId << ", status:" << UInt32(myPlayer->GetStatus()) << ", now:" << now << ", protect:" << myPlayer->m_protectTimestamp << LogStream::eos;
			}

			if (myPlayer != NULL && now <= myPlayer->m_protectTimestamp)
			{
				return ErrorCode::BATTLE_SELF_INVINCIBLE;
			}

			if (myPlayer != NULL && myPlayer->GetStatus() == BPS_PK)
			{
				return ErrorCode::BATTLE_SELF_PKING;
			}

			if (myPlayer != NULL && myPlayer->GetStatus() == BPS_ALIVE && now > myPlayer->m_protectTimestamp)
			{
				myTeam = iter;
			}
		}

		if (!dstPlayer)
		{
			dstPlayer = iter->FindMember(dstId);
			if (dstPlayer != NULL)
			{
				InfoLogStream << "player:" << dstId << ", status:" << UInt32(dstPlayer->GetStatus()) << ", now:" << now << ", protect:" << dstPlayer->m_protectTimestamp << LogStream::eos;
			}

			if (dstPlayer != NULL && now <= dstPlayer->m_protectTimestamp)
			{
				return ErrorCode::BATTLE_TARGET_INVINCIBLE;
			}

			if (dstPlayer != NULL && dstPlayer->GetStatus() == BPS_PK)
			{
				return ErrorCode::BATTLE_TARGET_PKING;
			}

			if (dstPlayer != NULL && dstPlayer->GetStatus() == BPS_ALIVE && now > dstPlayer->m_protectTimestamp)
			{
				dstTeam = iter;
			}
		}
	}

	if (myPlayer != NULL && dstPlayer != NULL && myPlayer != dstPlayer && myTeam != NULL && dstTeam != NULL && myTeam != dstTeam)
	{
		myPlayer->SetStatus(BPS_MATCH);
		dstPlayer->SetStatus(BPS_MATCH);

		myPlayer->m_matchTimestamp = now;
		dstPlayer->m_matchTimestamp = now;

		VBSTeam vTeam;
		vTeam.push_back(myTeam);
		vTeam.push_back(dstTeam);
		myTeam->SetStatus(BTS_MATCH);
		dstTeam->SetStatus(BTS_MATCH);
		if ( !BSBattleMgr::Instance()->OnMatchSuccess(GetId(), dungeonId, vTeam))
		{
			myPlayer->SetStatus(BPS_ALIVE);
			dstPlayer->SetStatus(BPS_ALIVE);
			myTeam->SetStatus(BTS_ALIVE);
			dstTeam->SetStatus(BTS_ALIVE);
			return ErrorCode::BATTLE_TARGET_FAILED;
		}

		return ErrorCode::SUCCESS;
	}

	InfoLogStream << "can't pk!" << LogStream::eos;
	return ErrorCode::BATTLE_TARGET_FAILED;
}

UInt32 BSBattle::LockSomeOne(UInt64 roleId, UInt64 dstId)
{
	//if (!m_canPK)
	//{
	//	return ErrorCode::TEAM_ERROR;
	//}

	//BSTeamMember* myPlayer = NULL;
	//BSTeamMember* dstPlayer = NULL;

	//BSTeam* myTeam = NULL;
	//BSTeam* dstTeam = NULL;

	//UInt32 now = Avalon::Time::CurrentTime().Sec();

	//for (auto iter : m_BattleTeams)
	//{
	//	if (!myPlayer)
	//	{
	//		myPlayer = iter->FindMember(roleId);
	//		if (myPlayer != NULL && myPlayer->GetStatus() == BPS_ALIVE && now > myPlayer->m_protectTimestamp)
	//		{
	//			myTeam = iter;
	//		}
	//	}

	//	if (!dstPlayer)
	//	{
	//		dstPlayer = iter->FindMember(dstId);
	//		if (dstPlayer != NULL && dstPlayer->GetStatus() == BPS_ALIVE && now > dstPlayer->m_protectTimestamp)
	//		{
	//			dstTeam = iter;
	//		}
	//	}
	//}

	//if (myPlayer != NULL && dstPlayer != NULL && myPlayer != dstPlayer && myTeam != NULL && dstTeam != NULL && myTeam != dstTeam)
	//{
	//	CLProtocol::BattleLockSomeOneRes res;
	//	res.roleId = roleId;
	//	res.dstId = dstId;
	//	res.retCode = ErrorCode::SUCCESS;
	//	myPlayer->SendProtocol(res);
	//	dstPlayer->SendProtocol(res);

	//	return ErrorCode::SUCCESS;
	//}

	return ErrorCode::TEAM_ERROR;
}

void BSBattle::NotifySwitchBattleScene()
{
	CLProtocol::SceneBattleMatchSync sync;
	sync.battleID = GetId();
	sync.suvivalNum = GetSurvivalNum();

	for (auto& iter : m_BattleTeams)
	{
		for (auto& it : iter->GetTeamMembers())
		{
			if (it.m_robot)
			{
				continue;
			}

			CLProtocol::PlayerSubjectInfo tmpPlayer;
			tmpPlayer.m_AccId = it.m_AccId;
			tmpPlayer.m_PlayerId = it.m_PlayerId;
			tmpPlayer.m_PlayerName = it.m_PlayerName;
			tmpPlayer.m_PlayerOccu = it.m_PlayerOccu;

			sync.players.push_back(tmpPlayer);
		}
	}

	sync.sceneNodeID = GetSceneNodeID();
	//InfoLogStream << "sync dst scene node id:" << sync.sceneNodeID << LogStream::eos;

	NotifyMsgToAllPlayer(sync);
}

void BSBattle::NotifyCreateSceneBattle()
{
	CLProtocol::SceneBattleCreateBattleReq req;
	req.battleType = BTY_CHIJI;
	req.battleID = GetId();
	for (auto& iter : m_BattleTeams)
	{
		for (auto& it : iter->GetTeamMembers())
		{
			if (it.m_robot)
			{
				continue;
			}

			req.playerIDs.push_back(it.m_PlayerId);
		}
	}

	BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), req);
}

void BSBattle::OnRaceEnd()
{
	CLProtocol::BattleNotifyBattleInfo notify;
	notify.battleID = GetId();
	notify.playerNum = 0;

	for (auto iter : m_BattleTeams)
	{
		for (auto player : iter->m_Members)
		{
			if (player.GetStatus() != BPS_OUT)
			{
				++notify.playerNum;
			}
		}
	}
	
	for (auto iter : m_BattleTeams)
	{
		iter->NotifyMsgToAll(notify);
	}
}

void BSBattle::OnSomeoneDead(UInt64 playerID, UInt64 killerID, UInt32 reason)
{
	UInt32 kills = 0;

	for (auto iter : m_BattleTeams)
	{
		BSTeamMember* player = iter->FindMember(playerID);
		if (player)
		{
			if (player->GetStatus() == BPS_OUT)
			{
				continue;
			}

			InfoLogStream << "battle:" << m_ID << ", player:" << playerID << ", killer:" << killerID << ", reason:" << reason << LogStream::eos;

			kills = player->m_kills;

			//目前都是单人队伍，1V1的约架模式
			iter->SetStatus(BTS_OUT);
			player->SetStatus(BPS_OUT);

			UInt32 rank = GetSurvivalNum() + 1;
			if (rank <= 1)
			{
				//这里特殊处理，都死光了，大家都是第2名
				rank = 2;
			}

			UInt32 score = ChiJiScoreDataEntryMgr::Instance()->GetScoreByRank(rank);
			ScoreSortList(player, score);

			UInt32 honorRank = player->m_IsPk ? rank : 0;
			UInt32 honor = CLHonor::PvpHonorReward(PK_3V3_CHIJI, 1, honorRank, player->m_kills);

			CLProtocol::SceneBattleBalanceEnd notify;
			notify.roleId = player->m_PlayerId;
			notify.rank = rank;
			notify.playerNum = GetPlayerNum();
			notify.kills = player->m_kills;
			notify.survivalTime = (Avalon::Time::CurrentTime() - GetBeginTime()).Sec();
			notify.score = score;
			notify.battleID = GetId();
			notify.getHonor = honor;
			player->SendToSelfScene(notify);

			//发回battle scene用于些UDP埋点日志
			SendBalanceLog(notify, playerID, reason);

			//统计最佳排名
			BSSortListMgr::Instance()->SetPlayerBestRank(player->m_PlayerId, player->m_ConnId, notify.rank);
			break;
		}
	}

	CLProtocol::SceneBattleNotifySomeoneDead sync;
	sync.battleID = m_ID;
	sync.playerID = playerID;
	sync.killerID = killerID;
	sync.reason = reason;
	sync.kills = kills;
	sync.suvivalNum = GetSurvivalNum();

	BSTeamMember* killer = GetPlayer(killerID);
	if (killer)
	{
		InfoLogStream << "battle:" << m_ID << ", player:" << playerID << ", killer:" << killerID << ", kills:" << killer->m_kills << LogStream::eos;
		sync.kills = killer->m_kills;
	}

	NotifyMsgToAllPlayer(sync);
}

void BSBattle::OnBattleTimeOut()
{
	UInt32 survivalNum = GetSurvivalNum();
	UInt32 score = ChiJiScoreDataEntryMgr::Instance()->GetScoreByRank(survivalNum);
	for (auto& iter : m_BattleTeams)
	{
		if (iter->GetStatus() == BTS_OUT)
			continue;
		
		for (auto& member : iter->m_Members)
		{
			if (member.GetStatus() == BPS_OUT)
				continue;

			iter->SetStatus(BTS_OUT);
			member.SetStatus(BPS_OUT);

			ScoreSortList(&member, score);

			UInt32 honorRank = member.m_IsPk ? survivalNum : 0;
			UInt32 honor = CLHonor::PvpHonorReward(PK_3V3_CHIJI, 1, honorRank, member.m_kills);

			CLProtocol::SceneBattleBalanceEnd notify;
			notify.roleId = member.m_PlayerId;
			notify.rank = survivalNum;
			notify.playerNum = GetPlayerNum();
			notify.kills = member.m_kills;
			notify.survivalTime = (Avalon::Time::CurrentTime() - GetBeginTime()).Sec();
			notify.score = score;
			notify.battleID = GetId();
			notify.getHonor = honor;
			member.SendToSelfScene(notify);

			//发回battle scene用于些UDP埋点日志
			SendBalanceLog(notify, member.m_PlayerId, CHIJI_DEAD_REASON_TIMEOUT);

			//统计最佳排名
			BSSortListMgr::Instance()->SetPlayerBestRank(member.m_PlayerId, member.m_ConnId, notify.rank);
		}
	}
}

UInt32 BSBattle::GetPlayerNum()
{
	UInt32 playerNum = 0;

	for (auto iter : m_BattleTeams)
	{
		playerNum += iter->GetSize();
	}

	return playerNum;
}

UInt32 BSBattle::GetSurvivalNum()
{
	UInt32 playerNum = 0;

	for (auto iter : m_BattleTeams)
	{
		for (auto player : iter->m_Members)
		{
			if (player.GetStatus() != BPS_OUT)
			{
				++playerNum;
			}
		}
	}

	return playerNum;
}

BSTeamMember* BSBattle::GetPlayer(UInt64 playerID)
{
	for (auto iter : m_BattleTeams)
	{
		BSTeamMember* player = iter->FindMember(playerID);
		if (player)
		{
			return player;
		}
	}

	return NULL;
}

void BSBattle::NotifyMsgToAllPlayer(Avalon::Protocol& protocol, bool includeDead)
{
	InfoLogStream << "send notify to player, msg id:" << protocol.GetProtocolID() << ", battle:" << m_ID << LogStream::eos;

	for (auto iter : m_BattleTeams)
	{
		for (auto player : iter->m_Members)
		{
			if (player.BeRobot())
			{
				continue;
			}

			if (player.GetStatus() == BPS_OUT && !includeDead)
			{
				continue;
			}

			if (!BSBattleMgr::Instance()->PlayerOnline(player.m_PlayerId))
			{
				InfoLogStream << "player is off line, send notify to player:" << player.m_PlayerId << ", msg id:" << protocol.GetProtocolID() << ", battle:" << m_ID << LogStream::eos;
				continue;
			}

			InfoLogStream << "send notify to player:" << player.m_PlayerId << ", msg id:" << protocol.GetProtocolID() << ", battle:" << m_ID << LogStream::eos;
			player.SendProtocol(protocol);
		}
	}
}

void BSBattle::ScoreSortList(BSTeamMember* member, UInt32 score)
{
	if (NULL == member || member->m_robot)
		return;

	InfoLogStream << "add chi ji score playerid:" << member->m_PlayerId << " playername:" << member->m_PlayerName
		<< " score:" << score << LogStream::eos;

	ChijiSortListEntry sortEntry;
	sortEntry.sortType = 250;
	sortEntry.serverName = "";
	sortEntry.playerId = member->m_PlayerId;
	sortEntry.playerName = member->m_PlayerName;
	sortEntry.score = score;
	sortEntry.connId = member->m_ConnId;
	sortEntry.playerOccu = member->m_PlayerOccu;

	BSSortListMgr::Instance()->UpdateSortList(sortEntry);
}

void BSBattle::UpdatePlayerState(UInt64 roleID, UInt32 playerState)
{
	BSTeamMember* member = GetPlayer(roleID);
	if (member != NULL)
	{
		member->SetStatus(BattlePlayerState(playerState));
	}
}

bool BSBattle::CheckNoWarCD(UInt64 roleID, UInt64 dstID)
{
	BSTeamMember* member = GetPlayer(roleID);
	if (member != NULL)
	{
		return member->IsNoWarCD(dstID);
	}

	return true;
}

void BSBattle::AddNoWarCD(UInt64 roleID, UInt64 dstID)
{
	BSTeamMember* member = GetPlayer(roleID);
	if (member != NULL)
	{
		member->AddNoWarCD(dstID);
	}
}

UInt32 BSBattle::CheckIsCanPk(UInt64 roleID, UInt64 dstID)
{
	BSTeamMember* myMember = GetPlayer(roleID);
	if (NULL == myMember || myMember->GetStatus() == BPS_PK)
	{
		return ErrorCode::BATTLE_SELF_PKING;
	}

	if (myMember->GetStatus() == BPS_NO_WAR)
	{
		return ErrorCode::BATTLE_NO_WAR_CHOICE;
	}

	BSTeamMember* dstMember = GetPlayer(dstID);
	if (NULL == dstMember || dstMember->GetStatus() == BPS_PK || dstMember->GetStatus() == BPS_NO_WAR)
	{
		return ErrorCode::BATTLE_TARGET_PKING;
	}
	
	if (myMember->GetStatus() != BPS_ALIVE || dstMember->GetStatus() != BPS_ALIVE)
	{
		return ErrorCode::BATTLE_TARGET_FAILED;
	}

	return ErrorCode::SUCCESS;
}

void BSBattle::SendBalanceLog(const CLProtocol::SceneBattleBalanceEnd& notify, UInt64 playerID, UInt32 reason)
{
	CLProtocol::SceneBattleBalanceLog logMsg;
	logMsg.rank = notify.rank;
	logMsg.playerNum = notify.playerNum;
	logMsg.kills = notify.kills;
	logMsg.survivalTime = notify.survivalTime;
	logMsg.score = notify.score;
	logMsg.reason = reason;
	logMsg.roleId = playerID;
	BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), logMsg);
}