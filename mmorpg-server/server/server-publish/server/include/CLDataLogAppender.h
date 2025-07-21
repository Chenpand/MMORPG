#ifndef _CL_DATALOG_APPENDER_H_
#define _CL_DATALOG_APPENDER_H_

#include <AvalonAppender.h>
#include <AvalonSocket.h>
#include <CLDefine.h>

/**
 *@brief data日志追加器   具体使用udp 发送统计数据
 */
class CLDataLogAppender : public Avalon::Appender
{
public:
	CLDataLogAppender(const std::string& logname, const Avalon::SockAddr& addr);
	~CLDataLogAppender();

public:
	bool DoOpen();
	bool DoAppend(const Avalon::LogEvent& logEvent);
	void DoClose();

private:
	//日志名
	std::string			m_Name;
	//udp套接字
	Avalon::Socket		m_Socket;
	//对端地址
	Avalon::SockAddr	m_PeerAddr;
};

#endif