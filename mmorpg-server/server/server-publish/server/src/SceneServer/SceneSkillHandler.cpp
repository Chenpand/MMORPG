#include "SceneSkillHandler.h"
#include "Player.h"
#include "CLErrorCode.h"
#include "ServiceSwitchMgr.h"
#include "SSServiceSwitchMgr.h"


void HandleProtocol(Player* player,CLProtocol::SceneChangeSkillsReq& protocol)
{
	CLProtocol::SceneChangeSkillsRes res;
	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		res.result = player->GetPvpSkillMgr().ChangeSkills(protocol.changeSkills);
	}
	else if (protocol.configType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		res.result = player->GetEqualPvpSkillMgr().ChangeSkills(protocol.changeSkills);
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
	else if (protocol.configType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		res.result = player->GetEqualPvpSkillMgr().InitAllSkills();
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
	else if (protocol.configType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		res.result = player->GetEqualPvpSkillMgr().RecommendSkills();
	}
	else
	{
		res.result = player->GetSkillMgr().RecommendSkills();
	}

	player->SendUdpLog("skill_recommend", LOG_SKILL_RECOMMEND, protocol.configType);

	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneSetSkillPageReq & protocol)
{
	CLProtocol::SceneSetSkillPageRes res;
	res.result = ErrorCode::ITEM_DATA_INVALID;
	res.configType = protocol.configType;
	res.page = protocol.page;

	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SKILL_PAGE))
	{
		player->SendProtocol(res);
		return;
	}

	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		if (protocol.page >= player->GetPvpSkillMgr().GetSkillPageCnt())
		{
			player->SendProtocol(res);
			return;
		}
		player->GetPvpSkillMgr().SetCurrentSkillPage(protocol.page);
		player->GetPvpSpMgr().SetCurrentPage(protocol.page);
		player->GetPvpSkillBar().SetBarIndex(protocol.page);
	}
	else if (protocol.configType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		if (protocol.page >= player->GetEqualPvpSkillMgr().GetSkillPageCnt())
		{
			player->SendProtocol(res);
			return;
		}
		player->GetEqualPvpSkillMgr().SetCurrentSkillPage(protocol.page);
	}
	else
	{
		if (protocol.page >= player->GetSkillMgr().GetSkillPageCnt())
		{
			player->SendProtocol(res);
			return;
		}
		player->GetSkillMgr().SetCurrentSkillPage(protocol.page);
		player->GetSpMgr().SetCurrentPage(protocol.page);
		player->GetSkillBar().SetBarIndex(protocol.page);
	}
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneBuySkillPageReq & protocol)
{
	CLProtocol::SceneBuySkillPageRes res;
	res.result = ErrorCode::ITEM_DATA_INVALID;

	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SKILL_PAGE))
	{
		player->SendProtocol(res);
		return;
	}

	if (!player->IsChangeOccu(player->GetOccu()))
	{
		player->SendProtocol(res);
		return;
	}

	UInt32 costNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SKILL_PAGE_OPEN_CONSUME_NUM);
	if (player->GetItemNum(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SKILL_PAGE_OPEN_CONSUME_CURRENCY)) < costNum)
	{
		player->SendProtocol(res);
		return;
	}

	if (protocol.page >= CL_MAX_SKILL_PAGE)
	{
		player->SendProtocol(res);
		return;
	}


	if (protocol.configType == SKILL_CONFIG_PVE)
	{
		if (player->GetSkillMgr().GetSkillPageUnlockStatus(protocol.page))
		{
			player->SendProtocol(res);
			return;
		}
	}
	else if (protocol.configType == SKILL_CONFIG_PVP)
	{
		if (player->GetPvpSkillMgr().GetSkillPageUnlockStatus(protocol.page))
		{
			player->SendProtocol(res);
			return;
		}
	}
	else
	{
		player->SendProtocol(res);
		return;
	}

	player->RemoveItem(GetReason(SOURCE_TYPE_UNLOCK_SKILL_PAGE).c_str(), SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SKILL_PAGE_OPEN_CONSUME_CURRENCY), costNum);

	//if (protocol.configType == SKILL_CONFIG_PVE)
	{
		player->GetSkillMgr().UnlockNewPage();
	}
	//else if (protocol.configType == SKILL_CONFIG_PVP)
	{
		player->GetPvpSkillMgr().UnlockNewPage();
	}

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

