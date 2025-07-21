#include "CLTaskDataEntry.h"
#include "CLTaskDefine.h"

TaskDataEntry::TaskDataEntry()
{
}

TaskDataEntry::~TaskDataEntry()
{
}
/*
bool TaskDataEntry::Read(Avalon::DataRow& row)
{
	id =	row.ReadUInt32();
	name =	row.ReadString();
	type =	row.ReadUInt8();
	triggerLevel =	row.ReadUInt16();
	preTask =		row.ReadUInt32();
	timeLimit =		row.ReadUInt32();
	abandomTimeLimit = row.ReadUInt32();

	std::istringstream stream(row.ReadString());
	char split = 0;
	while(stream)
	{
		ItemReward item;
		item.DecodeString(stream);
		if(item.id == 0) break;
		rewards.push_back(item);
		stream >> split;
	}
	describe =	row.ReadString();

	acceptNpc =		row.ReadUInt32();
	initTarget =	row.ReadString();
	unfinishedTarget =	row.ReadString();
	submitNpc =		row.ReadUInt32();
	finishedTarget =	row.ReadString();

	return true;
}
*/
bool TaskDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	param0 = row.ReadUInt32();
	type = (TaskType)row.ReadUInt8();
	subType = (TaskSubType)row.ReadUInt8();
	cycleType = (CycleTaskType)row.ReadUInt32();
	cycleWeight = row.ReadUInt32();
	poolType = (DailyTaskPoolType)row.ReadInt32();		//added by huchenhui 2016.09.06
	poolId = row.ReadInt32();							//added by huchenhui 2016.09.06
	dungeon = row.ReadUInt32();
	preTask = row.ReadUInt32();
	isServerTake = row.ReadInt8(); //added by aprilliu, 2016.05.17
	maxLevel = row.ReadInt16();
	triggerLevel = row.ReadUInt16();
	//occupation = row.ReadInt8();
	std::string occuSplit = "|";
	std::string strOccus = row.ReadString();
	std::vector<std::string> occuVec;
	Avalon::StringUtil::Split(strOccus, occuVec, occuSplit);
	for (UInt32 i = 0; i < occuVec.size(); ++i)
	{
		Occupation occu = (Occupation)Avalon::StringUtil::ConvertToValue<UInt32>(occuVec[i]);
		occus.push_back(occu);
	}

	std::string split1 = ",";
	std::string split2 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		OccuItemReward reward;
		if (strTmp.size() == 3)
		{
			reward.occu = (Occupation)Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.itemReward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			reward.itemReward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);
		}
		else if (strTmp.size() == 5)
		{
			reward.occu = (Occupation)Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.itemReward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			reward.itemReward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);
			reward.itemReward.equipType = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[3]);
			reward.itemReward.strenth = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[4]);
		}
		else
		{
			continue;
		}
		occuRewards.push_back(reward);
	}

	strRewards = row.ReadString();
	stdRewardVec.clear();
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		ItemReward reward;
		if (strTmp.size() == 2)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		}
		else if (strTmp.size() == 4)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			reward.equipType = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);
			reward.strenth = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[3]);
		}
		else
		{
			continue;
		}

		rewards.push_back(reward);
	}

	rewardAdapterId = row.ReadUInt32();

	publishType = row.ReadUInt8();
	submitType = row.ReadUInt8();
	acceptNpc = row.ReadUInt32();
	submitNPC = row.ReadUInt32();
	UInt32 tmpValue = row.ReadUInt32();
	isAnnouncement = tmpValue == 1 ? true : false;
	submitCount = row.ReadUInt32();
	vitality = row.ReadUInt32();
	params = row.ReadString();		//added by aprilliu,2016.05.25

	templateId = row.ReadUInt32();
	scriptParam = row.ReadString();

	open = row.ReadUInt8();

	FinishRightNowLevel = row.ReadInt16();
	FinishRightNowItemType = row.ReadUInt32();
	FinishRightNowItemNum = row.ReadUInt32();

	//
	timeLimit = 0;
	abandomTimeLimit = 0;

	return true;
}

UInt8 TaskDataEntry::GetTargetOccupation()
{
	return Avalon::StringUtil::ConvertToValue<UInt8>(params.c_str());
}

UInt32 TaskDataEntry::GetDailyTaskScore()
{
	return Avalon::StringUtil::ConvertToValue<UInt32>(params.c_str());
}

std::vector<ItemReward> TaskDataEntry::GetOccuItemRewards(Occupation occu)
{
	std::vector<ItemReward> result;
	for (UInt32 i = 0; i < occuRewards.size(); ++i)
	{
		if (occu == occuRewards[i].occu || occuRewards[i].occu == Occupation::OCCU_NONE)
		{
			result.push_back(occuRewards[i].itemReward);
		}
	}
	return result;
}

bool TaskDataEntry::IsLegendOccuItemCheck(Occupation occu)
{
	if (occuRewards.size() == 0) return true;
	for (UInt32 i = 0; i < occuRewards.size(); ++i)
	{
		if (occu == occuRewards[i].occu || occuRewards[i].occu == Occupation::OCCU_NONE)
		{
			return true;
		}
	}
	return false;
}

bool TaskDataEntry::CheckOccu(Occupation occu)
{
	if (occus.size() == 0) return true;
	for (UInt32 i = 0; i < occus.size(); ++i)
	{
		if (occus[i] == 0 || occus[i] == occu)
		{
			return true;
		}
	}
	return false;
}

TaskDataEntryMgr::TaskDataEntryMgr()
{
}

TaskDataEntryMgr::~TaskDataEntryMgr()
{
}

//该函数会在TaskDataEntryMgr::Instance()->LoadData()中被调用，是Avalon::DataEntryMgrBase<TaskDataEntry>的虚函数
bool TaskDataEntryMgr::AddEntry(TaskDataEntry* entry)
{
	if(!Avalon::DataEntryMgrBase<TaskDataEntry>::AddEntry(entry)) return false;

	OnAddEntry(entry);

	return true;
}

void TaskDataEntryMgr::OnAddEntry(TaskDataEntry* entry)
{
	if (entry->triggerLevel != 0 && !IsDailyTask(entry->type) 
		&& !IsAchievementTask(entry->type) && !IsCycleTask(entry->type) 
		&& !IsLegendTask(entry->type) && !IsMasterTask(entry->type) && entry->open)
	{
		m_LvlTasks[entry->triggerLevel].push_back(entry);
	}

	//huchenhui 2016.07.14 增加成就后置任务map
	if (entry->preTask != 0 && !IsDailyTask(entry->type) 
		&& !IsCycleTask(entry->type) && !IsLegendTask(entry->type) && !IsMasterTask(entry->type))
	{
		if (IsAchievementTask(entry->type))
			m_PostAchievementTask[entry->preTask].push_back(entry);
		else if (IsRedPacketTask(entry->type))
			m_PostRedPacketTasks[entry->preTask].push_back(entry);
		else
			m_PostTasks[entry->preTask].push_back(entry);
	}

	if (entry->acceptNpc != 0 && !IsDailyTask(entry->type) 
		&& !IsAchievementTask(entry->type) && !IsLegendTask(entry->type) && !IsMasterTask(entry->type))
	{
		m_AcceptTasks[entry->acceptNpc].push_back(entry->id);
	}

	if (entry->submitNPC != 0)
	{
		m_SubmitTasks[entry->submitNPC].push_back(entry->id);
	}

	//added by aprilliu, 2016.04.12
	if (entry->dungeon)
	{
		m_DungeonTasks[entry->dungeon].push_back(entry->id);
	}

	//added by huchenhui 2016.06.25
	if (IsAchievementTask(entry->type) && entry->preTask == 0)
	{
		m_AchievementTasks.push_back(entry);
	}

	//added by huchenhui 2019.02.15
	if (IsAccountAchievementTask(entry->type))
	{
		m_AccountAchievementTasks.push_back(entry);
	}

	if (IsAdventureTeamTask(entry->type))
	{
		m_AllAdventureTeamTasks.push_back(entry);

		UInt32 grade = Avalon::StringUtil::ConvertToValue<UInt32>(entry->params);
		switch ((AdventureTeamTaskGrade)grade)
		{
		case ADTT_GRADE_C:
			m_AdventureTeamCGTasks.push_back(entry);
			break;
		case ADTT_GRADE_A:
			m_AdventureTeamBGTasks.push_back(entry);
			break;
		case ADTT_GRADE_B:
			m_AdventureTeamAGTasks.push_back(entry);
			break;
		case ADTT_GRADE_S:
			m_AdventureTeamSGTasks.push_back(entry);
			break;
		case ADTT_GRADE_SS:
			m_AdventureTeamSSGTasks.push_back(entry);
			break;
		case ADTT_GRADE_SSS:
			m_AdventureTeamSSSGTasks.push_back(entry);
			break;

		default:
			ErrorLogStream << "error grade(" << grade << ")." << LogStream::eos;
			break;
		}
	}

	//added by huchenhui 2016.12.16
	if (IsRedPacketTask(entry->type) && entry->preTask == 0)
	{
		m_RedPacketTasks.push_back(entry);
	}

	if (IsDailyTask(entry->type) && IsValidDailyTask(entry->poolType))
	{
		for (UInt32 i = entry->triggerLevel; i <= entry->maxLevel; ++i)
		{
			//必出
			if (entry->poolType == DAILY_POOL_SURE)
			{
				m_SureDailyTasks[i].push_back(entry);
				continue;
			}

			DailyTaskPool::iterator iter = m_PoolMap[i][entry->poolType].begin();
			while (iter != m_PoolMap[i][entry->poolType].end())
			{
				if (iter->pooId == entry->poolId)
				{
					iter->dailyTasks.push_back(entry);
					break;
				}
				++iter;
			}
			if (iter == m_PoolMap[i][entry->poolType].end())
			{
				DailyTaskSubPool subPool;
				subPool.pooId = entry->poolId;
				subPool.dailyTasks.push_back(entry);
				m_PoolMap[i][entry->poolType].push_back(subPool);
			}
		}
	}

	if (IsCycleTask(entry->type) && entry->open)
	{
		for (UInt32 i = entry->triggerLevel; i <= entry->maxLevel; ++i)
		{
			m_LevelCycleTasks[i][entry->cycleType].cycleTask.push_back(entry);
			m_LevelCycleTasks[i][entry->cycleType].sumWeight += entry->cycleWeight;
		}
	}

	if (IsLegendTask(entry->type))
	{
		for (UInt32 i = entry->triggerLevel; i <= entry->maxLevel; ++i)
		{
			m_LevelLegendTasks[i].push_back(entry);
		}
	}

	if (IsMasterTask(entry->type))
	{
		if (IsMasterDailyTask(entry->type))
		{
			m_MasterDailyTasks.push_back(entry);
		}
		else if (IsMasteAcademicTask(entry->type))
		{
			m_MasterAcademicGrowthTasks.push_back(entry);
		}
	}
}

const static TaskDataEntryMgr::TaskIdVec NullTaskIds;
const static TaskDataEntryMgr::TaskDataVec NullTaskDatas;

const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetLvlTasks(UInt16 level) const
{
	TaskDataVecMap::const_iterator iter = m_LvlTasks.find(level);
	if(iter != m_LvlTasks.end())
	{
		return iter->second;
	}
	return NullTaskDatas;
}

const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetPostTasks(UInt32 taskid) const
{
	TaskDataVecMap::const_iterator iter = m_PostTasks.find(taskid);
	if(iter != m_PostTasks.end())
	{
		return iter->second;
	}
	return NullTaskDatas;
}

void TaskDataEntryMgr::GetAcceptTasks(UInt32 npcid, TaskIdVec& tasks) const
{
	TaskIdVecMap::const_iterator iter = m_AcceptTasks.find(npcid);
	if(iter != m_AcceptTasks.end())
	{
		tasks.insert(tasks.end(), iter->second.begin(), iter->second.end());
	}
}

void TaskDataEntryMgr::GetSubmitTasks(UInt32 npcid, TaskIdVec& tasks) const
{
	TaskIdVecMap::const_iterator iter = m_SubmitTasks.find(npcid);
	if(iter != m_SubmitTasks.end())
	{
		tasks.insert(tasks.end(), iter->second.begin(), iter->second.end());
	}
}

//huchenhui 2016.06.23
void TaskDataEntryMgr::GetDailyTasks(TaskType type, UInt16 level, TaskDataVec& tasks)
{
	//if(level < 5) return;
	if(!IsDailyTask(type)) return;

	const TaskDataVec& dailyTasks = m_DailyTasks[type];
	for(TaskDataVec::const_iterator iter = dailyTasks.begin(); iter != dailyTasks.end(); iter++)
	{
		if((*iter)->triggerLevel <= level)
			tasks.push_back(*iter);
	}
}

void TaskDataEntryMgr::GetAchievementTasks(UInt16 level, TaskDataVec& tasks)
{
	for (TaskDataVec::const_iterator iter = m_AchievementTasks.begin(); iter != m_AchievementTasks.end(); iter++)
	{
		if ((*iter)->triggerLevel <= level) tasks.push_back(*iter);
	}
}

void TaskDataEntryMgr::GetAccountAchievementTasks(TaskDataVec& tasks)
{
	for (TaskDataVec::const_iterator iter = m_AccountAchievementTasks.begin(); iter != m_AccountAchievementTasks.end(); iter++)
	{
		tasks.push_back(*iter);
	}
}

void TaskDataEntryMgr::GetAllAdventureTeamTasks(TaskDataVec& tasks)
{
	tasks = m_AllAdventureTeamTasks;
}

bool TaskDataEntryMgr::RandAdventureTeamTasks(TaskDataVec& tasks, UInt32 taskGrade, UInt32 taskNum)
{
	TaskDataVec tmpTasks;
	switch ((AdventureTeamTaskGrade)taskGrade)
	{
	case ADTT_GRADE_C:
		tmpTasks = m_AdventureTeamCGTasks;
		break;
	case ADTT_GRADE_A:
		tmpTasks = m_AdventureTeamBGTasks;
		break;
	case ADTT_GRADE_B:
		tmpTasks = m_AdventureTeamAGTasks;
		break;
	case ADTT_GRADE_S:
		tmpTasks = m_AdventureTeamSGTasks;
		break;
	case ADTT_GRADE_SS:
		tmpTasks = m_AdventureTeamSSGTasks;
		break;
	case ADTT_GRADE_SSS:
		tmpTasks = m_AdventureTeamSSSGTasks;
		break;

	default:
		ErrorLogStream << "error grade(" << taskGrade << ")." << LogStream::eos;
		return false;
	}

	if (tmpTasks.size() <= taskNum)
	{
		tasks = tmpTasks;
	}
	else
	{
		std::random_shuffle(tmpTasks.begin(), tmpTasks.end());
		for (UInt32 idx = 0; idx < taskNum; ++idx)
		{
			tasks.push_back(tmpTasks[idx]);
		}
	}

	return true;
}

bool TaskDataEntryMgr::RandAdventureTeamTasks(TaskDataVec& tasks, const std::map<UInt32, UInt32>& taskGrades)
{
	if (taskGrades.empty()) return false;

	for (auto itr = taskGrades.begin(); itr != taskGrades.end(); ++itr)
	{
		TaskDataVec tmpTasks;
		RandAdventureTeamTasks(tmpTasks, itr->first, itr->second);
		tasks.insert(tasks.end(), tmpTasks.begin(), tmpTasks.end());
	}
	return true;
}

const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetPostAchievementTasks(UInt32 taskid) const
{
	TaskDataVecMap::const_iterator iter = m_PostAchievementTask.find(taskid);
	if (iter != m_PostAchievementTask.end())
	{
		return iter->second;
	}
	return NullTaskDatas;
}

void TaskDataEntryMgr::GetRedPackTasks(TaskDataVec& tasks)
{
	tasks = m_RedPacketTasks;
}

const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetPostRedPackTasks(UInt32 taskId) const
{
	TaskDataVecMap::const_iterator iter = m_PostRedPacketTasks.find(taskId);
	if (iter != m_PostRedPacketTasks.end())
	{
		return iter->second;
	}
	return NullTaskDatas;
}

//added by aprilliu, 2016.04.20
void TaskDataEntryMgr::GetDungeonTasks(UInt32 dungeonId, TaskIdVec& tasks) const
{
	TaskIdVecMap::const_iterator iter = m_DungeonTasks.find(dungeonId);
	if (iter != m_DungeonTasks.end())
	{
		tasks.insert(tasks.end(), iter->second.begin(), iter->second.end());
	}
}

const static TaskDataEntryMgr::DailyTaskPool NullDailyTaskTool;

const TaskDataEntryMgr::DailyTaskVec& TaskDataEntryMgr::GetSureDailyTasks(UInt16 level)
{
	return m_SureDailyTasks[level];
}

const TaskDataEntryMgr::DailyTaskPool& TaskDataEntryMgr::GetDailyTaskPool(UInt16 level, DailyTaskPoolType type)
{
	DailyTaskPoolMap::const_iterator iter = m_PoolMap[level].find(type);
	if (iter != m_PoolMap[level].end())
	{
		return iter->second;
	}
	return NullDailyTaskTool;
}

const static TaskDataEntryMgr::CycleTypeTasks NullCycleTaskTool;
const TaskDataEntryMgr::CycleTypeTasks& TaskDataEntryMgr::GetCycleTasks(UInt16 level, CycleTaskType type)
{
	std::map<CycleTaskType, CycleTypeTasks>::const_iterator iter = m_LevelCycleTasks[level].find(type);
	if (iter != m_LevelCycleTasks[level].end())
	{
		return iter->second;
	}
	return NullCycleTaskTool;
}

TaskDataEntry* TaskDataEntryMgr::RandomCycleTask(UInt16 level, CycleTaskType type)
{
	const CycleTypeTasks tasks = GetCycleTasks(level, type);
	if (tasks.cycleTask.empty()) return NULL;
	UInt32 ranNum = Avalon::Random::RandBetween(1, tasks.sumWeight);
	for (UInt32 i = 0; i < tasks.cycleTask.size(); ++i)
	{
		if (ranNum>tasks.cycleTask[i]->cycleWeight)
			ranNum -= tasks.cycleTask[i]->cycleWeight;
		else
			return tasks.cycleTask[i];
	}
	return NULL;
}

const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetLegendTasks(UInt16 level)
{
	LevelTaskDataMap::const_iterator iter = m_LevelLegendTasks.find(level);
	if (iter != m_LevelLegendTasks.end())
	{
		return iter->second;
	}
	return NullTaskDatas;
}


const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetMasterDailyTasks()
{
	return m_MasterDailyTasks;
}

const TaskDataEntry* TaskDataEntryMgr::GetMasterDailyTask(UInt32 key)
{
	for (UInt32 i = 0; i < m_MasterDailyTasks.size(); i++)
	{
		if (m_MasterDailyTasks[i]->GetKey() == key)
		{
			return m_MasterDailyTasks[i];
		}
	}
	return NULL;
}

const TaskDataEntryMgr::TaskDataVec& TaskDataEntryMgr::GetMasterAcademicGrowthTasks()
{
	return m_MasterAcademicGrowthTasks;
}

const TaskDataEntry* TaskDataEntryMgr::GetMasterAcademicGrowthTask(UInt32 key)
{
	for (UInt32 i = 0; i < m_MasterAcademicGrowthTasks.size(); i++)
	{
		if (m_MasterAcademicGrowthTasks[i]->GetKey() == key)
		{
			return m_MasterAcademicGrowthTasks[i];
		}
	}
	return NULL;
}

DailyTaskDataEntry::DailyTaskDataEntry()
{
}

DailyTaskDataEntry::~DailyTaskDataEntry()
{
}

bool DailyTaskDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadInt32();
	type = (DailyTaskPoolType)row.ReadInt32();
	poolId = row.ReadInt32();

	return true;
}

DailyTaskDataEntryMgr::DailyTaskDataEntryMgr()
{
}

DailyTaskDataEntryMgr::~DailyTaskDataEntryMgr()
{
}

bool DailyTaskDataEntryMgr::AddEntry(DailyTaskDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DailyTaskDataEntry>::AddEntry(entry)) return false;

	return true;
}

DailyTaskScoreRewardDataEntry::DailyTaskScoreRewardDataEntry()
{}

DailyTaskScoreRewardDataEntry::~DailyTaskScoreRewardDataEntry()
{}


bool DailyTaskScoreRewardDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	score = row.ReadUInt32();

	std::string split = "|";
	std::string split3 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split3);
		if (strTmp.size() < 2) continue;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		itemReward.push_back(reward);
	}
	return true;
}

AchievementLevelDataEntry::AchievementLevelDataEntry()
{
}

AchievementLevelDataEntry::~AchievementLevelDataEntry()
{
}

bool AchievementLevelDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	minScore = row.ReadUInt32();
	maxScore = row.ReadUInt32();

	std::string split = "|";
	std::string strItemIds = row.ReadString();
	std::vector<std::string> strItemIdVec;
	Avalon::StringUtil::Split(strItemIds, strItemIdVec, split);

	std::string strItemCounts = row.ReadString();
	std::vector<std::string> strItemCountVec;
	Avalon::StringUtil::Split(strItemCounts, strItemCountVec, split);

	if (strItemCountVec.size() != strItemIdVec.size())
	{
		return false;
	}

	for (UInt32 i = 0; i < strItemIdVec.size(); ++i)
	{
		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strItemIdVec[i]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strItemCountVec[i]);
		itemReward.push_back(reward);
	}
	return true;
}


WeekSignSpringDataEntry::WeekSignSpringDataEntry()
{

}

WeekSignSpringDataEntry::~WeekSignSpringDataEntry()
{

}

UInt32 WeekSignSpringDataEntry::GetKey() const
{
	return id;
}

bool WeekSignSpringDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	startLv = row.ReadUInt32();
	endLv = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				if (value > 0) dungeonTag.push_back(value);
			}
		}
	}


	return true;
}



bool WeekSignSpringDataEntryMgr::AddEntry(WeekSignSpringDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeekSignSpringDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

bool WeekSignSpringDataEntryMgr::IsInScope(UInt32 lv, UInt32 tag)
{
	class WeekSignSpringVisit : public Avalon::DataEntryVisitor<WeekSignSpringDataEntry>
	{
	public:
		WeekSignSpringVisit(UInt32 lv, UInt32 tag) : m_Lv(lv), m_Tag(tag) {}
		bool operator()(WeekSignSpringDataEntry* dataEntry)
		{
			if (dataEntry->startLv <= m_Lv && m_Lv <= dataEntry->endLv)
			{
				for (auto &i : dataEntry->dungeonTag)
				{
					if (i == m_Tag)
					{
						flag = true;
					}
				}
			}
			return true;
		}
		bool flag = false;
	private:
		UInt32 m_Lv = 0;
		UInt32 m_Tag = 0;
		
	};
	WeekSignSpringVisit vistor(lv, tag);
	WeekSignSpringDataEntryMgr::Instance()->Visit(vistor);
	return vistor.flag;
}




