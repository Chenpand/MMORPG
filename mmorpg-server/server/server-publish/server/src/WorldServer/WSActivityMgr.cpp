#include "WSActivityMgr.h"
#include "WSRouter.h"
#include "WSNetwork.h"
#include "ScriptMgr.h"
#include "TimerScript.h"
#include "GameParamMgr.h"
#include "WSSelectInitDataCallback.h"
#include "AccountActivityTaskMgr.h"
#include "WSSortListMgr.h"
#include "AnnouncementMgr.h"
#include "WSApplication.h"
#include "BlackMarketMgr.h"
#include "WSPlayerMgr.h"

#include <algorithm>
#include <TimeUtil.h>
#include <ActivityUtil.h>
#include <CLRecordClient.h>
#include <CLOpActiveProtocol.h>
#include <CLRoutineProtocol.h>
#include <CLSortListProtocol.h>
#include <CLGlobalActivityProtocol.h>
#include <CLActiveEventDataEntry.h>
#include <CLBattleProtocol.h>
#include <CLUnionServerProtocol.h>


WSActivityMgr::WSActivityMgr()
{
	m_GmDataLoadOver = false;
	m_TickTimer10s.SetInterval(10 * 1000);
}

WSActivityMgr::~WSActivityMgr()
{

}

bool WSActivityMgr::Init()
{
	LoadActivityTabTable();

	if (!LoadOpActivityData())
	{
		return false;
	}

	CLProtocol::GASActivityInfoReq gasActInfoReq;
	Avalon::NetConnectionPtr adminConn = WSNetwork::Instance()->FindConnectionByType(ST_ADMIN);
	if (adminConn == NULL) return false;

	gasActInfoReq.adminNodeId = adminConn->GetID();
	Router::SendToGlobalActivity(gasActInfoReq);

	// 初始化后也同步运营活动到各个服务器，避免早连造成活动没有同步
	_BrocastOpActivitiesToServers();

	return true;
}

void WSActivityMgr::OpenServerTimeActivity(UInt32 firstOpenServerTime)
{
	for (auto& it : m_OpenServerTimeRegInfos)
	{
		ActivityRegInfo& info = it.second;
		UInt32 tempTime = firstOpenServerTime + info.preTime;
		info.preTime = 0;

		if (tempTime >= info.startTime && tempTime < info.startTime + info.lastTime)
		{
			info.lastTime = info.lastTime - (tempTime - info.startTime);
			info.startTime = tempTime;
		}
		else if (tempTime >= info.startTime && tempTime > info.startTime + info.lastTime)
		{
			continue;
		}

		m_mapRegInfos.insert(std::make_pair(info.id, info));
	}

	m_OpenServerTimeRegInfos.clear();
}

void WSActivityMgr::LoadActivityTabTable()
{
	class ActivityTabTableVisitor : public Avalon::DataEntryVisitor<ActivityTabDataEntry>
	{
	public:
		bool operator()(ActivityTabDataEntry* data)
		{
			if (data != NULL)
			{
				ActivityTabInfo* tabInfo = new ActivityTabInfo();
				tabInfo->id = data->id;
				tabInfo->name = data->name;
				tabInfo->actIds = data->actIds;

				WSActivityMgr::Instance()->AddActivityTabInfo(tabInfo);
			}

			return true;
		}
	};

	ActivityTabTableVisitor visitor;
	ActivityTabDataEntryMgr::Instance()->Visit(visitor);
}

void WSActivityMgr::AddActivityTabInfo(ActivityTabInfo* tabInfo)
{
	m_ActivityTabsInfo.push_back(tabInfo);
}

void WSActivityMgr::VisitActivityTabInfo(const std::function<bool(ActivityTabInfo*)>& F)
{
	for (auto elem : m_ActivityTabsInfo)
	{
		if (!elem) continue;

		if (!F(elem)) return;
	}
}

void WSActivityMgr::ClearActivityTabInfo()
{
	for (auto elem : m_ActivityTabsInfo)
	{
		CL_SAFE_DELETE(elem);
	}

	m_ActivityTabsInfo.clear();
}

bool WSActivityMgr::AddActivityToTab(UInt32 actId, UInt32 tabId)
{
	bool setSuccess = false;

	for (auto elem : m_ActivityTabsInfo)
	{
		if (!elem) continue;

		if (elem->id != tabId) continue;

		elem->actIds.push_back(actId);

		setSuccess = true;
	}

	if (!setSuccess)
	{
		ErrorLogStream << "Add activity(" << actId << ") to tab(" << tabId << ") failed!" << LogStream::eos;
		return false;
	}

	return true;
}

bool WSActivityMgr::UpdateActivityTab()
{
	bool updateSuccess = true;

	ClearActivityTabInfo();

	LoadActivityTabTable();

	VisitOpActivity([&updateSuccess](const OpActivityRegInfo& opAct){
		if (opAct.tabId == 0) return true;

		updateSuccess = WSActivityMgr::Instance()->AddActivityToTab(opAct.dataId, opAct.tabId);

		if (!updateSuccess)
		{
			return false;
		}

		return true;
	});

	CLProtocol::WorldActivityTabsInfoSync syncActTab;

	VisitActivityTabInfo([&syncActTab](ActivityTabInfo* pTabInfo){
		if (!pTabInfo) return true;

		CLProtocol::ActivityTabInfo tabInfo;
		tabInfo.id = pTabInfo->id;
		tabInfo.name = pTabInfo->name;
		tabInfo.actIds = pTabInfo->actIds;
		syncActTab.tabsInfo.push_back(tabInfo);

		return true;
	});

	Router::BroadcastToPlayer(syncActTab);

	return updateSuccess;
}

void WSActivityMgr::ClearRegInfos()
{
	m_mapRegInfos.clear();
	m_mapGlobalActivity.clear();
}

void WSActivityMgr::RegisterDailyActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime)
{
	if(0 == lastTime)
		return;

	Avalon::Date today(CURRENT_TIME);

	std::istringstream is(startTime);
	Int32 nVal = 0;
	char split = 0;
	is >> nVal >> split;
	today.Hour(nVal);
	is >> nVal >> split;
	today.Min(nVal);
	is >> nVal;
	today.Sec(nVal);

	UInt32	uStartTime = UInt32(today.ToTime().Sec()) - preTime;
	if(CURRENT_TIME.Sec() >= (uStartTime + preTime + lastTime))
		uStartTime += DAY_TO_SEC;

	ActivityRegInfo	info(AT_DAILY, level, uStartTime, preTime, lastTime, 0);
	info.id = id;	
	info.name = name;

	DebugLogStream << "id = " << info.id << " name = " << info.name << 
		" pretime = " << info.preTime << " starttime = " << info.startTime << 
		" duetime = " << info.lastTime << LogStream::eos;

	m_mapRegInfos.insert(std::make_pair(id, info));
}

void WSActivityMgr::UpdateDailyActivity(UInt32 id, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime)
{
	if (0 == lastTime)
		return;

	auto iter = m_mapRegInfos.find(id);
	if (iter != m_mapRegInfos.end())
	{
		Avalon::Date today(CURRENT_TIME);

		std::istringstream is(startTime);
		Int32 nVal = 0;
		char split = 0;
		is >> nVal >> split;
		today.Hour(nVal);
		is >> nVal >> split;
		today.Min(nVal);
		is >> nVal;
		today.Sec(nVal);

		UInt32	uStartTime = UInt32(today.ToTime().Sec()) - preTime;
		if (CURRENT_TIME.Sec() >= (uStartTime + preTime + lastTime))
		{
			uStartTime += DAY_TO_SEC;
		}

		iter->second.level = level;
		iter->second.startTime = uStartTime;
		iter->second.preTime = preTime;
		iter->second.lastTime = lastTime;
	}
}

void WSActivityMgr::RegisterOnceActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime)
{
	if(0 == lastTime) return;

	Avalon::Date	date;

	std::istringstream	is(startTime);
	Int32	nVal = 0;
	char	split = 0;
	is >> nVal >> split;
	date.Year(nVal);
	is >> nVal >> split;
	date.Month(nVal);
	is >> nVal >> split;
	date.Day(nVal);
	is >> nVal >> split;
	date.Hour(nVal);
	is >> nVal >> split;
	date.Min(nVal);
	is >> nVal;
	date.Sec(nVal);

	UInt32	uStartTime = UInt32(date.ToTime().Sec()) - preTime;
	UInt32 dueTime = uStartTime + preTime + lastTime;

	for (auto& activity : m_Activities)
	{
		if (activity.id == id && activity.dueTime != dueTime)
		{
			activity.dueTime = dueTime;
			if (activity.name == "DungeonSY3")
			{
				_BrocastActivityStateChangeToPlayers(id, name, activity.state, level, preTime, activity.startTime, dueTime);

				if (!IsAccountActivity(id))
				{
					CLProtocol::WorldNotifySceneActivity notify2;
					notify2.id = id;
					notify2.name = name;
					notify2.type = activity.state;
					notify2.level = level;
					notify2.preTime = preTime;
					notify2.startTime = activity.startTime;
					notify2.dueTime = dueTime;
					notify2.tribeId = 0;
					Router::BroadcastToScene(notify2);
				}
			}
		}
	}

	ActivityRegInfo	info(AT_ONCE, level, uStartTime, preTime, lastTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(id, info));
}

void WSActivityMgr::RegisterOnceActivity(const std::string &name, UInt16 level, UInt32 startTime, UInt32 preTime, UInt32 lastTime)
{
 	ActivityRegInfo	info(AT_ONCE, level, startTime - preTime, preTime, lastTime, 0);
 	info.id = m_mapRegInfos.size() + 1;
 	info.name = name;
 	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::RegisterOpenServerOnceActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, 
	UInt32 lastTime, UInt32 openServerTime)
{
	if (0 == lastTime) return;

	Int32 nVal = 0;
	char split = 0;
	Avalon::Date date;
	std::istringstream is(startTime);
	is >> nVal >> split;
	date.Year(nVal);
	is >> nVal >> split;
	date.Month(nVal);
	is >> nVal >> split;
	date.Day(nVal);
	is >> nVal >> split;
	date.Hour(nVal);
	is >> nVal >> split;
	date.Min(nVal);
	is >> nVal;
	date.Sec(nVal);

	UInt32 uStartTime = UInt32(date.ToTime().Sec());
	if (GAME_FIRST_START_TIME == 0)
	{
		// 说明起服加载
		ActivityRegInfo	info(AT_ONCE, level, uStartTime, openServerTime, lastTime, 0);
		info.id = id;
		info.name = name;
		m_OpenServerTimeRegInfos.insert(std::make_pair(id, info));
		return;
	}

	//reload
	UInt32 realStartTime = GAME_FIRST_START_TIME + openServerTime;
	if (realStartTime >= uStartTime && realStartTime < uStartTime + lastTime)
	{
		lastTime = lastTime - (realStartTime - uStartTime);
		uStartTime = realStartTime;
	}
	else if (realStartTime >= uStartTime && realStartTime > uStartTime + lastTime)
	{
		// 该活动已结束
		uStartTime = (UInt32)CURRENT_TIME.Sec() - 10;
		lastTime = 1;
	}

	UInt32 dueTime = uStartTime + lastTime;
	for (auto& activity : m_Activities)
	{
		if (activity.id == id && activity.startTime != uStartTime)
		{
			activity.startTime = uStartTime;
		}

		if (activity.id == id && activity.dueTime != dueTime)
		{
			activity.dueTime = dueTime;
		}
	}

	ActivityRegInfo	info(AT_ONCE, level, uStartTime, 0, lastTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(id, info));
}

void WSActivityMgr::RegisterWeekActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime)
{
	if(0 == lastTime)
		return;
	
	std::istringstream	is(startTime);

	Int32 nWDay = 0;
	char split = 0;
	is >> nWDay >> split;

	Avalon::Date date(CURRENT_TIME);

	Int32 nVal = 0;
	is >> nVal >> split;
	date.Hour(nVal);
	is >> nVal >> split;
	date.Min(nVal);
	is >> nVal;
	date.Sec(nVal);

	UInt32 uStartTime = UInt32(date.ToTime().Sec()) - (date.WDay() - nWDay) * DAY_TO_SEC - preTime;
	if(CURRENT_TIME.Sec() >= (uStartTime + preTime + lastTime))
		uStartTime += 7 * DAY_TO_SEC;
	
	ActivityRegInfo	info(AT_WEEK, level, uStartTime, preTime, lastTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(id, info));
}

void WSActivityMgr::UpdateWeekActivity(UInt32 id, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime)
{
	if (0 == lastTime)
		return;

	auto iter = m_mapRegInfos.find(id);
	if (iter != m_mapRegInfos.end())
	{
		std::istringstream is(startTime);

		Int32 nWDay = 0;
		char split = 0;
		is >> nWDay >> split;

		Avalon::Date date(CURRENT_TIME);

		Int32	nVal = 0;
		is >> nVal >> split;
		date.Hour(nVal);
		is >> nVal >> split;
		date.Min(nVal);
		is >> nVal;
		date.Sec(nVal);

		UInt32	uStartTime = UInt32(date.ToTime().Sec()) - (date.WDay() - nWDay) * DAY_TO_SEC - preTime;
		if (CURRENT_TIME.Sec() >= (uStartTime + preTime + lastTime))
		{
			uStartTime += 7 * DAY_TO_SEC;
		}

		iter->second.level = level;
		iter->second.startTime = uStartTime;
		iter->second.preTime = preTime;
		iter->second.lastTime = lastTime;
	}
}

void WSActivityMgr::RegisterMonthActivity(UInt32 id, const std::string &name, UInt16 level)
{
	Avalon::Date startDate = CURRENT_DATE;
	startDate.Day(1);
	startDate.Hour(0);
	startDate.Min(0);
	startDate.Sec(0);

	UInt32 startTime = UInt32(startDate.ToTime().Sec());
	UInt32 lastTime = startDate.GetDaysInMonth() * Avalon::Time::SECS_OF_DAY;
	UInt32 endTime = startTime + lastTime;
	
	for (auto& activity : m_Activities)
	{
		if (activity.id == id)
		{
			activity.level = level;
			activity.startTime = startTime;
			activity.dueTime = endTime;
		}
	}

	ActivityRegInfo	info(AT_MONTH, level, startTime, 0, lastTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(id, info));

	InfoLogStream << "month activity(" << id << ") start(" << TimeUtil::TimestampToStandardFormat(startTime) << ") end(" << TimeUtil::TimestampToStandardFormat(endTime) << ") duration(" << lastTime << ")." << LogStream::eos;
}

void WSActivityMgr::RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, UInt32 lastTime)
{
	if(0 == lastTime)
		return;

	UInt32	uStartTime = GAMESTART_TIME;
	if(CURRENT_TIME.Sec() >= (uStartTime + lastTime))
		return;

	ActivityRegInfo	info(AT_ONCE, level, uStartTime, 0, lastTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, const std::string& endTime)
{
	std::istringstream is(endTime);
	char split = 0;
	UInt32 day = 0;
	is >> day >> split;

	UInt16 hour = 0;
	UInt16 min = 0;
	UInt16 sec = 0;
	is >> hour >> split >> min >> split >> sec;

	UInt32 gameStartTime = GAMESTART_TIME;
	UInt32 startTemp = UInt32(GetDayBeginTime(Avalon::Time(UInt64(GAMESTART_TIME) * 1000)) / 1000);
	UInt32 toTime = startTemp + (day - 1) * DAY_TO_SEC + hour * HOUR_TO_SEC + min * MIN_TO_SEC + sec;

	if(toTime <= UInt32(CURRENT_TIME.Sec())) return;

	ActivityRegInfo info(AT_ONCE, level, gameStartTime, 0, toTime - gameStartTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, const std::string& fromTime, const std::string& toTime)
{
	char split = 0;

	UInt16 fromDay = 0;
	UInt16 fromHour = 0;
	UInt16 fromMin = 0;
	UInt16 fromSec = 0;
	std::istringstream fromIs(fromTime);
	fromIs >> fromDay >> split >> fromHour >> split >> fromMin >> split >> fromSec;

	UInt16 toDay = 0;
	UInt16 toHour = 0;
	UInt16 toMin = 0;
	UInt16 toSec = 0;
	std::istringstream toIs(toTime);
	toIs >> toDay >> split >> toHour >> split >> toMin >> split >> toSec;

	if (fromDay == 0 || toDay == 0 || toDay < fromDay)
	{
		return;
	}

	if (fromHour >= 24 || toHour >= 24)
	{
		return;
	}

	if (fromMin >= 60 || toMin >= 60)
	{
		return;
	}

	if (fromSec >= 60 || toSec >= 60)
	{
		return;
	}

	UInt32 startTemp = UInt32(GetDayBeginTime(Avalon::Time(UInt64(GAMESTART_TIME) * 1000)) / 1000);
 	UInt32 startTime = startTemp + (fromDay - 1) * DAY_TO_SEC + fromHour * HOUR_TO_SEC + fromMin * MIN_TO_SEC + fromSec;
	UInt32 endTime = startTemp + (toDay - 1) * DAY_TO_SEC + toHour * HOUR_TO_SEC + toMin * MIN_TO_SEC + toSec;

	ActivityRegInfo info(AT_ONCE, level, startTime, 0, endTime - startTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, UInt16 fromDay, UInt16 toDay)
{
	UInt32 startTime = GAMESTART_TIME + (fromDay - 1) * DAY_TO_SEC;
	UInt32 endTime = GAMESTART_TIME + (toDay - 1) * DAY_TO_SEC;

	ActivityRegInfo info(AT_ONCE, level, startTime, 0, endTime - startTime, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::RegisterConsistActivity(const std::string &name, UInt16 level)
{
	ActivityRegInfo	info(AT_ONCE, level, 0, 0, 0xFFFFFFFF, 0);
	info.id = m_mapRegInfos.size() + 1;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::RegisterConsistActivity(UInt32 id, const std::string &name, UInt16 level)
{
	ActivityRegInfo	info(AT_ONCE, level, 0, 0, 0xFFFFFFFF, 0);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(info.id, info));
}

void WSActivityMgr::SetGameStartLimit(UInt32 id, const std::string &gamestartTime, UInt8 gamestartFlag)
{
	Avalon::Date date;

	std::istringstream is(gamestartTime);
	Int32 nVal = 0;
	char split = 0;
	is >> nVal >> split;
	date.Year(nVal);
	is >> nVal >> split;
	date.Month(nVal);
	is >> nVal;
	date.Day(nVal);

	date.Hour(0);
	date.Min(0);
	date.Sec(0);

	UInt32	uSecTime = UInt32(date.ToTime().Sec());

	auto iter = m_mapRegInfos.find(id);
	if (iter != m_mapRegInfos.end())
	{
		iter->second.gamestartTime = uSecTime;
		iter->second.gamestartFlag = gamestartFlag;
	}
}

void WSActivityMgr::NotifyActivityStateChange(WSPlayer* player, Int32 id, const std::string& name, UInt16 level, UInt8 state, UInt32 preTime, UInt32 startTime, UInt32 dueTime)
{
	CLProtocol::WorldNotifyClientActivity notify1;
	notify1.id = id;
	notify1.name = name;
	notify1.type = state;
	notify1.level = level;
	notify1.preTime = preTime;
	notify1.startTime = startTime;
	notify1.dueTime = dueTime;
	player->SendProtocol(notify1);

	InfoLogStream << PLAYERINFO(player) << " WorldNotifyClientActivity: notify activity(" << id << ") state(" << (UInt32)state << ") change." << LogStream::eos;
}

void WSActivityMgr::VisitActivity(const std::function<bool(const ActivityInfo&)>& F)
{
	for (auto elem : m_Activities)
	{
		if (!F(elem)) break;
	}
}

bool WSActivityMgr::FindActivity(UInt32 actId, ActivityInfo& actInfo)
{
	for (const auto& elem : m_Activities)
	{
		if (elem.id == actId)
		{
			actInfo = elem;
			return true;
		}
	}

	return false;
}

bool WSActivityMgr::OpenActivity(UInt32 id, const std::string& name, UInt16 level, UInt8 state, UInt32 preTime, UInt32 startTime, UInt32 dueTime)
{
	DebugLogStream << "id = " << id << " name = " << name << 
		" pretime = " << preTime << " starttime = " << startTime << 
		" duetime = " << dueTime << " state = " << (UInt32)state <<  LogStream::eos;

	if(dueTime <= CURRENT_TIME.Sec()) return false;

	for(ActivityVec::iterator iter = m_Activities.begin()
		;iter != m_Activities.end(); ++iter)
	{
		if(iter->name == name) return false;
	}

	_BrocastActivityStateChangeToPlayers(id, name, state, level, preTime, startTime, dueTime);

	if (!IsAccountActivity(id))
	{
		CLProtocol::WorldNotifySceneActivity notify2;
		notify2.id = id;
		notify2.name = name;
		notify2.type = state;
		notify2.level = level;
		notify2.preTime = preTime;
		notify2.startTime = startTime;
		notify2.dueTime = dueTime;
		notify2.tribeId = 0;
		Router::BroadcastToScene(notify2);
	}

	ActivityInfo info(id, name, level, preTime, startTime, dueTime);
	info.state = state;

	DebugLogStream << "id = " << info.id << " name = " << info.name << 
		" pretime = " << info.preTime << " starttime = " << info.startTime <<
		" duetime = " << info.dueTime << " state = " << (UInt32)state << LogStream::eos;

	m_Activities.push_back(info);

	AccountActivityTaskMgr::Instance()->OnActivityOpen(info);
	BlackMarketMgr::Instance()->OnActOpen(info);

	return true;
}

void WSActivityMgr::CloseActivity(const std::string& name)
{
	ActivityVec::iterator iter = m_Activities.begin();
	for(; iter != m_Activities.end(); ++iter)
	{
		if (iter->name == name)
		{
			break;
		}
	}

	if (iter == m_Activities.end())
	{
		return;
	}

	NotifyActivityStateChange(*iter, AS_END);

	m_Activities.erase(iter);
}

bool WSActivityMgr::IsInActivity(const std::string& name) const
{
	ActivityVec::const_iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (iter->name == name && AS_IN == iter->state)
		{
			return true;
		}
	}

	return false;
}

bool WSActivityMgr::IsInActivity(WSPlayer* player, const std::string& name) const
{
	ActivityVec::const_iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (player->GetSrcZoneId() == 0 && iter->name == name && AS_IN == iter->state)
		{
			return true;
		}
	}

	return false;
}

bool WSActivityMgr::IsPreActivity(const std::string& name) const
{
	ActivityVec::const_iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (iter->name == name && AS_PREPARE == iter->state)
		{
			return true;
		}
	}

	return false;
}

bool WSActivityMgr::IsPreActivity(WSPlayer* player, const std::string& name) const
{
	ActivityVec::const_iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (player->GetSrcZoneId() == 0 && iter->name == name && AS_PREPARE == iter->state)
		{
			return true;
		}
	}

	return false;
}

bool WSActivityMgr::IsEndActivity(const std::string& name) const
{
	ActivityRegMap::const_iterator iter = m_mapRegInfos.begin();
	while (iter != m_mapRegInfos.end())
	{
		if (iter->second.name == name)
		{
			Avalon::Time now = CURRENT_TIME;
			if (now.Sec() < iter->second.startTime || now.Sec() > (iter->second.startTime + iter->second.preTime + iter->second.lastTime))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		++iter;
	}

	return true;
}

void WSActivityMgr::NotifyActivityStateChange(ActivityInfo &activity, UInt8 state)
{
	UInt16	level = 0;

	if(0 != activity.id && AS_IN == state)
	{
		auto iter = m_mapRegInfos.find(activity.id);
		if (iter != m_mapRegInfos.end())
		{
			if (iter->second.id == activity.id)
			{
				level = iter->second.level;
			}
		}
	}

	DebugLogStream << "WorldNotifyClientActivity: Activity(" << activity.id << ") is change state(" << state <<
		"), (startT=" << TimeUtil::TimestampToStandardFormat(activity.startTime) << 
		", preT=" << TimeUtil::TimestampToStandardFormat(activity.preTime) << 
		", endT=" << TimeUtil::TimestampToStandardFormat(activity.dueTime) << 
		")." << LogStream::eos;

	CLProtocol::WorldNotifyClientActivity notify1;
	notify1.id = activity.id;
	notify1.name = activity.name;
	notify1.type = state;
	notify1.level = level;
	notify1.startTime = UInt32(CURRENT_TIME.Sec());
	notify1.dueTime = activity.dueTime;
	Router::BroadcastToPlayer(notify1);

	if (!IsAccountActivity(activity.id))
	{
		CLProtocol::WorldNotifySceneActivity notify2;
		notify2.id = activity.id;
		notify2.name = activity.name;
		notify2.type = state;
		notify2.level = level;
		notify2.startTime = UInt32(CURRENT_TIME.Sec());
		notify2.dueTime = activity.dueTime;
		notify2.tribeId = 0;
		Router::BroadcastToScene(notify2);
	}
}

void WSActivityMgr::OnTick(const Avalon::Time& now)
{
	//开始的活动
	std::vector<std::string> actstart;

	//活动到时判断 放在开始判断前面 因为有开始下一轮任务 和 结束上一轮任务是在同一时间的 防止无法开始下一轮
	std::vector<std::string> activities;

	//准备中的活动
	std::vector<std::string> preActs;

	for (ActivityVec::iterator iter = m_Activities.begin();
		iter != m_Activities.end();)
	{
		if (AS_PREPARE == iter->state &&
			now.Sec() >= iter->startTime)
		{
			iter->state = AS_IN;
			NotifyActivityStateChange(*iter, AS_IN);
			actstart.push_back(iter->name);

			AccountActivityTaskMgr::Instance()->OnActivityOpen(*iter);
		}
		else if (AS_IN == iter->state &&
			now.Sec() >= iter->dueTime)
		{
			NotifyActivityStateChange(*iter, AS_END);
			OnActivityEnd(*iter);
			activities.push_back(iter->name);
			iter = m_Activities.erase(iter);
			continue;
		}

		++iter;
	}

	//判断开始活动
	for (auto iter = m_mapRegInfos.begin();
		iter != m_mapRegInfos.end(); iter++)
	{
		if (iter->second.gamestartTime != 0)
		{
			if (0 == iter->second.gamestartFlag)
			{
				// 新服无活动
				if (GAMESTART_TIME > iter->second.gamestartTime)
					continue;
			}
			else
			{
				// 老服无活动
				if (GAMESTART_TIME < iter->second.gamestartTime)
					continue;
			}
		}

		if (now.Sec() >= iter->second.startTime)
		{
			if (now.Sec() < (iter->second.startTime + iter->second.preTime))
			{
				if (OpenActivity(iter->second.id, iter->second.name, iter->second.level, AS_PREPARE, iter->second.startTime, iter->second.startTime + iter->second.preTime, iter->second.startTime + iter->second.preTime + iter->second.lastTime))
				{
					preActs.push_back(iter->second.name);
				}
			}
			else if (now.Sec() < (iter->second.startTime + iter->second.preTime + iter->second.lastTime))
			{
				if (OpenActivity(iter->second.id, iter->second.name, iter->second.level, AS_IN, iter->second.startTime, iter->second.startTime + iter->second.preTime, iter->second.startTime + iter->second.preTime + iter->second.lastTime))
				{
					actstart.push_back(iter->second.name);
				}
			}

			if (AT_DAILY == iter->second.type)
			{
				iter->second.startTime += DAY_TO_SEC;
			}
			else if (AT_ONCE == iter->second.type)
			{
				iter->second.startTime = 0xFFFFFFFF;
			}
			else if (AT_WEEK == iter->second.type)
			{
				iter->second.startTime += 7 * DAY_TO_SEC;
			}
			else if (AT_CIRCLE == iter->second.type)
			{
				//iter->second.startTime += iter->second.lastTime;
				iter->second.startTime += iter->second.cricleDays * DAY_TO_SEC;
			}
			else if (AT_MONTH == iter->second.type)
			{
				Avalon::Date startDate = CURRENT_DATE;
				startDate.Day(1);
				startDate.Hour(0);
				startDate.Min(0);
				startDate.Sec(0);

				Avalon::Date endDate = startDate;
				if (endDate.Month() == 12)
				{
					endDate.Year(endDate.Year() + 1);
					endDate.Month(1);
				}
				else
				{
					endDate.Month(endDate.Month() + 1);
				}

				iter->second.startTime = UInt32(endDate.ToTime().Sec());
				iter->second.lastTime = endDate.GetDaysInMonth() * Avalon::Time::SECS_OF_DAY;

				UInt32 endTime = iter->second.startTime + iter->second.lastTime;

				InfoLogStream << "month activity(" << iter->first << 
					") next start time(" << TimeUtil::TimestampToStandardFormat(iter->second.startTime) << 
					") end(" << TimeUtil::TimestampToStandardFormat(endTime) << 
					")." << LogStream::eos;
			}
		}
	}


	for (std::vector<std::string>::iterator iter = actstart.begin();
		iter != actstart.end(); ++iter)
	{
		OnActivityStart(*iter);
	}

	for (std::vector<std::string>::iterator iter = activities.begin();
		iter != activities.end(); ++iter)
	{
		OnActivityEnd(*iter);
	}

	for (std::vector<std::string>::iterator iter = preActs.begin();
		iter != preActs.end(); ++iter)
	{
		OnActivityPrepare(*iter);
	}

	OnTickOpAct(now);

	if (m_TickTimer10s.IsTimeout(now))
	{
		// BattleServer 活动
		_NotifyBattleActivityTime("chiji");
		_NotifyBattleActivityTime("lostDungeon");

		// UnionServer活动
		//_NotifyUnionActivityTime(USFT_GOLD_CONSIGNMENT, "GoldConsignment");
	}
}

void WSActivityMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player) return;

	CLProtocol::WorldActivityTabsInfoSync syncActTab;
	std::map<UInt32, ActivityTabInfo*> tempActMap;
	VisitActivityTabInfo([&tempActMap](ActivityTabInfo* pTabInfo){
		if (!pTabInfo) return true;

		tempActMap.insert(std::make_pair(pTabInfo->id, pTabInfo));
		return true;
	});

	for (auto it : tempActMap)
	{
		CLProtocol::ActivityTabInfo tabInfo;
		tabInfo.id = it.second->id;
		tabInfo.name = it.second->name;
		tabInfo.actIds = it.second->actIds;
		syncActTab.tabsInfo.push_back(tabInfo);
	}

	player->SendProtocol(syncActTab);

	InfoLogStream << "player(" << player->GetID() << ") send activity tab info." << LogStream::eos;
}

void WSActivityMgr::OnSceneConnected(UInt32 nodeId)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(nodeId);
	if(conn == NULL) 
		return;

	CLProtocol::WorldSyncSceneActivities sync;
	GetRoleActivity(sync.activities);
	CL_SEND_PROTOCOL(conn, sync);

	_SendOpActivitiesToServer(conn);
}

void WSActivityMgr::OnGateConnected(UInt32 nodeId)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(nodeId);
	if (conn == NULL) return;

	_SendOpActivitiesToServer(conn);
}

void WSActivityMgr::OnActivityStart(const std::string &name)
{
	DebugLogStream << "activity(" << name << ") started." << LogStream::eos;
	ScriptMgr::Instance()->GetTimerScript()->OnActivityStart(name.c_str());
}

void WSActivityMgr::OnActivityEnd(ActivityInfo &activity)
{
	BlackMarketMgr::Instance()->OnActClose(activity);
}

void WSActivityMgr::OnActivityEnd(const std::string& name)
{
	ScriptMgr::Instance()->GetTimerScript()->OnActivityEnd(name.c_str());
}

void WSActivityMgr::OnActivityPrepare(const std::string& name)
{
	ScriptMgr::Instance()->GetTimerScript()->OnActivityPrepare(name.c_str());
}

UInt32 WSActivityMgr::GetActivityPreTime(const std::string& name) const
{
	ActivityVec::const_iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (iter->name == name && (AS_IN == iter->state || AS_PREPARE == iter->state))
		{
			return iter->preTime;
		}
	}

	return 0;
}

UInt32 WSActivityMgr::GetActivityStartTime(const std::string& name) const
{
	ActivityVec::const_iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (iter->name == name && (AS_IN == iter->state || AS_PREPARE == iter->state))
		{
			return iter->startTime;
		}
	}

	return 0;
}

UInt32 WSActivityMgr::GetActivityDueTime( const std::string& name ) const
{
	for(ActivityVec::const_iterator iter = m_Activities.begin();
		iter != m_Activities.end(); ++iter)
	{
		if (iter->name == name && (AS_IN == iter->state || AS_PREPARE == iter->state))
		{
			return iter->dueTime;
		}
	}

	return 0;
}

void WSActivityMgr::AddGlobalActivity(const char *szName)
{
	m_mapGlobalActivity.insert(std::pair<std::string, std::string>(szName, szName));
}

void WSActivityMgr::RegisterCustomCircleActivity(UInt32 id, const std::string &name, UInt16 level, const std::string startTime, UInt32 preTime, const UInt32 lastTime)
{
	if (0 == lastTime) return;
	if ("" == startTime) return;

	UInt32	uStartTime = 0;
	UInt32 cricleDays = 0;
	Avalon::Date	date;

	std::istringstream	is(startTime);

	Int32 nVal = 0;
	char split = 0;
	
	is >> cricleDays >> split;

	is >> nVal >> split;
	date.Year(nVal);
	is >> nVal >> split;
	date.Month(nVal);
	is >> nVal >> split;
	date.Day(nVal);
	is >> nVal >> split;
	date.Hour(nVal);
	is >> nVal >> split;
	date.Min(nVal);
	is >> nVal;
	date.Sec(nVal);
	
	UInt32 curTime = CURRENT_TIME.Sec();
	uStartTime = UInt32(date.ToTime().Sec());
	if (curTime >= (uStartTime + preTime + lastTime))
	{
		UInt32 span = (curTime - uStartTime) / DAY_TO_SEC / cricleDays;
		uStartTime += span * cricleDays * DAY_TO_SEC;
		if (curTime > uStartTime + preTime + lastTime)
		{
			uStartTime += cricleDays * DAY_TO_SEC;
		}
	}

	ActivityRegInfo	info(AT_CIRCLE, level, uStartTime, preTime, lastTime, cricleDays);
	info.id = id;
	info.name = name;
	m_mapRegInfos.insert(std::make_pair(id, info));
}

void WSActivityMgr::OnDayChanged(WSPlayer* player)
{
	UInt32 day = player->GetCreateFromNowOnDay();

	switch (day)
	{
	case 2:	NotifyActivityStateChange(player, 7200, "SevenDayTwo", 1, AS_IN, 0, 0, 0); break;
	case 3:	NotifyActivityStateChange(player, 7300, "SevenDayThree", 1, AS_IN, 0, 0, 0); break;
	case 4:	NotifyActivityStateChange(player, 7400, "SevenDayFour", 1, AS_IN, 0, 0, 0); break;
	case 5:	NotifyActivityStateChange(player, 7500, "SevenDayFive", 1, AS_IN, 0, 0, 0); break;
	case 6:	NotifyActivityStateChange(player, 7600, "SevenDaySix", 1, AS_IN, 0, 0, 0); break;
	case 7:	NotifyActivityStateChange(player, 7700, "SevenDaySeven", 1, AS_IN, 0, 0, 0); break;
	default:break;
	}

	if (day > SEVEN_CLOSE_DAY)
	{
		NotifyActivityStateChange(player, 7100, "SevenDayOne", 1, AS_END, 0, 0, 0);
		NotifyActivityStateChange(player, 7200, "SevenDayTwo", 1, AS_END, 0, 0, 0);
		NotifyActivityStateChange(player, 7300, "SevenDayThree", 1, AS_END, 0, 0, 0);
		NotifyActivityStateChange(player, 7400, "SevenDayFour", 1, AS_END, 0, 0, 0);
		NotifyActivityStateChange(player, 7500, "SevenDayFive", 1, AS_END, 0, 0, 0);
		NotifyActivityStateChange(player, 7600, "SevenDaySix", 1, AS_END, 0, 0, 0);
		NotifyActivityStateChange(player, 7700, "SevenDaySeven", 1, AS_END, 0, 0, 0);
	}
}

void WSActivityMgr::GetOpenActivity(WSPlayer* player, std::vector<ActivityInfo>& infos)
{
	if (!player)
		return;

	UInt32 day = player->GetCreateFromNowOnDay();
	UInt32 dueDay = SEVEN_CLOSE_DAY;
	for (ActivityVec::iterator iter = m_Activities.begin(); iter != m_Activities.end(); iter++)
	{
		bool unStart = false;
		switch (iter->id)
		{
		case 7100:
		{
			if (day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		case 7200:
		{
			if (day < 2 || day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		case 7300:
		{
			if (day < 3 || day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		case 7400:
		{
			if (day < 4 || day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		case 7500:
		{
			if (day < 5 || day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		case 7600:
		{
			if (day < 6 || day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		case 7700:
		{
			if (day < 7 || day > dueDay)
			{
				unStart = true;
			}
			break;
		}
		default:break;
		}

		if (unStart)
		{
			continue;
		}

		infos.push_back(*iter);
	}
}

void WSActivityMgr::GmOpenActive(UInt32 id, UInt8 state)
{
	auto itr = m_mapRegInfos.find(id);
	if (itr == m_mapRegInfos.end())
	{
		return;
	}

	ActivityRegInfo& info = itr->second;
	if (state == AS_PREPARE)
	{
		if (OpenActivity(info.id, info.name, info.level, AS_PREPARE, info.startTime, info.startTime + info.preTime, info.startTime + info.preTime + info.lastTime))
		{
			OnActivityPrepare(info.name);
		}
	}
	else if (state == AS_IN)
	{
		for (ActivityVec::iterator iter = m_Activities.begin();
			iter != m_Activities.end(); ++iter)
		{
			if (iter->id == id &&
				AS_PREPARE == iter->state)
			{
				iter->state = AS_IN;
				NotifyActivityStateChange(*iter, AS_IN);
				break;
			}
		}

		if (OpenActivity(info.id, info.name, info.level, AS_IN, info.startTime, info.startTime + info.preTime, info.startTime + info.preTime + info.lastTime))
		{
			OnActivityStart(info.name);
		}
	}
	else if (state == AS_END)
	{
		for (ActivityVec::iterator iter = m_Activities.begin();
			iter != m_Activities.end(); ++iter)
		{
			if (iter->id == id && AS_IN == iter->state)
			{
				iter->state = AS_END;
				NotifyActivityStateChange(*iter, AS_END);
				m_Activities.erase(iter);

				OnActivityEnd(info.name);
				return;
			}
		}
	}
}

class SelectOpActivityDataCb : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		while (NextRow())
		{
			WSActivityMgr::Instance()->CreateOpActivity(this);
		}

		return true;
	}
};

bool WSActivityMgr::LoadOpActivityData()
{
	if (!LoadOpActivityFromDataTable())
	{
		return false;
	}

	CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_op");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectOpActivityDataCb());

	CLProtocol::WorldGmOpActivtyReq protocol;
	protocol.mergeServers = MERGED_SERVERS;
	Router::SendToCenter(protocol);

	return true;
}

bool WSActivityMgr::LoadOpActivityFromDataTable(bool isReload)
{
	class OpActivityDataVisitor : public Avalon::DataEntryVisitor<OpActivityDataEntry>
	{
	public:
		bool operator()(OpActivityDataEntry* data)
		{
			if (!data) return true;

			activityDatas.push_back(data);
			return true;
		}

		std::vector<OpActivityDataEntry*> activityDatas;
	};

	OpActivityDataVisitor visitor;
	OpActivityDataEntryMgr::Instance()->Visit(visitor);

	for (auto data : visitor.activityDatas)
	{
		if (!data) continue;

		switch ((OpActivityType)data->tmpType)
		{
		case OAT_GAMBING:
		case OAT_BET_HORSE:
		case OAT_JAR_DRAW_LOTTERY:
		case OAT_ARTIFACT_JAR_SHOW:
		case OAT_WEEK_SIGN_ACTIVITY:
		case OAT_WHOLE_BARGAIN_DISCOUNT:
			continue;

		default:
			break;
		}

		// 超级新服活动规则
		if (data->superNewServiceActFlag > 0)
		{
			if (!WSApplication::Instance()->IsSuperNewService())
			{
				// 不是超级新服
				continue;
			}

			std::vector<UInt32> superNewServiceOpActs = WSApplication::Instance()->GetSuperNewServiceOpActs();

			if (superNewServiceOpActs.empty())
			{
				// 没有配具体要显示的超级新服活动
				continue;
			}

			if (std::find(superNewServiceOpActs.begin(), superNewServiceOpActs.end(), data->id) == superNewServiceOpActs.end())
			{
				// 配置里没有这个活动
				continue;
			}
		}

		OpActivityRegInfo info;
		if (!ActivityUtil::InitOpActivityInfo(info, data))
		{
			continue;
		}

		if (isReload)
		{
			m_TmpOpRegInfos.insert(std::make_pair(info.dataId, info));
		}
		else
		{
			auto iter = m_opRegInfos.find(info.dataId);
			if (iter == m_opRegInfos.end())
			{
				m_opRegInfos[info.dataId] = info;
			}
			else
			{
				iter->second.copy(info);
			}
		}

		InfoLogStream << "Load OpActivity table success, info(id=" << info.dataId << ", state=" << info.state
			<< ", st=" << TimeUtil::TimestampToStandardFormat(info.startTime)
			<< ", et=" << TimeUtil::TimestampToStandardFormat(info.endTime)
			<< ", circly type=" << info.circleType << ")." << LogStream::eos;
	}

	return true;
}

void WSActivityMgr::ReloadOpActivityFromDataTable()
{
	InfoLogStream << "Start reloading..." << LogStream::eos;

	LoadOpActivityFromDataTable(true);

	for (auto& elem : m_opRegInfos)
	{
		if (elem.second.dataSource != OADS_TABLE) continue;

		auto iter = m_TmpOpRegInfos.find(elem.first);
		if (iter != m_TmpOpRegInfos.end())
		{
			if (elem.second != iter->second)
			{
				elem.second = iter->second;
				SyncOpActStateChange(elem.second);
			}

			m_TmpOpRegInfos.erase(iter);
		}
		else
		{
			// 现有内存中的数据在重载后的表格中找不到，则将之关闭
			elem.second.endTime = elem.second.startTime + 1;
			InfoLogStream << "OpActivity(" << elem.second.dataId << ") will close!" << LogStream::eos;
		}
	}

	// 说明有新加的活动
	if (!m_TmpOpRegInfos.empty())
	{
		InfoLogStream << "Add the new OpActivities!" << LogStream::eos;

		m_opRegInfos.insert(m_TmpOpRegInfos.begin(), m_TmpOpRegInfos.end());

		CLProtocol::WSSyncOpActivityData syncOpActData;
		syncOpActData.datas = m_TmpOpRegInfos;
		Router::BroadcastToScene(syncOpActData);
		Router::BroadcastToGate(syncOpActData);
	}

	m_TmpOpRegInfos.clear();
}

void WSActivityMgr::CreateOpActivity(CLRecordCallback* cb)
{
	if (!cb)
	{
		return;
	}

	OpActivityRegInfo info;
	info.dataId = (UInt32)cb->GetKey();
	info.tmpType = cb->GetData("tmptype");
	info.name = cb->GetData("name").GetString();
	info.tag = cb->GetData("tag");
	info.tagEndTime = cb->GetData("tagendtime");
	info.prepareTime = cb->GetData("preparetime");
	info.startTime = cb->GetData("starttime");
	info.endTime = cb->GetData("endtime");
	info.desc = cb->GetData("desc").GetString();
	info.ruleDesc = cb->GetData("ruledesc").GetString();
	info.circleType = cb->GetData("circletype");
	std::string tasksStr = cb->GetData("tasks").GetString();
	info.taskDesc = cb->GetData("taskdesc").GetString();
	std::string taskRewardStr = cb->GetData("taskrewards").GetString();
	info.parm = cb->GetData("parm");
	info.logoDesc = cb->GetData("logdesc").GetString();
	OpTaskDecode(info.tasks, tasksStr, taskRewardStr);

	auto iter = m_opRegInfos.find(info.dataId);
	if (iter == m_opRegInfos.end())
	{
		m_opRegInfos[info.dataId] = info;
	}
	else
	{
		ErrorLogStream << "Repeat activity(" << info.dataId << ", " << info.name << ")!" << LogStream::eos;
	}
}

void WSActivityMgr::AddOpActivity(OpActivityRegInfo& opAct)
{
	auto iter = m_opRegInfos.find(opAct.dataId);
	if (iter == m_opRegInfos.end())
	{
		m_opRegInfos[opAct.dataId] = opAct;
	}
	else
	{
		iter->second = opAct;
	}
}

void WSActivityMgr::UpdateOpActData(OpActivityRegInfo& info)
{
	auto itr = m_opRegInfos.find(info.dataId);
	if (itr != m_opRegInfos.end())
	{
		m_opRegInfos[info.dataId] = info;
		SyncOpActStateChange(info);

		InfoLogStream << "Update " << info.LogStr() << 
			", preT=" << TimeUtil::TimestampToStandardFormat(info.prepareTime) << 
			", st=" << TimeUtil::TimestampToStandardFormat(info.startTime) <<
			", et=" << TimeUtil::TimestampToStandardFormat(info.endTime) << 
			")." << LogStream::eos;
	}
	else
	{
		if (info.endTime < CURRENT_TIME.Sec())
		{
			return;
		}

		m_opRegInfos[info.dataId] = info;

		// InsertDB(info);

		CLProtocol::WSSyncOpActivityData syncOpActData;
		syncOpActData.datas[info.dataId] = info;
		Router::BroadcastToScene(syncOpActData);

		InfoLogStream << "Add activity info(id=" << info.dataId << ", state=" << info.state 
			<< ", preT=" << TimeUtil::TimestampToStandardFormat(info.prepareTime)
			<< ", st=" << TimeUtil::TimestampToStandardFormat(info.startTime)
			<< ", et=" << TimeUtil::TimestampToStandardFormat(info.endTime)
			<< ")." << LogStream::eos;
	}
}

bool WSActivityMgr::DeleteOpActData(UInt32 dataId)
{
	auto itr = m_opRegInfos.find(dataId);
	if (itr != m_opRegInfos.end())
	{
		itr->second.endTime = itr->second.startTime + 1;

		InfoLogStream << "Delete opActivity(" << dataId << ")!" << LogStream::eos;
	}
	else
	{
		return false;
	}

	return true;
}

void WSActivityMgr::VisitOpActivity(const std::function<bool(const OpActivityRegInfo&)>& F)
{
	for (auto& elem : m_opRegInfos)
	{
		if (!F(elem.second))
		{
			break;
		}
	}
}

void WSActivityMgr::OnCheckGmOpActLoadOver(UInt32 totalNum)
{
	UInt32 actCnt = 0;
	for (auto elem : m_opRegInfos)
	{
		const OpActivityRegInfo& opAct = elem.second;
		if (opAct.dataSource == OADS_FROM_GM)
		{
			actCnt++;
		}
	}

	InfoLogStream << "now opActivity num in server is " << actCnt << ", gm's is " << totalNum << LogStream::eos;

	if (totalNum == actCnt)
	{
		m_GmDataLoadOver = true;
	}
}

void WSActivityMgr::CustomSetOpActData(UInt32 id, UInt32 startTime, UInt32 endTime)
{
	auto itr = m_opRegInfos.find(id);
	if (itr != m_opRegInfos.end())
	{
		OpActivityRegInfo& newInfo = itr->second;
		newInfo.startTime = startTime;
		newInfo.endTime = endTime;

		InfoLogStream << "setOpAct(" << id << ") st(" << TimeUtil::TimestampToStandardFormat(startTime) << "), et(" << TimeUtil::TimestampToStandardFormat(endTime) << ")." << LogStream::eos;
	}
	else
	{
		ErrorLogStream << "Can not find activity(" << id << ")!" << LogStream::eos;
	}
}

void WSActivityMgr::QueryOpActData(std::map<UInt32,OpActivityRegInfo>& datas)
{
	datas = m_opRegInfos;
}

bool WSActivityMgr::OpTaskDecode(std::vector<OpActTaskData>& tasks, const std::string& taskStr, const std::string& rewardStr)
{
	std::vector<std::string> taskStrVec;
	Avalon::StringUtil::Split(taskStr, taskStrVec, ";");

	std::vector<std::string> rewardStrVec;
	Avalon::StringUtil::Split(rewardStr, rewardStrVec, ";");

	if (taskStrVec.size() != rewardStrVec.size())
	{
		return false;
	}

	for (size_t i = 0; i < taskStrVec.size(); ++i)
	{
		OpActTaskData task;
		std::istringstream	taskis(taskStrVec[i].c_str());

		task.DecodeString(taskis);
		
		std::vector<std::string> rewardStrs;
		Avalon::StringUtil::Split(rewardStrVec[i], rewardStrs, "|");

		for (size_t j = 0; j < rewardStrs.size(); ++j)
		{
			ItemReward reward;
			std::istringstream	rewardstr(rewardStrs[j].c_str());
			reward.DecodeString(rewardstr);
			task.rewards.push_back(reward);
		}
		
		tasks.push_back(task);
	}

	return true;
}

bool WSActivityMgr::OpTaskEncode(const std::vector<OpActTaskData>& tasks, std::string& taskStr, std::string& rewardStr)
{
	std::ostringstream taskos;
	std::ostringstream rewardos;

	for (size_t i = 0; i < tasks.size(); ++i)
	{
		if (i > 0)
		{
			taskos << ";";
			rewardos << ";";
		}

		tasks[i].EncodeString(taskos);

		for (size_t j = 0; j < tasks[i].rewards.size(); ++j)
		{
			if (j > 0)
			{
				rewardos << "|";
			}
			tasks[i].rewards[j].EncodeString(rewardos);
		}
	}

	taskStr = taskos.str();
	rewardStr = rewardos.str();

	return true;
}

bool WSActivityMgr::GMDecodeRewardItems(std::vector<ItemReward>& items, std::string strItems)
{
	if (strItems.empty())
	{
		return true;
	}

	std::vector<std::string> rewardsStr;
	std::vector<std::string> paramStr;
	Avalon::StringUtil::Split(strItems, rewardsStr, "|");
	for (std::vector<std::string>::iterator iter = rewardsStr.begin();
		iter != rewardsStr.end(); ++iter)
	{
		Avalon::StringUtil::Split(*iter, paramStr, ":");
		if (paramStr.size() != 2)
			return false;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());

		items.push_back(reward);
	}

	return true;
}

std::string WSActivityMgr::GMEncodRewardItems(std::vector<ItemReward>& items)
{
	std::ostringstream os;

	for (size_t i = 0; i < items.size(); ++i)
	{
		if (i > 0)
		{
			os << "|";
		}

		os << items[i].id << ":";
		os << items[i].num;
	}

	return os.str();
}

bool WSActivityMgr::IsInOpActivity(UInt32 activityId)
{
	auto itr = m_opRegInfos.find(activityId);
	if (itr != m_opRegInfos.end())
	{
		if (itr->second.state == AS_IN)
		{
			return true;
		}
	}

	return false;
}

bool WSActivityMgr::GetOpActivityById(UInt32 id, OpActivityRegInfo& opAct)
{
	auto iter = m_opRegInfos.find(id);
	if (iter != m_opRegInfos.end())
	{
		opAct = iter->second;
		return true;
	}

	return false;
}

void WSActivityMgr::GetActivityDropAddition(UInt32 dungeonId, DungeonAddition& addition)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return;
	}

	for (auto& itr : m_opRegInfos)
	{
		auto& activity = itr.second;
		if (activity.state != AS_IN)
		{
			continue;
		}

		if (activity.tmpType == OAT_DUNGEON_DROP_ACTIVITY)
		{
			if (dungeonData->level < activity.minLevel || dungeonData->level > activity.maxLevel)
			{
				continue;
			}

			for (auto dropItemType : activity.parm2)
			{
				if (dropItemType >= DIT_NUM)
				{
					continue;
				}

				addition.dropAddition[dropItemType] += activity.parm;
			}
		}

		if (activity.tmpType == OAT_DUNGEON_DROP_RATE_ADDITION)
		{
			if (dungeonData->level < activity.minLevel || dungeonData->level > activity.maxLevel)
			{
				continue;
			}

			for (auto dropItemType : activity.parm2)
			{
				if (dropItemType >= DIT_NUM)
				{
					continue;
				}

				addition.dropMultiplyRateAddition[dropItemType] += activity.parm;
			}
		}
	}
}

void WSActivityMgr::OnTickOpAct(const Avalon::Time& now)
{
	TraversalOpRegActs(now);
}

void WSActivityMgr::TraversalOpRegActs(const Avalon::Time& now)
{
	OpActRegMap::iterator iter = m_opRegInfos.begin();
	while (iter != m_opRegInfos.end())
	{
		OpActivityRegInfo& info = iter->second;

		if (info.dataSource == OADS_FROM_GLOBAL_ACTIVITY)
		{
			++iter;
			continue;
		}
		
		if (info.state == AS_INVALID)
		{
			continue;
		}

		if ((info.state == AS_IN || info.state == AS_PREPARE) && now.Sec() >= info.endTime)
		{
			OnOpActStateChange(info, AS_END);
		}
		else if ((info.state == AS_END || info.state == AS_PREPARE) && (now.Sec() >= info.startTime) && (now.Sec() < info.endTime))
		{
			OnOpActStateChange(info, AS_IN);
		}
		else if (info.state == AS_END && info.prepareTime != 0 && (now.Sec() >= info.prepareTime) && (now.Sec() < info.startTime))
		{
			OnOpActStateChange(info, AS_PREPARE);
		}
		
		++iter;
	}
}

void WSActivityMgr::OnOpActStateChange(OpActivityRegInfo& info, ActivityState state)
{
	if (state == info.state)
	{
		return;
	}

	info.state = state;
	if (state == AS_PREPARE)
	{
	}
	else if (state == AS_IN)
	{
		OnOpActivityChangeOpen(info);
	}
	else if (state == AS_END)
	{
		OnOpActivityChangeClose(info);
	}

	SyncOpActStateChange(info);
}

void WSActivityMgr::SyncOpActStateChange(OpActivityRegInfo& info)
{
	CLProtocol::WSOpActivityChange protocol;
	protocol.opActRegInfo = info;

	Router::BroadcastToScene(protocol);
	Router::BroadcastToGate(protocol);

	InfoLogStream << "OpActivity(" << info.dataId << ", " << info.name << "), state(" << info.state << ") sync..." << LogStream::eos;
}

void WSActivityMgr::GlobalActivitySync(const OpActivityInfoObjectVec& opActs)
{
	CLProtocol::WSSyncOpActivityData syncOpActData;

	for (auto opAct : opActs)
	{
		OpActivityRegInfo opActRegInfo;
		opActRegInfo.dataId = opAct.id;
		opActRegInfo.state = opAct.status;
		opActRegInfo.tmpType = opAct.type;
		opActRegInfo.name = opAct.name;
		opActRegInfo.prepareTime = opAct.prepareTime;
		opActRegInfo.startTime = opAct.startTime;
		opActRegInfo.endTime = opAct.endTime;
		opActRegInfo.playerLevelLimit = opAct.playerLevelLimit;
		opActRegInfo.desc = opAct.description;
		opActRegInfo.ruleDesc = opAct.ruleDesc;
		opActRegInfo.parm = opAct.parm;
		opActRegInfo.parm2 = opAct.parm2;
		opActRegInfo.dataSource = OADS_FROM_GLOBAL_ACTIVITY;
		opActRegInfo.tasks = opAct.tasks;
		opActRegInfo.taskDesc = opAct.taskDesc;

		if (opActRegInfo.prepareTime > opActRegInfo.startTime)
		{
			ErrorLogStream << "OpActivity(" << opActRegInfo.dataId << ") error time format: prepareTime(" 
				<< opActRegInfo.prepareTime << ") is greater than startTime("
				<< opActRegInfo.startTime << ")!" << LogStream::eos;
			continue;
		}

		if (opActRegInfo.startTime > opActRegInfo.endTime)
		{
			ErrorLogStream << "OpActivity(" << opActRegInfo.dataId << ") error time format: startTime("
				<< opActRegInfo.startTime << ") is greater than endTime("
				<< opActRegInfo.endTime << ")!" << LogStream::eos;
			continue;
		}

		auto iter = m_opRegInfos.find(opAct.id);
		if (iter == m_opRegInfos.end())
		{
			m_opRegInfos[opActRegInfo.dataId] = opActRegInfo;
			syncOpActData.datas[opActRegInfo.dataId] = opActRegInfo;
		}
		else
		{
			auto& oldOpAct = iter->second;
			if (oldOpAct != opActRegInfo)
			{
				oldOpAct = opActRegInfo;
				SyncOpActStateChange(opActRegInfo);
			}
		}

		InfoLogStream << "Sync activity info(id=" << opActRegInfo.dataId << ", state=" << opActRegInfo.state
			<< ", preT=" << TimeUtil::TimestampToStandardFormat(opActRegInfo.prepareTime)
			<< ", st=" << TimeUtil::TimestampToStandardFormat(opActRegInfo.startTime) 
			<< ", et=" << TimeUtil::TimestampToStandardFormat(opActRegInfo.endTime)
			<< ")." << LogStream::eos;
	}

	if (!syncOpActData.datas.empty())
	{
		Router::BroadcastToScene(syncOpActData);
		//Router::BroadcastToGate(syncOpActData);
	}
}

void WSActivityMgr::OnOpActivityChangeOpen(const OpActivityRegInfo& info)
{
	switch ((OpActivityID)info.dataId)
	{
	case OAID_LEVEL_SHOW_FOR_NEW_SERVER:
		OnChangeOpenNewServerFighting(info);
		break;

	default:break;
	}

	switch ((OpActivityType)info.tmpType)
	{
	case OAT_BLACK_MARKET_SHOP:
		BlackMarketMgr::Instance()->OnOpActOpen(info);
		break;

	default:break;
	}
}

void WSActivityMgr::OnChangeOpenNewServerFighting(const OpActivityRegInfo& info)
{
	// 根据排行榜得出前几名
	class NewServerFightingVisitor : public WSSortListVisitor
	{
		struct RankInfo
		{
			ObjID_t id;
			std::string name;
			UInt32 ranking;
		};

	public:
		NewServerFightingVisitor(UInt32 maxRankingNum) : m_MaxRankingNum(maxRankingNum) {}

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			// 只会发送符合奖励条件的排名（比如前10名)
			if (ranking <= m_MaxRankingNum)
			{
				RankInfo info;
				info.id = entry.id;
				info.name = entry.name;
				info.ranking = ranking;

				rankInfos.push_back(info);
			}

			return true;
		}

		std::vector<RankInfo> rankInfos;

	private:
		UInt32 m_MaxRankingNum;
	};

	NewServerFightingVisitor vistor((UInt32)info.tasks.size());
	WSSortListMgr::Instance()->VisitSortList(SORTLIST_NEW_SERVER_LEVEL, vistor);

	std::string sender = SysNotifyMgr::Instance()->MakeString(9211);
	std::string title = SysNotifyMgr::Instance()->MakeString(9212);

	for (const auto& rankInfo : vistor.rankInfos)
	{
		for (const auto& task : info.tasks)
		{
			if (task.completeNum == rankInfo.ranking)
			{
				std::string content = SysNotifyMgr::Instance()->MakeString(9213, rankInfo.ranking);
				std::string reason = GetReason(SOURCE_TYPE_NEW_SERVER_FIGHTING, rankInfo.id, rankInfo.ranking);
				MailBox::SendSysMail(rankInfo.id, sender, title, content, reason, task.rewards);
			}
		}

		// 第一名跑马灯一天 2小时一次
		if (rankInfo.ranking == 1)
		{
			UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
			AnnouncementMgr::Instance()->PublicAnnouncementList(0, 64, curTime, DAY_TO_SEC, 2 * HOUR_TO_SEC, rankInfo.name);
		}
	}
}

void WSActivityMgr::OnOpActivityChangeClose(const OpActivityRegInfo& info)
{
	switch ((OpActivityType)info.tmpType)
	{
	case OAT_BLACK_MARKET_SHOP:
		BlackMarketMgr::Instance()->OnOpActClose(info);
		break;

	default:break;
	}

	class NotifyWSPlayerVisitor : public WSPlayerVisitor
	{
	public:
		NotifyWSPlayerVisitor(const OpActivityRegInfo& info) :m_OpAct(info) {}

		bool operator()(WSPlayer* player)
		{
			if (!player) return true;

			player->OnOpActivityClose(m_OpAct);

			return true;
		}
		
	private:
		OpActivityRegInfo m_OpAct;
	};

	NotifyWSPlayerVisitor visitor(info);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void WSActivityMgr::OnPlayerLevelUp(WSPlayer* player, UInt16 oldLv)
{
	if (!player)
	{
		return;
	}

	for (size_t i = 0; i < m_Activities.size(); ++i)
	{
		ActivityInfo& info = m_Activities[i];
		if (info.level > oldLv && info.level <= player->GetLevel())
		{
			if (info.state == AS_IN)
			{
				CLProtocol::WorldNotifyClientActivity notify;
				notify.id = info.id;
				notify.name = info.name;
				notify.type = AS_IN;
				notify.level = info.level;
				notify.startTime = info.startTime;
				notify.dueTime = info.dueTime;
				player->SendProtocol(notify);

				InfoLogStream << PLAYERINFO(player) << " WorldNotifyClientActivity: notify activity(" << info.id << ") state(" << (UInt32)info.state << ") level(" << info.level << ") change." << LogStream::eos;
			}
		}
	}
}

UInt32 WSActivityMgr::GetMallSale(OpActivityType type)
{
	auto itr = m_opRegInfos.begin();
	for (; itr != m_opRegInfos.end(); ++itr)
	{
		OpActivityRegInfo& actInfo = itr->second;
		if (actInfo.tmpType == type && actInfo.state == ActivityState::AS_IN)
		{
			return actInfo.parm;
		}
	}

	return 0;
}

bool WSActivityMgr::IsInActivity(UInt16 playerLevel, UInt32 activityId)
{
	for (auto activity : m_Activities)
	{
		if (activity.id == activityId && playerLevel >= activity.level)
		{
			return true;
		}
	}

	return false;
}

UInt16 WSActivityMgr::GetActivityNeedLevel(UInt32 activityId)
{
	for (auto itr : m_mapRegInfos)
	{
		auto& activity = itr.second;
		if (activity.id == activityId)
		{
			return activity.level;
		}
	}

	return MAX_LEVEL;
}

bool WSActivityMgr::IsAccountActivity(UInt32 activityId)
{
	std::vector<ActiveTaskDataEntry*> activitys;
	ActiveTaskDataEntryMgr::Instance()->GetActives(activityId, activitys);

	for (auto activity : activitys)
	{
		if (activity->taskBelong > 0)
		{
			return true;
		}
	}

	return false;
}

void WSActivityMgr::GetRoleActivity(std::vector<ActivityInfo>& activities)
{
	for (auto activity : m_Activities)
	{
		if (!IsAccountActivity(activity.id))
		{
			activities.push_back(activity);
		}
	}
}

void WSActivityMgr::GetAccountActivity(std::vector<ActivityInfo>& activities, WSPlayer* player)
{
	if (!player) return;

	for (auto activity : m_Activities)
	{
		if (IsAccountActivity(activity.id) && activity.level <= player->GetLevel())
		{
			activities.push_back(activity);
		}
	}
}

WSActivityMgr::ActivityRegInfo*  WSActivityMgr::FindActivityRegInfo(UInt32 actId)
{
	auto it = m_mapRegInfos.find(actId);
	if (it == m_mapRegInfos.end())
	{
		return NULL;
	}
	return &(it->second);
}

ActivityInfo* WSActivityMgr::FindActivityInfo(std::string& actName)
{
	ActivityVec::iterator iter = m_Activities.begin();
	for (; iter != m_Activities.end(); ++iter)
	{
		if (iter->name == actName && AS_IN == iter->state)
		{
			return &(*iter);
		}
	}
	return NULL;
}

WSActivityMgr::ActivityRegInfo*  WSActivityMgr::FindNearestActivityRegInfo(std::string& actName)
{
	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
	ActivityRegInfo* pRegInfo = NULL;
	for (auto& it : m_mapRegInfos)
	{
		ActivityRegInfo& regInfo = it.second;
		if (regInfo.name != actName)
		{
			continue;
		}
		if (currTime > regInfo.startTime + regInfo.preTime + regInfo.lastTime)
		{
			continue;
		}
		if (pRegInfo == NULL || (pRegInfo && regInfo.startTime < pRegInfo->startTime))
		{
			pRegInfo = &regInfo;
		}
	}

	return pRegInfo;
}

void WSActivityMgr::_SendOpActivitiesToServer(Avalon::NetConnectionPtr conn)
{
	if (conn == NULL) return;

	CLProtocol::WSSyncOpActivityData syncOpActData;
	for (auto elem : m_opRegInfos)
	{
		if (OAT_BLACK_MARKET_SHOP == elem.second.tmpType)
		{
			continue;
		}
		syncOpActData.datas.insert(elem);

		if (syncOpActData.datas.size() == OPACT_SEND_PACKET_MAX_SIZE)
		{
			CL_SEND_PROTOCOL(conn, syncOpActData);
			syncOpActData.datas.clear();
		}
	}

	if (!syncOpActData.datas.empty())
	{
		CL_SEND_PROTOCOL(conn, syncOpActData);
	}
}

void WSActivityMgr::_BrocastOpActivitiesToServers()
{
	CLProtocol::WSSyncOpActivityData syncOpActData;
	for (const auto& elem : m_opRegInfos)
	{
		if (OAT_BLACK_MARKET_SHOP == elem.second.tmpType)
		{
			continue;
		}

		syncOpActData.datas.insert(elem);

		if (syncOpActData.datas.size() == OPACT_SEND_PACKET_MAX_SIZE)
		{
			Router::BroadcastToScene(syncOpActData);
			Router::BroadcastToGate(syncOpActData);
			syncOpActData.datas.clear();
		}
	}

	if (!syncOpActData.datas.empty())
	{
		Router::BroadcastToScene(syncOpActData);
		Router::BroadcastToGate(syncOpActData);
	}
}

void WSActivityMgr::_BrocastActivityStateChangeToPlayers(Int32 id, const std::string& name, UInt8 state, UInt16 level, UInt32 preTime, UInt32 startTime, UInt32 dueTime)
{
	InfoLogStream << "start brocast activity(" << id << "), state(" << (UInt32)state << "), level(" << level << ")." << LogStream::eos;

	CLProtocol::WorldNotifyClientActivity protocol;
	protocol.id = id;
	protocol.name = name;
	protocol.type = state;
	protocol.level = level;
	protocol.preTime = preTime;
	protocol.startTime = startTime;
	protocol.dueTime = dueTime;

	class NotifyActivityToPlayer : public WSPlayerVisitor
	{
	public:
		NotifyActivityToPlayer(const CLProtocol::WorldNotifyClientActivity& protocol)
			:m_Protocol(protocol) {}

		bool operator()(WSPlayer* player)
		{
			if (m_Protocol.type != AS_END)
			{
				if (m_Protocol.level > player->GetLevel())
				{
					return true;
				}
			}

			//DebugLogStream << PLAYERINFO(player) << "'s level(" << player->GetLevel() << ") WorldNotifyClientActivity: activity(" << m_Protocol.id << ")'s level(" << m_Protocol.level << ")." << LogStream::eos;
			player->SendProtocol(m_Protocol);
			return true;
		}

	private:
		CLProtocol::WorldNotifyClientActivity m_Protocol;
	};

	NotifyActivityToPlayer visitor(protocol);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);
}

void WSActivityMgr::_NotifyBattleActivityTime(const std::string& actName)
{
	if (actName.empty())
		return;

	UInt32 preTime = WSActivityMgr::Instance()->GetActivityPreTime(actName);
	UInt32 startTime = WSActivityMgr::Instance()->GetActivityStartTime(actName);
	UInt32 endTime = WSActivityMgr::Instance()->GetActivityDueTime(actName);

	CLProtocol::BattleNotifyActivityInfo info;
	info.actName = actName;
	info.preTime = preTime;
	info.startTime = startTime;
	info.endTime = endTime;
	Router::SendToBattle(info);
}

void WSActivityMgr::_NotifyUnionActivityTime(UInt32 unionFunType, const std::string& actName)
{
	if (actName.empty())
		return;

	UInt32 preTime = WSActivityMgr::Instance()->GetActivityPreTime(actName);
	UInt32 startTime = WSActivityMgr::Instance()->GetActivityStartTime(actName);
	UInt32 endTime = WSActivityMgr::Instance()->GetActivityDueTime(actName);

	CLProtocol::UnionNotifyActivityInfo info;
	info.funType = unionFunType;
	info.actName = actName;
	info.preTime = preTime;
	info.startTime = startTime;
	info.endTime = endTime;
	Router::SendToUnion(UnionServerFunctionType(unionFunType), info);
}