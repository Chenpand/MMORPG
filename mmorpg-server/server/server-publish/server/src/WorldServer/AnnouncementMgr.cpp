#include "AnnouncementMgr.h"

#include <CLRecordClient.h>
#include "WSSelectInitDataCallback.h"
#include "GameParamMgr.h"

class SelectAnnouncementCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return AnnouncementMgr::Instance()->OnSelectDataRet(this);
	}
};

AnnouncementMgr::AnnouncementMgr()
{
}

AnnouncementMgr::~AnnouncementMgr()
{
	m_Announcements.clear();
}

void AnnouncementMgr::Init()
{
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_announcement");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectAnnouncementCallback());
}

void AnnouncementMgr::Final()
{
	m_Announcements.clear();
}


UInt64 AnnouncementMgr::SaveAnnouncement(UInt32 id, UInt32 begintime, UInt32 interval, UInt32 times, std::string content, bool isSave, UInt32 gmId)
{
	UInt32 now = UInt32(CURRENT_TIME.Sec());
	UInt32 playCount = 0;
	if (times == 0 || now >= begintime + times * interval)
	{
		return 0;
	}
	if (interval == 0) interval = 1;
	if (now > begintime)
	{
		playCount = (now - begintime) / interval;
	}

	Announcement entry;
	entry.id = CLRecordClient::Instance()->GenGuid();
	entry.dataId = id;
	entry.content = content;
	entry.beginTime = begintime;
	entry.repeattimes = times;
	entry.interval = interval;
	entry.times = playCount;
	entry.gmid = gmId;

	m_Announcements.push_back(entry);

	if (isSave)
	{
		CLInsertCommand* insertcmd = CLRecordClient::Instance()->PrepareInsertCommand("t_announcement", entry.id);
		insertcmd->PutData("dataid", entry.dataId);
		insertcmd->PutData("content", entry.content);
		insertcmd->PutData("begintime", entry.beginTime);
		insertcmd->PutData("interval", entry.interval);
		insertcmd->PutData("repeattimes", entry.repeattimes);
		insertcmd->PutData("color", 0);
		insertcmd->PutData("showzone", 0);
		insertcmd->PutData("gmid", entry.gmid);
		CLRecordClient::Instance()->SendCommand(insertcmd);
	}
	return entry.id;
}

void AnnouncementMgr::RemoveAnnouncement(ObjID_t id)
{
	if(id == 0) return;
	for(AnnouncementVec::iterator iter = m_Announcements.begin(); iter != m_Announcements.end(); ++iter)
	{
		if(iter->id == id)
		{
			DeleteAnnounce(id);
			m_Announcements.erase(iter);
			return;
		}
	}
}


void AnnouncementMgr::RemoveAnnouncement(UInt32 dataId)
{
	AnnouncementVec::iterator iter = m_Announcements.begin();
	while (iter != m_Announcements.end())
	{
		if (iter->dataId == dataId)
		{
			DeleteAnnounce(iter->id);
			iter = m_Announcements.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AnnouncementMgr::RemoveCustomAnnouncement(UInt32 gmId)
{
	AnnouncementVec::iterator iter = m_Announcements.begin();
	while (iter != m_Announcements.end())
	{
		if (iter->gmid == gmId)
		{
			DeleteAnnounce(iter->id);
			iter = m_Announcements.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AnnouncementMgr::VisitAnnouncements(AnnouncementVisitor& visitor)
{
	for(AnnouncementVec::iterator iter = m_Announcements.begin(); iter != m_Announcements.end(); ++iter)
	{
		if(!visitor(&(*iter))) return;
	}
}

void AnnouncementMgr::Announce(Announcement entry)
{
	CLProtocol::SysAnnouncement info;
	info.id = entry.dataId;
	info.word = entry.content;
	Router::BroadcastToPlayer(info);

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(2017, buffer, sizeof(buffer));
	std::string strSenderName(buffer);

	CLProtocol::SceneSyncChat protocol;
	protocol.channel = CHAT_CHANNEL_SYSTEM;
	protocol.objid = 0;
	protocol.objname = strSenderName;
	protocol.word = entry.content;
	protocol.bLink = true;
	protocol.isGm = 0;
	protocol.voiceKey = "";
	protocol.voiceDuration = 0;
	Router::BroadcastToPlayer(protocol);
}

bool AnnouncementMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	UInt32 now = UInt32(CURRENT_TIME.Sec());
	while(callback->NextRow())
	{
		Announcement entry;
		entry.id = callback->GetKey();
		entry.dataId = callback->GetData("dataid");
		entry.content = callback->GetData("content").GetString();
		entry.beginTime = callback->GetData("begintime");
		entry.interval = callback->GetData("interval");
		entry.repeattimes = callback->GetData("repeattimes");
		entry.gmid = callback->GetData("gmid");

		if(entry.repeattimes == 0 || now >= entry.beginTime + (entry.repeattimes - 1) * entry.interval)
		{
			DeleteAnnounce(entry.id);
			continue;
		}

		if(now > entry.beginTime)
		{
			if(entry.interval == 0) entry.interval = 1;
			entry.times = 1 + (now - entry.beginTime) / entry.interval;
		}

		m_Announcements.push_back(entry);
	}
	return true;
}

void AnnouncementMgr::OnTick(const Avalon::Time& now)
{
	for(AnnouncementVec::iterator iter = m_Announcements.begin(); iter != m_Announcements.end();)
	{
		Announcement& entry = *iter;

		if(now.Sec() >= entry.beginTime + entry.times * entry.interval)
		{
			Announce(entry);

			++entry.times;
			if(entry.times >= entry.repeattimes)
			{
				DeleteAnnounce(entry.id);
				iter = m_Announcements.erase(iter);
				continue;
			}
		}
		++iter;
	}
}

void AnnouncementMgr::DeleteAnnounce(ObjID_t id)
{
	if(id == 0) return;
	CLDeleteCommand* deletecmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_announcement", id);
	CLRecordClient::Instance()->SendCommand(deletecmd);
}


