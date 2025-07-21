#include "CLGameLoggerMgr.h"

#include <AvalonXmlDocument.h>
#include <AvalonConsoleAppender.h>

#include "CLDefine.h"
#include "CLGameLoggerAppender.h"

GameLoggerMgr::GameLoggerMgr()
{
	m_bEcho = false;
}

GameLoggerMgr::~GameLoggerMgr()
{
}

bool GameLoggerMgr::Init(const std::string& config, bool bEcho, Avalon::NetConnectionPtr conn)
{
	m_bEcho = bEcho;
	m_Conn = conn;

	Avalon::XmlDocument doc(config);
	if(!doc.LoadFile()) return false;

	Avalon::XmlElement* root = doc.RootElement();
	if(root == NULL) return false;
	Avalon::XmlElement* filelog = doc.FirstChildElement(root, "filelog");
	if(filelog == NULL) return false;

	Avalon::XmlElement* loggerNode = doc.FirstChildElement(filelog, "logger");
	while(loggerNode != NULL)
	{
		std::string name;
		Int32 rollType = 0;
		doc.GetAttribute(loggerNode, "name", name);
		doc.GetAttribute(loggerNode, "rollType", rollType);

		if(!InitLogger(name, Avalon::FileRollType(rollType))) return false;  //noted by aprilliu, 根据\Config\GameLog.xml配置信息创建相应的日志记录器（比如：Player, Task,Billing等）

		loggerNode = doc.NextElement(loggerNode, "logger");
	}

	return true;
}

void GameLoggerMgr::Final()
{
}

bool GameLoggerMgr::InitLogger(const std::string& logName, Avalon::FileRollType rollType)
{
	CLGameLoggerAppender* appender = new CLGameLoggerAppender(new CLNetLogAppender(logName, m_Conn)
		, new Avalon::FileAppender(std::string(GAME_LOGGER_DIR) + logName, rollType));
	
	Avalon::Logger *logger = Avalon::Logger::getInstance(logName.c_str());
	if(!logger->AddAppender(appender))
	{
		return false;
	}

	if(m_bEcho)
	{
		logger->AddAppender(new Avalon::ConsoleAppender());
		logger->SetPriority(Avalon::LOG_DEBUG);
	}
	else
	{
		logger->SetPriority(Avalon::LOG_DEBUG);
	}
	return true;
}
