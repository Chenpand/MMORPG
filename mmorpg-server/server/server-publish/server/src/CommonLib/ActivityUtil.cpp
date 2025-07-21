#include "ActivityUtil.h"

#include "TimeUtil.h"
#include "CLSystemValueDataEntry.h"

namespace ActivityUtil
{

	bool InitOpActivityInfo(OpActivityRegInfo& info, const OpActivityDataEntry* data)
	{
		if (!data) return false;

		info.needStartServiceTime = data->needStartServiceTime;

		// 如果有规定开服时间，不达到要求的直接无视掉
		if (info.needStartServiceTime > 0 && info.needStartServiceTime <= GAME_FIRST_START_TIME)
		{
			return false;
		}

		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

		info.dataId = data->id;
		info.tmpType = data->tmpType;
		info.name = data->name;
		info.tag = data->tag;
		info.tagEndTime = data->tagEngTime;
		info.prepareTime = data->prepareTime;
		info.startTime = data->startTime;
		info.endTime = data->endTime;
		info.playerLevelLimit = data->playerLevelLimit;
		info.minLevel = data->minLevel;
		info.maxLevel = data->maxLevel;
		info.parm2 = data->parm2;
		info.desc = data->desc;
		info.ruleDesc = data->ruleDesc;
		info.circleType = data->circleType;
		info.parm = data->parm;
		info.logoDesc = data->logoDesc;
		info.dataSource = OADS_TABLE;
		info.countParam = data->countParam;
		info.parm3 = data->parm3;
		info.prefabPath = data->prefabPath;
		info.logoPath = data->logoPath;
		info.strParams = data->strParams;

		UInt32 gameStartTime = 0;
		std::string startTimeStr;
		std::string endTimeStr;

		if (!data->startTimeFromService.empty())
		{
			gameStartTime = GAMESTART_TIME;
			startTimeStr = data->startTimeFromService;
			endTimeStr = data->endTimeFromService;
		}

		if (!data->startTimeFromFirstService.empty())
		{
			gameStartTime = GAME_FIRST_START_TIME;
			startTimeStr = data->startTimeFromFirstService;
			endTimeStr = data->endTimeFromFirstService;
		}

		if (gameStartTime > 0)
		{
			UInt32 startTemp = UInt32(GetDayBeginTime(Avalon::Time(UInt64(gameStartTime) * SEC_TO_MSEC)) / SEC_TO_MSEC);

			if (-1 == TimeUtil::CalculateEndPointByStartPointAndInterval(startTemp, startTimeStr, info.startTime))
			{
				ErrorLogStream << " calculte time failed, open interval=" << startTimeStr << LogStream::eos;
				return false;
			}
			if (-1 == TimeUtil::CalculateEndPointByStartPointAndInterval(startTemp, endTimeStr, info.endTime))
			{
				ErrorLogStream << " calculte time failed, close interval=" << endTimeStr << LogStream::eos;
				return false;
			}
		}

		if (info.prepareTime > info.startTime)
		{
			ErrorLogStream << "OpActivity(" << info.dataId << ") error time format: prepareTime(" << info.prepareTime << ") is greater than startTime(" << info.startTime << ")!" << LogStream::eos;
			return false;
		}

		if (info.startTime > info.endTime)
		{
			ErrorLogStream << "OpActivity(" << info.dataId << ") error time format: startTime(" << info.startTime << ") is greater than endTime(" << info.endTime << ")!" << LogStream::eos;
			return false;
		}

		if (info.prepareTime != 0 && curTime >= info.prepareTime && curTime < info.startTime)
		{
			info.state = AS_PREPARE;
		}
		if (curTime >= info.startTime && curTime < info.endTime)
		{
			info.state = AS_IN;
		}

		std::vector<OpActivityTaskDataEntry*> taskDatas;

		if (!data->taskIds.empty())
		{
			for (auto taskId : data->taskIds)
			{
				auto taskDataEntry = OpActivityTaskDataEntryMgr::Instance()->FindEntry(taskId);
				if (!taskDataEntry)
				{
					ErrorLogStream << "Can not find task data(" << taskId << ") for activity(" << info.dataId << ")!" << LogStream::eos;
					continue;
				}

				taskDatas.push_back(taskDataEntry);
			}
		}
		//else
		{
			class OpActivityTaskDataVisitor : public Avalon::DataEntryVisitor<OpActivityTaskDataEntry>
			{
			public:
				OpActivityTaskDataVisitor(UInt32 activityId) : m_ActivityId(activityId) {}

				bool operator()(OpActivityTaskDataEntry* data)
				{
					if (!data) return true;

					if (data->opAvtivityId == m_ActivityId)
					{
						taskDatas.push_back(data);
					}

					return true;
				}

				std::vector<OpActivityTaskDataEntry*> taskDatas;

			private:
				UInt32 m_ActivityId;
			};

			OpActivityTaskDataVisitor visitor(info.dataId);
			OpActivityTaskDataEntryMgr::Instance()->Visit(visitor);

			for (auto taskData : visitor.taskDatas)
			{
				if (!data->taskIds.empty())
				{
					if (std::find(data->taskIds.begin(), data->taskIds.end(), taskData->id) != data->taskIds.end())
					{
						continue;
					}
				}
				taskDatas.push_back(taskData);
			}
		}

		if (!taskDatas.empty())
		{
			std::sort(taskDatas.begin(), taskDatas.end(), CustomLessOpActTaskData());

			if (info.tasks.empty())
			{
				for (auto taskData : taskDatas)
				{
					if (!taskData) continue;

					OpActTaskData task;
					task.dataId = taskData->id;
					task.completeNum = taskData->completeCount;
					task.rewards = taskData->rewards;
					task.variables = taskData->variables;
					task.variables2 = taskData->variables2;
					task.counters = taskData->counters;
					task.taskName = taskData->taskName;
					task.varProgressName = taskData->varProgressName;
					task.playerLevelLimit = taskData->playerLevelLimit;
					task.accountDailySubmitLimit = taskData->accountDailySubmitLimit;
					task.accountTotalSubmitLimit = taskData->accountTotalSubmitLimit;
					task.resetType = taskData->restType;
					task.acceptType = taskData->acceptType;
					task.eventType = taskData->eventType;
					task.subType = taskData->subType;
					task.accountWeeklySubmitLimit = taskData->accountWeeklySubmitLimit;
					task.accountTask = taskData->accountTask;

					if (!info.taskDesc.empty())
					{
						info.taskDesc += "|";
					}
					info.taskDesc += taskData->taskDesc;

					info.tasks.push_back(task);
				}
			}

			switch ((OpActivityType)info.tmpType)
			{
			case OAT_DAY_LOGIN:
				if (gameStartTime == 0)
				{
					break;
				}
			case OAT_DAILY_BUFF:
				if (!CustomGenOpActCompleteCount(info))
				{
					ErrorLogStream << "Generate opActivity(" << info.dataId << ")'s value failed!" << LogStream::eos;
					return false;
				}
				break;

			default: break;
			}
		}

		return true;
	}

	bool CustomGenOpActCompleteCount(OpActivityRegInfo& opActInfo)
	{
		// 自定义计数
		UInt32 customCompleteCount = 0;

		for (auto& taskData : opActInfo.tasks)
		{
			switch ((OpActivityType)opActInfo.tmpType)
			{
			case OAT_DAY_LOGIN:
				if (customCompleteCount == 0)
				{
					UInt64 dayBeginTime = GetDayBeginTime(Avalon::Time((UInt64)opActInfo.startTime * SEC_TO_MSEC));
					customCompleteCount = UInt32(dayBeginTime / SEC_TO_MSEC);
				}
				else
				{
					customCompleteCount += DAY_TO_SEC;
				}
				break;

			case OAT_DAILY_BUFF:
				customCompleteCount = opActInfo.startTime;
				break;

			default: continue;
			}

			if (customCompleteCount == 0)
			{
				ErrorLogStream << "OpActivity(" << opActInfo.dataId << ")'s start time=" << opActInfo.startTime << LogStream::eos;
				return false;
			}

			taskData.completeNum = customCompleteCount;
		}

		return true;
	}

	bool IsInVeteranReturnPeriod(UInt32 returnTime)
	{
		if (returnTime == 0) return false;

		auto activitySystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_VETERAN_RETURN);
		if (!activitySystemData)
		{
			ErrorLogStream << "Can not find activity system data(" << (UInt32)ACTEM_VETERAN_RETURN << ")!" << LogStream::eos;
			return false;
		}

		if (activitySystemData->isClose) return false;

		UInt32 veteranReturnDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_RETURN_DAYS);
		UInt32 dueTime = returnTime + veteranReturnDays * DAY_TO_SEC;
		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

		// 还在回归期内
		if (returnTime <= curTime && curTime <= dueTime)
		{
			return true;
		}

		return false;
	}

	bool CheckVeteranReturn(UInt32 accId, UInt16 maxLevelInRoles, UInt32 veteranReturnTime, UInt32 accountOfflineTime)
	{
		if (GAME_FIRST_START_TIME == 0)
		{
			ErrorLogStream << "GAME_FIRST_START_TIME is zero!" << LogStream::eos;
			return false;
		}

		auto activitySystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_VETERAN_RETURN);
		if (!activitySystemData)
		{
			ErrorLogStream << "Can not find activity system data(" << (UInt32)ACTEM_VETERAN_RETURN << ")!" << LogStream::eos;
			return false;
		}

		if (activitySystemData->isClose) return false;

		DebugLogStream << "Player(" << accId << ") max level in roles is " << maxLevelInRoles << ", veteran return time is " << veteranReturnTime << ", account offline time is " << accountOfflineTime << LogStream::eos;

		UInt64 curTime = CURRENT_TIME.MSec();

		UInt64 serviceBeginTime = GetDayBeginTime((UInt64)GAME_FIRST_START_TIME * SEC_TO_MSEC);
		UInt32 veteranDefineServiceDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_DEFINE_SERVICE_DAYS);
		UInt64 serviceBeginTimeLimit = serviceBeginTime + (UInt64)veteranDefineServiceDays * DAY_TO_MSEC;

		// 开服天数大于*天
		if (curTime < serviceBeginTimeLimit)
		{
			ErrorLogStream << "Service begin time limit=" << serviceBeginTimeLimit << LogStream::eos;
			return false;
		}

		UInt32 levelLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_DEFINE_ROLE_LEVEL);

		// 账号下有任意*级以上角色
		if ((UInt32)maxLevelInRoles < levelLimit)
		{
			ErrorLogStream << "Player(" << accId << ")'s max level in roles reach limit(" << levelLimit << ")!" << LogStream::eos;
			return false;
		}

		UInt32 veteranDefineReturnCD = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_DEFINE_RETURN_COOLDOWN);
		UInt64 veteranReturnTimeLimit = (UInt64)veteranReturnTime * SEC_TO_MSEC + (UInt64)veteranDefineReturnCD * DAY_TO_MSEC;

		// 每个账号*天内只享有一次回归奖励
		if (curTime < veteranReturnTimeLimit)
		{
			ErrorLogStream << "Player(" << accId << ") reach veteran return time limit(" << veteranReturnTimeLimit << ")!" << LogStream::eos;
			return false;
		}

		if (accountOfflineTime == 0)
		{
			ErrorLogStream << "Account offline time is zero!" << LogStream::eos;
			return false;
		}

		UInt64 offlineBeginTime = GetDayBeginTime((UInt64)accountOfflineTime * SEC_TO_MSEC) + DAY_TO_MSEC;
		UInt32 veteranDefineOfflineDays = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_DEFINE_OFFLINE_DAYS);
		UInt64 offlineTimeLimit = offlineBeginTime + (UInt64)veteranDefineOfflineDays * DAY_TO_MSEC;

		// 连续超过*天未登录
		if (curTime < offlineTimeLimit)
		{
			ErrorLogStream << "Player(" << accId << ") reach offline time limit(" << offlineTimeLimit << ")!" << LogStream::eos;
			return false;
		}

		InfoLogStream << "Player(" << accId << ") veteran return, service begin time limit=" << serviceBeginTimeLimit << ", level limit=" << levelLimit 
			<< ", veteran return time limit=" << veteranReturnTimeLimit << ", account offline time limit=" << offlineTimeLimit << LogStream::eos;

		return true;
	}
}