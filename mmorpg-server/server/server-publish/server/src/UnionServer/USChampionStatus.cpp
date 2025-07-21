#include "USChampionStatus.h"
#include <CLErrorCode.h>
#include "ChampionScene.h"
#include <CLMatchProtocol.h>
#include "USRouter.h"
#include "ChampionService.h"
#include "SysUtility.h"
#include "ChampionRaceMgr.h"
#include "UnionServiceMgr.h"
#include "USGamble.h"

const UInt32 STATUS_GROUP[CS_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 32, 8, 15, 7, 3, 1 };

const UInt32 BATTLE_SPEC = 6 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC;

const UInt32 SCORE_TOP_SPEC = 10 * Avalon::Time::MSECS_OF_SEC;

const UInt32 STATUS_WIN_PLAYER_COUNT[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 2, 0, 0, 0, 0 };

USChampionStatus::~USChampionStatus()
{

}

void USChampionStatus::OnTick(const Avalon::Time& now)
{
	if (m_ScoreTopSyncTimer.IsTimeout(now))
	{
		if (GetBattleStatus() == CS_RE_SEA_SELECT_BATTLE || GetBattleStatus() == CS_64_SELECT_BATTLE || GetBattleStatus() == CS_16_SELECT_BATTLE)
		{
			CLProtocol::UnionChampionScoreRankTopSync sync;
			for (auto& groupIter : m_GroupMap)
			{
				sync.rankList.clear();
				auto& group = groupIter.second;
				auto& allRanklist = group.GetScoreRankList();
				auto iter = allRanklist.begin();
				UInt32 cout = 0;
				while (iter != allRanklist.end() && cout++ < 5)
				{
					sync.rankList.push_back((*iter)->GetRankRole());
					++iter;
				}
				group.BroadCast(sync);
			}
		}
	}

	if (!m_BattleTimer.IsTimeout(now))
	{
		return;
	}

	InfoLogStream << "m_BattleTimer.IsTimeout m_LastTime is " << m_BattleTimer.GetLastTime() << LogStream::eos;

	CLProtocol::UnionChampionBattleCountdownSync sync;
	sync.endTime = (m_BattleTimer.GetLastTime() + BATTLE_SPEC) / Avalon::Time::MSECS_OF_SEC;
	m_SceneMgr.BroadcastProtocol(sync);

	for (auto& groupIter : m_GroupMap)
	{
		auto& group = groupIter.second;
		group.StartBattle(GetBattleStatus());
	}
}

void USChampionStatus::OnPrepareStart()
{
}

void USChampionStatus::OnPrepareEnd()
{
}

void USChampionStatus::OnBattleStart()
{
	m_BattleTimer.Reset();
	m_BattleTimer.SetInterval(BATTLE_SPEC);
	//战斗开始立刻开始比赛
	m_BattleTimer.SetLastTime(1);

	m_ScoreTopSyncTimer.Reset();
	m_ScoreTopSyncTimer.SetInterval(SCORE_TOP_SPEC);
}

void USChampionStatus::OnBattleEnd()
{
	m_SceneMgr.ClearScene();
	auto entryVec = ChampionRewardDataEntryMgr::Instance()->GetChampionStateReward(GetBattleStatus());
	if (entryVec.size() == 0)
	{
		InfoLogStream << "dont have this status entry: " << static_cast<UInt32>(GetBattleStatus()) << LogStream::eos;
	}
	for (auto& entry : entryVec)
	{
		auto playerVec = GetStatusPlayer(static_cast<ChampionStatus>(entry->playerState));
		if (playerVec.size() == 0)
		{
			InfoLogStream << "dont have this status player: " << entry->playerState << LogStream::eos;
		}
		for (auto& player : playerVec)
		{
			std::string title = SysNotifyMgr::Instance()->MakeString(entry->title);
			std::string sender = SysNotifyMgr::Instance()->MakeString(entry->sender);
			std::string content = SysNotifyMgr::Instance()->MakeString(entry->content);
			std::string reason = GetReason(SOURCE_CHAMPION_ENROLL);
			InfoLogStream << PLAYERINFO(player) << " send reward,entry id is " << entry->id << LogStream::eos;
			SysUtility::SendMail(player->GetZoneID(), player->GetID(), sender, title, content, reason, entry->itemRewards);
		}
	}
}

UInt32 USChampionStatus::OnPlayerJoinRace(ChampionPlayer * player)
{
	if (player == nullptr)
	{
		ErrorLogStream << "player not exists" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}
	player->SetOnline(true);
	player->SetIsInRace(false);
	SyncBattleCountdown(player);
	SyncGroupID(player);
	InfoLogStream << PLAYERINFO(player) << "player join race success,set online" << LogStream::eos;
	return ErrorCode::SUCCESS;
}

bool USChampionStatus::IsScoreStatus() const
{
	if (GetBattleStatus() == CS_RE_SEA_SELECT_BATTLE || GetBattleStatus() == CS_64_SELECT_BATTLE || GetBattleStatus() == CS_16_SELECT_BATTLE)
	{
		return true;
	}
	return false;
}

void USChampionStatus::OnPlayerScoreChanged(ChampionPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}
	m_GroupMap[player->GetGroup()].OnPlayerScoreChanged(player);
}

void USChampionStatus::OnGroupBattleEnd(UInt32 groupID)
{
	m_GroupMap[groupID].SetEnd();
	ChampionGroupInfoMgr::Instance()->ChangeStatus(groupID, CGBS_END);
	auto& vec = m_GroupMap[groupID].GetPlayerVec();
	for (auto& player : vec)
	{
		if (static_cast<UInt32>(player->GetStatus()) == static_cast<UInt32>(GetPrepareStatus() + 1) || player->GetStatus() ==CS_END_SHOW)
		{
			USGambleMgr::Instance()->OnGroupBattleEnd(groupID, player->GetID());
			break;
		}
	}
	if (groupID == 1)
	{
		//猜总比分结算
		auto& groupInfo = ChampionGroupInfoMgr::Instance()->GetGroupInfo(groupID);
		UInt64 tempOption = 0;
		if (groupInfo.scoreA > groupInfo.scoreB)
		{
			tempOption = groupInfo.scoreA * 10 + groupInfo.scoreB;
		}
		else
		{
			tempOption = groupInfo.scoreB * 10 + groupInfo.scoreA;
		}
		USGambleMgr::Instance()->OnGroupBattleEnd(groupID, tempOption);

		//猜总场数结算
		tempOption = 0;
		UInt32 totalNum = 0;
		for (UInt32 i = 1; i <= 15; ++i)
		{
			totalNum += ChampionRaceMgr::Instance()->GetRaceNum(i);
		}
		if (totalNum <= 55 && totalNum >= 45)
		{
			tempOption = 4555;
		}
		else if (totalNum <= 65 && totalNum >= 56)
		{
			tempOption = 5665;
		}
		else if (totalNum <= 75 && totalNum >= 66)
		{
			tempOption = 6675;
		}
		USGambleMgr::Instance()->OnGroupBattleEnd(groupID, tempOption);
	}
}

void USChampionStatus::OnGroupBattleChanged(UInt32 groupID, ChampionPlayer * winner, ChampionPlayer * loser)
{
	if (winner == nullptr || loser == nullptr)
	{
		return;
	}
	ChampionGroupInfoMgr::Instance()->OnChanged(groupID, winner, loser);
}

void USChampionStatus::OnAllGroupBattleStart()
{
	for (auto& groupPair : m_GroupMap)
	{
		ChampionGroupInfoMgr::Instance()->ChangeStatus(groupPair.second.GetID(), CGBS_STARTED);
	}
}

const std::list<ChampionPlayer*>& USChampionStatus::OnPlayerGetGroupScoreRank(ChampionPlayer * player)
{
	return m_GroupMap[player->GetGroup()].GetScoreRankList();
}

const std::vector<ChampionScoreBattleRecord>& USChampionStatus::OnPlayerGetGroupScoreRecord(ChampionPlayer* player)
{
	return m_GroupMap[player->GetGroup()].GetScoreRecord();
}

void USChampionStatus::SyncBattleCountdown(ChampionPlayer* player)
{
	if (player == nullptr)
	{
		return;
	}
	CLProtocol::UnionChampionBattleCountdownSync sync;
	sync.endTime = (m_BattleTimer.GetLastTime() + BATTLE_SPEC) / Avalon::Time::MSECS_OF_SEC;
	player->SendProtocol(sync);
}

void USChampionStatus::SyncScoreRankTop(ChampionPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}
	CLProtocol::UnionChampionScoreRankTopSync sync;
	auto& allRanklist = m_GroupMap[player->GetGroup()].GetScoreRankList();
	auto iter = allRanklist.begin();
	UInt32 cout = 0;
	while (iter != allRanklist.end() && cout++ < 5)
	{
		sync.rankList.push_back((*iter)->GetRankRole());
		++iter;
	}
	player->SendProtocol(sync);
}

void USChampionStatus::SyncGroupID(ChampionPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}
	CLProtocol::UnionChampionGroupIDSync sync;
	sync.id = player->GetGroup();
	player->SendProtocol(sync);
}

void USChampionStatus::SyncKnockoutScore(ChampionPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}
	auto rolesVec = m_GroupMap[player->GetGroup()].GetPlayerVec();
	if (rolesVec.size() != 2)
	{
		return;
	}
	CLProtocol::UnionChampionKnockoutScore sync;
	for (auto& player : rolesVec)
	{
		ChampionKnockoutPlayer knockoutPlayer;
		knockoutPlayer.roleID = player->GetID();
		knockoutPlayer.name = player->GetName();
		knockoutPlayer.occu = player->GetOccu();
		knockoutPlayer.server = player->GetServer();
		knockoutPlayer.score = player->GetWinCount();
		sync.roles.push_back(knockoutPlayer);
	}
	player->SendProtocol(sync);
}


void USChampionStatus::GroupPlayer(ChampionStatus status, UInt32 groupNum)
{
	auto playerVec = GetStatusPlayer(status);
	UInt32 size = playerVec.size();

	UInt32 breakPos = 0;
	{
		UInt32 groupId = 1;
		UInt32 currNum = 0;
		for (UInt32 i = 0; i < size; ++i)
		{
			++currNum;
			PlayerBelongToGroup(static_cast<ChampionPlayer*>(playerVec[i]), groupId);
			if (currNum >= STATUS_WIN_PLAYER_COUNT[GetPrepareStatus()])
			{
				++groupId;
				currNum = 0;
			}
			if (groupId > groupNum)
			{
				breakPos = i + 1;
				break;
			}
		}
	}

	if (breakPos >= size || breakPos == 0)
	{
		return;
	}

	UInt32 oneGroupPlayerNum = (size - breakPos) / groupNum;
	//测试用，正式不要放出来
	//if (oneGroupPlayerNum == 0)
	//{
	//	oneGroupPlayerNum = groupNum;
	//}
	UInt32 groupId = 1;
	UInt32 currNum = 0;
	bool full = false;
	for (UInt32 i = breakPos; i < size; ++i)
	{
		++currNum;
		PlayerBelongToGroup(static_cast<ChampionPlayer*>(playerVec[i]), groupId);
		if (full)
		{
			++groupId;
			continue;
		}
		if (currNum >= oneGroupPlayerNum)
		{
			currNum = 0;
			++groupId;
		}
		if (groupId > groupNum)
		{
			groupId = 1;
			full = true;
		}
	}
}

void USChampionStatus::GroupPlayerByScore(ChampionStatus status, UInt32 groupNum)
{
	auto playerVec = GetStatusPlayer(status);
	std::sort(playerVec.begin(), playerVec.end(), [&](USPlayer* a, USPlayer *b)->bool {
		auto newA = static_cast<ChampionPlayer*>(a);
		auto newB = static_cast<ChampionPlayer*>(b);
		if (newA->GetScore() > newB->GetScore())
		{
			return true;
		}
		else if (newA->GetScore() == newB->GetScore())
		{
			if (newA->GetScoreChangeTime() < newB->GetScoreChangeTime())
			{
				return true;
			}
		}
		return false;
	});
	UInt32 size = playerVec.size();


	UInt32 breakPos = 0;
	{
		UInt32 groupId = 1;
		UInt32 currNum = 0;
		for (UInt32 i = 0; i < size; ++i)
		{
			++currNum;
			PlayerBelongToGroup(static_cast<ChampionPlayer*>(playerVec[i]), groupId);
			if (currNum >= STATUS_WIN_PLAYER_COUNT[GetPrepareStatus()])
			{
				++groupId;
				currNum = 0;
			}
			if (groupId > groupNum)
			{
				breakPos = i + 1;
				break;
			}
		}
	}

	if (breakPos >= size || breakPos == 0)
	{
		return;
	}

	UInt32 oneGroupPlayerNum = (size - breakPos) / groupNum;
	//测试用代码
	//if (oneGroupPlayerNum == 0)
	//{
	//	oneGroupPlayerNum = groupNum;
	//}
	UInt32 groupId = 1;
	UInt32 currNum = 0;
	bool full = false;
	for (UInt32 i = breakPos; i < size; ++i)
	{
		++currNum;
		PlayerBelongToGroup(static_cast<ChampionPlayer*>(playerVec[i]), groupId);
		if (full)
		{
			++groupId;
			continue;
		}
		if (currNum >= oneGroupPlayerNum)
		{
			currNum = 0;
			++groupId;
		}
		if (groupId > groupNum)
		{
			groupId = 1;
			full = true;
		}
	}
}

void USChampionStatus::GroupPlayerByLastGroup(ChampionStatus status)
{
	auto playerVec = GetStatusPlayer(status);

	for (UInt32 i = 0; i < playerVec.size(); ++i)
	{
		auto player = static_cast<ChampionPlayer*>(playerVec[i]);
		PlayerBelongToGroup(player, player->GetGroup() / 2);
	}
	for (auto& groupPair : m_GroupMap)
	{
		auto vec = groupPair.second.GetPlayerVec();
		ChampionGroupStatus info;
		info.groupID = groupPair.first;
		info.roleA = vec[0]->GetID();
		info.roleB = vec[1]->GetID();
		info.scoreA = 0;
		info.scoreB = 0;
		info.status = 0;
		ChampionGroupInfoMgr::Instance()->Push(info);
	}
}

void USChampionStatus::GroupPlayerByRank(ChampionStatus status)
{
	const UInt32 RANK_GROUP_MAP[] = { 0, 8, 15, 12, 11, 10, 13, 14, 9, 9, 14, 13, 10, 11, 12, 15, 8 };
	const UInt32 RANK_POS_MAP[] = { 0, 16, 31, 24, 23, 21, 26, 29, 18, 19, 28, 27, 20, 22, 25, 30, 17 };
	auto playerVec = GetStatusPlayer(status);
	std::sort(playerVec.begin(), playerVec.end(), [&](USPlayer* a, USPlayer *b)->bool {
		auto newA = static_cast<ChampionPlayer*>(a);
		auto newB = static_cast<ChampionPlayer*>(b);
		if (newA->GetScore() > newB->GetScore())
		{
			return true;
		}
		else if (newA->GetScore() == newB->GetScore())
		{
			if (newA->GetScoreChangeTime() < newB->GetScoreChangeTime())
			{
				return true;
			}
		}
		return false;
	});
	for (UInt32 i = 0; i < playerVec.size(); ++i)
	{
		auto player = static_cast<ChampionPlayer*>(playerVec[i]);
		PlayerBelongToGroup(player, RANK_GROUP_MAP[i + 1]);
		player->SetRank(i + 1);
		player->SetPos(RANK_POS_MAP[i + 1]);
	}
	for (auto& groupPair : m_GroupMap)
	{
		auto vec = groupPair.second.GetPlayerVec();
		if (vec.size() < 2)
		{
			if (!m_Service.IsGMControlStatus())
			{
				ErrorLogStream << "group player faild, id " << groupPair.first << LogStream::eos;
				continue;
			}
			else
			{
				ChampionGroupStatus info;
				info.groupID = groupPair.first;
				info.roleA = vec[0]->GetID();
				info.roleB = vec[0]->GetID();
				info.scoreA = 0;
				info.scoreB = 0;
				info.status = 0;
				ChampionGroupInfoMgr::Instance()->Push(info);
				continue;
			}
		}
		ChampionGroupStatus info;
		info.groupID = groupPair.first;
		info.roleA = vec[0]->GetID();
		info.roleB = vec[1]->GetID();
		info.scoreA = 0;
		info.scoreB = 0;
		info.status = 0;
		ChampionGroupInfoMgr::Instance()->Push(info);
	}

}

void USChampionStatus::GroupPlayerByStatus(ChampionStatus status)
{
	auto playerVec = GetStatusPlayer(status);
	for (UInt32 i = 0; i < playerVec.size(); ++i)
	{
		auto player = static_cast<ChampionPlayer*>(playerVec[i]);
		PlayerBelongToGroup(player, player->GetGroup());
	}
}

void USChampionStatus::PlayerBelongToGroup(ChampionPlayer * player, UInt32 groupId)
{
	if (player == nullptr)
	{
		return;
	}
	if (groupId <= 0)
	{
		return;
	}
	m_GroupMap[groupId].SetID(groupId);
	m_GroupMap[groupId].Push(player);
	player->SetGroup(groupId);
}

void USChampionStatus::GenScene(UInt32 num)
{
	for (UInt32 i = 1; i <= num; ++i)
	{
		m_SceneMgr.AddScene(i);
	}
}

void USChampionStatus::PushAllGroupPlayerToScene(ChampionStatus status)
{
	auto playerVec = GetStatusPlayer(status);
	for (UInt32 i = 0; i < playerVec.size(); ++i)
	{
		auto player = static_cast<ChampionPlayer*>(playerVec[i]);
		if (player == nullptr)
		{
			ErrorLogStream << "player ptr is null,status is" << static_cast<UInt32>(status) << LogStream::eos;
			continue;
		}
		auto scene = static_cast<ChampionScene*>(m_SceneMgr.FindUScene(player->GetGroup()));
		if (scene == nullptr)
		{
			ErrorLogStream << "can't find scene, id is" << player->GetGroup() << " status is " << static_cast<UInt32>(status) << LogStream::eos;
			continue;
		}
		scene->AddPlayer(player);
		player->SetSceneID(scene->GetSceneID());
	}
}

void USChampionStatus::PushAllGroupPlayerToOneScene(ChampionStatus status)
{
	auto playerVec = GetStatusPlayer(status);
	for (UInt32 i = 0; i < playerVec.size(); ++i)
	{
		auto player = static_cast<ChampionPlayer*>(playerVec[i]);
		if (player == nullptr)
		{
			ErrorLogStream << "player ptr is null" << LogStream::eos;
			continue;
		}
		auto scene = static_cast<ChampionScene*>(m_SceneMgr.FindUScene(1));
		if (scene == nullptr)
		{
			ErrorLogStream << "can't find scene, id is" << player->GetGroup() << LogStream::eos;
			continue;
		}
		scene->AddPlayer(player);
		player->SetSceneID(1);
	}
}

UInt32 USChampionStatus::OnPlayerBattleEndScore(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result)
{
	if (winner == nullptr || loser == nullptr)
	{
		return ErrorCode::CHAMPION_NO_INFO;
	}

	winner->SetBattleCount(winner->GetBattleCount() + 1);
	loser->SetBattleCount(loser->GetBattleCount() + 1);

	ChampionScoreBattleRecord record;
	if (result == resultBitmask_invalid || result == resultBitmask_draw)
	{
		winner->SetScore(winner->GetScore() + 1);
		loser->SetScore(loser->GetScore() + 1);
		record.result = resultBitmask_draw;
	}
	else
	{
		winner->SetScore(winner->GetScore() + 3);
		record.result = resultBitmask_win;
	}
	record.roleIDA = winner->GetID();
	record.nameA = winner->GetName();
	record.roleIDB = loser->GetID();
	record.nameB = loser->GetName();
	m_GroupMap[winner->GetGroup()].PushRecord(record);

	return ErrorCode::SUCCESS;
}

UInt32 USChampionStatus::OnPlayerBattleEndCount(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result)
{
	if (result == resultBitmask_invalid || result == resultBitmask_draw)
	{
		return ErrorCode::SUCCESS;
	}
	if (winner != nullptr)
	{
		winner->SetWinCount(winner->GetWinCount() + 1);
		winner->SyncBattleCount();
	}
	if (loser != nullptr)
	{
		loser->SetStatus(CS_ENROLL);
		loser->SetLoseCount(loser->GetLoseCount() + 1);
		loser->SyncBattleCount();
	}

	if (winner->GetWinCount() >= 3)
	{
		winner->SetStatus(CS_RE_SEA_SELECT_PREPARE);
	}
	return ErrorCode::SUCCESS;
}

UInt32 USChampionStatus::OnPlayerBattleEnd3Win(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	if (winner == nullptr || loser == nullptr)
	{
		ErrorLogStream << "can't find all battle player" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}

	if (info == nullptr)
	{
		ErrorLogStream << "BattleResultActInfo is null" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}

	//除平局外都以Match返回结果为准
	if (result != resultBitmask_draw)
	{
		winner->SetWinCount(winner->GetWinCount() + 1);
		winner->SetBattleCount(winner->GetBattleCount() + 1);
		loser->SetLoseCount(loser->GetLoseCount() + 1);
		loser->SetBattleCount(loser->GetBattleCount() + 1);
		info->trueWinner = winner->GetID();
		info->trueLoser = loser->GetID();
	}
	else
	{
		if (loser->GetRank() < winner->GetRank())
		{
			loser->SetWinCount(loser->GetWinCount() + 1);
			loser->SetBattleCount(loser->GetBattleCount() + 1);
			winner->SetLoseCount(winner->GetLoseCount() + 1);
			winner->SetBattleCount(winner->GetBattleCount() + 1);
			info->trueWinner = loser->GetID();
			info->trueLoser = winner->GetID();
		}
		else if (loser->GetRank() > winner->GetRank())
		{
			winner->SetWinCount(winner->GetWinCount() + 1);
			winner->SetBattleCount(winner->GetBattleCount() + 1);
			loser->SetLoseCount(loser->GetLoseCount() + 1);
			loser->SetBattleCount(loser->GetBattleCount() + 1);
			info->trueWinner = winner->GetID();
			info->trueLoser = loser->GetID();
		}
	}
	
	OnGroupBattleChanged(winner->GetGroup(), winner, loser);

	if (winner->GetWinCount() >= 3)
	{
		if (GetPrepareStatus() == CS_1_SELECT_PREPARE)
		{
			//决赛情况特殊处理
			winner->SetStatus(static_cast<ChampionStatus>(CS_END_SHOW));
		}
		else
		{
			winner->SetStatus(static_cast<ChampionStatus>(GetPrepareStatus() + 1));
		}
		winner->SetPos(winner->GetPos() / 2);
		OnGroupBattleEnd(winner->GetGroup());
	}
	else if (loser->GetWinCount() >= 3)
	{
		if (GetPrepareStatus() == CS_1_SELECT_PREPARE)
		{
			//决赛情况特殊处理
			loser->SetStatus(static_cast<ChampionStatus>(CS_END_SHOW));
		}
		else
		{
			loser->SetStatus(static_cast<ChampionStatus>(GetPrepareStatus() + 1));
		}
		loser->SetPos(loser->GetPos() / 2);
		OnGroupBattleEnd(loser->GetGroup());
	}

	SyncKnockoutScore(winner);
	SyncKnockoutScore(loser);

	return ErrorCode::SUCCESS;
}

void USChampionStatus::LockOnlinePlayerForBattle()
{
	for (auto groupIter : m_GroupMap)
	{
		auto group = groupIter.second;
		for (auto& player : group.GetPlayerVec())
		{
			if (!player->GetOnline())
			{
				continue;
			}
			player->SetStatus(GetBattleStatus());
		}
	}
}

void USChampionStatus::ClearPlayerWinCount()
{
	for (auto& groupIter : m_GroupMap)
	{
		auto group = groupIter.second;
		for (auto& player : group.GetPlayerVec())
		{
			if (player->GetStatus() == GetPrepareStatus() + 1)
			{
				player->SetWinCount(0);
				player->SetLoseCount(0);
				player->SetBattleCount(0);
			}
		}
	}
}

void USChampionStatus::ClearGroup()
{
	m_GroupMap.clear();
}

USPlayerVec USChampionStatus::GetStatusPlayer(ChampionStatus status)
{
	return m_PlayerMgr.GetSuitUSPlayer([&](USPlayer* player)->bool {
		auto championPlayer = static_cast<ChampionPlayer*>(player);
		if (championPlayer->GetStatus() == status)
		{
			return true;
		}
		return false;
	});
}

void ChampionGroup::Push(ChampionPlayer * player)
{
	m_PlayerVec.emplace_back(player);
}

void ChampionGroup::PushRecord(ChampionScoreBattleRecord & record)
{
	m_ScoreRecords.push_back(record);
}

ChampionPlayer* ChampionGroup::Find(UInt64 roleId)
{
	for (auto player : m_PlayerVec)
	{
		if (player->GetID() == roleId)
		{
			return player;
		}
	}
	return nullptr;
}

std::vector<ChampionPlayer*> ChampionGroup::GetTopScorePlayer(UInt32 top)
{
	//std::sort(m_PlayerVec.begin(), m_PlayerVec.end(), [](ChampionPlayer* a, ChampionPlayer* b)->bool {
	//	if (a->GetScore() > b->GetScore())
	//	{
	//		return true;
	//	}
	//	else if (a->GetScore() == b->GetScore())
	//	{
	//		if (a->GetScoreChangeTime() < b->GetScoreChangeTime())
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//});

	std::vector<ChampionPlayer*> vec;
	if (top > m_RankList.size())
	{
		top = m_RankList.size();
	}
	auto iter = m_RankList.begin();
	while (top--)
	{
		++iter;
	}
	vec.insert(vec.begin(), m_RankList.begin(), iter);
	return vec;
}

void ChampionGroup::ModifyAllPlayer(std::function<void(ChampionPlayer*)> fun)
{

}

void ChampionGroup::StartBattle(ChampionStatus status)
{
	InfoLogStream << "group(" << GetID() << ")start battle,order is "  << m_Order  << "m_IsEnd is " << (m_IsEnd ? 1 : 0) <<  ",status is " << static_cast<UInt32>(status) << LogStream::eos;
	if (m_Order >= 5 || m_IsEnd)
	{
		InfoLogStream << "group(" << GetID() << ")start battle,order is " << m_Order << "m_IsEnd is " 
			<< (m_IsEnd ? 1 : 0) << ",status is " << static_cast<UInt32>(status) << " group start battle returnd" << LogStream::eos;
		return;
	}
	CLProtocol::MatchServerChampionBattleReq req;

	if (status == CS_1_SELECT_BATTLE || status == CS_2_SELECT_BATTLE || status == CS_4_SELECT_BATTLE || status == CS_8_SELECT_BATTLE)
	{
		
		ChampionPlayer* winner = nullptr;
		ChampionPlayer* loser = nullptr;
		if (m_PlayerVec.size() != 2)
		{
			ErrorLogStream << "group start battle faild,player num(" << static_cast<UInt32>(m_PlayerVec.size()) << ") is error" << LogStream::eos;
			return;
		}

		bool isThisRaceEnd = true;
		auto player1 = m_PlayerVec[0];
		auto player2 = m_PlayerVec[1];

		if (player1 == nullptr || player2 == nullptr)
		{
			ErrorLogStream << "one player is null" << LogStream::eos;
			return;
		}

		if (!player1->CanStartRace(status) && !player2->CanStartRace(status))
		{
			InfoLogStream << "all two player can't start race " << PLAYERINFO(player1) << "," << PLAYERINFO(player2) << LogStream::eos;
			winner = player1->GetRank() > player2->GetRank() ? player2 : player1;
			loser = player1->GetRank() > player2->GetRank() ? player1 : player2;
		}
		else if (!player1->CanStartRace(status))
		{
			InfoLogStream << "player(" << PLAYERINFO(player1) << ") can't start race, other player is " << PLAYERINFO(player2) << LogStream::eos;
			winner = player2;
			loser = player1;
		}
		else if (!player2->CanStartRace(status))
		{
			InfoLogStream << "player(" << PLAYERINFO(player2) << ") can't start race, other player is " << PLAYERINFO(player1) << LogStream::eos;
			winner = player1;
			loser = player2;
		}
		else
		{
			InfoLogStream << "all two player(" << PLAYERINFO(player1) << "," << PLAYERINFO(player2) << ") start race is ok" << LogStream::eos;
			req.roleIds.push_back(player1->GetID());
			req.roleIds.push_back(player2->GetID());
			isThisRaceEnd = false;
		}

		ChampionRaceMgr::Instance()->OnRaceBegin(player1, player2, status, GetID(), ++m_Order);

		if (isThisRaceEnd)
		{
			InfoLogStream << "one or two player can't start race,OnRaceEnd" << PLAYERINFO(player1) << "," << PLAYERINFO(player2) << LogStream::eos;
			ChampionRaceMgr::Instance()->OnRaceEnd(GetID(), m_Order, winner == nullptr ? 0 : winner->GetID(), loser == nullptr ? 0 : loser->GetID(), resultBitmask_win);

			if (winner != nullptr)
			{
				winner->NotifyNullTurn();
			}
			
			auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
			if (service == nullptr)
			{
				ErrorLogStream << "serveice not exists" << LogStream::eos;
				return;
			}
			BattleResultActInfo info;
			info.winnerID = winner == nullptr ? 0 : winner->GetID();
			info.loseID = loser == nullptr ? 0 : loser->GetID();
			info.result = resultBitmask_win;
			service->BeginAct(CA_RACE_END, 0, 0, &info);
			return;
		}
		
		req.order = m_Order;
	}
	else if (status == CS_64_SELECT_BATTLE || status == CS_16_SELECT_BATTLE)
	{
		std::vector<ChampionPlayer*> canStartPlayerVec;
		for (auto player : m_PlayerVec)
		{
			if (player->CanStartRace(status))
			{
				canStartPlayerVec.push_back(player);
			}
		}

		std::set<ChampionPlayer*> alreadyStartPlayer;
		std::vector<ChampionPlayer*> notFindEnemyPlayer;
		for (UInt32 i = 0; i < canStartPlayerVec.size(); ++i)
		{
			auto aPlayer = canStartPlayerVec[i];
			if (alreadyStartPlayer.count(aPlayer) > 0)
			{
				continue;
			}
			bool boFind = false;
			for (UInt32 j = i + 1; j < canStartPlayerVec.size(); ++j)
			{
				auto bPlayer = canStartPlayerVec[j];
				if (alreadyStartPlayer.count(bPlayer) > 0)
				{
					continue;
				}
				if (aPlayer->CanBattleWith(bPlayer->GetID()))
				{
					aPlayer->AddBattledRole(bPlayer->GetID());
					bPlayer->AddBattledRole(aPlayer->GetID());
					alreadyStartPlayer.insert(aPlayer);
					alreadyStartPlayer.insert(bPlayer);
					aPlayer->SetIsNoTurn(false);
					bPlayer->SetIsNoTurn(false);
					req.roleIds.push_back(aPlayer->GetID());
					req.roleIds.push_back(bPlayer->GetID());
					boFind = true;
					break;
				}
			}
			if (!boFind)
			{
				notFindEnemyPlayer.push_back(aPlayer);
			}
		}

		if (notFindEnemyPlayer.size() > 0)
		{
			std::random_shuffle(notFindEnemyPlayer.begin(), notFindEnemyPlayer.end());
			UInt32 pos = 0;
			for (UInt32 i = 0; i < notFindEnemyPlayer.size(); ++i)
			{
				if (notFindEnemyPlayer[i]->IsNoTurn())
				{
					std::swap(notFindEnemyPlayer[i], notFindEnemyPlayer[pos++]);
				}
			}
			for (auto player : notFindEnemyPlayer)
			{
				player->SetIsNoTurn(false);
				req.roleIds.push_back(player->GetID());
			}
		}
	}
	else if (status == CS_RE_SEA_SELECT_BATTLE)
	{
		std::vector<ChampionPlayer*> canStartPlayerVec;
		for (auto player : m_PlayerVec)
		{
			if (player->CanStartRace(status))
			{
				//临时处理加了场数上线
				if (player->GetBattleCount() >= 10)
				{
					continue;
				}
				canStartPlayerVec.push_back(player);
			}
		}
		if (canStartPlayerVec.size() > 0)
		{
			std::random_shuffle(canStartPlayerVec.begin(), canStartPlayerVec.end());
			UInt32 pos = 0;
			for (UInt32 i = 0; i < canStartPlayerVec.size(); ++i)
			{
				if (canStartPlayerVec[i]->IsNoTurn())
				{
					std::swap(canStartPlayerVec[i], canStartPlayerVec[pos++]);
				}
			}
			for (auto player : canStartPlayerVec)
			{
				player->SetIsNoTurn(false);
				req.roleIds.push_back(player->GetID());
			}
		}
	}
	else
	{
		for (auto& player : m_PlayerVec)
		{
			if (!player->CanStartRace(status))
			{
				InfoLogStream << PLAYERINFO(player) << "can't start race, status is " << static_cast<UInt32>(status) << LogStream::eos;
				continue;
			}
			player->SetIsNoTurn(false);
			req.roleIds.push_back(player->GetID());
		}
		std::random_shuffle(req.roleIds.begin(), req.roleIds.end());
	}

	if (req.roleIds.size() % 2 > 0)
	{
		UInt64 nullRoleID = req.roleIds.back();
		auto nullTurnPlayer = Find(nullRoleID);
		if (nullTurnPlayer != nullptr)
		{
			nullTurnPlayer->NotifyNullTurn();
			nullTurnPlayer->SetIsNoTurn(true);
			if (status == CS_64_SELECT_BATTLE || status == CS_16_SELECT_BATTLE)
			{
				nullTurnPlayer->SetBattleCount(nullTurnPlayer->GetBattleCount() + 1);
				nullTurnPlayer->SetScore(nullTurnPlayer->GetScore() + 3);
				ChampionScoreBattleRecord record;
				record.roleIDA = nullTurnPlayer->GetID();
				record.nameA = nullTurnPlayer->GetName();
				record.roleIDB = 0;
				record.nameB = "";
				record.result = resultBitmask_win;
				PushRecord(record);
			}
			if (status == CS_RE_SEA_SELECT_BATTLE)
			{
				nullTurnPlayer->SetBattleCount(nullTurnPlayer->GetBattleCount() + 1);
			}
		}
		req.roleIds.erase(req.roleIds.end() - 1);
	}

	InfoLogStream <<  "status is " << std::to_string(static_cast<UInt32>(status)) << " ,group: " 
		<< std::to_string(GetID()) << " ,order is " << std::to_string(m_Order) << " startbattle:" << LogStream::eos;
	for (auto& id : req.roleIds)
	{
		InfoLogStream << "id:" << std::to_string(id) << LogStream::eos;
	}
	
	req.groupID = GetID();

	CLProtocol::MatchServerChampionBattleReq newReq;
	newReq.groupID = req.groupID;
	newReq.order = req.order;
	newReq.battleStatus = req.battleStatus;
	//单次只发送CHAMPION_BATTLE_PACKET_LIMIT_ROLE个roleid，防止超出一个packet的大小
	while (req.roleIds.size() > 0)
	{
		newReq.roleIds.clear();
		UInt32 size = 0;
		if (req.roleIds.size() >= CHAMPION_BATTLE_PACKET_LIMIT_ROLE)
		{
			size = CHAMPION_BATTLE_PACKET_LIMIT_ROLE;
		}
		else
		{
			size = req.roleIds.size();
		}
		newReq.roleIds.insert(newReq.roleIds.begin(), req.roleIds.begin(), req.roleIds.begin() + size);
		req.roleIds.erase(req.roleIds.begin(), req.roleIds.begin() + size);
		USRouter::Instance()->SendToMatch(newReq);
	}
}

ChampionPlayer* ChampionGroup::GetEnemyPlayer(UInt64 roleId)
{
	if (m_PlayerVec.size() != 2)
	{
		return nullptr;
	}
	for (auto& player : m_PlayerVec)
	{
		if (player->GetID() != roleId)
		{
			return player;
		}
	}
	return nullptr;
}

bool IsFirstBigThanSecond(ChampionPlayer * first, ChampionPlayer * second)
{
	if (first->GetScore() > second->GetScore())
	{
		return true;
	}
	else if (first->GetScore() == second->GetScore())
	{
		if (first->GetBattleCount() > second->GetBattleCount())
		{
			return true;
		}
		else if (first->GetBattleCount() == second->GetBattleCount())
		{
			if (first->GetScoreChangeTime() <= second->GetScoreChangeTime())
			{
				return true;
			}
		}
	}
	return false;
}

void ChampionGroup::OnPlayerScoreChanged(ChampionPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}

	for (auto i = m_RankList.begin(); i != m_RankList.end(); ++i)
	{
		if (*i == player)
		{
			m_RankList.erase(i);
			break;
		}
	}

	if (m_RankList.empty())
	{
		m_RankList.push_back(player);
		return;
	}

	if (IsFirstBigThanSecond(m_RankList.back(), player))
	{
		m_RankList.push_back(player);
		return;
	}

	//if (player->GetScore() == m_RankList.back()->GetScore())
	//{
	//	if (player->GetBattleCount() > m_RankList.back()->GetBattleCount())
	//	{
	//		m_RankList.push_back(player);
	//		return;
	//	}
	//}
	//else if (player->GetScore() < m_RankList.back()->GetScore())
	//{
	//	m_RankList.push_back(player);
	//}

	for (auto i = m_RankList.begin(); i != m_RankList.end(); ++i)
	{
		if (!IsFirstBigThanSecond((*i), player))
		{
			m_RankList.insert(i, player);
			break;
		}
	}
}

const std::list<ChampionPlayer*>& ChampionGroup::GetScoreRankList() const
{
	return m_RankList;
}

const std::vector<ChampionScoreBattleRecord>& ChampionGroup::GetScoreRecord() const
{
	return m_ScoreRecords;
}

void ChampionGroup::BroadCast(Avalon::Protocol & protocol)
{
	for (auto& player : m_PlayerVec)
	{
		if (player->GetOnline())
		{
			player->SendProtocol(protocol);
		}
	}
}

void SeaStatus::OnPrepareStart()
{
	ClearGroup();
	auto playerVec = GetStatusPlayer(GetPrepareStatus());
	for (auto& player : playerVec)
	{
		static_cast<ChampionPlayer*>(player)->SetGroup(0);
		static_cast<ChampionPlayer*>(player)->SetBattleCount(0);
		static_cast<ChampionPlayer*>(player)->SetWinCount(0);
		static_cast<ChampionPlayer*>(player)->SetLoseCount(0);
	}
	return;
}

UInt32 SeaStatus::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);

	if (player->GetGroup() != 0)
	{
		player->SyncBattleCount();
		return ErrorCode::SUCCESS;
	}

	auto scene = static_cast<ChampionScene*>(m_SceneMgr.FindUScene(m_CurrSceneId));
	if (scene == nullptr)
	{
		scene = static_cast<ChampionScene*>(m_SceneMgr.AddScene(m_CurrSceneId));
	}
	if (scene == nullptr)
	{
		player->SetOnline(false);
		return ErrorCode::CHAMPION_NO_INFO;
	}

	if (scene->GetPlayerNum() >= 100)
	{
		++m_CurrSceneId;
		scene = static_cast<ChampionScene*>(m_SceneMgr.AddScene(m_CurrSceneId));
	}

	PlayerBelongToGroup(player, 1);
	scene->AddPlayer(player);
	player->SetSceneID(scene->GetSceneID());
	player->SyncBattleCount();

	InfoLogStream << PLAYERINFO(player) << "sea battle,player join race,scene id is " << scene->GetSceneID() << " player group is " << player->GetGroup() << LogStream::eos;

	return ErrorCode::SUCCESS;
}

UInt32 SeaStatus::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return OnPlayerBattleEndCount(winner, loser, result);
}

void SeaStatus::OnBattleEnd()
{
	auto playerVec = GetStatusPlayer(ChampionStatus::CS_SEA_SELECT_PREPARE);
	for (auto& player : playerVec )
	{
		static_cast<ChampionPlayer*>(player)->SetStatus(CS_ENROLL);
	}
	for (auto& groupIter : m_GroupMap)
	{
		auto group = groupIter.second;
		for (auto& player : group.GetPlayerVec())
		{
			player->SetWinCount(0);
			player->SetBattleCount(0);
			player->SetLoseCount(0);
			if (player->GetStatus() != GetPrepareStatus() + 1)
			{
				player->SetStatus(CS_ENROLL);
			}
		}
	}
	USChampionStatus::OnBattleEnd();
}

void ReSeaStatus::OnPrepareStart()
{
	ClearGroup();
	auto playerVec = GetStatusPlayer(GetPrepareStatus());
	//人数过少，轮空
	//if (!m_Service.IsGMControlStatus())
	//{
	//	if (playerVec.size() < 256)
	//	{
	//		for (auto& player : playerVec)
	//		{
	//			static_cast<ChampionPlayer*>(player)->SetStatus(CS_64_SELECT_PREPARE);
	//			//static_cast<ChampionPlayer*>(player)->SetScore(0);
	//			//todo 具体邮件内容
	//			SysUtility::SendMail(player->GetConnId(), player->GetID(), "", "", "", "", ItemRewardVec());
	//		}
	//		return;
	//	}
	//}
	GroupPlayer(GetPrepareStatus(), STATUS_GROUP[GetPrepareStatus()]);
	for (auto& player : playerVec)
	{
		static_cast<ChampionPlayer*>(player)->SetScoreChangeTime(0);
		static_cast<ChampionPlayer*>(player)->SetScore(0);
		static_cast<ChampionPlayer*>(player)->SetBattleCount(0);
		static_cast<ChampionPlayer*>(player)->SetIsNoTurn(false);
		static_cast<ChampionPlayer*>(player)->ClearBattledRole();
	}
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

UInt32 ReSeaStatus::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEndScore(winner, loser, result);
}

void ReSeaStatus::OnBattleEnd()
{
	
	for (auto& groupIter : m_GroupMap)
	{
		auto& group = groupIter.second;
		auto playerVec = group.GetTopScorePlayer(STATUS_WIN_PLAYER_COUNT[GetPrepareStatus()]);
		for (auto& player : playerVec)
		{
			player->SetStatus(CS_64_SELECT_PREPARE);
			static_cast<ChampionPlayer*>(player)->SetScoreChangeTime(0);
			static_cast<ChampionPlayer*>(player)->SetScore(0);
		}
	}
	USChampionStatus::OnBattleEnd();
}

UInt32 ReSeaStatus::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncScoreRankTop(player);
	return ErrorCode::SUCCESS;
}

void Select64Status::OnPrepareStart()
{
	ClearGroup();
	auto playerVec = GetStatusPlayer(GetPrepareStatus());
	//人数过少，轮空
	//if (!m_Service.IsGMControlStatus())
	//{
	//	if (playerVec.size() < 64)
	//	{
	//		for (auto& player : playerVec)
	//		{
	//			static_cast<ChampionPlayer*>(player)->SetStatus(CS_16_SELECT_PREPARE);
	//			//todo 具体邮件内容
	//			SysUtility::SendMail(player->GetConnId(), player->GetID(), "", "", "", "", ItemRewardVec());
	//		}
	//		return;
	//	}
	//}
	//测试用，所有角色进同一组
	//GroupPlayer(GetPrepareStatus(), 1);
	GroupPlayer(GetPrepareStatus(), STATUS_GROUP[GetPrepareStatus()]);
	for (auto& player : playerVec)
	{
		static_cast<ChampionPlayer*>(player)->SetScoreChangeTime(0);
		static_cast<ChampionPlayer*>(player)->SetScore(0);
		static_cast<ChampionPlayer*>(player)->SetBattleCount(0);
		static_cast<ChampionPlayer*>(player)->SetIsNoTurn(false);
		static_cast<ChampionPlayer*>(player)->ClearBattledRole();
	}
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

UInt32 Select64Status::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEndScore(winner, loser, result);
}

void Select64Status::OnBattleEnd()
{
	for (auto& groupIter : m_GroupMap)
	{
		auto& group = groupIter.second;
		auto playerVec = group.GetTopScorePlayer(STATUS_WIN_PLAYER_COUNT[GetPrepareStatus()]);
		for (auto& player : playerVec)
		{
			player->SetStatus(CS_16_SELECT_PREPARE);
		}
	}
	USChampionStatus::OnBattleEnd();
}

UInt32 Select64Status::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncScoreRankTop(player);
	return ErrorCode::SUCCESS;
}

void Select16Status::OnPrepareStart()
{
	ClearGroup();
	//测试用，所有角色进同一组
	//GroupPlayerByScore(GetPrepareStatus(), 1);
	GroupPlayerByScore(GetPrepareStatus(), STATUS_GROUP[GetPrepareStatus()]);
	auto playerVec = GetStatusPlayer(GetPrepareStatus());
	for (auto& player : playerVec)
	{
		static_cast<ChampionPlayer*>(player)->SetScoreChangeTime(0);
		static_cast<ChampionPlayer*>(player)->SetScore(0);
		static_cast<ChampionPlayer*>(player)->SetBattleCount(0);
		static_cast<ChampionPlayer*>(player)->SetIsNoTurn(false);
		static_cast<ChampionPlayer*>(player)->ClearBattledRole();
	}
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

UInt32 Select16Status::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEndScore(winner, loser, result);
}

void Select16Status::OnBattleEnd()
{
	std::vector<ChampionPlayer*> vec;
	for (auto& groupIter : m_GroupMap)
	{
		auto& group = groupIter.second;
		auto playerVec = group.GetTopScorePlayer(STATUS_WIN_PLAYER_COUNT[GetPrepareStatus()]);
		for (auto& player : playerVec)
		{
			player->SetStatus(CS_8_SELECT_PREPARE);
			vec.push_back(player);
		}
	}
	m_Service.SetTop16Table(vec);
	ClearGroup();
	GroupPlayerByRank(static_cast<ChampionStatus>(GetPrepareStatus() + 1));
	m_Service.Sync16Table(nullptr);

	USChampionStatus::OnBattleEnd();

	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_CHAMPION);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_CHAMPION);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet;
				for (auto& player : vec)
				{
					optionSet.insert(player->GetID());
				}
				project->SetOption(optionSet);
				project->SetGroupID(1);
				project->InsertToDB();
			}
		}
	}

	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_BATTLE_COUNT);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_BATTLE_COUNT);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet{ 4555, 5665, 6675 };
				project->SetOption(optionSet);
				project->SetGroupID(1);
				project->InsertToDB();
			}
		}
	}

	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_CHAMPION_BATTLE_SCORE);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_CHAMPION_BATTLE_SCORE);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet{ 30, 31, 32 };
				project->SetOption(optionSet);
				project->SetGroupID(1);
				project->InsertToDB();
			}
		}
	}

	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_1V1);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_1V1);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet;
				auto id = Avalon::Random::RandBetween(data->groupStart, data->groupEnd);
				for (auto& player : m_GroupMap[id].GetPlayerVec())
				{
					optionSet.insert(player->GetID());
				}
				project->SetGroupID(id);
				project->SetOption(optionSet);
				project->InsertToDB();
			}
		}
	}

}

UInt32 Select16Status::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncScoreRankTop(player);
	return ErrorCode::SUCCESS;
}

UInt32 Select8Status::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEnd3Win(winner, loser, result, info);
}

void Select8Status::OnPrepareStart()
{
	auto playerVec = GetStatusPlayer(ChampionStatus::CS_8_SELECT_PREPARE);
	for (auto& player : playerVec)
	{
		static_cast<ChampionPlayer*>(player)->SetBattleCount(0);
	}
	GroupPlayerByStatus(GetPrepareStatus());
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	//PushAllGroupPlayerToOneScene(GetPrepareStatus());
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

void Select8Status::OnBattleStart()
{
	USChampionStatus::OnBattleStart();
	OnAllGroupBattleStart();
	//LockOnlinePlayerForBattle();
}

void Select8Status::OnBattleEnd()
{
	ClearPlayerWinCount();
	ClearGroup();
	GroupPlayerByLastGroup(static_cast<ChampionStatus>(GetPrepareStatus() + 1));
	m_Service.Sync16Table(nullptr);
	USChampionStatus::OnBattleEnd();
	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_1V1);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_1V1);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet;
				auto id = Avalon::Random::RandBetween(data->groupStart, data->groupEnd);
				for (auto& player : m_GroupMap[id].GetPlayerVec())
				{
					optionSet.insert(player->GetID());
				}
				project->SetGroupID(id);
				project->SetOption(optionSet);
				project->InsertToDB();
			}
		}
	}
}

UInt32 Select8Status::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncKnockoutScore(player);
	return ErrorCode::SUCCESS;
}

UInt32 Select4Status::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEnd3Win(winner, loser, result, info);
}

void Select4Status::OnPrepareStart()
{
	GroupPlayerByStatus(GetPrepareStatus());
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	//PushAllGroupPlayerToOneScene(GetPrepareStatus());
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

void Select4Status::OnBattleStart()
{
	USChampionStatus::OnBattleStart();
	OnAllGroupBattleStart();
	//LockOnlinePlayerForBattle();
}

void Select4Status::OnBattleEnd()
{
	ClearPlayerWinCount();
	ClearGroup();
	GroupPlayerByLastGroup(static_cast<ChampionStatus>(GetPrepareStatus() + 1));
	m_Service.Sync16Table(nullptr);
	USChampionStatus::OnBattleEnd();
	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_1V1);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_1V1);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet;
				auto id = Avalon::Random::RandBetween(data->groupStart, data->groupEnd);
				for (auto& player : m_GroupMap[id].GetPlayerVec())
				{
					optionSet.insert(player->GetID());
				}
				project->SetGroupID(id);
				project->SetOption(optionSet);
				project->InsertToDB();
			}
		}
	}
}

UInt32 Select4Status::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncKnockoutScore(player);
	return ErrorCode::SUCCESS;
}

UInt32 Select2Status::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEnd3Win(winner, loser, result, info);
}

void Select2Status::OnPrepareStart()
{
	GroupPlayerByStatus(GetPrepareStatus());
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	//PushAllGroupPlayerToOneScene(GetPrepareStatus());
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

void Select2Status::OnBattleStart()
{
	USChampionStatus::OnBattleStart();
	OnAllGroupBattleStart();
	//LockOnlinePlayerForBattle();
}

void Select2Status::OnBattleEnd()
{
	ClearPlayerWinCount();
	ClearGroup();
	GroupPlayerByLastGroup(static_cast<ChampionStatus>(GetPrepareStatus() + 1));
	m_Service.Sync16Table(nullptr);
	USChampionStatus::OnBattleEnd();
	{
		auto data = ChampionGambleDataEntryMgr::Instance()->GetDataEntryByStateAndType(GetBattleStatus(), GT_1V1);
		if (data != nullptr)
		{
			auto project = USGambleMgr::Instance()->GenProject(data->GetKey(), GT_1V1);
			if (project != nullptr)
			{
				std::set<UInt64> optionSet;
				auto id = Avalon::Random::RandBetween(data->groupStart, data->groupEnd);
				for (auto& player : m_GroupMap[id].GetPlayerVec())
				{
					optionSet.insert(player->GetID());
				}
				project->SetGroupID(id);
				project->SetOption(optionSet);
				project->InsertToDB();
			}
		}
	}
}

UInt32 Select2Status::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncKnockoutScore(player);
	return ErrorCode::SUCCESS;
}

UInt32 Select1Status::OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info)
{
	return USChampionStatus::OnPlayerBattleEnd3Win(winner, loser, result, info);
}

void Select1Status::OnPrepareStart()
{
	GroupPlayerByStatus(GetPrepareStatus());
	GenScene(STATUS_GROUP[GetPrepareStatus()]);
	//PushAllGroupPlayerToOneScene(GetPrepareStatus());
	PushAllGroupPlayerToScene(GetPrepareStatus());
}

void Select1Status::OnBattleStart()
{
	USChampionStatus::OnBattleStart();
	OnAllGroupBattleStart();
	//LockOnlinePlayerForBattle();
}

void Select1Status::OnBattleEnd()
{
	ClearPlayerWinCount();
	ClearGroup();
	m_Service.Sync16Table(nullptr);
	USChampionStatus::OnBattleEnd();
}

UInt32 Select1Status::OnPlayerJoinRace(ChampionPlayer * player)
{
	USChampionStatus::OnPlayerJoinRace(player);
	SyncKnockoutScore(player);
	return ErrorCode::SUCCESS;
}

bool ChampionGroupInfoMgr::Init()
{
	return true;
}

void ChampionGroupInfoMgr::Clear()
{
	m_GroupStatus.clear();
	SyncAllGroupInfo();
}

void ChampionGroupInfoMgr::Push(ChampionGroupStatus& groupInfo)
{
	m_GroupStatus[groupInfo.groupID] = groupInfo;
	SyncOneGroupInfo(groupInfo.groupID);
}

void ChampionGroupInfoMgr::ChangeStatus(UInt32 groupID, UInt8 status)
{
	auto groupInfoPair = m_GroupStatus.find(groupID);
	if (groupInfoPair == m_GroupStatus.end())
	{
		return;
	}
	groupInfoPair->second.status = status;
	SyncOneGroupInfo(groupID);
}

void ChampionGroupInfoMgr::OnChanged(UInt32 groupID, ChampionPlayer * roleA, ChampionPlayer * roleB)
{
	if (roleA == nullptr || roleB == nullptr)
	{
		ErrorLogStream << "role not exists" << LogStream::eos;
		return;
	}
	if (m_GroupStatus.count(groupID) == 0)
	{
		ErrorLogStream << "group:" << groupID << "not exists" << LogStream::eos;
		return;
	}
	auto& groupInfo = m_GroupStatus[groupID];
	if (groupInfo.roleA == roleA->GetID())
	{
		groupInfo.scoreA = roleA->GetWinCount();
		groupInfo.scoreB = roleB->GetWinCount();
	}
	else if (groupInfo.roleA == roleB->GetID())
	{
		groupInfo.scoreA = roleB->GetWinCount();
		groupInfo.scoreB = roleA->GetWinCount();
	}
	else
	{
		ErrorLogStream << "group:" << groupID << "roleA:" << roleA->GetID() << "roleB:" << roleB->GetID() << LogStream::eos;
	}
	SyncOneGroupInfo(groupID);
}

void ChampionGroupInfoMgr::SyncAllGroupInfo()
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	//auto service = GET_UNION_SERVICE(ChampionService);
	if (service == nullptr)
	{
		return;
	}
	CLProtocol::SceneChampionAllGroupRes sync;
	for (auto& groupPair : m_GroupStatus)
	{
		sync.groups.push_back(groupPair.second);
	}
	service->BroadcastToZone(sync, ST_SCENE);
}

void ChampionGroupInfoMgr::SyncOneGroupInfo(UInt32 groupID)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	if (m_GroupStatus.count(groupID) == 0)
	{
		return;
	}
	CLProtocol::SceneChampionGroupStatusSync sync;
	sync.group = m_GroupStatus[groupID];
	service->BroadcastToZone(sync, ST_SCENE);
}

ChampionGroupStatus& ChampionGroupInfoMgr::GetGroupInfo(UInt32 groupID)
{
	return m_GroupStatus[groupID];
}
