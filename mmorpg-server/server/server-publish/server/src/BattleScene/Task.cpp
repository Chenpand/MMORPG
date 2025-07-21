#include "Task.h"

#include <AvalonStringUtil.h>
#include <CLTaskProtocol.h>
#include <CLLostDungeonProtocol.h>
#include <CLLostDungeonMissionDataEntry.h>

#include "Player.h"
#include "ScriptMgr.h"
#include "TaskScript.h"

Task::Task(Player* owner, UInt32 id) : BaseTask(owner, id)
{
	SetID(0);
	m_pDataEntry = NULL;
}

Task::Task(Player* owner, UInt32 id, QuestVarMap questVar)
	:Task(owner, id)
{
	SetID(0);
	m_QuestVars.SetQuestVarMap(questVar);
}

Task::Task(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr) : BaseTask(owner, id, guid)
{
	m_Status.SetDBValue(status);
	m_QuestVars.SetDBValue(questVarStr);
}

Task::~Task()
{
}

bool Task::SetupBase()
{
	m_pDataEntry = TaskDataEntryMgr::Instance()->FindEntry(m_DataId);

	if (m_pDataEntry == NULL) return false;

	return true;
}

RewardGroup* Task::GetRewards() const
{
	/*
	if (m_pDataEntry->rewards.empty())
	{
		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(m_pDataEntry->id);
		if (script != NULL)
		{
			return script->GetTaskRewards(GetOwner(), m_Quality);
		}
	}
	*/

	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	rewardGroup->GetFitRewards(GetOwner(), m_pDataEntry->rewards);
	rewardGroup->GetFitRewards(GetOwner(), m_pDataEntry->GetOccuItemRewards((Occupation)GetOwner()->GetOccu()));
	return rewardGroup;
}

/*
void Task::SetTaskMgrDirty()
{
	GetOwner()->GetTaskMgr().SetDirty();
}
*/

const std::string& Task::GetTargetByStatus() const
{
	return GetTargetByStatus(m_Status);
}

const std::string& Task::GetTargetByStatus(UInt8 status) const
{
	return GetTargetByStatus(GetOwner(), m_pDataEntry, status);
}

const std::string& Task::GetDescription() const
{
	return GetDescription(GetOwner(), m_pDataEntry);
}

const std::string& Task::GetDescription(Player* player, TaskDataEntry* dataEntry)
{
	return dataEntry->describe;
}

const std::string& Task::GetTargetByStatus(Player* player, TaskDataEntry* dataEntry, UInt8 status)
{
	switch(status)
	{
	case TASK_UNFINISH:
	case TASK_FAILED:
		return dataEntry->unfinishedTarget;
	case TASK_FINISHED:
		return dataEntry->finishedTarget;
	default:
		return dataEntry->initTarget;
	}	
}

void Task::GiveRewards()
{

	//added by aprilliu, 2016.05.25 对转职任务的特殊处理
	if (GetType() == TASK_TRANSFER)
	{
		if (!GetOwner()->IsInitOccu()) return;
		TaskDataEntryMgr::TaskDataVec taskVec = TaskDataEntryMgr::Instance()->GetPostTasks(m_pDataEntry->id);
		if (taskVec.size() == 0)
		{
			InfoLogStream << "in Task::GiveRewards(), task(id:" << this->GetDataId() << ") will transfer to target occupation("
				<< (UInt16)m_pDataEntry->GetTargetOccupation() << ")" << LogStream::eos;
			GetOwner()->ChangeOccu(m_pDataEntry->GetTargetOccupation());
		}
	}

	if (GetType() == TASK_AWAKEN)
	{
		TaskDataEntryMgr::TaskDataVec taskVec = TaskDataEntryMgr::Instance()->GetPostTasks(m_pDataEntry->id);
		if (taskVec.size() == 0)
		{
			InfoLogStream << "in Task::GiveRewards(), task(id:" << this->GetID() << ") awaken(1)" << LogStream::eos;
			GetOwner()->Awaken();
		}
	}

	BaseTask::GiveRewards();
}


DailyTask::DailyTask(Player* owner, UInt32 id) : Task(owner, id)
{
	m_pRewardDataEntry = NULL;
}

DailyTask::DailyTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr) : Task(owner, id, guid, status, questVarStr) { }

DailyTask::~DailyTask()
{
}

bool DailyTask::SetupBase()
{
	if (Task::SetupBase())
	{
		m_pRewardDataEntry = RewardAdapterDataEntryMgr::Instance()->FindEntry(m_pDataEntry->rewardAdapterId);
		return true;
	}
	return false;
}

void DailyTask::GiveRewards()
{
	BaseTask::GiveRewards();
	GetOwner()->IncDailyTaskScore(m_pDataEntry->GetDailyTaskScore());
	//更新活跃度
	GetOwner()->IncCounter(DAILY_ACTIVE_SCORE, m_pDataEntry->vitality);

	GetOwner()->SyncBaseInfoToWorld();
}

RewardGroup* DailyTask::GetRewards() const
{
	if (m_pRewardDataEntry == NULL)
	{
		return Task::GetRewards();
	}

	UInt32 level = (UInt32)GetVar(TASK_DAILY_LEVEL_VAR);
	if (m_pRewardDataEntry->levelRewards[level].empty())
	{
		return Task::GetRewards();
	}

	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	rewardGroup->GetFitRewards(GetOwner(), m_pRewardDataEntry->levelRewards[level]);
	rewardGroup->GetFitRewards(GetOwner(), m_pDataEntry->GetOccuItemRewards((Occupation)GetOwner()->GetBaseOccu()));
	return rewardGroup;
}

const char* DailyTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_DAILY, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

void DailyTask::InitVar(const char* name, Int64 val)
{
	if (name == NULL) return;
	std::string key(name);
	std::string value(Avalon::StringUtil::ConvertToString(val));
	m_QuestVars.SetVar(key, value);
}

/*
void DailyTask::SetTaskMgrDirty()
{
	GetOwner()->GetDailyTaskMgr().SetDirty();
}
*/

LegendTask::LegendTask(Player* owner, UInt32 id) :Task(owner, id)
{
}

LegendTask::LegendTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt8 submitCount) : Task(owner, id, guid, status, questVarStr)
{
	m_SubmitCount.SetDBValue(submitCount);
}

LegendTask::~LegendTask()
{
}

const char* LegendTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_LEGEND, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

RewardGroup* LegendTask::GetRewards() const
{
	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	rewardGroup->GetFitRewards(GetOwner(), m_pDataEntry->rewards);
	rewardGroup->GetFitRewards(GetOwner(), m_pDataEntry->GetOccuItemRewards((Occupation)GetOwner()->GetOccu()));
	return rewardGroup;
}


AchievementTask::AchievementTask(Player* owner, UInt32 id) :Task(owner, id)
{
}

AchievementTask::AchievementTask(Player* owner, UInt32 id, QuestVarMap questVar) : Task(owner, id, questVar)
{
}

AchievementTask::AchievementTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime) : Task(owner, id, guid, status, questVarStr)
{
	m_FinishedTime.SetDBValue(finTime);
}

AchievementTask::~AchievementTask()
{
}

void AchievementTask::GiveRewards()
{
	BaseTask::GiveRewards();

	if (m_pDataEntry != NULL)
	{
		GetOwner()->AddAchievementScore(m_pDataEntry->param0);
	}
}

const char* AchievementTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_ACHIEVEMENT, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

/*
void AchievementTask::OnSetStatus()
{
	OnSetVar();
}

void AchievementTask::OnSetVar()
{
	if (GetID() == 0)
	{
		InsertTask();
	}
}
*/

/*
void AchievementTask::SetTaskMgrDirty()
{
	GetOwner()->GetAchievementTaskMgr().SetDirty();
}
*/


CycleTask::CycleTask(Player* owner, UInt32 id) : Task(owner, id)
{
}

CycleTask::CycleTask(Player* owner, UInt32 id, QuestVarMap questVar) : Task(owner, id, questVar)
{
}

CycleTask::CycleTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr) : Task(owner, id, guid, status, questVarStr)
{
}

CycleTask::~CycleTask()
{
}

const char* CycleTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_CYCLE, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

RedPacketTask::RedPacketTask(Player* owner, UInt32 id) :AchievementTask(owner, id)
{
}

RedPacketTask::RedPacketTask(Player* owner, UInt32 id, QuestVarMap questVar) : AchievementTask(owner, id, questVar)
{
}

RedPacketTask::RedPacketTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime) : AchievementTask(owner, id, guid, status, questVarStr, finTime)
{
}

RedPacketTask::~RedPacketTask()
{
}

const char* RedPacketTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_RED_PACKET, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

/*
void RedPacketTask::OnSetStatus()
{
	OnSetVar();
}
*/

void RedPacketTask::GiveRewards()
{
	std::vector<ItemReward> rewards = m_pDataEntry->rewards;
	for (UInt32 i = 0; i < rewards.size(); ++i)
	{
		for (UInt32 j = 0; j < rewards[i].num; ++j)
		{
			GetOwner()->AddRedPacket((UInt16)rewards[i].id);
		}
	}
}

MasterDailyTask::MasterDailyTask(Player* owner, UInt32 id) : Task(owner, id)
{
}

MasterDailyTask::MasterDailyTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr)
	: Task(owner, id, guid, status, questVarStr)
{
}

MasterDailyTask::~MasterDailyTask()
{
}

const char* MasterDailyTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_MASTER_DAILY, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

void MasterDailyTask::GiveRewards()
{
	BaseTask::GiveRewards();
	RewardGroup* rewardGroup = GetRewards();
	const ItemRewardVec& itemRewards = rewardGroup->GetRewards();
	for (ItemRewardVec::const_iterator iter = itemRewards.begin(); iter != itemRewards.end(); iter++)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (dataEntry && dataEntry->subType == ItemSubType::ST_MASTER_ACADEMIC_VALUE) //学业成长值
		{
			GetOwner()->AddMasterDailyTaskGrowth(GetRewardsReason(), iter->num);
			//GetOwner()->AddMasterSysAcademicGrowth(GetRewardsReason(), iter->num);
		}
	}
}


void MasterDailyTask::OnSetStatus()
{
	BaseTask::OnSetStatus();
	if (m_Status == TaskStatus::TASK_FINISHED)
	{
		this->OnFinished();
	}

	InfoLogStream << "task id : " << this->GetID() << ", data id : " << this->GetDataId()
		<< ", owner : " << this->GetOwner()->GetID() << ", status : " << GetStatus() << LogStream::eos;
}

void MasterDailyTask::OnFinished()
{
	this->GiveRewards();
	if (this->GetOwner()->GetMasterDailyTaskMgr().CheckAllTasksFinished())
	{
		ObjID_t masterId = GetOwner()->GetMasterId();
		GetOwner()->SendDiscipleCompDailyTaskLog(GetOwner()->GetID(), masterId, UInt32(CURRENT_TIME.Sec()));
	}
}

void MasterDailyTask::OnSetVar()
{
	BaseTask::OnSetVar();
	InfoLogStream << "task id : " << this->GetID() << ", data id : " << this->GetDataId()  
		<< ", owner : " << this->GetOwner()->GetID() << ", parameter : " << GetVar("parameter") << LogStream::eos;
}

MasterAcademicGrowthTask::MasterAcademicGrowthTask(Player* owner, UInt32 id) : Task(owner, id)
{
}

MasterAcademicGrowthTask::MasterAcademicGrowthTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr)
: Task(owner, id, guid, status, questVarStr)
{
}

MasterAcademicGrowthTask::~MasterAcademicGrowthTask()
{
}

const char* MasterAcademicGrowthTask::GetRewardsReason()
{
	m_Reason = GetReason(SOURCE_TYPE_TASK_MASTER_ACADEMIC, GetDataId(), GetStatus());
	return m_Reason.c_str();
}

void MasterAcademicGrowthTask::GiveRewards()
{
	BaseTask::GiveRewards();
	RewardGroup* rewardGroup = GetRewards();
	const ItemRewardVec& itemRewards = rewardGroup->GetRewards();
	for (ItemRewardVec::const_iterator iter = itemRewards.begin(); iter != itemRewards.end(); iter++)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (dataEntry && dataEntry->subType == ItemSubType::ST_MASTER_ACADEMIC_VALUE) //学业成长值
		{
			GetOwner()->AddMasterAcademicTaskGrowth(GetRewardsReason(), iter->num);
			//GetOwner()->AddMasterSysAcademicGrowth(GetRewardsReason(), iter->num);
		}
	}
}

void MasterAcademicGrowthTask::OnSetStatus()
{
	BaseTask::OnSetStatus();
	if (m_Status == TaskStatus::TASK_FINISHED)
	{
		this->OnFinished();
	}

	InfoLogStream << "task id : " << this->GetID() << ", data id : " << this->GetDataId()
		<< ", owner : " << this->GetOwner()->GetID() << ", status : " << GetStatus() << LogStream::eos;
}

void MasterAcademicGrowthTask::OnFinished()
{
	this->GiveRewards();
}

void MasterAcademicGrowthTask::OnSetVar()
{
	BaseTask::OnSetVar();
	InfoLogStream << "task id : " << this->GetID() << ", data id : " << this->GetDataId()
		<< ", owner : " << this->GetOwner()->GetID() << ", parameter : " << GetVar("parameter") << LogStream::eos;
}


LostDungeonTask::LostDungeonTask(Player* owner, UInt32 id) : BaseTask(owner, id)
{
	m_Status = TASK_UNFINISH;
}

LostDungeonTask::LostDungeonTask(Player* owner, UInt32 id, UInt64 guid, TaskStatus status) : BaseTask(owner, id, guid)
{

}

LostDungeonTask::~LostDungeonTask()
{

}

bool LostDungeonTask::SetupBase()
{
	return true;
}

bool LostDungeonTask::CheckFinish()
{
	return true;
}

TaskType LostDungeonTask::GetType() const
{
	return TASK_MAIN;
}

TaskSubType LostDungeonTask::GetSubType() const
{
	return TST_INVALID;
}

const std::string& LostDungeonTask::GetName() const
{
	return m_Entry->value;
 }

UInt32 LostDungeonTask::GetLimitTime() const
{
	return 0;
}

UInt32 LostDungeonTask::GetAbandonLimitTime() const
{
	return 0;
}

UInt32 LostDungeonTask::GetMaxSubmitCount() const
{
	return 0;
}

RewardGroup* LostDungeonTask::GetRewards() const
{
	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	return rewardGroup;
}

bool LostDungeonTask::IsAnnouncement() const
{
	return false;
}

UInt32 LostDungeonTask::LostDungeonTaskType()
{
	return m_Entry->type;
}

void LostDungeonTask::SyncData()
{
	CLProtocol::SceneLostDungeonTaskDataNotify notify;
	notify.taskId = GetDataId();
	notify.status = m_Status;
	notify.vars = m_QuestVars.GetQuestVarMap();
	m_pOwner->SendProtocol(notify);
}
