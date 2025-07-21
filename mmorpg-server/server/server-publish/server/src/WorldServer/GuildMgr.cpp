#include "GuildMgr.h"
#include <CLGuildProtocol.h>
#include <CLFunctionDataEntry.h>
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include <CLGuildBattleScoreRankRewardDataEntry.h>
#include <CLForbitWord.h>

#include "WSPlayer.h"
#include "WSItemTransaction.h"
#include "WSSelectInitDataCallback.h"
#include "GuildBattleMgr.h"
#include "WSSortListMgr.h"
#include "GuildDungeonMgr.h"
#include "GuildAuctionMgr.h"
#include "WSServiceSwitchMgr.h"


/**
*@brief 创建公会日志
*/
class CreateGuildTransaction : public ItemTransaction
{
public:
	CreateGuildTransaction(WSPlayer *player, std::string name, std::string declaration)
		: ItemTransaction(player), m_Name(name), m_Declaration(declaration) {}
	~CreateGuildTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		if (GuildMgr::Instance()->OnCheckCreateGuildRet(player, m_Name, m_Declaration))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GUILD_CREATE));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldGuildCreateRes res;
		res.result = ErrorCode::GUILD_CREATE_NOT_ENOUGH_MONEY;
		player->SendProtocol(res);

		UnLock();
	}

private:
	std::string			m_Name;
	std::string			m_Declaration;
};

/**
*@brief 加载公会回调
*/
class SelectGuildCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildMgr::Instance()->OnSelectGuildRet(this);
	}
};

/**
*@brief 加载公会成员返回
*/
class SelectGuildMemberCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		bool result = GuildMgr::Instance()->OnSelectGuildMemberRet(this);
		if (result)
		{
			GuildBattleMgr::Instance()->OnGuildLoadFinish();
			//刷一波公会榜
			GuildMgr::Instance()->InitGuildLevelSortList();
		}
		return result;
	}
};

/**
*@brief 加载公会事件回调
*/
class SelectGuildEventCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildMgr::Instance()->OnSelectGuildEventRet(this);
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "SelectGuildCallback code:" << errorcode << LogStream::eos;
	}
};

GuildMgr::GuildMgr()
{
	m_DailyTimer.SetTime(6);
	m_GuildMemberSaveTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);
	m_LoadFinish = false;
}

GuildMgr::~GuildMgr()
{

}

bool GuildMgr::Init()
{
	CLSelectCommand* guild_command = CLRecordClient::Instance()->PrepareSelectCommand("t_guild");
	CLRecordClient::Instance()->SendCommand(guild_command, new SelectGuildCallback());

	return true;
}

bool GuildMgr::OnTick(const Avalon::Time& now)
{
	if (!m_LoadFinish)
	{
		return true;
	}

	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (guild)
		{
			guild->OnTick(now);
		}
	}

	std::vector<Guild*> dissmissGuildList;
	std::vector<Guild*> dissmissGuildMergerList;
	std::vector<Guild*> guildMergerList;
	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (guild)
		{
			if (guild->GetProsperity() == GUILD_PROSPERITY_IN_DISMISS)
			{
				dissmissGuildMergerList.push_back(guild);
				continue;
			}

			if (guild->GetProsperity() == GUILD_PROSPERITY_IN_MERGER)
			{
				guildMergerList.push_back(guild);
				continue;
			}

			if (guild->GetProsperity() == GUILD_PROSPERITY_IN_BY_MERGER)
			{
				continue;
			}

			if (guild->GetDismissTime() > 0 && now.Sec() >= guild->GetDismissTime())
			{
				dissmissGuildList.push_back(guild);
			}

			if (guild->GetMemberNum() == 0)
			{
				ErrorLogStream << "guild(" << guild->GetID() << ") is empty, need to dismiss." << LogStream::eos;
				dissmissGuildList.push_back(guild);
			}
		}
	}

	for (auto guild : guildMergerList)
	{

		if (GuildDungeonMgr::Instance()->IsGuildDungeon() ||
			GuildAuctionMgr::Instance()->CheckGuildIsAuction(guild->GetID()) ||
			GuildBattleMgr::Instance()->IsValidGuildBattle())
		{
			continue;
		}
		auto byGuild = FindGuildByID(guild->GetMergerGoalGuildID());
		if (byGuild != nullptr)
		{
			guild->OnMerger(byGuild);
			byGuild->OnDismiss(GUILD_DISMISS_MERGER);
			byGuild->DelFromDB();
			WSSortListMgr::Instance()->DeleteEntry(SortListType::SORTLIST_GUILD_LEVEL, byGuild->GetID());
			_DelGuild(byGuild);
		}
		else
		{
			ErrorLogStream << guild->GetID() << "Merger guild:" << guild->GetMergerGoalGuildID() << " faild!" << LogStream::eos;
		}
	}

	for (auto guild : dissmissGuildList)
	{
		if (GuildDungeonMgr::Instance()->IsGuildDungeon() ||
			GuildAuctionMgr::Instance()->CheckGuildIsAuction(guild->GetID()) ||
			GuildBattleMgr::Instance()->IsValidGuildBattle())
		{
			guild->CancelDismissNotify();
			continue;
		}
		
		guild->OnDismiss(GUILD_DISMISS_NORMAL);
		guild->DelFromDB();
		WSSortListMgr::Instance()->DeleteEntry(SortListType::SORTLIST_GUILD_LEVEL, guild->GetID());
		_DelGuild(guild);
	}

	for (auto guild : dissmissGuildMergerList)
	{
		if (GuildDungeonMgr::Instance()->IsGuildDungeon() ||
			GuildAuctionMgr::Instance()->CheckGuildIsAuction(guild->GetID()) ||
			GuildBattleMgr::Instance()->IsValidGuildBattle())
		{
			continue;
		}

		guild->OnDismiss(GUILD_DISMISS_LOW);
		guild->DelFromDB();
		WSSortListMgr::Instance()->DeleteEntry(SortListType::SORTLIST_GUILD_LEVEL, guild->GetID());
		_DelGuild(guild);
	}

	if (m_GuildMemberSaveTimer.IsTimeout(now))
	{
		// 保存成员信息,有一部分成员不在公会中
		for (auto itr : m_GuildMembers)
		{
			auto member = itr.second;
			if (member)
			{
				member->SaveToDB();
			}
		}
	}
	
	if (m_DailyTimer.IsTimeout(now))
	{
		OnDayChanged();
	}

	return true;
}

void GuildMgr::OnDayChanged()
{
	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (!guild)
		{
			continue;
		}

		guild->OnDayChanged();
	}

	// 刷新所有成员的活跃度为0
	std::ostringstream condstream;
	auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_member", condstream.str());
	cmd->PutData("active_degree", 0);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildMgr::OnWeekChanged()
{
	// 跨周兼并
	OnWeekChangedGuildMerge();

	// 发放头衔奖励
	OnWeekChangedGuidTerrScoreRankReward();
}

void GuildMgr::OnWeekChangedGuildMerge()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_GUILD_MERGER))
	{
		return;
	}

	if (!IsMergerEnoughOpenDay())
	{
		return;
	}

	m_MidGuild.clear();
	m_LowAndDismissGuild.clear();
	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (!guild)
		{
			continue;
		}

		guild->OnWeekChanged();

		if (guild->GetProsperity() == GUILD_PROSPERITY_IN_DISMISS || guild->GetProsperity() == GUILD_PROSPERITY_IN_MERGER || guild->GetProsperity() == GUILD_PROSPERITY_IN_BY_MERGER)
		{
			continue;
		}

		if (guild->ByAcceptedMerger() || guild->GetMergerRequestStatus() == GUILD_MERGER_REQUEST_ACCEPTED)
		{
			continue;
		}

		if (guild->GetProsperity() == GUILD_PROSPERITY_MID)
		{
			m_MidGuild.insert(guild);
		}
		else if (guild->GetProsperity() == GUILD_PROSPERITY_LOW || guild->GetProsperity() == GUILD_PROSPERITY_DISMISS)
		{
			m_LowAndDismissGuild.insert(guild);
		}
	}
}

void GuildMgr::OnWeekChangedGuidTerrScoreRankReward()
{
	class GuildBattleScoreRankVisitor : public WSSortListVisitor
	{
	public:
		GuildBattleScoreRankVisitor() {}
		~GuildBattleScoreRankVisitor() {}

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			const GuildBattleScoreRankRewardDataEntry* dataEntry = GuildBattleScoreRankRewardDataEntryMgr::Instance()->
				GetScoreRankReward(ranking);

			if (NULL == dataEntry)
				return true;

			Guild* guild = GuildMgr::Instance()->FindGuildByID(entry.id);
			if (NULL == guild)
				return true;

			UInt32 expireTime = (UInt32)CURRENT_TIME.Sec() + dataEntry->titleDueTime * MIN_TO_SEC;
			guild->OnGuildBattleScoreRankReward(dataEntry->titleId, expireTime);

			return true;
		}
	};

	GuildBattleScoreRankVisitor vistor;
	WSSortListMgr::Instance()->VisitSortList(SORTLIST_GUILD_BATTLE_WEEK_SCORE, vistor);
}

void GuildMgr::OnMemberNameChanged(ObjID_t roleId, const std::string& name)
{
	auto member = FindGuildMember(roleId);
	if (!member)
	{
		return;
	}

	member->SetName(name);

	auto guild = FindGuildByID(member->GetGuildID());
	if (guild && guild->GetLeader() == member)
	{
		guild->SetLeaderName(name);
	}
}

void GuildMgr::InitGuildLevelSortList()
{
	HashMap<ObjID_t, Guild*>::iterator iter = m_GuildsByID.begin();
	while (iter != m_GuildsByID.end())
	{
		iter->second->UpdateGuildLevelSortList();
		GuildMember* leader = iter->second->GetLeader();
		if (leader != NULL)
		{
			WSSortListMgr::Instance()->OnGuildLeaderChanged(iter->second->GetID(), leader->GetID(), leader->GetName());
		}
		iter++;
	}
}

Guild* GuildMgr::FindGuildByID(ObjID_t id)
{
	auto itr = m_GuildsByID.find(id);
	return itr == m_GuildsByID.end() ? NULL : itr->second;
}

Guild* GuildMgr::FindGuildByName(const std::string& name)
{
	auto itr = m_GuildsByName.find(name);
	return itr == m_GuildsByName.end() ? NULL : itr->second;
}

void GuildMgr::AddGuildMember(GuildMember* member)
{
	if (!member)
	{
		return;
	}

	m_GuildMembers[member->GetID()] = member;
}

GuildMember* GuildMgr::FindGuildMember(ObjID_t id)
{
	auto itr = m_GuildMembers.find(id);
	return itr == m_GuildMembers.end() ? NULL : itr->second;
}

bool GuildMgr::IsStrValid(const std::string& str, UInt32 maxWidth, bool isName)
{
	if (isName && !CheckNameValid(str.c_str(), maxWidth, false))
	{
		return false;
	}

	char acStr[1024];
	memset(acStr, 0, sizeof(acStr));
	avalon_strncpy(acStr, str.c_str(), sizeof(acStr) - 1);
	acStr[sizeof(acStr) - 1] = 0;

	if (ForbitWordParser::Instance()->Parse(acStr) != FORBIT_WORD_UNCHANGED)
	{
		return false;
	}

	return true;
}

UInt32 GuildMgr::IsNameValid(const std::string& name)
{
	if (!IsStrValid(name, GUILD_NAME_MAX_WIDTH, true))
	{
		return ErrorCode::GUILD_NAME_INVALID;
	}
	
	if (FindGuildByName(name))
	{
		return ErrorCode::GUILD_NAME_REPEAT;
	}

	return ErrorCode::SUCCESS;
}

void GuildMgr::ClearJoinGuildRequest(ObjID_t id)
{
	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (!guild)
		{
			continue;
		}

		if (guild->IsRequesting(id))
		{
			guild->DelRequester(id);

			// 如果没有其他玩家了，需要把红点清掉
			guild->NotifyRequesterIsEmpty();
		}
	}
}

void GuildMgr::OnPlayerForeverDeleted(ObjID_t roleId)
{
	auto member = FindGuildMember(roleId);
	if (!member)
	{
		return;
	}

	auto guild = FindGuildByID(member->GetGuildID());
	if (guild)
	{
		// 从工会中删除
		guild->OnPlayerForeverDeleted(roleId);
	}

	m_GuildMembers.erase(member->GetID());

	member->DelFromDB();
	delete member;
}

bool GuildMgr::IsInSameGuild(ObjID_t roleId1, ObjID_t roleId2)
{
	auto member = FindGuildMember(roleId1);
	if (!member)
	{
		return false;
	}

	auto guild = FindGuildByID(member->GetGuildID());
	if (!guild)
	{
		return false;
	}

	if (guild->FindMember(roleId1) == NULL || guild->FindMember(roleId2) == NULL)
	{
		return false;
	}

	return true;
}

void GuildMgr::SetTerrOccupy(UInt8 terrId, ObjID_t guildId)
{
	m_TerrOccupy[terrId] = guildId;
}

void GuildMgr::ClearEnroll()
{
	Avalon::Date nowData = CURRENT_DATE;
	bool isRefresh = false;
	UInt32 staticWDay = 1;
	//每周一清空历史领地
	if (nowData.WDay() == (Int32)staticWDay)
	{
		isRefresh = true;
	}

	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (!guild)
		{
			continue;
		}

		guild->ClearEnroll();

		//每周一清空历史领地
		if (isRefresh)
		{
			guild->SetHistoryTerrId(0);
		}
	}
}

void GuildMgr::OnEnrollBegin()
{
	HashMap<ObjID_t, GuildMember*>::iterator iterMember = m_GuildMembers.begin();
	while (iterMember != m_GuildMembers.end())
	{
		iterMember->second->OnEnrollBegin();
		++iterMember;
	}
}

UInt32 GuildMgr::GetGuildBattleInspireInfos(ObjID_t guildId, std::vector<GuildBattleInspireInfo>& inspireInfos)
{
	Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	return guild->GetGuildBattleInspireInfo(inspireInfos);
}

UInt32 GuildMgr::SetGuildStorageSetting(WSPlayer* player, GuildStorageSetting type, UInt32 value)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	GuildMember* member = GuildMgr::Instance()->FindGuildMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (member->GetPost() != GuildPost::GUILD_POST_LEADER) return ErrorCode::GUILD_NO_POWER;

	return guild->SetGuildStorageSetting(type, value);
}

UInt32 GuildMgr::GetStorageList(WSPlayer * player, UInt32& maxSize, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& record)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	Guild* guild = FindGuildByID(player->GetGuildID());
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;

	maxSize = guild->GetStorageMaxSize();
	guild->GetStorageItems(items);
	guild->GetStorageRecords(record);

	return ErrorCode::SUCCESS;
}

UInt32 GuildMgr::AddStorageItem(WSPlayer* player, std::vector<GuildStorageItemInfo> inItems)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;
	
	if (GuildBattleMgr::Instance()->IsGuildBattleLottery()) return ErrorCode::GUILD_STORAGE_LETTORY_ERROR;

	Guild* guild = FindGuildByID(player->GetGuildID());
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;
	if (guild->GetDismissTime() > 0) return ErrorCode::GUILD_STORAGE_DISMISS_ERROR;

	GuildMember* member = FindGuildMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (member->GetPost() < guild->GetStorageAddPost()) return ErrorCode::GUILD_NO_POWER;

	class GuildAddStorageItem : public AddGuildStorageItemCb
	{
	public:
		GuildAddStorageItem(WSPlayer* player, Guild* guild) :m_Player(player), m_Guild(guild){}
		virtual ~GuildAddStorageItem(){}

		void OnSuccess(std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageItemInfo>& recordItems)
		{
			if (m_Player == NULL || m_Guild == NULL) return;

			CLProtocol::WorldGuildStorageItemSync sync;
			sync.items = items;
			m_Guild->InsertStorageItemRecords(m_Player->GetName(), GuildStorageOpType::GSOT_PUT, recordItems, sync.itemRecords);
			m_Guild->Broadcast(sync);

			CLProtocol::WorldGuildAddStorageRes res;
			res.result = ErrorCode::SUCCESS;
			m_Player->SendProtocol(res);
		}

		void OnFailed(UInt32 code)
		{
			if (m_Player != NULL)
			{
				CLProtocol::WorldGuildAddStorageRes res;
				res.result = code;
				m_Player->SendProtocol(res);
			}
		}

	public:
		WSPlayer* m_Player;
		Guild* m_Guild;
	};

	GuildAddStorageItem* itemcb = new GuildAddStorageItem(player, guild);

	guild->AddStorageItem(player, inItems, itemcb);

	return ErrorCode::SUCCESS;
}

UInt32 GuildMgr::DelStorageItem(WSPlayer* player, std::vector<GuildStorageDelItemInfo> delItems)
{
	if (player == NULL) return ErrorCode::GUILD_BATTLE_NOT_PLAYER;

	if (GuildBattleMgr::Instance()->IsGuildBattleLottery()) return ErrorCode::GUILD_STORAGE_LETTORY_ERROR;

	Guild* guild = FindGuildByID(player->GetGuildID());
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;
	if (guild->GetDismissTime() > 0) return ErrorCode::GUILD_STORAGE_DISMISS_ERROR;

	GuildMember* member = FindGuildMember(player->GetID());
	if (member == NULL) return ErrorCode::GUILD_BATTLE_NOT_IS_MEMBER;

	if (member->GetPost() < guild->GetStorageDelPost()) return ErrorCode::GUILD_NO_POWER;

	CLProtocol::WorldGuildStorageItemSync sync;
	guild->DelStorageItems(delItems, sync.items);
	if (sync.items.size() > 0)
	{
		guild->Broadcast(sync);
	}
	return ErrorCode::SUCCESS;
}

void GuildMgr::OnPlayerRequestCreateGuild(WSPlayer* player, std::string name, std::string declaration)
{
	if (!player)
	{
		return;
	}

	InfoLogStream << "player(" << player->GetID() << ") request create guild name(" << name << ") declaration(" << declaration << ")." << LogStream::eos;

	CLProtocol::WorldGuildCreateRes res;
	if (player->GetGuildID() > 0)
	{
		res.result = ErrorCode::GUILD_CREATE_ALREADY_HAS_GUILD;
		player->SendProtocol(res);
		return;
	}

	// 等级
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_GUILD);
	if (functionData)
	{
		if (player->GetLevel() < functionData->endLevel)
		{
			res.result = ErrorCode::GUILD_CREATE_MIN_LEVEL;
			player->SendProtocol(res);
			return;
		}
	}

	auto member = FindGuildMember(player->GetID());
	if (member && member->GetLeaveTime() > 0 && CURRENT_TIME.Sec() < member->GetLeaveTime())
	{
		res.result = ErrorCode::GUILD_CREATE_COLDTIME;
		player->SendProtocol(res);
		return;
	}

	// 检测name和declaration的合法性(name要判断有没有被用过)
	if (name.empty())
	{
		res.result = ErrorCode::GUILD_CREATE_NAME_EMPTY;
		player->SendProtocol(res);
		return;
	}

	res.result = IsNameValid(name);
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
		return;
	}

	if (declaration.empty())
	{
		res.result = ErrorCode::GUILD_CREATE_DECLARATION_EMPTY;
		player->SendProtocol(res);
		return;
	}

	if (!IsStrValid(declaration, GUILD_DECLARATION_MAX_WIDTH))
	{
		res.result = ErrorCode::GUILD_DECLARATION_INVALID;
		player->SendProtocol(res);
		return;
	}

	// 开始事务
	auto trans = new CreateGuildTransaction(player, name, declaration);
	trans->SetSilver(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_CREATE_COST));
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << "player(" << player->GetID() << ") create guild, begin transaction failed." << LogStream::eos;

		res.result = ErrorCode::GUILD_CREATE_REPEAT;
		player->SendProtocol(res);
		return;
	}
}

void GuildMgr::OnPlayerRequestGuildList(WSPlayer* player, UInt16 start, UInt16 num)
{
	CLProtocol::WorldGuildListRes res;
	res.start = start;
	res.totalnum = m_GuildsByID.size();

	class GuildComparer
	{
	public:
		bool operator()(Guild* a, Guild* b)
		{
			if (a->GetLevel() != b->GetLevel())
			{
				return a->GetLevel() > b->GetLevel();
			}

			if (a->GetFund() != b->GetFund())
			{
				return a->GetFund() > b->GetFund();
			}

			return a->GetID() < b->GetID();
		}
	};
	std::set<Guild*, GuildComparer> guildList;
	for (auto itr : m_GuildsByID)
	{
		guildList.insert(itr.second);
	}

	GuildEntry entry;
	UInt16 index = 0;
	for (auto itr : guildList)
	{
		if (index++ < start)
		{
			continue;
		}

		if (index > start + num)
		{
			break;
		}

		itr->GetGuildEntry(player, entry);
		res.guilds.push_back(entry);
	}

	player->SendProtocol(res);
}

void GuildMgr::OnPlayerRequestJoinGuild(WSPlayer* player, ObjID_t id)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldJoinGuildRes res;
	
	if (id > 0)
	{
		auto guild = FindGuildByID(id);
		if (!guild)
		{
			ErrorLogStream << "player(" << player->GetID() << ") want to join unexist guild(" << id << ")." << LogStream::eos;
			res.result = ErrorCode::GUILD_JOIN_NOT_EXIST;
			player->SendProtocol(res);
			return;
		}

		res.result = guild->RequestJoinGuild(player);
	}
	else
	{
		// 一键加入所有的公会
		do 
		{
			if (player->GetGuildID() > 0)
			{
				DebugLogStream << "player(" << player->GetID() << ") already in guild(" << player->GetGuildID() << ")." << LogStream::eos;
				res.result = ErrorCode::GUILD_JOIN_ALREADY_HAS_GUILD;
				break;
			}

			// 等级
			auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_GUILD);
			if (functionData && player->GetLevel() < functionData->endLevel)
			{
				DebugLogStream << "player(" << player->GetID() << ") level limit(" << player->GetLevel() << ")." << LogStream::eos;
				res.result = ErrorCode::GUILD_JOIN_MIN_LEVEL;
				break;
			}

			// 退出公会冷却时间
			auto* oldMember = GuildMgr::Instance()->FindGuildMember(player->GetID());
			if (oldMember != NULL && CURRENT_TIME.Sec() < oldMember->GetLeaveTime())
			{
				DebugLogStream << "player(" << player->GetID() << ") enter cold down." << LogStream::eos;
				res.result = ErrorCode::GUILD_JOIN_COLDTIME;
				break;
			}

			res.result = ErrorCode::GUILD_NOT_CAN_JOIN;

			for (auto itr : m_GuildsByID)
			{
				auto guild = itr.second;
				if (!guild || guild->IsRequesting(player->GetID()))
				{
					continue;
				}

				UInt32 ret = guild->RequestJoinGuild(player);
				if (ret == ErrorCode::SUCCESS)
				{
					res.result = ErrorCode::SUCCESS;
				}
			}

		} while (0);
	}

	player->SendProtocol(res);
}

void GuildMgr::OnPlayerRequestCanMergerGuildList(WSPlayer* &player, UInt16 start, UInt16 num)
{
	CLProtocol::WorldGuildWatchCanMergerRet res;
	res.start = start;
	res.totalNum = 0;

	auto guild = FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		player->SendProtocol(res);
		ErrorLogStream << "Can't find guild  guildid is(" << player->GetGuildID() << ")!" << LogStream::eos;
		return;
	}

	auto member = guild->FindMember(player->GetID());
	if (!member)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " do not have mermber (" << player->GetID() << ")!" << LogStream::eos;
		return;
	}

	if (member->GetPost() != GUILD_POST_LEADER)
	{
		player->SendProtocol(res);
		ErrorLogStream << "player do not have access " << player->GetID() << " post is " << member->GetPost() << ")!" << LogStream::eos;
		return;
	}

	if (guild->GetProsperity() != GUILD_PROSPERITY_MID)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " GetProsperity error (" << guild->GetProsperity() << ")!" << LogStream::eos;
		return;
	}

	std::vector<Guild*> guildList;


	for (auto &i : m_LowAndDismissGuild)
	{
		if (i->GetLevel() <= guild->GetLevel())
		{
			guildList.push_back(i);
		}
	}
	std::sort(guildList.begin(), guildList.end(), GuildComparer());

	res.totalNum = guildList.size();

	GuildEntry entry;
	UInt16 index = 0;
	for (auto &itr : guildList)
	{
		if (index++ < start)
		{
			continue;
		}

		if (index > start + num)
		{
			break;
		}

		itr->GetGuildEntry(player, entry);
		entry.isRequested = 0;
		if (entry.id == guild->GetMergerGoalGuildID())
		{
			entry.isRequested = 1;
		}
		res.guilds.push_back(entry);
	}
	member->SyncEventToScene(SET_CLR_REDPOINT, RED_POINT_GUILD_MERGER);
	player->SendProtocol(res);
}

void GuildMgr::OnPlayerMergerRequestOperator(WSPlayer* &player, UInt64 guildId, UInt8 opType)
{
	CLProtocol::WorldGuildMergerRequestOperatorRet res;
	res.errorCode = ErrorCode::GUILD_MERGER_INVAILD;
	res.opType = opType;

	auto guild = FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		player->SendProtocol(res);
		ErrorLogStream << "Can't find guild  guildid is(" << player->GetGuildID() << ")!" << LogStream::eos;
		return;
	}

	if (!guild->CanMerger())
	{
		res.errorCode = ErrorCode::GUILD_MERGER_TIME_SHORT;
		player->SendProtocol(res);
		return;
	}

	if (guild->GetProsperity() != GUILD_PROSPERITY_MID)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " GetProsperity error (" << guild->GetProsperity() << ")!" << LogStream::eos;
		return;
	}

	auto member = guild->FindMember(player->GetID());
	if (!member)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " do not have mermber (" << player->GetID() << ")!" << LogStream::eos;
		return;
	}

	if (member->GetPost() != GUILD_POST_LEADER)
	{
		player->SendProtocol(res);
		ErrorLogStream << "player do not have access " << player->GetID() << " post is " << member->GetPost() << ")!" << LogStream::eos;
		return;
	}

	auto goalGuild = FindGuildByID(guildId);
	if (!goalGuild)
	{
		player->SendProtocol(res);
		ErrorLogStream << "Can't find goalGuild  goalGuildid is(" << guildId << ")!" << LogStream::eos;
		return;
	}

	if (goalGuild->GetProsperity() != GUILD_PROSPERITY_LOW && goalGuild->GetProsperity() != GUILD_PROSPERITY_DISMISS)
	{
		player->SendProtocol(res);
		ErrorLogStream << "goalGuild " << guildId << " GetProsperity error (" << guild->GetProsperity() << ")!" << LogStream::eos;
		return;
	}

	if (goalGuild->GetLevel() > guild->GetLevel())
	{
		player->SendProtocol(res);
		ErrorLogStream << "goalGuild " << guildId << " level is too high (" << goalGuild->GetLevel() << ")!" << LogStream::eos;
		return;
	}
	
	if (opType == 0)
	{
		res.errorCode = guild->RequestMerger(goalGuild);
		if (res.errorCode == ErrorCode::SUCCESS)
		{
			CLProtocol::WorldGuildReceiveMergerRequestRet msg;
			msg.isHave = 1;
			auto player = goalGuild->GetLeader();
			if (player != nullptr)
			{
				player->SendProtocol(msg);
			}
		}
	}
	else if (opType == 1)
	{
		res.errorCode = guild->CancelMerger(goalGuild);
	}
	else
	{
		res.errorCode = ErrorCode::GUILD_MERGER_INVAILD;
	}
	player->SendProtocol(res);
}

void GuildMgr::OnPlayerQueryIsHaveMergerRequest(WSPlayer* &player)
{
	CLProtocol::WorldGuildReceiveMergerRequestRet res;
	res.isHave = 0;

	auto guild = FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		player->SendProtocol(res);
		ErrorLogStream << "Can't find guild  guildid is(" << player->GetGuildID() << ")!" << LogStream::eos;
		return;
	}

	if (guild->GetProsperity() != GUILD_PROSPERITY_LOW && guild->GetProsperity() != GUILD_PROSPERITY_DISMISS)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " GetProsperity error (" << guild->GetProsperity() << ")!" << LogStream::eos;
		return;
	}

	auto member = guild->FindMember(player->GetID());
	if (!member)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " do not have mermber (" << player->GetID() << ")!" << LogStream::eos;
		return;
	}

	if (member->GetPost() != GUILD_POST_LEADER)
	{
		player->SendProtocol(res);
		ErrorLogStream << "player do not have access " << player->GetID() << " post is " << member->GetPost() << ")!" << LogStream::eos;
		return;
	}

	if (!guild->GetMergerRequestedGuildSet().empty())
	{
		res.isHave = 1;
	}
	player->SendProtocol(res);
}

void GuildMgr::OnPlayerQueryReceiveMergerRequestList(WSPlayer* &player)
{
	CLProtocol::WorldGuildWatchHavedMergerRequestRet res;

	auto guild = FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		player->SendProtocol(res);
		ErrorLogStream << "Can't find guild  guildid is(" << player->GetGuildID() << ")!" << LogStream::eos;
		return;
	}

	if (guild->GetProsperity() != GUILD_PROSPERITY_LOW && guild->GetProsperity() != GUILD_PROSPERITY_DISMISS)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " GetProsperity error (" << guild->GetProsperity() << ")!" << LogStream::eos;
		return;
	}

	auto member = guild->FindMember(player->GetID());
	if (!member)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " do not have mermber (" << player->GetID() << ")!" << LogStream::eos;
		return;
	}

	if (member->GetPost() != GUILD_POST_LEADER)
	{
		player->SendProtocol(res);
		ErrorLogStream << "player do not have access " << player->GetID() << " post is " << member->GetPost() << ")!" << LogStream::eos;
		return;
	}


	auto guildIDList = guild->GetMergerRequestedGuildSet();

	GuildEntry entry;
	for (auto &itr : guildIDList)
	{
		auto guild = FindGuildByID(itr);
		if (guild == nullptr)
		{
			continue;
		}
		guild->GetGuildEntry(player, entry);
		entry.isRequested = 0;
		if (guild->GetMergerRequestStatus() == GUILD_MERGER_REQUEST_ACCEPTED)
		{
			entry.isRequested = 1;
		}
		res.guilds.push_back(entry);
	}
	player->SendProtocol(res);
}

void GuildMgr::OnPlayerOperaMergerRequest(WSPlayer* &player, UInt64 guildId, UInt8 opType)
{
	CLProtocol::WorldGuildAcceptOrRefuseOrCancleMergerRequestRet res;
	res.errorCode = ErrorCode::GUILD_MERGER_INVAILD;
	res.opType = opType;
	auto guild = FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		player->SendProtocol(res);
		ErrorLogStream << "Can't find guild  guildid is(" << player->GetGuildID() << ")!" << LogStream::eos;
		return;
	}

	if (guild->GetProsperity() != GUILD_PROSPERITY_LOW && guild->GetProsperity() != GUILD_PROSPERITY_DISMISS)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " GetProsperity error (" << guild->GetProsperity() << ")!" << LogStream::eos;
		return;
	}

	auto member = guild->FindMember(player->GetID());
	if (!member)
	{
		player->SendProtocol(res);
		ErrorLogStream << "guild " << player->GetGuildID() << " do not have mermber (" << player->GetID() << ")!" << LogStream::eos;
		return;
	}

	if (member->GetPost() != GUILD_POST_LEADER)
	{
		player->SendProtocol(res);
		ErrorLogStream << "player do not have access " << player->GetID() << " post is " << member->GetPost() << ")!" << LogStream::eos;
		return;
	}


	if (opType == 0 || opType == 1 || opType == 2)
	{
		auto goalGuild = FindGuildByID(guildId);
		if (!goalGuild)
		{
			player->SendProtocol(res);
			ErrorLogStream << "Can't find goalGuild  goalGuildid is(" << guildId << ")!" << LogStream::eos;
			return;
		}

		if (goalGuild->GetProsperity() != GUILD_PROSPERITY_MID)
		{
			player->SendProtocol(res);
			ErrorLogStream << "goalGuild " << guildId << " GetProsperity error (" << goalGuild->GetProsperity() << ")!" << LogStream::eos;
			return;
		}

		if (goalGuild->GetLevel() < guild->GetLevel())
		{
			player->SendProtocol(res);
			ErrorLogStream << "goalGuild " << guildId << " level is too low (" << goalGuild->GetLevel() << ")!" << LogStream::eos;
			return;
		}

		//对别人发送给自己的请求操作 0接受请求 1拒绝请求 2取消已同意的请求
		if (opType == 0)
		{
			res.errorCode = guild->AcceptMergerRequest(goalGuild);
		}
		else if (opType == 1)
		{
			res.errorCode = guild->RefuseMergerRequest(goalGuild);
		}
		else if (opType == 2)
		{
			res.errorCode = guild->CancelMergerRequest(goalGuild);
		}
	}
	else if (opType == 3)
	{
		res.errorCode = guild->CleanAllMergerRequest();
	}
	else
	{
		res.errorCode = ErrorCode::GUILD_MERGER_INVAILD;
	}
	player->SendProtocol(res);
}

bool GuildMgr::IsMergerEnoughOpenDay()
{
	if (CURRENT_TIME.Sec() >  GAME_FIRST_START_TIME  + Avalon::Time::SECS_OF_DAY * 14)
	{
		return true;
	}
	return false;
}

void GuildMgr::OnPlayerOnline(WSPlayer* player, bool dayChanged)
{
	if (!player)
	{
		return;
	}

	auto member = FindGuildMember(player->GetID());
	if (!member || member->GetLeaveTime() != 0 || !member->GetGuildID())
	{
		player->SetGuildID(0);
		player->SetGuildLvl(0);
		// 没有公会也发个消息下去
		CLProtocol::WorldGuildSyncInfo info;
		player->SendProtocol(info);
		return;
	}

	member->SetPlayer(player);

	auto guild = FindGuildByID(member->GetGuildID());
	if (!guild)
	{
		// 没有公会也发个消息下去
		CLProtocol::WorldGuildSyncInfo info;
		player->SendProtocol(info);
		return;
	}

	if (guild->GetOccupyTerrID() != 0)
	{
		player->OnGuildOccupyTerritory(guild->GetOccupyTerrID());
	}

	player->OnJoinGuild();

	guild->OnPlayerOnline(player, dayChanged);
}

void GuildMgr::OnPlayerOffline(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	auto member = FindGuildMember(player->GetID());
	if (!member)
	{
		return;
	}

	if (member->GetGuildID() == 0 || member->GetLeaveTime() != 0)
	{
		member->SetPlayer(NULL);
		return;
	}

	auto guild = FindGuildByID(member->GetGuildID());
	if (!guild)
	{
		member->SetPlayer(NULL);
		return;
	}

	guild->OnPlayerOffline(player);
	member->SetPlayer(NULL);
}

void GuildMgr::OnPlayerPropertyChanged(WSPlayer* player)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto member = FindGuildMember(player->GetID());
	if (!member)
	{
		return;
	}

	member->SetLevel(player->GetLevel());
	member->SetOccu(player->GetOccu());
	member->SetVipLevel(player->GetVipLvl());
	member->SetSeasonLevel(player->GetSeasonLv());
}

void GuildMgr::OnScenePlayerAddContri(ObjID_t roleId, UInt32 num)
{
	auto member = FindGuildMember(roleId);
	if (!member || member->GetLeaveTime() > 0)
	{
		return;
	}

	auto guild = FindGuildByID(member->GetGuildID());
	if (!guild)
	{
		return;
	}

	guild->OnScenePlayerAddContri(roleId, num);
}

bool GuildMgr::OnCheckCreateGuildRet(WSPlayer* player, std::string name, std::string declaration)
{
	CLProtocol::WorldGuildCreateRes res;
	if (player->GetGuildID() > 0)
	{
		res.result = ErrorCode::GUILD_CREATE_ALREADY_HAS_GUILD;
		player->SendProtocol(res);
		return false;
	}

	// 创建公会
	auto guild = new Guild();
	guild->SetID(CLRecordClient::Instance()->GenGuid());
	guild->SetName(name);
	guild->SetGuildEventGuildID();
	guild->SetDeclaration(declaration);
	guild->SetLevel(1);
	guild->SetLastUpdateGuildBuildingTime((UInt32)CURRENT_TIME.Sec());
	guild->SetUpTime((UInt32)CURRENT_TIME.Sec());
	guild->Init(GUILD_INIT_TYPE_CREATE);
	guild->InsertToDB();

	// 设置会长
	auto leader = FindGuildMember(player->GetID());
	if (!leader)
	{
		leader = new GuildMember();
		leader->Init(player);
	}
	leader->SetLeaveTime(0);
	leader->SetPost(GUILD_POST_LEADER, false);

	guild->OnMemberEnter(player, leader);

	_AddGuild(guild);

	//设置公会头衔
	player->GetTitleMgr().OnPlayerGuildPostChange(GUILD_POST_NORMAL, GUILD_POST_LEADER);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	//创建公会.更新公会榜
	guild->UpdateGuildLevelSortList();

	// 通知公会地下城活动
	GuildDungeonMgr::Instance()->GuildLevelUp(guild);

	return true;
}

bool GuildMgr::OnSelectGuildRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		Guild* guild = new Guild();
		guild->SetID(callback->GetKey());
		guild->SetDBData(callback);
		if (!guild->Init(GUILD_INIT_TYPE_LOAD))
		{
			ErrorLogStream << "guild(" << guild->GetID() << ") init failed." << LogStream::eos;
			delete guild;
			continue;
		}

		_AddGuild(guild);

		if (guild->GetEnrollTerrID() > 0)
		{
			GuildBattleMgr::Instance()->LoadEnroll(guild);
		}

		if (guild->GetOccupyTerrID() > 0)
		{
			SetTerrOccupy(guild->GetOccupyTerrID(), guild->GetID());
			GuildBattleMgr::Instance()->SetTerrGuild(guild);
		}

		if (guild->GetOccupyCrossTerrID() > 0)
		{
			GuildBattleMgr::Instance()->SetCrossTerrGuild(guild);
		}
	}

	//加载公会事件
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_event");
	cmd->SetLimit(" ORDER BY `add_time` DESC");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectGuildEventCallback());

	// 加载成员信息
	CLSelectCommand* member_command = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_member");
	CLRecordClient::Instance()->SendCommand(member_command, new SelectGuildMemberCallback());

	// 加载公会地下城数据
	GuildDungeonMgr::Instance()->LoadData();

	return true;
}

bool GuildMgr::OnSelectGuildMemberRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		GuildMember* member = new GuildMember();
		member->SetID(callback->GetKey());
		member->SetDBData(callback);

		if (member->GetLeaveTime() == 0 && member->GetGuildID())
		{
			Guild* guild = FindGuildByID(member->GetGuildID());
			if (guild == NULL)
			{
				ErrorLogStream << "unknow guild " << member->GetGuildID() << " when load member " << member->GetID() << "!" << LogStream::eos;
				member->SetGuildID(0);
			}
			else
			{
				if (member->GetOfflineTime() == 0)
				{
					member->SetOfflineTime(UInt32(CURRENT_TIME.Sec()));
				}

				if (!guild->AddMember(member))
				{
					ErrorLogStream << "duplicate guild member " << member->GetID() << "!" << LogStream::eos;
					CL_SAFE_DELETE(member);
					continue;
				}
			}
		}

		GuildMgr::Instance()->AddGuildMember(member);
	}

	InfoLogStream << "load guild data finish..." << LogStream::eos;
	m_LoadFinish = true;
	InitMerger();

	return true;
}

bool GuildMgr::OnSelectGuildEventRet(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		Guild* guild = FindGuildByID(callback->GetData("guild_id"));
		if (guild)
		{
			GuildEvent* guildEvent = new GuildEvent();

			guildEvent->guid = callback->GetKey();
			guildEvent->addTime = callback->GetData("add_time");
			guildEvent->eventInfo = callback->GetData("event_info").GetString();

			guild->LoadGuildEvent(guildEvent);
		}
	}
	return true;
}


void GuildMgr::Broadcast(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		Broadcast(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GuildMgr::Broadcast(Avalon::Packet* packet)
{
	if (packet == NULL)
	{
		return;
	}

	HashMap<ObjID_t, Guild*>::iterator iter = m_GuildsByID.begin();
	while (iter != m_GuildsByID.end())
	{
		if (iter->second == NULL) continue;
		iter->second->Broadcast(packet);
		++iter;
	}
}

void GuildMgr::Broadcast(Avalon::Protocol& protocol, UInt32 lvlLimit, UInt32 memberLimitLvl)
{
	HashMap<ObjID_t, Guild*>::iterator iter = m_GuildsByID.begin();
	for (; iter != m_GuildsByID.end();++iter)
	{
		if (iter->second == NULL) continue;
		if (iter->second->GetLevel() < lvlLimit)
			continue;

		iter->second->Broadcast(protocol, memberLimitLvl);
	}
}

void GuildMgr::GuildChat(const std::string& content, UInt32 lvlLimit, bool bLink)
{
	HashMap<ObjID_t, Guild*>::iterator iter = m_GuildsByID.begin();
	for (; iter != m_GuildsByID.end();++iter)
	{
		if (iter->second == NULL) continue;
		if (iter->second->GetLevel() < lvlLimit) 
			continue;

		iter->second->Chat(content, bLink);
	}
}

void GuildMgr::_AddGuild(Guild* guild)
{
	m_GuildsByID[guild->GetID()] = guild;
	m_GuildsByName[guild->GetName()] = guild;
}

void GuildMgr::_DelGuild(Guild* guild)
{
	if (!guild)
	{
		return;
	}

	for (auto i : m_GuildsByID)
	{
		auto eachGuild = i.second;
		if (eachGuild != nullptr)
		{
			if (eachGuild->GetMergerGoalGuildID() == guild->GetID())
			{
				eachGuild->CleanSelfMergerRequest();
			}
		}
	}

	m_LowAndDismissGuild.erase(guild);
	m_MidGuild.erase(guild);
	m_GuildsByID.erase(guild->GetID());
	m_GuildsByName.erase(guild->GetName());

	delete guild;
}

void GuildMgr::InitMerger()
{
	m_MidGuild.clear();
	m_LowAndDismissGuild.clear();
	for (auto itr : m_GuildsByID)
	{
		auto guild = itr.second;
		if (!guild)
		{
			continue;
		}

		if (guild->GetMergerGoalGuildID() != 0)
		{
			auto goalGuild = guild->GetMergerGoalGuildID() != 0 ? FindGuildByID(guild->GetMergerGoalGuildID()) : nullptr;
			if (goalGuild != nullptr)
			{
				goalGuild->AddMergerRequestGuild(guild);
			}
			else
			{
				guild->CleanSelfMergerRequest();
				ErrorLogStream << "set merger goalGuild failed,can't find goalGuild(" << guild->GetMergerGoalGuildID() << ") " << LogStream::eos;
			}
		}

		if (guild->GetProsperity() == GUILD_PROSPERITY_MID)
		{
			m_MidGuild.insert(guild);
		}
		else if (guild->GetProsperity() == GUILD_PROSPERITY_LOW || guild->GetProsperity() == GUILD_PROSPERITY_DISMISS)
		{
			m_LowAndDismissGuild.insert(guild);
		}
	}
}
