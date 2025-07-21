#include "TFSApplication.h"

#include <PerformanceCheck.h>

#include "TFSNetwork.h"
#include "GameZoneMgr.h"

TFSApplication::TFSApplication()
{

}

TFSApplication::~TFSApplication()
{

}

bool TFSApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	LoadDynamicConfig();

	if (!TFSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if (!GameZoneMgr::Instance()->Init())
	{
		ErrorLogStream << "game zone manager init failed!" << LogStream::eos;
		return false;
	}

	m_TickTime = Avalon::Time::CurrentTime().MSec();

	return true;
}

void TFSApplication::OnTick(const Avalon::Time& now)
{
	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(TFSNetwork);
	TFSNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GameZoneMgr);
	GameZoneMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	m_TickTime = now.MSec();

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("TransferServer.cfg"))
		{
			ErrorLogStream << "load TransferServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}
}

void TFSApplication::OnQuit()
{
	GameZoneMgr::Instance()->Final();
	TFSNetwork::Instance()->Shutdown(200);

	CLApplication::OnQuit();
}

void TFSApplication::OnSignal(int signal)
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

void TFSApplication::LoadDynamicConfig()
{

}