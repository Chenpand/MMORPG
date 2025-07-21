#include "LogicClientInterface.h"
#include "VerifyMgr.h"
#include "VerifyTask.h"
#include "VSApplication.h"

bool LogicClentInterface::Init()
{
	m_LC_Init = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_Init>("", "LogicApplication", "Init");
	if (!m_LC_Init)
	{
		ErrorLogStream << "get function(LogicApplication:Init) failed." << LogStream::eos;
		return false;
	}

	if (!LC_Init())
	{
		ErrorLogStream << "LogicApplication Init failed." << LogStream::eos;
		return false;
	}

	m_LC_Update = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_Update>("", "LogicApplication", "Update");
	if (!m_LC_Update)
	{
		ErrorLogStream << "get function(LogicApplication:Update) failed." << LogStream::eos;
		return false;
	}

	m_LC_StartPVE = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_StartPVE>("", "LogicApplication", "StartPVE");
	if (!m_LC_StartPVE)
	{
		ErrorLogStream << "get function(LogicApplication:StartPVE) failed." << LogStream::eos;
		return false;
	}

	m_LC_StartPVP = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_StartPVP>("", "LogicApplication", "StartPVP");
	if (!m_LC_StartPVP)
	{
		ErrorLogStream << "get function(LogicApplication:StartPVP) failed." << LogStream::eos;
		return false;
	}

	m_LC_PushFrameCommand = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_PushFrameCommand>("", "LogicApplication", "PushFrameCommand");
	if (!m_LC_PushFrameCommand)
	{
		ErrorLogStream << "get function(LogicApplication:PushFrameCommand) failed." << LogStream::eos;
		return false;
	}

	m_LC_GiveUpVerify = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_GiveUpVerify>("", "LogicApplication", "GiveUpVerify");
	if (!m_LC_GiveUpVerify)
	{
		ErrorLogStream << "get function(LogicApplication:GiveUpVerify) failed." << LogStream::eos;
		return false;
	}

	m_LC_SaveRecord = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_SaveRecord>("", "LogicApplication", "SaveRecord");
	if (!m_LC_SaveRecord)
	{
		ErrorLogStream << "get function(LogicApplication:SaveRecord) failed." << LogStream::eos;
		return false;
	}

	m_LC_DumpMemory = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_DumpMemory>("", "LogicApplication", "DumpMemory");
	if (!m_LC_DumpMemory)
	{
		ErrorLogStream << "get function(LogicApplication:DumpMemory) failed." << LogStream::eos;
		return false;
	}

	m_LC_IsRunToLastFrame = AvalonMonoDelegate::Instance()->GetFunc<Func_LC_IsRunToLastFrame>("", "LogicApplication", "IsRunToLastFrame");
	if (!m_LC_IsRunToLastFrame)
	{
		ErrorLogStream << "get function(LogicApplication:IsRunToLastFrame) failed." << LogStream::eos;
		return false;
	}
	
#ifdef __RUN_REPLAY__
	FILE* pkFile = fopen("pvp_record", "rb");
	fseek(pkFile, 0, SEEK_END);
	long size = ftell(pkFile);
	char*buff = (char*)alloca(size);
	fseek(pkFile, 0, SEEK_SET);
	fread(buff, 1, size, pkFile);

	typedef void(stdcall *StartPVPRecord)(void*, int, MonoException**);
	auto startPVPRecord = AvalonMonoDelegate::Instance()->GetFunc<StartPVPRecord>("", "LogicApplication", "StartPVPRecord");
	for (int i = 0; i < 1; i++)
	{
		AvalonMonoDelegate::Instance()->Call(startPVPRecord, buff, size);
	}
#endif

	return true;
}

bool LogicClentInterface::LC_Init() 
{ 
	if (!m_LC_Init)
	{
		return false;
	}

	return AvalonMonoDelegate::Instance()->Call<bool>(m_LC_Init); 
}

void LogicClentInterface::LC_Update(int delta)
{
	if (!m_LC_Update)
	{
		return;
	}

	return AvalonMonoDelegate::Instance()->Call(m_LC_Update, delta); 
}

bool LogicClentInterface::LC_StartPVE(ObjID_t session, Avalon::Packet* packet)
{
	if (!m_LC_StartPVE)
	{
		return false;
	}

	return AvalonMonoDelegate::Instance()->Call<bool>(m_LC_StartPVE, session, (char*)packet->GetBuffer(), packet->GetSize());
}

bool LogicClentInterface::LC_StartPVP(ObjID_t session, Avalon::Packet* packet)
{
	if (!m_LC_StartPVP)
	{
		return false;
	}

	return AvalonMonoDelegate::Instance()->Call<bool>(m_LC_StartPVP, session, (char*)packet->GetBuffer(), packet->GetSize());
}

bool LogicClentInterface::LC_PushFrameCommand(ObjID_t session, Avalon::Packet* packet)
{
	if (!m_LC_PushFrameCommand)
	{
		return false;
	}

	return AvalonMonoDelegate::Instance()->Call<bool>(m_LC_PushFrameCommand, session, (char*)packet->GetBuffer(), packet->GetSize());
}

void LogicClentInterface::LC_GiveUpVerify(ObjID_t session)
{
	if (!m_LC_GiveUpVerify)
	{
		return;
	}

	return AvalonMonoDelegate::Instance()->Call(m_LC_GiveUpVerify, session);
}

void LogicClentInterface::LC_SaveRecord(ObjID_t session)
{
	if (!m_LC_SaveRecord)
	{
		return;
	}

	return AvalonMonoDelegate::Instance()->Call(m_LC_SaveRecord, session);
}

void LogicClentInterface::LC_DumpMemory()
{
	if (!m_LC_DumpMemory)
	{
		return;
	}

	AvalonMonoDelegate::Instance()->Call(m_LC_DumpMemory);
}

bool LogicClentInterface::LC_IsRunToLastFrame(ObjID_t session)
{
	if (!m_LC_IsRunToLastFrame)
	{
		return true;
	}

	return AvalonMonoDelegate::Instance()->Call<bool>(m_LC_IsRunToLastFrame, session);
}

void LogicClentInterface::OnReportCheckSum(ObjID_t session, UInt32 frame, UInt32 checksum)
{
	//DebugLogStream << "session(" << session << ") frame(" << frame << ") checksum(" << checksum << ")." << LogStream::eos;

	auto task = VerifyMgr::Instance()->FindTask(session);
	if (!task)
	{
		//ErrorLogStream << "can't find task(" << session << ")." << LogStream::eos;
		return;
	}

	task->SetLogicChecksum(frame, checksum);
}

void LogicClentInterface::OnReportRaceEnd(ObjID_t session, MonoArray* array, int len)
{
	DebugLogStream << "session(" << session << ") report race end." << LogStream::eos;

	auto task = VerifyMgr::Instance()->FindTask(session);
	if (!task)
	{
		ErrorLogStream << "can't find task(" << session << ")." << LogStream::eos;
		return;
	}

	if (task->GetLogicRaceEndInfo() != NULL)
	{
		ErrorLogStream << "repeat report race(" << session << ")'s raceend." << LogStream::eos;
		return;
	}

	auto str = mono_array_addr(array, char, 0);
	auto packet = Avalon::Packet::Create();

	try {
		Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
		stream.SeriaArray(str, len);
		packet->SetSize(len);
		task->SetLogicRaceEndInfo(packet);
	}
	catch (Avalon::NetSerializeException&)
	{
		ErrorLogStream << "session(" << session << ") encode packet failed." << LogStream::eos;
		Avalon::Packet::Destroy(packet);
	}

	// 添加统计
	{
		// 说明玩家还没上报结算(这种情况应该是不同步了)
		if (task->GetPlayerReportRaceEndTime() == 0)
		{
			ErrorLogStream << "race(" << session << ") player not report race end." << LogStream::eos;
		}
		else
		{
			UInt64 now = CURRENT_TIME.MSec();
			auto delay = now - task->GetPlayerReportRaceEndTime();

			InfoLogStream << "race(" << session << ") race end delay(" << delay << "), current(" << now << ") player(" << task->GetPlayerReportRaceEndTime() << ")." << LogStream::eos;

			auto& sta = VerifyMgr::Instance()->GetStatistic();
			sta.raceend++;
			sta.totalRaceEndDelayMs += delay;
			sta.averRaceEndDelayMs = sta.totalRaceEndDelayMs / sta.raceend;
			if (sta.maxRaceEndDelayMs < delay)
			{
				sta.maxRaceEndDelayMs = delay;
			}
		}
	}
}

void LogicClentInterface::OnRecordLog(LogicClientLogType type, MonoString* str)
{
	if (VSApplication::Instance()->GetVerifyLogLevel() > type)
	{
		return;
	}

	auto s = mono_string_to_utf8(str);
	if (type == LC_LOG_DEBUG)
	{
		DebugLogStream << s << LogStream::eos;
	}
	else if (type == LC_LOG_INFO)
	{
		InfoLogStream << s << LogStream::eos;
	}
	else if (type == LC_LOG_WARN)
	{
		TraceLogStream << s << LogStream::eos;
	}
	else if (type == LC_LOG_ERROR)
	{
		ErrorLogStream << s << LogStream::eos;
	}

	mono_free(s);
}
