#include "WSActivityHandler.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "GameParamMgr.h"
#include "ScriptMgr.h"
#include "WorldPlayerScript.h"
#include "WSActivityMonsterMgr.h"
#include "AccountActivityTaskMgr.h"
#include "WSActivityMgr.h"

void HandleProtocol(CLProtocol::WorldSyncActivesCounter &protocol)
{	
// 	WSPlayer *pPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
// 	if (pPlayer == NULL) return;
// 	WorldPlayerScript *script = ScriptMgr::Instance()->GetPlayerScript();
// 	if (script)
// 	{
// 		if (protocol.type == 0)
// 		{
// 			script->OnUseFireworks(pPlayer->GetID(),pPlayer->GetName().c_str(),  protocol.value);
// 		}
// 		else if (protocol.type == 1)
// 		{
// 			script->OnAddCharm(pPlayer->GetID(),pPlayer->GetName().c_str(),  protocol.value);
// 		}
// 	}
}

void HandleProtocol(WSPlayer *player, CLProtocol::WorldNotifyCounterWindow &protocol)
{
	player->SetOpenFlag(protocol.type, protocol.flag);
	if (protocol.flag != 1) return;
	CLProtocol::WorldSyncActivesCounter info;
	info.type = protocol.type;
	if (info.type == 0)
		info.value = GameParamMgr::Instance()->GetValue("fireworks_counts");
	else if(info.type == 1)
		info.value = GameParamMgr::Instance()->GetValue("charm_counts");
	player->SendProtocol(info);
}

void HandleProtocol(WSPlayer *player, CLProtocol::WorldActivityMonsterReq &protocol)
{
	CLProtocol::WorldActivityMonsterRes res;
	res.activityId = protocol.activityId;
	for (auto id : protocol.ids)
	{
		res.monsters.push_back(WSActivityMonsterMgr::Instance()->GetActivityMonsterInfo(id));
	}
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneActiveTaskSubmit& req)
{
	if (!player) return;

	AccountActivityTaskMgr::Instance()->SubmitTask(player, req.taskId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneSignInRp& req)
{
	if (!player) return;

	AccountActivityTaskMgr::Instance()->ReSignInSevenTask(player, req.taskIds);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldActivityDetailReq& req)
{
	if (!player) return;

	CLProtocol::WorldActivityDetailRet res;

	auto actInfo = WSActivityMgr::Instance()->FindActivityInfo(req.activeName);
	if (actInfo)
	{
		res.code = ErrorCode::SUCCESS;
		res.id = actInfo->id;
		res.name = actInfo->name;
		res.level = actInfo->level;
		res.preTime = actInfo->preTime;
		res.startTime = actInfo->startTime;
		res.dueTime = actInfo->dueTime;
		player->SendProtocol(res);
		return;
	}

	auto regInfo = WSActivityMgr::Instance()->FindNearestActivityRegInfo(req.activeName);

	if (!regInfo)
	{
		res.code = ErrorCode::OPERATE_DATA_ERROR;
		player->SendProtocol(res);
		return;
	}

	UInt32 startTime = regInfo->startTime;

	res.code = ErrorCode::SUCCESS;
	res.id = regInfo->id;
	res.name = regInfo->name;
	res.level = regInfo->level;
	res.preTime = startTime; 
	res.startTime = startTime + regInfo->preTime;
	res.dueTime = startTime + regInfo->preTime + regInfo->lastTime;
	player->SendProtocol(res);
}
