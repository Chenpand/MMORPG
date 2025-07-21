#include "VCApplication.h"
#include "VCInternalNetwork.h"
#include "VerifyServerMgr.h"

VCApplication::VCApplication()
{
	m_ShowAbilityTimer.SetInterval(5 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);
}

VCApplication::~VCApplication()
{

}

bool VCApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

	if (!VCInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}
	

	InfoLogStream << GetName() << " init success!" << LogStream::eos;

	return true;
}

void VCApplication::OnTick(const Avalon::Time& now)
{
	CLApplication::OnTick(now);
	VCInternalNetwork::Instance()->OnTick(now);

	if (m_ShowAbilityTimer.IsTimeout(now))
	{
		VerifyServerMgr::Instance()->ShowAbility();
	}
}

void VCApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;

	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
	{
	}
	case 12:
	{
		VerifyServerMgr::Instance()->ShowAbility();
		break;
	}
	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}


void VCApplication::OnQuit()
{

}