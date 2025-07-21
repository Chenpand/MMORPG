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

	//noted by aprilliu, �˴��Ĵ���Ӧ����������DataLogServer������ͳ�Ʒ��������Լ�������ͳ�Ʒ�����ʹ�õ�udplog.xml�Ĳ�ͬ��ʽ
	Avalon::XmlElement* logsvrNode = doc.FirstChildElement(root, "datalogsvr");//��ͨ������ʹ�õ�\Config\udplog.xml.new Ӧ���д����÷ֽ�
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
		Avalon::XmlElement* logdirNode = doc.FirstChildElement(root, "datalogdir"); //DataLogServerĿ¼�µ�udplog.xml �д˷ֽ�
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
	Avalon::FileAppender* pAppender = new Avalon::FileAppender(logprefix, Avalon::FILEROLL_HOUR);//modified by aprilliu. ��gcc 4.6���°汾 Avalon::FileRollType::FILEROLL_HOUR����д��
																												//�޷�ͨ�����룬��дΪAvalon::FILEROLL_HOUR �����ͨ��

	std::ostringstream namestream;
	namestream << "datalog_" << logName;
	Avalon::Logger* logger = Avalon::Logger::getInstance(namestream.str().c_str());
	if(!logger->AddAppender(pAppender))
	{
		return false;
	}
	return true;
}
