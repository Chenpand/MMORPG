#include "CLGameLoggerAppender.h"
#include "CLDefine.h"

CLGameLoggerAppender::CLGameLoggerAppender(CLNetLogAppender* netAppender, Avalon::FileAppender* fileAppender)
	:m_pNetAppender(netAppender), m_pFileAppender(fileAppender),m_bFileOpen(false){}

CLGameLoggerAppender::~CLGameLoggerAppender()
{
	CL_SAFE_DELETE(m_pNetAppender);
	CL_SAFE_DELETE(m_pFileAppender);
}

bool CLGameLoggerAppender::DoOpen()
{
	bool ret = false;

	if(m_pNetAppender != NULL && m_pNetAppender->Open())
		ret = true;
	if(m_pFileAppender != NULL)
		ret = true;
	return ret;
}

bool CLGameLoggerAppender::DoAppend(const Avalon::LogEvent& logEvent)
{
	if(m_pNetAppender != NULL && m_pNetAppender->Append(logEvent)) return true;

	if(m_pFileAppender != NULL)
	{
		if(!m_bFileOpen)
		{
			if(!m_pFileAppender->Open()) return false;
			m_bFileOpen = true;
		}
		if(m_pFileAppender->Append(logEvent)) return true;
	}

	return false;
}

void CLGameLoggerAppender::DoClose()
{
	if(m_pNetAppender != NULL) m_pNetAppender->Close();
	if(m_pFileAppender != NULL && m_bFileOpen) m_pFileAppender->Close();
}
