#include "SceneRetinueHandler.h"
#include "Player.h"

void HandleProtocol(Player* player, CLProtocol::SceneChanageRetinueReq req)
{
	CLProtocol::SceneChanageRetinueRes res;
	res.result = player->GetRetinueMgr().SetRetinue(req.id, req.index);
	res.mainId = player->GetRetinueMgr().GetRetinueFighting();
	res.offRetinueIds = player->GetRetinueMgr().GetOffRetinueIds();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneRetinueChangeSkillReq req)
{
	CLProtocol::SceneRetinueChangeSkillRes res;
	res.result = player->GetRetinueMgr().ChangeSkill(req.id, req.skillIds);
	player->SendProtocol(res);

	std::ostringstream os;
	for (UInt32 i = 0; i < req.skillIds.size(); ++i)
	{
		os << req.skillIds[i] << ",";
	}
	DebugLogStream << PLAYERINFO(player) << "SceneRetinueChangeSkillReq req.id=" << req.id << " req.skillIds=" << os.str() << " errorcode=" << res.result << LogStream::eos;

}

void HandleProtocol(Player* player, CLProtocol::SceneRetinueUnlockReq req)
{
	CLProtocol::SceneRetinueUnlockRes res;
	res.result = player->GetRetinueMgr().AddRetinue(req.id, true);
	if (res.result == ErrorCode::SUCCESS)
	{
		res.dataId = req.id;
	}
	player->SendProtocol(res);
	DebugLogStream << PLAYERINFO(player) << "SceneRetinueUnlockReq req.id=" << req.id << " errorcode=" << res.result << LogStream::eos;
}

void HandleProtocol(Player* player, CLProtocol::SceneRetinueUpLevelReq req)
{
	CLProtocol::SceneRetinueUpLevelRes res;
	res.type = req.type;
	res.retinueId = req.id;
	if (req.type == CLProtocol::RetinueUpType::RUT_UPLEVEL)
	{
		res.result = player->GetRetinueMgr().UpLevelRetinue(req.id);
	}
	else if (req.type == CLProtocol::RetinueUpType::RUT_UPSTAR)
	{
		res.result = player->GetRetinueMgr().UpStarRetinue(req.id);
	}
	else
	{
		res.result = ErrorCode::RETINUE_UP_TYPE_ERROR;
	}
	
	player->SendProtocol(res);

	DebugLogStream << PLAYERINFO(player) << "SceneRetinueUpLevelReq req.id=" << req.id << " type=" << req.type << " errorcode=" << res.result << LogStream::eos;
}