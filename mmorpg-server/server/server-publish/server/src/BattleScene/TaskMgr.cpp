#include "TaskMgr.h"

#include <CLTaskDefine.h>
#include <CLSysNotify.h>
#include <CLSystemValueDataEntry.h>
#include <CLDungeonDailyDropDataEntry.h>
#include <CLPlayerDefine.h>
#include <CLLostDungeonMissionDataEntry.h>
#include <CLLostDungeonBattleDataEntry.h>
#include <CLLostDungeonProtocol.h>

#include "TaskScript.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "EscortScript.h"
#include "Player.h"
#include "Scene.h"
#include "SSRouter.h"
#include "LostBattleScene.h"
#include "PlayerMgr.h"
#include "SceneMgr.h"

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
	if (dataEntry == NULL) return false;  //û���������

 	if(bForce)
	{
		if (FindTask(id) != NULL) return false; ////�ѽ�����
		if (!dataEntry->open) return false; //δ���� 
	}
	else
	{
		if(!IsTaskAcceptable(id, true)) return false;

		//�������ɽ�����
		if (IsLimitNumberTask(dataEntry->type) && GetTaskNum() >= GetMaxTaskNum())
		{
			if (bNotifyErr) GetOwner()->SendNotify(1032);
			return false;
		}
	}

	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(id);
	if(script == NULL) return false;

	if(!script->CheckAccept(GetOwner())) return false; //aprilliu:��������ű���һ������Ƿ�ɽ�

	Task* task = new Task(GetOwner(), id);
	task->SetQuality(quality);

	if(!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}
	
	//��Щ����ͨ���ű�����ֱ�����
	script->OnAccepted(GetOwner(), task);

	//������ֱ����Ϊ���
	if (GetOwner()->TestActionFlag(ACTION_FLAG_ROBOT))
	{
		task->SetStatus(TASK_FINISHED);
	}

	if(m_AcceptableTasks.find(id) == m_AcceptableTasks.end())
	{
		//֪ͨ������
		CLProtocol::SceneNotifyNewTask protocol;
		task->GetBriefInfo(protocol.task);
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AcceptTask, to notify new task:" << LogStream::eos;
		GetOwner()->SendProtocol(protocol);
	}
	else
	{
		//֪ͨ״̬���
		task->SyncStatus();   //noted by aprilliu, Ҳ���Ǹ�task��״̬��ԭ���� δ��ȡ ���Ϊ �ѽ�ȡδ���״̬
		m_AcceptableTasks.erase(id);
	}

	//��ʱ������
	if(task->GetLimitTime() != 0)
	{
		task->SetVar(TASK_DEADLINE_VAR, UInt32(CURRENT_TIME.Sec()) + task->GetLimitTime());
	}
	//����������
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

	//�жϵȼ��������񣬷����
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

	if (!task->CheckGiveRewards()) //��鱳�����Ƿ����㹻�Ŀռ���������
	{
		GetOwner()->SendNotify(1000004);// TODO:
		return false;
	}
	task->GiveRewards();

	script->OnSubmitted(GetOwner(), task);

	RemoveTask(task);

	GetOwner()->SetTaskFinished(task->GetDataId());

	if(TASK_MAIN == task->GetType())
	{
		/*
		std::ostringstream	param;
		param << task->GetID() << "_3";
		GetOwner()->SendUdpLog("guide", param.str().c_str(), "", "", "", "", "guide");
		*/
		GetOwner()->UpdateFuncNotify();
	}

	GameInfoLogStream("Task") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit task:"
		<< task->GetDataId() << "|" << int(task->GetType()) << "." << LogStream::eos;

	//�����º��ÿɽ�����
	const TaskDataEntryMgr::TaskDataVec& postTasks = TaskDataEntryMgr::Instance()->GetPostTasks(task->GetDataId());
	for (TaskDataEntryMgr::TaskDataVec::const_iterator iter = postTasks.begin(); iter != postTasks.end(); ++iter)
	{
		TaskDataEntry *dataEntry = *iter;
		if (dataEntry == NULL) { continue; }

		//��ȡ�������Զ���ȡ������.
		if (dataEntry->isServerTake && IsTaskAcceptable(dataEntry->id))
		{
			this->AcceptTask(dataEntry->id, 0, false);
		}

		//�·��ɽ�ȡ����
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

	//֪ͨɾ������
	CLProtocol::SceneNotifyDeleteTask protocol;
	protocol.taskId = task->GetDataId();
	protocol.reason = DELETE_TASK_REASON_SUBMIT;

	InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
		<< " in  TaskMgr::SubmitTask, to notify delete task(id:" << protocol.taskId << ", reason : " << (UInt16)protocol.reason << ")" << LogStream::eos;

	GetOwner()->SendProtocol(protocol);

	//added by aprilliu, 2016.04.18 ���ӶԳɹ��ύ�����¼��Ļص�
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

	//added by aprilliu, 2016.05.25 ���ӶԲ����������񲻿ɷ������߼��ж�
	if (IsNotAbandonTask(task->GetType()))
	{
		//
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AbandonTask, can't abandon task(id:" << task->GetDataId() << ", type:" << task->GetType() <<"), because it is transfer task!" << LogStream::eos;
		//GetOwner()->SendNotify(1000); //TODO: ��ʾ�û� ���񲻿ɷ�������Ҫ�߻���������֪ͨ��Ϣ
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
		//֪ͨ״̬�л�
		CLProtocol::SceneNotifyTaskStatus protocol;
		protocol.taskId = task->GetDataId();
		protocol.status = TASK_INIT;
		protocol.finTime = 0;
		//protocol.target = task->GetTargetByStatus(TASK_INIT);
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in TaskMgr::AbandonTask, to notify task status change, task(id:" << protocol.taskId
			<< ", status:" << (UInt16)protocol.status << ")" << LogStream::eos;
		GetOwner()->SendProtocol(protocol);

		//�Żؿɽ������б�
		m_AcceptableTasks.insert(task->GetDataId());
	}
	else
	{
		//֪ͨɾ��
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

bool TaskMgr::IsTaskAcceptable(UInt32 id, bool bNotifyErr)
{
	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //û���������

	if (!dataEntry->open) return false; //δ����

	if (FindTask(id) != NULL)
	{
		if (bNotifyErr)
			GetOwner()->SendNotify(10, dataEntry->name.c_str());
		return false; //�ѽ�����
	}

	//�����
	if (GetOwner()->CheckTaskFinished(id)) return false;

	//δ���ǰ������
	if (dataEntry->preTask != 0 && !GetOwner()->CheckTaskFinished(dataEntry->preTask)) return false;

	//�ȼ�δ�ﵽ
	if (dataEntry->triggerLevel != 0 && GetOwner()->GetLevel() < dataEntry->triggerLevel) return false;

	//added by aprilliu,���Ӷ��û�ְҵ�ļ��
	//if (dataEntry->occupation && GetOwner()->GetOccu() != dataEntry->occupation) return false;
	if (!dataEntry->CheckOccu(GetOwner()->GetOccupation())) return false;

	return true;
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
	//��ȡ�ɽ���������
	for (TaskIdSet::iterator iter = m_AcceptableTasks.begin(); iter != m_AcceptableTasks.end(); ++iter)
	{
		//TaskInfo info;
		//GetInitTaskInfo(*iter, info);
		TaskBriefInfo info;
		GetInitTaskBriefInfo(*iter, info);
		taskList.push_back(info);
	}
}

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

			if (!AcceptTask(*tmpIter, 0, false))
			{
				break;
			}
		}

		isBreak = true;
		iter = m_Tasks.begin();
		while (iter != m_Tasks.end())
		{
			TaskMap::iterator tmpIter = iter;
			++iter;

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

	//����ʱ���ɽ������б�
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

	//������������
	CLProtocol::SceneTaskList taskList;
	GetTaskList(taskList.tasks);

	//��ȡ�ɽ���������
	for(TaskIdSet::iterator iter = m_AcceptableTasks.begin(); iter != m_AcceptableTasks.end(); ++iter)
	{
		//TaskInfo info;
		//GetInitTaskInfo(*iter, info);
		TaskBriefInfo info;
		GetInitTaskBriefInfo(*iter, info);
		taskList.tasks.push_back(info);
	}

	//���ܽ�ȡ�ĳƺ�����
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

	if (!script->CheckAccept(GetOwner())) return false; //aprilliu:��������ű���һ������Ƿ�ɽ�

	DailyTask* task = new DailyTask(GetOwner(), id);
	task->SetQuality(quality);

	//�ճ�������Ҫ�����������ʱ����ҵȼ�.������ȡ����.
	task->InitVar(TASK_DAILY_LEVEL_VAR, (UInt64)GetOwner()->GetLevel());

	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//��Щ����ͨ���ű�����ֱ�����
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

	if (!task->CheckGiveRewards()) //��鱳�����Ƿ����㹻�Ŀռ���������
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
	//������Ҫ�ĵ��
	UInt32 taskPoint = m_TaskPoint * taskList.size();
	//�鿴����Ƿ��㹻
	if (GetOwner()->GetPointBoth() < taskPoint) return false;

	//���δ�������
	for (UInt32 i = 0; i < taskList.size(); ++i)
	{
		if (!SubmitTask(taskList[i], true))
		{
			return false;
		}
		//���ĵ��
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
	if (FindTask(id) != NULL) return false; ////�ѽ�����

	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //û���������

	if (!dataEntry->open) return false; //δ����

	if (!IsDailyTask(dataEntry->type)) return false; //��������

	return true;
}

UInt32 DailyTaskMgr::GetMaxTaskNum()
{
	return MAX_ACCEPT_TASK_NUM;
}

/**
*@brief �������
*/
void DailyTaskMgr::OnOnline(bool bDayChanged)
{
	// adder by huchenhui 2017.06.26 ��鵱ǰ�ȼ����ճ��ǲ��Ƕ���ȡ��,û��ȡ�Ľ�ȡ
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
	//���������ճ�����
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

		//�·��ճ�����
		SyncDailyTask();
	}

	//���û���
	GetOwner()->ResetDailyTaskScore();
	GetOwner()->ResetDailyTaskRewardMask();

	GetOwner()->SyncBaseInfoToWorld();
}

void DailyTaskMgr::SyncDailyTask()
{
	//�����ճ���������
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
	//���������л�ȡ��Ӧ�ȼ�������޶�
	std::vector<DailyTaskSelector> selector = GetOwner()->GetDailyTaskSelector();
	std::vector<UInt32> poodIndexs;

	//�س�
	TaskDataEntryMgr::DailyTaskVec sureTasks = TaskDataEntryMgr::Instance()->GetSureDailyTasks(GetOwner()->GetLevel());
	for (UInt32 i = 0; i < sureTasks.size(); ++i)
	{
		if (sureTasks[i] == NULL) continue;
		tasks.push_back(sureTasks[i]);
	}

	//ѭ��ÿ��ѡ����
	for (UInt32 i = 0; i < selector.size(); ++i)
	{
		//���ݳ�����ѡ�������
		TaskDataEntryMgr::DailyTaskPool pool = TaskDataEntryMgr::Instance()->GetDailyTaskPool(GetOwner()->GetLevel(), selector[i].type);
		poodIndexs.clear();

		//��ȡ����
		RandomPoolIndex(pool.size(), selector[i].num, poodIndexs);

		for (UInt32 j = 0; j < poodIndexs.size(); ++j)
		{
			//��ÿ�����͵���������������
			UInt32 index = poodIndexs[j];
			UInt32 max = pool[index].dailyTasks.size();
			//�������������
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

	//�س�
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
			//֪ͨ������
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
	//ѭ��ÿ��ѡ����
	for (UInt32 i = 0; i < selector.size(); ++i)
	{
		//���ݳ�����ѡ�������
		TaskDataEntryMgr::DailyTaskPool pool = TaskDataEntryMgr::Instance()->GetDailyTaskPool(GetOwner()->GetLevel(), selector[i].type);
		poodIndexs.clear();



		//��ȡ����
		RandomPoolIndex(pool.size(), selector[i].num, poodIndexs);

		for (UInt32 j = 0; j < poodIndexs.size(); ++j)
		{
			//��ÿ�����͵���������������
			UInt32 index = poodIndexs[j];
			UInt32 max = pool[index].dailyTasks.size();
			//�������������
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

	//��Щ����ͨ���ű�����ֱ�����
	script->OnAccepted(GetOwner(), task);

	//�Ѿ���ɵ�����״̬,���
	/*
	if (GetOwner()->CheckTaskFinished(taskDataEntry->id))
	{
		task->SetStatus(TASK_OVER);
	}
	*/

	if (isSync)
	{
		//֪ͨ������
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

	if (!task->CheckGiveRewards()) //��鱳�����Ƿ����㹻�Ŀռ���������
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

	//��������
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

		//��Щ����ͨ���ű�����ֱ�����
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

	if (!taskData->open) return false; //δ����

	//�ȼ�δ�ﵽ
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

	// 2018.12.04 by huchenhui �鴫��֮·δ����BUG
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

	//��Щ����ͨ���ű�����ֱ�����
	script->OnAccepted(GetOwner(), task);

	if (isSync)
	{
		//֪ͨ������
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

	if (!task->CheckGiveRewards()) //��鱳�����Ƿ����㹻�Ŀռ���������
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

	//adder by huchenhui 20190108  ���⴦��6124�ɾ�
	AchievementTask* atask = (AchievementTask*)FindTask(6124);
	std::string taskKey = "title_task_id_6124";
	if (atask != NULL && atask->GetStatus() == TaskStatus::TASK_OVER)
	{
		if (GetOwner()->GetCounter(taskKey.c_str()) == 0)
		{
			//�ʼ�����
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

	//���н�ѧ�Զ��ύ
	if (task->GetType() == TaskType::TASK_ACHIEVEMENT && task->GetSubType() == TaskSubType::TST_COMBO_GUIDE)
	{
		SubmitTask(task);
	}
}

bool AchievementTaskMgr::IsTaskAcceptable(UInt32 id)
{
	TaskDataEntry *dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;  //û���������

	if (!dataEntry->open) return false; //δ����

	if (!IsAchievementTask(dataEntry->type)) return false;

	//�ȼ�δ�ﵽ
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
		 if (AcceptTask(tasks[i], Task::QuestVarMap(), isSync))
		{
			isAccept = true;
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
		//�½�������߱���,��ȡ��SP���
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
	//���ͳɾ�����
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

	//�������ɽ�����
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
	std::string value(Avalon::StringUtil::ConvertToString(count + 1));		//���ڿͻ�����ʾ
	questVar[key] = value;

	CycleTask* task = new CycleTask(GetOwner(), taskDataEntry->id, questVar);
	task->SetQuality(0);

	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//��Щ����ͨ���ű�����ֱ�����
	script->OnAccepted(GetOwner(), task);

	//������ֱ����Ϊ���
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


	//֪ͨɾ������
	CLProtocol::SceneNotifyDeleteTask protocol;
	protocol.taskId = task->GetDataId();
	protocol.reason = DELETE_TASK_REASON_SUBMIT;

	InfoLogStream << "player(" << GetOwner()->GetAccID() << ","<<GetOwner()->GetID() << "," << GetOwner()->GetName() 
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
			// ��ȫ�����������������ĵ�ȯ
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
		//���һ��ѭ����������
		CycleTaskType type = RandomCycleType(force);
		//���һ������
		taskDataEntry = TaskDataEntryMgr::Instance()->RandomCycleTask(GetOwner()->GetLevel(), type);
		if (taskDataEntry == NULL) return ErrorCode::TASK_NOT_EXIST;
	}

	//��ȡ����
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

		//֪ͨɾ������
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
	//���һ�������Ǹ�������
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

	//��Щ����ͨ���ű�����ֱ�����
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
	if (dataEntry == NULL) return false;  //û���������

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
	//����Ƿ����
	if (bDayChanged)
	{
		//������Ҫ���ʦ������
		AbandonAllTask();
	}
	SyncToClient();
}


void MasterDailyTaskMgr::OnOffline()
{
	//����ͬ���������ݵ�world
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

	if (!task->SetupBase() || !AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	//��Щ����ͨ���ű�����ֱ�����
	script->OnAccepted(GetOwner(), task);

	//֪ͨ������
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
	//���ʦ������
	AbandonAllTask();
}

void MasterDailyTaskMgr::AbandonAllTask(bool notify)
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
			//֪ͨɾ������
			CLProtocol::SceneNotifyDeleteTask protocol;
			protocol.taskId = iter->second->GetDataId();
			protocol.reason = DELETE_TASK_REASON_SYSTEM;
			GetOwner()->SendProtocol(protocol);
		}
		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ")"
			<< " in  MasterDailyTaskMgr::AbandonAllTask, to notify delete task(id:" << iter->second->GetDataId() << ", reason : " << (UInt16)DELETE_TASK_REASON_SYSTEM << ")" << LogStream::eos;

		GetOwner()->OnAbandonTask(iter->second->GetDataId(), iter->second->GetType());

		++iter;
	}
	ClearTask();
}

UInt32 MasterDailyTaskMgr::GetMaxTaskNum()
{
	UInt32 taskNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_MASTERDAILYTASK_NUM);
	if (taskNum == 0) taskNum = 5;
	return taskNum;
}

void MasterDailyTaskMgr::SyncToClient()
{
	//�����ճ���������
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
		CLRecordClient::Instance()->SendCommand(deleteCmd);
	}
}

MasterAcademicGrowthTaskMgr::MasterAcademicGrowthTaskMgr()
{}

void MasterAcademicGrowthTaskMgr::OnOnline(bool bDayChanged)
{
	SyncToClient();
	//�Զ���ȡ����ѧҵ�ɳ�����
	AcceptAllTask();
	
}

void MasterAcademicGrowthTaskMgr::OnOffline()
{
	//����ͬ���������ݵ�world
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

	//��Щ����ͨ���ű�����ֱ�����
	script->OnAccepted(GetOwner(), task);

	//֪ͨ������
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
			//֪ͨɾ������
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
	VisitTasks(visitor);
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
	VisitTasks(visitor);
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
		CLRecordClient::Instance()->SendCommand(deleteCmd);
	}
}


LostDungeonTaskMgr::LostDungeonTaskMgr()
{
	m_AliveTime = 0;
	m_TaskGrade = 0;
	m_IsRefreshTask = false;
}

LostDungeonTaskMgr::~LostDungeonTaskMgr()
{

}

void LostDungeonTaskMgr::HandlerRandTaskList(UInt32 grade)
{
	LostBattleScene* lbScene = dynamic_cast<LostBattleScene*>(GetOwner()->GetScene());
	if (NULL == lbScene)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "not find lost dungeon scene!" << LogStream::eos;
		return;
	}

	LostDungeonBattleDataEntry* battleEntry = LostDungeonBattleDataEntryMgr::Instance()->FindEntry(lbScene->GetBattleDataId());
	if (NULL == battleEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "not find lost dungeon battle config battleId:" << lbScene->GetBattleDataId() << LogStream::eos;
		return;
	}

	std::map<UInt32, UInt32>& taskMap = battleEntry->GetTaskMap(grade);
	if (taskMap.empty())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "not find lost dungeon task config battleId:" << lbScene->GetBattleDataId()
			<< " grade:" << grade << LogStream::eos;
		return;
	}

	m_TaskGrade = grade;

	UInt32 battlePlayerNum = lbScene->BattlePlayerNum();
	InfoLogStream << PLAYERINFO(GetOwner()) << " scene player num:" << battlePlayerNum << " grade:" << grade << LogStream::eos;

	std::map<UInt32, UInt32> condMap;
	for (auto& iter : taskMap)
	{
		LostDungeonMissionDataEntry* taskEntry = LostDungeonMissionDataEntryMgr::Instance()->FindEntry(iter.first);
		if (NULL == taskEntry)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "not find lost dungeon task id:" << iter.first << LogStream::eos;
			continue;
		}

		// ��ȡ����
		if (taskEntry->acceptLimit != 0 && lbScene->GetTaskAcceptNum(iter.first) >= taskEntry->acceptLimit)
		{
			continue;
		}

		// �ж����˲ſ��Խ�ȡ
		if (taskEntry->frontCond1 != 0 && taskEntry->frontCond1 > battlePlayerNum)
		{
			continue;
		}

		condMap.insert(iter);
	}

	if (condMap.empty())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "cond task empty!" << LogStream::eos;
		return;
	}

	// ��Ȩ�������3������
	m_TaskChoiceVec.clear();
	for (UInt32 k = 0;k < 3;++k)
	{
		UInt32 totalWeight = 0;
		std::map<UInt32, UInt32> randMap;
		for (auto& it : condMap)
		{
			totalWeight += it.second;
			randMap[totalWeight] = it.first;
		}

		UInt32 randNum = Avalon::Random::RandBetween(0, totalWeight);
		auto iter = randMap.lower_bound(randNum);
		if (iter == randMap.end())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "rand lost dungeon task error!" << LogStream::eos;
			continue;
		}

		InfoLogStream << PLAYERINFO(GetOwner()) << "rand lost dungeon task id:" << iter->second << LogStream::eos;

		m_TaskChoiceVec.push_back(iter->second);
		condMap.erase(iter->second);
	}

	CLProtocol::SceneLostDungeonTaskListRes notify;
	notify.taskVec = m_TaskChoiceVec;
	GetOwner()->SendProtocol(notify);
}

void LostDungeonTaskMgr::HandlerChoiceTask(UInt32 taskId)
{
	CLProtocol::SceneLostDungeonChoiceTaskRes res;

	do
	{
		/*if ( !IsHasRandTask(taskId))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "not find rand task id:" << taskId << LogStream::eos;
			res.retCode = ErrorCode::TASK_NOT_EXIST;
			break;
		}*/
		
		LostDungeonMissionDataEntry* entry = LostDungeonMissionDataEntryMgr::Instance()->FindEntry(taskId);
		if (NULL == entry)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "not find config id:" << taskId << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		LostBattleScene* lbScene = dynamic_cast<LostBattleScene*>(GetOwner()->GetScene());
		if (NULL == lbScene)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "not find lost dungeon scene taskId:" << taskId << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		// ����������������
		ResetData(lbScene);

		if ( !AcceptTask(entry, lbScene))
		{
			res.retCode = ErrorCode::TASK_ERRORCODE_BEGIN;
			break;
		}

		lbScene->AddTaskAcceptNum(taskId);

		res.taskId = taskId;
		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	GetOwner()->SendProtocol(res);

	m_TaskChoiceVec.clear();

	UInt32 protTm = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_TASK_PROTECT_TIME);
	protTm = (protTm == 0) ? 10 : protTm;
	GetOwner()->SetLostDungProtTime(protTm + (UInt32)CURRENT_TIME.Sec());
}

bool LostDungeonTaskMgr::IsHasRandTask(UInt32 taskId)
{
	for (auto& iter : m_TaskChoiceVec)
	{
		if (iter == taskId)
			return true;
	}

	return false;
}

bool LostDungeonTaskMgr::AcceptTask(LostDungeonMissionDataEntry* taskDataEntry, LostBattleScene* scene)
{
	if (NULL == taskDataEntry || NULL == scene) return false;
	
	TaskScript* script = ScriptMgr::Instance()->FindLostDungeonTaskScript(taskDataEntry->id);
	if (script == NULL) return false;

	LostDungeonTask* task = new LostDungeonTask(GetOwner(), taskDataEntry->id);
	task->SetDataEntry(taskDataEntry);

	std::map<UInt64, UInt32> enemyDungeonMap;
	if (task->LostDungeonTaskType() == LDTT_TASK_KILL_FIX_ROLE)
	{
		UInt32 killNum = Avalon::StringUtil::ConvertToValue<UInt32>(taskDataEntry->value);
		if (killNum == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "accept lost dungeon task kill num error! taskId:" << taskDataEntry->id << LogStream::eos;
			return false;
		}

		// ��ɱָ�����
		std::set<UInt64>& playerSet = scene->RandGetPlayerList();
		if (playerSet.empty())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "accept lost dungeon task player list empty! taskId:" << taskDataEntry->id << LogStream::eos;
			return false;
		}

		std::vector<UInt64> tempVec(playerSet.begin(), playerSet.end());
		std::random_shuffle(tempVec.begin(), tempVec.end());
		for (auto& it : tempVec)
		{
			if (it == GetOwner()->GetID())
				continue;

			Player* player = PlayerMgr::Instance()->FindPlayer(it);
			if (NULL == player)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "not find player!" << LogStream::eos;
				continue;
			}

			InfoLogStream << PLAYERINFO(GetOwner()) << " kill player name:" << player->GetName() << " id:" << it << LogStream::eos;

			player->GetLostDungeonTaskMgr().AddKillMePlayer(GetOwner()->GetID());
			m_KillRoleSet.insert(it);

			std::string str;
			str.append(player->GetName()).append("_").append(std::to_string(it));
			task->SetVar(str.c_str(), 0);

			if (player->GetScene())
			{
				SceneDataEntry* sceneEntry = player->GetScene()->GetDataEntry();
				if (sceneEntry != NULL)
				{
					if (sceneEntry->subType == SCENE_SUB_LOST_DUNGEON_BATTLE)
						enemyDungeonMap[it] = 0;
					else if (sceneEntry->subType == SCENE_SUB_LOST_DUNGEON_ENTERY)
						enemyDungeonMap[it] = sceneEntry->dungeonId;
				}
			}

			--killNum;
			if (killNum == 0)
				break;
		}
	}
	else if (task->LostDungeonTaskType() == LDTT_TASK_CLEAR_FIX_DUNGEON
		|| task->LostDungeonTaskType() == LDTT_TASK_KILL_FIX_MONSTER
		|| task->LostDungeonTaskType() == LDTT_TASK_COLLECT_ITEM)
	{
		for (auto& it : taskDataEntry->valMap)
		{
			task->SetVar(std::to_string(it.first).c_str(), 0);
		}
	}
	else
	{
		task->SetVar("num", 0);
	}

	if (!AddTask(task))
	{
		CL_SAFE_DELETE(task);
		return false;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << " accept task id:" << task->GetDataId() << LogStream::eos;

	CLProtocol::SceneLostDungeonTaskDataNotify notify;
	notify.taskId = task->GetDataId();
	notify.status = task->GetStatus();
	notify.vars = task->GetQuestVar();
	m_pOwner->SendProtocol(notify);

	// ɱ�̶��ˣ�ѡ����ʱ��Ҫ�����ǲ����ڵ��³�
	if (task->LostDungeonTaskType() == LDTT_TASK_KILL_FIX_ROLE)
	{
		CLProtocol::SceneLostDungeonEnemyInDungeon InDungeon;
		for (auto& it : enemyDungeonMap)
		{
			InDungeon.enemyId = it.first;
			InDungeon.dungeonId = it.second;
		}
		m_pOwner->SendProtocol(InDungeon);
	}

	return true;
}

void LostDungeonTaskMgr::OnHeartbeat(const Avalon::Time& now)
{
	if (GetOwner()->GetLostDungBatSt() == LDPBT_NORMAL)
	{
		UInt32 sceneSubType = 0;
		if (GetOwner()->GetScene() && GetOwner()->GetScene()->GetDataEntry())
		{
			sceneSubType = GetOwner()->GetScene()->GetDataEntry()->subType;
		}

		if (sceneSubType == SCENE_SUB_LOST_DUNGEON_BATTLE)
		{
			m_AliveTime++;

			// ��10��֪ͨһ��
			if (m_AliveTime >= 10 && m_AliveTime % 10 == 0)
			{
				OnAliveTime();
			}

			if (GetTaskRefresh())
			{
				SetTaskRefresh(false);
				HandlerRandTaskList(m_TaskGrade);

				InfoLogStream << PLAYERINFO(GetOwner()) << "player refresh rand task grade:" << m_TaskGrade << LogStream::eos;
			}
		}
	}
}

void LostDungeonTaskMgr::OnSceneChanged(UInt32 sceneId)
{
	SceneDataEntry* data = SceneDataEntryMgr::Instance()->FindEntry(sceneId);
	if (NULL == data)
	{
		Scene* scene =  SceneMgr::Instance()->FindScene(sceneId);
		if (scene != NULL)
		{
			data = scene->GetDataEntry();
		}
		else
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "SceneDataEntry is null! id = " << sceneId << LogStream::eos;
			return;
		}
	}

	if (data->subType == SCENE_SUB_LOST_DUNGEON_BATTLE)
	{
		NotifyKillMePlayerInDungeon(0);
	}
	else if(data->subType == SCENE_SUB_LOST_DUNGEON_ENTERY)
	{
		NotifyKillMePlayerInDungeon(data->dungeonId);
	}
}

void LostDungeonTaskMgr::OnAliveTime()
{
	class OnAliveTimeVisitor : public BaseTaskVisitor
	{
	public:
		OnAliveTimeVisitor(UInt32 time) : m_time(time) {}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH && task->LostDungeonTaskType() == LDTT_TASK_ALIVE_TIME)
			{
				TaskScript* script = ScriptMgr::Instance()->FindLostDungeonTaskScript(task->GetDataId());
				if (script != NULL)
				{
					script->OnLostDungeonAliveTime(task->GetOwner(), task, m_time);
				}
			}
			return true;
		}

	private:
		UInt32 m_time;
	};

	// ����ʱ�������
	OnAliveTimeVisitor visitor(m_AliveTime);
	VisitTasks(visitor);
}

void LostDungeonTaskMgr::OnKillSomeOne(UInt64 dieId)
{
	class OnKillSomeOneVisitor : public BaseTaskVisitor
	{
	public:
		OnKillSomeOneVisitor(UInt64 dieId, UInt32 killNum) : m_DieId(dieId), m_KillNum(killNum) {}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (task->LostDungeonTaskType() == LDTT_TASK_KILL_FIX_ROLE)
				{
					std::vector<UInt32> tempVec;
					for (auto& iter : task->GetQuestVar())
					{
						std::vector<std::string> param;
						Avalon::StringUtil::Split(iter.first, param, "_");
						if (param.size() != 2)
						{
							ErrorLogStream << "kill player error! id:" << iter.first << LogStream::eos;
							continue;
						}

						UInt64 id = Avalon::StringUtil::ConvertToValue<UInt64>(param[1]);
						if (id == m_DieId)
						{
							tempVec.push_back(1);
							task->SetVar(iter.first.c_str(), 1);
							task->SyncData();
						}
						else
						{
							UInt32 isKill = Avalon::StringUtil::ConvertToValue<UInt64>(iter.second);
							tempVec.push_back(isKill);
						}
					}

					bool isFinish = true;
					for (auto& it :tempVec)
					{
						if (it == 0)
						{
							isFinish = false;
							break;
						}
					}

					if (isFinish)
						task->SetStatus(TASK_FINISHED);
				}
				else if (task->LostDungeonTaskType() == LDTT_TASK_KILL_ROLE_NUM)
				{
					TaskScript* script = ScriptMgr::Instance()->FindLostDungeonTaskScript(task->GetDataId());
					if (script != NULL)
						script->OnLostDungeonKillRoleNum(task->GetOwner(), task, m_KillNum);
				}
			}

			return true;
		}

	private:
		UInt64 m_DieId;
		UInt32 m_KillNum;
	};

	m_KillRoleMap[dieId]++;

	OnKillSomeOneVisitor visitor(dieId, CalMapNum(m_KillRoleMap));
	VisitTasks(visitor);
}

void LostDungeonTaskMgr::OnOwnerDead()
{
	// �Լ�����ʧ��
	for (auto& it : m_Tasks)
	{
		it.second->SetStatus(TASK_FAILED);
		//OnTaskResultNotify(it.first, 0);
	}

	// �������������
	for (auto& iter : m_KillMeSet)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(iter);
		if (NULL == player)
		{
			continue;
		}

		player->GetLostDungeonTaskMgr().OnKillSomeOne(GetOwner()->GetID());
	}

	m_KillMeSet.clear();
}

void LostDungeonTaskMgr::OnClearDungeon(UInt32 dungeonId)
{
	class OnClearDungeonVisitor : public BaseTaskVisitor
	{
	public:
		OnClearDungeonVisitor(UInt32 dungeonId, UInt32 clearNum, UInt32 totalNum)
			: m_DungeonId(dungeonId), m_ClearNum(clearNum), m_TotalNum(totalNum){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindLostDungeonTaskScript(task->GetDataId());
				if (NULL == script)
					return true;

				if (task->LostDungeonTaskType() == LDTT_TASK_CLEAR_FIX_DUNGEON)
				{
					script->OnLostDungeonFixClear(task->GetOwner(), task, m_DungeonId, m_ClearNum);
				}
				else if (task->LostDungeonTaskType() == LDTT_TASK_CLEAR_DUNGEON)
				{
					script->OnLostDungeonClearNum(task->GetOwner(), task, m_TotalNum);
				}
			}

			return true;
		}

	private:
		UInt32 m_DungeonId;
		UInt32 m_ClearNum;
		UInt32 m_TotalNum;
	};

	// С�ֹؿ������������
	DungeonDataEntry* dungeonEntry = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (NULL == dungeonEntry || dungeonEntry->threeType == DTT_LOST_DUNGEON_MONSTER)
	{
		return;
	}

	m_ClearDungeonMap[dungeonId]++;

	OnClearDungeonVisitor visitor(dungeonId, m_ClearDungeonMap[dungeonId], CalMapNum(m_ClearDungeonMap));
	VisitTasks(visitor);
}

void LostDungeonTaskMgr::OnKillMonster(UInt32 monsterId, UInt32 num)
{
	class OnKillMonsterVisitor : public BaseTaskVisitor
	{
	public:
		OnKillMonsterVisitor(UInt32 monsterId, UInt32 killNum) : m_MonsterId(monsterId), m_KillNum(killNum) {}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindLostDungeonTaskScript(task->GetDataId());
				if (NULL == script)
					return true;

				if (task->LostDungeonTaskType() == LDTT_TASK_KILL_FIX_MONSTER)
				{
					script->OnLostDungeonKillFixMonster(task->GetOwner(), task, m_MonsterId, m_KillNum);
				}
			}

			return true;
		}

	private:
		UInt32 m_MonsterId;
		UInt32 m_KillNum;
	};

	m_KillMonsterMap[monsterId] += num;

	OnKillMonsterVisitor visitor(monsterId, m_KillMonsterMap[monsterId]);
	VisitTasks(visitor);
}

void LostDungeonTaskMgr::OnCollectItem(UInt32 itemId, UInt32 itemNum)
{
	class OnGetItemVisitor : public BaseTaskVisitor
	{
	public:
		OnGetItemVisitor(UInt32 itemId, UInt32 itemNum) : m_ItemId(itemId), m_ItemNum(itemNum) {}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindLostDungeonTaskScript(task->GetDataId());
				if (NULL == script)
					return true;

				if (task->LostDungeonTaskType() == LDTT_TASK_COLLECT_ITEM)
				{
					script->OnLostDungeonCollectItem(task->GetOwner(), task, m_ItemId, m_ItemNum);
				}
			}

			return true;
		}

	private:
		UInt32 m_ItemId;
		UInt32 m_ItemNum;
	};

	m_GetItemMap[itemId] += itemNum;

	OnGetItemVisitor visitor(itemId, m_GetItemMap[itemId]);
	VisitTasks(visitor);
}

void LostDungeonTaskMgr::OnTaskResultNotify(UInt32 taskId, UInt32 result)
{
	CLProtocol::SceneLostDungeonTaskResultNotify notify;
	notify.taskId = taskId;
	notify.result = result;
	GetOwner()->SendProtocol(notify);

	InfoLogStream << PLAYERINFO(GetOwner()) << "task result id:" << taskId << " result:" << result << LogStream::eos;
}

void LostDungeonTaskMgr::AddKillMePlayer(UInt64 playerId)
{
	m_KillMeSet.insert(playerId);
}

void LostDungeonTaskMgr::RemoveKillMePlayer(UInt64 playerId)
{
	m_KillMeSet.erase(playerId);
}

void LostDungeonTaskMgr::NotifyKillMePlayer()
{
	for (auto& it : m_KillMeSet)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(it);
		if (NULL == player)
		{
			continue;
		}

		// ֪ͨ�����ɱ������ʧ��
		player->GetLostDungeonTaskMgr().KillTargetLeave(GetOwner()->GetID());
	}

	m_KillMeSet.clear();
}

void LostDungeonTaskMgr::NotifyKillMePlayerInDungeon(UInt32 dungeonId)
{
	if (m_KillMeSet.empty())
		return;

	CLProtocol::SceneLostDungeonEnemyInDungeon notify;
	notify.dungeonId = dungeonId;
	notify.enemyId = GetOwner()->GetID();

	for (auto& it : m_KillMeSet)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(it);
		if (NULL == player)
		{
			continue;
		}

		player->SendProtocol(notify);
	}
}

void LostDungeonTaskMgr::NotifyMeKillPlayer()
{
	for (auto& it : m_KillRoleSet)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(it);
		if (NULL == player)
		{
			continue;
		}

		player->GetLostDungeonTaskMgr().RemoveKillMePlayer(GetOwner()->GetID());
	}

	m_KillRoleSet.clear();
}

void LostDungeonTaskMgr::KillTargetLeave(UInt64 playerId)
{
	auto it = m_KillRoleSet.find(playerId);
	if (it == m_KillRoleSet.end())
		return;

	for (auto& iter : m_Tasks)
	{
		if (iter.second->LostDungeonTaskType() == LDTT_TASK_KILL_FIX_ROLE)
		{
			GetOwner()->SendNotify(8308);
			SetTaskRefresh(true);
		}
	}

	m_KillRoleSet.clear();
}

void LostDungeonTaskMgr::ResetData(LostBattleScene* lbScene)
{
	m_AliveTime = 0;
	m_ClearDungeonMap.clear();
	m_KillMonsterMap.clear();
	m_GetItemMap.clear();
	m_TaskChoiceVec.clear();
	m_KillMeSet.clear();

	for (auto& it: m_Tasks)
	{
		if (lbScene != NULL)
		{
			lbScene->RemoveTaskAcceptNum(it.first);
		}

		CL_SAFE_DELETE(it.second);
	}

	m_Tasks.clear();
}

UInt32 LostDungeonTaskMgr::CalMapNum(std::map<UInt32, UInt32>& numMap)
{
	UInt32 num = 0;
	for (auto& it : numMap)
	{
		num += it.second;
	}

	return num;
}

UInt32 LostDungeonTaskMgr::CalMapNum(std::map<UInt64, UInt32>& numMap)
{
	UInt32 num = 0;
	for (auto& it : numMap)
	{
		num += it.second;
	}

	return num;
}

bool LostDungeonTaskMgr::IsFinishTask()
{
	bool isFinish = true;
	for (auto& iter : m_Tasks)
	{
		if (iter.second->GetStatus() != TASK_FINISHED)
		{
			isFinish = false;
			break;
		}
	}

	return isFinish;
}

UInt32 LostDungeonTaskMgr::CurTaskId()
{
	if (m_Tasks.empty())
	{
		return 0;
	}

	return m_Tasks.begin()->first;
}

bool LostDungeonTaskMgr::IsKillFixNumTask()
{
	if (m_Tasks.empty())
	{
		return false;
	}

	return m_Tasks.begin()->second->LostDungeonTaskType() == LDTT_TASK_KILL_ROLE_NUM;
}

void LostDungeonTaskMgr::GetDungeonTaskList(std::vector<UInt32>& taskList)
{
	for (auto& iter : m_Tasks)
	{
		if (iter.second->LostDungeonTaskType() == LDTT_TASK_COLLECT_ITEM)
		{
			taskList.push_back(iter.first);
		}
	}
}

void LostDungeonTaskMgr::RestoreFailTask()
{
	for (auto& iter : m_Tasks)
	{
		if (iter.second->GetStatus() == TASK_FAILED)
		{
			iter.second->SetStatus(TASK_UNFINISH);
		}
	}
}