
#include "WSPlayerMgr.h"
#include "HireMgr.h"
#include "WorldHireHandler.h"
#include "WSRouter.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireInfoReq &req)
{
	CLProtocol::WorldQueryHireInfoRes res;
	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return;
	}
	res.identity = info->GetIdentify();
	res.code = info->GetCode();
	res.isBind = info->GetBindAccid() > 0 ? 1 : 0;
	res.isOtherBindMe = info->GetBindMeSet().size() > 0 ? 1 : 0;
 	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldUseHireCodeReq &req)
{
	CLProtocol::WorldUseHireCodeRes res;
	res.errorCode = HireMgr::Instance()->OnPlayerUseCode(player, req.code);
	if (res.errorCode != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryTaskStatusReq &req)
{
	CLProtocol::WorldQueryTaskStatusRes res;
	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		return;
	}

	auto taskList = info->GetHireTaskList();

	for (auto &task : taskList)
	{
		HireInfoData data;
		data.taskID = task->taskID;
		data.cnt = task->cnt;
		data.status = task->status;
		res.hireTaskInfoList.push_back(data);
	}

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireTaskAccidListReq &req)
{
	CLProtocol::WorldQueryHireTaskAccidListRes res;
	auto info = HireMgr::Instance()->GetHireInfo(player->GetAccId());
	if (info == nullptr)
	{
		player->SendProtocol(res);
		return;
	}

	auto task = info->GetHireTask(req.taskId);
	if (task == nullptr)
	{
		player->SendProtocol(res);
		return;
	}

	for (auto &info : task->completeInfoVec)
	{
		res.nameList.push_back(info.name);
	}

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireListReq &req)
{
	CLProtocol::WorldQueryHireListRes res;
	res.hireList = HireMgr::Instance()->OnPlayerQueryHireList(player);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSubmitHireTaskReq &req)
{
	CLProtocol::WorldSubmitHireTaskRes res;
	res.taskId = req.taskId;
	res.errorCode = HireMgr::Instance()->OnPlayerSubmitTask(player, req.taskId);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldQueryHireCoinReq & req)
{
	player->SyncAccountCounterToClient(ACC_COUNTER_HIRE_COIN);
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldQueryHirePushReq & req)
{
	if (req.type == 0)
	{
		player->SyncAccountCounterToClient(ACC_COUNTER_HIRE_PUSH);
	}
	else if (req.type == 1)
	{
		if (player->GetAccountInfo() == nullptr)
		{
			return;
		}
		player->GetAccountInfo()->SetAccountCounterNum(player, ACC_COUNTER_HIRE_PUSH, 1);
	}
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldQueryHireAlreadyBindReq& req)
{
	req.accid = player->GetAccId();
	req.platform = player->GetPf();
	req.zoneId = ZONE_ID;
	Router::SendToGlobalActivity(req);
}

void HandleProtocol(CLProtocol::WorldQueryHireAlreadyBindRes & req)
{
	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(req.accid);
	if (player == nullptr)
	{
		ErrorLogStream << "Hire Player bind not online accid is:" << req.accid << LogStream::eos;
	}
	player->SendProtocol(req);
}

void HandleProtocol(CLProtocol::WorldOnSceneTrigeTask &req)
{
	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(req.accid);
	if (player != nullptr)
	{
		HireMgr::Instance()->OnPlayerTrigeTask(player, static_cast<HireTaskType>(req.type), req.value, req.teammateIds);
	}
}

void HandleProtocol(CLProtocol::GASCheckHireBindRet & req)
{
	DebugLogStream << "receive GASCheckHireBindRet accid is " << req.accid << " code is " << req.code << LogStream::eos;
	CLProtocol::WorldUseHireCodeRes res;
	res.errorCode = req.errorCode;
	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(req.accid);

	if (player == nullptr)
	{
		ErrorLogStream << "Hire Player bind not online accid is:" << req.accid << LogStream::eos;
		return;
	}

	if (req.errorCode != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
		return;
	}

	res.errorCode = HireMgr::Instance()->OnPlayerUseCodeSuccess(player, req.code);
	DebugLogStream << "OnPlayerUseCodeSuccess errorcode is " << res.errorCode << LogStream::eos;
	player->SendProtocol(res);
}

