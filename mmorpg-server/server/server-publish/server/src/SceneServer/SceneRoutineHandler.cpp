#include "SceneRoutineHandler.h"
#include "PlayerMgr.h"
#include "ActivityMgr.h"

#include <CLGateProtocol.h>
#include <TimeUtil.h>

#include "SSRouter.h"


void HandleProtocol(Player* player, CLProtocol::SceneActiveDegreeReq& req)
{
	player->GetRoutineMgr().SendActiveDegreeInfo();
}

void HandleProtocol(Player* player, CLProtocol::SceneRoutineGetBoxReq& req)
{
	player->GetRoutineMgr().GetAward(req.index);
}

void HandleProtocol(Player* player, CLProtocol::SceneRoutineTodaySigninAwardReq& req)
{
	player->GetRoutineMgr().SendTodaySigninInfo();
}

void HandleProtocol(CLProtocol::WorldNotifySceneActivity& req)
{
	if (req.belongRole != 0)
	{
		auto player = PlayerMgr::Instance()->FindPlayer(req.belongRole);
		if (!player)
		{
			ErrorLogStream << "Can't find player(" << req.belongRole << ")!" << LogStream::eos;
			return;
		}

		bool bDirty = false;
		bool stateChanged = false;

		auto actInfo = player->GetPlayerActivityInfo().FindActivity(req.id);
		if (!actInfo)
		{
			ErrorLogStream << PLAYERINFO(player) << " can't find activity(" << req.id << ")!" << LogStream::eos;
			return;
		}
		else
		{
			if (actInfo->state != req.type)
			{
				actInfo->state = req.type;
				stateChanged = true;
				bDirty = true;
			}

			if (actInfo->name != req.name)
			{
				actInfo->name = req.name;
				bDirty = true;
			}

			if (actInfo->level != req.level)
			{
				actInfo->level = req.level;
				bDirty = true;
			}

			if (actInfo->preTime != req.preTime)
			{
				actInfo->preTime = req.preTime;
				bDirty = true;
			}

			if (actInfo->startTime != req.startTime)
			{
				actInfo->startTime = req.startTime;
				bDirty = true;
			}

			if (actInfo->dueTime != req.dueTime)
			{
				actInfo->dueTime = req.dueTime;
				bDirty = true;
			}
		}

		if (stateChanged && actInfo->state == AS_IN)
		{
			player->GetActiveTaskMgr().AcceptTask(actInfo->id);

			InfoLogStream << PLAYERINFO(player) << " activity(" << actInfo->id << ")'s state change to " << actInfo->state << ", st=" << TimeUtil::TimestampToStandardFormat(actInfo->startTime)
				<< ", et=" << TimeUtil::TimestampToStandardFormat(actInfo->dueTime) << LogStream::eos;
		}

		if (bDirty)
		{
			CLProtocol::WorldNotifyClientActivity notify1;
			notify1.id = actInfo->id;
			notify1.name = actInfo->name;
			notify1.type = actInfo->state;
			notify1.level = actInfo->level;
			notify1.preTime = actInfo->preTime;
			notify1.startTime = actInfo->startTime;
			notify1.dueTime = actInfo->dueTime;
			player->SendProtocol(notify1);

			InfoLogStream << PLAYERINFO(player) << " WorldNotifyClientActivity: activity(" << actInfo->id << ")' sync" << LogStream::eos;
		}

		return;
	}

	if(req.type == 1 || req.type == 2)
	{
		if(0 == req.tribeId)
		{
			ActivityMgr::Instance()->OpenActivity(req.id, req.name, req.level, req.preTime, req.startTime, req.dueTime, req.type);
		}
		else
		{
			ActivityMgr::Instance()->OpenTribeActivity(req.tribeId, req.name, req.level, req.dueTime);
		}
	}
	else if(req.type == 0)
	{
		if(0 == req.tribeId)
		{
			ActivityMgr::Instance()->CloseActivity(req.id);
		}
		else
		{
			ActivityMgr::Instance()->CloseTribeActivity(req.tribeId, req.name);
		}
	}
}

void HandleProtocol(CLProtocol::WorldSyncSceneActivities& req)
{
	ActivityMgr::Instance()->SetActivities(req.activities, req.tribeactivities);
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneActiveTaskSubmit& req)
{
	UInt8 bMakeup = 0xFF & (req.param1 >> 24);

	auto data = ActiveTaskDataEntryMgr::Instance()->FindEntry(req.taskId);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(pPlayer) << "Can not find active task data(" << req.taskId << ")!" << LogStream::eos;
		return; 
	}

	if (data->taskBelong > 0)
	{
		CLProtocol::GateTransmitPacket trans;
		trans.id = pPlayer->GetID();
		req.Encode(trans.packet);

		Router::SendToWorld(trans);
		return;
	}
	
	if (bMakeup > 0)
	{
		pPlayer->GetActiveTaskMgr().SubmitMakeUpTask(req.taskId, req.param1);
	}
	else
	{
		pPlayer->GetActiveTaskMgr().SubmitTask(req.taskId);
	}
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneSignInRp& req)
{
	if (!pPlayer) return;

	if (req.taskIds.empty())
	{
		ErrorLogStream << PLAYERINFO(pPlayer) << "resign failed, taskids is empty!" << LogStream::eos;
		return;
	}

	auto data = ActiveTaskDataEntryMgr::Instance()->FindEntry(req.taskIds[0]);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(pPlayer) << "Can not find active task data(" << req.taskIds[0] << ")!" << LogStream::eos;
		return;
	}

	if (data->taskBelong > 0)
	{
		CLProtocol::GateTransmitPacket trans;
		trans.id = pPlayer->GetID();
		req.Encode(trans.packet);

		Router::SendToWorld(trans);
		return;
	}

	pPlayer->GetActiveTaskMgr().SubmitTaskRp(req.taskIds);
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneRestTimeReq& req)
{
	CLProtocol::SceneRestTimeRet ret;
	pPlayer->GetActiveTaskMgr().GetRestTime(ret.time1, ret.time2);

	Avalon::Time nextTm = Avalon::Time(CURRENT_TIME.MSec() + DAY_TO_SEC * SEC_TO_MSEC);
	Avalon::Date nextDt = Avalon::Date(nextTm);
	nextDt.Hour(0);
	nextDt.Min(0);
	nextDt.Sec(0);
	nextDt.MSec(0);

	UInt32 nextZeroTime = nextDt.ToTime().Sec();
	ret.time3 = nextZeroTime - CURRENT_TIME.Sec();
	pPlayer->SendProtocol(ret);
}


void HandleProtocol(Player* pPlayer, CLProtocol::SceneNewSignIn& req)
{
	CLProtocol::SceneNewSignRet ret;
	if (req.isAll)
	{
		ret.errorCode = pPlayer->SignMonthAll(req.day);
	}
	else
	{
		ret.errorCode = pPlayer->SignMonth(req.day);
	}
	
	pPlayer->SendProtocol(ret);
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneNewSignInQuery& req)
{
	pPlayer->SendSignStatus();
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneNewSignInCollect& req)
{
	CLProtocol::SceneNewSignInCollectRet ret;
	ret.errorCode = pPlayer->SignCollect(req.day);
	pPlayer->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::WorldActivityTaskSendRewards& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		ErrorLogStream << "Failed to find normal player(" << protocol.roleId << ")!" << LogStream::eos;
		return;
	}

	std::string reason = GetReason(SOURCE_TYPE_TASK_ACTIVE, protocol.taskId);

	RewardGroup* grp = player->GetRewardGroup();
	for (auto reward : protocol.rewards)
	{
		grp->AddReward(reward.id, reward.num, 0, reward.strenth, 0, static_cast<EquipType>(reward.equipType));
	}

	player->AddRewards(reason.c_str(), grp, (protocol.notify > 0 ? true : false));
}
