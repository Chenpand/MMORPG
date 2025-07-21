#include "BaseTask.h"

#include <AvalonStringUtil.h>
#include <CLTaskProtocol.h>
//#include <udplog_typedef.h>

#include "Player.h"
#include "ScriptMgr.h"
#include "TaskScript.h"

TaskQuestVarMap::TaskQuestVarMap() { }
TaskQuestVarMap::~TaskQuestVarMap() { }

void TaskQuestVarMap::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_QuestVars;
}

void TaskQuestVarMap::Decode(Avalon::NetInputStream& stream)
{
	stream & m_QuestVars;
}


void TaskQuestVarMap::DecodeString(std::istringstream& is)
{
	std::string questVars = is.str();
	std::vector<std::string> valuePairs;
	Avalon::StringUtil::Split(questVars, valuePairs, ",");

	for (std::vector<std::string>::iterator iter = valuePairs.begin(); iter != valuePairs.end(); ++iter)
	{
		std::vector<std::string> pair;
		Avalon::StringUtil::Split(*iter, pair, "=");
		if (pair.size() < 2) continue;

		m_QuestVars.insert(std::make_pair(pair[0], pair[1]));
	}
}

void TaskQuestVarMap::EncodeString(std::ostringstream& os)
{
	for (QuestVarMap::iterator iter = m_QuestVars.begin(); iter != m_QuestVars.end(); ++iter)
	{
		os << iter->first << "=" << iter->second << ',';
	}
}

Int64 TaskQuestVarMap::GetVar(const char* name) const
{
	if (name == NULL) return 0;
	QuestVarMap::const_iterator iter = m_QuestVars.find(name);
	if (iter != m_QuestVars.end()) return Avalon::StringUtil::ConvertToValue<Int64>(iter->second);
	return 0;
}

const char* TaskQuestVarMap::GetNameVar(const char* name) const
{
	if (name == NULL) return 0;

	QuestVarMap::const_iterator iter = m_QuestVars.find(name);
	if (iter != m_QuestVars.end()) return iter->second.c_str();
	return "";
}

void TaskQuestVarMap::SetVar(std::string key, std::string value)
{
	if (key.empty()) return;
	m_QuestVars[key] = value;
	//SetDirty();
}

BaseTask::BaseTask(Player* owner, UInt32 id)
	: m_pOwner(owner)
{
	SetID(0);
	m_DataId.SetDBValue(id);
	//m_Quality.SetDBValue(0);
	m_Status.SetDBValue(TASK_UNFINISH);
	m_SubmitCount.SetDBValue(0);
}

BaseTask::BaseTask(Player* owner, UInt32 id, UInt64 guid) : BaseTask(owner, id)
{
	SetID(guid);
}

void BaseTask::SetStatus(TaskStatus status)
{
	if (m_Status != status)
	{
		m_Status = status;

		if (TASK_ACHIEVEMENT == GetType() && status == TASK_FINISHED)
		{
			m_FinishedTime = (UInt32)CURRENT_TIME.Sec();
		}

		SyncStatus();
		OnSetStatus();

		if (status == TASK_FINISHED)
		{
			GetOwner()->OnTaskSucced(this);
		}
		else if (status == TASK_FAILED)
		{
			GetOwner()->OnTaskFailed(GetDataId());
		}
	}
}

void BaseTask::SyncStatus()
{
	//通知任务状态改变
	CLProtocol::SceneNotifyTaskStatus protocol;
	protocol.taskId = GetDataId();
	protocol.status = m_Status;
	protocol.finTime = m_FinishedTime;
	m_pOwner->SendProtocol(protocol);
}

Int64 BaseTask::GetVar(const char* name) const
{
	return m_QuestVars.GetVar(name);
}

void BaseTask::SetVar(const char* name, Int64 val)
{
	if (name == NULL) return;

	std::string key(name);
	std::string value(Avalon::StringUtil::ConvertToString(val));

	if (m_QuestVars.GetVar(name) == val)
	{
		return;
	}

	m_QuestVars.SetVar(key, value);

	OnSetVar();

	SyncVar(key, value);
}

void BaseTask::ResetVar()
{
	QuestVarMap var;
	m_QuestVars.SetQuestVarMap(var);
}

const char* BaseTask::GetNameVar(const char* name) const
{
	return m_QuestVars.GetNameVar(name);
}

void BaseTask::SetNameVar(const char* name, const char* val)
{
	if (name == NULL) return;

	std::string key(name);
	std::string value(val);
	m_QuestVars.SetVar(name, val);

	OnSetVar();

	SyncVar(name, val);
}

void BaseTask::SyncVar(std::string key, std::string value)
{
	//通知任务变量改变
	CLProtocol::SceneNotifyTaskVar protocol;
	protocol.taskId = GetDataId();
	protocol.key = key;
	protocol.value = value;
	m_pOwner->SendProtocol(protocol);
}

void BaseTask::GetBriefInfo(TaskBriefInfo& info)
{
	info.id = GetDataId();
	info.status = m_Status;
	info.vars = m_QuestVars.GetQuestVarMap();
	info.finTime = m_FinishedTime;
	info.submitCount = m_SubmitCount;
}

void BaseTask::AddTaskItem(UInt32 itemId, UInt32 prob)
{
	if (!m_pOwner || GetStatus() != TASK_UNFINISH)
	{
		return;
	}

	if (!Avalon::Random::SelectInHundred(prob))
	{
		return;
	}

	m_pOwner->AddItem(GetRewardsReason(), itemId, 1);
}


bool BaseTask::CheckGiveRewards()
{
	return ErrorCode::SUCCESS == m_pOwner->CheckAddRewards(GetRewards()); 
}

void BaseTask::GiveRewards() 
{
	m_pOwner->AddRewards(GetRewardsReason(), GetRewards(), true); 
}

const char* BaseTask::GetRewardsReason()
{
	LogSourceType logType = LogSourceType::SOURCE_TYPE_TASK;
	switch (GetType())
	{
	case TASK_MAIN:
		logType = SOURCE_TYPE_TASK_MAIN;
		break;
	case TASK_BRANCH:
		logType = SOURCE_TYPE_TASK_BRANCH;
		break;
	case TASK_TRANSFER:
		logType = SOURCE_TYPE_TASK_CHANGE_OCCU;
		break;
	case TASK_AWAKEN:
		logType = SOURCE_TYPE_TASK_AWAKEN;
		break;
	case TASK_TITLE:
		logType = SOURCE_TYPE_TASK_TITLE;
	default:
		break;
	}
	m_Reason = GetReason(logType, GetDataId(), GetStatus());
	return m_Reason.c_str();
}


void BaseTask::OnSetStatus()
{
	OnSetVar();
}

bool BaseTask::CheckFinish()
{
	if (TASK_UNFINISH == m_Status || TASK_FINISHED == m_Status)
	{
		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(m_DataId);
		if (script == NULL) return false;
		script->CheckFinish(GetOwner(), this);
	}
	return true;
}


void BaseTask::OnSetVar()
{
	UpdateTask();
}

void BaseTask::UpdateTask()
{
	if (GetID() == 0)
	{
		InsertTask();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_task", GetID(), false);
		this->GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());
	}
}


void BaseTask::InsertTask()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_task", GetID());
	cmd->PutData("owner", m_pOwner->GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());

	m_pOwner->SendUdpLog("task_accept", LOG_TASK_ACCEPT_FORMAT, GetDataId(), GetType(), GetSubType());
}

void BaseTask::DeleteTask()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_task", GetID());
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());
}

