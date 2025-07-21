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
 *@brief ������Ϣ
 */
struct Announcement
{
	Announcement():id(0), dataId(0), beginTime(0), times(0), interval(0), repeattimes(0), gmid(0){}

	ObjID_t		id;
	UInt32		dataId;			//���ݱ�ID
	std::string	content;		//����
	UInt32		beginTime;		//���濪ʼʱ��
	UInt32		times;			//�ѹ������
	UInt32		interval;		//������
	UInt32		repeattimes;	//�ظ�����
	UInt32		gmid;			//gmϵͳ��ID,����ɾ������
};

typedef CLVisitor<Announcement> AnnouncementVisitor;

/**
 *@brief ���������
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
	*@brief ��������
	*/
	UInt64 SaveAnnouncement(UInt32 id, UInt32 begintime, UInt32 interval, UInt32 times, std::string content, bool isSave = true, UInt32 gmId = 0);

	/**
	*@brief ��������
	*/
	template <typename... Args>
	void PublicAnnouncement(UInt64 guildId, UInt32 id, UInt32 begintime, const Args & ... args)
	{
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;
		 
		char buffer[CL_MAX_CHAT_LEN + 1];
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		SaveAnnouncement(id, begintime, dataEntry->interval, dataEntry->times, std::string(buffer));

		// ���͵�������־
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
	*@brief ��������
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

		// ���͵�������־
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
	*@brief �����Զ��幫��
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
	*@brief ���������
	*/
	static void PublicActivityAnnouncement(UInt32 id, UInt32 begintime, UInt32 interval, UInt32 times, bool isSave = false)
	{
		std::string content = SysNotifyMgr::Instance()->MakeString(id);
		AnnouncementMgr::Instance()->SaveAnnouncement(id, begintime, interval, times, content, isSave);
	}

	/**
	 *@brief �Ƴ�����
	 */
	void RemoveAnnouncement(ObjID_t id);
	void RemoveAnnouncement(UInt32 dataId);
	void RemoveCustomAnnouncement(UInt32 gmId);

	/**
	 *@brief �������й���
	 */
	void VisitAnnouncements(AnnouncementVisitor& visitor);
	
	/**
	 *@brief ��ȡ������
	 */
	UInt32 GetAnnouncementNum() const{ return m_Announcements.size(); }


public://�¼�
	/** 
	 *@brief ��ѯ���ݷ���
	 */
	bool OnSelectDataRet(CLRecordCallback* callback);

	/**
	 *@brief �����¼�
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief ����
	 */
	void Announce(Announcement entry);

	/**
	 *@brief ɾ������
	 */
	void DeleteAnnounce(ObjID_t id);

private:
	//����
	AnnouncementVec	m_Announcements;
};

#endif
