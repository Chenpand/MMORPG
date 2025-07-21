#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <CLItemDefine.h>
#include <kingnet_analyzer.h>


namespace SysUtility
{
	/**
	*@brief ·¢ËÍÓÊ¼þ
	*/
	bool SendMail(UInt32 destCenterNode, UInt32 destAdminNode, const std::string& uid, const std::string& senderName, const std::string& title, const std::string& content, const std::string& reason, const ItemRewardVec& rewards);

	/**
	*@brief ·¢ËÍudplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_GLOBAL_ACTIVITY_UID);
		if (NULL == pLogger) return;
		pLogger->SetUserMsg("");
		pLogger->SetUserIP("");
		pLogger->LogMsg(typeInfo, format, args...);
	}
}

#endif