#include "SceneBattleHandler.h"

#include <CLSceneProtocol.h>
#include <CLChijiRewardDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLBattleDefine.h>

#include "SceneMgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"
#include "HonorMgr.h"
#include "BattleMgr.h"

void HandleProtocol(CLProtocol::BattleEnterSceneRes& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		// ���ͷ���,ToDo...
		return;
	}

	if (protocol.result != ErrorCode::SUCCESS)
	{
		// ���ͷ���,ToDo...
		return;
	}

	if (protocol.battleType == BTY_DILAO)
	{
		player->SetLostDungeonBattleId(protocol.battleId);
	}
	else if (protocol.battleType == BTY_CHIJI)
	{
	}
	
	player->SetBattleSceneNodeID(protocol.battleSceneNodeId);

	InfoLogStream << "player(" << player->GetID() << ") enter bscene, nodeid(" << protocol.battleSceneNodeId << ")." << LogStream::eos;

	// ͬ����world
	player->SyncEventToWorld(SET_ENTER_CROSS_SCENE, 0);
}

void HandleProtocol(CLProtocol::BattleExitSceneReq& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		// ���ͷ���,ToDo...
		ErrorLogStream << "not find player id : " << protocol.roleId  << ", battle type : " << protocol.battleType
			<< ", battle id : " << protocol.battleId << LogStream::eos;
		CLProtocol::BattleExitSceneRes res;
		res.roleId = protocol.roleId;
		res.result = ErrorCode::ITEM_INVALID_INDEX;
		//Router::SendToBScene(player->GetBattleSceneNodeID(), res);
		return;
	}

	//��ʧ����ս������,�ж��ܷ񸴻�
	if (protocol.battleType == BTY_DILAO)
	{
		if (protocol.battleDead == 1 
			&& player->GetLostDungeon().OnPlayerDeadInBattle(protocol.floor))
		{
			CLProtocol::BattleExitSceneRes res;
			res.roleId = player->GetID();
			res.result = ErrorCode::SUCCESS;

			res.battleType = protocol.battleType;
			res.battleResult = protocol.result;
			res.battleId = protocol.battleId;
			res.revive = 1;
			res.reviveItem = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_REVIVE_STONE_ITEM);

			UInt32 nodeId = player->GetBattleSceneNodeID();
			Router::SendToBScene(nodeId, res);
			return;
		}
		player->SetMoveSpeed(0);
		player->CountMoveSpeed();
		player->SetLostDungeonBattleId(0);
	}

	UInt32 nodeId = player->GetBattleSceneNodeID();
	player->SetBattleSceneNodeID(0);

	//֪ͨ���ؽ��볡��
	player->NotifyGateEnterScene();

	//֪ͨ�ͻ��˼��س�����
	player->NotifyLoadScene();

	//ͬ����������
	player->SyncAroundToMe(true);

	CLProtocol::BattleExitSceneRes res;
	res.roleId = player->GetID();
	res.result = ErrorCode::SUCCESS;

	res.battleType = protocol.battleType;
	res.battleResult = protocol.result;
	res.battleId = protocol.battleId;
	res.revive = 0;

	Router::SendToBScene(nodeId, res);

	//���µ���һ��
	if (protocol.battleType == BTY_CHIJI)
	{
		player->OnOnline();
	}

	InfoLogStream << "player(" << player->GetID() << ") return to scene from bscene(" << nodeId << ")." << LogStream::eos;

	// ͬ����world
	player->SyncEventToWorld(SET_EXIT_CROSS_SCENE, 0);

	// ֪ͨworld��ҵ�ǰ����
	if (player->GetScene())
	{
		CLProtocol::SceneNotifyWorldEnterScene notify;
		notify.roleId = player->GetID();
		notify.sceneId = player->GetScene()->GetID();
		Router::SendToWorld(notify);
	}
}

void HandleProtocol(CLProtocol::SceneBattleBalanceEnd& protocol)
{
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (NULL == player)
	{
		return;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_CHIJI_REWARD);
	// �����ҫ����
	player->IncChijiScore(protocol.score);
	player->AddCounterCoin(reason.c_str(), COUNTER_CHI_JI_COIN, protocol.score);

	UInt32 oddVal = 0;
	UInt32 maxVal = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PK_CHIJI_HONOR_MAX);
	UInt32 curVal = player->GetCounter(COUNTER_CHI_JI_HONOR);
	if (curVal < maxVal)
		oddVal = maxVal - curVal;

	if (protocol.getHonor > oddVal)
		protocol.getHonor = oddVal;

	if (protocol.getHonor != 0)
	{
		
		player->IncCounter(COUNTER_CHI_JI_HONOR, protocol.getHonor);
		player->AddHonor(reason.c_str(), protocol.getHonor);
	}

	if (protocol.rank == 1)
	{
		player->SendGuildEventToWorld(SysNotifyMgr::Instance()->MakeString(14009, player->GetName()));
	}

	player->GetHistoryHonor().OnPvP(PK_3V3_CHIJI, protocol.getHonor);
	player->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::BattleNotifyRankAward& protocol)
{
	auto rewardData = ChijiRewardDataEntryMgr::Instance()->GetChiJiRewardDataEntry(protocol.rank);
	if (!rewardData)
	{
		return;
	}

	// ����ͨ���ʼ�����
	std::string sender = SysNotifyMgr::Instance()->MakeString(10054);
	std::string title = SysNotifyMgr::Instance()->MakeString(10054 + rewardData->id);
	std::string content = SysNotifyMgr::Instance()->MakeString(10057 + rewardData->id);
	std::string reason = GetReason(LogSourceType::SOURCE_CHIJI_REWARD);

	RewardGroup group;
	group.AddReward(rewardData->rewardBoxID, 1);

	Sys::SendSysMail(protocol.playerId, sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), &group);
}

void HandleProtocol(CLProtocol::BattleOpenNotify& protocol)
{
	InfoLogStream << "chiji open time:" << protocol.openTime << LogStream::eos;

	BattleMgr::Instance()->SetOpenTime(protocol.openTime);
}