#include "TaskMgr.h"

#include <CLTaskDefine.h>
#include <CLSysNotify.h>
#include <CLSystemValueDataEntry.h>
#include <CLDungeonDailyDropDataEntry.h>
#include <CLPlayerDefine.h>
#include <CLRelationProtocol.h>

#include "TaskScript.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "EscortScript.h"
#include "Player.h"
#include "Scene.h"
#include "SSRouter.h"
#include "SSApplication.h"

TaskMgr::TaskMgr()
{
	m_pOwner = NULL;
}

TaskMgr::~TaskMgr()
{
	CLDeleteVisitor<BaseTask> delVisitor;
	VisitTasks(delVisitor);
	m_Tasks.clear();
}

bool TaskMgr::AcceptTask(UInt32 id, UInt8 quality, bool bForce, bool bNotifyErr)
{
	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //没有这个任务

 	if(bForce)
	{
		if (FindTask(id) != NULL) return false; ////已接任务
		if (!dataEntry->open) return false; //未开放 
	}
	else
	{
		if(!IsTaskAcceptable(id, true)) return false;

		//任务最大可接数量
		if (IsLimitNumberTask(dataEntry->type) && GetTaskNum() >= GetMaxTaskNum())
		{
			if (bNotifyErr) GetOwner()->SendNotify(1032);
			return false;
		}
	}

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(id);
	if(script == NULL) return false;

	if(!script->CheckAccept(GetOwner())) return false; //aprilliu:调用任务脚本进一步检查是否可接

	Task* task = new Task(GetOwner(), id);
	task->SetQuality(quality);

	if(!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}
	
	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	//机器人直接置为完成
	if (GetOwner()->TestActionFlag(ACTION_FLAG_ROBOT))
	{
		task->SetStatus(TASK_FINISHED);
	}

	if(m_AcceptableTasks.find(id) == m_AcceptableTasks.end())
	{
		//通知新任务
		CLProtocol::SceneNotifyNewTask protocol;
		task->GetBriefInfo(protocol.task);
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AcceptTask, to notify new task:" << LogStream::eos;
		GetOwner()->SendProtocol(protocol);
	}
	else
	{
		//通知状态变更
		task->SyncStatus();   //noted by aprilliu, 也就是该task的状态由原来的 未接取 变更为 已接取未完成状态
		m_AcceptableTasks.erase(id);
	}

	//限时任务处理
	if(task->GetLimitTime() != 0)
	{
		task->SetVar(TASK_DEADLINE_VAR, UInt32(CURRENT_TIME.Sec()) + task->GetLimitTime());
	}
	//放弃任务处理
	if(task->GetAbandonLimitTime() != 0)
	{
		task->SetVar(TASK_ABANDOM_TIME_VAR, UInt32(CURRENT_TIME.Sec()) + task->GetAbandonLimitTime());
	}

	if(TASK_MAIN == task->GetType())
	{
		/*
		std::ostringstream	param;
		param << task->GetID() << "_1";
		GetOwner()->SendUdpLog("guide", param.str().c_str(), "", "", "", "", "guide");
		*/
		GetOwner()->UpdateFuncNotify();
	}

	//判断等级条件任务，放最后
	if(script->CheckLevelupEvent(GetOwner()->GetLevel()))
	{
		script->OnPlayerLevelup(GetOwner(), task);
	}

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool TaskMgr::SubmitTask(BaseTask* task, bool force)
{
	if (NULL == task) { return false; }

	if (!force && TASK_FINISHED != task->GetStatus()) { return false; }

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	if (!task->CheckGiveRewards() && !force) //检查背包中是否有足够的空间存放任务奖励
	{
		GetOwner()->SendNotify(1000004);// TODO:
		return false;
	}
	task->GiveRewards();

	script->OnSubmitted(GetOwner(), task);

	RemoveTask(task);

	GetOwner()->SetTaskFinished(task->GetDataId());

	// 主线和团本任务(团本特殊处理)
	if(TASK_MAIN == task->GetType() || IsTCUnlockTask(task->GetDataId()))
	{
		GetOwner()->UpdateFuncNotify();
	}

	GameInfoLogStream("Task") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit task:"
		<< task->GetDataId() << "|" << int(task->GetType()) << "." << LogStream::eos;

	//查找新后置可接任务
	const TaskDataEntryMgr::TaskDataVec& postTasks = TaskDataEntryMgr::Instance()->GetPostTasks(task->GetDataId());
	for (TaskDataEntryMgr::TaskDataVec::const_iterator iter = postTasks.begin(); iter != postTasks.end(); ++iter)
	{
		TaskDataEntry *dataEntry = *iter;
		if (dataEntry == NULL) { continue; }

		//接取服务器自动接取的任务.
		if (dataEntry->isServerTake && IsTaskAcceptable(dataEntry->id))
		{
			this->AcceptTask(dataEntry->id, 0, false);
		}

		//下发可接取任务
		if (IsTaskAcceptable(dataEntry->id) && m_AcceptableTasks.find(dataEntry->id) == m_AcceptableTasks.end())
		{
			CLProtocol::SceneNotifyNewTask notify;
			GetInitTaskBriefInfo(dataEntry->id, notify.task);
			InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
				<< " in TaskMgr::SubmitTask, to notify init task:" << LogStream::eos;
			GetOwner()->SendProtocol(notify);
			m_AcceptableTasks.insert(dataEntry->id);
		}

		if (IsTitleTask(dataEntry->type) && dataEntry->triggerLevel > GetOwner()->GetLevel())
		{
			CLProtocol::SceneNotifyNewTask notify;
			GetInitTaskBriefInfo(dataEntry->id, notify.task);
			InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
				<< " in TaskMgr::SubmitTask, to notify init title task:" << LogStream::eos;
			GetOwner()->SendProtocol(notify);

		}
	}

	//通知删除任务
	CLProtocol::SceneNotifyDeleteTask protocol;
	protocol.taskId = task->GetDataId();
	protocol.reason = DELETE_TASK_REASON_SUBMIT;

	InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
		<< " in  TaskMgr::SubmitTask, to notify delete task(id:" << protocol.taskId << ", reason : " << (UInt16)protocol.reason << ")" << LogStream::eos;

	GetOwner()->SendProtocol(protocol);

	//added by aprilliu, 2016.04.18 增加对成功提交任务事件的回调
	GetOwner()->OnSubmitTaskSucceed(task->GetDataId(), task->GetType());

	CL_SAFE_DELETE(task);

	return true;
}

bool TaskMgr::AbandonTask(BaseTask* task)
{
	if(task == NULL) return false;
	if(task->GetDataId() == LOOP_TASKID) return false;
	if(task->GetDataId() == TRIBE_TRANS_TASKID && task->GetStatus() != TASK_FAILED) return false;

	if(task->GetAbandonLimitTime() != 0 && CURRENT_TIME.Sec() < UInt32(task->GetVar(TASK_ABANDOM_TIME_VAR)))
	{
		UInt32 leftTime = UInt32(task->GetVar(TASK_ABANDOM_TIME_VAR)) - UInt32(CURRENT_TIME.Sec());
		std::string timestr = SysNotifyMgr::Instance()->TimeToStr(leftTime);
		//modified by aprilliu,2016.06.06
		//GetOwner()->SendNotify(15, timestr.c_str());
		return false;
	}

	//added by aprilliu, 2016.05.25 增加对部分类型任务不可放弃的逻辑判断
	if (IsNotAbandonTask(task->GetType()))
	{
		//
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AbandonTask, can't abandon task(id:" << task->GetDataId() << ", type:" << task->GetType() <<"), because it is transfer task!" << LogStream::eos;
		//GetOwner()->SendNotify(1000); //TODO: 提示用户 任务不可放弃，需要策划具体配置通知消息
		return false;
	}

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if(script != NULL)
	{
		script->OnAbandomed(GetOwner(), task);
	}

	RemoveTask(task);
	
	if(IsTaskAcceptable(task->GetDataId()) && ((Task*)task)->GetDataEntry()->triggerLevel != 0)
	{
		//通知状态切换
		CLProtocol::SceneNotifyTaskStatus protocol;
		protocol.taskId = task->GetDataId();
		protocol.status = TASK_INIT;
		protocol.finTime = 0;
		//protocol.target = task->GetTargetByStatus(TASK_INIT);
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AbandonTask, to notify task status change, task(id:" << protocol.taskId
			<< ", status:" << (UInt16)protocol.status << ")" << LogStream::eos;
		GetOwner()->SendProtocol(protocol);

		//放回可接任务列表
		m_AcceptableTasks.insert(task->GetDataId());
	}
	else
	{
		//通知删除
		CLProtocol::SceneNotifyDeleteTask protocol;
		protocol.taskId = task->GetDataId();
		protocol.reason = DELETE_TASK_REASON_ABANDON;
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AbandonTask, to notify delete task(id:" << protocol.taskId << ", reason : " << (UInt16)protocol.reason << LogStream::eos;
		GetOwner()->SendProtocol(protocol);
	}

	GetOwner()->OnAbandonTask(task->GetDataId(), task->GetType());

	CL_SAFE_DELETE(task);
	return true;
}

bool TaskMgr::IsTaskAcceptable(UInt32 id, bool bNotifyErr,bool skipPreTask)
{
	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //没有这个任务
	if (dataEntry->type == TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY ||
		dataEntry->type == TASK_ACCOUNT_ACHIEVEMENT)
	{
		return false;
	}

	if (!dataEntry->open) return false; //未开放

	if (FindTask(id) != NULL)
	{
		if (bNotifyErr)
			GetOwner()->SendNotify(10, dataEntry->name.c_str());
		return false; //已接任务
	}

	//已完成
	if (GetOwner()->CheckTaskFinished(id)) return false;

	if (!skipPreTask)
	{
		//未完成前置任务
		if (dataEntry->preTask != 0 && !GetOwner()->CheckTaskFinished(dataEntry->preTask)) return false;
	}

	//等级未达到
	if (dataEntry->triggerLevel != 0 && GetOwner()->GetLevel() < dataEntry->triggerLevel) return false;

	//added by aprilliu,增加对用户职业的检查
	//if (dataEntry->occupation && GetOwner()->GetOccu() != dataEntry->occupation) return false;
	if (!dataEntry->CheckOccu(GetOwner()->GetOccupation())) return false;

	return true;
}

void TaskMgr::AcceptWithOutPreTask(UInt32 id)
{
	//查找新后置可接任务
	const TaskDataEntryMgr::TaskDataVec& postTasks = TaskDataEntryMgr::Instance()->GetPostTasks(id);
	for (TaskDataEntryMgr::TaskDataVec::const_iterator iter = postTasks.begin(); iter != postTasks.end(); ++iter)
	{
		TaskDataEntry *dataEntry = *iter;
		if (dataEntry == NULL) { continue; }

		//接取服务器自动接取的任务.
		if (dataEntry->isServerTake && IsTaskAcceptable(dataEntry->id,false,true))
		{
			this->AcceptTask(dataEntry->id, true, false);
		}

		//下发可接取任务
		if (IsTaskAcceptable(dataEntry->id,false,true) && m_AcceptableTasks.find(dataEntry->id) == m_AcceptableTasks.end())
		{
			CLProtocol::SceneNotifyNewTask notify;
			GetInitTaskBriefInfo(dataEntry->id, notify.task);
			InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
				<< " in TaskMgr::SubmitTask, to notify init task:" << LogStream::eos;
			GetOwner()->SendProtocol(notify);
			m_AcceptableTasks.insert(dataEntry->id);
		}

		if (IsTitleTask(dataEntry->type) && dataEntry->triggerLevel > GetOwner()->GetLevel())
		{
			CLProtocol::SceneNotifyNewTask notify;
			GetInitTaskBriefInfo(dataEntry->id, notify.task);
			InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
				<< " in TaskMgr::SubmitTask, to notify init title task:" << LogStream::eos;
			GetOwner()->SendProtocol(notify);

		}
	}
}

UInt32 TaskMgr::GetMaxTaskNum()
{
	return MAX_ACCEPT_TASK_NUM;
}

UInt32 TaskMgr::GetTaskNum() const
{
	UInt32 reslut = 0;

	TaskMap::const_iterator iter = m_Tasks.begin();
	while (iter != m_Tasks.end())
	{
		if (IsLimitNumberTask(iter->second->GetType()))
		{
			reslut += 1;
		}
		++iter;
	}
	return reslut;
}

void TaskMgr::GetTaskBriefList(std::vector<TaskBriefInfo>& taskList)
{
	GetTaskList(taskList);
	//获取可接任务数据
	for (TaskIdSet::iterator iter = m_AcceptableTasks.begin(); iter != m_AcceptableTasks.end(); ++iter)
	{
		//TaskInfo info;
		//GetInitTaskInfo(*iter, info);
		TaskBriefInfo info;
		GetInitTaskBriefInfo(*iter, info);
		taskList.push_back(info);
	}
}

//转职任务的主线任务
const UInt32 CHANGE_OCCU_TASK_ID = 2271;

UInt32 TaskMgr::FinishAllTask(TaskType taskType, UInt32 taskId)
{
	if (taskId != 0)
	{
		TaskDataEntry* taskData = TaskDataEntryMgr::Instance()->FindEntry(taskId);
		if (taskData == NULL) return ErrorCode::SUCCESS;
		if (taskType != TaskType::TASK_MAX && taskData->type != taskType) return ErrorCode::SUCCESS;
		if (GetOwner()->CheckTaskFinished(taskId)) return ErrorCode::SUCCESS;
	}

	while (!m_Tasks.empty())
	{
		bool isBreak = false;
		TaskMap::iterator iter = m_Tasks.begin();
		while (iter != m_Tasks.end())
		{
			TaskMap::iterator tmpIter = iter;
			++iter;

			if (taskType != TaskType::TASK_MAX && tmpIter->second->GetType() != taskType)
			{
				continue;
			}

			UInt32 tmpTaskId = tmpIter->second->GetDataId();

			//如果是转职任务的主线任务留着不完成
			if (tmpTaskId == CHANGE_OCCU_TASK_ID)
			{
				AcceptWithOutPreTask(tmpTaskId);
				continue;
			}

			if (!SubmitTask(tmpIter->second, true))
			{
				isBreak = true;
				break;
			}

			if (tmpTaskId == taskId)
			{
				isBreak = true;
				break;
			}
		}
		if (isBreak)
			break;

		bool flag = false;
		TaskIdSet::iterator acceptIter = m_AcceptableTasks.begin();
		while (acceptIter != m_AcceptableTasks.end())
		{
			TaskIdSet::iterator tmpIter = acceptIter;
			++acceptIter;
			UInt32 taskId = *tmpIter;
			TaskDataEntry* tmpTaskData = TaskDataEntryMgr::Instance()->FindEntry(taskId);
			if (tmpTaskData == NULL) break;

			if (taskType != TaskType::TASK_MAX && tmpTaskData->type != taskType)
			{
				continue;
			}

			if (!AcceptTask(*tmpIter, 0, true))
			{
				break;
			}
			else
			{
				//第一次接到转职任务要打标记
				if (taskId == CHANGE_OCCU_TASK_ID)
				{
					flag = true;
				}
			}
			}

		isBreak = true;
		iter = m_Tasks.begin();
		while (iter != m_Tasks.end())
		{
			TaskMap::iterator tmpIter = iter;
			++iter;

			//如果是转职任务并且第一次接到就跳过
			if (tmpIter->second->GetDataId() == CHANGE_OCCU_TASK_ID && !flag)
			{
				continue;
			}

			if (taskType == TaskType::TASK_MAX || tmpIter->second->GetType() == taskType)
			{
				isBreak = false;
			}
		}
		if (isBreak)
			break;

	}
	return ErrorCode::SUCCESS;
}

void TaskMgr::OnChangeOccu()
{
	BaseTaskMgr::OnChangeOccu();
	//转职时可能会有因旧职业没法接的任务，所以调下online刷新下
	OnOnline(false);
}

void TaskMgr::OnHeartbeat(const Avalon::Time& now, int tickType)
{
	class OnTickVisitor : public BaseTaskVisitor
	{
	public:
		OnTickVisitor(UInt32 now):m_Now(now){}

		bool operator()(BaseTask* task)
		{
			if(task->GetStatus() == TASK_UNFINISH && task->GetLimitTime() != 0)
			{
				UInt32 deadline = UInt32(task->GetVar(TASK_DEADLINE_VAR));
				if(m_Now > deadline)
				{
					TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
					if(script != NULL)
					{
						script->OnTimeout(task->GetOwner());
						return true;
					}
				}
			}

			// by huchenhui
			/*
			if(task->GetType() == TASK_QUESTION && !ActivityMgr::Instance()->IsInActivity("dati"))
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetID());
				if(script != NULL)
				{
					script->OnTimeout(task->GetOwner());
					return true;
				}
			}
			*/
			
			if(task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if(script != NULL)
				{
					UInt32 uCompleteTime = script->GetTimeEvent();
					if(0 != uCompleteTime && m_Now >= (UInt32(task->GetVar(TASK_ACCEPTTIME_VAL)) + uCompleteTime))
						task->SetStatus(TASK_FINISHED);
				}
			}

			return true;
		}
	private:
		UInt32	m_Now;
	};

	if(tickType == LOGICTICK_SECOND)
	{
		OnTickVisitor visitor(UInt32(now.Sec()));
		VisitTasks(visitor);
	}
}

void TaskMgr::OnOnline(bool bDayChanged)
{
	m_AcceptableTasks.clear();

	//上线时检查可接任务列表
	UInt16 level = GetOwner()->GetLevel();
	UInt16 maxLevel = PlayerDataEntryMgr::Instance()->GetMaxLevel();

	for(UInt16 i = 1; i <= level; ++i)
	{
		const TaskDataEntryMgr::TaskDataVec& lvlTasks = TaskDataEntryMgr::Instance()->GetLvlTasks(i);

		for(TaskDataEntryMgr::TaskDataVec::const_iterator iter = lvlTasks.begin(); iter != lvlTasks.end(); ++iter)
		{
			TaskDataEntry* dataEntry = *iter;
			if ((level == i) && IsServerTake(dataEntry->type) && dataEntry->isServerTake)
			{
				AcceptTask(dataEntry->id, 0, false, false);
			}
			else if (IsTaskAcceptable(dataEntry->id))
			{
				m_AcceptableTasks.insert(dataEntry->id);
			}
		}
	}

	//发送任务数据
	CLProtocol::SceneTaskList taskList;
	GetTaskList(taskList.tasks);

	//获取可接任务数据
	for(TaskIdSet::iterator iter = m_AcceptableTasks.begin(); iter != m_AcceptableTasks.end(); ++iter)
	{
		//TaskInfo info;
		//GetInitTaskInfo(*iter, info);
		TaskBriefInfo info;
		GetInitTaskBriefInfo(*iter, info);
		taskList.tasks.push_back(info);
	}

	//不能接取的称号任务
	for (UInt16 i = level + 1; i <= maxLevel; ++i)
	{
		const TaskDataEntryMgr::TaskDataVec& lvlTasks = TaskDataEntryMgr::Instance()->GetLvlTasks(i);
		for (TaskDataEntryMgr::TaskDataVec::const_iterator iter = lvlTasks.begin(); iter != lvlTasks.end(); ++iter)
		{
			TaskDataEntry* dataEntry = *iter;
			if (IsTitleTask(dataEntry->type) &&
				(dataEntry->preTask == 0 || GetOwner()->CheckTaskFinished(dataEntry->preTask)))
			//if (IsTitleTask(dataEntry->type) && dataEntry->preTask == 0)
			{
				TaskBriefInfo info;
				GetInitTaskBriefInfo(dataEntry->id, info);
				taskList.tasks.push_back(info);
			}
		}
	}
	GetOwner()->SendProtocol(taskList);
}

void TaskMgr::OnLevelup()
{
	UInt16 level = GetOwner()->GetLevel();

	class OnLevelupVisitor : public BaseTaskVisitor
	{
	public:
		OnLevelupVisitor(UInt16 level):m_Level(level){}

		bool operator()(BaseTask* task)
		{
			if(task->GetStatus() == TASK_UNFINISH && IsAchievementStatus(task->GetType(), task->GetStatus()))
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if(script != NULL && script->CheckLevelupEvent(m_Level))
				{
					script->OnPlayerLevelup(task->GetOwner(), task);
				}
			}
			return true;
		}
	private:
		UInt16	m_Level;
	};
	OnLevelupVisitor visitor(level);
	VisitTasks(visitor);

	for(UInt16 i = 1; i <= level; ++i)
	{
		const TaskDataEntryMgr::TaskDataVec& lvlTasks = TaskDataEntryMgr::Instance()->GetLvlTasks(i);

		for(TaskDataEntryMgr::TaskDataVec::const_iterator iter = lvlTasks.begin();
			iter != lvlTasks.end(); ++iter)
		{
			TaskDataEntry* dataEntry = *iter;

			if ((level == i) && IsServerTake(dataEntry->type) && dataEntry->isServerTake)
			{
				AcceptTask(dataEntry->id, 0, false);
			}
			else if(IsTaskAcceptable(dataEntry->id)
				&& m_AcceptableTasks.find(dataEntry->id) == m_AcceptableTasks.end())
			{
				CLProtocol::SceneNotifyNewTask notify;
				GetInitTaskBriefInfo(dataEntry->id, notify.task);
				InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
					<< " in TaskMgr::OnLevelup, to notify init task : " << LogStream::eos;
				GetOwner()->SendProtocol(notify);

				m_AcceptableTasks.insert(dataEntry->id);
			}
		}
	}

	//adder by huchenhui 2016.06.29
	//InitAchievementTask();

}
DailyTaskMgr::DailyTaskMgr()
{
	m_TaskPoint = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DAILY_TASK_POINT);
}


bool DailyTaskMgr::AcceptTask(UInt32 id, UInt8 quality, bool bForce)
{
	if (!IsTaskAcceptable(id, true)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false; //aprilliu:调用任务脚本进一步检查是否可接

	DailyTask* task = new DailyTask(GetOwner(), id);
	task->SetQuality(quality);

	//日常任务需要保存接受任务时的玩家等级.用于领取奖励.
	task->InitVar(TASK_DAILY_LEVEL_VAR, (UInt64)GetOwner()->GetLevel());

	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	//ScriptMgr::Instance()->GetPlayerScript()->OnDailyTaskAccepted(GetOwner(), task);

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool DailyTaskMgr::SubmitTask(BaseTask* task, bool force)
{
	if (NULL == task) { return false; }

	if (!force && TASK_FINISHED != task->GetStatus()) { return false; }

	if (!IsDailyTask(task->GetType())) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	if (!task->CheckGiveRewards()) //检查背包中是否有足够的空间存放任务奖励
	{
		GetOwner()->SendNotify(1000004);// TODO:
		return false;
	}
	task->GiveRewards();

	script->OnSubmitted(GetOwner(), task);
	
	task->SetStatus(TASK_OVER);
	//ScriptMgr::Instance()->GetPlayerScript()->OnDailyTaskSubmitted(GetOwner(), task);

	GetOwner()->OnDailyTaskSubmitted(task->GetDataId(), task->GetType(), ((DailyTask*)task)->GetSubType());
	GetOwner()->OnSubmitTaskSucceed(task->GetDataId(), task->GetType());

	return true;
}

bool DailyTaskMgr::SubmitAllTask()
{
	bool isManager = false;
	GetOwner()->GetVipValue(VipPrivilegeType::VIP_PRIVILEGE_DAILY_TASK_MANAGER, isManager);
	if (isManager) return false;

	std::vector<DailyTask*> taskList = GetUnfinishedTasks();
	//计算需要的点卷
	UInt32 taskPoint = m_TaskPoint * taskList.size();
	//查看点卷是否足够
	if (GetOwner()->GetPointBoth() < taskPoint) return false;

	//完成未完成任务
	for (UInt32 i = 0; i < taskList.size(); ++i)
	{
		if (!SubmitTask(taskList[i], true))
		{
			return false;
		}
		//消耗点卷
		GetOwner()->RemovePointBoth(GetReason(LogSourceType::SOURCE_TYPE_TASK_DAILY, taskList[i]->GetDataId(), taskList[i]->GetStatus()).c_str(), m_TaskPoint);
	}
	return true;
}

void DailyTaskMgr::SetAllTaskStatus()
{
	std::vector<DailyTask*> taskList = GetUnfinishedTasks();
	for (UInt32 i = 0; i < taskList.size(); ++i)
	{
		taskList[i]->SetStatus(TaskStatus::TASK_FINISHED);
	}
}

bool DailyTaskMgr::IsTaskAcceptable(UInt32 id, bool bNotifyErr)
{
	if (FindTask(id) != NULL) return false; ////已接任务

	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //没有这个任务

	if (!dataEntry->open) return false; //未开放

	if (!IsDailyTask(dataEntry->type)) return false; //任务类型

	return true;
}

UInt32 DailyTaskMgr::GetMaxTaskNum()
{
	return MAX_ACCEPT_TASK_NUM;
}

/**
*@brief 玩家上线
*/
void DailyTaskMgr::OnOnline(bool bDayChanged)
{
	// adder by huchenhui 2017.06.26 检查当前等级的日常是不是都接取了,没接取的接取
	UplevelAcceptTask(GetOwner()->GetLevel());

	// adder by huchenhui 2016.06.23
	SyncDailyTask();
}

void DailyTaskMgr::OnLevelup()
{
	if (GetTaskNum() <= 0)
	{
		RefreshDailyTask();
	}
	else
	{
		UplevelAcceptTask(GetOwner()->GetLevel());
	}
}

void DailyTaskMgr::RefreshDailyTask()
{
	//放弃所有日常任务
	ClearTask();

	TaskDataEntryMgr::TaskDataVec taskVec;
	//TaskDataEntryMgr::Instance()->GetDailyTasks(TASK_DAILY, GetOwner()->GetLevel(), taskVec);
	RandomDailyTask(taskVec);

	UInt32 size = taskVec.size();
	bool isAccept = false;
	for (UInt32 i = 0; i < size; i++)
	{
		if (AcceptTask(taskVec[i]->id, 0, false))
			isAccept = true;
	}

	if (isAccept)
	{
		//SetDirty();

		//下发日常任务
		SyncDailyTask();
	}

	//重置积分
	GetOwner()->ResetDailyTaskScore();
	GetOwner()->ResetDailyTaskRewardMask();

	GetOwner()->SyncBaseInfoToWorld();
}

void DailyTaskMgr::SyncDailyTask()
{
	//发送日常任务数据
	CLProtocol::SceneDailyTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}

void DailyTaskMgr::RandomPoolIndex(UInt32 maxIndex, UInt32 size, std::vector<UInt32>& indexs)
{
	if (maxIndex == 0) return;
	std::vector<UInt32> indexVec;
	for (UInt32 i = 0; i < maxIndex; ++i)
	{
		indexVec.push_back(i);
	}

	if (maxIndex <= size)
	{
		indexs = indexVec;
		return;
	}

	for (UInt32 i = 0; i < size; ++i)
	{
		UInt32 ranNum = Avalon::Random::RandBetween(0, maxIndex - i - 1);
		indexs.push_back(indexVec[ranNum]);
		indexVec[ranNum] = indexVec[maxIndex - i - 1];
	}
}

void DailyTaskMgr::RandomDailyTask(TaskDataEntryMgr::TaskDataVec& tasks)
{
	//在升级表中获取对应等级的随机限定
	std::vector<DailyTaskSelector> selector = GetOwner()->GetDailyTaskSelector();
	std::vector<UInt32> poodIndexs;

	//必出
	TaskDataEntryMgr::DailyTaskVec sureTasks = TaskDataEntryMgr::Instance()->GetSureDailyTasks(GetOwner()->GetLevel());
	for (UInt32 i = 0; i < sureTasks.size(); ++i)
	{
		if (sureTasks[i] == NULL) continue;
		tasks.push_back(sureTasks[i]);
	}

	//循环每个选择器
	for (UInt32 i = 0; i < selector.size(); ++i)
	{
		//根据池类型选择随机池
		TaskDataEntryMgr::DailyTaskPool pool = TaskDataEntryMgr::Instance()->GetDailyTaskPool(GetOwner()->GetLevel(), selector[i].type);
		poodIndexs.clear();

		//获取池子
		RandomPoolIndex(pool.size(), selector[i].num, poodIndexs);

		for (UInt32 j = 0; j < poodIndexs.size(); ++j)
		{
			//在每个类型的随机池中随机任务
			UInt32 index = poodIndexs[j];
			UInt32 max = pool[index].dailyTasks.size();
			//随机真正的任务
			UInt32 ranNum = Avalon::Random::RandBetween(0, max - 1);
			if (pool[index].dailyTasks[ranNum] == NULL) continue;
			tasks.push_back(pool[index].dailyTasks[ranNum]);
		}
	}
}

std::vector<DailyTask*> DailyTaskMgr::GetUnfinishedTasks()
{
	std::vector<DailyTask*> result;
	class GetUnfinishedTaskVisitor : public BaseTaskVisitor
	{
	public:
		GetUnfinishedTaskVisitor(std::vector<DailyTask*>& taskList) : m_TaskList(taskList){}
		~GetUnfinishedTaskVisitor(){};

		bool operator()(BaseTask* task)
		{
			DailyTask* tmpTask = (DailyTask*)task;
			if (tmpTask->GetStatus() == TASK_UNFINISH && tmpTask->GetSubType() == TST_DAILY_TASK)
			{
				m_TaskList.push_back(tmpTask);
			}
			return true;
		}

	private:
		std::vector<DailyTask*>& m_TaskList;
	};

	GetUnfinishedTaskVisitor vis(result);
	VisitTasks(vis);
	return result;
}


void DailyTaskMgr::UplevelAcceptTask(UInt16 level)
{
	TaskDataEntryMgr::TaskDataVec tasks;

	//必出
	TaskDataEntryMgr::DailyTaskVec sureTasks = TaskDataEntryMgr::Instance()->GetSureDailyTasks(level);
	for (UInt32 i = 0; i < sureTasks.size(); ++i)
	{
		if (sureTasks[i] == NULL) continue;
		if (sureTasks[i]->triggerLevel == level)
		{
			tasks.push_back(sureTasks[i]);
		}
	}

	UInt32 size = tasks.size();
	for (UInt32 i = 0; i < size; i++)
	{
		if (AcceptTask(tasks[i]->id, 0, false))
		{
			BaseTask* task = FindTask(tasks[i]->id);
			//通知新任务
			CLProtocol::SceneNotifyNewTask protocol;
			task->GetBriefInfo(protocol.task);
			InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
				<< "in DailyTaskMgr::UplevelAcceptTask, to notify new task:" << LogStream::eos;
			GetOwner()->SendProtocol(protocol);
		}
	}



	/*
	class DailyTaskSelectorVisitor : public BaseTaskVisitor
	{
	public:
		DailyTaskSelectorVisitor(std::vector<DailyTaskSelector>& selector) : m_Selector(selector){}
		~DailyTaskSelectorVisitor(){};

		bool operator()(BaseTask* task)
		{
			DailyTask* tmpTask = (DailyTask*)task;
			if (tmpTask->GetPoolType() == DAILY_POOL_INVALID || tmpTask->GetPoolType() == DAILY_POOL_SURE)
			{
				return;
			}

			std::vector<DailyTaskSelector>::iterator iter = m_Selector.begin();
			while (iter != m_Selector.end())
			{
				std::vector<DailyTaskSelector>::iterator tmpIter = iter;
				++iter;
				if (tmpIter->type == tmpTask->GetPoolType())
				{
					tmpIter->num--;
					if (tmpIter->num == 0)
						m_Selector.erase(tmpIter);
				}
			}
		}

	private:
		std::vector<DailyTaskSelector>& m_Selector;
	};
	*/

	/*
	//循环每个选择器
	for (UInt32 i = 0; i < selector.size(); ++i)
	{
		//根据池类型选择随机池
		TaskDataEntryMgr::DailyTaskPool pool = TaskDataEntryMgr::Instance()->GetDailyTaskPool(GetOwner()->GetLevel(), selector[i].type);
		poodIndexs.clear();



		//获取池子
		RandomPoolIndex(pool.size(), selector[i].num, poodIndexs);

		for (UInt32 j = 0; j < poodIndexs.size(); ++j)
		{
			//在每个类型的随机池中随机任务
			UInt32 index = poodIndexs[j];
			UInt32 max = pool[index].dailyTasks.size();
			//随机真正的任务
			UInt32 ranNum = Avalon::Random::RandBetween(0, max - 1);
			if (pool[index].dailyTasks[ranNum] == NULL) continue;
			tasks.push_back(pool[index].dailyTasks[ranNum]);
		}
	}
	*/
}

bool LegendTaskMgr::Init()
{
	class DungeonDailyDropVisitor :public Avalon::DataEntryVisitor<DungeonDailyDropDataEntry>
	{
	public:
		bool operator()(DungeonDailyDropDataEntry* dataEntry)
		{
			std::string counterName = DUNGEON_DAILY_DROP + Avalon::StringUtil::ConvertToString(dataEntry->id);
			CounterCfg::Instance()->RegCounter(counterName.c_str(), COUNTER_CYCLE_DAY, 6);
			return true;
		}
	};

	DungeonDailyDropVisitor vistor;
	DungeonDailyDropDataEntryMgr::Instance()->Visit(vistor);
	return true;
}

bool LegendTaskMgr::AddTask(BaseTask* task)
{
	if (task == NULL) return false;
	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
	{
		return true;
	}
	return false;
}

bool LegendTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry, bool isSync)
{
	if (taskDataEntry == NULL) return false;

	if (!IsTaskAcceptable(taskDataEntry)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;
	
	LegendTask* task = new LegendTask(GetOwner(), taskDataEntry->id);
	task->SetQuality(0);
	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	//已经完成的设置状态,入库
	/*
	if (GetOwner()->CheckTaskFinished(taskDataEntry->id))
	{
		task->SetStatus(TASK_OVER);
	}
	*/

	if (isSync)
	{
		//通知新任务
		CLProtocol::SceneNotifyNewTask protocol;
		task->GetBriefInfo(protocol.task);
		GetOwner()->SendProtocol(protocol);
	}

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool LegendTaskMgr::SubmitTask(BaseTask* task, bool force)
{
	if (NULL == task) { return false; }
	if (!force && TASK_FINISHED != task->GetStatus()) { return false; }

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	if (!task->CheckGiveRewards()) //检查背包中是否有足够的空间存放任务奖励
	{
		GetOwner()->SendNotify(1000004);// TODO:
		return false;
	}
	task->GiveRewards();

	script->OnSubmitted(GetOwner(), task);

	GameInfoLogStream("Task") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit task:"
		<< task->GetDataId() << "|" << int(task->GetType()) << "." << LogStream::eos;

	task->IncSubmitCount();

	task->SetStatus(TASK_OVER);

	GetOwner()->OnSubmitLegendTaskSucceed(task);
	GetOwner()->OnSubmitTaskSucceed(task->GetDataId(), task->GetType());

	//重置任务
	ResetTask(task);

	return true;
}

void LegendTaskMgr::ResetTask(BaseTask* task)
{
	if (task == NULL) return;
	InfoLogStream << PLAYERINFO(GetOwner()) << " in  LegendTaskMgr::SubmitTask, to notify delete task(id:" << task->GetDataId() << ", submitCount = " << task->GetSubmitCount() << ")" << LogStream::eos;

	if (task->GetMaxSubmitCount() == 0 || task->GetSubmitCount() < task->GetMaxSubmitCount())
	{
		task->ResetVar();
		task->SetStatus(TaskStatus::TASK_UNFINISH);

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (script == NULL) return;

		//有些任务通过脚本设置直接完成
		script->OnAccepted(GetOwner(), task);

		GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());
	}

	CLProtocol::SceneResetTaskSync sync;
	task->GetBriefInfo(sync.info);
	GetOwner()->SendProtocol(sync);
}

bool LegendTaskMgr::IsTaskAcceptable(TaskDataEntry* taskData)
{
	if (!IsLegendTask(taskData->type)) return false;

	BaseTask* task = FindTask(taskData->id);
	if (task != NULL) return false;

	if (!taskData->open) return false; //未开放

	//等级未达到
	if (taskData->triggerLevel != 0 && GetOwner()->GetLevel() < taskData->triggerLevel) return false;

	if (!taskData->CheckOccu(GetOwner()->GetOccupation())) return false;

	return true;
}

void LegendTaskMgr::GetLegendTasks(std::vector<TaskBriefInfo>& tasks)
{
	GetTaskList(tasks);
}

bool LegendTaskMgr::CheckFinishTask(UInt32 taskId)
{
	BaseTask* task = FindTask(taskId);
	if (task == NULL)
	{
		return false;
	}

	if ((task->GetStatus() == TaskStatus::TASK_OVER || task->GetStatus() == TaskStatus::TASK_SUBMITTING) ||
		(task->GetSubmitCount() > 0))
	{
		return true;
	}
	return false;
}

void LegendTaskMgr::OnOnline(bool bDayChanged)
{
	InitLegendTasks(false);

	CLProtocol::SceneLegendTaskListRes sync;
	GetTaskList(sync.tasks);

	// 2018.12.04 by huchenhui 查传奇之路未解锁BUG
	for (UInt32 i = 0; i < sync.tasks.size(); ++i)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << " in LegendTaskMgr::OnOnline, sync task(id:" << sync.tasks[i].id << ", status = " << sync.tasks[i].status << ")" << LogStream::eos;
	}

	GetOwner()->SendProtocol(sync);

	TaskMap::iterator iter = m_Tasks.begin();
	while (iter != m_Tasks.end())
	{
		BaseTask* task = iter->second;
		if (task != NULL && task->GetSubmitCount() == 0 && task->GetMaxSubmitCount() == 0 && task->GetStatus() == TaskStatus::TASK_OVER)
		{
			task->IncSubmitCount();
			ResetTask(task);
		}
		++iter;
	}

	if (bDayChanged)
	{
		OnLogicDayChanged();
	}
}

void LegendTaskMgr::OnLogicDayChanged()
{
	TaskMap::iterator iter = m_Tasks.begin();
	while (iter != m_Tasks.end())
	{
		BaseTask* task = iter->second;
		if (task != NULL && task->GetSubmitCount() > 0 && task->GetMaxSubmitCount() > 0)
		{
			task->ResetSubmitCount();
			ResetTask(task);
		}
		++iter;
	}
}

void LegendTaskMgr::OnLevelup()
{
	InitLegendTasks();
}

void LegendTaskMgr::InitLegendTasks(bool isSync)
{
	TaskDataEntryMgr::TaskDataVec taskDatas = TaskDataEntryMgr::Instance()->GetLegendTasks(GetOwner()->GetLevel());
	UInt32 size = taskDatas.size();
	for (UInt32 i = 0; i < size; ++i)
	{
		AcceptTask(taskDatas[i], isSync);
	}
}

bool AchievementTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry, Task::QuestVarMap questVar, bool isSync)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry->id)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;
	AchievementTask* task = new AchievementTask(GetOwner(), taskDataEntry->id, questVar);
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

	if (script->CheckLevelupEvent(GetOwner()->GetLevel()))
	{
		script->OnPlayerLevelup(GetOwner(), task);
	}

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool AchievementTaskMgr::AcceptTask(TaskDataEntry * taskDataEntry, bool isSync)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry->id)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;
	AchievementTask* task = new AchievementTask(GetOwner(), taskDataEntry->id);
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

	if (script->CheckLevelupEvent(GetOwner()->GetLevel()))
	{
		script->OnPlayerLevelup(GetOwner(), task);
	}

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool AchievementTaskMgr::SubmitTask(BaseTask* task, bool force)
{
	if (NULL == task) { return false; }

	if (!force && TASK_FINISHED != task->GetStatus()) { return false; }

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	if (!task->CheckGiveRewards()) //检查背包中是否有足够的空间存放任务奖励
	{
		GetOwner()->SendNotify(1000004);// TODO:
		return false;
	}
	task->GiveRewards();

	script->OnSubmitted(GetOwner(), task);

	//GetOwner()->SetTaskFinished(task->GetDataId());

	GameInfoLogStream("Task") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit task:"
		<< task->GetDataId() << "|" << int(task->GetType()) << "." << LogStream::eos;

	task->SetStatus(TASK_OVER);

	/*
	bool isAccept = false;
	const TaskDataEntryMgr::TaskDataVec& postTasks = TaskDataEntryMgr::Instance()->GetPostAchievementTasks(task->GetDataId());
	for (TaskDataEntryMgr::TaskDataVec::const_iterator iter = postTasks.begin(); iter != postTasks.end(); ++iter)
	{
		TaskDataEntry *dataEntry = *iter;
		if (dataEntry == NULL) continue;

		isAccept = AcceptTask(dataEntry, task->GetQuestVar());
	}
	*/

	GetOwner()->OnSubmitTaskSucceed(task->GetDataId(), task->GetType());

	/*
	if (isAccept)
	{
		CLProtocol::SceneNotifyDeleteTask protocol;
		protocol.taskId = task->GetDataId();
		protocol.reason = DELETE_TASK_REASON_SUBMIT;
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in  TaskMgr::OnAchievementSubmitted, to notify delete Achievement task(id:" << protocol.taskId << ", reason : " << (UInt16)protocol.reason << LogStream::eos;
		GetOwner()->SendProtocol(protocol);

		RemoveTask(task);

		CL_SAFE_DELETE(task);
	}
	*/

	return true;
}


void AchievementTaskMgr::OnOnline(bool bDayChanged)
{
	//adder by huchenhui 2016.06.27
	InitAchievementTask(false);
	SyncAchievementTask();

	//adder by huchenhui 20190108  特殊处理6124成就
	AchievementTask* atask = (AchievementTask*)FindTask(6124);
	std::string taskKey = "title_task_id_6124";
	if (atask != NULL && atask->GetStatus() == TaskStatus::TASK_OVER)
	{
		if (GetOwner()->GetCounter(taskKey.c_str()) == 0)
		{
			//邮件发奖
			RewardGroup* group = GetOwner()->GetRewardGroup();
			group->AddReward(130193141, 1);
			std::string title = SysNotifyMgr::Instance()->MakeString(9975);
			std::string senderName = SysNotifyMgr::Instance()->MakeString(9976);
			std::string content = SysNotifyMgr::Instance()->MakeString(9977);
			std::string strReason = GetReason(SOURCE_TYPE_TASK_ACHIEVEMENT, atask->GetDataId(), atask->GetStatus());
			GetOwner()->SendSysMail(senderName.c_str(), title.c_str(), content.c_str(), strReason.c_str(), group);
			GetOwner()->IncCounter(taskKey.c_str(), 1);
		}
	}
}

void AchievementTaskMgr::OnLevelup()
{
	UInt16 level = GetOwner()->GetLevel();

	class OnLevelupVisitor : public BaseTaskVisitor
	{
	public:
		OnLevelupVisitor(UInt16 level) :m_Level(level){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckLevelupEvent(m_Level))
				{
					script->OnPlayerLevelup(task->GetOwner(), task);
				}
			}
			return true;
		}
	private:
		UInt16	m_Level;
	};
	OnLevelupVisitor visitor(level);
	VisitTasks(visitor);

	InitAchievementTask();
}

void AchievementTaskMgr::OnTaskSucced(BaseTask* task)
{
	BaseTaskMgr::OnTaskSucced(task);

	//连招教学自动提交
	if (task->GetType() == TaskType::TASK_ACHIEVEMENT && task->GetSubType() == TaskSubType::TST_COMBO_GUIDE)
	{
		SubmitTask(task);
	}
}

bool AchievementTaskMgr::IsTaskAcceptable(UInt32 id)
{
	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //没有这个任务

	if (!dataEntry->open) return false; //未开放

	if (!IsAchievementTask(dataEntry->type)) return false;

	//等级未达到
	if (dataEntry->triggerLevel != 0 && GetOwner()->GetLevel() < dataEntry->triggerLevel) return false;

	if (FindTask(id) != NULL) return false;

	return true;
}

UInt32 AchievementTaskMgr::GetMaxTaskNum()
{
	return MAX_ACCEPT_ACHIEVEMENT_NUM;
}

bool AchievementTaskMgr::AddTask(BaseTask* task)
{
	if (task == NULL) return false;
	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
	{
		return true;
	}
	return false;
}

bool AchievementTaskMgr::CheckFinishTask(UInt32 taskId)
{
	BaseTask* task = FindTask(taskId);
	if (task != NULL && (task->GetStatus() == TaskStatus::TASK_OVER || task->GetStatus() == TaskStatus::TASK_SUBMITTING))
	{
		return true;
	}
	return false;
}

void AchievementTaskMgr::InitAchievementTask(bool isSync)
{
	TaskDataEntryMgr::TaskDataVec tasks;
	TaskDataEntryMgr::Instance()->GetAchievementTasks(GetOwner()->GetLevel(), tasks);

	bool isAccept = false;
	for (UInt32 i = 0; i < tasks.size(); ++i)
	{
		if (SSApplication::Instance()->OpenPlayerOnOnline())
		{
			if (AcceptTask(tasks[i], isSync))
			{
				isAccept = true;
			}
		}
		else 
		{
			if (AcceptTask(tasks[i], Task::QuestVarMap(), isSync))
			{
				isAccept = true;
			}
		}
	}
	if (isAccept)
	{
		//SetDirty();
	}
}

UInt32 AchievementTaskMgr::GetRewardSPNum()
{
	UInt32 result = 0;
	UInt32 spItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SP_ITEM_ID);
	TaskMap::iterator iter = m_Tasks.begin();
	while (iter != m_Tasks.end())
	{
		//章节任务或者宝箱,领取过SP点的
		if (iter->second != NULL &&
			(iter->second->GetSubType() == TaskSubType::TST_DUNGEON_MISSION || iter->second->GetSubType() == TaskSubType::TST_DUNGEON_CHEST) &&
			iter->second->GetStatus() == TaskStatus::TASK_OVER)
		{
			RewardGroup* rewards = iter->second->GetRewards();
			ItemRewardVec items = rewards->GetRewards();
			for (UInt32 i = 0; i < items.size(); ++i)
			{
				if (items[i].id == spItemId)
				{
					result += items[i].num;
				}
			}
		}
		++iter;
	}
	return result;
}

void AchievementTaskMgr::SyncAchievementTask()
{
	//发送成就数据
	CLProtocol::SceneAchievementTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}



CycleTaskMgr::CycleTaskMgr()
{
	m_CycleTaskMax = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CYCLE_TASK_MAX);
	m_CycleTaskDailyMax = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CYCLE_TASK_DAILY_MAX);
	m_CycleTaskRefreshFreeCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CYCLE_TASK_REFRESH_FREE_COUNT);
	m_CycleTaskRefreshConsume = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CYCLE_TASK_REFRESH_CONSUME);

	m_CycleTaskTypeMax[CTT_DUNGEON] = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CTT_DUNGEON_TIMES_MAX);
	m_CycleTaskTypeMax[CTT_SET_ITEM] = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CTT_SET_ITEM_TIMES_MAX);
	m_CycleTaskTypeMax[CTT_STORY] = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CTT_STORY_TIMES_MAX);
}

CycleTaskMgr::~CycleTaskMgr() {}

bool CycleTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry)
{
	if (!IsTaskAcceptable(taskDataEntry)) return false;

	//任务最大可接数量
	if (GetTaskNum() >= GetMaxTaskNum())
	{
		GetOwner()->SendNotify(1032);
		return false;
	}

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;

	UInt32 count = GetOwner()->GetCounter(COUNTER_CYCLE_TASK_COUNT);
	BaseTask::QuestVarMap questVar;
	std::string key(COUNTER_CYCLE_TASK_COUNT);
	std::string value(Avalon::StringUtil::ConvertToString(count + 1));		//用于客户端显示
	questVar[key] = value;

	CycleTask* task = new CycleTask(GetOwner(), taskDataEntry->id, questVar);
	task->SetQuality(0);

	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	//机器人直接置为完成
	if (GetOwner()->TestActionFlag(ACTION_FLAG_ROBOT))
	{
		task->SetStatus(TASK_FINISHED);
	}

	CLProtocol::SceneNotifyNewTask protocol;
	task->GetBriefInfo(protocol.task);
	GetOwner()->SendProtocol(protocol);

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool CycleTaskMgr::SubmitTask(BaseTask* task, bool force)
{
	if (NULL == task) return false;
	if (!force && task->GetStatus() != TASK_FINISHED) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	if (!task->CheckGiveRewards())
	{
		GetOwner()->SendNotify(1000004);
		return false;
	}
	task->GiveRewards();
	script->OnSubmitted(GetOwner(), task);
	RemoveTask(task);

	GameInfoLogStream("Task") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit task:"
		<< task->GetDataId() << "|" << int(task->GetType()) << "." << LogStream::eos;


	//通知删除任务
	CLProtocol::SceneNotifyDeleteTask protocol;
	protocol.taskId = task->GetDataId();
	protocol.reason = DELETE_TASK_REASON_SUBMIT;

	DebugLogStream << "player(" << GetOwner()->GetAccID() << ","<<GetOwner()->GetID() << "," << GetOwner()->GetName() 
		<< ") in CycleTaskMgr::SubmitTask, to notify delete task(id:" << protocol.taskId << ", reason:" << (UInt16)protocol.reason << ")" << LogStream::eos;

	GetOwner()->SendProtocol(protocol);

	GetOwner()->OnSubmitCycleTaskSucceed(task->GetDataId());
	GetOwner()->OnSubmitTaskSucceed(task->GetDataId(), task->GetType());

	OnCompleteTaskSucc(task);

	UInt32 count = GetOwner()->GetCounter(COUNTER_CYCLE_TASK_COUNT);
	count++;
	if (count >= m_CycleTaskMax)
	{
		GetOwner()->SetCounter(COUNTER_CYCLE_TASK_COUNT, 0);
		UInt32 dailyCount = GetOwner()->GetCounter(COUNTER_CYCLE_TASK_DAILY_COUNT);
		dailyCount -= 1;
		GetOwner()->SetCounter(COUNTER_CYCLE_TASK_DAILY_COUNT, dailyCount);
		ClearTypeCounter();
		if (dailyCount > 0)
		{
			RefreshTask();
		}
	}
	else
	{
		GetOwner()->SetCounter(COUNTER_CYCLE_TASK_COUNT, count);
		RefreshTask();
	}
	
	GetOwner()->IncCounter(COUNTER_CYCLE_TASK_DAILY_TOTAL_COUNT, 1);

	CL_SAFE_DELETE(task);
	return true;
}

UInt32 CycleTaskMgr::RefreshCycleTask(UInt32 taskId)
{
	if (GetTaskNum() == 0) return ErrorCode::TASK_CYCLE_NOT_EXIST;
	if (GetOwner()->GetCounter(COUNTER_CYCLE_REFRESH_COUNT) >= m_CycleTaskRefreshFreeCount)
	{
		if (GetOwner()->GetPointBoth() >= m_CycleTaskRefreshConsume)
		{
			// 安全锁开启，不允许消耗点券
			if (!GetOwner()->SecurityLockCheck())
				return ErrorCode::SECURITY_LOCK_FORBID_OP;

			GetOwner()->RemovePointBoth(GetReason(LogSourceType::SOURCE_TYPE_TASK_CYCLE_REFRESH).c_str(), m_CycleTaskRefreshConsume);
		}
		else
		{
			return ErrorCode::TASK_CYCLE_REFRESH_NOT_CONSUME;
		}
	}

	GetOwner()->IncCounter(COUNTER_CYCLE_REFRESH_COUNT, 1);
	return RefreshTask(taskId, true);
}

UInt32 CycleTaskMgr::RefreshTask(UInt32 taskId, bool force)
{
	TaskDataEntry* taskDataEntry = NULL;
	if (taskId != 0)
	{
		taskDataEntry = TaskDataEntryMgr::Instance()->FindEntry(taskId);
		if (taskDataEntry == NULL) return ErrorCode::TASK_NOT_EXIST;
		if (!IsCycleTask(taskDataEntry->type)) return ErrorCode::TASK_TYPE_ERROR;
	}

	AbandonAllTask();

	if (taskDataEntry == NULL)
	{
		//随机一个循环任务类型
		CycleTaskType type = RandomCycleType(force);
		//随机一个任务
		taskDataEntry = TaskDataEntryMgr::Instance()->RandomCycleTask(GetOwner()->GetLevel(), type);
		if (taskDataEntry == NULL) return ErrorCode::TASK_NOT_EXIST;
	}

	//接取任务
	if (AcceptTask(taskDataEntry))
	{
		return ErrorCode::SUCCESS;
	}
	return ErrorCode::TASK_ERRORCODE_BEGIN;
	
}

void CycleTaskMgr::AbandonAllTask()
{
	TaskMap::iterator iter = m_Tasks.begin();
	while (iter != m_Tasks.end())
	{
		auto task = iter->second;
		if (task == NULL) continue;

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (script != NULL)
		{
			script->OnAbandomed(GetOwner(), task);
		}

		//通知删除任务
		CLProtocol::SceneNotifyDeleteTask protocol;
		protocol.taskId = iter->second->GetDataId();
		protocol.reason = DELETE_TASK_REASON_SYSTEM;
		GetOwner()->SendProtocol(protocol);
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in  CycleTaskMgr::RefreshTask, to notify delete task(id:" << protocol.taskId << ", reason : " << (UInt16)protocol.reason << ")" << LogStream::eos;

		GetOwner()->OnAbandonTask(iter->second->GetDataId(), iter->second->GetType());

		++iter;
	}
	ClearTask();
}

void CycleTaskMgr::RefreshCycleTaskDailyCount()
{
	GetOwner()->SetCounter(COUNTER_CYCLE_TASK_DAILY_COUNT, 1);
	GetOwner()->SetCounter(COUNTER_CYCLE_TASK_COUNT, 0);
	ClearTypeCounter();
	RefreshTask();
}

void CycleTaskMgr::OnOnline(bool bDayChanged)
{
	InitCycle();

	for (auto itr : m_Tasks)
	{
		auto task = itr.second;
		if (!task)
		{
			continue;
		}

		auto data = TaskDataEntryMgr::Instance()->FindEntry(task->GetDataId());
		if (!data || data->subType != TST_SUMMON_NPC_TASK)
		{
			continue;
		}

		GetOwner()->SummonTaskNpc((Task*)task, false);
	}

	CLProtocol::SceneTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}

void CycleTaskMgr::OnLevelup()
{
	InitCycle();
}

CycleTaskType CycleTaskMgr::RandomCycleType(bool force)
{
	if (force)
	{
		UInt32 ranNum = Avalon::Random::RandBetween(1, CYCLE_SUM_ODDS);
		Int32 tmpNum = ranNum - CYCLE_DUNGEON_ODDS;
		if (tmpNum < 0) return CTT_DUNGEON;

		tmpNum = tmpNum - CYCLE_SET_ITEM_ODDS;
		if (tmpNum < 0) return CTT_SET_ITEM;

		tmpNum = tmpNum - CYCLE_STORY_ODDS;
		if (tmpNum < 0) return CTT_STORY;
		
		return CTT_INVALID;
	}

	std::vector<UInt8> types;
	//最近一次任务是副本类型
	if (GetOwner()->GetCounter(COUNTER_CYCLE_TASK_LATEST_TYPE) == CTT_DUNGEON)
	{
		GetCanGenTaskType(types, CTT_DUNGEON);
		if (types.size() == 0)
		{
			GetCanGenTaskType(types);
		}
	}
	else
	{
		GetCanGenTaskType(types);
	}

	if (types.size() > 0)
	{
		UInt32 index = Avalon::Random::RandBetween(0, types.size() - 1);
		return (CycleTaskType)types[index];
	}
	return CTT_INVALID;
}

void CycleTaskMgr::GetCanGenTaskType(std::vector<UInt8>& types, UInt8 excludeType)
{
	for (UInt8 i = CTT_DUNGEON; i < CTT_MAX; ++i)
	{
		std::ostringstream typess;
		typess << COUNTER_CYCLE_TASK_TYPETIMES_PER << i;
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(typess.str());
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(typess.str().c_str(), COUNTER_CYCLE_DAY, 6);
		}
		UInt32 genedNum = GetOwner()->GetCounter(typess.str().c_str());
		
		if (excludeType > CTT_INVALID && excludeType < CTT_MAX && i == excludeType)
		{
			continue;
		}

		if (genedNum < m_CycleTaskTypeMax[i])
		{
			types.push_back(i);
		}
	}
}

void CycleTaskMgr::OnCompleteTaskSucc(BaseTask* task)
{
	if (!task)
	{
		return;
	}
	TaskDataEntry* taskDataEntry = TaskDataEntryMgr::Instance()->FindEntry(task->GetDataId());
	
	if (!taskDataEntry)
	{
		return;
	}
	UInt8 type = taskDataEntry->cycleType;
	if (type <= CTT_INVALID || type >= CTT_MAX )
	{
		return;
	}

	std::ostringstream typess;
	typess << COUNTER_CYCLE_TASK_TYPETIMES_PER << type;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(typess.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(typess.str().c_str(), COUNTER_CYCLE_DAY, 6);
	}
	GetOwner()->IncCounter(typess.str().c_str(), 1);
	GetOwner()->SetCounter(COUNTER_CYCLE_TASK_LATEST_TYPE, type);
}

void CycleTaskMgr::ClearTypeCounter()
{
	for (UInt8 i = CTT_DUNGEON; i < CTT_MAX; ++i)
	{
		std::ostringstream typess;
		typess << COUNTER_CYCLE_TASK_TYPETIMES_PER << i;
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(typess.str());
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(typess.str().c_str(), COUNTER_CYCLE_DAY, 6);
		}
		GetOwner()->SetCounter(typess.str().c_str(), 0);
	}
	GetOwner()->SetCounter(COUNTER_CYCLE_TASK_LATEST_TYPE, CTT_INVALID);
}

void CycleTaskMgr::InitCycle()
{
	if (GetTaskNum() <= 0)
	{
		if (GetOwner()->GetCounter(COUNTER_CYCLE_TASK_DAILY_COUNT) > 0)
		{
			RefreshTask();
		}
	}
}

bool CycleTaskMgr::IsTaskAcceptable(TaskDataEntry* taskDataEntry)
{
	if (taskDataEntry == NULL) return false;
	if (!taskDataEntry->open) return false;
	if (FindTask(taskDataEntry->id) != NULL) return false;

	if (taskDataEntry->triggerLevel != 0 && taskDataEntry->triggerLevel > GetOwner()->GetLevel()) return false;
	if (taskDataEntry->maxLevel != 0 && taskDataEntry->maxLevel < GetOwner()->GetLevel()) return false;

	//if (taskDataEntry->occupation && GetOwner()->GetOccu() != taskDataEntry->occupation) return false;
	if (!taskDataEntry->CheckOccu(GetOwner()->GetOccupation())) return false;

	return true;

}

bool RedPacketTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry, Task::QuestVarMap questVar)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry->id, true)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;
	RedPacketTask* task = new RedPacketTask(GetOwner(), taskDataEntry->id, questVar);
	task->SetQuality(0);
	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

bool RedPacketTaskMgr::SubmitTask(BaseTask* task, bool force)
{
	if (NULL == task) { return false; }
	if (!force && TASK_FINISHED != task->GetStatus()) { return false; }

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return false;

	task->GiveRewards();

	script->OnSubmitted(GetOwner(), task);
	GetOwner()->SetTaskFinished(task->GetDataId());

	GameInfoLogStream("Task") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit red packet task:"
		<< task->GetDataId() << "|" << int(task->GetType()) << "." << LogStream::eos;

	const TaskDataEntryMgr::TaskDataVec& postTasks = TaskDataEntryMgr::Instance()->GetPostRedPackTasks(task->GetDataId());
	for (TaskDataEntryMgr::TaskDataVec::const_iterator iter = postTasks.begin(); iter != postTasks.end(); ++iter)
	{
		TaskDataEntry *dataEntry = *iter;
		if (dataEntry == NULL) continue;

		AcceptTask(dataEntry, task->GetQuestVar());
	}

	GetOwner()->OnSubmitTaskSucceed(task->GetDataId(), task->GetType());

	//task->SetStatus(TASK_OVER);
	RemoveTask(task);
	CL_SAFE_DELETE(task);

	return true;
}

bool RedPacketTaskMgr::IsTaskAcceptable(UInt32 id, bool bNotifyErr)
{
	if (!AchievementTaskMgr::IsTaskAcceptable(id)) return false;

	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //没有这个任务

	if (!IsRedPacketTask(dataEntry->type)) return false;

	return true;
}

void RedPacketTaskMgr::InitRedPacketTask()
{
	TaskDataEntryMgr::TaskDataVec tasks;
	TaskDataEntryMgr::Instance()->GetRedPackTasks(tasks);

	for (UInt32 i = 0; i < tasks.size(); ++i)
	{
		if (!AcceptTask(tasks[i], Task::QuestVarMap()))
		{
			ErrorLogStream "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
				<< " RedPacketTaskMgr::InitRedPacketTask, accept task(id:" << tasks[i]->id <<")" << LogStream::eos;
		}
	}
}

void RedPacketTaskMgr::OnHeartbeat(const Avalon::Time& now, int tickType)
{
	class OnTickVisitor : public BaseTaskVisitor
	{
	public:
		OnTickVisitor(Player* player) :m_Player(player){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_FINISHED)
			{
				m_Player->GetRedPacketTaskMgr().SubmitTask(task);
			}
			return true;
		}

	private:
		Player* m_Player;

	};

	if (tickType == LOGICTICK_SECOND)
	{
		OnTickVisitor visitor(GetOwner());
		VisitTasks(visitor);
	}
}

void RedPacketTaskMgr::OnOnline(bool bDayChanged)
{
	InitRedPacketTask();
}


MasterDailyTaskMgr::MasterDailyTaskMgr()
{

}

void MasterDailyTaskMgr::OnOnline(bool bDayChanged)
{
	//检查是否跨天
	if (bDayChanged)
	{
		//跨天需要清空师门任务
		AbandonAllTask(true, true);
	}
	SyncToClient();
}


void MasterDailyTaskMgr::OnOffline()
{
	//下线同步任务数据到world
	SyncToWorld();
}

void MasterDailyTaskMgr::AcceptAllTask()
{
	AbandonAllTask();
	TaskDataEntryMgr::TaskDataVec taskDatas = TaskDataEntryMgr::Instance()->GetMasterDailyTasks();
	if (taskDatas.size() > 0)
	{
		UInt32 taskNum = GetMaxTaskNum();
		std::random_shuffle(taskDatas.begin(), taskDatas.end());
		
		UInt32 acceptedTasks = 0;
		for (UInt32 i = 0; i < taskDatas.size(); ++i)
		{
			TaskDataEntry* taskDataEntry = taskDatas[i];
			if (taskDataEntry && AcceptTask(taskDataEntry))
			{
				++acceptedTasks;
				if (acceptedTasks >= taskNum)
				{
					break;
				}
			}
		}
	}
}

bool MasterDailyTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;

	MasterDailyTask* task = new MasterDailyTask(GetOwner(), taskDataEntry->id);
	
	if (!task->SetupBase())
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	task->SetVar("acceptTime", (UInt32)CURRENT_TIME.Sec());

	if (!AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	//通知新任务
	CLProtocol::SceneNotifyNewTask protocol;
	task->GetBriefInfo(protocol.task);
	GetOwner()->SendProtocol(protocol);
	
	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
		<< " in  MasterDailyTaskMgr::AcceptTask taskid(" << taskDataEntry->id <<  ")" << LogStream::eos;

	return true;
}

bool  MasterDailyTaskMgr::SubmitTask(BaseTask* task, bool)
{	
	return true;
}

bool MasterDailyTaskMgr::IsTaskAcceptable(TaskDataEntry* taskDataEntry)
{
	if (taskDataEntry == NULL) return false;
	if (!IsMasterDailyTask(taskDataEntry->type)) return false;
	if (!taskDataEntry->open) return false;
	if (FindTask(taskDataEntry->id) != NULL) return false;
	
	if (taskDataEntry->triggerLevel != 0 && taskDataEntry->triggerLevel > GetOwner()->GetLevel()) return false;

	return true;
}

void MasterDailyTaskMgr::OnLogicDayChanged()
{
	//清空师门任务
	AbandonAllTask(true, true);
}

void MasterDailyTaskMgr::AbandonAllTask(bool notify, bool checkAcceptTime)
{
	std::vector<UInt32> delTasks;
	UInt32 currTm = (UInt32)CURRENT_TIME.Sec();
	//TaskMap::iterator iter = m_Tasks.begin();
	//while (iter != m_Tasks.end())
	for (auto iter : m_Tasks)
	{
		auto task = iter.second;
		if (task == NULL) continue;

		if (checkAcceptTime)
		{
			UInt32 acceptTm = task->GetVar("acceptTime");
			if (CheckInSameLogicDay(acceptTm, currTm))
			{
				continue;
			}
		}

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (script != NULL)
		{
			script->OnAbandomed(GetOwner(), task);
		}
		if (notify)
		{
			//通知删除任务
			CLProtocol::SceneNotifyDeleteTask protocol;
			protocol.taskId = task->GetDataId();
			protocol.reason = DELETE_TASK_REASON_SYSTEM;
			GetOwner()->SendProtocol(protocol);
		}
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in  MasterDailyTaskMgr::AbandonAllTask, to notify delete task(id:" << task->GetDataId() << ", reason : " << (UInt16)DELETE_TASK_REASON_SYSTEM << ")" << LogStream::eos;

		GetOwner()->OnAbandonTask(task->GetDataId(), task->GetType());

		delTasks.push_back(task->GetDataId());
	}

	for (auto taskId : delTasks)
	{
		auto task = FindTask(taskId);
		if (!task)	continue;

		task->DeleteTask();
		delete task;
		m_Tasks.erase(taskId);
	}

	//ClearTask();
}

UInt32 MasterDailyTaskMgr::GetMaxTaskNum()
{
	UInt32 taskNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_MASTERDAILYTASK_NUM);
	if (taskNum == 0) taskNum = 5;
	return taskNum;
}

void MasterDailyTaskMgr::SyncToClient()
{
	//发送日常任务数据
	CLProtocol::SceneSyncMasterDailyTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}

void MasterDailyTaskMgr::SyncToWorld()
{
	/*CLProtocol::SceneSyncMasterDailyTaskToWorld taskList;
	GetTaskList(taskList.tasks);
	taskList.roleId = GetOwner()->GetID();
	Router::SendToWorld(taskList);*/
}

void MasterDailyTaskMgr::GetShareData(MasterTaskShareData& sharedata)
{
	this->GetTaskList(sharedata.m_MasterDailyTasks);
}

bool MasterDailyTaskMgr::CheckAllTasksFinished()
{
	class TaskVisitor : public BaseTaskVisitor
	{
	public:
		TaskVisitor(ObjID_t playerId) : m_AllFinshed(true), m_playerId(playerId){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() != TaskStatus::TASK_FINISHED)
			{
				InfoLogStream << "CheckAllTasksFinished playerId(" << m_playerId << ")" << ", taskId(" << task->GetDataId() << "), status=" << task->GetStatus() << LogStream::eos;
				m_AllFinshed = false;
				return false;
			}
			return true;
		}

		bool m_AllFinshed;
		ObjID_t m_playerId;
	};

	TaskVisitor visiter(GetOwner()->GetID());
	this->VisitTasks(visiter);

	if (visiter.m_AllFinshed)
	{
		CLProtocol::SceneSyncMasterDailyTaskComp sync;
		sync.roleId = GetOwner()->GetID();
		Router::SendToWorld(sync);
	}

	return visiter.m_AllFinshed;
}

void MasterDailyTaskMgr::OnRelationRelieveMaster(ObjID_t masterId)
{
	AbandonAllTask();
}

void MasterDailyTaskMgr::OnRelationDiscipleFinishSchool(ObjID_t masterId)
{
	AbandonAllTask();
}

void MasterDailyTaskMgr::ClearOfflinePlayerAllTask(ObjID_t ownerId)
{
	TaskDataEntryMgr::TaskDataVec taskDatas = TaskDataEntryMgr::Instance()->GetMasterDailyTasks();
	if (taskDatas.size() > 0)
	{
		std::ostringstream condstream;
		condstream << "`owner`=" << ownerId << " AND `dataid` IN (";
		for (UInt32 i = 0; i < taskDatas.size(); i++)
		{
			TaskDataEntry* taskDataEntry = taskDatas[i];
			condstream << taskDataEntry->GetKey();
			if (i != taskDatas.size() - 1)
			{
				condstream << ",";
			}
		}
		condstream << ")";
		CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_task", condstream.str());
		CLRecordClient::Instance()->SendCommand(deleteCmd, NULL, ownerId);
	}
}

MasterAcademicGrowthTaskMgr::MasterAcademicGrowthTaskMgr()
{}

void MasterAcademicGrowthTaskMgr::OnOnline(bool bDayChanged)
{
	SyncToClient();
	//自动接取新增学业成长任务
	AcceptAllTask();
	
}

void MasterAcademicGrowthTaskMgr::OnOffline()
{
	//下线同步任务数据到world
	SyncToWorld();
}

void MasterAcademicGrowthTaskMgr::AcceptAllTask(bool force)
{
	if (GetOwner()->HasRelation(RELATION_MASTER) || force)
	{
		InfoLogStream << "accept all master academic growth tasks force = " << force << " , player id = " << GetOwner()->GetID() << LogStream::eos;

		const TaskDataEntryMgr::TaskDataVec& taskDatas = TaskDataEntryMgr::Instance()->GetMasterAcademicGrowthTasks();
		if (taskDatas.size() > 0)
		{
			for (UInt8 i = 0; i < taskDatas.size(); ++i)
			{
				TaskDataEntry* taskDataEntry = taskDatas[i];
				if (taskDataEntry && FindTask(taskDataEntry->id) == NULL)
				{
					AcceptTask(taskDataEntry);
				}
			}
		}
	}
}

bool MasterAcademicGrowthTaskMgr::AcceptTask(TaskDataEntry* taskDataEntry)
{
	if (taskDataEntry == NULL) return false;
	if (!IsTaskAcceptable(taskDataEntry)) return false;

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	if (!script->CheckAccept(GetOwner())) return false;

	MasterAcademicGrowthTask* task = new MasterAcademicGrowthTask(GetOwner(), taskDataEntry->id);

	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//有些任务通过脚本设置直接完成
	script->OnAccepted(GetOwner(), task);

	//通知新任务
	CLProtocol::SceneNotifyNewTask protocol;
	task->GetBriefInfo(protocol.task);
	GetOwner()->SendProtocol(protocol);

	GetOwner()->OnAcceptTask(task->GetDataId(), task->GetType());

	return true;
}

void MasterAcademicGrowthTaskMgr::AbandonAllTask(bool notify)
{
	TaskMap::iterator iter = m_Tasks.begin();
	while (iter != m_Tasks.end())
	{
		auto task = iter->second;
		if (task == NULL) continue;

		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
		if (script != NULL)
		{
			script->OnAbandomed(GetOwner(), task);
		}
		if (notify)
		{
			//通知删除任务
			CLProtocol::SceneNotifyDeleteTask protocol;
			protocol.taskId = iter->second->GetDataId();
			protocol.reason = DELETE_TASK_REASON_SYSTEM;
			GetOwner()->SendProtocol(protocol);
		}
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in  MasterDailyTaskMgr::OnLogicDayChanged, to notify delete task(id:" << iter->second->GetDataId() << ", reason : " << (UInt16)DELETE_TASK_REASON_SYSTEM << ")" << LogStream::eos;

		GetOwner()->OnAbandonTask(iter->second->GetDataId(), iter->second->GetType());

		++iter;
	}
	ClearTask();
}

bool MasterAcademicGrowthTaskMgr::IsTaskAcceptable(TaskDataEntry* taskDataEntry)
{
	if (taskDataEntry == NULL) return false;
	if (!IsMasteAcademicTask(taskDataEntry->type)) return false;
	if (!taskDataEntry->open) return false;
	if (FindTask(taskDataEntry->id) != NULL) return false;

	if (taskDataEntry->triggerLevel != 0 && taskDataEntry->triggerLevel > GetOwner()->GetLevel()) return false;

	return true;
}

bool MasterAcademicGrowthTaskMgr::SubmitTask(BaseTask* task, bool force)
{
	return true;
}

UInt32 MasterAcademicGrowthTaskMgr::GetMaxTaskNum()
{
	return 1;
}

void MasterAcademicGrowthTaskMgr::OnAddMaster()
{
	ClearTask();
	AcceptAllTask();
}

void MasterAcademicGrowthTaskMgr::SyncToClient()
{
	CLProtocol::SceneSyncMasterAcademicTaskList taskList;
	GetTaskList(taskList.tasks);
	GetOwner()->SendProtocol(taskList);
}

void MasterAcademicGrowthTaskMgr::SyncToWorld()
{
	/*CLProtocol::SceneSyncMasterAcademicTaskToWorld taskList;
	GetTaskList(taskList.tasks);
	taskList.roleId = GetOwner()->GetID();
	Router::SendToWorld(taskList);*/
}

void MasterAcademicGrowthTaskMgr::GetShareData(MasterTaskShareData& sharedata)
{
	this->GetTaskList(sharedata.m_MasterAcademicTasks);
}

void MasterAcademicGrowthTaskMgr::OnRelationRelieveMaster(ObjID_t masterId)
{
	AbandonAllTask();
}

void MasterAcademicGrowthTaskMgr::OnRelationDiscipleFinishSchool(ObjID_t masterId)
{
	AbandonAllTask();
}

void MasterAcademicGrowthTaskMgr::OnLevelup()
{
	UInt16 level = GetOwner()->GetLevel();

	class OnLevelupVisitor : public BaseTaskVisitor
	{
	public:
		OnLevelupVisitor(UInt16 level) :m_Level(level){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckLevelupEvent(m_Level))
				{
					script->OnPlayerLevelup(task->GetOwner(), task);
				}
			}
			return true;
		}
	private:
		UInt16	m_Level;
	};
	OnLevelupVisitor visitor(level);
	VisitTasks(visitor);
}

void MasterAcademicGrowthTaskMgr::OnTeamPVP(PkType type, PkRaceResult status, std::vector<ObjID_t>& teammateIds)
{
	class OnPVPVisitor :public BaseTaskVisitor
	{
	public:
		OnPVPVisitor(PkType type, PkRaceResult status, std::vector<ObjID_t>& teammateIds) :m_Type(type), m_Status(status), m_TeammateIds(teammateIds){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckPVPEvent(m_Type, m_Status))
				{
					script->OnTeamPVP(pTask->GetOwner(), pTask, m_Type, m_Status, m_TeammateIds);
				}
			}
			return true;
		}

	private:
		PkType m_Type;
		PkRaceResult m_Status;
		std::vector<ObjID_t> m_TeammateIds;
	};

	OnPVPVisitor visitor(type, status, teammateIds);
	//VisitTasks(visitor);


	PVPEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckPVPEvent(type, status))
				{
					script->OnTeamPVP(task->GetOwner(), task, type, status, teammateIds);
				}
			}
		}
	}
}

void MasterAcademicGrowthTaskMgr::OnTeamKillCityMonster(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds)
{
	class OnKillCityMonsterVisitor : public BaseTaskVisitor
	{
	public:
		OnKillCityMonsterVisitor(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds)
			:m_CityMonsterType(cityMonsterType), m_MonsterId(monsterId), m_MonsterGuid(monsterGuid), m_teammateIds(teammateIds){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckKillCityMonsterEvent(m_CityMonsterType, m_MonsterId, m_MonsterGuid))
				{
					script->OnTeamKillCityMonster(task->GetOwner(), task, m_CityMonsterType, m_MonsterId, m_MonsterGuid, m_teammateIds);
				}
			}
			return true;
		}

	private:
		CityMonsterType m_CityMonsterType;
		UInt32 m_MonsterId;
		UInt64 m_MonsterGuid;
		std::vector<ObjID_t> m_teammateIds;
	};

	OnKillCityMonsterVisitor visitor(cityMonsterType, monsterId, monsterGuid, teammateIds);
	//VisitTasks(visitor);


	KillCityMonsterEvent event(cityMonsterType, monsterId);
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (script->CheckKillCityMonsterEvent(event.type, event.monsterId, monsterGuid))
			{
				script->OnTeamKillCityMonster(task->GetOwner(), task, event.type, event.monsterId, monsterGuid, teammateIds);
			}
		}
	}
}

void MasterAcademicGrowthTaskMgr::ClearOfflinePlayerAllTask(ObjID_t ownerId)
{
	TaskDataEntryMgr::TaskDataVec taskDatas = TaskDataEntryMgr::Instance()->GetMasterAcademicGrowthTasks();
	if (taskDatas.size() > 0)
	{
		std::ostringstream condstream;
		condstream << "`owner`=" << ownerId << " AND `dataid` IN (";
		for (UInt32 i = 0; i < taskDatas.size(); i++)
		{
			TaskDataEntry* taskDataEntry = taskDatas[i];
			condstream << taskDataEntry->GetKey();
			if (i != taskDatas.size() - 1)
			{
				condstream << ",";
			}
		}
		condstream << ")";
		CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_task", condstream.str());
		CLRecordClient::Instance()->SendCommand(deleteCmd, NULL, ownerId);
	}
}