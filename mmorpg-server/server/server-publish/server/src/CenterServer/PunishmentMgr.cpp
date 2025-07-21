#include "PunishmentMgr.h"

#include <AvalonResultSet.h>
#include <CLCenterProtocol.h>
#include "CSApplication.h"
#include "GameZoneMgr.h"

PunishmentMgr::PunishmentMgr()
{
}

PunishmentMgr::~PunishmentMgr()
{
}

bool PunishmentMgr::Init()
{
	Avalon::RetryDBConnection* dbconn = CSApplication::Instance()->GetDBConnection();
	Avalon::ResultSet* result = dbconn->Query("SELECT * FROM `t_g_punishment`");
	if(result == NULL) return false;
	while(result->NextRow())
	{
		UInt32 accid = result->GetField("accid").GetUInt32();
		UInt32 dueTime = result->GetField("duetime").GetUInt32();

		m_PunishmentMap.insert(std::make_pair(accid, dueTime));
	}
	dbconn->FreeResultSet(result);

	return true;
}

void PunishmentMgr::Final()
{
}

void PunishmentMgr::ForbitTalk(UInt32 accid)
{
	if(accid == 0) return;
	UInt32 dueTime = UInt32(CURRENT_TIME.Sec()) + FORBITTALK_TIME;
	PlayerMap::iterator iter = m_PunishmentMap.find(accid);
	if(iter != m_PunishmentMap.end())
	{
		iter->second = dueTime;
		if(CSApplication::Instance()->GetDBConnection()->UpdateFmt("UPDATE `t_g_punishment` set `duetime`=%u where `accid`=%u", iter->second, iter->first) < 0)
		{
			ErrorLogStream << "execute database statement failed!" << CSApplication::Instance()->GetDBConnection()->GetError() << LogStream::eos;
			CSApplication::Instance()->Exit();
			return;
		}
	}
	else
	{
		m_PunishmentMap.insert(std::make_pair(accid, dueTime));
		if(CSApplication::Instance()->GetDBConnection()->UpdateFmt("INSERT INTO `t_g_punishment`(`accid`,`duetime`) VALUES (%u, %u)", accid, dueTime) < 0)
		{
			ErrorLogStream << "execute database statement failed!" << CSApplication::Instance()->GetDBConnection()->GetError() << LogStream::eos;
			CSApplication::Instance()->Exit();
			return;
		}
		OnPunished(accid);
	}
}

void PunishmentMgr::UnForbitTalk(UInt32 accid)
{
	PlayerMap::iterator iter = m_PunishmentMap.find(accid);
	if(iter == m_PunishmentMap.end()) return;

	OnUnPunished(accid);
	DeleteForbidTalk(accid);
	m_PunishmentMap.erase(iter);
}

void PunishmentMgr::OnTick(const Avalon::Time& now)
{
	for(PlayerMap::iterator iter = m_PunishmentMap.begin();
		iter != m_PunishmentMap.end();)
	{
		if(UInt32(CURRENT_TIME.Sec()) > iter->second)
		{
			OnUnPunished(iter->first);
			DeleteForbidTalk(iter->first);
			m_PunishmentMap.erase(iter++);
		}
		else ++iter;
	}
}

void PunishmentMgr::OnPunished(UInt32 accid)
{
	CLProtocol::CenterNotifyNewForbidTalk notify;
	notify.accid = accid;
	GameZoneMgr::Instance()->BroadcastToGameZone(notify);
}

void PunishmentMgr::OnUnPunished(UInt32 accid)
{
	CLProtocol::CenterNotifyDelForbidTalk notify;
	notify.accid = accid;
	GameZoneMgr::Instance()->BroadcastToGameZone(notify);
}

void PunishmentMgr::SendForbidList(GameZone* zone)
{
	if(zone == NULL) return;

	CLProtocol::CenterForbidTalkListRet ret;
	for(PlayerMap::iterator iter = m_PunishmentMap.begin();
		iter != m_PunishmentMap.end(); ++iter)
	{
		ret.players.push_back(iter->first);
		if(ret.players.size() >= 10000)
		{
			zone->SendProtocol(ret);
			ret.players.clear();
		}
	}
	zone->SendProtocol(ret);
}

void PunishmentMgr::OnGameZoneConnected(GameZone* zone)
{
}

void PunishmentMgr::DeleteForbidTalk(UInt32 accid)
{
	if(CSApplication::Instance()->GetDBConnection()->UpdateFmt("DELETE FROM `t_g_punishment` where `accid`=%u", accid) < 0)
	{
		ErrorLogStream << "execute database statement failed!" << CSApplication::Instance()->GetDBConnection()->GetError() << LogStream::eos;
		CSApplication::Instance()->Exit();
		return;
	}
}
