#include "AccountTask.h"

#include "ScriptMgr.h"
#include "CLRecordClient.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"

#include <CLTaskProtocol.h>
#include <CLAdventWeeklyTaskAddRewardDataEntry.h>


AccountTaskQuestVarMap::AccountTaskQuestVarMap() { }
AccountTaskQuestVarMap::~AccountTaskQuestVarMap() { }

void AccountTaskQuestVarMap::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_QuestVars;
}

void AccountTaskQuestVarMap::Decode(Avalon::NetInputStream& stream)
{
	stream & m_QuestVars;
}


void AccountTaskQuestVarMap::DecodeString(std::istringstream& is)
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

void AccountTaskQuestVarMap::EncodeString(std::ostringstream& os)
{
	for (QuestVarMap::iterator iter = m_QuestVars.begin(); iter != m_QuestVars.end(); ++iter)
	{
		os << iter->first << "=" << iter->second << ',';
	}
}

void AccountTaskQuestVarMap::SetQuestVarMap(const QuestVarMap& var)
{
	m_QuestVars = var;
	SetDirty();
}

Int64 AccountTaskQuestVarMap::GetVar(const char* name) const
{
	if (name == NULL) return 0;
	QuestVarMap::const_iterator iter = m_QuestVars.find(name);
	if (iter != m_QuestVars.end()) return Avalon::StringUtil::ConvertToValue<Int64>(iter->second);
	return 0;
}

void AccountTaskQuestVarMap::SetVar(const std::string& key, const std::string& value)
{
	if (key.empty()) return;
	m_QuestVars[key] = value;
	SetDirty();
}

AccountTask::AccountTask(AccountInfo* owner, UInt32 id) : m_pOwner(owner)
{
	SetID(0);
	m_DataId.SetDBValue(id);
	m_Status.SetDBValue(TASK_UNFINISH);
	m_SubmitCount.SetDBValue(0);
	m_pDataEntry = NULL;
}

AccountTask::AccountTask(AccountInfo* owner, UInt32 id, QuestVarMap questVar)
	:AccountTask(owner, id)
{
	SetID(0);
	m_QuestVars.SetQuestVarMap(questVar);
}

AccountTask::AccountTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr) :AccountTask(owner, id)
{
	SetID(guid);
	m_Status.SetDBValue(status);
	m_QuestVars.SetDBValue(questVarStr);
}

void AccountTask::SetStatus(TaskStatus status)
{
	if (m_Status != status)
	{
		m_Status = status;

		if (TASK_ACCOUNT_ACHIEVEMENT == GetType() && status == TASK_FINISHED)
		{
			m_FinishedTime = (UInt32)CURRENT_TIME.Sec();
		}

		SyncStatus();
		OnSetStatus();
	}
}

void AccountTask::SyncStatus()
{
	//通知任务状态改变
	CLProtocol::SceneNotifyTaskStatus protocol;
	protocol.taskId = GetDataId();
	protocol.status = m_Status;
	protocol.finTime = m_FinishedTime;
	m_pOwner->SendProtocol(protocol);
}

Int64 AccountTask::GetVar(const char* name) const
{
	return m_QuestVars.GetVar(name);
}

void AccountTask::SetVar(const char* name, Int64 val)
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

void AccountTask::ResetVar()
{
	QuestVarMap var;
	m_QuestVars.SetQuestVarMap(var);
}


void AccountTask::SetNameVar(const char* name, const char* val)
{
	if (name == NULL) return;

	std::string key(name);
	std::string value(val);
	m_QuestVars.SetVar(name, val);

	OnSetVar();

	SyncVar(name, val);
}

void AccountTask::SyncVar(std::string key, std::string value)
{
	//通知任务变量改变
	CLProtocol::SceneNotifyTaskVar protocol;
	protocol.taskId = GetDataId();
	protocol.key = key;
	protocol.value = value;
	m_pOwner->SendProtocol(protocol);
}

void AccountTask::GetBriefInfo(TaskBriefInfo& info)
{
	info.id = GetDataId();
	info.status = m_Status;
	info.vars = m_QuestVars.GetQuestVarMap();
	info.finTime = m_FinishedTime;
	info.submitCount = m_SubmitCount;
}

bool AccountTask::SetupBase()
{
	m_pDataEntry = TaskDataEntryMgr::Instance()->FindEntry(m_DataId);

	if (m_pDataEntry == NULL) return false;

	m_TaskType = m_pDataEntry->type;

	return true;
}

bool AccountTask::CheckFinish()
{
	if (TASK_UNFINISH == m_Status || TASK_FINISHED == m_Status)
	{
		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(m_DataId);
		if (script == NULL) return false;
		script->CheckFinish(m_pOwner, this);
	}
	return true;
}


void AccountTask::CheckGiveRewards()
{
	if (!GetOwner()) return;

	ObjID_t roleid = GetOwner()->GetRoleID();
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(roleid);
	if (!player) return;

	CLProtocol::WorldAccountTaskCheckPakcageEnoughReq protocol;
	protocol.roleid = roleid;
	protocol.taskId = GetDataId();
	protocol.rewards = GetRewards();
	player->SendToScene(protocol);
}

ItemRewardVec AccountTask::GetRewards()
{
	return GetDataEntry()->rewards;
}

void AccountTask::GiveRewards()
{
	if (!GetOwner()) return;

	ObjID_t roleid = GetOwner()->GetRoleID();
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(roleid);
	if (!player) return;

	player->AddItem(GetRewardsReason(), GetRewards(), false);

	if (GetType() == TASK_ACCOUNT_ACHIEVEMENT && player->GetAccountInfo())
	{
		player->GetAccountInfo()->AddAccountAchievementScore(player, GetDataEntry()->param0);
	}
}

const char* AccountTask::GetRewardsReason()
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
		break;
	case TASK_ACCOUNT_ACHIEVEMENT:
		logType = SOURCE_TYPE_ACCOUNT_TASK_ACHIEVEMENT;
		break;
	case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
		logType = SOURCE_TYPE_ADVENTURE_TEAM_ACCOUNT_WEEKLY_TASK;
		break;
	default:
		break;
	}
	m_Reason = GetReason(logType, GetDataId(), GetStatus());
	return m_Reason.c_str();
}


void AccountTask::OnSetStatus()
{
	OnSetVar();
	if (m_Status == TASK_FINISHED)
	{
		//OnFinish();
	}
	else if (TASK_OVER == m_Status)
	{
		OnSubmited();
	}
	else
	{
	}
}


void AccountTask::OnSetVar()
{
	SaveDBTask();
}

void AccountTask::OnRefresh()
{
	SetStatus(TASK_INIT);
	ResetVar();
}

Int32 AccountTask::GetScriptParm()
{
	if (!GetDataEntry()) return 0;
	Int32 tmpV = Avalon::StringUtil::ConvertToValue<Int32>(GetDataEntry()->scriptParam);
	return tmpV;
}

std::string AccountTask::LogStr()
{
	std::ostringstream oss;
	oss << "Account task(" << GetID() << "," << GetDataId() << "," << (UInt32)GetStatus() << ")";
	return oss.str();
}

void AccountTask::OnSubmited()
{
	if (m_Status != TASK_OVER)
	{
		return;
	}
	if (!GetDataEntry()) return;

	if (GetDataEntry()->type == TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY)
	{
		OnSubmitedAdventWeeklyTask();
	}
}

void AccountTask::OnSubmitedAdventWeeklyTask()
{
	if (!GetDataEntry()) return;
	UInt32 taskLevel = Avalon::StringUtil::ConvertToValue<UInt32>(GetDataEntry()->params);
	GetOwner()->GetAccountAchievementTaskMgr().OnCompleteAdventureWeeklyTask(taskLevel);
}

void AccountTask::SaveDBTask()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		m_CreateTime = (UInt32)CURRENT_TIME.Sec();
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_task", GetID());
		GetDBData(cmd, false);
		cmd->PutData("accid", GetOwner()->GetAccID());
		cmd->PutData("acc_guid", GetOwner()->GetID());
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_task", GetID(), false);
		GetDBData(cmd, true);
		if(cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}

void AccountTask::DeleteDBTask()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_account_task", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);

	CLProtocol::SceneNotifyDeleteTask protocol;
	protocol.taskId = GetDataId();
	GetOwner()->SendProtocol(protocol);
}

AdventureWeeklyTask::AdventureWeeklyTask(AccountInfo* owner, UInt32 id)
	:AccountTask(owner, id)
{

}

AdventureWeeklyTask::AdventureWeeklyTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr)
	: AccountTask(owner, id, guid, status, questVarStr)
{

}

ItemRewardVec AdventureWeeklyTask::GetRewards()
{
	ItemRewardVec rewards = AccountTask::GetRewards();
	UInt8 hard = Avalon::StringUtil::ConvertToValue<UInt8>(GetDataEntry()->params);
	ItemReward itemReward;
	if (AdventWeeklyTaskAddRewardDataEntryMgr::Instance()->GetRewardByTaskHard(hard, itemReward))
	{
		rewards.push_back(itemReward);
	}
	return rewards;
}