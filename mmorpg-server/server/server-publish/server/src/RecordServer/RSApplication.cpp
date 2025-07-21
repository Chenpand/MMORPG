#include "RSApplication.h"
#include <AvalonLogger.h>
#include "RSNetwork.h"
#include "RSTableMgr.h"
#include "PerformanceCheck.h"
#include "DBMgr.h"
#include "ScriptMgr.h"

RSApplication::RSApplication()
{
	m_TickTime = 0;
	m_IsRecordUpdateLog = false;
	m_ReloadConfig = false;
	m_OpenTransRequest = false;
	m_checkMysqlLastErrNo = false;
}

RSApplication::~RSApplication()
{
}

bool RSApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	std::string rootpath = ROOT_PATH;
	if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

#ifdef _DEBUG
	if (!GetProperties()->GetProperty("Database", "db_patch"))
	{
		if (!ScriptMgr::Instance()->Init(GetProperties()))
		{
			ErrorLogStream << "script system init failed!" << LogStream::eos;
			return false;
		}
	}
#endif

	if (!RSTableMgr::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "tablemgr init failed!" << LogStream::eos;
		return false;
	}

	if (!RSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "network startup failed!" << LogStream::eos;
		return false;
	}

	LoadDynamicConfig();

#ifdef _NEW_DB_MGR_
	if (!DBMgr::Instance()->Init(5))
	{
		ErrorLogStream << "dbmgr init failed!" << LogStream::eos;
		return false;
	}
#endif

	m_TickTime = Avalon::Time::CurrentTime().MSec();
	return true;
}

void RSApplication::OnTick(const Avalon::Time &now)
{
	m_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(RSNetwork);
	RSNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(RSTableMgr);
	RSTableMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(DBMgr);
	DBMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	UInt64 offsetTime = Avalon::Time::CurrentTime().MSec() - m_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
		SHOW_PERF();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("RecordServer.cfg"))
		{
			ErrorLogStream << "load RecordServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}
}

void RSApplication::OnQuit()
{
	RSNetwork::Instance()->Shutdown(200);
	RSTableMgr::Instance()->Final();

	CLApplication::OnQuit();
}

void RSApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;

	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
	{
		m_ReloadConfig = true;
		break;
	}
	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}

void RSApplication::LoadDynamicConfig()
{
	DebugLogStream << "load record server dynamic config." << LogStream::eos;

	m_IsRecordUpdateLog = GetProperties()->GetProperty("Game", "record_update_log");

	m_OpenTransRequest = GetProperties()->GetProperty("Database", "open_trans_request");
	InfoLogStream << "open trans request(" << m_OpenTransRequest << ")." << LogStream::eos;

	{
		m_MergeFlushSize = GetProperties()->GetProperty("Game", "merge_flush_size");

		m_BatchTables.clear();
		std::string batchTableStr = GetProperties()->GetProperty("Game", "batch_tables").GetString();
		Avalon::StringUtil::Split(batchTableStr, m_BatchTables, "|");

		InfoLogStream << "merge flush size(" << m_MergeFlushSize << ") batch tables(" << batchTableStr << ")." << LogStream::eos;
	}

	m_checkMysqlLastErrNo = GetProperties()->GetProperty("Database", "check_mysql_last_error_no");

	UInt8 tmpPriority = GetProperties()->GetProperty("Log", "priority");
	if (tmpPriority >= Avalon::LOG_DEBUG && tmpPriority < Avalon::LOG_MAX)
	{
		GetSysLog()->SetPriority((Avalon::LogPriority)tmpPriority);
	}
}