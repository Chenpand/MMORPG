#include "VerifyHandler.h"

#include <CLErrorCode.h>

#include "VerifyMgr.h"
#include "VerifyTask.h"
#include "VSApplication.h"

void HandleProtocol(CLProtocol::VerifyCreateTaskReq& req)
{
	CLProtocol::VerifyCreateTaskRes res;
	res.raceId = req.raceId;

	// 查看验证服务器还能不能验证
	if (VSApplication::Instance()->GetAbility() == 0)
	{
		ErrorLogStream << "verify server too busy, cant create task(" << req.raceId << ")." << LogStream::eos;
		res.result = ErrorCode::VERIFY_BUSY;
		req.SendBack(res);

		// 更新自己的负载到中心服务器
		VerifyMgr::Instance()->ReportAbilityToCenter();
		return;
	}

	auto task = VerifyMgr::Instance()->FindTask(req.raceId);
	if (task)
	{
		// 重复创建
		ErrorLogStream << "repeat create verify task(" << req.raceId << ")." << LogStream::eos;
		res.result = ErrorCode::VERIFY_REPEAT;
		req.SendBack(res);
		return;
	}

	task = new VerifyTask((RaceType)req.raceType, req.raceId, req.GetConnID());
	if (!task->OnInit(req.initPacket))
	{
		ErrorLogStream << "verify task(" << req.raceId << ") type(" << req.raceType << ") init failed." << LogStream::eos;
		res.result = ErrorCode::VERIFY_INIT_FAILED;
		req.SendBack(res);
		delete task;
		return;
	}

	VerifyMgr::Instance()->AddTask(task);

	res.result = ErrorCode::SUCCESS;
	req.SendBack(res);
}

void HandleProtocol(CLProtocol::VerifyReportFrame& report)
{
	auto task = VerifyMgr::Instance()->FindTask(report.raceId);
	if (!task)
	{
		return;
	}

	task->PushFrame(report.frames);
}

void HandleProtocol(CLProtocol::VerifyReportChecksum& report)
{
	auto task = VerifyMgr::Instance()->FindTask(report.raceId);
	if (!task)
	{
		ErrorLogStream << "can't find task(" << report.raceId << ")." << LogStream::eos;
		return;
	}

	for (auto& info : report.playerChecksums)
	{
		for (auto& checksum : info.checksums)
		{
			task->SetPlayerChecksum(info.roleId, checksum.frame, checksum.checksum);
		}
	}
}

void HandleProtocol(CLProtocol::VerifyReportRaceEnd& report)
{
	auto task = VerifyMgr::Instance()->FindTask(report.raceId);
	if (!task)
	{
		return;
	}

	InfoLogStream << "race(" << report.raceId << ") player(" << report.roleId << ") report race end." << LogStream::eos;
	task->SetPlayerRaceEndInfo(report.roleId, report.packet);
	
	report.Detach();
}

void HandleProtocol(CLProtocol::VerifyReportRaceDismiss& report)
{
	auto task = VerifyMgr::Instance()->FindTask(report.raceId);
	if (!task)
	{
		return;
	}

	task->SetStatus(VERIFY_TASK_WAIT_LOGIC_FINISH);
}

void HandleProtocol(CLProtocol::VerifyGiveupTask& protocol)
{
	auto task = VerifyMgr::Instance()->FindTask(protocol.raceId);
	if (!task)
	{
		return;
	}

	//task->SetStatus(VERIFY_TASK_WAIT_LOGIC_FINISH);
	task->GiveupVerify();
}