#include "PunishmentMgr.h"

#include <CLRecordClient.h>
#include <CLSyncObjectProtocol.h>
#include <CLCenterProtocol.h>
#include <CLGameDefine.h>
#include <CLCenterProtocol.h>

#include "WSSelectInitDataCallback.h"
#include "WSPlayerMgr.h"
#include "WSRouter.h"

class SelectPunishmentCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return PunishmentMgr::Instance()->OnSelectDataRet(this);
	}
};

PunishmentMgr::PunishmentMgr()
{
	m_RequestTimer.SetInterval(30 * 1000);
	m_bResponse = false;
}

PunishmentMgr::~PunishmentMgr()
{
}

void PunishmentMgr::Init()
{
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_punishment");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectPunishmentCallback());
}

void PunishmentMgr::Final()
{
	m_Punishments.clear();
}

void PunishmentMgr::Punish(UInt32 accid, int type, UInt32 time, UInt32 param)
{
	if(accid == 0) return;
	if(type == 0 || type >= PUNISH_MAX) return;
	if(time == 0 || time >= MAX_PUNISH_TIME) time = MAX_PUNISH_TIME;

	UInt32 dueTime = UInt32(CURRENT_TIME.Sec()) + time;
	PunishmentVec& punishments = m_Punishments[accid];
	for(PunishmentVec::iterator	iter = punishments.begin();
		iter != punishments.end(); ++iter)
	{
		if(iter->type == type)
		{
			if(dueTime > iter->duetime)
			{
				iter->duetime = dueTime;
				iter->param = param;
				CLUpdateCommand* updatecmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_punishment",iter->id,true);
				updatecmd->PutData("duetime", iter->duetime);
				updatecmd->PutData("param", iter->param);
				CLRecordClient::Instance()->SendCommand(updatecmd);
				SyncPunishment(accid);
			}
			return;
		}
	}

	Punishment entry;
	entry.id = CLRecordClient::Instance()->GenGuid();
	entry.accid = accid;
	entry.type = type;
	entry.param = param;
	entry.duetime = dueTime;

	CLInsertCommand* insertcmd = CLRecordClient::Instance()->PrepareInsertCommand("t_punishment", entry.id);
	insertcmd->PutData("accid",		accid);
	insertcmd->PutData("type",		type);
	insertcmd->PutData("duetime",	dueTime);
	insertcmd->PutData("param", param);
	CLRecordClient::Instance()->SendCommand(insertcmd);

	punishments.push_back(entry);
	SyncPunishment(accid);
}

void PunishmentMgr::UnPunish(UInt32 accid, int type)
{
	PunishmentMap::iterator iter = m_Punishments.find(accid);
	if(iter == m_Punishments.end()) return;
	
	for (UInt32 i = 0; i < iter->second.size();)
	{
		auto& punish = iter->second[i];
		if (punish.type == type)
		{
			DeletePunishment(punish.id);
			iter->second.erase(iter->second.begin() + i);
			if (iter->second.empty())
			{
				m_Punishments.erase(iter);
				return;
			}
			SyncPunishment(accid);
		}
		else
		{
			i++;
		}
	}
}

void PunishmentMgr::SetAllForbidTalkList(const std::vector<UInt32>& players)
{
	m_ALLForbidTalkList.insert(players.begin(), players.end());
	for(std::set<UInt32>::const_iterator iter = m_ALLForbidTalkList.begin();
		iter != m_ALLForbidTalkList.end(); ++iter)
	{
		SyncPunishment(*iter);
	}
	m_bResponse = true;
}

void PunishmentMgr::AddAllForbidTalkPlayer(UInt32 accid)
{
	if(m_ALLForbidTalkList.find(accid) == m_ALLForbidTalkList.end())
	{
		m_ALLForbidTalkList.insert(accid);
		SyncPunishment(accid);
	}
}

void PunishmentMgr::DelAllForbidTalkPlayer(UInt32 accid)
{
	std::set<UInt32>::iterator iter = m_ALLForbidTalkList.find(accid);
	if(iter != m_ALLForbidTalkList.end())
	{
		m_ALLForbidTalkList.erase(iter);
		SyncPunishment(accid);
	}
}

UInt32 PunishmentMgr::GetPunishRemainTime(UInt32 accid, UInt8 type)
{
	UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
	PunishmentMap::const_iterator iter1 = m_Punishments.find(accid);
	if (iter1 != m_Punishments.end())
	{
		for (PunishmentVec::const_iterator subiter = iter1->second.begin();
			subiter != iter1->second.end(); ++subiter)
		{
			if (subiter->type == type)
			{
				return subiter->duetime < curSec ? 0 : subiter->duetime - curSec;
			}
		}
	}

	return 0;
}

ForbidLoginReason PunishmentMgr::GetForbidLoginReason(UInt32 accid)
{
	PunishmentMap::const_iterator iter1 = m_Punishments.find(accid);
	if (iter1 != m_Punishments.end())
	{
		for (PunishmentVec::const_iterator subiter = iter1->second.begin();
			subiter != iter1->second.end(); ++subiter)
		{
			if (subiter->type == PUNISH_FORBIDLOGIN)
			{
				return (ForbidLoginReason)subiter->param;
			}
		}
	}

	return FORBID_LOGIN_COMMON;
}

bool PunishmentMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	while(callback->NextRow())
	{
		Punishment entry;
		entry.id = callback->GetKey();
		entry.accid = callback->GetData("accid");
		entry.type = callback->GetData("type");
		entry.duetime = callback->GetData("duetime");
		entry.param = callback->GetData("param");
		m_Punishments[entry.accid].push_back(entry);
	}
	return true;
}

void PunishmentMgr::OnTick(const Avalon::Time& now)
{
	if(!m_bResponse)
	{
		if(m_RequestTimer.IsTimeout(now))
		{
			CLProtocol::CenterForbidTalkListReq req;
			Router::SendToCenter(req);
		}
	}

	for(PunishmentMap::iterator iter = m_Punishments.begin();
		iter != m_Punishments.end();)
	{
		bool bChanged = false;
		for(PunishmentVec::iterator subiter = iter->second.begin();
			subiter != iter->second.end();)
		{
			if(now.Sec() > subiter->duetime)
			{
				bChanged = true;
				DeletePunishment(subiter->id);
				subiter = iter->second.erase(subiter);
			}
			else
			{
				++subiter;
			}
		}
		if(bChanged)
		{
			SyncPunishment(iter->first);
		}

		if(iter->second.empty())
		{
			m_Punishments.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void PunishmentMgr::OnPlayerOnline(WSPlayer* player)
{
	SyncPunishment(player->GetAccId());
}

void PunishmentMgr::OnServerChanged(WSPlayer* player)
{
	SyncPunishment(player->GetAccId());
}

void PunishmentMgr::OnReportPlayerCheat(WSPlayer* player, PlayerCheatReason reason, UInt32 param1, UInt32 param2)
{
	if (!player)
	{
		return;
	}

	CLProtocol::CenterReportPlayerCheat report;
	report.zoneid = ZONE_ID;
	report.platform = player->GetPf();
	report.openid = player->GetOpenId();
	report.accid = player->GetAccId();
	report.roleId = player->GetID();
	report.name = player->GetName();
	report.level = player->GetLevel();
	report.occu = player->GetOccu();
	report.roleCreateTime = player->GetCreateTime();
	report.loginTime = player->GetOnlineTime();
	report.ip = player->GetIp();
	report.isForbidLogin = PunishmentMgr::Instance()->GetPunishRemainTime(player->GetAccId(), PUNISH_FORBIDLOGIN) > 0 ? 1 : 0;
	report.vipLevel = player->GetVipLvl();
	report.totalCharge = player->GetTotalChargeNum();
	report.time = (UInt32)CURRENT_TIME.Sec();
	report.reason = reason;
	report.param1 = param1;
	report.param2 = param2;

	Router::SendToCenter(report);
}

void PunishmentMgr::SyncPunishment(UInt32 accid) const
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(accid);
	if(player == NULL || player->GetGameStatus() != PS_NORMAL) return;
	
	UInt32 punishFlag = 0;
	UInt32 forbitTalkDueTime = 0;
	UInt32 forbitLoginDueTime = 0;
	PunishmentMap::const_iterator iter1 = m_Punishments.find(accid);
	if(iter1 != m_Punishments.end())
	{
		for(PunishmentVec::const_iterator subiter = iter1->second.begin();
			subiter != iter1->second.end();++subiter)
		{
			punishFlag |= (1 << subiter->type);
			if(subiter->type == PUNISH_FORBIDTALK)
			{
				forbitTalkDueTime = subiter->duetime;
			}
			else if(subiter->type == PUNISH_FORBIDLOGIN)
			{
				forbitLoginDueTime = subiter->duetime;
			}
		}
	}
	std::set<UInt32>::const_iterator iter2 = m_ALLForbidTalkList.find(accid);
	if(iter2 != m_ALLForbidTalkList.end())
	{
		punishFlag |= (1 << PUNISH_FORBIDTALK);
		if(UInt32(CURRENT_TIME.Sec()) + 30 * DAY_TO_SEC > forbitTalkDueTime)
		{
			forbitTalkDueTime = UInt32(CURRENT_TIME.Sec()) + 30 * DAY_TO_SEC;
		}
	}
	if(player->GetPunishFlag() == 0 && punishFlag == 0) return;

	CLProtocol::SceneSyncPunishment sync;
	sync.id = player->GetID();
	sync.punishflag = punishFlag;
	sync.forbitTalkDueTime = forbitTalkDueTime;
	player->SendToScene(sync);

	player->SetPunishFlag(punishFlag);
	player->SetForbitTalkDueTime(forbitTalkDueTime);
	player->SetForbitLoginDueTime(forbitLoginDueTime);
}

void PunishmentMgr::DeletePunishment(ObjID_t id)
{
	CLDeleteCommand* deletecmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_punishment", id);
	CLRecordClient::Instance()->SendCommand(deletecmd);
}
