#include "WorldSysNotify.h"

namespace Sys
{
	/*
	// by huchenhui
	void BroadcastNotify(UInt32 id, ...)
	{
		va_list args;
		va_start(args, id);
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN+1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
		va_end(args);

		if(!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.color = color;
		notify.word = buffer;
		Router::BroadcastToPlayer(notify);
	}

	void BroadcastNotifyInChannel(UInt8 channel, UInt32 id, ...)
	{
		va_list args;
		va_start(args, id);
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN+1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
		va_end(args);

		if(!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = 1;
		notify.channel = channel;
		notify.color = color;
		notify.word = buffer;
		Router::BroadcastToPlayer(notify);
	}
	*/

	void BroadcastNotify(UInt16 type, UInt8 color, const char* word)
	{
		if(word == NULL) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.color = color;
		notify.word = word;
		Router::BroadcastToPlayer(notify);
	}

	void BroadcastNotifyByLvl(UInt16 level, UInt16 type, UInt8 color, const char* word)
	{
		if(word == NULL) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.color = color;
		notify.word = word;
		Router::BroadcastToPlayerByLvl(level, notify);
	}
}

