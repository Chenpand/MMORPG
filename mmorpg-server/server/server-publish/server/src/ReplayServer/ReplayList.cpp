#include "ReplayList.h"
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include "ReplayMgr.h"
#include "RSApplication.h"

class ILoadReplayListCallback : public CLRecordCallback
{
public:
	ILoadReplayListCallback(ReplayListType type, ObjID_t owner) : m_Type(type), m_Owner(owner) {}

	virtual void OnFinished()
	{
		auto replayList = GetList();
		if (!replayList)
		{
			return;
		}

		OnSuccess(replayList);
		replayList->DecDBQueryNum();
		if (replayList->IsLoadFinish())
		{
			replayList->OnLoadFinish();
		}
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		auto replayList = GetList();
		OnFailure(replayList);
		ReplayMgr::Instance()->DelReplayList(m_Type, m_Owner);
	}

	virtual void OnSuccess(ReplayList* replayList) {}
	virtual void OnFailure(ReplayList* replayList) {}

	ReplayList* GetList()
	{
		auto replayList = ReplayMgr::Instance()->FindReplayList(m_Type, m_Owner);
		if (!replayList)
		{
			FatalLogStream << "load replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ") callback, can't find list..." << LogStream::eos;
			return NULL;
		}

		// 加载好了
		if (replayList->GetStatus() == REPLAY_LIST_STATUS_FINISH)
		{
			return NULL;
		}

		return replayList;
	}

protected:
	ReplayListType			m_Type;
	ObjID_t					m_Owner;
};

class LoadReplayFighterCallback : public ILoadReplayListCallback
{
public:
	LoadReplayFighterCallback(ReplayListType type, ObjID_t owner, ReplayInfoPtr replayInfo) : ILoadReplayListCallback(type, owner), m_ReplayInfo(replayInfo) {}

	virtual void OnSuccess(ReplayList* replayList)
	{
		if (!m_ReplayInfo)
		{
			return;
		}

		ObjID_t raceid = m_ReplayInfo->raceId;
		auto entry = replayList->FindEntry(raceid);
		if (!entry)
		{
			ErrorLogStream << "replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ") can't find entry(" << raceid << ")." << LogStream::eos;
			return;
		}

		auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(raceid);
		if (replayInfo)
		{
			// 已经加载好了 
			replayList->SetReplayInfo(replayInfo);
			return;
		}

		replayInfo = m_ReplayInfo;
		while (NextRow())
		{
			ReplayFighterInfo fighter;
			fighter.guid = GetKey();
			fighter.roleId = GetData("roleid");
			fighter.name = GetData("name").GetString();
			fighter.level = GetData("level");
			fighter.occu = GetData("occu");
			fighter.seasonLevel = GetData("season_level");
			fighter.seasonStars = GetData("season_stars");
			fighter.pos = GetData("pos");

			replayInfo->fighters.push_back(fighter);
		}

		ReplayMgr::Instance()->AddReplayInfo(m_ReplayInfo);
		replayList->SetReplayInfo(m_ReplayInfo);
	}

	void OnFailure(ReplayList* replayList)
	{
	}

	ReplayInfoPtr m_ReplayInfo;
};

class LoadReplayInfoCallback : public ILoadReplayListCallback 
{
public:
	LoadReplayInfoCallback(ReplayListType type, ObjID_t owner, ObjID_t raceid) : ILoadReplayListCallback(type, owner), m_RaceId(raceid) {}

	virtual void OnSuccess(ReplayList* replayList)
	{
		auto entry = replayList->FindEntry(m_RaceId);
		if (!entry)
		{
			ErrorLogStream << "replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ") can't find entry(" << m_RaceId << ")." << LogStream::eos;
			return;
		}

		auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(m_RaceId);
		if (replayInfo)
		{
			// 已经加载好了 
			replayList->SetReplayInfo(replayInfo);
			return;
		}

		if (!NextRow())
		{
			ErrorLogStream << "replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ") can't find replay(" << m_RaceId << ") info in db." << LogStream::eos;
			replayList->DelEntry(entry);
		}
		else
		{
			// 加载录像简介
			replayInfo = ReplayInfoPtr(new ReplayInfo());
			replayInfo->raceId = m_RaceId;
			replayInfo->version = GetData("version");
			replayInfo->recordTime = GetData("record_time");
			replayInfo->evaluation = GetData("evaluation");
			replayInfo->result = GetData("result");
			replayInfo->score = GetData("score");
			replayInfo->viewNum = GetData("view_num");
			replayInfo->dbRefNum = GetData("ref_num");
			replayInfo->type = GetData("race_type");

			// 加载录像玩家信息
			auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_replay_fighter");
			cmd->PutCond("raceid", m_RaceId);
			CLRecordClient::Instance()->SendCommand(cmd, new LoadReplayFighterCallback(m_Type, m_Owner, replayInfo), m_Owner);
			replayList->IncDBQueryNum();
		}
	}
private:
	ObjID_t			m_RaceId;
};

class BatchLoadReplayFighterCallback : public ILoadReplayListCallback
{
public:
	BatchLoadReplayFighterCallback(ReplayListType type, ObjID_t owner, const std::vector<ReplayInfoPtr>& replays) : ILoadReplayListCallback(type, owner) 
	{
		for (auto replay : replays)
		{
			if (!replay)
			{
				continue;
			}

			m_Replays[replay->raceId] = replay;
		}
	}

	virtual void OnSuccess(ReplayList* replayList)
	{
		while (NextRow())
		{
			ReplayFighterInfo fighter;
			fighter.guid = GetKey();
			fighter.roleId = GetData("roleid");
			fighter.name = GetData("name").GetString();
			fighter.level = GetData("level");
			fighter.occu = GetData("occu");
			fighter.seasonLevel = GetData("season_level");
			fighter.seasonStars = GetData("season_stars");
			fighter.pos = GetData("pos");

			ObjID_t raceid = GetData("raceid");
			auto replayInfo = GetReplay(raceid);
			if (!replayInfo)
			{
				ErrorLogStream << "race(" << raceid << ") fighter(" << fighter.roleId << ") cant find replayInfo." << LogStream::eos;
				continue;
			}

			auto entry = replayList->FindEntry(raceid);
			if (!entry)
			{
				ErrorLogStream << "replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ") can't find entry(" << raceid << ")." << LogStream::eos;
				return;
			}

			auto tmpReplay = ReplayMgr::Instance()->FindReplayInfo(raceid);
			if (tmpReplay)
			{
				// 已经加载好了 
				replayList->SetReplayInfo(tmpReplay);
				continue;
			}

			replayInfo->fighters.push_back(fighter);
		}

		for (auto itr : m_Replays)
		{
			auto replay = itr.second;
			ReplayMgr::Instance()->AddReplayInfo(replay);
			replayList->SetReplayInfo(replay);
		}
	}

	void OnFailure(ReplayList* replayList)
	{
	}

	ReplayInfoPtr GetReplay(ObjID_t raceid)
	{
		auto itr = m_Replays.find(raceid);
		if (itr == m_Replays.end())
		{
			return nullptr;
		}

		return itr->second;
	}

	std::map<ObjID_t, ReplayInfoPtr> m_Replays;
};

// 批量加载路线信息
class BatchLoadReplayInfoCallback : public ILoadReplayListCallback
{
public:
	BatchLoadReplayInfoCallback(ReplayListType type, ObjID_t owner) : ILoadReplayListCallback(type, owner) {}

	virtual void OnSuccess(ReplayList* replayList)
	{
		// 需要批量加载的录像
		std::vector<ReplayInfoPtr> batchReplays;
		// 单次加载的最大录像数
		UInt32 BatchNum = 100;

		while(NextRow())
		{
			ObjID_t raceid = GetKey();
			auto entry = replayList->FindEntry(raceid);
			if (!entry)
			{
				ErrorLogStream << "replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ") can't find entry(" << raceid << ")." << LogStream::eos;
				continue;
			}

			auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(raceid);
			if (replayInfo)
			{
				// 已经加载好了 
				replayList->SetReplayInfo(replayInfo);
				continue;
			}

			// 加载录像简介
			replayInfo = ReplayInfoPtr(new ReplayInfo());
			replayInfo->raceId = raceid;
			replayInfo->version = GetData("version");
			replayInfo->recordTime = GetData("record_time");
			replayInfo->evaluation = GetData("evaluation");
			replayInfo->result = GetData("result");
			replayInfo->score = GetData("score");
			replayInfo->viewNum = GetData("view_num");
			replayInfo->dbRefNum = GetData("ref_num");
			replayInfo->type = GetData("race_type");

			// 批量加载录像玩家信息
			if (batchReplays.size() >= BatchNum)
			{
				StartBatchLoadReplayFighter(replayList, batchReplays);
			}
		}

		// 加载剩余的录像
		StartBatchLoadReplayFighter(replayList, batchReplays);
	}

	void StartBatchLoadReplayFighter(ReplayList* replayList, std::vector<ReplayInfoPtr>& replays)
	{
		if (replays.empty())
		{
			return;
		}

		std::stringstream ss;
		ss << "raceid in (";

		for (auto& replay : replays)
		{
			ss << replay->raceId;
		}

		ss << ")";


		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_replay_fighter");
		cmd->SetLimit(ss.str());
		CLRecordClient::Instance()->SendCommand(cmd, new BatchLoadReplayFighterCallback(m_Type, m_Owner, replays), m_Owner);
		replayList->IncDBQueryNum();

		replays.clear();
	}
};

class LoadReplayListCallback : public ILoadReplayListCallback
{
public:
	LoadReplayListCallback(ReplayListType type, ObjID_t owner) : ILoadReplayListCallback(type, owner) {}

	virtual void OnSuccess(ReplayList* replayList)
	{
		// 需要加载的录像信息
		std::vector<ObjID_t> needLoadReplayInfoList;

		while (NextRow())
		{
			auto entry = new ReplayListEntry(m_Type);
			entry->SetFromDB(this);

			if (entry->type == REPLAY_LIST_TYPE_MASTER_WAIT)
			{
				auto replayFile = ReplayMgr::Instance()->FindReplayFile(entry->raceId);
				if (!replayFile)
				{
					FatalLogStream << "Add race(" << entry->raceId << ") to master wait list, replay file is miss..." << LogStream::eos;
					continue;
				}
			}

			replayList->AddEntry(entry);
			
			auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(entry->raceId);
			if (!replayInfo)
			{
				needLoadReplayInfoList.push_back(entry->raceId);
			}
			else
			{
				replayList->SetReplayInfo(replayInfo);
			}
		}

		// 没有需要加载的录像信息，加载结束
		if (needLoadReplayInfoList.empty())
		{
			return;
		}

		// 加载录像信息
		for (auto raceid : needLoadReplayInfoList)
		{
			// 加载录像基础信息
			auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_replay_info");
			cmd->PutCond("race_id", raceid);
			CLRecordClient::Instance()->SendCommand(cmd, new LoadReplayInfoCallback(m_Type, m_Owner, raceid), m_Owner);
			replayList->IncDBQueryNum();
		}
	}
};

class BatchLoadReplayListCallback : public ILoadReplayListCallback
{
public:
	BatchLoadReplayListCallback(ReplayListType type, ObjID_t owner, UInt32 batchSize = 100) : ILoadReplayListCallback(type, owner), m_BatchSize(batchSize) {}

	virtual void OnSuccess(ReplayList* replayList)
	{
		// 需要加载的录像信息
		std::vector<ObjID_t> needLoadReplayInfoList;

		while (NextRow())
		{
			auto entry = new ReplayListEntry(m_Type);
			entry->SetFromDB(this);

			if (entry->type == REPLAY_LIST_TYPE_MASTER_WAIT)
			{
				auto replayFile = ReplayMgr::Instance()->FindReplayFile(entry->raceId);
				if (!replayFile)
				{
					FatalLogStream << "Add race(" << entry->raceId << ") to master wait list, replay file is miss..." << LogStream::eos;
					continue;
				}
			}

			replayList->AddEntry(entry);

			auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(entry->raceId);
			if (!replayInfo)
			{
				needLoadReplayInfoList.push_back(entry->raceId);
			}
			else
			{
				replayList->SetReplayInfo(replayInfo);
			}
		}

		// 没有需要加载的录像信息，加载结束
		if (needLoadReplayInfoList.empty())
		{
			return;
		}

		// 批量加载录像信息
		std::vector<ObjID_t> raceids;
		for (auto raceid : needLoadReplayInfoList)
		{
			raceids.push_back(raceid);

			if (raceids.size() >= m_BatchSize)
			{
				StartBatchLoadReplayInfo(replayList, raceids);
			}
		}

		// 加载录像信息
		StartBatchLoadReplayInfo(replayList, raceids);
	}

	void StartBatchLoadReplayInfo(ReplayList* replayList, std::vector<ObjID_t>& raceids)
	{
		if (raceids.empty())
		{
			return;
		}

		std::stringstream ss;
		ss << "race_id in (";

		for (auto& id : raceids)
		{
			ss << id;
		}

		ss << ")";

		// 批量加载录像基础信息
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_replay_info");
		cmd->SetLimit(ss.str());
		CLRecordClient::Instance()->SendCommand(cmd, new BatchLoadReplayInfoCallback(m_Type, m_Owner), m_Owner);
		replayList->IncDBQueryNum();

		raceids.clear();
	}

	UInt32		m_BatchSize;
};

ReplayListEntry::ReplayListEntry(ReplayListType type_)
: guid(0), ownerid(0), type(type_), raceId(0), info(NULL)
{

}

ReplayListEntry::~ReplayListEntry()
{
	SetReplayInfo(NULL);
}

void ReplayListEntry::SetReplayInfo(ReplayInfoPtr info_)
{
	info = info_;
}

void ReplayListEntry::SaveToDB()
{
	guid = CLRecordClient::Instance()->GenGuid();

	auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_replay_list", guid);
	cmd->PutData("ownerid", ownerid);
	cmd->PutData("type", type);
	cmd->PutData("raceid", raceId);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, ownerid);
}

void ReplayListEntry::SetFromDB(CLRecordCallback* callback)
{
	guid = callback->GetKey();
	ownerid = callback->GetData("ownerid");
	type = callback->GetData("type");
	raceId = callback->GetData("raceid");
}

void ReplayListEntry::Del()
{
	// 从数据库中删除
	auto cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_replay_list", guid);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, ownerid);

	if (info)
	{
		info->dbRefNum--;

		// 更新到数据库
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_replay_info", info->dbRefNum, false);
		cmd->PutData("ref_num", info->dbRefNum);
		CLRecordClient::Instance()->SendCommand(cmd, NULL, ownerid);
	}
	else
	{
		// 正常情况下也不会走到这里
		// 更新数据库中录像信息的引用计数
		// ToDo...

		FatalLogStream << "replay(" << raceId << ") list(" << guid << ") invalid info..." << LogStream::eos;
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

ReplayList::ReplayList(ObjID_t owner, ReplayListType type)
: m_Status(REPLAY_LIST_STATUS_INIT), m_Owner(owner), m_Type(type), m_RemoveTime(0), m_DBQueryNum(0)
{

}

ReplayList::~ReplayList()
{
	for (auto entry : m_Entrys)
	{
		CL_SAFE_DELETE(entry);
	}
	m_Entrys.clear();
}

ReplayListEntry* ReplayList::FindEntry(ObjID_t raceid)
{
	for (auto entry : m_Entrys)
	{
		if (entry && entry->raceId == raceid)
		{
			return entry;
		}
	}

	return NULL;
}

void ReplayList::AddEntry(ReplayListEntry* entry)
{
	if (!entry)
	{
		return;
	}

	if (m_Entrys.size() == GetMaxEntryNum())
	{
		auto entry = m_Entrys.front();
		DelEntry(entry);
	}

	m_Entrys.push_back(entry);
}

void ReplayList::DelEntry(ReplayListEntry* entry)
{
	for (auto itr = m_Entrys.begin(); itr != m_Entrys.end(); ++itr)
	{
		if (*itr == entry)
		{
			m_Entrys.erase(itr);
			if (entry)
			{
				CL_SAFE_DELETE(entry);
			}

			return;
		}
	}
}

void ReplayList::SetReplayInfo(ReplayInfoPtr info)
{
	if (!info)
	{
		return;
	}

	for (auto entry : m_Entrys)
	{
		if (entry && entry->raceId == info->raceId)
		{
			entry->SetReplayInfo(info);
		}
	}
}

void ReplayList::StartLoad(ReplayListCallback* cb)
{
	DebugLogStream << "start to load replay list(" << (UInt8)m_Type << ") owner(" << m_Owner << ")." << LogStream::eos;
	SetStatus(REPLAY_LIST_STATUS_LOADING);

	IncDBQueryNum();

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_replay_list");
	cmd->PutCond("type", (UInt8)m_Type);
	cmd->PutCond("ownerid", m_Owner);

	if (RSApplication::Instance()->IsBatchLoadReplay())
	{
		CLRecordClient::Instance()->SendCommand(cmd, new BatchLoadReplayListCallback(m_Type, m_Owner), m_Owner);
	}
	else
	{
		CLRecordClient::Instance()->SendCommand(cmd, new LoadReplayListCallback(m_Type, m_Owner), m_Owner);
	}
}

void ReplayList::AddLoadCallback(ReplayListCallback* cb)
{
	m_Callbacks.push_back(cb);
}

void ReplayList::OnLoadFinish()
{
	SetStatus(REPLAY_LIST_STATUS_FINISH);

	for (auto cb : m_Callbacks)
	{
		cb->OnFinish(this);
		CL_SAFE_DELETE(cb);
	}
	m_Callbacks.clear();
}

void ReplayList::OnLoadFailure()
{
	for (auto cb : m_Callbacks)
	{
		cb->OnFailure();
		CL_SAFE_DELETE(cb);
	}
	m_Callbacks.clear();
}

UInt32 ReplayList::GetMaxEntryNum()
{
	// 不同类型的列表有不同的长度
	if (m_Type == REPLAY_LIST_TYPE_MASTER)
	{
		UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_REPLAY_LIST_TYPE_MASTER_LIMIT);
		if (num == 0)
		{
			num = 4;
		}
		return num;
	}
	else if (m_Type == REPLAY_LIST_TYPE_MASTER_WAIT)
	{
		UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_REPLAY_LIST_TYPE_MASTER_WAIT_LIMIT);
		if (num == 0)
		{
			num = 2;
		}
		return num;
	}
	
	return 20;
}