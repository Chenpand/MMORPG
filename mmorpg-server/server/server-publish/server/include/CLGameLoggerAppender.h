#ifndef _CL_GAMELOGGER_APPENDER_H_
#define _CL_GAMELOGGER_APPENDER_H_

#include "CLNetLogAppender.h"
#include <AvalonFileAppender.h>

/**
 *@brief ��Ϸ��־׷����
 */
class CLGameLoggerAppender : public Avalon::Appender
{
public:
	CLGameLoggerAppender(CLNetLogAppender* netAppender, Avalon::FileAppender* fileAppender);
	~CLGameLoggerAppender();

public:
	bool DoOpen();
	bool DoAppend(const Avalon::LogEvent& logEvent);
	void DoClose();

private:
	//������־׷����
	CLNetLogAppender*		m_pNetAppender;
	//�ļ�׷����
	Avalon::FileAppender*	m_pFileAppender;
	//�ļ��Ƿ��Ѵ�
	bool					m_bFileOpen;
};

#endif
