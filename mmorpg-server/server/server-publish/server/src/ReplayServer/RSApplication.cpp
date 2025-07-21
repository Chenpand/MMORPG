#include "RSApplication.h"

#include <AvalonMemoryTracker.h>
#include <AvalonLogger.h>
#include <PerformanceCheck.h>
#include <CLGameLoggerMgr.h>
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>

#include "RSInternalNetwork.h"
#include "ReplayMgr.h"

RSApplication::RSApplication()
{
	m_ReloadConfig = false;
	m_CanViewMasterReplay = true;
}

RSApplication::~RSApplication()
{
}

bool RSApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

	if (!GameLoggerMgr::Instance()->Init(GAME_LOGGER_CFG, IsDebugMode()))
	{
		ErrorLogStream << "game loggermgr init failed!" << LogStream::eos;
		return false;
	}

	if (!RSInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	LoadDynamicConfig();

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	std::vector<Avalon::NetConnectionPtr> connVec;
	RSInternalNetwork::Instance()->FindConnectionByType(ST_GLOBAL_RECORD, connVec);

	if (connVec.empty() || !CLRecordClient::Instance()->Init(this, connVec))
	{
		ErrorLogStream << "record client init failed!" << LogStream::eos;
		return false;
	}

	if (!ReplayMgr::Instance()->Init())
	{
		ErrorLogStream << "ReplayMgr init failed!" << LogStream::eos;
		return false;
	}

	std::string cfg = GetProperties()->GetProperty("Database", "database_config").GetString();
	Avalon::XmlDocument doc(cfg);
	if (!doc.LoadFile()) return false;

	Avalon::XmlElement* root = doc.RootElement();
	if (root == NULL) return false;

	//加载本地数据库表
	Avalon::XmlElement* localDBNode = doc.FirstChildElement(root, "localdb");
	m_DBConn = CreateDBConn(doc, localDBNode);
	if (localDBNode == NULL) return false;

	InfoLogStream << GetName() << " init success!" << LogStream::eos;

	return true;
}

void RSApplication::OnTick(const Avalon::Time &now)
{
	static UInt64 s_TickTime = 0;
	s_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(RSInternalNetwork);
	RSInternalNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(CLRecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
	PERF_SEG_END();

	PERF_SEG_BEGIN(ReplayMgr);
	ReplayMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	UInt64 offsetTime = Avalon::Time::CurrentTime().MSec() - s_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
		SHOW_PERF();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("ReplayServer.cfg"))
		{
			ErrorLogStream << "load ReplayServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
		ReloadDataTables();
	}
}

void RSApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;
	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
		m_ReloadConfig = true;
		break;

	default:
		ServerApplication::OnSignal(signal);
		break;

	}
}

bool RSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;

	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}

	return true;
}

void RSApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		SystemValueDataEntryMgr::Instance()->ReloadData(configpath + "SystemValueTable.txt");

	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}

}

void RSApplication::LoadDynamicConfig()
{
	InfoLogStream << "Begin load dynamic config." << LogStream::eos;

	m_CanViewMasterReplay = !GetProperties()->GetProperty("Replay", "close_master_replay");
	m_IsBatchLoadReplay = !GetProperties()->GetProperty("Replay", "batch_load_replay");
}

Avalon::RetryDBConnection* RSApplication::CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node)
{
	if (node == NULL) return NULL;

	std::string host;
	UInt32 port = 0;
	std::string user;
	std::string passwd;
	std::string dbname;

	doc.GetAttribute(node, "host", host);
	doc.GetAttribute(node, "port", port);
	doc.GetAttribute(node, "user", user);
	doc.GetAttribute(node, "passwd", passwd);
	doc.GetAttribute(node, "dbname", dbname);

	Avalon::RetryDBConnection* conn = Avalon::RetryDBConnection::Create("mysql", 60, 500);
	if (!conn->Open(host.c_str(), port, user.c_str(), passwd.c_str(), dbname.c_str()))
	{
		ErrorLogStream << "can not connect database " << dbname << "! host="
			<< host << ", port=" << port << "." << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(conn);
		return NULL;
	}

	return conn;
}

void RSApplication::OnQuit()
{
	ReplayMgr::Instance()->Final();
	GameLoggerMgr::Instance()->Final();
	CLApplication::OnQuit();
}

