#ifndef _CL_NETLOG_APPENDER_H_
#define _CL_NETLOG_APPENDER_H_

#include <AvalonAppender.h>
#include <AvalonNetConnection.h>
#include <CLDefine.h>

/**
 *@brief ������־׷����
 */
class CLNetLogAppender : public Avalon::Appender
{
public:
	CLNetLogAppender(const std::string& logname, Avalon::NetConnectionPtr conn);
	~CLNetLogAppender();

public:
	bool DoOpen();
	bool DoAppend(const Avalon::LogEvent& logEvent);
	void DoClose();

private:
	//��־��
	std::string	m_Name;
	//��������
	Avalon::NetConnectionPtr m_Conn;
};

#endif
