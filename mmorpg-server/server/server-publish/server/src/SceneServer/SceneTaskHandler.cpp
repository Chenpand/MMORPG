#include "SceneTaskHandler.h"

#include "Scene.h"
#include "Player.h"
#include "TaskScript.h"
#include "ScriptMgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "CLMasterSystemDefine.h"
#include "PlayerLoadCallback.h"

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

	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneAcceptTask& req), player(role: " << player->GetID()  
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
	//	<< ", publish type:" << (UInt32)req.publishType << ", publish id:" << req.publishId << ", task:" << req.taskId << ")" << LogStream::eos;

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

	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitTask& req), player(role: " << player->GetID()
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
	//	<< ", npc:" << req.npcId << ", task:" << req.taskId << ")" << LogStream::eos;

	player->HandleProtocol(req);
}

void HandleProtocol(Player* player, CLProtocol::SceneAbandonTask& req)
{
	/*
	player->SuspendAction();
	player->AbandonTask(req.taskId);
	*/
	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneAcceptTask& req), player(role: " << player->GetID()
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
	//	<< ", task:" << req.taskId << ")" << LogStream::eos;

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

	//DebugLogStream << PLAYERINFO(pPlayer) << " SceneSubmitLegend taskId = " << req.taskId << LogStream::eos;
	pPlayer->SubmitLegendTask(req.taskId);
}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req)
{
	if (NULL == player)
	{
		return;
	}

	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req), player(role: " << player->GetID()
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
	//	<< ", task:" << req.taskId << ")" << LogStream::eos;

	BaseTask* pTask = player->GetDailyTaskMgr().FindTask(req.taskId);
	if (pTask != NULL)
	{
		player->GetDailyTaskMgr().SubmitTask(pTask);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitAllDailyTask& req)
{
	if (NULL == player) return;

	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitAllDailyTask& req), player(role: " << player->GetID()
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID() << ", submit all daily task:" << LogStream::eos;

	player->GetDailyTaskMgr().SubmitAllTask();
}

void HandleProtocol(Player* player, CLProtocol::SceneSubmitAchievementTask& req)
{
	if (NULL == player)
	{
		return;
	}

	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSubmitDailyTask& req), player(role: " << player->GetID()
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
	//	<< ", task:" << req.taskId << ")" << LogStream::eos;

	BaseTask* task = player->GetAchievementTaskMgr().FindTask(req.taskId);
	player->GetAchievementTaskMgr().SubmitTask(task);
}

void HandleProtocol(Player* player, CLProtocol::SceneSetTaskItemReq& req)
{
	if (NULL == player)
	{
		return;
	}

	//DebugLogStream << "In HandleProtocol(Player* player, CLProtocol::SceneSetTaskItem& req), player(role: " << player->GetID()
	//	<< ",accId:" << player->GetAccID() << "), protocol(id:" << req.GetProtocolID()
	//	<< ", task:" << req.taskId << ")" << LogStream::eos;

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
		ErrorLogStream << PLAYERINFO(player) << "SceneRefreshCycleTask error = " << result << LogStream::eos;
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

class PlayerLoadingFoPublishMasterTask : public PlayerLoadingCallback
{
public:
	PlayerLoadingFoPublishMasterTask(UInt64 masterId, UInt64 discipleId)
		 : m_masterId(masterId), m_discipleId(discipleId){}
	virtual void OnFinish(Player* player)
	{
		CLProtocol::SceneNotifyPublishMasterTaskResToWorld notify;
		notify.code = ErrorCode::SUCCESS;
		notify.masterId = m_masterId;
		notify.discipleId = m_discipleId;
		if (!player)
		{
			ErrorLogStream << "PlayerLoadingFoPublishMasterTask player null" << LogStream::eos;
			notify.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
			Router::SendToWorld(notify);
			return;
		}

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}

		if (!player->HasRelation(RELATION_MASTER, m_masterId))
		{
			InfoLogStream << "HandleProtocol WorldNotifyPublishMasterTaskToScene !disciple->HasRelation(RELATION_MASTER, req.masterId), masterId(" << m_masterId << ") discipleId(" << m_discipleId << ")" << LogStream::eos;
			//notify.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
			//Router::SendToWorld(notify);
			//return;
		}

		player->GetMasterDailyTaskMgr().AcceptAllTask();
		

		//同步师门任务离线数据
		CLProtocol::SceneSyncMasterTaskShareDataToWorld protocol;
		protocol.roleId = player->GetID();
		player->GetMasterTaskShareData(protocol.masterTaskShareData);
		Router::SendToWorld(protocol);
		player->SyncToRecord();

		Router::SendToWorld(notify);
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			player->SetGameStatus(PS_DESTORY);
		}
		CLProtocol::SceneNotifyPublishMasterTaskResToWorld notify;
		notify.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
		notify.masterId = m_masterId;
		notify.discipleId = m_discipleId;
		Router::SendToWorld(notify);
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	UInt64 m_masterId;
	UInt64 m_discipleId;
};

void HandleProtocol(CLProtocol::WorldNotifyPublishMasterTaskToScene& req)
{
	
	CLProtocol::SceneNotifyPublishMasterTaskResToWorld notify;
	notify.code = ErrorCode::SUCCESS;
	notify.masterId = req.masterId;
	notify.discipleId = req.discipleId;

	Player* disciple = PlayerMgr::Instance()->FindPlayer(req.discipleId);
	if (disciple == NULL)
	{
		InfoLogStream << "HandleProtocol WorldNotifyPublishMasterTaskToScene disciple == NULL, masterId(" << req.masterId << ") discipleId(" << req.discipleId << "), start off load" << LogStream::eos;
		//notify.code = ErrorCode::RELATION_DISCIPLE_NOTONLINE;
		//Router::SendToWorld(notify);
		//return;
		PlayerMgr::Instance()->LoadOfflinePlayerCb(req.discipleId, 0, new PlayerLoadingFoPublishMasterTask(req.masterId, req.discipleId));
		return;
	}

	if (!disciple->HasRelation(RELATION_MASTER, req.masterId))
	{
		InfoLogStream << "HandleProtocol WorldNotifyPublishMasterTaskToScene !disciple->HasRelation(RELATION_MASTER, req.masterId), masterId(" << req.masterId << ") discipleId(" << req.discipleId << ")" << LogStream::eos;
		//notify.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
		//Router::SendToWorld(notify);
		//return;
	}

	disciple->GetMasterDailyTaskMgr().AcceptAllTask();
	

	//同步师门任务离线数据
	CLProtocol::SceneSyncMasterTaskShareDataToWorld protocol;
	protocol.roleId = disciple->GetID();
	disciple->GetMasterTaskShareData(protocol.masterTaskShareData);
	Router::SendToWorld(protocol);
	disciple->SyncToRecord();

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

void HandleProtocol(CLProtocol::WorldAccountTaskCheckPakcageEnoughReq& req)
{
	DebugLogStream << "recv WorldAccountTaskCheckPakcageEnoughReq player id = " << req.roleid << ", taskId : " << req.taskId << LogStream::eos;
	auto player = PlayerMgr::Instance()->FindNormalPlayer(req.roleid);
	if (!player)
	{
		ErrorLogStream << "not find normal player id = " << req.roleid << LogStream::eos;
		return;
	}
	RewardGroup* rewardGroup = player->GetRewardGroup();
	rewardGroup->AddRewards(req.rewards);

	CLProtocol::WorldAccountTaskCheckPakcageEnoughRes res;
	res.resCode = player->CheckAddRewards(rewardGroup);
	res.roleid = req.roleid;
	res.taskId = req.taskId;
	res.rewards = req.rewards;
	req.SendBack(res);
}