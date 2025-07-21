#include "ActiveTask.h"

#include <AvalonStringUtil.h>
#include <CLTaskProtocol.h>
#include <CLActiveEventDataEntry.h>
#include "Player.h"
#include "ScriptMgr.h"
#include "TaskScript.h"
#include "ActivityScript.h"

ActiveTask::ActiveTask(Player* owner, UInt32 id)
	:m_Id(id), m_pOwner(owner) 
{
	m_pDataEntry = NULL;
	m_Status = TASK_UNFINISH;
	m_AcceptTime = 0;
	m_Uid = 0;
	m_DbFlag = df_insert;
}

ActiveTask::~ActiveTask()
{
}

void ActiveTask::SetStatus(UInt8 status)
{
	if(m_Status != status)
	{
		m_Status = status;
		SyncStatus();

		if (m_Status == TASK_FAILED)
		{
			//m_DbFlag = df_delete;
		}
		else if (m_DbFlag == df_none)
		{
			m_DbFlag = df_update;
		}	
	}
}

void ActiveTask::SyncStatus()
{
	//通知任务状态改变
	CLProtocol::SceneNotifyActiveTaskStatus protocol;
	protocol.taskId = m_Id;
	protocol.status = m_Status;
// 	InfoLogStream << "player(" << GetOwner()->GetID() << ")" << "Active Task::SyncStatus, to notify task status change, task(id:" << protocol.taskId 
// 		<< ", status:" << (UInt16)protocol.status << ")" << LogStream::eos;
	m_pOwner->SendProtocol(protocol);
}

void ActiveTask::SetData(ActiveTaskDataEntry* data)
{
	m_pDataEntry = data;
}

void ActiveTask::SetData()
{
	ActiveTaskDataEntry* data = ActiveTaskDataEntryMgr::Instance()->FindEntry(m_Id);
	if (!data)
	{
		GameErrorLogStream("Player") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") has invalid ActiveTask Data id:"
			<< m_Id << ".";
	}

	m_pDataEntry = data;
}

Int64 ActiveTask::GetVar(const char* name) const
{
	if(name == NULL) return 0;
	QuestVarMap::const_iterator iter = m_QuestVars.find(name);
	if(iter != m_QuestVars.end()) return Avalon::StringUtil::ConvertToValue<Int64>(iter->second);
	return 0;
}

void ActiveTask::SetVar(const char* name, Int64 val)
{
	if(name == NULL) return;

	std::string key(name);
	std::string value(Avalon::StringUtil::ConvertToString(val));
	m_QuestVars[key] = value;
	if (m_DbFlag == df_none)
		m_DbFlag = df_update;

	//通知任务变量改变
	CLProtocol::SceneNotifyActiveTaskVar protocol;
	protocol.taskId = m_Id;
	protocol.key = key;
	protocol.value = value;

// 	InfoLogStream << "player(" << GetOwner()->GetID() << ")" << "Active Task::SetVar, task(id:" << m_Id << ",status:"
// 		<< (UInt16)m_Status << ")'s var(key:" << protocol.key << ", value:" << protocol.value << ")" << LogStream::eos;
	m_pOwner->SendProtocol(protocol);
}

void ActiveTask::SetVarNoSync(const char* name, Int64 val)
{
	if (name == NULL) return;

	std::string key(name);
	std::string value(Avalon::StringUtil::ConvertToString(val));
	m_QuestVars[key] = value;

	if (m_DbFlag == df_none)
		m_DbFlag = df_update;

}

const char* ActiveTask::GetNameVar(const char* name) const
{
	if(name == NULL) return 0;

	QuestVarMap::const_iterator iter = m_QuestVars.find(name);
	if(iter != m_QuestVars.end()) return iter->second.c_str();
	return "";
}

void ActiveTask::SetNameVar(const char* name, const char* val)
{
	if(name == NULL) return;

	std::string key(name);
	std::string value(val);
	m_QuestVars[key] = value;

	//通知任务变量改变
	CLProtocol::SceneNotifyActiveTaskVar protocol;
	protocol.taskId = m_Id;
	protocol.key = key;
	protocol.value = value;

	InfoLogStream << "player(" << GetOwner()->GetID() << ")" << "Active Task::SetNameVar, task(id:" << m_Id << ",status:"
		<< (UInt16)m_Status << ")'s var(key:" << protocol.key << ", value:" << protocol.value << ")" << LogStream::eos;
	m_pOwner->SendProtocol(protocol);

	GetOwner()->GetActiveTaskMgr().SetDirty();
}

RewardGroup* ActiveTask::GetRewards(Player* player, ActiveTaskDataEntry* dataEntry)
{
	RewardGroup* rewardGroup = player->GetRewardGroup();
	rewardGroup->AddRewards(dataEntry->rewards);

	//vip double bonus
	if (dataEntry->vipLv > 0 && player->GetVipLvl() >= dataEntry->vipLv)
	{
		ItemRewardVec& rwds = rewardGroup->GetRewardsRef();
		for (size_t i = 0; i < rwds.size(); ++i)
		{
			rwds[i].num *= 2;
		}
	}
	
	return rewardGroup;
}

RewardGroup* ActiveTask::GetRewards() const
{
	return GetRewards(GetOwner(), m_pDataEntry);
}

void ActiveTask::AddTaskItem(UInt32 itemId, UInt32 prob)
{
	if (!m_pOwner || GetStatus() != TASK_UNFINISH)
	{
		return;
	}

	if (!Avalon::Random::SelectInHundred(prob))
	{
		return;
	}

	m_pOwner->AddItem(GetReason(LogSourceType::SOURCE_TYPE_DUNGEON_TASK_ITEM, this->GetID()).c_str(), itemId, 1);
}

bool ActiveTask::CheckGiveRewards()
{
	UInt32 ckCode = m_pOwner->CheckAddRewards(GetRewards());
	if (ErrorCode::SUCCESS == ckCode)
		return true;
	else
		return false;
}

void ActiveTask::GiveRewards()
{
	m_pOwner->AddRewards(GetReason(SOURCE_TYPE_TASK_ACTIVE, GetID(), GetStatus()).c_str(), GetRewards(), true);
}

void ActiveTask::DecodeString(std::istringstream& is)
{
	int status = 0;
	is >> status;
	m_Status = status;

	char split = 0;
	is >> split;

	is >> m_AcceptTime >> split;

	std::string values;
	std::getline(is, values, ';');

	std::vector<std::string> valuePairs;
	Avalon::StringUtil::Split(values, valuePairs, ",");

	
	for(std::vector<std::string>::iterator iter = valuePairs.begin();
		iter != valuePairs.end(); ++iter)
	{
		std::vector<std::string> pair;
		Avalon::StringUtil::Split(*iter, pair, "=");
		if(pair.size() < 2) continue;

		m_QuestVars.insert(std::make_pair(pair[0], pair[1]));
	}

	m_DbFlag = df_none;
}

void ActiveTask::EncodeString(std::ostringstream& os)
{
	os << int(m_Status) << ',' << m_AcceptTime;

	for(QuestVarMap::iterator iter = m_QuestVars.begin();
		iter != m_QuestVars.end(); ++iter)
	{
		os << ',' << iter->first << '=' << iter->second ;
	}
	os << ';';
}

void ActiveTask::SetAcceptTime(UInt32 time)
{
	m_AcceptTime = time;
}

UInt32 ActiveTask::GetAcceptTime()
{
	return m_AcceptTime;
}

void ActiveTask::UpdateTask()
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_active_task", GetUID(), true);
	cmd->PutData("status", (UInt8)GetStatus());
	cmd->PutData("parameter", EncodeQuestVar());
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());
}

void ActiveTask::InsertTask()
{
	m_Uid = CLRecordClient::Instance()->GenGuid();
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_active_task", GetUID());
	cmd->PutData("owner", GetOwner()->GetID());
	cmd->PutData("dataid", GetID());
	cmd->PutData("status", (UInt8)GetStatus());
	cmd->PutData("parameter", EncodeQuestVar());
	cmd->PutData("accepttime", m_AcceptTime);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());
}

void ActiveTask::DeleteTask()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_active_task", GetUID());
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());
}

void ActiveTask::DecodeQuestVar(std::string str)
{
	std::vector<std::string> valuePairs;
	Avalon::StringUtil::Split(str, valuePairs, ",");

	for (std::vector<std::string>::iterator iter = valuePairs.begin(); iter != valuePairs.end(); ++iter)
	{
		std::vector<std::string> pair;
		Avalon::StringUtil::Split(*iter, pair, "=");
		if (pair.size() < 2) continue;

		m_QuestVars.insert(std::make_pair(pair[0], pair[1]));
	}
}

std::string ActiveTask::EncodeQuestVar()
{
	std::string result;
	for (QuestVarMap::iterator iter = m_QuestVars.begin(); iter != m_QuestVars.end(); ++iter)
	{
		result = result + iter->first + "=" + iter->second + ',';
	}
	return result;
}

void ActiveTask::SetStatusNoSync(UInt8 status)
{
	m_Status = status;

	if (m_Status == TASK_FAILED)
	{
		//m_DbFlag = df_delete;
	}
	else if (m_DbFlag == df_none)
	{
		m_DbFlag = df_update;
	}

}

void ActiveTask::SaveToDB()
{
	if (m_DbFlag == df_insert)
	{
		InsertTask();
	}
	else if (m_DbFlag == df_update)
	{
		UpdateTask();
	}
	else if (m_DbFlag == df_delete)
	{
		DeleteTask();
	}

	m_DbFlag = df_none;
}

