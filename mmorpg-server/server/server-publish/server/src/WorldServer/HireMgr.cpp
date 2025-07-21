#include "HireMgr.h"
#include <AvalonRandom.h>
#include "WSSelectInitDataCallback.h"
#include "WSPlayer.h"
#include <CLHireTaskDataEntry.h>
#include <CLCenterProtocol.h>
#include "WSRouter.h"
#include "AccountMgr.h"

const UInt32 OLD_LEVEL = 50;

const UInt32 NEW_LEVEL = 19;

const UInt32 POW_ARRAY[4] = { 27000, 900, 30, 1 };


enum HirePlayerOnlineStatus
{
	HPOS_OFFLINE =0,
	HPOS_ONLINE = 1,
};

/**
*@brief 加载勇士招募信息
*/
class SelectHireInfoCallback : public SelectInitDataCallback
{
public:
	SelectHireInfoCallback() {

	}

	bool OnLoadFinished()
	{
		HireMgr::Instance()->LoadDBData(this);
		HireMgr::Instance()->OnLoadDBEnd();
		return true;
	}

};

//							  0123456789
const std::string CODE_LIB = "23456789ABCDEFGHJKMNPQSTUVWXYZ";
const UInt32 CODE_LEN = 6;

std::string GetRandomCode(UInt32 len, UInt32 zoneId)
{
	std::string prefix;
	for (UInt32 i = 0; i < 4; ++i)
	{
		if (zoneId >= POW_ARRAY[i])
		{
			UInt32 base = zoneId / POW_ARRAY[i];
			prefix.push_back(CODE_LIB[base]);
			zoneId -= base * POW_ARRAY[i];
		}
		else
		{
			prefix.push_back(CODE_LIB[0]);
		}
	}

	std::string str;
	if (len <= 0)
	{
		return str;
	}
	while (str.size() < len)
	{
		UInt32 random = Avalon::Random::RandBetween(0, CODE_LIB.size() - 1);
		str.push_back(CODE_LIB[random]);
	}
	return prefix + str;

}


void HireTask::Input(Avalon::NetInputStream & stream)
{
	//stream & taskID & cnt & completeInfoVec;
}

void HireTask::Output(Avalon::NetOutputStream & stream)
{
	//stream & taskID & cnt & completeInfoVec;
}

void HireTask::Encode(Avalon::NetOutputStream & stream, UInt32 type, bool bDirty)
{
	//stream & taskID & cnt & completeInfoVec;
}

void HireTask::Decode(Avalon::NetInputStream & stream)
{
	//stream & taskID & cnt & completeInfoVec;
}

void HireTask::DecodeString(std::istringstream & is)
{
	//char split;
	//is >> taskID >> split >> cnt >> split >> status;
	//UInt32 size;
	//is >> split >> size >> split;
	//UInt32 tempAccid = 0;
	//while (size-- > 0)
	//{
	//	is >> tempAccid >> split;
	//	completeInfoVec.emplace_back(tempAccid);
	//}
}

void HireTask::EncodeString(std::ostringstream & os)
{
	os << taskID << ':' << cnt << ':' << status << '_';
	bool first = true;
	for (UInt32 i = 0; i < completeInfoVec.size(); ++i)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			os << ',';
		}
		os << completeInfoVec[i].accid << '/' << completeInfoVec[i].roleId << '/' << completeInfoVec[i].name;
	}
}

void HireTask::CopyFromDataEntry(HireTaskDataEntry * dataEntry)
{
	this->taskID = dataEntry->id;
	this->fullCnt = dataEntry->fullCnt;
	this->type = dataEntry->type;
	this->rewards = dataEntry->rewards;
	this->identity = dataEntry->identity;
	this->refreshType = dataEntry->refreshType;
	this->param = dataEntry->param;
}


HireInfo::HireInfo()
{
	m_OnlinePlayerData.online = HPOS_OFFLINE;
}

HireInfo::~HireInfo()
{
	for (auto &taskIter : m_HireTaskMap)
	{
		delete taskIter.second;
	}
	m_HireTaskMap.clear();
}


void HireInfo::AddNewTask(HireTaskDataEntry* data)
{
	auto task = GetHireTask(data->id);
	if (task != nullptr)
	{
		return;
	}

	task = new HireTask;
	task->CopyFromDataEntry(data);
	m_HireTaskMap.insert(std::make_pair(data->id, task));
}

std::vector<HireTask*> HireInfo::GetHireTaskList()
{
	std::vector<HireTask*> vec;

	for (auto &i : m_HireTaskMap)
	{
		vec.push_back(i.second);
	}

	return vec;
}

HireTask * HireInfo::GetHireTask(UInt32 taskId)
{
	auto taskIter = m_HireTaskMap.find(taskId);
	if (taskIter == m_HireTaskMap.end())
	{
		return nullptr;
	}
	return taskIter->second;
}

void HireInfo::OnPlayerOnline(WSPlayer * player)
{

	if (player->GetLevel() >= OLD_LEVEL || player->HasRoleLevelLimit(OLD_LEVEL))
	{
		SetIdentify(HI_OLDMAN);
	}
	else if(player->GetLevel() <= NEW_LEVEL && !player->HasRoleLevelLimit(NEW_LEVEL + 1))
	{
		SetIdentify(HI_NEWBIE);
	}

	//检测改变身份
	if (player->IsVeteranReturn() && !IsIdentify(HI_NEWBIE))
	{
		SetIdentify(HI_RETURNMAN);
	}


	if (GetIdentify() == 0)
	{
		return;
	}

	//维护招募列表用在线信息
	m_OnlinePlayerData.name = player->GetName();
	m_OnlinePlayerData.occu = player->GetOccu();
	m_OnlinePlayerData.userId = player->GetID();
	m_OnlinePlayerData.lv = player->GetLevel();
	m_OnlinePlayerData.online = HPOS_ONLINE;


	//接受合适的任务
	class visit : public Avalon::DataEntryVisitor<HireTaskDataEntry>
	{
	public:
		visit(HireInfo& info) : m_Info(info) {}
		virtual bool operator()(HireTaskDataEntry* dataEntry)
		{
			if (!m_Info.IsIdentify(static_cast<HireIdentify>(dataEntry->identity)))
			{
				return true;
			}

			auto task = m_Info.GetHireTask(dataEntry->id);
			if (task == nullptr)
			{
				m_Info.AddNewTask(dataEntry);
			}
			return true;
		}
	private:
		UInt32 m_Lv = 0;
		UInt32 m_Tag = 0;
		HireInfo& m_Info;
	};
	visit vistor(*this);
	HireTaskDataEntryMgr::Instance()->Visit(vistor);
}

void HireInfo::OnPlayerOffline(WSPlayer * player)
{
	m_OnlinePlayerData.online = HPOS_OFFLINE;
}

void HireInfo::OnPlayerTrigeTask(WSPlayer* player, HireTaskType type, UInt32 value)
{
	for (auto &taskIter : m_HireTaskMap)
	{
		bool boFinish = false;
		HireTask* task = taskIter.second;
		if (task->type != type)
		{
			continue;
		}

		if (task->status >= HireTaskStatus::HIRE_TASK_FINISHED)
		{
			continue;
		}

		switch (type)
		{
		case HTT_LEVEL:
		{
			task->cnt = value;
			if (task->cnt >= task->fullCnt)
			{
				task->status = HireTaskStatus::HIRE_TASK_FINISHED;
				boFinish = true;
			}
			break;
		}
		case HTT_SHENG_YUAN_PIAO:
		case HTT_YUAN_GU_PIAO:
		case HTT_GUILD_DUNGEON:
		case HTT_GUILD_BATTLE:
		case HTT_CHAOS_DUNGEON:
		case HTT_TEAM_DUNGEON:
		case HTT_NONE_DUNGEON:
		case HTT_MAKE_TEAM:
		case HTT_BIND_OLD_MAN:
		case HTT_BIND_RETURN_MAN:
		{
			task->cnt += value;
			if (task->cnt >= task->fullCnt)
			{
				task->status = HireTaskStatus::HIRE_TASK_FINISHED;
				boFinish = true;
			}
			break;
		}
		default:
			break;
		}

		if (!boFinish)
		{
			continue;
		}

		if (player == nullptr)
		{
			continue;
		}

		auto bindInfo = HireMgr::Instance()->GetHireInfo(GetBindAccid());
		if (bindInfo != nullptr)
		{
			bindInfo->OnPlayerTrigeBindTask(task->taskID, player->GetAccId(), player->GetID(), player->GetName());
		}
	}
}

void HireInfo::OnPlayerTrigeBindTask(UInt32 value, UInt32 accid, UInt64 roleId, const std::string& name)
{
	for (auto &taskIter : m_HireTaskMap)
	{
		HireTask* task = taskIter.second;
		if (task->type != HTT_BIND_COMPLETE)
		{
			continue;
		}

		if (task->status >= HireTaskStatus::HIRE_TASK_FINISHED)
		{
			continue;
		}

		if (task->param.size() < 1)
		{
			continue;
		}
		if (task->param[0] != value)
		{
			continue;
		}

		bool boFind = false;
		for (auto& i : task->completeInfoVec)
		{
			if (i.accid == accid)
			{
				boFind = true;
				break;
			}
		}
		if (boFind)
		{
			continue;
		}

		if (task->completeInfoVec.size() < task->fullCnt)
		{
			task->completeInfoVec.emplace_back(accid, roleId, name);
		}
		
		task->cnt += 1;
		if (task->cnt >= task->fullCnt)
		{
			task->status = HireTaskStatus::HIRE_TASK_FINISHED;
		}
	}
}

void HireInfo::OnPlayerChangeName(UInt64 roleId, const std::string & name)
{
	for (auto& iter : m_HireTaskMap)
	{
		auto task = iter.second;
		for (auto& info : task->completeInfoVec)
		{
			if (info.roleId == roleId)
			{
				info.name = name;
			}
		}
	}
}

UInt32 HireInfo::OnPlayerSubmitTask(WSPlayer* player, UInt32 taskId)
{
	auto taskIter = m_HireTaskMap.find(taskId);
	if (taskIter == m_HireTaskMap.end())
	{
		return ErrorCode::HIRE_NO_INFO;
	}
	HireTask* task = taskIter->second;

	if (task->status != HireTaskStatus::HIRE_TASK_FINISHED)
	{
		return ErrorCode::HIRE_TASK_ERR_UNFINISH;
	}

	task->status = HireTaskStatus::HIRE_TASK_OVER;
	player->AddItem( GetReason(LogSourceType::SOURCE_HIRE_TASK), task->rewards);

	return ErrorCode::SUCCESS;
}

void HireInfo::OnWeekChange()
{
	for (auto &taskIter : m_HireTaskMap)
	{
		HireTask* task = taskIter.second;
		if (task->refreshType != HireRefreshType::HRT_WEEK)
		{
			continue;
		}

		task->cnt = 0;
		task->status = HireTaskStatus::HIRE_TASK_INIT;

	}
}

void HireInfo::OnMonthChange()
{
	for (auto &taskIter : m_HireTaskMap)
	{
		HireTask* task = taskIter.second;
		if (task->refreshType != HireRefreshType::HRT_MONTH)
		{
			continue;
		}

		task->cnt = 0;
		task->status = HireTaskStatus::HIRE_TASK_INIT;

	}
}

void HireInfo::CheckTask()
{
	for (auto &taskIter : m_HireTaskMap)
	{
		HireTask* task = taskIter.second;

		if (task->status >= HireTaskStatus::HIRE_TASK_FINISHED)
		{
			continue;
		}

		switch (task->type)
		{
		case HTT_LEVEL:
		{
			if (task->cnt >= task->fullCnt)
			{
				task->status = HireTaskStatus::HIRE_TASK_FINISHED;
			}
			break;
		}
		case HTT_SHENG_YUAN_PIAO:
		case HTT_YUAN_GU_PIAO:
		case HTT_GUILD_DUNGEON:
		case HTT_GUILD_BATTLE:
		case HTT_CHAOS_DUNGEON:
		case HTT_TEAM_DUNGEON:
		case HTT_NONE_DUNGEON:
		case HTT_MAKE_TEAM:
		case HTT_BIND_OLD_MAN:
		case HTT_BIND_RETURN_MAN:
		{
			if (task->cnt >= task->fullCnt)
			{
				task->status = HireTaskStatus::HIRE_TASK_FINISHED;
			}
			break;
		}
		default:
			break;
		}
	}
}

void HireInfo::SaveToDB()
{
	SaveTaskToString();
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_hire", GetID(), false);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}
}

void HireInfo::InsertToDB()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_hire", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
}

void HireInfo::DeleteFromDB()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_hire", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void HireInfo::Init()
{
	LoadTaskFromString();
}

void HireInfo::ComboHireInfo(HireInfo & abandon)
{
	auto &remain = *this;

	for (auto &i : abandon.m_HireTaskMap)
	{
		HireTask* abandonTask = i.second;
		auto remainTaskIter = remain.m_HireTaskMap.find(abandonTask->taskID);
		if (remainTaskIter == remain.m_HireTaskMap.end())
		{
			auto task = new HireTask;
			task->fullCnt = abandonTask->fullCnt;
			task->type = abandonTask->type;
			task->rewards = abandonTask->rewards;
			task->refreshType = abandonTask->refreshType;
			task->identity = abandonTask->identity;
			task->param = abandonTask->param;
			m_HireTaskMap.insert(std::make_pair(abandonTask->taskID, task));
			continue;
		}
		HireTask* remainTask = remainTaskIter->second;
		remainTask->cnt += abandonTask->cnt;
		remainTask->status = remainTask->status > abandonTask->status ? remainTask->status : abandonTask->status;
	}

	if (remain.IsIdentify(HI_NEWBIE) && abandon.IsIdentify(HI_NEWBIE))
	{
		remain.SetCode(remain.GetBindTime() < abandon.GetBindTime() ? remain.GetCode() : abandon.GetCode());
	}

	if ((remain.IsIdentify(HI_NEWBIE)&& abandon.IsIdentify(HI_OLDMAN)) || (abandon.IsIdentify(HI_NEWBIE) && remain.IsIdentify(HI_OLDMAN)))
	{
		remain.SetIdentify(HI_NEWBIE);
		remain.SetIdentify(HI_OLDMAN);
	}
}

void HireInfo::SaveTaskToString()
{
	std::ostringstream os;
	bool first = true;
	for (auto &i : m_HireTaskMap)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			os << '|';
		}
		i.second->EncodeString(os);;
	}
	if (static_cast<std::string>(m_TaskString) != os.str())
	{
		m_TaskString = os.str();
	}
}

void HireInfo::LoadTaskFromString()
{
	if (static_cast<std::string>(m_TaskString).size() == 0)
	{
		return;
	}
	std::string splitTask = "|";
	std::string splitAccidAndContent = "_";
	std::string splitContent = ":";
	std::string splitAccid = ",";
	std::string splitRoleid = "/";
	std::vector<std::string> taskVec;
	Avalon::StringUtil::Split(m_TaskString, taskVec, splitTask);
	for (UInt32 i = 0; i < taskVec.size(); ++i)
	{
		std::vector<std::string> accidAndContentVec;
		Avalon::StringUtil::Split(taskVec[i], accidAndContentVec, splitAccidAndContent);
		if (accidAndContentVec.size() != 2)
		{
			continue;
		}

		std::vector<std::string> contentVec;
		Avalon::StringUtil::Split(accidAndContentVec[0], contentVec, splitContent);
		if (contentVec.size() != 3)
		{
			continue;
		}
		auto task = new HireTask;
		task->taskID = Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[0]);
		task->cnt = Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[1]);
		task->status = Avalon::StringUtil::ConvertToValue<UInt32>(contentVec[2]);

		std::vector<std::string> accidInfoVec;
		Avalon::StringUtil::Split(accidAndContentVec[1], accidInfoVec, splitAccid);
		for (UInt32 j = 0; j < accidInfoVec.size(); ++j)
		{
			std::vector<std::string> roleIdVec;
			Avalon::StringUtil::Split(accidInfoVec[j], roleIdVec, splitRoleid);
			task->completeInfoVec.emplace_back(Avalon::StringUtil::ConvertToValue<UInt32>(roleIdVec[0]), Avalon::StringUtil::ConvertToValue<UInt64>(roleIdVec[1]), roleIdVec[2]);
		}
		m_HireTaskMap.insert(std::make_pair(task->taskID, task));
		auto data = HireTaskDataEntryMgr::Instance()->FindEntry(task->taskID);
		if (data != nullptr)
		{
			task->CopyFromDataEntry(data);
		}
	}
}


HireMgr::HireMgr()
{
}

HireMgr::~HireMgr()
{
}

HireInfo* HireMgr::AddHireInfo(UInt32 accid, UInt32 zoneId)
{
	auto data = m_AccidHireInfoMap.find(accid);
	if (data != m_AccidHireInfoMap.end())
	{
		ErrorLogStream << "already have same accid" << accid << LogStream::eos;
		return nullptr;
	}

	std::string code = "";
	do 
	{
		code = GetRandomCode(CODE_LEN, zoneId);
	} while (m_CodeAccidMap.count(code) > 0);

	m_CodeAccidMap[code] = accid;
	m_AccidHireInfoMap[accid].SetAccid(accid);
	m_AccidHireInfoMap[accid].SetCode(code);
	m_AccidHireInfoMap[accid].InsertToDB();
	return &m_AccidHireInfoMap[accid];
}

HireInfo* HireMgr::GetHireInfo(UInt32 accid)
{
	auto data = m_AccidHireInfoMap.find(accid);
	if (data != m_AccidHireInfoMap.end())
	{
		return &(data->second);
	}
	return nullptr;
}

HireInfo * HireMgr::GetHireInfo(std::string code)
{
	auto data = m_CodeAccidMap.find(code);
	if (data != m_CodeAccidMap.end())
	{
		return GetHireInfo(data->second);
	}
	return nullptr;
}

void HireMgr::OnPlayerOnline(WSPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}

	auto hireInfo = GetHireInfo(player->GetAccId());
	if (hireInfo == nullptr)
	{
		hireInfo = AddHireInfo(player->GetAccId(), player->GetZoneId());
	}
	hireInfo->OnPlayerOnline(player);
}

void HireMgr::OnPlayerOffline(WSPlayer * player)
{
	if (player == nullptr)
	{
		return;
	}
	auto hireInfo = GetHireInfo(player->GetAccId());
	if (hireInfo == nullptr)
	{
		return;
	}
	hireInfo->OnPlayerOffline(player);
}

UInt32 HireMgr::OnPlayerUseCode(WSPlayer * player, std::string code)
{
	if (player == nullptr)
	{
		return ErrorCode::HIRE_NO_INFO;
	}



	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return ErrorCode::HIRE_NO_INFO;
	}

	if (player->HasRoleLevelLimit(NEW_LEVEL + 1) && info->IsIdentify(HI_NEWBIE))
	{
		return ErrorCode::HIRE_IDENTIFY_ERR;
	}

	if (info->GetBindAccid() != 0)
	{
		return ErrorCode::HIRE_ALREADY_BIND;
	}
	if (!info->IsIdentify(HI_NEWBIE) && !info->IsIdentify(HI_RETURNMAN))
	{
		return ErrorCode::HIRE_IDENTIFY_ERR;
	}

	if (info->GetCode() == code)
	{
		return ErrorCode::HIRE_IDENTIFY_ERR;
	}

	auto bindInfo = HireMgr::Instance()->GetHireInfo(code);
	if (bindInfo == nullptr)
	{
		return ErrorCode::HIRE_CODE_ERROR;
	}

	if (!bindInfo->IsIdentify(HI_OLDMAN))
	{
		return ErrorCode::HIRE_IDENTIFY_ERR;
	}

	CLProtocol::GASCheckHireBindReq req;
	req.accid = info->GetAccid();
	req.code = code;
	req.zoneId = ZONE_ID;
	req.platform = player->GetPf();
	Router::SendToGlobalActivity(req);
	return ErrorCode::SUCCESS;
}

UInt32 HireMgr::OnPlayerUseCodeSuccess(WSPlayer * player, std::string code)
{
	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return ErrorCode::HIRE_NO_INFO;
	}
	auto bindInfo = HireMgr::Instance()->GetHireInfo(code);
	if (bindInfo == nullptr)
	{
		return ErrorCode::HIRE_NO_INFO;
	}
	info->SetBindAccid(bindInfo->GetAccid());
	info->SetBindTime(CURRENT_TIME.Sec());
	bindInfo->AddBindMeAccid(info->GetAccid());

	info->OnPlayerTrigeTask(player, HTT_BIND_OLD_MAN, 1);
	if (info->IsIdentify(HI_RETURNMAN))
	{
		bindInfo->OnPlayerTrigeTask(nullptr, HTT_BIND_RETURN_MAN, 1);
	}

	return ErrorCode::SUCCESS;
}

std::vector<HirePlayerData> HireMgr::OnPlayerQueryHireList(WSPlayer * player)
{
	if (player == nullptr)
	{
		return {};
	}

	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return {};
	}

	if (!info->IsIdentify(HI_OLDMAN))
	{
		return {};
	}

	std::vector<HirePlayerData> vec;
	for (auto &accId : info->GetBindMeSet())
	{
		auto bindInfo = HireMgr::Instance()->GetHireInfo(accId);
		if (bindInfo == nullptr)
		{
			continue;
		}
		if (bindInfo->IsIdentify(HI_RETURNMAN))
		{
			continue;
		}
		if (bindInfo->GetOnlinePlayerData().userId == 0)
		{
			continue;
		}

		vec.push_back(bindInfo->GetOnlinePlayerData());
	}
	
	return vec;
}

void HireMgr::OnPlayerTrigeTask(WSPlayer* player, HireTaskType type, UInt32 value, std::vector<UInt32> teammateIds)
{
	if (player == nullptr)
	{
		return;
	}

	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return;
	}

	if (type == HTT_MAKE_TEAM)
	{
		for (auto &i : teammateIds)
		{
			auto mateInfo = GetHireInfo(i);
			if (mateInfo == nullptr)
			{
				continue;
			}
			if (!mateInfo->IsIdentify(HI_NEWBIE))
			{
				continue;
			}
			if (mateInfo->GetBindAccid() == info->GetAccid())
			{
				info->OnPlayerTrigeTask(player, HTT_MAKE_TEAM, 1);
				break;
			}
		}
		return;
	}

	info->OnPlayerTrigeTask(player, type, value);
}

UInt32 HireMgr::OnPlayerSubmitTask(WSPlayer* player, UInt32 taskId)
{
	if (player == nullptr)
	{
		return ErrorCode::HIRE_NO_INFO;
	}

	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return ErrorCode::HIRE_NO_INFO;
	}

	return info->OnPlayerSubmitTask(player, taskId);
}

void HireMgr::OnPlayerNameChange(ObjID_t roleId, const std::string & name)
{
	auto accid = AccountMgr::Instance()->GetAccIDByRoleID(roleId);
	auto info = HireMgr::Instance()->GetHireInfo(accid);
	if (info == nullptr)
	{
		return;
	}
	auto bindInfo = HireMgr::Instance()->GetHireInfo(info->GetBindAccid());
	if (bindInfo == nullptr)
	{
		return;
	}
	bindInfo->OnPlayerChangeName(roleId, name);
}


bool HireMgr::Init()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_hire");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectHireInfoCallback());
	return true;
}

void HireMgr::OnTick(const Avalon::Time & now)
{
	for (auto &i : m_AccidHireInfoMap)
	{
		i.second.SaveToDB();
	}
}

void HireMgr::OnWeekChange()
{
	for (auto &i : m_AccidHireInfoMap)
	{
		i.second.OnWeekChange();
	}
}

void HireMgr::OnMonthChange()
{
	for (auto &i : m_AccidHireInfoMap)
	{
		i.second.OnMonthChange();
	}
}

bool HireMgr::LoadDBData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt32 accid = callback->GetData("accid");

		auto firstIter = m_AccidHireInfoMap.find(accid);
		if (firstIter != m_AccidHireInfoMap.end())
		{
			HireInfo info;
			info.SetID(callback->GetKey());
			info.SetDBData(callback);
			info.Init();
			firstIter->second.ComboHireInfo(info);
			info.DeleteFromDB();
			firstIter->second.CheckTask();
			continue;
		}
		m_AccidHireInfoMap[accid].SetID(callback->GetKey());
		m_AccidHireInfoMap[accid].SetDBData(callback);
		m_AccidHireInfoMap[accid].Init();
	}
	return true;
}

void HireMgr::OnLoadDBEnd()
{
	for (auto &infoIter : m_AccidHireInfoMap)
	{
		auto hireInfo = &infoIter.second;
		m_CodeAccidMap[hireInfo->GetCode()] = hireInfo->GetAccid();
		if (hireInfo->GetBindAccid() == 0)
		{
			continue;
		}
		auto bindInfo = GetHireInfo(hireInfo->GetBindAccid());
		if (bindInfo == nullptr)
		{
			continue;
		}

		bindInfo->AddBindMeAccid(hireInfo->GetAccid());
	}
}