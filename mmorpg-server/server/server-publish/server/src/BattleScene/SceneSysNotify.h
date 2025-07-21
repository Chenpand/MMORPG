#ifndef _SCENE_SYS_NOTIFY_H_
#define _SCENE_SYS_NOTIFY_H_

#include <CLDefine.h>
#include <CLSysNotify.h>
#include <CLChatProtocol.h>
#include <CLMailProtocol.h>
#include <CLAnnouncementDataEntry.h>
#include <kingnet_analyzer.h>

#include "SSRouter.h"

class Item;
class RewardGroup;

namespace Sys
{
	/**
	 *@brief 发送一个公告
	 */
	// by huchenhui
	//void SendNotify(ObjID_t playerId, UInt32 id, ...)
	template <typename... Args>
	void SendNotify(ObjID_t playerId, UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		Router::TransmitToPlayer(playerId, notify);
	}

	/**
	 *@brief 广播一个公告 Modify by huchenhui
	 */
	template <typename... Args>
	void BroadcastNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.color = color;
		notify.word = buffer;
		Router::BroadcastToPlayer(notify);
	}

	/**
	*@brief 发送一封系统邮件 Modify by huchenhui
	*/
	UInt32 SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, UInt32 validDay = MAIL_VALID_DAY, RewardGroup* rewards = NULL, std::vector<Item*> detailItems = std::vector<Item*>());

	UInt32 SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards = NULL, UInt32 validDay = MAIL_VALID_DAY);

	//WorldSendAnnouncement

	/**
	*@brief 发送一个离线公告 Added by huchenhui
	*/
	template <typename... Args>
	void SendAnnouncement(UInt32 id, const Args & ... args)
	{
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;

		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		CLProtocol::WorldSendAnnouncement req;
		req.id = id;
		req.content = std::string(buffer);
		req.startTime = 0;
		Router::SendToWorld(req);
	}

	/**
	*@brief 发送一个离线公告 Added by huchenhui
	*/
	template <typename... Args>
	void SendAnnouncementByStartTime(UInt32 id, UInt32 startTime, const Args & ... args)
	{
		AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return;

		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, args...)) return;

		CLProtocol::WorldSendAnnouncement req;
		req.id = id;
		req.content = std::string(buffer);
		req.startTime = startTime;
		Router::SendToWorld(req);
	}

	/**
	 *@brief 发送一个离线公告 Modify by huchenhui
	 */
	template <typename... Args>
	void SendOfflineNotify(const std::string& type, UInt32 id, const Args & ... args)
	{
		UInt16 itype = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, itype, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::WorldSendOfflineNotify req;
		req.type = type;
		req.word = buffer;
		Router::SendToWorld(req);
	}

	/**
	 *@brief 发送一个离线公告
	 */
	void SendOfflineNotify(const std::string& type, const std::string& word);

	/**
	*@brief 发送udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_SCENE_UID);
		if (NULL == pLogger) return;
		pLogger->LogMsg(typeInfo, format, args...);
	}
};

#endif
