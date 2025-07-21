#include "WorldTaskHandler.h"

#include "WSPlayerMgr.h"
#include "AccountAchievementTaskMgr.h"


void HandleProtocol(CLProtocol::WorldAccountTaskCheckPakcageEnoughRes& res)
{
	InfoLogStream << "recv WorldAccountTaskCheckPakcageEnoughRes resCode : " << res.resCode << ", player id : " << res.roleid  
		<< ", taskId : " << res.taskId << LogStream::eos;
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(res.roleid);
	if (!player)
	{
		ErrorLogStream << "WorldAccountTaskCheckPakcageEnoughRes not find normal player id = " << res.roleid << ", task id : "
			<< res.taskId << LogStream::eos;
		return;
	}
	if (res.resCode != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player id : " << res.roleid << ", accid : " << player->GetAccId() 
			<< ", WorldAccountTaskCheckPakcageEnoughRes res code is not success res.resCode = " << res.resCode << ", task id : "
			<< res.taskId << LogStream::eos;
		player->SendNotify(1000004);
		return;
	}

	auto account = player->GetAccountInfo();
	if (!account)
	{
		ErrorLogStream << "WorldAccountTaskCheckPakcageEnoughRes account is null role id = " << res.roleid << ", task id : "
			<< res.taskId << LogStream::eos;
		return;
	}
	account->OnCheckPackageEnoughForTaskRewardsRes(res.taskId, res.rewards);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSubmitAccountTask& protocol)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account)
	{
		ErrorLogStream << PLAYERINFO(player) << " not find accout " << LogStream::eos;
		return;
	}

	account->SubmitAccounTask(protocol.taskId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetTaskItemReq& protocol)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;

	CLProtocol::WorldSetTaskItemRes res;
	res.result = account->SetTaskItem(protocol.taskId, protocol.items);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " SceneSetTaskItemReq SetTaskItem Result = " << res.result << LogStream::eos;
		player->SendProtocol(res);
	}
}