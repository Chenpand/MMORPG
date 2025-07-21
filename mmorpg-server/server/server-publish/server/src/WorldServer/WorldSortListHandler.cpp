#include "WorldSortListHandler.h"

#include <set>
#include <CLSystemValueDataEntry.h>
#include "WSSortListMgr.h"
#include "ChatDataMgr.h"
#include "WSPlayerMgr.h"
#include "WSSceneMgr.h"
#include "WSRouter.h"
#include "ScriptMgr.h"
#include "TimerScript.h"
#include "GuildBattleMgr.h"
#include "CLScoreWarDataEntry.h"
#include "CLBattleProtocol.h"
#include "CL2V2ScoreWarDataEntry.h"
#include "AccountMgr.h"
#include "WSSortListMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSortListReq& protocol)
{
	if(IsWorldSortList(protocol.type))
	{
		Router::TransmitPlayerProtocolToSortList(player->GetID(), protocol);
	}
	else if (IsGuildBattleSortList(protocol.type))
	{
		GuildBattleMgr::Instance()->ListSortEntries(player, (SortListType)protocol.type, protocol.start, protocol.num);
	}
	else if (IsScoreWarSortList(protocol.type))
	{
		CLProtocol::CrossScoreWarSortListReq req;
		req.type = protocol.type;
		req.playerId = player->GetID();
		req.num = protocol.num;
		req.start = protocol.start;
		Router::SendToCross(req);
	}
	else if (Is2V2ScoreWarSortList(protocol.type))
	{
		CLProtocol::Cross2V2ScoreWarSortListReq req;
		req.type = protocol.type;
		req.playerId = player->GetID();
		req.num = protocol.num;
		req.start = protocol.start;
		Router::SendToCross(req);
	}
	else if (IsChijiSortList(protocol.type))
	{
		CLProtocol::BattleSortListReq req;
		req.type = protocol.type;
		req.playerId = player->GetID();
		req.num = protocol.num;
		req.start = protocol.start;
		Router::SendToBattle(req);
	}
	else
	{
		WSSortListMgr::Instance()->ListSortEntries(player, protocol.type, protocol.occu, protocol.start, protocol.num);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSortListNearReq& protocol)
{
	if(IsWorldSortList(protocol.type))
	{
		Router::TransmitPlayerProtocolToSortList(player->GetID(), protocol);
	}
	else
	{
		WSSortListMgr::Instance()->ListSortEntriesNear(player, protocol.type, protocol.num);
	}
}

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldSortListSelfInfoReq &protocol)
{
	CLProtocol::WorldSortListSelfInfoRet ret;
	if (IsGuildBattleSortList(protocol.type))
	{
		ret.ranking = GuildBattleMgr::Instance()->GetRanking((SortListType)protocol.type, pPlayer);
	}
	else
	{
		ret.ranking = WSSortListMgr::Instance()->GetRanking(protocol.type, pPlayer->GetID());
	}
	pPlayer->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::WorldSortListUpdateReq& protocol)
{
	WSSortListMgr::Instance()->UpdateSortList(protocol.entry);
}

void HandleProtocol(CLProtocol::WorldSortListOwnerChangedReq& protocol)
{
	WSSortListMgr::Instance()->OnOwnerChanged((SortListType)protocol.type, protocol.entryId, protocol.ownerId, protocol.ownerName);
}

void HandleProtocol(CLProtocol::WorldSortListDeleteEntryReq& protocol)
{
	WSSortListMgr::Instance()->DeleteEntryById(protocol.sortType, protocol.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSortListWatchDataReq& protocol)
{
	WSSortListMgr::Instance()->WatchData(player, protocol.sortType, protocol.id);
}

void SortListWatchDataFunction::operator()(Avalon::Packet* packet) const
{
	UInt8 type = 0;
	ObjID_t watcher = 0;
	ObjID_t id = 0;

	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	try
	{
		stream & type & watcher;
		UInt8* buf = packet->GetBuffer() + stream.GetPos();
		UInt16 size = packet->GetSize() - stream.GetPos();
		stream & id;

		ChatDataMgr::Instance()->AddData(id, (const char*)buf, size);
	}
	catch(Avalon::NetException&)
	{
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(watcher);
	if(player != NULL)
	{
		ChatDataMgr::Instance()->SendLinkData(player, type, id);
	}
}

void HandleProtocol(CLProtocol::WorldSortListVisitRet& protocol)
{
	TimerScript* script = ScriptMgr::Instance()->GetTimerScript();
	UInt16 ranking = 0;
	for(std::vector<CLProtocol::SortListVisitEntry>::iterator iter = protocol.sortlist.begin();
		iter != protocol.sortlist.end(); ++iter)
	{
		++ranking;
		script->OnVisitWorldSortlist(protocol.function.c_str(), iter->id, ranking, iter->value1, iter->value2, iter->value3);
	}

	if(protocol.sortType == SORTLIST_ACTIVITYCONSUME)
	{
		std::set<ObjID_t>	sortIdSet;
		for(std::vector<CLProtocol::SortListVisitEntry>::iterator iter = protocol.sortlist.begin();
			iter != protocol.sortlist.end(); ++iter)
		{
			sortIdSet.insert(iter->id);
		}

		class GetOtherConsumePlayerVisitor : public WSSortListVisitor
		{
		public:
			GetOtherConsumePlayerVisitor(const std::set<ObjID_t>& idsets, TimerScript* script, const std::string& funcname, UInt16 ranking)
				:m_SortIdSets(idsets),m_pScript(script), m_FuncName(funcname), m_Ranking(ranking){}

			bool operator()(SortListEntry& entry, UInt32 ranking)
			{
				if(entry.value1 >= 1000 && m_SortIdSets.find(entry.id) == m_SortIdSets.end())
				{
					++m_Ranking;
					m_pScript->OnVisitWorldSortlist(m_FuncName.c_str(), entry.id, m_Ranking, entry.value1, 0, 0);
				}
				return true;
			}

		private:
			std::set<ObjID_t> m_SortIdSets;
			TimerScript* m_pScript;
			std::string	m_FuncName;
			UInt16		m_Ranking;
		};
		GetOtherConsumePlayerVisitor visitor(sortIdSet, script, protocol.function, 100);
		WSSortListMgr::Instance()->VisitSortList(SORTLIST_ACTIVITYCONSUME1, visitor);
	}
}

void HandleProtocol(CLProtocol::CrossScoreWarSortListRes& protocol)
{
	InfoLogStream << "HandleProtocol CrossScoreWarSortListRes(playerId=" << protocol.playerId << ")" << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;
	player->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::Cross2V2ScoreWarSortListRes& protocol)
{
	InfoLogStream << "HandleProtocol CrossScoreWarSortListRes(playerId=" << protocol.playerId << ")" << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;
	player->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::WorldScoreWarRankingReward& protocol)
{
	InfoLogStream << "HandleProtocol WorldScoreWarRankingReward(playerId=" << protocol.playerId << ", ranking=" << protocol.ranking << ")" << LogStream::eos;
	ScoreWarRewardDataEntry* dataEntry = ScoreWarRewardDataEntryMgr::Instance()->GetRankingDataEntry(protocol.ranking);
	if (dataEntry != NULL)
	{
		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_SCORE_WAR);
		std::string title = SysNotifyMgr::Instance()->MakeString(2221);
		std::string senderName = SysNotifyMgr::Instance()->MakeString(2222);
		std::string content = SysNotifyMgr::Instance()->MakeString(2223, protocol.ranking);
		MailBox::SendSysMail(protocol.playerId, senderName, title, content, reason, dataEntry->rewards);
	}
	else
	{
		ErrorLogStream << "HandleProtocol WorldScoreWarRankingReward(playerId=" << protocol.playerId << ", ranking=" << protocol.ranking << ")" << LogStream::eos;
	}
}


class SortList2V2GiveRewardCallback : public AccountCallback
{
public:
	SortList2V2GiveRewardCallback(UInt32 accid, UInt32 titleId, UInt32 dueTime)
		: AccountCallback(accid), m_titleId(titleId), m_dueTime(dueTime) {}
	~SortList2V2GiveRewardCallback() {}
	void OnFinish(AccountInfo* info)
	{
		if (info)
		{
			info->GetTitleMgr().AddOneTitleByTitleId(m_titleId, m_dueTime);
		}
	}

	void OnFailure()
	{
		ErrorLogStream << "load account fail for give rewards for sortlist accid : " << m_AccId << ", titleId : " << m_titleId << ", dueTime : " << m_dueTime << LogStream::eos;
	}

private:
	UInt32 m_titleId;
	UInt32 m_dueTime;
};

void HandleProtocol(CLProtocol::World2V2ScoreWarRankingReward& protocol)
{
	InfoLogStream << "HandleProtocol World2V2ScoreWarRankingReward(playerId=" << protocol.playerId << ", ranking=" << protocol.ranking << ")" << LogStream::eos;
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_2V2_SCORE_WAR);
	std::string title = SysNotifyMgr::Instance()->MakeString(2229);
	std::string senderName = SysNotifyMgr::Instance()->MakeString(2230);
	std::string content = SysNotifyMgr::Instance()->MakeString(2231, protocol.ranking);
	std::vector<ItemReward> items;
	MailBox::SendSysMail(protocol.playerId, senderName, title, content, reason, items);
	auto onlinePlayer = WSPlayerMgr::Instance()->FindPlayer(protocol.playerId);
	if (onlinePlayer != nullptr)
	{
		onlinePlayer->GetTitleMgr().AddOneTitleByTitleId(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_TITLE_ID),
			SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_TITLE_TIME) * Avalon::Time::SECS_OF_MIN + CURRENT_TIME.Sec());
	}
	else
	{
		WSPlayer* player = new WSPlayer();
		player->SetID(protocol.playerId);
		player->SetOfflineReplyCtx(player, LoadType::LT_2V2_SORTLIST_REWARD);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
		player->LoadData();
	}
}
