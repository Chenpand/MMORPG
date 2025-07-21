#include "AvalonApplication.h"
#include "AvalonFileAppender.h"
#include "AvalonConsoleAppender.h"
#include "AvalonFileAppenderThread.h"

namespace Avalon{

	const static std::string DEFAULT_LOGDIR = "Log/";

	Application::Application()
	{
		m_pSysLog = NULL;
	}

	Application::~Application()
	{
	}

	void Application::Init(const std::string& appName,const std::string& configName)
	{
		m_AppName = appName;
		m_ConfigName = configName;
	}

	void Application::Main(int argc, char *argv[])
	{
		m_Properties.LoadFromCmdline(GetName(),argc,argv);

		m_LogDir = m_Properties.GetProperty(GetName(), "LogDir").GetString();
		if(m_LogDir.empty()) m_LogDir = DEFAULT_LOGDIR;

		m_pSysLog = Logger::getInstance("syslog");
		Appender* pfileAppender = NULL;

		if (m_Properties.GetProperty(m_AppName, "LOG_THREAD"))
		{
			pfileAppender = new FileAppenderThread(m_LogDir + GetName(), Avalon::FILEROLL_DAY);
		}
		else
		{
			pfileAppender = new FileAppender(m_LogDir + GetName(), Avalon::FILEROLL_DAY);
		}

		if (!pfileAppender || !m_pSysLog->AddAppender(pfileAppender)) //以天为单位进行日志文件的滚动
		{
			m_pSysLog = NULL;
			Logger::Shutdown();
			return;
		}

		if(m_Properties.GetProperty(m_AppName,"DEBUG")) // 
		{
			m_pSysLog->AddAppender(new ConsoleAppender());
			m_pSysLog->SetPriority(LOG_DEBUG);
		}
		else
		{
			m_pSysLog->SetPriority(LOG_DEBUG);
		}

		if(!m_Properties.LoadFromFile(m_ConfigName))
		{
			m_pSysLog->Error("load config %s failed!",m_ConfigName.c_str());
			m_pSysLog = NULL;
			Logger::Shutdown();
			return;
		}

		BeginProcess();

		m_pSysLog->Info("application exit!");
		m_pSysLog = NULL;
		Logger::Shutdown();
	}

	void Application::Exit()
	{
		EndProcess();
	}

	bool Application::OnInit()
	{
		return true;
	}

	void Application::OnTick(const Time& now)
	{
	}

	void Application::OnQuit()
	{
	}
}

