#include "GASAnnouncementMgr.h"

#include <CLAnnouncementDefine.h>
#include <CLGlobalActivityProtocol.h>

#include "GASGameZoneMgr.h"


GASAnnouncementMgr::GASAnnouncementMgr()
{
	m_PerMinTimer.SetInterval(MIN_TO_SEC * SEC_TO_MSEC);
}

GASAnnouncementMgr::~GASAnnouncementMgr()
{
}

void GASAnnouncementMgr::Final()
{
	for (auto data : m_AnnouncementData)
	{
		delete data;
	}
	m_AnnouncementData.clear();
}

void GASAnnouncementMgr::OnTick(const Avalon::Time &now)
{
	for (auto data : m_AnnouncementData)
	{
		if (!data) continue;

		if ((UInt32)now.Sec() >= data->begintime)
		{
			SendAnnouncement(data);
			data->begintime = UINT32_MAX;
		}
	}

	if (m_PerMinTimer.IsTimeout(now))
	{
		for (auto iter = m_AnnouncementData.begin(); iter != m_AnnouncementData.end();)
		{
			auto data = *iter;
			if (data != NULL && data->begintime == UINT32_MAX)
			{
				delete data;
				iter = m_AnnouncementData.erase(iter);
				
				continue;
			}

			++iter;
		}
	}
}

void GASAnnouncementMgr::AddAnnouncement(AnnouncementData* data)
{
	m_AnnouncementData.push_back(data);
}

void GASAnnouncementMgr::SendAnnouncement(AnnouncementData* data)
{
	if (!data) return;

	CLProtocol::GASSendAnnounceReq req;
	req.announcements.push_back(*data);
	GASGameZoneMgr::Instance()->BroadcastToWorld(req);
}