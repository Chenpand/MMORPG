#include "ILogic.h"
#include "VerifyMgr.h"
#include "VerifyTask.h"

ObjID_t	ILogic::GetID() 
{ 
	return m_Task == NULL ? 0 : m_Task->GetID(); 
}

bool ILogic::OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket)
{
	m_Task = task;
	return true;
}

void ILogic::OnTick(const Avalon::Time& now)
{
	for (auto data : m_WaitPushFrames)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (!data.Encode(packet))
		{
			ErrorLogStream << "task(" << GetID() << ") encode frames failed." << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			m_WaitPushFrames.clear();
			return;
		}

		//DebugLogStream << "task(" << GetID() << ") start to push frames." << LogStream::eos;
		if (!VerifyMgr::Instance()->LC_PushFrameCommand(GetID(), packet))
		{
			ErrorLogStream << "task(" << GetID() << ") push frames failed." << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			m_WaitPushFrames.clear();
			return;
		}

		Avalon::Packet::Destroy(packet);
	}

	m_WaitPushFrames.clear();
}

void ILogic::PushFrame(const CLProtocol::Frame& frame)
{
	if (m_WaitPushFrames.empty() || m_WaitPushFrames[m_WaitPushFrames.size() - 1].frames.size() >= 2000)
	{
		CLProtocol::RelaySvrFrameDataNotify datas;
		datas.frames.push_back(frame);
		m_WaitPushFrames.push_back(datas);
	}
	else
	{
		m_WaitPushFrames[m_WaitPushFrames.size() - 1].frames.push_back(frame);
	}
	
}

bool ILogic::IsRunToLastFrame()
{
	return VerifyMgr::Instance()->LC_IsRunToLastFrame(GetID());
}