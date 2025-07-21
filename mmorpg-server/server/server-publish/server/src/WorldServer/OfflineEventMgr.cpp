#include "OfflineEventMgr.h"

#include <CLRecordClient.h>
#include "WSPlayer.h"





/**
 *@brief 查询离线事件返回
 */
class SelectOfflineEventsCallback : public SelectRoleInfoCallback
{
public:
	SelectOfflineEventsCallback(WSPlayer* player):SelectRoleInfoCallback(player){}
	~SelectOfflineEventsCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->GetOfflineEventMgr().OnLoadDataRet(this);
	}

	void OnFailed(WSPlayer* player)
	{
		player->GetOfflineEventMgr().OnLoadDataRet(this);
	}
};


OfflineEventMgr::OfflineEventMgr()
{
	m_pOwner = NULL;
}

OfflineEventMgr::~OfflineEventMgr()
{
	for(OfflineEventVec::iterator iter = m_OfflineEvents.begin();
		iter != m_OfflineEvents.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	m_OfflineEvents.clear();
}

void OfflineEventMgr::LoadEvents()
{
	if(m_pOwner->GetSrcZoneId() != 0) return;

	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_offline_event");
	cmd->PutCond("owner",m_pOwner->GetID());
	CLRecordClient::Instance()->SendCommand(cmd, new SelectOfflineEventsCallback(m_pOwner));
}

void OfflineEventMgr::PutEvent(OfflineEvent* offlineevent)
{ 
	m_OfflineEvents.push_back(offlineevent);
}

void OfflineEventMgr::OnOnline()
{
	bool bDel = false;
	for(OfflineEventVec::iterator iter = m_OfflineEvents.begin();
		iter != m_OfflineEvents.end(); ++iter)
	{
		(*iter)->OnTriggered(m_pOwner);
		CL_SAFE_DELETE(*iter);
		bDel = true;
	}
	m_OfflineEvents.clear();

	if(bDel)
	{
		std::ostringstream oss;
		oss << "`owner`='" << m_pOwner->GetID() << "'";
		CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_offline_event", oss.str());
		CLRecordClient::Instance()->SendCommand(cmd);

		GlobalOfflineEventMgr::Instance()->DelOfflineEvent(m_pOwner->GetID());
		
	}
}

void OfflineEventMgr::OnLoadDataRet(CLRecordCallback* callback)
{
	while(callback->NextRow())
	{
		UInt16 type = callback->GetData("type");

		OfflineEvent* offline_event = OfflineEvent::Create(type);
		if (offline_event == NULL) continue;

		offline_event->SetDBData(callback);
		m_OfflineEvents.push_back(offline_event);
	}
}

GlobalOfflineEventMgr::GlobalOfflineEventMgr()
{

}

GlobalOfflineEventMgr::~GlobalOfflineEventMgr()
{
	for (size_t i = 0; i < m_EventsPtr.size(); ++i)
	{
		if (m_EventsPtr[i])
		{
			CL_SAFE_DELETE(m_EventsPtr[i]);
		}
	}

	m_EventsPtr.clear();
	m_OfflineEvents.clear();
}

GlobalOfflineEventQueryType GlobalOfflineEventMgr::FindOfflineEvent(ObjID_t uid, UInt16 type, std::vector<OfflineEvent*>& vec)
{
	OfflineEventMap::iterator itr = m_OfflineEvents.find(uid);
	if (itr == m_OfflineEvents.end())
	{
		return GOEQT_NO_CACHE;
	}
	else
	{
		OfflineType2Event& typeEvents = itr->second;
		if (typeEvents.size() == 0)
		{
			return GOEQT_NO_EVENT;
		}

		OfflineType2Event::iterator typeItr = typeEvents.find(type);
		if (typeItr == typeEvents.end())
		{
			return GOEQT_NO_EVENT;
		}
		
		OfflineEventVec& eventVec = typeItr->second;
		if (eventVec.size() > 0)
		{
			vec = typeItr->second;
			return GOEQT_HAS_EVENT;
		}
		else
		{
			return GOEQT_NO_EVENT;
		}
	}
}

void GlobalOfflineEventMgr::SelectEvent(ObjID_t uid, CLRecordCallback* callback)
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_offline_event");
	cmd->PutCond("owner", uid);
	CLRecordClient::Instance()->SendCommand(cmd, callback);
}

void GlobalOfflineEventMgr::AddOfflineEvent(ObjID_t uid, OfflineEvent* event)
{
	if (0 == uid)
	{
		return;
	}
	
	OfflineType2Event& typeMap = m_OfflineEvents[uid];

	if (!event)
	{
		return;
	}

	event->SetTarUid(uid);

	OfflineEventVec& vec = typeMap[event->GetType()];
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (vec[i] == event)
		{
			return;
		}
	}
	vec.push_back(event);
	m_EventsPtr = vec;
}

void GlobalOfflineEventMgr::DelOfflineEvent(ObjID_t uid)
{
	OfflineEventMap::iterator itr = m_OfflineEvents.find(uid);
	if (itr != m_OfflineEvents.end())
	{
		OfflineType2Event& typeEvents = itr->second;
		typeEvents.clear();
		m_OfflineEvents.erase(itr);
		return;
	}
	
}
