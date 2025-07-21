#include "PremiumLeagueMgr.h"
#include <CLErrorCode.h>
#include <CLPremiumLeagueDataEntry.h>
#include <CLGameSessionProtocol.h>
#include <CLRecordClient.h>
#include <CLPremiumLeagueProtocol.h>
#include <CLItemProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <CLMsgParser.h>
#include <CLHonor.h>

#include "WSPlayer.h"
#include "WSItemTransaction.h"
#include "WSRouter.h"
#include "WSSortListMgr.h"
#include "WSPlayerMgr.h"
#include "WSSelectInitDataCallback.h"
#include "GameParamMgr.h"
#include "AnnouncementMgr.h"
#include "MailBox.h"
#include "GuildMember.h"
#include "GuildMgr.h"

/**
*@brief 加载赏金联赛成员回调
*/
class SelectPremiumLeagueGamerCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return PremiumLeagueMgr::Instance()->OnSelectGamerRet(this);
	}
};

/**
*@brief 加载赏金联赛成员回调
*/
class SelectPremiumLeagueBattleCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return PremiumLeagueMgr::Instance()->OnSelectBattleRet(this);
	}
};

/**
*@brief 赏金联赛报名
*/
class PremiumLeagueEnrollTransaction : public ItemTransaction
{
public:
	PremiumLeagueEnrollTransaction(WSPlayer *player, bool reEnroll = false)
		: ItemTransaction(player), m_IsReEnroll(reEnroll) {}
	~PremiumLeagueEnrollTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		UInt32 result = m_IsReEnroll ? PremiumLeagueMgr::Instance()->OnCheckPlayerReEnrollRet(player) : PremiumLeagueMgr::Instance()->OnCheckPlayerEnrollRet(player);
		if (result == ErrorCode::SUCCESS)
		{
			RemoveItem(GetReason(m_IsReEnroll ? LogSourceType::SOURCE_TYPE_PREMIUM_LEAGUE_REENROLL : LogSourceType::SOURCE_TYPE_PREMIUM_LEAGUE_ENROLL));
		}
		else
		{
			UnLock();
		}

		// 返回给客户端
		CLProtocol::WorldPremiumLeagueEnrollRes res;
		res.result = result;
		player->SendProtocol(res);
	}

	void OnCheckFailed(WSPlayer *player)
	{
		// 返回给客户端
		CLProtocol::WorldPremiumLeagueEnrollRes res;
		res.result = ErrorCode::PREMIUM_LEAGUE_ENROLL_MONEY_NOT_ENOUGH;
		player->SendProtocol(res);

		UnLock();
	}

private:
	bool		m_IsReEnroll;

};

PremiumLeagueMgr::PremiumLeagueMgr()
{
	m_LoadFinish = false;
	memset(m_FinalBattleGamer, 0, sizeof(m_FinalBattleGamer));
	m_SaveTimer.SetInterval(Avalon::Time::MSECS_OF_SEC);
	m_DailyTimer.SetTime(0);
}

PremiumLeagueMgr::~PremiumLeagueMgr()
{

}

bool PremiumLeagueMgr::OnInit()
{
	// 报名阶段
	/*{
		PremiumLeaguePoint point;
		point.status = PLS_INIT;
		point.startTime = (UInt32)CURRENT_TIME.Sec();
		point.endTime = (UInt32)CURRENT_TIME.Sec() + 10;
		m_TimePoints[point.status] = point;
	}

	// 报名阶段
	{
		PremiumLeaguePoint point;
		point.status = PLS_ENROLL;
		point.startTime = (UInt32)CURRENT_TIME.Sec() + 10;
		point.endTime = (UInt32)CURRENT_TIME.Sec() + 30;
		m_TimePoints[point.status] = point;
	}

	// 预选赛阶段
	{
		PremiumLeaguePoint point;
		point.status = PLS_PRELIMINAY;
		point.startTime = (UInt32)CURRENT_TIME.Sec() + 30;
		point.endTime = (UInt32)CURRENT_TIME.Sec();
		m_TimePoints[point.status] = point;
	}*/

	_CountStatusPoint();
	_CountCurrentPoint();

	// 注册回调
	REGISTER_STATUS_CALLBACK(PLS_ENROLL, &PremiumLeagueMgr::_OnEnrollStart, &PremiumLeagueMgr::_OnEnrollEnd);
	REGISTER_STATUS_CALLBACK(PLS_PRELIMINAY, &PremiumLeagueMgr::_OnPreliminayStart, &PremiumLeagueMgr::_OnPreliminayEnd);
	REGISTER_STATUS_CALLBACK(PLS_FINAL_EIGHT, &PremiumLeagueMgr::_OnFinalEigthStart, &PremiumLeagueMgr::_OnFinalEigthEnd);
	REGISTER_STATUS_CALLBACK(PLS_FINAL_FOUR, &PremiumLeagueMgr::_OnFinalFourStart, &PremiumLeagueMgr::_OnFinalFourEnd);
	REGISTER_STATUS_CALLBACK(PLS_FINAL, &PremiumLeagueMgr::_OnFinalBattleStart, &PremiumLeagueMgr::_OnFinalBattleEnd);
	REGISTER_STATUS_CALLBACK(PLS_FINAL_WAIT_CLEAR, &PremiumLeagueMgr::_OnNousefulStatusCallback, &PremiumLeagueMgr::_OnAllOver);

	// 加载数据
	// 加载玩家数据
	{
		CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_premium_league_gamer");
		CLRecordClient::Instance()->SendCommand(cmd, new SelectPremiumLeagueGamerCallback());
	}

	return true;
}

void PremiumLeagueMgr::OnLoadFinish()
{
	m_LoadFinish = true;

	auto destStatus = _CountCurrentPoint();
	if (destStatus == m_Status)
	{
		return;
	}

	InfoLogStream << "premium league adjest status, (" << (UInt8)m_Status << ") to (" << (UInt8)destStatus << ")." << LogStream::eos;

	// 加载出来的状态和现在正确的状态不一致，需要调整至现在正确的状态
	if (destStatus < m_Status)
	{
		for (int status = m_Status + 1; status <= destStatus + PLS_NUM; status++)
		{
			ChangeStatus((PremiumLeagueStatus)(status % PLS_NUM));
		}
	}
	else
	{
		for (int status = m_Status + 1; status <= destStatus; status++)
		{
			ChangeStatus((PremiumLeagueStatus)status);
		}
	}
	
}

void PremiumLeagueMgr::OnDayChanged()
{
	// 清除所有数据
	_Clear();
	// 重新同步奖金池信息
	SyncRewardPool(NULL);
	// 计算时间点
	_CountStatusPoint();
}

void PremiumLeagueMgr::OnTick(const Avalon::Time& now)
{
	auto& curPoint = m_TimePoints[m_Status];

	// 判断当前状态有没有过期,如果当前状态已经过期了，就说明该进入下一状态了，状态都是连续的
	if (m_LoadFinish && ((m_Status == PLS_INIT && CURRENT_TIME.Sec() >= m_TimePoints[PLS_ENROLL].startTime) || (m_Status != PLS_INIT && CURRENT_TIME.Sec() >= curPoint.endTime)))
	{
		PremiumLeagueStatus targetStatus;
		if (m_Status == PLS_NUM - 1)
		{
			targetStatus = PLS_INIT;
		}
		else
		{
			targetStatus = (PremiumLeagueStatus)((UInt8)m_Status + 1);
		}
		ChangeStatus(targetStatus);
	}

	if (m_SaveTimer.IsTimeout(now))
	{
		for (auto itr : m_Gamers)
		{
			auto gamer = itr.second;
			if (gamer)
			{
				gamer->SaveToDB();

				if (gamer->IsDirty())
				{
					gamer->SyncSelfInfo();
				}
			}
		}
	}

	if (m_DailyTimer.IsTimeout(now))
	{
		OnDayChanged();
	}
}

void PremiumLeagueMgr::OnFinal()
{

}

bool PremiumLeagueMgr::OnSelectGamerRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		auto* gamer = new PremiumLeagueGamer();
		gamer->SetID(callback->GetKey());
		gamer->SetName(callback->GetData("name").GetString());
		gamer->SetDBData(callback);
		AddGamer(gamer);

		if (gamer->GetRanking() > 0 && gamer->GetRanking() <= FINAL_BATTLE_GAMER_NUM)
		{
			m_FinalBattleGamer[gamer->GetRanking() - 1] = gamer;
		}
	}

	InfoLogStream << "load premium league gamer data finish..." << LogStream::eos;

	// 加载淘汰赛信息
	{
		CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_premium_league_battle");
		CLRecordClient::Instance()->SendCommand(cmd, new SelectPremiumLeagueBattleCallback());
	}

	return true;
}

bool PremiumLeagueMgr::OnSelectBattleRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		PremiumLeagueBattle battle;
		battle.guid = callback->GetKey();
		battle.raceId = callback->GetData("race_id");
		battle.type = callback->GetData("type");
		battle.fighter1.id = callback->GetData("role_id1");
		battle.fighter1.name = callback->GetData("role_name1").GetString();
		battle.fighter1.occu = callback->GetData("role_occu1");
		battle.fighter2.id = callback->GetData("role_id2");
		battle.fighter2.name = callback->GetData("role_name2").GetString();
		battle.fighter2.occu = callback->GetData("role_occu2");
		battle.isEnd = callback->GetData("is_end");
		battle.winnerId = callback->GetData("winner");
		AddBattle(battle);

	}

	InfoLogStream << "load premium league gamer data finish..." << LogStream::eos;
	OnLoadFinish();
	return true;
}

PremiumLeagueStatusInfo PremiumLeagueMgr::GetStatusInfo() const
{
	PremiumLeagueStatusInfo info;
	info.status = (UInt8)GetStatus();
	info.startTime = m_TimePoints[GetStatus()].startTime;
	info.endTime = m_TimePoints[GetStatus()].endTime;
	return info;
}

void PremiumLeagueMgr::SyncRewardPool(WSPlayer* player)
{
	CLProtocol::WorldPremiumLeagueRewardPoolRes sync;
	sync.enrollPlayerNum = (UInt32)m_Gamers.size();
	sync.money = GetRewardPool();
	for (int i = 0; i < PL_REWARD_NUM; i++)
	{
		sync.rewards[i] = _GetReward((PremiumLeagueRewardType)i);
	}

	if (player)
	{
		player->SendProtocol(sync);
	}
	else
	{
		Router::BroadcastToPlayer(sync);
	}
}

UInt32 PremiumLeagueMgr::GetRewardPool()
{
	return 2000 + m_Gamers.size() * 20;
}

std::vector<PremiumLeagueBattleGamer> PremiumLeagueMgr::GetFinalBattleGamer() const
{
	std::vector<PremiumLeagueBattleGamer> gamers;
	for (auto gamer : m_FinalBattleGamer)
	{
		if (gamer)
		{
			gamers.push_back(gamer->GetBattleGamerInfo());
		}
	}

	return gamers;
}

void PremiumLeagueMgr::Broadcast(Avalon::Protocol& protocol)
{
	for (auto itr : m_Gamers)
	{
		auto gamer = itr.second;
		if (gamer)
		{
			gamer->SendProtocol(protocol);
		}
	}
}

void PremiumLeagueMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	// 同步赏金联赛状态
	{
		CLProtocol::WorldPremiumLeagueSyncStatus sync;
		sync.info = PremiumLeagueMgr::Instance()->GetStatusInfo();
		player->SendProtocol(sync);
	}

	// 同步奖金池信息
	{
		PremiumLeagueMgr::Instance()->SyncRewardPool(player);
	}
	
	// 同步淘汰赛玩家信息
	if (GetStatus() >= PLS_FINAL_EIGHT_PREPARE)
	{
		CLProtocol::WorldPremiumLeagueBattleGamerInit init;
		init.gamers = GetFinalBattleGamer();
		player->SendProtocol(init);

		CLProtocol::WorldPremiumLeagueBattleInfoInit init2;
		for (auto& battle : m_PremiumBattles)
		{
			init2.battles.push_back(battle);
		}
		player->SendProtocol(init2);
	}

	auto gamer = FindGamer(player->GetID());
	if (gamer)
	{
		gamer->SetPlayer(player);
		gamer->SyncSelfInfo();
	}
	else
	{
		// 同步一个空的数据
		CLProtocol::WorldPremiumLeagueSelfInfo sync;
		player->SendProtocol(sync);
	}
}

void PremiumLeagueMgr::OnPlayerOffline(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	auto gamer = FindGamer(player->GetID());
	if (gamer)
	{
		gamer->SetPlayer(NULL);
	}
}

UInt32 PremiumLeagueMgr::OnPlayerEnroll(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_PLAYER_NOT_EXIST;
	}

	DebugLogStream << "player(" << player->GetID() << ") enroll premium league." << LogStream::eos;

	UInt32 result = _CheckPlayerCanEnroll(player);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}


	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_ENROLL_COST_ITEM);
	UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_ENROLL_COST);
	auto trans = new PremiumLeagueEnrollTransaction(player);
	trans->SetItem(itemId, num);
	player->BeginItemTransaction(trans);

	return ErrorCode::SUCCESS;
}

UInt32 PremiumLeagueMgr::OnPlayerReEnroll(WSPlayer* player)
{
	InfoLogStream << "player(" << player->GetID() << ") re enroll premium league." << LogStream::eos;

	UInt32 result = _CheckPlayerCanReEnroll(player);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_REENROLL_COST_ITEM);
	UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_REENROLL_COST);
	auto trans = new PremiumLeagueEnrollTransaction(player, true);
	trans->SetItem(itemId, num);
	player->BeginItemTransaction(trans);

	return ErrorCode::SUCCESS;
}

UInt32 PremiumLeagueMgr::OnPlayerStartPreliminay(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::PREMIUM_LEAGUE_PRELIMINAY_NOT_ENROLL;
	}

	auto gamer = FindGamer(player->GetID());
	if (!gamer)
	{
		// 没有报名
		return ErrorCode::PREMIUM_LEAGUE_PRELIMINAY_NOT_ENROLL;
	}

	// 不是预选赛状态不能参赛
	if (GetStatus() != PLS_PRELIMINAY)
	{
		return ErrorCode::PREMIUM_LEAGUE_PRELIMINAY_NOT_START;
	}

	// 这里要判断玩家是不是已经输了
	if (gamer->IsLose())
	{
		return ErrorCode::PREMIUM_LEAGUE_PRELIMINAY_ALREADY_LOSE;
	}

	CLProtocol::MatchServerPremiumLeagueMatchReq req;
	req.info.accid = player->GetAccId();
	req.info.name = player->GetName();
	req.info.id = player->GetID();
	req.winStreak = gamer->GetWinNum();
	req.seasonLevel = player->GetSeasonLv();
	Router::SendToMatch(req);

	return ErrorCode::SUCCESS;
}

void PremiumLeagueMgr::OnPreliminayRaceEnd(CLProtocol::MatchServerPremiumLeagueRaceEnd raceEnd)
{
	DebugLogStream << "race(" << raceEnd.raceId << ") winner(" << raceEnd.winner.id << ", " << raceEnd.winner.name << ") loser(" << raceEnd.loser.id << ", "
					<< raceEnd.loser.name << ") result(" << raceEnd.result << ") preliminay race end" << LogStream::eos;

	// 已经过了预选赛了
	if (GetStatus() != PLS_PRELIMINAY)
	{
		InfoLogStream << "recv race(" << raceEnd.raceId << ") end, time over, current status(" << (UInt8)GetStatus() << ")." << LogStream::eos;
		return;
	}

	auto winnerId = raceEnd.winner;
	auto winner = FindGamer(raceEnd.winner.id);
	auto loserId = raceEnd.loser;
	auto loser = FindGamer(raceEnd.loser.id);

	if (raceEnd.result != 0)
	{
		// 平局或者结果异常，发送结算给客户端
		CLProtocol::WorldPremiumLeagueRaceEnd ret;
		ret.result = raceEnd.result;
		ret.isPreliminay = 1;
		if (winner)
		{
			ret.oldScore = winner->GetScore();
			ret.newScore = winner->GetScore();
			winner->SendProtocol(ret);

			if (winner->GetPlayer())
			{
				winner->GetPlayer()->SendPvpEndUdpLog(ZONE_ID, loserId.id, PK_PREMIUM_LEAGUE_PRELIMINAY, raceEnd.result == resultBitmask_invalid ? PKRR_INVALID : PKRR_DOGFALL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, raceEnd.dungeonID, raceEnd.usedTime);
			}
		}
		
		if (loser)
		{
			ret.oldScore = loser->GetScore();
			ret.newScore = loser->GetScore();
			loser->SendProtocol(ret);

			if (loser->GetPlayer())
			{
				loser->GetPlayer()->SendPvpEndUdpLog(ZONE_ID, winnerId.id, PK_PREMIUM_LEAGUE_PRELIMINAY, raceEnd.result == resultBitmask_invalid ? PKRR_INVALID : PKRR_DOGFALL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, raceEnd.dungeonID, raceEnd.usedTime);
			}
		}

		return;
	}

	PremiumLeagueRecordFighter loseFighter;
	loseFighter.id = raceEnd.loser.id;
	loseFighter.name = raceEnd.loser.name;

	
	if (loser)
	{
		CLProtocol::WorldPremiumLeagueRaceEnd ret;
		ret.result = resultBitmask_lose;
		ret.isPreliminay = 1;
		ret.oldScore = loser->GetScore();

		loser->SetRemainHp(0);
		loser->SetRemainMp(0);
		UInt32 gotScore = _CountLoseGotScore();
		loser->SetScore(loser->GetScore() + gotScore);
		loser->SetWinTime((UInt32)CURRENT_TIME.Sec());
		loser->UpdateSortList();

		loseFighter.winStreak = loser->GetWinNum();
		loseFighter.gotScore = gotScore;

		ret.newScore = loser->GetScore();
		ret.getHonor = CLHonor::PvpHonorReward(PK_PREMIUM_LEAGUE_PRELIMINAY, PKRR_LOSE);
		loser->SendProtocol(ret);

		SendHonorToScene(loser->GetID(), PK_PREMIUM_LEAGUE_BATTLE, ret.getHonor);

		InfoLogStream << "race(" << raceEnd.raceId << ") loser(" << loser->GetID() << ") hp(" << loser->GetRemainHp() << ") mp(" << loser->GetRemainMp() << ")." << LogStream::eos;

		if (loser->GetPlayer())
		{
			loser->GetPlayer()->SendPvpEndUdpLog(ZONE_ID, winnerId.id, PK_PREMIUM_LEAGUE_PRELIMINAY, PKRR_LOSE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, raceEnd.dungeonID, raceEnd.usedTime);
		}
	}

	PremiumLeagueRecordFighter winFighter;
	winFighter.id = raceEnd.winner.id;
	winFighter.name = raceEnd.winner.name;

	if (winner)
	{
		CLProtocol::WorldPremiumLeagueRaceEnd ret;
		ret.result = resultBitmask_win;
		ret.isPreliminay = 1;
		ret.oldScore = winner->GetScore();
		if (raceEnd.winner.remainHp > winner->GetRemainHp())
		{
			raceEnd.winner.remainHp = winner->GetRemainHp();
		}

		if (raceEnd.winner.remainHp + 200 > HP_MP_FULL_VALUE)
		{
			winner->SetRemainHp(HP_MP_FULL_VALUE);
		}
		else
		{
			winner->SetRemainHp(raceEnd.winner.remainHp + 200);
		}
	
		winner->SetRemainMp(HP_MP_FULL_VALUE);
		winner->SetWinNum(winner->GetWinNum() + 1);
		winner->SetWinTime((UInt32)CURRENT_TIME.Sec());
		UInt32 gotScore = _CountWinGotScore(winner->GetWinNum(), loser ? loser->GetWinNum() : 0);
		winner->SetScore(winner->GetScore() + gotScore);
		winner->UpdateSortList();

		winFighter.winStreak = winner->GetWinNum();
		winFighter.gotScore = gotScore;
		winFighter.totalScore = winner->GetScore();

		ret.newScore = winner->GetScore();

		// 预选赛奖金
		auto maxRewardNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_TOTAL_RACE_MAX_REWARD_NUM);
		auto rewardItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_RACE_REWARD_ITEM);
		auto everyRaceMaxRewardNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_EVERY_RACE_MAX_REWARD_NUM);
		auto remainRewardNum = winner->GetPreliminayRewardNum() < maxRewardNum ? maxRewardNum - winner->GetPreliminayRewardNum() : 0;
		if (remainRewardNum > 0)
		{
			everyRaceMaxRewardNum = everyRaceMaxRewardNum < remainRewardNum ? everyRaceMaxRewardNum : remainRewardNum;

			auto num = everyRaceMaxRewardNum;

			auto itemData = ItemDataEntryMgr::Instance()->FindEntry(rewardItemId);
			if (itemData)
			{
				ret.preliminayRewardNum = num;
				winner->AddPreliminayRewardNumNum(num);
				AddTotalPreliminayRewardNum(num);

				InfoLogStream << "race(" << raceEnd.raceId << ") winner(" << raceEnd.winner.id << ", " << raceEnd.winner.name << ") got preliminay reward("
					<< rewardItemId << ") num(" << num << ") total(" << winner->GetPreliminayRewardNum() << ")." << LogStream::eos;

				if (winner->GetPlayer())
				{
					winner->GetPlayer()->AddMoneyItem(GetReason(SOURCE_TYPE_PREMIUM_LEAGUE_PRELIMINAY_REWARD).c_str(), itemData->subType, num);
				}
				else
				{
					// 发邮件
					// 发送邮件
					std::string title = SysNotifyMgr::Instance()->MakeString(9310);
					std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
					std::string content = SysNotifyMgr::Instance()->MakeString(9311);
					ItemReward reward;
					reward.id = rewardItemId;
					reward.num = num;
					ItemRewardVec rewards;
					rewards.push_back(reward);
					MailBox::SendSysMail(winner->GetID(), sender, title, content, GetReason(SOURCE_TYPE_PREMIUM_LEAGUE_FINAL_EIGHT_NOTIFY), rewards);
				}
			}
		}

		ret.getHonor = CLHonor::PvpHonorReward(PK_PREMIUM_LEAGUE_PRELIMINAY, PKRR_WIN);
		winner->SendProtocol(ret);

		SendHonorToScene(winner->GetID(), PK_PREMIUM_LEAGUE_BATTLE, ret.getHonor);

		InfoLogStream << "race(" << raceEnd.raceId << ") winner(" << winner->GetID() << ") hp(" << winner->GetRemainHp() << ") mp(" << winner->GetRemainMp() << ")." << LogStream::eos;

		if (winner->GetPlayer())
		{
			winner->GetPlayer()->SendPvpEndUdpLog(ZONE_ID, loserId.id, PK_PREMIUM_LEAGUE_PRELIMINAY, PKRR_WIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, raceEnd.dungeonID, raceEnd.usedTime);
		}
	}
	else
	{
		winFighter.winStreak = 1;
		winFighter.gotScore = _CountWinGotScore(1, loser ? loser->GetWinNum() : 0);
		winFighter.totalScore = winFighter.gotScore;
	}

	// 对战记录相关
	{
		// 插入对战记录
		auto entry = new PremiumLeagueRecordEntry();
		entry->SetWinner(winFighter);
		entry->SetLoser(loseFighter);
		m_BattleRecord.AddEntry(entry);
		
		DebugLogStream << "premium league add battle record winner(" << winFighter.id << ", " << winFighter.name << ") loser(" << loseFighter.id << ", " << loseFighter.name << ")." << LogStream::eos;

		// 广播给所有玩家
		CLProtocol::WorldPremiumLeagueBattleRecordSync sync;
		sync.record = *entry;
		Broadcast(sync);
	}
}

void PremiumLeagueMgr::OnBattleRaceEnd(CLProtocol::MatchServerPremiumLeagueRaceEnd raceEnd)
{
	DebugLogStream << "race(" << raceEnd.raceId << ") winner(" << raceEnd.winner.id << ") loser(" << raceEnd.loser.id << ") result(" << raceEnd.result << ") preliminay race end" << LogStream::eos;

	// 已经过了预选赛了
	if (GetStatus() <= PLS_FINAL_EIGHT_PREPARE || GetStatus() > PLS_FINAL)
	{
		InfoLogStream << "recv race(" << raceEnd.raceId << ") end, time over, current status(" << (UInt8)GetStatus() << ")." << LogStream::eos;

		// 过了预选赛，发送一个无效局的结算给玩家
		std::vector<ObjID_t> gamerIds;
		gamerIds.push_back(raceEnd.winner.id);
		gamerIds.push_back(raceEnd.loser.id);

		for (auto gamerId : gamerIds)
		{
			auto player = WSPlayerMgr::Instance()->FindNormalPlayer(gamerId);
			if (!player)
			{
				continue;
			}

			CLProtocol::WorldPremiumLeagueRaceEnd ret;
			ret.result = resultBitmask_invalid;
			ret.isPreliminay = 1;
			player->SendProtocol(ret);
		}
		
		return;
	}

	if (raceEnd.result != 0)
	{
		// 平局或者结果异常，排名在前面的玩家获得胜利
		auto gamer1 = FindGamer(raceEnd.winner.id);
		auto gamer2 = FindGamer(raceEnd.winner.id);
		if (gamer1 == NULL || gamer2 == NULL)
		{
			ErrorLogStream << "race(" << raceEnd.raceId << ") can't find gamers." << LogStream::eos;
			return;
		}

		if (gamer1 == NULL)
		{
			std::swap(raceEnd.winner, raceEnd.loser);
		}
		else if (gamer1 && gamer2 && gamer1->GetRanking() > gamer2->GetRanking())
		{
			std::swap(raceEnd.winner, raceEnd.loser);
		}
	}

	auto winnerId = raceEnd.winner;
	auto loserId = raceEnd.loser;

	auto winner = FindGamer(raceEnd.winner.id);
	if (winner)
	{
		winner->OnBattleWin();
		InfoLogStream << "race(" << raceEnd.raceId << ") status(" << (UInt8)GetStatus() << ") winner(" << winner->GetID() << ")." << LogStream::eos;
	}

	auto loser = FindGamer(raceEnd.loser.id);
	if (loser)
	{
		loser->OnBattleLose();
		InfoLogStream << "race(" << raceEnd.raceId << ") status(" << (UInt8)GetStatus() << ") loser(" << loser->GetID() << ")." << LogStream::eos;
	}

	if (_IsFinishCurrentRollBattle())
	{
		_SendNextRollPrepareAnnouncement();

		// 如果决赛打完了，强制切换下状态
		if (GetStatus() == PLS_FINAL)
		{
			ChangeStatus(PLS_FINAL_WAIT_CLEAR);
		}
	}
}

void PremiumLeagueMgr::SendHonorToScene(UInt64 roleId, UInt32 pvpType, UInt32 honor)
{
	std::string reason = GetReason(SOURCE_TYPE_PREMIUM_LEAGUE_REWARD, pvpType);

	CLProtocol::WorldPremiumLeagueEndToScene toScene;
	toScene.roleId = roleId;
	toScene.pvpType = pvpType;
	toScene.reason = reason;
	toScene.honor = honor;
	Router::BroadcastToScene(toScene);
}

UInt32 PremiumLeagueMgr::OnCheckPlayerEnrollRet(WSPlayer* player)
{
	DebugLogStream << "player(" << player->GetID() << ") enroll premium league ret." << LogStream::eos;

	UInt32 result = _CheckPlayerCanEnroll(player);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	// 新建报名玩家
	auto gamer = new PremiumLeagueGamer();
	gamer->Init(player);

	m_Gamers[player->GetID()] = gamer;
	gamer->OnEnroll();

	gamer->InsertToDB();

	// 插入到排行榜
	gamer->UpdateSortList();

	// 广播新的奖金池
	SyncRewardPool(NULL);

	player->SyncEventToScene(SET_PREMIUM_LEAGUE_ENROLL);

	return ErrorCode::SUCCESS;
}

UInt32 PremiumLeagueMgr::OnCheckPlayerReEnrollRet(WSPlayer* player)
{
	DebugLogStream << "player(" << player->GetID() << ") re enroll premium league ret." << LogStream::eos;

	UInt32 result = _CheckPlayerCanReEnroll(player);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	// 重新报名玩家
	auto gamer = FindGamer(player->GetID());
	if (!gamer)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_PLAYER_NOT_EXIST;
	}

	gamer->OnEnroll();
	gamer->UpdateSortList();

	return ErrorCode::SUCCESS;
}

void PremiumLeagueMgr::ChangeStatus(PremiumLeagueStatus status)
{
	if (status == m_Status)
	{
		return;
	}

	InfoLogStream << "premium league change status from (" << (UInt8)m_Status << ") to (" << (UInt8)status << ")." << LogStream::eos;

	auto oldStatus = m_Status;
	m_Status = status;
	GameParamMgr::Instance()->SetValue(PARAM_NAME_PREMIUM_LEAGUE_STATUS, (UInt8)GetStatus());

	// 同步当前状态给客户端
	CLProtocol::WorldPremiumLeagueSyncStatus sync;
	sync.info = PremiumLeagueMgr::Instance()->GetStatusInfo();
	Router::BroadcastToPlayer(sync);

	// 上一个状态结束了
	{
		auto itr = m_StatusCallbacks.find(oldStatus);
		if (itr != m_StatusCallbacks.end())
		{
			auto cb = itr->second;
			if (cb.onEnd != NULL)
			{
				cb.onEnd();
			}
		}
	}

	// 新的状态开始了
	{
		auto itr = m_StatusCallbacks.find(status);
		if (itr != m_StatusCallbacks.end())
		{
			auto cb = itr->second;
			if (cb.onStart != NULL)
			{
				cb.onStart();
			}
		}
	}
}

PremiumLeagueGamer* PremiumLeagueMgr::FindGamer(ObjID_t id)
{
	auto itr = m_Gamers.find(id);
	return itr == m_Gamers.end() ? NULL : itr->second;
}

void PremiumLeagueMgr::AddGamer(PremiumLeagueGamer* gamer)
{
	if (!gamer)
	{
		return;
	}

	m_Gamers[gamer->GetID()] = gamer;
}

void PremiumLeagueMgr::DelGamer(ObjID_t id)
{
	auto gamer = FindGamer(id);
	if (!gamer)
	{
		return;
	}

	for (UInt32 i = 0; i < sizeof(m_FinalBattleGamer) / sizeof(m_FinalBattleGamer[0]); i++) 
	{
		if (m_FinalBattleGamer[i] == gamer)
		{
			m_FinalBattleGamer[i] = NULL;
		}
	}

	delete gamer;
	m_Gamers.erase(id);
}

void PremiumLeagueMgr::_RegisterStatusCallback(PremiumLeagueStatus status, PremiumLeaguePointFunc onStartFunc, PremiumLeaguePointFunc onEndFunc)
{
	PremiumLeagueStatusCallback cb;
	cb.onStart = onStartFunc;
	cb.onEnd = onEndFunc;

	m_StatusCallbacks[status] = cb;
}

void PremiumLeagueMgr::_OnNousefulStatusCallback()
{

}

void PremiumLeagueMgr::_OnEnrollStart()
{
	InfoLogStream << "premium league enroll start..." << LogStream::eos;

	// 清除之前的数据
	_Clear();
	// 同步一遍赏金池
	SyncRewardPool(NULL);

	// 发送公告
	std::string systemLinkStr;
	MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_PREMIUM_LEAGUE, 0);
	AnnouncementMgr::Instance()->PublicAnnouncement(0, 47, UInt32(CURRENT_TIME.Sec()), systemLinkStr);
}

void PremiumLeagueMgr::_OnEnrollEnd()
{
	InfoLogStream << "premium league enroll end..." << LogStream::eos;

	// 没人报名，发送通知
	if (m_Gamers.empty())
	{
		// 发送公告
		AnnouncementMgr::Instance()->PublicAnnouncement(0, 56, UInt32(CURRENT_TIME.Sec()), _GetReward(PL_REWARD_NO_1));
	}
}

void PremiumLeagueMgr::_OnPreliminayStart()
{
	InfoLogStream << "premium league preliminay start..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	// 发送公告
	AnnouncementMgr::Instance()->PublicAnnouncement(0, 48, UInt32(CURRENT_TIME.Sec()));
}

void PremiumLeagueMgr::_OnPreliminayEnd()
{
	InfoLogStream << "premium league preliminay end..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	// 根据排行榜算出前8名
	class PremiumLeagueVisitor : public WSSortListVisitor
	{
	public:
		PremiumLeagueVisitor() { memset(rankingPlayer, 0, sizeof(rankingPlayer)); }
		~PremiumLeagueVisitor() {}

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			auto gamer = PremiumLeagueMgr::Instance()->FindGamer(entry.id);
			if (gamer)
			{
				gamer->SetRanking(ranking);
			}

			if (ranking > FINAL_BATTLE_GAMER_NUM)
			{
				return true;
			}

			rankingPlayer[ranking - 1] = entry.id;
			return true;
		}

		ObjID_t			rankingPlayer[FINAL_BATTLE_GAMER_NUM];
	};
	PremiumLeagueVisitor vistor;
	WSSortListMgr::Instance()->VisitSortList(SORTLIST_PREMIUM_LEAGUE, vistor);

	for (UInt32 i = 1; i <= sizeof(vistor.rankingPlayer) / sizeof(vistor.rankingPlayer[0]); i++)
	{
		auto roleId = vistor.rankingPlayer[i - 1];
		if (roleId == 0)
		{
			continue;
		}

		auto gamer = FindGamer(roleId);
		if (!gamer)
		{
			ErrorLogStream << "premium league can't find final eight gamer(" << roleId << ")." << LogStream::eos;
			continue;
		}

		gamer->SetRanking(i);
		gamer->SetNextRollPos(_GetFinalEightPosByRanking(i));
		gamer->SetBattleFlag(1);
		gamer->SetFinishRollBattle(false);
		gamer->SetRemainHp(HP_MP_FULL_VALUE);

		m_FinalBattleGamer[i - 1] = gamer;

		// 发送邮件
		std::string title = SysNotifyMgr::Instance()->MakeString(9016);
		std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
		std::string content = SysNotifyMgr::Instance()->MakeString(9017);
		ItemRewardVec rewards;
		MailBox::SendSysMail(gamer->GetID(), sender, title, content, GetReason(SOURCE_TYPE_PREMIUM_LEAGUE_FINAL_EIGHT_NOTIFY), rewards);
	}

	// 给9-16名发放荣誉奖励
	for (auto itr : m_Gamers)
	{
		auto gamer = itr.second;
		if (gamer && gamer->GetRanking() >= 9 && gamer->GetRanking() <= 16)
		{
			_SendHonorReward(PK_PREMIUM_LEAGUE_PRELIMINAY, gamer->GetID(), gamer->GetRanking());
		}
	}

	// 同步淘汰赛玩家信息
	{
		CLProtocol::WorldPremiumLeagueBattleGamerInit init;
		init.gamers = GetFinalBattleGamer();
		Router::BroadcastToPlayer(init);
	}
	
	// 发送公告
	AnnouncementMgr::Instance()->PublicAnnouncement(0, 49, UInt32(CURRENT_TIME.Sec()));
}

void PremiumLeagueMgr::_OnFinalEigthStart()
{
	InfoLogStream << "premium league final eight start..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	_StartBattle(FINAL_BATTLE_GAMER_NUM);
}

void PremiumLeagueMgr::_OnFinalEigthEnd()
{
	InfoLogStream << "premium league final eight end..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	_CountNextRollGamer(4);
}

void PremiumLeagueMgr::_OnFinalFourStart()
{
	InfoLogStream << "premium league final four start..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	_StartBattle(4);
}

void PremiumLeagueMgr::_OnFinalFourEnd()
{
	InfoLogStream << "premium league final four end..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	_CountNextRollGamer(2);
}

void PremiumLeagueMgr::_OnFinalBattleStart()
{
	InfoLogStream << "premium league final battle start..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		return;
	}

	_StartBattle(2);
}

void PremiumLeagueMgr::_OnFinalBattleEnd()
{
	InfoLogStream << "premium league final battle end..." << LogStream::eos;

	if (m_Gamers.empty())
	{
		_CountStatusPoint();
		return;
	}

	_CountNextRollGamer(1);

	// 5-8名
	std::string gamer_5_8;
	// 3-4名
	std::string gamer_3_4;
	// 亚军
	std::string gamer_2;
	// 冠军
	std::string gamer_1;
	UInt64 gamer1_guidId = 0;
	for (auto gamer : m_FinalBattleGamer)
	{
		if (!gamer)
		{
			continue;
		}

		UInt32 rank = 0;
		if (gamer->GetFinalBattleWinNum() == 3)
		{
			// 3胜，说明是冠军
			gamer_1 = gamer->GetName();
			_SendReward(gamer, PL_REWARD_NO_1);
			rank = 1;

			if (gamer->GetPlayer() != NULL)
			{
				gamer1_guidId = gamer->GetPlayer()->GetGuildID();
			}
			else
			{
				GuildMember* guildMember = GuildMgr::Instance()->FindGuildMember(gamer->GetID());
				if (guildMember != NULL)
				{
					gamer1_guidId = guildMember->GetGuildID();
				}
			}
			Guild* guild = NULL;
			if (gamer1_guidId != 0)
			{
				guild = GuildMgr::Instance()->FindGuildByID(gamer1_guidId);
			}
			if (guild != NULL)
			{
				std::vector<UInt32> conds(GuildHonorConditionType::GUILD_COND_MAX, 0);
				conds[GuildHonorConditionType::GUILD_COND_MEMBER] = 1;
				guild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_WUDAO, conds);
			}
		}
		else if (gamer->GetFinalBattleWinNum() == 2)
		{
			//2胜，说明是2名
			gamer_2 = gamer->GetName();
			_SendReward(gamer, PL_REWARD_NO_2);
			rank = 2;
		}
		else if (gamer->GetFinalBattleWinNum() == 1)
		{
			//1胜，说明是3-4名
			if (gamer_3_4.empty())
			{
				gamer_3_4 = gamer->GetName();
			}
			else
			{
				gamer_3_4 = gamer_3_4 + "," + gamer->GetName();
			}

			_SendReward(gamer, PL_REWARD_NO_3_4);
			rank = 4;
		}
		else
		{
			//0胜，说明是5-8名
			if (gamer_5_8.empty())
			{
				gamer_5_8 = gamer->GetName();
			}
			else
			{
				gamer_5_8 = gamer_5_8 + "," + gamer->GetName();
			}

			_SendReward(gamer, PL_REWARD_NO_5_8);
			rank = 8;
		}

		_SendHonorReward(PK_PREMIUM_LEAGUE_BATTLE, gamer->GetID(), rank);
	}

	InfoLogStream << "premium league champion(" << gamer_1 << ")." << LogStream::eos;

	// 发送公告
	{
		// 第一名奖励
		if (!gamer_1.empty())
		{

			AnnouncementMgr::Instance()->PublicAnnouncement(gamer1_guidId, 52, (UInt32)CURRENT_TIME.Sec(), gamer_1, _GetReward(PL_REWARD_NO_1));
		}

		// 第二名奖励
		if (!gamer_2.empty())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, 53, (UInt32)CURRENT_TIME.Sec(), gamer_2, _GetReward(PL_REWARD_NO_2));
		}

		// 3-4奖励
		if (!gamer_3_4.empty())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, 54, (UInt32)CURRENT_TIME.Sec(), gamer_3_4, _GetReward(PL_REWARD_NO_3_4));
		}

		// 5-8奖励
		if (!gamer_5_8.empty())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, 55, (UInt32)CURRENT_TIME.Sec(), gamer_5_8, _GetReward(PL_REWARD_NO_5_8));
		}
	}
}

void PremiumLeagueMgr::_OnAllOver()
{
	InfoLogStream << "premium league all over..." << LogStream::eos;

	// 计算下一次的时间
	_CountStatusPoint();
	// 清除所有数据
	_Clear();
	// 重新同步奖金池信息
	SyncRewardPool(NULL);
}

ObjID_t PremiumLeagueMgr::AddNewBattle(PremiumLeagueGamer* gamer1, PremiumLeagueGamer* gamer2)
{
	if (gamer1 == NULL && gamer2 == NULL)
	{
		return 0;
	}

	PremiumLeagueBattle battle;
	battle.type = GetStatus();
	battle.guid = CLRecordClient::Instance()->GenGuid();
	if (gamer1)
	{
		battle.fighter1 = gamer1->GetBattleFighterInfo();
	}
	if (gamer2)
	{
		battle.fighter2 = gamer2->GetBattleFighterInfo();
	}

	battle.InsertToDB();
	battle.SyncToPlayer();
	AddBattle(battle);
	return battle.guid;
}

void PremiumLeagueMgr::AddBattle(const PremiumLeagueBattle& battle)
{
	m_PremiumBattles.push_back(battle);
}

PremiumLeagueBattle* PremiumLeagueMgr::FindBattleByGuid(ObjID_t guid)
{
	for (auto& battle : m_PremiumBattles)
	{
		if (battle.guid == guid)
		{
			return &battle;
		}
	}

	return NULL;
}

PremiumLeagueBattle* PremiumLeagueMgr::FindBattleByRoleAndStatus(ObjID_t roleId, PremiumLeagueStatus status)
{
	for (auto& battle : m_PremiumBattles)
	{
		if (battle.type == status && (battle.fighter1.id == roleId || battle.fighter2.id == roleId))
		{
			return &battle;
		}
	}

	return NULL;
}

void PremiumLeagueMgr::ClearBattle()
{
	m_PremiumBattles.clear();

	// 从数据库删除
	auto cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_premium_league_battle", "");
	CLRecordClient::Instance()->SendCommand(cmd);
}

void PremiumLeagueMgr::_CountNextRollGamer(UInt32 maxNum)
{
	PremiumLeagueGamer*	gamerByPos[100];
	memset(gamerByPos, 0, sizeof(gamerByPos));

	// 获取8强的名单
	for (auto itr : m_Gamers)
	{
		auto gamer = itr.second;
		if (!gamer || gamer->GetNextRollPos() == 0 || gamer->GetNextRollPos() > maxNum * 2)
		{
			continue;
		}

		// 判断玩家能否参加比赛
		gamerByPos[gamer->GetNextRollPos() - 1] = gamer;
	}

	for (UInt32 i = 0; i < maxNum; i++)
	{
		// 位置排前面的肯定是排名在前的
		auto gamer1 = gamerByPos[i * 2];
		auto gamer2 = gamerByPos[i * 2 + 1];

		if (gamer1 == NULL && gamer2 == NULL)
		{
			continue;
		}

		// 只有一个人
		if (gamer1 == NULL || gamer2 == NULL)
		{
			if (gamer1)
			{
				gamer1->SetNextRollPos((gamer1->GetNextRollPos() + 1) / 2);
			}
			else if (gamer2)
			{
				gamer2->SetNextRollPos((gamer2->GetNextRollPos() + 1) / 2);
			}
			continue;
		}

		// 如果2个人都还没输，排名在前面的进入下一轮
		if (!gamer1->IsLose() && !gamer2->IsLose())
		{
			gamer1->OnBattleWin();
			gamer2->OnBattleLose();
		}

		if (!gamer1->IsLose())
		{
			gamer1->SetNextRollPos((gamer1->GetNextRollPos() + 1) / 2);
		}
		else
		{
			gamer2->SetNextRollPos((gamer2->GetNextRollPos() + 1) / 2);
		}
	}
}

void PremiumLeagueMgr::_StartBattle(UInt32 maxNum)
{
	PremiumLeagueGamer*	gamerByPos[100];
	memset(gamerByPos, 0, sizeof(gamerByPos));

	for (auto gamer : m_FinalBattleGamer)
	{
		if (!gamer || gamer->GetNextRollPos() == 0 || gamer->GetNextRollPos() > maxNum)
		{
			continue;
		}

		// 判断玩家能否参加比赛
		gamerByPos[gamer->GetNextRollPos() - 1] = gamer;
		gamer->SetFinishRollBattle(false);
	}

	// 开始战局
	for (UInt32 i = 0; i < maxNum / 2; i++)
	{
		auto gamer1 = gamerByPos[i * 2];
		auto gamer2 = gamerByPos[i * 2 + 1];

		auto battleId = AddNewBattle(gamer1, gamer2);

		// 先判断是不是战斗双方都有
		if (gamer1 == NULL && gamer2 == NULL)
		{
			// 如果对战双方的ID都是0，说明没有这一局
			continue;
		}
		else if (gamer1 == NULL || gamer2 == NULL)
		{
			// 其中有一个人为0，认为另一个人赢了
			if (gamer1)
			{
				gamer1->OnBattleWin();
				if (gamer1->GetPlayer())
				{
					gamer1->GetPlayer()->SendNotify(9018);
				}
			}
			else if (gamer2)
			{
				gamer2->OnBattleWin();
				if (gamer2->GetPlayer())
				{
					gamer2->GetPlayer()->SendNotify(9018);
				}
			}
			
			continue;
		}

		// 判断双方是不是都能进行战斗
		if (!gamer1->CanStartBattle() || !gamer2->CanStartBattle())
		{
			// 其中有一个人不能进行战斗，认为另一个人赢了
			if (!gamer2->CanStartBattle())
			{
				gamer1->OnBattleWin();
				gamer2->OnBattleLose();

				if (gamer1->GetPlayer())
				{
					gamer1->GetPlayer()->SendNotify(9018);
				}
			}
			else
			{
				gamer1->OnBattleLose();
				gamer2->OnBattleWin();

				if (gamer2->GetPlayer())
				{
					gamer2->GetPlayer()->SendNotify(9018);
				}
			}
			continue;
		}

		CLProtocol::MatchServerPremiumLeagueBattleReq req;
		req.battleId = battleId;
		req.battleStatus = (UInt8)GetStatus();
		req.roleIds.push_back(gamer1->GetID());
		req.roleIds.push_back(gamer2->GetID());
		Router::SendToMatch(req);
	}

	if (_IsFinishCurrentRollBattle())
	{
		_SendNextRollPrepareAnnouncement();

		if (GetStatus() == PLS_FINAL)
		{
			// 如果决赛打完了，强制切换下状态
			ChangeStatus(PLS_FINAL_WAIT_CLEAR);
		}
	}
}

bool PremiumLeagueMgr::_IsFinishCurrentRollBattle()
{
	for (auto gamer : m_FinalBattleGamer)
	{
		if (!gamer)
		{
			continue;
		}

		if (!gamer->IsFinishRollBattle())
		{
			return false;
		}
	}

	return true;
}

void PremiumLeagueMgr::_SendNextRollPrepareAnnouncement()
{
	if (GetStatus() == PLS_FINAL_EIGHT)
	{
		AnnouncementMgr::Instance()->PublicAnnouncement(0, 50, (UInt32)CURRENT_TIME.Sec());
	}
	else if (GetStatus() == PLS_FINAL_FOUR)
	{
		std::string playerNames = "";
		for (auto gamer : m_FinalBattleGamer)
		{
			if (!gamer || gamer->GetNextRollPos() == 0 || gamer->IsLose())
			{
				continue;
			}

			if (playerNames == "")
			{
				playerNames = gamer->GetName();
			}
			else
			{
				playerNames = playerNames + ", " + gamer->GetName();
			}	
		}

		if (playerNames.empty())
		{
			ErrorLogStream << "premium league no final gamer." << LogStream::eos;
			return;
		}

		AnnouncementMgr::Instance()->PublicAnnouncement(0, 51, (UInt32)CURRENT_TIME.Sec(), playerNames);
	}
	else if (GetStatus() == PLS_FINAL)
	{
	}
}

std::string Time2Str(UInt32 timestamp)
{
	Avalon::Date date = Avalon::Date(Avalon::Time((UInt64)timestamp * 1000));
	char str[1024];
	sprintf(str, "%d-%d-%d %d:%d:%d", date.Year(), date.Month(), date.Day(), date.Hour(), date.Min(), date.Sec());
	return str;
}

void PremiumLeagueMgr::_CountStatusPoint()
{
	class PremiumLeagueVistor : public Avalon::DataEntryVisitor<PremiumLeagueDataEntry>
	{
	public:
		bool operator()(PremiumLeagueDataEntry* dataEntry)
		{
			datas.push_back(dataEntry);
			return true;
		}

		std::vector<PremiumLeagueDataEntry*> datas;
	};

	memset(m_TimePoints, 0, sizeof(m_TimePoints));

	PremiumLeagueVistor vistor;
	PremiumLeagueDataEntryMgr::Instance()->Visit(vistor);

	for (auto data : vistor.datas)
	{
		if (!data)
		{
			return;
		}

		PremiumLeaguePoint point;
		point.status = data->status;

		Avalon::Date curDate = CURRENT_DATE;
		Avalon::Date pointDate = curDate;
		pointDate.Hour(data->hour);
		pointDate.Min(data->minute);
		pointDate.Sec(0);

		UInt32 pointTime;
		if (data->week >= curDate.WDay())
		{
			pointTime = UInt32(pointDate.ToTime().Sec()) + (data->week - curDate.WDay()) * DAY_TO_SEC;
		}
		else
		{
			pointTime = UInt32(pointDate.ToTime().Sec()) + (7 + data->week - curDate.WDay()) * DAY_TO_SEC;
		}
		
		point.startTime = pointTime;
		point.endTime = point.startTime + data->durningMin * Avalon::Time::SECS_OF_MIN;

		if (point.endTime <= CURRENT_TIME.Sec())
		{
			point.startTime += 7 * Avalon::Time::SECS_OF_DAY;
			point.endTime += 7 * Avalon::Time::SECS_OF_DAY;
		}

		if (m_TimePoints[point.status].endTime == 0 || point.endTime < m_TimePoints[point.status].endTime)
		{
			m_TimePoints[point.status] = point;
		}
	}

	// 初始状态的时间为上一次的决赛结束时间到这次的报名开始时间
	/*PremiumLeaguePoint initPoint;
	initPoint.status = PLS_INIT;
	initPoint.startTime = m_TimePoints[PLS_FINAL].endTime - 7 * Avalon::Time::SECS_OF_DAY;
	initPoint.endTime = m_TimePoints[PLS_ENROLL].startTime;
	m_TimePoints[initPoint.status] = initPoint;*/

	for (int i = PLS_ENROLL; i < PLS_NUM; i++)
	{
		auto& point = m_TimePoints[i];
		InfoLogStream << "status(" << (UInt8)point.status << ") start(" << Time2Str(point.startTime) << ") end(" << Time2Str(point.endTime) << ")." << LogStream::eos;
	}

	InfoLogStream << "success." << LogStream::eos;
}

PremiumLeagueStatus PremiumLeagueMgr::_CountCurrentPoint()
{
	UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
	for (auto& point : m_TimePoints)
	{
		if (point.status == PLS_INIT)
		{
			continue;
		}

		if (curSec >= point.startTime && curSec <= point.endTime)
		{
			InfoLogStream << "count current point (" << (UInt8)m_Status << ")." << LogStream::eos;
			return point.status;
		}
	}

	InfoLogStream << "count current point (" << (UInt8)m_Status << ")." << LogStream::eos;
	return PLS_INIT;
}

UInt32 PremiumLeagueMgr::_CheckPlayerCanEnroll(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_PLAYER_NOT_EXIST;
	}

	// 最小等级读表
	UInt32 minLevel = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_LEVEL);
	if (player->GetLevel() < minLevel)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_LEVEL_LIMIT;
	}

	auto gamer = FindGamer(player->GetID());
	if (gamer)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_REPEAT;
	}

	// 只有报名阶段可以报名
	if (GetStatus() != PLS_ENROLL)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_CLOSE;
	}

	return ErrorCode::SUCCESS;
}

UInt32 PremiumLeagueMgr::_CheckPlayerCanReEnroll(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_PLAYER_NOT_EXIST;
	}

	// 只有预选赛的时候可以重新报名
	if (GetStatus() != PLS_PRELIMINAY)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_CLOSE;
	}

	// 重新报名，那么之前肯定已经报名过了
	auto gamer = FindGamer(player->GetID());
	if (!gamer)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_PLAYER_NOT_EXIST;
	}

	// 重新报名肯定是已经输了的玩家
	if (!gamer->IsLose())
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_DONT_NEED;
	}

	UInt32 maxCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_ENROLL_MAX_COUNT);
	if (gamer->GetEnrollCount() > maxCount)
	{
		return ErrorCode::PREMIUM_LEAGUE_ENROLL_COUNT;
	}

	return ErrorCode::SUCCESS;
}

void PremiumLeagueMgr::_Clear()
{
	// 清除报名玩家
	_ClearGamers();
	// 清空排行榜
	WSSortListMgr::Instance()->ClearSortList(SORTLIST_PREMIUM_LEAGUE);
	// 清空对战记录
	m_BattleRecord.Clear();
	// 清空淘汰赛记录
	{
		m_PremiumBattles.clear();

		CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_premium_league_battle", "");
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void PremiumLeagueMgr::_ClearGamers()
{
	for (auto itr : m_Gamers)
	{
		auto gamer = itr.second;
		if (gamer)
		{
			gamer->Clear();
			gamer->SyncSelfInfo();
			delete gamer;
		}
	}
	m_Gamers.clear();

	memset(m_FinalBattleGamer, 0, sizeof(m_FinalBattleGamer));

	// 清除数据库
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_premium_league_gamer", "");
	CLRecordClient::Instance()->SendCommand(cmd);
}

UInt32 PremiumLeagueMgr::_GetFinalEightPosByRanking(UInt32 ranking)
{
	/*
		8强对战关系是，1 vs 8, 4 vs 5, 2 vs 7, 3 vs 6
	*/
	UInt32 s_ranking2Pos[] = { 1, 8, 4, 5, 2, 7, 3, 6 };
	for (UInt32 i = 0; i < sizeof(s_ranking2Pos) / sizeof(s_ranking2Pos[0]); i++)
	{
		if (s_ranking2Pos[i] == ranking)
		{
			return i + 1;
		}
	}

	return 0;
}

void PremiumLeagueMgr::_SendReward(PremiumLeagueGamer* gamer, PremiumLeagueRewardType type)
{
	if (!gamer || type >= PL_REWARD_NO_9_20)
	{
		return;
	}

	UInt32 point = _GetReward(type);
	InfoLogStream << "premium race end, gamer(" << gamer->GetID() << ", " << gamer->GetName() << ") got reward(" << (UInt8)type << ") point(" << point << ")." << LogStream::eos;

	if (point == 0)
	{
		return;
	}

	// 发送奖励到邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(9010);
	std::string sender = SysNotifyMgr::Instance()->MakeString(1258);
	UInt32 notifyIds[] = { 9011, 9012, 9013, 9014, 9015 };
	std::string content = SysNotifyMgr::Instance()->MakeString(notifyIds[type], point);

	ItemRewardVec rewards;
	UInt32 POINT_ITEM_ID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_REWARD_ITEM);
	rewards.push_back(ItemReward(POINT_ITEM_ID, point, 0, 0));
	MailBox::SendSysMail(gamer->GetID(), sender, title, content, GetReason(SOURCE_TYPE_PREMIUM_LEAGUE_REWARD, type), rewards);
}

UInt32 PremiumLeagueMgr::_GetReward(PremiumLeagueRewardType type)
{
	UInt32 point = 0;
	UInt32 enrollPlayerNum = m_Gamers.size();
	UInt32 rewardPool = GetRewardPool();

	/*
		奖励算法修改：
			只有前8名有奖励，权重分别为40%, 20%, 10%, 10%, 5%, 5%, 5%, 5%
	*/

	if (type == PL_REWARD_NO_1 && enrollPlayerNum < 1)
	{
		return 0;
	}
	else if (type == PL_REWARD_NO_2 && enrollPlayerNum < 2)
	{
		return 0;
	}
	else if (type == PL_REWARD_NO_3_4 && enrollPlayerNum < 3)
	{
		return 0;
	}
	else if (type == PL_REWARD_NO_5_8 && enrollPlayerNum < 5)
	{
		return 0;
	}
	else if (type == PL_REWARD_NO_9_20)
	{
		return 0;
	}

	UInt32 rates[] = { 40, 20, 10, 5 };
	UInt32 totalRate = 0;
	for (UInt32 i = 1; i <= enrollPlayerNum && i <= FINAL_BATTLE_GAMER_NUM; i++)
	{
		if (i == 1)
		{
			totalRate += rates[PL_REWARD_NO_1];
		}
		else if (i == 2)
		{
			totalRate += rates[PL_REWARD_NO_2];
		}
		else if (i <= 4)
		{
			totalRate += rates[PL_REWARD_NO_3_4];
		}
		else if (i <= 8)
		{
			totalRate += rates[PL_REWARD_NO_5_8];
		}
	}

	if (totalRate == 0)
	{
		return 0;
	}

	point = rewardPool * rates[type] / totalRate;
	return point;
}

UInt32 PremiumLeagueMgr::_CountWinGotScore(UInt32 winnerWinNum, UInt32 loserWinNum)
{
	UInt32 baseScore = 100;
	// 胜场加成
	UInt32 winsteakAdditions[] = { 0, 0, 0, 0, 0 };
	// 终结连胜加成
	UInt32 breakWinsteakAdditions[] = { 5, 10, 15, 20, 25 };

	UInt32 winsteakAdditionNum = sizeof(winsteakAdditions) / sizeof(winsteakAdditions[0]);
	UInt32 winsteakAddition = 0;
	if (winnerWinNum > 0 && winnerWinNum <= winsteakAdditionNum)
	{
		winsteakAddition = winsteakAdditions[winnerWinNum - 1];
	}
	else if (winnerWinNum > winsteakAdditionNum)
	{
		winsteakAddition = winsteakAdditions[winsteakAdditionNum - 1];
	}

	UInt32 breakWinsteakAdditionNum = sizeof(breakWinsteakAdditions) / sizeof(breakWinsteakAdditions[0]);
	UInt32 breakWinsteakAddition = 0;
	if (loserWinNum > 0 && loserWinNum <= breakWinsteakAdditionNum)
	{
		breakWinsteakAddition = breakWinsteakAdditions[loserWinNum - 1];
	}
	else if (loserWinNum > breakWinsteakAdditionNum)
	{
		breakWinsteakAddition = winsteakAdditions[breakWinsteakAdditionNum - 1];
	}

	return baseScore + winsteakAddition + breakWinsteakAddition;
}

UInt32 PremiumLeagueMgr::_CountLoseGotScore()
{
	return 50;
}

void PremiumLeagueMgr::_SendHonorReward(UInt32 pvpType, UInt64 roleId, UInt32 rank)
{
	UInt32 honor = CLHonor::PvpHonorReward(pvpType, 0, rank);

	SendHonorToScene(roleId, pvpType, honor);

	// 邮件提示
	std::string title = SysNotifyMgr::Instance()->MakeString(10064);
	std::string sender = SysNotifyMgr::Instance()->MakeString(10065);
	std::string content = SysNotifyMgr::Instance()->MakeString(10066, rank, honor);

	ItemRewardVec rewards;
	MailBox::SendSysMail(roleId, sender, title, content, "" , rewards);
}
