#include "AccountActivityTaskMgr.h"

#include <CLActiveEventDataEntry.h>
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include <CLTaskProtocol.h>
#include <CLErrorCode.h>
#include <CLDrawPrizeDataEntry.h>

#include "WSActivityMgr.h"
#include "WSPlayerMgr.h"
#include "WSSelectInitDataCallback.h"
#include "WSItemTransaction.h"


/**
*@brief 任务扣除道具检查
*/
class ActivityTaskRmItemTransaction : public ItemTransaction
{
public:
	ActivityTaskRmItemTransaction(WSPlayer *player, const std::vector<UInt32>& taskIds)
		: ItemTransaction(player), m_TaskIds(taskIds) {}
	~ActivityTaskRmItemTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		if (AccountActivityTaskMgr::Instance()->OnCheckReSignInSevenTaskRet(player, m_TaskIds))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_SIGN_IN_SEVEN));
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		UnLock();
	}

private:
	std::vector<UInt32> m_TaskIds;
};

class AccountChargeRebateSelectPlayerInfoCallback : public CLRecordCallback
{
public:
	AccountChargeRebateSelectPlayerInfoCallback(ObjID_t roleId, WSActivityTask* task, UInt16 levelLimit) 
		: m_RoleId(roleId), m_Task(task), m_LevelLimit(levelLimit) {}

	virtual void OnFinished()
	{
		if (!NextRow())
		{
			ErrorLogStream << "Can not find player(" << m_RoleId << ")!" << LogStream::eos;
			return;
		}

		if (!m_Task)
		{
			ErrorLogStream << "Find player(" << m_RoleId << "), but WSActivity task ptr is null!" << LogStream::eos;
			return;
		}

		if (m_Task->GetStatus() != TASK_UNFINISH)
		{
			ErrorLogStream << "Player(" << m_RoleId << ")'s task(" << m_Task->GetUID() << ", " << m_Task->GetTaskId() << ")'s status=" << m_Task->GetStatus() << LogStream::eos;
			return;
		}

		UInt16 playerLevel = GetData("level");
		if (playerLevel >= m_LevelLimit)
		{
			Int64 cnt = m_Task->GetVar("charge_cnt");
			m_Task->SetVarNoSync("charge_cnt", cnt + 1);
			m_Task->SetStatusNoSync(TASK_FINISHED);
			m_Task->SaveToDB();
		}
		else
		{
			ErrorLogStream << "Player(" << m_RoleId << ")'s level=" << playerLevel << ", but limit=" << m_LevelLimit << LogStream::eos;
		}
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Select player(" << m_RoleId << ") failed!" << LogStream::eos;
	}

private:
	ObjID_t m_RoleId;
	WSActivityTask* m_Task;
	UInt16  m_LevelLimit;
};


AccountActivityTaskMgr::AccountActivityTaskMgr()
{
	m_OpenCheck = true;
	m_OpenCheckDrawCount = true;
	m_DailyTimer.SetTime(6);
}

AccountActivityTaskMgr::~AccountActivityTaskMgr()
{

}

void AccountActivityTaskMgr::OnLoadTaskInfo(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		auto task = FindActivityTask(callback->GetData("accid"), callback->GetData("task_id"));
		if (!task)
		{
			task = new WSActivityTask();
			task->SetUID(callback->GetKey());
			task->SetAccId(callback->GetData("accid"));
			task->SetActivityId(callback->GetData("activity_id"));
			task->SetTaskId(callback->GetData("task_id"));
			task->SetStatusNoSync(callback->GetData("status"));
			task->DecodeVar(callback->GetData("params").GetString());
			task->SetAcceptTime(callback->GetData("accept_time"));
			task->SetUpdateTime(callback->GetData("update_time"));
			task->SetDayChangeTime(callback->GetData("day_change_time"));

			AddActivityTaskNoCheck(task);
		}

		TryRefreshTaskByActivityNoSync(task);
	}
}

WSActivityTask* AccountActivityTaskMgr::CreateActivityTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData)
	{
		ErrorLogStream << "WSPlayer ptr or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return NULL;
	}

	WSActivityTask* task = new WSActivityTask();
	task->SetAccId(player->GetAccId());
	task->SetActivityId(taskData->templateID);
	task->SetTaskId(taskData->id);
	task->SetStatus(player, (UInt8)TaskStatus::TASK_UNFINISH);
	task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
	task->SetDBFlag(df_insert);

	DebugLogStream << PLAYERINFO(player) << "create new task(" << task->GetTaskId() << ")." << LogStream::eos;

	return task;
}

bool AccountActivityTaskMgr::AddActivityTask(WSActivityTask* task)
{
	if (!task)
	{
		ErrorLogStream << "WSActivityTask ptr is null!" << LogStream::eos;
		return false;
	}

	if (FindActivityTask(task->GetAccId(), task->GetTaskId()) != NULL)
	{
		ErrorLogStream << "Can not add the same activity task(" << task->GetTaskId() << ")!" << LogStream::eos;
		return false;
	}

	m_AccountActivityTasks[task->GetAccId()].push_back(task);

	return true;
}

void AccountActivityTaskMgr::AddActivityTaskNoCheck(WSActivityTask* task)
{
	m_AccountActivityTasks[task->GetAccId()].push_back(task);
}

WSActivityTask* AccountActivityTaskMgr::FindActivityTask(UInt32 accid, UInt32 taskId)
{
	auto iter = m_AccountActivityTasks.find(accid);
	if (iter == m_AccountActivityTasks.end())
	{
		return NULL;
	}

	for (auto task : iter->second)
	{
		if (!task)
		{
			ErrorLogStream << "player(" << accid << ")'s activity task(" << taskId << ") ptr is null!" << LogStream::eos;
			continue;
		}

		if (task->GetTaskId() == taskId)
		{
			return task;
		}
	}

	return NULL;
}

void AccountActivityTaskMgr::GetActivityTaskByAccID(UInt32 accid, std::vector<WSActivityTask*>& tasks)
{
	auto iter = m_AccountActivityTasks.find(accid);
	if (iter != m_AccountActivityTasks.end())
	{
		tasks = iter->second;
	}
}

//void AccountActivityTaskMgr::AcceptTask(WSPlayer* player)
//{
//	if (!player)
//	{
//		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
//		return;
//	}
//
//	const std::vector<ActivityInfo>& openActivitys = WSActivityMgr::Instance()->GetOpenActivitys();
//
//	if (openActivitys.empty())
//	{
//		InfoLogStream << PLAYERINFO(player) << "There no open activity!" << LogStream::eos;
//	}
//
//	for (auto activity : openActivitys)
//	{
//		std::vector<ActiveTaskDataEntry*> datas;
//		ActiveTaskDataEntryMgr::Instance()->GetActives(activity.id, datas);
//
//		for (auto data : datas)
//		{
//			if (!data) continue;
//			if (data->taskBelong == 0) continue;
//
//			if (CheckCond() && !data->applyChannel.empty() && data->applyChannel != player->GetPf())
//			{
//				continue;
//			}
//
//			switch ((ActiveID)data->templateID)
//			{
//			case AID_LOGIN_PRIVILEGE:
//			case AID_VIVO_LOGIN_PRIVILEGE:
//				_AcceptLoginPrivilegeTask(player, data);
//				break;
//
//			case AID_SIGN_IN_SEVEN:
//				_AcceptSignInSevenTask(player, data);
//				break;
//
//			case AID_SIGN_IN_SEVEN_FINISH:
//				_AcceptSignInSevenFinishTask(player, data);
//				break;
//
//			case AID_DRAW_PRIZE:
//				_AcceptDrawPrizeTask(player, data);
//				break;
//
//			case AID_AMBER_PRIVILEGE:
//				_AcceptAmberPrivilegeTask(player, data);
//				break;
//
//			case AID_ACCOUNT_CHARGE_REBATE:
//				_AcceptChargeRebateTask(player, data);
//				break;
//
//			default: break;
//			}
//		}
//
//		switch ((ActiveID)activity.id)
//		{
//		case AID_AMBER_PRIVILEGE:
//			if (IsOppoPlayer(player))
//			{
//				player->RequestGlobalServerInfo();
//			}
//			break;
//
//		default:break;
//		}
//	}
//}

void AccountActivityTaskMgr::AcceptActivities(WSPlayer* player)
{
	if (!player) return;

	const std::vector<ActivityInfo>& openActivitys = WSActivityMgr::Instance()->GetOpenActivitys();

	if (openActivitys.empty())
	{
		InfoLogStream << PLAYERINFO(player) << "There no open activity!" << LogStream::eos;
		return;
	}

	for (auto activity : openActivitys)
	{
		AcceptActivityTasks(player, activity);

		switch ((ActiveID)activity.id)
		{
		case AID_AMBER_PRIVILEGE:
			if (IsOppoPlayer(player))
			{
				player->RequestGlobalServerInfo();
			}
			break;

		default:break;
		}
	}
}

void AccountActivityTaskMgr::AcceptActivityTasks(WSPlayer* player, const ActivityInfo& actInfo)
{
	if (!player) return;

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(actInfo.id, datas);

	if (datas.empty())
	{
		DebugLogStream << PLAYERINFO(player) << " activity(" << actInfo.id << ")'s task datas are empty!" << LogStream::eos;
		return;
	}

	for (auto taskData : datas)
	{
		if (!taskData) continue;

		// 取账号任务
		if (taskData->taskBelong != 1) continue;

		// 渠道筛选
		if (CheckCond() && !taskData->applyChannel.empty() && taskData->applyChannel != player->GetPf())
		{
			continue;
		}

		// 旧的任务处理方式(以后不要在这里加了)
		switch ((ActiveID)taskData->templateID)
		{
		case AID_LOGIN_PRIVILEGE:
		case AID_VIVO_LOGIN_PRIVILEGE:
			_AcceptLoginPrivilegeTask(player, taskData);
			break;

		case AID_SIGN_IN_SEVEN:
			_AcceptSignInSevenTask(player, taskData);
			break;

		case AID_SIGN_IN_SEVEN_FINISH:
			_AcceptSignInSevenFinishTask(player, taskData);
			break;

		case AID_DRAW_PRIZE:
			_AcceptDrawPrizeTask(player, taskData);
			break;

		case AID_AMBER_PRIVILEGE:
			_AcceptAmberPrivilegeTask(player, taskData);
			break;

		case AID_ACCOUNT_CHARGE_REBATE:
			_AcceptChargeRebateTask(player, taskData);
			break;

		default: break;
		}

		// 新的处理方式
		// 接受任务
		auto task = FindActivityTask(player->GetAccId(), taskData->id);
		if (!task)
		{
			task = new WSActivityTask();
			task->SetAccId(player->GetAccId());
			task->SetActivityId(actInfo.id);
			task->SetTaskId(taskData->id);
			task->SetStatus(player, (UInt8)TaskStatus::TASK_UNFINISH);
			task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
			task->SetDBFlag(df_insert);
			AddActivityTaskNoCheck(task);
			InfoLogStream << PLAYERINFO(player) << " create " << task->LogStr() << LogStream::eos;
		}
		else
		{
			task->SyncStatus(player);
		}

		TryRefreshTaskByActivity(player, actInfo, task);
	}
}

void AccountActivityTaskMgr::GiveRewards(WSPlayer* player, WSActivityTask* task, bool notify)
{
	if (!player || !task)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	CLProtocol::WorldActivityTaskSendRewards protocol;
	protocol.roleId = player->GetID();
	protocol.taskId = task->GetTaskId();
	GetRewards(task, protocol.rewards);
	protocol.notify = (notify == true ? 1 : 0);

	player->SendToScene(protocol);
}

void AccountActivityTaskMgr::GiveRewards(WSPlayer* player, WSActivityTask* task, const ItemRewardVec& rewards, bool notify)
{
	if (!player || !task)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	CLProtocol::WorldActivityTaskSendRewards protocol;
	protocol.roleId = player->GetID();
	protocol.taskId = task->GetTaskId();
	protocol.rewards = rewards;
	protocol.notify = notify == true ? 1 : 0;

	player->SendToScene(protocol);
}

void AccountActivityTaskMgr::GetRewards(WSActivityTask* task, std::vector<ItemReward>& rewards)
{
	if (!task)
	{
		ErrorLogStream << "WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	auto data = ActiveTaskDataEntryMgr::Instance()->FindEntry(task->GetTaskId());
	if (!data)
	{
		ErrorLogStream << "Can nnt find activity task data(" << task->GetTaskId() << ")!" << LogStream::eos;
		return;
	}

	rewards = data->rewards;
}

void AccountActivityTaskMgr::SubmitTask(WSPlayer* player, UInt32 taskId)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	WSActivityTask* task = FindActivityTask(player->GetAccId(), taskId);
	if (!task)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << taskId << ")!" << LogStream::eos;
		return;
	}

	if (_IsLimit(player, task))
	{
		return;
	}

	bool submitSuccess = false;
	switch ((ActiveID)task->GetActivityId())
	{
	case AID_LOGIN_PRIVILEGE:
	case AID_SIGN_IN_SEVEN:
	case AID_SIGN_IN_SEVEN_FINISH:
	case AID_AMBER_PRIVILEGE:
	case AID_VIVO_LOGIN_PRIVILEGE:
	case AID_ACCOUNT_TOTAL_CHARGE:
	case AID_OPPO_VIP_REWARD:
		submitSuccess = _SubmitClickRewardTask(player, task);
		break;
	case AID_ACCOUNT_CHARGE_REBATE:
		submitSuccess = _SubmitAccountChargeRebateTask(player, task);
		break;
	case AID_DRAW_PRIZE:
		submitSuccess = _SubmitDrawPrizeTask(player, task);
		break;

	default: break;
	}

	if (submitSuccess)
	{
		if (task->GetStatus() == TASK_SUBMITTING)
		{
			task->SetStatus(player, TASK_OVER);
			task->SaveToDB();
		}

		switch ((ActiveID)task->GetActivityId())
		{
		case AID_SIGN_IN_SEVEN:
			_UpdateSignInSevenTaskAfterSubmit(player, task);
			_UpdateSignInSevenFinishTask(player);
			break;

		default: break;
		}
	}
}

void AccountActivityTaskMgr::TryRefreshTaskByActivity(WSPlayer* player, const ActivityInfo& actInfo, WSActivityTask* task)
{
	if (!player || !task) return;

	switch ((ActiveID)actInfo.id)
	{
	case AID_ACCOUNT_CHARGE_REBATE: break;
	case AID_ACCOUNT_TOTAL_CHARGE: break;
	default: return;
	}

	UInt32 taskUpdateTime = task->GetUpdateTime();
	if (actInfo.startTime <= taskUpdateTime && taskUpdateTime <= actInfo.dueTime)
	{
		return;
	}

	task->Refresh(player);
}

void AccountActivityTaskMgr::TryRefreshTaskByActivityNoSync(WSActivityTask* task)
{
	if (!task) return;

	ActivityInfo actInfo;
	if (!WSActivityMgr::Instance()->FindActivity(task->GetActivityId(), actInfo))
	{
		ErrorLogStream << "Can't find activity(" << task->GetActivityId() << ")!" << LogStream::eos;
		return;
	}

	switch ((ActiveID)actInfo.id)
	{
	case AID_ACCOUNT_CHARGE_REBATE: break;
	case AID_ACCOUNT_TOTAL_CHARGE: break;
	default: return;
	}

	TryRefreshTaskByActivityNoSync(actInfo, task);
}

void AccountActivityTaskMgr::TryRefreshTaskByActivityNoSync(const ActivityInfo& actInfo, WSActivityTask* task)
{
	if (!task) return;

	switch ((ActiveID)actInfo.id)
	{
	case AID_ACCOUNT_CHARGE_REBATE: break;
	case AID_ACCOUNT_TOTAL_CHARGE: break;
	default: return;
	}

	UInt32 taskUpdateTime = task->GetUpdateTime();
	if (actInfo.startTime <= taskUpdateTime && taskUpdateTime <= actInfo.dueTime)
	{
		return;
	}

	task->RefreshNoSync();
}

void AccountActivityTaskMgr::ReSignInSevenTask(WSPlayer* player, const std::vector<UInt32>& taskIds)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	if (taskIds.empty())
	{
		ErrorLogStream << PLAYERINFO(player) << "resubmit task failed, taskIds empty!" << LogStream::eos;
		return;
	}

	for (auto id : taskIds)
	{
		auto task = FindActivityTask(player->GetAccId(), id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << id << ")!" << LogStream::eos;
			continue;;
		}

		if (_IsLimit(player, task))
		{
			return;
		}
	}

	UInt32 costItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SIGN_IN_SEVEN_TASK_RE_SUBMIT_COST_ITEM);
	UInt32 costNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SIGN_IN_SEVEN_TASK_RE_SUBMIT_COST_NUM);
	costNum *= (UInt32)taskIds.size();

	auto trans = new ActivityTaskRmItemTransaction(player, taskIds);
	trans->SetItem(costItemId, costNum);
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << " resubmit task failed, begin transaction failed." << LogStream::eos;
	}
}

bool AccountActivityTaskMgr::OnCheckReSignInSevenTaskRet(WSPlayer* player, const std::vector<UInt32>& taskIds)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return false;
	}

	for (auto id : taskIds)
	{
		auto task = FindActivityTask(player->GetAccId(), id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << id << ")!" << LogStream::eos;
			continue;;
		}

		if (task->GetStatus() != TASK_FAILED)
		{
			ErrorLogStream << PLAYERINFO(player) << "Sign in seven task status(" << task->GetStatus() << ") is error!" << LogStream::eos;
			return false;
		}

		task->SetStatus(player, TASK_OVER);

		GiveRewards(player, task);
	}

	_UpdateSignInSevenFinishTask(player);
	return true;
}

void AccountActivityTaskMgr::UpdateAmberPrivilegeTask(WSPlayer* player)
{
	if (!player || !IsOppoPlayer(player)) return;

	UInt8 oppoAmberLevel = player->GetOppoAmberLevel();

	InfoLogStream << PLAYERINFO(player) << "start to updata amber privilege(" << oppoAmberLevel << ") task." << LogStream::eos;

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_AMBER_PRIVILEGE, datas);

	UInt32 matchTaskId = GetTaskIdByOppoAmberLevel(oppoAmberLevel);

	for (auto taskData : datas)
	{
		auto task = FindActivityTask(player->GetAccId(), taskData->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << taskData->id << ")!" << LogStream::eos;
			continue;
		}

		if (task->GetTaskId() == matchTaskId)
		{
			//普通用户没有奖励(这里设置FAILED,是为了在客户端展示)
			if (oppoAmberLevel == (UInt8)OAL_NONE)
			{
				task->SetStatus(player, TASK_FAILED);
			}
			else
			{
				if (task->GetStatus() == TASK_UNFINISH)
				{
					task->SetStatus(player, TASK_FINISHED);
				}
			}
		}
		else
		{
			task->SetStatus(player, TASK_UNFINISH);
		}

		task->SaveToDB();
	}
}

void AccountActivityTaskMgr::UpdateOppoVipLevelTask(WSPlayer* player)
{
	if (!player || !IsOppoPlayer(player))
	{
		return;
	}

	// 判断活动是否开启
	ActivityInfo actInfo;
	if (!WSActivityMgr::Instance()->FindActivity(AID_OPPO_VIP_REWARD, actInfo))
	{
		ErrorLogStream << "player(" << player->GetID() << ") activity(" << (UInt32)AID_OPPO_VIP_REWARD << ") unexist!" << LogStream::eos;
		return;
	}

	if (actInfo.state != AS_IN)
	{
		ErrorLogStream << "player(" << player->GetID() << ") activity(" << actInfo.id << ") is closed!" << LogStream::eos;
		return;
	}

	UInt8 oppoAmberLevel = player->GetOppoAmberLevel();
	InfoLogStream << PLAYERINFO(player) << "start to updata oppo vip level(" << oppoAmberLevel << ") task." << LogStream::eos;

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(actInfo.id, datas);

	for (auto taskData : datas)
	{
		auto task = FindActivityTask(player->GetAccId(), taskData->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "can not find oppo activity task(" << taskData->id << ")!" << LogStream::eos;
			continue;
		}

		if (task->GetStatus() != TASK_UNFINISH)
		{
			continue;
		}

		if (taskData->param1 <= oppoAmberLevel)
		{
			task->SetStatus(player, TASK_FINISHED);

			InfoLogStream << "player(" << player->GetID() << ") finish oppo vip level activity task(" << taskData->id << ") current level(" << oppoAmberLevel << ")." << LogStream::eos;

			task->SaveToDB();
		}
	}
}

void AccountActivityTaskMgr::OnTick(const Avalon::Time& now)
{
	if (m_DailyTimer.IsTimeout(now))
	{
		OnLogicDayChange();
	}
}

void AccountActivityTaskMgr::OnOnline(WSPlayer* player, bool logicDayChangesd)
{
	if (!player) return;

	//同步客户端活动(账号活动)
	CLProtocol::SceneSyncClientActivities sync;
	WSActivityMgr::Instance()->GetAccountActivity(sync.activities, player);
	player->SendProtocol(sync);

	//AcceptTask(player);
	AcceptActivities(player);

	if (logicDayChangesd)
	{
		OnLogicDayChangeForOnePlayer(player);
	}

	// 同步所有的活动任务
	{
		auto accItr = m_AccountActivityTasks.find(player->GetAccId());
		if (accItr != m_AccountActivityTasks.end())
		{
			CLProtocol::SceneSyncActiveTaskList taskList;

			auto& tasks = accItr->second;
			for (auto task : tasks)
			{
				if (!task || task->GetActivityId() == AID_DRAW_PRIZE)
				{
					continue;
				}

				TaskBriefInfo info;
				info.id = task->GetTaskId();
				info.status = task->GetStatus();
				info.vars = task->GetVars();
				taskList.tasks.push_back(info);
			}

			player->SendProtocol(taskList);
		}
	}

}
	

void AccountActivityTaskMgr::OnLogicDayChange()
{
	InfoLogStream << "on logic day change..." << LogStream::eos;

	class VisitNormalPlayerVisitor : public WSPlayerVisitor
	{
	public:
		bool operator()(WSPlayer* player)
		{
			AccountActivityTaskMgr::Instance()->OnLogicDayChangeForOnePlayer(player);
			return true;
		}
	};

	VisitNormalPlayerVisitor visitor;
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void AccountActivityTaskMgr::OnLogicDayChangeForOnePlayer(WSPlayer* player)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	std::vector<WSActivityTask*> tasks;
	GetActivityTaskByAccID(player->GetAccId(), tasks);

	for (auto task : tasks)
	{
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Activity task ptr is null!" << LogStream::eos;
			continue;
		}

		if (!_CanChangeOnDayChange(task))
		{
			continue;
		}

		task->SetDayChangeTime((UInt32)CURRENT_TIME.Sec());
		task->SaveToDB(false);

		switch ((ActiveID)task->GetActivityId())
		{
		case AID_LOGIN_PRIVILEGE:
		case AID_VIVO_LOGIN_PRIVILEGE:
			_OnLogicDayChangeForLoginPrivilegeTask(player, task);
			break;

		case AID_SIGN_IN_SEVEN:
			_OnLogicDayChangeForSignInSevenTask(player, task);
			break;

		case AID_DRAW_PRIZE:
			_OnLogicDayChangeForDrawPrizeTask(player, task);
			break;

		case AID_AMBER_PRIVILEGE:
			_OnLogicDayChangeForAmberPrivilegeTask(player, task);
			break;

		default: break;
		}
	}
}

void AccountActivityTaskMgr::OnActivityOpen(const ActivityInfo& actInfo)
{
	class VisitNormalPlayerVisitor : public WSPlayerVisitor
	{
	public:
		VisitNormalPlayerVisitor(const ActivityInfo& actInfo) :m_ActivityInfo(actInfo) {}

		bool operator()(WSPlayer* player)
		{
			AccountActivityTaskMgr::Instance()->AcceptActivityTasks(player, m_ActivityInfo);

			return true;
		}

	private:
		ActivityInfo m_ActivityInfo;
	};

	VisitNormalPlayerVisitor visitor(actInfo);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
	InfoLogStream << "Activity(" << actInfo.id << ") open!" << LogStream::eos;
}

void AccountActivityTaskMgr::OnCharge(UInt32 accid, ObjID_t roleId, UInt32 num)
{
	_OnChargeForChargeRebateTask(accid, roleId, num);
}

bool AccountActivityTaskMgr::_CanChangeOnDayChange(WSActivityTask* task)
{
	if (!task) return false;

	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();
	UInt32 lastDayChangeTime = task->GetDayChangeTime();

	if (lastDayChangeTime == 0)
	{
		return true;
	}

	// 预期跨天时间
	UInt32 expectTime = (UInt32)(GetDayBeginTime2((UInt64)lastDayChangeTime * SEC_TO_MSEC) / SEC_TO_MSEC) + DAY_TO_SEC;

	if (curTime >= expectTime)
	{
		return true;
	}

	return false;
}

bool AccountActivityTaskMgr::_IsLimit(WSPlayer* player, WSActivityTask* task)
{
	if (!player || !task)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return true;
	}

	if (!WSActivityMgr::Instance()->IsInActivity(player->GetLevel(), task->GetActivityId()))
	{
		player->SendNotify(9201);
		ErrorLogStream << PLAYERINFO(player) << "Activity(" << task->GetActivityId() << ") is closed!" << LogStream::eos;
		return true;
	}

	ActiveTaskDataEntry* data = ActiveTaskDataEntryMgr::Instance()->FindEntry(task->GetTaskId());
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find ActiveTaskData(" << task->GetTaskId() << ")!" << LogStream::eos;
		return true;
	}

	if (CheckCond() && !data->applyChannel.empty() && data->applyChannel != player->GetPf())
	{
		player->SendNotify(9204, data->applyChannel.c_str());
		ErrorLogStream << PLAYERINFO(player) << "Error platform(" << player->GetPf() << ")." << LogStream::eos;
		return true;
	}

	return false;
}

bool AccountActivityTaskMgr::_SubmitClickRewardTask(WSPlayer* player, WSActivityTask* task)
{
	if (!player || !task)
	{
		return false;
	}

	if (task->GetStatus() != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(player) << "Error task(" << task->GetTaskId() << ") status(" << task->GetStatus() << ")!" << LogStream::eos;
		return false;
	}

	task->SetStatus(player, TASK_SUBMITTING);

	GiveRewards(player, task);
	
	return true;
}

bool AccountActivityTaskMgr::_SubmitAccountChargeRebateTask(WSPlayer* player, WSActivityTask* task)
{
	if (!player || !task)
	{
		return false;
	}

	if (task->GetStatus() != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(player) << "Error task(" << task->GetTaskId() << ") status(" << task->GetStatus() << ")!" << LogStream::eos;
		return false;
	}

	ActiveTaskDataEntry* data = ActiveTaskDataEntryMgr::Instance()->FindEntry(task->GetTaskId());
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find ActiveTaskData(" << task->GetTaskId() << ")!" << LogStream::eos;
		return false;
	}

	if (data->parm2.size() != 2)
	{
		ErrorLogStream << PLAYERINFO(player) << "ActiveTaskData parm2 size error:(" << task->GetTaskId() << ")!" << LogStream::eos;
		return false;
	}

	GiveRewards(player, task);

	Int64 drawCnt = task->GetVar("draw_cnt");
	Int64 chargeCnt = task->GetVar("charge_cnt");

	task->SetVar(player, "draw_cnt", drawCnt + 1);
	if (chargeCnt > drawCnt + 1)
	{
		task->SetStatus(player, TASK_FINISHED);
	}
	else
	{
		if (data->parm2[1] > chargeCnt)
			task->SetStatus(player, TASK_UNFINISH);
		else
			task->SetStatus(player, TASK_SUBMITTING);
	}

	return true;
}

bool AccountActivityTaskMgr::_CanRefreshTaskByWeek(UInt64 time, WSActivityTask* task)
{
	if (!task) return false;

	UInt64 acceptTime = (UInt64)task->GetAcceptTime() * SEC_TO_MSEC;
	UInt64 acceptTimeOnDayBegin = GetDayBeginTime2(Avalon::Time(acceptTime));
	UInt64 overTime = acceptTimeOnDayBegin + _GetRemainingIntervalOverTime(acceptTimeOnDayBegin, REFRESH_CYCLE_WEEK);

	InfoLogStream << "player(" << task->GetAccId() << ") is checking refresh task(" << task->GetTaskId() << "), accept time("
		<< acceptTime << "), overTime(" << overTime << "), param time(" << time << ")." << LogStream::eos;

	if (time >= overTime)
	{
		return true;
	}

	return false;
}

UInt64 AccountActivityTaskMgr::_GetRemainingIntervalOverTime(UInt64 time, RefreshCycleType refreshCycleType)
{
	Avalon::Date data(time);

	switch (refreshCycleType)
	{
	case REFRESH_CYCLE_DAILY:
		return DAY_TO_SEC * SEC_TO_MSEC;

	case REFRESH_CYCLE_WEEK:
		return (UInt64)DAY_TO_SEC * SEC_TO_MSEC * (7 - data.WDay() + 1); // 每周一

	case REFRESH_CYCLE_MONTH:
		return (UInt64)DAY_TO_SEC * SEC_TO_MSEC * (data.GetDaysInMonth() - data.Day() + 1); // 每月一号

	default:
		ErrorLogStream << "refreshCycleType(" << (UInt8)refreshCycleType << ") is error." << LogStream::eos;
		return 0;
	}
}

void AccountActivityTaskMgr::_AcceptLoginPrivilegeTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData)
	{
		ErrorLogStream << "WSPlayer ptr or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	auto loginTask = FindActivityTask(player->GetAccId(), taskData->id);
	if (!loginTask)
	{
		loginTask = CreateActivityTask(player, taskData);
		if (!loginTask) return;

		loginTask->SetStatus(player, TASK_FINISHED);

		if (AddActivityTask(loginTask))
		{
			loginTask->SaveToDB();
		}
		else
		{
			if (loginTask)
			{
				CL_SAFE_DELETE(loginTask);
			}
		}
	}
	else
	{
		loginTask->SyncStatus(player);
	}
}

void AccountActivityTaskMgr::_OnLogicDayChangeForLoginPrivilegeTask(WSPlayer* player, WSActivityTask* loginTask)
{
	if (!player || !loginTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	loginTask->SetStatus(player, TASK_FINISHED);
	loginTask->SaveToDB();
}

//bool AccountActivityTaskMgr::_SubmitLoginPrivilegeTask(WSPlayer* player, WSActivityTask* loginTask)
//{
	//if (!player || !loginTask)
	//{
	//	ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
	//	return false;
	//}

	//if (loginTask->GetStatus() != TASK_FINISHED)
	//{
	//	ErrorLogStream << PLAYERINFO(player) << "Error task(" << loginTask->GetTaskId() << ") status(" << loginTask->GetStatus() << ")!" << LogStream::eos;
	//	return false;
	//}

	//loginTask->SetStatus(player, TASK_SUBMITTING);

	//GiveRewards(player, loginTask);

	//return true;
//}

void AccountActivityTaskMgr::_AcceptSignInSevenTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData)
	{
		ErrorLogStream << "WSPlayer ptr or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	auto signInSevenTask = FindActivityTask(player->GetAccId(), taskData->id);
	if (!signInSevenTask)
	{
		signInSevenTask = CreateActivityTask(player, taskData);
		if (!signInSevenTask) return;

		//第一天签到默认开启
		if ((signInSevenTask->GetTaskId() % signInSevenTask->GetActivityId()) == 1)
		{
			signInSevenTask->SetStatus(player, TASK_FINISHED);
		}

		if (AddActivityTask(signInSevenTask))
		{
			signInSevenTask->SaveToDB();
		}
		else
		{
			if (signInSevenTask)
			{
				CL_SAFE_DELETE(signInSevenTask);
			}
		}
	}
	else
	{
		signInSevenTask->SyncStatus(player);
	}

	_UpdateSignInSevenTask(player, signInSevenTask);
}

void AccountActivityTaskMgr::_UpdateSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask)
{
	if (!player || !signInSevenTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (signInSevenTask->GetStatus() == (UInt8)TASK_UNFINISH || signInSevenTask->GetStatus() == (UInt8)TASK_FINISHED)
	{
		// 在预期签到时间内，设置任务为已完成，可进行领奖
		UInt32 startTime = (UInt32)signInSevenTask->GetVar("expect_sign_st");
		UInt32 endTime = (UInt32)signInSevenTask->GetVar("expect_sign_et");

		// 第一天没有签到，剩下的任务都是初始状态，不可以设置其他的状态
		if (startTime == 0 && endTime == 0)
		{
			return;
		}
		
		if (curTime >= startTime && curTime < endTime)
		{
			signInSevenTask->SetStatus(player, TASK_FINISHED);
			signInSevenTask->SaveToDB();
		}
		else if (curTime >= endTime)
		{
			if (signInSevenTask->GetStatus() != TASK_OVER)
			{
				signInSevenTask->SetStatus(player, TASK_FAILED);
				signInSevenTask->SaveToDB();
			}
		}
	}

	// 检查最后一个任务是否过期，过期刷新所有任务
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_SEVEN, datas);

	// 最后一个任务id
	UInt32 endTaskId = (UInt32)AID_SIGN_IN_SEVEN + datas.size();

	if (endTaskId != signInSevenTask->GetTaskId())
	{
		return;
	}

	auto task = FindActivityTask(player->GetAccId(), endTaskId);
	if (!task)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << endTaskId << ")!" << LogStream::eos;
		return;
	}

	UInt32 endTime = (UInt32)signInSevenTask->GetVar("expect_sign_et");
	if (endTime == 0 || curTime < endTime)
	{
		return;
	}

	// 最后一个任务过期，刷新所有任务
	for (auto data : datas)
	{
		if (!data)
		{
			ErrorLogStream << PLAYERINFO(player) << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		auto task = FindActivityTask(player->GetAccId(), data->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << data->id << ")!" << LogStream::eos;
			continue;
		}

		//第一天签到默认开启
		if ((data->id % data->templateID) == 1)
		{
			task->SetStatus(player, TASK_FINISHED);
		}
		else
		{
			task->SetStatus(player, TASK_UNFINISH);
		}

		task->SetVarNoSync("expect_sign_st", 0);
		task->SetVarNoSync("expect_sign_et", 0);

		task->SaveToDB();
	}

	// 将七日签到累计也刷新
	std::vector<ActiveTaskDataEntry*> datas2;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_SEVEN_FINISH, datas2);

	for (auto data : datas2)
	{
		if (!data)
		{
			ErrorLogStream << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		auto task = FindActivityTask(player->GetAccId(), data->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << data->id << ")!" << LogStream::eos;
			continue;
		}

		task->SetStatus(player, TASK_UNFINISH);
		task->SaveToDB();
	}
}

//void AccountActivityTaskMgr::_UpdateFailedSignInSevenTask(WSPlayer* player)
//{
//	if (!player)
//	{
//		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
//		return;
//	}
//
//	std::vector<ActiveTaskDataEntry*> datas;
//	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_SEVEN, datas);
//
//	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
//
//	std::vector<WSActivityTask*> failedTasks;
//	std::vector<WSActivityTask*> overTasks;
//
//	// 七日签到最末完成的任务id
//	UInt32 lastOverTaskId = 0;
//
//	for (auto data : datas)
//	{
//		if (!data)
//		{
//			ErrorLogStream << PLAYERINFO(player) << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
//			continue;
//		}
//
//		WSActivityTask* task = FindActivityTask(player->GetAccId(), data->id);
//		if (!task)
//		{
//			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << data->id << ")!" << LogStream::eos;
//			continue;
//		}
//
//		// 第一天任务,如果没有完成，则不用检查失败的任务（失败任务只有第一天任务完成后才会出现）
//		if ((task->GetTaskId() % data->templateID) == 1)
//		{
//			if (task->GetStatus() != TASK_OVER)
//			{
//				return;
//			}
//		}
//
//		// 在预期签到时间内，设置任务为已完成，可进行领奖
//		if (task->GetStatus() == (UInt8)TASK_UNFINISH)
//		{
//			UInt32 startTime = task->GetVar("expect_sign_st");
//			UInt32 endTime = task->GetVar("expect_sign_et");
//			if (curTime >= startTime && curTime < endTime)
//			{
//				task->SetStatus(player, TASK_FINISHED);
//				task->SaveToDB();
//			}
//		}
//
//		// 完成的任务
//		if (task->GetStatus() == TASK_OVER || task->GetStatus() == TASK_FINISHED)
//		{
//			if (task->GetTaskId() > lastOverTaskId)
//			{
//				lastOverTaskId = task->GetTaskId();
//			}
//		}
//		else
//		{
//			failedTasks.push_back(task);
//		}
//
//	}
//
//	std::vector<WSActivityTask*> realFailedTaskIds;
//	for (auto task : failedTasks)
//	{
//		if (task->GetTaskId() < lastOverTaskId)
//			realFailedTaskIds.push_back(task);
//	}
//
//	for (auto task : realFailedTaskIds)
//	{
//		if (!task) continue;
//
//		task->SetStatus(player, TASK_FAILED);
//		task->SaveToDB();
//	}
//}

void AccountActivityTaskMgr::_UpdateSignInSevenTaskAfterSubmit(WSPlayer* player, WSActivityTask* signInSevenTask)
{
	if (!player || !signInSevenTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	ActiveTaskDataEntry* taskData = ActiveTaskDataEntryMgr::Instance()->FindEntry(signInSevenTask->GetTaskId());
	if (!taskData)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find ActiveTaskData(" << signInSevenTask->GetTaskId() << ")!" << LogStream::eos;
		return;
	}

	/* 签到第一天任务时的处理 */
	bool isFirstSign = false;

	if ((taskData->templateID == AID_SIGN_IN_SEVEN) &&
		(signInSevenTask->GetTaskId() % taskData->templateID == 1))
	{
		isFirstSign = true;
	}

	if (!isFirstSign)
	{
		return;
	}

	// 签到时的今日开始时间
	UInt32 todayBeginTime = (UInt32)(GetDayBeginTime2(CURRENT_TIME.MSec()) / SEC_TO_MSEC);

	// 七日签到
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_SEVEN, datas);

	for (auto data : datas)
	{
		if (!data)
		{
			ErrorLogStream << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		auto task = FindActivityTask(player->GetAccId(), data->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << data->id << ")!" << LogStream::eos;
			continue;
		}

		// 第一天签到后就把剩余六天的预期签到时间计算出来，在这个时间签到则奖励
		if (isFirstSign && task->GetTaskId() != signInSevenTask->GetTaskId())
		{
			UInt32 days = task->GetTaskId() % data->templateID - 1;
			UInt32 expectSignStartTime = todayBeginTime + days * DAY_TO_SEC;
			UInt32 expectSignEndTime = expectSignStartTime + DAY_TO_SEC;

			task->SetVarNoSync("expect_sign_st", (Int64)expectSignStartTime);
			task->SetVarNoSync("expect_sign_et", (Int64)expectSignEndTime);
			task->SaveToDB();
		}
	}
}

void AccountActivityTaskMgr::_OnLogicDayChangeForSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask)
{
	if (!player || !signInSevenTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	_UpdateSignInSevenTask(player, signInSevenTask);
}

//bool AccountActivityTaskMgr::_SubmitSignInSevenTask(WSPlayer* player, WSActivityTask* signInSevenTask)
//{
	//if (!player || !signInSevenTask)
	//{
	//	ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
	//	return false;
	//}

	//if (signInSevenTask->GetStatus() != TASK_FINISHED)
	//{
	//	ErrorLogStream << PLAYERINFO(player) << "Error activeTask(id: " << signInSevenTask->GetTaskId() << ") status(" << signInSevenTask->GetStatus() << ")!" << LogStream::eos;
	//	return false;
	//}

	//signInSevenTask->SetStatus(player, TASK_SUBMITTING);

	//_UpdateSignInSevenTaskAfterSubmit(player, signInSevenTask);

	//GiveRewards(player, signInSevenTask);

	//return true;
//}

void AccountActivityTaskMgr::_AcceptSignInSevenFinishTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData)
	{
		ErrorLogStream << "WSPlayer ptr or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	WSActivityTask* signFinishTask = FindActivityTask(player->GetAccId(), taskData->id);
	if (!signFinishTask)
	{
		signFinishTask = CreateActivityTask(player, taskData);
		if (!signFinishTask) return;

		if (AddActivityTask(signFinishTask))
		{
			signFinishTask->SaveToDB();
		}
		else
		{
			if (signFinishTask)
			{
				CL_SAFE_DELETE(signFinishTask);
			}
		}
	}
	else
	{
		signFinishTask->SyncStatus(player);
	}
}

void AccountActivityTaskMgr::_UpdateSignInSevenFinishTask(WSPlayer* player)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	// 已签到的任务数
	UInt32 overTaskNum = 0;
	// 漏签的任务数
	UInt32 failedTaskNum = 0;

	// 七日签到
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_SEVEN, datas);

	for (auto data : datas)
	{
		if (!data)
		{
			ErrorLogStream << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		auto task = FindActivityTask(player->GetAccId(), data->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << data->id << ")!" << LogStream::eos;
			continue;
		}

		if (task->GetStatus() == TASK_OVER)
		{
			overTaskNum++;
		}
		if (task->GetStatus() == TASK_FAILED)
		{
			failedTaskNum++;
		}
	}

	// 签到累计奖励
	std::vector<ActiveTaskDataEntry*> datas2;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_SEVEN_FINISH, datas2);

	for (auto data : datas2)
	{
		if (!data)
		{
			ErrorLogStream << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		auto task = FindActivityTask(player->GetAccId(), data->id);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find activity task(" << data->id << ")!" << LogStream::eos;
			continue;
		}

		//if (task->GetVar(task->GetDataEntry()->overTaskNumKey.c_str()) != overTaskNum)
		//{
		//	task->SetVar(task->GetDataEntry()->overTaskNumKey.c_str(), overTaskNum);
		//	task->SaveToDB();
		//}
		//if (task->GetVar(task->GetDataEntry()->failedTaskNumKey.c_str()) != failedTaskNum)
		//{
		//	task->SetVar(task->GetDataEntry()->failedTaskNumKey.c_str(), failedTaskNum);
		//	task->SaveToDB();
		//}

		// 七日签到全部完成，则开放累计奖励
		if (overTaskNum == datas.size() && task->GetStatus() != TASK_FINISHED)
		{
			task->SetStatus(player, TASK_FINISHED);
			task->SaveToDB();
		}
	}

	InfoLogStream << PLAYERINFO(player) << " over task num(" << overTaskNum << "), failed task num(" << failedTaskNum << ")." << LogStream::eos;
}

//bool AccountActivityTaskMgr::_SubmitSignInSevenFinishTask(WSPlayer* player, WSActivityTask* signFinishTask)
//{
	//if (!player || !signFinishTask)
	//{
	//	ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
	//	return false;
	//}

	//if (signFinishTask->GetStatus() != TASK_FINISHED)
	//{
	//	ErrorLogStream << PLAYERINFO(player) << "Error task(" << signFinishTask->GetTaskId() << ") status(" << signFinishTask->GetStatus() << ")!" << LogStream::eos;
	//	return false;
	//}

	//signFinishTask->SetStatus(player, TASK_SUBMITTING);

	//GiveRewards(player, signFinishTask);

	//return true;
//}

void AccountActivityTaskMgr::_AcceptDrawPrizeTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData)
	{
		ErrorLogStream << "WSPlayer ptr or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	DrawPrizeDataEntry* drawPrizeData = DrawPrizeDataEntryMgr::Instance()->GetDrawPrizeDataByType(taskData->templateID);
	if (!drawPrizeData)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find draw prize data by type(" << taskData->templateID << ")!" << LogStream::eos;
		return;
	}

	WSActivityTask* drawPrizeTask = FindActivityTask(player->GetAccId(), taskData->id);
	if (!drawPrizeTask)
	{
		drawPrizeTask = CreateActivityTask(player, taskData);
		if (!drawPrizeTask) return;

		drawPrizeTask->SetVarNoSync(drawPrizeData->drawPrizeCountKey.c_str(), 0);
		drawPrizeTask->SetVarNoSync(drawPrizeData->allCountKey.c_str(), drawPrizeData->baseCount);

		drawPrizeTask->SetVar(player, drawPrizeData->continuousDrawKey.c_str(), 0);

		// 有次数就开启任务
		if (drawPrizeData->baseCount > 0)
		{
			drawPrizeTask->SetStatusNoSync(TASK_FINISHED);
		}
		else
		{
			InfoLogStream << PLAYERINFO(player) << "Draw prize base count(" << drawPrizeData->baseCount << ")." << LogStream::eos;
		}

		if (AddActivityTask(drawPrizeTask))
		{
			drawPrizeTask->SaveToDB(false);
		}
		else
		{
			if (drawPrizeTask)
			{
				CL_SAFE_DELETE(drawPrizeTask);
			}
		}
	}
	else
	{
		drawPrizeTask->SyncVar(player, drawPrizeData->continuousDrawKey.c_str(), drawPrizeTask->GetVar(drawPrizeData->continuousDrawKey.c_str()));
	}

	Int64 restCount = drawPrizeTask->GetVar(drawPrizeData->allCountKey) - drawPrizeTask->GetVar(drawPrizeData->drawPrizeCountKey);
	drawPrizeTask->SyncVar(player, drawPrizeData->restDrawCountKey, restCount);
}

void AccountActivityTaskMgr::_UpdateDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask)
{
	if (!player || !drawPrizeTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	auto drawPrizeData = DrawPrizeDataEntryMgr::Instance()->GetDrawPrizeDataByType(drawPrizeTask->GetActivityId());
	if (!drawPrizeData)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find DrawPrizeDataEntry(" << drawPrizeTask->GetTaskId() << ")!" << LogStream::eos;
		return;
	}

	if (drawPrizeData->continuousDrawDays == 0)
	{
		InfoLogStream << PLAYERINFO(player) << "Draw prize data continuous days(" << drawPrizeData->continuousDrawDays << ")." << LogStream::eos;
		return;
	}

	if (drawPrizeData->refreshTime != 6)
	{
		ErrorLogStream << PLAYERINFO(player) << "Draw prize refresh time(" << drawPrizeData->refreshTime << ") is error!" << LogStream::eos;
		return;
	}

	if (drawPrizeData->refreshInterval == 0)
	{
		InfoLogStream << PLAYERINFO(player) << "Draw prize data refreshInterval(" << drawPrizeData->refreshInterval << ")." << LogStream::eos;
		return;
	}

	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();
	Int64 continuousDraws = drawPrizeTask->GetVar(drawPrizeData->continuousDrawKey);
	UInt64 refreshInterval = (UInt64)drawPrizeData->refreshInterval * DAY_TO_SEC;

	// 预期抽奖时间，在预期抽奖时间内抽奖，则连续抽奖计数+1
	UInt32 expectDrawStartTime = (UInt32)(GetDayBeginTime2((UInt64)(drawPrizeTask->GetUpdateTime() + refreshInterval) * SEC_TO_MSEC) / SEC_TO_MSEC);
	UInt32 expectDrawEndTime = (UInt32)(GetDayBeginTime2((UInt64)(expectDrawStartTime + refreshInterval) * SEC_TO_MSEC) / SEC_TO_MSEC);
	if (curTime > expectDrawStartTime)
	{
		if (curTime < expectDrawEndTime)
		{
			continuousDraws += 1;
		}
		else
		{
			continuousDraws = 1;
		}
	}

	InfoLogStream << PLAYERINFO(player) << "On update draw prize task(" << drawPrizeTask->GetTaskId() << "), curtime(" << curTime << "), "
		<< ", expectDrawStartTime(" << expectDrawStartTime << ", expectDrawEndTime(" << expectDrawEndTime << ") continuousDraws(" 
		<< continuousDraws << ")." << LogStream::eos;

	if (continuousDraws != (UInt32)drawPrizeTask->GetVar(drawPrizeData->continuousDrawKey))
	{
		drawPrizeTask->SetVar(player, drawPrizeData->continuousDrawKey, continuousDraws);
		drawPrizeTask->SaveToDB(false);
	}

	// 连续抽奖天数达到后，抽奖总数增加
	if (continuousDraws >= drawPrizeData->continuousDrawDays)
	{
		Int64 oldAllCount = drawPrizeTask->GetVar(drawPrizeData->allCountKey);
		Int64 newAllCount = drawPrizeData->baseCount + drawPrizeData->rewardCount;
		if (oldAllCount == newAllCount)
		{
			return;
		}
		drawPrizeTask->SetVar(player, drawPrizeData->allCountKey, newAllCount);
		drawPrizeTask->SetStatusNoSync(TASK_FINISHED);
		drawPrizeTask->SaveToDB(false);

		InfoLogStream << PLAYERINFO(player) << "Reward draw prize count, now all count=" << newAllCount << LogStream::eos;
	}
}

void AccountActivityTaskMgr::_OnLogicDayChangeForDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask)
{
	if (!player || !drawPrizeTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(player) << "draw prize task(" << drawPrizeTask->GetTaskId() << ") on logic day change." << LogStream::eos;

	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();

	auto drawPrizeData = DrawPrizeDataEntryMgr::Instance()->GetDrawPrizeDataByType(drawPrizeTask->GetActivityId());
	if (!drawPrizeData)
	{
		ErrorLogStream << PLAYERINFO(player) << "DrawPrizeDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	// 为0 不刷新
	if (drawPrizeData->refreshInterval == 0)
	{
		InfoLogStream << PLAYERINFO(player) << "Draw prize data refreshInterval(" << drawPrizeData->refreshInterval << ")." << LogStream::eos;
		return;
	}

	// 已抽奖次数刷新
	drawPrizeTask->SetVarNoSync(drawPrizeData->drawPrizeCountKey, 0);

	// 总抽奖次数刷新
	drawPrizeTask->SetVarNoSync(drawPrizeData->allCountKey, drawPrizeData->baseCount);

	// 剩余抽奖次数刷新
	drawPrizeTask->SyncVar(player, drawPrizeData->restDrawCountKey, drawPrizeData->baseCount);

	if (drawPrizeData->baseCount > 0)
	{
		drawPrizeTask->SetStatusNoSync(TASK_FINISHED);
	}
	else
	{
		drawPrizeTask->SetStatusNoSync(TASK_UNFINISH);
	}

	// 预期抽奖时间，不在预期抽奖最后时间内抽奖，则抽奖计数刷新
	UInt64 refreshInterval = (UInt64)drawPrizeData->refreshInterval * DAY_TO_SEC;
	UInt32 expectDrawStartTime = (UInt32)(GetDayBeginTime2((UInt64)(drawPrizeTask->GetUpdateTime() + refreshInterval) * SEC_TO_MSEC) / SEC_TO_MSEC);
	UInt32 expectDrawEndTime = (UInt32)(GetDayBeginTime2((UInt64)(expectDrawStartTime + refreshInterval) * SEC_TO_MSEC) / SEC_TO_MSEC);

	if (curTime >= expectDrawEndTime)
	{
		drawPrizeTask->SetVar(player, drawPrizeData->continuousDrawKey, 0);
	}

	// 连续抽奖天数刷新
	if (drawPrizeTask->GetVar(drawPrizeData->continuousDrawKey) == drawPrizeData->continuousDrawDays)
	{
		drawPrizeTask->SetVar(player, drawPrizeData->continuousDrawKey, 0);
	}

	drawPrizeTask->SaveToDB(false);
}

bool AccountActivityTaskMgr::_SubmitDrawPrizeTask(WSPlayer* player, WSActivityTask* drawPrizeTask)
{
	if (!player || !drawPrizeTask)
	{
		ErrorLogStream << "WSPlayer ptr or WSActivityTask ptr is null!" << LogStream::eos;
		return false;
	}
	
	DrawPrizeDataEntry* drawPrizeData = DrawPrizeDataEntryMgr::Instance()->GetDrawPrizeDataByType(drawPrizeTask->GetActivityId());
	if (!drawPrizeData)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find draw prize data by type(" << drawPrizeTask->GetActivityId() << ")!" << LogStream::eos;
		return false;
	}

	if (CheckDrawCount() && drawPrizeTask->GetStatus() != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(player) << "Activity task(" << drawPrizeTask->GetTaskId() << ") status(" << drawPrizeTask->GetStatus() << ") is error!" << LogStream::eos;
		return false;
	}

	drawPrizeTask->SetStatusNoSync(TASK_SUBMITTING);

	// 已抽次数
	Int64 count = drawPrizeTask->GetVar(drawPrizeData->drawPrizeCountKey);
	// 总次数
	Int64 allCount = drawPrizeTask->GetVar(drawPrizeData->allCountKey);

	if (CheckDrawCount() && count >= allCount)
	{
		ErrorLogStream << PLAYERINFO(player) << "draw prize task(" << drawPrizeTask->GetTaskId() << ") count(" << count << ") allcount(" << allCount << ")." << LogStream::eos;
		return false;
	}

	class RewardPoolVisitor : public Avalon::DataEntryVisitor<RewardPoolDataEntry>
	{
	public:
		RewardPoolVisitor(UInt32 drawPrizeId) : m_DrawPrizeId(drawPrizeId) {}

	public:
		bool operator()(RewardPoolDataEntry* dataEntry)
		{
			if (!dataEntry)
			{
				ErrorLogStream << "RewardPoolDataEntry ptr is null!" << LogStream::eos;
				return true;
			}

			if (m_DrawPrizeId == dataEntry->drawPrizeTableId)
			{
				rewardPool.push_back(dataEntry);
			}

			return true;
		}

		std::vector<RewardPoolDataEntry*> rewardPool;

	private:
		// 抽奖表id
		UInt32	m_DrawPrizeId;
	};

	RewardPoolVisitor visitor(drawPrizeData->GetKey());
	RewardPoolDataEntryMgr::Instance()->Visit(visitor);

	std::vector<UInt32> wts;
	ItemRewardVec rewards;
	for (auto rewardData : visitor.rewardPool)
	{
		if (!rewardData) continue;

		wts.push_back(rewardData->itemWeight);
		rewards.push_back(ItemReward(rewardData->itemId, rewardData->itemNum, rewardData->strengthenLevel, 0));
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= (Int32)rewards.size() || hitIdx < 0)
	{
		ErrorLogStream << PLAYERINFO(player) << "Rand reward failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return false;
	}

	std::vector<ItemReward> giveRewards = { rewards[hitIdx] };

	// 第一次抽奖
	if (drawPrizeTask->GetVar(drawPrizeData->continuousDrawKey) == 0)
	{
		drawPrizeTask->SetVar(player, drawPrizeData->continuousDrawKey, 1);
	}
	else
	{
		// 连续抽奖
		_UpdateDrawPrizeTask(player, drawPrizeTask);
	}

	// 抽奖次数加1
	count += 1;
	drawPrizeTask->SetVarNoSync(drawPrizeData->drawPrizeCountKey, count);

	// 计算剩余抽奖次数(客户端用来显示)
	Int64 restCount = drawPrizeTask->GetVar(drawPrizeData->allCountKey) - count;
	drawPrizeTask->SyncVar(player, drawPrizeData->restDrawCountKey, restCount);

	// 奖品位置
	drawPrizeTask->SyncVar(player, drawPrizeData->rewardIdKey, hitIdx + 1);

	// 没有剩余抽奖次数则准备设置任务完成
	if (restCount == 0)
	{
		drawPrizeTask->SetStatusNoSync(TASK_OVER);
	}

	drawPrizeTask->SaveToDB();

	GiveRewards(player, drawPrizeTask, giveRewards, false);

	return false;
}

void AccountActivityTaskMgr::_AcceptAmberPrivilegeTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData) return;

	if (!IsOppoPlayer(player)) return;

	auto task = FindActivityTask(player->GetAccId(), taskData->id);
	if (!task)
	{
		task = CreateActivityTask(player, taskData);
		if (!task) return;

		if (AddActivityTask(task))
		{
			task->SaveToDB();
		}
		else
		{
			if (task)
			{
				CL_SAFE_DELETE(task);
			}
		}
	}
	else
	{
		task->SyncStatus(player);
	}
}

void AccountActivityTaskMgr::_OnLogicDayChangeForAmberPrivilegeTask(WSPlayer* player, WSActivityTask* task)
{
	if (!player || !task) return;

	if (!IsOppoPlayer(player)) return;

	UInt64 curTime = CURRENT_TIME.MSec();

	if (!_CanRefreshTaskByWeek(curTime, task)) return;

	UInt8 oppoAmberLevel = player->GetOppoAmberLevel();
	UInt32 matchTaskId = GetTaskIdByOppoAmberLevel(oppoAmberLevel);

	InfoLogStream << PLAYERINFO(player) << "start to updata amber privilege(" << oppoAmberLevel << "), match task(" << matchTaskId
		<< ") task(" << task->GetTaskId() << "), status(" << task->GetStatus() << ")." << LogStream::eos;

	if (task->GetTaskId() == matchTaskId)
	{
		//普通用户没有奖励(这里设置FAILED,是为了在客户端展示)
		if (oppoAmberLevel == (UInt8)OAL_NONE)
		{
			task->SetStatus(player, TASK_FAILED);
		}
		else
		{
			task->SetStatus(player, TASK_FINISHED);
		}
	}
	else
	{
		task->SetStatus(player, TASK_UNFINISH);
	}

	task->SetAcceptTime((UInt32)(curTime / SEC_TO_MSEC));
	task->SaveToDB();
}

bool AccountActivityTaskMgr::IsOppoPlayer(WSPlayer* player)
{
	if (!player) return false;

	if (CheckCond() && player->GetPf() != "oppo")
	{
		return false;
	}

	return true;
}

void AccountActivityTaskMgr::OnAccountTotalCharge(UInt32 accid, ObjID_t roleId, UInt32 num)
{
	ActivityInfo actInfo;
	if (!WSActivityMgr::Instance()->FindActivity(AID_ACCOUNT_TOTAL_CHARGE, actInfo))
	{
		ErrorLogStream << "Player(" << accid << "," << roleId << ") activity(" << actInfo.id << ") is closed!" << LogStream::eos;
		return;
	}

	if (actInfo.state != AS_IN)
	{
		ErrorLogStream << "Player(" << accid << "," << roleId << ") activity(" << actInfo.id << ") is closed!" << LogStream::eos;
		return;
	}

	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(accid);
	if (!player)
	{
		
		InfoLogStream << "player(" << accid << ", " << roleId << ") offline when charge(" << num << ") by account." << LogStream::eos;

		// 通过角色ID查找player
		player = WSPlayerMgr::Instance()->FindPlayer(roleId);
		if (!player)
		{
			InfoLogStream << "player(" << accid << ", " << roleId << ") offline when charge(" << num << ") by roleid." << LogStream::eos;
		}
	}

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(actInfo.id, datas);

	if (datas.empty())
	{
		ErrorLogStream << "Activity(" << actInfo.id << ")'s task datas are empty!" << LogStream::eos;
		return;
	}

	for (auto taskData : datas)
	{
		if (!taskData) continue;

		auto task = FindActivityTask(accid, taskData->id);
		if (!task)
		{
			ErrorLogStream << "Player(" << accid << "," << roleId << ") cant't find task(" << taskData->id << ")!" << LogStream::eos;
			continue;
		}

		if (!task || !taskData)
		{
			continue;
		}

		if (task->GetStatus() != TASK_UNFINISH)
		{
			continue;
		}

		UInt32 totalChargeLimit = (UInt32)taskData->param1;
		UInt32 oldChargeNum = (UInt32)task->GetVar("num");
		UInt32 newChargeNum = oldChargeNum + num;
		newChargeNum = newChargeNum > totalChargeLimit ? totalChargeLimit : newChargeNum;
		InfoLogStream << "Player(" << roleId << ") start update task(" << task->GetUID() << ", " << task->GetTaskId() << ") charge num (" << oldChargeNum << ") => (" << newChargeNum << ")..." << LogStream::eos;

		if (player)
		{
			task->SetVar(player, "num", newChargeNum);
		}
		else
		{
			task->SetVarNoSync("num", newChargeNum);
		}
		
		
		if (newChargeNum >= totalChargeLimit)
		{
			InfoLogStream << "Player(" << accid << ", " << roleId << ") charge(" << newChargeNum << "), task(" << task->GetUID() << ", " << task->GetTaskId() << ")'s status=" << task->GetStatus() << LogStream::eos;

			if (player)
			{
				task->SetStatus(player, TASK_FINISHED);
			}
			else
			{
				task->SetStatusNoSync(TASK_FINISHED);
			}
		}

		task->SaveToDB();
	}
}

void AccountActivityTaskMgr::_AcceptChargeRebateTask(WSPlayer* player, ActiveTaskDataEntry* taskData)
{
	if (!player || !taskData) return;

	auto task = FindActivityTask(player->GetAccId(), taskData->id);
	if (!task)
	{
		task = CreateActivityTask(player, taskData);
		if (!task) return;

		if (AddActivityTask(task))
		{
			task->SaveToDB();
		}
		else
		{
			CL_SAFE_DELETE(task);
		}
	}
	else
	{
		task->SyncStatus(player);
	}
}

void AccountActivityTaskMgr::_OnChargeForChargeRebateTask(UInt32 accid, ObjID_t roleId, UInt32 chargeNum)
{
	ActivityInfo actInfo;
	if (!WSActivityMgr::Instance()->FindActivity(AID_ACCOUNT_CHARGE_REBATE, actInfo))
	{
		ErrorLogStream << "Player(" << accid << "," << roleId << ") activity(" << actInfo.id << ") is closed!" << LogStream::eos;
		return;
	}

	if (actInfo.state != AS_IN)
	{
		ErrorLogStream << "Player(" << accid << "," << roleId << ") activity(" << actInfo.id << ") is closed!" << LogStream::eos;
		return;
	}

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(actInfo.id, datas);

	if (datas.empty())
	{
		ErrorLogStream << "Activity(" << actInfo.id << ")'s task datas are empty!" << LogStream::eos;
		return;
	}

	for (auto taskData : datas)
	{
		if (!taskData) continue;

		auto task = FindActivityTask(accid, taskData->id);
		if (!task)
		{
			ErrorLogStream << "Player(" << accid << "," << roleId << ") cant't find task(" << taskData->id << ")!" << LogStream::eos;
			continue;
		}

		_OnChargeForChargeRebateTask(roleId, task, taskData, chargeNum);
	}
}

void AccountActivityTaskMgr::_OnChargeForChargeRebateTask(ObjID_t roleId, WSActivityTask* task, ActiveTaskDataEntry* taskData, UInt32 chargeNum)
{
	if (!task || !taskData) return;

	InfoLogStream << "Player(" << roleId << ") start update task(" << task->GetUID() << ", " << task->GetTaskId() << ")..." << LogStream::eos;

	if (taskData->param1 != chargeNum)
	{
		return;
	}

	if (taskData->parm2.size() != 2)
	{
		ErrorLogStream << "Task(" << task->GetTaskId() << ")'s data entry parm2 size error!" << LogStream::eos;
		return;
	}

	Int64 cnt = task->GetVar("charge_cnt");
	if (cnt >= taskData->parm2[1])
	{
		InfoLogStream << "account charge rebate cnt enough player id:" << roleId << " cnt:" << chargeNum << LogStream::eos;
		return;
	}

	UInt32 levelLimit = taskData->parm2[0];
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
	if (NULL == player)
	{
		CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		cmd->PutCond("guid", roleId);
		cmd->PutColumn("level");
		CLRecordClient::Instance()->SendCommand(cmd, new AccountChargeRebateSelectPlayerInfoCallback(roleId, task, levelLimit));
	}
	else if(player->GetLevel() >= levelLimit)
	{
		InfoLogStream << "Player(" << roleId << ") charge(" << chargeNum << "), task(" << task->GetUID() << ", " << task->GetTaskId() << ")'s status=" << task->GetStatus() << LogStream::eos;

		task->SetVar(player, "charge_cnt", cnt + 1);
		task->SetStatus(player, TASK_FINISHED);
		task->SaveToDB();
	}
}
