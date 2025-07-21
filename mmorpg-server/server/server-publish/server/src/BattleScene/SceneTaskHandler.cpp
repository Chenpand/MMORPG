#include "SceneTaskHandler.h"

#include "Scene.h"
#include "Player.h"
#include "TaskScript.h"
#include "ScriptMgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "CLMasterSystemDefine.h"

void HandleProtocol(Player* player, CLProtocol::SceneVisitTask& req)
{
	
}

void HandleProtocol(Player* player, CLProtocol::SceneAcceptTask& req)
{
	/*
	player->SuspendAction();

	if(req.publishType == TASK_PUBLISH_NPC)
	{
		Npc* npc = player->GetScene()->FindNpc(req.publishId);
		if(npc == NULL) return;

		npc->AcceptTask(player, req.taskId);
	}
	else if(req.publishType == TASK_PUBLISH_ZONE)
	{
		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(req.taskId);
		if(script == NULL) return;

		if(script->CheckInTriggerZone(player->GetScene()->GetMapID(), player->GetPos()))
		{
			player->AcceptTask(req.taskId, 0);
		}
	}
	*/
	if (NULL == player)
	{
		return;
	}

	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneAcceptTask& req), player(role: " << player->GetID()  
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
		<< ", publish type:" << (UInt32)req.publishType << ", publish id:" << req.publishId << ", task:" << req.taskId << ")" << LogStream::eos;

	//player->AcceptTask(req.taskId, 0, false);
	player->HandleProtocol(req);

}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitTask& req)
{
	/*
	Npc* npc = player->GetScene()->FindNpc(req.npcId);
	if(npc == NULL) return;

	player->SuspendAction();
	npc->SubmitTask(player, req.taskId);
	*/
	if (NULL == player)
	{
		return;
	}

	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitTask& req), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
		<< ", npc:" << req.npcId << ", task:" << req.taskId << ")" << LogStream::eos;

	player->HandleProtocol(req);
}

void HandleProtocol(Player* player, CLProtocol::SceneAbandonTask& req)
{
	/*
	player->SuspendAction();
	player->AbandonTask(req.taskId);
	*/
	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneAcceptTask& req), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
		<< ", task:" << req.taskId << ")" << LogStream::eos;

	player->AbandonTask(req.taskId);
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneLegendTaskListReq &req)
{
	CLProtocol::SceneLegendTaskListRes res;
	pPlayer->GetLegendTaskList(res.tasks);
	pPlayer->SendProtocol(res);
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneSubmitLegendTask &req)
{
	// 安全锁开启，不允许传奇之路兑换
	if (!pPlayer->SecurityLockCheck())
		return;

	InfoLogStream << PLAYERINFO(pPlayer) << " SceneSubmitLegend taskId = " << req.taskId << LogStream::eos;
	pPlayer->SubmitLegendTask(req.taskId);
}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req)
{
	if (NULL == player)
	{
		return;
	}

	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
		<< ", task:" << req.taskId << ")" << LogStream::eos;

	BaseTask* pTask = player->GetDailyTaskMgr().FindTask(req.taskId);
	if (pTask != NULL)
	{
		player->GetDailyTaskMgr().SubmitTask(pTask);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitAllDailyTask& req)
{
	if (NULL == player) return;

	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitAllDailyTask& req), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID() << ", submit all daily task:" << LogStream::eos;

	player->GetDailyTaskMgr().SubmitAllTask();
}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitAchievementTask& req)
{
	if (NULL == player)
	{
		return;
	}

	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
		<< ", task:" << req.taskId << ")" << LogStream::eos;

	BaseTask* task = player->GetAchievementTaskMgr().FindTask(req.taskId);
	player->GetAchievementTaskMgr().SubmitTask(task);
}

void HandleProtocol(Player* player, CLProtocol::SceneSetTaskItemReq& req)
{
	if (NULL == player)
	{
		return;
	}

	InfoLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSetTaskItem& req), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
		<< ", task:" << req.taskId << ")" << LogStream::eos;

	UInt32 nResult = player->SetTaskItem(req.taskId, req.itemIds);

	if (nResult != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " SceneSetTaskItemReq SetTaskItem Result = " << nResult << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneRefreshCycleTask& req)
{
	if (NULL == player) return;

	UInt32 result = player->GetCycleTaskMgr().RefreshCycleTask();

	if (result != ErrorCode::SUCCESS)
	{
		InfoLogStream << PLAYERINFO(player) << "SceneRefreshCycleTask error = " << result << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneDailyScoreRewardReq& req)
{
	if (NULL == player) return;

	CLProtocol::SceneDailyScoreRewardRes res;
	res.result = player->ReceiveDailyTaskScoreReward(req.boxId);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneAchievementScoreRewardReq& req)
{
	if (NULL == player) return;

	CLProtocol::SceneAchievementScoreRewardRes res;
	res.result = player->ReceiveAchievementScoreReward(req.rewardId);
	player->SendProtocol(res);

}

void HandleProtocol(CLProtocol::WorldNotifyPublishMasterTaskToScene& req)
{
	
	CLProtocol::SceneNotifyPublishMasterTaskResToWorld notify;
	notify.code = ErrorCode::SUCCESS;
	notify.masterId = req.masterId;
	notify.discipleId = req.discipleId;

	Player* disciple = PlayerMgr::Instance()->FindNormalPlayer(req.discipleId);
	if (disciple == NULL)
	{
		InfoLogStream << "HandleProtocol WorldNotifyPublishMasterTaskToScene disciple == NULL, masterId(" << req.masterId << ") discipleId(" << req.discipleId << ")" << LogStream::eos;
		notify.code = ErrorCode::RELATION_DISCIPLE_NOTONLINE;
		Router::SendToWorld(notify);
		return;
	}

	if (!disciple->HasRelation(RELATION_MASTER, req.masterId))
	{
		InfoLogStream << "HandleProtocol WorldNotifyPublishMasterTaskToScene !disciple->HasRelation(RELATION_MASTER, req.masterId), masterId(" << req.masterId << ") discipleId(" << req.discipleId << ")" << LogStream::eos;
		notify.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
		Router::SendToWorld(notify);
		return;
	}

	disciple->GetMasterDailyTaskMgr().AcceptAllTask();
	Router::SendToWorld(notify);
}

void HandleProtocol(CLProtocol::WorldGetDiscipleMasterTasksToSceneReq& req)
{
	CLProtocol::WorldGetDiscipleMasterTasksToSceneRes notify;
	notify.masterId = req.masterId;
	notify.discipleId = req.discipleId;

	Player* disciple = PlayerMgr::Instance()->FindNormalPlayer(req.discipleId);
	if (disciple)
	{
		notify.code = ErrorCode::SUCCESS;
		disciple->GetMasterTaskShareData(notify.masterTaskShareData);
	}
	else
	{
		notify.code = ErrorCode::GETDISCIPLE_MASTERTASKS_FAIL;
	}
	Router::SendToWorld(notify);
}

void HandleProtocol(CLProtocol::WorldCheckMasterDailyTaskReqToScene& req)
{
	CLProtocol::SceneCheckMasterDailyTaskResToWorld notify;
	notify.discipleId = req.discipleId;

	Player* disciple = PlayerMgr::Instance()->FindPlayer(req.discipleId);
	if (disciple)
	{
		if (disciple->GetMasterDailyTaskMgr().CheckAllTasksFinished())
		{
			notify.completed = MDTCS_COMPLTETEED;
		}
		else
		{
			notify.completed = MDTCS_NOTCOMPLTETEED;
		}
	}
	else
	{
		notify.completed = MDTCS_NOTCOMPLTETEED;
	}

	Router::SendToWorld(notify);
}

void HandleProtocol(CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene& req)
{
	CLProtocol::SceneNotifyGiveMasterSysGiftResToWorld notify;
	notify.masterId = req.masterId;
	notify.type = req.type;
	notify.discipleId = req.discipleId;
	notify.giftDataId = req.giftDataId;

	Player* player = NULL;
	if (req.type == MasterSysGiftType::MSGT_MASTER_DAILYTASK 
		|| req.type == MasterSysGiftType::MSGT_FISNISHCL_MASTER)
	{
		player = PlayerMgr::Instance()->FindPlayer(req.masterId);
	}
	else if (req.type == MasterSysGiftType::MSGT_DISCIPLE_DAILYTASK
		|| req.type == MasterSysGiftType::MSGT_DISCIPLE_ACADEMIC
		|| req.type == MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE)
	{
		player = PlayerMgr::Instance()->FindPlayer(req.discipleId);
	}

	if (player)
	{
		notify.code = player->OnMasterSysGiveGift(req.type, req.giftDataId);
	}
	else
	{
		notify.code = ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
	}
	Router::SendToWorld(notify);
}