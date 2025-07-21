#include "AccountAchievementTaskMgr.h"

#include <CLTaskProtocol.h>
#include <CLRecordClient.h>
#include <CLPlayerDataEntry.h>
#include <CLJobDataEntry.h>

#include "TaskScript.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "WSPlayer.h"



/**
*@brief 玩家等级和职业索引
*/
struct PlayerLevelAndRoleForAccAchTaskIndex
{
	PlayerLevelAndRoleForAccAchTaskIndex(UInt16 level, UInt8 occu)
		: _level(level), _occu(occu) {}

	bool operator<(const PlayerLevelAndRoleForAccAchTaskIndex& rhs) const
	{
		if (_level != rhs._level)
			return _level < rhs._level;
		if (_occu != rhs._occu)
			return _occu < rhs._occu;
		return false;
	}

	UInt16 _level;
	UInt8 _occu;
};

/**
*@brief 查询玩家角色回调
*/
class QueryPlayerRolesForAccountAchieveTaskCallback : public CLRecordCallback
{
public:
	QueryPlayerRolesForAccountAchieveTaskCallback(UInt32 accid, const std::vector<UInt32>& tasks)
		: m_AccId(accid), m_NotAcceptTasks(tasks) {}

public:
	virtual void OnFinished()
	{
		auto account = AccountMgr::Instance()->FindAccount(m_AccId);
		if (!account)
		{
			ErrorLogStream << "Can't find account(" << m_AccId << ")!" << LogStream::eos;
			return;
		}

		// 各等级对应的角色数
		std::map<UInt16, UInt32> levelToNum;
		// 等级和职业对应的角色数
		std::map<PlayerLevelAndRoleForAccAchTaskIndex, UInt32> levelAndOccuToNum;
		// 等级和基础职业对应的角色数
		std::map<PlayerLevelAndRoleForAccAchTaskIndex, UInt32> levelAndBaseOccuToNum;

		while (NextRow())
		{
			UInt16 level = GetData("level");
			UInt8 occu = GetData("occu");

			levelToNum[level]++;
			levelAndOccuToNum[PlayerLevelAndRoleForAccAchTaskIndex(level, occu)]++;
			Occupation baseOccu = JobDataEntry::GetBaseOccu(occu);
			levelAndBaseOccuToNum[PlayerLevelAndRoleForAccAchTaskIndex(level, baseOccu)]++;
		}

		TaskDataEntryMgr::TaskDataVec taskDatas;
		TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(taskDatas);

		for (auto taskData : taskDatas)
		{
			if (!taskData) continue;

			if (taskData->subType != TST_COND_ROLE_NUM_CHANGE)
			{
				continue;
			}

			if (std::find(m_NotAcceptTasks.begin(), m_NotAcceptTasks.end(), taskData->id) == m_NotAcceptTasks.end())
			{
				continue;
			}

			AccountTask* task = account->GetAccountAchievementTaskMgr().FindTask(taskData->id);
			if(!task) continue;

			if (task->GetStatus() != TASK_UNFINISH)
			{
				continue;
			}

			TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
			if (!script)
			{
				continue;
			}

			UInt16 levelCond = 0;
			UInt8 occuCond = 0;
			script->GetRoleNumChangedEventCond(levelCond, occuCond);

			// 满足条件的数量
			UInt32 meetCondNum = 0;
			if (occuCond > 0)
			{
				PlayerLevelAndRoleForAccAchTaskIndex index(levelCond, occuCond);
				{
					auto itr = levelAndOccuToNum.find(index);
					if (itr != levelAndOccuToNum.end())
					{
						meetCondNum = itr->second;
					}
				}
				{
					// 如果是基础职业，以基础职业统计为准
					auto itr = levelAndBaseOccuToNum.find(index);
					if (itr != levelAndBaseOccuToNum.end())
					{
						meetCondNum = itr->second;
					}
				}
			}
			else if(occuCond == 0 && levelCond > 0)
			{
				for (auto& it : levelToNum)
				{
					if (it.first >= levelCond)
						meetCondNum += it.second;
				}
			}
			else
			{
				continue;
			}

			if (meetCondNum > 0)
			{
				script->OnInitRoleNum(task->GetOwner(), task, meetCondNum);
			}
		}
	}

	virtual void OnFailed(UInt32 errorcode)override
	{
		ErrorLogStream << "Failed to select table t_player_info use accid(" << m_AccId << ")! errorcode=" << errorcode << LogStream::eos;
	}

private:
	UInt32 m_AccId;
	// 未接取任务
	std::vector<UInt32> m_NotAcceptTasks;
};


AccountAchievementTask::AccountAchievementTask(AccountInfo* owner, UInt32 id) 
	:AccountTask(owner, id)
{

}

AccountAchievementTask::AccountAchievementTask(AccountInfo* owner, UInt32 id, QuestVarMap questVar) 
	: AccountTask(owner, id, questVar)
{

}

AccountAchievementTask::AccountAchievementTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime)
	:AccountTask(owner, id, guid, status, questVarStr)
{
	m_FinishedTime.SetDBValue(finTime);
}

bool AccountAchievementTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry, AccountTask::QuestVarMap questVar, bool isSync)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry->id)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;
	AccountAchievementTask* task = new AccountAchievementTask(GetOwner(), taskDataEntry->id, questVar);
	task->SetQuality(0);
	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	if (isSync)
	{
		//通知新任务
		CLProtocol::SceneNotifyNewTask protocol;
		task->GetBriefInfo(protocol.task);
		InfoLogStream << "player(" << GetOwner()->GetAccID() << ")" << " in AccountAchievementTaskMgr::AcceptTask, to notify new task:" << task->GetDataId() << LogStream::eos;
		GetOwner()->SendProtocol(protocol);
	}

	/*
	if (script->CheckLevelupEvent(GetOwner()->GetLevel()))
	{
		script->OnPlayerLevelup(GetOwner(), task);
	}

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());
	*/
	OnAcceptOneTask(task);
	return true;
}

bool AccountAchievementTaskMgr::SubmitTask(AccountTask* task, bool force)
{
	if (NULL == task) { return false; }

	if (!force && TASK_FINISHED != task->GetStatus()) { return false; }

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	task->CheckGiveRewards();

	OnSubmitOneTask(task);
	return true;
}

bool AccountAchievementTaskMgr::IsTaskAcceptable(UInt32 id)
{
	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //没有这个任务

	if (!dataEntry->open) return false; //未开放

	if (!IsAccountAchievementTask(dataEntry->type)) return false;

	//等级未达到
	//if (dataEntry->triggerLevel != 0 && GetOwner()->GetLevel() < dataEntry->triggerLevel) return false;

	if (FindTask(id) != NULL) return false;

	return true;
}


UInt32 AccountAchievementTaskMgr::GetMaxTaskNum()
{
	return MAX_ACCEPT_ACHIEVEMENT_NUM;
}

//bool AccountAchievementTaskMgr::AddTask(AccountTask* task)
//{
//	if (task == NULL) return false;
//	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
//	{
//		return true;
//	}
//	return false;
//}

bool AccountAchievementTaskMgr::CheckFinishTask(UInt32 taskId)
{
	AccountTask* task = FindTask(taskId);
	if (task != NULL && (task->GetStatus() == TaskStatus::TASK_OVER || task->GetStatus() == TaskStatus::TASK_SUBMITTING))
	{
		return true;
	}
	return false;
}

void AccountAchievementTaskMgr::OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards)
{
	AccountTaskMgr::OnCheckPackageEnoughForRewardsRes(taskId, rewards);
}


void AccountAchievementTaskMgr::InitAccountAchievementTask(bool isSync)
{
	TaskDataEntryMgr::TaskDataVec tasks;
	TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(tasks);

	bool isAccept = false;
	for (UInt32 i = 0; i < tasks.size(); ++i)
	{
		if (AcceptTask(tasks[i], AccountTask::QuestVarMap(), isSync))
		{
			isAccept = true;
		}
	}
	if (isAccept)
	{
		//SetDirty();
	}
}

void AccountAchievementTaskMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player) return;

	_BeginQueryRoleLevelAndOccuInfos();

	InitAccountAchievementTask(false);
	SyncAccountAchievementTask();

	OnAdventureTeamGradeChanged();
}

void AccountAchievementTaskMgr::OnCreateAdventTeam()
{
	_BeginQueryRoleLevelAndOccuInfos();

	InitAccountAchievementTask(false);
	SyncAccountAchievementTask();
}

void AccountAchievementTaskMgr::OnPlayerLevelChanged(WSPlayer* player, UInt32 oldLvl)
{
	if (!player) return;

	_OnCheckCondRoleNumTasks(player, TaskScript::crntr_level, oldLvl);
}

void AccountAchievementTaskMgr::OnPlayerOccuChanged(WSPlayer* player)
{
	if (!player) return;

	_OnCheckCondRoleNumTasks(player, TaskScript::crntr_occu);
}

void AccountAchievementTaskMgr::OnAdventureTeamLevelChanged()
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;

		AccountTask* task = FindTask(taskData->id);
		if (task && task->GetStatus() == TASK_UNFINISH)
		{
			TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
			if (script != NULL && script->CheckAdventureTeamLevelChangedEvent())
			{
				script->OnAdventureTeamLevelChanged(task->GetOwner(), task);
			}
		}
	}
}

void AccountAchievementTaskMgr::OnAdventureTeamGradeChanged()
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;

		AccountTask* task = FindTask(taskData->id);
		if (task && task->GetStatus() == TASK_UNFINISH)
		{
			TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
			if (script != NULL && script->CheckAdventureTeamGradeChangedEvent())
			{
				script->OnAdventureTeamGradeChanged(task->GetOwner(), task);
			}
		}
	}
}

void AccountAchievementTaskMgr::OnCompleteAdventureWeeklyTask(UInt32 taskLevel)
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;
		AccountTask* task = FindTask(taskData->id);
		if (task && task->GetStatus() == TASK_UNFINISH)
		{
			TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
			if (script != NULL && script->CheckPlayerCompAdventWeeklyTaskEvent(taskLevel))
			{
				script->OnCompAdventWeeklyTask(task->GetOwner(), task, taskLevel);
			}
		}
	}
}

void AccountAchievementTaskMgr::SyncAccountAchievementTask()
{
	//发送成就数据
	CLProtocol::SceneAchievementTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}

void AccountAchievementTaskMgr::_BeginQueryRoleLevelAndOccuInfos()
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(taskDatas);
	std::vector<UInt32> notAccpetTasks;
	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;

		if (taskData->subType != TST_COND_ROLE_NUM_CHANGE)
		{
			continue;
		}

		AccountTask* task = GetOwner()->GetAccountAchievementTaskMgr().FindTask(taskData->id);
		if (!task || task->GetStatus() == TASK_UNFINISH)
		{
			notAccpetTasks.push_back(taskData->id);
		}
	}

	if (notAccpetTasks.empty())
	{
		return;
	}

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	cmd->PutCond("accid", GetOwner()->GetAccID());
	cmd->PutColumn("level");
	cmd->PutColumn("occu");
	std::ostringstream limitoss;
	limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DELETE_PLAYER_SAVE_TIME << ")";
	cmd->SetLimit(limitoss.str());
	CLRecordClient::Instance()->SendCommand(cmd, new QueryPlayerRolesForAccountAchieveTaskCallback(GetOwner()->GetAccID(), notAccpetTasks));
}

void AccountAchievementTaskMgr::_OnCheckCondRoleNumTasks(WSPlayer* player, UInt8 reason, UInt32 oldLvl)
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAccountAchievementTasks(taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;

		if (taskData->subType != TST_COND_ROLE_NUM_CHANGE)
		{
			continue;
		}

		AccountTask* task = GetOwner()->GetAccountAchievementTaskMgr().FindTask(taskData->id);
		if (!task) continue;

		if (task->GetStatus() != TASK_UNFINISH)
		{
			continue;
		}

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (!script) continue;

		if (reason == TaskScript::crntr_level)
		{
			for (UInt32 i = oldLvl + 1;i <= player->GetLevel();i++)
			{
				if (script->CheckRoleNumChangedEvent(i, player->GetOccu(), reason))
				{
					script->OnRoleNumChanged(task->GetOwner(), task, 1);
				}
			}
		}
		else
		{
			if (script->CheckRoleNumChangedEvent(player->GetLevel(), player->GetOccu(), reason))
			{
				script->OnRoleNumChanged(task->GetOwner(), task, 1);
			}
		}
	}
}
