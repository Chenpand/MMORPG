#ifndef _ANNOUNCEMENT_MGR_H_
#define _ANNOUNCEMENT_MGR_H_

#include <AvalonSingleton.h>
#include <CLRecordCallback.h>
#include <CLObject.h>
#include <string>
#include <CLErrorCode.h>
#include "WorldSysNotify.h"
#include <CLAnnouncementDataEntry.h>

#include "GuildMgr.h"

/**
 *@brief 公告信息
 */
struct Announcement
{
	Announcement():id(0), dataId(0), beginTime(0), times(0), interval(0), repeattimes(0), gmid(0){}

	ObjID_t		id;
	UInt32		dataId;			//数据表ID
	std::string	content;		//内容
	UInt32		beginTime;		//公告开始时间
	UInt32		times;			//已公告次数
	UInt32		interval;		//公告间隔
	UInt32		repeattimes;	//重复次数
	UInt32		gmid;			//gm系统的ID,用于删除公告
};

typedef CLVisitor<Announcement> AnnouncementVisitor;

/**
 *@brief 公告管理器
 */
class AnnouncementMgr : public Avalon::Singleton<AnnouncementMgr>
{
	typedef std::vector<Announcement> AnnouncementVec;

public:
	AnnouncementMgr();
	~AnnouncementMgr();

public:
	void Init();
	void Final();

public:

	/**
	*@brief 发布公告
	*/
	UInt64 SaveAnnouncement(UInt32 id, UInt32 begintime, UInt32 interval, UInt32 times, std::string content, bool isSave = true, UInt32 gmId = 0);

	/**
	*@brief 发布公告
	*/
	template <typename... Args>
	void PublicAnnouncement(UInt64 guildId, UInt32 id, UInt32 begintime, const Args & ... args)
	{
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;
		 
		char buffer[CL_MAX_CHAT_LEN + 1];
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		SaveAnnouncement(id, begintime, dataEntry->interval, dataEntry->times, std::string(buffer));

		// 发送到公会日志
		if ((dataEntry->SyncEventBit & AnnouncementSyncEvent::SyncGuildLog) && guildId != 0)
		{
			Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
			if (guild != NULL)
			{
				guild->AddGuildEvent(std::string(buffer));
			}
		}
	}

	/**
	*@brief 发布公告
	*/
	template <typename... Args>
	void PublicAnnouncementList(UInt64 guildId, UInt32 id, UInt32 begintime, UInt32 durationTime, UInt32 intervalTime, const Args & ... args)
	{
		if (durationTime == 0 || intervalTime == 0) return;
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;

		char buffer[CL_MAX_CHAT_LEN + 1];
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		UInt32 startTime = begintime;
		UInt32 endTime = startTime + durationTime;
		while (startTime < endTime)
		{
			SaveAnnouncement(id, startTime, dataEntry->interval, dataEntry->times, std::string(buffer));
			startTime += intervalTime;
		}

		// 发送到公会日志
		if ((dataEntry->SyncEventBit & AnnouncementSyncEvent::SyncGuildLog) && guildId != 0)
		{
			Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
			if (guild != NULL)
			{
				guild->AddGuildEvent(std::string(buffer));
			}
		}
	}

	/**
	*@brief 发布自定义公告
	*/
	template <typename... Args>
	UInt64 PublicCustomAnnouncement(UInt32 id, UInt32 begintime, UInt32 interval, UInt32 times, UInt32 gmId, const Args & ... args)
	{
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return 0;

		char buffer[CL_MAX_CHAT_LEN + 1];
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return 0;

		return SaveAnnouncement(id, begintime, interval, times, std::string(buffer), true, gmId);
	}

	/**
	*@brief 发布活动公告
	*/
	static void PublicActivityAnnouncement(UInt32 id, UInt32 begintime, UInt32 interval, UInt32 times, bool isSave = false)
	{
		std::string content = SysNotifyMgr::Instance()->MakeString(id);
		AnnouncementMgr::Instance()->SaveAnnouncement(id, begintime, interval, times, content, isSave);
	}

	/**
	 *@brief 移除公告
	 */
	void RemoveAnnouncement(ObjID_t id);
	void RemoveAnnouncement(UInt32 dataId);
	void RemoveCustomAnnouncement(UInt32 gmId);

	/**
	 *@brief 访问所有公告
	 */
	void VisitAnnouncements(AnnouncementVisitor& visitor);
	
	/**
	 *@brief 获取公告数
	 */
	UInt32 GetAnnouncementNum() const{ return m_Announcements.size(); }


public://事件
	/** 
	 *@brief 查询数据返回
	 */
	bool OnSelectDataRet(CLRecordCallback* callback);

	/**
	 *@brief 心跳事件
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief 公告
	 */
	void Announce(Announcement entry);

	/**
	 *@brief 删除公告
	 */
	void DeleteAnnounce(ObjID_t id);

private:
	//公告
	AnnouncementVec	m_Announcements;
};

#endif
