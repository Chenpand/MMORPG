#include "WorldBossMgr.h"
#include "WSPlayer.h"
#include "WorldSysNotify.h"

#include <CLChatDefine.h>
#include <CLSceneDataEntry.h>

WorldBossMgr::WorldBossMgr()
{
	m_minTimer.SetInterval(60000);
}

WorldBossMgr::~WorldBossMgr()
{
	for(WorldBossInfoMap::iterator iter = m_mapWorldBossInfo.begin(); iter != m_mapWorldBossInfo.end(); iter++)
		delete iter->second;
	m_mapWorldBossInfo.clear();
}

void WorldBossMgr::RegisterBossInfo(UInt32 uNpcId, ObjID_t uId, std::string strName, UInt32 uMapId, UInt32 uReviveInterval, UInt32 uBirthTime, UInt8 uFlag)
{
	WorldBossInfoMap::iterator	iter = m_mapWorldBossInfo.find(uNpcId);
	WorldBossInfo	*pBossInfo;
	if(iter != m_mapWorldBossInfo.end())
	{
		pBossInfo = iter->second;
	}
	else
	{
		pBossInfo = new WorldBossInfo;
		m_mapWorldBossInfo[uNpcId] = pBossInfo;
	}

	pBossInfo->uId = uId;
	pBossInfo->strName = strName;
	pBossInfo->uMapId = uMapId;
	pBossInfo->uReviveInterval = uReviveInterval;
	pBossInfo->uFlag = uFlag;
	pBossInfo->uReviveTime = uBirthTime;
}

void WorldBossMgr::RegisterCountryInfo( UInt32 npcId, std::string& name, UInt32 mapId, std::string& strRefreshTime )
{	
	WorldBossInfoMap::iterator iter = m_mapWorldBossInfo.find(npcId);
	WorldBossInfo* pBossInfo;
	if(iter != m_mapWorldBossInfo.end())
	{
		pBossInfo = iter->second;
	}
	else
	{
		pBossInfo = new WorldBossInfo;
		m_mapWorldBossInfo[npcId] = pBossInfo;
	}

	pBossInfo->uId = 0;
	pBossInfo->strName = name;
	pBossInfo->uMapId = mapId;
	pBossInfo->uReviveInterval = 0;
	pBossInfo->uFlag = 2;

	std::vector<std::string> refreshVec;
	Avalon::StringUtil::Split(strRefreshTime, refreshVec, "|");
	for (std::vector<std::string>::iterator iter1 = refreshVec.begin();
		iter1 != refreshVec.end(); ++iter1)
	{
		char split = 0;
		UInt16 hour = 0;
		UInt16 min = 0;
		std::istringstream stream(*iter1);
		stream >> hour >> split >> min;

		UInt32 timePoint = hour * HOUR_TO_SEC + min * MIN_TO_SEC;

		RefreshTimeMap::iterator iter2 = m_RefreshTimeMap.find(npcId);
		if (iter2 == m_RefreshTimeMap.end())
		{
			RefreshTimeList refreshTimeList;
			refreshTimeList.insert(refreshTimeList.begin(), timePoint);
			m_RefreshTimeMap.insert(std::make_pair(npcId, refreshTimeList));
		} 
		else
		{
			RefreshTimeList::iterator iter3 = iter2->second.begin();
			while(iter3 != iter2->second.end())
			{
				if(timePoint < (*iter3)) break;
				++iter3;
			}

			iter2->second.insert(iter3, timePoint);
		}
	}

	pBossInfo->uReviveTime = CalcRebirthTime(npcId);
}

void WorldBossMgr::RefreshBossInfo(UInt32 uNpcId, ObjID_t uId, UInt8 uStatus, std::string strKiller)
{
	WorldBossInfoMap::iterator	iter = m_mapWorldBossInfo.find(uNpcId);
	if(iter == m_mapWorldBossInfo.end())
		return;

	WorldBossInfo	*pBossInfo = iter->second;
	pBossInfo->uId = uId;

	if(0 == uStatus)
	{
		if(0 == pBossInfo->uFlag)
		{
			pBossInfo->uReviveTime = UInt32(CURRENT_TIME.Sec() + pBossInfo->uReviveInterval);
		}
		else if(pBossInfo->uFlag == 1)
		{
			UInt32	uRound = (UInt32(CURRENT_TIME.Sec()) - pBossInfo->uReviveTime) / pBossInfo->uReviveInterval + 1;
			pBossInfo->uReviveTime = pBossInfo->uReviveTime + pBossInfo->uReviveInterval * uRound;
		}
		else if (pBossInfo->uFlag == 2)
		{
			pBossInfo->uReviveTime = CalcRebirthTime(uNpcId);
		}

		pBossInfo->strLastKiller = strKiller;
	}
	else
	{
		pBossInfo->uReviveTime = UInt32(CURRENT_TIME.Sec());
		SceneDataEntry	*pSceneData = SceneDataEntryMgr::Instance()->FindEntry(pBossInfo->uMapId);
		if(NULL == pSceneData)
			return;

		if(0 == pBossInfo->uFlag)
		{
			Notify(pBossInfo, 109, pBossInfo->strName.c_str(), pSceneData->name.c_str(), pBossInfo->uMapId, iter->first);
		}
		else
		{
			Notify(pBossInfo, 117, pBossInfo->strName.c_str(), pSceneData->name.c_str());
		}

		Sys::BroadcastNotify(113, pBossInfo->strName.c_str(), pSceneData->name.c_str());
	}
}

void WorldBossMgr::SyncBossInfo(WSPlayer *pPlayer)
{
	Avalon::Packet	*pPacket = Avalon::Packet::Create();
	pPacket->SetID(CLProtocol::WORLD_SYNC_WORLD_BOSS);

	Avalon::NetOutputStream	stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
	try
	{
		for(WorldBossInfoMap::iterator iter = m_mapWorldBossInfo.begin(); iter != m_mapWorldBossInfo.end(); iter++)
		{
			stream & iter->first;
			
			WorldBossInfo	*pBossInfo = iter->second;
			UInt32			uNow = UInt32(CURRENT_TIME.Sec());
			if(pBossInfo->uReviveTime <= uNow)
			{
				stream & UInt32(0);
			}
			else
			{
				if (0 == pBossInfo->uFlag)
				{
					stream & (pBossInfo->uReviveTime - uNow);
				} 
				else
				{
					stream & pBossInfo->uReviveTime;
				}
			}

			if (0 == pBossInfo->uFlag)
			{
				stream & pBossInfo->uFlag;
			} 
			else
			{
				stream & UInt8(1);//客户端阵营BOSS和超级BOSS用同一个标记
			}

			stream & pBossInfo->strLastKiller;
		}

		stream & UInt32(0);
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(pPacket);
		return;
	}
	pPacket->SetSize(stream.GetPos());

	pPlayer->SendPacket(pPacket);
	Avalon::Packet::Destroy(pPacket);
}

void WorldBossMgr::TraceBoss(WSPlayer *pPlayer, UInt32 uNpcId, UInt8 uFlag)
{
	WorldBossInfoMap::iterator	iter = m_mapWorldBossInfo.find(uNpcId);
	if(iter == m_mapWorldBossInfo.end())
		return;

	WorldBossInfo	*pBossInfo = iter->second;
	if(1 == uFlag)
	{
		pBossInfo->mapPlayers[pPlayer->GetID()] = pPlayer;
	}
	else
	{
		std::map<ObjID_t, WSPlayer*>::iterator	iterPlayer = pBossInfo->mapPlayers.find(pPlayer->GetID());
		if(iterPlayer != pBossInfo->mapPlayers.end())
			pBossInfo->mapPlayers.erase(iterPlayer);
	}
}

void WorldBossMgr::CancelAllTrace(WSPlayer *pPlayer)
{
	for(WorldBossInfoMap::iterator iter = m_mapWorldBossInfo.begin(); iter != m_mapWorldBossInfo.end(); iter++)
	{
		WorldBossInfo	*pBossInfo = iter->second;
		std::map<ObjID_t, WSPlayer*>::iterator	iterPlayer = pBossInfo->mapPlayers.find(pPlayer->GetID());
		if(iterPlayer != pBossInfo->mapPlayers.end())
			pBossInfo->mapPlayers.erase(iterPlayer);
	}
}

void WorldBossMgr::OnTick(const Avalon::Time &now)
{
	if(m_minTimer.IsTimeout(now))
	{
		for(WorldBossInfoMap::iterator iter = m_mapWorldBossInfo.begin(); iter != m_mapWorldBossInfo.end(); iter++)
		{
			WorldBossInfo	*pBossInfo = iter->second;
			if(now.Sec() >= pBossInfo->uReviveTime || ((pBossInfo->uReviveTime - now.Sec()) / 60 != 5))
				continue;
			
			SceneDataEntry	*pSceneData = SceneDataEntryMgr::Instance()->FindEntry(pBossInfo->uMapId);
			if(NULL == pSceneData)
				return;

			if(0 == iter->second->uFlag)
			{
				Notify(pBossInfo, 108, pBossInfo->strName.c_str(), pSceneData->name.c_str(), pBossInfo->uMapId, iter->first);
			}
			else
			{
				Notify(pBossInfo, 116, pBossInfo->strName.c_str(), pSceneData->name.c_str(), 118, 118110);
			}

			Sys::BroadcastNotify(112, pBossInfo->strName.c_str(), pSceneData->name.c_str());
		}
	}
}

/*
// huchenhui
void WorldBossMgr::Notify(WorldBossInfo *pBossInfo, UInt32 id, ...)
{
	if(pBossInfo->mapPlayers.empty())
		return;
	
	va_list args;
	va_start(args, id);
	UInt16 type = 0;
	UInt8 color = 0;
	char buffer[CL_MAX_CHAT_LEN+1];
	bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
	va_end(args);

	if(!ret) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = buffer;

	Avalon::Packet	*pPacket = Avalon::Packet::Create();
	if(notify.Encode(pPacket))
	{
		for(std::map<ObjID_t, WSPlayer*>::iterator iterPlayer = pBossInfo->mapPlayers.begin(); 
			iterPlayer != pBossInfo->mapPlayers.end(); iterPlayer++)
		{
			iterPlayer->second->SendPacket(pPacket);
		}
	}
	Avalon::Packet::Destroy(pPacket);
}
*/

void WorldBossMgr::SendNotify(WorldBossInfo* pBossInfo, UInt16 type, UInt8 color, char *word)
{
	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;

	Avalon::Packet	*pPacket = Avalon::Packet::Create();
	if (notify.Encode(pPacket))
	{
		for (std::map<ObjID_t, WSPlayer*>::iterator iterPlayer = pBossInfo->mapPlayers.begin();
			iterPlayer != pBossInfo->mapPlayers.end(); iterPlayer++)
		{
			iterPlayer->second->SendPacket(pPacket);
		}
	}
	Avalon::Packet::Destroy(pPacket);
}

UInt32 WorldBossMgr::CalcRebirthTime(UInt32 npcId)
{
	Avalon::Date date(CURRENT_TIME);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	UInt32 dayStartTime = UInt32(date.ToTime().Sec());

	RefreshTimeMap::iterator iter1= m_RefreshTimeMap.find(npcId);
	if(iter1 == m_RefreshTimeMap.end()) return 0;

	UInt32 curTime = UInt32(CURRENT_DATE.Hour() * HOUR_TO_SEC + CURRENT_DATE.Min() * MIN_TO_SEC + CURRENT_DATE.Sec());
	RefreshTimeList::iterator iter2 = iter1->second.begin();
	for (; iter2 != iter1->second.end(); ++iter2)
	{
		if(curTime < (*iter2)) break;
	}

	if(iter2 == iter1->second.end()) return (dayStartTime + DAY_TO_SEC + *(iter1->second.begin()));
	return dayStartTime + *iter2;
}

