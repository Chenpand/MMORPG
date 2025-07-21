#include "SceneLostDungeonHandle.h"
#include "CLLostDungeonProtocol.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "ActivityMgr.h"

void HandleProtocol(Player* player, CLProtocol::LostDungeonChallengeReq& req)
{
	CLProtocol::LostDungeonChallengeRes res; 
	res.code = player->GetLostDungeon().OnPlayerChallengeReq(req.floor, req.hardType);
	if (res.code != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::LostDungeonOpenReq& req)
{
	CLProtocol::LostDungeonOpenRes res;
	if (!ActivityMgr::Instance()->IsInActivity(ACTIVITY_LOST_DUNGEON))
	{
		res.code = ErrorCode::LOSTDUNGEON_FAIL_ACT_CLOSE;
		player->SendProtocol(res);
		return;
	}
	
	UInt32 secs = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_CANT_ENTER_TIME);
	secs = (secs == 0) ? 10 : secs;

	UInt32 startTime = 0;
	UInt32 endTime = 0;
	std::string actName = ACTIVITY_LOST_DUNGEON;
	ActivityMgr::Instance()->GetOpenActivityTime(actName, startTime, endTime);
	if (CURRENT_TIME.Sec() + secs > endTime)
	{
		res.code = ErrorCode::LOSTDUNGEON_CANT_ENTER_BAT_BEFEND;
		player->SendProtocol(res);
		return;
	}

	UInt8 lost_dugeon_st = player->GetCounter(COUNTER_LOSTDUNG_STATE);
	if (lost_dugeon_st == LDS_CLOSE)
	{
		//¼ì²é´ÎÊý
		if (player->GetCounter(COUNTER_LOSTDUNG_REMAIN_NUM) < 1)
		{
			res.code = ErrorCode::LOSTDUNGEON_OPEN_FAIL_NOTIMES;
			player->SendProtocol(res);
			return;
		}
		res.code = player->GetLostDungeon().OnOpen();
		if (res.code == ErrorCode::SUCCESS)
		{
			player->GetLostDungeon().GetFloorDatas(res.floorDatas);
		}
		player->SendProtocol(res);
	}
	else if (lost_dugeon_st == LDS_PROGRESSING || lost_dugeon_st == LDS_END_CAN_REWARD)
	{
		res.code = ErrorCode::SUCCESS;
		player->GetLostDungeon().GetFloorDatas(res.floorDatas);
		player->SendProtocol(res);
	}
	else
	{
		ErrorLogStream << "state of lost dungeon is err st : " << (UInt32)lost_dugeon_st 
			<< ", player id : " << player->GetID() << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::LostDungeonOpenBoxReq& req)
{
	CLProtocol::LostDungeonOpenBoxRes res;
	res.code = player->GetLostDungeon().OnPlayerOpenBox(req.floor, req.boxId, res.itemVec);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonBoxChoiceItemReq& req)
{
	CLProtocol::SceneLostDungeonBoxChoiceItemRes res;
	res.retCode = player->GetLostDungeon().OnPlayerChoiceBoxItem(req.itemId, res.item);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneLostDungeonSettleFloor& req)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(req.roleId);
	if (!player)
	{
		ErrorLogStream << "not find player id : "  << req.roleId << ", floor : " << req.floor << LogStream::eos;
		return;
	}
	player->GetLostDungeon().SettleFloor(req.floor, req.battleResult, req.score, req.btState);
}

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonGetRewardReq& req)
{
	CLProtocol::SceneLostDungeonGetRewardRes res;
	res.code = player->GetLostDungeon().GetReward();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonGiveUpReq& req)
{
	CLProtocol::SceneLostDungeonGiveUpRes res;
	res.code = player->GetLostDungeon().OnPlayerGiveUp();
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::BattleLostDungSyncDataToSelfScene& req)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(req.roleId);
	if (!player)
	{
		ErrorLogStream << "not find player id : " << req.roleId  << LogStream::eos;
		return;
	}
	player->SetLostDungeonHp(0);
	player->SetLostDungeonMp(0);
	player->SetLostDungeonHp(req.lostDungeonHp);
	player->SetLostDungeonMp(req.lostDungeonMp);
	player->GetLostDungeon().SaveDirtyData();
}