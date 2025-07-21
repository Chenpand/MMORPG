
#include "kingnet_analyzer.h"
#include "udplog_lib.h"
#include <cstdio>
#include <AvalonSocket.h>

#ifndef AVALON_OS_WINDOWS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#else
#include <Ws2tcpip.h>
#endif

#if defined(AVALON_OS_WINDOWS)
class NetInitHelper
{
public:
	NetInitHelper()
	{
		WORD wVersionRequested = MAKEWORD(2, 0);
		WSADATA wsaData;

		WSAStartup(wVersionRequested, &wsaData);
	}
	~NetInitHelper()
	{
		WSACleanup();
	}
};
static NetInitHelper __helper;
#endif

Int32 CKingnetAnalyzer::Init(const char* path/* = DEFAULT_CONFIGFILENAME*/)
{
	return _config.Init(path);
}

/*
Int32 CUserLogger::LogMsg(const char *str1, const char *str2, const char *str3, const char *str4, const char *str5, const char *str6, const char *typeinfo, Int32 nCount)
{
	if(NULL == str1 || NULL == str2 || NULL == str3 || NULL == str4 || NULL == str5 || NULL == str6 || NULL == typeinfo )
	{
		return UL_FAIL;
	}
	Int32 type = _config.GetLogFileID(typeinfo);
	if(type < 0)
	{
		return UL_FAIL;
	}
	m_nTime		= (Int32)time(NULL);

#ifndef AVALON_OS_WINDOWS
	snprintf(m_szLogBuffer,sizeof(m_szLogBuffer), "%s|%s|%s|%s|%s|%s|%d|%s|%d|%s",str1, str2, str3, str4, str5, str6, nCount,m_szUserIP, m_nTime, m_szUserMsg);
#else
	_snprintf_s(m_szLogBuffer,sizeof(m_szLogBuffer), "%s|%s|%s|%s|%s|%s|%d|%s|%d|%s",str1, str2, str3, str4, str5, str6, nCount,m_szUserIP, m_nTime,m_szUserMsg);
#endif
	
	m_szLogBuffer[MAX_LOG_LENGTH-1] = '\0';
	SendUDPLog(m_nUid, type, m_szLogBuffer);

	return UL_OK;
}
*/

Int32 CUserLogger::SendUDPLog(Int32 uid, Int32 type, const char* msg)
{
	if(NULL == msg)
	{
		return UL_FAIL;
	}

	Int32 ret = UL_OK;
	stHost host;
	ret = _config.GetHostRandomly(host);
	if(ret < 0)
	{
		return ret;
	}
	CCSNotifyWriteLog body;
	body.m_nUid			= uid;
	body.m_nTableType	= type;

#ifndef AVALON_OS_WINDOWS
	strncpy(body.m_szLog, msg, MAX_LOG_LENGTH-1);
#else
	strncpy_s(body.m_szLog, sizeof(body.m_szLog), msg, strlen(msg));
#endif
	
	CCSHead head;
	head.m_nUid				= uid;
	head.m_nHeaderLength	= 30;//不包括打头的httphead

	char sendbuf[MAX_MESSAGE_LENGTH] = {0};
	
	UInt32 offset = 30 + 1;
	ret = body.Encode((UInt8*)sendbuf, sizeof(sendbuf), offset);
	if(ret < 0)
	{
		return ret;
	}

	head.m_nPackageLength = offset - 1;
	offset = 0;

	ret = head.Encode((UInt8*)sendbuf,sizeof(sendbuf),offset);
	if(ret < 0)
	{
		return ret;
	}

#ifndef AVALON_OS_WINDOWS
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	Int32 sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		return UL_FAIL;
	}
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(host.nPort);
	if(inet_aton(host.szIP, &addr.sin_addr) < 0)
	{
		Avalon::sock_close(sockfd);
		return UL_FAIL;
	}
	if(sendto(sockfd, sendbuf, (size_t)(head.m_nPackageLength+1), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		Avalon::sock_close(sockfd);
		return UL_FAIL;
	}
	Avalon::sock_close(sockfd);
#else
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	Int32 sockfd = Avalon::sock_create(SOCK_DGRAM);
	if (sockfd == -1)
	{
		UL_FAIL;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(host.nPort);
	
	if (inet_pton(AF_INET, host.szIP, &addr.sin_addr) < 0)
	{
        Avalon::sock_close(sockfd);
		return UL_FAIL;
	}

	if (sendto(sockfd, sendbuf, (size_t)(head.m_nPackageLength + 1), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
	{
        Avalon::sock_close(sockfd);
		return UL_FAIL;
	}
	Avalon::sock_close(sockfd);
#endif

	return UL_OK;
}

