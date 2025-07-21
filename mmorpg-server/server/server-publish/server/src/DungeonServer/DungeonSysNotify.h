#ifndef _DUNGEON_SYS_NOTIFY_H_ 
#define _DUNGEON_SYS_NOTIFY_H_

#include <CLDefine.h>

#include "kingnet_analyzer.h"

namespace Sys
{
	/**
	*@brief ·¢ËÍudplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_DUNGEON_UID);
		if (NULL == pLogger) return;
		pLogger->SetUserMsg("");
		pLogger->SetUserIP("");
		pLogger->LogMsg(typeInfo, format, args...);
	}
}

#endif