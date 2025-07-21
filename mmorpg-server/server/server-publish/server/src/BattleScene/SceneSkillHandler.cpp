#include "SceneSkillHandler.h"
#include "Player.h"
#include "CLErrorCode.h"


void HandleProtocol(Player* player,CLProtocol::SceneChangeSkillsReq& protocol)
{
	CLProtocol::SceneChangeSkillsRes res;
	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		res.result = player->GetPvpSkillMgr().ChangeSkills(protocol.changeSkills);
	}
	else
	{
		res.result = player->GetSkillMgr().ChangeSkills(protocol.changeSkills);
	}
	
	player->SendProtocol(res);
}

void HandleProtocol(Player* player,CLProtocol::SceneUseSkillsReq& protocol)
{
	CLProtocol::SceneUseSkillRet ret;
	ret.skillId = protocol.skillid;
	ret.timestamp = protocol.timestamp;
	ret.result = 0;
	if(player->UseSkill(protocol.skillid,protocol.pos,protocol.target))
	{
		ret.result = 1;
	}
	player->SendProtocol(ret);
	
	player->SetBusyTime();
}

void HandleProtocol(Player* player,CLProtocol::SceneReviveReq& protocol)
{
}

void HandleProtocol(Player* player, CLProtocol::SceneSwitchPkMode& protocol)
{
}

void HandleProtocol(Player* player, CLProtocol::SceneAddBuff& protocol)
{
	player->GetBuffMgr().AddBuff(protocol.buffId);
}

void HandleProtocol(Player* player, CLProtocol::SceneInitSkillsReq& protocol)
{
	CLProtocol::SceneInitSkillsRes res;
	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		res.result = player->GetPvpSkillMgr().InitAllSkills();
	}
	else
	{
		res.result = player->GetSkillMgr().InitAllSkills();
	}

	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneRecommendSkillsReq& protocol)
{
	CLProtocol::SceneRecommendSkillsRes res;
	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		res.result = player->GetPvpSkillMgr().RecommendSkills();
	}
	else
	{
		res.result = player->GetSkillMgr().RecommendSkills();
	}

	player->SendUdpLog("skill_recommend", LOG_SKILL_RECOMMEND, protocol.configType);

	player->SendProtocol(res);
}