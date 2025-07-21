#include "RoutineMgr.h"

#include <CLRoutineProtocol.h>
#include <CLActiveEventDataEntry.h>
#include <CLGiftBagDataEntry.h>
#include <CLSysNotify.h>
#include "CLErrorCode.h"

#include "Player.h"
#include "ScriptMgr.h"
#include "ItemScript.h"
#include "ItemMaker.h"
#include "SceneSysNotify.h"
#include "ActivityMgr.h"


RoutineMgr::RoutineMgr()
{
	m_pOwner = NULL;
}

RoutineMgr::~RoutineMgr()
{
}

void RoutineMgr::SendActiveDegreeInfo()
{
	CLProtocol::SceneActiveDegreeRet ret;
	ret.activeEvents = m_ActiveEvents;
	ret.boxGotFlag = GetOwner()->GetCounter("dailybox_flag");
	ret.activeDegree = GetActiveDegree();
	GetOwner()->SendProtocol(ret);
}

void RoutineMgr::SendTodaySigninInfo()
{
	CLProtocol::SceneRoutineTodaySigninAwardRet ret;
	UInt32 signinDays = GetSigninDays();
	ret.signinDays = signinDays;
	ret.signinBoxId = GetSigninBoxByDays(signinDays);
	ret.vipSigninBoxId = GetVipSigninBoxByDays(signinDays);
	ret.boxGotFlag = GetOwner()->GetCounter("dailybox_flag");
	GetOwner()->SendProtocol(ret);
}

UInt32 RoutineMgr::GetDailyOnlineTime() const
{
	Avalon::Time onlineTime(GetOwner()->GetOnlineTime());
	if(!IsSameDay(onlineTime, CURRENT_TIME))
	{
		Avalon::Date dayBegin(CURRENT_TIME);
		dayBegin.Hour(0);
		dayBegin.Min(0);
		dayBegin.Sec(0);
		return UInt32(CURRENT_TIME.Sec() - dayBegin.ToTime().Sec());
	}
	
	return GetOwner()->GetCounter("online_time") + 
		UInt32(CURRENT_TIME.Sec() > onlineTime.Sec() ? CURRENT_TIME.Sec() - onlineTime.Sec() : 0);
}

UInt32 RoutineMgr::GetActiveDegree() const
{
	UInt32 value = 0;
// 	for(ActiveEventMap::const_iterator iter = m_ActiveEvents.begin();
// 		iter != m_ActiveEvents.end(); ++iter)
// 	{
// 		ActiveEventDataEntry* dataEntry = ActiveEventDataEntryMgr::Instance()->FindEntry(iter->first);
// 		if(dataEntry != NULL)
// 		{
// 			value += UInt32(iter->second) * dataEntry->activeDegree;
// 		}
// 	}
	return value;
}

void RoutineMgr::Signin()
{
	UInt32 signin_flag = GetOwner()->GetCounter("signin_flag");
	Int32 weekday = CURRENT_DATE.WDay();
	if(!(signin_flag & (1 << weekday)))
	{
		signin_flag |= (1 << weekday);
		GetOwner()->SetCounter("signin_flag", signin_flag);
	}
}

UInt32 RoutineMgr::GetSigninDays() const
{
	UInt32 signin_flag = GetOwner()->GetCounter("signin_flag");
	Int32 weekDay = CURRENT_DATE.WDay();
	UInt32 days = 0;
	while(weekDay > 0)
	{
		if(signin_flag & (1 << weekDay)) ++days;
		else break;

		--weekDay;
	}
	return days;
}

bool RoutineMgr::GetAward(UInt8 index)
{
	return true;
}

bool RoutineMgr::GiveAward(const char* reason, const std::vector<ItemReward>& rewards)
{
	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	rewardGroup->GetFitRewards(GetOwner(), rewards);
	if (ErrorCode::SUCCESS != GetOwner()->CheckAddRewards(rewardGroup))
	{
		GetOwner()->SendNotify(1602);
		return false;
	}
	GetOwner()->AddRewards(reason, rewardGroup);
	return true;
}

void RoutineMgr::GiveMailAward(const char* name, const std::vector<ItemReward>& rewards)
{
	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	rewardGroup->GetFitRewards(GetOwner(), rewards);
	if (ErrorCode::SUCCESS != GetOwner()->CheckAddRewards(rewardGroup))
	{
		char content[512];
		SysNotifyMgr::Instance()->MakeNotify(409, content, 512);
		//Sys::SendSysMail(GetOwner()->GetID(), name, content, 0, rewardGroup);
	}
	else
	{
		GetOwner()->AddRewards(GetReason(LogSourceType::SOURCE_TYPE_INVALID).c_str(), rewardGroup);
	}
}

void RoutineMgr::OnOnline(bool bDayChanged)
{
	if(bDayChanged)
	{
		if(!m_ActiveEvents.empty())
		{
			m_ActiveEvents.clear();
			SetDirty();
		}
	}

	Signin();

	CLProtocol::SceneRoutineSyncBaseInfo sync;
	sync.bGotQQVipGiftbag = 0;
	UInt32 dailyBoxFlag = GetOwner()->GetCounter("dailybox_flag");
	if((dailyBoxFlag & (1 << DAILY_GIFTBAG_QQVIP_RECOMMEND1))
		|| (dailyBoxFlag & (1 << DAILY_GIFTBAG_QQVIP_RECOMMEND2)))
	{
		sync.bGotQQVipGiftbag = 1;
	}
	sync.onlineTime = GetDailyOnlineTime();
	GetOwner()->SendProtocol(sync);

	OnActiveEventFinished(10);
}

void RoutineMgr::OnOffline()
{
	UInt32 onlineTime = GetDailyOnlineTime();
	GetOwner()->SetCounter("online_time", onlineTime);
}

void RoutineMgr::OnDayChanged()
{
	Signin();

	CLProtocol::SceneNotifyResetActiveDegree notify;
	notify.signinDays = GetSigninDays();
	notify.signinBoxId = GetSigninBoxByDays(notify.signinDays);
	notify.vipSigninBoxId = GetVipSigninBoxByDays(notify.signinDays);
	GetOwner()->SendProtocol(notify);

	if(!m_ActiveEvents.empty())
	{
		m_ActiveEvents.clear();
		SetDirty();
	}
}

void RoutineMgr::OnActiveEventFinished(UInt8 id)
{
	++m_ActiveEvents[id];
	SetDirty();

	CLProtocol::SceneNotifyActiveEventFinished notify;
	notify.id = id;
	notify.times = m_ActiveEvents[id];
	notify.activeDegree = GetActiveDegree();
	GetOwner()->SendProtocol(notify);

	SendActiveDegreeInfo();
}

void RoutineMgr::DecodeString(std::istringstream& is)
{
	UInt32 id = 0;
	UInt32 times = 0;
	char split = 0;

	while((is >> id) && id != 0)
	{
		is >> split >> times >> split;

		m_ActiveEvents.insert(std::make_pair(UInt8(id), UInt8(times)));
	}
}

void RoutineMgr::EncodeString(std::ostringstream& os)
{
	for(ActiveEventMap::iterator iter = m_ActiveEvents.begin();
		iter != m_ActiveEvents.end(); ++iter)
	{
		os << UInt32(iter->first) << ',' << UInt32(iter->second) << ';';
	}
}
