#ifndef _CL_DATALOG_APPENDER_H_
#define _CL_DATALOG_APPENDER_H_

#include <AvalonAppender.h>
#include <AvalonSocket.h>
#include <CLDefine.h>

/**
 *@brief data��־׷����   ����ʹ��udp ����ͳ������
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
	//��־��
	std::string			m_Name;
	//udp�׽���
	Avalon::Socket		m_Socket;
	//�Զ˵�ַ
	Avalon::SockAddr	m_PeerAddr;
};

#endif