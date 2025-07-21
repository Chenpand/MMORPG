#ifndef _WORLD_CHAT_CHANNEL_H_
#define _WORLD_CHAT_CHANNEL_H_

#include <CLDefine.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>

#include "kingnet_analyzer.h"
#include "WSRouter.h"

namespace Sys
{
	/**
	 *@brief �㲥һ��ϵͳ��ʾ modify by huchenhui
	 */
	//void BroadcastNotify(UInt32 id, ...);
	template <typename... Args>
	void BroadcastNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.color = color;
		notify.word = buffer;
		Router::BroadcastToPlayer(notify);
	}

	/**
	 *@brief �㲥һ��ϵͳ��ʾ��ָ��Ƶ�� modify by huchenhui
	 */
	//void BroadcastNotifyInChannel(UInt8 channel, UInt32 id, ...);
	template <typename... Args>
	void BroadcastNotifyInChannel(UInt8 channel, UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = 1;
		notify.channel = channel;
		notify.color = color;
		notify.word = buffer;
		Router::BroadcastToPlayer(notify);
	}

	/**
	 *@brief �㲥һ��ϵͳ��ʾ
	 */
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);

	/**
	 *@brief ���ȼ��㲥ϵͳ��ʾ
	 */
	void BroadcastNotifyByLvl(UInt16 level, UInt16 type, UInt8 color, const char* word);

	/**
	*@brief ����udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_WORLD_UID);
		if (NULL == pLogger) return;
		pLogger->SetUserMsg("");
		pLogger->SetUserIP("");
		pLogger->LogMsg(typeInfo, format, args...);
	}

}

#endif
