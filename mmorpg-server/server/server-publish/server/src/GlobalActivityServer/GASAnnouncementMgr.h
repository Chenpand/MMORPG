#ifndef _ANNOUNCEMENT_MGR_H_
#define _ANNOUNCEMENT_MGR_H_

#include <AvalonSingleton.h>
#include <CLObject.h>
#include <CLErrorCode.h>
#include <CLAnnouncementDataEntry.h>
#include <CLSysNotify.h>
#include <CLChatDefine.h>
#include <CLGlobalActivityProtocol.h>
#include <AvalonSimpleTimer.h>

#include "GASPlayerMgr.h"
#include "GASGameZoneMgr.h"


/**
 *@brief 公告管理器
 */
class GASAnnouncementMgr : public Avalon::Singleton<GASAnnouncementMgr>
{
public:
	GASAnnouncementMgr();
	~GASAnnouncementMgr();

public:
	void Init();
	void Final();

	void OnTick(const Avalon::Time &now);

	void AddAnnouncement(AnnouncementData* data);

public:

	/**
	*@brief 发布公告
	*/
	void SendAnnouncement(AnnouncementData* data);

	/**
	*@brief 发布公告
	*/
	template <typename... Args>
	void PublicAnnouncement(bool isDelay, UInt32 id, UInt32 begintime, const Args & ... args)
	{
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;

		char buffer[CL_MAX_CHAT_LEN + 1] = { 0 };
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		AnnouncementData* data = new AnnouncementData();
		data->id = id;
		data->begintime = begintime;
		data->interval = dataEntry->interval;
		data->times = dataEntry->times;
		data->content = buffer;
		data->isSave = 1;
		data->gmId = 0;

		if (isDelay)
		{
			AddAnnouncement(data);
		}
		else
		{
			SendAnnouncement(data);
			delete data;
		}
	}

	/**
	*@brief 发布公告
	*/
	template <typename... Args>
	void PublicAnnouncementList(bool isDelay, UInt32 id, UInt32 begintime, UInt32 durationTime, UInt32 intervalTime, const Args & ... args)
	{
		if (durationTime == 0 || intervalTime == 0) return;
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;

		char buffer[CL_MAX_CHAT_LEN + 1] = { 0 };
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		CLProtocol::GASSendAnnounceReq req;

		UInt32 startTime = begintime;
		UInt32 endTime = startTime + durationTime;

		while (startTime < endTime)
		{
			AnnouncementData* data = new AnnouncementData();
			data->id = id;
			data->begintime = startTime;
			data->interval = dataEntry->interval;
			data->times = dataEntry->times;
			data->content = buffer;
			data->isSave = 1;
			data->gmId = 0;

			if (isDelay)
			{
				AddAnnouncement(data);
			}
			else
			{
				req.announcements.push_back(*data);
				delete(data);
			}

			startTime += intervalTime;
		}

		if (!req.announcements.empty())
		{
			GASGameZoneMgr::Instance()->BroadcastToWorld(req);
		}
	}

private:
	std::vector<AnnouncementData*> m_AnnouncementData;

	// 每分钟定时器
	Avalon::SimpleTimer m_PerMinTimer;
};

#endif
