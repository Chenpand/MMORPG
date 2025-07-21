#include "Guild.h"
#include <CLFunctionDataEntry.h>
#include <CLRecordClient.h>
#include <CLGuildProtocol.h>
#include <CLShopItemDataEntry.h>
#include <CLGuildDataEntry.h>
#include <CLGuildBuildingDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLGuildSkillDataEntry.h>
#include <CLGuildStatueDataEntry.h>
#include <CLGuildTableDataEntry.h>
#include <CLRedPacketDataEntry.h>
#include <CLGuildEmblemDataEntry.h>
#include <CLGuildDungeonTypeDataEntry.h>
#include <CLGuildBuildingPointDataEntry.h>
#include <CLSysNotify.h>
#include <CLCounterNameDefine.h>
#include <CLCounterDefine.h>
#include <CLGuildDungeonTimeDataEntry.h>
#include <CLGuildProsperityDataEntry.h>
#include <CLGuildProtocol.h>

#include "WSPlayer.h"
#include "GuildMgr.h"
#include "WSPlayerMgr.h"
#include "WSItemTransaction.h"
#include "MailBox.h"
#include "RedPacketMgr.h"
#include "WSSortListMgr.h"
#include "SysInterface.h"
#include "GuildBattleMgr.h"
#include "WSServiceSwitchMgr.h"
#include "FigureStatueMgr.h"
#include "GuildDungeonMgr.h"
#include "NewTitle.h"
#include "OfflineEventMgr.h"

/**
*@brief 修改公会名事务
*/
class GuildChangeNameTransaction : public ItemTransaction
{
public:
	GuildChangeNameTransaction(WSPlayer *player, const std::string& name)
		: ItemTransaction(player), m_Name(name) {}
	~GuildChangeNameTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->OnCheckChangeGuildNameRet(player, m_Name))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_CHANGE_NAME));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		InfoLogStream << "player(" << player->GetID() << ") request to modify guild name but don't have enough money." << LogStream::eos;

		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_MODIFY_NAME;
		res.result = ErrorCode::GUILD_NO_ENOUGH_POINT;
		player->SendProtocol(res);

		UnLock();
	}

private:
	std::string			m_Name;
};

/**
*@brief 公会捐献事务
*/
class GuildDonateTransaction : public ItemTransaction
{
public:
	GuildDonateTransaction(WSPlayer *player, GuildDonateType type, UInt8 num)
		: ItemTransaction(player), m_Type(type), m_Num(num) {}
	~GuildDonateTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->OnCheckDonateRet(player, m_Type, m_Num))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_DONATE, m_Type, m_Num));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		InfoLogStream << "player(" << player->GetID() << ") request to donate(" << (UInt8)m_Type << ", " << m_Num << ") but don't have enough money." << LogStream::eos;

		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_DONATE;
		res.result = ErrorCode::GUILD_NOT_ENOUGH_MONEY;
		player->SendProtocol(res);

		UnLock();
	}

private:
	GuildDonateType			m_Type;
	UInt8					m_Num;
};

/**
*@brief 升级公会技能事务
*/
class GuildSkillUpgradeTransaction : public ItemTransaction
{
public:
	GuildSkillUpgradeTransaction(WSPlayer *player, UInt16 skillId, UInt8 level)
		: ItemTransaction(player), m_SkillId(skillId), m_Level(level) {}
	~GuildSkillUpgradeTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->OnCheckUpgradeSkillRet(player, m_SkillId, m_Level))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_UPGRADE_SKILL));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		InfoLogStream << "player(" << player->GetID() << ") request to upgrade skill(" << m_SkillId << ", " << m_Level << ") but don't have enough money." << LogStream::eos;

		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_UPGRADE_SKILL;
		res.result = ErrorCode::GUILD_NOT_ENOUGH_MONEY;
		player->SendProtocol(res);

		UnLock();
	}

private:
	UInt16		m_SkillId;
	UInt8		m_Level;
};

/**
*@brief 发送公会邮件事务
*/
class GuildMailTransaction : public ItemTransaction
{
public:
	GuildMailTransaction(WSPlayer *player, const std::string& content)
		: ItemTransaction(player), m_Content(content) {}
	~GuildMailTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->OnCheckMailCounterRet(player, m_Content))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_MAIL));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		InfoLogStream << "player(" << player->GetID() << ") request to send guild mail but no times." << LogStream::eos;

		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_SEND_MAIL;
		res.result = ErrorCode::GUILD_NOT_ENOUGH_TIMES;
		player->SendProtocol(res);

		UnLock();
	}

private:
	std::string m_Content;
};

/**
*@brief 发送公会邮件事务
*/
class GuildDismissTransaction : public ItemTransaction
{
public:
	GuildDismissTransaction(WSPlayer *player)
		: ItemTransaction(player) {}
	~GuildDismissTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->OnCheckDismissRet(player))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_DISMISS));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		InfoLogStream << "player(" << player->GetID() << ") request to dismiss guild but no times." << LogStream::eos;

		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_DISMISS;
		res.result = ErrorCode::GUILD_NOT_ENOUGH_TIMES;
		player->SendProtocol(res);

		UnLock();
	}

private:
	std::string m_Content;
};

/**
*@brief 公会膜拜
*/
class GuildOrzTransaction : public ItemTransaction
{
public:
	GuildOrzTransaction(WSPlayer *player, GuildOrzType type)
		: ItemTransaction(player), m_Type(type) {}
	~GuildOrzTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->CheckOrzRet(player, m_Type))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_ORZ, m_Type));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_ORZ;
		if (GetErrorCode() == ErrorCode::ITEM_TRANS_NOT_ENOUGH_MONEY)
		{
			InfoLogStream << "player(" << player->GetID() << ") request to orz but don't have enough money." << LogStream::eos;
			res.result = ErrorCode::GUILD_NOT_ENOUGH_MONEY;
		}
		else
		{
			InfoLogStream << "player(" << player->GetID() << ") request to orz but no times." << LogStream::eos;
			res.result = ErrorCode::GUILD_NOT_ENOUGH_TIMES;
		}
		
		player->SendProtocol(res);

		UnLock();
	}

private:
	GuildOrzType m_Type;
};

/**
*@brief 公会圆桌会议
*/
class GuildTableTransaction : public ItemTransaction
{
public:
	GuildTableTransaction(WSPlayer *player, UInt8 seat, UInt8 type)
		: ItemTransaction(player), m_Seat(seat), m_Type(type) {}
	~GuildTableTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild && guild->CheckJoinTable(player, m_Seat, m_Type))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_TABLE));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		InfoLogStream << "player(" << player->GetID() << ") request to join guild table but no times." << LogStream::eos;

		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_TABLE;
		res.result = ErrorCode::GUILD_NOT_ENOUGH_TIMES;
		player->SendProtocol(res);

		UnLock();
	}

private:
	UInt8 m_Seat;
	UInt8 m_Type;
};

/**
*@brief 公会自费红包
*/
class GuildPayRedPacketTransaction : public ItemTransaction
{
public:
	GuildPayRedPacketTransaction(WSPlayer *player, UInt16 reason, std::string name, UInt8 num)
		: ItemTransaction(player), m_Reason(reason), m_Name(name), m_Num(num){}
	~GuildPayRedPacketTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		UInt32 result = ErrorCode::SUCCESS;
		if (!guild)
		{
			result = ErrorCode::RED_PACKET_NOT_IN_GUILD;
		}
		else
		{
			result = guild->OnCheckPayRedPacketRet(player, m_Reason, m_Name, m_Num);
		}

		if (result == ErrorCode::SUCCESS)
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_RED_PACKET_GUILD_PAY));
		}
		else
		{
			player->OnSendRedPacketFailure(result);
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_PAY_REDPACKET;
		if (GetErrorCode() == ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM)
		{
			InfoLogStream << "player(" << player->GetID() << ") request to buy pay_red_packet but don't have enough money." << LogStream::eos;
			res.result = ErrorCode::GUILD_NOT_ENOUGH_MONEY;
		}
		else
		{
			InfoLogStream << "player(" << player->GetID() << ") request to buy pay_red_packet but no times." << LogStream::eos;
			res.result = ErrorCode::GUILD_NOT_ENOUGH_TIMES;
		}
		player->SendProtocol(res);

		UnLock();
	}

private:
	UInt16 m_Reason;
	std::string m_Name;
	UInt8 m_Num;
};

/**
*@brief 公会升级建筑,点券换资金
*/
class GuildPointFundTransaction : public ItemTransaction
{
public:
	GuildPointFundTransaction(WSPlayer* player, UInt32 fund, UInt8 level, GuildBuildingType type) 
		: ItemTransaction(player), m_Fund(fund), m_Level(level), m_Type(type){}
	~GuildPointFundTransaction() {}


	void OnCheckSucced(WSPlayer *player)
	{
		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		
		if (guild != NULL && guild->GetBuildingLevel(m_Type) < m_Level)
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_POINT_FUND));
			guild->AddFund(m_Fund);
			guild->RequestUpgradeBuilding(player, m_Type);
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_UPGRADE_BUILDING;
		res.result = ErrorCode::GUILD_BUILDING_NOT_ENOUGH_FUND;
		player->SendProtocol(res);
		UnLock();
	}
private:
	UInt32 m_Fund;
	UInt8 m_Level;
	GuildBuildingType m_Type;
};

/**
*@brief 公会徽记升级
*/
class GuildEmblemTransaction : public ItemTransaction
{
public:
	GuildEmblemTransaction(WSPlayer* player, UInt32 emblemLvl)
		: ItemTransaction(player), m_EmblemLvl(emblemLvl) {}
	~GuildEmblemTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		if (NULL == player) return;

		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild == NULL)
		{
			CLProtocol::WorldGuildEmblemUpRes res;
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			res.emblemLevel = m_EmblemLvl;
			player->SendProtocol(res);
			return;
		}
		
		guild->EmblemUpgradeTransaction(player, m_EmblemLvl, this);
		UnLock();
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldGuildEmblemUpRes res;
		res.result = ErrorCode::GUILD_NO_ENOUGH_ITEM;
		res.emblemLevel = m_EmblemLvl;
		player->SendProtocol(res);
		UnLock();
	}
private:

	UInt32 m_EmblemLvl;
};

Guild::Guild()
{
	m_DataEntry = NULL;
	m_Leader = NULL;
	m_SetupTime = 0;
	m_Level = 0;
	m_Fund = 0;
	m_ShopId = 0;
	m_ShopRefreshTime = (UInt32)CURRENT_TIME.Sec();
	m_DismissTime = 0;
	m_MemberNum = 0;
	m_EnrollTerrId = 0;
	m_GuildBattleScore = 0;
	m_OccupyTerrId = 0;
	m_JoinLevel = 0;
	m_GuildDungeonType = 0;
	m_ProsperityStatus = GUILD_PROSPERITY_NULL;
	m_MergerRequsetStatus = GUILD_MERGER_REQUEST_NULL;
	m_MergerRequestTime = 0;
	m_MergerGoalGuildID = 0;
	m_MergerRequestAccpetTime = 0;
	m_MergerOtherGuildTime = 0;

	m_WinProbability.SetDBValue((UInt8)GUILD_BATTLE_REWARD_PROBABILITY_MAX);
	m_LoseProbability.SetDBValue((UInt8)(GUILD_BATTLE_REWARD_PROBABILITY_MAX / 2));
	m_StorageAddPost.SetDBValue((UInt8)GUILD_POST_NORMAL);
	m_StorageDelPost.SetDBValue((UInt8)GUILD_POST_ASSISTANT);

	m_SaveTimer.SetLastTime(CURRENT_TIME.MSec());
	m_SaveTimer.SetInterval(1000);
	m_SyncTimer.SetInterval(5000);

	m_GuildHonorMgr.SetOwner(this);
}

Guild::~Guild()
{
	m_Members.clear();
	m_HavedMergerRequest.clear();
}

bool Guild::Init(GUILD_INIT_TYPE type)
{
	// 根据主城等级设置帮会等级
	SetLevel(GetBuildingLevel(GUILD_BUILDING_MAIN));
	// 设置公会日志
	SetGuildEventGuildID();

	m_DataEntry = GuildDataEntryMgr::Instance()->FindEntry(GetLevel());
	if (!m_DataEntry)
	{
		return false;
	}

	UpdateMemberNum();

	m_GuildMemberBattleCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_MEMBER_BATTLE_COUNT);
	m_InspireSkillId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_INSPIRE_SKILL_ID);

	m_guildStorage.Init(this);

	if (type == GUILD_INIT_TYPE_LOAD)
	{
		m_guildStorage.LoadData();
	}

	//UpdateGuildLevelSortList();

	return true;
}

void Guild::OnTick(const Avalon::Time& now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		SaveToDB();
	}

	if (m_SyncTimer.IsTimeout(now))
	{
		BroadcastInfo();
		SyncMergerInfo();
	}

	if (now.Sec() >= m_ShopRefreshTime)
	{
		RefreshShop();
	}

	m_guildStorage.OnTick();


	if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_MERGER))
	{
		if (m_MergerRequsetStatus == GUILD_MERGER_REQUEST_SEND)
		{
			if (now.Sec() > m_MergerRequestTime + Avalon::Time::SECS_OF_DAY * 3)
			{
				auto guild = GuildMgr::Instance()->FindGuildByID(m_MergerGoalGuildID);
				if (guild != nullptr)
				{
					guild->RemoveMergerRequestGuild(this);
				}
				CleanSelfMergerRequest();
			}
		}

		if (m_MergerRequsetStatus == GUILD_MERGER_REQUEST_ACCEPTED)
		{
			if (now.Sec() > m_MergerRequestAccpetTime + Avalon::Time::SECS_OF_DAY)
			{
				auto guild = GuildMgr::Instance()->FindGuildByID(m_MergerGoalGuildID);
				if (guild != nullptr)
				{
					//OnMerger(guild);
					SetProsperity(GUILD_PROSPERITY_IN_MERGER);
					guild->SetProsperity(GUILD_PROSPERITY_IN_BY_MERGER);
					SendMergerUDPLog(GUILD_MERGER_RESULT_SUCCESS);
				}
				else
				{
					ErrorLogStream << this->GetID() << "Merger guild:" << m_MergerGoalGuildID << " faild!" << LogStream::eos;
				}
				//CleanSelfMergerRequest();
				//m_MergerOtherGuildTime = now.Sec();
			}
		}
	}
}

void Guild::SetLevel(UInt8 level)
{
	m_Level = level;

	for (auto& iter : m_Members)
	{
		SyncMemberInfoToScene(iter.second);
	}
}

void Guild::OnDismiss(UInt8 dismissType)
{
	InfoLogStream << "guild(" << GetID() << ") dismissed." << LogStream::eos;

	if (dismissType == GUILD_DISMISS_MERGER)
	{
		SendMergerUDPLog(GUILD_MERGER_RESULT_DISMISS_MERGER);
		CleanAllMergerRequest(false);
	}
	else
	{
		CleanAllMergerRequest(true);
	}

	if (m_MergerGoalGuildID != 0)
	{
		CancelMerger(GuildMgr::Instance()->FindGuildByID(m_MergerGoalGuildID));
	}
	CleanSelfMergerRequest();
	
	// 清除所有帮会成员
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			continue;
		}

		member->SetGuildID(0);
		member->SetLeaveTime((UInt32)CURRENT_TIME.Sec());
		member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_TERR_ID);
		member->SetPost(GUILD_POST_INVALID);

		auto player = WSPlayerMgr::Instance()->FindPlayer(member->GetID());
		if (player)
		{
			player->SetGuildID(0);
			player->SetGuildLvl(0);
			player->GetRelationMgr().OnGuildChanged();
			player->GetTitleMgr().OnGuidLeave();
		}

		SyncMemberInfoToScene(member);

		// 修改公会红包
		RedPacketMgr::Instance()->OnPlayerChangeGuild(member->GetID(), GetID(), 0);
	}

	if (dismissType == GUILD_DISMISS_NORMAL)
	{
		// 发送邮件
		std::string title = SysNotifyMgr::Instance()->MakeString(2050);
		std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
		std::string content = SysNotifyMgr::Instance()->MakeString(2054);
		std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
		_BroadcastMail(title, sender, content, reason);
		SendMergerUDPLog(GUILD_MERGER_RESULT_DISMISS_YES);

	}
	else if (dismissType == GUILD_DISMISS_LOW)
	{
		std::string title = SysNotifyMgr::Instance()->MakeString(12010);
		std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
		std::string content = SysNotifyMgr::Instance()->MakeString(12011);
		std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
		_BroadcastMail(title, sender, content, reason);
		SendMergerUDPLog(GUILD_MERGER_RESULT_DISMISS_BY);
	}

	SendNotify(3011, GetName().c_str());
	m_Members.clear();

	// 删除公会副本信息
	GuildDungeonMgr::Instance()->GuildDismiss(GetID());
	// 删除工会领地信息
	GuildBattleMgr::Instance()->GuildDismiss(GetID());
	// 删除数据库公会日志
	DelGuildEvent();
}

void Guild::OnChangeLeader(GuildMember* newLeader)
{
	if (!newLeader)
	{
		return;
	}

	InfoLogStream << "guild(" << GetID() << ") change new leader(" << newLeader->GetID() << ", " << newLeader->GetName() << ")." << LogStream::eos;
	
	m_Leader = newLeader;
	m_LeaderName = newLeader->GetName();
	m_LeaderPopularoty = 0;

	WSSortListMgr::Instance()->OnGuildLeaderChanged(GetID(), m_Leader->GetID(), m_LeaderName);
}

void Guild::OnDayChanged()
{
	UInt32 curSec = (UInt32)CURRENT_TIME.Sec();

	// 所有成员添加圆桌和膜拜的红点
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			return;
		}

		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_TABLE);
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_ORZ);

		// 重置活跃度积分
		if (member->GetActiveDegreeUpdateTime() < curSec)
		{
			member->SetActiveDegree(0);
		}
	}

	// 自动转让会长
	CheckAutoChangeLeader();
}

void Guild::OnWeekChanged()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_MERGER))
	{
		return;
	}

	if ((GetProsperity() == GUILD_PROSPERITY_NULL) || ((this->GetSetUpTime() + Avalon::Time::SECS_OF_DAY * 14) >= CURRENT_TIME.Sec()))
	{
		m_HavedMergerRequest.clear();
		m_LastWeekAddedFund = GetWeekAddedFund();
		SetWeekAddedFund(0);
		SetProsperity(GUILD_PROSPERITY_HIGH);
		return;
	}
	auto dataEntry = GuildProsperityDataEntryMgr::Instance()->FindEntry(GetLevel());
	if (dataEntry)
	{
		if (m_MergerRequsetStatus != GUILD_MERGER_REQUEST_ACCEPTED && !ByAcceptedMerger())
		{
			if (GetProsperity() == GUILD_PROSPERITY_DISMISS && GetWeekAddedFund() >= dataEntry->dismissLevel)
			{
				SendMergerUDPLog(GUILD_MERGER_RESULT_DISMISS_FAILD);
			}

			if (GetWeekAddedFund() < dataEntry->dismissLevel)
			{
				if (GetProsperity() == GUILD_PROSPERITY_DISMISS)
				{
					if (this->GetSetUpTime() + Avalon::Time::SECS_OF_DAY * 14 < CURRENT_TIME.Sec())
					{
						SetProsperity(GUILD_PROSPERITY_IN_DISMISS);
					}
				}
				else
				{
					SetProsperity(GUILD_PROSPERITY_DISMISS);
					std::string title = SysNotifyMgr::Instance()->MakeString(12010);
					std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
					std::string content = SysNotifyMgr::Instance()->MakeString(12013, dataEntry->dismissLevel);
					std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
					_BroadcastMail(title, sender, content, reason);
					AddGuildEvent(content);
				}
			}
			else if (GetWeekAddedFund() < dataEntry->midLevel)
			{
				SetProsperity(GUILD_PROSPERITY_LOW);
			}
			else if (GetWeekAddedFund() < dataEntry->highLevel)
			{
				SetProsperity(GUILD_PROSPERITY_MID);
			}
			else
			{
				SetProsperity(GUILD_PROSPERITY_HIGH);
			}
			m_HavedMergerRequest.clear();
			CleanSelfMergerRequest();
		}
		else
		{
			if (ByAcceptedMerger())
			{
				CleanAllMergerRequest();
			}
			else if (m_MergerRequsetStatus == GUILD_MERGER_REQUEST_ACCEPTED)
			{
				m_HavedMergerRequest.clear();
			}	
		}
	}
	else
	{
		SetProsperity(GUILD_PROSPERITY_NULL);
		ErrorLogStream << "Can't find GuildProsperity Config guildid is(" << this->GetID() << "guild level is"<< this->GetLevel() << ")!" << LogStream::eos;
	}
	m_LastWeekAddedFund = GetWeekAddedFund();
	SetWeekAddedFund(0);
}

void Guild::CheckAutoChangeLeader()
{
	UInt32 leaderNeedLogoutDay = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_AUTO_CHAGE_LEADER_DAY);
	auto curTime = (UInt32)CURRENT_TIME.Sec();

	if (m_Leader)
	{
		if (WSPlayerMgr::Instance()->FindPlayer(m_Leader->GetID()))
		{
			return;
		}

		auto lastLoginTime = m_Leader->GetOfflineTime();
		
		UInt32 leaderNeedLogoutTime = leaderNeedLogoutDay * Avalon::Time::SECS_OF_DAY;
		if (curTime < lastLoginTime + leaderNeedLogoutTime)
		{
			UInt32 notifyNeedTime = (leaderNeedLogoutDay - 1) * Avalon::Time::SECS_OF_DAY;
			if (curTime > lastLoginTime + notifyNeedTime)
			{
				// 发送即将自动转让会长的通知
				std::string title = SysNotifyMgr::Instance()->MakeString(2050);
				std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
				std::string content = SysNotifyMgr::Instance()->MakeString(3057, m_Leader->GetName());
				std::string reason = GetReason(SOURCE_TYPE_GUILD_AUTO_CHANGE);
				_BroadcastMail(title, sender, content, reason);
			}
			return;
		}
	}

	// 选出一个新的会长
	GuildMember* newLeader = NULL;
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			continue;
		}

		if (member->GetPost() == GUILD_POST_LEADER)
		{
			continue;
		}

		auto lastLogoutTime = member->GetOfflineTime();
		auto needLoginTime = leaderNeedLogoutDay  * Avalon::Time::SECS_OF_DAY;
		if (curTime > lastLogoutTime + needLoginTime && !WSPlayerMgr::Instance()->FindPlayer(member->GetID()))
		{
			continue;
		}

		if (member->GetPost() == GUILD_POST_ASSISTANT)
		{
			newLeader = member;
			break;
		}
		else
		{
			if (newLeader == NULL || member->GetTotalContribution() > newLeader->GetTotalContribution())
			{
				newLeader = member;
			}
		}
	}

	if (!newLeader)
	{
		return;
	}
	
	auto oldPost = newLeader->GetPost();
	std::string oldLeaderName = "";
	if (m_Leader)
	{
		oldLeaderName = m_Leader->GetName();
		m_Leader->SetPost(GUILD_POST_NORMAL);
		SyncMemberInfoToScene(m_Leader);
	}
	
	newLeader->SetPost(GUILD_POST_LEADER);
	OnChangeLeader(newLeader);
	SyncMemberInfoToScene(newLeader);

	// 发送邮件通知
	std::string title = SysNotifyMgr::Instance()->MakeString(2050);
	std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
	std::string content = "";
	if (oldPost == GUILD_POST_ASSISTANT)
	{
		content = SysNotifyMgr::Instance()->MakeString(3058, oldLeaderName, newLeader->GetName());
	}
	else
	{
		content = SysNotifyMgr::Instance()->MakeString(3059, oldLeaderName, newLeader->GetName());
	}
	
	std::string reason = GetReason(SOURCE_TYPE_GUILD_AUTO_CHANGE);
	_BroadcastMail(title, sender, content, reason);
}

bool Guild::AddMember(GuildMember* member)
{
	if (!member)
	{
		return false;
	}

	if (FindMember(member->GetID()))
	{
		ErrorLogStream << "guild(" << GetID() << ") add repeat member(" << member->GetID() << ")." << LogStream::eos;
		return false;
	}

	if (member->GetPost() == GUILD_POST_LEADER)
	{
		m_Leader = member;
		m_LeaderName = member->GetName();
	}

	m_Members[member->GetID()] = member;

	GuildMgr::Instance()->AddGuildMember(member);

	UpdateMemberNum();

	return true;
}

GuildMember* Guild::FindMember(ObjID_t id)
{
	auto itr = m_Members.find(id);
	return itr == m_Members.end() ? NULL : itr->second;
}

void Guild::DelMember(GuildMember* member)
{
	if (!member)
	{
		return;
	}

	if (!FindMember(member->GetID()))
	{
		ErrorLogStream << "guild(" << GetID() << ") want to del unexist member(" << member->GetID() << ")." << LogStream::eos;
		return;
	}

	if (member->GetPlayer()!=nullptr)
	{
		member->GetPlayer()->SendGuildJoinExitUdpLog(1);
	}
	
	m_Members.erase(member->GetID());

	UpdateMemberNum();
	WSSortListMgr::Instance()->OnGuildMemeberChanged(GetID(), m_MemberNum);

	if (m_Table.IsInTable(member->GetID()))
	{
		m_Table.EraseMember(member->GetID());

		CLProtocol::WorldGuildTableDelMember sync;
		sync.id = member->GetID();
		Broadcast(sync);
	}

	// 这里不删除member对象，在GuildMgr中维护了所有的member对象
}

void Guild::GetGuildMemberList(std::vector<GuildMemberEntry>& memberList)
{
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member)
		{
			memberList.push_back(member->GetEntry());
		}
	}
}

void Guild::GetGuildMemberListByPost(std::vector<GuildMember*>& memberList, GuildPost post)
{
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		if (iter->second != NULL && iter->second->GetPost() == post)
		{
			memberList.push_back(iter->second);
		}
		++iter;
	}
}

void Guild::GetGuildMembers(std::vector<GuildMember*>& memberList)
{
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		if (iter->second != NULL)
		{
			memberList.push_back(iter->second);
		}
		++iter;
	}
}

void Guild::GetGuildEntry(WSPlayer* player, GuildEntry& entry)
{
	entry.id = GetID();
	entry.name = GetName();
	entry.level = GetLevel();
	entry.memberNum = GetMemberNum();
	entry.leaderName = m_Leader ? m_Leader->GetName() : "";
	entry.declaration = GetDeclaration();
	
	if (player && player->GetGuildID() == 0)
	{
		entry.isRequested = m_Requesters.find(player->GetID()) != m_Requesters.end() ? 1 : 0;
	}
	else
	{
		entry.isRequested = 0;
	}

	entry.occupyCrossTerrId = GetOccupyCrossTerrID();
	entry.occupyTerrId = GetOccupyTerrID();
	entry.joinLevel = GetJoinLevel();
	entry.leaderId = GetLeader() ? GetLeader()->GetID() : 0;
}

bool Guild::IsGuildFull() const
{
	if (!m_DataEntry)
	{
		return true;
	}

	if (m_Members.size() >= m_DataEntry->memberNum)
	{
		return true;
	}

	return false;
}

bool Guild::IsRequestQueueFull() const
{
	return m_Requesters.size() >= GUILD_REQUEST_QUEEU_MAX_SIZE;
}

bool Guild::IsRequesting(ObjID_t id) const
{
	return m_Requesters.find(id) != m_Requesters.end();
}

void Guild::DelRequester(ObjID_t id)
{
	m_Requesters.erase(id);
}

void Guild::NotifyRequesterIsEmpty()
{
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			continue;
		}

		if (!member->HasPower(GUILD_POWER_SETMEMBER))
		{
			continue;
		}

		// 清除请求者队列红点
		member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_REQUESTER);
		SyncMemberInfoToScene(member);
	}
}

UInt32 Guild::RequestJoinGuild(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::GUILD_SYS_ERROR;
	}

	//DebugLogStream << "player(" << player->GetID() << ") request join guild(" << GetID() << ")." << LogStream::eos;

	if (player->GetGuildID() > 0)
	{
		DebugLogStream << "player(" << player->GetID() << ") already in guild(" << player->GetGuildID() << ")." << LogStream::eos;
		return ErrorCode::GUILD_JOIN_ALREADY_HAS_GUILD;
	}

	// 等级
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_GUILD);
	if (functionData && player->GetLevel() < functionData->endLevel)
	{
		DebugLogStream << "player(" << player->GetID() << ") level limit(" << player->GetLevel() << ")." << LogStream::eos;
		return ErrorCode::GUILD_JOIN_MIN_LEVEL;
	}

	// 入会等级要求
	if (player->GetLevel() < GetJoinLevel())
	{
		DebugLogStream << "player(" << player->GetID() << ") join level limit playerLevel(" << player->GetLevel() << ") joinLevel(" << GetJoinLevel() << ")." << LogStream::eos;
		return ErrorCode::GUILD_JOIN_LEVEL_LIMIT_APPLY;
	}

	// 退出公会冷却时间
	auto* oldMember = GuildMgr::Instance()->FindGuildMember(player->GetID());
	if (oldMember != NULL && CURRENT_TIME.Sec() < oldMember->GetLeaveTime())
	{
		DebugLogStream << "player(" << player->GetID() << ") enter cold down." << LogStream::eos;
		return ErrorCode::GUILD_JOIN_COLDTIME;
	}

	// 公会是否已满
	if (IsGuildFull())
	{
		DebugLogStream << "player(" << player->GetID() << ") request join guild(" << GetID() << ") is full." << LogStream::eos;
		return ErrorCode::GUILD_FULL;
	}

	// 公会是不是正在解散
	if (GetDismissTime() > 0)
	{
		DebugLogStream << "player(" << player->GetID() << ") request join guild(" << GetID() << ") is in dismiss." << LogStream::eos;
		return ErrorCode::GUILD_JOIN_IN_DISMISS;
	}

	// 申请列表是否已满
	if (IsRequestQueueFull())
	{
		return ErrorCode::GUILD_JOIN_REQUEST_QUEUE_FULL;
	}

	// 是否已经申请过
	if (IsRequesting(player->GetID()))
	{
		return ErrorCode::GUILD_JOIN_REPEAT;
	}

	GuildRequesterInfo requester;
	requester.id = player->GetID();
	requester.name = player->GetName();
	requester.level = player->GetLevel();
	requester.occu = player->GetOccu();
	requester.vipLevel = player->GetVipLvl();
	requester.requestTime = UInt32(CURRENT_TIME.Sec());
	requester.seasonLevel = player->GetSeasonLv();
	player->GetPlayerLabelInfo(requester.playerLabelInfo);
	m_Requesters[player->GetID()] = requester;

	// 通知公会管理员
	CLProtocol::WorldGuildNewRequester notify;
	BroadcastByPower(notify, GUILD_POWER_SETMEMBER);

	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			continue;
		}

		if (!member->HasPower(GUILD_POWER_SETMEMBER))
		{
			continue;
		}

		// 添加请求者队列红点
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_REQUESTER);
		SyncMemberInfoToScene(member);
	}

	return ErrorCode::SUCCESS;
}

void Guild::ReplyJoinRequest(WSPlayer* player, ObjID_t targetId, bool agree)
{
	if (player == NULL || player->GetGuildID() != GetID())
	{
		return;
	}

	ErrorLogStream << "player(" << player->GetID() << ") guild(" << GetID() << ") reply requester(" << targetId << ") agree(" << agree << ")" << LogStream::eos;

	CLProtocol::WorldGuildProcessRequesterRes res;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		ErrorLogStream << "player(" << player->GetID() << ") not in guild(" << GetID() << ")." << LogStream::eos;
		return;
	}

	auto itr = m_Requesters.find(targetId);
	if (itr == m_Requesters.end()) 
	{
		ErrorLogStream << "player(" << player->GetID() << ") can't find requester(" << targetId << ") in guild(" << GetID() << ")." << LogStream::eos;
		res.result = ErrorCode::GUILD_REPLY_REQUESTER_UNEXIST;
		player->SendProtocol(res);
		return;
	}
	
	if (!HasGuildPower(member->GetPost(), GUILD_POWER_SETMEMBER))
	{
		ErrorLogStream << "player(" << player->GetID() << ") guild(" << GetID() << ") don't have reply requester power, post(" << (UInt8)member->GetPost() << ")." << LogStream::eos;
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	auto requester = itr->second;
	GuildMember* newMember = NULL;
	if (agree)
	{
		// 同意加入

		// 公会地下城期间不可入会
		if (!GuildDungeonMgr::Instance()->IsJoinOrLeaveGuild() && m_Level >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT))
		{
			res.result = ErrorCode::GUILD_DUNGEON_NOT_JOIN;
			player->SendProtocol(res);
			return;
		}

		if (IsGuildFull())
		{
			res.result = ErrorCode::GUILD_FULL;
			player->SendProtocol(res);
			return;
		}

		// 入会等级
		if (requester.level < GetJoinLevel())
		{
			res.result = ErrorCode::GUILD_JOIN_LEVEL_LIMIT_APPROVAL;
			player->SendProtocol(res);
			return;
		}

		auto* oldMember = GuildMgr::Instance()->FindGuildMember(targetId);
		if (oldMember != NULL)
		{
			// 已经在某个公会中
			if (oldMember->GetGuildID() > 0 && oldMember->GetLeaveTime() == 0)
			{
				res.result = ErrorCode::GUILD_REPLY_IN_OTHER_GUILD;
				player->SendProtocol(res);

				// 从申请列表中删除
				DelRequester(targetId);
				return;
			}
			else if (oldMember->GetGuildID() > 0 && CURRENT_TIME.Sec() < oldMember->GetLeaveTime())
			{
				// 刚从公会中退出
				res.result = ErrorCode::GUILD_REPLY_COLDTIME;
				player->SendProtocol(res);
				return;
			}
		}

		newMember = oldMember;
		if (newMember == NULL)
		{
			newMember = new GuildMember();
			GuildMgr::Instance()->AddGuildMember(newMember);
		}

		auto newPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(requester.id);
		if (newPlayer)
		{
			newMember->Init(newPlayer);
		}
		else
		{
			newMember->SetID(requester.id);
			newMember->SetName(requester.name);
			newMember->SetOccu(requester.occu);
			newMember->SetLevel(requester.level);
			newMember->SetVipLevel(requester.vipLevel);
			newMember->SetSeasonLevel(requester.seasonLevel);
			newMember->SetLeaveTime(0);
		}

		OnMemberEnter(player ,newMember);

		//更新公会头衔
		newMember->SetPost(GUILD_POST_NORMAL, true);

		// 工会邮件
		std::string reason = GetReason(SOURCE_TYPE_GUILD_MEMBER_REPLY_JOIN);
		std::string title = SysNotifyMgr::Instance()->MakeString(2050);
		std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
		std::string content = SysNotifyMgr::Instance()->MakeString(3005, GetName().c_str());
		std::vector<ItemReward> reward;
		MailBox::SendSysMail(newMember->GetID(), sender, title, content, reason, reward);

		if (newMember->GetPlayer())
		{
			newMember->GetPlayer()->SendNotify(3007, GetName().c_str());
		}
	}

	// 从申请列表中删除
	DelRequester(targetId);
	res.result = ErrorCode::SUCCESS;
	if (newMember)
	{
		res.entry = newMember->GetEntry();
	}
	
	player->SendProtocol(res);

	// 请求者列表空了之后要清除所有人的红点
	if (m_Requesters.empty())
	{
		for (auto itr : m_Members)
		{
			auto member = itr.second;
			if (!member)
			{
				continue;
			}

			if (!member->HasPower(GUILD_POWER_SETMEMBER))
			{
				continue;
			}

			// 清除请求者队列红点
			member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_REQUESTER);
			SyncMemberInfoToScene(member);
		}
	}
	
}

void Guild::RequestClearRequestList(WSPlayer* player)
{
	if (player == NULL || player->GetGuildID() != GetID())
	{
		return;
	}

	ErrorLogStream << "player(" << player->GetID() << ") guild(" << GetID() << ") clear request list." << LogStream::eos;

	auto member = FindMember(player->GetID());
	if (!member)
	{
		ErrorLogStream << "player(" << player->GetID() << ") not in guild(" << GetID() << ")." << LogStream::eos;
		return;
	}

	CLProtocol::WorldGuildProcessRequesterRes res;
	if (!HasGuildPower(member->GetPost(), GUILD_POWER_SETMEMBER))
	{
		ErrorLogStream << "player(" << player->GetID() << ") guild(" << GetID() << ") don't have reply requester power, post(" << (UInt8)member->GetPost() << ")." << LogStream::eos;
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	m_Requesters.clear();

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

std::vector<GuildRequesterInfo> Guild::GetRequesterList()
{
	std::vector<GuildRequesterInfo> requesters;
	for (auto itr : m_Requesters)
	{
		requesters.push_back(itr.second);
	}

	return requesters;
}

void Guild::RequestChangeGuildName(WSPlayer* player, const std::string& name, ObjID_t itemId, UInt32 itemTypeId)
{
	if (!player || player->GetGuildID() != GetID())
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to change guild name(" << name << ") item(" << itemId << ", " << itemTypeId << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_MODIFY_NAME;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_CHANGE_NAME))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	// 验证名字合法性
	res.result = GuildMgr::Instance()->IsNameValid(name);
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
		return;
	}

	auto trans = new GuildChangeNameTransaction(player, name);
	// 如果指定使用道具的话就使用那个道具
	if (itemId > 0)
	{
		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
		if (!itemData)
		{
			ErrorLogStream << "player(" << player->GetID() << ") request to change guild name(" << name << ") item(" << itemTypeId << ") not exist." << LogStream::eos;
			res.result = ErrorCode::GUILD_NO_ENOUGH_ITEM;
			player->SendProtocol(res);
			return;
		}
		else if (itemData->thirdType != TT_ITEM_CHANGE_GUILD_NAME)
		{
			ErrorLogStream << "player(" << player->GetID() << ") request to change guild name(" << name << ") item(" << itemTypeId << ") can't change guild name." << LogStream::eos;
			res.result = ErrorCode::GUILD_NO_ENOUGH_ITEM;
			player->SendProtocol(res);
			return;
		}

		trans->AddDetailItem(itemId, itemTypeId, 1);
	}
	else
	{
		// 安全锁开启，不允许消耗点券修改公会名
		if (!player->SecurityLockCheck())
		{
			delete trans;
			trans = NULL;
			return;
		}

		trans->SetGold(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_CHANGE_NAME_COST));
	}
	
	if (!player->BeginItemTransaction(trans))
	{
		return;
	}
}

void Guild::RequestChangeAnnouncement(WSPlayer* player, const std::string& content)
{
	if (!player || player->GetGuildID() != GetID())
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to change announcement(" << content << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_MODIFY_ANNOUNCE;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_CHANGE_ANNOUNCE))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	if (!GuildMgr::Instance()->IsStrValid(content, GUILD_ANNOUNCEMENT_MAX_WIDTH))
	{
		res.result = ErrorCode::GUILD_ANNOUNCEMENT_INVALID;
		player->SendProtocol(res);
		return;
	}

	m_Announcement = content;

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void Guild::RequestChangeDeclaration(WSPlayer* player, const std::string& content)
{
	if (!player || player->GetGuildID() != GetID())
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to change declaration(" << content << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_MODIFY_DECLAR;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_CHANGE_DECLAR))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	if (!GuildMgr::Instance()->IsStrValid(content, GUILD_DECLARATION_MAX_WIDTH))
	{
		res.result = ErrorCode::GUILD_DECLARATION_INVALID;
		player->SendProtocol(res);
		return;
	}

	m_Declaration = content;

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void Guild::SendGuildMail(WSPlayer* player, const std::string& content)
{
	if (!player || player->GetGuildID() != GetID())
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to send mail(" << content << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_SEND_MAIL;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_MAIL))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	if (!GuildMgr::Instance()->IsStrValid(content, GUILD_MAIL_MAX_WIDTH))
	{
		res.result = ErrorCode::GUILD_MAIL_INVALID;
		player->SendProtocol(res);
		return;
	}

	auto trans = new GuildMailTransaction(player, content);
	trans->SetCounter(COUNTER_GUILD_MAIL, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_MAIL_MAX_TIMES));
	if (!player->BeginItemTransaction(trans))
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return;
	}
}

void Guild::RequestChangePost(WSPlayer* player, ObjID_t targetId, ObjID_t replaceId, GuildPost post)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") want to change target(" << targetId << ")'s post to (" << (UInt8)post << ") replacer(" << replaceId << ")." << LogStream::eos;

	// 申请成为会长
	if (targetId == player->GetID() && post == GUILD_POST_LEADER)
	{
		CLProtocol::WorldGuildChangePostRes res;
		res.result = RequestBecomeLeader(player);
		player->SendProtocol(res);
		return;
	}

	auto member = FindMember(player->GetID());
	auto target = FindMember(targetId);

	if (!member || !target || member == target)
	{
		return;
	}

	CLProtocol::WorldGuildChangePostRes res;
	// 只能任命比自己职务低的人
	if (member->GetPost() <= target->GetPost() || 
		!_CanSetPost(member->GetPost(), post) ||
		target->GetPost() == post)
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	auto targetPostMemberNum = _GetMemberNumByPost(post);
	auto targetPostMaxNum = _GetMaxNumByPost(post);

	if (targetPostMemberNum >= targetPostMaxNum)
	{
		auto replacer = FindMember(replaceId);
		if (!replacer || replacer->GetPost() != post)
		{
			res.result = ErrorCode::GUILD_POST_FULL;
			player->SendProtocol(res);
			return;
		}

		replacer->SetPost(GUILD_POST_NORMAL);
		SyncMemberInfoToScene(replacer);
	}

	target->SetPost(post);
	if (target->HasPower(GUILD_POWER_SETMEMBER) && !m_Requesters.empty())
	{
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_REQUESTER);
	}
	else if (!target->HasPower(GUILD_POWER_SETMEMBER))
	{
		member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_REQUESTER);
	}
	SyncMemberInfoToScene(target);

	if (post == GUILD_POST_LEADER)
	{
		OnChangeLeader(target);
	}

	if (post == GUILD_POST_NORMAL)
	{
		if (target->GetPlayer())
		{
			target->GetPlayer()->SendNotify(3015, GetName().c_str());
		}

		// 发送公会日志
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3028, target->GetName()));
	}
	else if (post == GUILD_POST_ELDER)
	{
		if (target->GetPlayer())
		{
			target->GetPlayer()->SendNotify(3014, GetName().c_str());
		}

		// 发送公会日志
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3025, target->GetName()));
	}
	else if (post == GUILD_POST_ASSISTANT)
	{
		if (target->GetPlayer())
		{
			target->GetPlayer()->SendNotify(3013, GetName().c_str());
		}

		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3027, target->GetName()));
	}
	else if (post == GUILD_POST_LEADER)
	{
		if (target->GetPlayer())
		{
			target->GetPlayer()->SendNotify(3012, GetName().c_str());
		}

		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3026, player->GetName(),target->GetName(),target->GetName()));
	}

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void Guild::RequestKickPlayer(WSPlayer* player, ObjID_t targetId)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") want to kill player(" << targetId << ")." << LogStream::eos;

	auto member = FindMember(player->GetID());
	auto target = FindMember(targetId);

	if (!member || !target || member == target)
	{
		return;
	}

	CLProtocol::WorldGuildKickRes res;
	//已报名公会战或跨服公会战
	if (GetEnrollTerrID() > 0 && GuildBattleMgr::Instance()->IsGuildBattle())
	{
		res.result = ErrorCode::GUILD_BATTLE_NOT_LEAVE;
		player->SendProtocol(res);
		return;
	}

	// 是否在公会地下城活动期间
	if (!GuildDungeonMgr::Instance()->IsJoinOrLeaveGuild() && m_Level >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT))
	{
		res.result = ErrorCode::GUILD_DUNGEON_NOT_KICK;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_KICK))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	if (member->GetPost() <= target->GetPost())
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	target->SetPost(GUILD_POST_INVALID);
	target->SetLeaveTime((UInt32)CURRENT_TIME.Sec() + GUILD_ENTER_COLDDOWN_KICK);
	member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_TERR_ID);
	SyncMemberInfoToScene(target, true);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
	player->SendNotify(10003, target->GetName());

	// 公会日志
	AddGuildEvent(SysNotifyMgr::Instance()->MakeString(9981, target->GetName()));

	DelMember(target);

	InfoLogStream << "player(" << target->GetID() << ") kick by player(" << player->GetID() << ")." << LogStream::eos;

	// 工会邮件
	std::string reason = GetReason(SOURCE_TYPE_GUILD_MEMBER_KICK);
	std::string title = SysNotifyMgr::Instance()->MakeString(2050);
	std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
	std::string content = SysNotifyMgr::Instance()->MakeString(3006, GetName().c_str());
	std::vector<ItemReward> reward;
	MailBox::SendSysMail(target->GetID(), sender, title, content, reason, reward);

	if (target->GetPlayer())
	{
		target->GetPlayer()->SetGuildID(0);
		target->GetPlayer()->SetGuildLvl(0);
		target->GetPlayer()->GetRelationMgr().OnGuildChanged();
		target->GetPlayer()->SendNotify(3008, GetName().c_str());
		target->GetPlayer()->GetTitleMgr().OnGuidLeave();
	}
}

void Guild::RequestLeave(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") want leave guild(" << GetID() << ")." << LogStream::eos;

	CLProtocol::WorldGuildLeaveRes res;
	//已报名公会战或跨服公会战
	if (GetEnrollTerrID() > 0 && GuildBattleMgr::Instance()->IsGuildBattle())
	{
		res.result = ErrorCode::GUILD_BATTLE_NOT_LEAVE;
		player->SendProtocol(res);
		return;
	}

	// 是否在公会地下城活动期间,且公会开启工会地下城
	if (!GuildDungeonMgr::Instance()->IsJoinOrLeaveGuild()&& m_Level >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT))
	{
		res.result = ErrorCode::GUILD_DUNGEON_NOT_LEAVE;
		player->SendProtocol(res);
		return;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	// 会长要先转让给别人才能退帮
	if (member->GetPost() == GUILD_POST_LEADER)
	{
		res.result = ErrorCode::GUILD_LEAVE_TRANSFER_LEADER;
		player->SendProtocol(res);
		return;
	}

	player->SetGuildID(0);
	player->SetGuildLvl(0);
	member->SetPost(GUILD_POST_INVALID);
	member->SetLeaveTime((UInt32)CURRENT_TIME.Sec() + GUILD_ENTER_COLDDOWN_LEAVE);
	SyncMemberInfoToScene(member, true);

	member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_SHOP | RED_POINT_GUILD_REQUESTER | RED_POINT_GUILD_TERR_ID);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	// 修改公会红包
	RedPacketMgr::Instance()->OnPlayerChangeGuild(member->GetID(), GetID(), 0);

	// 公会频道退出消息,加入公会日志
	AddGuildEvent(SysNotifyMgr::Instance()->MakeString(9981, player->GetName()));
	player->GetTitleMgr().OnGuidLeave();
	player->GetRelationMgr().OnGuildChanged();

	DelMember(member);

	InfoLogStream << "player(" << player->GetID() << ") leave guild(" << GetID() << ")." << LogStream::eos;
}

void Guild::RequestUpgradeBuilding(WSPlayer* player, GuildBuildingType type)
{
	if (!player || !m_DataEntry)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") want upgrade guild(" << GetID() << ")'s building(" << (UInt8)type << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_UPGRADE_BUILDING;

	if (type >= GUILD_BUILDING_NUM)
	{
		return;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_UPDATE_BUILDING))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	UInt8 curLevel = GetBuildingLevel(type);
	// 判断是否达到该主城等级下的最高等级
	if (type != GUILD_BUILDING_MAIN)
	{
		if (curLevel >= m_DataEntry->buildingMaxLevel[type])
		{
			res.result = ErrorCode::GUILD_BUILDING_UPGRADE_MAIN_FIRST;
			player->SendProtocol(res);
			return;
		}
	}

	// 判断是否已经到满级
	auto buildingNextLvData = GuildBuildingDataMgr::Instance()->FindEntry(curLevel + 1);
	if (!buildingNextLvData)
	{
		res.result = ErrorCode::GUILD_BUILDING_TOP_LEVEL;
		player->SendProtocol(res);
		return;
	}

	// 升级需要资源
	UInt32 cost = buildingNextLvData->buildingCost[type];
	UInt32 curFund = GetFund();
	if (cost > curFund)
	{
		UInt32 needPoint = 0;
		UInt32 needFund = cost - curFund;
		UInt32 pointFund = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_POINT_FUND_NUM);
		if (pointFund == 0)
		{
			res.result = ErrorCode::GUILD_BUILDING_NOT_ENOUGH_FUND;
			player->SendProtocol(res);
		}
		
		needPoint = needFund / pointFund;
		if (needFund % pointFund != 0)
		{
			needPoint += 1;
		}

		GuildPointFundTransaction* trans = new GuildPointFundTransaction(player, needPoint * pointFund, curLevel + 1, type);
		trans->SetGold(needPoint);
		if (!player->BeginItemTransaction(trans))
		{
			res.result = ErrorCode::GUILD_BUILDING_POINT_FUND_TRANSACTION;
			player->SendProtocol(res);
		}
		return;
	}

	m_BuildingMgr.SetLevel(type, curLevel + 1);
	if (type == GUILD_BUILDING_MAIN)
	{
		auto levelData = GuildDataEntryMgr::Instance()->FindEntry(curLevel + 1);
		
		if (!levelData)
		{
			ErrorLogStream << "guild(" << GetID() << ") current level(" << GetLevel() << ") can't find guild data entry." << LogStream::eos;
			return;
		}

		m_DataEntry = levelData;
		SetLevel(curLevel + 1);
	}
	else if (type == GUILD_BUILDING_SHOP)
	{
		RefreshShop();
	}
	SetLastUpdateGuildBuildingTime(Avalon::Time::CurrentTime().Sec());//公会排行时用,必须要在排序前修改

	UpdateGuildLevelSortList();

	m_Fund = m_Fund - cost;
	
	// 广播到scene
	for (auto itr : m_Members)
	{
		SyncMemberInfoToScene(itr.second);
	}

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	// 通知公会地下城活动
	if (type == GUILD_BUILDING_MAIN)
	{
		GuildDungeonMgr::Instance()->GuildLevelUp(this);
	}

	auto newLevel = curLevel + 1;
	// 发送公会日志
	if (type == GUILD_BUILDING_MAIN)
	{
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3029, newLevel));
	}
	else if (type == GUILD_BUILDING_SHOP)
	{
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3030, newLevel));
	}
	else if (type == GUILD_BUILDING_BATTLE)
	{
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3031, newLevel));
	}
	else if (type == GUILD_BUILDING_WELFARE)
	{
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3032, newLevel));
	}
	else if (type == GUILD_BUILDING_DUNGEON)
	{
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3033, newLevel));
	}
	else if (type == GUILD_BUILDING_HONOUR)
	{
		GuildEmblemDataEntry* dataEntry = GuildEmblemDataEntryMgr::Instance()->GetMaxStageEmblemDataEntry(newLevel);
		if (dataEntry != NULL)
		{
			AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3037, newLevel,dataEntry->name,dataEntry->stageLevel ));
		}
	}
	else if (type == GUILD_BUILDING_ALTAR)
	{
		UInt32 dungeonType = GuildDungeonTypeDataEntryMgr::Instance()->GetGuildDungeonType(newLevel);

		std::string typeName = GuildDungeonTypeDataEntryMgr::Instance()->GetGuildDungeonTypeName(dungeonType);

		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3038, newLevel,typeName));
	}
	else
	{
		assert(0 && "invalid guild building type");
	}

	DebugLogStream << "player(" << player->GetID() << ") upgrade guild(" << GetID() << ")'s building(" << (UInt8)type << ") cur level(" << curLevel + 1 << ")." << LogStream::eos;
}

UInt32 Guild::RequestBecomeLeader(WSPlayer* player)
{
	if (!player)
	{
		return ErrorCode::GUILD_SYS_ERROR;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to become guild(" << GetID() << ")'s leader." << LogStream::eos;

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return ErrorCode::GUILD_NOT_IN_GUILD;
	}

	if (member->GetPost() == GUILD_POST_LEADER)
	{
		return ErrorCode::SUCCESS;
	}

	CLProtocol::WorldGuildChangePostRes res;

	// 只有副会长能申请成为会长
	if (member->GetPost() != GUILD_POST_ASSISTANT)
	{
		return ErrorCode::GUILD_NO_POWER;
	}

	if (m_Leader && (m_Leader->GetOfflineTime() == 0 ||
		CURRENT_TIME.Sec() < m_Leader->GetOfflineTime() + 7 * Avalon::Time::SECS_OF_DAY))
	{
		return ErrorCode::GUILD_POST_LEADER_LEAVE_TIME;
	}

	if (m_Leader)
	{
		m_Leader->SetPost(GUILD_POST_ASSISTANT);
		SyncMemberInfoToScene(m_Leader);
	}

	member->SetPost(GUILD_POST_LEADER);
	SyncMemberInfoToScene(member);
	OnChangeLeader(member);

	return ErrorCode::SUCCESS;
}

void Guild::RequestDismiss(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to dismiss guild(" << GetID() << ")." << LogStream::eos;

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_DISMISS;

	if (GuildBattleMgr::Instance()->IsEnroll(this))
	{
		res.result = ErrorCode::GUILD_BATTLE_ENROLL_NOT_DISMISS;
		player->SendProtocol(res);
		return;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_DISMISS))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	if (m_DismissTime > 0)
	{
		res.result = ErrorCode::GUILD_DISMISS_IN_DISMISS;
		player->SendProtocol(res);
		return;
	}

	auto trans = new GuildDismissTransaction(player);
	trans->SetCounter(COUNTER_GUILD_DISMISS, 1);
	if (!player->BeginItemTransaction(trans))
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return;
	}
}

void Guild::RequestCancelDismiss(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to cancel dismiss guild(" << GetID() << ")." << LogStream::eos;

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_CANCEL_DISMISS;

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_DISMISS))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return;
	}

	if (m_DismissTime == 0)
	{
		res.result = ErrorCode::GUILD_NOT_IN_DISMISS;
		player->SendProtocol(res);
		return;
	}

	// 通知客户端
	CancelDismissNotify();
	SendNotify(3010);
	//AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3010));

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void Guild::CancelDismissNotify()
{
	m_DismissTime = 0;

	// 广播邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(2050);
	std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
	std::string content = SysNotifyMgr::Instance()->MakeString(2053);
	std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
	_BroadcastMail(title, sender, content, reason);

	// 发送通知到公会日志
	AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3010));
}

UInt32 Guild::OnRequestPayRedPacket(WSPlayer* player, UInt16 reason, std::string name, UInt8 num)
{
	if (!player)
	{
		return ErrorCode::RED_PACKET_SYS_ERROR;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to buy pay_red_packet reason(" << reason << ") num(" << num << ")." << LogStream::eos;

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return ErrorCode::RED_PACKET_NOT_IN_GUILD;
	}

	if (player->GetVipValue(VIP_PRIVILEGE_GUILD_RED_PACKET) == 0)
	{
		// 没有权限
		ErrorLogStream << "player(" << player->GetID() << ") don't have vip privilege to buy red packet." << LogStream::eos;
		return ErrorCode::RED_PACKET_NO_VIP_PRIVILEGE;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_PAY_REDPACKET;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_RED_PACKET))
	{
		// 功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(reason);
	if (!dataEntry || dataEntry->subType != RPST_BUY || dataEntry->type != RED_PACKET_TYPE_GUILD)
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (!dataEntry->IsValidNum(num))
	{
		return ErrorCode::RED_PACKET_INVALID_NUM;
	}

	UInt32 ckeck_res = CheckGuildRedPacketCanBuy(player, *dataEntry);
	if (ckeck_res != ErrorCode::SUCCESS)
	{
		return ckeck_res;
	}

	auto trans = new GuildPayRedPacketTransaction(player, reason, name, num);
	trans->SetCounter(COUNTER_GUILD_PAY_RED_PACKET, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_PAY_RED_PACKET));
	trans->SetItem(dataEntry->costMoneyId, dataEntry->totalMoney);
	if (!player->BeginItemTransaction(trans))
	{
		return ErrorCode::GUILD_SYS_ERROR;
	}
	return ErrorCode::SUCCESS;
}

UInt32  Guild::CheckGuildRedPacketCanBuy(WSPlayer* player, RedPacketDataEntry& dataEntry)
{
	if (!player)
	{
		return ErrorCode::GUILD_SYS_ERROR;;
	}
	if (dataEntry.thrType == RPT_THR_TYPE_GUILD_ALL)
	{
		return ErrorCode::SUCCESS;
	}

	if (dataEntry.thrType == RPT_THR_TYPE_GUILD_BATTLE)
	{
		GuildBattleType guildBattleTy = GuildBattleMgr::Instance()->GetGuildBattleType();
		GuildBattleStatus guildBattleSt = GuildBattleMgr::Instance()->GetGuildBattleStatus();

		if (guildBattleTy == GBT_NORMAL
			&& guildBattleSt >= GBS_PREPARE && guildBattleSt <= GBS_BATTLE)
		{
			return ErrorCode::RED_PACKET_IN_GUILD_BATTLE;
		}
	}
	else if (dataEntry.thrType == RPT_THR_TYPE_GUILD_CROSS_BATTLE)
	{
		GuildBattleType guildBattleTy = GuildBattleMgr::Instance()->GetGuildBattleType();
		GuildBattleStatus guildBattleSt = GuildBattleMgr::Instance()->GetGuildBattleStatus();
	
		if (guildBattleTy == GBT_CROSS
			&& guildBattleSt >= GBS_PREPARE && guildBattleSt <= GBS_BATTLE)
		{
			return ErrorCode::RED_PACKET_IN_GUILD_CROSS_BATTLE;
		}
	}
	else if (dataEntry.thrType == RPT_THR_TYPE_GUILD_DUNGEON)
	{
		UInt32 guildDungeonSt = GuildDungeonMgr::Instance()->GetGuildDungeonStatus();
		if (guildDungeonSt >= GUILD_DUNGEON_PREPARE && guildDungeonSt <= GUILD_DUNGEON_START)
		{
			return ErrorCode::RED_PACKET_IN_GUILD_DUNGEON;
		}
	}
	else{
		ErrorLogStream << "player(" << player->GetID() << ") request to buy guild red packet reason(" << dataEntry.reason << ") thrType(" << dataEntry.thrType << ") is err." << LogStream::eos;
		return ErrorCode::GUILD_SYS_ERROR;
	}

	return ErrorCode::SUCCESS;
}

void Guild::RequestSetJoinLevel(WSPlayer* player, UInt32 joinLevel)
{
	CLProtocol::WorldGuildSetJoinLevelRes res;
	res.result = ErrorCode::SUCCESS;

	do 
	{
		auto member = FindMember(player->GetID());
		if (!member)
		{
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			break;
		}

		if (!HasGuildPower(member->GetPost(), GUILD_SET_JOIN_LEVEL))
		{
			res.result = ErrorCode::GUILD_NO_POWER;
			break;
		}

		UInt32 max_join_level = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_SET_MAX_JOIN_LEVEL);
		if (joinLevel > max_join_level)
		{
			joinLevel = max_join_level;
		}

		SetJoinLevel(joinLevel);

		Chat(SysNotifyMgr::Instance()->MakeString(9983, player->GetName(), joinLevel));

	} while (0);

	player->SendProtocol(res);
}

bool Guild::OnCheckChangeGuildNameRet(WSPlayer* player, const std::string& name)
{
	if (!player || player->GetGuildID() != GetID())
	{
		return false;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_MODIFY_NAME;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return false;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_CHANGE_NAME))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return false;
	}

	if (name == GetName())
	{
		res.result = ErrorCode::GUILD_NAME_REPEAT;
		player->SendProtocol(res);
		return false;
	}

	InfoLogStream << "guild(" << GetID() << ") new name(" << name << ")." << LogStream::eos;

	SetName(name);
	WSSortListMgr::Instance()->OnGuildNameChanged(GetID(), name);

	// 发送公会邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(2050);
	std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
	std::string content = SysNotifyMgr::Instance()->MakeString(2055, name.c_str());
	std::string reason = GetReason(SOURCE_TYPE_GUILD_CHANGE_NAME);
	_BroadcastMail(title, sender, content, reason);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member)
		{
			SyncMemberInfoToScene(member);
			WSPlayer* player = member->GetPlayer();
			if (player)
			{
				player->GetTitleMgr().OnGuidNameChange();
			}
		}
	}

	return true;
}

bool Guild::OnCheckMailCounterRet(WSPlayer* player, const std::string& content)
{
	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_SEND_MAIL;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return false;
	}

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_MAIL))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return false;
	}

	InfoLogStream << "player(" << player->GetID() << ") send guild mail." << LogStream::eos;

	// 发送邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(2050);
	std::string sender = player->GetName();
	std::string reason = GetReason(SOURCE_TYPE_GUILD_MAIL);
	_BroadcastMail(title, sender, content, reason);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	return true;
}

bool Guild::OnCheckDismissRet(WSPlayer* player)
{
	if (!player)
	{
		return false;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return false;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_DISMISS;

	if (!HasGuildPower(member->GetPost(), GUILD_POWER_DISMISS))
	{
		res.result = ErrorCode::GUILD_NO_POWER;
		player->SendProtocol(res);
		return false;
	}

	if (m_DismissTime > 0)
	{
		res.result = ErrorCode::GUILD_DISMISS_IN_DISMISS;
		player->SendProtocol(res);
		return false;
	}

	m_DismissTime = (UInt32)CURRENT_TIME.Sec() + GUILD_DISSMISS_WAIT_TIME;

	InfoLogStream << "player(" << player->GetID() << ") start dismiss guild(" << GetID() << ") dismiss time(" << (UInt32)m_DismissTime << ")." << LogStream::eos;
	
	// 广播工会邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(2050);
	std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
	std::string content = SysNotifyMgr::Instance()->MakeString(2052);
	std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
	_BroadcastMail(title, sender, content, reason);

	SendNotify(3009, member->GetName());

	AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3009,member->GetName()));

	res.result = ErrorCode::SUCCESS;
	res.param1 = m_DismissTime;
	player->SendProtocol(res);

	return true;
}

UInt32 Guild::OnCheckPayRedPacketRet(WSPlayer* player, UInt16 reason, std::string name, UInt8 num)
{
	if (!player)
	{
		return ErrorCode::RED_PACKET_SYS_ERROR;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return ErrorCode::GUILD_NOT_IN_GUILD;
	}

	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(reason);
	if (!dataEntry)
	{
		return ErrorCode::RED_PACKET_INVALID;
	}

	if (!dataEntry->IsValidNum(num))
	{
		return ErrorCode::RED_PACKET_INVALID_NUM;
	}

	// 添加红包
	ObjID_t redPacketId = 0;
	auto result = RedPacketMgr::Instance()->OnPlayerAddRedPacket(player, reason, name, redPacketId);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	// 发送红包
	result = RedPacketMgr::Instance()->OnPlayerSendRedPacket(player->GetID(), redPacketId, num);
	if (result != ErrorCode::SUCCESS)
	{
		RedPacketMgr::Instance()->DelRedPacketByID(redPacketId);
		return result;
	}

	player->OnSendRedPacketSuccess(redPacketId, reason, num, name);
	InfoLogStream << "player(" << player->GetID() << ") send pay_red_packet reason(" << reason << ") num(" << num << ")." << LogStream::eos;

	return ErrorCode::SUCCESS;
}

void Guild::RequestDonate(WSPlayer* player, GuildDonateType type, UInt8 num)
{
	if (!player || type > GUILD_DONATE_NUM || num == 0)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to donate type(" << (UInt8)type << ") num(" << num << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_DONATE;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_DONATE))
	{
		// 功能未开放
		res.result = ErrorCode::SYS_NOT_OPEN;
		player->SendProtocol(res);
		return;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}


	UInt32 cost = _GetDonateCost(type) * num;
	UInt32 maxCount = _GetDonateMaxCount(player, type);
	if (num > maxCount)
	{
		res.result = ErrorCode::GUILD_DONATE_NO_REMAIN_TIMES;
		player->SendProtocol(res);
		return;
	}

	auto trans = new GuildDonateTransaction(player, type, num);
	trans->SetCounter(_GetDonateCounter(type), maxCount, num);
	if (type == GUILD_DONATE_GOLD)
	{
		trans->SetBindSilver(cost);
	}
	else
	{
		trans->SetBindGold(cost);
	}
	if (!player->BeginItemTransaction(trans))
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
	}
}

bool Guild::OnCheckDonateRet(WSPlayer* player, GuildDonateType type, UInt8 num)
{
	if (!player)
	{
		return false;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_DONATE;
	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return false;
	}

	UInt32 contri = _GetDonateContribution(type, num);

	// 记录日志
	m_DonateLogMgr.PushLog(member->GetID(), member->GetName(), type, num, contri);

	// 添加贡献
	MemberAddContri(member, contri, GetReason(SOURCE_TYPE_GUILD_DONATE, type, num).c_str());

	DebugLogStream << "player(" << player->GetID() << ") donate succss, guild(" << GetID() << ") add fund(" << contri << ") current fund(" << GetFund() << ")." << LogStream::eos;

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	return true;
}

std::vector<GuildDonateLog> Guild::GetDonateLogs()
{
	std::vector<GuildDonateLog> logs;
	for (auto& log : m_DonateLogMgr.GetLogs())
	{
		if (log.name.empty())
		{
			auto member = GuildMgr::Instance()->FindGuildMember(log.id);
			if (!member)
			{
				continue;
			}
			log.name = member->GetName();
		}

		logs.push_back(log);
	}

	return logs;
}

void Guild::RequestUpgradeSkill(WSPlayer* player, UInt16 skillId)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to upgrade skill(" << skillId << ")." << LogStream::eos;

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_UPGRADE_SKILL;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_SKILL))
	{
		// 功能未开放
		res.result = ErrorCode::SYS_NOT_OPEN;
		player->SendProtocol(res);
		return;
	}

	UInt8 curLevel = player->GetGuildSkillLevel(skillId);

	// 下一级信息
	auto skillData = FindSkillEntry(skillId, curLevel + 1);
	if (!skillData || skillData->needBattleLevel > GetBuildingLevel(GUILD_BUILDING_BATTLE))
	{
		res.result = ErrorCode::GUILD_SKILL_TOP_LEVEL;
		player->SendProtocol(res);
		return;
	}

	auto trans = new GuildSkillUpgradeTransaction(player, skillId, curLevel + 1);
	// 设置消耗的资源
	trans->SetBindSilver(skillData->goldCost);
	trans->SetGuildContri(skillData->contriCost);
	if (!player->BeginItemTransaction(trans))
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return;
	}
}

bool Guild::OnCheckUpgradeSkillRet(WSPlayer* player, UInt16 skillId, UInt8 level)
{
	if (!player)
	{
		return false;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return false;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_UPGRADE_SKILL;

	UInt8 curLevel = player->GetGuildSkillLevel(skillId);
	if (curLevel != level - 1)
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return false;
	}
	
	player->SetGuildSkillLevel(skillId, level);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	InfoLogStream << "player(" << player->GetID() << ") upgrade skill(" << skillId << "), level(" << level << ")." << LogStream::eos;

	return true;
}

void Guild::RefreshShop()
{
	UInt8 shopId = _GetShopIDByLevel();
	auto data = ShopDataEntryMgr::Instance()->FindEntry(shopId);
	if (!data || data->refreshTimes.empty())
	{
		ErrorLogStream << "guild(" << GetID() << ") shop(" << shopId << ") error.";
		return;
	}

	Int32 refreshHour = data->refreshTimes[0];

	Avalon::Date nextRefreshDate = CURRENT_DATE;
	nextRefreshDate.Hour(refreshHour);
	nextRefreshDate.Min(0);
	nextRefreshDate.Sec(0);
	nextRefreshDate.MSec(0);

	UInt32 refreshTime = (UInt32)nextRefreshDate.ToTime().Sec();
	if (CURRENT_DATE.Hour() >= refreshHour)
	{
		refreshTime += Avalon::Time::SECS_OF_DAY;
	}

	m_ShopRefreshTime = refreshTime;

	// 商店ID变化了，同步到scene
	m_ShopId = shopId;

	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			continue;
		}

		SyncMemberInfoToScene(member);
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_SHOP);
	}

	OnRefreshShop();
}

void Guild::OnRefreshShop()
{
	char buffer[CL_MAX_CHAT_LEN + 1];
	if (!SysNotifyMgr::Instance()->MakeNotify(3019, buffer, CL_MAX_CHAT_LEN )) return;
	Chat(std::string(buffer), true);
}

void Guild::RequestLeaderInfo(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	if (!m_Leader)
	{
		ErrorLogStream << "guild(" << GetID() << ") can't find leader." << LogStream::eos;
		return;
	}

	CLProtocol::WorldGuildLeaderInfoRes res;
	res.info.id = m_Leader->GetID();
	res.info.name = m_Leader->GetName();
	res.info.occu = m_Leader->GetOccu();
	res.info.avatar = m_Leader->GetAvatar();
	res.info.popularoty = m_LeaderPopularoty;
	m_Leader->GetPlayerLabelInfo(res.info.playerLabelInfo);
	player->SendProtocol(res);
}

void Guild::RequestOrz(WSPlayer* player, GuildOrzType type)
{
	if (!player)
	{
		return;
	}

	if (type >= GUILD_ROZ_NUM)
	{
		return;
	}

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_ORZ))
	{
		// 功能未开放
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_ORZ;
		res.result = ErrorCode::SYS_NOT_OPEN;
		player->SendProtocol(res);
		return;
	}

	if (type == GUILD_ORZ_HIGH && player->GetVipValue(VIP_PRIVILEGE_GUILD_LUXURY_ORZ) == 0)
	{
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_ORZ;
		res.result = ErrorCode::GUILD_ORZ_VIP_PRIVILEGE;
		player->SendProtocol(res);
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to orz(" << (UInt8)type << ")." << LogStream::eos;

	auto statueData = GuildStatueEntryMgr::Instance()->FindEntry(type);
	if (!statueData)
	{
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_ORZ;
		res.result = ErrorCode::GUILD_ORZ_INVALID_TYPE;
		player->SendProtocol(res);
		return;
	}

	auto trans = new GuildOrzTransaction(player, type);
	if (statueData->costItemId == 600000007)
	{
		trans->SetBindSilver(statueData->costItemNum);
	}
	else
	{
		trans->SetBindGold(statueData->costItemNum);
	}
	trans->SetCounter(COUNTER_GUILD_ORZ, _GetOrzMaxTime());
	if (!player->BeginItemTransaction(trans))
	{
		CLProtocol::WorldGuildOperRes res;
		res.type = GUILD_OP_ORZ;
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return;
	}
}

bool Guild::CheckOrzRet(WSPlayer* player, GuildOrzType type)
{
	if (!player)
	{
		return false;
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_ORZ;

	do {
		auto member = FindMember(player->GetID());
		if (!member)
		{
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			break;
		}

		auto statueData = GuildStatueEntryMgr::Instance()->FindEntry(type);
		if (!statueData)
		{
			res.result = ErrorCode::GUILD_ORZ_INVALID_TYPE;
			break;
		}

		UInt32 contri = statueData->awardContri;

		// 添加贡献
		MemberAddContri(member, contri, GetReason(SOURCE_TYPE_GUILD_ORZ, type).c_str());

		// 加人气
		m_LeaderPopularoty = m_LeaderPopularoty + 1;

		res.result = ErrorCode::SUCCESS;

		InfoLogStream << "player(" << player->GetID() << ") orz(" << (UInt8)type << ") success." << LogStream::eos;

		if (type == GUILD_ORZ_HIGH)
		{
			Chat(SysNotifyMgr::Instance()->MakeString(3034, player->GetName(), contri));
		}
	} while (0);
	
	player->SendProtocol(res);
	return res.result == ErrorCode::SUCCESS;
}

void Guild::RequestJoinTable(WSPlayer* player, UInt8 seat, UInt8 type)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to join table seat(" << (UInt8)seat << ") type(" << type << ")." << LogStream::eos;

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_TABLE;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_TABLE))
	{
		// 圆桌会议功能未开放
		res.result = ErrorCode::SYS_NOT_OPEN;
		player->SendProtocol(res);
		return;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	auto data = GuildTableDataEntryMgr::Instance()->FindEntry(type);
	if (!data)
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return;
	}

	if (data->maxTimes == 0)
	{
		CheckJoinTable(player, seat, type);
		return;
	}

	auto trans = new GuildTableTransaction(player, seat, type);
	trans->SetCounter(_GetTableCounterName(type), data->maxTimes);
	if (!player->BeginItemTransaction(trans))
	{
		res.result = ErrorCode::GUILD_SYS_ERROR;
		player->SendProtocol(res);
		return;
	}
}

bool Guild::CheckJoinTable(WSPlayer* player, UInt8 seat, UInt8 type)
{
	if (!player)
	{
		return false;
	}

	// 如果seat是-1，就随便找一个空的位置
	if (seat == 0xFF)
	{
		seat = m_Table.GetEmptyPos();
	}

	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_TABLE;

	auto member = FindMember(player->GetID());
	do 
	{
		if (!member)
		{
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			break;
		}

		res.result = m_Table.JoinMember(member->GetID(), seat, type);
		if (res.result != ErrorCode::SUCCESS)
		{
			break;
		}

		// 通知有新的圆桌成员
		CLProtocol::WorldGuildTableNewMember sync;
		sync.member = member->GetTableMember(seat, type);
		Broadcast(sync);

		InfoLogStream << "player(" << player->GetID() << ") join table seat(" << (UInt8)seat << ") type(" << type << ")." << LogStream::eos;
	} while (0);
	
	player->SendProtocol(res);

	// 圆桌满了
	if (m_Table.IsFull())
	{
		OnTableFull();
	}

	return res.result == ErrorCode::SUCCESS;
}

void Guild::OnTableFull()
{
	auto tableMembers = m_Table.GetMembers();

	InfoLogStream << "guild(" << GetID() << ")'s table full, start to send reward." << LogStream::eos;

	for (auto tableMember : tableMembers)
	{
		auto member = FindMember(tableMember.id);
		if (!member)
		{
			ErrorLogStream << "guild(" << GetID() << ") can't find member(" << tableMember.id << ") but can't find him in guild." << LogStream::eos;
			continue;
		}

		// 发放奖励
		auto data = GuildTableDataEntryMgr::Instance()->FindEntry(tableMember.type);
		if (!data)
		{
			ErrorLogStream << "guild(" << GetID() << ") can't find member(" << tableMember.id << ")'s table data type(" << tableMember.type << ")." << LogStream::eos;
			continue;
		}

		std::string reason = GetReason(SOURCE_TYPE_GUILD_TABLE);
		std::string title = SysNotifyMgr::Instance()->MakeString(2050);
		std::string sender = SysNotifyMgr::Instance()->MakeString(2051);
		std::string content = SysNotifyMgr::Instance()->MakeString(data->mailContentId);
		MailBox::SendSysMail(member->GetID(), sender, title, content, reason, data->rewards);
	}

	m_Table.ClearMembers();

	CLProtocol::WorldGuildTableFinish finish;
	Broadcast(finish);
}

void Guild::OnGuildBattleRaceEnd(UInt32 terrId, UInt32 score)
{
	GuildTerritoryDataEntry* terrDataEntry = GuildTerritoryDataEntryMgr::Instance()->FindEntry(terrId);
	if (NULL == terrDataEntry)
	{
		ErrorLogStream << "not find guild terr config guildid:" << GetID() << " terrId:" << terrId << LogStream::eos;
		return;
	}

	if (terrDataEntry->type == GuildTerritoryType::GTT_CROSS)
	{
		m_GuildBattleScore = m_GuildBattleScore + score;
		return;
	}

	// 如果打开取前n名工会成员的积分算总积分，那么总积分在更新完所有玩家的积分后算
	UInt32 numRank = WSApplication::Instance()->GetGuildBattleScoreRank();
	if (numRank != 0)
	{
		return;
	}

	m_GuildBattleScore = m_GuildBattleScore + score;
	UpdateGuildBattleWeekScoreRank(score);

	/*
	SortListEntry entry;
	entry.sortType = SORTLIST_GUILD_BATTLE_SCORE;
	entry.id = GetID();
	entry.name = GetName();
	entry.ownerId = GetLeader()->GetID();
	entry.value1 = m_GuildBattleScore;
	entry.value3 = m_EnrollTerrId;
	WSSortListMgr::Instance()->UpdateSortList(entry);
	*/
}

void Guild::ClearEnroll()
{
	m_Inspire = 0;
	m_EnrollTerrId = 0;
	m_GuildBattleScore = 0;
	m_EnrollPlayerId = 0;
}

UInt32 Guild::Inspire()
{
	m_Inspire = m_Inspire + 1;

	return ErrorCode::SUCCESS;
}

UInt32 Guild::GetGuildBattleInspireInfo(std::vector<GuildBattleInspireInfo>& infos)
{
	infos.clear();
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		if (iter->second != NULL && iter->second->GetGuildBattleInspire() > 0)
		{
			GuildBattleInspireInfo info;
			info.inspireNum = iter->second->GetGuildBattleInspire();
			info.playerId = iter->second->GetID();
			info.playerName = iter->second->GetName();
			infos.push_back(info);
		}
		++iter;
	}

	return ErrorCode::SUCCESS;
}

UInt32 Guild::SetGuildStorageSetting(GuildStorageSetting type, UInt32 value)
{
	switch (type)
	{
	case GUILD_POST_INVALID:
		return ErrorCode::GUILD_NO_POWER;
	case GSS_WIN_PROBABILITY:
		if (value < GUILD_BATTLE_REWARD_PROBABILITY_MIN || value > GUILD_BATTLE_REWARD_PROBABILITY_MAX)
		{
			return ErrorCode::GUILD_BATTLE_SET_PROBABILITY_ERROR;
		}
		m_WinProbability = (UInt8)value;
		break;
	case GSS_LOSE_PROBABILITY:
		if (value < GUILD_BATTLE_REWARD_PROBABILITY_MIN || value > GUILD_BATTLE_REWARD_PROBABILITY_MAX)
		{
			return ErrorCode::GUILD_BATTLE_SET_PROBABILITY_ERROR;
		}
		m_LoseProbability = (UInt8)value;
		break;
	case GSS_STORAGE_ADD_POST:
		if (value < GuildPost::GUILD_POST_INVALID || value > GuildPost::GUILD_MANAGER_END)
		{
			return ErrorCode::GUILD_BATTLE_SET_SETTING_ERROR;
		}
		m_StorageAddPost = (UInt8)value;
		break;
	case GSS_STORAGE_DEL_POST:
		if (value < GuildPost::GUILD_POST_INVALID || value > GuildPost::GUILD_MANAGER_END)
		{
			return ErrorCode::GUILD_BATTLE_SET_SETTING_ERROR;
		}
		m_StorageDelPost = (UInt8)value;
		break;
	case GSS_MAX:
		return ErrorCode::GUILD_NO_POWER;
	default:
		return ErrorCode::GUILD_NO_POWER;
	}
	return ErrorCode::SUCCESS;
}

void Guild::SendGuildBattleReward(const std::vector<ItemReward>& leaderReward, const std::vector<ItemReward>& memberReward)
{
	if (leaderReward.empty() || memberReward.empty()) return;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(3040, buffer, sizeof(buffer));
	std::string	strTitle(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3041, buffer, sizeof(buffer));
	std::string strSenderName(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3042, buffer, sizeof(buffer));
	std::string	strContent(buffer);
	std::string strReason = GetReason(SOURCE_TYPE_GUILD_BATTLE_WIN, GetID(), GetOccupyTerrID());

	UInt64 leaderId = 0;
	if (GetLeader() != NULL)
	{
		leaderId = GetLeader()->GetID();
	}

	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		if (iter->first == leaderId)
			MailBox::SendSysMail(iter->first, strTitle, strSenderName, strContent, strReason, leaderReward);
		else
			MailBox::SendSysMail(iter->first, strTitle, strSenderName, strContent, strReason, memberReward);
		++iter;
	}
}

void Guild::SendGuildBattleLoseReward(std::vector<ItemReward> itemReward)
{
	if (itemReward.size() <= 0) return;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(3046, buffer, sizeof(buffer));
	std::string	strTitle(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3047, buffer, sizeof(buffer));
	std::string strSenderName(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3048, buffer, sizeof(buffer));
	std::string	strContent(buffer);
	std::string strReason = GetReason(SOURCE_TYPE_GUILD_BATTLE_LOSE);

	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		MailBox::SendSysMail(iter->first, strTitle, strSenderName, strContent, strReason, itemReward);
		++iter;
	}
}

void Guild::SendGuildLeaderReward(std::vector<ItemReward> itemReward)
{
	if (m_Leader == NULL) return;
	if (itemReward.size() <= 0) return;

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(3043, buffer, sizeof(buffer));
	std::string	strTitle(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3044, buffer, sizeof(buffer));
	std::string strSenderName(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3045, buffer, sizeof(buffer));
	std::string	strContent(buffer);
	std::string strReason = GetReason(SOURCE_TYPE_GUILD_BATTLE_WIN, GetID(), GetOccupyTerrID());
	MailBox::SendSysMail(m_Leader->GetID(), strTitle, strSenderName, strContent, strReason, itemReward);
}

void Guild::SendGuildMemberReward(std::vector<ItemReward> itemReward)
{
	if (itemReward.size() <= 0) return;

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(3051, buffer, sizeof(buffer));
	std::string	strTitle(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3052, buffer, sizeof(buffer));
	std::string strSenderName(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3053, buffer, sizeof(buffer));
	std::string	strContent(buffer);
	std::string strReason = GetReason(SOURCE_TYPE_GUILD_BATTLE_WIN, GetID(), GetOccupyTerrID());
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		if (m_Leader == NULL || m_Leader->GetID() == iter->second->GetID())
		{
			iter++;
			continue;
		}
		MailBox::SendSysMail(iter->first, strTitle, strSenderName, strContent, strReason, itemReward);
		++iter;
	}
}

void Guild::SetGuildBattleFigureStatue()
{
	//设置雕像
	if (GetLeader() != NULL)
	{
		FigureStatueMgr::Instance()->OnCreateFigureStatue(GetLeader()->GetID(), (UInt8)FigureStatueType::FST_GUILD, GetName());
	}
	else
	{
		FigureStatueMgr::Instance()->OnCreateFigureStatue(0, (UInt8)FigureStatueType::FST_GUILD, GetName());
	}
	std::vector<GuildMember*> memberList;
	GetGuildMemberListByPost(memberList, GuildPost::GUILD_POST_ASSISTANT);
	UInt32 memberListSize = memberList.size();

	if (memberListSize == 1)
	{
		FigureStatueMgr::Instance()->OnCreateFigureStatue(memberList[0]->GetID(), (UInt8)FigureStatueType::FST_GUILD_ASSISTANT, GetName());
		FigureStatueMgr::Instance()->OnCreateFigureStatue(0, (UInt8)FigureStatueType::FST_GUILD_ASSISTANT_TWO, "");
	}
	else if (memberListSize == 2)
	{
		FigureStatueMgr::Instance()->OnCreateFigureStatue(memberList[0]->GetID(), (UInt8)FigureStatueType::FST_GUILD_ASSISTANT, GetName());
		FigureStatueMgr::Instance()->OnCreateFigureStatue(memberList[1]->GetID(), (UInt8)FigureStatueType::FST_GUILD_ASSISTANT_TWO, GetName());
	}
	else
	{
		FigureStatueMgr::Instance()->OnCreateFigureStatue(0, (UInt8)FigureStatueType::FST_GUILD_ASSISTANT, "");
		FigureStatueMgr::Instance()->OnCreateFigureStatue(0, (UInt8)FigureStatueType::FST_GUILD_ASSISTANT_TWO, "");
	}
}

void Guild::ResultChallengeEnroll(std::vector<ItemReward> itemReward, std::string guildName, UInt32 price)
{

	UInt64 playerId = GetEnrollPlayer();
	SetEnrollPlayer(0);

	if (itemReward.size() <= 0) return;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(3060, buffer, sizeof(buffer));
	std::string	strTitle(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3061, buffer, sizeof(buffer));
	std::string strSenderName(buffer);
	SysNotifyMgr::Instance()->MakeNotify(3062, buffer, sizeof(buffer), guildName.c_str(), price);
	std::string	strContent(buffer);
	std::string strReason = GetReason(SOURCE_TYPE_GUILD_BATTLE_CHALLENGE_RESULT);
	MailBox::SendSysMail(playerId, strTitle, strSenderName, strContent, strReason, itemReward);
}

void Guild::AddStorageItem(WSPlayer* player, std::vector<GuildStorageItemInfo>& inItems, AddGuildStorageItemCb* cb)
{
	m_guildStorage.AddItem(player, inItems, cb);
}

void Guild::AddStorageItemCb(UInt64 playerUid, UInt32 code, std::vector<WSItem*>& items)
{
	m_guildStorage.RealAddItem(playerUid, code, items);
}

void Guild::GetStorageItemCb(UInt64 playerUid, WSItem* item, UInt64 oldUid)
{
	m_guildStorage.RealGetItem(playerUid, item, oldUid);
}

WSItem* Guild::QueryStorageItem(UInt64 uid)
{
	return m_guildStorage.QueryItemInfo(uid);
}

bool Guild::CheckStorageCapacity(WSItem* item)
{
	return m_guildStorage.CheckCapacity(item);
}

bool Guild::ShopBuyAddStorage(WSPlayer* player, WSItem* item)
{
	return m_guildStorage.ShopBuyAddItem(player, item);
}

void Guild::GetStorageItems(std::vector<GuildStorageItemInfo>& items)
{
	items = m_guildStorage.QueryItems();
}

void Guild::GetStorageRecords(std::vector<GuildStorageOpRecord>& record)
{
	record = m_guildStorage.QueryOpRecord();
}

UInt32 Guild::DelStorageItems(std::vector<GuildStorageDelItemInfo>& delItems, std::vector<GuildStorageItemInfo>& itemInfos)
{
	for (UInt32 i = 0; i < delItems.size(); ++i)
	{
		GuildStorageItemInfo info;
		if (m_guildStorage.DelItem(delItems[i].uid, delItems[i].num, info))
		{
			itemInfos.push_back(info);
		}
	}
	return ErrorCode::SUCCESS;
}

void Guild::InsertStorageItemRecords(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records)
{
	m_guildStorage.InsertOpRecord(name, type, items, records);
}

UInt32 Guild::GetStorageMaxSize()
{
	return m_guildStorage.GetTotalSize();
}

void Guild::SetEnrollPlayer(UInt64 playerId)
{
	m_EnrollPlayerId = playerId;
}

UInt64 Guild::GetEnrollPlayer()
{
	return m_EnrollPlayerId;
}

UInt32 Guild::Lottery(WSPlayer* player, bool isWin, bool isCheck)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	GuildMember* member = FindMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (isCheck)
	{
		if (!member->IsCanLottery()) return ErrorCode::GUILD_BATTLE_PLAYER_NOT_LETTORY;
	}

	member->SetGuildBattleLotteryStatus(GuildBattleLotteryStatus::GBLS_FIN);
	SyncMemberInfoToScene(member);

	UInt32 proba = 0;

	if (isWin)
	{
		proba = GetWinProbability();
	}
	else
	{
		proba = GetLoseProbability();
	}

	class LotteryGuildStorageItem : public GetGuildStorageItemCb
	{
	public:
		LotteryGuildStorageItem(WSPlayer* player, Guild* guild) :m_Player(player), m_Guild(guild) { }
		virtual ~LotteryGuildStorageItem() { }

		void OnSuccess(WSItem* item, GuildStorageItemInfo& itemInfo)
		{
			if (m_Player == NULL || m_Guild == NULL) return;

			if (item == NULL)
			{
				m_Guild->GiveContribution(m_Player);
				return;
			}

			std::vector<GuildStorageItemInfo> recordItemInfos;
			GuildStorageItemInfo recordItemInfo;
			recordItemInfo.dataId = item->m_DataId;
			recordItemInfo.num = item->m_Num;
			recordItemInfo.str = item->m_Strengthen;
			recordItemInfo.uid = item->GetID();
			recordItemInfos.push_back(recordItemInfo);

			CLProtocol::WorldGuildStorageItemSync sync;
			sync.items.push_back(itemInfo);
			m_Guild->InsertStorageItemRecords(m_Player->GetName(), GuildStorageOpType::GSOT_GET, recordItemInfos, sync.itemRecords);
			m_Guild->Broadcast(sync);

			Avalon::Packet *packet = Avalon::Packet::Create();
			packet->SetID(CLProtocol::WORLD_GUILD_BATTLE_LOTTERY_RES);
			Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());

			UInt32 nRes = ErrorCode::SUCCESS;
			UInt32 contribution = 0;
			bool ret = false;
			try
			{
				stream & nRes & contribution & item->GetID() & item->m_DataId;
				ret = item->Encode(stream, SST_SELF);
				stream & UInt64(0);
			}
			catch (const Avalon::NetException&)
			{
				Avalon::Packet::Destroy(packet);
			}
			packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
			if (!ret)
			{
				Avalon::Packet::Destroy(packet);
				return;
			}
			m_Player->SendPacket(packet);
			Avalon::Packet::Destroy(packet);
		}

		void OnFailed(UInt32 code)
		{
			if (m_Player == NULL || m_Guild == NULL) return;

			m_Guild->GiveContribution(m_Player);
		}
	
	private:
		WSPlayer* m_Player;

		Guild* m_Guild;
	};

	

	std::vector<GuildStorageItemInfo> infos = m_guildStorage.QueryItems();
	UInt32 randNum = Avalon::Random::RandBetween(1, 100);
	if (proba >= randNum && infos.size() > 0)
	{
		randNum = Avalon::Random::RandBetween(1, infos.size());
		
		UInt32 index = randNum - 1;
		UInt32 num = 1;

		GuildBattleLotteryNumDataEntry* lotteryNumDataEntry = GuildBattleLotteryNumDataEntryMgr::Instance()->FindEntry(infos[index].dataId);
		if (lotteryNumDataEntry != NULL)
		{
			num = lotteryNumDataEntry->num;
		}
		
		LotteryGuildStorageItem* lotteryCb = new LotteryGuildStorageItem(player, this);

		m_guildStorage.GetItem(player, index, num, lotteryCb);
	}
	else
	{
		return GiveContribution(player);
	}
	return ErrorCode::SUCCESS;
}

UInt32 Guild::GiveContribution(WSPlayer* player)
{
	UInt32 nRes = ErrorCode::SUCCESS;
	UInt32 contribution = 0;

	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	GuildMember* member = FindMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (nRes == ErrorCode::SUCCESS)
	{
		std::vector<UInt32> contributionRands = { 100, 300, 500, 800, 1000 };
		UInt32 randNum = Avalon::Random::RandBetween(1, contributionRands.size());
		contribution = contributionRands[randNum - 1];
		MemberAddContri(member, contribution, GetReason(LogSourceType::SOURCE_TYPE_GUILD_LOTTRY).c_str());
	}

	/*
	CLProtocol::WorldGuildBattleLotteryRes res;
	res.result = ErrorCode::SUCCESS;
	res.contribution = contributionRands[randNum - 1];
	player->SendProtocol(res);
	*/

	Avalon::Packet *packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_GUILD_BATTLE_LOTTERY_RES);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	UInt64 uId = 0;
	try
	{
		stream & nRes & contribution & uId;
	}
	catch (const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return nRes;
	}
	packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	return nRes;
}

void Guild::OnPlayerOnline(WSPlayer* player, bool dayChanged)
{
	if (!player)
	{
		return;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return;
	}

	member->SetPlayer(player);
	member->SetOfflineTime(0);
	player->SetGuildID(GetID());
	player->SetGuildLvl(GetLevel());

	// 发一个当前徽记等级的头衔
	GuildEmblemDataEntry* emblemEntry = GuildEmblemDataEntryMgr::Instance()->FindEntry(member->GetEmblemLevel());
	if (emblemEntry != NULL)
	{
		if (NULL == player->GetTitleMgr().GetOneTitleByDataId(emblemEntry->titleId))
		{
			player->GetTitleMgr().AddOneTitleByTitleId(emblemEntry->titleId);
		}
	}

	if (member->HasPower(GUILD_POWER_SETMEMBER) && !m_Requesters.empty())
	{
		// 添加请求者队列红点
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_REQUESTER);
	}
	else
	{
		//清除请求者队列红点
		member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_REQUESTER);
	}

	if (dayChanged)
	{
		// 隔了一天登录，添加商店红点
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_SHOP);

		// 公会领地奖励
		if (GetOccupyTerrID() == 0 || member->GetGuildBattleTerrDayReward() == 0)
		{
			InfoLogStream << PLAYERINFO(player) << "remove terr reward red point!" << LogStream::eos;
			member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_TERR_ID);
		}
		else if (GetOccupyTerrID() != 0 && member->GetGuildBattleTerrDayReward() != 0)
		{
			InfoLogStream << PLAYERINFO(player) << "add terr reward red point!" << LogStream::eos;
			member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_TERR_ID);
		}
	}

	if (member->GetPost() == GUILD_POST_LEADER && this->GetProsperity() == GUILD_PROSPERITY_MID)
	{
		member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_MERGER);
	}

	// 加入7天后成为精英
	if (member->GetPost() == GUILD_POST_NORMAL && 
		CURRENT_TIME.Sec() - member->GetEnterTime() >= 7 * Avalon::Time::SECS_OF_DAY)
	{
		member->SetPost(GUILD_POST_ELITE);

		// 发送提示到聊天栏
		Chat(SysNotifyMgr::Instance()->MakeString(3035, player->GetName()));
	}

	SyncBaseInfo(member);
	SyncMemberInfoToScene(member);
	SyncEmblemToScene(member);
	SyncMergerInfo();

	player->SendUdpLog("guild_member_login", LOG_GUILD_MEMBER_LOGIN, GetID(), GetName().c_str(), GetLevel(), GetMemberNum());

	if (player->IsVeteranReturn() && player->IsReturnFirstLogin())
	{
		UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RETURN_HELL_DROP_DAILY_MAX_NUM);

		CLProtocol::SceneSyncChat protocol;
		protocol.channel = CHAT_CHANNEL_TRIBE;
		protocol.objid = player->GetID();
		protocol.occu = player->GetOccu();
		protocol.sex = player->GetSex();
		protocol.level = player->GetLevel();
		protocol.viplvl = player->GetVipLvl();
		protocol.objname = player->GetName();
		protocol.bLink = false;
		protocol.isGm = 0;
		protocol.voiceKey = "";
		protocol.voiceDuration = 0;
		protocol.word = SysNotifyMgr::Instance()->MakeString(3036, num);
		protocol.headFrame = player->GetHeadFrame();
		Broadcast(protocol);
	}
}

void Guild::OnPlayerOffline(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	auto member = FindMember(player->GetID());
	if (!member)
	{
		return;
	}

	member->SetPlayer(NULL);
	member->SetOfflineTime((UInt32)CURRENT_TIME.Sec());
}

void Guild::OnMemberEnter(WSPlayer* replyPlayer, GuildMember* member)
{
	if (!member || !replyPlayer)
	{
		return;
	}

	if (!AddMember(member))
	{
		return;
	}
	WSSortListMgr::Instance()->OnGuildMemeberChanged(GetID(), m_MemberNum);

	// 去掉该玩家申请的其它公会中的申请列表
	GuildMgr::Instance()->ClearJoinGuildRequest(member->GetID());

	InfoLogStream << "player(" << member->GetID() << ") enter guild(" << GetID() << ")." << LogStream::eos;

	auto oldGuildId = member->GetGuildID();
	member->SetGuildID(GetID());
	bool isNewObj = false;
	if (member->GetEnterTime() == 0)
	{
		// 说明是新new出来的GuildMember
		isNewObj = true;
	}
	member->SetEnterTime((UInt32)CURRENT_TIME.Sec());
	// 清除原来的历史贡献
	member->SetTotalContribution(0);
	// 添加公会商店红点
	member->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_SHOP);

	// 保存到数据库
	if (isNewObj)
	{
		member->InsertToDB();
	}
	else
	{
		member->SaveToDB();
	}
	

	// 如果玩家在线
	// 设置状态，发送通知
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(member->GetID());
	if (player)
	{
		member->SetPlayer(player);
		member->SetOfflineTime(0);
		player->SetGuildID(GetID());
		player->SetGuildLvl(GetLevel());
		SyncBaseInfo(member);
		SyncMemberInfoToScene(member);
		player->OnJoinGuild();

		// 通知公会地下城的活动
		GuildDungeonMgr::Instance()->NotifyGuildDungeonActivity(player);

		// 好友更新
		player->GetRelationMgr().OnGuildChanged();
	}
	else
	{
		member->SetOfflineTime((UInt32)CURRENT_TIME.Sec());
		//离线事件
		replyPlayer->PutOfflineEvent(member->GetID(), new JoinGuildEvent());
	}
	
	// 修改公会红包
	RedPacketMgr::Instance()->OnPlayerChangeGuild(member->GetID(), oldGuildId, GetID());

	// 发送到公会日志
	AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3022, member->GetName()));

	// 通知公会战
	OnGuildBattleEnrollMail(GetEnrollTerrID(), member);
}

void Guild::OnScenePlayerAddContri(ObjID_t roleId, UInt32 num)
{
	auto member = FindMember(roleId);
	if (!member)
	{
		return;
	}

	member->AddTotalContribution(num);
	AddFund(num);
}

void Guild::OnPlayerForeverDeleted(ObjID_t roleId)
{
	auto member = FindMember(roleId);
	if (!member)
	{
		return;
	}

	InfoLogStream << "guild(" << GetID() << ") member(" << roleId << ") is forever deleted..." << LogStream::eos;

	// 如果是会长，就换一个会长
	if (member->GetPost() == GUILD_POST_LEADER)
	{
		auto newLeader = _GenNewLeader();
		if (newLeader)
		{
			newLeader->SetPost(GUILD_POST_LEADER);
			SyncMemberInfoToScene(newLeader);
			OnChangeLeader(newLeader);

			InfoLogStream << "guild(" << GetID() << ") leader(" << roleId << ") is forever deleted, new leader(" << newLeader->GetID() << ")..." << LogStream::eos;
		}
	}

	DelMember(member);
}

void Guild::Broadcast(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		Broadcast(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void Guild::Broadcast(Avalon::Packet* packet)
{
	if (packet == NULL)
	{
		return;
	}

	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member && member->GetPlayer())
		{
			member->GetPlayer()->SendPacket(packet);
		}
	}
}

void Guild::Broadcast(Avalon::Protocol& protocol, UInt32 playerLevelLimit)
{
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member && member->GetPlayer())
		{
			if (member->GetPlayer()->GetLevel() >= playerLevelLimit)
			{
				member->GetPlayer()->SendProtocol(protocol);
			}
		}
	}
}

void Guild::Broadcast(Avalon::Protocol& protocol, const std::function<void(WSPlayer*, Avalon::Protocol*)>& callback)
{
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member && member->GetPlayer())
		{
			callback(member->GetPlayer(), &protocol);
			member->GetPlayer()->SendProtocol(protocol);
		}
	}
}

void Guild::BroadcastByPower(Avalon::Protocol& protocol, GuildPowerFlag power)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		BroadcastByPower(packet, power);
	}
	Avalon::Packet::Destroy(packet);
}

void Guild::BroadcastByPower(Avalon::Packet* packet, GuildPowerFlag power)
{
	if (packet == NULL)
	{
		return;
	}

	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member && member->GetPlayer() && HasGuildPower(member->GetPost(), power))
		{
			member->GetPlayer()->SendPacket(packet);
		}
	}
}

void Guild::SyncBaseInfo(GuildMember* member)
{
	if (!member || !member->GetPlayer())
	{
		return;
	}

	CLProtocol::WorldGuildSyncInfo sync;
	sync.info.id = GetID();
	sync.info.name = GetName();
	sync.info.level = GetLevel();
	sync.info.fund = GetFund();
	sync.info.declaration = GetDeclaration();
	sync.info.announcement = m_Announcement;
	sync.info.dismissTime = GetDismissTime();
	sync.info.memberNum = m_MemberNum;
	sync.info.leaderName = m_LeaderName;
	sync.info.winProbability = m_WinProbability;
	sync.info.loseProbability = m_LoseProbability;
	sync.info.storageAddPost = m_StorageAddPost;
	sync.info.storageDelPost = m_StorageDelPost;
	sync.info.buildings = m_BuildingMgr.GetBuildings();
	sync.info.guildBattleInfo.enrollTerrId = m_EnrollTerrId;
	sync.info.guildBattleInfo.occupyTerrId = m_OccupyTerrId;
	sync.info.guildBattleInfo.occupyCrossTerrId = m_OccupyCrossTerrId;
	sync.info.guildBattleInfo.historyTerrId = m_HistoryTerrId;
	sync.info.guildBattleInfo.guildBattleScore = m_GuildBattleScore;
	sync.info.guildBattleInfo.inspire = m_Inspire;
	GuildBattleMgr::Instance()->OnRequestBattleSelfRecord(member->GetPlayer(), sync.info.guildBattleInfo.selfGuildBattleRecord);
	GuildBattleMgr::Instance()->GetTerritroyBaseInfos(sync.info.guildBattleInfo.terrInfos);
	sync.info.guildBattleInfo.guildBattleType = (UInt8)GuildBattleMgr::Instance()->GetGuildBattleType();
	sync.info.guildBattleInfo.guildBattleStatus = (UInt8)GuildBattleMgr::Instance()->GetGuildBattleStatus();
	sync.info.guildBattleInfo.guildBattleStatusEndTime = GuildBattleMgr::Instance()->GetGuildBattleStatusEndTime(sync.info.guildBattleInfo.guildBattleStatus);
	if (!member->HasPower(GUILD_POWER_SETMEMBER) || m_Requesters.empty())
	{
		sync.info.hasRequester = 0;
	}
	else
	{
		sync.info.hasRequester = 1;
	}

	auto tableMembers = m_Table.GetMembers();
	for (auto tableMember : tableMembers)
	{
		auto member = FindMember(tableMember.id);
		if (!member)
		{
			ErrorLogStream << "guild(" << GetID() << ") can't find member(" << tableMember.id << ") but can't find him in guild." << LogStream::eos;
			m_Table.EraseMember(tableMember.id);
			continue;
		}

		sync.info.tableMembers.push_back(member->GetTableMember(tableMember.seat, tableMember.type));
	}

	sync.info.joinLevel = GetJoinLevel();
	sync.info.emblemLevel = member->GetEmblemLevel();
	sync.info.guildDungeonType = GetGuildDungeonType();
	sync.info.weekAddedFund = GetWeekAddedFund();
	sync.info.lastWeekAddedFund = GetLastWeekAddFund();

	member->GetPlayer()->SendProtocol(sync);
}

void Guild::BroadcastInfo()
{
	Avalon::Packet	*packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_GUILD_SYNC_INFO);

	Avalon::NetOutputStream	stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		if (!Encode(stream, SST_WORLD, true))
		{
			Avalon::Packet::Destroy(packet);
			return;
		}
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	packet->SetSize(stream.GetPos());
	if (packet->GetSize() == 0)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	Broadcast(packet);

	if(!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_CLEAR_DIRTY))
	{
		ClearDirty();
	}

	Avalon::Packet::Destroy(packet);
}

void Guild::Chat(const std::string& content, bool bLink)
{
	if (content.empty())
	{
		return;
	}

	CLProtocol::SceneSyncChat sync;
	sync.channel = CHAT_CHANNEL_TRIBE;
	sync.word = content;
	sync.bLink = bLink;
	sync.objid = 0;
	sync.objname = "";
	sync.isGm = 0;
	sync.voiceKey = " ";

	Broadcast(sync);
}

void Guild::SyncMemberInfoToScene(GuildMember* member, bool leaveGuild)
{
	if (!member || !member->GetPlayer())
	{
		return;
	}

	CLProtocol::WorldGuildSyncToScene sync;
	sync.id = member->GetID();
	sync.guildId = GetID();
	sync.guildLvl = GetLevel();
	sync.name = GetName();
	sync.post = member->GetPost();
	sync.enterTime = member->GetEnterTime();
	sync.shopId = leaveGuild == true ? 0 : m_ShopId;
	for (UInt32 i = 0; i < GUILD_BUILDING_NUM; i++)
	{
		GuildBuilding building;
		building.type = (UInt8)i;
		building.level = m_BuildingMgr.GetLevel((GuildBuildingType)i);

		sync.buildings.push_back(building);
	}
	sync.guildBattleNumber = member->GetGuildBattleNumber();
	sync.guildBattleScore = member->GetGuildBattleScore();
	sync.guildBattleRewardMask = member->GetRewardMask();
	sync.guildBattleLotteryStatus = member->GetGuildBattleLotteryStatus();
	if (leaveGuild)
	{
		sync.occupyCrossTerrId = 0;
		sync.occupyTerrId = 0;
	}
	else
	{
		sync.occupyCrossTerrId = GetOccupyCrossTerrID();
		sync.occupyTerrId = GetOccupyTerrID();
	}

	member->GetPlayer()->SendToScene(sync);
}

void Guild::SyncEmblemToScene(GuildMember* member)
{
	CLProtocol::WorldGuildEmblemSync sync;
	sync.roleId = member->GetID();
	sync.emblemLevel = member->GetEmblemLevel();
	member->GetPlayer()->SendToScene(sync);
}

void Guild::SyncCrossOccupyToScene()
{
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		SyncMemberInfoToScene(iter->second, false);
		++iter;
	}
}

void Guild::SyncOccupyToScene()
{
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		SyncMemberInfoToScene(iter->second, false);
		++iter;
	}
}

void Guild::SaveToDB()
{
	// 保存自身信息
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild", GetID(), false);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}

	// 保存成员信息
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member)
		{
			member->SaveToDB();
		}
	}
}

void Guild::InsertToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);

	ClearDirty();
}

void Guild::DelFromDB()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}

UInt32 Guild::_GetMaxNumByPost(GuildPost post)
{
	if (post == GUILD_POST_LEADER)
	{
		return 1;
	}
	else if (post == GUILD_POST_ASSISTANT)
	{
		return 2;
	}
	else if (post == GUILD_POST_ELDER)
	{
		return 4;
	}
	else
	{
		return -1;
	}
}

UInt32 Guild::_GetMemberNumByPost(GuildPost post)
{
	UInt32 num = 0;
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member && member->GetPost() == post)
		{
			num++;
		}
	}

	return num;
}

bool Guild::_CanSetPost(GuildPost post, GuildPost targetPost)
{
	if (!HasGuildPower(post, GUILD_POWER_SETPOST))
	{
		return false;
	}

	// 不能任命精英
	if (post == GUILD_POST_ELITE)
	{
		return false;
	}

	if (post <= targetPost && post != GUILD_POST_LEADER)
	{
		return false;
	}

	if (targetPost <= GUILD_MEMBER_BEGIN || (targetPost >= GUILD_MEMBER_END && targetPost <= GUILD_MANAGER_BEGIN) || targetPost >= GUILD_MANAGER_END)
	{
		return false;
	}

	return true;
}

UInt32 Guild::_GetDonateCost(GuildDonateType type)
{
	if (type == GUILD_DONATE_GOLD)
	{
		return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DONATE_GOLD_COST);
	}

	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DONATE_POINT_COST);
}

UInt8 Guild::_GetDonateMaxCount(WSPlayer* player, GuildDonateType type)
{
	if (!player)
	{
		return 0;
	}

	UInt32 num = 0;
	if (type == GUILD_DONATE_GOLD)
	{
		num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DONATE_GOLD_NUM);
		//player->GetVipValue(VIP_PRIVILEGE_GUILD_GOLDDONATE_TIMES, num);
	}
	else
	{
		player->GetVipValue(VIP_PRIVILEGE_GUILD_DAILY_DONATE_NUM, num);
	}
	
	return num;
}

const char* Guild::_GetDonateCounter(GuildDonateType type)
{
	if (type == GUILD_DONATE_POINT)
	{
		return COUNTER_GUILD_DONATE_POINT;
	}
	return COUNTER_GUILD_DONATE_GOLD;
}

UInt32 Guild::_GetDonateContribution(GuildDonateType type, UInt8 num)
{
	UInt32 contri = 0;
	if (type == GUILD_DONATE_GOLD)
	{
		contri = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DONATE_GOLD_CONTRI);
	}
	else
	{
		contri = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DONATE_POINT_CONTRI);
	}
	return num * contri;
}

UInt8 Guild::_GetShopIDByLevel()
{
	auto shopLevel = m_BuildingMgr.GetLevel(GUILD_BUILDING_SHOP);
	auto buildingData = GuildBuildingDataMgr::Instance()->FindEntry(shopLevel);
	if (!buildingData)
	{
		return 0;
	}

	return buildingData->shopId;
}

void Guild::_BroadcastMail(const std::string& title, const std::string& sender, const std::string& content, const std::string& reason)
{
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (!member)
		{
			continue;
		}

		std::vector<ItemReward> reward;
		MailBox::SendSysMail(member->GetID(), sender, title, content, reason, reward);
	}
}

UInt32 Guild::_GetOrzMaxTime()
{
	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_WORSHIP_TIME);
}

void Guild::MemberAddContri(GuildMember* member, UInt32 contri, std::string reason)
{
	if (!member)
	{
		return;
	}

	member->AddTotalContribution(contri);
	AddFund(contri);

	InfoLogStream << "guild(" << GetID() << ") player(" << member->GetID() << ") add contribution(" << contri << ") current guild fund(" << GetFund() << ")." << LogStream::eos;

	auto player = member->GetPlayer();
	if (!player)
	{
		ErrorLogStream << "guild(" << GetID() << ") member(" << member->GetID() << ") offline." << LogStream::eos;
		return;
	}

	// 通知scene添加公会贡献
	CLProtocol::WorldGuildSyncContribution sync;
	sync.id = member->GetID();
	sync.isAdd = 1;
	sync.value = contri;
	sync.reason = reason;
	player->SendToScene(sync);
}

void Guild::OnGuildOccupyTerritory(UInt8 terrId)
{
	HashMap<ObjID_t, GuildMember*>::iterator iter = m_Members.begin();
	while (iter != m_Members.end())
	{
		if (iter->second != NULL && iter->second->GetPlayer() != NULL)
		{
			iter->second->GetPlayer()->OnGuildOccupyTerritory(terrId);
		}
		++iter;
	}
}

void Guild::OnGuildBattleEnrollMail(UInt8 terrId, GuildMember* member)
{
	GuildTerritoryDataEntry* terrDataEntry = GuildTerritoryDataEntryMgr::Instance()->FindEntry(terrId);
	if (NULL == terrDataEntry)
	{
		ErrorLogStream << "not find guild terr config id:" << terrId << LogStream::eos;
		return;
	}

	

	if ( !GuildBattleMgr::Instance()->IsNotifyGuildEnroll(this))
	{
		return;
	}

	UInt32 time = GuildBattleMgr::Instance()->GetGuildBattleStatusEndTime(GBS_PREPARE);
	if (time == 0)
	{
		ErrorLogStream << "guild battle status time error!" << LogStream::eos;
		return;
	}

	Avalon::Date date(Avalon::Time((UInt64)time * SEC_TO_MSEC));

	std::string title = SysNotifyMgr::Instance()->MakeString(12019);
	std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
	std::string content = SysNotifyMgr::Instance()->MakeString(12020, terrDataEntry->name, 
		date.Year(), date.Month(), date.Day(), date.Hour(),date.Min(), date.Sec());
	std::string reason = GetReason(SOURCE_TYPE_GUILD_BATTLE_ENROLL);
	if (member == NULL)
	{
		_BroadcastMail(title, sender, content, reason);
	}
	else
	{
		std::vector<ItemReward> reward;
		MailBox::SendSysMail(member->GetID(), sender, title, content, reason, reward);
	}
}

void Guild::OnGuildBattleScoreRankReward(UInt32 titleId, UInt32 expireTime)
{
	for (auto& iter : m_Members)
	{
		GuildMember* member = iter.second;
		if (NULL == member)
			continue;

		WSPlayer* player = member->GetPlayer();
		if (player != NULL)
		{
			player->GetTitleMgr().AddOneTitleByTitleId(titleId, expireTime);
		}
		else
		{
			OfflineEvent* event =  new TitleEvent(titleId, expireTime);
			GlobalOfflineEventMgr::Instance()->AddOfflineEvent(member->GetID(), event);
			event->InsertToDB(member->GetID());
		}
	}
}

std::string Guild::_GetTableCounterName(UInt8 help)
{
	if (help)
	{
		return COUNTER_GUILD_TABLE_HELP;
	}
	return COUNTER_GUILD_TABLE;
}

UInt32 Guild::_GetTableMaxTime(UInt8 help)
{
	auto data = GuildTableDataEntryMgr::Instance()->FindEntry(help);
	if (!data)
	{
		return 0;
	}

	return data->maxTimes;
}

GuildMember* Guild::_GenNewLeader()
{
	GuildMember* richMember = NULL;
	for (auto itr : m_Members)
	{
		auto member = itr.second;
		if (member->GetPost() == GUILD_POST_ASSISTANT)
		{
			return member;
		}
		else if (member->GetPost() == GUILD_POST_LEADER)
		{
			continue;
		}
		else if (richMember == NULL || member->GetTotalContribution() > richMember->GetTotalContribution())
		{
			richMember = member;
		}
	}

	return richMember;
}

void Guild::UpdateMemberNum()
{
	m_MemberNum = (UInt16)m_Members.size();
}

void Guild::UpdateGuildLevelSortList()
{
	UInt32 buildPoint = 0;
	for (UInt32 k = 0; k < GUILD_BUILDING_NUM; ++k)
	{
		UInt32 buildLvl = m_BuildingMgr.GetLevel(GuildBuildingType(k));
		buildPoint += GuildBuildingPointDataEntryMgr::Instance()->GetBuildTotalPointByType(k, buildLvl);
	}

	SortListEntry entry;
	entry.sortType = SortListType::SORTLIST_GUILD_LEVEL;
	entry.id = GetID();
	entry.level = GetLevel();
	entry.name = GetName();
	if (m_Leader != NULL)
	{
		entry.ownerId = m_Leader->GetID();
	}
	entry.ownerName = m_LeaderName;
	entry.value1 = GetLevel();
	entry.value2 = GetLastUpdateGuildBuildingTime();//GetSetUpTime();
	entry.value3 = buildPoint;
	entry.guildMemberNum = GetMemberNum();
	WSSortListMgr::Instance()->UpdateSortList(entry);

	DebugLogStream << "guild(" << GetID() << ", " << GetName() << ") guild sort value(" << GetLevel() << ", " << GetSetUpTime() << ", " << buildPoint << ") member(" << entry.guildMemberNum << ")." << LogStream::eos;
}

void Guild::HandleEmblemUpgrade(WSPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::WorldGuildEmblemUpRes res;
	do
	{
		GuildMember* member = FindMember(player->GetID());
		if (NULL == member)
		{
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			break;
		}

		UInt32 roleNeedLvl = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EMBLEM_ROLE_LEVEL);
		UInt32 guildNeedLvl = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EMBLEM_GUILD_LEVEL);
		UInt32 honourNeedLvl = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EMBLEM_HONOUR_LEVEL);

		// 激活徽记条件
		if (member->GetEmblemLevel() == 0)
		{
			if (player->GetLevel() < roleNeedLvl)
			{
				ErrorLogStream << "emblem activation failed playerId:" << player->GetID() << " guild(" << GetID() << ") current player level(" << player->GetLevel() << ") need (" << roleNeedLvl << ")." << LogStream::eos;
				res.result = ErrorCode::GUILD_EMBLEM_ACTIVATE_ERROR;
				break;
			}

			if (m_Level < guildNeedLvl)
			{
				ErrorLogStream << "emblem activation failed playerId:" << player->GetID() << " guild(" << GetID() << ") current guild level(" << m_Level << ") need (" << guildNeedLvl << ")." << LogStream::eos;
				res.result = ErrorCode::GUILD_EMBLEM_ACTIVATE_ERROR;
				break;
			}

			if (GetBuildingLevel(GUILD_BUILDING_HONOUR) < honourNeedLvl)
			{
				ErrorLogStream << "emblem activation failed playerId:" << player->GetID() << " guild(" << GetID() << ") current building level(" << GetBuildingLevel(GUILD_BUILDING_HONOUR) << ") need (" << honourNeedLvl << ")." << LogStream::eos;
				res.result = ErrorCode::GUILD_EMBLEM_ACTIVATE_ERROR;
				break;
			}
		}

		UInt32 nextLvl = member->GetEmblemLevel() + 1;
		GuildEmblemDataEntry* entry = GuildEmblemDataEntryMgr::Instance()->FindEntry(nextLvl);
		if (entry == NULL)
		{
			res.result = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		// 需要的荣耀殿堂等级
		if (GetBuildingLevel(GUILD_BUILDING_HONOUR) < entry->needHonourLevel)
		{
			ErrorLogStream << "emblem activation failed playerId:" << player->GetID() << " guild(" << GetID() << ") current building level(" << GetBuildingLevel(GUILD_BUILDING_HONOUR) << ") need (" << entry->needHonourLevel << ")." << LogStream::eos;
			//ErrorLogStream << "honour level not enough failed playerId:" << player->GetID() << LogStream::eos;
			res.result = ErrorCode::GUILD_EMBLEM_HONOUR_LEVEL_NOT_ENOUGH;
			break;
		}

		// 检查材料，事务
		auto trans = new GuildEmblemTransaction(player, member->GetEmblemLevel());
		for (UInt32 k = 0; k < entry->costId.size(); ++k)
		{
			UInt32 useEqual = entry->useEqual.size() > k ? entry->useEqual[k] : 0;
			trans->SetItem(entry->costId[k], entry->costNum[k], useEqual);
		}

		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << "tran failed playerId:" << player->GetID() << LogStream::eos;
			res.result = ErrorCode::GUILD_SYS_ERROR;
			break;
		}

		// 此处返回客户端消息，通过事务返回消息
		return;

	} while (0);

	player->SendProtocol(res);
}

void Guild::EmblemUpgradeTransaction(WSPlayer* player, UInt32 emblemLvl, GuildEmblemTransaction* tran)
{
	if (NULL == player || NULL == tran) return;
	
	CLProtocol::WorldGuildEmblemUpRes res;
	GuildMember* member = FindMember(player->GetID());
	if (NULL == member)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		res.emblemLevel = emblemLvl;
		player->SendProtocol(res);
		return;
	}

	UInt32 nextLvl = emblemLvl + 1;
	
	InfoLogStream << PLAYERINFO(player) << " Upgrade Guild Emblem Success! nextLvl:" << nextLvl << LogStream::eos;

	// 扣除材料
	tran->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_EMBLEM_UGRADE, player->GetGuildID(), nextLvl));

	// 升级
	member->SetEmblemLevel(nextLvl);
	member->SaveToDB();

	res.emblemLevel = nextLvl;
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	// 同步徽记到Scene
	SyncEmblemToScene(member);

	GuildEmblemDataEntry* entry = GuildEmblemDataEntryMgr::Instance()->FindEntry(nextLvl);
	if (entry != NULL)
	{
		// 公会日志
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(3063, player->GetName(), entry->name, entry->stageLevel));

		// 删除旧的头衔
		GuildEmblemDataEntry* oldEntry = GuildEmblemDataEntryMgr::Instance()->FindEntry(emblemLvl);
		if (oldEntry != NULL)
		{
			player->GetTitleMgr().RemoveOneTitleByDataId(oldEntry->titleId);
		}

		// 发放新头衔
		player->GetTitleMgr().AddOneTitleByTitleId(entry->titleId);
	}

	// 学习公会徽记的技能
	UInt32 skill_pvp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EMBLEM_SKILL_PVP);
	if (skill_pvp != 0)
	{
		player->SetGuildSkillLevel(skill_pvp, nextLvl, SKILL_CONFIG_PVP);
	}

	UInt32 skill_pve = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EMBLEM_SKILL_PVE);
	if (skill_pve != 0)
	{
		player->SetGuildSkillLevel(skill_pve, nextLvl, SKILL_CONFIG_PVE);
	}
}

void Guild::HandleGuildDungeonType(WSPlayer* player, UInt32 dType)
{
	CLProtocol::WorldGuildSetDungeonTypeRes res;

	do
	{
		auto member = FindMember(player->GetID());
		if (!member)
		{
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			break;
		}

		if (!HasGuildPower(member->GetPost(), GUILD_SET_DUNGEON_BOSS_TYPE))
		{
			res.result = ErrorCode::GUILD_NO_POWER;
			break;
		}

		if (GuildDungeonMgr::Instance()->IsGuildDungeon())
		{
			res.result = ErrorCode::GUILD_DUNGEON_NOT_SET_TYPE;
			break;
		}

		UInt32 dungeonType = GuildDungeonTypeDataEntryMgr::Instance()->GetGuildDungeonType(m_BuildingMgr.GetLevel(GUILD_BUILDING_ALTAR));
		if (dType > dungeonType)
		{
			res.result = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		res.dungeonType = dType;
		res.result = ErrorCode::SUCCESS;
		std::string typeName = GuildDungeonTypeDataEntryMgr::Instance()->GetGuildDungeonTypeName(dType);
		AddGuildEvent(SysNotifyMgr::Instance()->MakeString(14015,member->GetName(),typeName));
		SetGuildDungeonType(dType);

	} while (0);

	player->SendProtocol(res);
}

UInt32 Guild::HandleGuildTerrDayReward(WSPlayer* player)
{
	auto member = FindMember(player->GetID());
	if (!member)
		return ErrorCode::GUILD_NOT_IN_GUILD;

	if (GetOccupyTerrID() == 0)
		return ErrorCode::GUILD_BATTLE_NOT_OCCUPY_TERRITORY;

	if (member->GetGuildBattleTerrDayReward() == 0)
		return ErrorCode::GUILD_BATTLE_GIVE_REWARD_ALREADY;

	GuildTerritoryDataEntry* terrDataEntry = GuildTerritoryDataEntryMgr::Instance()->FindEntry(GetOccupyTerrID());
	if (NULL == terrDataEntry)
	{
		ErrorLogStream << "not find guild terr config id:" << GetOccupyTerrID() << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(SOURCE_TYPE_GUILD_TERR_DAY_REWARD, GetOccupyTerrID());

	if (member->GetGuildBattleTerrDayReward() == GUILD_POST_LEADER)
	{
		player->AddItem(reason, terrDataEntry->leaderDayRewards);
	}
	else
	{
		player->AddItem(reason, terrDataEntry->memberDayRewards);
	}

	InfoLogStream << PLAYERINFO(player) << "get terr day reward terrid:" << GetOccupyTerrID() << LogStream::eos;

	member->SetGuildBattleTerrDayReward(0);
	member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_TERR_ID);

	return ErrorCode::SUCCESS;
}

void Guild::ClearMemberTerrDayReward(bool isClear)
{
	for (auto& iter : m_Members)
	{
		if (isClear)
		{
			iter.second->SetGuildBattleTerrDayReward(0);
			iter.second->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_TERR_ID);
		}
		else
		{
			iter.second->SyncEventToScene(SET_ADD_REDPOINT, RED_POINT_GUILD_TERR_ID);
			if (iter.second->GetPost() == GUILD_POST_LEADER)
			{
				iter.second->SetGuildBattleTerrDayReward(GUILD_POST_LEADER);
			}
			else
			{
				iter.second->SetGuildBattleTerrDayReward(GUILD_POST_NORMAL);
			}
		}
	}
}

void Guild::UpdateGuildBattleWeekScoreRank(UInt32 addScore)
{
	SortListEntry entry;
	entry.sortType = SortListType::SORTLIST_GUILD_BATTLE_WEEK_SCORE;
	entry.id = GetID();
	entry.level = GetLevel();
	entry.name = GetName();
	entry.value2 = (UInt32)CURRENT_TIME.Sec();

	if (GetOccupyTerrID() != 0 && addScore == 0)
	{
		GuildTerritoryDataEntry* terrData = GuildTerritoryDataEntryMgr::Instance()->FindEntry(GetOccupyTerrID());
		if (terrData != NULL)
			entry.value1 = terrData->occupyScore;
	}
	else
	{
		GuildTerritoryDataEntry* terrData = GuildTerritoryDataEntryMgr::Instance()->FindEntry(GetEnrollTerrID());
		if (terrData != NULL)
			entry.value1 = addScore * terrData->battleScoreFactor / 100;
	}

	auto oldEntry = WSSortListMgr::Instance()->FindEntry(SortListType::SORTLIST_GUILD_BATTLE_WEEK_SCORE, GetID());
	if (oldEntry)
	{
		entry.value1 += oldEntry->value1;
	}

	WSSortListMgr::Instance()->UpdateSortList(entry);

	InfoLogStream << "guild battle score rank id:" << entry.id << " score:" << entry.value1 << LogStream::eos;
}

GuildSkillDataEntry* Guild::FindSkillEntry(UInt16 skillId, UInt8 level)
{
	return GuildSkillDataEntryMgr::Instance()->FindEntry(GuildSkillDataEntry::MakeKey(skillId, level));
}

void Guild::CleanSelfMergerRequest()
{
	m_MergerRequsetStatus = GUILD_MERGER_REQUEST_NULL;
	m_MergerGoalGuildID = 0;
	m_MergerRequestTime = 0;
	m_MergerRequestAccpetTime = 0;
}

void Guild::AddMergerRequestGuild(Guild * guild)
{
	if (guild == nullptr)
	{
		return;
	}
	if (m_HavedMergerRequest.find(guild->GetID()) != m_HavedMergerRequest.cend())
	{
		return;
	}
	if (guild != nullptr)
	{
		m_HavedMergerRequest.insert(guild->GetID());
	}
}

void Guild::RemoveMergerRequestGuild(Guild * guild)
{
	if (guild == nullptr)
	{
		return;
	}
	m_HavedMergerRequest.erase(guild->GetID());
}

UInt32 Guild::RequestMerger(Guild * guild)
{
	if (guild == nullptr)
	{
		return ErrorCode::GUILD_MERGER_INVAILD;
	}

	if (m_MergerRequsetStatus != GUILD_MERGER_REQUEST_NULL)
	{
		return ErrorCode::GUILD_MERGER_REQUEST_EXISTS;
	}

	m_MergerRequsetStatus = GUILD_MERGER_REQUEST_SEND;
	m_MergerGoalGuildID = guild->GetID();
	m_MergerRequestTime = CURRENT_TIME.Sec();
	m_MergerRequestAccpetTime = 0;
	guild->AddMergerRequestGuild(this);
	SendMergerUDPLog(GUILD_MERGER_RESULT_SEND);
	return ErrorCode::SUCCESS;
}

UInt32 Guild::CancelMerger(Guild * guild)
{
	if (guild == nullptr)
	{
		return ErrorCode::GUILD_MERGER_INVAILD;
	}
	if (m_MergerRequsetStatus != GUILD_MERGER_REQUEST_SEND && m_MergerRequsetStatus != GUILD_MERGER_REQUEST_ACCEPTED)
	{
		return ErrorCode::GUILD_MERGER_REQUEST_NOT_EXISTS;
	}

	if (m_MergerRequsetStatus == GUILD_MERGER_REQUEST_ACCEPTED)
	{
		// 发送邮件
		std::string title = SysNotifyMgr::Instance()->MakeString(12000);
		std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
		std::string content = SysNotifyMgr::Instance()->MakeString(12006, GetName(), GetLeader()->GetName());
		std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
		guild->_BroadcastMail(title, sender, content, reason);
		//guild->AddGuildEvent(content);
	}

	CleanSelfMergerRequest();
	guild->RemoveMergerRequestGuild(this);
	SendMergerUDPLog(GUILD_MERGER_RESULT_CANCEL);
	return ErrorCode::SUCCESS;
}

std::set<ObjID_t>& Guild::GetMergerRequestedGuildSet()
{
	return m_HavedMergerRequest;
}

UInt32 Guild::AcceptMergerRequest(Guild * guild)
{
	if (guild == nullptr)
	{
		return ErrorCode::GUILD_MERGER_INVAILD;
	}
	if (guild->m_MergerRequsetStatus != GUILD_MERGER_REQUEST_SEND)
	{
		ErrorLogStream << "guild->m_MergerRequsetStatus error " << guild->GetID() << " MergerRequsetStatus is  (" << guild->m_MergerRequsetStatus << ")!" << LogStream::eos;
		return ErrorCode::GUILD_MERGER_INVAILD;
	}

	if (m_HavedMergerRequest.find(guild->GetID()) == m_HavedMergerRequest.cend())
	{
		return ErrorCode::GUILD_MERGER_REQUEST_NOT_EXISTS;
	}

	for (auto &i : m_HavedMergerRequest)
	{
		auto tmpGuild = GuildMgr::Instance()->FindGuildByID(i);
		if (tmpGuild != nullptr)
		{
			if (tmpGuild->GetMergerRequestStatus() == GUILD_MERGER_REQUEST_ACCEPTED)
			{
				return ErrorCode::GUILD_MERGER_REQUEST_ONE;
			}
		}
	}

	// 发送邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(12000);
	std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
	std::string content = SysNotifyMgr::Instance()->MakeString(12002,guild->GetName());
	std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
	_BroadcastMail(title, sender, content, reason);
	//AddGuildEvent(content);

	content = SysNotifyMgr::Instance()->MakeString(12003, GetName());
	guild->_BroadcastMail(title, sender, content, reason);

	guild->m_MergerRequsetStatus = GUILD_MERGER_REQUEST_ACCEPTED;
	guild->m_MergerRequestAccpetTime = CURRENT_TIME.Sec();
	SendMergerUDPLog(GUILD_MERGER_RESULT_ACCEPT);

	//guild->AddGuildEvent(content);
	return ErrorCode::SUCCESS;
}

UInt32 Guild::RefuseMergerRequest(Guild * guild)
{
	if (guild == nullptr)
	{
		return ErrorCode::GUILD_MERGER_INVAILD;
	}
	if (guild->m_MergerRequsetStatus != GUILD_MERGER_REQUEST_SEND)
	{
		ErrorLogStream << "guild->m_MergerRequsetStatus error " << guild->GetID() << " MergerRequsetStatus is  (" << guild->m_MergerRequsetStatus << ")!" << LogStream::eos;
		return ErrorCode::GUILD_MERGER_INVAILD;
	}

	if (m_HavedMergerRequest.find(guild->GetID()) == m_HavedMergerRequest.cend())
	{
		return ErrorCode::GUILD_MERGER_REQUEST_NOT_EXISTS;
	}


	{
		// 发送邮件
		std::string title = SysNotifyMgr::Instance()->MakeString(12000);
		std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
		std::string content = SysNotifyMgr::Instance()->MakeString(12012,GetName());
		std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
		std::vector<ItemReward> reward;
		MailBox::SendSysMail(guild->GetLeader()->GetID(), sender, title, content, reason, reward);
	}
	
	guild->CleanSelfMergerRequest();
	RemoveMergerRequestGuild(guild);
	SendMergerUDPLog(GUILD_MERGER_RESULT_REFUSE);
	return ErrorCode::SUCCESS;
}

UInt32 Guild::CancelMergerRequest(Guild * guild)
{
	if (guild == nullptr)
	{
		return ErrorCode::GUILD_MERGER_INVAILD;
	}
	if (guild->m_MergerRequsetStatus != GUILD_MERGER_REQUEST_ACCEPTED)
	{
		ErrorLogStream << "guild->m_MergerRequsetStatus error " << guild->GetID() << " MergerRequsetStatus is  (" << guild->m_MergerRequsetStatus << ")!" << LogStream::eos;
		return ErrorCode::GUILD_MERGER_INVAILD;
	}

	if (m_HavedMergerRequest.find(guild->GetID()) == m_HavedMergerRequest.cend())
	{
		return ErrorCode::GUILD_MERGER_REQUEST_NOT_EXISTS;
	}


	// 发送邮件
	std::string title = SysNotifyMgr::Instance()->MakeString(12000);
	std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
	std::string content = SysNotifyMgr::Instance()->MakeString(12004, guild->GetName());
	std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
	_BroadcastMail(title, sender, content, reason);
	//AddGuildEvent(content);

	content = SysNotifyMgr::Instance()->MakeString(12005, GetName(),GetLeader()->GetName());
	guild->_BroadcastMail(title, sender, content, reason);
	//guild->AddGuildEvent(content);

	guild->CleanSelfMergerRequest();
	RemoveMergerRequestGuild(guild);
	return ErrorCode::SUCCESS;
}

UInt32 Guild::CleanAllMergerRequest(bool isSendMail)
{
	Guild * acceptedGuild = nullptr;
	for (auto &i : m_HavedMergerRequest)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(i);
		if (guild == nullptr)
		{
			ErrorLogStream << "CleanAllMergerRequest error!" << LogStream::eos;
			continue;
		}
		if (guild->m_MergerRequsetStatus == GUILD_MERGER_REQUEST_ACCEPTED)
		{
			acceptedGuild = guild;
			continue;
		}
		guild->CleanSelfMergerRequest();
		if (isSendMail)
		{
			// 发送邮件
			std::string title = SysNotifyMgr::Instance()->MakeString(12000);
			std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
			std::string content = SysNotifyMgr::Instance()->MakeString(12012, GetName());
			std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
			std::vector<ItemReward> reward;
			MailBox::SendSysMail(guild->GetLeader()->GetID(), sender, title, content, reason, reward);
		}
	}
	m_HavedMergerRequest.clear();
	AddMergerRequestGuild(acceptedGuild);
	return ErrorCode::SUCCESS;
}


void Guild::OnMerger(Guild *byGuild)
{
	InfoLogStream << "guild(" << GetID() << ") by merger " << byGuild->GetID() << LogStream::eos;

	{
		// 发送邮件
		std::string title = SysNotifyMgr::Instance()->MakeString(12000);
		std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
		std::string content = SysNotifyMgr::Instance()->MakeString(12007, byGuild->GetName());
		std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
		_BroadcastMail(title, sender, content, reason);
	//	AddGuildEvent(content);
	}

	m_HavedMergerRequest.clear();
	CleanSelfMergerRequest();
	SetProsperity(GUILD_PROSPERITY_HIGH);
	m_MergerOtherGuildTime = CURRENT_TIME.Sec();


	std::vector<GuildMember*> vecMermber;
	for (auto itr : byGuild->m_Members)
	{
		vecMermber.push_back(itr.second);
	}
	std::sort(vecMermber.begin(), vecMermber.end(), [](GuildMember *a, GuildMember *b)->bool{return a->GetOfflineTime() > b->GetOfflineTime(); });

	//处理被合并所有公会成员
	for (auto member : vecMermber)
	{
		if (!member)
		{
			continue;
		}

		if (!IsGuildFull())
		{
			member->SetGuildID(GetID());
			auto oldPost = member->GetPost();
			member->SetPost(GUILD_POST_NORMAL, false);
			AddMember(member);

			auto player = WSPlayerMgr::Instance()->FindPlayer(member->GetID());
			if (player)
			{
				player->SetGuildID(GetID());
				player->SetGuildLvl(GetLevel());
				player->GetRelationMgr().OnGuildChanged();
				player->GetTitleMgr().OnPlayerGuildPostChange(oldPost, member->GetPost());
				//player->GetTitleMgr().OnGuidNameChange();
			}

			SyncMemberInfoToScene(member);
			SyncBaseInfo(member);

			// 修改公会红包
			RedPacketMgr::Instance()->OnPlayerChangeGuild(member->GetID(), byGuild->GetID(), GetID());

			{
				// 发送邮件
				std::string title = SysNotifyMgr::Instance()->MakeString(12000);
				std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
				std::string content = SysNotifyMgr::Instance()->MakeString(12008, GetName());
				std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
				std::vector<ItemReward> reward;
				MailBox::SendSysMail(member->GetID(), sender, title, content, reason, reward);
			}
		}
		else
		{
			member->SetGuildID(0);
			member->SetPost(GUILD_POST_INVALID);

			auto player = WSPlayerMgr::Instance()->FindPlayer(member->GetID());
			if (player)
			{
				player->SetGuildID(0);
				player->SetGuildLvl(0);
				player->GetRelationMgr().OnGuildChanged();
				player->GetTitleMgr().OnGuidLeave();
			}

			SyncMemberInfoToScene(member);
			SyncBaseInfo(member);

			// 修改公会红包
			RedPacketMgr::Instance()->OnPlayerChangeGuild(member->GetID(), byGuild->GetID(), 0);

			{
				// 发送邮件
				std::string title = SysNotifyMgr::Instance()->MakeString(12000);
				std::string sender = SysNotifyMgr::Instance()->MakeString(12001);
				std::string content = SysNotifyMgr::Instance()->MakeString(12009, GetName());
				std::string reason = GetReason(SOURCE_TYPE_GUILD_DISMISS);
				std::vector<ItemReward> reward;
				MailBox::SendSysMail(member->GetID(), sender, title, content, reason, reward);
			}
		}
	}

	//SendNotify(3011, GetName().c_str());

	byGuild->m_Members.clear();
	byGuild->SetProsperity(GUILD_PROSPERITY_HIGH);

	// 删除公会副本信息
	GuildDungeonMgr::Instance()->GuildDismiss(byGuild->GetID());
	// 删除工会领地信息
	GuildBattleMgr::Instance()->GuildDismiss(byGuild->GetID());
}

void Guild::SyncMergerInfo()
{
	CLProtocol::WorldGuildSyncMergerInfo msg;
	msg.prosperityStatus = m_ProsperityStatus;
	msg.mergerRequsetStatus = m_MergerRequsetStatus;
	auto player = this->GetLeader();
	if (player != nullptr)
	{
		player->SendProtocol(msg);
	}
}

void Guild::SendMergerUDPLog(UInt32 result)
{
	UInt32 mermberLoginNum = 0;
	for (auto &i : m_Members)
	{
		if (Avalon::Date{ i.second->GetOfflineTime() }.Day() == CURRENT_DATE.Day()
			&& Avalon::Date{ i.second->GetOfflineTime() }.Month() == CURRENT_DATE.Month()
			&& Avalon::Date{ i.second->GetOfflineTime() }.Year() == CURRENT_DATE.Year())
		{
			++mermberLoginNum;
		}
	}
	Sys::SendUdpLog("guild_merger", LOG_GUILD_MERGER, GAME_VERSION.c_str(), ZONE_ID, (UInt32)CURRENT_TIME.Sec(), PLATFORM.c_str(), GetID(), GetName().c_str(), GetLevel(), m_MemberNum, mermberLoginNum, m_LastWeekAddedFund,result);
}

bool Guild::CanMerger()
{
	if (CURRENT_TIME.Sec() > m_MergerOtherGuildTime + Avalon::Time::SECS_OF_DAY * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_MERGER_SPACE_DAY))
	{
		return true;
	}
	return false;
}

bool Guild::ByAcceptedMerger()
{
	for (auto &i : m_HavedMergerRequest)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(i);
		if (guild == nullptr)
		{
			continue;
		}
		if (guild->m_MergerGoalGuildID == GetID() && guild->m_MergerRequsetStatus == GUILD_MERGER_REQUEST_ACCEPTED)
		{
			return true;
		}
	}
	return false;
}

bool GuildComparer::operator()(Guild* a, Guild* b)
{
	if (a == nullptr || b == nullptr)
	{
		return false;
	}

	if (a->GetLastWeekAddFund() > b->GetLastWeekAddFund())
	{
		return true;
	}
	else if (a->GetLastWeekAddFund() == b->GetLastWeekAddFund())
	{
		if (a->GetLevel() > b->GetLevel())
		{
			return true;
		}
		else if (a->GetLevel() == b->GetLevel())
		{
			if (a->GetID() < b->GetID())
			{
				return true;
			}
		}
	}
	return false;
}
