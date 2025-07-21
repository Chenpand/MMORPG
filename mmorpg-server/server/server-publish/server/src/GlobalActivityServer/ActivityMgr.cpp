#include "ActivityMgr.h"

#include <json/writer.h>
#include <AvalonMd5.h>
#include <CLActiveEventDataEntry.h>
#include <CLGlobalActivityProtocol.h>
#include <TimeUtil.h>

#include "GASGameZoneMgr.h"
#include "GambingMgr.h"
#include "BetHorseMgr.h"
#include "GASApplication.h"
#include "HttpClient.h"
#include "ArtifactJarMgr.h"
#include "WeekSignMgr.h"
#include "WholeBargainMgr.h"

void GlobalGmOpActivtyRes(HttpClient* client, HttpResponse* response)
{
	InfoLogStream << "gm activity response..." << LogStream::eos;

	if (!response)
	{
		return;
	}

	if (!response->getHttpRequest())
	{
		return;
	}

	std::string reqDataStr(response->getHttpRequest()->getRequestData());
	std::string resDataStr;
	for (auto data : *response->getResponseData())
	{
		resDataStr += data;
	}

	InfoLogStream << "Request(" << reqDataStr << ") and response(" << resDataStr << ")." << LogStream::eos;
}


ActivityMgr::ActivityMgr()
{
	m_PerSecTimer.SetInterval(SEC_TO_MSEC);
}

ActivityMgr::~ActivityMgr()
{
}

bool ActivityMgr::OnInit()
{
	if (!_LoadTable())
	{
		return false;
	}

	if (!_LoadTaskTable())
	{
		return false;
	}

	RequestGmOpActivity();

	return true;
}

void ActivityMgr::Final()
{
	for (auto opAct : m_OpActivityInfoVec)
	{
		delete opAct;
	}
	m_OpActivityInfoVec.clear();

	for (auto elem : m_NeedSyncActGameZones)
	{
		CL_SAFE_DELETE(elem);
	}
	m_NeedSyncActGameZones.clear();
}

void ActivityMgr::OnTick(const Avalon::Time& now)
{
	if (m_PerSecTimer.IsTimeout(now))
	{
		for (auto opAct : m_OpActivityInfoVec)
		{
			if (!opAct) continue;;

			if (opAct->status == AS_INVALID)
			{
				continue;
			}

			if ((opAct->status == AS_IN || opAct->status == AS_PREPARE) && (UInt32)now.Sec() >= opAct->endTime)
			{
				_OnActivityStateChange(opAct, AS_END);
			}
			else if ((opAct->status == AS_END || opAct->status == AS_PREPARE) && ((UInt32)now.Sec() >= opAct->startTime) && (now.Sec() < opAct->endTime))
			{
				_OnActivityStateChange(opAct, AS_IN);
			}
			else if (opAct->status == AS_END && ((UInt32)now.Sec() >= opAct->prepareTime) && ((UInt32)now.Sec() < opAct->startTime))
			{
				_OnActivityStateChange(opAct, AS_PREPARE);
			}
		}

		if (m_NeedSyncActGameZones.empty()) return;

		for (auto elem : m_NeedSyncActGameZones)
		{
			if (!elem) continue;

			if (elem->needSync)
			{
				auto gameZone = GASGameZoneMgr::Instance()->FindGASGameZone(elem->zoneId);
				if (!gameZone) continue;

				_BroadcastActivities(gameZone);
				elem->needSync = false;

				DebugLogStream << "Broadcast activities to zone(" << gameZone->GetID() << ")" << LogStream::eos;
			}
		}
	}
}

void ActivityMgr::ReloadTable()
{
	InfoLogStream << "Start reloading..." << LogStream::eos;

	_LoadTable(true);

	if (m_TempOpActivityInfoVec.empty())
	{
		ErrorLogStream << "Reload datas are empty!" << LogStream::eos;
		return;
	}

	for (auto opAct : m_OpActivityInfoVec)
	{
		if (!opAct) continue;

		if (opAct->dataSource != OADS_TABLE) continue;

		OpActivityInfo* tempOpAct = NULL;
		auto tempOpActIter = m_TempOpActivityInfoVec.begin();
		for (; tempOpActIter != m_TempOpActivityInfoVec.end(); ++tempOpActIter)
		{
			auto elem = *tempOpActIter;
			if (!elem) continue;

			if (elem->id == opAct->id)
			{
				tempOpAct = elem;
				break;
			}
		}

		if (tempOpAct != NULL)
		{
			*opAct = *tempOpAct;

			delete tempOpAct;
			m_TempOpActivityInfoVec.erase(tempOpActIter);
		}
		else
		{
			// 现有内存中的数据在重载后的表格中找不到，则将之关闭
			opAct->endTime = opAct->startTime + 1;
			InfoLogStream << "OpActivity(" << opAct->id << ") will close!" << LogStream::eos;
		}
	}

	// 说明有新加的
	if (!m_TempOpActivityInfoVec.empty())
	{
		InfoLogStream << "Add the new OpActivities!" << LogStream::eos;

		m_OpActivityInfoVec.insert(m_OpActivityInfoVec.end(), m_TempOpActivityInfoVec.begin(), m_TempOpActivityInfoVec.end());
	}

	_ResetNeedSyncActGameZoneInfo();

	m_TempOpActivityInfoVec.clear();
}

void ActivityMgr::AddOpActivityInfo(OpActivityInfo* opAct)
{
	if (!opAct) return;
	m_OpActivityInfoVec.push_back(opAct);
}

OpActivityInfo* ActivityMgr::FindOpActivityInfo(UInt32 actId)
{
	for (auto info : m_OpActivityInfoVec)
	{
		if (!info) continue;

		if (info->id == actId) return info;
	}

	return NULL;
}

void ActivityMgr::VisitOpActivities(const std::function<bool(OpActivityInfo*)>& F)
{
	for (auto info : m_OpActivityInfoVec)
	{
		if (!info) continue;

		if (!F(info)) return;
	}
}

void ActivityMgr::RequestGmOpActivity()
{
	std::string url = GASApplication::Instance()->GetGmOpActivityLibUrl();
	if (url.empty())
	{
		ErrorLogStream << "gmOpActivityLibUrl is empty!" << LogStream::eos;
		return;
	}

	std::string applyServer = Avalon::StringUtil::ConvertToString(GASApplication::Instance()->GetServerIdInGM());
	std::string md5Str;
	std::string key = GASApplication::Instance()->GetGmOpActivityKey();
	md5Str += applyServer + key;

	char md5Buff[512] = { 0 };
	Avalon::md5_encode32(md5Buff, (void*)md5Str.c_str(), md5Str.length());
	std::string sign(md5Buff);

	std::string str;
	str.append("apply_server_id=").append(applyServer).append("&serverid=")
		.append(applyServer).append("&sign=").append(sign);

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());
	httpReq->setResponseCallback(GlobalGmOpActivtyRes);
	http->send(httpReq);

	InfoLogStream << "start request activities from gm(" << url << ") by request data(" << str << ")." << LogStream::eos;
}

void ActivityMgr::GmSetOpActivity(const OpActivityInfo& opAct)
{
	InfoLogStream << "GmSetOpActivity opActivity(" << opAct.id <<
		"), pt=" << TimeUtil::TimestampToStandardFormat(opAct.prepareTime) <<
		", st=" << TimeUtil::TimestampToStandardFormat(opAct.startTime) <<
		", et=" << TimeUtil::TimestampToStandardFormat(opAct.endTime) << LogStream::eos;

	auto oldOpAct = FindOpActivityInfo(opAct.id);
	if (!oldOpAct)
	{
		OpActivityInfo* newAct = new OpActivityInfo();
		*newAct = opAct;
		AddOpActivityInfo(newAct);

		InfoLogStream << "Add opActivity Success!" << LogStream::eos;
	}
	else
	{
		*oldOpAct = opAct;
		_BroadcastActivity(oldOpAct);

		InfoLogStream << "Update opActivity Success!" << LogStream::eos;
	}
}

bool ActivityMgr::GmCloseOpActivity(UInt32 id)
{
	auto opAct = FindOpActivityInfo(id);
	if (!opAct)
	{
		return false;
	}

	opAct->endTime = opAct->startTime + 1;
	return true;
}

void ActivityMgr::OnConnected(UInt32 id)
{
	DebugLogStream << "Zone(" << id << ") connected." << LogStream::eos;

	bool isNewZone = true;
	for (auto elem : m_NeedSyncActGameZones)
	{
		if (!elem) continue;

		if (elem->zoneId == id)
		{
			elem->needSync = true;
			isNewZone = false;
		}
	}

	if (isNewZone)
	{
		NeedSyncActivityGameZoneInfo* info = new NeedSyncActivityGameZoneInfo();
		info->zoneId = id;
		info->needSync = true;
		m_NeedSyncActGameZones.push_back(info);

		DebugLogStream << "Add new zone(" << id << ")." << LogStream::eos;
	}
}

void ActivityMgr::_ResetNeedSyncActGameZoneInfo()
{
	for (auto elem : m_NeedSyncActGameZones)
	{
		if (!elem) continue;

		elem->needSync = true;
	}
}

void ActivityMgr::_BroadcastActivity(const OpActivityInfo* opAct)
{
	if (!opAct) return;

	CLProtocol::GASActivitySync protocol;
	protocol.opActs.push_back(*opAct);
	GASGameZoneMgr::Instance()->BroadcastToWorld(protocol);
}

void ActivityMgr::_BroadcastActivities()
{
	CLProtocol::GASActivitySync protocol;
	VisitOpActivities([&protocol](OpActivityInfo* opAct)->bool{
		protocol.opActs.push_back(*opAct);
		return true;
	});

	if (protocol.opActs.empty())
	{
		ErrorLogStream << "Activties are empty!" << LogStream::eos;
	}
	else
	{
		GASGameZoneMgr::Instance()->BroadcastToWorld(protocol);
	}
}

void ActivityMgr::_BroadcastActivities(GASGameZone* zone)
{
	if (!zone) return;

	CLProtocol::GASActivitySync protocol;
	VisitOpActivities([&protocol](OpActivityInfo* opAct)->bool{
		protocol.opActs.push_back(*opAct);
		return true;
	});

	if (protocol.opActs.empty())
	{
		ErrorLogStream << "Activties are empty!" << LogStream::eos;
	}
	else
	{
		zone->BroadcastToWorld(protocol);
	}
}

bool ActivityMgr::_LoadTable(bool isReload)
{
	class OpActivityDataVisitor : public Avalon::DataEntryVisitor<OpActivityDataEntry>
	{
	public:
		bool operator()(OpActivityDataEntry* data)
		{
			if (NULL == data) return false;

			switch (data->tmpType)
			{
			case OAT_GAMBING:
			case OAT_BET_HORSE:
			case OAT_JAR_DRAW_LOTTERY:
			case OAT_ARTIFACT_JAR_SHOW:
			case OAT_WEEK_SIGN_ACTIVITY:
			case OAT_WHOLE_BARGAIN_DISCOUNT:
			{
				activityDatas.push_back(data); 
				break;
			}
			default:break;
			}

			return true;
		}

		std::vector<OpActivityDataEntry*> activityDatas;
	};

	OpActivityDataVisitor visitor;
	OpActivityDataEntryMgr::Instance()->Visit(visitor);

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto data : visitor.activityDatas)
	{
		if (!data) continue;

		OpActivityInfo* opAct = new OpActivityInfo();
		opAct->id = data->id;
		opAct->type = data->tmpType;
		opAct->name = data->name;
		opAct->prepareTime = data->prepareTime;
		opAct->startTime = data->startTime;
		opAct->endTime = data->endTime;

		if (opAct->prepareTime > opAct->startTime)
		{
			ErrorLogStream << "OpActivity(" << opAct->id << ") error time format: prepareTime(" << opAct->prepareTime << ") is greater than startTime(" << opAct->startTime << ")!" << LogStream::eos;
			delete opAct;
			continue;
		}

		if (opAct->startTime > 0 && opAct->startTime >= opAct->endTime)
		{
			ErrorLogStream << "OpActivity(" << opAct->id << ") error time format: startTime(" << opAct->startTime << ") is greater than endTime(" << opAct->endTime << ")!" << LogStream::eos;
			delete opAct;
			continue;
		}

		if (opAct->endTime > 0 && opAct->endTime < curTime)
		{
			ErrorLogStream << "OpActivity(" << opAct->id << ") endTime(" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << ") reach current time." << LogStream::eos;
			delete opAct;
			continue;
		}

		opAct->playerLevelLimit = data->playerLevelLimit;
		opAct->description = data->desc;
		opAct->ruleDesc = data->ruleDesc;
		opAct->parm = data->parm;
		opAct->parm2 = data->parm2;

		opAct->dataSource = OADS_TABLE;

		if (isReload)
		{
			m_TempOpActivityInfoVec.push_back(opAct);
		}
		else
		{
			m_OpActivityInfoVec.push_back(opAct);
		}

		InfoLogStream << "Load OpActivity table success, opAct(id=" << opAct->id << ", status=" << opAct->status << ", st=" << TimeUtil::TimestampToStandardFormat(opAct->startTime)
			<< ", et=" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << ")." << LogStream::eos;
	}

	return true;
}

bool ActivityMgr::_LoadTaskTable()
{
	class OpActivityTaskDataVisitor : public Avalon::DataEntryVisitor<OpActivityTaskDataEntry>
	{
	public:

		OpActivityTaskDataVisitor(OpActivityInfoVec* opVec) : m_OpActInfoVec(opVec) {}

		bool operator()(OpActivityTaskDataEntry* data)
		{
			if (NULL == data)
			{
				return false;
			}

			switch (data->opActivityType)
			{
			case OAT_WEEK_SIGN_ACTIVITY:
			{
				for (auto& it : *m_OpActInfoVec)
				{
					if (it->id == data->opAvtivityId)
					{
						OpActTaskData task;
						task.dataId = data->id;
						task.completeNum = data->completeCount;
						task.rewards = data->rewards;
						task.variables = data->variables;
						task.variables2 = data->variables2;
						task.counters = data->counters;
						task.taskName = data->taskName;
						task.varProgressName = data->varProgressName;

						if (!it->tasks.empty())
							it->taskDesc += "|";

						it->taskDesc += data->taskDesc;

						it->tasks.push_back(task);
					}
				}
			}
			break;
			default:break;
			}

			return true;
		}

		OpActivityInfoVec* m_OpActInfoVec;
	};

	OpActivityTaskDataVisitor visitor(&m_OpActivityInfoVec);
	OpActivityTaskDataEntryMgr::Instance()->Visit(visitor);

	return true;
}

void ActivityMgr::_OnActivityStateChange(OpActivityInfo* opAct, ActivityState status)
{
	if (!opAct) return;

	if (opAct->type == OAT_BET_HORSE && GASApplication::Instance()->IsOpenBetHorse())
	{
		return;
	}

	if (status == opAct->status)
	{
		return;
	}

	opAct->status = status;

	switch (status)
	{
	case AS_PREPARE:
	{
		_OnActivityPrepare(opAct);
		break;
	}
	case AS_IN:
	{
		_OnActivityStart(opAct);
		break;
	}
	case AS_END:
	{
		_OnActivityEnd(opAct);
		break;
	}

	default: break;
	}

	_BroadcastActivity(opAct);

	InfoLogStream << "Sync activity state change: id=" << opAct->id << ", state=" << opAct->status << ", preT=" << TimeUtil::TimestampToStandardFormat(opAct->prepareTime) << ", st=" << TimeUtil::TimestampToStandardFormat(opAct->startTime)
		<< ", et=" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << LogStream::eos;
}

void ActivityMgr::_OnActivityPrepare(const OpActivityInfo* opAct)
{
	if (!opAct) return;

	switch ((OpActivityType)opAct->type)
	{
	case OAT_GAMBING:
	{
		GambingMgr::Instance()->OnActivityPrepare(opAct);
		break;
	}

	default: break;
	}
}

void ActivityMgr::_OnActivityStart(const OpActivityInfo* opAct)
{
	if (!opAct) return;

	switch ((OpActivityType)opAct->type)
	{
	case OAT_GAMBING:
	{
		GambingMgr::Instance()->OnActivityStart(opAct);
		break;
	}
	case OAT_BET_HORSE:
	{
		BetHorseMgr::Instance()->SetActivityStatus(opAct->id, opAct->status);
		break;
	}
	case OAT_JAR_DRAW_LOTTERY:
	{
		ArtifactJarMgr::Instance()->SetActivityStatus(opAct->status);
		break;
	}
	case OAT_WEEK_SIGN_ACTIVITY:
	{
		WeekSignMgr::Instance()->SetActivityStatus(opAct->type, opAct->id, opAct->status);
		break;
	}
	case OAT_WHOLE_BARGAIN_DISCOUNT:
	{
		WholeBargainMgr::Instance()->OnActivityStart(opAct->id);
		break;
	}
	default: break;
	}
}

void ActivityMgr::_OnActivityEnd(const OpActivityInfo* opAct)
{
	if (!opAct) return;

	switch ((OpActivityType)opAct->type)
	{
	case OAT_GAMBING:
	{
		GambingMgr::Instance()->OnActivityEnd(opAct);
		break;
	}
	case OAT_BET_HORSE:
	{
		BetHorseMgr::Instance()->SetActivityStatus(opAct->id, opAct->status);
		break;
	}
	case OAT_JAR_DRAW_LOTTERY:
	{
		ArtifactJarMgr::Instance()->SetActivityStatus(opAct->status);
		break;
	}
	case OAT_WEEK_SIGN_ACTIVITY:
	{
		WeekSignMgr::Instance()->SetActivityStatus(opAct->type, opAct->id, opAct->status);
		break;
	}
	case OAT_WHOLE_BARGAIN_DISCOUNT:
	{
		WholeBargainMgr::Instance()->OnActivityEnd();
		break;
	}
	default: break;
	}
}