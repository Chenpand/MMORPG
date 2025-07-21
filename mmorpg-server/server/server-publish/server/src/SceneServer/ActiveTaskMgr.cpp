#include "ActiveTaskMgr.h"
#include "ActiveTask.h"
#include "ActivityMgr.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "ActivityScript.h"
#include "SSDungeonConfigMgr.h"
#include "SortListMgr.h"
#include "SceneSysNotify.h"
#include "SSServiceSwitchMgr.h"
#include "ActiveRecordMgr.h"
#include "SSApplication.h"
#include "PlayerMgr.h"
#include "GameParamMgr.h"

#include <CLChargeMallDataEntry.h>
#include <CLDrawPrizeDataEntry.h>
#include <CLBuffRandomDataEntry.h>
#include <CLWeekSignDataEntry.h>
#include <CLWeekSignSumDataEntry.h>
#include <CLActiveEventDataEntry.h>
#include <CLPkLevelDataEntry.h>
#include <CLWarpStoneDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLDungeonTimesDataEntry.h>
#include <CLOpActiveProtocol.h>
#include <CLRelationProtocol.h>
#include <CLGlobalActivityProtocol.h>
#include <CLTeamCopyDefine.h>


ReadNotifyProperty::ReadNotifyProperty()
{

}

ReadNotifyProperty::~ReadNotifyProperty()
{

}

void ReadNotifyProperty::AddProperty(UInt32 type, UInt32 param)
{
	NotifyInfo notify;
	notify.type = type;
	notify.param = param;

	m_Properties.push_back(notify);
	SetDirty();
}

bool ReadNotifyProperty::IsReaded(UInt32 type, UInt32 param)
{
	for (size_t i = 0; i < m_Properties.size(); ++i)
	{
		if (m_Properties[i].type == type &&
			m_Properties[i].param == param)
		{
			return true;
		}
	}

	return false;
}

void ReadNotifyProperty::DecodeString(std::istringstream& is)
{
	UInt32 type = 0;
	UInt32 param = 0;
	char split;

	while ((is >> type) && type != 0)
	{
		is >> split >> param >> split;
		 
		if (type < NT_MAX)
		{
			NotifyInfo notify;
			notify.type = type;
			notify.param = param;

			m_Properties.push_back(notify);
		}
	}
}

void ReadNotifyProperty::EncodeString(std::ostringstream& os)
{
	for (UInt32 i = 0; i < m_Properties.size(); ++i)
	{
		if (m_Properties[i].type != 0)
		{
			os << UInt32(m_Properties[i].type) << ';' << UInt32(m_Properties[i].param) << ';';
		}
	}
}

ActiveTaskMgr::ActiveTaskMgr()
{
	m_pOwner = NULL;
	m_ActiveTasks.clear();
	m_rpCost = 0;
	m_waitGiftData = NULL;
}

ActiveTaskMgr::~ActiveTaskMgr()
{
	UnRegistGameEvent();

	for (ActivityMap::iterator itr = m_ActiveTasks.begin(); itr != m_ActiveTasks.end(); ++itr)
	{
		CL_SAFE_DELETE(itr->second);
	}
	m_ActiveTasks.clear();

	std::set<OpActTask*> allTasks;
	for (auto itr = m_idToOpActTask.begin(); itr != m_idToOpActTask.end(); ++itr)
	{
		allTasks.insert(itr->second);
	}
	m_idToOpActTask.clear();

	for (auto task : m_OldOpActTaskMap)
	{
		allTasks.insert(task);
	}
	m_OldOpActTaskMap.clear();

	for (auto itr = m_idToOpActAccountTask.begin(); itr != m_idToOpActAccountTask.end(); ++itr)
	{
		allTasks.insert(itr->second);
	}
	m_idToOpActAccountTask.clear();

	for (auto task : allTasks)
	{
		if (task)
		{
			CL_SAFE_DELETE(task);
		}
	}
}

void ActiveTaskMgr::OnHeartbeat(const Avalon::Time& now, int tickType)
{
	if (LOGICTICK_FIVESEC == tickType)
	{
		ActivityMap::iterator itr = m_ActiveTasks.begin();
		for (; itr != m_ActiveTasks.end(); ++itr)
		{
			ActiveTask* task = itr->second;
			if (task && task->GetDataEntry() && task->GetDataEntry()->templateID == AID_DAY_ONLINE)
			{
				ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
				if (script != NULL && script->CheckTickEvent())
				{
					UInt8 oldStatus = task->GetStatus();
					script->OnMinTick(task->GetOwner(), task);
					UInt8 newStatus = task->GetStatus();

					if (oldStatus != newStatus)
					{
						task->SaveToDB();
					}
				
				}
			}
		}

		_UpdateGiftRightOpActTask(now);
		_UpdateAccountCumulateOnlineOpActTask();
	}

	if (LOGICTICK_SECOND == tickType)
	{
		_OnTickFatigueBurningOpAct(now);
		_CrossWeekSignTaskReward(OAT_WEEK_SIGN_ACTIVITY);
		_CrossWeekSignTaskReward(OAT_WEEK_SIGN_NEW_PLAYER);
		_UpdateCumulateOnlineOpActTask();
		_UpdateSpringFestivalRainOpActTask();
		_UpdateOnlineGiftOpActTask();
	}
}

void ActiveTaskMgr::OnOnline(bool bDayChanged)
{
	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_CHALLENGE_HUB))
	{
		GetOwner()->SetCounter(OPACT_CHALLENGE_SCORE, 0);
		GetOwner()->SetCounter(OPACT_CHALLENGE_TOTAL_SCORE, 0);
		GetOwner()->SetCounter(OPACT_CHALLENGE_DAY_SCORE, 0);
	}

	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_CHALLENGE_HUB_SCORE))
	{

	}

	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_SPRING_CHALLENGE))
	{
		GetOwner()->SetCounter(OPACT_SPRING_SCORE, 0);
	}

	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_SPRING_CHALLENGE_SCORE))
	{
		GetOwner()->SetCounter(OPACT_SPRING_TOTAL_SCORE, 0);
	}

	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_WEEK_SIGN_SPRING))
	{
		GetOwner()->SetCounter(OPACT_WEEK_SIGN_SPRING_NUM, 0);
	}

	RegistGameEvent();

	MixOpActTasks();

	UInt32 offlineDay = GetOwner()->GetOfflineDay(6);
	if (offlineDay > 0)
	{
		UpdateDungeonLastNum();
	}

	//添加当前开放活动的任务
	std::vector<ActivityInfo> opens;
	ActivityMgr::Instance()->GetOpenActivity(GetOwner(), opens);
	//添加玩家自身活動
	GetOwner()->GetPlayerActivityInfo().GetOpenActivities(opens);

	//同步客户端活动
	CLProtocol::SceneSyncClientActivities sync;
	sync.activities = opens;
	GetOwner()->SendProtocol(sync);
	
	for (size_t i = 0; i < opens.size(); ++i)
	{
		//DebugLogStream << "id = " << opens[i].id << " name = " << opens[i].name
		//	<< " pretime = " << opens[i].preTime
		//	<< " starttime = " << opens[i].startTime
		//	<< " duetime = " << opens[i].dueTime
		//	<< LogStream::eos;

		if (IsSignIn(opens[i].id))
		{
			SighInOnline();
		}
		else if (IsSignInTotal(opens[i].id))
		{
			SignInTotalOnline();
		}
		else if (AID_CHARGE_DAY == opens[i].id)
		{
			DayChargeOnline(GetOwner()->GetOfflineDay());
		}
		else if (AID_CHARGE_DAY_NEW == opens[i].id)
		{
			DayChargeNewOnline(GetOwner()->GetOfflineDay() > 0 ? true : false);
		}
		else
		{
			AcceptTask(opens[i].id, true);
		}
	}

	UpdateActiveTasks(offlineDay);
	

	//疲劳找回
	MakeUpFatgue(true);
	//关卡找回
	MakeUpDungen(true);
	//初始化活动罐子
	InitJarAct();

	// 运营活动相关上线处理
	{
		// 跨天刷新
		if (bDayChanged)
		{
			OnLogicDayChangeForOpAct();
			RefreshDailyChallengeHell();
			// 更新礼遇特权任务
			UpdateGiftRightOnlineTask();
		}

		// 接受活动
		_AcceptOpActivities();
	}

	//上线通知各类活动信息
	SendOnlineNotifyInfo();
}

void ActiveTaskMgr::OnBirth()
{
	if (ActivityMgr::Instance()->IsInOpActivity(OAID_LEVEL_FIGHTING_FOR_NEW_SERVER))
	{
		SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_NEW_SERVER_LEVEL, GetOwner()->GetID(), GetOwner()->GetName(), 0, GetOwner()->GetOccu(), 0, GetOwner()->GetLevel(), (UInt32)GetOwner()->GetLevel(), (UInt32)GetOwner()->GetExp(), 0);
	}
}

void ActiveTaskMgr::OnLevelup()
{
	//添加当前开放活动的任务
	std::vector<ActivityInfo> opens;
	ActivityMgr::Instance()->GetOpenActivity(GetOwner(), opens);

	//添加玩家自身活動
	GetOwner()->GetPlayerActivityInfo().GetOpenActivities(opens);

	// 接受活动任务
	for (size_t i = 0; i < opens.size(); ++i)
	{
		AcceptTask(opens[i].id);
	}

	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckLevelUpEvent())
		{
			script->OnLvUpChange(task->GetOwner(), task);
		}
	}

	// 接受运营活动任务
	_AcceptOpActivities();

	OnHireTask(HTT_LEVEL, GetOwner()->GetLevel());
}

bool ActiveTaskMgr::AcceptTask(UInt32 activeId, bool online)
{
	//七日各天可接时间判断
	if (IsSevenDay(activeId) && !CanTakeSevenDayTask(activeId))
	{
		return false;
	}

	if (IsMonthCard(activeId))
	{
		return false;
	}

	if (IsSignInTotal(activeId))
	{
		return false;
	}

	if (AID_CHARGE_DAY == activeId)
	{
		return false;
	}

	if (AID_CHARGE_DAY_NEW == activeId)
	{
		return false;
	}

	if (AID_CHARGE_TOT == activeId)
	{
		//接受vip11的任务
		auto data = ActiveTaskDataEntryMgr::Instance()->FindEntry(8411);
		if (data != NULL)
		{
			ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(data->id);
			if (script != NULL)
			{
				ActiveTask* task = FindTask(data->id);
				if (!task)
				{
					//没有任务就接取
					task = new ActiveTask(GetOwner(), data->id);
					task->SetData(data);
					task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
					script->OnAccepted(GetOwner(), task);
					script->OnCharge(GetOwner(), task, GetOwner()->GetPlayerChargeTotal());
					task->SaveToDB();

					m_ActiveTasks.insert(std::make_pair(data->id, task));

					InfoLogStream << PLAYERINFO(GetOwner()) << " accept activeTask(" << data->id << ")" << LogStream::eos;
				}
			}
		}
	}

	if (IsItemChange(activeId))
	{
		std::vector<ActiveTaskDataEntry*> datas;
		ActiveTaskDataEntryMgr::Instance()->GetActives(activeId, datas);

		if (datas.empty())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "Activity(" << activeId << ") task is empty!" << LogStream::eos;
		}

		AcceptItemChangeTask(datas);
		return true;
	}

	_CreateActiveTask(activeId);

	return true;
}

void ActiveTaskMgr::OnLogicWeekChangeForActivities()
{
	OnLogicWeekChangeForItemChangeTask();
}

void ActiveTaskMgr::SyncAllTask()
{
	CLProtocol::SceneSyncActiveTaskList taskList;
	
	for (ActivityMap::iterator itr = m_ActiveTasks.begin(); itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;
		if (!task)
		{
			continue;
		}

		TaskBriefInfo info;
		info.id = task->GetID();
		info.status = task->GetStatus();
		info.vars = task->GetQuestVar();

		taskList.tasks.push_back(info);
	}
	
	GetOwner()->SendProtocol(taskList);
}

bool ActiveTaskMgr::ClearTask(UInt32 activeId)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	while (itr != m_ActiveTasks.end())
	{
		ActiveTask* task = itr->second;
		if (!task || !task->GetDataEntry())
		{
			++itr;
			continue;
		}

		if (activeId == task->GetDataEntry()->templateID)
		{
			CL_SAFE_DELETE(task);
			itr = m_ActiveTasks.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	SyncAllTask();

	return true;
}

bool ActiveTaskMgr::SubmitTask(UInt32 taskId, bool bRp)
{
	// 消耗点券的活动ID
	static std::set<UInt32> point_active_set{ 7150, 7250, 7350, 7450, 7550, 7650, 7750 };

	// 安全锁开启，不允许消耗点券购买，首日尊享礼包
	if (point_active_set.find(taskId) != point_active_set.end() && !GetOwner()->SecurityLockCheck())
		return false;

	ActiveTask* task = FindTask(taskId);
	if (NULL == task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << taskId << ")!" << LogStream::eos;
		return false;
	}
		
	bool isScriptGiveRewards = false;
	if (IsCharge(task->GetDataEntry()->templateID))
	{
		isScriptGiveRewards = true;
	}

	if (!isScriptGiveRewards) //检查背包中是否有足够的空间存放任务奖励
	{
		if (!task->CheckGiveRewards())
		{
			GetOwner()->SendNotify(1000004);
			return false;
		}
	}

	bool subSuc = false;
	if (task->GetDataEntry()->templateID == AID_SIGN_IN)
	{
		subSuc = SubmitSignInTask(task, bRp);
		
		WeekSignTrigger(WEEK_SIGN_ACT_DAILY);
	}
	else if (task->GetDataEntry()->templateID == AID_MONTH_CARD)
	{
		if (task->GetStatus() != TASK_FINISHED) return false;
		task->SetStatus(TASK_SUBMITTING);
		subSuc = true;
	}
	else if (task->GetDataEntry()->templateID == AID_SIGN_IN_BONUS)
	{
		if (task->GetStatus() != TASK_FINISHED) return false;
		task->SetStatus(TASK_UNFINISH);
		task->SetVar("tt_si", 0);

		subSuc = true;
	}
	else if (task->GetDataEntry()->templateID == AID_CHARGE_DAY)
	{
		if (task->GetStatus() != TASK_FINISHED) return false;
		task->SetStatus(TASK_SUBMITTING);
		subSuc = true;
	}
	else if (IsItemChange(task->GetDataEntry()->templateID))
	{
		subSuc = SubmitItemChangeTask(task);
	}
	else
	{
		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script == NULL)
		{
			GameErrorLogStream("Player") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") ActiveTask Script Err id:"
				<< taskId << ".";
			return false;
		}

		subSuc = script->OnSubmitted(GetOwner(), task, bRp);
	}
	
	if (subSuc)
	{
		//充值活动走脚本给奖励
		if (!isScriptGiveRewards)
		{
			task->GiveRewards();
		}
		
		if (TASK_SUBMITTING == task->GetStatus())
		{
			task->SetStatus(TASK_OVER);
		}

		GameInfoLogStream("ActiveTask") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") submit task:" << task->GetID() << LogStream::eos;

		//by huchenhui
		GetOwner()->OnSubmitActiveTaskSucceed(task->GetDataEntry()->templateID, task->GetID());
		
		task->SaveToDB();

		if (IsItemChange(task->GetDataEntry()->templateID))
		{
			UpdateItemChangeTask(task);
		}

		return true;
	}
	
	return false;
}

bool ActiveTaskMgr::SubmitMakeUpTask(UInt32 taskId, UInt32 param1)
{
	ActiveTask* task = FindTask(taskId);

	if (NULL == task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " task is null! id = " << taskId << LogStream::eos;
		return false;
	}

	if (task->GetStatus() != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "task status error! status:" << task->GetStatus() << LogStream::eos;
		return false;
	}

	UInt8 perfect = 0xFF & (param1 >> 16);
	UInt16 num = 0xFFFF & param1;
	 
	if (task->GetDataEntry()->templateID == AID_MAKEUP_FATIGUE)
	{
		if (GetOwner()->IsFullLevel())
		{
			GetOwner()->SendNotify(3132);
			return false;
		}

		return SubmitFatigueTask(task, perfect, num);
	}
	else if (task->GetDataEntry()->templateID == AID_MAKEUP_DUNGEN)
	{
		return SubmitDungeonTask(task, perfect, num); 
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " task templateID is err! id = " 
			<< task->GetDataEntry()->templateID << LogStream::eos;
		return false;
	}
}

ActiveTask* ActiveTaskMgr::FindTask(UInt32 id) const
{
	auto iter = m_ActiveTasks.find(id);
	if (iter != m_ActiveTasks.end())
	{
		return iter->second;
	}

	return NULL;
}

void ActiveTaskMgr::DecodeString(std::istringstream& is)
{
	char split;
	UInt32 taskId = 0;
	while ((is >> taskId) && taskId != 0)
	{
		is >> split;

		ActiveTaskDataEntry* data = ActiveTaskDataEntryMgr::Instance()->FindEntry(taskId);
		if (!data)
		{
			GameErrorLogStream("Player") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID()
				<< "," << GetOwner()->GetName() << ") has invalid ActiveTask Data id:" << taskId << ".";
			return;
		}

		ActiveTask* task = new ActiveTask(GetOwner(), taskId);
		task->SetData();
		task->DecodeString(is);
		m_ActiveTasks.insert(std::make_pair(taskId, task));
	}

	is >> split;
}

void ActiveTaskMgr::EncodeString(std::ostringstream& os)
{
	for (ActivityMap::iterator iter = m_ActiveTasks.begin(); iter != m_ActiveTasks.end(); ++iter)
	{
		os << iter->first << ',';
		iter->second->EncodeString(os);
	}

	os << UInt32(0) << ';';
}

void ActiveTaskMgr::DecodeOpString(std::istringstream& is)
{
	char split;
	do 
	{
		OpActTask* task = new OpActTask();
		task->DecodeString(is);
		task->isOverdue = false;

		OpActTaskData* data = ActivityMgr::Instance()->GetOpActTaskData(task->dataId);
		if (!data)
		{
			task->isOverdue = true;
		}
		else
		{
			// 旧活动没有表数据，疲劳燃烧活动不设置
			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(data->dataId);
			if (!taskDataEntry || taskDataEntry->opActivityType != OAT_FATIGUE_BURNING)
			{
				task->state = TaskStatus::TASK_INIT;
				if (task->curNum == UINT32_MAX)
				{
					task->state = TaskStatus::TASK_OVER;
				}
				else if (task->curNum >= data->completeNum)
				{
					task->state = TaskStatus::TASK_FINISHED;
				}
				else
				{
					task->state = TaskStatus::TASK_UNFINISH;
				}
			}
		}

		m_OldOpActTaskMap.push_back(task);
	} 
	while (is >> split);
}

void ActiveTaskMgr::EncodeOpString(std::ostringstream& os)
{
	std::map<UInt32, OpActTask*>::iterator itr = m_idToOpActTask.begin();
	bool bContinue = false;
	for (; itr != m_idToOpActTask.end(); ++itr)
	{
		if (itr != m_idToOpActTask.begin() && !bContinue)
		{
			os << ';';
		}

		OpActTask* task = itr->second;
		if (!task || task->isOverdue)	
		{
			bContinue = true;
			continue;
		}
		else
		{
			bContinue = false;
		}

		task->EncodeString(os);
	}
}

void ActiveTaskMgr::OnGetItem(UInt32 id, UInt32 num)
{
	ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (!data)
		return;

	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;
		bool needSave = false;

		if (!task || !task->GetDataEntry())
		{
			ErrorLogStream << "ActiveTask ptr or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		if (IsItemChange(task->GetDataEntry()->templateID))
		{
			UpdateItemChangeTask(task);
		}
		else
		{
			if (ITEM_TITLE == data->type)
			{
				ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
				if (script != NULL && script->CheckGetTitleEvent())
				{
					script->OnGetTitle(task->GetOwner(), task);
					needSave = true;
				}
			}
		}

		if (needSave)
		{
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnRmItem(UInt32 id, UInt32 num)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;
		if (!task)
		{
			ErrorLogStream << "ActiveTask ptr is null!" << LogStream::eos;
			continue;
		}
		
		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckGetItemEvent(id))
		{
			script->OnRmItem(task->GetOwner(), task, id, num);
			task->SaveToDB();
		}
		else
		{
			if (IsItemChange(task->GetDataEntry()->templateID))
			{
				UpdateItemChangeTask(task);
			}
		}
	}
}

void ActiveTaskMgr::OnChangeEquip(UInt32 id)
{
	for (auto tasks : m_ActiveTasks)
	{
		auto task = tasks.second;
		if (!task) continue;

		if (IsItemChange(task->GetDataEntry()->templateID))
		{
			UpdateItemChangeTask(task);
		}
	}
}

void ActiveTaskMgr::OnBuyMonthCard()
{
	InfoLogStream << PLAYERINFO(GetOwner()) << "Buy month card..." << LogStream::eos;

	_OnOpActBuyMonthCard();

	_UpdateSpringRedPacketRecvOpActTask(SRPTT_MONTH_CARD, 0);
}

void ActiveTaskMgr::OnDayChange()
{
	InfoLogStream << PLAYERINFO(GetOwner()) << "OnDayChange " << LogStream::eos;

	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::EventType::ET_DAYCHANGE, NULL);

	//七日跨日
	OnActiveDayChange();

	DayChargeNewOnDayChange();

	SyncAllTask();

	// 运营活动
	_UpdateDailyLoginOp();

	_OnDayChangeForDailyBuffOpAct();

	_OnDayChangeForArtifactJarOpAct();

	// 更新周年祈福
	_UpdateSecondAnniversaryPrayOpActTask();

}

void ActiveTaskMgr::OnLogicDayChanged()
{
	UpdateDungeonLastNum();

	OnLogicDayChangeForOpAct();

	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;
		if (!task)
			continue;

		if (IsSignIn(task->GetDataEntry()->templateID))
		{
			UInt32 day = GetOwner()->GetCreateFromNowOnDay();
			UInt32 loopNum = day / SIGH_IN_DAYNUM;
			day = day % SIGH_IN_DAYNUM;
			if (day == 0)
			{
				day = SIGH_IN_DAYNUM;
				loopNum--;
			}

			bool preTaskCompleted = false;
			auto preItr = m_ActiveTasks.find(task->GetDataEntry()->preTaskID);
			if (preItr != m_ActiveTasks.end())
			{
				ActiveTask* pre = preItr->second;
				if (pre && pre->GetStatus() == TASK_OVER)
				{
					preTaskCompleted = true;
				}
			}

			UpdateSignInTask(task, preTaskCompleted, loopNum, day);
		}
		else if (IsMonthCard(task->GetDataEntry()->templateID))
		{
			UInt32 oldRd = (UInt32)task->GetVar("rd");
			if (0 == oldRd)
			{
				task->SetStatusNoSync(TASK_INIT);
			}
			else
			{
				UInt32 newRd = oldRd - 1;
				task->SetVar("rd", newRd);
				task->SetStatusNoSync(TASK_FINISHED);
			}
		}
		else if (IsSevenDay(task->GetDataEntry()->templateID))
		{
			continue;
		}
		else
		{
			OnDayChangeScript(task, false, 1);
		}
	}

	//签到跨日
	UpdateSignInCount();

	MakeUpFatgue(false);
	MakeUpDungen(false);

	SyncAllTask();

	// 回归活动刷新挑战深渊消耗票子的数量
	RefreshDailyChallengeHell();
	RefreshDailyChallengeHell(1);

	_SendOpActOnlineEvent(true);

	UpdateGiftRightOnlineTask();
}

void ActiveTaskMgr::OnLogicWeekChanged()
{
	OnLogicWeekChangeForActivities();

	OnLogicWeekChangeForOpAct();
}

bool ActiveTaskMgr::SubmitTaskRp(std::vector<UInt32>& taskId)
{
	if (0 == taskId.size())
		return false;

	if ( (GetOwner()->GetCounter(SIGN_IN_RP_FREE_COUNT) == 0) &&
		(taskId.size() * 50 > GetOwner()->GetPointBoth()) )
		return false;

	ActiveTask* firstTask = FindTask(taskId[0]);
	if (!firstTask)
		return false;

	//如果第一个是补签已完成的 就要将nextTask置为finish
	if (firstTask->GetStatus() == TASK_FINISHED)
	{
		ActiveTask* lastTask = FindTask(taskId[taskId.size() - 1]);
		if (!lastTask)
			return false;

		UInt32 nextID = lastTask->GetDataEntry()->nextTaskID;
		ActiveTask* nextTask = FindTask(nextID);
		if (!nextTask)
			return false;

		firstTask->SetStatus(TASK_UNFINISH);
		nextTask->SetStatus(TASK_FINISHED);
		nextTask->SaveToDB();
	}
	
	for (size_t i = 0; i < taskId.size(); ++i)
	{
		ActiveTask* task = FindTask(taskId[i]);
		if (!task)
			continue;;

		SubmitTask(taskId[i], true);
	}
	
	GetOwner()->SyncProperty();
	return true;
}

bool ActiveTaskMgr::TakePhaseGift(UInt32 id)
{
	GiftPhaseData* data = GiftPhaseDataMgr::Instance()->FindEntry(id);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " data is null id:" << id << LogStream::eos;
		return false;
	}

	if (data->eventType == GPT_ONLINE_TIME)
	{
		if (m_onlineTimeBonusId != id + 1)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " param error! m_onlineTimeBonusId:" << m_onlineTimeBonusId << " id:" << id << LogStream::eos;
			return false;
		}
	}
	else if (data->eventType == GPT_REACH_LEVEL)
	{
		if (m_reachLvBonusId != id + 1)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " param error! m_reachLvBonusId:" << m_reachLvBonusId << " id:" << id << LogStream::eos;
			return false;
		}
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param error! data->eventType:" << data->eventType << LogStream::eos;
		return false;
	}

	//发奖励
	RewardGroup* grp = GetOwner()->GetRewardGroup();
	grp->AddRewards(data->rewards);
	GetOwner()->AddRewards(GetReason(SOURCE_TYPE_ROUTINE).c_str(), grp, true);

	return true;
}

void ActiveTaskMgr::UpdateSignInCount(UInt32 day, UInt32 loop)
{
	if (ActivityMgr::Instance()->IsInActivity(AID_SIGN_IN))
	{
		if (day == 0)
		{
			day = GetOwner()->GetCreateFromNowOnLogicDay();
			loop = day / SIGH_IN_DAYNUM;
			day = day % SIGH_IN_DAYNUM;

			if (day == 0)
			{
				day = SIGH_IN_DAYNUM;
				loop--;
			}
		}
	
		UInt32 isSignIn = 1;
		if (GetOwner()->GetCounter(DAY_SIGN_IN) > 0)
		{
			isSignIn = 0;
		}
		//计算补签次数
		UInt32 newRpCount = day - isSignIn - GetOwner()->GetCounter(SIGN_IN_TT_COUNT);
		if (GetOwner()->GetCount(SIGN_IN_RP_COUNT) != newRpCount)
		{
			GetOwner()->SetCounter(SIGN_IN_RP_COUNT, newRpCount);
		}

// 		DebugLogStream << PLAYERINFO(GetOwner()) << " day = " << day << " rp_count = " << GetOwner()->GetCounter(SIGN_IN_RP_COUNT) <<
// 			" sign_tt = " << GetOwner()->GetCounter(SIGN_IN_TT_COUNT) << LogStream::eos;
	}
}

bool ActiveTaskMgr::IsInActive(UInt32 activeId, ActiveTask* task)
{
	if (!task)
	{
		return false;
	}

	return ActivityMgr::Instance()->IsInActivity(activeId, task->GetAcceptTime());
}

bool ActiveTaskMgr::IsTimeInAcitveType(UInt32 time, UInt32 tmpType)
{
	auto activitys = ActivityMgr::Instance()->GetOpActDataByTmpTypes(tmpType);
	for (auto activity : activitys)
	{
		if (activity == NULL)
		{
			continue;
		}

		if (activity->state != AS_IN)
		{
			continue;
		}

		if (time < activity->startTime || time > activity->endTime)
		{
			continue;
		}

		return true;
	}

	return false;
}

void ActiveTaskMgr::GetRestTime(UInt32& time1, UInt32& time2)
{
	UInt32 ct = GetOwner()->GetCreateTime();
	Avalon::Time creatTime = Avalon::Time((UInt64)ct * 1000);
	Avalon::Date dt = Avalon::Date(creatTime);
	dt.Hour(0);
	dt.Min(0);
	dt.Sec(0);
	dt.MSec(0);

	UInt32 endtime1 = (UInt32)dt.ToTime().Sec() + SEVEN_END_DAY * 3600 * 24;
	UInt32 endtime2 = (UInt32)dt.ToTime().Sec() + SEVEN_CLOSE_DAY * 3600 * 24;

	if (CURRENT_TIME.Sec() >= endtime1)
		time1 = 0;
	else
		time1 = endtime1 - (UInt32)CURRENT_TIME.Sec();

	if ((UInt32)CURRENT_TIME.Sec() >= endtime2)
		time2 = 0;
	else
		time2 = endtime2 - (UInt32)CURRENT_TIME.Sec();
}

void ActiveTaskMgr::OnStartDungeon(const DungeonDataEntry* dungeonData, bool hasFriend, bool hasSameGuild)
{
	if (!dungeonData) return;

	_OnPlayerStartDungeonForDungeonRandomBuffOpAct(dungeonData);


	if (dungeonData->IsHellDungeon() || dungeonData->IsHellEntry())
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_ABYESS_DUNGEON, hasFriend || hasSameGuild);
		OnWeekSignSpring(dungeonData->tag);
	}
	else if (dungeonData->subType == DST_GUILD_DUNGEON)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_GUILD_DUNGEON, hasFriend || hasSameGuild);
		OnHireTask(HTT_GUILD_DUNGEON, 1);
	}
}

void ActiveTaskMgr::OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckClearDungeonEvent(dungeonId))
		{
			script->OnClearDungeon(task->GetOwner(), task, dungeonId);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::GMCommandClearDungeon(UInt32 dungeonId, bool isTeam)
{
	InfoLogStream << "Gm command clear dungeon(" << dungeonId << ")." << LogStream::eos;
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return;
	}

	CompleteDungeonParam param(dungeonId);
	param._eventType = CLEventMgr::ET_CLEAR_DUNGEON;
	OnEventCallback(&param);
}

void ActiveTaskMgr::OnPkLvUp(UInt32 type, UInt8 lv)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckPkLvUpEvent())
		{
			script->OnPkLvUp(task->GetOwner(), task, (UInt8)type, lv);
			task->SaveToDB();
		}
	}
}

bool ActiveTaskMgr::OnDayChangeScript(ActiveTask* task, bool online, UInt32 offlineDay)
{
	if (!task)
	{
		return false;
	}

	ActiveTask* preTask = NULL;
	if (task->GetDataEntry()->preTaskID != 0)
	{
		auto itr = m_ActiveTasks.find(task->GetDataEntry()->preTaskID);
		if (itr != m_ActiveTasks.end())
		{
			preTask = itr->second;
		}
	}

	ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
	if (script != NULL && script->CheckDayChange())
	{
		UInt8 oldState = task->GetStatus();
		script->OnDayChange(task->GetOwner(), task, preTask, offlineDay);
		if (oldState != task->GetStatus())
		{
			task->SaveToDB();
		}
		return true;
	}

	GameErrorLogStream("Player") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() 
		<< "," << GetOwner()->GetName() << ") ActiveTask Script Err id:" << task->GetID() << ".";

	return false;
}

void ActiveTaskMgr::UpdateActiveTasks(UInt32 offlineDay)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	UInt32 day = GetOwner()->GetCreateFromNowOnDay();

	while (itr != m_ActiveTasks.end())
	{
		ActiveTask* task = itr->second;
		if (!task || !task->GetDataEntry())
		{
			++itr;
			continue;
		}

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (!script)
		{
			++itr;
			continue;
		}

		if (IsSevenDay(task->GetDataEntry()->templateID))
		{
			//七日活动结束
			if (day > SEVEN_CLOSE_DAY)
			{
				//_OnSevenDayClose(task);
				
				CL_SAFE_DELETE(task);
				itr = m_ActiveTasks.erase(itr);
				continue;
			}
			else if (day > SEVEN_END_DAY && (task->GetStatus() != TASK_FINISHED && task->GetStatus() != TASK_OVER))
			{
				//七日活动超过7日的只能领取不能做
				task->SetStatusNoSync(TASK_FAILED);
			}

			++itr;
			continue;
		}
		
		if (script->CheckDayChange() && offlineDay > 0)
		{
			OnDayChangeScript(task, true, offlineDay);
		}

		if (script->CheckLevelUpEvent())
		{
			script->OnLvUpChange(m_pOwner, task);
		}

		++itr;
	}
}

void ActiveTaskMgr::SighInOnline()
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN, datas);

	UInt32 day = GetOwner()->GetCreateFromNowOnLogicDay();
	UInt32 loopNum = day / SIGH_IN_DAYNUM;
	day = day % SIGH_IN_DAYNUM;

	if (day == 0)
	{
		day = SIGH_IN_DAYNUM;
		loopNum--;
	}
		
	for (auto data : datas)
	{
		bool preTaskCompleted = false;
		auto preItr = m_ActiveTasks.find(data->preTaskID);
		if (preItr != m_ActiveTasks.end())
		{
			ActiveTask* pre = preItr->second;
			if (pre && pre->GetStatus() == TASK_OVER)
			{
				preTaskCompleted = true;
			}
		}

		auto itr = m_ActiveTasks.find(data->id);
		if (itr == m_ActiveTasks.end())
		{
			CreateSignInTask(data, preTaskCompleted, loopNum, day);
		}
		else
		{
			UpdateSignInTask(itr->second, preTaskCompleted, loopNum, day);
		}
	}

	UpdateSignInCount(day, loopNum);
}

void ActiveTaskMgr::SignInTotalOnline()
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_BONUS, datas);

	for (auto data : datas)
	{
		auto itr = m_ActiveTasks.find(data->id);
		if (itr == m_ActiveTasks.end())
		{
			ActiveTask* task = new ActiveTask(GetOwner(), data->id);
			task->SetData(data);
			task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
			task->SetVarNoSync("tt_si", 0);
			task->SetStatusNoSync(TaskStatus::TASK_UNFINISH);

			m_ActiveTasks.insert(std::make_pair(data->id, task));
		}
	}
}

void ActiveTaskMgr::CreateSignInTask(ActiveTaskDataEntry* data, bool pre, UInt32 loopNum, UInt32 day)
{
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " ActiveTaskData is null!" << LogStream::eos;
		return;
	}

	//InfoLogStream << PLAYERINFO(GetOwner()) << " begin create signin task id = " << data->id << " pre = " << pre << LogStream::eos;

	UInt32 dayCount = data->id % 100;
	UInt32 daySignin = GetOwner()->GetCount(DAY_SIGN_IN);

	ActiveTask* task = new ActiveTask(GetOwner(), data->id);
	task->SetData(data);
	task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
	task->SetStatusNoSync(TASK_UNFINISH);
	task->SetVarNoSync("ln", loopNum);

	if ((day >= dayCount && pre && daySignin == 0) || dayCount == 1)
	{
		task->SetStatusNoSync(TASK_FINISHED);

		if (dayCount == 1)
		{	
			GetOwner()->SetCounter(SIGN_IN_RP_FREE_COUNT, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SIGHIN_FREE_NUM));
		}
	}

	m_ActiveTasks.insert(std::make_pair(data->id, task));

	//InfoLogStream << PLAYERINFO(GetOwner()) << " create signin task id = " << data->id << " status = " << task->GetStatus() << " day = " << day << " dayCount = " << dayCount << LogStream::eos;
}

void ActiveTaskMgr::UpdateSignInTask(ActiveTask* task, bool pre, UInt32 loopNum, UInt32 day)
{
	if (!task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " task is null!" << LogStream::eos;
		return;
	}

	//InfoLogStream << PLAYERINFO(GetOwner()) << " begin update signin task id = " << task->GetDataEntry()->id << " pre = " << pre << LogStream::eos;

	UInt32 dayCount = task->GetDataEntry()->id % 100;
	UInt32 oldLoopNum = (UInt32)task->GetVar("ln");
	UInt32 daySignin = GetOwner()->GetCount(DAY_SIGN_IN);

	if (loopNum > oldLoopNum)
	{
		task->SetStatusNoSync(TASK_UNFINISH);
		if ((day >= dayCount && pre && daySignin == 0 && task->GetStatus() == TASK_UNFINISH) || dayCount == 1)
		{
			task->SetStatusNoSync(TASK_FINISHED);
			task->SetVarNoSync("ln", loopNum);

			if (dayCount == 1)
			{
				GetOwner()->SetCounter(SIGN_IN_TT_COUNT, 0);
				GetOwner()->SetCounter(SIGN_IN_RP_FREE_COUNT, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SIGHIN_FREE_NUM));
			}
		}
	}
	else
	{
		if (day >= dayCount && pre && daySignin == 0 && task->GetStatus() == TASK_UNFINISH)
		{
			task->SetStatusNoSync(TASK_FINISHED);
			task->SetVarNoSync("ln", loopNum);
		}
	}

// 	InfoLogStream << PLAYERINFO(GetOwner()) << " loopNum = " << loopNum << " oldLoopNum = " << oldLoopNum << " pre = " << pre
// 		<< " day = " << day << " dayCount = " << dayCount << " status = " << task->GetStatus() << LogStream::eos;
}

bool ActiveTaskMgr::SubmitSignInTask(ActiveTask* task, bool bRp /*= false*/)
{
	if (!task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " task is null!" << LogStream::eos;
		return false;
	}

	if (bRp)
	{
		if (task->GetStatus() != TASK_UNFINISH)
			return false;

		UInt32 rpCount = GetOwner()->GetCounter(SIGN_IN_RP_COUNT);
		UInt32 rpFreeCount = GetOwner()->GetCounter(SIGN_IN_RP_FREE_COUNT);
		if (rpFreeCount == 0)
		{
			// 付费补签
			UInt32 costItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RP_SIGHIN_COST_ITEMID);
			if (NULL == ItemDataEntryMgr::Instance()->FindEntry(costItemId))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " costItemId is invalid! id:" << costItemId << LogStream::eos;
				return false;
			}

			UInt32 costNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RP_SIGHIN_COST_NUM);

			if (GetOwner()->GetItemNum(costItemId) < costNum)
				return false;
			
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, task->GetID(), task->GetStatus());

			GetOwner()->RemoveItem(reason.c_str(), costItemId, costNum);
		}
		else
		{
			GetOwner()->SetCounter(SIGN_IN_RP_FREE_COUNT, rpFreeCount - 1);
		}

		GetOwner()->SetCounter(SIGN_IN_RP_COUNT, rpCount - 1);

		UInt32 day = GetOwner()->GetCreateFromNowOnLogicDay();
		UInt32 loopNum = day / SIGH_IN_DAYNUM;
		task->SetVarNoSync("ln", loopNum);
	}
	else
	{
		if (task->GetStatus() != TASK_FINISHED)
			return false;

		GetOwner()->IncCounter(DAY_SIGN_IN, 1);
	}

	task->SetStatus(TASK_SUBMITTING);
	GetOwner()->IncCounter(SIGN_IN_TT_COUNT, 1);

	//计算累签数量
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_SIGN_IN_BONUS, datas);

	for (auto data : datas)
	{
		auto itr = m_ActiveTasks.find(data->id);
		if (itr != m_ActiveTasks.end())
		{
			ActiveTask* task = itr->second;
			if (!task)
			{
				continue;
			}

			UInt32 maxNum = 30;
			UInt32 newNum = (UInt32)task->GetVar("tt_si") + 1;
			if (newNum >= maxNum)
			{
				newNum = maxNum;
				task->SetStatus(TaskStatus::TASK_FINISHED);
			}

			task->SetVar("tt_si", newNum);

			task->SaveToDB();
		}
	}

	return true;
}


void ActiveTaskMgr::OnMonthCardWelfare(UInt32 remainDays, bool isRenew, bool bDayChange, bool isOnline)
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_MONTH_CARD, datas);

	if (datas.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " create month card active task data num err!" << LogStream::eos;
		return;
	}

	ActiveTaskDataEntry* data = datas[0];
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " create month card active task data err!" << LogStream::eos;
		return;
	}

	auto itr = m_ActiveTasks.find(data->id);

	if (itr == m_ActiveTasks.end())
	{
		if (remainDays > 0)
		{
			CreateMonthCardTask(remainDays, data);
		}
	}
	else
	{
		UpdateMonthCardTask(remainDays, itr->second, isRenew, bDayChange, isOnline);
	}

	SyncAllTask();
}

void ActiveTaskMgr::CreateMonthCardTask(UInt32 remainDays, ActiveTaskDataEntry* data)
{
	if (!data) return;
	
	DebugLogStream << PLAYERINFO(GetOwner()) << " create month card task" << LogStream::eos;

	ActiveTask* task = new ActiveTask(GetOwner(), data->id);
	task->SetData(data);
	task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
	task->SetStatusNoSync(TASK_FINISHED);
	task->SetVarNoSync("rd", remainDays - 1);

	task->SaveToDB();
	m_ActiveTasks.insert(std::make_pair(data->id, task));
}

void ActiveTaskMgr::UpdateMonthCardTask(UInt32 remainDays, ActiveTask* task, bool isRenew, bool bDayChange, bool isOnline)
{
	if (!task) return;
	
	//过期
	if (0 == remainDays)
	{
		task->SetStatusNoSync(TASK_INIT);
		task->SetVarNoSync("rd", remainDays);
		task->SaveToDB();

		DebugLogStream << PLAYERINFO(GetOwner()) << " month card expired!" << LogStream::eos;
	}
	else
	{
		UInt32 newRd = remainDays - 1;
		UInt32 oldRd = (UInt32)task->GetVar("rd");
	
		//登陆
		if (isOnline)
		{
			//登陆跨日更新
			if (bDayChange)
			{
				task->SetStatusNoSync(TASK_FINISHED);
				task->SetVarNoSync("rd", newRd);
				task->SaveToDB();

				DebugLogStream << PLAYERINFO(GetOwner()) << "daychange update month card! newRd:" << newRd << 
					" oldRd:" << oldRd << " status:" << task->GetStatus() << LogStream::eos;
			}

			//做个自我修正,正常情况下newRd == oldRd
			if (newRd != oldRd)
			{
				task->SetStatusNoSync(TASK_FINISHED);
				task->SetVarNoSync("rd", newRd);
				task->SaveToDB();

				DebugLogStream << PLAYERINFO(GetOwner()) << "fix update month card! newRd:" << newRd << " oldRd:" << oldRd << " status:" << task->GetStatus() << LogStream::eos;
			}
			return;
		}
		else
		{
			//购买月卡
			//叠加购买
			if (!isRenew)
			{
				task->SetVarNoSync("rd", newRd);

				DebugLogStream << PLAYERINFO(GetOwner()) << " overlay buy month card! newRd:" << newRd << " oldRd:" << oldRd << " status:" << task->GetStatus() << LogStream::eos;
			}
			else
			{
				//非叠加购买
				task->SetStatusNoSync(TASK_FINISHED);
				task->SetVarNoSync("rd", newRd);

				DebugLogStream << PLAYERINFO(GetOwner()) << " buy month card! newRd:" << newRd << " oldRd:" << oldRd << " status:" << task->GetStatus() << LogStream::eos;
			}

			task->SaveToDB();
		}
	}
}

bool ActiveTaskMgr::IsItemChange(UInt32 ActiveId)
{
	// 道具兑换一开始是从 13000 开始的，所以这里避免一些无效的查找
	if (ActiveId < (UInt32)AID_ITEM_CHANGE)
	{
		return false;
	}

	UInt8 activityTmpType = ActivityTemplateDataEntryMgr::Instance()->GetActivityTemplateType(ActiveId);

	if (activityTmpType == (UInt8)ATT_ITEM_EXCHANGE || activityTmpType == (UInt8)ATT_ITEM_EXCHANGE2)
	{
		return true;
	}
	
	return false;
}

void ActiveTaskMgr::SendUnreciveBonusToMail(ActiveTask* task)
{
	if (!task)
	{
		return;
	}

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	const char* reason = GetReason(LogSourceType::SOURCE_TYPE_CHARGE).c_str();
	GetOwner()->GetMailInfoByReason(reason, strSenderName, strTitle, strContent);

	RewardGroup rg(task->GetDataEntry()->rewards);
	
	Sys::SendSysMail(GetOwner()->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason, &rg);
}

void ActiveTaskMgr::AcceptItemChangeTask(const std::vector<ActiveTaskDataEntry*>& taskDatas)
{
	for (auto data : taskDatas)
	{
		if (!data)
		{
			ErrorLogStream << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		auto iter = m_ActiveTasks.find(data->id);
		if (iter == m_ActiveTasks.end())
		{
			CreateItemChangeTask(data);
			continue;
		}

		auto itemChangeTask = iter->second;
		if (!itemChangeTask)
		{
			ErrorLogStream << "ActiveTask ptr is null!" << LogStream::eos;
			continue;
		}

		itemChangeTask->SyncStatus();

		const char* countVarKey = itemChangeTask->GetDataEntry()->taskCycleKey.c_str();
		itemChangeTask->SetVar(countVarKey, itemChangeTask->GetVar(countVarKey));
		itemChangeTask->SetDBFlag(df_none);

		UpdateItemChangeTask(itemChangeTask);
	}
}

void ActiveTaskMgr::CreateItemChangeTask(ActiveTaskDataEntry* data)
{
	if (!data)
	{
		ErrorLogStream << "ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	ActiveTask* itemChangeTask = new ActiveTask(GetOwner(), data->id);
	if (!itemChangeTask)
	{
		ErrorLogStream << "ActiveTask ptr is null!" << LogStream::eos;
		return;
	}

	itemChangeTask->SetData(data);
	itemChangeTask->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
	itemChangeTask->SyncStatus();

	if (data->taskCycleCount >= 0)
	{
		itemChangeTask->SetVar(data->taskCycleKey.c_str(), 0);	// 对于道具兑换，这里的变量是指兑换次数
	}
	else
	{
		itemChangeTask->SetVar(data->taskCycleKey.c_str(), data->taskCycleCount);
	}

	itemChangeTask->SaveToDB();

	m_ActiveTasks.insert(std::make_pair(data->id, itemChangeTask));

	UpdateItemChangeTask(itemChangeTask);

	InfoLogStream << PLAYERINFO(GetOwner()) << " create task(" << itemChangeTask->GetID() << ", " << itemChangeTask->GetUID() << ")" << LogStream::eos;
}

void ActiveTaskMgr::UpdateItemChangeTask(ActiveTask* itemChangeTask)
{
	if (!itemChangeTask || !itemChangeTask->GetDataEntry())
	{
		ErrorLogStream << "ActiveTask or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return;
	}

	//检查兑换次数
	if (itemChangeTask->GetDataEntry()->accountTotalSubmitLimit > 0)
	{
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ITEM_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), itemChangeTask->GetID());
		if (times >= itemChangeTask->GetDataEntry()->accountTotalSubmitLimit)
		{
			itemChangeTask->SetStatus(TASK_OVER);
			itemChangeTask->SaveToDB();
			return;
		}
	}
	else
	{
		Int64 itemChangeCount = itemChangeTask->GetVar(itemChangeTask->GetDataEntry()->taskCycleKey.c_str());
		if (itemChangeCount > 0)
		{
			Int64 maxItemChangeCount = itemChangeTask->GetDataEntry()->taskCycleCount;
			if (itemChangeCount >= maxItemChangeCount)
			{
				itemChangeTask->SetStatus(TASK_OVER);
				itemChangeTask->SaveToDB();
				return;
			}
		}
	}


	// 兑换满足的条件数量
	size_t condNum = 0;

	for (auto consumeItem : itemChangeTask->GetDataEntry()->consumeItems)
	{                                      
		UInt32 hasNum = GetOwner()->GetItemNum(consumeItem.id);
		if (hasNum >= consumeItem.num)
		{
			condNum++;
		}
	}

	if (condNum != itemChangeTask->GetDataEntry()->consumeItems.size())
	{
		itemChangeTask->SetStatus(TASK_UNFINISH);
	}
	else
	{
		itemChangeTask->SetStatus(TASK_FINISHED);
	}

	itemChangeTask->SaveToDB();
}

bool ActiveTaskMgr::SubmitItemChangeTask(ActiveTask* itemChangeTask)
{
	if (!itemChangeTask || !itemChangeTask->GetDataEntry())
	{
		ErrorLogStream << "ActiveTask or ActiveTaskDataEntry ptr is null!" << LogStream::eos;
		return false;
	}

	if (!IsInActive(itemChangeTask->GetDataEntry()->templateID, itemChangeTask))
	{
		GetOwner()->SendNotify(9201);
		return false;
	}

	if (itemChangeTask->GetStatus() != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " task(" << itemChangeTask->GetID() << "," << itemChangeTask->GetDataEntry()->id << ") status(" << itemChangeTask->GetStatus() << ") is error!" << LogStream::eos;
		return false;
	}

	Int64 itemChangeCount = 0;
	if (itemChangeTask->GetDataEntry()->accountTotalSubmitLimit > 0)
	{
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ITEM_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), itemChangeTask->GetID());
		if (times >= itemChangeTask->GetDataEntry()->accountTotalSubmitLimit)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " ItemChangeTask(" << itemChangeTask->GetID() << ") account total times limit, current(" << times << ") max(" << itemChangeTask->GetDataEntry()->accountTotalSubmitLimit << ")." << LogStream::eos;
			return false;
		}
	}
	else
	{
		// 检查兑换次数
		itemChangeCount = itemChangeTask->GetVar(itemChangeTask->GetDataEntry()->taskCycleKey.c_str());
		if (itemChangeCount > 0)
		{
			Int64 maxItemChangeCount = itemChangeTask->GetDataEntry()->taskCycleCount;
			if (itemChangeCount >= maxItemChangeCount)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Item change count(" << itemChangeCount << ") has reached max(" << maxItemChangeCount << ")!" << LogStream::eos;
				return false;
			}
		}
	}


	// 检查消耗的道具
	for (auto consumeItem : itemChangeTask->GetDataEntry()->consumeItems)
	{
		UInt32 hasNum = GetOwner()->GetItemNum(consumeItem.id);
		if (hasNum < consumeItem.num)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Consume item(" << consumeItem.id << ") num(" << hasNum << ") are not enough!" << LogStream::eos;
			return false;
		}
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ITEM_CHANGE);

	// 扣道具
	for (auto consumeItem : itemChangeTask->GetDataEntry()->consumeItems)
	{
		if (consumeItem.num > 0 && !GetOwner()->RemoveItem(reason.c_str(), consumeItem.id, consumeItem.num))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << consumeItem.id << ", num: " << consumeItem.num << ") failed." << LogStream::eos;
			return false;
		}
	}

	itemChangeTask->SetStatus(TASK_SUBMITTING);

	if (itemChangeTask->GetDataEntry()->accountTotalSubmitLimit > 0)
	{
		OpActivityRecordMgr::Instance()->UpdateRecord(OP_ITEM_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), itemChangeTask->GetDataEntry()->id, 1);
	}
	else
	{
		if (itemChangeCount >= 0)
		{
			itemChangeCount++;
			itemChangeTask->SetVar(itemChangeTask->GetDataEntry()->taskCycleKey.c_str(), itemChangeCount);
		}
	}

	return true;
}

void ActiveTaskMgr::OnLogicWeekChangeForItemChangeTask()
{
	ActivityMgr::Instance()->VisitOpenActivities(GetOwner(), [&](const ActivityInfo& activity) {

		if (!IsItemChange(activity.id))
		{
			return true;
		}

		std::vector<ActiveTaskDataEntry*> datas;
		ActiveTaskDataEntryMgr::Instance()->GetActives(activity.id, datas);
		for (auto data : datas)
		{
			if (!data) continue;

			// 不是周刷新任务
			if (data->param1 != TimeUtil::REFRESH_TYPE_PER_WEEK)
			{
				continue;
			}

			auto itemChangeTask = FindTask(data->id);
			if(!itemChangeTask) continue;

			itemChangeTask->SetVar(data->taskCycleKey.c_str(), 0);
			itemChangeTask->SetStatus(TASK_UNFINISH);

			UpdateItemChangeTask(itemChangeTask);
			itemChangeTask->SaveToDB();

			InfoLogStream << PLAYERINFO(GetOwner()) << " refresh task(" << itemChangeTask->GetID() << "," << data->id << ") var=" 
				<< itemChangeTask->GetVar(data->taskCycleKey.c_str()) << ", status=" << itemChangeTask->GetStatus() << LogStream::eos;
		}

		return true;
	});
}

void ActiveTaskMgr::DayChargeOnline(UInt32 offlineDay)
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_CHARGE_DAY, datas);

	UInt32 dayChargeNum = GetOwner()->GetDayChargeNum();

	for (auto data : datas)
	{
		auto itr = m_ActiveTasks.find(data->id);

		if (itr == m_ActiveTasks.end())
		{
			//没有任务就接取
			ActiveTask* task = new ActiveTask(GetOwner(), data->id);
			task->SetData(data);
			task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
			
			if (dayChargeNum >= task->GetDataEntry()->param1)
			{
				task->SetStatus(TASK_FINISHED);
			}
			else
			{
				task->SetStatus(TASK_UNFINISH);
			}

			m_ActiveTasks.insert(std::make_pair(data->id, task));

			InfoLogStream << "player (" << GetOwner()->GetAccID() << ") accept activeTask(" << data->id << ")" << LogStream::eos;
		}
		else
		{
			//有任务就更新
			ActiveTask* task = itr->second;
			if (!task) continue;
			
			//需要重置
			if (offlineDay > 0)
			{
				//任务是完成得就把上次得奖励发邮件
				if (task->GetStatus() == TASK_FINISHED)
				{
					SendUnreciveBonusToMail(task);
				}

				//重置任务状态
				if (dayChargeNum >= task->GetDataEntry()->param1)
				{
					task->SetStatus(TASK_FINISHED);
				}
				else
				{
					task->SetStatus(TASK_UNFINISH);
				}

				task->SaveToDB();
			}
		}
	}
}

void ActiveTaskMgr::DayChargeNewOnline(bool bDayChanged)
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_CHARGE_DAY_NEW, datas);

	for (auto data : datas)
	{
		if (!data) continue;

		auto itr = m_ActiveTasks.find(data->id);

		if (itr == m_ActiveTasks.end())
		{
			//没有任务就接取
			ActiveTask* task = new ActiveTask(GetOwner(), data->id);
			task->SetData(data);
			task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
			task->SetStatus(TASK_UNFINISH);
			task->SaveToDB();

			m_ActiveTasks.insert(std::make_pair(data->id, task));

			InfoLogStream << PLAYERINFO(GetOwner()) << " accept activeTask(" << data->id << ")" << LogStream::eos;
		}
		else
		{
			//有任务就更新
			ActiveTask* task = itr->second;
			if (!task) continue;

			if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_CHECK_DAY_CHANGE_FOR_CHARGE_DAY_ACT))
			{
				//需要重置
				if (bDayChanged)
				{
					task->SetStatus(TASK_UNFINISH);

					task->SaveToDB();

					InfoLogStream << PLAYERINFO(GetOwner()) << " task(" << task->GetID() << "," << data->id << ") on day change." << LogStream::eos;
				}
			}
		}
	}
}

void ActiveTaskMgr::DayChargeNewOnDayChange()
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_CHARGE_DAY_NEW, datas);

	for (auto data : datas)
	{
		if (!data) continue;

		auto task = FindTask(data->id);
		if (task == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can't find task(" << data->id << ")!" << LogStream::eos;
			continue;
		}

		task->SetStatus(TASK_UNFINISH);
		task->SaveToDB();
	}
}

bool ActiveTaskMgr::CanBuyGiftRightCard()
{
	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_GIFT_RIGHT))
	{
		return false;
	}

	if (GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD_BUY_TIME) > 0)
	{
		return false;
	}

	if (GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD) > 0)
	{
		return false;
	}

	bool canBuy = false;
	VisitOpenOpActDataByOpType(OAT_GIFT_RIGHT, [&](const OpActivityRegInfo* opAct) {
		if (!opAct) 
		{
			return false;
		}

		if (CURRENT_TIME.Sec() < opAct->parm)
		{
			canBuy = true;
			return false;
		}
		return true;
	});

	if (!canBuy)
	{
		return false;
	}

	return true;
}

bool ActiveTaskMgr::CanBuyDungeonRightCard()
{
	if (!IsTimeInAcitveType(CURRENT_TIME.Sec(), OAT_ABYESS_TICKET_REDUCE))
	{
		return false;
	}

	bool canBuy = false;
	VisitOpenOpActAndTaskByOpType(OAT_ABYESS_TICKET_REDUCE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;


		if (taskData.variables.size() != 2 || taskData.variables2.size() != 2)
		{
			return false;
		}

		if (CURRENT_TIME.Sec() >= taskData.variables[0] && CURRENT_TIME.Sec() <= taskData.variables[1])
		{
			if (task->state == TASK_UNFINISH)
			{
				canBuy = true;
			}
			return false;
		}

	});

	return canBuy;
}

bool ActiveTaskMgr::HaveDungeonRightCard()
{
	bool have = false;
	VisitOpenOpActAndTaskByOpType(OAT_ABYESS_TICKET_REDUCE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;


		if (taskData.variables.size() != 2 || taskData.variables2.size() != 2)
		{
			return false;
		}

		if (CURRENT_TIME.Sec() >= taskData.variables2[0] && CURRENT_TIME.Sec() <= taskData.variables2[1])
		{
			if (task->state == TASK_FINISHED)
			{
				have = true;
			}
		}

		return false;

	});

	return have;
}

void ActiveTaskMgr::SetDungeonRightCard()
{
	if (!CanBuyDungeonRightCard())
	{
		return;
	}

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_ABYESS_TICKET_REDUCE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;


		if (taskData.variables.size() != 2 || taskData.variables2.size() != 2)
		{
			return false;
		}

		if (task->state == TASK_UNFINISH)
		{
			task->state = TASK_FINISHED;
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
			return true;
		}
		return false;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}


void ActiveTaskMgr::OnOffline()
{
	
}

void ActiveTaskMgr::OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckDeathTower())
		{
			//DebugLogStream << PLAYERINFO(GetOwner()) << ",taskid = " << task->GetID() << ",towerId = " << towerId << ",layer = " << layer << ",usedTime = " << usedTime << LogStream::eos;

			script->OnDeathTower(task->GetOwner(), task, towerId, layer, usedTime);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnWarpStoneUpLevel(UInt32 type, UInt32 level)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckWarpStoneUpLevel())
		{
			script->OnWarpStoneUpLevel(task->GetOwner(), task, type, level);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnBuyBlackShop()
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckBuyBlackShop())
		{
			script->OnBuyBlackShop(task->GetOwner(), task);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckShopBuyEvent(shopId, shopItemId))
		{
			script->OnShopBuy(task->GetOwner(), task, shopId, shopItemId, num);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnBuyMallItem(UInt32 mallItemId, UInt32 mallItemNum)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckBuyMallItemEvent(mallItemId))
		{
			script->OnBuyMallItem(task->GetOwner(), task, mallItemId, mallItemNum);
			task->SaveToDB();
		}
	}

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_BUY_PRRSENT, [mallItemId, mallItemNum, &protocol, this](const OpActivityRegInfo* opAct, const OpActTaskData& taskData){
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}

		if (_UpdateBuyPresentOpActTask(*task, taskData, *opAct, mallItemId, mallItemNum))
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::OnPVP(PkType type, PkRaceResult status, std::vector<Pk3V3PlayerRaceEndInfo> results)
{
	// Pk事件
	CompletePkParam param(type, status);
	param._eventType = CLEventMgr::ET_COMPLETE_PK;
	OnEventCallback(&param);

	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckPk())
		{
			script->OnPk(task->GetOwner(), task);
			task->SaveToDB();
		}
	}

	_OnPlayerPVPEndForPVPPKCoinOpAct(type, status);

	NewYear2020(NEW_YEAR_2020_1V1, 1, type);

	UInt32 selfRoomID = 0;
	for (auto &i : results)
	{
		if (i.info.roleId == GetOwner()->GetID())
		{
			selfRoomID = i.roomId;
			break;
		}
	}

	bool hasFriend = false, hasSameGuild = false;
	for (auto &i : results)
	{
		auto data = i.info.roleId;
		if (GetOwner()->HasRelation(RELATION_FRIEND, data))
		{
			hasFriend = true;
			break;
		}

		if (GetOwner()->GetGuildId() != 0)
		{
			auto mate = PlayerMgr::Instance()->FindPlayer(data);
			if (mate != nullptr)
			{
				if (GetOwner()->GetGuildId() == mate->GetGuildId() && GetOwner() != mate && i.roomId == selfRoomID)
				{
					hasSameGuild = true;
					break;
				}
			}
		}
	}

	if (type == PkType::PK_3V3_ACTIVITY)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_3V3_PK, hasFriend || hasSameGuild);
	}

	if (type == PkType::PK_2V2_ACTIVITY)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_2V2_PK, false);
	}
}

void ActiveTaskMgr::OnStrengthenEquip()
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckEquipStrenth())
		{
			script->OnEquipStrenth(task->GetOwner(), task);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnRetinueChangeSkill(std::vector<RetinueSkill> newSkills)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		for (UInt32 i = 0; i < newSkills.size(); ++i)
		{
			if (script != NULL && script->CheckRetinueChangeSkill())
			{
				script->OnRetinueChangeSkill(task->GetOwner(), task, newSkills[i].buffId);
				task->SaveToDB();
			}
		}
	}
}

void ActiveTaskMgr::OnRetinueUpStar(UInt32 retinueId, UInt8 retinueStar)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		
		if (script != NULL && script->CheckRetinueUpStarEvent())
		{
			script->OnRetinueUpStar(task->GetOwner(), task, retinueId, retinueStar);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnRetinueLvUp(UInt32 retinueId, UInt8 lv)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());

		if (script != NULL && script->CheckRetinueUpLevelEvent())
		{
			script->OnRetinueUpLevel(task->GetOwner(), task, retinueId, lv);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnWearFashion(UInt32 num)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckWearFashionEquip())
		{
			script->OnWearFashionEquip(task->GetOwner(), task, num);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnAddMagic()
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckAddMagic())
		{
			script->OnAddMagic(task->GetOwner(), task);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnCharge(const char* reason, UInt32 value)
{
	_CreateActiveTask(AID_CHARGE_FIRST);
	_CreateActiveTask(AID_CHARGE_TOT);
	
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckCharge())
		{
			script->OnCharge(task->GetOwner(), task, value);
			task->SaveToDB();
		}
	}

	_OnChargeForSpringRedPacketOpActTask(value);
	_OnMoneyChargeForRechargeAndConsumeOpActTask(reason, value);
}

void ActiveTaskMgr::OnEventChargeMoney(IEventParam* param)
{
	if (!param)
	{
		return;
	}

	UInt32 dayChargeNum = GetOwner()->GetDayChargeNum();

	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;
		if (!task) continue;
		
		if (AID_CHARGE_DAY == task->GetDataEntry()->templateID)
		{
			if (dayChargeNum >= task->GetDataEntry()->param1 &&
				task->GetStatus() == TASK_UNFINISH)
			{
				task->SetStatus(TASK_FINISHED);
			}

			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnBuyDayChargeWelfare(UInt32 chargeGoodsId, UInt32 chargeNum)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVEICE_DAY_CHARGE_WELFARE))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Switch has closed! charge goods id=" <<
			chargeGoodsId << ", charge num=" << chargeNum << LogStream::eos;
		return;
	}

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_CHARGE_DAY_NEW, datas);

	InfoLogStream << PLAYERINFO(GetOwner()) << " buy day charge welfare, charge goods id=" << 
		chargeGoodsId << ", charge num=" << chargeNum << LogStream::eos;

	for (auto data : datas)
	{
		if (!data) continue;

		if (data->param1 != chargeGoodsId)
		{
			continue;
		}

		auto task = FindTask(data->id);
		if (!task)
		{
			//没有任务就接取
			ActiveTask* task = new ActiveTask(GetOwner(), data->id);
			task->SetData(data);
			task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
			task->SetStatus(TASK_UNFINISH);

			task->SaveToDB();

			m_ActiveTasks.insert(std::make_pair(data->id, task));

			InfoLogStream << PLAYERINFO(GetOwner()) << " accept activeTask(" << data->id << ")" << LogStream::eos;
		}

		InfoLogStream << PLAYERINFO(GetOwner()) << " the task(" << task->GetID() << ", " << task->GetUID() << ") status=" << task->GetStatus() << LogStream::eos;

		if (task->GetStatus() != TASK_UNFINISH)
		{
			if (task->GetStatus() == TASK_OVER)
			{
				auto chargeMallData = ChargeMallDataEntryMgr::Instance()->FindEntry(chargeGoodsId);
				if (chargeMallData != NULL)
				{
					// 已经做了的任务,只给点券
					GetOwner()->AddItem(GetReason(SOURCE_TYPE_TASK_ACTIVE, data->id, chargeGoodsId).c_str(), chargeMallData->itemId, chargeMallData->num);
					InfoLogStream << PLAYERINFO(GetOwner()) << " charge again today, give item(" << chargeMallData->itemId << ", " << chargeMallData->num << ")" << LogStream::eos;
				}
			}

			return;
		}

		task->SetStatus(TASK_OVER);
		task->SaveToDB();

		// 给账号计数
		OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_DAILY_CHARGE_DAY, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), data->id, 1);

		//发奖励
		RewardGroup* grp = GetOwner()->GetRewardGroup();
		grp->AddRewards(data->rewards);
		GetOwner()->AddRewards(GetReason(SOURCE_TYPE_TASK_ACTIVE, data->id, chargeGoodsId).c_str(), grp, true);

		InfoLogStream << PLAYERINFO(GetOwner()) << "first charge today, task(" << task->GetID() << ") charge goods id=" << chargeGoodsId << ", charge num=" << chargeNum << LogStream::eos;
	}
}

void ActiveTaskMgr::OnVipLvUp()
{
	UpdateMaxFatigue();
}

void ActiveTaskMgr::OnRandQL(IEventParam* param)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckRandQL())
		{
			script->OnRandQL(task->GetOwner(), task);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnEventLevelUp(IEventParam* param)
{
	if (!param)
	{
		return;
	}

	LevelUpParam* myParam = (LevelUpParam*)param;

	std::vector<ActivityInfo> openActs;
	ActivityMgr::Instance()->GetOpenActivity(GetOwner(), openActs);

	for (size_t i = 0; i < openActs.size(); ++i)
	{
		ActivityInfo& info = openActs[i];
		if (info.level > myParam->_oldLevel && info.level <= myParam->_newLevel)
		{
			if (info.id >= AID_JAR_ACT_1 && info.id <= AID_JAR_ACT_MAX)
			{
				SendUpdateNotifyInfo((UInt32)NT_JAR_OPEN, info.id);
			}
		}
	}
}

void ActiveTaskMgr::OnWearEquip()
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckWearEquip())
		{
			script->OnWearEquip(task->GetOwner(), task);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnOpenMagicJar()
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckOpenJar())
		{
			script->OnOpenJar(task->GetOwner(), task);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnClearHell(UInt32 dungeonId, UInt32 usedTime)
{
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	for (; itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;

		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(task->GetID());
		if (script != NULL && script->CheckAddClearHell())
		{
			script->OnClearHell(task->GetOwner(), task, dungeonId, usedTime);
			task->SaveToDB();
		}
	}
}

void ActiveTaskMgr::OnLoadTask(UInt64 guid, UInt32 dataId, TaskStatus status, std::string parameter, UInt32 acceptTime)
{
	ActiveTaskDataEntry* dataEntry = ActiveTaskDataEntryMgr::Instance()->FindEntry(dataId);
	if (dataEntry == NULL) return;
	if (dataEntry->isAbandon)	return;

	//判断活动是否有效
	//七日活动是否有效
	UInt32 day = GetOwner()->GetCreateFromNowOnDay();
	if (IsSevenDay(dataEntry->templateID) && day > SEVEN_CLOSE_DAY)
	{
		return;
	}
	//活动是否还开启
	if (!ActivityMgr::Instance()->IsInActivity(dataEntry->templateID, GetOwner()))
	{
		return;
	}

	UInt32 activityStartTime = 0;
	UInt32 activityEndTime = 0;
	if (!ActivityMgr::Instance()->GetActivityTime(dataEntry->templateID, activityStartTime, activityEndTime))
	{
		auto pPlayerInfo = GetOwner()->GetPlayerActivityInfo().FindActivity(dataEntry->templateID);
		if (!pPlayerInfo)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find activity(" << dataEntry->templateID << ")!" << LogStream::eos;
			return;
		}

		activityStartTime = pPlayerInfo->startTime;
		activityEndTime = pPlayerInfo->dueTime;
	}

	if (acceptTime < activityStartTime || acceptTime > activityEndTime)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << guid << ", " << dataId << ")'s accept time(" << acceptTime 
			<< ") reach limit(" << activityStartTime << ", " << activityEndTime << ")!" << LogStream::eos;
		return;
	}

	ActiveTask* task = NULL;
	if (m_ActiveTasks.find(dataId) != m_ActiveTasks.end())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "find repeat activetask! dataid:" << dataId << LogStream::eos;
		return;
	}

	task = new ActiveTask(GetOwner(), dataId);
	task->SetData(dataEntry);
	task->SetUID(guid);
	task->SetStatusNoSync(status);
	task->DecodeQuestVar(parameter);
	task->SetAcceptTime(acceptTime);
	task->SetDBFlag(df_none);

	m_ActiveTasks.insert(std::make_pair(dataId, task));
}

void ActiveTaskMgr::OnActiveDayChange()
{
	UInt32 day = GetOwner()->GetCreateFromNowOnDay();

	//更新已有七日任务
	ActivityMap::iterator itr = m_ActiveTasks.begin();
	while (itr != m_ActiveTasks.end())
	{
		ActiveTask* task = itr->second;
		if (!task)
		{
			++itr;
			continue;
		}
			
		if (IsSevenDay(task->GetDataEntry()->templateID))
		{
			//七日活动结束
			if (day > SEVEN_CLOSE_DAY)
			{
				//_OnSevenDayClose(task);

				CL_SAFE_DELETE(task);
				itr = m_ActiveTasks.erase(itr);
				continue;
			}
			else if (day > SEVEN_END_DAY && (task->GetStatus() != TASK_FINISHED && task->GetStatus() != TASK_OVER))
			{
				//七日活动只能领取不能做
				task->SetStatusNoSync(TASK_FAILED);
 			}
		}
		else if (AID_CHARGE_DAY == task->GetDataEntry()->templateID)
		{
			if (task->GetStatus() == TASK_FINISHED)
			{
				SendUnreciveBonusToMail(task);
			}

			task->SetStatus(TASK_UNFINISH);
			task->SetVar("v", 0);
			task->SaveToDB();
		}

		++itr;
	}

	//接受今日的七日任务
	switch (day)
	{
	case 2:	AcceptTask(AID_TWO_DAY);		break;
	case 3: AcceptTask(AID_THREE_DAY);	break;
	case 4: AcceptTask(AID_FOUR_DAY);	break;
	case 5: AcceptTask(AID_FIVE_DAY);	break;
	case 6: AcceptTask(AID_SIX_DAY);		break;
	case 7: AcceptTask(AID_SEVEN_DAY);	break;
	default:
		break;
	}
}

void ActiveTaskMgr::SaveToDB()
{
	for (ActivityMap::iterator itr = m_ActiveTasks.begin(); itr != m_ActiveTasks.end(); ++itr)
	{
		ActiveTask* task = itr->second;
		if (!task)
			continue;

		task->SaveToDB();
	}
}

void ActiveTaskMgr::OnEventBindPhone(IEventParam* param)
{
	InfoLogStream << PLAYERINFO(GetOwner()) << " on bind phone..." << LogStream::eos;

	_UpdateBindPhoneOpActTasks();
}

void ActiveTaskMgr::OnAppointmentActivityClose()
{
	std::vector<OpActTaskData*> taskDatas;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_APPOINTMENT_OCCU, taskDatas);

	for (auto taskData : taskDatas)
	{
		if (!taskData)
		{
			continue;
		}

		OpActTask* task = FindNewOpActTask(taskData->dataId);
		if (!task)
		{
			continue;
		}

		if (task->state != TASK_FINISHED)
		{
			continue;
		}

		task->curNum = taskData->completeNum;
		task->state = TASK_OVER;

		//同步DB
		UpdateOpTaskDB(task);

		//同步客户端
		CLProtocol::SCSyncOpActTaskChange protocol;
		protocol.tasks.push_back(*task);
		GetOwner()->SendProtocol(protocol);

		UInt32 dataId = task->dataId;
		UInt32 opActId = dataId / 1000;
		std::string reason = GetReason(SOURCE_TYPE_TASK_ACTIVE, opActId, dataId);

		RewardGroup group;
		for (auto& reward : taskData->rewards)
		{
			auto itemData = ItemDataEntryMgr::Instance()->FindEntry(reward.id);
			if (!itemData || itemData->subType == ST_APPOINTMENT_COIN)
			{
				// 活动结束，就不加预约币了
				continue;
			}

			group.AddReward(reward.id, reward.num, reward.qualityLv, reward.strenth);
		}

		if (group.GetRewards().size() == 0)
		{
			continue;
		}

		// 奖励通过邮件发送
		std::string sender = SysNotifyMgr::Instance()->MakeString(9501);
		std::string title = SysNotifyMgr::Instance()->MakeString(9502);
		std::string content = SysNotifyMgr::Instance()->MakeString(9504);
		
		Sys::SendSysMail(GetOwner()->GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), &group);
	}
}

void ActiveTaskMgr::OnUsedHellTicket(UInt32 usedNum)
{
	_UpdateHellTicketOpActTasks(usedNum);

	// 回归使用深渊票
	ReturnUseHellTicket(usedNum);

	OnHireTask(HTT_SHENG_YUAN_PIAO, usedNum);
}

void ActiveTaskMgr::OnUsedYuanGuTicket(UInt32 usedNum)
{
	OnHireTask(HTT_YUAN_GU_PIAO, usedNum);
}

void ActiveTaskMgr::OnRemoveFatigue(UInt16 usedFatigue)
{
	if (usedFatigue == 0) return;

	_OnRemoveFatigueForFatigueBuffOpAct(usedFatigue);

	_OnRemoveFatigueForFatigueTokenOpAct(usedFatigue);

	_OnMagpieBridgeOpTaskRmFatigue(usedFatigue);

	NewYear2020(NEW_YEAR_2020_ENERGY, usedFatigue);
}

void ActiveTaskMgr::OnOpItemSumChange(UInt32 itemId)
{
	OnOpItemChangeFashionExchange(itemId);
	OnOpItemChangeFashionWelfare(itemId);
}

void ActiveTaskMgr::OnOpChangeFashion(UInt32 itemId)
{
	OnOpItemChangeFashionExchange(itemId);
}

void ActiveTaskMgr::OnEventCallback(IEventParam* param)
{
	if (!param)
	{
		return;
	}

	//优先特殊处理，根据活动类型
	OnPlayerClearDungeonForOpActTasks(param);

	//后续通用处理，根据任务类型
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTask([&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return true;

		if (param->_eventType != (CLEventMgr::EventType)taskData.eventType) return true;

		if (!task) return true;

		if (_CheckAndUpdateTask(opAct, taskData, task, param))
		{
			SaveOpTaskDB(task, taskData.accountTask);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool ActiveTaskMgr::_CheckAndUpdateTask(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param)
{
	if (TASK_UNFINISH != task->state)
	{
		return false;
	}

	bool ret = false;

	switch ((CLEventMgr::EventType)taskData.eventType)
	{
	case CLEventMgr::ET_CLEAR_DUNGEON:
		ret = _CheckAndUpdateTaskOnClearDungeon(opAct, taskData, task, param);
		break;
	case CLEventMgr::ET_COMPLETE_PK:
		ret = _CheckAndUpdateTaskOnCompletePk(opAct, taskData, task, param);
		break;
	case CLEventMgr::ET_ENTER_DUNGEON:
		ret = _CheckAndUpdateTaskOnEnterDungeon(opAct, taskData, task, param);
		break;
	case CLEventMgr::ET_TASK_FINISHED:
		ret = _CheckAndUpdateTaskOnTaskFinished(opAct, taskData, task, param);
		break;
	case CLEventMgr::ET_ON_ONLINE:
		ret = _CheckAndUpdateTaskOnOnline(opAct, taskData, task, param);
		break;
	default:
		break;
	}

	if (ret)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << " update " << task->LogStr() << LogStream::eos;
	}
	
	return ret;
}

void ActiveTaskMgr::_SendOpActTaskFinishedEvent(UInt32 taskId)
{
	TaskFinishedParam param(taskId);
	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::ET_TASK_FINISHED, &param);
}

void ActiveTaskMgr::_SendOpActOnlineEvent(bool dayChange)
{
	OnlineEventParam param;
	param._isDayChange = dayChange;
	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::EventType::ET_ON_ONLINE, &param);
}

bool ActiveTaskMgr::_UpdateTaskBySubEventType(const OpActTaskData& taskData, OpActTask* task)
{
	UInt32 curTime = Avalon::Time::CurrentTime().Sec();
	UInt32 updateTime = task->GetParamVar(OPACT_UPDATETIME_VAR_KEY);

	bool ret = false;
	switch ((CLEventMgr::SubEventType)taskData.subType)
	{
	case CLEventMgr::SUB_ET_ADD_ONE_DAILY:
	case CLEventMgr::SUB_ET_DRAW_PRIZE:
		//特殊子类型，每天加1
		if (!CheckInSameLogicDay(curTime, updateTime) && task->AddCurNum(1, taskData.completeNum, true))
		{
			ret = true;
			task->SetParamVar(OPACT_UPDATETIME_VAR_KEY, curTime);
		}
		break;

	case CLEventMgr::SUB_ET_ADD_ONE_WEEKLY:
		//特殊子类型，每周加1
		if (!isSameLogicWeek(curTime, updateTime) && task->AddCurNum(1, taskData.completeNum, true))
		{
			ret = true;
			task->SetParamVar(OPACT_UPDATETIME_VAR_KEY, (curTime));
		}
		break;

	case CLEventMgr::SUB_ET_TRIGGER_OTHER_TASK:
		//特殊子类型，完成后触发其他活动任务
		ret = task->AddCurNum(1, taskData.completeNum, true);
		if (ret && TASK_FINISHED == task->state)
		{
			//发送完成任务事件
			_SendOpActTaskFinishedEvent(task->dataId);
		}
		break;

	case CLEventMgr::SUB_ET_SPECIAL_CALL_FUNC:
		//特殊子类型，调用函数（比如挑战者活动等等）
		ret = task->AddCurNum(1, taskData.completeNum, true);
		if (ret && TASK_FINISHED == task->state)
		{
			//发送完成任务事件
			_SendOpActTaskFinishedEvent(task->dataId);

			//触发挑战者活动
			OnChallengeActivityTask(OACT_EVERYDAT_CHALLENGE, false);
		}
		break;

	default:
		//默认子类型，自动加1
		ret = task->AddCurNum(1, taskData.completeNum, true);
		break;
	}

	return ret;
}

bool ActiveTaskMgr::_CheckAndUpdateTaskOnClearDungeon(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param)
{
	CompleteDungeonParam* myParam = (CompleteDungeonParam*)param;
	if (!myParam)
	{
		return false;
	}

	const DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(myParam->_dungeonId);
	if (!dungeonData)
	{
		return false;
	}

	// variables用来标记适用地下城类型
	if (!taskData.variables.empty())
	{
		if (std::find(taskData.variables.begin(), taskData.variables.end(), (UInt32)dungeonData->subType) == taskData.variables.end())
		{
			return false;
		}
	}

	// variables2用来标记适用地下城ID
	if (!taskData.variables2.empty())
	{
		if (std::find(taskData.variables2.begin(), taskData.variables2.end(), (UInt32)dungeonData->id) == taskData.variables2.end())
		{
			return false;
		}
	}

	// 推荐地下城
	if (taskData.variables.empty() && taskData.variables2.empty())
	{
		if (!WeekSignSpringDataEntryMgr::Instance()->IsInScope(GetOwner()->GetLevel(), dungeonData->tag))
		{
			return false;
		}
	}

	return _UpdateTaskBySubEventType(taskData, task);
}

bool ActiveTaskMgr::_CheckAndUpdateTaskOnCompletePk(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param)
{
	CompletePkParam* pkParam = (CompletePkParam*)param;
	if (!pkParam)
	{
		return false;
	}

	// variables用来标记适用PK类型，枚举类型PkType，1表示普通1V1
	if (!taskData.variables.empty())
	{
		if (std::find(taskData.variables.begin(), taskData.variables.end(), pkParam->_pkType) == taskData.variables.end())
		{
			return false;
		}
	}

	return _UpdateTaskBySubEventType(taskData, task);
}

bool ActiveTaskMgr::_CheckAndUpdateTaskOnEnterDungeon(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param)
{
	EnterDungeonParam* myParam = (EnterDungeonParam*)param;
	if (!myParam)
	{
		return false;
	}

	const DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(myParam->_dungeonId);
	if (!dungeonData)
	{
		return false;
	}

	// variables用来标记适用地下城类型
	if (!taskData.variables.empty())
	{
		if (std::find(taskData.variables.begin(), taskData.variables.end(), (UInt32)dungeonData->subType) == taskData.variables.end())
		{
			return false;
		}
	}

	// variables2用来标记适用地下城ID
	if (!taskData.variables2.empty())
	{
		if (std::find(taskData.variables2.begin(), taskData.variables2.end(), (UInt32)dungeonData->id) == taskData.variables2.end())
		{
			return false;
		}
	}

	// 推荐地下城
	if (taskData.variables.empty() && taskData.variables2.empty())
	{
		if (!WeekSignSpringDataEntryMgr::Instance()->IsInScope(GetOwner()->GetLevel(), dungeonData->tag))
		{
			return false;
		}
	}

	return _UpdateTaskBySubEventType(taskData, task);
}

bool ActiveTaskMgr::_CheckAndUpdateTaskOnTaskFinished(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param)
{
	TaskFinishedParam* myParam = (TaskFinishedParam*)param;
	if (!myParam)
	{
		return false;
	}

	// variables用来标记适用的任务ID
	if (taskData.variables.empty())
	{
		return false;
	}

	if (std::find(taskData.variables.begin(), taskData.variables.end(), (UInt32)myParam->_taskId) == taskData.variables.end())
	{
		return false;
	}

	return _UpdateTaskBySubEventType(taskData, task);
}

bool ActiveTaskMgr::_CheckAndUpdateTaskOnOnline(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param)
{
	OnlineEventParam* myParam = (OnlineEventParam*)param;
	if (!myParam)
	{
		return false;
	}

	return _UpdateTaskBySubEventType(taskData, task);
}

void ActiveTaskMgr::OnPlayerClearDungeonForOpActTasks(IEventParam* param)
{
	if (!param)
	{
		return;
	}

	if (param->_eventType != CLEventMgr::ET_CLEAR_DUNGEON)
	{
		return;
	}

	CompleteDungeonParam* myParam = (CompleteDungeonParam*)param;
	if (!myParam)
	{
		return;
	}

	const DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(myParam->_dungeonId);
	if (!dungeonData)
	{
		return;
	}

	_OnPlayerClearDungeonForDungeonPassedCumulateDayOpAct(dungeonData);

	if (!dungeonData->IsHellDungeon() && !dungeonData->IsHellEntry())
	{
		OnWeekSignSpring(dungeonData->tag);
	}

	OnChallengeActivity(dungeonData, myParam->_teammateIds);
}

void ActiveTaskMgr::OnOpItemChangeFashionExchange(UInt32 itemId)
{
	std::vector<OpActivityRegInfo*> opActs;
	ActivityMgr::Instance()->GetOpActDataByTmpType(OAT_CHANGE_FASHION_EXCHANGE, opActs);
	for (auto opAct : opActs)
	{
		if (!opAct) continue;

		auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opAct->dataId);
		if (playerOpAct != NULL)
		{
			opAct = playerOpAct;
		}
		if (!IsMeetOpActRequireByPlayer(opAct))
		{
			continue;
		}
		if (opAct->state != AS_IN)
		{
			continue;
		}

		_UpdateChangeFashionExchangeOpActTasks(opAct);
	}
}

void ActiveTaskMgr::OnOpItemChangeFashionWelfare(UInt32 itemId)
{
	std::vector<OpActivityRegInfo*> opActs;
	ActivityMgr::Instance()->GetOpActDataByTmpType(OAT_CHANGE_FASHION_WELFARE, opActs);
	for (auto opAct : opActs)
	{
		if (!opAct) continue;

		auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opAct->dataId);
		if (playerOpAct != NULL)
		{
			opAct = playerOpAct;
		}
		if (!IsMeetOpActRequireByPlayer(opAct))
		{
			continue;
		}
		if (opAct->state != AS_IN)
		{
			continue;
		}

		_UpdateChangeFashionWelfareOpActTasks(opAct);
	}
}

void ActiveTaskMgr::OnLoadOpActTasks(const std::string& taskInfoStr)
{
	if (taskInfoStr.empty())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Delete op task db, taskInfoStr is empty!" << LogStream::eos;
		return;
	}

	// 整合旧数据
	std::ostringstream newTaskInfoOss;
	{
		std::vector<std::string> allTaskInfo;
		Avalon::StringUtil::Split(taskInfoStr, allTaskInfo, ";");
		for (auto elem : allTaskInfo)
		{
			if (!newTaskInfoOss.str().empty())
			{
				newTaskInfoOss << ";";
			}

			std::vector<std::string> oneTaskInfo;
			Avalon::StringUtil::Split(elem, oneTaskInfo, ",");
			if (oneTaskInfo.size() >= 3)
			{
				newTaskInfoOss << oneTaskInfo[0] << "," << oneTaskInfo[1] << "," << oneTaskInfo[2];
			}
			else if (oneTaskInfo.size() == 2)
			{
				newTaskInfoOss << oneTaskInfo[0] << "," << oneTaskInfo[1] << "," << (UInt32)TASK_INIT;
			}
			else
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Error task info size(" << (UInt32)oneTaskInfo.size() << ")!" << LogStream::eos;
			}
		}
	}

	std::istringstream is(newTaskInfoOss.str());

	DecodeOpString(is);
}

void ActiveTaskMgr::OnLoadNewOpActTasks(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		ObjID_t uid = callback->GetKey();
		UInt32 taskId = callback->GetData("task_id");
		UInt32 create_time = callback->GetData("create_time");

		UInt32 opActId = taskId / OPACT_ID_TRANS_VALUE;
		auto opActData = ActivityMgr::Instance()->GetOpActData(opActId);
		if (!opActData)
		{
			continue;
		}
		
		switch ((OpActivityType)opActData->tmpType)
		{
		case OAT_DAY_LOGIN:
		case OAT_DAILY_REWARD:
		case OAT_CHAMPION_GIFT:
		case OAT_FATIGUE_FOR_TOKEN_COIN:
		{
			//活动是否还开启
			if (!ActivityMgr::Instance()->IsInOpActivity(opActId))
			{
				continue;
			}

			OpActivityRegInfo* opAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opActId);

			if (opAct != NULL)
			{
				if (create_time < opAct->startTime || create_time > opAct->endTime)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << uid << ", " << taskId << ")'s create time(" << create_time
						<< ") reach limit(" << opAct->startTime << ", " << opAct->endTime << ")!" << LogStream::eos;
					continue;
				}
			}
			break;
		}

		default:
			break;
		}

		if (FindNewOpActTask(taskId) != NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " repeat task(" << uid << ", " << taskId << ")!" << LogStream::eos;
			continue;
		}

		OpActTask* opTask = new OpActTask();
		opTask->uid = uid;
		opTask->ownerId = callback->GetData("owner_id");
		opTask->dataId = taskId;
		opTask->state = callback->GetData("status");
		{
			std::string parmStr = callback->GetData("parameter").GetString();
			std::vector<std::string> strVec;
			Avalon::StringUtil::Split(parmStr, strVec, ",");
			for (const auto& elem : strVec)
			{
				std::vector<std::string> strVec2;
				Avalon::StringUtil::Split(elem, strVec2, "=");
				if (strVec2.size() != 2)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << opTask->LogStr() << " parm format is error!" << LogStream::eos;
					continue;
				}

				if (strVec2[0] == OPACT_COMMON_VAR_KEY)
				{
					opTask->curNum = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
				}
				else{
					opTask->params[strVec2[0]] = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
				}
			}
		}

		if (!AddNewOpActTask(opTask))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Add " << opTask->LogStr() << ") failed!" << LogStream::eos;
			delete opTask;
		}
	}

	return;
}

void ActiveTaskMgr::MixOpActTasks()
{
	for (auto oldTask : m_OldOpActTaskMap)
	{
		if (!oldTask) continue;

		auto opActTaskData = ActivityMgr::Instance()->GetOpActTaskData(oldTask->dataId);
		if (!opActTaskData)
		{
			delete oldTask;
			continue;
		}

		if (FindNewOpActTask(oldTask->dataId) != NULL)
		{
			delete oldTask;
			continue;
		}

		AddNewOpActTask(oldTask);
		InsertOpTaskDB(oldTask);

		InfoLogStream << PLAYERINFO(GetOwner()) << " add new opAct's task(" << oldTask->dataId << ")." << LogStream::eos;
	}

	m_OldOpActTaskMap.clear();
}

OpActTask* ActiveTaskMgr::FindNewOpActTask(UInt32 taskId, UInt32 isAccountTask)
{
	if (isAccountTask != 0)
	{
		return FindOpActAccountTask(taskId);
	}

	auto iter = m_idToOpActTask.find(taskId);
	if (iter == m_idToOpActTask.end())
	{
		return NULL;
	}

	return iter->second;
}

bool ActiveTaskMgr::AddNewOpActTask(OpActTask* task, UInt32 accountTask)
{
	if (accountTask != 0)
	{
		return AddOpActAccountTask(task);
	}

	if (!task)
	{
		return false;
	}

	m_idToOpActTask.insert(std::make_pair(task->dataId, task));
	return true;
}

void ActiveTaskMgr::DelOpActTask(UInt32 taskDataId)
{
	auto iter = m_idToOpActTask.find(taskDataId);
	if (iter == m_idToOpActTask.end())
	{
		return;
	}

	m_idToOpActTask.erase(iter);
}

void ActiveTaskMgr::OnTakeOpActTaskReward(UInt32 opActDataId, UInt32 dataId, UInt64 param)
{
	OpActivityRegInfo* opActData = ActivityMgr::Instance()->GetOpActData(opActDataId);
	if (!opActData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find OpActivityRegInfo(" << opActDataId << ")!" << LogStream::eos;
		return;
	}

	OpActTaskData* taskData = ActivityMgr::Instance()->GetOpActTaskData(dataId);
	if (!taskData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data(" << dataId << ")!" << LogStream::eos;
		return;
	}

	if (taskData->accountTask != 0)
	{
		OnTakeOpActAccountTaskReward(opActDataId, dataId, param);
		return;
	}

	auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opActDataId);
	if (playerOpAct != NULL)
	{
		opActData = playerOpAct;
	}

	if (opActData->state != AS_IN)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpActivity(" << opActDataId << ") was not open!" << LogStream::eos;
		GetOwner()->SendNotify(2200008);
		return;
	}

	if (!IsMeetOpActRequireByPlayer(opActData))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Not meet OpAct(" << opActData->dataId << ")'s requirement!" << LogStream::eos;
		return;
	}

	OpActTask* task = FindNewOpActTask(dataId);
	if (!task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << dataId << ")!" << LogStream::eos;
		return;
	}

	// 检测账号领奖次数限制
	if (!_CheckOpTaskAccountSubmitLimit(taskData))
	{
		return;
	}

	// 转盘抽奖，角色任务
	if (CLEventMgr::SUB_ET_DRAW_PRIZE == taskData->subType && TASK_FINISHED == task->state && 1 == taskData->rewards.size())
	{
		//更新任务状态
		task->state = TASK_OVER;

		//同步DB
		UpdateOpTaskDB(task);

		//同步客户端
		CLProtocol::SCSyncOpActTaskChange protocol;
		protocol.tasks.push_back(*task);
		GetOwner()->SendProtocol(protocol);

		//抽奖
		GetOwner()->DrawPrizeForOpActivity(taskData->rewards[0].id);

		// 领完奖了，添加账号计数
		_UpdateOpTaskAccountSubmitTimes(taskData);
		return;
	}

	bool bGiveReward = true;
	bool bSubmitSuccess = true;

	switch ((OpActivityType)opActData->tmpType)
	{
	case OAT_PLAYER_LEVEL_UP:
	case OAT_APPOINTMENT_OCCU:
		if (!_OnSubmitOpActCheckMaoxianheijReward(opActData, task, taskData))
		{
			return;
		}
		task->curNum = taskData->completeNum;
		task->state = TASK_OVER;
		break;

	case OAT_FATIGUE_FOR_TOKEN_COIN:
		if (task->state != TASK_FINISHED)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << (UInt8)TASK_FINISHED << ")!" << LogStream::eos;
			return;
		}
		if (!_OnSubmitFatigueToken(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		break;

	case OAT_FATIGUE_BURNING:
		if (!_OnSubmitFatigueBurning(task, taskData))
		{
			bSubmitSuccess = false;
		}
		bGiveReward = false;
		break;

	case OAT_CHANGE_FASHION_WELFARE:
		if (!_OnSubmitChangeFashionWelfareOpActTask(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		break;

	case OAT_CHANGE_FASHION_EXCHANGE:
		if (!_OnSubmitChangeFashionExchangeOpActTask(opActData, task, taskData, param))
		{
			bSubmitSuccess = false;
		}
		break;
	case OAT_RETURN_PRESENT:
		if (!_OnSubmitReturnPresent(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		bGiveReward = false;
		break;
	case OAT_ENERGY_EXCHANGE:
		if (!_OnSubmitEnergyShopExchangeItem(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		bGiveReward = false;
		break;
	case OAT_RETURN_CHARGE_REBATE:
		if (!_OnSubmitReturnChargeRebate(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		bGiveReward = false;
		break;
	case OAT_CHALLENGE_HELL:
		if (!_OnSubmitChallengeHell(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		bGiveReward = false;
		break;
	case OAT_WEEK_SIGN_NEW_PLAYER:
	case OAT_WEEK_SIGN_ACTIVITY:
		if (!_OnSubmitWeekSign(opActData, task, taskData))
		{
			bSubmitSuccess = false;
		}
		bGiveReward = false;
		break;
	case OAT_GNOME_COIN_GIFT:
		if (task->state != TASK_FINISHED)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << (UInt8)TASK_FINISHED << ")!" << LogStream::eos;
			return;
		}
		task->state = TASK_OVER;
		break;

	case OAT_GIFT_RIGHT:
		if (task->state != TASK_FINISHED)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << (UInt8)TASK_FINISHED << ")!" << LogStream::eos;
			return;
		}
		if (!GetOwner()->HaveGiftRight())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "request get reward but don't have right card(" << taskData->dataId << ")!" << LogStream::eos;
			return;
		}
		if (taskData->variables.size() != 1)
		{
			return;
		}
		if (taskData->variables[0] == OAGRTT_LEND_EQUIP)
		{
			if (taskData->variables2.size() != 1)
			{
				return;
			}
			if (GetOwner()->GetLevel() < taskData->variables2[0])
			{
				return;
			}
		}
		task->curNum = taskData->completeNum;
		task->state = TASK_OVER;
		break;
	default:
		if (task->state != TASK_FINISHED)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << (UInt8)TASK_FINISHED << ")!" << LogStream::eos;
			return;
		}
		task->curNum = taskData->completeNum;
		task->state = TASK_OVER;
		break;
	}

	//同步DB
	UpdateOpTaskDB(task);

	//同步客户端
	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	InfoLogStream << PLAYERINFO(GetOwner()) << " submit " << task->LogStr() << ", bSubmitSuccess(" << (bSubmitSuccess ? 1 : 0) 
		<< "), bGiveReward(" << (bGiveReward ? 1 : 0) << ")." << LogStream::eos;

	if (!bSubmitSuccess) return;

	// 领完奖了，添加账号计数
	_UpdateOpTaskAccountSubmitTimes(taskData);

	if (bGiveReward)
	{
		RewardGroup* group = m_pOwner->GetRewardGroup();
		group->AddRewards(taskData->rewards);
		m_pOwner->AddRewards(GetReason(SOURCE_TYPE_TASK_ACTIVE, opActDataId, dataId).c_str(), group, true);
	}

	switch ((OpActivityType)opActData->tmpType)
	{
	case OAT_DUNGEON_DROP_ACTIVITY:
	case OAT_DUNGEON_EXP_ADDITION:
	case OAT_PVP_PK_COIN:
	case OAT_DUNGEON_DROP_RATE_ADDITION:
	{
		// 是否为可接任务
		if (taskData->acceptType == OATAT_ACCEPT_CANT)
		{
			break;
		}

		if (task->state != TASK_OVER)
		{
			break;
		}

		auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(task->dataId);
		if (!taskDataEntry || taskDataEntry->nextTaskId == 0)
		{
			break;
		}

		auto nextTask = FindNewOpActTask(taskDataEntry->nextTaskId);
		if (nextTask != NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " task(" << task->dataId << ")'s next task(" <<
				taskDataEntry->nextTaskId << ") is already exist!" << LogStream::eos;
		}
		else
		{
			nextTask = new OpActTask(taskDataEntry->nextTaskId);
			AddNewOpActTask(nextTask);
		}

		nextTask->state = TASK_UNFINISH;
		SaveOpTaskDB(nextTask);

		CLProtocol::SCSyncOpActTaskChange protocol;
		protocol.tasks.push_back(*nextTask);
		GetOwner()->SendProtocol(protocol);

		InfoLogStream << PLAYERINFO(GetOwner()) << " accpet task(" << task->dataId << ")'s next task(" 
			<< nextTask->dataId << ")." << LogStream::eos;
		break;
	}
	default:break;
	}
}

void ActiveTaskMgr::OnOpActTasksReq(UInt32 dataId, std::vector<OpActTask>& tasks)
{
	auto opActInfo = ActivityMgr::Instance()->GetOpActData(dataId);
	if (!opActInfo)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find opAct(" << dataId << ")!" << LogStream::eos;
		return;
	}

	for (auto taskData : opActInfo->tasks)
	{
		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data(" << taskData.dataId << ")!" << LogStream::eos;
			continue;
		}

		tasks.push_back(*task);
	}
}

void ActiveTaskMgr::OnOpActOpen(const OpActivityRegInfo* info)
{
	if (!info) return;

	_AcceptOpActivity(info);

	switch ((OpActivityType)info->tmpType)
	{
	case OAT_DAY_LOGIN:
		// 更新每日登录
		_UpdateDailyLoginOp();
		break;

	case OAT_DAILY_BUFF:
		// 更新每日buff
		_UpdateDailyBuffOpActTask();
		break;

	case OAT_SECOND_ANNIVERSARY_PRAY:
		// 更新周年祈福
		_UpdateSecondAnniversaryPrayOpActTask();
		break;

	case OAT_CUMULATE_ONLINE:
	case OAT_HALLOWEEN_PUMPKIN_HELMET:
		// 更新累计在线
		_UpdateCumulateOnlineOpActTask();
		break;
	case OAT_ONLINE_GIFT:
		_UpdateOnlineGiftOpActTask();
		break;

	default:
		break;
	}
}

void ActiveTaskMgr::OnOpActClosed(const OpActivityRegInfo* info)
{
	if (!info)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpActivityRegInfo ptr is null!" << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << info->LogStr() << " close, refresh data!" << LogStream::eos;

	if (info->tmpType == OAT_HELL_TICKET_FOR_DRAW_PRIZE)
	{
		std::string ticketCountName = Avalon::StringUtil::ConvertToString(info->tmpType) + OPACT_COST_HELL_TICKETS;
		GetOwner()->SetCounter(ticketCountName.c_str(), 0);

		std::string usedDrawCntName = Avalon::StringUtil::ConvertToString(info->tmpType) + OPACT_USED_DARW_COUNT;
		GetOwner()->SetCounter(usedDrawCntName.c_str(), 0);

		std::string drawCountName = Avalon::StringUtil::ConvertToString(info->tmpType) + OPACT_DARW_PRIZE_NUM;
		GetOwner()->SetCounter(drawCountName.c_str(), 0);
	}
	else if (info->tmpType == OAT_FATIGUE_FOR_BUFF)
	{
		for (auto data : info->tasks)
		{
			if (data.variables.size() != 1)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "OpActivityTask(" << data.dataId << ") data's var format was error!" << LogStream::eos;
				continue;
			}

			UInt32 buffId = data.variables.front();
			GetOwner()->GetBuffMgr().RemoveBuff(buffId);
		}
	}
	else if (info->tmpType == OAT_FATIGUE_FOR_TOKEN_COIN)
	{
		std::string fatigueCountName = Avalon::StringUtil::ConvertToString(info->dataId) + OPACT_COST_FATIGUE;
		GetOwner()->SetCounter(fatigueCountName.c_str(), 0);

		std::string validTokensCountName = Avalon::StringUtil::ConvertToString(info->dataId) + OPACT_FATIGUE_TOKENS;
		GetOwner()->SetCounter(validTokensCountName.c_str(), 0);
	}
	else if (info->tmpType == OAT_CHALLENGE_HUB)
	{
		GetOwner()->SetCounter(OPACT_CHALLENGE_SCORE, 0);
		GetOwner()->SetCounter(OPACT_CHALLENGE_TOTAL_SCORE, 0);
		GetOwner()->SetCounter(OPACT_CHALLENGE_DAY_SCORE, 0);
	}
	else if (info->tmpType == OAT_CHALLENGE_HUB_SCORE)
	{

	}
	else if (info->tmpType == OAT_SPRING_CHALLENGE)
	{
		GetOwner()->SetCounter(OPACT_SPRING_SCORE, 0);
	}

	else if (info->tmpType == OAT_SPRING_CHALLENGE_SCORE)
	{
		GetOwner()->SetCounter(OPACT_SPRING_TOTAL_SCORE, 0);
	}
	else if (info->tmpType == OAT_WEEK_SIGN_SPRING)
	{
		GetOwner()->SetCounter(OPACT_WEEK_SIGN_SPRING_NUM, 0);
	}
	else if (info->tmpType == OAT_GNOME_COIN_GIFT)
	{
		OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_GNOMECOIN_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), info->dataId, 0);
	}
	else if (info->tmpType == OAT_BUFF_ADDITION)
	{
		for (auto buffId : info->parm2)
		{
			GetOwner()->GetBuffMgr().RemoveBuff(buffId);
		}
	}
	else if (info->tmpType == OAT_DUNGEON_RANDOM_BUFF)
	{
		for (auto buffId : info->parm2)
		{
			GetOwner()->GetBuffMgr().RemoveOpActBuff(info->dataId, buffId);
		}
	}
	else if (info->tmpType == OAT_CHANGE_FASHION_ACT)
	{
		std::string scoreCountName = ACTIVE_SCORE_PER + Avalon::StringUtil::ConvertToString(info->parm);
		GetOwner()->SetCounter(scoreCountName.c_str(), 0);

		std::string consumeCountName = ACTIVE_SCORE_CONSUME_PER + Avalon::StringUtil::ConvertToString(info->parm);
		GetOwner()->SetCounter(consumeCountName.c_str(), 0);
	}
	else if (info->tmpType == OAT_HALLOWEEN_PUMPKIN_HELMET)
	{
		std::string usedDrawCntName = Avalon::StringUtil::ConvertToString(info->tmpType) + OPACT_USED_DARW_COUNT;
		GetOwner()->SetCounter(usedDrawCntName.c_str(), 0);

		std::string drawCountName = Avalon::StringUtil::ConvertToString(info->tmpType) + OPACT_DARW_PRIZE_NUM;
		GetOwner()->SetCounter(drawCountName.c_str(), 0);
	}
	else if (info->tmpType == OAT_SPRING_FESTIVAL_RED_PACKET_RECV)
	{
		std::string scoreCountName = ACTIVE_SCORE_PER + Avalon::StringUtil::ConvertToString(info->dataId);
		GetOwner()->SetCounter(scoreCountName.c_str(), 0);
	}
	else if (info->tmpType == OAT_GIFT_RIGHT)
	{
		GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD, 0);
		GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD_BUY_TIME, 0);
		GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD_LOGIN_TOTAL, 0);
	}
}

bool ActiveTaskMgr::IsMeetOpActRequireByPlayer(const OpActivityRegInfo* info)
{
	if (!info)
	{
		return false;
	}

	if (info->playerLevelLimit > 0 && GetOwner()->GetLevel() < info->playerLevelLimit)
	{
		return false;
	}

	return true;
}

void ActiveTaskMgr::GetOpActivityTaskAddition(OpActivityType type, DungeonAddition& addition, const DungeonDataEntry* dungeonData)
{
	InfoLogStream << PLAYERINFO(GetOwner()) << "Start add addition, opAct(" << (UInt32)type << ")." << LogStream::eos;

	if (!dungeonData)
	{
		return;
	}

	if (type == OAT_FATIGUE_BURNING)
	{
		if (dungeonData->threeType == DTT_TEAM_ELITE)
		{
			return;
		}
		// 获取在开放的活动(只能有一个)
		std::vector<OpActivityRegInfo*> infos = ActivityMgr::Instance()->GetOpActDataByTmpTypes((UInt32)type);
		if (infos.size() != 1)
		{
			return;
		}

		auto opActInfo = infos.front();
		if (!opActInfo) return;

		if (!IsMeetOpActRequireByPlayer(opActInfo))
		{
			return;
		}

		UInt32 fatigue = (UInt32)GetOwner()->GetFatigue();
		if (fatigue <= opActInfo->parm)
		{
			return;
		}

		class FatigueBurnBuffVisitor : public BuffVisitor
		{
		public:
			FatigueBurnBuffVisitor() {}

			bool operator()(Buff* buff)
			{
				if (!buff) return true;

				if (buff->GetDataEntry()->buffType == (Int16)BT_FATIGUE_BURN)
					fatigueBurnBuffs.push_back(buff);

				return true;
			}

			std::vector<Buff*> fatigueBurnBuffs;
		};

		FatigueBurnBuffVisitor visitor;
		GetOwner()->GetBuffMgr().VisitBuffs(visitor);

		for (auto buff : visitor.fatigueBurnBuffs)
		{
			if (!buff) continue;

			addition.additionRate[DART_PINK_DROP_RATE] += buff->GetDataEntry()->pinkDropRate;
			addition.dropAddition[DIT_CHEST] += buff->GetDataEntry()->chestDropRate;
			addition.additionRate[DART_FATIGUE_BURNING_RATE] += 100;

			InfoLogStream << PLAYERINFO(GetOwner()) << "Add addition by fatigue burn buff(" <<
				buff->GetId() << ")." << LogStream::eos;
		}
	}
}

void ActiveTaskMgr::GetOpActivityTaskAddition(const DungeonDataEntry* dungeonData, DungeonAddition& addition)
{
	if (!dungeonData) return;

	VisitOpenOpActAndTaskByOpType(OAT_DAILY_BUFF, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task){
		if (!opAct || !task) return true;

		if (task->state != TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables.size() != 1)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " task(" << task->dataId << ")'s data variable size is error!" << LogStream::eos;
			return true;
		}

		UInt32 type = taskData.variables.front();

		if (type != PrayActivityBuffType::PABT_DUNGEON_DORP_ADD)
		{
			return true;
		}

		if (taskData.rewards.size() != taskData.variables2.size())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " task(" << task->dataId << ")'s data variable2 size is not match rewards size!" << LogStream::eos;
			return true;
		}

		for (UInt32 idx = 0; idx != taskData.variables2.size(); ++idx)
		{
			UInt32 dungeonSubType = taskData.variables2[idx];

			if (dungeonData->subType != dungeonSubType)
			{
				continue;
			}

			auto taskAdditionInfo = taskData.rewards[idx];

			switch ((DropItemType)taskAdditionInfo.id)
			{
			case DIT_BIND_GOLD:
				addition.additionRate[DART_BIND_GOLD_RATE] += taskAdditionInfo.num;
				break;

			case DIT_BERYL:
				addition.additionRate[DART_BERYL_RATE] += taskAdditionInfo.num;
				break;

			default:
				ErrorLogStream << PLAYERINFO(GetOwner()) << " drop item type(" << taskAdditionInfo.id << ") is error!" << LogStream::eos;
				continue;
			}

			DebugLogStream << PLAYERINFO(GetOwner()) << " task(" << task->dataId << ") add drop addition(" << taskAdditionInfo.id 
				<< ", " << taskAdditionInfo.num << ") to dungeon(" << dungeonData->id << ")" << LogStream::eos;
		}

		return true;
	});

	VisitOpenOpActAndTaskByOpType(OAT_SECOND_ANNIVERSARY_PRAY, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct || !task) return true;

		if (task->state != TASK_FINISHED)
		{
			return true;
		}

		UInt32 type = taskData.completeNum;

		if (type != SAPABT_HELL_EPIC_EQUIP_DROP_ADD && 
			type != SAPABT_YUANGU_PINK_EQUIP_DROP_ADD &&
			type != SAPABT_HUNDUN_DROP_ADD)
		{
			return true;
		}

		if (taskData.rewards.size() != taskData.variables2.size())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " task(" << task->dataId << ")'s data variable2 size is not match rewards size!" << LogStream::eos;
			return true;
		}

		for (UInt32 idx = 0; idx != taskData.variables2.size(); ++idx)
		{
			UInt32 dungeonSubType = taskData.variables2[idx];

			if (dungeonData->subType != dungeonSubType)
			{
				continue;
			}

			auto taskAdditionInfo = taskData.rewards[idx];

			switch ((DropItemType)taskAdditionInfo.id)
			{
			case DIT_SS:
			case DIT_YUANGU_PINK:
				addition.dropAddition[taskAdditionInfo.id] += taskAdditionInfo.num;
				break;

			case DIT_XWZY:
				addition.dropMultiplyRateAddition[taskAdditionInfo.id] += taskAdditionInfo.num;
				break;

			default:
				ErrorLogStream << PLAYERINFO(GetOwner()) << " drop item type(" << taskAdditionInfo.id << ") is error!" << LogStream::eos;
				continue;
			}

			DebugLogStream << PLAYERINFO(GetOwner()) << " task(" << task->dataId << ") add drop addition(" << taskAdditionInfo.id
				<< ", " << taskAdditionInfo.num << ") to dungeon(" << dungeonData->id << ")" << LogStream::eos;
		}

		return true;
	});
}

void ActiveTaskMgr::GetOpActivityTaskBuff(const DungeonDataEntry* dungeonData, std::vector<RaceBuffInfo>& buffs)
{
	VisitOpenOpActAndTaskByOpType(OAT_SECOND_ANNIVERSARY_PRAY, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct || !task) return true;

		if (task->state != TASK_FINISHED)
		{
			return true;
		}

		UInt32 type = taskData.completeNum;

		if (type != SAPABT_DUNGEON_BUFF_ADD)
		{
			return true;
		}

		if (taskData.rewards.size() != taskData.variables2.size())
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " task(" << task->dataId << ")'s data variable2 size is not match rewards size!" << LogStream::eos;
			return true;
		}

		for (UInt32 idx = 0; idx != taskData.variables2.size(); ++idx)
		{
			UInt32 dungeonSubType = taskData.variables2[idx];

			if (dungeonData->subType != dungeonSubType)
			{
				continue;
			}

			auto buffId = taskData.rewards[idx].id;
			auto buffDataEntry = BuffDataEntryMgr::Instance()->FindEntry(buffId);
			if (!buffDataEntry)
			{
				continue;
			}

			// 添加战斗buff
			RaceBuffInfo raceBuff;
			raceBuff.m_id = buffDataEntry->id;
			raceBuff.m_overlayNums = buffDataEntry->overLayLimit;
			raceBuff.m_startTime = Avalon::Time::CurrentTime().MSec();
			raceBuff.m_duration = buffDataEntry->duration;
			buffs.push_back(raceBuff);
		}

		return true;
	});
}

UInt32 ActiveTaskMgr::GetOpActTaskFatigueRmAddition(OpActivityType type, const DungeonDataEntry* dungeonData)
{
	if (!dungeonData)
	{
		return 0;
	}
	UInt32 addition = 0;

	if (type == OAT_FATIGUE_BURNING)
	{
		if (dungeonData->threeType == DTT_TEAM_ELITE)
		{
			return addition;
		}
		// 获取在开放的活动(只能有一个)
		std::vector<OpActivityRegInfo*> infos = ActivityMgr::Instance()->GetOpActDataByTmpTypes((UInt32)type);
		if (infos.size() != 1)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << (UInt32)type << ") size(" << (UInt32)infos.size() << ") was error!" << LogStream::eos;
			return addition;
		}

		auto opActInfo = infos.front();
		if (!opActInfo)
		{
			return addition;
		}

		if (!IsMeetOpActRequireByPlayer(opActInfo))
		{
			return addition;
		}

		UInt32 fatigue = (UInt32)GetOwner()->GetFatigue();
		if (fatigue <= opActInfo->parm)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Fatigue(" << fatigue << ") was not enough(" << opActInfo->parm << ")!" << LogStream::eos;
			return addition;
		}

		for (auto data : opActInfo->tasks)
		{
			auto task = FindNewOpActTask(data.dataId);
			if (!task)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << data.dataId << ")!" << LogStream::eos;
				continue;
			}

			if (task->state == TASK_FINISHED)
			{
				addition += 100;
			}
		}
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "OpAct fatigue remove addition(" << addition << ")." << LogStream::eos;
	return addition;
}

void ActiveTaskMgr::OnIncReturnToken(UInt32 opActId)
{
	auto opAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opActId);
	if (!opAct)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find opActivity(" << opActId << ")!" << LogStream::eos;
		return;
	}

	if (opAct->state != AS_IN || !IsMeetOpActRequireByPlayer(opAct))
	{
		return;
	}

	_UpdateFatigueTokenOpActTasks(opAct);
}

void ActiveTaskMgr::OnUpdateOpTaskParamVar(OpActTask* task)
{
	//同步DB
	UpdateOpTaskDB(task);

	//同步客户端
	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);
}

bool ActiveTaskMgr::IsNotLoseStrenLvAfterEquipStrengthenFaile(UInt8 oldStrenLv, UInt8 upperLv)
{
	bool bValue = false;
	VisitOpenOpActDataByOpType(OAT_PROMOTE_EQUIP_STRENGTHEN_RATE, [&](const OpActivityRegInfo* opAct) {
		if (!opAct) return true;

		if (opAct->parm2.size() != 6)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " parm2 size is error!" << LogStream::eos;
			return true;
		}

		UInt8 checkMinLv = (UInt8)opAct->parm2[0];
		UInt8 checkMaxLv = (UInt8)opAct->parm2[1];
		if (checkMinLv > checkMaxLv)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " checkMinLv(" << (UInt32)checkMinLv 
				<< ") is more than maxLv(" << (UInt32)checkMaxLv << ")!" << LogStream::eos;
			return true;
		}

		UInt8 newStrenLv = oldStrenLv + upperLv;
		if (checkMinLv <= newStrenLv && newStrenLv <= checkMaxLv)
		{
			bValue = true;
			InfoLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " not lose, old level(" << (UInt32)oldStrenLv 
				<< "), new level(" << (UInt32)newStrenLv << ") ." << LogStream::eos;
		}

		return true;
	});

	return bValue;
}

void ActiveTaskMgr::EnhanceEquipStrengthenRate(UInt8 oldStrenLv, UInt8 upperLv, float& strenRate)
{
	UInt32 curHour = (UInt32)CURRENT_DATE.Hour();
	VisitOpenOpActDataByOpType(OAT_PROMOTE_EQUIP_STRENGTHEN_RATE, [&](const OpActivityRegInfo* opAct) {
		if (!opAct) return true;

		if (opAct->parm2.size() != 6)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " parm2 size is error!" << LogStream::eos;
			return true;
		}

		UInt8 checkMinLv = (UInt8)opAct->parm2[2];
		UInt8 checkMaxLv = (UInt8)opAct->parm2[3];
		if (checkMinLv > checkMaxLv)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " checkMinLv(" << (UInt32)checkMinLv
				<< ") is more than maxLv(" << (UInt32)checkMaxLv << ")!" << LogStream::eos;
			return true;
		}

		UInt8 newStrenLv = oldStrenLv + upperLv;
		if (checkMinLv <= newStrenLv && newStrenLv <= checkMaxLv)
		{
			UInt32 checkMinHour = opAct->parm2[4];
			UInt32 checkMaxHour = opAct->parm2[5];
			if (checkMinHour > checkMaxHour)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " checkMinHour(" << (UInt32)checkMinHour
					<< ") is more than max(" << checkMaxHour << ")!" << LogStream::eos;
				return true;
			}
			if (checkMinHour <= curHour && curHour < checkMaxHour)
			{
				UInt32 enhanceProb = opAct->parm;
				strenRate = strenRate * (float)(100 + enhanceProb) / 100;
				InfoLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " oldLv(" << oldStrenLv << "), newLv(" << newStrenLv 
					<< "), newRate(" << strenRate << "), enhance prob(" << enhanceProb << ")." << LogStream::eos;
			}
		}

		return true;
	});
}

void ActiveTaskMgr::_OnSevenDayClose(ActiveTask* task)
{
	if (!task)
	{
		return;
	}

	// 任务没完成不处理
	if (TASK_FINISHED != task->GetStatus())
	{
		return;
	}

	// 完成了自动领取到邮件
	RewardGroup* rg = task->GetRewards();
	if (!rg)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "rewardGroup is null! task id: " << task->GetID() << LogStream::eos;
		return;
	}

	// 获取邮件信息
	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string reason = GetReason(SOURCE_TYPE_SEVEN);
	GetOwner()->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

	Sys::SendSysMail(GetOwner()->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), rg);
}

void ActiveTaskMgr::_CreateActiveTask(UInt32 actID)
{
	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(actID, datas);

	for (auto data : datas)
	{
		ActivityScript* script = ScriptMgr::Instance()->FindActivityScript(data->id);
		if (script == NULL)
		{
			GameErrorLogStream("Player") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," 
				<< GetOwner()->GetName() << ") ActiveTask Script Err id:"<< data->id << ".";
			continue;
		}

		auto itr = m_ActiveTasks.find(data->id);
		if (itr == m_ActiveTasks.end())
		{
			//没有任务就接取
			ActiveTask* task = new ActiveTask(GetOwner(), data->id);
			task->SetData(data);
			task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
			script->OnAccepted(GetOwner(), task);

			m_ActiveTasks.insert(std::make_pair(data->id, task));

			InfoLogStream << "player (" << GetOwner()->GetAccID() << ") accept activeTask(" << data->id << ")" << LogStream::eos;
		}
	}
}

void ActiveTaskMgr::SaveOpTaskDB(OpActTask* task, UInt32 accountTask)
{
	if (task->uid == 0)
	{
		InsertOpTaskDB(task, accountTask);
	}
	else
	{
		UpdateOpTaskDB(task, accountTask);
	}
}

void ActiveTaskMgr::UpdateOpTaskDB(OpActTask* task, UInt32 accountTask)
{
	if (accountTask != 0)
	{
		UpdateOpAccountTaskDB(task);
		return;
	}

	if (!task)
	{
		ErrorLogStream << "OpActTask ptr is null!" << LogStream::eos;
		return;
	}

	std::ostringstream oss;
	oss << OPACT_COMMON_VAR << task->curNum;
	for (auto elem : task->params)
	{
		oss << "," << elem.first << "=" << elem.second;
	}

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_activity_op_task_new", task->uid, false);
	cmd->PutData("parameter", oss.str());
	cmd->PutData("status", task->state);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());

	if (SSApplication::Instance()->IsOpenDetailLog())
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << " update database: " << task->LogStr() << LogStream::eos;
	}
}

void ActiveTaskMgr::InsertOpTaskDB(OpActTask* task, UInt32 accountTask)
{
	if (accountTask != 0)
	{
		InsertOpAccountTaskDB(task);
		return;
	}

	if (!task)
	{
		ErrorLogStream << "OpActTask ptr is null!" << LogStream::eos;
		return;
	}

	task->uid = CLRecordClient::Instance()->GenGuid();
	task->ownerId = GetOwner()->GetID();

	std::ostringstream oss;
	oss << OPACT_COMMON_VAR << task->curNum;
	for (auto elem : task->params)
	{
		oss << "," << elem.first << "=" << elem.second;
	}

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_activity_op_task_new", task->uid);
	cmd->PutData("owner_id", task->ownerId);
	cmd->PutData("task_id", task->dataId);
	cmd->PutData("status", task->state);
	cmd->PutData("parameter", oss.str());
	cmd->PutData("create_time", UInt32(CURRENT_TIME.Sec()));
	CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());

	InfoLogStream << PLAYERINFO(GetOwner()) << " insert database: " << task->LogStr() << LogStream::eos;
}

void ActiveTaskMgr::VisitOpenOpActData(const std::function<bool(const OpActivityRegInfo*)>& F)
{
	std::vector<OpActivityRegInfo*> opActs;
	ActivityMgr::Instance()->GetOpenOpActs(opActs);
	for (auto opAct : opActs)
	{
		if (!opAct) continue;

		auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opAct->dataId);
		if (playerOpAct != NULL)
		{
			opAct = playerOpAct;
		}

		if (!IsMeetOpActRequireByPlayer(opAct))
		{
			continue;
		}

		if (opAct->state != AS_IN)
		{
			continue;
		}

		if (!F(opAct))
		{
			return;
		}
	}
}

void ActiveTaskMgr::VisitOpenOpActAndTaskData(const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&)>& F)
{
	VisitOpenOpActData([&F](const OpActivityRegInfo* opAct) {
		if (!opAct) return false;

		for (auto taskData : opAct->tasks)
		{
			if (!F(opAct, taskData))
			{
				return false;
			}
		}

		return true;
	});
}

void ActiveTaskMgr::VisitOpenOpActAndTask(const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&, OpActTask*)>& F)
{
	VisitOpenOpActAndTaskData([&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return false;

		// 是否为可接任务
		if (OATAT_ACCEPT_CANT == taskData.acceptType)
		{
			return true;
		}

		auto task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task)
		{
			DebugLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (!F(opAct, taskData, task))
		{
			return false;
		}

		return true;
	});
}

void ActiveTaskMgr::VisitOpenOpActDataByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*)>& F)
{
	std::vector<OpActivityRegInfo*> opActs;
	ActivityMgr::Instance()->GetOpActDataByTmpType(opType, opActs);
	for (auto opAct : opActs)
	{
		if (!opAct) continue;

		auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opAct->dataId);
		if (playerOpAct != NULL)
		{
			opAct = playerOpAct;
		}

		if (!IsMeetOpActRequireByPlayer(opAct))
		{
			continue;
		}

		if (opAct->state != AS_IN)
		{
			continue;
		}

		if (!F(opAct))
		{
			return;
		}
	}
}

void ActiveTaskMgr::VisitOpenOpActAndTaskDataByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&)>& F)
{
	VisitOpenOpActDataByOpType(opType, [&F](const OpActivityRegInfo* opAct){
		if (!opAct) return false;

		for (auto taskData : opAct->tasks)
		{
			if (!F(opAct, taskData))
			{
				return false;
			}
		}

		return true;
	});
}

void ActiveTaskMgr::VisitOpenOpActAndTaskByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&, OpActTask*)>& F)
{
	VisitOpenOpActAndTaskDataByOpType(opType, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData){
		if (!opAct) return false;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (!F(opAct, taskData, task))
		{
			return false;
		}

		return true;
	});
}

void ActiveTaskMgr::OnLogicDayChangeForOpAct()
{
	GetOwner()->SetCounter(OPACT_CHALLENGE_DAY_SCORE, 0);

	InfoLogStream << PLAYERINFO(GetOwner()) << " on logic day change for opActivity." << LogStream::eos;

	_OnMagpieBridgeOpTaskLogicDayChange();

	_CheckAndRestTaskByRestType(TimeUtil::REFRESH_TYPE_PER_DAY);
}

void ActiveTaskMgr::OnLogicWeekChangeForOpAct()
{
	_CheckAndRestTaskByRestType(TimeUtil::REFRESH_TYPE_PER_WEEK);
}

void ActiveTaskMgr::OnHireTask(HireTaskType type, UInt32 value, std::vector<UInt32> teammateIds)
{
	if (type == HTT_GUILD_DUNGEON)
	{
		if (GetOwner()->GetCounter(OPACT_HIRE_TASK_GUILD_DUNGEON_NUM) > 0)
		{
			return;
		}
		GetOwner()->SetCounter(OPACT_HIRE_TASK_GUILD_DUNGEON_NUM, 1);
	}
	CLProtocol::WorldOnSceneTrigeTask protocol;
	protocol.accid = GetOwner()->GetAccID();
	protocol.type = type;
	protocol.value = value;
	protocol.teammateIds = teammateIds;
	Router::SendToWorld(protocol);
}

void ActiveTaskMgr::_CheckAndRestTaskByRestType(UInt32 restType)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTask([&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return true;

		//账号任务不自动重置，需要活动自身逻辑去实现
		if (0 != taskData.accountTask && opAct->tmpType != OAT_ACCOUNT_ONLINE)
		{
			return true;
		}

		if (restType != taskData.resetType) return true;

		if (!task) return true;

		if (_CheckAndRestTask(opAct, taskData, task))
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
		});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool ActiveTaskMgr::_CheckAndRestTask(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task)
{
	switch (opAct->tmpType)
	{
	case OAT_DAILY_REWARD:
	case OAT_CHAMPION_GIFT:
		if (task->state != TASK_FINISHED)
		{
			task->state = TASK_FINISHED;

			// 如果已经不能领奖了，直接改成已完成状态
			if (!_CheckOpTaskAccountSubmitLimit(&taskData))
			{
				task->state = TASK_OVER;
			}
		}
		break;

	case OAT_DUNGEON_CLEAR_GET_REWARD:
		if (task->state == TASK_FINISHED)
		{
			OnTakeOpActTaskReward(opAct->dataId, task->dataId, 0);
		}

		task->Rest();
		task->state = TASK_UNFINISH;
		break;

	case OAT_SUMMER_CHALLENGE_WEEKLY:
		if (task->state != TASK_INIT)
		{
			task->Rest();
		}
		break;

	case OAT_HALLOWEEN_PUMPKIN_HELMET:
		if (taskData.completeNum == 1 && task->state != TASK_UNFINISH)
		{
			task->state = TASK_UNFINISH;
			task->curNum = 0;
			task->SetParamVar("lastTm", 0);
		}
		break;

	case OAT_NEW_YEAR_2020:
		if (taskData.variables.size() == 1)
		{
			// 每日登录，直接完成
			if (taskData.variables[0] == NEW_YEAR_2020_LOGIN)
			{
				task->state = TASK_FINISHED;
				task->curNum = 1;
			}
			else
			{
				task->state = TASK_UNFINISH;
				task->curNum = 0;
			}
		}	
		break;

	default:
		task->Rest();
		if (taskData.acceptType == OATAT_ACCEPT_AUTO)
		{
			task->state = TASK_UNFINISH;
		}
		break;
	}

	DebugLogStream << PLAYERINFO(GetOwner()) << " reset " << task->LogStr() << LogStream::eos;
	return true;
}

void ActiveTaskMgr::_AcceptOpActivities()
{
	if (GetOwner()->IsInFlyUP() && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_FLY_UP))
	{
		return;
	}
	CLProtocol::SCSyncOpActData opActProtocol;

	std::vector<OpActivityRegInfo*> opActs;
	ActivityMgr::Instance()->GetOpActs(opActs);

	for (auto pOpAct : opActs)
	{
		if (!pOpAct) continue;

		// 删除玩家身上的过期活动
		if (!ActivityMgr::Instance()->IsInOpActivity(pOpAct->dataId, GetOwner()))
		{
			for (auto& iter : pOpAct->tasks)
			{
				DebugLogStream << PLAYERINFO(GetOwner()) << "del expire op act id:" << iter.dataId<< LogStream::eos;
				DelOpActTask(iter.dataId);
			}
			continue;
		}

		GetOwner()->GetActivityAttributeMgr().SyncActivityAttributes(pOpAct);

		// 活动数据处理
		_AcceptOpActivity(pOpAct);

		// 同步客户端数据处理
		auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(pOpAct->dataId);
		if (playerOpAct != NULL)
		{
			pOpAct = playerOpAct;
		}

		OpActivityRegInfo info = *pOpAct;
		if (!IsMeetOpActRequireByPlayer(pOpAct))
		{
			info.state = AS_END;
		}

		switch ((OpActivityType)info.tmpType)
		{
		case OAT_GAMBING:
		case OAT_BET_HORSE:
			if (info.state == AS_END)
			{
				continue;
			}
			break;
		case OAT_RETURN_PRESENT:
			{
				std::string taskDesc = "";
				std::vector<std::string> descs;
				Avalon::StringUtil::Split(info.taskDesc, descs, "|");
				UInt32 index = 0;
				std::vector<OpActTaskData>::iterator it = info.tasks.begin();
				while (it != info.tasks.end())
				{
					OpActTask* task = FindNewOpActTask(it->dataId);
					if (!task || task->state == TASK_INIT)
					{
						it = info.tasks.erase(it);
					}
					else
					{
						++it;
						auto desc = index < descs.size() ? descs[index] : "";
						if (taskDesc == "")
						{
							taskDesc = desc;
						}
						else
						{
							taskDesc = taskDesc + "|" + desc;
						}
					}
					index++;
				}
				info.taskDesc = taskDesc;
			}
			break;
		case OAT_MALL_BUY_GOT:
			if (info.parm2.size() != 3)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << info.LogStr() << " param2 size is error!" << LogStream::eos;
			}
			else
			{
				info.endTime = info.parm2[2];
			}
			break;

		default: break;
		}

		opActProtocol.datas.push_back(info);
		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_OPACT_ONLINE_SYNC))
		{
			if (opActProtocol.datas.size() >= OPACT_SEND_PACKET_MAX_SIZE)
			{
				GetOwner()->SendProtocol(opActProtocol);
				opActProtocol.datas.clear();
			}
		}
	}

	if (!opActProtocol.datas.empty())
	{
		GetOwner()->SendProtocol(opActProtocol);
	}

	// 更新每日登录
	_UpdateDailyLoginOp();
	// 更新每日buff
	_UpdateDailyBuffOpActTask();
	// 更新周年祈福
	_UpdateSecondAnniversaryPrayOpActTask();
	// 更新礼遇特权任务
	UpdateGiftRightOnlineTask();

	// 更新登陆活动
	_SendOpActOnlineEvent();
}

void ActiveTaskMgr::_AcceptOpActivity(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	// 取个人活动
	auto playerOpAct = GetOwner()->GetPlayerActivityInfo().FindOpActivity(opAct->dataId);
	if (playerOpAct != NULL)
	{
		opAct = playerOpAct;
	}

	if (opAct->state != AS_IN)
	{
		return;
	}

	if (!IsMeetOpActRequireByPlayer(opAct))
	{
		return;
	}

	// 先更新一下记录数据
	_UpdateOpActivityPlayerRecordData(opAct);

	// 一些活动不需要接任务
	switch ((OpActivityType)opAct->tmpType)
	{
	case OAT_BUFF_ADDITION:
	case OAT_RETURN_PRIVILEGE:
		_AddBuffAddition(opAct);
		return;
	case OAT_WHOLE_BARGAIN_SHOP:
		_AcceptWholeBargain();
		return;
	default: break;
	}

	// 处理活动任务
	_AcceptOpActivityTasks(opAct);
}

void ActiveTaskMgr::_AcceptOpActivityTasks(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	// 特殊活动先处理
	switch ((OpActivityType)opAct->tmpType)
	{
	case OAT_DUNGEON_DROP_ACTIVITY:
	case OAT_DUNGEON_EXP_ADDITION:
	case OAT_PVP_PK_COIN:
	case OAT_DUNGEON_DROP_RATE_ADDITION:
		_AcceptFirstOrderlyTask(opAct);return;
	case OAT_MAGPIE_BRIDGE:
		_AcceptMagpieBridgeOpActTasks(opAct); return;
	case OAT_RETURN_PRESENT:
		_AcceptReturnPresent(opAct); return;
	case OAT_ENERGY_EXCHANGE:
		_AcceptReturnEnergyExchange(opAct); return;
	case OAT_RETURN_CHARGE_REBATE:
	case OAT_CHALLENGE_HELL:
		_AcceptReturnActivityTask(opAct); return;
	case OAT_CHALLENGE_HUB:
	{
		if (GetOwner()->GetCounterMgr().GetCounterUpdateTime(OPACT_CHALLENGE_SCORE) < opAct->startTime)
		{
			GetOwner()->SetCounter(OPACT_CHALLENGE_SCORE, 0);
		}
		if (GetOwner()->GetCounterMgr().GetCounterUpdateTime(OPACT_CHALLENGE_TOTAL_SCORE) < opAct->startTime)
		{
			GetOwner()->SetCounter(OPACT_CHALLENGE_TOTAL_SCORE, 0);
		}
		if (GetOwner()->GetCounterMgr().GetCounterUpdateTime(OPACT_CHALLENGE_DAY_SCORE) < opAct->startTime)
		{
			GetOwner()->SetCounter(OPACT_CHALLENGE_DAY_SCORE, 0);
		}
		break;
	}
	case OAT_CHALLENGE_HUB_SCORE:
	{

		break;
	}
	default: break;
	}

	CLProtocol::SCSyncOpActTasks protocol;

	
	// 通用的活动后处理
	for (auto taskData : opAct->tasks)
	{
		// 跳过不可接的任务
		if (taskData.acceptType == OATAT_ACCEPT_CANT)
		{
			continue;
		}

		bool taskChange = false;
		OpActTask* task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task)
		{
			task = new OpActTask(taskData.dataId);
			if (taskData.acceptType == OATAT_ACCEPT_AUTO)
			{
				task->state = TASK_UNFINISH;
			}
			AddNewOpActTask(task, taskData.accountTask);
			taskChange = true;
			InfoLogStream << PLAYERINFO(GetOwner()) << "Create " << task->LogStr() << LogStream::eos;

			switch ((OpActivityType)opAct->tmpType)
			{
				case OAT_DAY_LOGIN:
					_AcceptDailyLoginOpTasks(task, taskData);
					break;

				case OAT_FATIGUE_FOR_TOKEN_COIN:
					_AcceptFatigueTokenOpActTasks(task, taskData);
					break;

				case OAT_DAILY_REWARD:
				case OAT_CHAMPION_GIFT:
					_AcceptDailyRewardOpActTasks(task);
					break;

				case OAT_CHANGE_FASHION_EXCHANGE:
					_AcceptChangeFashionExchangeOpActTask(task, taskData);
					break;

				case OAT_CHANGE_FASHION_WELFARE:
					_AcceptChangeFashionOpWelfareActTask(task, taskData);
					break;
				case OAT_WEEK_SIGN_NEW_PLAYER:
				case OAT_WEEK_SIGN_ACTIVITY:
					_AddWeekSignData(opAct->tmpType);
					break;
				case OAT_NEW_YEAR_2020:
				{
						if (taskData.variables.size() != 1)
							continue;

						if (taskData.variables[0] == NEW_YEAR_2020_LOGIN && task->state == TASK_UNFINISH)
						{
							task->state = TASK_FINISHED;
							task->curNum = 1;
						}

						break;
				}
				case OAT_SPRING_FESTIVAL_RED_PACKET_RECV:
					_AcceptSpringRedPacketRecvOpActTask(task, taskData);
					break;
				case OAT_GIFT_RIGHT:
					//接任务的时候把couter都清一下，防止上次活动记录保留了
					GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD, 0);
					GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD_BUY_TIME, 0);
					GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD_LOGIN_TOTAL, 0);
					if (taskData.variables.size() != 1)
					{
						break;
					}
					if (taskData.variables[0] != OAGRTT_LEND_EQUIP)
					{
						break;
					}
					task->state = TASK_FINISHED;
					task->curNum = 1;
					break;
				case OAT_CHALLENGE_HUB:
				{
					if (taskData.variables.size() != 1)
					{
						break;
					}
					if (taskData.variables[0] != OACT_TEAM_DUNGEON)
					{
						break;
					}
					UInt32 num = GetOwner()->GetTeamCopyMgr().GetWeekNum(TC_TYPE_ONE, TEAM_COPY_TEAM_GRADE_COMMON);
					num += GetOwner()->GetTeamCopyMgr().GetWeekNum(TC_TYPE_ONE, TEAM_COPY_TEAM_GRADE_DIFF);
					if (num > 0)
					{
						OnChallengeActivityTask(OACT_TEAM_DUNGEON, false, num);
					}
					break;
				}
				case OAT_WEEK_SIGN_SPRING:
				{
					GetOwner()->SetCounter(OPACT_WEEK_SIGN_SPRING_NUM, 0);
				}
				default:
					break;
			}
		}

		// 更新
		switch ((OpActivityType)opAct->tmpType)
		{
		case OAT_PLAYER_LEVEL_UP:
		case OAT_COMMON_AWARD:
			_UpdatePlayerLevelUpOpActTasks(task, taskData);
			taskChange = true;
			break;

		case OAT_APPOINTMENT_OCCU:
			_UpdateAppointmentOpActTasks(task, taskData);
			taskChange = true;
			break;

		case OAT_FATIGUE_BURNING:
			_AddFatigueBurningBuff(task, taskData);
			taskChange = true;
			break;
		case OAT_GIFT_RIGHT:
			GetOwner()->SyncGiftRightToWorld();
			break;
		case OAT_ABYESS_TICKET_REDUCE:
			GetOwner()->SyncDungeonRightToWorld();
			break;

		default: break;
		}

		// 如果已经不能领奖了，直接改成已完成状态
		if ((opAct->tmpType == OAT_DAILY_REWARD || opAct->tmpType == OAT_CUMULATEPASS_DUNGEON_REWARD 
			|| opAct->tmpType == OAT_FATIGUE_FOR_TOKEN_COIN || opAct->tmpType == OAT_CHAMPION_GIFT)
			&& !_CheckOpTaskAccountSubmitLimit(&taskData))
		{
			task->state = TASK_OVER;
			taskChange = true;
		}

		if (SSApplication::Instance()->OpenPlayerOnOnline())
		{
			if (taskChange)
			{
				SaveOpTaskDB(task, taskData.accountTask);
			}
		}
		else
		{
			SaveOpTaskDB(task, taskData.accountTask);
		}

		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	// 任务相关连其他系统
	switch ((OpActivityType)opAct->tmpType)
	{
	case OAT_FATIGUE_FOR_BUFF:
		_AddFatigueBuff(opAct);
		break;
	case OAT_WEEK_SIGN_ACTIVITY:
	case OAT_WEEK_SIGN_NEW_PLAYER:
	{
		_NotifyWeekSignSumData(opAct->tmpType);
		_NotifyWeekSignBoxData(opAct->tmpType);
		break;
	}
	case OAT_TEAM_COPY_SUPPORT:
	{
		AddTeamCopyBuff(opAct);
		break;
	}
	case OAT_GNOME_COIN_GIFT:
	{
		OnGnomeCoinAdd(0);
		break;
	}
	default:
		break;
	}
}

void ActiveTaskMgr::_UpdateOpActivityPlayerRecordData(const OpActivityRegInfo* opAct)
{
	if (!opAct || opAct->state != AS_IN) return;

	switch ((OpActivityType)opAct->tmpType)
	{
	case OAT_FATIGUE_FOR_TOKEN_COIN:
	{
		std::string fatigueCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_COST_FATIGUE;
		_CheckRefreshOpActCounter(fatigueCountName, opAct);

		std::string validTokensCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_FATIGUE_TOKENS;
		_CheckRefreshOpActCounter(validTokensCountName, opAct);

		break;
	}

	case OAT_HELL_TICKET_FOR_DRAW_PRIZE:
	{
		std::string ticketCountName = Avalon::StringUtil::ConvertToString(opAct->tmpType) + OPACT_COST_HELL_TICKETS;
		_CheckRefreshOpActCounter(ticketCountName, opAct);

		std::string usedDrawCntName = Avalon::StringUtil::ConvertToString(opAct->tmpType) + OPACT_USED_DARW_COUNT;
		_CheckRefreshOpActCounter(usedDrawCntName, opAct);

		std::string drawCountName = Avalon::StringUtil::ConvertToString(opAct->tmpType) + OPACT_DARW_PRIZE_NUM;
		_CheckRefreshOpActCounter(drawCountName, opAct);

		break;
	}
	
	case OAT_CHANGE_FASHION_ACT:
	{
		std::string scoreCountName = ACTIVE_SCORE_PER + Avalon::StringUtil::ConvertToString(opAct->parm);
		_CheckRefreshOpActCounter(scoreCountName, opAct);

		std::string consumeCountName = ACTIVE_SCORE_CONSUME_PER + Avalon::StringUtil::ConvertToString(opAct->parm);
		_CheckRefreshOpActCounter(consumeCountName, opAct);
		break;
	}

	case OAT_HALLOWEEN_PUMPKIN_HELMET:
	{
		std::string usedDrawCntName = Avalon::StringUtil::ConvertToString(opAct->tmpType) + OPACT_USED_DARW_COUNT;
		_CheckRefreshOpActCounter(usedDrawCntName, opAct);

		std::string drawCountName = Avalon::StringUtil::ConvertToString(opAct->tmpType) + OPACT_DARW_PRIZE_NUM;
		_CheckRefreshOpActCounter(drawCountName, opAct);
		break;
	}

	case OAT_SPRING_FESTIVAL_RED_PACKET_RECV:
	{
		std::string scoreCountName = ACTIVE_SCORE_PER + Avalon::StringUtil::ConvertToString(opAct->dataId);
		_CheckRefreshOpActCounter(scoreCountName, opAct);
		break;
	}

	case OAT_ONLINE_GIFT:
	{
		_CheckRefreshOpActCounter(COUNTER_ACTIVE_ONLINE_GIFT_DAY, opAct);
		break;
	}

	case OAT_PLANT_TREE:
	{
	
		_CheckRefreshOpActCounter(COUNTER_PLANT_PROF, opAct);
		_CheckRefreshOpActCounter(COUNTER_PLANT_GROW_LASTTIME, opAct);
		_CheckRefreshOpActCounter(COUNTER_PLANT_STATE, opAct);
		_CheckRefreshOpActCounter(COUNTER_PLANT_ENDTM, opAct);

		std::vector<std::string> treeGetkeys;
		Avalon::StringUtil::Split(opAct->countParam, treeGetkeys, "|");
		for (auto counter : treeGetkeys)
		{
			_CheckRefreshOpActCounter(counter, opAct);
		}

		break;
	}

	default:
		break;
	}
}

void ActiveTaskMgr::_CheckRefreshOpActCounter(const std::string& countName, const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	if (CounterCfg::Instance()->GetCycleType(countName) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(countName.c_str(), COUNTER_CYCLE_NONE);
	}

	if (GetOwner()->GetCount(countName.c_str()) > 0)
	{
		UInt32 updateTime = GetOwner()->GetCounterMgr().GetCounterUpdateTime(countName.c_str());
		if (updateTime < opAct->startTime || updateTime > opAct->endTime)
		{
			GetOwner()->SetCounter(countName.c_str(), 0);

			InfoLogStream << PLAYERINFO(GetOwner()) << " refresh opAct(id=" << opAct->dataId << ", name=" << opAct->name
				<< ", st=" << TimeUtil::TimestampToStandardFormat(opAct->startTime)
				<< ", et=" << TimeUtil::TimestampToStandardFormat(opAct->endTime)
				<< ")'s count(name=" << countName 
				<< ", updatetime=" << TimeUtil::TimestampToStandardFormat(updateTime)
				<< ") value." << LogStream::eos;
		}
	}
}

void ActiveTaskMgr::_AcceptDailyLoginOpTasks(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (taskData.completeNum + DAY_TO_SEC <= curTime)
	{
		// 超过1天过期了
		task->state = TASK_FAILED;
	}
	else if (curTime >= taskData.completeNum)
	{
		// 完成
		task->state = TASK_FINISHED;
	}
	else
	{
		task->state = TASK_UNFINISH;
	}
}

void ActiveTaskMgr::_UpdateDailyLoginOp()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	UInt32 now = (UInt32)Avalon::Time::CurrentTime().Sec();

	VisitOpenOpActAndTaskByOpType(OAT_DAY_LOGIN, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task){
		if (!opAct) return false;

		if (!task) return true;

		if (task->curNum != UINT32_MAX && task->state == TaskStatus::TASK_UNFINISH)
		{
			task->curNum = now;

			if (taskData.completeNum + 24 * 3600 <= now)
			{
				// 超过1天过期了
				task->state = TaskStatus::TASK_FAILED;
			}
			else if (now >= taskData.completeNum)
			{
				// 完成
				task->state = TaskStatus::TASK_FINISHED;
			}
			else
			{
				task->state = TaskStatus::TASK_UNFINISH;
			}

			UpdateOpTaskDB(task);
		}

		if (task->state == TaskStatus::TASK_FINISHED && taskData.completeNum + 24 * 3600 <= task->curNum)
		{
			// 超过1天过期了
			task->state = TaskStatus::TASK_FAILED;
			UpdateOpTaskDB(task);
		}

		protocol.tasks.push_back(*task);

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_UpdatePlayerLevelUpOpActTasks(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;

	task->AddCurNum(GetOwner()->GetLevel(), taskData.completeNum, false);
}

void ActiveTaskMgr::_UpdateBindPhoneOpActTasks()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_BIND_PHONE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task){
		if (!opAct) return false;

		if (!task) return true;

		if (!task->AddCurNum(1, taskData.completeNum, false))
		{
			return true;
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);

		InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << " update success." << LogStream::eos;

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_OnPlayerPVPEndForPVPPKCoinOpAct(PkType type, PkRaceResult status)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_PVP_PK_COIN, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task){
		if (!opAct || !task) return true;
		
		// 是否为可接任务
		if (taskData.acceptType == OATAT_ACCEPT_CANT)
		{
			return true;
		}

		if (task->state != TASK_UNFINISH)
		{
			return true;
		}

		if (task->AddCurNum(1, taskData.completeNum, true))
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		if (task->state != TASK_FINISHED)
		{
			return false;
		}

		auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(task->dataId);
		if (!taskDataEntry)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find OpActivity task data(" << task->dataId << ")!" << LogStream::eos;
			return false;
		}

		auto nextTask = FindNewOpActTask(taskDataEntry->nextTaskId);
		if (nextTask != NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " task(" << task->dataId << ")'s next task(" << taskDataEntry->nextTaskId << ") is already exist!" << LogStream::eos;
		}
		else
		{
			nextTask = new OpActTask(taskDataEntry->nextTaskId);
			AddNewOpActTask(nextTask);
		}

		nextTask->state = TASK_UNFINISH;
		SaveOpTaskDB(nextTask);
		protocol.tasks.push_back(*nextTask);

		InfoLogStream << PLAYERINFO(GetOwner()) << " task(" << task->dataId << ")'s next task(" << taskDataEntry->nextTaskId << ") create." << LogStream::eos;

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_UpdateAppointmentOpActTasks(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;

	if (!GetOwner()->IsAppointmentOccu())
	{
		return;
	}

	task->AddCurNum(GetOwner()->GetLevel(), taskData.completeNum, false);
	InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << LogStream::eos;
}

void ActiveTaskMgr::_UpdateHellTicketOpActTasks(UInt32 usedTickets)
{
	//获取开放的活动
	std::vector<OpActivityRegInfo*> opActs = ActivityMgr::Instance()->GetOpActDataByTmpTypes(OAT_HELL_TICKET_FOR_DRAW_PRIZE);

	if (opActs.empty()) return;

	UInt32 opActSize = opActs.size();

	//当前只能有一个深渊票活动
	if (opActSize != 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " opActivity(" << (UInt32)OAT_HELL_TICKET_FOR_DRAW_PRIZE << ") has too more, size=" << opActSize << LogStream::eos;
		return;
	}

	auto opAct = opActs.front();
	if (!opAct) return;

	//其他条件不满足
	if (!IsMeetOpActRequireByPlayer(opAct))
	{
		return;
	}

	std::string countName = Avalon::StringUtil::ConvertToString(opAct->tmpType) + OPACT_COST_HELL_TICKETS;
	if (CounterCfg::Instance()->GetCycleType(countName) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(countName.c_str(), COUNTER_CYCLE_NONE);
	}

	// 增加总的深渊票消耗量
	GetOwner()->IncCounter(countName.c_str(), usedTickets);
	UInt32 totalTickets = GetOwner()->GetCount(countName.c_str());

	InfoLogStream << PLAYERINFO(GetOwner()) << "On used hell tickets(" << usedTickets << "), total tickets=" << totalTickets << LogStream::eos;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can not find opAct task(" << task->dataId << ")." << LogStream::eos;
			continue;
		}

		//增加计数
		if (!task->AddCurNum(totalTickets, taskData.completeNum, false))
		{
			continue;
		}

		//达到计数条件，任务完成，给予奖励(抽奖券)
		if (task->state == TASK_FINISHED)
		{
			if (taskData.rewards.size() != 1)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") data's rewards size error!" << LogStream::eos;
				continue;
			}

			// 奖励的抽奖券数量
			UInt32 rewardNum = taskData.rewards.front().num;

			std::string drawCountName = Avalon::StringUtil::ConvertToString((UInt32)opAct->tmpType) + OPACT_DARW_PRIZE_NUM;
			if (CounterCfg::Instance()->GetCycleType(drawCountName) == COUNTER_CYCLE_INVALID)
			{
				CounterCfg::Instance()->RegCounter(drawCountName.c_str(), COUNTER_CYCLE_NONE);
			}

			GetOwner()->IncCounter(drawCountName.c_str(), rewardNum);
			UInt32 drawPrizeCount = GetOwner()->GetCount(drawCountName.c_str());

			task->state = TASK_OVER;

			InfoLogStream << PLAYERINFO(GetOwner()) << "Give reward, num=" << rewardNum << ", now draw prize count=" << drawPrizeCount << LogStream::eos;
		}

		UpdateOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_AcceptFatigueTokenOpActTasks(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;
	task->curNum = taskData.completeNum;
}

void ActiveTaskMgr::_OnRemoveFatigueForFatigueTokenOpAct(UInt32 fatigueNum)
{
	// 1代币可转疲劳数
	UInt32 tokenToFatigueRate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ONE_TOKEN_COIN_TO_FATIGUE_NUM);
	if (tokenToFatigueRate == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Error token coin to fatigue rate(" << tokenToFatigueRate << ")!" << LogStream::eos;
		return;
	}

	VisitOpenOpActDataByOpType(OAT_FATIGUE_FOR_TOKEN_COIN, [&](const OpActivityRegInfo* opAct){
		if (!opAct) return false;

		std::string fatigueCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_COST_FATIGUE;
		if (CounterCfg::Instance()->GetCycleType(fatigueCountName) == COUNTER_CYCLE_INVALID)
		{
			CounterCfg::Instance()->RegCounter(fatigueCountName.c_str(), COUNTER_CYCLE_NONE);
		}

		std::string validTokensCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_FATIGUE_TOKENS;
		if (CounterCfg::Instance()->GetCycleType(validTokensCountName) == COUNTER_CYCLE_INVALID)
		{
			CounterCfg::Instance()->RegCounter(validTokensCountName.c_str(), COUNTER_CYCLE_NONE);
		}

		// 增加总的疲劳数
		UInt32 totalFatigue = GetOwner()->IncCounter(fatigueCountName.c_str(), fatigueNum);
		// 增加的代币
		UInt32 addTokens = fatigueNum / tokenToFatigueRate;
		// 增加总的代币
		UInt32 fatigueTokens = GetOwner()->IncCounter(validTokensCountName.c_str(), addTokens);

		InfoLogStream << PLAYERINFO(GetOwner()) << " remove fatigue(" << fatigueNum << "), add tokens(" << addTokens << "), opAct(" << opAct->dataId << ", " << opAct->name
			<< ")'s total fatigue=" << totalFatigue << ", valid token num=" << fatigueTokens << "." << LogStream::eos;

		_UpdateFatigueTokenOpActTasks(opAct);

		return true;
	});
}

void ActiveTaskMgr::_UpdateFatigueTokenOpActTasks(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	std::string validTokensCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_FATIGUE_TOKENS;
	if (CounterCfg::Instance()->GetCycleType(validTokensCountName) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(validTokensCountName.c_str(), COUNTER_CYCLE_NONE);
	}

	UInt32 fatigueTokens = GetOwner()->GetCount(validTokensCountName.c_str());

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		if (taskData.variables.size() != 1)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Error variable format in task data(" << taskData.dataId << ")!" << LogStream::eos;
			continue;
		}

		// 代币满足条件
		UInt32 tokenMeetCond = taskData.variables.front();

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			continue;
		}

		if (taskData.accountTotalSubmitLimit > 0)
		{
			auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskData.dataId);
			if (times < taskData.accountTotalSubmitLimit)
			{
				if (fatigueTokens >= tokenMeetCond)
				{
					if (task->state != TASK_FINISHED)
					{
						// 货币充足可兑换
						task->state = TASK_FINISHED;
						SaveOpTaskDB(task);
					}
				}
				else
				{
					if (task->state != TASK_UNFINISH)
					{
						// 货币不足
						task->state = TASK_UNFINISH;
						SaveOpTaskDB(task);
					}
				}
			}
			else
			{
				if (task->state != TASK_OVER)
				{
					// 兑换次数为0
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
		}
		else
		{
			//旧的角色限制
			if (task->curNum == UINT32_MAX)
			{
				if (task->state != TASK_OVER)
				{
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
			else if (task->curNum > 0)
			{
				if (fatigueTokens >= tokenMeetCond)
				{
					if (task->state != TASK_FINISHED)
					{
						// 货币充足可兑换
						task->state = TASK_FINISHED;
						SaveOpTaskDB(task);
					}
				}
				else
				{
					if (task->state != TASK_UNFINISH)
					{
						// 货币不足
						task->state = TASK_UNFINISH;
						SaveOpTaskDB(task);
					}
				}
			}
			else
			{
				if (task->state != TASK_OVER)
				{
					// 兑换次数为0
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
		}




		protocol.tasks.push_back(*task);
	}

	GetOwner()->SendProtocol(protocol);
}

bool ActiveTaskMgr::_OnSubmitFatigueToken(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData) return false;

	InfoLogStream << PLAYERINFO(GetOwner()) << "start submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	if (taskData->variables.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") data's variables size error!" << LogStream::eos;
		return false;
	}


	CLProtocol::SCSyncOpActTaskChange protocol;
	if (taskData->accountTotalSubmitLimit > 0)
	{
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskData->dataId);
		if (times >= taskData->accountTotalSubmitLimit)
		{
			task->state = TASK_OVER;
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Fatigue token coin exchange num(" << times << ") reach the limit(data: " << taskData->dataId << ")!" << LogStream::eos;
			return false;
		}

		std::string validTokensCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_FATIGUE_TOKENS;
		if (CounterCfg::Instance()->GetCycleType(validTokensCountName) == COUNTER_CYCLE_INVALID)
		{
			CounterCfg::Instance()->RegCounter(validTokensCountName.c_str(), COUNTER_CYCLE_NONE);
		}

		Int32 tokenNum = (Int32)GetOwner()->GetCount(validTokensCountName.c_str());
		UInt32 costNum = taskData->variables.front();
		Int32 remainNum = tokenNum - (Int32)costNum;
		if (remainNum < 0)
		{
			if (times < taskData->accountTotalSubmitLimit) task->state = TASK_UNFINISH;
			else task->state = TASK_OVER;
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Fatigue token coin num(" << tokenNum << ") reach the limit(data: " << taskData->dataId << ")!" << LogStream::eos;
			return false;
		}

		GetOwner()->SetCounter(validTokensCountName.c_str(), (UInt32)remainNum);


		InfoLogStream << PLAYERINFO(GetOwner()) << "submit task(" << task->dataId << ", " << task->uid << "), remove token(" << costNum << "), remain token(" << remainNum << LogStream::eos;


		for (auto taskDataIner : opAct->tasks)
		{
			if (taskDataIner.variables.size() != 1)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Error variable format in task data(" << taskDataIner.dataId << ")!" << LogStream::eos;
				continue;
			}

			auto task = FindNewOpActTask(taskDataIner.dataId);
			if (!task)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << taskDataIner.dataId << ")!" << LogStream::eos;
				continue;
			}

			if (taskDataIner.accountTotalSubmitLimit > 0)
			{
				auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskDataIner.dataId);
				if (taskDataIner.dataId == taskData->dataId)
				{
					++times;
				}
				if (times < taskDataIner.accountTotalSubmitLimit)
				{
					if ((UInt32)remainNum >= taskDataIner.variables.front())
					{
						// 货币充足可兑换
						task->state = TASK_FINISHED;
						SaveOpTaskDB(task);
					}
					else
					{
						// 货币不足
						task->state = TASK_UNFINISH;
						SaveOpTaskDB(task);
					}
				}
				else
				{
					// 兑换次数为0
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
			else
			{
				if (task->curNum == UINT32_MAX)
				{
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
				else if (task->curNum > 0)
				{
					if ((UInt32)remainNum >= taskDataIner.variables.front())
					{
						// 货币充足可兑换
						task->state = TASK_FINISHED;
						SaveOpTaskDB(task);
					}
					else
					{
						// 货币不足
						task->state = TASK_UNFINISH;
						SaveOpTaskDB(task);
					}
				}
				else
				{
					// 兑换次数为0
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
			protocol.tasks.push_back(*task);
		}
	}
	else
	{
		Int32 num = (Int32)task->curNum;
		num -= 1;

		if (num < 0)
		{
			task->state = TASK_OVER;
			task->curNum = 0;
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Fatigue token coin exchange num(" << num << ") reach the limit(data: " << taskData->dataId << ")!" << LogStream::eos;
			return false;
		}

		std::string validTokensCountName = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_FATIGUE_TOKENS;
		if (CounterCfg::Instance()->GetCycleType(validTokensCountName) == COUNTER_CYCLE_INVALID)
		{
			CounterCfg::Instance()->RegCounter(validTokensCountName.c_str(), COUNTER_CYCLE_NONE);
		}

		Int32 tokenNum = (Int32)GetOwner()->GetCount(validTokensCountName.c_str());
		UInt32 costNum = taskData->variables.front();
		Int32 remainNum = tokenNum - (Int32)costNum;
		if (remainNum < 0)
		{
			if (task->curNum > 0) task->state = TASK_UNFINISH;
			else task->state = TASK_OVER;
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Fatigue token coin num(" << tokenNum << ") reach the limit(data: " << taskData->dataId << ")!" << LogStream::eos;
			return false;
		}

		task->curNum = num;

		GetOwner()->SetCounter(validTokensCountName.c_str(), (UInt32)remainNum);

		InfoLogStream << PLAYERINFO(GetOwner()) << "submit task(" << task->dataId << ", " << task->uid << "), remove token(" << costNum << "), remain token(" << remainNum << LogStream::eos;

		for (auto taskDataIner : opAct->tasks)
		{
			if (taskDataIner.variables.size() != 1)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Error variable format in task data(" << taskDataIner.dataId << ")!" << LogStream::eos;
				continue;
			}

			auto task = FindNewOpActTask(taskDataIner.dataId);
			if (!task)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << taskDataIner.dataId << ")!" << LogStream::eos;
				continue;
			}

			if (taskDataIner.accountTotalSubmitLimit > 0)
			{
				auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskDataIner.dataId);
				if (taskDataIner.dataId == taskData->dataId)
				{
					++times;
				}
				if (times < taskDataIner.accountTotalSubmitLimit)
				{
					if ((UInt32)remainNum >= taskDataIner.variables.front())
					{
						// 货币充足可兑换
						task->state = TASK_FINISHED;
						SaveOpTaskDB(task);
					}
					else
					{
						// 货币不足
						task->state = TASK_UNFINISH;
						SaveOpTaskDB(task);
					}
				}
				else
				{
					// 兑换次数为0
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
			else
			{
				if (task->curNum == UINT32_MAX)
				{
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
				else if (task->curNum > 0)
				{
					if ((UInt32)remainNum >= taskDataIner.variables.front())
					{
						// 货币充足可兑换
						task->state = TASK_FINISHED;
						SaveOpTaskDB(task);
					}
					else
					{
						// 货币不足
						task->state = TASK_UNFINISH;
						SaveOpTaskDB(task);
					}
				}
				else
				{
					// 兑换次数为0
					task->state = TASK_OVER;
					SaveOpTaskDB(task);
				}
			}
			protocol.tasks.push_back(*task);
		}
	}

	GetOwner()->SendProtocol(protocol);
	return true;
}

void ActiveTaskMgr::_OnRemoveFatigueForFatigueBuffOpAct(UInt32 fatigueNum)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActDataByOpType(OAT_FATIGUE_FOR_BUFF, [&](const OpActivityRegInfo* opAct){
		if (!opAct) return false;

		InfoLogStream << PLAYERINFO(GetOwner()) << " remove fatigue(" << fatigueNum << "), " << opAct->LogStr() << LogStream::eos;

		bool hasTaskChangeToFinished = false;
		for (auto taskData : opAct->tasks)
		{
			auto task = FindNewOpActTask(taskData.dataId);
			if (!task)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " can not find opAct task(" << task->dataId << ")." << LogStream::eos;
				continue;
			}

			UInt8 oldStatus = task->state;

			//增加计数
			if (!task->AddCurNum(fatigueNum, taskData.completeNum, true))
			{
				continue;
			}

			UpdateOpTaskDB(task);

			InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << " curNum=" << task->curNum << LogStream::eos;

			if (oldStatus != task->state && task->state == TASK_FINISHED)
			{
				hasTaskChangeToFinished = true;
			}

			protocol.tasks.push_back(*task);
		}

		if (hasTaskChangeToFinished)
		{
			_AddFatigueBuff(opAct);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_AddFatigueBuff(const OpActivityRegInfo* opAct)
{
	if (!opAct || opAct->state != AS_IN) return;

	std::vector<OpActTaskData> tmpTaskData = opAct->tasks;

	struct CustomMoreOpActTaskData
	{
		bool operator()(const OpActTaskData& lhs, const OpActTaskData& rhs)
		{
			return lhs.completeNum > rhs.completeNum;
		}
	};

	//将最大的排到最前面
	std::sort(tmpTaskData.begin(), tmpTaskData.end(), CustomMoreOpActTaskData());

	struct OverTaskInfo
	{
		OpActTask* task;
		UInt32 buffId;
	};

	//当前需要展示的buff
	UInt32 showBuffId = 0;
	//其他需要结束的任务
	std::vector<OverTaskInfo> overTasks;

	for (auto data : tmpTaskData)
	{
		auto task = FindNewOpActTask(data.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find opAct task(" << data.dataId << ")!" << LogStream::eos;
			continue;
		}

		if (data.variables.size() != 1)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Error variable format in task data(" << data.dataId << ")!" << LogStream::eos;
			continue;
		}

		UInt32 buffId = data.variables.front();

		if (task->state == TASK_FINISHED)
		{
			//将最新最靠前的任务展示，其他的结束掉
			if (showBuffId == 0)
			{
				showBuffId = buffId;
				continue;
			}
			else
			{
				OverTaskInfo taskInfo;
				taskInfo.task = task;
				taskInfo.buffId = buffId;

				overTasks.push_back(taskInfo);
			}
		}
	}

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskInfo : overTasks)
	{
		if (!taskInfo.task) continue;

		taskInfo.task->state = TASK_OVER;
		UpdateOpTaskDB(taskInfo.task);

		//结束的任务将buff也清除掉
		if (!GetOwner()->GetBuffMgr().RemoveBuff(taskInfo.buffId))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove OpActBuff(" << taskInfo.buffId << ") failed, " << taskInfo.task->LogStr() << LogStream::eos;
		}

		protocol.tasks.push_back(*taskInfo.task);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	if (showBuffId == 0)
	{
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (opAct->endTime < curTime)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " end time=" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << LogStream::eos;
		return;
	}

	//添加新的buff
	UInt32 buffDuration = opAct->endTime - curTime;
	GetOwner()->GetBuffMgr().AddOpActBuff(showBuffId, buffDuration);
}

void ActiveTaskMgr::_OnTickFatigueBurningOpAct(const Avalon::Time& now)
{
	UInt32 curTime = (UInt32)now.Sec();

	VisitOpenOpActAndTaskByOpType(OAT_FATIGUE_BURNING, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task){
		if (!opAct) return false;

		if (!task) return true;

		if (taskData.variables.size() != OPACT_FATIGUE_BURNING_VARIABLE_SIZE)
		{
			return true;
		}

		if (task->state != TASK_FINISHED)
		{
			return true;
		}

		// 累计开启一定时间关闭
		if (task->curNum > curTime)
		{
			return true;
		}

		InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << " curNum(" << task->curNum << ") reach limit(" << curTime << ")." << LogStream::eos;

		task->state = TASK_UNFINISH;
		task->curNum = 0;

		UpdateOpTaskDB(task);

		CLProtocol::SCSyncOpActTaskChange protocol;
		protocol.tasks.push_back(*task);
		GetOwner()->SendProtocol(protocol);

		UInt32 buffId = taskData.variables[2];
		GetOwner()->GetBuffMgr().RemoveBuff(buffId);

		return true;
	});
}

bool ActiveTaskMgr::_OnSubmitFatigueBurning(OpActTask* task, OpActTaskData* taskData)
{
	if (!task || !taskData) return false;

	InfoLogStream << PLAYERINFO(GetOwner()) << "on submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	auto opActId = task->GetOpActId();
	auto opActInfo = ActivityMgr::Instance()->GetOpActData(opActId);
	if (!opActInfo)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find opAct(" << opActId << "), " << task->LogStr() << LogStream::eos;
		return false;
	}

	if ((UInt32)GetOwner()->GetFatigue() <= opActInfo->parm && task->state != TASK_FINISHED)
	{
		GetOwner()->SendNotify(9055, opActInfo->parm);
		return false;
	}

	// 判断选择开启任务时其他任务是否已开启
	for (auto data : opActInfo->tasks)
	{
		if (data.variables.size() != OPACT_FATIGUE_BURNING_VARIABLE_SIZE)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Error variable format in task data(" << data.dataId << ")!" << LogStream::eos;
			return false;
		}

		auto otherTask = FindNewOpActTask(data.dataId);
		if (!otherTask)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << data.dataId << ")!" << LogStream::eos;
			continue;
		}

		if (otherTask->dataId == task->dataId)
		{
			continue;
		}

		if (otherTask->state == TASK_FINISHED)
		{
			GetOwner()->SendNotify(9054);
			return false;
		}
	}

	if (taskData->variables.size() != OPACT_FATIGUE_BURNING_VARIABLE_SIZE)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Error variable format in task data(" << taskData->dataId << ")!" << LogStream::eos;
		return false;
	}

	UInt32 costItemId = taskData->variables[0];
	UInt32 costItemNum = taskData->variables[1];
	UInt32 buffId = taskData->variables[2];

	switch ((TaskStatus)task->state)
	{
	// 开启
	case TASK_INIT:
	case TASK_UNFINISH:
	{
		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(costItemId);
		if (!itemData)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find item data entry(" << costItemId << ")!" << LogStream::eos;
			return false;
		}

		if (!GetOwner()->CheckRmMoney(itemData, costItemNum))
		{
			GetOwner()->SendNotify(1000044);
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Money(" << costItemId << ") were not enough!" << LogStream::eos;
			return false;
		}

		if (!GetOwner()->RemoveItem(GetReason(SOURCE_TYPE_TASK_ACTIVE, task->dataId).c_str(), costItemId, costItemNum))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(" << costItemId << ", num=" << costItemNum << ") failed!" << LogStream::eos;
			return false;
		}

		task->state = TASK_FINISHED;
		task->curNum = (UInt32)Avalon::Time::CurrentTime().Sec() + taskData->completeNum;

		GetOwner()->GetBuffMgr().AddOpActBuff(buffId, taskData->completeNum);
		break;
	}
	// 冻结
	case TASK_FINISHED:
		task->state = TASK_FAILED;
		task->curNum = (Int32)task->curNum - (Int32)Avalon::Time::CurrentTime().Sec();
		if (task->curNum < 0)
		{
			task->curNum = 0;
		}

		if (!GetOwner()->GetBuffMgr().RemoveBuff(buffId))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove OpActBuff(" << buffId
				<< ") failed, task(" << taskData->dataId << ")." << LogStream::eos;
		}
		break;
	// 解冻
	case TASK_FAILED:
		task->state = TASK_FINISHED;
		task->curNum += (UInt32)Avalon::Time::CurrentTime().Sec();

		GetOwner()->GetBuffMgr().AddOpActBuff(buffId, taskData->completeNum);
		break;

	default:
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Error task(" << task->dataId << ") status(" << task->state 
			<< "), curNum=" << task->curNum << ")!" << LogStream::eos;
		return false;
	}

	return true;
}

void ActiveTaskMgr::_AddFatigueBurningBuff(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;

	if (task->state != TASK_FINISHED)
	{
		return;
	}

	if (taskData.variables.size() != OPACT_FATIGUE_BURNING_VARIABLE_SIZE)
	{
		return;
	}

	UInt32 buffId = taskData.variables[2];

	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();
	UInt32 duration = task->curNum > curTime ? task->curNum - curTime : 0;

	GetOwner()->GetBuffMgr().AddOpActBuff(buffId, duration);
}

void ActiveTaskMgr::_AcceptDailyRewardOpActTasks(OpActTask* task)
{
	if (!task) return;
	task->state = TASK_FINISHED;
}

void ActiveTaskMgr::_AcceptMagpieBridgeOpActTasks(const OpActivityRegInfo* opAct)
{
	// 接受第一个任务
	if (!opAct || opAct->state != AS_IN) return;

	// 每日进度计数key
	std::string dailyProgCountKey = Avalon::StringUtil::ConvertToString(opAct->dataId) + OPACT_MAGPIE_BRIDGE_DAILY_PROGRESS;
	if (CounterCfg::Instance()->GetCycleType(dailyProgCountKey) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(dailyProgCountKey.c_str(), COUNTER_CYCLE_DAY, 6);
	}

	// 今日进度
	UInt32 countValue = GetOwner()->GetCount(dailyProgCountKey.c_str());

	InfoLogStream << PLAYERINFO(GetOwner()) << "OpActivity(" << opAct->dataId << "), count(key=" << dailyProgCountKey << ", value=" << countValue << ")." << LogStream::eos;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		OpActTask* canDoTask = FindNewOpActTask(taskData.dataId);
		if (!canDoTask)
		{
			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(taskData.dataId);
			if (!taskDataEntry)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data entry(" << taskData.dataId << ")!" << LogStream::eos;
				continue;
			}

			if (taskDataEntry->preTaskId == 0)
			{
				// 如果前置任务id为0，那么该任务为第一个任务
				canDoTask = new OpActTask(taskData.dataId);
				canDoTask->state = TASK_UNFINISH;

				AddNewOpActTask(canDoTask);
				SaveOpTaskDB(canDoTask);

				protocol.tasks.push_back(*canDoTask);

				InfoLogStream << PLAYERINFO(GetOwner()) << "Create " << canDoTask->LogStr() << LogStream::eos;
			}
		}
		else
		{
			protocol.tasks.push_back(*canDoTask);
		}
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_OnMagpieBridgeOpTaskRmFatigue(UInt32 fatigueNum)
{
	UInt32 opActType = OAT_MAGPIE_BRIDGE;

	// 获取在开放的活动(只能有一个)
	std::vector<OpActivityRegInfo*> infos = ActivityMgr::Instance()->GetOpActDataByTmpTypes(opActType);

	if (infos.empty()) return;

	if (infos.size() > 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << opActType << ") size(" << (UInt32)infos.size() << ") was error!" << LogStream::eos;
		return;
	}

	auto opActInfo = infos.front();
	if (!opActInfo) return;

	if (opActInfo->parm2.size() != 2)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << opActInfo->dataId << ")'s param2 size=" << (UInt32)opActInfo->parm2.size() << ") is error!" << LogStream::eos;
		return;
	}

	// 每日进度控制
	UInt32 dailyProgressLimit = opActInfo->parm2[1];

	// 每日进度计数key
	std::string dailyProgCountKey = Avalon::StringUtil::ConvertToString(opActInfo->dataId) + OPACT_MAGPIE_BRIDGE_DAILY_PROGRESS;
	if (CounterCfg::Instance()->GetCycleType(dailyProgCountKey) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(dailyProgCountKey.c_str(), COUNTER_CYCLE_DAY, 6);
	}

	// 今日进度
	UInt32 todayProgressMeters = GetOwner()->GetCount(dailyProgCountKey.c_str());

	// 今日进度达到限制
	if (todayProgressMeters >= dailyProgressLimit)
	{
		return;
	}

	// 找到可以做的任务
	OpActTask* canDoTask = NULL;
	OpActTaskData canDoTaskData;

	for (auto taskData : opActInfo->tasks)
	{
		auto opTask = FindNewOpActTask(taskData.dataId);
		if (!opTask) continue;

		if (opTask->state == TASK_UNFINISH)
		{
			canDoTask = opTask;
			canDoTaskData = taskData;
			break;
		}
	}

	if (!canDoTask) return;

	// 1疲劳可转鹊桥进度
	UInt32 oneFatigueToMBProg = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ONE_FATIGUE_TO_MAGPIE_BRIDGE_PROGRESS);
	if (oneFatigueToMBProg == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Error fatigue fatigue to progress meter(" << oneFatigueToMBProg << ")!" << LogStream::eos;
		return;
	}

	// 增加的进度
	UInt32 incProgress = fatigueNum * oneFatigueToMBProg;

	CLProtocol::SCSyncOpActTaskChange protocol;

	// 可循环次数
	UInt32 rollTimes = opActInfo->tasks.size();

	while (incProgress > 0 || canDoTask->state == TASK_FINISHED)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "Current doing task(" << canDoTask->LogStr() << ") curNum=" << canDoTask->curNum << ", incProgress=" << incProgress << LogStream::eos;

		if (canDoTask->state == TASK_FINISHED)
		{
			// 当前任务完成后要接受下一个任务

			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(canDoTask->dataId);
			if (!taskDataEntry)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data entry(" << canDoTask->dataId << ")!" << LogStream::eos;
				break;
			}

			auto nextTask = FindNewOpActTask(taskDataEntry->nextTaskId);
			if (nextTask != NULL)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "There has next task(" << taskDataEntry->nextTaskId <<
					"), current task(" << canDoTask->dataId << ")!" << LogStream::eos;
			}
			else
			{
				nextTask = new OpActTask(taskDataEntry->nextTaskId);
				nextTask->state = TASK_UNFINISH;
				AddNewOpActTask(nextTask);
				SaveOpTaskDB(nextTask);
			}

			InfoLogStream << PLAYERINFO(GetOwner()) << "IncProgress=" << incProgress << ", Current do task(" << canDoTask->LogStr() 
				<< "), next do task(" << nextTask->LogStr() << ")" << LogStream::eos;

			canDoTask = nextTask;

			if (incProgress == 0)
			{
				protocol.tasks.push_back(*canDoTask);
				break;
			}
		}

		todayProgressMeters = GetOwner()->GetCount(dailyProgCountKey.c_str());

		// 今日进度达到限制
		if (todayProgressMeters >= dailyProgressLimit)
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "Current do task(" << canDoTask->LogStr() << "), today progress(" << todayProgressMeters << ") reach limitProg(" << dailyProgressLimit << ")!" << LogStream::eos;
			break;
		}

		// 新增进度超出每日限制
		if (incProgress + todayProgressMeters > dailyProgressLimit)
		{
			incProgress = dailyProgressLimit - todayProgressMeters;
			InfoLogStream << PLAYERINFO(GetOwner()) << "Current do task(" << canDoTask->LogStr() << "), today progress(" << todayProgressMeters << ") and incProg(" << incProgress << ") reach limitProg(" << dailyProgressLimit << ")!" << LogStream::eos;
		}

		// 原来的进度
		UInt32 oldTaskCurNum = canDoTask->curNum;

		// 增加当前任务进度
		canDoTask->curNum += incProgress;

		if (canDoTask->curNum >= canDoTaskData.completeNum)
		{
			canDoTask->state = TASK_FINISHED;
			incProgress = canDoTask->curNum - canDoTaskData.completeNum;
			canDoTask->curNum = canDoTaskData.completeNum;
		}
		else
		{
			incProgress = 0;
		}

		SaveOpTaskDB(canDoTask);
		protocol.tasks.push_back(*canDoTask);

		// 新增进度
		UInt32 dailyProgIncCount = canDoTask->curNum - oldTaskCurNum;
		GetOwner()->IncCounter(dailyProgCountKey.c_str(), dailyProgIncCount);

		InfoLogStream << PLAYERINFO(GetOwner()) << "Current do task(" << canDoTask->LogStr() << "), oldCurNum(" << oldTaskCurNum << "), nowCurNum(" << canDoTask->curNum << ") today progress(" << todayProgressMeters << "), actual incProg(" << dailyProgIncCount << "), after increase, today prog=" << GetOwner()->GetCount(dailyProgCountKey.c_str()) << LogStream::eos;

		rollTimes--;

		// 一个异常保护
		if (rollTimes == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Current doing task(" << canDoTask->LogStr() <<
				") reach roll limit times(" << rollTimes << ")!" << LogStream::eos;
			break;
		}
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_OnMagpieBridgeOpTaskLogicDayChange()
{
	UInt32 opActType = OAT_MAGPIE_BRIDGE;

	// 获取在开放的活动(只能有一个)
	std::vector<OpActivityRegInfo*> infos = ActivityMgr::Instance()->GetOpActDataByTmpTypes(opActType);

	if (infos.empty()) return;

	if (infos.size() > 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << opActType << ") size(" << (UInt32)infos.size()
			<< ") was error!" << LogStream::eos;
		return;
	}

	auto opActInfo = infos.front();
	if (!opActInfo) return;

	// 每日进度计数key
	std::string dailyProgCountKey = Avalon::StringUtil::ConvertToString(opActInfo->dataId) + OPACT_MAGPIE_BRIDGE_DAILY_PROGRESS;
	if (CounterCfg::Instance()->GetCycleType(dailyProgCountKey) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(dailyProgCountKey.c_str(), COUNTER_CYCLE_DAY, 6);
	}

	// 今日进度
	UInt32 countValue = GetOwner()->GetCount(dailyProgCountKey.c_str());

	InfoLogStream << PLAYERINFO(GetOwner()) << "OpActivity(" << opActInfo->dataId << ") on logic day change, count(key=" << dailyProgCountKey << ", value=" << countValue << ")." << LogStream::eos;
}

void ActiveTaskMgr::_OnOpActBuyMonthCard()
{
	UInt32 opActType = OAT_MONTH_CARD;

	// 获取在开放的活动(只能有一个)
	std::vector<OpActivityRegInfo*> infos = ActivityMgr::Instance()->GetOpActDataByTmpTypes(opActType);

	if (infos.empty()) return;

	if (infos.size() > 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << opActType << ") size(" << (UInt32)infos.size() << ") was error!" << LogStream::eos;
		return;
	}

	auto opActInfo = infos.front();
	if (!opActInfo) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opActInfo->tasks)
	{
		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << taskData.dataId << ")!" << LogStream::eos;
			continue;
		}

		if (task->state == TASK_UNFINISH)
		{
			task->state = TASK_FINISHED;
			SaveOpTaskDB(task);

			protocol.tasks.push_back(*task);
		}
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_AddBuffAddition(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (opAct->endTime < curTime)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " OpActvitiy(" << opAct->dataId << ") et=" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << LogStream::eos;
		return;
	}

	for (auto task : opAct->tasks)
	{
		for (auto& buffId : task.variables2)
		{
			UInt32 buffDuration = opAct->endTime - curTime;
			GetOwner()->GetBuffMgr().AddOpActBuff(buffId, buffDuration);
		}
	}
}

void ActiveTaskMgr::_OnPlayerStartDungeonForDungeonRandomBuffOpAct(const DungeonDataEntry* dungeonData)
{
	if (!dungeonData) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	VisitOpenOpActDataByOpType(OAT_DUNGEON_RANDOM_BUFF, [&](const OpActivityRegInfo* opAct){
		if (!opAct) return true;

		for (auto id : opAct->parm2)
		{
			auto buffRandomData = BuffRandomDataEntryMgr::Instance()->FindEntry(id);
			if (!buffRandomData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " can not find buff random data(" << id << ")!" << LogStream::eos;
				continue;
			}

			GetOwner()->GetBuffMgr().RemoveOpActBuff(opAct->dataId, buffRandomData->buffId);
		}

		if (std::find(opAct->parm3.begin(),opAct->parm3.end(), (UInt32)dungeonData->subType) != opAct->parm3.end())
		{
			if (opAct->endTime > curTime)
			{
				UInt32 buffId = BuffRandomDataEntryMgr::Instance()->RandBuff(opAct->parm2);
				UInt32 buffDuration = opAct->endTime - curTime;
				GetOwner()->GetBuffMgr().AddOpActBuff(buffId, buffDuration, opAct->dataId);
				return true;
			}
		}

		return true;
	});
}

void ActiveTaskMgr::_OnDayChangeForDailyBuffOpAct()
{
	_UpdateDailyBuffOpActTask();
}

void ActiveTaskMgr::_UpdateDailyBuffOpActTask()
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	CLProtocol::SCSyncOpActTaskChange protocol;

	InfoLogStream << PLAYERINFO(GetOwner()) << " start update..." << LogStream::eos;

	VisitOpenOpActAndTaskDataByOpType(OAT_DAILY_BUFF, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData){
		if (!opAct) return true;

		UInt32 dayBeginTime = opAct->startTime;

		if (dayBeginTime == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " completeNum is zero!" << LogStream::eos;
			return false;
		}

		UInt32 dayEndTime = opAct->endTime;

		if (dayBeginTime <= curTime && curTime <= dayEndTime)
		{
			auto task = FindNewOpActTask(taskData.dataId);
			if (!task)
			{
				task = new OpActTask(taskData.dataId);
				AddNewOpActTask(task);
			}

			task->curNum = dayEndTime;
			task->state = TASK_FINISHED;

			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);

			InfoLogStream << PLAYERINFO(GetOwner()) << " accept " << task->LogStr() << LogStream::eos;
		}
		else
		{
			auto task = FindNewOpActTask(taskData.dataId);
			if (task != NULL && task->state != TASK_OVER)
			{
				task->state = TASK_OVER;

				SaveOpTaskDB(task);
				protocol.tasks.push_back(*task);

				InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << " over." << LogStream::eos;
			}
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_OnDayChangeForArtifactJarOpAct()
{
	VisitOpenOpActDataByOpType(OAT_ARTIFACT_JAR, [&](const OpActivityRegInfo* opAct){
		if (!opAct || opAct->state != AS_IN) return true;

		GetOwner()->GetActivityAttributeMgr().SyncArtifactJarDiscountInfo(opAct->dataId);

		return true;
	});
}

void ActiveTaskMgr::_AcceptFirstOrderlyTask(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		// 是否为可接任务
		if (taskData.acceptType == OATAT_ACCEPT_CANT)
		{
			continue;
		}

		OpActTask* canDoTask = FindNewOpActTask(taskData.dataId);
		if (!canDoTask)
		{
			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(taskData.dataId);
			// 如果前置任务id为0，那么该任务为第一个任务
			if (taskDataEntry != NULL && taskDataEntry->preTaskId != 0)
			{
				continue;
			}

			canDoTask = new OpActTask(taskData.dataId);
			canDoTask->state = TASK_UNFINISH;

			AddNewOpActTask(canDoTask);
			SaveOpTaskDB(canDoTask);

			protocol.tasks.push_back(*canDoTask);

			InfoLogStream << PLAYERINFO(GetOwner()) << "Create " << canDoTask->LogStr() << LogStream::eos;
		}
		else
		{
			protocol.tasks.push_back(*canDoTask);
		}
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::MakeUpFatgue(bool online)
{
	//判断是否满足疲劳找回等级
	if (GetOwner()->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MAKEUP_NEED_LV))
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << "make up need lv not enough!" << LogStream::eos;
		return;
	}

	UInt32 createTime = GetOwner()->GetCreateTime();
	//创建角色的第一天不算
	if (CheckInSameLogicDay((UInt32)CURRENT_TIME.Sec(), createTime))
	{
		return;
	}

	//计算上一次结余疲劳
	UInt32 lastFatigue = GetOwner()->GetLastFatigue();
	DebugLogStream << PLAYERINFO(GetOwner()) << " GetLastFatigue = " << lastFatigue << LogStream::eos;

	UInt32 offlineDay = GetOwner()->GetOfflineDay(6);
	if (offlineDay > 0)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << " offlineDay = " << offlineDay << " lastSaveTime = " << GetOwner()->GetSavebackTime() << LogStream::eos;
		lastFatigue = (offlineDay - 1) * GetOwner()->GetMaxFatigue() + lastFatigue;
	}

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_MAKEUP_FATIGUE, datas);

	for (auto data : datas)
	{
		if (data->id > MAKEUP_USELESSID)
		{
			continue;
		}

		auto itr = m_ActiveTasks.find(data->id);
		if (itr == m_ActiveTasks.end())
		{
			CreatFatigueTask(data, lastFatigue);
		}
		else
		{
			ActiveTask* task = itr->second;
			if (!task)
			{
				continue;
			}

			if (lastFatigue > 0)
			{
				UpdateFatigueTask(task, lastFatigue);
			}
			else if (task->GetVar("lv") == 0)
			{
				//对老数据重新计算一编
				UpdateFatigueTask(task, lastFatigue);
			}
		}
	}
}

void ActiveTaskMgr::CreatFatigueTask(ActiveTaskDataEntry* data, UInt32 lastFatigue)
{
	ActiveTask* task = new ActiveTask(GetOwner(), data->id);
	task->SetData(data);
	task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
	if (lastFatigue > 0)
	{
		task->SetStatus(TASK_FINISHED);
	}
	else
	{
		task->SetStatus(TASK_INIT);
	}

	//单次找到的疲劳数
	//UInt32 step = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FATIGUE_MAKEUP_STEP);
	//UInt32 singleMakeUp = lastFatigue < step ? lastFatigue : step;

	//单次找到的疲劳数
	FatigueMakeUpData* makeUpData = FatigueMakeUpDataMgr::Instance()->FindEntry(GetOwner()->GetLevel());
	if (!makeUpData)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << " FatigueMakeUpData is null! key = " << GetOwner()->GetLevel() << LogStream::eos;
		return;
	}

	//计算当前vip下最多可保存的疲劳
	UInt32 makeupFatigueMax = 0;
	if (GetOwner()->GetVipLvl() > 0)
	{
		UInt32 vipVal = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_OFFLINE_FATIGUE_LIMIT, vipVal);
		makeupFatigueMax = vipVal;
	}
	else
	{
		makeupFatigueMax = makeUpData->fatigueMax;
	}

	lastFatigue = lastFatigue > makeupFatigueMax ? makeupFatigueMax : lastFatigue;

	task->SetVarNoSync("lgf", 0);										// 低档已找回的疲劳值
	task->SetVarNoSync("hgf", 0);										// 高档已找回的疲劳值
	task->SetVarNoSync("lf", lastFatigue);								// 前一天结余的疲劳值
	task->SetVarNoSync("mf", makeupFatigueMax);							// 最多能保存的疲劳值
	task->SetVarNoSync("le", makeUpData->lowExp * lastFatigue);			// 低档找回的奖励经验值
	task->SetVarNoSync("lmi", makeUpData->lowNeedMoneyID);						// 低档找回的所需货币
	//task->SetVarNoSync("lc", FatigueMakeUpPriceMgr::Instance()->GetPrice(1, true) * singleMakeUp);		// 低档找回的费用
	task->SetVarNoSync("he", makeUpData->hiExp * lastFatigue);			// 高档找回的奖励经验值
	task->SetVarNoSync("hmi", makeUpData->hiNeedMoneyID);						// 高档找回的所需货币
	//task->SetVarNoSync("hc", FatigueMakeUpPriceMgr::Instance()->GetPrice(1, false) * singleMakeUp);			// 高档找回的费用
	task->SetVarNoSync("vip", GetOwner()->GetVipLvl());					// 当前vip等级
	task->SetVarNoSync("lv", GetOwner()->GetLevel());					// 等级

	m_ActiveTasks.insert(std::make_pair(data->id, task));

	DebugLogStream << PLAYERINFO(GetOwner()) << " creat fatigue activeTask(" << data->id << ")" <<
		//" singleMakeUp:" << singleMakeUp <<
		" lastFatigue:" << lastFatigue <<
		" makeupFatigueMax:" << makeupFatigueMax <<
		" lv:" << GetOwner()->GetLevel() <<
		" vipLv:" << GetOwner()->GetVipLvl() << LogStream::eos;

	task->SaveToDB();
}

void ActiveTaskMgr::UpdateFatigueTask(ActiveTask* task, UInt32 lastFatigue)
{
	UInt32 lv = (UInt32)GetOwner()->GetLevel();
	
	//获得等级对应的data
	FatigueMakeUpData* makeUpData = FatigueMakeUpDataMgr::Instance()->FindEntry(lv);
	if (!makeUpData)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << " FatigueMakeUpData is null! key = " << task->GetVar("lv") << LogStream::eos;
		return;
	}

	//计算当前vip下最多可保存的疲劳
	UInt32 makeupFatigueMax = 0;
	if (GetOwner()->GetVipLvl() > 0)
	{
		UInt32 vipVal = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_OFFLINE_FATIGUE_LIMIT, vipVal);
		makeupFatigueMax = vipVal;
	}
	else
	{
		makeupFatigueMax = makeUpData->fatigueMax;
	}

	UInt32 oldLastFatigue = (UInt32)task->GetVar("lf");
	UInt32 newLastFatigue = oldLastFatigue + lastFatigue;
	newLastFatigue = newLastFatigue > makeupFatigueMax ? makeupFatigueMax : newLastFatigue;

	if (newLastFatigue > 0)
	{
		task->SetStatus(TASK_FINISHED);

		//单次找到的疲劳数
		//UInt32 step = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FATIGUE_MAKEUP_STEP);
		//UInt32 singleMakeUp = newLastFatigue < step ? newLastFatigue : step;

		task->SetVarNoSync("lgf", 0);										// 低档已找回的疲劳值
		task->SetVarNoSync("hgf", 0);										// 高档已找回的疲劳值
		task->SetVarNoSync("lf", newLastFatigue);							// 前一天结余的疲劳值
		task->SetVarNoSync("mf", makeupFatigueMax);							// 最多能保存的疲劳值
		task->SetVarNoSync("le", makeUpData->lowExp * newLastFatigue);			// 低档找回的奖励经验值
		task->SetVarNoSync("lmi", makeUpData->lowNeedMoneyID);						// 低档找回的所需货币
		//task->SetVarNoSync("lc", FatigueMakeUpPriceMgr::Instance()->GetPrice(1, true) * singleMakeUp);		// 低档找回的费用
		task->SetVarNoSync("he", makeUpData->hiExp * newLastFatigue);			// 高档找回的奖励经验值
		task->SetVarNoSync("hmi", makeUpData->hiNeedMoneyID);						// 高档找回的所需货币
		//task->SetVarNoSync("hc", FatigueMakeUpPriceMgr::Instance()->GetPrice(1, false) * singleMakeUp);			// 高档找回的费用
		task->SetVarNoSync("vip", GetOwner()->GetVipLvl());					// 当前vip等级
		task->SetVarNoSync("lv", lv);										// 等级

		DebugLogStream << PLAYERINFO(GetOwner()) << " update fatigue activeTask(" << task->GetDataEntry()->id << ")" <<
			//" singleMakeUp:" << singleMakeUp <<
			" lastFatigue:" << lastFatigue <<
			" makeupFatigueMax:" << makeupFatigueMax <<
			" lv:" << lv <<
			" vipLv:" << GetOwner()->GetVipLvl() << LogStream::eos;
	}
	else
	{
		task->SetStatus(TASK_INIT);
	}
	
	task->SaveToDB();
}

bool ActiveTaskMgr::SubmitFatigueTask(ActiveTask* task, UInt16 perfect, UInt16 num)
{
	UInt64 addExp = 0;
	Int64 cost = 0;
	UInt32 costItemID = 0;
	bool bPerfect = perfect > 0 ? true : false;
	UInt32 oldLastFatigue = (UInt32)task->GetVar("lf");

	UInt32 step = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FATIGUE_MAKEUP_STEP);
	if (0 == step)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " step is 0!" << LogStream::eos;
		return false;
	}

	//单次找到的疲劳数
	UInt32 makeUpNum = num * step;
	UInt32 singleMakeUp = oldLastFatigue < makeUpNum ? oldLastFatigue : makeUpNum;
	UInt32 newLastFatigue = oldLastFatigue > singleMakeUp ? (oldLastFatigue - singleMakeUp) : 0;

	FatigueMakeUpData* makeUpData = FatigueMakeUpDataMgr::Instance()->FindEntry((UInt32)task->GetVar("lv"));
	if (!makeUpData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " FatigueMakeUpData is null! key = " << task->GetVar("lv") << LogStream::eos;
		return false;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, task->GetID(), task->GetStatus());

	UInt32 completNum = singleMakeUp / step;
	UInt32 restNum = singleMakeUp % step;
	UInt32 getFatigueNum = 0;

	if (bPerfect)
	{
		getFatigueNum = (UInt32)task->GetVar("hgf");
		addExp = makeUpData->hiExp * singleMakeUp;
		costItemID = makeUpData->hiNeedMoneyID;
	}
	else
	{
		getFatigueNum = (UInt32)task->GetVar("lgf");
		addExp = makeUpData->lowExp * singleMakeUp;
		costItemID = makeUpData->lowNeedMoneyID;
	}

	ItemDataEntry* costItemData = ItemDataEntryMgr::Instance()->FindEntry(costItemID);
	if (!costItemData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " cost itemData error! id = " << costItemID << LogStream::eos;
		return false;
	}

	for (UInt32 i = 0; i < completNum; ++i)
	{
		getFatigueNum += step;
		cost += FatigueMakeUpPriceMgr::Instance()->GetPrice(getFatigueNum, !bPerfect) * step;
	}

	cost += FatigueMakeUpPriceMgr::Instance()->GetPrice(getFatigueNum, !bPerfect) * restNum;

	if (cost <= 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "makeup fatigue cost is err! cost = " << cost << LogStream::eos;
		return false;
	}
	
	if (GetOwner()->GetItemNum(costItemID) < cost)
	{
		//所需货币不够
		GetOwner()->SendNotify(1250, costItemData->name.c_str());
		return false;
	}
		
	if (!GetOwner()->RemoveItem(reason.c_str(), costItemID, (UInt32)cost))
	{
		//所需货币不够
		GetOwner()->SendNotify(1250, costItemData->name.c_str());
		return false;
	}

	GetOwner()->IncExp(reason.c_str(), addExp);

	CLProtocol::SceneNotifyGetItem notify;
	notify.sourceType = SOURCE_TYPE_TASK_ACTIVE;
	ItemReward item;
	item.id = 600000003;
	item.num = (UInt32)addExp;
	notify.items.push_back(item);
	GetOwner()->SendProtocol(notify);

	task->SetVar("lf", newLastFatigue);

	task->SetVar("he", makeUpData->hiExp * newLastFatigue);
	task->SetVar("le", makeUpData->lowExp * newLastFatigue);

	if (bPerfect)
	{
		task->SetVar("hgf", task->GetVar("hgf") + singleMakeUp);
	}
	else
	{
		task->SetVar("lgf", task->GetVar("lgf") + singleMakeUp);
	}

	DebugLogStream << PLAYERINFO(GetOwner()) << " submit fatigue task! addExp = " << addExp << ",cost = " << cost 
		<< ",perfect = " << perfect << ",newLastFatigue = " << newLastFatigue << ",singleMakeUp = " << singleMakeUp
		<< LogStream::eos;

	if (newLastFatigue == 0)
	{
		task->SetVar("lgf", 0);
		task->SetVar("hgf", 0);
		task->SetStatus(TASK_OVER);
	}

	task->SaveToDB();
	return true;
} 

void ActiveTaskMgr::UpdateMaxFatigue()
{
	FatigueMakeUpData* data = FatigueMakeUpDataMgr::Instance()->FindEntry(GetOwner()->GetLevel());
	if (!data)
	{
		return;
	}

	UInt32 fatigueMax = 0;
	
	if (GetOwner()->GetVipLvl() > 0)
	{
		UInt32 vipVal = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_OFFLINE_FATIGUE_LIMIT, vipVal);
		fatigueMax = vipVal;
	}
	else
	{
		fatigueMax = data->fatigueMax;
	}

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_MAKEUP_FATIGUE, datas);

	for (auto data : datas)
	{
		if (data->id > MAKEUP_USELESSID)
		{
			continue;
		}

		auto itr = m_ActiveTasks.find(data->id);
		if (itr != m_ActiveTasks.end())
		{
			ActiveTask* task = itr->second;
			if (!task) return;
			
			task->SetVar("mf", fatigueMax);
			task->SetVar("vip", GetOwner()->GetVipLvl());
		}
	}
}

void ActiveTaskMgr::MakeUpDungen(bool online)
{
	if (GetOwner()->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MAKEUP_NEED_LV))
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << "make up dungen lv not enough!" << LogStream::eos;
		return;
	}

	std::vector<ActiveTaskDataEntry*> datas;
	ActiveTaskDataEntryMgr::Instance()->GetActives(AID_MAKEUP_DUNGEN, datas);

	for (auto data : datas)
	{
		if (data->id > MAKEUP_USELESSID)
		{
			continue;
		}

		DungenTaskValue value;
		GetDungenValue(data->dungeonId, value);

// 		DebugLogStream << PLAYERINFO(GetOwner()) << " id = " << value.id
// 			<< " normalCost = " << value.normalCost << " perfectCost = " << value.perfectCost
// 			<< " itemNormalNum size = " << (UInt32)value.itemNormalNum.size() << " itemPerfectNum size = " << (UInt32)value.itemPerfectNum.size()
// 			<< " count = " << value.count << LogStream::eos;

		auto itr = m_ActiveTasks.find(data->id);
		if (itr == m_ActiveTasks.end())
		{
			CreatDungenTask(data, value);
		}
		else
		{
			ActiveTask* task = itr->second;
			if (!task)
			{
				continue;
			}
			
			if ((online && GetOwner()->GetOfflineDay(6) > 0) ||
				!online)
			{
				UpdateDungenTask(task, value);
			}
		}
	}
}

void ActiveTaskMgr::GetDungenValue(UInt32 id, DungenTaskValue& value)
{
	//DebugLogStream << PLAYERINFO(GetOwner()) << " dungeonType = " << id << " lv = " << GetOwner()->GetLevel() << LogStream::eos;

	DungeonMakeUpData* data = DungeonMakeUpDataMgr::Instance()->GetData(id, GetOwner()->GetLevel());
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " DungeonMakeUpData is null! dungeonID = " << id
			<< " level = " << GetOwner()->GetLevel() << LogStream::eos;
		return;
	}

	UInt32 maxTime = SSActivityDungeonMgr::Instance()->GetDungeonMaxTimes(GetOwner(), (DungeonSubType)id);

	UInt32 lastMissNum = GetDungeonMissNum(id);
	UInt32 offlineDay = GetOwner()->GetOfflineDay(6);
	UInt32 totMissNum = offlineDay > 0 ? (lastMissNum + (offlineDay - 1) * maxTime) : lastMissNum;

	value.id = id;
	value.count = totMissNum > data->countMax ? data->countMax : totMissNum;
	value.normalCost = data->goldCost;
	value.perfectCost = data->pointCost;
	value.normalItems = data->normalItems;
	value.itemNormalNum = data->normalNum;
	value.perfectItems = data->perfectItems;
	value.itemPerfectNum = data->costNum;
	value.max = data->countMax;
}

void ActiveTaskMgr::CreatDungenTask(ActiveTaskDataEntry* data, DungenTaskValue& value)
{
	ActiveTask* task = new ActiveTask(GetOwner(), data->id);
	task->SetData(data);
	task->SetAcceptTime((UInt32)Avalon::Time::CurrentTime().Sec());
	if (value.count > 0)
	{
		task->SetStatus(TASK_FINISHED);
	}
	else
	{
		task->SetStatus(TASK_INIT);
	}
	
	task->SetVarNoSync("ct", value.count);
	task->SetVarNoSync("nsz", value.normalItems.size());
	task->SetVarNoSync("psz", value.perfectItems.size());

	for (size_t i = 0; i < value.normalItems.size(); ++i)
	{
		std::string key;
		key += "ni";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
		task->SetVarNoSync(key.c_str(), value.normalItems[i]);
	}

	for (size_t i = 0; i < value.perfectItems.size(); ++i)
	{
		std::string key;
		key += "pi";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
		task->SetVarNoSync(key.c_str(), value.perfectItems[i]);
	}

	for (size_t i = 0; i < value.itemNormalNum.size(); ++i)
	{
		std::string key;
		key += "nnum";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i+1);
		task->SetVarNoSync(key.c_str(), value.itemNormalNum[i]);
	}

	for (size_t i = 0; i < value.itemPerfectNum.size(); ++i)
	{
		std::string key;
		key += "pnum";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i+1);
		task->SetVarNoSync(key.c_str(), value.itemPerfectNum[i]);
	}
	
	task->SetVarNoSync("ncs", value.normalCost);
	task->SetVarNoSync("pcs", value.perfectCost);
	m_ActiveTasks.insert(std::make_pair(data->id, task));

	task->SaveToDB();
}

void ActiveTaskMgr::UpdateDungenTask(ActiveTask* task, DungenTaskValue& value)
{
	UInt32 oldCount = (UInt32)task->GetVar("ct");
	UInt32 newCount = oldCount + value.count;
	if (newCount > value.max)
	{
		newCount = value.max;
	}

	if (newCount > 0)
	{
		task->SetStatus(TASK_FINISHED);
	}
	else
	{
		task->SetStatus(TASK_INIT);
	}

	task->SetVarNoSync("ct", newCount);
	task->SetVarNoSync("nsz", value.normalItems.size());
	task->SetVarNoSync("psz", value.perfectItems.size());

	for (size_t i = 0; i < value.normalItems.size(); ++i)
	{
		std::string key;
		key += "ni";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
		task->SetVarNoSync(key.c_str(), value.normalItems[i]);
	}

	for (size_t i = 0; i < value.perfectItems.size(); ++i)
	{
		std::string key;
		key += "pi";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
		task->SetVarNoSync(key.c_str(), value.perfectItems[i]);
	}

	for (size_t i = 0; i < value.itemNormalNum.size(); ++i)
	{
		std::string key;
		key += "nnum";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i+1);
		task->SetVarNoSync(key.c_str(), value.itemNormalNum[i]);
	}

	for (size_t i = 0; i < value.itemPerfectNum.size(); ++i)
	{
		std::string key;
		key += "pnum";
		key += Avalon::StringUtil::ConvertToString<UInt32>(i+1);
		task->SetVarNoSync(key.c_str(), value.itemPerfectNum[i]);
	}

	task->SetVarNoSync("ncs", value.normalCost);
	task->SetVarNoSync("pcs", value.perfectCost);
	task->SaveToDB();
}

bool ActiveTaskMgr::SubmitDungeonTask(ActiveTask* task, UInt16 perfect, UInt16 num)
{
	std::vector<UInt64> addNum;
	std::vector<UInt32> itemId;
	UInt32 cost = 0;
	int sz = 0;
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, task->GetID(), task->GetStatus());

	int ct = (int)task->GetVar("ct");

	if (num > ct)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "num is err! num = " << num << LogStream::eos;
		return false;
	}

	if (perfect > 0)
	{
		UInt32 vipVal = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_PERFECT_FIND, vipVal);
		if (vipVal == 0)
		{
			GetOwner()->SendNotify(1000053);
			return false;
		}
		
		sz = (int)task->GetVar("psz");

		for (int i = 0; i < sz; ++i)
		{
			std::string numKey;
			numKey += "pnum";
			numKey += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
			addNum.push_back(task->GetVar(numKey.c_str()));

			std::string idKey;
			idKey += "pi";
			idKey += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
			itemId.push_back((UInt32)task->GetVar(idKey.c_str()));
		}

		cost = (UInt32)task->GetVar("pcs") * num;
		if (cost == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "makeup dungeon cost is err! cost = " << cost << LogStream::eos;
			return false;
		}

		bool allExp = true;
		for (int i = 0; i < sz; ++i)
		{
			ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemId[i]);
			if (itemdata && itemdata->subType != ST_ITEM_EXP)
			{
				allExp = false;
				break;
			}
		}

		if (allExp && GetOwner()->IsFullLevel())
		{
			GetOwner()->SendNotify(3132);
			return false;
		}

		if (!GetOwner()->CheckRmMoney(ST_BIND_POINT, cost))
		{
			//点券不够
			GetOwner()->SendNotify(1000027);
			return false;
		}

		GetOwner()->RemovePointBoth(reason.c_str(), cost);
	}
	else
	{
		sz = (int)task->GetVar("nsz");

		for (int i = 0; i < sz; ++i)
		{
			std::string numKey;
			numKey += "nnum";
			numKey += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
			addNum.push_back(task->GetVar(numKey.c_str()));

			std::string idKey;
			idKey += "ni";
			idKey += Avalon::StringUtil::ConvertToString<UInt32>(i + 1);
			itemId.push_back((UInt32)task->GetVar(idKey.c_str()));
		}
		
		cost = (UInt32)task->GetVar("ncs") * num;
		if (cost == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "makeup dungeon cost is err! cost = " << cost << LogStream::eos;
			return false;
		}

		bool allExp = true;
		for (int i = 0; i < sz; ++i)
		{
			ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemId[i]);
			if (itemdata && itemdata->subType != ST_ITEM_EXP)
			{
				allExp = false;
				break;
			}
		}

		if (allExp && GetOwner()->IsFullLevel())
		{
			GetOwner()->SendNotify(3132);
			return false;
		}

		if (!GetOwner()->CheckRmMoney(ST_ITEM_GOLD, cost))
		{
			//金币不够
			GetOwner()->SendNotify(1000014);
			return false;
		}

		GetOwner()->RemoveGold(reason.c_str(), cost);
	}

	RewardGroup* grp = GetOwner()->GetRewardGroup();
	for (int i = 0; i < sz; ++i)
	{
		grp->AddReward(itemId[i], (UInt32)addNum[i]);
	}

	for (int i = 0; i < num; ++i)
		GetOwner()->AddRewards(reason.c_str(), grp, true);

	if (num >= ct)
	{
		task->SetVar("ct", 0);
		task->SetStatus(TASK_OVER);
	}
	else
	{
		task->SetVar("ct", ct - num);
	}
	
	DebugLogStream << PLAYERINFO(GetOwner()) << "submit task id = " << task->GetID() << " num = " << num << LogStream::eos;

	task->SaveToDB();
	return true;
}

UInt32 ActiveTaskMgr::GetDungeonMissNum(UInt32 subType)
{
	//爬塔副本
	if (subType == DST_SIWANGZHITA)
	{
		std::ostringstream osTower;
		osTower << COUNTER_TOWER_RESET_REMAIN_TIMES << "_l";
		return GetOwner()->GetCounter(osTower.str().c_str());
	}

	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(subType);
	if (!dungeonTimesData)
	{
		return 0;
	}

	std::ostringstream os;
	os << dungeonTimesData->usedTimesCounter.c_str() << "_l";

	UInt32 lastCount = GetOwner()->GetCounter(os.str().c_str());
	return lastCount;
}

void ActiveTaskMgr::UpdateDungeonLastNum()
{
	class DungeonVisitor : public Avalon::DataEntryVisitor<DungeonTimesDataEntry>
	{
	public:
		DungeonVisitor(Player* player) :m_pPlayer(player){}

		bool operator()(DungeonTimesDataEntry* dataEntry)
		{
			if (!m_pPlayer)
			{
				return false;
			}

			std::ostringstream os;
			os << dataEntry->usedTimesCounter.c_str() << "_l";
			const CounterConfig* cfg = CounterCfg::Instance()->GetCycleConfig(os.str());
			if (!cfg)
			{
				CounterCfg::Instance()->RegCounter(os.str().c_str(), 0);
			}

			UInt32 maxTime = SSActivityDungeonMgr::Instance()->GetDungeonMaxTimes(m_pPlayer, (DungeonSubType)dataEntry->type);
			if (0 == maxTime)
			{
				ErrorLogStream << PLAYERINFO(m_pPlayer) << "maxTime = 0" << LogStream::eos;
				
				m_pPlayer->SetCounter(os.str().c_str(), 0);
				return true;
			}

			UInt32 restNum = 0;
			UInt32 dailyCount = 0;
			
			dailyCount = m_pPlayer->GetCounterWithoutCycle(dataEntry->usedTimesCounter.c_str());

#ifdef _DEBUG
			DebugLogStream << PLAYERINFO(m_pPlayer) << " dungeon(" << dataEntry->type << ") daily count(" << dailyCount << ")." << LogStream::eos;
#endif

			if (maxTime < dailyCount)
			{
				ErrorLogStream << PLAYERINFO(m_pPlayer) << "maxTime(" << maxTime << ") < dailyCount(" << dailyCount << ")" << LogStream::eos;

				m_pPlayer->SetCounter(os.str().c_str(), 0);
				return true;
			}
			
			restNum = maxTime - dailyCount;
			m_pPlayer->SetCounter(os.str().c_str(), restNum);
			return true;
		}

	private:
		Player*	m_pPlayer;
	};

	DungeonVisitor visitor(GetOwner());
	DungeonTimesDataEntryMgr::Instance()->Visit(visitor);

	std::ostringstream osTower;
	osTower << COUNTER_TOWER_RESET_REMAIN_TIMES << "_l";

	const CounterConfig* towerCfg = CounterCfg::Instance()->GetCycleConfig(osTower.str());
	if (!towerCfg)
	{
		CounterCfg::Instance()->RegCounter(osTower.str().c_str(), 0);
	}

	UInt32 lastTowerNum = GetOwner()->GetCounterWithoutCycle(COUNTER_TOWER_RESET_REMAIN_TIMES);
	GetOwner()->SetCounter(osTower.str().c_str(), lastTowerNum);
}

bool ActiveTaskMgr::IsCharge(UInt32 ActiveId)
{
	if (ActiveId == AID_CHARGE_FIRST || ActiveId == AID_CHARGE_TOT)
	{
		return true;
	}

	return false;
}

bool ActiveTaskMgr::IsSevenDay(UInt32 ActiveId)
{
	if (ActiveId >= AID_ONE_DAY && ActiveId <= AID_SEVEN_CHARGE)
	{
		return true;
	}

	return false;
}

bool ActiveTaskMgr::CanTakeSevenDayTask(UInt32 ActiveId)
{
	//判断七日活动是否结束
	UInt32 day = GetOwner()->GetCreateFromNowOnDay();

	if (IsSevenDay(ActiveId))
	{	
		if (day > 10)
		{
			return false;
		}
	}

	// 判断七日活动是否开始
	switch (ActiveId)
	{
	case AID_ONE_DAY: return true;	
	case AID_TWO_DAY:
		if (day < 2)
		{
			return false;
		}
		break;
	case AID_THREE_DAY:
		if (day < 3)
		{
			return false;
		}
		break;
	case AID_FOUR_DAY:
		if (day < 4)
		{
			return false;
		}
		break;
	case AID_FIVE_DAY:
		if (day < 5)
		{
			return false;
		}
		break;
	case AID_SIX_DAY:
		if (day < 6)
		{
			return false;
		}
		break;
	case AID_SEVEN_DAY:
		if (day < 7)
		{
			return false;
		}
		break;
	case AID_SEVEN_CHARGE:return true;
	default:return false;
	}

	return true;
}

bool ActiveTaskMgr::IsMonthCard(UInt32 ActiveId)
{
	if (ActiveId == AID_MONTH_CARD)
	{
		return true;
	}

	return false;
}

bool ActiveTaskMgr::IsSignIn(UInt32 ActiveId)
{
	if (ActiveId == AID_SIGN_IN)
	{
		return true;
	}

	return false;
}

bool ActiveTaskMgr::IsSignInTotal(UInt32 ActiveId)
{
	if (ActiveId == AID_SIGN_IN_BONUS)
	{
		return true;
	}

	return false;
}

void ActiveTaskMgr::UpdateOnlineTimeBonus()
{
	GiftPhaseData* data = GiftPhaseDataMgr::Instance()->GetNextGift(GPT_ONLINE_TIME, m_onlineTimeBonusId);
	if (!data)
	{
		return;
	}

	if (data->value > GetOwner()->GetTotelOnlineTime())
	{
		SendSyncPhaseGift(data->id, 0);
		m_waitGiftData = data;
	}
	else
	{
		SendSyncPhaseGift(data->id, 1);
		m_waitGiftData = NULL;
	}
}

void ActiveTaskMgr::UpdateReachLvBonus()
{
	GiftPhaseData* data = GiftPhaseDataMgr::Instance()->GetNextGift(GPT_REACH_LEVEL, m_reachLvBonusId);
	if (!data)
	{
		return;
	}

	if (data->value > GetOwner()->GetLevel())
	{
		SendSyncPhaseGift(data->id, 0);
	}
	else
	{
		SendSyncPhaseGift(data->id, 1);
	}
}

void ActiveTaskMgr::InitJarAct()
{
	class InitJarActVistor : public Avalon::DataEntryVisitor<JarBonusDataEntry>
	{
	public:
		InitJarActVistor(Player* player) { m_player = player; }

		bool operator()(JarBonusDataEntry *dataEntry)
		{
			if (!dataEntry)
			{
				return false;
			}

			if (dataEntry->type <= JT_NORMAL_MAX || dataEntry->type >= JT_ACT_MAX)
			{
				return true;
			}

			std::ostringstream os;
			os << ACTJAR_DIS_NUM_PRE << dataEntry->id;
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
			if (!counterConfig)
			{
				CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
			}

			UInt32 upTime = m_player->GetCounterMgr().GetCounterUpdateTime(os.str().c_str());
			if (upTime == 0)
			{
				m_player->SetCounter(os.str().c_str(), dataEntry->discountMaxNum);
			}

			return true;
		}

		Player* m_player;
	};

	if (ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_1) ||
		ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_2) ||
		ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_3) ||
		ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_4) ||
		ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_5) ||
		ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_6))
	{
		InitJarActVistor vistor(GetOwner());
		JarBonusDataMgr::Instance()->Visit(vistor);
	}
}

void ActiveTaskMgr::SendOnlineNotifyInfo()
{
	CLProtocol::SCInitNotifyList list;

	//活动罐子
	UInt32 max_jar_id = 0;
	if (ActivityMgr::Instance()->IsInActivity(AID_JAR_ACT_1, GetOwner())
		&& !GetOwner()->IsReadedNotify(NT_JAR_OPEN, AID_JAR_ACT_1))
	{
		NotifyInfo info;
		info.type = (UInt32)NT_JAR_OPEN;
		info.param = AID_JAR_ACT_1;
		list.notifList.push_back(info);
	}
	for (UInt32 i = AID_JAR_ACT_2; i <= AID_JAR_ACT_MAX; ++i)
	{
		if (ActivityMgr::Instance()->IsInActivity(i, GetOwner()))
		{
			if (i > max_jar_id)
			{
				max_jar_id = i;
			}
		}
	}

	if (max_jar_id > 0 && !GetOwner()->IsReadedNotify(NT_JAR_OPEN, max_jar_id))
	{
		NotifyInfo info;
		info.type = (UInt32)NT_JAR_OPEN;
		info.param = max_jar_id;
		list.notifList.push_back(info);
	}

	GetOwner()->SendProtocol(list);
}

void ActiveTaskMgr::SendUpdateNotifyInfo(UInt32 type, UInt32 param)
{
	CLProtocol::SCUpdateNotifyList update;
	update.notify.type = type;
	update.notify.param = param;
	GetOwner()->SendProtocol(update);
}

void ActiveTaskMgr::SendSyncPhaseGift(UInt32 giftId, UInt8 canTake)
{
	CLProtocol::SceneSyncPhaseGift protocol;
	protocol.giftId = giftId;
	protocol.canTake = canTake;
	GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::RegistGameEvent()
{
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_EQUIP_REMAKE_QL, this, &ActiveTaskMgr::OnRandQL);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_PHONE_BIND, this, &ActiveTaskMgr::OnEventBindPhone);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_LEVEL_UP, this, &ActiveTaskMgr::OnEventLevelUp);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_CHARGE_RMB, this, &ActiveTaskMgr::OnEventChargeMoney);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_CLEAR_DUNGEON, this, &ActiveTaskMgr::OnEventCallback);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_COMPLETE_PK, this, &ActiveTaskMgr::OnEventCallback);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_ENTER_DUNGEON, this, &ActiveTaskMgr::OnEventCallback);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_TASK_FINISHED, this, &ActiveTaskMgr::OnEventCallback);
	REGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_ON_ONLINE, this, &ActiveTaskMgr::OnEventCallback);
}

void ActiveTaskMgr::UnRegistGameEvent()
{
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_EQUIP_REMAKE_QL, this, &ActiveTaskMgr::OnRandQL);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_PHONE_BIND, this, &ActiveTaskMgr::OnEventBindPhone);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_LEVEL_UP, this, &ActiveTaskMgr::OnEventLevelUp);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_CHARGE_RMB, this, &ActiveTaskMgr::OnEventChargeMoney);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_CLEAR_DUNGEON, this, &ActiveTaskMgr::OnEventCallback);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_COMPLETE_PK, this, &ActiveTaskMgr::OnEventCallback);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_ENTER_DUNGEON, this, &ActiveTaskMgr::OnEventCallback);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_TASK_FINISHED, this, &ActiveTaskMgr::OnEventCallback);
	UNREGIST_PLAYER_EVT_HANDLER(GetOwner()->GetID(), ActiveTaskMgr, ET_ON_ONLINE, this, &ActiveTaskMgr::OnEventCallback);
}

UInt32 ActiveTaskMgr::GetActIdFromJarId(UInt32 jarId)
{
	switch (jarId)
	{
	case 601:	return AID_JAR_ACT_2;
	case 602:	return AID_JAR_ACT_3;
	case 603:	return AID_JAR_ACT_4;
	case 604:	return AID_JAR_ACT_5;
	case 605:   return AID_JAR_ACT_6;
	case 501:	return AID_JAR_ACT_1;
	default:			return 0;
	}
}

UInt32 ActiveTaskMgr::GetJarIdFromActId(UInt32 actId)
{
	switch (actId)
	{
	case AID_JAR_ACT_2:	return 601;
	case AID_JAR_ACT_3:	return 602;
	case AID_JAR_ACT_4:	return 603;
	case AID_JAR_ACT_5:	return 604;
	case AID_JAR_ACT_6: return 605;
	case AID_JAR_ACT_1:	return 501;
	default:			return 0;
	}
}

void ActiveTaskMgr::_AcceptChangeFashionOpWelfareActTask(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;
	task->curNum = taskData.completeNum;

	if (task->curNum == UINT32_MAX)
	{
		if (task->state != TASK_OVER)
		{
			task->state = TASK_OVER;
		}
	}
	else if (task->curNum > 0)
	{
		bool itemEnough = true;
		if (taskData.variables.size() == 2
			&& GetOwner()->GetItemNum(taskData.variables[0]) < taskData.variables[1])
		{
			itemEnough = false;
		}

		if (itemEnough && taskData.counters.size() == 1 &&
			GetOwner()->GetItemNum(taskData.counters[0].currencyId) < taskData.counters[0].value)
		{
			itemEnough = false;
		}

		if (itemEnough && task->state != TASK_FINISHED)
		{
			task->state = TASK_FINISHED;
		}
		else if (!itemEnough && task->state != TASK_UNFINISH)
		{
			task->state = TASK_UNFINISH;
		}
	}
	else
	{
		if (task->state != TASK_OVER)
		{
			// 兑换次数为0
			task->state = TASK_OVER;
		}
	}
}

void ActiveTaskMgr::_UpdateChangeFashionWelfareOpActTasks(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		bool sync = false;
		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			continue;
		}

		if (task->curNum == UINT32_MAX)
		{
			if (task->state != TASK_OVER)
			{
				task->state = TASK_OVER;
				SaveOpTaskDB(task);
				sync = true;
			}
		}
		else if (task->curNum > 0)
		{
			bool itemEnough = true;
			if (taskData.variables.size() == 2
				&& GetOwner()->GetItemNum(taskData.variables[0]) < taskData.variables[1])
			{
				itemEnough = false;
			}

			if (itemEnough && taskData.counters.size() == 1 &&
				GetOwner()->GetItemNum(taskData.counters[0].currencyId) < taskData.counters[0].value)
			{
				itemEnough = false;
			}

			if (itemEnough && task->state != TASK_FINISHED)
			{
				task->state = TASK_FINISHED;
				SaveOpTaskDB(task);
				sync = true;
			}
			else if (!itemEnough && task->state != TASK_UNFINISH)
			{
				task->state = TASK_UNFINISH;
				SaveOpTaskDB(task);
				sync = true;
			}
		}
		else
		{
			if (task->state != TASK_OVER)
			{
				// 兑换次数为0
				task->state = TASK_OVER;
				SaveOpTaskDB(task);
				sync = true;
			}
		}
		if (sync)
		{
			protocol.tasks.push_back(*task);
		}
	}

	if (protocol.tasks.size() > 0)
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool ActiveTaskMgr::_OnSubmitChangeFashionWelfareOpActTask(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData) return false;

	InfoLogStream << PLAYERINFO(GetOwner()) << "start submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	if (task->state != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") state error! state = " << task->state << LogStream::eos;
		return false;
	}

	//消耗
	if (taskData->variables.size() != 2 && taskData->counters.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") data's variables size and counters size error!" << LogStream::eos;
		return false;
	}

	Int32 num = (Int32)task->curNum;
	num -= 1;
	if (num < 0)
	{
		task->state = TASK_OVER;
		task->curNum = 0;
		ErrorLogStream << PLAYERINFO(GetOwner()) << "exchange num(" << num << ") reach the limit(data: " << taskData->dataId << ")!" << LogStream::eos;
		return false;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_CHANGE_FASHION_WELFARE);
	//道具消耗
	if (taskData->variables.size() == 2)
	{
		if (!GetOwner()->RemoveItem(reason.c_str(), taskData->variables[0], taskData->variables[1]))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << taskData->variables[0] << ", num: " << taskData->variables[1] << ") failed." << LogStream::eos;
			return false;
		}
	}

	if (taskData->counters.size() == 1)
	{
		if (!GetOwner()->RemoveItem(reason.c_str(), taskData->counters[0].currencyId, taskData->counters[0].value))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << taskData->counters[0].currencyId << ", num: " << taskData->counters[0].value << ") failed." << LogStream::eos;
			return false;
		}
	}

	task->curNum = num;
	SaveOpTaskDB(task);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	return true;
}

void ActiveTaskMgr::_AcceptChangeFashionExchangeOpActTask(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;
	task->curNum = taskData.completeNum;

	if (task->curNum == UINT32_MAX)
	{
		if (task->state != TASK_OVER)
		{
			task->state = TASK_OVER;
		}
	}
	else if (task->curNum > 0)
	{
		bool itemEnough = true;
		UInt32 consItemId = taskData.variables[0];
		if (GetOwner()->GetItemNum(consItemId) < taskData.variables[1])
		{
			itemEnough = false;
		}
		if (itemEnough == true
			&& GetOwner()->GetItemMgr().GetNumByPartAndColor(taskData.variables2[0], taskData.variables2[1], true, PACK_FASHION) < taskData.variables2[2])
		{
			itemEnough = false;
		}
		if (itemEnough && task->state != TASK_FINISHED)
		{
			task->state = TASK_FINISHED;
		}
		else if (!itemEnough && task->state != TASK_UNFINISH)
		{
			task->state = TASK_UNFINISH;
		}
	}
	else
	{
		if (task->state != TASK_OVER)
		{
			// 兑换次数为0
			task->state = TASK_OVER;
		}
	}
}

void ActiveTaskMgr::_UpdateChangeFashionExchangeOpActTasks(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		bool sync = false;
		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			continue;
		}
		if (task->curNum == UINT32_MAX)
		{
			if (task->state != TASK_OVER)
			{
				task->state = TASK_OVER;
				SaveOpTaskDB(task);
				sync = true;
			}
		}
		else if (task->curNum > 0)
		{
			bool itemEnough = true;
			UInt32 consItemId = taskData.variables[0];
			if (GetOwner()->GetItemNum(consItemId) < taskData.variables[1])
			{
				itemEnough = false;
			}

			if (itemEnough == true
				&& GetOwner()->GetItemMgr().GetNumByPartAndColor(taskData.variables2[0], taskData.variables2[1], true, PACK_FASHION) < taskData.variables2[2])
			{
				itemEnough = false;
			}

			if (itemEnough && task->state != TASK_FINISHED)
			{
				task->state = TASK_FINISHED;
				SaveOpTaskDB(task);
				sync = true;
			}
			else if (!itemEnough && task->state != TASK_UNFINISH)
			{
				task->state = TASK_UNFINISH;
				SaveOpTaskDB(task);
				sync = true;
			}
		}
		else
		{
			if (task->state != TASK_OVER)
			{
				// 兑换次数为0
				task->state = TASK_OVER;
				SaveOpTaskDB(task);
				sync = true;
			}
		}
		if (sync)
		{
			protocol.tasks.push_back(*task);
		}
	}

	if (protocol.tasks.size() > 0)
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool ActiveTaskMgr::_OnSubmitChangeFashionExchangeOpActTask(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData, ObjID_t fashionId)
{
	if (!opAct || !task || !taskData) return false;

	InfoLogStream << PLAYERINFO(GetOwner()) << "start submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	if (task->state != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") state error! state = " << task->state << LogStream::eos;
		return false;
	}

	Item* fashion = GetOwner()->FindItem(fashionId);
	if (fashion == NULL)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param err!" << LogStream::eos;
		return false;
	}

	if (taskData->variables.size() != 2)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") data's variables size error!" << LogStream::eos;
		return false;
	}

	if (taskData->variables2.size() != 3)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Task(" << task->dataId << ") data's variables2 size error!" << LogStream::eos;
		return false;
	}

	UInt8 subtype = taskData->variables2[0];
	UInt8 color = taskData->variables2[1];
	UInt32 consNum = taskData->variables2[2];
	if (fashion->GetSubType() != subtype || fashion->GetQuality() != color)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "exchange fashion id(" << fashion->GetDataID() << ") subtype or color error!" << LogStream::eos;
		return false;
	}

	if (GetOwner()->GetItemMgr().GetNumByPartAndColor(subtype, color, true, PACK_FASHION) < consNum)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "item not enough" << LogStream::eos;
		return false;
	}

	UInt32  consumeItemId = taskData->variables[0];
	UInt32  consumeItemNum = taskData->variables[1];
	if (GetOwner()->GetItemNum(consumeItemId) < consumeItemNum)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Consume item(" << consumeItemId << ") num(" << consumeItemNum << ") are not enough!" << LogStream::eos;
		return false;
	}

	Int32 num = (Int32)task->curNum;
	num -= 1;
	if (num < 0)
	{
		task->state = TASK_OVER;
		task->curNum = 0;
		ErrorLogStream << PLAYERINFO(GetOwner()) << "exchange num(" << num <<
			") reach the limit(data: " << taskData->dataId << ")!" << LogStream::eos;
		return false;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_CHANGE_FASHION_EXCHANGE);
	//消耗
	if (!GetOwner()->RemoveItem(reason.c_str(), consumeItemId, consumeItemNum))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << consumeItemId << 
			", num: " << consumeItemNum << ") failed." << LogStream::eos;
		return false;
	}

	if (GetOwner()->ReduceItemNum(reason.c_str(), fashion, 1) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove fashion(id: " << fashionId << ", num: " << 1 << ") failed." << LogStream::eos;
		return false;
	}

	task->curNum = num;
	SaveOpTaskDB(task);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	return true;
}

void ActiveTaskMgr::ReturnChargeActivity(UInt32 chargeNum)
{
	if (chargeNum == 0 || !GetOwner()->IsVeteranReturn()) return;

	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_RETURN_CHARGE_REBATE, opActVec);

	if (opActVec.empty())
		return;

	// 充值返利是账号活动
	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter: opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		if (task->state == TASK_OVER)
		{
			continue;
		}

		ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), iter->dataId);
		if (NULL == record)
		{
			record = new ActivityAccRecord();
			record->guid = CLRecordClient::Instance()->GenGuid();
			record->actId = iter->dataId;
			record->curNum = chargeNum;
			record->state = record->curNum >= iter->completeNum ? TASK_FINISHED:TASK_UNFINISH;
			ActivityMgr::Instance()->AddAccActivityRecord(GetOwner()->GetAccID(), record);
			ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record, true);
		}
		else
		{
			record->curNum += chargeNum;
			record->state = record->curNum >= iter->completeNum ? TASK_FINISHED : TASK_UNFINISH;
			ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record);
		}

		task->curNum = record->curNum;
		task->state = record->state;

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::ReturnUseHellTicket(UInt32 useNum)
{
	if (!GetOwner()->IsVeteranReturn()) return;

	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_CHALLENGE_HELL, opActVec);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		if (task->state == TASK_OVER)
		{
			continue;
		}

		if (iter->variables.empty())
		{
			ErrorLogStream << "task variables empty id:"<< iter->dataId << LogStream::eos;
			continue;
		}

		// 账号活动
		if (iter->variables[0] != 0)
		{
			ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), iter->dataId);
			if (NULL == record)
			{
				record = new ActivityAccRecord();
				record->guid = CLRecordClient::Instance()->GenGuid();
				record->actId = iter->dataId;
				record->curNum = useNum;
				record->state = record->curNum >= iter->completeNum ? TASK_FINISHED : TASK_UNFINISH;
				ActivityMgr::Instance()->AddAccActivityRecord(GetOwner()->GetAccID(), record);
				ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record, true);
			}
			else
			{
				record->curNum += useNum;
				record->state = record->curNum >= iter->completeNum ? TASK_FINISHED : TASK_UNFINISH;
				ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record);
			}

			task->curNum = record->curNum;
			task->state = record->state;
		}
		else // 角色活动
		{
			task->AddCurNum(useNum, iter->completeNum, true);
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::ReturnEnergyShop()
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_ENERGY_EXCHANGE, opActVec);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		if (task->state == TASK_OVER)
		{
			continue;
		}

		if (iter->variables.empty() || iter->variables2.empty())
		{
			ErrorLogStream << "task variables empty id:" << iter->dataId << LogStream::eos;
			continue;
		}

		// 账号活动
		UInt32 tempState = GetOwner()->GetAccountCounter(ACC_COUNTER_ENERGY_COIN) >= iter->variables[0] ? TASK_FINISHED : TASK_UNFINISH;
		if (iter->variables2[0] != 0)
		{
			ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), iter->dataId);
			if (NULL != record)
			{
				if (record->state == TASK_OVER)
					tempState = task->state;
				else
					record->state = tempState;

				ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record);
			}
		}
		
		task->state = tempState;

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::_CheckReturnExpire()
{
	if (GetOwner()->GetAgainReturn() == 0)
		return;

	GetOwner()->SetAgainReturn(0);

	InfoLogStream << "Again Return PlayerId:" << GetOwner()->GetID() << " Name:" << GetOwner()->GetName() << LogStream::eos;

	// 删除活动记录
	ActivityMgr::Instance()->DelAccActivityRecord(GetOwner()->GetAccID());
	ActivityMgr::Instance()->DBDelAccActivityRecord(GetOwner()->GetAccID());

	// 再次回归，清空精力币
	GetOwner()->SetAccountCounterSyncWorld(ACC_COUNTER_ENERGY_COIN, 0);
}

void ActiveTaskMgr::RefreshDailyChallengeHell(UInt32 isAcc)
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_CHALLENGE_HELL, opActVec);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		if (iter->variables.empty() || iter->variables2.empty())
		{
			ErrorLogStream << "task variables empty id:" << iter->dataId << LogStream::eos;
			continue;
		}

		// 如果是非0则刷新
		if (iter->variables2[0] == 0)
			continue;

		if (isAcc != iter->variables[0])
			continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		task->curNum = 0;
		task->state = TASK_UNFINISH;
		SaveOpTaskDB(task);

		// 如果是账号活动，还得更新账号活动记录
		if (iter->variables[0] != 0)
		{
			ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), iter->dataId);
			if (NULL != record)
			{
				record->curNum = task->curNum;
				record->state = task->state;
				ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record);
			}
		}

		protocol.tasks.push_back(*task);
	}
	
	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::HandleReturnActExpire()
{
	InfoLogStream << "HandleReturnActExpire PlayerId:"<< GetOwner()->GetID() <<" Name:"<< GetOwner()->GetName() << LogStream::eos;

	//检查回归活动数据是否过期
	_CheckReturnExpire();

	_ReturnTaskExpire(OAT_RETURN_PRESENT);
	_ReturnTaskExpire(OAT_ENERGY_EXCHANGE);
	_ReturnTaskExpire(OAT_RETURN_CHARGE_REBATE);
	_ReturnTaskExpire(OAT_CHALLENGE_HELL);
}

void ActiveTaskMgr::LoadWeekSign(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt32 opActType = callback->GetData("act_type");

		WeekSignData data;
		data.actId = callback->GetData("act_id");
		data.weekTime = callback->GetData("week_time");
		data.weekNum = callback->GetData("week_num");

		std::string weekIdStr = callback->GetData("week_id").GetString();
		data.EncodeWeekId(weekIdStr);

		std::string weekBoxStr = callback->GetData("week_box").GetString();
		data.EncodeBoxData(weekBoxStr);

		m_WeekSignMap[opActType] = data;
	}
}

void ActiveTaskMgr::HandleWeekSignSum(UInt32 opActType, UInt32 weekId)
{
	if (opActType == OAT_WEEK_SIGN_ACTIVITY)
	{
		if ( !ActivityMgr::Instance()->IsInOpActivityByTmpType(opActType, GetOwner()->GetLevel()))
		{
			CLProtocol::SceneWeekSignRewardRes res;
			res.retCode = ErrorCode::OPERATE_ACTIVITY_CLOSE;
			GetOwner()->SendProtocol(res);
			return;
		}
	}
	else if (opActType == OAT_WEEK_SIGN_NEW_PLAYER)
	{
		if ( !GetOwner()->GetPlayerActivityInfo().OpActivitiesIsOpen(OpActivityType(opActType)))
		{
			CLProtocol::SceneWeekSignRewardRes res;
			res.retCode = ErrorCode::OPERATE_ACTIVITY_CLOSE;
			GetOwner()->SendProtocol(res);
			return;
		}
	}
	else
	{
		ErrorLogStream << "weekSign opActType error type:" << opActType << LogStream::eos;
		return;
	}

	CLProtocol::SceneWeekSignRewardRes res;
	do
	{
		auto iter = m_WeekSignMap.find(opActType);
		if (iter == m_WeekSignMap.end())
		{
			res.retCode = ErrorCode::OPERATE_DATA_ERROR;
			break;
		}

		bool isYet = false;
		WeekSignData& signData = iter->second;
		for (auto& it : signData.weekIdVec)
		{
			if (it != weekId)
				continue;

			isYet = true;
		}

		if (isYet)
		{
			res.retCode = ErrorCode::OPERATE_GET_REWARD_REPEATED;
			break;
		}

		if (signData.weekNum < weekId)
		{
			res.retCode = ErrorCode::OPERATE_NOT_FINISH;
			break;
		}

		const WeekSumReward* reward = WeekSignSumDataEntryMgr::Instance()->GetReward(opActType, weekId);
		if (NULL == reward)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) <<"weekSign get reward error opActType:"<< opActType 
				<< " weekId"<< weekId << LogStream::eos;
			res.retCode = ErrorCode::OPERATE_GET_REWARD_ERROR;
			break;
		}

		{
			ItemReward itemReward(reward->rewardId, reward->rewardNum, 0, 0);
			ItemRewardVec rewardVec;
			rewardVec.push_back(itemReward);
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_WEEK_SIGN_WEEK_REWARD, opActType, weekId);
			RewardGroup rewardGroup(rewardVec);
			GetOwner()->AddRewards(reason.c_str(), &rewardGroup, true);
		}

		_ModifyWeekSignData(opActType, WeekSignData::ID_VEC_SAVE, weekId, 0, NULL);

		res.retCode = ErrorCode::SUCCESS;
	} while (0);

	GetOwner()->SendProtocol(res);
}

void ActiveTaskMgr::WeekSignTrigger(UInt32 weekSignActType)
{
	if (ActivityMgr::Instance()->IsInOpActivityByTmpType(OpActivityType::OAT_WEEK_SIGN_ACTIVITY, GetOwner()->GetLevel()))
	{
		if (_WeekSignTrigger(OAT_WEEK_SIGN_ACTIVITY, weekSignActType))
		{
			_CheckWeekSignFinish(OAT_WEEK_SIGN_ACTIVITY);
		}
	}

	if (GetOwner()->GetPlayerActivityInfo().OpActivitiesIsOpen(OAT_WEEK_SIGN_NEW_PLAYER))
	{
		if (_WeekSignTrigger(OAT_WEEK_SIGN_NEW_PLAYER, weekSignActType))
		{
			_CheckWeekSignFinish(OAT_WEEK_SIGN_NEW_PLAYER);
		}
	}
}

void ActiveTaskMgr::HandleWeekSignLottery(UInt32 opActType, UInt32 actId, const ItemRewardVec& rewardVec)
{
	if (rewardVec.empty())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) <<" WeekSign Reward Empty! opActType:"<<opActType<<" ActId:"<< actId << LogStream::eos;
		return;
	}

	OpActTask* task = FindNewOpActTask(actId);
	if (NULL == task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "HandleWeekSignLottery Not Find Task:" << actId << LogStream::eos;
		return;
	}

	if (task->state == TASK_OVER)
	{
		return;
	}

	task->state = TASK_OVER;
	SaveOpTaskDB(task);

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, task->dataId, task->curNum);
	auto reward = RewardGroup(rewardVec);
	GetOwner()->AddRewards(reason.c_str(), &reward);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	_ModifyWeekSignData(opActType, WeekSignData::BOX_VEC_SAVE, 0, actId, &rewardVec);
}


void ActiveTaskMgr::GMWeekSignNum(UInt32 num)
{
	auto iter = m_WeekSignMap.find(OAT_WEEK_SIGN_ACTIVITY);
	if (iter == m_WeekSignMap.end())
		return;

	iter->second.weekNum = num;
	iter->second.weekIdVec.clear();

	_RefreshWeekTimeBox(OAT_WEEK_SIGN_ACTIVITY, iter->second);

	_NotifyWeekSignSumData(OAT_WEEK_SIGN_ACTIVITY);
}

void ActiveTaskMgr::GMResetWeekSignTask()
{
	UInt32 actType = OAT_WEEK_SIGN_ACTIVITY;
	auto iter = m_WeekSignMap.find(actType);
	if (iter == m_WeekSignMap.end())
		return;

	// 刷新活动任务
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(actType, opActVec);
	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& itTask : opActVec)
	{
		if (NULL == itTask) continue;
		if (itTask->variables.empty())
		{
			ErrorLogStream << "task variables empty id:" << itTask->dataId << LogStream::eos;
			continue;
		}

		OpActTask* task = FindNewOpActTask(itTask->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << itTask->dataId << ")!" << LogStream::eos;
			continue;
		}

		if (itTask->variables[0] == WEEK_SIGN_ACT_ACTIVE)
		{
			std::string key("lf");
			task->SetParamVar(key, 0);
		}

		task->curNum = 0;
		task->state = TASK_UNFINISH;
		SaveOpTaskDB(task);

		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);

	// 刷新周时间，宝箱奖励
	UInt32 act_id = ActivityMgr::Instance()->GetOpenActivityIdByTmplateType(actType);

	iter->second.actId = act_id;
	iter->second.weekTime = UInt32(getWeekBeginTime(CURRENT_TIME));
	iter->second.signBoxVec.clear();
	_NotifyWeekSignBoxData(actType);

	_RefreshWeekTimeBox(actType, iter->second);
}

void ActiveTaskMgr::GMFinishWeekSignTask()
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_WEEK_SIGN_ACTIVITY, opActVec);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		if (task->state != TASK_UNFINISH)
		{
			continue;
		}

		if (iter->variables.empty())
		{
			ErrorLogStream << "task variables empty id:" << iter->dataId << LogStream::eos;
			continue;
		}

		switch (iter->variables[0])
		{
		case WEEK_SIGN_ACT_ACTIVE:
		{
			std::string key("lf");
			task->SetParamVar("lf", 0);
			task->curNum = iter->completeNum;
			task->state = TASK_FINISHED;

			break;
		}
		case WEEK_SIGN_ACT_DAILY:
		case WEEK_SIGN_ACT_DUNGEON:
		{
			task->curNum = iter->completeNum;
			task->state = TASK_FINISHED;
			break;
		}
		default:return;
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::GMLimitTimeHellNum(UInt32 num)
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_LIMIT_TIME_HELL, opActVec);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		task->curNum = num;

		if (task->curNum >= iter->completeNum && task->state == TASK_UNFINISH)
		{
			task->state = TASK_FINISHED;
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::_ReturnTaskExpire(UInt32 type)
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(type, opActVec);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task) continue;

		switch (type)
		{
		case OAT_RETURN_PRESENT:
		{
			task->curNum = 0;
			task->state = TASK_FINISHED;
			if (iter->variables[0] != 0)
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter->dataId, task);
		}
		break;
		case OAT_ENERGY_EXCHANGE:
		{
			task->curNum = 0;
			task->state = GetOwner()->GetAccountCounter(ACC_COUNTER_ENERGY_COIN) >= iter->variables[0] ? TASK_FINISHED : TASK_UNFINISH;

			if (iter->variables2[0] != 0)
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter->dataId, task);
		}
		break;
		case OAT_RETURN_CHARGE_REBATE:
		case OAT_CHALLENGE_HELL:
		{
			task->curNum = 0;
			task->state = TASK_UNFINISH;

			if (iter->variables[0] != 0)
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter->dataId, task);
		}
		break;
		default: return;
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_RefreshActivityRecordByAcc(UInt32 accid, UInt32 actId, OpActTask* task)
{
	if (!task) return;

	ActivityAccRecord* accRecord = ActivityMgr::Instance()->FindAccActivityRecord(accid, actId);
	if (NULL != accRecord)
	{
		task->curNum = accRecord->curNum;
		task->state = accRecord->state;
	}
}

void ActiveTaskMgr::_AcceptReturnPresent(const OpActivityRegInfo* opAct)
{
	if (NULL == opAct || opAct->state != AS_IN || !GetOwner()->IsVeteranReturn()) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto& iter : opAct->tasks)
	{
		// variables2消耗,variables账号或者角色活动|玩家最小回归等级|玩家最大回归等级
		if (iter.variables.size() != 5 || iter.variables2.size() != 2)
		{
			ErrorLogStream << "task variables error taskid: " << opAct->dataId << LogStream::eos;
			continue;
		}

		if (GetOwner()->GetRoleReturnLevel() < iter.variables[1] || GetOwner()->GetRoleReturnLevel() > iter.variables[2])
			continue;

		OpActTask* canDoTask = FindNewOpActTask(iter.dataId);
		if (!canDoTask)
		{
			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(iter.dataId);
			if (!taskDataEntry)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data entry(" << iter.dataId << ")!" << LogStream::eos;
				continue;
			}

			canDoTask = new OpActTask(iter.dataId);
			canDoTask->curNum = 0;
			canDoTask->state = TASK_FINISHED;

			// 账号活动
			if (iter.variables[0] != 0)
			{
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter.dataId, canDoTask);
			}

			AddNewOpActTask(canDoTask);
			SaveOpTaskDB(canDoTask);
		}
		else
		{	
			// 如果是账号活动，需要去账号活动列表查数据
			if (iter.variables[0] != 0)
			{
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter.dataId, canDoTask);
				SaveOpTaskDB(canDoTask);
			}
		}

		protocol.tasks.push_back(*canDoTask);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::_AcceptReturnEnergyExchange(const OpActivityRegInfo* opAct)
{
	if (NULL == opAct || opAct->state != AS_IN || !GetOwner()->IsVeteranReturn()) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto& iter : opAct->tasks)
	{
		// variables兑换消耗，variables2角色或账号
		if (iter.variables.size() != 1 || iter.variables2.size() != 1)
		{
			ErrorLogStream << "task variables error taskid: " << opAct->dataId << LogStream::eos;
			continue;
		}

		OpActTask* canDoTask = FindNewOpActTask(iter.dataId);
		if (!canDoTask)
		{
			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(iter.dataId);
			if (!taskDataEntry)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data entry(" << iter.dataId << ")!" << LogStream::eos;
				continue;
			}

			canDoTask = new OpActTask(iter.dataId);
			canDoTask->curNum = 0;
			canDoTask->state = GetOwner()->GetAccountCounter(ACC_COUNTER_ENERGY_COIN) >= iter.variables[0] ? TASK_FINISHED : TASK_UNFINISH;

			// 账号活动
			if (iter.variables2[0] != 0)
			{
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter.dataId, canDoTask);
			}

			AddNewOpActTask(canDoTask);
		}
		else
		{
			// 如果是账号活动，需要去账号活动列表查数据
			if (iter.variables2[0] != 0)
			{
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter.dataId, canDoTask);
			}
		}

		SaveOpTaskDB(canDoTask);
		protocol.tasks.push_back(*canDoTask);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::_AcceptReturnActivityTask(const OpActivityRegInfo* opAct)
{
	if (NULL == opAct || opAct->state != AS_IN || !GetOwner()->IsVeteranReturn()) return;

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto& iter : opAct->tasks)
	{
		OpActTask* canDoTask = FindNewOpActTask(iter.dataId);
		if (!canDoTask)
		{
			auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(iter.dataId);
			if (!taskDataEntry)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data entry(" << iter.dataId << ")!" << LogStream::eos;
				continue;
			}

			canDoTask = new OpActTask(iter.dataId);
			canDoTask->curNum = 0;
			canDoTask->state = TASK_UNFINISH;

			// 账号活动
			if (iter.variables[0] != 0)
			{
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter.dataId, canDoTask);
			}

			AddNewOpActTask(canDoTask);
			SaveOpTaskDB(canDoTask);
		}
		else
		{
			// 账号活动
			if (iter.variables[0] != 0)
			{
				_RefreshActivityRecordByAcc(GetOwner()->GetAccID(), iter.dataId, canDoTask);
				SaveOpTaskDB(canDoTask);
			}
		}

		protocol.tasks.push_back(*canDoTask);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool ActiveTaskMgr::_OnSubmitReturnPresent(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData || !GetOwner()->IsVeteranReturn())
	{
		return false;
	}

	if (taskData->variables.size() != 5 || taskData->variables2.size() != 2)
	{
		ErrorLogStream << "task data variables error id:" << taskData->dataId << LogStream::eos;
		return false;
	}

	if (taskData->variables[3] != 0 && taskData->variables[4] != 0)
	{
		UInt32 curTime = CURRENT_TIME.Sec();
		if (curTime < taskData->variables[3] || curTime > taskData->variables[4])
		{
			ErrorLogStream << "task data variables time error id:" << taskData->dataId << LogStream::eos;
			return false;
		}
	}

	if (task->state == TASK_OVER)
	{
		return false;
	}

	// 检查钱够不够
	UInt32 consumeItemId = taskData->variables2[0];
	UInt32 consumeItemNum = taskData->variables2[1];
	if (GetOwner()->GetItemNum(consumeItemId) < consumeItemNum)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Consume item(" << consumeItemId << ") num(" << consumeItemNum << ") are not enough!" << LogStream::eos;
		GetOwner()->SendNotify(10004);
		return false;
	}

	// 账号活动
	if (taskData->variables[0] != 0)
	{
		bool isInsert = false;
		ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), taskData->dataId);
		if (NULL == record)
		{
			isInsert = true;
			record = new ActivityAccRecord();
			record->guid = CLRecordClient::Instance()->GenGuid();
			record->actId = taskData->dataId;
			record->curNum = 1;
			ActivityMgr::Instance()->AddAccActivityRecord(GetOwner()->GetAccID(), record);
		}
		else
		{
			record->curNum++;
		}

		if (record->curNum >= taskData->completeNum)
			record->state = TASK_OVER;

		ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record, isInsert);

		task->curNum = record->curNum;
		task->state = record->state;
	}
	else
	{
		task->curNum++;

		if (task->curNum >= taskData->completeNum)
			task->state = TASK_OVER;
	}

	SaveOpTaskDB(task);

	// 扣钱
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, taskData->dataId, task->curNum);
	GetOwner()->RemoveItem(reason.c_str(), consumeItemId, consumeItemNum);

	// 发奖励
	auto reward = RewardGroup(taskData->rewards);
	GetOwner()->AddRewards(reason.c_str(), &reward, true);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);
	return true;
}

bool ActiveTaskMgr::_OnSubmitEnergyShopExchangeItem(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData || !GetOwner()->IsVeteranReturn())
	{
		return false;
	}

	if (task->state == TASK_OVER)
	{
		return false;
	}

	if (taskData->variables.empty() || taskData->variables2.empty())
	{
		ErrorLogStream << "task data variables empty id:" << taskData->dataId << LogStream::eos;
		return false;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "start submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	// 检查钱够不够
	UInt32 consumeItemNum = taskData->variables[0];
	UInt32 energyCoin = GetOwner()->GetAccountCounter(ACC_COUNTER_ENERGY_COIN);
	if (energyCoin < consumeItemNum)
	{
		ErrorLogStream << "energyCoin Num Not Enough taskId:" << taskData->dataId << " consumeCoinNum:" << consumeItemNum << " currCoinNum:" << energyCoin << LogStream::eos;
		return false;
	}

	if (taskData->variables2[0] != 0)
	{
		bool isInsert = false;
		ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), taskData->dataId);
		if (NULL == record)
		{
			isInsert = true;
			record = new ActivityAccRecord();
			record->guid = CLRecordClient::Instance()->GenGuid();
			record->actId = taskData->dataId;
			record->curNum = 1;
			ActivityMgr::Instance()->AddAccActivityRecord(GetOwner()->GetAccID(), record);
		}
		else
		{
			record->curNum++;
		}

		if (record->curNum >= taskData->completeNum)
			record->state = TASK_OVER;

		ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record, isInsert);

		task->curNum = record->curNum;
		task->state = record->state;
	}
	else
	{
		task->curNum++;
		if (task->curNum >= taskData->completeNum)
			task->state = TASK_OVER;
	}
	
	SaveOpTaskDB(task);

	// 扣精力货币
	GetOwner()->SetAccountCounterSyncWorld(ACC_COUNTER_ENERGY_COIN, energyCoin - consumeItemNum);

	// 加奖励
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, taskData->dataId, task->curNum);
	auto reward = RewardGroup(taskData->rewards);
	GetOwner()->AddRewards(reason.c_str(), &reward, true);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	// 扣除精力币后，更新其他任务状态
	ReturnEnergyShop();

	return true;
}

bool ActiveTaskMgr::_OnSubmitReturnChargeRebate(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData || !GetOwner()->IsVeteranReturn())
	{
		return false;
	}

	if (task->state != TASK_FINISHED)
	{
		ErrorLogStream << "task id:"<< task->dataId<<" state:" << task->state << LogStream::eos;
		return false;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "start submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	// 账号活动
	ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), taskData->dataId);
	if (NULL == record)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Not Find ActivityAccRecord TaskId:" << taskData->dataId << LogStream::eos;
		return false;
	}

	record->state = TASK_OVER;
	task->state = TASK_OVER;
	ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record);
	SaveOpTaskDB(task);

	// 发奖励	
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, taskData->dataId, task->curNum);
	auto reward = RewardGroup(taskData->rewards);
	GetOwner()->AddRewards(reason.c_str(), &reward, true);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);
	return true;
}

bool ActiveTaskMgr::_OnSubmitChallengeHell(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData || !GetOwner()->IsVeteranReturn())
	{
		return false;
	}

	if (task->state != TASK_FINISHED)
	{
		ErrorLogStream << "task id:" << task->dataId << " state:" << task->state << LogStream::eos;
		return false;
	}

	if (taskData->variables.empty())
	{
		ErrorLogStream << "task variables empty id :" << task->dataId << LogStream::eos;
		return false;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "start submit task(" << task->dataId << "), state(" << task->state << ")." << LogStream::eos;

	// 账号活动
	if (taskData->variables[0] != 0)
	{
		ActivityAccRecord* record = ActivityMgr::Instance()->FindAccActivityRecord(GetOwner()->GetAccID(), taskData->dataId);
		if (NULL == record)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Not Find ActivityAccRecord TaskId:" << taskData->dataId << LogStream::eos;
			return false;
		}
		
		record->state = TASK_OVER;
		ActivityMgr::Instance()->UpdateAccActivityRecord(GetOwner()->GetAccID(), record);
	}
	
	task->state = TASK_OVER;
	SaveOpTaskDB(task);

	// 发奖励	
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_TASK_ACTIVE, taskData->dataId, task->curNum);
	auto reward = RewardGroup(taskData->rewards);
	GetOwner()->AddRewards(reason.c_str(), &reward, true);

	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	return true;
}

void ActiveTaskMgr::UpdateGiftRightOnlineTask()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	if (!GetOwner()->HaveGiftRight())
	{
		return;
	}

	VisitOpenOpActAndTaskDataByOpType(OAT_GIFT_RIGHT, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables.size() != 1)
		{
			return true;
		}

		if (taskData.variables[0] == OAGRTT_EVERY_DAY)
		{
			if (task->AddCurNum(1, taskData.completeNum, true))
			{
				GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD_LOGIN_TOTAL, GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD_LOGIN_TOTAL) + 1);
				sync = true;
			}
		}
		else if (taskData.variables[0] == OAGRTT_TOTAL_DAY)
		{
			if (taskData.variables2.size() != 1)
			{
				return true;
			}
			if (GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD_LOGIN_TOTAL) >= taskData.variables2[0])
			{
				if (task->AddCurNum(1, taskData.completeNum, true))
				{
					sync = true;
				}
			}
		}
		else if (taskData.variables[0] == OAGRTT_LEND_EQUIP)
		{
			return true;
		}
		else
		{
			return true;
		}

		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::OnGiftRightTeamDungeonTask(UInt32 grade,UInt32 type)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	if (!GetOwner()->HaveGiftRight())
	{
		return;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "type is " << type << " grade is " << grade << LogStream::eos;

	VisitOpenOpActAndTaskDataByOpType(OAT_GIFT_RIGHT, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables.size() != 1)
		{
			return true;
		}

		if ((taskData.variables[0] == OAGRTT_TEAM_DUNGEON_NORMAL && grade == TEAM_COPY_TEAM_GRADE_COMMON && type == TC_TYPE_ONE) ||
			(taskData.variables[0] == OAGRTT_TEAM_DUNGEON_HARD && grade == TEAM_COPY_TEAM_GRADE_DIFF && type == TC_TYPE_ONE) ||
			(taskData.variables[0] == OAGRTT_TEAM_DUNGEON_2 && type == TC_TYPE_TWO)
			)
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "variable is " << taskData.variables[0] << "type is " << type << " grade is "  << grade << LogStream::eos;
			std::string sender = SysNotifyMgr::Instance()->MakeString(10073);
			std::string	title = SysNotifyMgr::Instance()->MakeString(10072);
			std::string	content = SysNotifyMgr::Instance()->MakeString(10074);
			auto grp = GetOwner()->GetRewardGroup();
			grp->AddRewards(taskData.rewards);
			GetOwner()->SendSysMail(sender.c_str(), title.c_str(), content.c_str(), GetReason(LogSourceType::SOURCE_GIFT_RIGHT_CARD).c_str(), grp);
			grp->ClearRewards();
			sync = true;
		}
		else
		{
			return true;
		}

		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool  ActiveTaskMgr::_UpdateBuyPresentOpActTask(OpActTask& task, const OpActTaskData& taskData, const OpActivityRegInfo& opAct, UInt32 mallItemId, UInt32 mallItemNum)
{
	bool sync = false;
	bool find = false;
	UInt32 index = 0;
	for (auto var : taskData.variables)
	{
		if (var == mallItemId)
		{
			find = true;
			break;
		}
		++index;
	}

	if (!find)
	{
		return false;
	}

	if (index >= taskData.variables2.size() || index > taskData.varProgressName.size())
	{
		return false;
	}

	UInt32 maxProg = taskData.variables2[index];
	std::string varPrigKey = taskData.varProgressName[index];

	UInt32 curProg = task.GetParamVar(varPrigKey);
	if (curProg >= maxProg)
	{
		return false;
	}

	UInt32 newProg = curProg + mallItemNum;
	newProg = newProg > maxProg ? maxProg : newProg;

	task.SetParamVar(varPrigKey, newProg);
	sync = true;

	if (newProg >= maxProg)
	{
		bool finshed = true;
		for (UInt32 i = 0; i < taskData.variables.size(); ++i)
		{
			UInt32 maxProg = taskData.variables2[i];
			std::string varPrigKey = taskData.varProgressName[i];
			UInt32 curProg = task.GetParamVar(varPrigKey);
			if (curProg < maxProg)
			{
				finshed = false;
				break;
			}
		}
		if (finshed)
		{
			task.state = TASK_FINISHED;
		}
	}
	
	return sync;
}

void ActiveTaskMgr::_OnPlayerClearDungeonForDungeonPassedCumulateDayOpAct(const DungeonDataEntry* dungeonData)
{
	if (!dungeonData)
	{
		return;
	}
	CLProtocol::SCSyncOpActTaskChange protocol;
	UInt32 curTm = (UInt32)CURRENT_TIME.Sec();

	bool hasAddNum = false;

	VisitOpenOpActAndTaskDataByOpType(OAT_CUMULATEPASS_DUNGEON_REWARD, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData){
		if (!opAct) return true;

		bool find = false;
		for (auto var : taskData.variables)
		{
			if (var == dungeonData->subType)
			{
				find = true;
				break;
			}
		}

		if (!find)
		{
			return true;
		}

		if (!hasAddNum)
		{
			GetOwner()->IncCounter(COUNTER_CUMULATEPASS_DUNGEON, 1);
			hasAddNum = true;
		}

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}

		// 如果已经不能领奖了，直接改成已完成状态
		if (!_CheckOpTaskAccountSubmitLimit(&taskData))
		{
			return true;
		}

		std::string tmkey = "tm";
		std::string passtimeKey = "passTimes";
		UInt32 timestamp = task->GetParamVar(tmkey);
		UInt32 maxTimes = taskData.variables2[0];
		UInt32 newPassTimes = 0;
		//同一逻辑天
		if (timestamp  && CheckInSameLogicDay(timestamp, curTm))
		{
			
			UInt32 curPassTimes = task->GetParamVar(passtimeKey);
			if (curPassTimes >= maxTimes)
			{
				return true;
			}
			newPassTimes = curPassTimes + 1;
			sync = true;	
		}
		else 
		{
			newPassTimes = 1;
			sync = true;
		}

		
		newPassTimes = newPassTimes > maxTimes ? maxTimes : newPassTimes;
		task->SetParamVar(passtimeKey, newPassTimes);
		task->SetParamVar(tmkey, curTm);
		if (newPassTimes >= maxTimes)
		{
			task->AddCurNum(1, taskData.completeNum, true);
		}

		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

bool ActiveTaskMgr::_WeekSignTrigger(UInt32 opActType, UInt32 weekSignActType)
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(opActType, opActVec);

	if (opActVec.empty())
		return false;

	bool isFinish = false;
	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}			

		if (task->state != TASK_UNFINISH)
		{
			continue;
		}

		if (iter->variables.empty())
		{
			ErrorLogStream << "task variables empty id:" << iter->dataId << LogStream::eos;
			continue;
		}

		if (iter->variables[0] != weekSignActType)
			continue;

		switch (weekSignActType)
		{
		case WEEK_SIGN_ACT_ACTIVE:
		{
			if (iter->variables.size() != 2)
				continue;
			
			std::string key("lf");
			if (GetOwner()->GetDailyTaskScore() >= iter->variables[1] && !CheckInSameLogicDay(UInt32(CURRENT_TIME.Sec()), task->GetParamVar(key)))
			{
				task->SetParamVar("lf", UInt32(CURRENT_TIME.Sec()));
				task->curNum++;
			}
			
			if (task->curNum >= iter->completeNum)
			{
				task->state = TASK_FINISHED;
				isFinish = true;
			}

			break;
		}
		case WEEK_SIGN_ACT_DAILY:
		case WEEK_SIGN_ACT_DUNGEON:
		{
			task->curNum++;
			if (task->curNum >= iter->completeNum)
			{
				task->state = TASK_FINISHED;
				isFinish = true;
			}
			break;
		}
		default:return false;
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);

	return isFinish;
}

void ActiveTaskMgr::_CheckWeekSignFinish(UInt32 actType)
{
	std::vector<OpActTaskData*> taskDatas;
	ActivityMgr::Instance()->GetOpActTaskDataVec(actType, taskDatas);

	if (taskDatas.empty())
		return;

	for (auto taskData : taskDatas)
	{
		if (!taskData)
			return;

		OpActTask* task = FindNewOpActTask(taskData->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << taskData->dataId << ")!" << LogStream::eos;
			return;
		}

		if (task->state != TASK_FINISHED && task->state != TASK_OVER)
			return;
	}

	_ModifyWeekSignData(actType, WeekSignData::WEEK_NUM_SAVE);
}

bool ActiveTaskMgr::_OnSubmitWeekSign(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData)
	{
		return false;
	}

	if (task->state != TASK_FINISHED)
	{
		ErrorLogStream << "task id:" << task->dataId << " state:" << task->state << LogStream::eos;
		return false;
	}

	CLProtocol::GASWeekSignLotteryReq req;
	req.actType = opAct->tmpType;
	req.actId = taskData->dataId;
	req.vipLvl = GetOwner()->GetVipLvl();
	GetOwner()->SendToGlobalActivity(req);

	InfoLogStream << PLAYERINFO(GetOwner()) << "Send Global Lottery Week Sign Reward ActType:" << opAct->tmpType <<
		" ActId:" << taskData->dataId << LogStream::eos;
	
	return true;
}

void ActiveTaskMgr::_NotifyWeekSignBoxData(UInt32 opActType)
{
	auto iter = m_WeekSignMap.find(opActType);
	if (iter == m_WeekSignMap.end())
		return;
	
	CLProtocol::SceneWeekSignBoxNotify notify;
	notify.opActType = opActType;
	notify.boxData = iter->second.signBoxVec;
	GetOwner()->SendProtocol(notify);
}

void ActiveTaskMgr::_NotifyWeekSignSumData(UInt32 opActType)
{
	auto iter = m_WeekSignMap.find(opActType);
	if (iter == m_WeekSignMap.end())
		return;

	CLProtocol::SceneWeekSignNotify notify;
	notify.opActType = opActType;
	notify.signWeekSum = iter->second.weekNum;
	notify.yetWeek = iter->second.weekIdVec;
	GetOwner()->SendProtocol(notify);
}

void ActiveTaskMgr::_AddWeekSignData(UInt32 opActType)
{
	auto iter = m_WeekSignMap.find(opActType);
	if (iter == m_WeekSignMap.end())
	{
		_ModifyWeekSignData(opActType, WeekSignData::WEEK_TIME_SAVE);
	}
}

void ActiveTaskMgr::_RefreshWeekTimeBox(UInt32 opActType, WeekSignData& signData)
{
	std::ostringstream cond;
	cond << "owner_id='" << GetOwner()->GetID() << "' and act_type=" << opActType;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_week_sign", cond.str());

	cmd->PutData("act_id", signData.actId);
	cmd->PutData("week_time", signData.weekTime);
	cmd->PutData("week_num", signData.weekNum);
	cmd->PutData("week_id", signData.DeCodeWeekId());
	cmd->PutData("week_box", signData.DeCodeBoxData());

	CLRecordClient::Instance()->SendCommand(cmd);
}

void ActiveTaskMgr::_ModifyWeekSignData(UInt32 opActType, UInt32 option, UInt32 weekId, UInt32 actId, const ItemRewardVec* rewadVec)
{
	auto iter = m_WeekSignMap.find(opActType);
	if (iter == m_WeekSignMap.end())
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_week_sign", CLRecordClient::Instance()->GenGuid());
	
		UInt32 act_id = ActivityMgr::Instance()->GetOpenActivityIdByTmplateType(opActType);

		cmd->PutData("owner_id", GetOwner()->GetID());
		cmd->PutData("act_type", opActType);
		cmd->PutData("act_id", act_id);

		WeekSignData signData;
		_UpdateWeekSignData(opActType, option, signData, cmd, weekId, actId, rewadVec);
		
		CLRecordClient::Instance()->SendCommand(cmd);

		m_WeekSignMap[opActType] = signData;
	}
	else
	{
		std::ostringstream cond;
		cond << "owner_id='" << GetOwner()->GetID() << "' and act_type=" << opActType;
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_week_sign", cond.str());

		WeekSignData& signData = iter->second;
		_UpdateWeekSignData(opActType, option, signData, cmd, weekId, actId, rewadVec);

		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void ActiveTaskMgr::_UpdateWeekSignData(UInt32 opActType, UInt32 option, WeekSignData& signData, CLSaveCommand* cmd, UInt32 weekId, UInt32 actId, const ItemRewardVec* rewadVec)
{
	if (option & WeekSignData::WEEK_TIME_SAVE)
	{
		signData.weekTime = UInt32(getWeekBeginTime(CURRENT_TIME));
		cmd->PutData("week_time", signData.weekTime);
	}

	if (option & WeekSignData::WEEK_NUM_SAVE)
	{
		signData.weekNum++;
		cmd->PutData("week_num", signData.weekNum);
		_NotifyWeekSignSumData(opActType);
	}

	if (option & WeekSignData::ID_VEC_SAVE)
	{
		signData.weekIdVec.push_back(weekId);
		cmd->PutData("week_id", signData.DeCodeWeekId());
		_NotifyWeekSignSumData(opActType);
	}

	if (option & WeekSignData::BOX_VEC_SAVE)
	{
		if (NULL == rewadVec)
			return;

		WeekSignBox signBox;
		signBox.opActId = actId;
		signBox.itemVec = *rewadVec;
		signData.signBoxVec.push_back(signBox);

		cmd->PutData("week_box", signData.DeCodeBoxData());
		_NotifyWeekSignBoxData(opActType);
	}
}

void ActiveTaskMgr::_CrossWeekSignTaskReward(UInt32 actType)
{
	auto iter = m_WeekSignMap.find(actType);
	if (iter == m_WeekSignMap.end())
		return;

	UInt64 weekTime = UInt64(iter->second.weekTime);
	if (isSameWeek(CURRENT_TIME.MSec(), weekTime * 1000))
		return;

	// 刷新活动任务
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(actType, opActVec);
	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& itTask : opActVec)
	{
		if (NULL == itTask) continue;
		if (itTask->variables.empty())
		{
			ErrorLogStream << "task variables empty id:" << itTask->dataId << LogStream::eos;
			continue;
		}

		OpActTask* task = FindNewOpActTask(itTask->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << itTask->dataId << ")!" << LogStream::eos;
			continue;
		}

		if (itTask->variables[0] == WEEK_SIGN_ACT_ACTIVE)
		{
			std::string key("lf");
			task->SetParamVar(key, 0);
		}

		task->curNum = 0;
		task->state = TASK_UNFINISH;
		SaveOpTaskDB(task);

		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);

	// 刷新周时间，宝箱奖励
	UInt32 act_id = ActivityMgr::Instance()->GetOpenActivityIdByTmplateType(actType);

	iter->second.actId = act_id;
	iter->second.weekTime = UInt32(getWeekBeginTime(CURRENT_TIME));
	iter->second.signBoxVec.clear();
	_NotifyWeekSignBoxData(actType);

	_RefreshWeekTimeBox(actType, iter->second);
}

void ActiveTaskMgr::_AcceptWholeBargain()
{
	CLProtocol::GASWholeBargainDiscountSync sync;
	sync.discount = GameParamMgr::Instance()->GetValue(WHOLE_BARGAIN_DISCOUNT);
	GetOwner()->SendProtocol(sync);
}

void ActiveTaskMgr::TeamCopyClearDungeon(UInt32 grade, UInt32 type)
{
	std::vector<OpActTaskData*> opActVec;
	ActivityMgr::Instance()->GetOpActTaskDataVec(OAT_TEAM_COPY_SUPPORT, opActVec);
	if (grade == TEAM_COPY_TEAM_GRADE_COMMON || grade == TEAM_COPY_TEAM_GRADE_DIFF)
	{
		if (type == TC_TYPE_ONE)
		{
			OnChallengeActivityTask(OpActivityChallengeType::OACT_TEAM_DUNGEON, false);
		}
		
		if (type == TC_TYPE_TWO)
		{
			OnChallengeActivityTask(OpActivityChallengeType::OACT_TEAM_DUNGEON_2, false);
		}

		OnGiftRightTeamDungeonTask(grade, type);
	}

	OnHireTask(HTT_TEAM_DUNGEON, 1);

	if (opActVec.empty())
		return;

	CLProtocol::SCSyncOpActTaskChange protocol;
	for (auto& iter : opActVec)
	{
		if (NULL == iter) continue;

		OpActTask* task = FindNewOpActTask(iter->dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << iter->dataId << ")!" << LogStream::eos;
			continue;
		}

		task->AddCurNum(1, iter->completeNum, true);

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
		GetOwner()->SendProtocol(protocol);
}

void ActiveTaskMgr::AddTeamCopyBuff(const OpActivityRegInfo* opAct)
{
	if (NULL == opAct) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	if (opAct->endTime < curTime)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " end time=" <<
			TimeUtil::TimestampToStandardFormat(opAct->endTime) << LogStream::eos;
		return;
	}

	for (auto& buffId : opAct->parm2)
	{
		GetOwner()->GetBuffMgr().AddOpActBuff(buffId, opAct->endTime - curTime);
	}
}

void ActiveTaskMgr::NewYear2020(UInt32 type, UInt32 param1, UInt32 param2)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_NEW_YEAR_2020, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		if (taskData.variables.size() != 1 || taskData.variables[0] != type)
		{
			return true;
		}

		// 1v1需要检查第二个参数
		if (type == NEW_YEAR_2020_1V1)
		{
			if (taskData.variables2.size() != 1 || taskData.variables2[0] != param2)
			{
				return true;
			}
		}

		OpActTask* task = FindNewOpActTask(taskData.dataId);
		if (NULL == task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (!task->AddCurNum(param1, taskData.completeNum, true))
		{
			return true;
		}

		SaveOpTaskDB(task);
		protocol.tasks.push_back(*task);
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::AcceptOpActivityTask(UInt32 opActId, UInt32 taskId)
{
	const OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpActData(opActId);
	if (!opAct)
	{
		return;
	}

	if (opAct->state != AS_IN)
	{
		return;
	}

	bool allow = true;
	OpActivityBehavior tmpBeh = OP_ACTIVITY_BEHAVIOR_NULL;
	if (opAct->tmpType == OAT_SUMMER_CHALLENGE_WEEKLY)
	{
		tmpBeh = OP_ACTIVITY_BEHAVIOR_WEEKLY_SUMMER_CHALLENGE;
	}
	else if (opAct->tmpType == OAT_DAILY_CHALLENGE)
	{
		tmpBeh = OP_ACTIVITY_BEHAVIOR_DAILY_ACCEPT_TASK;
	}

	if (tmpBeh != OP_ACTIVITY_BEHAVIOR_NULL)
	{
		allow = _CheckAcceptTaskAccountLimit(opAct, tmpBeh);
		if (!allow)
		{
			return;
		}
	}

	allow = _CheckAcceptTaskPlayerLimit(opAct);
	if (!allow)
	{
		return;
	}

	CLProtocol::SCSyncOpActTaskChange protocol;

	// 通用的活动后处理
	for (auto taskData : opAct->tasks)
	{
		if (taskId != taskData.dataId)
		{
			continue;
		}

		OpActTask* task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task || task->state != TASK_INIT)
		{
			break;
		}

		//角色等级限制
		if (taskData.playerLevelLimit > 0 && GetOwner()->GetLevel() < taskData.playerLevelLimit)
		{
			break;
		}

		task->state = TASK_UNFINISH;

		if (tmpBeh != OP_ACTIVITY_BEHAVIOR_NULL)
		{
			OpActivityRecordMgr::Instance()->UpdateRecord(tmpBeh, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), opAct->dataId, 1);
		}

		InfoLogStream << PLAYERINFO(GetOwner()) << "Accept " << task->LogStr() << LogStream::eos;

		SaveOpTaskDB(task, taskData.accountTask);

		protocol.tasks.push_back(*task);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	if (opAct->tmpType == OAT_SUMMER_CHALLENGE_WEEKLY)
	{
		OnPlayerLoadDataSuccessForSummerChallengeOpAct();
	}
}

void ActiveTaskMgr::OnChallengeActivityTask(OpActivityChallengeType type, bool haveExScore, UInt32 param)
{
	UInt32 score = 0;
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_CHALLENGE_HUB, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;

		if (taskData.variables2.size() != 2 || taskData.variables.size() != 1)
		{
			return true;
		}

		if (taskData.variables[0] == type)
		{
			if (taskData.completeNum > 0)
			{
				task->RefreshState(taskData.completeNum);
				SaveOpTaskDB(task);
				if (task->state == TASK_FINISHED)
				{
					return false;
				}
			}
			if (!haveExScore)
			{
				score = taskData.variables2[0];
			}
			else
			{
				score = taskData.variables2[1];
			}
			if (param != 0)
			{
				score *= param;
				return false;
			}
			task->AddCurNum(1, taskData.completeNum, true);
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
			return false;
		}
		return true;
	});

	auto reason = GetReason(SOURCE_CHALLENGE_SCORE, type);
	auto grp = GetOwner()->GetRewardGroup();
	grp->AddReward(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHALLENGER_INTEGRAL_ITEM_ID), score);
	GetOwner()->AddRewards(reason.c_str(), grp);

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
	OnChallengeScoreAdd(score);
}

void ActiveTaskMgr::OnSpringActivityTask(OpActivitySpringType type, UInt32 score)
{
	UInt32 addScore = 0;

	VisitOpenOpActAndTaskByOpType(OAT_SPRING_CHALLENGE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;

		if (taskData.variables2.size() != 2 || taskData.variables.size() != 1)
		{
			return true;
		}

		if (taskData.variables[0] == type && score >= taskData.variables2[0])
		{
			if (taskData.completeNum > 0)
			{
				task->RefreshState(taskData.completeNum);
				SaveOpTaskDB(task);
				if (task->state == TASK_FINISHED)
				{
					return true;
				}
			}

			addScore += taskData.variables2[1];

			task->AddCurNum(1, taskData.completeNum, true);
			SaveOpTaskDB(task);
			return true;
		}
		return true;
	});

	auto reason = GetReason(SOURCE_SPRING_SCORE, type);
	auto grp = GetOwner()->GetRewardGroup();
	grp->AddReward(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHALLENGER_SPRING_ITEM_ID), addScore);
	GetOwner()->AddRewards(reason.c_str(), grp);
	OnSpringScoreAdd(addScore);
}

void ActiveTaskMgr::OnSpringScoreAdd(UInt32 num)
{
	GetOwner()->IncCounter(OPACT_SPRING_TOTAL_SCORE, num);

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_SPRING_CHALLENGE_SCORE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;

		if (taskData.variables2.size() != 1)
		{
			return true;
		}

		if (task->state >= TASK_FINISHED)
		{
			return true;
		}


		if (taskData.variables2[0] <= GetOwner()->GetCounter(OPACT_SPRING_TOTAL_SCORE))
		{
			task->state = TASK_FINISHED;
			protocol.tasks.push_back(*task);
			SaveOpTaskDB(task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::OnChallengeScoreAdd(UInt32 num)
{
	GetOwner()->IncCounter(OPACT_CHALLENGE_TOTAL_SCORE, num);
	GetOwner()->IncCounter(OPACT_CHALLENGE_DAY_SCORE, num);

	OnSpringActivityTask(OAST_CHALLENGE, GetOwner()->GetCounter(OPACT_CHALLENGE_TOTAL_SCORE));

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_CHALLENGE_HUB_SCORE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;

		if (taskData.variables.size() != 1 || taskData.variables2.size() != 1)
		{
			return true;
		}

		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables[0] == OACT_CHALLENGE_SCORE_DAY)
		{
			if (taskData.variables2[0] <= GetOwner()->GetCounter(OPACT_CHALLENGE_DAY_SCORE))
			{
				task->state = TASK_FINISHED;
				protocol.tasks.push_back(*task);
				SaveOpTaskDB(task);
			}
		}
		else if (taskData.variables[0] == OACT_CHALLENGE_SCORE_TOTAL)
		{
			if (taskData.variables2[0] <= GetOwner()->GetCounter(OPACT_CHALLENGE_TOTAL_SCORE))
			{
				task->state = TASK_FINISHED;
				protocol.tasks.push_back(*task);
				SaveOpTaskDB(task);
			}
		}
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::OnWeekSignSpring(UInt32 dungeonTag)
{
	if (!WeekSignSpringDataEntryMgr::Instance()->IsInScope(GetOwner()->GetLevel(), dungeonTag))
	{
		return;
	}

	CLProtocol::SCSyncOpActTaskChange protocol;
	
	UInt32 num = 0;
	VisitOpenOpActAndTaskByOpType(OAT_WEEK_SIGN_SPRING, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;

		if (taskData.variables.size() != 1 || taskData.varProgressName.size() != 2)
		{
			return true;
		}

		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables[0] == OAWSST_EVERY_WEEK)
		{
			UInt32 startTime = Avalon::StringUtil::ConvertToValue<UInt32>(taskData.varProgressName[0]);
			UInt32 endTime = Avalon::StringUtil::ConvertToValue<UInt32>(taskData.varProgressName[1]);

			if ((CURRENT_TIME.Sec() >= startTime) && (CURRENT_TIME.Sec() < endTime))
			{
				if (taskData.completeNum > 0)
				{
					task->AddCurNum(1, taskData.completeNum, true);
					protocol.tasks.push_back(*task);
					SaveOpTaskDB(task);
					if (task->state == TASK_FINISHED)
					{
						GetOwner()->IncCounter(OPACT_WEEK_SIGN_SPRING_NUM, 1);
						OnSpringActivityTask(OAST_WEEK_SIGN, GetOwner()->GetCounter(OPACT_WEEK_SIGN_SPRING_NUM));
						++num;
					}
				}

			}
		}
		return true;
	});

	if (num > 0)
	{
		VisitOpenOpActAndTaskByOpType(OAT_WEEK_SIGN_SPRING, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
			if (!opAct) return false;

			if (!task) return true;

			if (taskData.variables.size() != 1 || taskData.variables2.size() != 1)
			{
				return true;
			}

			if (task->state >= TASK_FINISHED)
			{
				return true;
			}

			if (taskData.variables[0] == OAWSST_TOTAL_WEEK)
			{

				if (taskData.variables2[0] <= GetOwner()->GetCounter(OPACT_WEEK_SIGN_SPRING_NUM))
				{
					task->state = TASK_FINISHED;
					protocol.tasks.push_back(*task);
					SaveOpTaskDB(task);
				}
			}
			return true;
		});
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::OnGnomeCoinAdd(UInt32 num)
{

	UInt32 dataId = 0;
	VisitOpenOpActDataByOpType(OAT_GNOME_COIN_GIFT, [&](const OpActivityRegInfo* opAct) {
		if (!opAct)
		{
			return true;
		}
		dataId = opAct->dataId;
		return true;
	});


	UInt32 count = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_GNOMECOIN_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
	OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_GNOMECOIN_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId, count += num);

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskByOpType(OAT_GNOME_COIN_GIFT, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
		if (!opAct) return false;

		if (!task) return true;

		if (task->curNum < count)
		{
			task->curNum = count;
		}

		if (task->state <= TASK_FINISHED)
		{
			if (task->curNum >= taskData.completeNum)
			{
				task->state = TASK_FINISHED;
			}
		}
		protocol.tasks.push_back(*task);
		SaveOpTaskDB(task);
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::OnChallengeActivity(const DungeonDataEntry* dungeonData, const std::vector<ObjID_t>& teammateIds)
{
	bool haveFriend = false, haveSameGuild = false;
	for (auto &i : teammateIds)
	{
		if (GetOwner()->HasRelation(RELATION_FRIEND, i))
		{
			haveFriend = true;
			break;
		}
		if (GetOwner()->GetGuildId() != 0)
		{
			auto mate = PlayerMgr::Instance()->FindPlayer(i);
			if (mate != nullptr)
			{
				if (GetOwner()->GetGuildId() == mate->GetGuildId() && mate != GetOwner())
				{
					haveSameGuild = true;
					break;
				}
			}
		}
	}

	if (dungeonData->subType == DST_YUANGU)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_ASCENT_DUNGEON, haveFriend || haveSameGuild);
	}
	else if (dungeonData->threeType == DTT_TEAM_ELITE)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_ELITE_DUNGEON, haveFriend || haveSameGuild);
	}
	else if (dungeonData->subType == DST_CITY_MONSTER)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_MONSTER_ATTACK, haveFriend || haveSameGuild);
	}
	else if (dungeonData->subType == DST_TREASUREMAP)
	{
		OnChallengeActivityTask(OpActivityChallengeType::OACT_UNDER_SEEK, false);
	}
	else if (dungeonData->subType == DST_DEVILDDOM)
	{
		OnHireTask(HTT_NONE_DUNGEON, 1);
	}
	else if (dungeonData->subType == DST_WEEK_HELL || dungeonData->subType == DST_WEEK_HELL_ENTRY || dungeonData->subType == DST_WEEK_HELL_PER)
	{
		OnHireTask(HTT_CHAOS_DUNGEON, 1);
	}

	{
		std::vector<UInt32> tempVec;
		for (auto &i : teammateIds)
		{
			auto mate = PlayerMgr::Instance()->FindPlayer(i);
			if (mate != nullptr)
			{
				if (mate != GetOwner())
				{
					tempVec.push_back(mate->GetAccID());
				}
			}
		}
		OnHireTask(HTT_MAKE_TEAM, 1, tempVec);
	}
}

bool ActiveTaskMgr::_CheckAcceptTaskAccountLimit(const OpActivityRegInfo* opAct, OpActivityBehavior behavior)
{
	if (opAct->parm2.size() != 2)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << opAct->dataId << ")'s param2 size=" << (UInt32)opAct->parm2.size() << ") is error!" << LogStream::eos;
		return true;
	}

	UInt32 count = OpActivityRecordMgr::Instance()->GetRecordValue(behavior, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), opAct->dataId);
	
	// 超过数量就不能再接任务了
	if (count >= opAct->parm2[0])
	{
		return false;
	}

	return true;
}

bool ActiveTaskMgr::_CheckAcceptTaskPlayerLimit(const OpActivityRegInfo* opAct)
{
	if (opAct->parm2.size() != 2)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << "OpAct(" << opAct->dataId << ")'s param2 size=" << (UInt32)opAct->parm2.size() << ") is error!" << LogStream::eos;
		return true;
	}
	
	UInt32 count = 0;
	for (auto taskData : opAct->tasks)
	{
		OpActTask* task = FindNewOpActTask(taskData.dataId);
		if (task && task->state != TASK_INIT && taskData.acceptType == OATAT_ACCEPT_MANUAL)
		{
			++count;
		}
	}

	// 超过数量就不能再接任务了
	if (count >= opAct->parm2[1])
	{
		return false;
	}

	return true;
}

bool ActiveTaskMgr::_OnSubmitOpActCheckMaoxianheijReward(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData)
{
	if (!opAct || !task || !taskData)
	{
		return false;
	}

	if (task->state != TASK_FINISHED)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << task->state << ")!" << LogStream::eos;
		return false;
	}

	UInt32 num = 0;
	for (auto itemReward : taskData->rewards)
	{
		if (itemReward.id == 600002534)
		{
			num = itemReward.num;
			break;
		}
	}

	if (num > 0)
	{
		UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WEAPON_LEASE_TICKETS_MAXBUN);
		if (GetOwner()->GetItemNum(600002534) + num > limit)
		{
			ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(600002534);
			std::string name = itemData ? itemData->name : "";
			GetOwner()->SendNotify(9102, name);
			return false;
		}
	}

	return true;
}

void ActiveTaskMgr::OnLoadOpActAccountTasks(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		ObjID_t uid = callback->GetKey();
		UInt32 taskId = callback->GetData("task_id");
		//UInt32 create_time = callback->GetData("create_time");

		UInt32 opActId = taskId / OPACT_ID_TRANS_VALUE;
		auto opActData = ActivityMgr::Instance()->GetOpActData(opActId);
		if (!opActData)
		{
			continue;
		}

		OpActTask* opTask = new OpActTask();
		opTask->uid = uid;
		opTask->ownerId = callback->GetData("owner_id");
		opTask->dataId = taskId;
		opTask->state = callback->GetData("status");
		{
			std::string parmStr = callback->GetData("parameter").GetString();
			std::vector<std::string> strVec;
			Avalon::StringUtil::Split(parmStr, strVec, ",");
			for (const auto& elem : strVec)
			{
				std::vector<std::string> strVec2;
				Avalon::StringUtil::Split(elem, strVec2, "=");
				if (strVec2.size() != 2)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << opTask->LogStr() << " parm format is error!" << LogStream::eos;
					continue;
				}

				if (strVec2[0] == OPACT_COMMON_VAR_KEY)
				{
					opTask->curNum = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
				}
				else{
					opTask->params[strVec2[0]] = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
				}
			}
		}

		OpActTask* dstTask = FindOpActAccountTask(taskId);
		if (dstTask != NULL)
		{
			// 合并数据
			if (dstTask->uid != uid)
			{
				if (dstTask->curNum < opTask->curNum)
				{
					dstTask->curNum = opTask->curNum;
				}

				if (dstTask->state < opTask->state)
				{
					dstTask->state = opTask->state;
				}

				OnMergeActAccountTask(opActData, opTask, dstTask);

				SaveOpAccountTaskDB(dstTask);
				DeleteOpAccountTaskDB(opTask);
			}

			ErrorLogStream << PLAYERINFO(GetOwner()) << " repeat task(" << uid << ", " << taskId << ")!" << LogStream::eos;
			delete opTask;
			continue;
		}

		if (!AddOpActAccountTask(opTask))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Add " << opTask->LogStr() << ") failed!" << LogStream::eos;
			delete opTask;
		}
	}

	return;
}

// opTask 是要被删除的， dstTask 是保留下来的
void ActiveTaskMgr::OnMergeActAccountTask(OpActivityRegInfo* info, OpActTask* opTask, OpActTask* dstTask)
{
	if (info->tmpType == OAT_SECRET_SELL)
	{
		dstTask->SetParamVar("cost_point_current", opTask->GetParamVar("cost_point_current") + dstTask->GetParamVar("cost_point_current"));
	}
}

OpActTask* ActiveTaskMgr::FindOpActAccountTask(UInt32 taskId)
{
	auto iter = m_idToOpActAccountTask.find(taskId);
	if (iter == m_idToOpActAccountTask.end())
	{
		return NULL;
	}

	return iter->second;
}

bool ActiveTaskMgr::AddOpActAccountTask(OpActTask* task)
{
	if (!task)
	{
		return false;
	}

	m_idToOpActAccountTask.insert(std::make_pair(task->dataId, task));
	return true;
}

void ActiveTaskMgr::ClearAllOpActAccountTasks()
{
	for (auto itr = m_idToOpActAccountTask.begin(); itr != m_idToOpActAccountTask.end(); ++itr)
	{
		CL_SAFE_DELETE(itr->second);
	}
	m_idToOpActAccountTask.clear();
}

void ActiveTaskMgr::SaveOpAccountTaskDB(OpActTask* task)
{
	if (task->uid == 0)
	{
		InsertOpAccountTaskDB(task);
	}
	else
	{
		UpdateOpAccountTaskDB(task);
	}
}

void ActiveTaskMgr::UpdateOpAccountTaskDB(OpActTask* task)
{
	if (!task)
	{
		ErrorLogStream << "OpActTask ptr is null!" << LogStream::eos;
		return;
	}

	std::ostringstream oss;
	oss << OPACT_COMMON_VAR << task->curNum;
	for (auto elem : task->params)
	{
		oss << "," << elem.first << "=" << elem.second;
	}

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_activity_op_account_task", task->uid, false);
	cmd->PutData("parameter", oss.str());
	cmd->PutData("status", task->state);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ActiveTaskMgr::InsertOpAccountTaskDB(OpActTask* task)
{
	if (!task)
	{
		ErrorLogStream << "OpActTask ptr is null!" << LogStream::eos;
		return;
	}

	task->uid = CLRecordClient::Instance()->GenGuid();
	task->ownerId = GetOwner()->GetAccID();

	std::ostringstream oss;
	oss << OPACT_COMMON_VAR << task->curNum;
	for (auto elem : task->params)
	{
		oss << "," << elem.first << "=" << elem.second;
	}

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_activity_op_account_task", task->uid);
	cmd->PutData("owner_id", task->ownerId);
	cmd->PutData("task_id", task->dataId);
	cmd->PutData("status", task->state);
	cmd->PutData("parameter", oss.str());
	cmd->PutData("create_time", UInt32(CURRENT_TIME.Sec()));
	CLRecordClient::Instance()->SendCommand(cmd);

	InfoLogStream << PLAYERINFO(GetOwner()) << " insert database: " << task->LogStr() << LogStream::eos;
}

void ActiveTaskMgr::DeleteOpAccountTaskDB(OpActTask* task)
{
	if (!task)
	{
		ErrorLogStream << "OpActTask ptr is null!" << LogStream::eos;
		return;
	}

	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_activity_op_account_task", task->uid);
	CLRecordClient::Instance()->SendCommand(cmd);

	InfoLogStream << PLAYERINFO(GetOwner()) << " delete from database: " << task->LogStr() << LogStream::eos;
}

bool ActiveTaskMgr::_CheckOpTaskAccountSubmitLimit(const OpActTaskData* taskData)
{
	if (!taskData)
	{
		return false;
	}

	auto dataId = taskData->dataId;
	// 检测账户每日领奖限制
	if (taskData->accountDailySubmitLimit > 0)
	{
		auto dailyTimes = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_DAILY_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
		if (dailyTimes >= taskData->accountDailySubmitLimit)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " activity task(" << dataId << ") account daily times limit, current(" << dailyTimes << ") max(" << taskData->accountDailySubmitLimit << ")." << LogStream::eos;
			return false;
		}
	}

	// 检测账户每周领奖限制
	if (taskData->accountWeeklySubmitLimit > 0)
	{
		auto weeklyTimes = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_WEEKLY_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
		if (weeklyTimes >= taskData->accountWeeklySubmitLimit)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " activity task(" << dataId << ") account weekly times limit, current(" << weeklyTimes << ") max(" << taskData->accountWeeklySubmitLimit << ")." << LogStream::eos;
			return false;
		}
	}

	// 检测账户总领奖限制
	if (taskData->accountTotalSubmitLimit > 0)
	{
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
		if (times >= taskData->accountTotalSubmitLimit)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " activity task(" << dataId << ") account total times limit, current(" << times << ") max(" << taskData->accountTotalSubmitLimit << ")." << LogStream::eos;
			return false;
		}
	}

	return true;
}

void ActiveTaskMgr::_UpdateOpTaskAccountSubmitTimes(const OpActTaskData* taskData)
{
	if (!taskData)
	{
		return;
	}

	auto dataId = taskData->dataId;
	// 检测账户每日领奖限制
	if (taskData->accountDailySubmitLimit > 0)
	{
		OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_DAILY_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId, 1);
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_DAILY_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
		ErrorLogStream << PLAYERINFO(GetOwner()) << " update activity task(" << dataId << ") account daily times current(" << times << ") max(" << taskData->accountDailySubmitLimit << ")." << LogStream::eos;
	}

	// 检测账户每周领奖限制
	if (taskData->accountWeeklySubmitLimit > 0)
	{
		OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_WEEKLY_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId, 1);
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_WEEKLY_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
		ErrorLogStream << PLAYERINFO(GetOwner()) << " update activity task(" << dataId << ") account weekly times current(" << times << ") max(" << taskData->accountWeeklySubmitLimit << ")." << LogStream::eos;
	}

	// 检测账户总领奖限制
	if (taskData->accountTotalSubmitLimit > 0)
	{
		OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId, 1);
		auto times = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId);
		ErrorLogStream << PLAYERINFO(GetOwner()) << " update activity task(" << dataId << ") account total times current(" << times << ") max(" << taskData->accountTotalSubmitLimit << ")." << LogStream::eos;
	}
}

void ActiveTaskMgr::OnTakeOpActAccountTaskReward(UInt32 opActDataId, UInt32 dataId, UInt64 param)
{
	OpActivityRegInfo* opActData = ActivityMgr::Instance()->GetOpActData(opActDataId);
	if (!opActData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find OpActivityRegInfo(" << opActDataId << ")!" << LogStream::eos;
		return;
	}

	if (opActData->state != AS_IN)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "OpActivity(" << opActDataId << ") was not open!" << LogStream::eos;
		GetOwner()->SendNotify(2200008);
		return;
	}

	if (!IsMeetOpActRequireByPlayer(opActData))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Not meet OpAct(" << opActData->dataId << ")'s requirement!" << LogStream::eos;
		return;
	}

	OpActTask* task = FindOpActAccountTask(dataId);
	if (!task)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task(" << dataId << ")!" << LogStream::eos;
		return;
	}

	OpActTaskData* taskData = ActivityMgr::Instance()->GetOpActTaskData(dataId);
	if (!taskData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find task data(" << dataId << ")!" << LogStream::eos;
		return;
	}

	// 转盘抽奖，账号任务
	if (CLEventMgr::SUB_ET_DRAW_PRIZE == taskData->subType && TASK_FINISHED == task->state && 1 == taskData->rewards.size())
	{
		//更新任务状态
		task->state = TASK_OVER;

		//同步DB
		UpdateOpAccountTaskDB(task);

		//同步客户端
		CLProtocol::SCSyncOpActTaskChange protocol;
		protocol.tasks.push_back(*task);
		GetOwner()->SendProtocol(protocol);

		//抽奖
		GetOwner()->DrawPrizeForOpActivity(taskData->rewards[0].id);
		return;
	}

	bool bGiveReward = true;
	bool bSubmitSuccess = true;

	switch ((OpActivityType)opActData->tmpType)
	{
	case OAT_ACCOUNT_ONLINE:
		if (task->state != TASK_FINISHED)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << (UInt8)TASK_FINISHED << ")!" << LogStream::eos;
			return;
		}
		task->curNum = taskData->completeNum;
		task->state = TASK_OVER;
		OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dataId, 2);
		break;
	default:
		if (task->state != TASK_FINISHED)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Invalid task state(" << (UInt8)TASK_FINISHED << ")!" << LogStream::eos;
			return;
		}
		task->curNum = taskData->completeNum;
		task->state = TASK_OVER;
		break;
	}

	//同步DB
	UpdateOpAccountTaskDB(task);

	//同步客户端
	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*task);
	GetOwner()->SendProtocol(protocol);

	InfoLogStream << PLAYERINFO(GetOwner()) << " submit " << task->LogStr() << ", bSubmitSuccess(" << (bSubmitSuccess ? 1 : 0)
		<< "), bGiveReward(" << (bGiveReward ? 1 : 0) << ")." << LogStream::eos;

	if (!bSubmitSuccess) return;

	if (bGiveReward)
	{
		RewardGroup* group = m_pOwner->GetRewardGroup();
		for (const auto& reward : taskData->rewards)
		{
			auto itemData = ItemDataEntryMgr::Instance()->FindEntry(reward.id);
			if (!itemData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find item data(" << reward.id << ")!" << LogStream::eos;
				continue;
			}

			// 超过叠加数
			if (reward.num > itemData->maxNum)
			{
				for (UInt32 idx = 0; idx != (reward.num / itemData->maxNum); ++idx)
				{
					group->AddReward(reward.id, itemData->maxNum, 0, reward.strenth, 0, static_cast<EquipType>(reward.equipType));
				}

				UInt32 restRewardNum = reward.num % itemData->maxNum;
				if (restRewardNum > 0)
				{
					group->AddReward(reward.id, restRewardNum, 0, reward.strenth, 0, static_cast<EquipType>(reward.equipType));
				}
			}
			else
			{
				group->AddReward(reward.id, reward.num, 0, reward.strenth, 0, static_cast<EquipType>(reward.equipType));
			}
		}

		m_pOwner->AddRewards(GetReason(SOURCE_TYPE_TASK_ACTIVE, opActDataId, dataId).c_str(), group, true);
	}
}

void ActiveTaskMgr::OnPlayerLoadDataSuccessForSummerChallengeOpAct()
{
	VisitOpenOpActDataByOpType(OAT_SUMMER_CHALLENGE_WEEKLY, [&](const OpActivityRegInfo* opAct){
		if (!opAct) return true;

		_UpdateSummerChallengeWeeklyTasksAfterPlayerLoadDataSuccess(opAct);
		return true;
	});
}

void ActiveTaskMgr::_UpdateSummerChallengeWeeklyTasksAfterPlayerLoadDataSuccess(const OpActivityRegInfo* opAct)
{
	if (!opAct)
	{
		return;
	}

	if (opAct->state != AS_IN)
	{
		return;
	}

	CLProtocol::SCSyncOpActTaskChange protocol;

	for (auto taskData : opAct->tasks)
	{
		if (taskData.variables.empty())
		{
			continue;
		}

		const DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(*(taskData.variables.begin()));
		if (!dungeonData || !dungeonData->IsWeekHellEntry())
		{
			continue;
		}

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			continue;
		}

		if (task->state != TASK_UNFINISH)
		{
			continue;
		}

		UInt32 weekCount = GetOwner()->GetCounter("dw_used_22");
		if (weekCount <= task->curNum)
		{
			continue;
		}

		UInt32 incNum = weekCount - task->curNum;
		if (task->AddCurNum(incNum, taskData.completeNum, true))
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
			InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << " increase num(" << incNum << ")." << LogStream::eos;
		}

		if (TASK_FINISHED == task->state)
		{
			_SendOpActTaskFinishedEvent(task->dataId);
		}
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_UpdateSecondAnniversaryPrayOpActTask()
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_SECOND_ANNIVERSARY_PRAY, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		if (taskData.variables.size() != 2)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " param size is error!" << LogStream::eos;
			return false;
		}

		UInt32 dayBeginTime = taskData.variables[0];
		UInt32 dayEndTime = taskData.variables[1];

		if (dayBeginTime >= dayEndTime)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " param timestamp is error!" << LogStream::eos;
			return false;
		}

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " no task record!" << LogStream::eos;
			return false;
		}

		if (dayBeginTime <= curTime && curTime <= dayEndTime)
		{
			if (task->state == TASK_UNFINISH)
			{
				task->curNum = dayEndTime;
				task->state = TASK_FINISHED;

				SaveOpTaskDB(task);
				protocol.tasks.push_back(*task);

				InfoLogStream << PLAYERINFO(GetOwner()) << " finish " << task->LogStr() << LogStream::eos;
			}
		}
		else if (curTime > dayEndTime)
		{
			if (task->state != TASK_OVER)
			{
				task->state = TASK_OVER;

				SaveOpTaskDB(task);
				protocol.tasks.push_back(*task);

				InfoLogStream << PLAYERINFO(GetOwner()) << task->LogStr() << " over." << LogStream::eos;
			}
		}

		if (SAPABT_DUNGEON_EXP_ADD == (SecondAnniversaryPrayActivityBuffType)taskData.completeNum && TASK_FINISHED == task->state)
		{
			for (auto buffId : taskData.variables2)
			{
				GetOwner()->GetBuffMgr().AddOpActBuff(buffId, (task->curNum - curTime));
			}
		}

		return true;
		});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_UpdateCumulateOnlineOpActTask()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	UInt32 oplayerOnlineTm = GetOwner()->GetDayOnlineTime();
	UInt32 currTm = (UInt32)CURRENT_TIME.Sec();

	VisitOpenOpActAndTaskDataByOpType(OAT_CUMULATE_ONLINE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		UInt32 taskeOnlineTm = opAct->parm * MIN_TO_SEC;

		if (oplayerOnlineTm < taskeOnlineTm)
		{
			return true;
		}

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}

		UInt32 lastTm = task->GetParamVar("lastTm");

		if (CheckInSameLogicDay(currTm, lastTm))
		{
			return true;
		}

		if (task->AddCurNum(1, taskData.completeNum, true))
		{
			sync = true;
			task->SetParamVar("lastTm", currTm);
		}
		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	VisitOpenOpActAndTaskDataByOpType(OAT_HALLOWEEN_PUMPKIN_HELMET, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		UInt32 taskeOnlineTm = opAct->parm * MIN_TO_SEC;

		if (oplayerOnlineTm < taskeOnlineTm)
		{
			return true;
		}

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}

		UInt32 lastTm = task->GetParamVar("lastTm");

		if (CheckInSameLogicDay(currTm, lastTm))
		{
			return true;
		}

		if (task->AddCurNum(1, taskData.completeNum, true))
		{
			sync = true;
			task->SetParamVar("lastTm", currTm);
		}

		//达到计数条件，任务完成，给予奖励(抽奖券)
		if (task->state == TASK_FINISHED && taskData.completeNum == 1 && taskData.rewards.size() == 1)
		{
			// 奖励的抽奖券数量
			UInt32 rewardNum = taskData.rewards.front().num;

			std::string drawCountName = Avalon::StringUtil::ConvertToString((UInt32)opAct->tmpType) + OPACT_DARW_PRIZE_NUM;
			if (CounterCfg::Instance()->GetCycleType(drawCountName) == COUNTER_CYCLE_INVALID)
			{
				CounterCfg::Instance()->RegCounter(drawCountName.c_str(), COUNTER_CYCLE_NONE);
			}

			GetOwner()->IncCounter(drawCountName.c_str(), rewardNum);
			UInt32 drawPrizeCount = GetOwner()->GetCount(drawCountName.c_str());

			task->state = TASK_OVER;

			DebugLogStream << PLAYERINFO(GetOwner()) << "Give reward, num=" << rewardNum << ", now draw prize count=" << drawPrizeCount << LogStream::eos;
		}

		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}


void ActiveTaskMgr::OnMoneyConsume(UInt32 value, const char* reason)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	ReasonInfo tmpInfo;
	if (!ParseReason(reason, tmpInfo))
	{
		return;
	}

	UInt32 reasonId = (UInt32)tmpInfo.type;
	
	VisitOpenOpActAndTaskDataByOpType(OAT_MONEY_CONSUME_REBATE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " no task record!" << LogStream::eos;
			return false;
		}

		if (std::find(taskData.variables.begin(), taskData.variables.end(), reasonId) != taskData.variables.end())
		{
			return true;
		}

		if (task->state == TASK_UNFINISH)
		{
			task->AddCurNum(value, taskData.completeNum, true);

			// 同步到数据库和客户端
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
		});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	_OnMoneyConsumeForRechargeAndConsumeOpActTask(value, reasonId);

	_OnPointRemoveSecretSell(value, reasonId, tmpInfo.param1, tmpInfo.param2);
}

UInt8 ActiveTaskMgr::GetDiscoutOpTaskStByTmpTypeAndExchTaskId(UInt32 tmpType, UInt32 exchTaskId)
{
	OpActivityRegInfo* opActRegInfo = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(GetOwner(), (OpActivityType)tmpType);
	if (!opActRegInfo)
	{
		return TASK_INIT;
	}

	UInt32 opTaskId = 0;
	for (auto opTaskData : opActRegInfo->tasks)
	{
		for (auto v : opTaskData.variables)
		{
			if (v == exchTaskId)
			{
				opTaskId = opTaskData.dataId;
				break;
			}
		}
	}

	if (opTaskId == 0)
	{
		return TASK_INIT;
	}

	auto opTask = this->FindNewOpActTask(opTaskId);
	if (opTask == NULL)
	{
		return TASK_INIT;
	}
	return opTask->state;
}

void ActiveTaskMgr::SetDiscoutOpTaskStByTmpTypeAndExchTaskId(UInt32 tmpType, UInt32 exchTaskId, UInt8 st)
{
	OpActivityRegInfo* opActRegInfo = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(GetOwner(), (OpActivityType)tmpType);
	if (!opActRegInfo)
	{
		return;
	}

	UInt32 opTaskId = 0;
	for (auto opTaskData : opActRegInfo->tasks)
	{
		for (auto v : opTaskData.variables)
		{
			if (v == exchTaskId)
			{
				opTaskId = opTaskData.dataId;
				break;
			}
		}
	}

	if (opTaskId == 0)
	{
		return;
	}

	auto opTask = this->FindNewOpActTask(opTaskId);
	if (!opTask || opTask->state == st)
	{
		return;
	}
	
	opTask->state = st;
	SaveOpTaskDB(opTask);
	CLProtocol::SCSyncOpActTaskChange protocol;
	protocol.tasks.push_back(*opTask);
	GetOwner()->SendProtocol(protocol);
	return;
}

void ActiveTaskMgr::_UpdateAccountCumulateOnlineOpActTask()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_ACCOUNT_ONLINE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;


		auto task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables2.size() != 1)	return true;
		if (taskData.variables.size() != 1)		return true;

		//每日在线任务
		if (taskData.variables[0] == 0)
		{
			OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskData.dataId, LOGICTICK_FIVESEC * 5);
			auto onlineTime = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskData.dataId);
			task->SetParamVar("daily_online_count", onlineTime);
			sync = true;

			if (onlineTime >= taskData.variables2[0])
			{
				UInt32 state = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskData.dataId);
				if (state == 0)
				{
					OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_TASK, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), taskData.dataId, 1);
					if (task->AddCurNum(1, taskData.completeNum, true))
					{
						//发送完成任务事件
						_SendOpActTaskFinishedEvent(task->dataId);
					}
				}
				else if (state == 1)
				{
					task->state = TASK_FINISHED;
				}
				else if (state == 2)
				{
					task->state = TASK_OVER;
				}
			}
		}
		if (sync)
		{
			SaveOpTaskDB(task, taskData.accountTask);
			protocol.tasks.push_back(*task);
		}
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_UpdateGiftRightOpActTask(const Avalon::Time& now)
{
	if (GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD) == 0)
	{
		return;
	}
	if (now.Sec() > GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD_BUY_TIME) + 30 * Avalon::Time::SECS_OF_DAY)
	{
		GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD, 0);
		GetOwner()->SyncGiftRightToWorld();
	}
}

void ActiveTaskMgr::_UpdateSpringFestivalRainOpActTask()
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_SPRING_FESTIVAL_RED_PACKET_RAIN, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		if (taskData.variables.size() != 2 || taskData.variables2.size() != 2)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " param size is error!" << LogStream::eos;
			return false;
		}

		UInt32 dayBeginTime = taskData.variables[0];
		UInt32 dayEndTime = taskData.variables[1];

		UInt32 awardBeginTime = taskData.variables2[0];
		UInt32 awardEndTime = taskData.variables2[1];

		if (dayBeginTime >= dayEndTime || awardBeginTime >= awardEndTime)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " param timestamp is error!" << LogStream::eos;
			return false;
		}

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " no task record!" << LogStream::eos;
			return false;
		}

		if (task->state != TASK_UNFINISH)
		{
			return true;
		}

		if (dayBeginTime <= curTime && curTime <= dayEndTime)
		{
			bool update = false;
			if (curTime <= awardEndTime && 
				task->curNum < taskData.completeNum &&
				task->curNum < GetOwner()->GetDailyTaskScore())
			{
				task->curNum = (GetOwner()->GetDailyTaskScore() < taskData.completeNum) ? GetOwner()->GetDailyTaskScore() : taskData.completeNum;
				update = true;
			}


			if (awardBeginTime <= curTime && curTime <= awardEndTime && task->params.size() <= 0)
			{
				task->params["online"] = 1;
				update = true;
			}

			if (task->curNum >= taskData.completeNum && task->params.size() > 0)
			{
				task->state = TASK_FINISHED;
				update = true;
			}

			if (update)
			{
				SaveOpTaskDB(task);
				protocol.tasks.push_back(*task);
			}
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_AcceptSpringRedPacketRecvOpActTask(OpActTask* task, const OpActTaskData& taskData)
{
	if (!task) return;
	
	if (taskData.variables2.size() == 0 || taskData.variables2[0] != 1)
	{
		return;
	}

	//拥有月卡领取奖励
	if (GetOwner()->HasMonthCard())
	{
		task->state = TASK_FINISHED;
		task->curNum = taskData.completeNum;
	}
	return;
}

void ActiveTaskMgr::_OnChargeForSpringRedPacketOpActTask(UInt32 chargeNum)
{
	OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(GetOwner(), OAT_SPRING_FESTIVAL_RED_PACKET_RECV);
	if (opAct == NULL)
	{
		ErrorLogStream << "curOpenActId = NULL, OpActTempId = " << OAT_SPRING_FESTIVAL_RED_PACKET_RECV << LogStream::eos;
		return;
	}
	std::ostringstream actscoreCounter;
	actscoreCounter << ACTIVE_SCORE_PER << opAct->dataId;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(actscoreCounter.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(actscoreCounter.str().c_str(), COUNTER_CYCLE_NONE);
	}
	UInt32 oldScore = GetOwner()->GetCounter(actscoreCounter.str().c_str());
	UInt32 newScore = 0;
	if (oldScore + chargeNum > MAX_MONEY) newScore = MAX_MONEY;
	else newScore = oldScore + chargeNum;

	if (newScore > oldScore)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << " Add total charge num old:" << oldScore << " new:" << newScore << " add:" << chargeNum << LogStream::eos;

		GetOwner()->SetCounter(actscoreCounter.str().c_str(), newScore);
	}
	_UpdateSpringRedPacketRecvOpActTask(SRPTT_CHARGE, chargeNum);
}

void ActiveTaskMgr::_UpdateSpringRedPacketRecvOpActTask(UInt8 type, UInt32 chargeNum)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_SPRING_FESTIVAL_RED_PACKET_RECV, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}

		if (taskData.variables2.size() == 0)	return true;
		if (taskData.variables2[0] != type)		return true;

		//月卡
		if (taskData.variables2[0] == SRPTT_MONTH_CARD)
		{
			if (GetOwner()->HasMonthCard())
			{
				task->state = TASK_FINISHED;
				task->curNum = taskData.completeNum;
				sync = true;
			}
		}
		else if (taskData.variables2[0] == SRPTT_CHARGE)
		{
			if (task->AddCurNum(chargeNum, taskData.completeNum, true))
			{
				sync = true;
			}
		}
		else
		{
			return true;
		}

		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_UpdateOnlineGiftOpActTask()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	UInt32 oplayerOnlineTm = GetOwner()->GetDayOnlineTime();
	bool setCounter = false;

	VisitOpenOpActAndTaskDataByOpType(OAT_ONLINE_GIFT, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}

		if (taskData.variables2.size() == 0)	return true;
		if (taskData.variables.size() == 0)		return true;
		
		if (taskData.variables2[0] == OGTT_TIME) //在线时间
		{
			if (oplayerOnlineTm < taskData.variables[0] * MIN_TO_SEC)
			{
				return true;
			}

			if (task->AddCurNum(1, taskData.completeNum, true))
			{
				sync = true;
			}
		}
		else if (taskData.variables2[0] == SRPTT_DAY)
		{
			if (oplayerOnlineTm < taskData.variables[0] * MIN_TO_SEC)
			{
				return true;
			}
			if (GetOwner()->GetCounter(COUNTER_ACTIVE_ONLINE_GIFT_DAY) == 1)
			{
				return true;
			}
			if (task->AddCurNum(1, taskData.completeNum, true))
			{
				OnSpringActivityTask(OAST_ONLINE, task->curNum);
				sync = true;
			}
			setCounter = true;
		}
		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}
		return true;
	});

	if (setCounter)
	{
		GetOwner()->SetCounter(COUNTER_ACTIVE_ONLINE_GIFT_DAY, 1);
	}

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::UpdatePlantOpActTask()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_PLANT_TREE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		auto task = FindNewOpActTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		bool sync = false;
		if (task->state == TASK_FINISHED || task->state == TASK_OVER)
		{
			return true;
		}
		if (taskData.varProgressName.size() == 0)
		{
			return true;
		}
		for (auto counterName : taskData.varProgressName)
		{
			const CounterConfig* counterConfig1 = CounterCfg::Instance()->GetCycleConfig(counterName);
			if (!counterConfig1)
			{
				CounterCfg::Instance()->RegCounter(counterName.c_str(), COUNTER_CYCLE_NONE);
			}
			UInt8 istreeGet = GetOwner()->GetCounter(counterName.c_str());
			if (!istreeGet)
			{
				return true;
			}
		}

		if (task->AddCurNum(1, taskData.completeNum, true))
		{
			if (taskData.variables2.size() == 1)
			{
				OnSpringActivityTask(OAST_SPRING_TREE, taskData.variables2[0]);
			}
			sync = true;
		}
		if (sync)
		{
			SaveOpTaskDB(task);
			protocol.tasks.push_back(*task);
		}
		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

UInt32 ActiveTaskMgr::GetSecretCoin()
{
	UInt32 num = 0;
	VisitOpenOpActAndTaskDataByOpType(OAT_SECRET_SELL, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;


		auto task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		num = task->GetParamVar("cost_point_current");
		return false;
	});
	return num;
}

void ActiveTaskMgr::SetSecretCoin(UInt32 num)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_SECRET_SELL, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;


		auto task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		// 用来记录总消费点券数量
		if (num > task->GetParamVar("cost_point_current"))
		{
			task->SetParamVar("cost_point_total", task->GetParamVar("cost_point_total") + num - task->GetParamVar("cost_point_current"));
		}
		
		task->SetParamVar("cost_point_current", num);
		SaveOpTaskDB(task, taskData.accountTask);
		protocol.tasks.push_back(*task);
		return false;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	GetOwner()->SendSecretCoin();
}

void ActiveTaskMgr::OnPlayerMonopolyRoll()
{
	OnChallengeActivityTask(OACT_MONOPOLY, false);
}

void ActiveTaskMgr::_OnMoneyChargeForRechargeAndConsumeOpActTask(const char* reason, UInt32 chargeNum)
{
	if (!reason)
	{
		return;
	}

	ReasonInfo tmpInfo;
	if (!ParseReason(reason, tmpInfo))
	{
		return;
	}

	// 王者通行证不计入
	if (SOURCE_TYPE_BILLING_GOODS == tmpInfo.type)
	{
		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(tmpInfo.param2);
		if (itemData && itemData->subType == ST_ADVENTURE_KING)
		{
			return;
		}
	}

	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_RECHARGE_CONSUME_REBATE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		// 参数2格式不对或者充值金额对不上，就不记录
		if (taskData.variables2.size() != 2 || taskData.variables2[0] != chargeNum)
		{
			return true;
		}

		auto task = FindOpActAccountTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " no task record!" << LogStream::eos;
			return false;
		}

		if (task->state == TASK_UNFINISH)
		{
			task->SetParamVar(OPACT_CHARGE_NUM_VAR_KEY, chargeNum);

			// 同步到数据库和客户端
			SaveOpAccountTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	_OnCheckRechargeAndConsumeOpActTask();
}

void ActiveTaskMgr::_OnMoneyConsumeForRechargeAndConsumeOpActTask(UInt32 value, UInt32 reasonId)
{
	bool bAdded = false;
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_RECHARGE_CONSUME_REBATE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		// 参数1代表来源过滤
		if (std::find(taskData.variables.begin(), taskData.variables.end(), reasonId) != taskData.variables.end())
		{
			return true;
		}

		// 用来记录总消费点券数量
		if (!bAdded)
		{
			UInt32 count = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_MONEY_CONSUME_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), opAct->dataId);
			count += value;
			OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_MONEY_CONSUME_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), opAct->dataId, count);
			bAdded = true;
		}

		auto task = FindOpActAccountTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " no task record!" << LogStream::eos;
			return false;
		}

		if (task->state == TASK_UNFINISH)
		{
			UInt32 consumeNum = task->GetParamVar(OPACT_CONSUME_NUM_VAR_KEY);
			consumeNum += value;
			task->SetParamVar(OPACT_CONSUME_NUM_VAR_KEY, consumeNum);

			// 同步到数据库和客户端
			SaveOpAccountTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}

	_OnCheckRechargeAndConsumeOpActTask();
}

void ActiveTaskMgr::_OnCheckRechargeAndConsumeOpActTask()
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_RECHARGE_CONSUME_REBATE, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;

		// 参数2格式不对或者充值金额对不上，就不记录
		if (taskData.variables2.size() != 2)
		{
			return true;
		}

		auto task = FindOpActAccountTask(taskData.dataId);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " no task record!" << LogStream::eos;
			return false;
		}

		if (task->state != TASK_UNFINISH)
		{
			return true;
		}

		UInt32 count = 0;
		if (task->GetParamVar(OPACT_CHARGE_NUM_VAR_KEY) == taskData.variables2[0])
		{
			++count;
		}

		if (task->GetParamVar(OPACT_CONSUME_NUM_VAR_KEY) >= taskData.variables2[1])
		{
			++count;
		}

		if (task->AddCurNum(count, taskData.completeNum, false))
		{
			// 同步到数据库和客户端
			SaveOpAccountTaskDB(task);
			protocol.tasks.push_back(*task);
		}

		return true;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}

void ActiveTaskMgr::_OnPointRemoveSecretSell(UInt32 value, UInt32 reasonId, UInt64 p1, UInt32 p2)
{
	CLProtocol::SCSyncOpActTaskChange protocol;

	VisitOpenOpActAndTaskDataByOpType(OAT_SECRET_SELL, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData) {
		if (!opAct) return true;


		auto task = FindNewOpActTask(taskData.dataId, taskData.accountTask);
		if (!task)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " can't find task(" << taskData.dataId << ")!" << LogStream::eos;
			return true;
		}

		if (task->state >= TASK_FINISHED)
		{
			return true;
		}

		if (taskData.variables.empty())
		{
			return true;
		}

		// 参数1代表来源过滤
		auto iter = std::find(taskData.variables.begin(), taskData.variables.end(), reasonId);
		if (iter != taskData.variables.end())
		{
			if (taskData.variables.end() - iter <= 2)
			{
				return true;
			}
			auto iter2 = iter + 1;
			auto iter3 = iter + 2;
			if (*iter2 == 0 && *iter3 == 0)
			{
				return true;
			}
			if (reasonId == SOURCE_TYPE_SHOP_MALL)
			{
				if (*iter2 == p1)
				{
					return true;
				}
			}
		}

		// 用来记录总消费点券数量
		task->SetParamVar("cost_point_total", task->GetParamVar("cost_point_total") + value);
		//OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_MONEY_CONSUME_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), opAct->dataId, value);
		task->SetParamVar("cost_point_current", task->GetParamVar("cost_point_current") + value);
		SaveOpTaskDB(task, taskData.accountTask);
		protocol.tasks.push_back(*task);
		return false;
	});

	if (!protocol.tasks.empty())
	{
		GetOwner()->SendProtocol(protocol);
	}
}
