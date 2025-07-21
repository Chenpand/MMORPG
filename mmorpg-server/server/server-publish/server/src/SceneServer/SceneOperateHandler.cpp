#include "SceneOperateHandler.h"

#include <TimeUtil.h>
#include "Player.h"
#include "ActivityMgr.h"
#include "PlayerMgr.h"
#include "ActiveRecordMgr.h"

void HandleProtocol(CLProtocol::WSSyncOpActivityData& req)
{
	ActivityMgr::Instance()->OnLoadOpActData(req.datas);
}


void HandleProtocol(CLProtocol::WSOpActivityChange& req)
{
	if (req.belongRole != 0)
	{
		// 玩家自身的活動

		auto player = PlayerMgr::Instance()->FindPlayer(req.belongRole);
		if (!player)
		{
			ErrorLogStream << "Can't find player(" << req.belongRole << ")!" << LogStream::eos;
			return;
		}

		if (!player->GetActiveTaskMgr().IsMeetOpActRequireByPlayer(&req.opActRegInfo))
		{
			return;
		}

		bool bDirty = false;
		bool stateChanged = false;

		auto opAct = player->GetPlayerActivityInfo().FindOpActivity(req.opActRegInfo.dataId);
		if (!opAct)
		{
			ErrorLogStream << PLAYERINFO(player) << " can't find opActivity(" << req.opActRegInfo.dataId << ")!" << LogStream::eos;
			return;
		}
		else
		{
			if (opAct->state != req.opActRegInfo.state)
			{
				stateChanged = true;
				bDirty = true;
			}

			if (*opAct != req.opActRegInfo)
			{
				*opAct = req.opActRegInfo;
				bDirty = true;
			}
		}

		if (stateChanged)
		{
			switch ((ActivityState)opAct->state)
			{
			case AS_IN:
				player->GetActiveTaskMgr().OnOpActOpen(opAct);
				break;

			case AS_END:
				player->GetActiveTaskMgr().OnOpActClosed(opAct);
				break;

			default:
				break;
			}

			InfoLogStream << PLAYERINFO(player) << " opAct(" << opAct->dataId << ")'s state change to " << opAct->state << ", st=" << TimeUtil::TimestampToStandardFormat(opAct->startTime)
				<< ", et=" << TimeUtil::TimestampToStandardFormat(opAct->endTime) << LogStream::eos;
		}

		if (bDirty)
		{
			CLProtocol::SCSyncOpActStateChange protocol;
			protocol.info = *opAct;
			player->SendProtocol(protocol);

			InfoLogStream << PLAYERINFO(player) << " opAct(" << opAct->dataId << ") sync" << LogStream::eos;
		}

		return;
	}

	ActivityMgr::Instance()->OnOpActStateChange(req.opActRegInfo);
}

void HandleProtocol(Player* player, CLProtocol::CSOpActTakeRewardReq& req)
{
	player->GetActiveTaskMgr().OnTakeOpActTaskReward(req.activityDataId, req.dataId, req.param);
}

void HandleProtocol(CLProtocol::CSCDKVerifyRes& res)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(res.playerUid);
	if (!player)
	{
		return;
	}

	if (res.result != 1)
	{
		player->SendNotify(res.errorMsg.c_str());
		return;
	}

	RewardGroup* grp = player->GetRewardGroup();
	if (!grp)
	{
		return;
	}

	grp->AddRewards(res.itemRewards);
	player->AddRewards(GetReason(SOURCE_CDK_GIVE).c_str(), grp, true);
}

void HandleProtocol(Player* player, CLProtocol::CSDeleteNotifyList& req)
{
	player->ReadNotify(req.notify.type, req.notify.param);
}

void HandleProtocol(Player* player, CLProtocol::SceneOpActivityTaskInfoReq& req)
{
	if (!player) return;

	CLProtocol::SCSyncOpActTaskChange res;
	player->GetActiveTaskMgr().OnOpActTasksReq(req.opActId, res.tasks);

	player->SendProtocol(res);

	InfoLogStream << PLAYERINFO(player) << "Request opAct(" << req.opActId << ") task info(size: " << (UInt32)res.tasks.size() << ")." << LogStream::eos;
}

void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarDiscountInfoReq& req)
{
	if (!player) return;

	UInt32 opActId = req.opActId;

	if (!ActivityMgr::Instance()->CheckOpActivityDataValid(player, opActId, OAT_ARTIFACT_JAR))
	{
		player->SyncArtifactJarDiscountInfo(opActId);
		return;
	}
	
	player->GetActivityAttributeMgr().SyncArtifactJarDiscountInfo(opActId);
}

void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarExtractDiscountReq& req)
{
	if (!player) return;

	UInt32 opActId = req.opActId;

	CLProtocol::SceneArtifactJarExtractDiscountRes res;
	res.opActId = opActId;
	res.errorCode = ErrorCode::SUCCESS;
	
	if (!ActivityMgr::Instance()->CheckOpActivityDataValid(player, opActId, OAT_ARTIFACT_JAR))
	{
		res.errorCode = ErrorCode::ITEM_ARTIFACT_JAR_ACTIVITY_CLOSE;
		player->SendProtocol(res);
		return;
	}

	if (!player->GetActivityAttributeMgr().CanExtractArtifactJarDiscount(opActId))
	{
		res.errorCode = ErrorCode::ITEM_ARTIFACT_JAR_DISCOUNT_EXTRACT_FAILE;
		player->SendProtocol(res);
		return;
	}

	UInt32 discountRate = 0;
	UInt32 effectTimes = 0;

	if (!VipDataEntryMgr::Instance()->RandArtifactJarDiscount(player->GetVipLvl(), discountRate, effectTimes))
	{
		ErrorLogStream << PLAYERINFO(player) << " rand artifact jar discount failed!" << LogStream::eos;
		res.errorCode = ErrorCode::ITEM_ARTIFACT_JAR_DISCOUNT_EXTRACT_FAILE;
		player->SendProtocol(res);
		return;
	}
	
	player->GetActivityAttributeMgr().OnExtractArtifactJarDiscount(opActId, discountRate, effectTimes);
	player->GetActivityAttributeMgr().SyncArtifactJarDiscountInfo(opActId);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::WorldFirstReturnSync& sync)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(sync.roleId);
	if (!player) return;

	player->GetActiveTaskMgr().HandleReturnActExpire();
}

void HandleProtocol(Player* player, CLProtocol::SceneWeekSignRewardReq& req)
{
	if (NULL == player) return;

	player->GetActiveTaskMgr().HandleWeekSignSum(req.opActType, req.weekId);
}

void HandleProtocol(CLProtocol::GASWeekSignLotteryRes& res)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(res.roleId);
	if (!player) return;

	player->GetActiveTaskMgr().HandleWeekSignLottery(res.actType, res.actId, res.rewardVec);
}

void HandleProtocol(Player* player, CLProtocol::SceneOpActivityAcceptTaskReq& req)
{
	player->GetActiveTaskMgr().AcceptOpActivityTask(req.opActId, req.taskId);
}

void HandleProtocol(Player* player, CLProtocol::SceneOpActivityGetCounterReq& req)
{
	CLProtocol::SceneOpActivityGetCounterRes res;
	res.opActId = req.opActId;
	res.counterId = req.counterId;
	res.counterValue = 0;

	OpActivityBehavior behavior = (OpActivityBehavior)req.counterId;
	switch (behavior)
	{
	case OP_ACTIVITY_BEHAVIOR_NULL:
		res.counterValue = OpActivityRecordMgr::Instance()->GetRecordValue(behavior, OP_ACTIVITY_BEHAVIOR_OWNER_ROLE, player->GetID(), req.opActId);
		break;

	default:
		res.counterValue = OpActivityRecordMgr::Instance()->GetRecordValue(behavior, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, player->GetAccID(), req.opActId);
		break;
	}

	player->SendProtocol(res);
}


void HandleProtocol(Player* player, CLProtocol::SceneChallengeScoreReq& req)
{
	CLProtocol::SceneChallengeScoreRet ret;
	ret.score = player->GetChallengeScore();
	player->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::SceneMonopolyPlayerRollNotify & req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == NULL)
	{
		InfoLogStream << "World MonopolyPlayerRollNotify but player not online,roleid is: " << req.roleId << LogStream::eos;
		return;
	}
	player->GetActiveTaskMgr().OnPlayerMonopolyRoll();
}
