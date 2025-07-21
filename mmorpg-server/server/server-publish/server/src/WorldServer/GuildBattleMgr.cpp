#include "GuildBattleMgr.h"
#include <CLErrorCode.h>
#include <CLGuildBattleDataEntry.h>
#include <CLSystemValueDataEntry.h>

#include "WSServiceSwitchMgr.h"
#include "WSActivityMgr.h"
#include "GuildMgr.h"
#include "Guild.h"
#include "WSPlayer.h"
#include "WSItemTransaction.h"
#include "WSSortListMgr.h"
#include "AnnouncementMgr.h"
#include "CLMsgParser.h"
#include "GameParamMgr.h"
#include "FigureStatueMgr.h"
#include "PlayerJoins.h"

class GuildBattleMatchTransaction : public ItemTransaction
{
public:
	GuildBattleMatchTransaction(WSPlayer* player, Guild* guild, GuildMember* member, UInt8 isCross) : ItemTransaction(player), m_Guild(guild), m_Member(member), m_IsCross(isCross){}

	void OnCheckSucced(WSPlayer* player)
	{
		RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_BATTLE_MATCH));

		UInt32 number = m_Member->GetGuildBattleNumber();
		if (number > 0)
		{
			m_Member->SetGuildBattleNumber(number - 1);
			if (m_Guild != NULL)
			{
				m_Guild->SyncMemberInfoToScene(m_Member);
			}
		}

		CLProtocol::MatchServerGuildBattleMatchReq req;
		req.isCross = m_IsCross;
		req.info.accid = player->GetAccId();
		req.info.id = player->GetID();
		req.info.name = player->GetName();
		req.guildId = player->GetGuildID();
		req.terrId = m_Guild->GetEnrollTerrID();
		req.winStreak = m_Member->GetGuildBattleWinStreak();
		Router::SendToMatch(req);

		UnLock();
	}

	void OnCheckFailed(WSPlayer* player)
	{
		//player->SendNotify(ErrorCode::GUILD_BATTLE_MEMBER_MATCH_COUNT);
		CLProtocol::WorldMatchStartRes res;
		res.result = ErrorCode::GUILD_BATTLE_MEMBER_MATCH_COUNT;
		player->SendProtocol(res);
		UnLock();
	}

private:
	Guild* m_Guild;
	GuildMember* m_Member;
	UInt8 m_IsCross;
};

class GuildBattleInspireTransaction : public ItemTransaction
{
public:
	GuildBattleInspireTransaction(WSPlayer* player, Guild* guild, GuildMember* member) :ItemTransaction(player), m_Guild(guild), m_GuildMember(member){}

	void OnCheckSucced(WSPlayer* player)
	{
		CLProtocol::WorldGuildBattleInspireRes res;
		UInt32 nRes = ErrorCode::GUILD_BATTLE_INSPIRE_ITEM;
		if (m_Guild != NULL && m_GuildMember!=NULL)
		{
			nRes = m_GuildMember->Inspire();
			if (nRes == ErrorCode::SUCCESS)
			{
				std::string reason = GetReason(LogSourceType::SOURCE_TYPE_GUILD_BATTLE_INSPIRE);
				if (GuildBattleMgr::Instance()->IsChallengeBattle())
				{
					reason = GetReason(LogSourceType::SOURCE_TYPE_GUILD_BATTLE_CHALLENGE_INSPIRE);
				}

				RemoveItem(reason);
				res.inspire = m_Guild->GetInspire();
			}
		}

		res.result = nRes;
		player->SendProtocol(res);
		UnLock();
	}

	void OnCheckFailed(WSPlayer* player)
	{
		CLProtocol::WorldGuildBattleInspireRes res;
		res.result = ErrorCode::GUILD_BATTLE_INSPIRE_ITEM;
		player->SendProtocol(res);

		UnLock();
	}

private:
	Guild* m_Guild;

	GuildMember *m_GuildMember;
};

class GuildBattleEnrollTransaction : public ItemTransaction
{
public:
	GuildBattleEnrollTransaction(WSPlayer* player, Guild* guild, UInt8 terrId)
		:ItemTransaction(player), m_Guild(guild), m_TerritoryId(terrId){}

	void OnCheckSucced(WSPlayer* player)
	{
		GuildBattleMgr::Instance()->ClearTerritoryTransaction(m_TerritoryId);
		UInt32 nRes = ErrorCode::GUILD_BATTLE_ENROLL_GUILD_NOT_FIND;
		if (m_Guild != NULL)
		{
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_GUILD_BATTLE_ENROLL, m_Guild->GetID(), m_TerritoryId);
			if (GuildBattleMgr::Instance()->IsChallengeBattle())
			{
				nRes = GuildBattleMgr::Instance()->AddChallengeEnroll(m_Guild, m_TerritoryId, player->GetID());
				reason = GetReason(LogSourceType::SOURCE_TYPE_GUILD_BATTLE_CHALLENGE);
			}
			else
			{
				nRes = GuildBattleMgr::Instance()->AddEnroll(m_Guild, m_TerritoryId, player->GetID());
			}
			
			if (nRes == ErrorCode::SUCCESS)
			{
				RemoveItem(reason);
			}
		}

		if (GuildBattleMgr::Instance()->IsChallengeBattle())
		{
			CLProtocol::WorldGuildChallengeRes res;
			res.result = nRes;
			player->SendProtocol(res);

			GuildBattleMgr::Instance()->SyncGuildBattleChallengeInfoToPlayers();
		}
		else
		{
			CLProtocol::WorldGuildBattleRes res;
			res.terrId = m_TerritoryId;
			res.enrollSize = GuildBattleMgr::Instance()->GetEnrollSize(m_TerritoryId);
			res.result = nRes;
			player->SendProtocol(res);
		}

		UnLock();
	}

	void OnCheckFailed(WSPlayer* player)
	{
		GuildBattleMgr::Instance()->ClearTerritoryTransaction(m_TerritoryId);
		
		if (GuildBattleMgr::Instance()->IsChallengeBattle())
		{
			CLProtocol::WorldGuildChallengeRes res;
			res.result = ErrorCode::GUILD_BATTLE_ENROLL_ITEM_NUM;
			player->SendProtocol(res);
		}
		else
		{
			CLProtocol::WorldGuildBattleRes res;
			res.result = ErrorCode::GUILD_BATTLE_ENROLL_TERRITORY_LEVEL_HIGH;
			player->SendProtocol(res);
		}

		UnLock();
	}


private:
	Guild* m_Guild;
	UInt8 m_TerritoryId;
};

bool GuildBattleMgr::Init()
{
	class GuildTerritoryDataEntryVisitor : public Avalon::DataEntryVisitor<GuildTerritoryDataEntry>
	{
	public:
		GuildTerritoryDataEntryVisitor(HashMap<UInt8, GuildBattleTerritory>& territory) :m_Territory(territory){}
		~GuildTerritoryDataEntryVisitor(){}
		
		bool operator()(GuildTerritoryDataEntry* dataEntry)
		{
			GuildBattleTerritory territory;
			if (territory.Init(dataEntry))
			{
				m_Territory[dataEntry->id] = territory;
			}
			return true;
		}
	private:
		HashMap<UInt8, GuildBattleTerritory>& m_Territory;
	};

	GuildTerritoryDataEntryVisitor visitor(m_Territory);
	GuildTerritoryDataEntryMgr::Instance()->Visit(visitor);

	_InitGuildBattleStatus();
	_InitGuildBattleChallengePrice();

	SetGuildBattleTimeInfo(GuildBattleTimeDataEntryMgr::Instance()->GetGuildBattleTimeInfo(CURRENT_TIME));

	return true;
}

UInt32 GuildBattleMgr::AddEnroll(Guild* guild, UInt8 terrId, UInt64 playerId)
{
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	guild->SetEnrollTerrID(terrId);
	guild->SetEnrollPlayer(playerId);

	if (guild->GetOccupyTerrID() == terrId)
	{
		m_Enroll[terrId].insert(m_Enroll[terrId].begin(), guild->GetID());
	}
	else
	{
		m_Enroll[terrId].push_back(guild->GetID());
	}

	// 公会战报名，全员邮件通知
	guild->OnGuildBattleEnrollMail(terrId);

	GuildTerritoryDataEntry* terrDataEntry = GuildTerritoryDataEntryMgr::Instance()->FindEntry(terrId);
	if (terrDataEntry != NULL)
	{
		if (GuildBattleMgr::Instance()->IsCrossBattle())
		{
			guild->AddGuildEvent(SysNotifyMgr::Instance()->MakeString(14012, terrDataEntry->name));
		}
		else
		{
			guild->AddGuildEvent(SysNotifyMgr::Instance()->MakeString(14011, terrDataEntry->name));
		}
	}

	//第一次被宣战的时候
	if (IsChallengeBattle() && m_Enroll[terrId].size() == 1)
	{
		//邮件,推送,
	}

	if (IsCrossBattle())
	{
		CLProtocol::CrossGuildBattleEnrollReq req;
		req.info.terrId = terrId;
		req.info.guildId = guild->GetID();
		req.info.guildName = guild->GetName();
		req.info.serverName = SERVER_NAME;
		if (guild->GetLeader() != NULL)
		{
			req.info.guildLeaderName = guild->GetLeader()->GetName();
		}
		Router::SendToCross(req);
	}

	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::AddChallengeEnroll(Guild* guild, UInt8 terrId, UInt64 playerId)
{
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;
	if (GetGuildBattleChallengeTerrId() != terrId) return ErrorCode::GUILD_BATTLE_ENROLL_NOT_TERRITORY;

	std::vector<ItemReward> items;
	for (UInt32 i = 0; i < m_Enroll[terrId].size(); ++i)
	{
		ObjID_t guildId = m_Enroll[terrId][i];
		Guild* tmpGuild = GuildMgr::Instance()->FindGuildByID(guildId);
		if (tmpGuild == NULL) continue;
		tmpGuild->SetEnrollTerrID(0);

		ItemReward reward;
		reward.id = GetGuildBattleChallengeItemId();
		reward.num = m_GuildBattleChallengePrice;
		items.push_back(reward);

		tmpGuild->ResultChallengeEnroll(items, guild->GetName(), m_GuildBattleChallengePrice);
	}

	m_GuildBattleChallengePrice = m_GuildBattleChallengePrice + GetGuildBattleChallengeItemNum();
	_SaveGuildBattleChallengePrice();

	m_Enroll[terrId].clear();

	guild->SetEnrollTerrID(terrId);
	guild->SetEnrollPlayer(playerId);
	m_Enroll[terrId].push_back(guild->GetID());

	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::LoadEnroll(Guild* guild)
{
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	UInt8 territoryId = guild->GetEnrollTerrID();

	guild->SetEnrollTerrID(territoryId);

	if (guild->GetOccupyTerrID() == territoryId)
	{
		m_Enroll[territoryId].insert(m_Enroll[territoryId].begin(), guild->GetID());
	}
	else
	{
		m_Enroll[territoryId].push_back(guild->GetID());
	}

	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::OnRequestEnroll(WSPlayer* player, UInt8 territoryId, UInt32 itemId, UInt32 itemNum)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	if (!_IsGuildBattleEnroll()) return ErrorCode::GUILD_BATTLE_ENROLL_NOT_ENROLL;

	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (!guild) return ErrorCode::GUILD_BATTLE_NOT_EXIST;
	if (guild->GetDismissTime() > 0) return ErrorCode::GUILD_BATTLE_DISMISS_NOT_ENROLL;

	//判断玩家职务
	GuildMember* member = guild->FindMember(player->GetID());
	if (!member) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if(!IsEnrollGuildBattlePost(member->GetPost())) return ErrorCode::GUILD_BATTLE_ENROLL_NOT_POWER;

	//重复报名
	if (_IsEnroll(guild->GetID())) return ErrorCode::GUILD_BATTLE_ENROLL_EXIST;

	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(territoryId);
	if (iter == m_Territory.end()) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	if (iter->second.IsCrossTerritory() && (GAME_FIRST_START_TIME + Avalon::Time::SECS_OF_DAY * 14) > (UInt32)CURRENT_TIME.Sec())
		return ErrorCode::CROSS_GUILD_BATTLE_NOT_OPEN;

	//检查开启的工会战类型和领地类型是否一致
	if (IsCrossBattle() != iter->second.IsCrossTerritory()) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	//检查公会等级
	if (guild->GetLevel() < iter->second.GetNeedGuildLevel()) return ErrorCode::GUILD_BATTLE_ENROLL_GUILD_LEVEL;

	//检查是否有正在处理的事务
	if (iter->second.IsEnrollTransaction()) return ErrorCode::GUILD_BATTLE_ENROLL_BUSY;

	std::vector<ConsumeItem> items;
	if (IsChallengeBattle())
	{
		//宣战
		if (territoryId != GetGuildBattleChallengeTerrId()) return ErrorCode::GUILD_BATTLE_ENROLL_NOT_TERRITORY;
		ConsumeItem consume;
		consume.itemId = GetGuildBattleChallengeItemId();
		if (consume.itemId != itemId) return ErrorCode::GUILD_BATTLE_ENROLL_ITEM_ERROR;

		consume.num = GetGuildBattleChallengeItemNum() + m_GuildBattleChallengePrice;
		if (consume.num != itemNum) return ErrorCode::GUILD_BATTLE_ENROLL_ITEM_NUM;

		items.push_back(consume);
	}
	else
	{
		//检查报名人数
		if (iter->second.GetMaxEnrollSize() <= GetEnrollSize(&iter->second)) return ErrorCode::GUILD_BATTLE_ENROLL_FULL;
		if (iter->second.IsCrossTerritory())
		{
			if (iter->second.GetNeedTerritoryId() == 0)
			{
				if (guild->GetOccupyTerrID() == 0 && guild->GetHistoryTerrId() == 0)
				{
					items = iter->second.GetConsumeItem();
				}
			}
			else
			{
				if (iter->second.GetNeedTerritoryId() != guild->GetOccupyTerrID() && iter->second.GetNeedTerritoryId() != guild->GetHistoryTerrId())
				{
					//items = iter->second.GetConsumeItem();
					InfoLogStream << "player(" << player->GetID() << ") guild(" << guild->GetID() << ") cant enroll cross  territory("
						<< territoryId << "),history TerrID(" << guild->GetHistoryTerrId() << ") occupyTerr(" << guild->GetOccupyTerrID() << ")." << LogStream::eos;

					return ErrorCode::GUILD_BATTLE_ENROLL_NOT_POWER;
				}
			}
			InfoLogStream << "player(" << player->GetID() << ") enroll cross guild(" << guild->GetID() << ") battle territoryId(" 
				<< territoryId << "),history TerrID(" << guild->GetHistoryTerrId() << ") ." << LogStream::eos;
		}
		else
		{
			items = iter->second.GetConsumeItem();
		}
	}

	//报名消耗
	if (!items.empty())
	{
		//开始事务
		GuildBattleEnrollTransaction* trans = new GuildBattleEnrollTransaction(player, guild, territoryId);
		for (UInt32 i = 0; i < items.size(); ++i)
		{
			// 安全锁开启，不允许消耗点券,公会战报名
			if (items[i].itemId == ITEM_POINT && !player->SecurityLockCheck())
			{
				delete trans;
				trans = NULL;
				return ErrorCode::SECURITY_LOCK_FORBID_OP;
			}

			trans->SetItem(items[i].itemId, items[i].num);
		}

		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << "player(" << player->GetID() << ") enroll guild battle territoryId(" << territoryId << "), begin transaction failed." << LogStream::eos;
			return ErrorCode::GUILD_BATTLE_ENROLL_TERRITORY_LEVEL_HIGH;
		}

		iter->second.SetEnrollTransaction(trans);
		return ErrorCode::GUILD_BATTLE_ENROLL_TRANSACTION;
	}

	UInt32 nRes = AddEnroll(guild, territoryId, player->GetID());

	return nRes;
}

UInt32 GuildBattleMgr::OnRequestInspire(WSPlayer* player, UInt8& inspire)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	if (IsChallengeBattle())
	{
		if (m_Status != GuildBattleStatus::GBS_PREPARE) return ErrorCode::GUILD_BATTLE_INSPIRE_CHALLENGE_NUM;
	}
	else
	{
		if (m_Status != GuildBattleStatus::GBS_PREPARE && !_IsGuildBattleEnroll()) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;
	}
	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (!guild) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	if (guild->GetEnrollTerrID() == 0) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;
	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(guild->GetEnrollTerrID());
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;

	inspire = guild->GetInspire();
	if (inspire >= GuildInspireDataEntryMgr::Instance()->GetMaxLevel()) return ErrorCode::GUILD_BATTLE_INSPIRE_MAX_COUNT;

	GuildInspireDataEntry* inspireData = GuildInspireDataEntryMgr::Instance()->FindEntry(inspire);
	if (inspireData == NULL) return ErrorCode::GUILD_BATTLE_INSPIRE_MAX_COUNT;

	GuildMember* member = guild->FindMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;
	UInt32 enterTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_INSPIRE_ENTER_TIME);
	UInt32 needEnterSec = ((UInt32)CURRENT_TIME.Sec()) - (Avalon::Time::SECS_OF_HOUR * enterTime);
	if (needEnterSec < member->GetEnterTime()) return ErrorCode::GUILD_BATTLE_INSPIRE_ENTER_TIME_ERROR;

	//鼓舞消耗
	if (!inspireData->consumeItem.empty())
	{
		//开始事务
		GuildBattleInspireTransaction* trans = new GuildBattleInspireTransaction(player, guild, member);
		std::vector<ConsumeItem> items;
		if (pTerr->GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
		{
			items = inspireData->crossConsumeItem;
		}
		else
		{
			items = inspireData->consumeItem;
		}

		for (UInt32 i = 0; i < items.size(); ++i)
		{
			// 安全锁开启，不允许消耗点券鼓舞
			if (items[i].itemId == ITEM_POINT && !player->SecurityLockCheck())
			{
				delete trans;
				trans = NULL;
				return ErrorCode::SECURITY_LOCK_FORBID_OP;
			}

			trans->SetItem(items[i].itemId, items[i].num);
		}
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << "player(" << player->GetID() << ") inspire(" << inspire << ") guild battle begin transaction failed." << LogStream::eos;
			return ErrorCode::GUILD_BATTLE_INSPIRE_ITEM;
		}
		return ErrorCode::GUILD_BATTLE_INSPIRE_TRANSACTION;
	}

	UInt32 nRes = member->Inspire();
	inspire = guild->GetInspire();
	return nRes;
}

UInt32 GuildBattleMgr::OnMatchBattleStart(WSPlayer* player)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_CROSS_BATTLE) && IsCrossBattle())
	{
		return ErrorCode::SYS_NOT_OPEN;
	}

	if (!IsGuildBattle()) return ErrorCode::GUILD_BATTLE_IS_END;
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;
	if (guild->GetEnrollTerrID() == 0) return  ErrorCode::GUILD_BATTLE_NOT_ENROLL;
	GuildMember* member = guild->FindMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (member->GetBattleStatus() != GuildBattleMemberStatus::GBS_NORMAL) return ErrorCode::GUILD_BATTLE_RACE_NOT_END;

	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(guild->GetEnrollTerrID());
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;

	std::vector<ConsumeItem> items = pTerr->GetMatchConsumeItem(member->GetGuildBattleNumber());
	if (!items.empty())
	{
		GuildBattleMatchTransaction* trans = new GuildBattleMatchTransaction(player, guild, member, (UInt8)pTerr->IsCrossTerritory());
		for (UInt32 i = 0; i < items.size(); ++i)
		{
			trans->SetItem(items[i].itemId, items[i].num);
		}
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << "player(" << player->GetID() << ") OnMatchBattleStart transaction failed." << LogStream::eos;
			return ErrorCode::GUILD_BATTLE_MEMBER_MATCH_COUNT;
		}
		return ErrorCode::GUILD_BATTLE_MATCH_TRANSACTION;
	}
	

	CLProtocol::MatchServerGuildBattleMatchReq req;
	req.isCross = pTerr->IsCrossTerritory();
	req.info.accid = player->GetAccId();
	req.info.id = player->GetID();
	req.info.name = player->GetName();
	req.guildId = player->GetGuildID();
	req.terrId = guild->GetEnrollTerrID();
	req.winStreak = member->GetGuildBattleWinStreak();
	Router::SendToMatch(req);

	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::OnBattleRaceStart(WSPlayer* player)
{
	GuildMember* member = GuildMgr::Instance()->FindGuildMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;
	member->SetGuildBattleStatus(GuildBattleMemberStatus::GBS_START);
	if (GetGuildBattleType() == GBT_NORMAL || GetGuildBattleType() == GBT_CROSS)
	{
		UInt8 type = (GetGuildBattleType() == GBT_NORMAL) ? PJET_THR_TYPE_GUILD_BATTLE : PJET_THR_TYPE_GUILD_CROSS_BATTLE;
		PlayerJoinMgr::Instance()->AddOnePlayer(type, m_GuildBattleTimeInfo.preTime, member->GetGuildID(), player->GetID());
	}
	InfoLogStream << "guild(" << player->GetGuildID() << ") member(" << player->GetID() << ") Set BattleStatus GBS_START" << LogStream::eos;
	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::OnBattleRaceEnd(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd)
{
	DebugLogStream << "winner(" << raceEnd.winner.id << ") loser(" << raceEnd.loser.id << ") result(" << raceEnd.result << ") guild battle race end" << LogStream::eos;
	if (!IsGuildBattle())
	{
		_BattleInvalid(raceEnd.winner.id, raceEnd.loser.id, (UInt8)_resultBitmask::resultBitmask_invalid);
		return ErrorCode::GUILD_BATTLE_IS_END;
	}
	if (raceEnd.result != 0)
	{ 
		_BattleInvalid(raceEnd.winner.id, raceEnd.loser.id, raceEnd.result);
		return ErrorCode::SUCCESS;
	}

	GuildMember* winMember = NULL;
	GuildMember* loserMember = NULL;
	Guild* winGuild = NULL;
	Guild* loserGuild = NULL;
	UInt32 winOldGuildScore = 0;
	UInt32 loseOldGuildScore = 0;

	GuildBattleMember& winGuildBattleMember = raceEnd.winner;
	GuildBattleMember& loserGuildBattleMember = raceEnd.loser;

	if (raceEnd.winner.id != 0)
	{
		winMember = GuildMgr::Instance()->FindGuildMember(raceEnd.winner.id);
		if (winMember != NULL)
		{
			if (winMember->IsInGuild())
			{
				winGuild = GuildMgr::Instance()->FindGuildByID(winMember->GetGuildID());
			}
			if (winGuild != NULL)
			{
				raceEnd.terrId = winGuild->GetEnrollTerrID();
				winGuildBattleMember.winStreak = winMember->GetGuildBattleWinStreak();
			}
		}
	}

	if (raceEnd.loser.id != 0)
	{
		loserMember = GuildMgr::Instance()->FindGuildMember(raceEnd.loser.id);
		if (loserMember != NULL)
		{
			if (loserMember->IsInGuild())
			{
				loserGuild = GuildMgr::Instance()->FindGuildByID(loserMember->GetGuildID());
			}
			if (loserGuild != NULL)
			{
				raceEnd.terrId = loserGuild->GetEnrollTerrID();
				loserGuildBattleMember.winStreak = loserMember->GetGuildBattleWinStreak();
			}
		}
	}

	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(raceEnd.terrId);
	if (iter == m_Territory.end())
	{
		return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;
	}
	iter->second.OnBattleRaceEnd(winGuildBattleMember, loserGuildBattleMember);

	if (winMember != NULL && winGuild != NULL)
	{
		winOldGuildScore = winMember->GetGuildBattleScore();
		winMember->OnGuildBattleRaceEnd(winGuildBattleMember, true, raceEnd.terrId, raceEnd.winerRemainHp, raceEnd.winerRemainMp);
	}

	if (loserMember != NULL && loserGuild != NULL)
	{
		loseOldGuildScore = loserMember->GetGuildBattleScore();
		loserMember->OnGuildBattleRaceEnd(loserGuildBattleMember, false, raceEnd.terrId);
	}

	iter->second.AddGuildBattleRecord(winGuildBattleMember, winGuild, loserGuildBattleMember, loserGuild);

	if (!winGuild && loserGuild)
	{
		iter->second.SetGuild(loserGuild);
	}
	else if (winGuild || !loserGuild)
	{
		iter->second.SetGuild(winGuild);
	}
	else if (winGuild && loserGuild)
	{
		if (winGuild->GetGuildBattleScore() > loserGuild->GetGuildBattleScore())
			iter->second.SetGuild(winGuild);
		else
			iter->second.SetGuild(loserGuild);
	}

	if (iter->second.GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		Router::SendToCross(raceEnd);
	}

	_SendGuildPvpUdpLog(winMember, winOldGuildScore, loserMember, loseOldGuildScore, raceEnd.result);

	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::OnRequestReceive(WSPlayer* player, UInt32 boxId)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_GIVE_REWARD_NOT_MEMBER;
	GuildMember* member = GuildMgr::Instance()->FindGuildMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_GIVE_REWARD_NOT_MEMBER;

	if (member->CheckRewardMask(boxId)) return ErrorCode::GUILD_BATTLE_GIVE_REWARD_ALREADY;

	GuildRewardDataEntry* dataEntry = GuildRewardDataEntryMgr::Instance()->FindEntry(boxId);
	if (dataEntry == NULL) return ErrorCode::GUILD_BATTLE_GIVE_REWARD_DATA_ERROR;

	if (member->GetGuildBattleScore() < dataEntry->beginScore) return ErrorCode::GUILD_BATTLE_GIVE_REWARD_SCORE_ERROR;

	CLProtocol::WorldGuildGiveItemReq req;
	req.playerId = player->GetID();
	req.boxId = boxId;
	req.itemReward = dataEntry->itemReward;
	player->SendToScene(req);

	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::OnRequestBattleRecord(WSPlayer* player, std::vector<GuildBattleRecord>& records, Int32 startIndex, UInt32 count)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	
	UInt8 terrId = GetTerritoryId(player->GetGuildID());
	if (terrId == 0) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;

	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	if (pTerr->GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		CLProtocol::CrossGuildBattleRecordReq req;
		req.playerId = player->GetID();
		req.terrId = terrId;
		req.isSelf = false;
		req.startIndex = startIndex;
		req.count = count;
		Router::SendToCross(req);
		return ErrorCode::CROSS_GUILD_BATTLE_NOT_RESULT;
	}
	else
	{
		pTerr->GetRecords(records, startIndex, count);
		return ErrorCode::SUCCESS;
	}
}

UInt32 GuildBattleMgr::OnRequestBattleSelfRecord(WSPlayer* player, std::vector<GuildBattleRecord>& records)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	UInt8 terrId = GetTerritoryId(player->GetGuildID());
	if (terrId == 0) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;

	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	if (pTerr->GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		CLProtocol::CrossGuildBattleRecordReq req;
		req.playerId = player->GetID();
		req.terrId = terrId;
		req.isSelf = true;
		req.startIndex = 0;
		req.count = 0;
		Router::SendToCross(req);
		return ErrorCode::CROSS_GUILD_BATTLE_NOT_RESULT;
	}
	else
	{
		pTerr->GetSelfRecords(records, player->GetID());
		return ErrorCode::SUCCESS;
	}
}

UInt32 GuildBattleMgr::GetTerritroyBaseInfo(UInt8 terrId, GuildTerritoryBaseInfo& info)
{
	info.terrId = terrId;
	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	info.guildName = pTerr->GetGuildName();
	info.serverName = pTerr->GetCrossOccupyServerName();

	if (m_Status != GuildBattleStatus::GBS_INVALID)
	{
		info.enrollSize = GetEnrollSize(pTerr);
	}
	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::GetTerritroyBaseInfos(std::vector<GuildTerritoryBaseInfo>& infos)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		GuildTerritoryBaseInfo info;
		info.terrId = iter->first;
		info.guildName = iter->second.GetGuildName();
		info.serverName = iter->second.GetCrossOccupyServerName();
		if (_IsGuildBattleEnroll() || IsGuildBattle())
		{
			info.enrollSize = GetEnrollSize(&iter->second);
		}
		infos.push_back(info);
		++iter;
	}
	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::GetGuildBattleStatusEndTime(UInt32 status)
{
	switch (status)
	{
	case GBS_INVALID:
		return m_GuildBattleTimeInfo.enrollTime;
		break;
	case GBS_ENROLL:
		return m_GuildBattleTimeInfo.preTime;
		break;
	case GBS_PREPARE:
		return m_GuildBattleTimeInfo.battleTime;
		break;
	case GBS_BATTLE:
		return m_GuildBattleTimeInfo.rewardTime;
		break;
	case GBS_REWARD:
		return m_GuildBattleTimeInfo.endTime;
		break;
	case GBS_MAX:
		return 0;
		break;
	default:
		break;
	}
	return 0;
}

void GuildBattleMgr::ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count)
{
	UInt8 terrId = GuildBattleMgr::Instance()->GetTerritoryId(player->GetGuildID());
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		iter->second.ListSortEntries(player, sortType, start, count);
	}
}

UInt32 GuildBattleMgr::GetRanking(SortListType sortType, WSPlayer* player)
{
	UInt8 terrId = GuildBattleMgr::Instance()->GetTerritoryId(player->GetGuildID());
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		return iter->second.GetRanking(sortType, player);
	}
	return 0;
}

UInt32 GuildBattleMgr::SelectGuildBattleChallengeInfo(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	GuildMember* member = GuildMgr::Instance()->FindGuildMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (IsEnrollGuildBattlePost(member->GetPost()))
	{
		std::vector<ObjID_t>::iterator iter = std::find(m_GuildBattlePreEnrollPlayers.begin(), m_GuildBattlePreEnrollPlayers.end(), player->GetID());
		if (iter == m_GuildBattlePreEnrollPlayers.end())
		{
			m_GuildBattlePreEnrollPlayers.push_back(player->GetID());
		}
	}

	return _SyncGuildBattleChallengeInfo(player);
}

UInt32 GuildBattleMgr::Lottery(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	if (!IsGuildBattleLottery()) return ErrorCode::GUILD_BATTLE_LETTORY_STATUS_ERROR;

	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	//没有报名.不能抽奖
	if (guild->GetEnrollTerrID() == 0) return ErrorCode::GUILD_BATTLE_NOT_ENROLL;

	//判断此公会战输赢
	bool isWin = false;
	if (guild->GetOccupyTerrID() != 0)
	{
		isWin = true;
	}

	return guild->Lottery(player, isWin);
}

UInt32 GuildBattleMgr::AddTestRecord(WSPlayer* player)
{
	UInt8 terrId = GuildBattleMgr::Instance()->GetTerritoryId(player->GetGuildID());
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		UInt32 index = iter->second.GetRecordSize();

		UInt32 winScore = Avalon::Random::RandBetween(1, 200);
		UInt32 lostScore = Avalon::Random::RandBetween(1, winScore);
		UInt32 winStreak = Avalon::Random::RandBetween(1, 10);
		UInt32 lostStreak = Avalon::Random::RandBetween(1, 10);

		GuildBattleMember winnerMember;
		winnerMember.id = player->GetID();
		winnerMember.gotScore = winScore;
		winnerMember.name = player->GetName();
		winnerMember.winStreak = winStreak;

		GuildBattleMember loserMember;
		loserMember.id = 0;
		loserMember.gotScore = lostScore;
		loserMember.name = "test" + Avalon::StringUtil::ConvertToString(index);
		loserMember.winStreak = lostStreak;

		GuildBattleRecord record;
		record.time = (UInt32)(CURRENT_TIME.MSec() / 1000);
		record.winner = winnerMember;
		record.loser = loserMember;

		iter->second.AddGuildBattleRecord(record);
		return ErrorCode::SUCCESS;
	}
	return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;
}

bool GuildBattleMgr::Dismiss(UInt64 guildId)
{
	if (m_Status == GuildBattleStatus::GBS_INVALID || m_Status == GuildBattleStatus::GBS_ENROLL)
	{
		HashMap<UInt8, GuildBattleTerritory>::iterator iterTerritory = m_Territory.begin();
		while (iterTerritory != m_Territory.end())
		{
			Guild* guild = iterTerritory->second.GetGuild();
			if (guild != NULL && guild->GetID() == guildId)
			{
				iterTerritory->second.SetGuild(NULL);
			}
			++iterTerritory;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool GuildBattleMgr::IsValidGuildBattle()
{
	return ((m_Status != GuildBattleStatus::GBS_INVALID) && (m_Status != GuildBattleStatus::GBS_ENROLL));
}

UInt32 GuildBattleMgr::GetGuildTerritoryType(UInt64 guildId)
{
	UInt8 terrId = GetTerritoryId(guildId);
	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (NULL == pTerr)
	{
		return 0;
	}

	return pTerr->GetTerritoryType();
}

UInt32 GuildBattleMgr::GetCrossEnrollInfo(std::map<UInt8, std::vector<CSEnrollInfo>>& enrollInfo, std::vector<GuildTerritoryBaseInfo>& baseInfos)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
		{
			HashMap<UInt8, std::vector<ObjID_t>>::iterator enrollIter = m_Enroll.find(iter->second.GetTerritoryId());
			if (enrollIter != m_Enroll.end())
			{
				std::vector<CSEnrollInfo> enrollInfos;

				for (UInt32 i = 0; i < enrollIter->second.size(); ++i)
				{
					Guild* guild = GuildMgr::Instance()->FindGuildByID(enrollIter->second[i]);
					if (guild == NULL) continue;

					CSEnrollInfo info;
					info.terrId = enrollIter->first;
					info.guildId = guild->GetID();
					info.guildName = guild->GetName();
					info.serverName = SERVER_NAME;
					if (guild->GetLeader() != NULL)
					{
						info.guildLeaderName = guild->GetLeader()->GetName();
					}
					enrollInfos.push_back(info);
				}

				enrollInfo.insert(std::make_pair(enrollIter->first, enrollInfos));
			}

			if (iter->second.GetGuild() != NULL)
			{

				GuildTerritoryBaseInfo baseInfo;
				baseInfo.guildName = iter->second.GetGuild()->GetName();
				baseInfo.serverName = SERVER_NAME;
				baseInfo.terrId = iter->first;
				baseInfos.push_back(baseInfo);
			}
		}
		++iter;
	}
	return ErrorCode::SUCCESS;
}

void GuildBattleMgr::InitCrossSortListInfo()
{
	if (m_Status != GuildBattleStatus::GBS_BATTLE) return;

	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() != GuildTerritoryType::GTT_CROSS)
		{
			++iter;
			continue;
		}
		
		HashMap<UInt8, std::vector<ObjID_t> >::iterator enrollIter = m_Enroll.find(iter->first);
		if (enrollIter != m_Enroll.end())
		{
			for (UInt32 i = 0; i < enrollIter->second.size(); ++i)
			{
				Guild* guild = GuildMgr::Instance()->FindGuildByID(enrollIter->second[i]);
				if (guild != NULL)
				{
					iter->second.InitCrossSortList(guild);
				}
			}
		}

		++iter;
	}
}

UInt32 GuildBattleMgr::SetCrossTerritoryInfo(GuildTerritoryBaseInfo& info)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(info.terrId);
	if (iter != m_Territory.end())
	{
		iter->second.SetCrossEnrollSize(info.enrollSize);
		iter->second.SetCrossGuildName(info.guildName);
		iter->second.SetCrossOccupyServerName(info.serverName);
	}
	return ErrorCode::SUCCESS;
}

UInt32 GuildBattleMgr::SetCrossTerritoryInfos(std::vector<GuildTerritoryBaseInfo>& infos)
{
	for (UInt32 i = 0; i < infos.size(); ++i)
	{
		SetCrossTerritoryInfo(infos[i]);
	}
	return ErrorCode::SUCCESS;
}

void GuildBattleMgr::GuildDismiss(ObjID_t id)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
		{
			if (iter->second.GetCrossGuildId() == id)
			{
				iter->second.SetCrossGuildId(0);
				iter->second.SetCrossGuildLeaderName("");
				iter->second.SetCrossGuildName("");
				iter->second.SetCrossOccupyServerName("");
				iter->second.SetGuild(NULL);
			}
		}
		else
		{
			Guild* guild = iter->second.GetGuild();
			if (guild != NULL && guild->GetID() == id)
			{
				iter->second.SetGuild(NULL);
			}
		}
		++iter;
	}
}

void GuildBattleMgr::OnTick(Avalon::Time now)
{
	if (!m_IsGuildLoad) return;

	if (m_IsByTime)
	{
		UInt32 nowSec = (UInt32)now.Sec();
		if (nowSec > m_GuildBattleTimeInfo.endTime || nowSec < m_GuildBattleTimeInfo.enrollTime)
		{
			if (m_Status != GuildBattleStatus::GBS_INVALID)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_INVALID);
				SetGuildBattleTimeInfo(GuildBattleTimeDataEntryMgr::Instance()->GetGuildBattleTimeInfo(now));
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.rewardTime)
		{
			if (m_Status != GuildBattleStatus::GBS_REWARD)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_REWARD);
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.battleTime)
		{
			if (m_Status != GuildBattleStatus::GBS_BATTLE)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_BATTLE);
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.preTime)
		{
			if (m_Status != GuildBattleStatus::GBS_PREPARE)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_PREPARE);
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.enrollTime)
		{
			if (m_Status != GuildBattleStatus::GBS_ENROLL)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_ENROLL);
			}
		}
	}

	_ChangeToTargetStatus();
}

void GuildBattleMgr::OnEnrollBegin()
{

	InfoLogStream << " GuildBattleMgr::OnEnrollBegin " << LogStream::eos;

	//WSServiceSwitchMgr::Instance()->OpenService(SERVICE_GUILD_BATTLE_ENROLL);

	if (IsChallengeBattle())
	{
		UInt8 terrId = GetGuildBattleChallengeTerrId();
		HashMap<UInt8, GuildBattleTerritory>::iterator challengeTerrIter = m_Territory.find(terrId);
		if (challengeTerrIter != m_Territory.end())
		{
			Guild* guild = challengeTerrIter->second.GetGuild();
			if (guild == NULL)
			{
				_SkipGuildBattle();
				return;
			}
		}
	}

	_ClearEnroll();
	_ResetGuildBattleChallengePrice();

	GuildMgr::Instance()->OnEnrollBegin();

	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		//清空战斗记录
		iter->second.ClearGuildBattleRecord();

		if (IsNormalBattle() && !iter->second.IsCrossTerritory())
		{
			//自动报名
			iter->second.OnEnrollBegin();
		}

		++iter;
	}

	_SetGuildBattleStatus(GuildBattleStatus::GBS_ENROLL);

}

void GuildBattleMgr::OnEnrollEnd()
{
	InfoLogStream << " GuildBattleMgr::OnEnrollEnd " << LogStream::eos;
	//WSServiceSwitchMgr::Instance()->CloseService(SERVICE_GUILD_BATTLE_ENROLL);

	if (IsChallengeBattle())
	{
		UInt8 terrId = GetGuildBattleChallengeTerrId();
		HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
		if (iter != m_Territory.end())
		{

			if (m_Enroll[terrId].size() == 0)
			{
				//公告一下
				Guild* guild = iter->second.GetGuild();
				if (guild != NULL)
				{
					AnnouncementMgr::Instance()->PublicAnnouncement(guild->GetID(), 43, UInt32(CURRENT_TIME.Sec()), guild->GetName().c_str());
				}
				_SkipGuildBattle();
				return;
			}
			iter->second.AutoEnroll();
		}
	}
	_SetGuildBattleStatus(GuildBattleStatus::GBS_PREPARE);
}

void GuildBattleMgr::OnBattlePrepare()
{
	_SetGuildBattleStatus(GuildBattleStatus::GBS_PREPARE);
}

void GuildBattleMgr::OnBattleBegin()
{
	InfoLogStream << " GuildBattleMgr::OnBattleBegin " << LogStream::eos;

	if (IsChallengeBattle())
	{
		//指定宣战领地清空占领公会
		UInt8 terrId = GetGuildBattleChallengeTerrId();
		_ClearTerritory(terrId);
	}
	else
	{
		_ClearTerritory();
	}

	m_GuildBattleEndInfo.clear();

	_SetGuildBattleStatus(GuildBattleStatus::GBS_BATTLE);
}

void GuildBattleMgr::OnBattleEnd()
{
	InfoLogStream << " GuildBattleMgr::OnBattleEnd " << LogStream::eos;
	//WSServiceSwitchMgr::Instance()->CloseService(SERVICE_GUILD_BATTLE);

	UInt8 challengeTerrId = GetGuildBattleChallengeTerrId();

	// 计算各领地领主
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
		{
			++iter;
			continue;
		}

		GuildBattleEndInfo info;
		info.terrId = iter->first;
		info.terrName = iter->second.GetTerritoryName();
		Guild* guild = iter->second.GetGuild();

		//如果没人占领,选择最先报名的公会占领
		HashMap<UInt8, std::vector<ObjID_t> >::iterator enrollIter = m_Enroll.find(iter->first);
		if (enrollIter != m_Enroll.end())
		{
			for (UInt32 i = 0; i < enrollIter->second.size(); ++i)
			{
				Guild* tmpGuild = GuildMgr::Instance()->FindGuildByID(enrollIter->second[i]);
				if (tmpGuild == NULL) continue;
				if (guild == NULL)
				{
					guild = tmpGuild;
				}
				else if (tmpGuild->GetID() != guild->GetID())
				{
					//发放失败公会奖励
					tmpGuild->SendGuildBattleLoseReward(iter->second.GetLoseItemReward());
				}
			}
		}
		if (guild != NULL)
		{
			//设置占领的领地
			if (iter->second.GetGuild() == NULL)
				iter->second.SetGuild(guild);

			guild->SetOccupyTerrID(iter->first);
			guild->OnGuildOccupyTerritory(iter->first);
			guild->UpdateGuildBattleWeekScoreRank(0);

			GuildTerritoryDataEntry* dataEntry = GuildTerritoryDataEntryMgr::Instance()->FindEntry(guild->GetHistoryTerrId());
			if (dataEntry == NULL || iter->second.GetLevel() > dataEntry->level)
			{
				guild->SetHistoryTerrId(iter->first);
			}

			GuildMgr::Instance()->SetTerrOccupy(iter->first, guild->GetID());

			guild->SendGuildBattleReward(iter->second.GetLearderItemReward(), iter->second.GetMemberItemReward());
			guild->SendGuildLeaderReward(iter->second.GetLearderReward());
			guild->SendGuildMemberReward(iter->second.GetMemberReward());

			if (challengeTerrId == iter->first)
			{
				//设置雕像
				guild->SetGuildBattleFigureStatue();
			}

			info.guildName = guild->GetName();

			if (guild->GetLeader() != NULL)
			{
				info.guildLeaderName = guild->GetLeader()->GetName();
			}

			GuildTerritoryDataEntry* terrData = GuildTerritoryDataEntryMgr::Instance()->FindEntry(iter->first);

			if (terrData)
			{
				guild->AddGuildEvent(SysNotifyMgr::Instance()->MakeString(14013, terrData->name));
			}
			else
			{
				ErrorLogStream << "Not find GuildTerritoryDataEntry id (" << iter->first << ")" << LogStream::eos;
			}

			{
				//公会荣誉
				std::vector<UInt32> conds(GuildHonorConditionType::GUILD_COND_MAX,0);
				conds[GuildHonorConditionType::GUILD_COND_TYPE] = info.terrId;
				conds[GuildHonorConditionType::GUILD_COND_MEMBER] = 1;
				guild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_BATTLE, conds);
			}

		}
		m_GuildBattleEndInfo.push_back(info);
		++iter;
	}
	//_SendBattleMember(res);
	_BattleEndAnnouncement();

	_SetGuildBattleStatus(GuildBattleStatus::GBS_REWARD);

	//公会战结束，跑马灯提示
}

void GuildBattleMgr::OnChallengeBattleEnd()
{
	InfoLogStream << " GuildBattleMgr::OnChallengeBattleEnd " << LogStream::eos;
	//WSServiceSwitchMgr::Instance()->CloseService(SERVICE_GUILD_BATTLE);

	UInt8 challengeTerrId = GetGuildBattleChallengeTerrId();

	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(challengeTerrId);
	if (iter == m_Territory.end())
	{
		ErrorLogStream << " GuildBattleMgr::OnChallengeBattleEnd terr not find:" << challengeTerrId << LogStream::eos;
		return;
	}

	if (iter->second.GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		ErrorLogStream << " GuildBattleMgr::OnChallengeBattleEnd terr is cross:" << challengeTerrId << LogStream::eos;
		return;
	}

	Guild* guild = iter->second.GetGuild();

	UInt64 oldTerrGuildId = 0;

	HashMap<UInt8, std::vector<ObjID_t> >::iterator enrollIter = m_Enroll.find(iter->first);
	if (enrollIter != m_Enroll.end())
	{
		//第一个报名的是原领地占领公会
		if (enrollIter->second.size() > 0)
		{
			oldTerrGuildId = enrollIter->second[0];
		}
		for (UInt32 i = 0; i < enrollIter->second.size(); ++i)
		{
			Guild* tmpGuild = GuildMgr::Instance()->FindGuildByID(enrollIter->second[i]);
			if (tmpGuild == NULL) continue;
			if (guild == NULL)
			{
				guild = tmpGuild;
			}
			else if (tmpGuild->GetID() != guild->GetID())
			{
				//发放失败公会奖励
				tmpGuild->SendGuildBattleLoseReward(iter->second.GetLoseItemReward());
			}
		}
	}

	if (guild != NULL)
	{
		//原本拥有的领地放弃
		UInt8 occupyTerrId = guild->GetOccupyTerrID();
		HashMap<UInt8, GuildBattleTerritory>::iterator occupyTerrIter = m_Territory.find(occupyTerrId);
		if (occupyTerrIter != m_Territory.end())
		{
			occupyTerrIter->second.SetGuild(NULL);
		}

		//设置占领的领地
		if (iter->second.GetGuild() == NULL)
			iter->second.SetGuild(guild);
		guild->SetOccupyTerrID(iter->first);
		guild->OnGuildOccupyTerritory(iter->first);
		guild->UpdateGuildBattleWeekScoreRank(0);

		GuildTerritoryDataEntry* dataEntry = GuildTerritoryDataEntryMgr::Instance()->FindEntry(guild->GetHistoryTerrId());
		if (dataEntry == NULL || iter->second.GetLevel() > dataEntry->level)
		{
			guild->SetHistoryTerrId(iter->first);
		}

		GuildMgr::Instance()->SetTerrOccupy(iter->first, guild->GetID());

		guild->SendGuildBattleReward(iter->second.GetLearderItemReward(), iter->second.GetMemberItemReward());
		guild->SendGuildLeaderReward(iter->second.GetLearderReward());
		guild->SendGuildMemberReward(iter->second.GetMemberReward());

		//设置雕像
		guild->SetGuildBattleFigureStatue();

		//公告
		std::string guildLeaderName = "";
		if (guild->GetLeader() != NULL)
		{
			guildLeaderName = guild->GetLeader()->GetName();
		}
		if (guild->GetID() == oldTerrGuildId)
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(guild->GetID(), 45, UInt32(CURRENT_TIME.Sec()), guild->GetName().c_str(), guildLeaderName.c_str());
		}
		else
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(guild->GetID(), 46, UInt32(CURRENT_TIME.Sec()), guild->GetName().c_str(), guildLeaderName.c_str());
		}
		{
			//公会荣誉
			std::vector<UInt32> conds(GuildHonorConditionType::GUILD_COND_MAX);
			conds[GuildHonorConditionType::GUILD_COND_TYPE] = guild->GetOccupyTerrID();
			conds[GuildHonorConditionType::GUILD_COND_MEMBER] = 1;
			guild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_BATTLE, conds);
		}
	}

	iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.IsCrossTerritory())
		{
			++iter;
			continue;
		}

		GuildBattleEndInfo info;
		info.terrId = iter->first;
		info.terrName = iter->second.GetTerritoryName();
		Guild* tmpGuild = iter->second.GetGuild();
		if (tmpGuild != NULL)
		{
			info.guildName = tmpGuild->GetName();
			if (tmpGuild->GetLeader() != NULL)
				info.guildLeaderName = tmpGuild->GetLeader()->GetName();
		}
		m_GuildBattleEndInfo.push_back(info);
		++iter;
	}

	_SetGuildBattleStatus(GuildBattleStatus::GBS_REWARD);
}

void GuildBattleMgr::OnCrossBattleEnd()
{
	InfoLogStream << " GuildBattleMgr::OnCrossBattleEnd " << LogStream::eos;

	_ClearTerritoryCrossEnrollInfo();

	_SetGuildBattleStatus(GuildBattleStatus::GBS_REWARD);
}

void GuildBattleMgr::OnBattleReward()
{
	_SetGuildBattleStatus(GuildBattleStatus::GBS_INVALID);
}

void GuildBattleMgr::OnDayChanged()
{
	//if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_BATTLE) || WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_BATTLE_ENROLL))
	//	return;
	if (IsGuildBattle() || _IsGuildBattleEnroll())
	{
		InfoLogStream << "day change remove terr reward! type:" << m_Type << " status:" << m_Status <<  LogStream::eos;

		// 清除掉所有可领奖的玩家
		_DayMemberTerrRewardRefresh(true);
		return;
	}

	InfoLogStream << "day change add terr reward!" << LogStream::eos;
	//发放奖励
	_DayMemberTerrRewardRefresh(false);
}

void GuildBattleMgr::OnGuildLoadFinish()
{
	m_IsGuildLoad = true;
}

void GuildBattleMgr::SyncBattleRecord(UInt8 terrId, CLProtocol::WorldGuildBattleRecordSync sync)
{
	HashMap<UInt8, std::vector<ObjID_t> >::iterator iter = m_Enroll.find(terrId);
	if (iter != m_Enroll.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			Guild* guild = GuildMgr::Instance()->FindGuildByID(iter->second[i]);
			if (guild == NULL) continue;
			guild->Broadcast(sync);
		}
	}
}

UInt8 GuildBattleMgr::GetTerritoryId(UInt64 guildId)
{
	HashMap<UInt8, std::vector<ObjID_t> >::iterator iter = m_Enroll.begin();
	while (iter != m_Enroll.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i] == guildId)
			{
				return iter->first;
			}
		}
		++iter;
	}
	return 0;
}

void GuildBattleMgr::SetTerrGuild(Guild* guild)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(guild->GetOccupyTerrID());
	if (iter != m_Territory.end())
	{
		iter->second.SetGuild(guild);
	}
}

void GuildBattleMgr::SetCrossTerrGuild(Guild* guild)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(guild->GetOccupyCrossTerrID());
	if (iter != m_Territory.end())
	{
		iter->second.SetGuild(guild);
		iter->second.SetCrossGuildId(guild->GetID());
	}
}

bool GuildBattleMgr::IsGuildBattle()
{
	//return WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_BATTLE);
	return m_Status == GuildBattleStatus::GBS_BATTLE;
}

bool GuildBattleMgr::IsEnroll(Guild* guild)
{
	return _IsGuildBattleEnroll() && guild->GetEnrollTerrID() > 0;
}

bool GuildBattleMgr::IsNotifyGuildEnroll(Guild* guild)
{
	if (NULL == guild || m_Status < GBS_ENROLL || m_Status >= GBS_REWARD)
	{
		return false;
	}

	return guild->GetEnrollTerrID() > 0;
}

void GuildBattleMgr::ChangeTargetStatus(GuildBattleStatus targetStatus)
{
	m_TargetStatus = targetStatus;
}

bool GuildBattleMgr::IsNormalBattle()
{
	return m_Type == GuildBattleType::GBT_NORMAL;
}

bool GuildBattleMgr::IsChallengeBattle()
{
	return m_Type == GuildBattleType::GBT_CHALLENGE;
}

bool GuildBattleMgr::IsCrossBattle()
{
	return m_Type == GuildBattleType::GBT_CROSS;
}

UInt8 GuildBattleMgr::GetGuildBattleChallengeTerrId()
{ 
	return (UInt8)SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_BATTLE_CHALLENGE_TERR_ID);
}

UInt32 GuildBattleMgr::GetGuildBattleChallengeItemId()
{
	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_BATTLE_CHALLENGE_ITEM_ID);
}

UInt32 GuildBattleMgr::GetGuildBattleChallengeItemNum()
{
	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_BATTLE_CHALLENGE_ITEM_NUM);
}

void GuildBattleMgr::ClearTerritoryTransaction(UInt8 terrId)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		iter->second.SetEnrollTransaction(NULL);
	}
}

void GuildBattleMgr::SyncGuildBattleChallengeInfoToPlayers()
{
	for (UInt32 i = 0; i < m_GuildBattlePreEnrollPlayers.size(); ++i)
	{
		GuildMember* member = GuildMgr::Instance()->FindGuildMember(m_GuildBattlePreEnrollPlayers[i]);
		if (member != NULL)
		{
			_SyncGuildBattleChallengeInfo(member->GetPlayer());
		}
	}
}

bool GuildBattleMgr::IsGuildBattleLottery()
{
	return m_Status == GuildBattleStatus::GBS_REWARD;
}

void GuildBattleMgr::SyncCrossGuildBattleEndInfo(std::vector<GuildBattleEndInfo> endInfo)
{
	m_GuildBattleEndInfo = endInfo;

	// 计算各领地领主
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() != GuildTerritoryType::GTT_CROSS)
		{
			++iter;
			continue;
		}

		std::vector<GuildBattleEndInfo>::iterator endInfoIter = m_GuildBattleEndInfo.begin();
		while (endInfoIter != m_GuildBattleEndInfo.end())
		{
			if (endInfoIter->terrId == iter->first)
			{
				break;
			}
			++endInfoIter;
		}

		if (endInfoIter == m_GuildBattleEndInfo.end() || endInfoIter->guildId == 0)
		{
			++iter;
			continue;
		}

		//如果没人占领,选择最先报名的公会占领
		HashMap<UInt8, std::vector<ObjID_t> >::iterator enrollIter = m_Enroll.find(iter->first);
		if (enrollIter != m_Enroll.end())
		{
			for (UInt32 i = 0; i < enrollIter->second.size(); ++i)
			{
				Guild* tmpGuild = GuildMgr::Instance()->FindGuildByID(enrollIter->second[i]);
				if (tmpGuild == NULL) continue;

				if (endInfoIter != m_GuildBattleEndInfo.end() && endInfoIter->guildId == tmpGuild->GetID())
				{
					tmpGuild->SetOccupyCrossTerrID(iter->first);
					iter->second.SetGuild(tmpGuild);
					iter->second.SetCrossGuildName(tmpGuild->GetName());
					iter->second.SetCrossGuildId(tmpGuild->GetID());
					iter->second.SetCrossOccupyServerName(SERVER_NAME);
					if (tmpGuild->GetLeader() != NULL)
					{
						iter->second.SetCrossGuildLeaderName(tmpGuild->GetLeader()->GetName());
					}
					//功能开关
					if (WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_CROSS_BATTLE))
					{
						tmpGuild->SendGuildBattleReward(iter->second.GetMemberItemReward(), iter->second.GetMemberItemReward());
						tmpGuild->SendGuildLeaderReward(iter->second.GetLearderReward());
						tmpGuild->SendGuildMemberReward(iter->second.GetMemberReward());
					}

					GuildTerritoryDataEntry* terrData = GuildTerritoryDataEntryMgr::Instance()->FindEntry(iter->first);

					if (terrData)
					{
						tmpGuild->AddGuildEvent(SysNotifyMgr::Instance()->MakeString(14014, terrData->name));
					}
					else
					{
						ErrorLogStream << "Not find GuildTerritoryDataEntry id (" << iter->first << ")" << LogStream::eos;
					}

					{
						//公会荣誉
						std::vector<UInt32> conds(GuildHonorConditionType::GUILD_COND_MAX);
						conds[GuildHonorConditionType::GUILD_COND_TYPE] = tmpGuild->GetOccupyCrossTerrID();
						conds[GuildHonorConditionType::GUILD_COND_MEMBER] = 1;
						tmpGuild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_BATTLE, conds);
					}
					tmpGuild->SyncCrossOccupyToScene();
				}
				else
				{
					if (WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_CROSS_BATTLE))
					{
						tmpGuild->SendGuildBattleLoseReward(iter->second.GetLoseItemReward());
					}
				}
			}
		}

		if (iter->second.GetCrossGuildId() != endInfoIter->guildId)
		{
			iter->second.SetCrossGuildId(endInfoIter->guildId);
			iter->second.SetCrossGuildLeaderName(endInfoIter->guildLeaderName);
			iter->second.SetCrossGuildName(endInfoIter->guildName);
			iter->second.SetCrossOccupyServerName(endInfoIter->serverName);
		}

		++iter;
	}

	if (IsGuildBattleLottery() && IsCrossBattle())
	{
		_SyncGuildBattleStatus();
	}

	_BattleEndAnnouncement(GuildTerritoryType::GTT_CROSS);
}

bool GuildBattleMgr::_IsGuildBattleEnroll()
{
	//return WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_BATTLE_ENROLL);
	return m_Status == GuildBattleStatus::GBS_ENROLL;
}

UInt32 GuildBattleMgr::GetEnrollSize(UInt8 terrId)
{
	GuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	return GetEnrollSize(pTerr);
}

UInt32 GuildBattleMgr::GetEnrollSize(GuildBattleTerritory* terr)
{
	if (terr == NULL) return 0;
	if (terr->GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		return terr->GetCrossEnrollSize();
	}

	HashMap<UInt8, std::vector<ObjID_t> >::iterator iter = m_Enroll.find(terr->GetTerritoryId());
	if (iter != m_Enroll.end())
		return iter->second.size();
	return 0;
}

void GuildBattleMgr::SetGuildBattleTimeInfo(GuildBattleTimeInfo info)
{
	m_GuildBattleTimeInfo = info;
	SetGuildBattleType(info.type);
}

bool GuildBattleMgr::_IsEnroll(UInt64 guildId)
{
	HashMap<UInt8, std::vector<ObjID_t> >::iterator iter = m_Enroll.begin();
	while (iter != m_Enroll.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (guildId == iter->second[i]) return true;
		}
		++iter;
	}
	return false;
}

void GuildBattleMgr::_ClearEnroll()
{
	m_GuildBattlePreEnrollPlayers.clear();

	GuildMgr::Instance()->ClearEnroll();

	HashMap<UInt8, std::vector<ObjID_t> >::iterator iter = m_Enroll.begin();
	while (iter != m_Enroll.end())
	{
		iter->second.clear();
		++iter;
	}
}

void GuildBattleMgr::_ClearTerritory()
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if ((GetGuildBattleType() == GuildBattleType::GBT_CROSS && iter->second.GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
			|| (GetGuildBattleType() == GuildBattleType::GBT_NORMAL && iter->second.GetTerritoryType() == GuildTerritoryType::GTT_NORMAL))
		{
			Guild* guild = iter->second.GetGuild();
			if (guild != NULL)
			{
				if (guild->GetOccupyTerrID() == iter->first)
				{
					guild->ClearMemberTerrDayReward(true);
					guild->SetOccupyTerrID(0);
					guild->SyncOccupyToScene();
				}
				if (guild->GetOccupyCrossTerrID() == iter->first)
				{
					guild->SetOccupyCrossTerrID(0);
					guild->SyncCrossOccupyToScene();
				}
			}
			iter->second.SetGuild(NULL);
			iter->second.SetCrossGuildName("");
			iter->second.SetCrossGuildLeaderName("");
			iter->second.SetCrossOccupyServerName("");
			iter->second.SetCrossGuildId(0);
		}
		++iter;
	}
}

void GuildBattleMgr::_ClearTerritoryCrossEnrollInfo()
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		iter->second.SetCrossEnrollSize(0);
		++iter;
	}
}

void GuildBattleMgr::_ClearTerritory(UInt8 terrId)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		Guild* guild = iter->second.GetGuild();
		if (guild != NULL)
		{
			if (guild->GetOccupyTerrID() == terrId)
			{
				guild->ClearMemberTerrDayReward(true);
				guild->SetOccupyTerrID(0);
				guild->SyncOccupyToScene();
			}
			if (guild->GetOccupyCrossTerrID() == terrId)
			{
				guild->SetOccupyCrossTerrID(0);
				guild->SyncCrossOccupyToScene();
			}
		}
		iter->second.SetGuild(NULL);
		iter->second.SetCrossGuildName("");
		iter->second.SetCrossGuildLeaderName("");
		iter->second.SetCrossOccupyServerName("");
		iter->second.SetCrossGuildId(0);
	}
}

void GuildBattleMgr::_ClearTerritoryCrossEnrollInfo(UInt8 terrId)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		iter->second.SetCrossEnrollSize(0);
	}
}

GuildBattleTerritory* GuildBattleMgr::_GetGuildBattleTerritory(UInt8 terrId)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		return &iter->second;
	}
	return NULL;
}

void GuildBattleMgr::_BattleEndAnnouncement(GuildTerritoryType type)
{
	//30
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() != type)
		{
			++iter;
			continue;
		}

		std::string leaderTitle = "";
		std::string terrName = iter->second.GetTerritoryName();
		std::string guildLeaderName = "";
		std::string guildName = "";
		UInt32 itemId = iter->second.GetLeaderTitleId();
		std::string linkItemStr;
		if (MsgPackage::GetItemMsgTag(linkItemStr, 0, itemId, 0))
		{
			leaderTitle = linkItemStr;
		}
		else
		{
			ItemDataEntry* itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
			if (itemDataEntry != NULL)
			{
				leaderTitle = itemDataEntry->name;
			}
		}

		if (type == GuildTerritoryType::GTT_CROSS && iter->second.GetCrossGuildId() != 0)
		{
			guildName = iter->second.GetCrossGuildName();
			guildLeaderName = iter->second.GetCrossGuildLeaderName();
			AnnouncementMgr::Instance()->PublicAnnouncement(iter->second.GetCrossGuildId(), 71, UInt32(CURRENT_TIME.Sec()), iter->second.GetCrossOccupyServerName().c_str(),
				guildName.c_str(), terrName.c_str(), guildLeaderName.c_str(), leaderTitle.c_str());
		}
		else
		{
			Guild* guild = iter->second.GetGuild();
			if (guild != NULL)
			{
				guildName = guild->GetName();
				if (guild->GetLeader() != NULL)
				{
					GuildMember* guildMember = guild->GetLeader();
					std::string playerLinkStr;
					if (MsgPackage::GetPlayerMsgTag(playerLinkStr, guildMember->GetID(), guildMember->GetName(), guildMember->GetOccu(), guildMember->GetLevel()))
					{
						guildLeaderName = playerLinkStr;
					}
					else
					{
						guildLeaderName = guildMember->GetName();
					}
				}
				AnnouncementMgr::Instance()->PublicAnnouncement(guild->GetID(), 30, UInt32(CURRENT_TIME.Sec()), guildName.c_str(), terrName.c_str(), guildLeaderName.c_str(), leaderTitle.c_str());
			}
		}
		++iter;
	}
}

void GuildBattleMgr::_BattleInvalid(UInt64 playerA, UInt64 playerB, UInt8 result)
{
	UInt32 playerScoreA = 0;
	UInt32 playerScoreB = 0;
	GuildMember* memberA = NULL;
	GuildMember* memberB = NULL;

	CLProtocol::WorldGuildBattleRaceEnd res;
	res.result = result;

	if (playerA != 0)
	{
		memberA = GuildMgr::Instance()->FindGuildMember(playerA);
		if (memberA != NULL)
		{
			res.oldScore = memberA->GetGuildBattleScore();
			res.newScore = res.oldScore;
			memberA->SendProtocol(res);
			playerScoreA = memberA->GetGuildBattleScore();
			memberA->SetGuildBattleStatus(GuildBattleMemberStatus::GBS_NORMAL);
			InfoLogStream << "in GuildMember::_BattleInvalid, guild(" << memberA->GetGuildID() << ") member(" << memberA->GetID() << ") Set BattleStatus GBS_NORMAL" << LogStream::eos;
		}
	}

	if (playerB != 0)
	{
		memberB = GuildMgr::Instance()->FindGuildMember(playerB);
		if (memberB != NULL)
		{
			res.oldScore = memberB->GetGuildBattleScore();
			res.newScore = res.oldScore;
			memberB->SendProtocol(res);
			playerScoreB = memberB->GetGuildBattleScore();
			memberB->SetGuildBattleStatus(GuildBattleMemberStatus::GBS_NORMAL);
			InfoLogStream << "in GuildMember::_BattleInvalid, guild(" << memberB->GetGuildID() << ") member(" << memberB->GetID() << ") Set BattleStatus GBS_NORMAL" << LogStream::eos;
		}
	}

	_SendGuildPvpUdpLog(memberA, playerScoreA, memberB, playerScoreB, result);

}

void GuildBattleMgr::_SendBattleMember(Avalon::Protocol &protocol)
{
	HashMap<UInt8, std::vector<ObjID_t>>::iterator iter = m_Enroll.begin();
	while (iter != m_Enroll.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			Guild* guild = GuildMgr::Instance()->FindGuildByID(iter->second[i]);
			if (guild != NULL)
			{
				guild->Broadcast(protocol);
			}
		}
		++iter;
	}
}

void GuildBattleMgr::_SendGuildPvpUdpLog(GuildMember* winGuildMember, UInt32 winOldGuildScore, GuildMember* loseGuildMember, UInt32 loseOldGuildScore, UInt8 result)
{
	UInt64 winPlayerId = 0;
	std::string winPlayerName;
	UInt64 losePlayerId = 0;
	std::string losePlayerName = "";
	_resultBitmask resultBit = _resultBitmask(result);
	if (winGuildMember != NULL)
	{
		winPlayerId = winGuildMember->GetID();
		winPlayerName = winGuildMember->GetName();
	}
	if (loseGuildMember != NULL)
	{
		losePlayerId = loseGuildMember->GetID();
		losePlayerName = loseGuildMember->GetName();
	}

	if (winGuildMember != NULL && winGuildMember->GetPlayer() != NULL)
	{
		if (result == 0)
		{
			resultBit = _resultBitmask::resultBitmask_win;
		}

		UInt64 winGuildId = 0;
		std::string winGuildName = "";
		Guild* winGuild = GuildMgr::Instance()->FindGuildByID(winGuildMember->GetGuildID());
		if (winGuild != NULL)
		{
			winGuildId = winGuild->GetID();
			winGuildName = winGuild->GetName();
		}

		winGuildMember->GetPlayer()->SendGuildPvpUdpLog(winGuildId, winGuildName, losePlayerId, losePlayerName, PkType::PK_GUILD_BATTLE, GetGuildBattleType(), resultBit, winOldGuildScore, winGuildMember->GetGuildBattleScore());
	}
	if (loseGuildMember != NULL && loseGuildMember->GetPlayer() != NULL)
	{
		if (result == 0)
		{
			resultBit = _resultBitmask::resultBitmask_lose;
		}

		UInt64 loseGuildId = 0;
		std::string loseGuildName = "";
		Guild* loseGuild = GuildMgr::Instance()->FindGuildByID(loseGuildMember->GetGuildID());
		if (loseGuild != NULL)
		{
			loseGuildId = loseGuild->GetID();
			loseGuildName = loseGuild->GetName();
		}

		loseGuildMember->GetPlayer()->SendGuildPvpUdpLog(loseGuildId, loseGuildName, winPlayerId, winPlayerName, PkType::PK_GUILD_BATTLE, GetGuildBattleType(), resultBit, loseOldGuildScore, loseGuildMember->GetGuildBattleScore());
	}
}

void GuildBattleMgr::_InitGuildBattleStatus()
{
	m_Status = (GuildBattleStatus)GameParamMgr::Instance()->GetValue(GUILD_BATTLE_STATUS_KEY);
	m_TargetStatus = m_Status;
}

void GuildBattleMgr::_SaveGuildBattleStatus()
{
	GameParamMgr::Instance()->SetValue(GUILD_BATTLE_STATUS_KEY, m_Status);
}

void GuildBattleMgr::_InitGuildBattleChallengePrice()
{
	m_GuildBattleChallengePrice = GameParamMgr::Instance()->GetValue(GUILD_BATTLE_CHALLENGE_PRICE);
}

void GuildBattleMgr::_SaveGuildBattleChallengePrice()
{
	GameParamMgr::Instance()->SetValue(GUILD_BATTLE_CHALLENGE_PRICE, m_GuildBattleChallengePrice);
}

UInt32 GuildBattleMgr::_SyncGuildBattleChallengeInfo(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	if (!IsChallengeBattle()) return ErrorCode::GUILD_BATTLE_TYPE_ERROR;

	UInt8 terrId = GetGuildBattleChallengeTerrId();

	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter == m_Territory.end()) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	CLProtocol::WorldGuildChallengeInfoSync sync;
	sync.itemId = GetGuildBattleChallengeItemId();
	sync.itemNum = m_GuildBattleChallengePrice;
	GetTerritroyBaseInfo(terrId, sync.info);
	Guild* enrollGuild = _GetEnrollChallengeGuild();
	if (enrollGuild != NULL)
	{
		sync.enrollGuildId = enrollGuild->GetID();
		if (enrollGuild->GetLeader() != NULL)
		{
			sync.enrollGuildleaderName = enrollGuild->GetLeader()->GetName();
		}
		sync.enrollGuildLevel = enrollGuild->GetLevel();
		sync.enrollGuildName = enrollGuild->GetName();
	}
	else
	{
		sync.enrollGuildId = 0;
		sync.enrollGuildleaderName = "";
		sync.enrollGuildName = "";
		sync.enrollGuildLevel = 0;
	}

	player->SendProtocol(sync);

	return ErrorCode::SUCCESS;
}

void GuildBattleMgr::_SyncGuildBattleStatus()
{
	CLProtocol::WorldGuildBattleStatusSync sync;
	sync.type = (UInt8)GetGuildBattleType();
	sync.status = (UInt8)m_Status;
	sync.time = 0;
	UInt32 nowSec = (UInt32)CURRENT_TIME.Sec();

	std::string systemLinkStr;
	if (GetGuildBattleType() != GuildBattleType::GBT_CROSS)
	{
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_GUID_BATTLE, 0);
	}
	else
	{
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_CROSS_GUID_BATTLE, 0);
	}

	switch (m_Status)
	{
	case GBS_INVALID:
		sync.time = 0;
		break;
	case GBS_ENROLL:
		sync.time = m_GuildBattleTimeInfo.preTime;

		if (IsChallengeBattle())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, 42, nowSec);
		}
		else
		{
			//公会战报名开始，跑马灯每2小时提示一次
			AnnouncementMgr::Instance()->PublicAnnouncementList(0, 26, nowSec, m_GuildBattleTimeInfo.preTime - nowSec, 2 * Avalon::Time::SECS_OF_HOUR, systemLinkStr.c_str());
		}
		break;
	case GBS_PREPARE:
		sync.time = m_GuildBattleTimeInfo.battleTime;

		if (IsChallengeBattle())
		{

		}
		else
		{
			//公会战报名结束，跑马灯每5分钟提示一次
			AnnouncementMgr::Instance()->PublicAnnouncementList(0, 27, nowSec, m_GuildBattleTimeInfo.battleTime - nowSec, 5 * Avalon::Time::SECS_OF_MIN, systemLinkStr.c_str());
		}
		AnnouncementMgr::Instance()->RemoveAnnouncement((UInt32)26);
		break;
	case GBS_BATTLE:
		sync.time = m_GuildBattleTimeInfo.rewardTime;

		if (IsChallengeBattle())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, 44, nowSec);
		}
		else
		{
			//公会战开始，跑马灯提示
			AnnouncementMgr::Instance()->PublicAnnouncementList(0, 28, nowSec, 1, 1, systemLinkStr.c_str());
			//公会战还有10分钟结束，跑马灯提示
			AnnouncementMgr::Instance()->PublicAnnouncementList(0, 29, m_GuildBattleTimeInfo.rewardTime - 10 * Avalon::Time::SECS_OF_MIN, 1, 1, systemLinkStr.c_str());
		}
		AnnouncementMgr::Instance()->RemoveAnnouncement((UInt32)27);

		break;
	case GBS_REWARD:
		sync.time = m_GuildBattleTimeInfo.endTime;
		sync.endInfo = m_GuildBattleEndInfo;

		// 跨服工会战由Cross服务器下发
		if (m_GuildBattleEndInfo.size() <= 0)
		{
			return;
		}
		break;
	default:
		break;
	}
	GuildMgr::Instance()->Broadcast(sync);
}

void GuildBattleMgr::_SetGuildBattleStatus(GuildBattleStatus status)
{
	if (m_Status != status)
	{
		m_Status = status;
		_SaveGuildBattleStatus();
		_SyncGuildBattleStatus();
	}
}

void GuildBattleMgr::_ChangeToTargetStatus()
{
	if (m_Status == m_TargetStatus || m_TargetStatus >= GuildBattleStatus::GBS_MAX) return;
	GuildBattleStatus tmpStatus = (GuildBattleStatus)(m_Status + 1);
	if (tmpStatus >= GuildBattleStatus::GBS_MAX)
	{
		tmpStatus = GuildBattleStatus::GBS_INVALID;
	}

	switch (tmpStatus)
	{
	case GuildBattleStatus::GBS_ENROLL:
		OnEnrollBegin();
		break;
	case GuildBattleStatus::GBS_PREPARE:
		OnEnrollEnd();
		break;
	case GuildBattleStatus::GBS_BATTLE:
		OnBattleBegin();
		break;
	case GuildBattleStatus::GBS_REWARD:
		if (IsChallengeBattle())
		{
			OnChallengeBattleEnd();
		}
		else if (IsCrossBattle())
		{
			OnCrossBattleEnd();
		}
		else
		{
			OnBattleEnd();
		}
		break;
	case GuildBattleStatus::GBS_INVALID:
		OnBattleReward();
		break;
	default:
		m_Status = tmpStatus;
		_SaveGuildBattleStatus();
		break;
	}

	_ChangeToTargetStatus();
}

Guild* GuildBattleMgr::_GetEnrollChallengeGuild()
{
	if (!IsChallengeBattle()) return NULL;

	Guild* result = NULL;

	UInt8 terrId = GetGuildBattleChallengeTerrId();
	if (m_Enroll[terrId].size() == 0) return NULL;
	UInt64 guildId = 0;
	if (_IsGuildBattleEnroll())
	{
		guildId = m_Enroll[terrId][0];
	}
	else if (m_Enroll[terrId].size()>=2)
	{
		guildId = m_Enroll[terrId][1];
	}
	result = GuildMgr::Instance()->FindGuildByID(guildId);
	return result;
}

void GuildBattleMgr::_ResetGuildBattleChallengePrice()
{
	m_GuildBattleChallengePrice = 0;
	_SaveGuildBattleChallengePrice();
}

void GuildBattleMgr::_SkipGuildBattle()
{
	_SetGuildBattleStatus(GuildBattleStatus::GBS_INVALID);
	m_TargetStatus = GuildBattleStatus::GBS_INVALID;


	UInt32 nowTime = (UInt32)(CURRENT_TIME.Sec());
	if (nowTime >= m_GuildBattleTimeInfo.enrollTime)
	{
		Avalon::Time endTime((UInt64)(m_GuildBattleTimeInfo.endTime + 1) * (UInt64)1000);
		SetGuildBattleTimeInfo(GuildBattleTimeDataEntryMgr::Instance()->GetGuildBattleTimeInfo(endTime));
	}
}

void GuildBattleMgr::_DayMemberTerrRewardRefresh(bool isClear)
{
	HashMap<UInt8, GuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		if (iter->second.GetTerritoryType() == GuildTerritoryType::GTT_NORMAL)
		{
			Guild* guild = iter->second.GetGuild();
			if (guild != NULL)
			{
				//guild->SendGuildBattleReward(iter->second.GetItemReward());
				guild->ClearMemberTerrDayReward(isClear);

				InfoLogStream << "guild terr day reward! id" << guild->GetID() << LogStream::eos;
			}
		}
		++iter;
	}
}