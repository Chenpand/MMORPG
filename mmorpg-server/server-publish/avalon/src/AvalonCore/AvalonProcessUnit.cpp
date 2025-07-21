#include "AvalonProcessUnit.h"
#include "AvalonRandom.h"

#if defined(AVALON_OS_WINDOWS)
#	include <windows.h>
#	define	AVALON_SLEEP(msec) ::Sleep(DWORD(msec))
#else
#	include <unistd.h>
#	define	AVALON_SLEEP(msec) ::usleep(int(msec)*1000)
#endif

namespace Avalon
{
	ProcessUnit::ProcessUnit()
	{
		m_bRun = true;
	}

	ProcessUnit::~ProcessUnit()
	{
	}

	void ProcessUnit::SetInterval(const Time &interval)
	{
		m_Interval = interval;
	}

	void ProcessUnit::BeginProcess()
	{
		bool ret = OnInit();

		if(!ret)
		{
			OnQuit();
			return;
		}

		Time curtime = Time::CurrentTime();
		Random::SRand((UInt32)curtime.Sec());

		while(m_bRun)
		{
			Time oldtime = curtime;
			OnTick(curtime);
			curtime = Time::CurrentTime();

			if(curtime < oldtime + m_Interval)
			{
				Sleep(oldtime + m_Interval - curtime);
			}
			else
			{
				Sleep(1);
			}
		}

		OnQuit();

		m_bRun = true;
	}

	void ProcessUnit::EndProcess()
	{
		m_bRun = false;
	}

	void ProcessUnit::Sleep(const Time& time)
	{
		UInt32 msec = (UInt32)time.MSec();
		if(msec != 0)
		{
			AVALON_SLEEP(msec);
		}
	}
}
