#include "WSSortListMgr.h"

#include <CLRecordClient.h>
#include <CLSortListProtocol.h>
#include <CLChatDefine.h>
#include <CLSeasonRewardDataEntry.h>
#include <CLItemDefine.h>
#include <CLSeasonDataEntry.h>
#include <CLNewTitleDataEntry.h>

#include "WSSelectInitDataCallback.h"
#include "WSPlayerMgr.h"
#include "WSRouter.h"
#include "WSNetwork.h"
#include "ChatDataMgr.h"
#include "WSSceneMgr.h"
#include "WSNetwork.h"
#include "WSServiceSwitchMgr.h"
#include "GuildBattleMgr.h"
#include "GameParamMgr.h"
#include "CLSystemValueDataEntry.h"
#include "AccountMgr.h"

class SelectSortDataCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return WSSortListMgr::Instance()->OnSelectDataRet(this);
	}
};

class SelectResetSeasonCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectResetSeasonCallback, Avalon::Mutex)
public:
	SelectResetSeasonCallback(){};
	~SelectResetSeasonCallback(){};

	void OnFinished()
	{
		while (NextRow())
		{
			UInt64 playerId = GetKey();
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
			if (player != NULL)
			{
				CLProtocol::SceneSortListResetSeason protocol;
				protocol.playerId = playerId;
				player->SendToScene(protocol);
			}
			else
			{
				UInt32 matchScore = GetData("match_score");
				auto newScore = WSSortListMgr::GetResetMatchScore(matchScore);
				CLUpdateCommand	*cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", playerId, true);
				cmd->PutData("match_score", newScore);
				CLRecordClient::Instance()->SendCommand(cmd);
			}
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "SelectResetSeasonCallback::OnFailed : errorcode = " << errorcode << LogStream::eos;
	}
};

class SelectMatchScoreCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectMatchScoreCallback, Avalon::Mutex)
public:
	SelectMatchScoreCallback(std::string sendName, std::string title, std::string content, std::string reason) 
		:m_SendName(sendName), m_Title(title), m_Content(content), m_Reason(reason){};
	~SelectMatchScoreCallback(){};

	void OnFinished()
	{
		while (NextRow())
		{
			UInt64 playerId = GetKey();

			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
			if (player != NULL)
			{
				CLProtocol::SceneSortListDailyReward protocol;
				protocol.playerId = playerId;
				player->SendToScene(protocol);
			}
			else
			{
				UInt32 matchScore = GetData("match_score");
				std::vector<ItemReward> itemReward = SeasonDailyDataEntryMgr::Instance()->GetItemRewardVec(matchScore);
				MailBox::SendSysMail(playerId, m_SendName, m_Title, m_Content, m_Reason, itemReward, MAIL_VALID_DAY);
			}
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "SelectMatchScoreCallback::OnFailed : errorcode = " << errorcode << LogStream::eos;
	}

private:
	std::string m_SendName;
	std::string m_Title;
	std::string m_Content;
	std::string m_Reason;
};

void WSSortListGiveReward::Init(SortListType type, Avalon::Time now, UInt32 intervalTime)
{
	Init(type, now, intervalTime,m_SendName, m_Title, m_Content, m_Reason);
}

void WSSortListGiveReward::Init(SortListType type, Avalon::Time now, UInt32 intervalTime, std::string sendName, std::string title, std::string content, std::string reason)
{
	m_Entry.clear();

	class GiveRewardInitVisitor :public WSSortListVisitor
	{
	public:
		GiveRewardInitVisitor(std::vector<WSSortListRewardEntry>& entryList) :m_EntryList(entryList){}
		~GiveRewardInitVisitor(){ }

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			WSSortListRewardEntry rewardEntry;
			rewardEntry.playerId = entry.id;
			rewardEntry.ranking = ranking;
			rewardEntry.value = entry.value1;
			m_EntryList.push_back(rewardEntry);
			return true;
		}

	private:
		std::vector<WSSortListRewardEntry>& m_EntryList;
	};

	GiveRewardInitVisitor visitor(m_Entry);
	WSSortListMgr::Instance()->VisitSortList(SORTLIST_1V1_SEASON, visitor);

	m_Index = 0;
	m_LastGiveTime = now.Sec();

	m_Interval = intervalTime;
	//奖励发放在半个小时内完成
	m_SingleSize = ((m_Entry.size() / (Avalon::Time::SECS_OF_MIN * 30)) + 1) * m_Interval;

	m_SendName = sendName;
	m_Title = title;
	m_Content = content;
}

bool WSSortListGiveReward::GetEntry(UInt32 index, WSSortListRewardEntry& entry)
{
	if (index < m_Entry.size())
	{
		entry = m_Entry[m_Index];
		return true;
	}
	return false;
}

WSSortListMgr::WSSortListMgr()
{
	m_reopen1v1SeasonTime = 0;
	m_SaveTimer.SetInterval(1 * 60 * 1000);
}

WSSortListMgr::~WSSortListMgr()
{
}

void WSSortListMgr::Init()
{
	//noted by aprilliu,2017.4.21 Record Server会对查询结果进行分包处理，此处可以一次性拉取所有排行榜
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_sortlist");
	selectcmd->SetLimit("ORDER BY `update_time`");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectSortDataCallback());
}

void WSSortListMgr::Final()
{
	Save();
}

bool WSSortListMgr::ReloadBlackRoleList(std::string strs)
{
	std::vector<std::string> vecRoleIdList;
	Avalon::StringUtil::Split(strs, vecRoleIdList, "|");
	DebugLogStream << "reload black role list : " << strs << LogStream::eos;
	m_BlackRoleList.clear();
	for (auto str : vecRoleIdList)
	{
		ObjID_t roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(str);
		m_BlackRoleList.insert(roleId);
		DebugLogStream << "delete all entry : roid = " << roleId << LogStream::eos;
		DeleteAllEntry(roleId);
	}

	return true;
}

void WSSortListMgr::UpdateSortList(SortListEntry& entry)
{
	//TODO: 待修改
	if (IsPlayerSortList(entry.sortType) && IsInBlackRoleList(entry.id))
	{
		DebugLogStream << "entry.sortType : " << entry.sortType << "entry.id: " << entry.id << " is in black list, just return " << LogStream::eos;
		return;
	}

	if(entry.sortType >= SORTLIST_MAX) return;

	/*
	if (entry.sortType == SORTLIST_1V1_SEASON && !WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_1V1_SEASON))
	{
		// 说明没有开放赛季功能
		return;
	}
	*/

	//跨服不上榜
	if(IsPlayerSortList(entry.sortType))
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(entry.id);
		if(player != NULL && player->GetSrcZoneId() != 0) return;
	}
	else if (!IsGuildSortList(entry.sortType))
	{
		WSPlayer* owner = WSPlayerMgr::Instance()->FindNormalPlayer(entry.ownerId);
		if(owner != NULL && owner->GetSrcZoneId() != 0) return;
	}
	else if (IsAccountSortList(entry.sortType))
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid((UInt32)entry.id);
		if (player != NULL && player->GetSrcZoneId() != 0) return;
	}

	UInt64 oldMinValue = GetMinSortValue(entry.sortType);

	SortEntryList& list = m_SortEntrys[entry.sortType];
	SortEntryList::iterator iter = list.begin();
	while(iter != list.end())
	{
		if(iter->id == entry.id) break;
		++iter;
	}

	if(iter != list.end()) //在排行榜中, 如果排名或者表项信息发生变化 会在OnTick的时候调用Save()进行更新
	{
		iter->guildMemberNum = entry.guildMemberNum;

		if(iter->value1 == entry.value1 && iter->value2 == entry.value2 && iter->value3 == entry.value3)
		{
			return;
		}

		if(IsMaxValueSortList(entry.sortType) && entry.GetSortFactor() < iter->GetSortFactor())
		{
			return;
		}
		
		// 如果是武器榜, sex代表武器的装备类型，也需要保存
		if (IsEquipSortList(entry.sortType))
		{
			iter->sex = entry.sex;
		}

		iter->occu = entry.occu;
		iter->value1 = entry.value1;
		iter->value2 = entry.value2;
		iter->value3 = entry.value3;
		iter->guildMemberNum = entry.guildMemberNum;
		iter->updateTime = (UInt32)CURRENT_TIME.Sec();

		//if(SORTLIST_WEAPON == entry.sortType) iter->quality = entry.quality;
		iter->bDirty = true;

		bool bUpdate = false;
		if(iter != list.begin())
		{
			//如果之前不是第一 note by huchenhui
			SortEntryList::iterator preIter = iter;
			--preIter;
			//如果超过前一个玩家的排名 note by huchenhui
			if(iter->GetSortFactor() >= preIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if(!bUpdate)
		{
			SortEntryList::iterator nextIter = iter;
			++nextIter;
			//如果被后一个玩家超过   note by huchenhui
			if(nextIter != list.end() &&  iter->GetSortFactor() < nextIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}

		if(bUpdate)
		{
			SortListEntry sortEntry = *iter;
			list.erase(iter);
			AddToList(sortEntry);
		}
	}
	else  //不再排行榜中
	{
		//if(IsPlayerSortList(entry.sortType) && entry.name.empty())
		if (IsPlayerSortList(entry.sortType))
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(entry.id);
			if(player == NULL) return;
			
			entry.name = player->GetName();
			entry.level = player->GetLevel();
			entry.occu = player->GetOccu();
			entry.sex = player->GetSex();
			entry.vip = player->GetVipLvl();
		}
		else if(entry.ownerId != 0 && !IsGuildSortList(entry.sortType))
		{
			//非人物榜单. note by huchenhui
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(entry.ownerId);
			if(player == NULL) return;

			if(entry.occu == 0)
			{
				entry.occu = player->GetOccu();
			}
			entry.ownerId = player->GetID();		//如果是武器榜其实从scene server发来的entry已经含有ownerId
			entry.ownerAccid = player->GetAccId();
			entry.ownerName = player->GetName();
		}
		entry.updateTime = (UInt32)CURRENT_TIME.Sec();

		auto listSize = GetSortListSize(entry.sortType);
		if(listSize > 0 && list.size() >= listSize)		//榜单满了 note by huchenhui
		{
			SortEntryList::iterator lastIter = --list.end();
			if(entry.GetSortFactor() <= lastIter->GetSortFactor()) return;

			//挤掉最后一名 note by huchenhui
			DeleteEntry(entry.sortType, lastIter);
		}
	
		if(IsSortDataNeedSave(entry.sortType))
		{
			entry.key = CLRecordClient::Instance()->GenGuid();

			CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_sortlist",entry.key);
			insertCmd->PutData("id", entry.id);
			insertCmd->PutData("sorttype", entry.sortType);
			insertCmd->PutData("name", entry.name);
			insertCmd->PutData("quality", entry.quality);
			insertCmd->PutData("level", entry.level);
			insertCmd->PutData("occu", entry.occu);
			insertCmd->PutData("sex", entry.sex);
			insertCmd->PutData("vip", entry.vip);
			insertCmd->PutData("ownerid", entry.ownerId);
			insertCmd->PutData("owneraccid", entry.ownerAccid);
			insertCmd->PutData("ownername", entry.ownerName);
			insertCmd->PutData("value1", entry.value1);
			insertCmd->PutData("value2", entry.value2);
			insertCmd->PutData("value3", entry.value3);
			insertCmd->PutData("update_time", entry.updateTime);
			CLRecordClient::Instance()->SendCommand(insertCmd);
		}

		AddToList(entry);
		
		CLProtocol::SceneSyncSortListEntry sync;
		sync.sortType = entry.sortType;
		sync.flag = 1;
		sync.id = entry.id;
		Router::BroadcastToScene(sync);
	}

	UInt64 minValue = GetMinSortValue(entry.sortType);
	if(minValue != oldMinValue)
	{
		CLProtocol::SceneSyncSortListMinValue sync;
		sync.sortType = entry.sortType;
		sync.value = minValue;
		Router::BroadcastToScene(sync);
	}

	OnSortListChanged(entry.sortType);
}

void WSSortListMgr::UpdatePetName(ObjID_t id, const std::string& name)
{
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsPetSortList(i))
		{
			SortEntryList& list = m_SortEntrys[i];
			for(SortEntryList::iterator iter = list.begin();
				iter != list.end(); ++iter)
			{
				if(iter->id == id)
				{
					iter->name = name;
					iter->bDirty = true;
				}
			}
		}
	}
}

void WSSortListMgr::ListSortEntries(WSPlayer* player, UInt8 sortType, UInt8 occu, UInt16 start, UInt16 count)
{
	if(player == NULL || sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	if(count > 100) count = 100;	//最大100条
	if(start > list.size()) start = list.size();

	if(size_t(start + count) > list.size())
	{
		count = UInt16(list.size()) - start;
	}

	UInt16 totalNum = 0;
	UInt16 num = 0;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SORTLIST_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		UInt16 ranking = 0;
		stream & sortType;
		//stream & occu & start;
		stream & start;
		UInt8* pos1 = packet->GetBuffer() + stream.GetPos();
		stream & totalNum;
		UInt8* pos2 = packet->GetBuffer() + stream.GetPos();
		stream & num;

		//UInt64 lastSortFactor = 0;
		SortEntryList::iterator playerIter = list.end();
		UInt16 playerRanking = 0;

		for (SortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			if (occu != 0 && iter->occu != occu) continue;

			ranking++;

			if ((IsPlayerSortList(sortType) && player->GetID() == iter->id) ||
				(IsGuildSortList(sortType) && player->GetGuildID() == iter->id) ||
				(playerIter == list.end() && player->GetID() == iter->ownerId) ||
				(IsAccountSortList(sortType) && player->GetAccId() == (UInt32)iter->id))
			{
				//发送自身排行  by huchenhui
				playerIter = iter;
				playerRanking = ranking;
				
			}
			++totalNum;

			if(totalNum > start && totalNum <= start + count)
			{
				stream & ranking;
				iter->SerializeForClient(stream);
				++num;
			}
		}

		*((UInt16*)pos1) = totalNum;
		*((UInt16*)pos2) = num;

		//加上自己的排名
		if (playerIter != list.end())
		{
			stream & playerRanking;
			playerIter->SerializeForClient(stream);
		}
		stream & UInt16(0);

	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
}

void WSSortListMgr::ListSortEntriesNear(WSPlayer* player, UInt8 sortType, UInt32 count)
{
	if(sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	if(count > 10) count = 10;	//最大30条

	UInt16 ranking = GetRanking(sortType, player->GetID());

	UInt16 totalNum = 0;
	UInt16 num = 0;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SORTLIST_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & sortType;
		stream & ranking & UInt8(0) & UInt16(ranking > count + 1 ? ranking - count - 1 : 0);
		UInt8* pos1 = packet->GetBuffer() + stream.GetPos();
		stream & totalNum;
		UInt8* pos2 = packet->GetBuffer() + stream.GetPos();
		stream & num;

		for(SortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			++totalNum;

			if(ranking != 0 && totalNum + count >= ranking && totalNum <= ranking + count)
			{
				iter->SerializeForClient(stream);
				++num;
			}
		}
		*((UInt16*)pos1) = num;
		*((UInt16*)pos2) = num;
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
}

void WSSortListMgr::SyncPlayerEntry(WSPlayer* player, UInt8 sortType)
{
	if (sortType >= SORTLIST_MAX) return;

	SortEntryList list = m_SortEntrys[sortType];
	UInt16 ranking = 0;
	for (SortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		if (IsPlayerSortList(sortType))
		{
			ranking++;
			if (player->GetID() == iter->id)
			{
				SyncPlayerEntry(player, *iter, ranking);
			}
		}
	}
}

void WSSortListMgr::SyncPlayerEntry(WSPlayer* player, SortListEntry entry, UInt16 ranking)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SORTLIST_SELFINFO_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & ranking;
		entry.SerializeForClient(stream);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
}


void WSSortListMgr::DeleteEntry(UInt8 sortType, ObjID_t id)
{
	if(sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	SortEntryList::iterator iter = list.begin();
	while (iter != list.end())
	{
		SortEntryList::iterator tmpIter = iter;
		++iter;
		if ((IsPlayerSortList(sortType) || IsAccountSortList(sortType)) && tmpIter->id == id)
		{
			DeleteEntry(sortType, tmpIter);
			return;
		}
		else if (!IsPlayerSortList(sortType) && tmpIter->ownerId == id)
		{
			DeleteEntry(sortType, tmpIter);
		}
	}
}

void WSSortListMgr::DeleteEntryById(UInt8 sortType, ObjID_t itemId)
{
	if (sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	SortEntryList::iterator iter = list.begin();
	while (iter != list.end())
	{
		SortEntryList::iterator tmpIter = iter;
		++iter;
		if (tmpIter->id == itemId)
		{
			DeleteEntry(sortType, tmpIter);
			break;
		}
	}
}

void WSSortListMgr::DeleteAllEntry(ObjID_t id)
{
	for (int i = 1; i < SORTLIST_MAX; ++i)
	{
		DeleteEntry(i, id);
	}
}

void WSSortListMgr::WatchData(WSPlayer* player, UInt8 sortType, ObjID_t id)
{
	if(sortType >= SORTLIST_MAX) return;
	if(!IsPetSortList(sortType) && !IsEquipSortList(sortType)) return;

	SortListEntry* entry = FindEntry(sortType, id);
	if(entry == NULL) return;

	WSPlayer* owner = WSPlayerMgr::Instance()->FindNormalPlayer(entry->ownerId);
	if(owner != NULL || !ChatDataMgr::Instance()->IsExistData(id)) //noted by aprilliu: if owner was offline or the object which we wanted to check was not cached on world server,so world server would request to scene server
	{
		CLProtocol::SceneSortListWatchDataReq req;
		req.sortType = sortType;
		req.watcher = player->GetID();
		req.id = id;
		req.ownerId = entry->ownerId;
		req.ownerName = entry->ownerName;
		req.accId = entry->ownerAccid;
		if(owner != NULL)
		{
			owner->SendToScene(req);
		}
		else
		{
			UInt32 server = WSSceneMgr::Instance()->SelectRandSceneServer();
			if(server != 0)
			{
				Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(server);
				if(conn != NULL)
				{
					CL_SEND_PROTOCOL(conn, req);
				}
			}
		}
	}
	else
	{
		if(IsPetSortList(sortType))
		{
			ChatDataMgr::Instance()->SendLinkData(player, CHAT_LINKDATA_PET, id);
		}
		else if(IsEquipSortList(sortType))
		{
			ChatDataMgr::Instance()->SendLinkData(player, CHAT_LINKDATA_ITEM, id);
		}
	}
}

void WSSortListMgr::VisitSortList(UInt8 sortType, WSSortListVisitor& visitor)
{
	if(sortType >= SORTLIST_MAX) return;
	UInt32 ranking = 0;
	for(SortEntryList::iterator iter = m_SortEntrys[sortType].begin();
		iter != m_SortEntrys[sortType].end(); ++iter)
	{
		ranking++;
		if(!visitor(*iter, ranking)) return;
	}
}

UInt32 WSSortListMgr::GetRanking(UInt8 sortType, ObjID_t id) const
{
	if(sortType >= SORTLIST_MAX) return 0;
	UInt32 ranking = 0;
	for(SortEntryList::const_iterator iter = m_SortEntrys[sortType].begin(); iter != m_SortEntrys[sortType].end(); ++iter)
	{
		ranking++;
		if(iter->id == id) return ranking;
	}
	return 0;
}

void WSSortListMgr::ClearSortList(UInt8 sortType)
{
	if(sortType >= SORTLIST_MAX) return;

	/*
	if (sortType == SORTLIST_1V1_SEASON)
	{
		OnResetSeasonMatch();
	}
	*/

	if (m_SortEntrys[sortType].size() <= 0) return;

	if(IsSortDataNeedSave(sortType))
	{
		std::ostringstream oss;
		oss << "`sorttype` = '" << int(sortType) << "'";
		CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_sortlist", oss.str());
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	m_SortEntrys[sortType].clear();
}

void WSSortListMgr::OnSceneConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
	if(conn == NULL) return;
	
	for(UInt8 index = 1; index < SORTLIST_MAX; ++index)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::SCENE_SYNC_SORTLISTS);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			stream & index;
			stream & GetMinSortValue(index);
			SortEntryList& list = m_SortEntrys[index];
			stream & UInt16(list.size());
			for(SortEntryList::iterator iter = list.begin();
				iter != list.end(); ++iter)
			{
				stream & iter->id;
			}
		}
		catch(Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);
			return;
		}

		packet->SetSize(stream.GetPos());
		conn->SendPacket(packet);
		Avalon::Packet::Destroy(packet);
	}
}

void WSSortListMgr::OnTick(const Avalon::Time& now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		Save();
	}

	if (m_reopen1v1SeasonTime > 0 && now.MSec() >= m_reopen1v1SeasonTime)
	{
		m_reopen1v1SeasonTime = 0;
		WSServiceSwitchMgr::Instance()->OpenService(SERVICE_1V1_SEASON);
	}

	//if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_1V1_SEASON))
	//{
	//	GiveRewardMatchDaily(now);
	//}

	/*
	bool fuck = false;
	if (fuck)
	{
		ClearSortList(SORTLIST_1V1_SEASON);
	}
	*/
	


}

bool WSSortListMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	while(callback->NextRow())
	{
		SortListEntry entry;
		entry.key = callback->GetKey();
		entry.id = callback->GetData("id");
		entry.sortType = callback->GetData("sorttype");
		entry.name = callback->GetData("name").GetString();
		entry.quality = callback->GetData("quality");
		entry.level = callback->GetData("level");
		entry.occu = callback->GetData("occu");
		entry.sex = callback->GetData("sex");
		entry.vip = callback->GetData("vip");
		entry.ownerId = callback->GetData("ownerid");
		entry.ownerAccid = callback->GetData("owneraccid");
		entry.ownerName = callback->GetData("ownername").GetString();
		entry.value1 = callback->GetData("value1");
		entry.value2 = callback->GetData("value2");
		entry.value3 = callback->GetData("value3");
		entry.updateTime = callback->GetData("update_time");

		if (IsInBlackRoleList(entry.id))
		{
			CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_sortlist", entry.key);
			CLRecordClient::Instance()->SendCommand(deleteCmd);
			DebugLogStream << "black role list delete entry : roid = " << entry.id << " key = " << entry.key << LogStream::eos;
		}
		else
		{
			AddToList(entry);
		}
	}

	//删除多余的
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(!IsSortDataNeedSave(i)) continue;

		UInt32 maxSize = GetSortListSize(i);
		if(m_SortEntrys[i].size() <= maxSize || maxSize == 0) continue;

		SortEntryList::iterator delIter = m_SortEntrys[i].begin();
		std::advance(delIter, maxSize);

		for(SortEntryList::iterator iter = delIter; iter != m_SortEntrys[i].end(); ++iter)
		{
			CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_sortlist", iter->key);
			CLRecordClient::Instance()->SendCommand(deleteCmd);
		}
		m_SortEntrys[i].erase(delIter, m_SortEntrys[i].end());
	}

	return true;
}

void WSSortListMgr::OnDayChanged()
{
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsDailySortList(i))
		{
			ClearSortList(i);
		}
	}

	if (CheckGiveAdventureSortReward())
	{
		GiveAdventureSortRewards();
	}
}

void WSSortListMgr::OnWeekChanged()
{
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsWeeklySortList(i))
		{
			ClearSortList(i);
		}
	}
}

void WSSortListMgr::OnPlayerOnline(WSPlayer* player)
{
	if(player == NULL) return;

	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsPlayerSortList(i))
		{
			SortListEntry* entry = FindEntry(i, player->GetID());
			if(entry != NULL)
			{
				if(entry->vip != player->GetVipLvl())
				{
					entry->vip = player->GetVipLvl();
					entry->bDirty = true;
				}
				if(entry->level != player->GetLevel())
				{
					entry->level = player->GetLevel();
					entry->bDirty = true;
				}
			}
		}
	}
}

void WSSortListMgr::OnVipLvlChanged(WSPlayer* player)
{
	if(player == NULL) return;

	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsPlayerSortList(i))
		{
			SortListEntry* entry = FindEntry(i, player->GetID());
			if(entry != NULL && entry->vip != player->GetVipLvl())
			{
				entry->vip = player->GetVipLvl();
				entry->bDirty = true;
			}
		}
	}
}

void WSSortListMgr::OnPlayerLevelChanged(WSPlayer* player)
{
	if(player == NULL) return;

	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsPlayerSortList(i))
		{
			SortListEntry* entry = FindEntry(i, player->GetID());
			if(entry != NULL && entry->level != player->GetLevel())
			{
				entry->level = player->GetLevel();
				entry->bDirty = true;
			}
		}
	}
}

void WSSortListMgr::OnSortListChanged(UInt8 type)
{
	if(type >= SORTLIST_MAX) return;

}

void WSSortListMgr::OnNameChanged(ObjID_t playerId, const std::string& name)
{
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(IsPlayerSortList(i))
		{
			SortListEntry* entry = FindEntry(i, playerId);
			if(entry != NULL)
			{
				entry->name = name;
				entry->bDirty = true;
				Save(*entry);
			}
		}
		else
		{
			for(SortEntryList::iterator iter = m_SortEntrys[i].begin();
				iter != m_SortEntrys[i].end(); ++iter)
			{
				if(iter->ownerId == playerId)
				{
					iter->ownerName = name;
					iter->bDirty = true;
					Save(*iter);
				}
			}
		}
	}
}

void WSSortListMgr::OnOwnerChanged(SortListType sortType, ObjID_t entryId, ObjID_t playerId, const std::string& name)
{
	if (sortType > SORTLIST_MAX) return;
	if (!IsPlayerSortList(sortType) && !IsAccountSortList(sortType))
	{
		SortListEntry* entry = FindEntry((UInt8)sortType, entryId);
		if (entry != NULL)
		{
			entry->ownerId = playerId;
			entry->ownerName = name;
			entry->bDirty = true;
			Save(*entry);
		}
	}
}

void WSSortListMgr::OnOccuChanged(WSPlayer* player)
{
	if (player == NULL) return;
	for (int i = 1; i < SORTLIST_MAX; ++i)
	{
		if (IsPlayerSortList(i))
		{
			SortListEntry* entry = FindEntry(i, player->GetID());
			if (entry != NULL)
			{
				entry->occu = player->GetOccu();
				Save(*entry);
			}
		}
		else
		{
			for (SortEntryList::iterator iter = m_SortEntrys[i].begin(); iter != m_SortEntrys[i].end(); ++iter)
			{
				if (iter->ownerId == player->GetID())
				{
					iter->occu = player->GetOccu();
					Save(*iter);
				}
			}
		}
	}
}

void WSSortListMgr::OnGuildLeaderChanged(ObjID_t guildId, ObjID_t leaderId, std::string leader)
{
	for (UInt32 i = 1; i < SORTLIST_MAX; ++i)
	{
		if (IsGuildSortList(i))
		{
			SortListEntry* entry = FindEntry(i, guildId);
			if (entry != NULL && (entry->ownerId != leaderId || entry->ownerName != leader))
			{
				entry->ownerId = leaderId;
				entry->ownerName = leader;
				Save(*entry);
			}
		}
	}
}

void WSSortListMgr::OnGuildMemeberChanged(ObjID_t guildId, UInt32 memberCount)
{
	SortListEntry* entry = FindEntry(SORTLIST_GUILD_LEVEL, guildId);
	if (entry != NULL)
	{
		entry->guildMemberNum = memberCount;
		Save(*entry);
	}
}

void WSSortListMgr::OnGuildLevelChanged(ObjID_t guildId, UInt32 level)
{
	for (int i = 1; i < SORTLIST_MAX; ++i)
	{
		if (IsGuildSortList(i))
		{
			SortListEntry* entry = FindEntry(i, guildId);
			if (entry != NULL)
			{
				entry->level = level;
				Save(*entry);
			}
		}
	}
}

void WSSortListMgr::OnGuildNameChanged(ObjID_t guildId, std::string name)
{
	for (int i = 1; i < SORTLIST_MAX; ++i)
	{
		if (IsGuildSortList(i))
		{
			SortListEntry* entry = FindEntry(i, guildId);
			if (entry != NULL)
			{
				entry->name = name;
				Save(*entry);
			}
		}
	}
}

void WSSortListMgr::OnUpdateValue3(SortListType sortType, ObjID_t roleId, UInt32 value3)
{
	SortListEntry* entry = FindEntry(sortType, roleId);
	if (entry)
	{
		entry->value3 = value3;
		entry->bDirty = true;
		Save(*entry);
	}
}

void WSSortListMgr::OnAdventureTeamLevelChanged(SortListType sortType, ObjID_t uid, UInt16 level)
{
	SortListEntry* entry = FindEntry(sortType, uid);
	if (entry)
	{
		entry->level = level;
		entry->bDirty = true;
		Save(*entry);
	}
}

void WSSortListMgr::OnAdventureTeamNameChanged(SortListType sortType, ObjID_t uid, const std::string& name)
{
	SortListEntry* entry = FindEntry(sortType, uid);
	if (entry)
	{
		entry->ownerName = name;
		entry->bDirty = true;
		Save(*entry);
	}
}

void WSSortListMgr::OnAdventureTeamGradeChanged(SortListType sortType, ObjID_t uid, UInt8 grade)
{
	SortListEntry* entry = FindEntry(sortType, uid);
	if (entry)
	{
		if (entry->value2 == grade)
		{
			return;
		}
		entry->value2 = grade;
		entry->bDirty = true;
		Save(*entry);
	}
}

void WSSortListMgr::AddToList(SortListEntry& entry)
{
	if(entry.sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[entry.sortType];
	UInt64 sortFactor = entry.GetSortFactor();

	SortEntryList::iterator iter = list.begin();
	while(iter != list.end())
	{
		if(sortFactor > iter->GetSortFactor()) break;
		++iter;
	}

	list.insert(iter, entry);
}

bool WSSortListMgr::IsInBlackRoleList(ObjID_t id)
{
	if (m_BlackRoleList.find(id) != m_BlackRoleList.end())
		return true;
	return false;
}

UInt64 WSSortListMgr::GetMinSortValue(UInt8 sortType) const
{
	if(m_SortEntrys[sortType].empty()) return 0;
	return (--m_SortEntrys[sortType].end())->GetSortFactor();
}

void WSSortListMgr::DeleteEntry(UInt8 sortType, SortEntryList::iterator iter)
{
	/*
	CLProtocol::SceneSyncSortListEntry sync;
	sync.sortType = sortType;
	sync.flag = 0;
	sync.id = iter->id;
	Router::BroadcastToScene(sync);
	*/

	if(IsSortDataNeedSave(sortType))
	{
		CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_sortlist", iter->key);
		CLRecordClient::Instance()->SendCommand(deleteCmd);
	}
	m_SortEntrys[sortType].erase(iter);
}

SortListEntry* WSSortListMgr::FindEntry(UInt8 sortType, ObjID_t id)
{
	if(sortType >= SORTLIST_MAX) return NULL;

	SortEntryList& list = m_SortEntrys[sortType];
	SortEntryList::iterator iter = list.begin();
	while(iter != list.end())
	{
		if(iter->id == id) break;
		++iter;
	}

	if(iter == list.end()) return NULL;
	return &(*iter);
}

void WSSortListMgr::Save()
{
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(!IsSortDataNeedSave(i)) continue;

		for(SortEntryList::iterator iter = m_SortEntrys[i].begin();
			iter != m_SortEntrys[i].end(); ++iter)
		{
			if(iter->bDirty)
			{
				Save(*iter);
			}
		}
	}
}

void WSSortListMgr::Save(SortListEntry& entry)
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_sortlist", entry.key, true);
	updateCmd->PutData("name", entry.name);
	updateCmd->PutData("vip", entry.vip);
	updateCmd->PutData("level", entry.level);
	updateCmd->PutData("ownerid", entry.ownerId);
	updateCmd->PutData("ownername", entry.ownerName);

	// 如果是武器榜, sex代表武器的装备类型，也需要保存
	if (IsEquipSortList(entry.sortType))
	{
		updateCmd->PutData("sex", entry.sex);
	}

	updateCmd->PutData("occu", entry.occu);
	updateCmd->PutData("value1", entry.value1);
	updateCmd->PutData("value2", entry.value2);
	updateCmd->PutData("value3", entry.value3);
	updateCmd->PutData("update_time", entry.updateTime);
	CLRecordClient::Instance()->SendCommand(updateCmd);
	entry.bDirty = false;
}

void WSSortListMgr::TestGiveReward()
{
	char buffer[1024] = { 0 };
	// 发送者
	SysNotifyMgr::Instance()->MakeNotify(2008, buffer, sizeof(buffer));
	std::string	strSendName(buffer);

	//标题
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(2009, buffer, sizeof(buffer));
	std::string	strTitle(buffer);

	//内容
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(2010, buffer, sizeof(buffer));
	std::string	strContent(buffer);
	std::string strReason = GetReason(SOURCE_TYPE_SEASON);

	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand(0, "t_player_info");
	selectcmd->PutColumn("match_score");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectMatchScoreCallback(strSendName, strTitle, strContent, strReason));
}

void WSSortListMgr::OnResetSeasonMatch()
{
	// 赛季重置
	// 计算所有玩家重置后的积分，并且根据拍卖发放奖励

	class ResetSeasonMatchVisitor : public WSSortListVisitor
	{
	public:
		ResetSeasonMatchVisitor(UInt32 totalRank){ m_totalRank = totalRank; }

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			auto roleId = entry.id;

			// 重置积分
			/*
			//noted by huchenhui
			auto newScore = WSSortListMgr::GetResetMatchScore(entry.value1);
			CLProtocol::WorldChangeMatchScore resetScore;
			resetScore.roleId = roleId;
			resetScore.matchScore = newScore;
			
			auto player = WSPlayerMgr::Instance()->FindPlayer(roleId);
			if (player)
			{
				player->SendToScene(resetScore);
			}
			else
			{
				CLUpdateCommand	*cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleId, true);
				cmd->PutData("match_score", newScore);
				CLRecordClient::Instance()->SendCommand(cmd);
			}
			*/

			// 发放奖励
			WSSortListMgr::GrantSeasonMatchAward(roleId, ranking, m_totalRank);
			return true;
		}

		UInt32 m_totalRank;
	};

	// 关闭匹配功能
	WSServiceSwitchMgr::Instance()->CloseService(SERVICE_1V1_SEASON);
	const UInt32 seasonCloseMs = 1 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC;
	m_reopen1v1SeasonTime = Avalon::Time::CurrentTime().MSec() + seasonCloseMs;

	//奖励发放由赛季管理通知Scene,由scene发放
	//ResetSeasonMatchVisitor vistor(m_SortEntrys[SORTLIST_1V1_SEASON].size());
	//this->VisitSortList(SORTLIST_1V1_SEASON, vistor);
	
	//重置积分 by huchenhui
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand(0, "t_player_info");
	selectcmd->PutColumn("match_score");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectResetSeasonCallback());

}

void WSSortListMgr::GiveRewardMatchDaily(const Avalon::Time& now)
{
	//每日奖励21点发
	UInt32 dailyRewardTime = GameParamMgr::Instance()->GetValue("sortlist_daily_reward_time");
	if (now.Sec() >= dailyRewardTime + DAY_TO_SEC)
	{
		Avalon::Date date(now);
		date.Hour(21);
		date.Min(0);
		date.Sec(0);
		dailyRewardTime = UInt32(date.ToTime().Sec());
		GameParamMgr::Instance()->SetValue("sortlist_daily_reward_time", dailyRewardTime);

		if (m_1v1DailyReward.IsLoadMailInfo())
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));

			// 发送者
			SysNotifyMgr::Instance()->MakeNotify(2008, buffer, sizeof(buffer));
			std::string	strSendName(buffer);

			//标题
			memset(buffer, 0, sizeof(buffer));
			SysNotifyMgr::Instance()->MakeNotify(2009, buffer, sizeof(buffer));
			std::string	strTitle(buffer);

			//内容
			memset(buffer, 0, sizeof(buffer));
			SysNotifyMgr::Instance()->MakeNotify(2010, buffer, sizeof(buffer));
			std::string	strContent(buffer);

			std::string strReason = GetReason(SOURCE_TYPE_SEASON);

			m_1v1DailyReward.Init(SORTLIST_1V1_SEASON, now, 1, strSendName, strTitle, strContent, strReason);
		}
		else
		{
			m_1v1DailyReward.Init(SORTLIST_1V1_SEASON, now, 1);
		}
	}

	UInt32 offsetTime = now.Sec() - m_1v1DailyReward.GetLastGiveTime();
	if (offsetTime > m_1v1DailyReward.GetInterval())
	{
		m_1v1DailyReward.SetLastGiveTime(now.Sec());
		UInt32 singleSize = m_1v1DailyReward.GetSingleSize();
		UInt32 count = 0;
		while (m_1v1DailyReward.IsContinue() && count<singleSize)
		{
			WSSortListRewardEntry entry;
			if (m_1v1DailyReward.GetEntry(entry))
			{
				UInt64 roleId = entry.playerId;
				std::vector<ItemReward> itemReward = SeasonLevelDataEntryMgr::Instance()->GetDailyItemRewardVec(entry.value);
				//std::vector<ItemReward> itemReward = SeasonDailyDataEntryMgr::Instance()->GetItemRewardVec(entry.value);
				MailBox::SendSysMail(roleId, m_1v1DailyReward.GetSendName(), m_1v1DailyReward.GetTitle(), m_1v1DailyReward.GetContent(), m_1v1DailyReward.GetReason(), itemReward, MAIL_VALID_DAY);
			}

			count++;
			m_1v1DailyReward.IncIndex();
		}
	}

	//CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand(0, "t_player_info");
	//selectcmd->PutColumn("match_score");
	//CLRecordClient::Instance()->SendCommand(selectcmd, new SelectMatchScoreCallback(strSendName, strTitle, strContent));
}

UInt32 WSSortListMgr::GetResetMatchScore(UInt32 oldScore)
{
	return 1000 + (UInt32)round((oldScore - 1000) * 0.35f);
}

void WSSortListMgr::GrantSeasonMatchAward(ObjID_t roleId, UInt32 rank, UInt32 totalRank)
{
	class SeasonRewardVistor : public Avalon::DataEntryVisitor<SeasonRewardDataEntry>
	{
	public:
		SeasonRewardVistor(ObjID_t roleId, UInt32 rank) {}
		bool operator()(SeasonRewardDataEntry *data)
		{
			if (!data)
			{
				return true;
			}

			if ((data->startRank <= m_rank && data->endRank >= m_rank) || (data->endRank == 0))
			{
				m_rewards = data->rewards;
			}

			return true;
		}
	public:
		ObjID_t				m_roleId;
		UInt32				m_rank;
		ItemRewardVec		m_rewards;
	};

	SeasonRewardVistor vistor(roleId, rank);
	SeasonRewardDataEntryMgr::Instance()->Visit(vistor);
	if (vistor.m_rewards.empty())
	{
		return;
	}

	std::string strTitle = "system";
	std::string strContent = "season";
	std::string senderName = "system";
	std::vector<WSItem*> vecItems;
	std::string reason = GetReason(SOURCE_TYPE_SEASON);
	if (MailBox::SendSysMail(roleId, senderName, strTitle, strContent, reason, vistor.m_rewards, vecItems) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "send season reward to player(" << roleId << ") failed, rank(" << rank << ")." << LogStream::eos;
	}
}

std::vector<UInt64> WSSortListMgr::GetSeasonPlayerIds(UInt32 size)
{
	std::vector<UInt64> result;
	UInt32 ranking = 0;
	for (SortEntryList::iterator iter = m_SortEntrys[SortListType::SORTLIST_1V1_SEASON].begin();
		iter != m_SortEntrys[SortListType::SORTLIST_1V1_SEASON].end(); ++iter)
	{
		result.push_back(iter->id);
		ranking++;
		if (ranking >= size) break;
	}
	return result;
}

void WSSortListMgr::OnNewYearRedPacketEnd()
{
	if (m_SortEntrys[SortListType::SORTLIST_NEW_YEAR_RED_PACKET].size() == 0)
	{
		InfoLogStream << "OnNewYearRedPacketEnd is empty" << LogStream::eos;
		return;
	}

	UInt32 titleNeedNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_NEW_YEAR_RED_PACKET_TITLE_NEED_NUM);

	SortEntryList::iterator iter = m_SortEntrys[SortListType::SORTLIST_NEW_YEAR_RED_PACKET].begin();
	if (iter->value1 >= titleNeedNum)
	{
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(9303, buffer, sizeof(buffer));
		std::string	strSendName(buffer);

		//标题
		memset(buffer, 0, sizeof(buffer));
		SysNotifyMgr::Instance()->MakeNotify(9304, buffer, sizeof(buffer));
		std::string	strTitle(buffer);

		//内容
		memset(buffer, 0, sizeof(buffer));
		SysNotifyMgr::Instance()->MakeNotify(9305, buffer, sizeof(buffer));
		std::string	strContent(buffer);

		std::string strReason = GetReason(SOURCE_TYPE_RED_PACKET);

		UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_NEW_YEAR_RED_PACKET_TITLE_ID);

		std::vector<ItemReward> items;
		items.push_back(ItemReward(itemId, 1, 0, 0));

		MailBox::SendSysMail(iter->id, strSendName, strTitle, strContent, strReason , items, MAIL_VALID_DAY);
	}
}

bool WSSortListMgr::CheckGiveAdventureSortReward()
{
	UInt32 settleTime = GameParamMgr::Instance()->GetValue(PARAM_SORTLIST_ADVENTURE_SETTLE_TIME);
	if (settleTime == 0)
	{
		//计算下月1号6点时间戳
		GameParamMgr::Instance()->SetValue(PARAM_SORTLIST_ADVENTURE_SETTLE_TIME, (UInt32)getNextMonthLogicBeginTime(CURRENT_TIME.MSec()));
		return false;
	}
	else
	{
		UInt64 curTime = CURRENT_TIME.MSec();
		if (curTime < (UInt64)settleTime * 1000)
		{
			return false;
		}
		GameParamMgr::Instance()->SetValue(PARAM_SORTLIST_ADVENTURE_SETTLE_TIME, (UInt32)getNextMonthLogicBeginTime(curTime));
		return true;
	}
}

class SortListGiveRewardCallback : public AccountCallback
{
public:
	SortListGiveRewardCallback(UInt32 accid, UInt32 titleId, UInt32 dueTime)
		: AccountCallback(accid), m_titleId(titleId), m_dueTime(dueTime) {}
	~SortListGiveRewardCallback(){}
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

void WSSortListMgr::GiveAdventureSortRewards()
{
	class Visitor : public WSSortListVisitor
	{
	public:
		Visitor(UInt32 dueTime) : m_dueTime(dueTime)
		{}
		virtual bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			if (ranking > 10)
			{
				return false;
			}

			if (entry.value2 < 3)
			{
				return true;
			}

			UInt32 newTitleId = NewTitleItemDataEntryMgr::Instance()->GetYbtTitleIdForSortList(ranking);
			if (newTitleId == 0)
			{
				return true;
			}
			AccountInfo* accountInfo = AccountMgr::Instance()->FindAccount(entry.id);
			if (accountInfo)
			{
				accountInfo->GetTitleMgr().AddOneTitleByTitleId(newTitleId, m_dueTime);
			}
			else
			{	
				AccountMgr::Instance()->LoadAccountInfo(entry.id, new SortListGiveRewardCallback(entry.id, newTitleId, m_dueTime));
			}
			return true;
		}
		UInt32 m_dueTime;
	};
	UInt32 dueTime = GameParamMgr::Instance()->GetValue(PARAM_SORTLIST_ADVENTURE_SETTLE_TIME);
	Visitor visitor(dueTime);

	VisitSortList(SORTLIST_ADVENTURE_TEAM_GRADE, visitor);
}