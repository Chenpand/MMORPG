#include "AccountTaskMgr.h"

#include <CLTaskProtocol.h>
#include <CLAdventureTeamGradeDataEntry.h>
#include <CLNpcDataEntry.h>

#include "CLRecordClient.h"
#include "AccountInfo.h"
#include "ScriptMgr.h"
#include "udplog_typedef.h"
#include "WSPlayer.h"
#include "WSItemTransaction.h"

AccountTaskMgr::AccountTaskMgr()
{
	m_pOwner = NULL;
}

AccountTaskMgr::~AccountTaskMgr()
{
	CLDeleteVisitor<AccountTask> delVisitor;
	VisitTasks(delVisitor);
	m_Tasks.clear();
}

bool AccountTaskMgr::AddTask(AccountTask* task)
{
	if (task == NULL) return false;
	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
	{
		task->SaveDBTask();
		return true;
	}
	return false;
}

bool AccountTaskMgr::LoadTask(AccountTask* task)
{
	if (task == NULL) return false;
	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
	{
		return true;
	}
	return false;
}

AccountTask* AccountTaskMgr::FindTask(UInt32 id) const
{
	TaskMap::const_iterator iter = m_Tasks.find(id);
	if (iter != m_Tasks.end()) return iter->second;
	return NULL;
}


void AccountTaskMgr::RemoveTask(AccountTask* task)
{
	if (task == NULL) return;
	TaskMap::iterator iter = m_Tasks.find(task->GetDataId());
	if (iter != m_Tasks.end())
	{
		if (iter->second != NULL)
		{
			iter->second->DeleteDBTask();
			CL_SAFE_DELETE(iter->second);
		}
		m_Tasks.erase(iter);
		//SetDirty();
	}
}

void AccountTaskMgr::ClearTask()
{
	class ClearTaskVisitor : public AccountTaskVisitor
	{
	public:
		ClearTaskVisitor(){};
		~ClearTaskVisitor(){};

		bool operator()(AccountTask* task)
		{
			task->DeleteDBTask();
			return true;
		}
	};

	ClearTaskVisitor clearVisitor;
	VisitTasks(clearVisitor);

	CLDeleteVisitor<AccountTask> delVisitor;
	VisitTasks(delVisitor);
	m_Tasks.clear();

	//SetDirty();
}

UInt32 AccountTaskMgr::GetTaskNum() const
{
	return m_Tasks.size();
}

void AccountTaskMgr::VisitTasks(AccountTaskVisitor& visitor)
{
	for (TaskMap::iterator iter = m_Tasks.begin(); iter != m_Tasks.end();)
	{
		AccountTask* task = iter->second;
		++iter;
		if (!visitor(task)) return;
	}
}

void AccountTaskMgr::GetTaskList(std::vector<TaskBriefInfo>& taskList)
{
	GetAccountTaskInfoVisitor visitor(taskList);
	VisitTasks(visitor);
}

bool AccountTaskMgr::GetInitTaskBriefInfo(UInt32 id, TaskBriefInfo& info)
{
	TaskDataEntry* dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;

	info.id = id;
	info.status = TASK_INIT;

	return true;
}

void AccountTaskMgr::CheckFinish()
{
	class CheckFinishVisitor : public AccountTaskVisitor
	{
	public:
		CheckFinishVisitor(){}
		bool operator()(AccountTask* task)
		{
			task->CheckFinish();
			return true;
		}
	};

	CheckFinishVisitor visitor;
	VisitTasks(visitor);
}

UInt32 AccountTaskMgr::SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items) {
	return ErrorCode::SUCCESS;
}

void AccountTaskMgr::OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards)
{
	if (!GetOwner())
	{
		ErrorLogStream << "!GetOwner(), taskId = " << taskId << LogStream::eos;
		return;
	}
	auto task = FindTask(taskId);
	if (!task)
	{
		ErrorLogStream << "task is null, taskId = " << taskId << ", " << ACCOUNTINFO(GetOwner()) << LogStream::eos;
		return;
	}

	if (TASK_FINISHED != task->GetStatus())
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << ", TASK_FINISHED != task->GetStatus(), task guid = " << task->GetID() << ", taskId = " << taskId << ", status = " << task->GetStatus() << LogStream::eos;
		return;
	}
	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL)
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << ", task script is null, task id = " << taskId << LogStream::eos;
		return;
	}

	if (!script->CheckSubmit(GetOwner()))
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << "script check " << task->LogStr() << " submit failed!" << LogStream::eos;
		return;
	}

	script->OnSubmitted(GetOwner(), task);
	task->SetStatus(TASK_OVER);
	task->GiveRewards();

	GameInfoLogStream("Task") << ACCOUNTINFO(GetOwner()) << " submit " << task->LogStr() << LogStream::eos;

	InfoLogStream << ACCOUNTINFO(GetOwner()) << ", give rewards of task, task id = " << taskId << LogStream::eos;
}

void AccountTaskMgr::OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon)
{
	class OnOwnedNpcDiedVisitor : public AccountTaskVisitor
	{
	public:
		OnOwnedNpcDiedVisitor(UInt32 npcid, UInt8 npctype, UInt32 dungeonId) :m_NpcId(npcid), m_NpcType(npctype), m_Dungeon(dungeonId){}

		bool operator()(AccountTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckKillNpcEvent(m_NpcId, m_NpcType, m_Dungeon))
				{
					script->OnKillNpc(task->GetOwner(), task, m_NpcId, m_NpcType, m_Dungeon);
				}
			}
			return true;
		}

	private:
		UInt32 m_NpcId;
		UInt8 m_NpcType;
		UInt32 m_Dungeon;
	};
	OnOwnedNpcDiedVisitor visitor(npcid, type, dungeon);
	VisitTasks(visitor);
}

void AccountTaskMgr::SaveToDB()
{
	class SaveTaskVisitor : public AccountTaskVisitor
	{
	public:
		SaveTaskVisitor(AccountInfo* owner) : m_pOwner(owner){}
		~SaveTaskVisitor(){}

		bool operator()(AccountTask *pTask)
		{
			CLUpdateCommand	*pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_task", pTask->GetID(), true);
			pTask->GetDBData(pCmd, true);
			CLRecordClient::Instance()->SendCommand(pCmd);
			return true;
		}

	private:
		AccountInfo*	m_pOwner;
	};

	SaveTaskVisitor visitor(GetOwner());
	VisitTasks(visitor);
}

void AccountTaskMgr::OnAcceptOneTask(AccountTask* pTask)
{
	if (!pTask || !m_pOwner)
	{
		return;
	}

	m_pOwner->SendUdpLog("task_accept", LOG_TASK_ACCEPT_FORMAT, pTask->GetDataId(), pTask->GetType(), pTask->GetSubType());
}

void AccountTaskMgr::OnSubmitOneTask(AccountTask* pTask)
{
	if (!pTask || !m_pOwner)
	{
		return;
	}

	m_pOwner->SendUdpLog("task_submit", LOG_TASK_SUBMIT_FORMAT, pTask->GetDataId(), pTask->GetType(), 0);
}

WeeklyTaskMgr::WeeklyTaskMgr()
{

}

bool WeeklyTaskMgr::AcceptTask(const TaskDataEntry* taskDataEntry, bool isSync /* = true */)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;
	AccountTask* task = new AdventureWeeklyTask(GetOwner(), taskDataEntry->id);
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
		GetOwner()->SendProtocol(protocol);
	}

	OnAcceptOneTask(task);

	InfoLogStream << "player(" << GetOwner()->GetAccID() << ")" << " accept " << task->LogStr() << LogStream::eos;
	return true;
}

bool WeeklyTaskMgr::SubmitTask(AccountTask* task, bool force /* = false */)
{
	if (NULL == task) { return false; }

	if (!force && TASK_FINISHED != task->GetStatus()) 
	{ 
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << "task : " << task->LogStr() << " force : " << force << " task state : " << task->GetStatus() << LogStream::eos;
		return false; 
	}

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL)
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << "task : " << task->LogStr() << " script is null " << LogStream::eos;
		return false;
	}
	if (!script->CheckSubmit(GetOwner()))
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << " check " << task->LogStr() << " submit failed!" << LogStream::eos;
		return false;
	}

	task->CheckGiveRewards();

	OnSubmitOneTask(task);
	InfoLogStream << ACCOUNTINFO(GetOwner()) << " submited task : " << task->LogStr() << LogStream::eos;
	return true;
}

bool WeeklyTaskMgr::IsTaskAcceptable(const TaskDataEntry* dataEntry, bool bNotifyErr /* = false */)
{
	if (dataEntry == NULL) return false;

	if (!dataEntry->open) return false; //未开放

	if (!IsAccountWeeklyTask(dataEntry->type)) return false;

	if (FindTask(dataEntry->id) != NULL) return false;

	return true;
}

void WeeklyTaskMgr::RefreshTasks()
{
	_RemoveTasksByType(TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY);
	_AcceptAdventureTeamTasks();
}

UInt32 WeeklyTaskMgr::GetOverAdventureTeamTasks()
{
	class MyTaskVisitor : public AccountTaskVisitor
	{
	public:
		MyTaskVisitor() :_num(0) {}
		bool operator()(AccountTask* task)
		{
			if (!task || !task->GetDataEntry()) return true;

			if (task->GetStatus() != TASK_OVER)
			{
				return true;
			}

			if (task->GetDataEntry()->type == TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY)
			{
				_num++;
			}

			return true;
		}

		UInt32 _num;
	};

	MyTaskVisitor visitor;
	VisitTasks(visitor);
	return visitor._num;
}

void WeeklyTaskMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player) return;

	_AcceptTasks();

	CLProtocol::SceneTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}

void WeeklyTaskMgr::OnLogicWeekChange()
{
	// 每周刷新冒险队任务
	{
		RefreshTasks();
	}
}

void WeeklyTaskMgr::OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards)
{
	AccountTaskMgr::OnCheckPackageEnoughForRewardsRes(taskId, rewards);
}

void WeeklyTaskMgr::OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime)
{
	if (!dungeonData) return;

	if (score <= LS_C)
	{
		return;
	}

	if (dungeonData->subType == DST_CITY_MONSTER)
	{
		OnPlayerClearDungeonForTask(dungeonData, score, usedTime, TST_CLEAR_CITYMONSTER);
	}
	else
	{
		OnPlayerClearDungeonForTask(dungeonData, score, usedTime, TST_CLEAR_DUNGEON);
	}
}

void WeeklyTaskMgr::OnPlayerClearDungeonForTask(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime, TaskSubType taskSubType)
{
	if (dungeonData == NULL)
	{
		return;
	}
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAllAdventureTeamTasks(taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;

		if (taskData->subType != taskSubType)
		{
			continue;
		}

		AccountTask* task = FindTask(taskData->id);
		if (!task) continue;

		if (task->GetStatus() != TASK_UNFINISH)
		{
			continue;
		}

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (!script) continue;

		switch (taskData->subType)
		{
		case TST_CLEAR_DUNGEON:
			if (dungeonData->subType == DungeonSubType::DST_HELL || dungeonData->subType == DungeonSubType::DST_HELL_ENTRY || dungeonData->subType == DungeonSubType::DST_LIMIT_TIME_HELL)
			{
				if (script->CheckPlayerClearHellEvent(dungeonData->id, usedTime, score))
				{
					script->OnPlayerClearHell(task->GetOwner(), task, dungeonData, score, usedTime);
				}
			}
			else
			{
				if (script->CheckPlayerClearDungeonEvent(dungeonData->id, usedTime, score))
				{
					script->OnPlayerClearDungeon(task->GetOwner(), task, dungeonData, score, usedTime);
				}
			}
			
			break;
		case TST_CLEAR_CITYMONSTER:
			if (script->CheckPlayerClearCityMonsterEvent())
			{
				UInt32 hardType = NpcDataEntryMgr::Instance()->GetCityMonsterHardTypeByDungeonId(dungeonData->id);
				script->OnPlayerClearCityMonster(task->GetOwner(), task, hardType);
			}
			break;

		default:
			break;
		}
	}
}

void WeeklyTaskMgr::OnPlayerRemoveFatigue(UInt32 removeFatigue)
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	TaskDataEntryMgr::Instance()->GetAllAdventureTeamTasks(taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;

		AccountTask* task = FindTask(taskData->id);
		if (!task) continue;

		if (task->GetStatus() != TASK_UNFINISH)
		{
			continue;
		}

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (!script) continue;

		if (script->CheckPlayerRemoveFatigueEvent())
		{
			script->OnPlayerRemoveFatigue(task->GetOwner(), task, removeFatigue);
		}
	}
}

void WeeklyTaskMgr::_AcceptTasks()
{
	TaskDataEntryMgr::TaskDataVec taskDatas;

	if (GetOwner() && !GetOwner()->HasAdventureTeam())
	{
		return;
	}

	if(!_HasAcceptedAdventureTeamTasks())
	{
		UInt32 adventureTeamGrade = GetOwner()->GetAdventureTeamGrade();
		adventureTeamGrade = adventureTeamGrade == 0 ? 3 : adventureTeamGrade;
		auto adventureTeamGradeData = AdventureTeamGradeDataEntryMgr::Instance()->FindEntry(adventureTeamGrade);
		if (!adventureTeamGradeData)
		{
			ErrorLogStream << ACCOUNTINFO(GetOwner()) << " can't find adventure team grade data(" << GetOwner()->GetAdventureTeamGrade() << ")!" << LogStream::eos;
		}
		else
		{
			TaskDataEntryMgr::Instance()->RandAdventureTeamTasks(taskDatas, adventureTeamGradeData->optionalTaskMap);
		}
	}

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;
		AcceptTask(taskData);
	}
}

void WeeklyTaskMgr::_RemoveTasksByType(TaskType taskType)
{
	for (auto itr = m_Tasks.begin(); itr != m_Tasks.end();)
	{
		auto task = itr->second;
		if (!task || !task->GetDataEntry())
		{
			++itr;
			continue;
		}

		if (task->GetDataEntry()->type == taskType)
		{
			task->DeleteDBTask();
			delete task;
			itr = m_Tasks.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

bool WeeklyTaskMgr::_HasAcceptedAdventureTeamTasks()
{
	class MyTaskVisitor : public AccountTaskVisitor
	{
	public:
		MyTaskVisitor() :bHas(false) {}
		bool operator()(AccountTask* task)
		{
			if (!task || !task->GetDataEntry()) return true;
			if (task->GetDataEntry()->type == TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY)
			{
				bHas = true;
				return false;
			}
			return true;
		}
	
		bool bHas;
	};

	MyTaskVisitor visitor;
	VisitTasks(visitor);
	return visitor.bHas;
}

void WeeklyTaskMgr::_AcceptAdventureTeamTasks()
{
	TaskDataEntryMgr::TaskDataVec taskDatas;
	auto adventureTeamGradeData = AdventureTeamGradeDataEntryMgr::Instance()->FindEntry(GetOwner()->GetAdventureTeamGrade());
	if (!adventureTeamGradeData)
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << " can't find adventure team grade data(" << GetOwner()->GetAdventureTeamGrade() << ")!" << LogStream::eos;
	}
	else
	{
		TaskDataEntryMgr::Instance()->RandAdventureTeamTasks(taskDatas, adventureTeamGradeData->optionalTaskMap);
	}

	for (auto taskData : taskDatas)
	{
		if (!taskData) continue;
		AcceptTask(taskData);
	}
}

bool WeeklyTaskMgr::FinishTask(UInt32 id, bool force)
{
	AccountTask* task = FindTask(id);
	if (NULL == task) { return false; }

	if (TASK_FINISHED == task->GetStatus()) { return false; }

	if (!force && TASK_FINISHED != task->GetStatus()) {	return false; }

	task->SetStatus(TASK_FINISHED);

	return true;
}

/**
*@brief 任务扣除道具检查
*/
class AccountWeeklyTaskCheckItemTransaction : public ItemTransaction
{
public:
	AccountWeeklyTaskCheckItemTransaction(WSPlayer *player, UInt32 taskId)
		: ItemTransaction(player), m_TaskId(taskId) {}
	~AccountWeeklyTaskCheckItemTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		if (player->GetAccountInfo()->GetWeeklyTaskMgr().OnCheckItemForSubmitItemTaskRet(m_TaskId))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_TEAM_ACCOUNT_WEEKLY_TASK));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		player->SendNotify(GetErrorCode());
		UnLock();
	}

private:
	UInt32 m_TaskId;
};

UInt32 WeeklyTaskMgr::SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items)
{
	WSPlayer* player = GetOwner()->GetCurrentRole();
	if (!player)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	auto task = FindTask(taskId);
	if (!task)
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << " can't find task(" << taskId << ")!" << LogStream::eos;
		return ErrorCode::TASK_NOT_EXIST;
	}

	if (task->GetStatus() != TASK_UNFINISH) return ErrorCode::TASK_NOT_UNFINISH;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return ErrorCode::TASK_SCRIPT_NOT_EXIST;

	if (script->GetSetTaskItemEventNum() != items.size())
	{
		return ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
	}

	//检查是否有重复guid
	std::set<UInt64> itemGuids;
	for (auto item : items)
	{
		auto iter = itemGuids.find(item.itemGuid);
		if (iter != itemGuids.end())
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		else
		{
			itemGuids.insert(item.itemGuid);
		}
	}

	auto trans = new AccountWeeklyTaskCheckItemTransaction(player, taskId);

	std::bitset<32> markFlag;
	for (UInt32 i = 0; i < items.size(); ++i)
	{
		WorldSetTaskItemStruct& item = items[i];
		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(item.itemId);
		if (!itemData)
		{
			CL_SAFE_DELETE(trans);
			return ErrorCode::ITEM_DATA_INVALID;
		}

		UInt32 itemNum = 0;
		if (itemData->maxNum <= 1)
		{
			if (script->CheckSetTaskItemEvent((ItemQuality)itemData->color, itemData->subType, itemData->needLevel, 0, itemNum, i))
			{
				trans->AddDetailItem(item.itemGuid, item.itemId, itemNum);
				markFlag.set(i);
			}
			else
			{
				CL_SAFE_DELETE(trans);
				player->SendNotify(ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR);
				return ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
			}
		}
		else
		{
			if (script->CheckSetTaskItemEvent(ITEM_QUALITY_INVLID, 0, 0, itemData->GetKey(), itemNum, i))
			{
				trans->SetItem(itemData->GetKey(), itemNum);
				markFlag.set(i);
			}
			else
			{
				CL_SAFE_DELETE(trans);
				player->SendNotify(ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR);
				return ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
			}
		}
	}

	for (UInt8 i = 0; i < script->GetSetTaskItemEventNum(); ++i)
	{
		if (!markFlag.test(i))
		{
			CL_SAFE_DELETE(trans);
			return ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
		}
	}

	trans->SetIsWeeklySetTask(1);
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << " set task item failed, begin transaction failed." << LogStream::eos;
	}
	return ErrorCode::SUCCESS;
}

bool WeeklyTaskMgr::OnCheckItemForSubmitItemTaskRet(UInt32 taskId)
{
	auto task = FindTask(taskId);
	if (!task)
	{
		ErrorLogStream << ACCOUNTINFO(GetOwner()) << " can't find task(" << taskId << ")!" << LogStream::eos;
		return false;
	}

	task->SetStatus(TASK_FINISHED);
	WSPlayer* player = GetOwner()->GetCurrentRole();
	if (player)
	{
		CLProtocol::WorldSetTaskItemRes res;
		res.result = ErrorCode::SUCCESS;
		player->SendProtocol(res);
	}
	return true;
}