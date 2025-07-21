#include "SysInterface.h"

#include <CLChatProtocol.h>
#include <CLSceneProtocol.h>
#include <CLSceneDataEntry.h>
#include <CLCounterDefine.h>
#include <CLMailProtocol.h>
#include <CLGSActivityProtocol.h>
#include <CLRoutineProtocol.h>
#include <CLGiftBagProtocol.h>
#include <CLItemDataEntry.h>
#include <CLLevelAdapterDataEntry.h>

#include "Player.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "SortListMgr.h"
#include "SceneSysNotify.h"
#include "ActivityMgr.h"
#include "SceneMgr.h"
#include "ScriptMgr.h"

SysInterface sys;

bool SysInterface::IsSameDay(UInt32 time1, UInt32 time2)
{
	return ::IsSameDay(Avalon::Time(UInt64(time1) * 1000), Avalon::Time(UInt64(time2) * 1000));
}

void SysInterface::RegCounter(const char* name, Int32 cycle, UInt32 param)
{
	CounterCfg::Instance()->RegCounter(name, cycle, param);
}

void SysInterface::Notify(ObjID_t id, UInt16 type, UInt8 color, const char* word)
{
	if(word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;
	Router::TransmitToPlayer(id, notify);
}

void SysInterface::BroadcastNotify(UInt16 type, UInt8 color, const char* word)
{
	if(word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;
	Router::BroadcastToPlayer(notify);
}

void SysInterface::OnSortValueChanged(UInt8 sortType, ObjID_t id, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3)
{
	SortListMgr::Instance()->OnSortValueChanged(sortType, id, "", 0, 0, 0, level, value1, value2, value3);
}

void SysInterface::OnWorldSortValueChanged(Player* player, UInt8 sortType, UInt32 value1, UInt32 value2, UInt32 value3)
{
	SortListMgr::Instance()->OnWorldSortValueChanged(sortType, player->GetID(), player->GetName(), player->GetVipLvl(), value1, value2, value3);
}

void SysInterface::SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards)
{
	Sys::SendSysMail(tReceiverId, senderName, title, content, reason, rewards);
}

RewardGroup*  SysInterface::GetRewardGroup()
{
	static RewardGroup group;
	group.ClearRewards();
	return &group;
}

bool SysInterface::IsInActivity(const char* name)
{
	if(name == NULL) return false;

	return ActivityMgr::Instance()->IsInActivity(name);
}

const char* SysInterface::GetMapName(UInt32 mapId)
{
	SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(mapId);
	if(dataEntry != NULL)
	{
		return dataEntry->name.c_str();
	}
	return "";
}

UInt8 SysInterface::GetRandDirection()
{
	return Avalon::GetRandDir();
}

UInt32 SysInterface::GetBaseExp(UInt16 level)
{
	return (BASEEXP_FACTOR_A * UInt32(level) + BASEEXP_FACTOR_B);
}

UInt32 SysInterface::GetBaseSilver(UInt16 level)
{
	return UInt32(level) * 40 + 100;
}

UInt32 SysInterface::GetWorldLevel()
{
	return WORLD_LEVEL;
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


void SysInterface::SendOfflineNotify(const char* type, const char* word)
{
	Sys::SendOfflineNotify(type, word);
}
void SysInterface::SyncCounter(Player *pPlayer, const char* szCounterName)
{
	CLProtocol::SceneFestivalCounterRet ret;
	ret.name = szCounterName;
	ret.value = pPlayer->GetCounter(szCounterName);
	pPlayer->SendProtocol(ret);
}

UInt32 SysInterface::GetNeutralZoneId() const
{
	return NEUTRAL_ZONE_ID;
}

UInt32 SysInterface::GetNeutralRandMajorCity() const
{
	return ::GetNeutralRandMajorCity();
}

void SysInterface::GenActivityCityMonster()
{
	SceneMgr::Instance()->GenActivityCityMonster();
}

void SysInterface::RemoveTaskCityMonster(ObjID_t owner, const char* idstr)
{
	if (idstr == NULL)
	{
		return;
	}

	auto id = Avalon::StringUtil::ConvertToValue<ObjID_t>(idstr);
	SceneMgr::Instance()->RemoveTaskCityMonster(owner, id);
}

UInt32  SysInterface::GetEnergyExchangeExp(UInt32 playerLev)
{
	return LevelAdapterDataEntryMgr::Instance()->GetValue(1001, playerLev);
}