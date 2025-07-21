#include "SysInterface.h"

#include <CLRecordClient.h>
#include <CLCounterDefine.h>
#include <CLRoutineProtocol.h>
#include <CLSortListProtocol.h>
#include <CLSceneProtocol.h>
#include <CLGoldConsignmentProtocol.h>

#include "ScriptMgr.h"
#include "TimerScript.h"
#include "WSSceneMgr.h"
#include "WorldSysNotify.h"
#include "MailBox.h"
#include "WSRouter.h"
#include "WSActivityMgr.h"
#include "WSPlayer.h"
#include "WSSortListMgr.h"
#include "GameParamMgr.h"
#include "WorldPlayerScript.h"
#include "WSPlayerMgr.h"
#include "GameParamMgr.h"
#include "AnnouncementMgr.h"
#include "WSServiceSwitchMgr.h"
#include "GuildBattleMgr.h"
#include "MallMgr.h"
#include "WSActivityMonsterMgr.h"

SysInterface sys;

void SysInterface::RegCounter(const char* name, Int32 cycle, UInt32 param)
{
	CounterCfg::Instance()->RegCounter(name, cycle, param);
}

UInt32 SysInterface::TimeOfDay()
{
	Avalon::Date date(CURRENT_TIME);
	UInt32 time = UInt32(date.Hour() * 60 * 60 + date.Min() * 60 + date.Sec());
	return time;
}

UInt32 SysInterface::MakeTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec)
{
	Avalon::Date date;
	date.Year(year);
	date.Month(month);
	date.Day(day);
	date.Hour(hour);
	date.Min(min);
	date.Sec(sec);
	return UInt32(date.ToTime().Sec());
}

WSScene* SysInterface::FindScene(UInt32 id)
{
	return WSSceneMgr::Instance()->FindScene(id);
}

UInt32 SysInterface::SelectDynSceneServer()
{
	UInt32	uNodeId = WSSceneMgr::Instance()->SelectRandSceneServer();
	uNodeId = WSSceneMgr::Instance()->SelectDynSceneServer(uNodeId);

	return uNodeId;
}

void SysInterface::CreatePublicDynScene(UInt32 uMapId, UInt16 uLevel, UInt64 uParam, UInt32 uLastTime)
{
	WSSceneMgr::Instance()->CreatePublicDynScene(uMapId, uLevel, uParam, uLastTime);
}

void SysInterface::BroadcastNotify(UInt16 type, UInt8 color, const char* word)
{
	Sys::BroadcastNotify(type, color, word);
}

void SysInterface::BroadcastNotifyByLvl(UInt16 level, UInt16 type, UInt8 color, const char* word)
{
	Sys::BroadcastNotifyByLvl(level, type, color, word);
}

std::vector<ItemReward>* SysInterface::GetRewardGroup()
{
	static std::vector<ItemReward>	group;
	group.clear();
	return &group;
}

void SysInterface::AddReward(std::vector<ItemReward> *pGroup, UInt32 uId, UInt32 uNum, UInt8 qualityLv, UInt8 strength)
{
	ItemReward	reward;
	reward.id = uId;
	reward.num = uNum;
	reward.qualityLv = qualityLv;
	reward.strenth = strength;

	pGroup->push_back(reward);
}

void SysInterface::SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, std::vector<ItemReward>* rewards)
{
	if(rewards != NULL)
	{
		MailBox::SendSysMail(tReceiverId, senderName, title, content, reason, *rewards);
	}
	else
	{
		std::vector<ItemReward>	items;
		MailBox::SendSysMail(tReceiverId, senderName, title, content, reason, items);
	}
}

void SysInterface::BroadcastSysMailOnline(const char* senderName, const char* title, const char* content, const char* reason, std::vector<ItemReward>* rewards)
{
	if(rewards != NULL)
	{
		MailBox::BroadcastSysMailOnline(senderName, title, content, reason, *rewards);
	}
	else
	{
		std::vector<ItemReward>	items;
		MailBox::BroadcastSysMailOnline(senderName, title, content, reason, items);
	}
}

void SysInterface::ClearActivityRegInfos()
{
	WSActivityMgr::Instance()->ClearRegInfos();
}

void SysInterface::RegisterDailyActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime)
{
	WSActivityMgr::Instance()->RegisterDailyActivity(id, name, level, startTime, preTime, lastTime);
}

void SysInterface::RegisterOnceActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime)
{
	WSActivityMgr::Instance()->RegisterOnceActivity(id, name, level, startTime, preTime, lastTime);
}

void SysInterface::RegisterOpenServerOnceActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 lastTime, UInt32 openServerTime)
{
	WSActivityMgr::Instance()->RegisterOpenServerOnceActivity(id, name, level, startTime, lastTime, openServerTime);
}

void SysInterface::RegisterWeekActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime)
{
	WSActivityMgr::Instance()->RegisterWeekActivity(id, name, level, startTime, preTime, lastTime);
}

void SysInterface::RegisterMonthActivity(UInt32 id, const char *name, UInt16 level)
{
	WSActivityMgr::Instance()->RegisterMonthActivity(id, name, level);
}

void SysInterface::RegisterGSActivity(UInt32 id, const char *name, UInt16 level, UInt32 lastTime)
{
	WSActivityMgr::Instance()->RegisterGSActivity(id, name, level, lastTime);
}

void SysInterface::RegisterGSActivityEndTime(UInt32 id, const char *name, UInt16 level, const char* endTime)
{
	WSActivityMgr::Instance()->RegisterGSActivity(id, name, level, endTime);
}

void SysInterface::RegisterGSActivityIntervalTime(UInt32 id, const char *name, UInt16 level, const char* fromTime, const char* toTime)
{
	WSActivityMgr::Instance()->RegisterGSActivity(id, name, level, fromTime, toTime);
}

void SysInterface::RegisterGSActivityIntervalDay(UInt32 id, const char *name, UInt16 level, UInt16 fromDay, UInt16 toDay)
{
	WSActivityMgr::Instance()->RegisterGSActivity(id, name, level, fromDay, toDay);
}

void SysInterface::RegisterConsistActivity(UInt32 id, const char *name, UInt16 level)
{
	WSActivityMgr::Instance()->RegisterConsistActivity(id, name, level);
}

void SysInterface::SetActGameStartLimit(UInt32 id, const char *gamestarttime, UInt8 gamestartFlag)
{
	WSActivityMgr::Instance()->SetGameStartLimit(id, gamestarttime, gamestartFlag);
}

void SysInterface::CloseActivity(const char* name)
{
	WSActivityMgr::Instance()->CloseActivity(name);
}

bool SysInterface::IsInActivity(const char* name)
{
	return WSActivityMgr::Instance()->IsInActivity(name);
}

void SysInterface::AddGlobalActivity(const char *name)
{
	return WSActivityMgr::Instance()->AddGlobalActivity(name);
}

void SysInterface::SendAnnouncementNow(UInt32 id, UInt32 interval, UInt32 times)
{
	AnnouncementMgr::Instance()->PublicAnnouncement(0, id, (UInt32)CURRENT_TIME.Sec());
}

void SysInterface::SendAnnouncement(UInt32 id, const char* begintime, UInt32 interval, UInt32 times)
{
	char split;
	UInt32 beginHour = 0;
	UInt32 beginMin = 0;
	std::istringstream fromIs(begintime);
	fromIs >> beginHour >> split >> beginMin;

	Avalon::Date	date = CURRENT_DATE;
	date.Hour(beginHour);
	date.Min(beginMin);
	date.Sec(0);

	AnnouncementMgr::Instance()->PublicActivityAnnouncement(id, (UInt32)date.ToTime().Sec(), interval, times);
}

void SysInterface::PublicAnnouncementList(UInt32 id, UInt32 durationTime, UInt32 intervalTime, UInt32 startTimeOffset)
{
	AnnouncementMgr::Instance()->PublicAnnouncementList(0, id, UInt32(CURRENT_TIME.Sec()) + startTimeOffset, durationTime, intervalTime);
}

void SysInterface::RemoveAnnouncement(UInt32 dataId)
{
	AnnouncementMgr::Instance()->RemoveAnnouncement(dataId);
}

void SysInterface::SetActivityMonsterOpen(UInt8 open)
{
	WSActivityMonsterMgr::Instance()->SetOpen(open != 0 ? true : false);
}

void SysInterface::ClearSortList(UInt8 type)
{
	WSSortListMgr::Instance()->ClearSortList(type);
}

void SysInterface::OnNewYearRedPacketEnd()
{
	WSSortListMgr::Instance()->OnNewYearRedPacketEnd();
}

class SelectCounterCallback : public CLRecordCallback
{
public:
	SelectCounterCallback(const std::string& key):m_Key(key){}
	~SelectCounterCallback(){}

	void OnFinished()
	{
		while(NextRow())
		{
			std::string name = GetData("name").GetString();
			UInt32 updatetime = GetData("updatetime");
			UInt32 value = GetData("value");
			auto config = CounterCfg::Instance()->GetCycleConfig(name);
			if(!config) continue;
			if(value != 0 && CounterCfg::Instance()->CheckReset(updatetime, config))
			{
				value = 0;
			}
			ScriptMgr::Instance()->GetTimerScript()->OnSelectCounterReturn(m_Key.c_str(), GetData("owner"), name.c_str(), value);
		}
	}

private:
	std::string m_Key;
};

void SysInterface::SelectCounterValue(const char* key, const char* counter, UInt32 minValue)
{
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_counter");
	selectCmd->PutCond("name", counter);

	std::ostringstream condstream;
	condstream << " AND `value` >= " << minValue;
	selectCmd->SetLimit(condstream.str());
	CLRecordClient::Instance()->SendCommand(selectCmd, new SelectCounterCallback(key));
}

void SysInterface::VisitSortList(UInt8 sortType, UInt32 uNum, const char *func)
{
	if(::IsWorldSortList(sortType))
	{
		CLProtocol::WorldSortListVisitReq req;
		req.sortType = sortType;
		req.function = func;
		req.num = uNum;
		Router::SendToSortList(req);
		return;
	}

	class ScriptVisitor : public WSSortListVisitor
	{
	public:
		ScriptVisitor(UInt32 uNum, const char *func) : m_uNum(uNum), m_func(func) {}
		~ScriptVisitor() {}

		bool operator()(SortListEntry& entry, UInt32 ranking)
		{
			if(ranking > m_uNum) return false;

			ScriptMgr::Instance()->GetTimerScript()->OnVisitSortlist(m_func, entry.id, entry.ownerId, entry.name.c_str(), ranking);
			return true;
		}

	private:
		UInt32		m_uNum;
		const char	*m_func;
	};

	ScriptVisitor	visitor(uNum, func);
	WSSortListMgr::Instance()->VisitSortList(sortType, visitor);
}

void SysInterface::VisitPlayer(const char* func)
{
	WorldPlayerScript *script = ScriptMgr::Instance()->GetPlayerScript();
	if (script == NULL) return;
	class ScriptPlayerVistor : public WSPlayerVisitor
	{
	public:
		ScriptPlayerVistor(const char* func, WorldPlayerScript *script)
			:m_func(func), m_pScript(script) {}

		bool operator()(WSPlayer* player)
		{
			m_pScript->VisitPlayer(m_func, player);
			return true;
		}
	private:
		const char* m_func;
		WorldPlayerScript *m_pScript; 
	};
	ScriptPlayerVistor vistor(func, script);
	WSPlayerMgr::Instance()->VisitNormalPlayers(vistor);

}

void SysInterface::RegisterNpc(UInt32 npcId, UInt32 mapId, UInt16 x, UInt16 y)
{
	WSSceneMgr::Instance()->RegisterNpc(npcId, mapId, CLPosition(x, y));
}

UInt32 SysInterface::GetGameValue(const char* name)
{
	return GameParamMgr::Instance()->GetValue(name);
}

void SysInterface::SetGameValue(const char* name, UInt32 value)
{
	GameParamMgr::Instance()->SetValue(name, value);
}

void SysInterface::SyncActivityCounter(UInt8 type, UInt32 value)
{
	class SyncCounterVisitor : public WSPlayerVisitor
	{
	public:
		SyncCounterVisitor(UInt8 type, UInt32 value)
		{
			info.type = type;
			info.value = value;
		}
	public:
		bool operator()(WSPlayer* player)
		{
			if (player->GetOpenFlag(info.type) != 0)
			{
				player->SendProtocol(info);
			}
			return true;
		}

	private:
		CLProtocol::WorldSyncActivesCounter info;
	};

	SyncCounterVisitor visitor(type, value);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void SysInterface::BroadcastEffectEvent(UInt8 type)
{
	CLProtocol::SceneBroadcastEffect info;
	info.type = type;
	Router::BroadcastToPlayer(info);
}

UInt32 SysInterface::GetZoneId()
{
	return ZONE_ID;
}

UInt32 SysInterface::GetNeutralZoneId() const
{
	return NEUTRAL_ZONE_ID;
}

const char* SysInterface::GetPlatform() const
{
	return PLATFORM.c_str();
}

void SysInterface::OpenService(UInt32 type)
{
	WSServiceSwitchMgr::Instance()->OpenService((ServiceType)type);
}

void SysInterface::CloseService(UInt32 type)
{
	WSServiceSwitchMgr::Instance()->CloseService((ServiceType)type);
}

void SysInterface::SetAutoBattleMode(UInt8 mode)
{
	WSServiceSwitchMgr::Instance()->SetAutoBattleMode(mode);
}

void SysInterface::RegisterCustomCircleActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, const UInt32 lastTime)
{
	WSActivityMgr::Instance()->RegisterCustomCircleActivity(id, name, level, startTime, preTime, lastTime);
}

void SysInterface::ChangeGuildBattleStatus(UInt32 targetStatus)
{
	GuildBattleMgr::Instance()->ChangeTargetStatus((GuildBattleStatus)targetStatus);
}

void SysInterface::OnSaleMallWaitGoods(UInt32 goodId)
{
	MallMgr::Instance()->OnSaleMallWaitGoods(goodId);
}

void SysInterface::OffSaleMallWaitGoods(UInt32 goodId)
{
	MallMgr::Instance()->OffSaleMallWaitGoods(goodId);
}

void SysInterface::SummonCityMonster()
{
	InfoLogStream << "start to summon city monster..." << LogStream::eos;
	CLProtocol::SceneSummonNpc protocol;
	Router::BroadcastToScene(protocol);
}

void SysInterface::ChijiOpen(const char* name)
{

}

void SysInterface::GoldConsignmentClose()
{
	CLProtocol::UnionGoldConsignmentActivityClose close;
	close.zoneId = ZONE_ID;
	close.endTime = UInt32(CURRENT_TIME.Sec());
	Router::SendToUnion(USFT_GOLD_CONSIGNMENT, close);

	InfoLogStream << "gold consignment close!" << LogStream::eos;
}