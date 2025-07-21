#ifndef _SYS_UTILITY_H_
#define _SYS_UTILITY_H_

#include <CLItemDefine.h>
#include <kingnet_analyzer.h>

/**
*@brief ϵͳ����
*/
namespace SysUtility
{
	/**
	*@brief �����ʼ�
	*/
	bool SendMail(UInt32 destConnId, UInt64 playerId, const std::string& senderName, const std::string& title, 
		const std::string& content, const std::string& reason, const ItemRewardVec& rewards);

	/**
	*@brief ����udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_TEAM_COPY_UID);
		if (NULL == pLogger) return;
		pLogger->SetUserMsg("");
		pLogger->SetUserIP("");
		pLogger->LogMsg(typeInfo, format, args...);
	}
};

#endif
