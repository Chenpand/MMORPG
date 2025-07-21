#include "VerifyMgr.h"
#include <CLVerifyProtocol.h>

#include "VerifyTask.h"
#include "AvalonMonoDelegate.h"
#include "VSApplication.h"
#include "VSRouter.h"

VerifyMgr::VerifyMgr()
{
	m_ReportTimer.SetInterval(Avalon::Time::MSECS_OF_SEC * 10);
	m_StatistaicTimer.SetInterval(Avalon::Time::MSECS_OF_SEC * Avalon::Time::SECS_OF_MIN);
}

VerifyMgr::~VerifyMgr()
{

}

bool VerifyMgr::Init()
{
	if (!AvalonMonoDelegate::Instance()->Init("Assembly-CSharp.dll"))
	{
		ErrorLogStream << "init mono delegate failed." << LogStream::eos;
		return false;
	}

	if (!LogicClentInterface::Init())
	{
		ErrorLogStream << "init logic interface failed." << LogStream::eos;
		return false;
	}


	return true;
}

void VerifyMgr::Final()
{

}

void VerifyMgr::OnTick(const Avalon::Time& now)
{
	std::vector<VerifyTask*> needDelTasks;

	LC_Update(VSApplication::Instance()->MaxFramePerUpdate());

	for (auto itr : m_Tasks)
	{
		auto task = itr.second;
		if (task)
		{
			task->OnTick(now);
			if (task->GetStatus() == VERIFY_TASK_STATUS_FINISH)
			{
				needDelTasks.push_back(task);
			}
		}
	}

	for (auto task : needDelTasks)
	{
		DelTask(task->GetID());
	}

	if (m_ReportTimer.IsTimeout(now))
	{
		// 上报负载情况到Center
		ReportAbilityToCenter();
	}

	if (m_StatistaicTimer.IsTimeout(now))
	{
		InfoLogStream << "CURRENT NUM: " << (UInt32)m_Tasks.size() << LogStream::eos;
		InfoLogStream << m_Statistic.ToString() << LogStream::eos;
	}
}

VerifyTask* VerifyMgr::FindTask(ObjID_t id)
{
	auto itr = m_Tasks.find(id);
	return itr == m_Tasks.end() ? NULL : itr->second;
}

void VerifyMgr::AddTask(VerifyTask* task)
{
	if (!task)
	{
		return;
	}

	m_Tasks[task->GetID()] = task;
}

void VerifyMgr::DelTask(ObjID_t id)
{
	auto task = FindTask(id);
	if (task)
	{
		delete task;
		m_Tasks.erase(id);
	}
}

void VerifyMgr::ReportAbilityToCenter()
{
	CLProtocol::VerifyCenterReportAbility report;
	report.ability = VSApplication::Instance()->GetAbility();
	Router::SendToVerifyCenter(report);
}
