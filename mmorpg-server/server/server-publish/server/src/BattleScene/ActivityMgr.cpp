#include "ActivityMgr.h"

#include <TimeUtil.h>

#include "Player.h"
#include "PlayerMgr.h"
#include "CLOpActiveProtocol.h"

ActivityMgr::ActivityMgr()
{
	
}

ActivityMgr::~ActivityMgr()
{
}

void ActivityMgr::OpenActivity(UInt32 id, const std::string& name, UInt16 level, UInt32 preTime, UInt32 startTime, UInt32 dueTime, UInt8 state)
{
	for(ActivityVec::iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if(iter->id == id)
		{
			iter->state = state;
			iter->dueTime = dueTime;
			return;
		}
	}

	ActivityInfo info(id, name, level, preTime, startTime, dueTime);
	info.state = state;
	m_Activities.push_back(info);

	//通知活动任务
	NotifyPlayerAcceptActiveTask(id);
}

void ActivityMgr::CloseActivity(UInt32 id)
{
	for(ActivityVec::iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if(iter->id == id)
		{
			iter->state = 0;
			//通知删除活动任务
			NotifyPlayerRemoveActiveTask(iter->id);
			NotifyPlayerActiveClosed(iter->id);
			m_Activities.erase(iter);

			return;
		}
	}

	
}

void ActivityMgr::OpenTribeActivity(ObjID_t uTribeId, const std::string& name, UInt16 level, UInt32 dueTime)
{
	ActivityVec& activities = m_TribeActivities[uTribeId];
	for(ActivityVec::iterator iter = activities.begin();
		iter != activities.end(); ++iter)
	{
		if(iter->name == name) return;
	}
	activities.push_back(ActivityInfo(0, name, level, 0, 0, dueTime));
}

void ActivityMgr::CloseTribeActivity( ObjID_t uTribeId, const std::string& name)
{
	ActivityVec& activities = m_TribeActivities[uTribeId];
	for(ActivityVec::iterator iter = activities.begin();
		iter != activities.end(); ++iter)
	{
		if(iter->name == name)
		{
			activities.erase(iter);
			return;
		}
	}
}

void ActivityMgr::SetActivities(const ActivityVec& activities, const TribeActivityMap &tribeactivities)
{
	m_Activities = activities;
	m_TribeActivities = tribeactivities;
}

bool ActivityMgr::IsInActivity(const std::string& name) const
{
	for(ActivityVec::const_iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if(iter->name == name && 1 == iter->state) return true;
	}
	return false;
}

bool ActivityMgr::IsInActivity(Player* player, const std::string& name) const
{
	for(ActivityVec::const_iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		//if(player->GetSrcZoneId() == 0 && iter->name == name && 1 == iter->state) return true;
		if (iter->name == name && 1 == iter->state && iter->level <= player->GetLevel())
			return true;
	}
	return false;
}

bool ActivityMgr::IsInActivity(UInt32 activeId) const
{
	for (ActivityVec::const_iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if (iter->id == activeId && 1 == iter->state) return true;
	}
	return false;
}

bool ActivityMgr::IsInActivity(UInt32 activeId, UInt32 time) const
{
	for (ActivityVec::const_iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if (iter->id == activeId && 1 == iter->state &&
			iter->startTime <= time && iter->dueTime >= time) 
			return true;
	}
	return false;
}

bool ActivityMgr::IsInActivity(UInt32 activeId, Player* player) const
{
	if (!player)
	{
		return false;
	}

	auto playerAct = player->GetPlayerActivityInfo().FindActivity(activeId);
	if (playerAct != NULL)
	{
		if (playerAct->state == AS_IN && playerAct->level <= player->GetLevel())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	for (const auto& activity : m_Activities)
	{
		if (activity.id == activeId && 1 == activity.state && activity.level <= player->GetLevel())
		{
			return true;
		}
	}

	return false;
}

UInt32 ActivityMgr::GetActivityStartTime(const std::string& name) const
{
	for(ActivityVec::const_iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if(iter->name == name && 1 == iter->state) return iter->startTime; 
	}
	return 0;
}

bool ActivityMgr::IsInTribeActivity(ObjID_t uTribeId, const std::string& name) const
{
	if(0 == uTribeId) return false;

	TribeActivityMap::const_iterator	itertribe = m_TribeActivities.find(uTribeId);
	if(itertribe == m_TribeActivities.end()) return false;

	const ActivityVec& activities = itertribe->second;
	for(ActivityVec::const_iterator iter = activities.begin();
		iter != activities.end(); ++iter)
	{
		if(iter->name == name) return true;
	}

	return false;
}

void ActivityMgr::GetOpenActivity(Player* player, std::vector<ActivityInfo>& activities)
{
	if (!player)
		return;

	UInt32 day = player->GetCreateFromNowOnDay();
	UInt32 dueDay = 10;
	for (auto activity : m_Activities)
	{
		switch (activity.id)
		{
			case 7100:
			{
						 if (day > dueDay)	activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7200:
			{
						 if (day < 2 || day > dueDay) activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7300:
			{
						 if (day < 3 || day > dueDay) activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7400:
			{
						 if (day < 4 || day > dueDay)	activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7500:
			{
						 if (day < 5 || day > dueDay) activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7600:
			{
						 if (day < 6 || day > dueDay) activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7700:
			{
						 if (day < 7 || day > dueDay) activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			case 7800:
			{
						 if (day > dueDay)	activity.state = 0;
						 else activity.state = 1;
						 break;
			}
			default:	break;
		}

		if (activity.state != 0 &&
			activity.level <= player->GetLevel())
		{
			activities.push_back(activity);
		}
	}
}

void ActivityMgr::NotifyPlayerAcceptActiveTask(UInt32 id)
{
	class AcceptActiveTaskVisit : public PlayerVisitor
	{
	public:
		AcceptActiveTaskVisit(UInt32 activeId)
			:m_activeId(activeId){}

		bool operator()(Player* player)
		{
			player->AcceptActiveTask(m_activeId);
			return true;
		}

	private:
		UInt32	m_activeId;
	};

	AcceptActiveTaskVisit visitor(id);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void ActivityMgr::NotifyPlayerRemoveActiveTask(UInt32 id)
{
	class RemoveActiveTaskVisit : public PlayerVisitor
	{
	public:
		RemoveActiveTaskVisit(UInt32 activeId)
			:m_activeId(activeId){}

		bool operator()(Player* player)
		{
			player->RemoveActiveTask(m_activeId);
			return true;
		}

	private:
		UInt32	m_activeId;
	};

	RemoveActiveTaskVisit visitor(id);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void ActivityMgr::NotifyPlayerActiveClosed(UInt32 id)
{
	class NotifyPlayerActiveClosedVisit : public PlayerVisitor
	{
	public:
		NotifyPlayerActiveClosedVisit(UInt32 activeId)
			:m_activeId(activeId){}

		bool operator()(Player* player)
		{
			player->OnActiveClosed(m_activeId);
			return true;
		}

	private:
		UInt32	m_activeId;
	};

	NotifyPlayerActiveClosedVisit visitor(id);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void ActivityMgr::OnTick(const Avalon::Time &now)
{
	
}

void ActivityMgr::OnLoadOpActData(OpActRegMap& activeData)
{
	InfoLogStream << "On load opAct data..." << LogStream::eos;

	for (OpActRegMap::iterator itr = activeData.begin(); itr != activeData.end(); ++itr)
	{
		_AddOpActData(itr->second);
	}

	class OpActAddVisit : public PlayerVisitor
	{
	public:
		OpActAddVisit(CLProtocol::SCSyncOpActData protocol)
			:m_protocol(protocol){}

		bool operator()(Player* player)
		{
			if (!player) return true;

			for (auto& info : m_protocol.datas)
			{
				auto playerOpAct = player->GetPlayerActivityInfo().FindOpActivity(info.dataId);
				if (playerOpAct != NULL)
				{
					info = *playerOpAct;
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

				default: break;
				}
			}

			player->SendProtocol(m_protocol);
			return true;
		}

	private:
		CLProtocol::SCSyncOpActData m_protocol;
	};

	//同步客户端活动数据
	CLProtocol::SCSyncOpActData opActProtocol;
	std::vector<OpActivityRegInfo*> openDatas;
	ActivityMgr::Instance()->GetOpActs(openDatas);
	for (size_t i = 0; i < openDatas.size(); ++i)
	{
		opActProtocol.datas.push_back(*openDatas[i]);
	}

	OpActAddVisit visitor(opActProtocol);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

// void ActivityMgr::OnLoadOpActInfo(OpActivityVec& activeInfo)
// {
// 	for (OpActivityVec::iterator itr = activeInfo.begin(); itr != activeInfo.end(); ++itr)
// 	{
// 		m_opActInfoMap[itr->tmpType].push_back(*itr);
// 	}
// 	
// }

void ActivityMgr::OnOpActStateChange(OpActivityRegInfo& info)
{
	auto itr = m_dataIdToOpActData.find(info.dataId);
	if (itr == m_dataIdToOpActData.end())
	{
		//添加
		_AddOpActData(info);
	}
	else
	{
		UInt8 oldStatus = itr->second->state;
		//更新
		itr->second->copy(info);

		if ((oldStatus == AS_PREPARE || oldStatus == AS_END) && info.state == AS_IN)
		{
			OnOpActOpen(itr->second);
		}

		if (oldStatus == AS_IN && info.state == AS_END)
		{
			// 说明活动结束了
			OnOpActClosed(itr->second);
		}
	}
	
	NotifyPlayerOpActChange(info);

	InfoLogStream << "On OpActivity change...(id=" << info.dataId << ", state=" << info.state << ", preT=" << TimeUtil::TimestampToStandardFormat(info.prepareTime)
		<< ", st=" << TimeUtil::TimestampToStandardFormat(info.startTime) << ", et=" << TimeUtil::TimestampToStandardFormat(info.endTime) << ")." << LogStream::eos;
}

void ActivityMgr::OnOpActOpen(OpActivityRegInfo* info)
{
	if (!info) return;

	InfoLogStream << "OpActivity(id=" << info->dataId << ", state=" << info->state << ", preT=" << TimeUtil::TimestampToStandardFormat(info->prepareTime)
		<< ", st=" << TimeUtil::TimestampToStandardFormat(info->startTime) << ", et=" << TimeUtil::TimestampToStandardFormat(info->endTime) << ") open." << LogStream::eos;

	class OpActOpenVisit : public PlayerVisitor
	{
	public:
		OpActOpenVisit(OpActivityRegInfo* info) : m_Info(info) {}

		bool operator()(Player* player)
		{
			return true;
		}

	private:
		OpActivityRegInfo* m_Info;
	};
	OpActOpenVisit visitor(info);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void ActivityMgr::OnOpActClosed(OpActivityRegInfo* info)
{
	if (!info) return;

	InfoLogStream << "OpActivity(id=" << info->dataId << ", state=" << info->state << ", preT=" << TimeUtil::TimestampToStandardFormat(info->prepareTime)
		<< ", st=" << TimeUtil::TimestampToStandardFormat(info->startTime) << ", et=" << TimeUtil::TimestampToStandardFormat(info->endTime) << ") close." << LogStream::eos;

	if (info->tmpType == OAT_APPOINTMENT_OCCU)
	{
		class AppointmentOpActCloseVisit : public PlayerVisitor
		{
		public:
			bool operator()(Player* player)
			{
				player->OnAppointmentActivityClose();
				return true;
			}
		};
		AppointmentOpActCloseVisit visitor;
		PlayerMgr::Instance()->VisitNormalPlayers(visitor);
		return;
	}

	class OpActCloseVisit : public PlayerVisitor
	{
	public:
		OpActCloseVisit(OpActivityRegInfo* info) : m_Info(info) {}

		bool operator()(Player* player)
		{
			return true;
		}

	private:
		OpActivityRegInfo* m_Info;
	};
	OpActCloseVisit visitor(info);
	PlayerMgr::Instance()->VisitPlayers(visitor);
}

// ActivityState ActivityMgr::GetOpActState(UInt32 id)
// {
// 	for (ActivityVec::iterator itr = m_opActivities.begin(); itr != m_opActivities.end(); ++itr)
// 	{
// 		ActivityInfo& tmpInfo = *itr;
// 		if (tmpInfo.id == id)
// 		{
// 			return (ActivityState)tmpInfo.state;
// 		}
// 	}
// 
// 	return AS_END;
// }

OpActTaskData* ActivityMgr::GetOpActTaskData(UInt32 dataId)
{
	auto itr = m_dataIdToTaskDatas.find(dataId);
	if (itr == m_dataIdToTaskDatas.end())
	{
		return NULL;
	}

	return itr->second;
}

void ActivityMgr::GetOpenOpActs(std::vector<OpActivityRegInfo*>& opActs)
{
	
	std::map<UInt32, OpActivityRegInfo*>::iterator itr = m_dataIdToOpActData.begin();
	for (; itr != m_dataIdToOpActData.end(); ++itr)
	{
		if (!itr->second) continue;

		if (itr->second->state == AS_IN)
		{
			opActs.push_back(itr->second);
		}
	}
}

OpActivityRegInfo* ActivityMgr::GetOpActData(UInt32 dataId)
{
	auto itr = m_dataIdToOpActData.find(dataId);
	if (itr == m_dataIdToOpActData.end())
	{
		return NULL;
	}

	return itr->second;

}

void ActivityMgr::GetOpActDataByTmpType(OpActivityType type, std::vector<OpActivityRegInfo*>& activities)
{
	auto itr = m_tmpTypeToOpActDatas.find((UInt32)type);
	if (itr != m_tmpTypeToOpActDatas.end())
	{
		activities = itr->second;
	}
}

std::vector<OpActivityRegInfo*> ActivityMgr::GetOpActDataByTmpTypes(UInt32 tmpType, bool mustInActivity)
{
	std::vector<OpActivityRegInfo*> infoes;

	auto itr = m_tmpTypeToOpActDatas.find(tmpType);
	if (itr != m_tmpTypeToOpActDatas.end())
	{
		if (!mustInActivity)
		{
			return itr->second;
		}

		for (auto info : itr->second)
		{
			if (info->state == AS_IN)
			{
				infoes.push_back(info);
			}
		}
	}

	return infoes;
}

bool ActivityMgr::GetOpActTaskDataVec(UInt32 tmpType, std::vector<OpActTaskData*>& datas)
{
	auto itr = m_tmpTypeToTaskDatas.find(tmpType);
	if (itr == m_tmpTypeToTaskDatas.end())
	{
		return false;
	}

	datas = itr->second;

	return true;
}

void ActivityMgr::NotifyPlayerOpActChange(OpActivityRegInfo& info)
{
	class OpActChangeVisit : public PlayerVisitor
	{
	public:
		OpActChangeVisit(CLProtocol::SCSyncOpActStateChange protocol)
			:m_protocol(protocol){}

		bool operator()(Player* player)
		{
			player->SendProtocol(m_protocol);
			return true;
		}

	private:
		CLProtocol::SCSyncOpActStateChange m_protocol;
	};

	CLProtocol::SCSyncOpActStateChange protocol;
	protocol.info = info;

	OpActChangeVisit visitor(protocol);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

bool ActivityMgr::IsInOpActivity(UInt32 activityId)
{
	auto itr = m_dataIdToOpActData.find(activityId);
	if (itr != m_dataIdToOpActData.end())
	{
		OpActivityRegInfo* info = itr->second;
		if (!info) return false;

		if (info->state == AS_IN)
		{
			return true;
		}
	}

	return false;
}

bool ActivityMgr::IsInOpActivity(UInt32 activityId, Player* player)
{
	if (!player) return false;

	auto playerOpAct = player->GetPlayerActivityInfo().FindOpActivity(activityId);
	if (playerOpAct != NULL)
	{
		if (playerOpAct->state == AS_IN)
		{
			if (playerOpAct->playerLevelLimit > 0 && player->GetLevel() < playerOpAct->playerLevelLimit)
			{
				return false;
			}
			return true;
		}
		return false;
	}

	auto itr = m_dataIdToOpActData.find(activityId);
	if (itr != m_dataIdToOpActData.end())
	{
		OpActivityRegInfo* info = itr->second;
		if (!info) return false;

		if (info->state == AS_IN)
		{
			if (info->playerLevelLimit > 0 && player->GetLevel() < info->playerLevelLimit)
			{
				return false;
			}
			return true;
		}
	}

	return false;
}

bool ActivityMgr::IsInOpActivity(const OpActivityRegInfo* opAct, Player* player)
{
	if (!opAct || !player) return false;

	if (opAct->state != AS_IN)
	{
		return false;
	}

	return true;
}

bool ActivityMgr::IsInOpActivityByTmpType(UInt32 tmpId, UInt16 playerLevel)
{
	auto itr = m_tmpTypeToOpActDatas.find(tmpId);
	if (itr != m_tmpTypeToOpActDatas.end())
	{
		for (auto info : itr->second)
		{
			if (!info) continue;

			if (info->playerLevelLimit > 0 && playerLevel < info->playerLevelLimit)
			{
				continue;
			}

			if (info->state == AS_IN)
			{
				return true;
			}
		}
	}

	return false;
}

UInt32 ActivityMgr::GetOpActivityAddition(UInt32 tmpId, Player* player)
{
	UInt32 addition = 0;
	for (auto elem : m_tmpTypeToOpActDatas)
	{
		if (elem.first == tmpId)
		{
			for (auto info : elem.second)
			{
				if (!info) continue;

				if (IsInOpActivity(info, player))
				{
					addition += info->parm;
				}
			}
		}
	}

	InfoLogStream << "Get OpActivity(" << tmpId << ") addition(" << addition << ")." << LogStream::eos;
	return addition;
}

void ActivityMgr::GetVeteranReturnOpActBuffs(Player* player, std::vector<Buff*>& veteranBuffs, const DungeonDataEntry* dungeonData)
{
	if (!player || !dungeonData) return;

	switch ((DungeonSubType)dungeonData->subType)
	{
	case DST_NORMAL:
	case DST_YUANGU:
	case DST_HELL:
	case DST_HELL_ENTRY:
	case DST_LIMIT_TIME_HELL:
		break;

	default: return;
	}

	std::vector<OpActivityRegInfo*> buffAddOpActivities;
	player->GetPlayerActivityInfo().FindOpActivity(OAT_RETURN_PRIVILEGE, buffAddOpActivities);

	for (auto opAct : buffAddOpActivities)
	{
		if (!opAct) continue;

		if (opAct->state != AS_IN)
		{
			continue;
		}

		for (auto task : opAct->tasks)
		{
			for (auto& buffId : task.variables2)
			{
				auto buff = player->GetBuffMgr().FindBuff(buffId);
				if (!buff)
				{
					ErrorLogStream << PLAYERINFO(player) << "Can not find buff(" << buffId << ")!" << LogStream::eos;
					continue;
				}

				veteranBuffs.push_back(buff);
			}
		}
	}
}

bool ActivityMgr::GetActivityInfo(UInt32 activeId, ActivityInfo& info)
{
	for (const auto& activity : m_Activities)
	{
		if (activity.id == activeId)
		{
			info = activity;
			return true;
		}
	}

	return false;
}

void ActivityMgr::VisitOpenActivities(Player* player, const std::function<bool(const ActivityInfo&)>& F)
{
	if (!player) return;

	for (const auto& activity : m_Activities)
	{
		if (activity.state != AS_IN)
		{
			continue;
		}

		if (activity.level > player->GetLevel())
		{
			continue;
		}

		if (!F(activity))
		{
			break;
		}
	}
}

bool ActivityMgr::GetActivityTime(UInt32 activeId, UInt32& startTime, UInt32& endTime)
{
	for (const auto& activity : m_Activities)
	{
		if (activity.id == activeId)
		{
			startTime = activity.startTime;
			endTime = activity.dueTime;
			return true;
		}
	}

	return false;
}

void ActivityMgr::_AddOpActData(OpActivityRegInfo& info)
{
	OpActivityRegInfo* data = NULL;

	auto itr = m_dataIdToOpActData.find(info.dataId);
	if (itr == m_dataIdToOpActData.end())
	{
		data = new OpActivityRegInfo(info);

		m_dataIdToOpActData[data->dataId] = data;

		std::vector<OpActivityRegInfo*>& datas = m_tmpTypeToOpActDatas[data->tmpType];
		datas.push_back(data);

		std::vector<OpActTaskData*>& taskDatas = m_tmpTypeToTaskDatas[data->tmpType];
		for (size_t i = 0; i < data->tasks.size(); ++i)
		{
			m_dataIdToTaskDatas[data->tasks[i].dataId] = &data->tasks[i];
			taskDatas.push_back(&data->tasks[i]);
		}
	}
	else
	{
		data = itr->second;
		if (!data)
		{
			ErrorLogStream << "OpActivityRegInfo ptr is null! id=" << info.dataId << LogStream::eos;
			return;
		}

		*data = info;
	}
}

bool ActivityMgr::IsJustInActivity(UInt32 activeId, Player* player) const
{
	if (!player)
	{
		return false;
	}

	UInt32 lastTime = (UInt32)(player->GetSavebackTime() / 1000);

	for (const auto& activity : m_Activities)
	{
		if (activity.id == activeId && 1 == activity.state && activity.level <= player->GetLevel())
		{
			// 活动是在玩家下线得时候开始得
			if (activity.startTime > lastTime)
			{
				return true;
			}
		}
	}

	return false;
}

void ActivityMgr::GetOpActs(std::vector<OpActivityRegInfo*>& opActs)
{
	std::map<UInt32, OpActivityRegInfo*>::iterator itr = m_dataIdToOpActData.begin();
	for (; itr != m_dataIdToOpActData.end(); ++itr)
	{
		auto info = itr->second;
		if (!info)
		{
			ErrorLogStream << "OpActivityRegInfo(id: " << itr->first << ") ptr is null!" << LogStream::eos;
			continue;
		}

		if (info->tmpType == OAT_DAY_LOGIN)
		{
			opActs.insert(opActs.begin(), info);
			continue;
		}

		opActs.push_back(info);
		
	}
}

UInt32 ActivityMgr::GetOpenActivityIdByTmplateType(UInt32 actTmpType)
{
	std::map<UInt32, OpActivityRegInfo*>::iterator itr = m_dataIdToOpActData.begin();
	for (; itr != m_dataIdToOpActData.end(); ++itr)
	{
		if (!itr->second) continue;

		if ((itr->second->state == AS_IN || itr->second->state == AS_PREPARE) && itr->second->tmpType == actTmpType)
		{
			return itr->second->dataId;
		}
	}
	return 0;
}

OpActivityRegInfo* ActivityMgr::GetOpenActivityRegInfoByTmplateType(Player* player, OpActivityType actTmpType)
{
	if (!player) return NULL;

	std::vector<OpActivityRegInfo*> opActs;
	ActivityMgr::Instance()->GetOpActDataByTmpType(actTmpType, opActs);
	for (auto opAct : opActs)
	{
		if (!opAct) continue;

		auto playerOpAct = player->GetPlayerActivityInfo().FindOpActivity(opAct->dataId);
		if (playerOpAct != NULL)
		{
			opAct = playerOpAct;
		}

		if (opAct->state != AS_IN)
		{
			continue;
		}

		return opAct;
	}
	return NULL;
}

bool ActivityMgr::CheckOpActivityDataValid(Player* player, UInt32 opActId, OpActivityType opActType)
{
	if (!player) return false;

	auto opAct = GetOpActData(opActId);
	if (!opAct)
	{
		ErrorLogStream << PLAYERINFO(player) << " OpActivity(" << opActId << ")'s data is not exist!" << LogStream::eos;
		return false;
	}

	if (opAct->tmpType != opActType)
	{
		ErrorLogStream << PLAYERINFO(player) << opAct->LogStr() << " type(" << opAct->tmpType << ") is error!" << LogStream::eos;
		return false;
	}

	if (!IsInOpActivity(opAct, player))
	{
		return false;
	}

	return true;
}

void ActivityMgr::LoadActivityAccRecord(UInt32 accid, ActivityAccRecord* record)
{
	AddAccActivityRecord(accid, record);
}

ActivityAccRecord* ActivityMgr::FindAccActivityRecord(UInt32 accid, UInt32 actId)
{
	auto iter = m_accActivityRecordMap.find(accid);
	if (iter != m_accActivityRecordMap.end())
	{
		auto it = iter->second.find(actId);
		if (it != iter->second.end())
		{
			return it->second;
		}
	}

	return NULL;
}

void ActivityMgr::AddAccActivityRecord(UInt32 accid, ActivityAccRecord* record)
{
	if (!record) return;

	auto iter = m_accActivityRecordMap.find(accid);
	if (iter == m_accActivityRecordMap.end())
	{
		std::map<UInt32, ActivityAccRecord*> tempMap;
		tempMap[record->actId] = record;

		m_accActivityRecordMap[accid] = tempMap;
	}
	else
	{
		auto it = iter->second.find(record->actId);
		if (it == iter->second.end())
		{
			iter->second[record->actId] = record;
		}
		else
		{
			CL_SAFE_DELETE(record);
		}
	}
}

void ActivityMgr::UpdateAccActivityRecord(UInt32 accid, const ActivityAccRecord* record, bool isInsert)
{
	if (!record) return;

	if (isInsert)
	{
		CLInsertCommand* insert = CLRecordClient::Instance()->PrepareInsertCommand("t_activity_account_record", CLRecordClient::Instance()->GenGuid());
		insert->PutData("acc_id", accid);
		insert->PutData("activity_id", record->actId);
		insert->PutData("cur_num", record->curNum);
		insert->PutData("states", record->state);
		CLRecordClient::Instance()->SendCommand(insert);
	}
	else
	{
		std::ostringstream cond;
		cond << "acc_id=" << accid << " and activity_id=" << record->actId;
		CLUpdateCommand* update = CLRecordClient::Instance()->PrepareUpdateCommand("t_activity_account_record", cond.str());
		update->PutData("cur_num", record->curNum);
		update->PutData("states", record->state);
		CLRecordClient::Instance()->SendCommand(update);
	}
}

void ActivityMgr::DelExpireAccActivityRecord(UInt32 accid)
{
	auto iter = m_accActivityRecordMap.find(accid);
	if (iter == m_accActivityRecordMap.end())
		return;

	for (auto& it : iter->second)
	{
		CL_SAFE_DELETE(it.second);
		it.second = NULL;
	}

	iter->second.clear();
	m_accActivityRecordMap.erase(iter);

	std::ostringstream cond;
	cond << "acc_id=" << accid;
	CLDeleteCommand* del = CLRecordClient::Instance()->PrepareDeleteCommand("t_activity_account_record", cond.str());
	CLRecordClient::Instance()->SendCommand(del);
}
