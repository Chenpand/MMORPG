#include "SceneSortListHandler.h"

#include "PlayerMgr.h"
#include "SceneMgr.h"
#include "SortListMgr.h"
#include "SSRouter.h"
#include "ScriptMgr.h"
#include "PlayerScript.h"
#include <CLItemDataEntry.h>
#include <CLSeasonRewardDataEntry.h>
#include "SceneSysNotify.h"
#include <set>

void SyncSortListFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	
	UInt8 sortType = 0;
	UInt64 minValue = 0;
	std::vector<ObjID_t> ids;
	try
	{
		stream & sortType & minValue & ids;
	}
	catch(Avalon::NetException&)
	{
		return;
	}

	SortListMgr::Instance()->SetEntries(sortType, ids);
	SortListMgr::Instance()->SetMinValue(sortType, minValue);
}

void HandleProtocol(CLProtocol::SceneSyncSortListMinValue& protocol)
{
	SortListMgr::Instance()->SetMinValue(protocol.sortType, protocol.value);
}

void HandleProtocol(CLProtocol::SceneSyncSortListEntry& protocol)
{
	if(protocol.flag == 1)
	{
		SortListMgr::Instance()->AddEntry(protocol.sortType, protocol.id);
	}
	else
	{
		SortListMgr::Instance()->RemoveEntry(protocol.sortType, protocol.id);
	}
}

void HandleProtocol(CLProtocol::SceneSortListWatchDataReq& protocol)
{
	SortListMgr::Instance()->WatchData(protocol.watcher, protocol.sortType, protocol.id, protocol.dataId
		, protocol.ownerId, protocol.ownerName, protocol.accId);
}

void HandleProtocol(CLProtocol::SceneSortListDailyReward& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player != NULL)
	{
		RewardGroup group;
		group.AddRewards(SeasonDailyDataEntryMgr::Instance()->GetItemRewardVec(player->GetMatchScore()));

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		SysNotifyMgr::Instance()->MakeNotify(2008, buffer, sizeof(buffer));
		std::string strSenderName(buffer);
		SysNotifyMgr::Instance()->MakeNotify(2009, buffer, sizeof(buffer));
		std::string	strTitle(buffer);
		SysNotifyMgr::Instance()->MakeNotify(2010, buffer, sizeof(buffer));
		std::string	strContent(buffer);
		std::string strReason = GetReason(SOURCE_TYPE_SEASON);
		Sys::SendSysMail(player->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), strReason.c_str(), MAIL_VALID_DAY, &group);
	}
}

void HandleProtocol(CLProtocol::SceneSortListResetSeason& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player != NULL)
	{
		UInt32 newScore = SortListMgr::Instance()->GetResetMatchScore(player->GetMatchScore());
		player->SetMatchScore(newScore);
	}
}
