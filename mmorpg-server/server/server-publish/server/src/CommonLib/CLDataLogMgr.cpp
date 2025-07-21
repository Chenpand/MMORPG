#include "CLDataLogMgr.h"

#include <sstream>
#include <AvalonXmlDocument.h>
#include <AvalonLogger.h>
#include <AvalonFileAppender.h>
#include "CLDataLogAppender.h"

CLDataLogMgr::CLDataLogMgr()
{
}

CLDataLogMgr::~CLDataLogMgr()
{
}

bool CLDataLogMgr::Init(const std::string& config)
{
	Avalon::XmlDocument doc(config);
	if(!doc.LoadFile()) return false;

	Avalon::XmlElement* root = doc.RootElement();
	if(root == NULL) return false;

	bool bNet = false;
	Avalon::SockAddr addr;
	std::string logdir;

	//noted by aprilliu, 此处的处理应该是区分了DataLogServer（数据统计服务器）以及其他非统计服务器使用的udplog.xml的不同格式
	Avalon::XmlElement* logsvrNode = doc.FirstChildElement(root, "datalogsvr");//普通服务器使用的\Config\udplog.xml.new 应该有此配置分节
	if(logsvrNode != NULL)
	{
		bNet = true;
		std::string ip;
		Avalon::UInt16 port = 0;
		if(!doc.GetAttribute(logsvrNode, "ip", ip) || !doc.GetAttribute(logsvrNode, "port", port)) return false;
	
		addr.SetIP(ip);
		addr.SetPort(port);
	}
	else
	{
		Avalon::XmlElement* logdirNode = doc.FirstChildElement(root, "datalogdir"); //DataLogServer目录下的udplog.xml 有此分节
		if(logdirNode == NULL) return false;

		if(!doc.GetAttribute(logdirNode, "dir", logdir)) return false;
	}

	Avalon::XmlElement* logtypeNode = doc.FirstChildElement(root, "logtype");
	if(logtypeNode == NULL) return false;

	Avalon::XmlElement* logfileItem = doc.FirstChildElement(logtypeNode, "item");
	while(logfileItem != NULL)
	{
		std::string tname;
		if(doc.GetAttribute(logfileItem, "name", tname))
		{
			if(bNet) InitLogger(tname, addr);
			else InitLogger(tname, logdir);
		}
		logfileItem = doc.NextElement(logfileItem, "item");
	}
	return true;
}

void CLDataLogMgr::Final()
{
}

bool CLDataLogMgr::InitLogger(const std::string& logName, const Avalon::SockAddr& addr)
{
	CLDataLogAppender* pAppender = new CLDataLogAppender(logName, addr);

	std::ostringstream namestream;
	namestream << "datalog_" << logName;
	Avalon::Logger* logger = Avalon::Logger::getInstance(namestream.str().c_str());
	if(!logger->AddAppender(pAppender))
	{
		return false;
	}
	return true;
}

bool CLDataLogMgr::InitLogger(const std::string& logName, const std::string& logDir)
{
	std::string logprefix = logDir + logName + "_";
	Avalon::FileAppender* pAppender = new Avalon::FileAppender(logprefix, Avalon::FILEROLL_HOUR);//modified by aprilliu. 在gcc 4.6以下版本 Avalon::FileRollType::FILEROLL_HOUR这种写法
																												//无法通过编译，改写为Avalon::FILEROLL_HOUR 则可以通过

	std::ostringstream namestream;
	namestream << "datalog_" << logName;
	Avalon::Logger* logger = Avalon::Logger::getInstance(namestream.str().c_str());
	if(!logger->AddAppender(pAppender))
	{
		return false;
	}
	return true;
}
