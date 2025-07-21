#ifndef _CL_GAMELOGGER_APPENDER_H_
#define _CL_GAMELOGGER_APPENDER_H_

#include "CLNetLogAppender.h"
#include <AvalonFileAppender.h>

/**
 *@brief 游戏日志追加器
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
	//网络日志追加器
	CLNetLogAppender*		m_pNetAppender;
	//文件追加器
	Avalon::FileAppender*	m_pFileAppender;
	//文件是否已打开
	bool					m_bFileOpen;
};

#endif
