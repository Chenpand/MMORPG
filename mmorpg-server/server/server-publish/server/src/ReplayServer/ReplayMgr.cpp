#include "ReplayMgr.h"
#include <CLRecordClient.h>
#include <AvalonMd5.h>
#include <AvalonCompress.h>
#include <AvalonBase64.h>
#include <CLSystemValueDataEntry.h>
#include "RSApplication.h"

ReplayMgr::ReplayMgr()
{

}

ReplayMgr::~ReplayMgr()
{
	Final();
}

bool ReplayMgr::Init()
{
	auto curDate = CURRENT_DATE;
	curDate.Min(0);
	curDate.Sec(0);
	curDate.MSec(0);

	UInt64 updateTime = curDate.ToTime().MSec() + Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC;

	m_MasterListTimer.SetInterval(updateTime - CURRENT_TIME.MSec());

	m_ReplayInfoTickTimer.SetInterval(10 * Avalon::Time::MSECS_OF_SEC);

	m_ReplayListTickTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);

	LoadReplayList(REPLAY_LIST_TYPE_MASTER, 0, NULL);
	LoadReplayList(REPLAY_LIST_TYPE_MASTER_WAIT, 0, NULL);

	return true;
}

void ReplayMgr::Final()
{

}

void ReplayMgr::OnTick(const Avalon::Time& now)
{
	if (m_ReplayInfoTickTimer.IsTimeout(now))
	{
		// 检测引用计数为2的
		std::vector<ReplayInfoPtr> needDelInfoes;
		for (auto info : m_ReplayInfoes)
		{
			// 这里有两个引用，一个是m_ReplayInfoes，一个是迭代器，所以如果是2的话，说明已经没有其它引用了
			if (info.second.use_count() == 2)
			{
				needDelInfoes.push_back(info.second);
			}
		}

		for (auto info : needDelInfoes)
		{
			m_ReplayInfoes.erase(info->raceId);
		}
	}

	if (m_ReplayListTickTimer.IsTimeout(now))
	{
		std::vector<ReplayList*> needDelLists;
		for (int i = 0; i < REPLAY_LIST_NUM; i++)
		{
			auto& lists = m_ReplayLists[i];
			for (auto& itr : lists)
			{
				auto replayList = itr.second;
				if (replayList && replayList->GetRemoveTime() > 0 && replayList->GetRemoveTime() <= now.Sec())
				{
					needDelLists.push_back(replayList);
				}
			}
		}

		for (auto replayList : needDelLists)
		{
			DelReplayList(replayList->GetType(), replayList->GetOwnerID());
		}
	}

	if (m_MasterListTimer.IsTimeout(now))
	{
		UpgradeMasterWaitList();

		m_MasterListTimer.SetInterval(Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC);
	}

}

void ReplayMgr::LoadReplayList(ReplayListType type, ObjID_t ownerid, ReplayListCallback* cb)
{
	auto replayList = FindReplayList(type, ownerid);
	if (replayList)
	{
		if (replayList->GetStatus() == REPLAY_LIST_STATUS_FINISH && cb != NULL)
		{
			cb->OnFinish(replayList);
		}
		else
		{
			replayList->AddLoadCallback(cb);
		}
		
		return;
	}

	replayList = new ReplayList(ownerid, type);
	replayList->StartLoad(cb);
	AddReplayList(replayList);
}

ReplayList* ReplayMgr::FindReplayList(ReplayListType type, ObjID_t ownerid)
{
	if (type <= REPLAY_LIST_TYPE_INVALID || type >= REPLAY_LIST_NUM)
	{
		return NULL;
	}

	auto& lists = m_ReplayLists[type];
	auto itr = lists.find(ownerid);
	if (itr == lists.end())
	{
		return NULL;
	}

	return itr->second;
}

void ReplayMgr::AddReplayList(ReplayList* list)
{
	if (!list)
	{
		return;
	}

	m_ReplayLists[list->GetType()][list->GetOwnerID()] = list;
}

void ReplayMgr::DelayDelReplayList(ReplayListType type, ObjID_t id, UInt64 delaySec)
{
	if (type <= REPLAY_LIST_TYPE_INVALID || type >= REPLAY_LIST_NUM)
	{
		return;
	}

	auto& lists = m_ReplayLists[type];
	auto itr = lists.find(id);
	if (itr == lists.end())
	{
		return;
	}

	auto replayList = itr->second;
	replayList->SetRemoveTime(CURRENT_TIME.Sec() + delaySec);
}

void ReplayMgr::DelReplayList(ReplayListType type, ObjID_t id)
{
	if (type <= REPLAY_LIST_TYPE_INVALID || type >= REPLAY_LIST_NUM)
	{
		return;
	}

	auto& lists = m_ReplayLists[type];
	auto itr = lists.find(id);
	if (itr == lists.end())
	{
		return;
	}

	CL_SAFE_DELETE(itr->second);
	lists.erase(itr);
}

void ReplayMgr::InsertReplayInfoToDB(ReplayInfoPtr info)
{
	if (!info)
	{
		return;
	}

	{
		info->guid = CLRecordClient::Instance()->GenGuid();
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_replay_info", info->guid);
		cmd->PutData("race_id", info->raceId);
		cmd->PutData("version", info->version);
		cmd->PutData("record_time", info->recordTime);
		cmd->PutData("evaluation", info->evaluation);
		cmd->PutData("result", info->result);
		cmd->PutData("score", info->score);
		cmd->PutData("view_num", info->viewNum);
		cmd->PutData("ref_num", info->dbRefNum);
		cmd->PutData("race_type", info->type);
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	for (auto& fighter : info->fighters)
	{
		fighter.guid = CLRecordClient::Instance()->GenGuid();
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_replay_fighter", fighter.guid);
		cmd->PutData("raceid", info->raceId);
		cmd->PutData("roleid", fighter.roleId);
		cmd->PutData("name", fighter.name);
		cmd->PutData("level", fighter.level);
		cmd->PutData("occu", fighter.occu);
		cmd->PutData("season_level", fighter.seasonLevel);
		cmd->PutData("season_stars", fighter.seasonStars);
		cmd->PutData("pos", fighter.pos);
		CLRecordClient::Instance()->SendCommand(cmd, NULL, fighter.roleId);
	}
}

void ReplayMgr::UpdateReplayInfoToDB(ReplayInfoPtr info)
{
	if (!info)
	{
		return;
	}

	auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_replay_info", info->guid, false);
	cmd->PutData("view_num", info->viewNum);
	cmd->PutData("ref_num", info->dbRefNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

ReplayFile* ReplayMgr::FindReplayFile(ObjID_t raceid)
{
	auto itr = m_ReplayFiles.find(raceid);
	return itr == m_ReplayFiles.end() ? NULL : itr->second;
}

void ReplayMgr::DelReplayFile(ObjID_t raceid)
{
	auto itr = m_ReplayFiles.find(raceid);
	if (itr == m_ReplayFiles.end())
	{
		return;
	}

	CL_SAFE_DELETE(itr->second);
	m_ReplayFiles.erase(itr);
}

bool ReplayMgr::InsertReplayFileToDB(ReplayFile* replay)
{
	if (!replay)
	{
		return false;
	}

	// 是否已经插入到数据库
	if (replay->addedToDB)
	{
		return true;
	}

	replay->addedToDB = true;
	auto conn = RSApplication::Instance()->GetDBConn();
	if (conn == NULL)
	{
		FatalLogStream << "insert replay(" << replay->header.sessionId << ") failed, db connection is null." << LogStream::eos;
		return false;
	}

	static UInt8 buffer[1024 * 1024];
	Avalon::NetOutputStream stream(buffer, sizeof(buffer));
	try
	{
		stream & *replay;
	}
	catch (Avalon::NetSerializeException& e)
	{
		FatalLogStream << "encode replay(" << replay->header.sessionId << ") failed, " << e.what() << LogStream::eos;
		return false;
	}

	static UInt8 dstBuffer[1024 * 1024];
	size_t dstLen = sizeof(dstBuffer);
	Avalon::snappy_compress((char*)dstBuffer, &dstLen, (const char*)buffer, stream.GetPos());

	static char STRING_BUF[1024 * 1024];
	//size_t size = Avalon::base64_encode(STRING_BUF, sizeof(STRING_BUF), (const char*)dstBuffer, dstLen);
	size_t size = conn->EscapeString(STRING_BUF, (const char*)dstBuffer, dstLen);
	STRING_BUF[size] = 0;

	std::ostringstream ss;
	ss << "insert into t_replay_file values(" << replay->header.sessionId << ", " << "\'" << STRING_BUF << "\');";

	auto result = conn->Update(ss.str());
	if (result < 0)
	{
		ErrorLogStream << "insert replay(" << replay->header.sessionId << "):" << conn->GetError() << LogStream::eos;
		return false;
	}
	
	char md5[33];
	Avalon::md5_encode32(md5, dstBuffer, dstLen);
	md5[32] = '\0';
	InfoLogStream << "insert replay(" << replay->header.sessionId << ")'s file, md5(" << md5 << ")." << LogStream::eos;

	return true;
}

bool ReplayMgr::CanJoinMasterWaitList(UInt32 score)
{
	UInt32 lowerLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_REPLAY_MASTER_SCORE_LOWER_LIMIT);
	if (lowerLimit == 0)
	{
		ErrorLogStream << "Master wait replay list limit score(" << lowerLimit << ")" << LogStream::eos;
		lowerLimit = 1000;
	}
	UInt32 upperLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_REPLAY_MASTER_SCORE_UPPER_LIMIT);
	if (upperLimit == 0)
	{
		ErrorLogStream << "Master wait replay list score upper limit(" << upperLimit << ")!" << LogStream::eos;
		upperLimit = 100000;
	}

	if (score < lowerLimit || score > upperLimit)
	{
		DebugLogStream << "Can not add to wait list(score=" << score << "), score was reach limit(" << lowerLimit 
			<< ", " << upperLimit << ")!" << LogStream::eos;
		return false;
	}

	auto replayList = FindReplayList(REPLAY_LIST_TYPE_MASTER_WAIT, 0);
	if (!replayList)
	{
		DebugLogStream << "Can add to wait list(score=" << score << "), replay list was null!" << LogStream::eos;
		return true;
	}

	if (replayList->GetEntrys().size() < replayList->GetMaxEntryNum())
	{
		DebugLogStream << "Can add to wait list(score=" << score << "), replay list size was not full!" << LogStream::eos;
		return true;
	}

	for (auto& entry : replayList->GetEntrys())
	{
		if (!entry || !entry->info)
		{
			replayList->DelEntry(entry);
			DebugLogStream << "Can add to wait list(score=" << score << "), one entry was null!" << LogStream::eos;
			return true;
		}

		if (score > entry->info->score)
		{
			DebugLogStream << "Can add to wait list(score=" << score << "), one entry was low(raceid=" << entry->raceId 
				<< ", score=" << entry->info->score << ")!" << LogStream::eos;
			return true;
		}
	}

	DebugLogStream << "Can not add to wait list(score=" << score << ")!" << LogStream::eos;
	return false;
}

bool ReplayMgr::AddToMasterWaitList(ReplayInfoPtr info, ReplayFile* file)
{
	if (!info)
	{
		CL_SAFE_DELETE(file);
		return false;
	}

	InfoLogStream << "start to add master wait replay(" << info->raceId << ", score=" << info->score << ")" << LogStream::eos;

	auto replayList = FindReplayList(REPLAY_LIST_TYPE_MASTER_WAIT, 0);
	if (!replayList)
	{
		replayList = new ReplayList(0, REPLAY_LIST_TYPE_MASTER_WAIT);
		AddReplayList(replayList);
	}

	auto newEntry = new ReplayListEntry(REPLAY_LIST_TYPE_MASTER_WAIT);
	newEntry->guid = CLRecordClient::Instance()->GenGuid();
	newEntry->ownerid = 0;
	newEntry->raceId = info->raceId;

	// 找到评分较低的那场，并删除
	if (replayList->GetEntrys().size() == replayList->GetMaxEntryNum())
	{
		ReplayListEntry* lowEntry = NULL;
		for (auto entry : replayList->GetEntrys())
		{
			if (!entry || !entry->info)
			{
				break;
			}

			if (!lowEntry || entry->info->score < lowEntry->info->score)
			{
				lowEntry = entry;
			}
		}

		if (lowEntry)
		{
			InfoLogStream << "Delete low replay(" << lowEntry->raceId << ", score=" << lowEntry->info->score << ")." << LogStream::eos;

			lowEntry->Del();
			DelReplayFile(lowEntry->raceId);
		}

		replayList->DelEntry(lowEntry);
	}
	
	newEntry->SetReplayInfo(info);
	newEntry->SaveToDB();
	replayList->AddEntry(newEntry);

	if (m_ReplayFiles.find(info->raceId) != m_ReplayFiles.end())
	{
		ErrorLogStream << "Add the same replay file(" << info->raceId << ", type=" << info->type << ", score=" << info->score << ")." << LogStream::eos;
		return false;
	}

	m_ReplayFiles[info->raceId] = file;

	InfoLogStream << "Success add replay(" << info->raceId << ", type=" << info->type << ", score=" << info->score << ")." << LogStream::eos;
	return true;
}

bool ReplayMgr::AddToPremiumLeagueBattleList(ReplayInfoPtr info, ReplayFile* file)
{
	if (!info)
	{
		CL_SAFE_DELETE(file);
		return false;
	}

	auto replayList = FindReplayList(REPLAY_LIST_TYPE_PREMIUM_LEAGUE_BATTLE, 0);
	if (!replayList)
	{
		replayList = new ReplayList(0, REPLAY_LIST_TYPE_PREMIUM_LEAGUE_BATTLE);
		AddReplayList(replayList);
	}

	auto newEntry = new ReplayListEntry(REPLAY_LIST_TYPE_PREMIUM_LEAGUE_BATTLE);
	newEntry->guid = CLRecordClient::Instance()->GenGuid();
	newEntry->ownerid = 0;
	newEntry->raceId = info->raceId;
	newEntry->SetReplayInfo(info);
	newEntry->SaveToDB();
	replayList->AddEntry(newEntry);

	if (m_ReplayFiles.find(info->raceId) == m_ReplayFiles.end())
	{
		m_ReplayFiles[info->raceId] = file;
	}

	if (file && !file->addedToDB)
	{
		InsertReplayFileToDB(file);
	}

	return true;
}

void ReplayMgr::UpgradeMasterWaitList()
{
	auto waitList = FindReplayList(REPLAY_LIST_TYPE_MASTER_WAIT, 0);
	if (!waitList)
	{
		FatalLogStream << "Can not find replay list(" << (UInt32)REPLAY_LIST_TYPE_MASTER_WAIT << ")!" << LogStream::eos;
		return;
	}

	auto masterList = FindReplayList(REPLAY_LIST_TYPE_MASTER, 0);
	if (!masterList)
	{
		masterList = new ReplayList(0, REPLAY_LIST_TYPE_MASTER);
		masterList->SetStatus(REPLAY_LIST_STATUS_FINISH);
		AddReplayList(masterList);
	}

	std::vector<ReplayListEntry*> waitEntrys;
	for (auto entry : waitList->GetEntrys())
	{
		if (!entry || !entry->info)
		{
			continue;
		}

		auto replayFile = FindReplayFile(entry->raceId);
		if (!replayFile)
		{
			FatalLogStream << "upgrade race(" << entry->raceId << ") to master list, replay file is miss..." << LogStream::eos;
			waitEntrys.push_back(entry);
			continue;
		}

		if (!InsertReplayFileToDB(replayFile))
		{
			FatalLogStream << "upgrade race(" << entry->raceId << ") to master list, insert replay file to db failed..." << LogStream::eos;
			continue;
		}

		DelReplayFile(entry->raceId);

		auto newEntry = new ReplayListEntry(REPLAY_LIST_TYPE_MASTER);
		newEntry->guid = CLRecordClient::Instance()->GenGuid();
		newEntry->raceId = entry->raceId;
		
		newEntry->SetReplayInfo(entry->info);
		newEntry->SaveToDB();
		masterList->AddEntry(newEntry);

		waitEntrys.push_back(entry);

		InfoLogStream << "Success add replay(" << newEntry->raceId << ")!" << LogStream::eos;
	}

	for (auto entry : waitEntrys)
	{
		entry->Del();
		waitList->DelEntry(entry);
	}
}

ReplayInfoPtr ReplayMgr::FindReplayInfo(ObjID_t raceid)
{
	auto itr = m_ReplayInfoes.find(raceid);
	return itr == m_ReplayInfoes.end() ? NULL : itr->second;
}

bool ReplayMgr::AddReplayInfo(ReplayInfoPtr info)
{
	if (!info)
	{
		return false;
	}

	ObjID_t raceid = info->raceId;
	if (FindReplayInfo(raceid))
	{
		return false;
	}

	m_ReplayInfoes[raceid] = info;
	return true;
}

void ReplayMgr::AddToReplayBuffer(ObjID_t raceid, UInt8* buffer)
{
	if (!buffer)
	{
		return;
	}

	DelReplayFile(raceid);
	m_ReplayBuffer[raceid] = buffer;
}

UInt8* ReplayMgr::GetReplayBuffer(ObjID_t raceid)
{
	auto itr = m_ReplayBuffer.find(raceid);
	return itr == m_ReplayBuffer.end() ? NULL : itr->second;
}

void ReplayMgr::DelReplayBuffer(ObjID_t raceid)
{
	auto buffer = GetReplayBuffer(raceid);
	if (!buffer)
	{
		return;
	}

	CL_SAFE_DELETE(buffer);
	m_ReplayBuffer.erase(raceid);
}