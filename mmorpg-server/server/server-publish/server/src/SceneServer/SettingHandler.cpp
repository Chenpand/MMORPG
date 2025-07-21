#include "SettingHandler.h"

#include <CLUtf8Define.h>
#include <CLForbitWord.h>
#include <CLRecordClient.h>
#include <CLSysNotify.h>
#include <CLQuickBuyDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLMallShopMultiIDataEntry.h>

#include "PlayerMgr.h"
#include "SSRouter.h"


/**
 *@brief 改名回调
 */
class ChangeNameCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(ChangeNameCallback, Avalon::Mutex)
public:
	ChangeNameCallback(ObjID_t playerId, const std::string& newname, UInt64 itemUid)
		:m_PlayerId(playerId), m_NewName(newname), m_ItemUid(itemUid){}

	void OnFinished()
	{
		CLProtocol::SceneChangeNameRes res;

		Player* player = PlayerMgr::Instance()->FindPlayer(m_PlayerId);
		if(player != NULL)
		{
			player->SetName(m_NewName);
			player->SyncRedPacketInfoToCenter();
		}
		player = PlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if(player != NULL)
		{
			if (m_ItemUid > 0)
			{
				Item* item = player->FindItem(m_ItemUid);
				if (!item)
				{
					res.code = ErrorCode::ITEM_DATA_INVALID;
					player->SendProtocol(res);
					return;
				}
				player->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_USE_CHANGE_NAME_CARD, item->GetDataID()).c_str(), item);
			}
			else
			{
				//扣钱
				UInt32 cardId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CHANGE_PLAYERNAME_CARD);

				auto quickData = QuickBuyDataEntryMgr::Instance()->FindEntry(cardId);
				if (!quickData)
				{
					res.code = ErrorCode::ITEM_DATA_INVALID;
					player->SendProtocol(res);
					return;
				}

				//钱不够
				if (quickData->costNum > player->GetItemNum(quickData->costItemId))
				{
					res.code = ErrorCode::SETTING_CHANGENAME_BUYCARD_NOT_ENOUGH_MONEY;
					player->SendProtocol(res);
					return;
				}

				if (quickData->costItemId == ITEM_POINT)
				{
					UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE);
					UInt8 multiple = 0;
					UInt32 endTime = 0;
					MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(quickData->id, multiple, endTime);
					player->AddMallPoint(GetReason().c_str(), quickData->costNum * (quickData->multiple + multiple) * rate);
				}

				player->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_USE_CHANGE_NAME_CARD, quickData->costItemId).c_str(), quickData->costItemId, quickData->costNum);

			}
			
			player->OnNameChanged();
		}

		CLProtocol::SceneNotifyChangeNameToWorld notify;
		notify.playerId = m_PlayerId;
		notify.newName = m_NewName;
		Router::SendToWorld(notify);

		res.code = ErrorCode::SUCCESS;
		if (player)
		{
			player->SendProtocol(res);
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if(player != NULL)
		{
			CLProtocol::SceneChangeNameRes res;
			res.code = ErrorCode::SETTING_CHANGENAME_FAIL;
			player->SendProtocol(res);
		}
	}
private:
	//玩家id
	ObjID_t	m_PlayerId;
	//新名字
	std::string	m_NewName;
	//改名卡uid
	UInt64 m_ItemUid;
};

/**
*@brief 查询曾用名回调
*/
class QueryUsedNameCb : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(QueryUsedNameCb, Avalon::Mutex)
public:
	QueryUsedNameCb(ObjID_t playerId, std::string newName, CLProtocol::SceneChangeNameReq& protocol)
		:m_PlayerId(playerId), m_NewName(newName), m_Req(protocol){}

	void OnFinished()
	{
		CLProtocol::SceneChangeNameRes res;
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if (!player)
		{
			ErrorLogStream << "player:" << m_PlayerId << " after QueryUsedNameCb can not find self!" << LogStream::eos;
			return;
		}

		//检查消耗
		//使用改名卡
		if (m_Req.itemUid > 0)
		{
			Item* card = player->FindItem(m_Req.itemUid);
			if (!card)
			{
				res.code = ErrorCode::SETTING_CHANGENAME_CARD_NOT_ENOUGH_ERROR;
				player->SendProtocol(res);
				return;
			}

			if (card->GetDataEntry()->subType != ST_CHANGE_NAME || card->GetNum() == 0)
			{
				res.code = ErrorCode::SETTING_CHANGENAME_CARD_NOT_ENOUGH_ERROR;
				player->SendProtocol(res);
				return;
			}
		}
		else
		{
			//使用钱
			UInt32 cardId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CHANGE_PLAYERNAME_CARD);

			//进行快速购买
			auto quickData = QuickBuyDataEntryMgr::Instance()->FindEntry(cardId);
			if (!quickData)
			{
				res.code = ErrorCode::ITEM_DATA_INVALID;
				player->SendProtocol(res);
				return;
			}

			//钱不够
			if (quickData->costNum > player->GetItemNum(quickData->costItemId))
			{
				res.code = ErrorCode::SETTING_CHANGENAME_BUYCARD_NOT_ENOUGH_MONEY;
				player->SendProtocol(res);
				return;
			}
		}

		UInt8 isInsert = 1;
		// 没有曾用名需要插入名字表
		if (NextRow())
		{
			isInsert = 0;
		}

		CLChangeNameCommand* cmd = CLRecordClient::Instance()->PrepareChangeNameCommand(player->GetSrcZoneId(), "t_player_info", player->GetID(), m_NewName, isInsert);
		CLRecordClient::Instance()->SendCommand(cmd, new ChangeNameCallback(player->GetID(), m_NewName, m_Req.itemUid));
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "player:" << m_PlayerId << " QueryUsedName error! errcode:" << errorcode << LogStream::eos;
	}

	//玩家id
	ObjID_t	m_PlayerId;
	//要改的名字
	std::string m_NewName;
	CLProtocol::SceneChangeNameReq m_Req;
};

/**
*@brief 查询是否重名返回
*/
class SelectRepeatNameCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectRepeatNameCallback, Avalon::Mutex)
public:
	SelectRepeatNameCallback(ObjID_t guid)
		:m_PlayerId(guid) {}
	~SelectRepeatNameCallback() {}

public:
	void OnFinished()
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if (!player)
		{
			return;
		}

		CLProtocol::SceneCheckChangeNameRes res;

		if (!NextRow())
		{
			res.code = ErrorCode::SUCCESS;
			player->SendProtocol(res);
			return;
		}

		ObjID_t selGuid = GetKey();
		if (m_PlayerId != selGuid)
		{
			res.code = ErrorCode::SETTING_CHANGENAME_REPEAT_ERROR;
			player->SendProtocol(res);
			return;
		}
		else
		{
			res.code = ErrorCode::SUCCESS;
			player->SendProtocol(res);
			return;
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		GameErrorLogStream("Player") << "player(" << m_PlayerId << ","
			<< ") select repeat name failed." << LogStream::eos;
	}

private:
	//玩家id
	ObjID_t	m_PlayerId;
};

void HandleProtocol(Player* player, CLProtocol::SceneExchangeSkillBarReq& protocol)
{
	CLProtocol::SceneExchangeSkillBarRes res;
	//res.result = player->GetSkillBar().ExchangeGrid(protocol.sourceId, protocol.sourceSlot, protocol.targetId, protocol.targetSlot);
	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		res.result = player->GetPvpSkillBar().SetBars(protocol.skillBars.index, protocol.skillBars.skillBars);
	}
	else if (protocol.configType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		res.result = player->GetEqualPvpSkillBar().SetBars(protocol.skillBars.index, protocol.skillBars.skillBars);
	}
	else
	{
		res.result = player->GetSkillBar().SetBars(protocol.skillBars.index, protocol.skillBars.skillBars);
	}
	
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName()
			<< ") SceneExchangeSkillBarReq result=" << res.result << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSetWeaponBarReq& protocol)
{
	DebugLogStream << PLAYERINFO(player) << " revice SceneSetWeaponBarReq msg(" << protocol.index << "|" << protocol.weaponId << ")" << LogStream::eos;

	CLProtocol::SceneSetWeaponBarRes res;
	res.result = player->SetWeaponBarByIndex(protocol.index, protocol.weaponId);
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " revice SceneSetWeaponBarRes result = " << res.result << ")" << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSetPvpSkillConfigReq& protocol)
{
	return;
	DebugLogStream << PLAYERINFO(player) << " revice SceneSetPvpSkillConfigReq msg(" << protocol.isUsePvpConfig << ")" << LogStream::eos;

	player->SetCounter(PVP_SKILL_CONFIG, protocol.isUsePvpConfig);

	CLProtocol::SceneSetPvpSkillConfigRes res;
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << " revice SceneSetPvpSkillConfigRes result = " << res.result << ")" << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSetEqualPvpSkillConfigReq& protocol)
{
	DebugLogStream << PLAYERINFO(player) << " revice SceneSetEqualPvpSkillConfigReq msg(" << protocol.isSetedEqualPvPConfig << ")" << LogStream::eos;

	CLProtocol::SceneSetEqualPvpSkillConfigRes res;
	res.result = 0;

	if (protocol.isSetedEqualPvPConfig == EqualPvpSkillConfigReqType::EQUAL_PVP_SKILL_CONFIG_QUERY)
	{
		res.result = player->GetCounter(EQUAL_PVP_SKILL_CONFIG);
	}
	else if (protocol.isSetedEqualPvPConfig == EqualPvpSkillConfigReqType::EQUAL_PVP_SKIL_LCONFIG_SET)
	{
		if (!player->GetCounter(EQUAL_PVP_SKILL_CONFIG))
		{
			player->SetCounter(EQUAL_PVP_SKILL_CONFIG, protocol.isSetedEqualPvPConfig);
		}
		res.result = player->GetCounter(EQUAL_PVP_SKILL_CONFIG);
	}
	player->SendProtocol(res);
}


/*  by huchenhui 2016.07.22
void HandleProtocol(Player* player, CLProtocol::SceneExchangeItemBarReq& protocol)
{
	CLProtocol::SceneExchangeItemBarRes res;
	res.result = player->GetItemBar().ExchangeGrid(protocol.itemBar);
	player->SendProtocol(res);
}
*/

void HandleProtocol(Player* player, CLProtocol::SceneSaveOptionsReq& protocol)
{
	player->SetOptions(protocol.options);
}



void HandleProtocol(Player* player, CLProtocol::SceneCheckChangeNameReq& protocol)
{
	CLProtocol::SceneCheckChangeNameRes res;

	if (!CheckNameValid(protocol.newName.c_str()))
	{
		res.code = ErrorCode::SETTING_CHANGENAME_LEN_ERROR;
		player->SendProtocol(res);
		return;
	}

	char namebuf[NAME_LENGTH + 1];
	avalon_strncpy(namebuf, protocol.newName.c_str(), NAME_LENGTH);
	if (ForbitWordParser::Instance()->Parse(namebuf) != FORBIT_WORD_UNCHANGED)
	{
		res.code = ErrorCode::SETTING_CHANGENAME_FORBID_ERROR;
		player->SendProtocol(res);
		return;
	}

	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info_name");
	selectcmd->PutCond("name", std::string(namebuf));
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectRepeatNameCallback(player->GetID()));
}

void HandleProtocol(Player* player, CLProtocol::SceneChangeNameReq& protocol)
{
	CLProtocol::SceneChangeNameRes res;

	InfoLogStream << "player(" << player->GetID() << ") request change name to (" << protocol.newName << ")." << LogStream::eos;

	char namebuf[NAME_LENGTH + 1];
	avalon_strncpy(namebuf, protocol.newName.c_str(), NAME_LENGTH);
	if (ForbitWordParser::Instance()->Parse(namebuf) != FORBIT_WORD_UNCHANGED)
	{
		res.code = ErrorCode::SETTING_CHANGENAME_FORBID_ERROR;
		player->SendProtocol(res);
		return;
	}

	if (player->GetName().compare(namebuf) == 0)
	{
		res.code = ErrorCode::SETTING_CHANGENAME_UNCHANGE_ERROR;
		player->SendProtocol(res);
		return;
	}

	std::string newName(namebuf);

	//查看是否是曾用名
	CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_player_info_name");
	selCmd->PutCond("name", newName);
	CLRecordClient::Instance()->SendCommand(selCmd, new QueryUsedNameCb(player->GetID(), newName, protocol));
}

void HandleProtocol(Player* player, CLProtocol::SceneChangeSexReq& protocol)
{
	if(player->GetSrcZoneId() != 0)
	{
		player->SendNotify(1312);
		return;
	}

	if(!IsValidSex(protocol.sex) || player->GetSex() == protocol.sex) return;

	ItemDataEntry* itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(ITEM_CHANGESEX_ID);
	if(itemDataEntry == NULL) return;

	if(player->GetItemNum(ITEM_CHANGESEX_ID) < 1)
	{
		player->SendNotify(1307, itemDataEntry->name.c_str());
		return;
	}

	player->SetSex(protocol.sex);
	player->SyncToRecord();

	player->RemoveItem("change_sex", ITEM_CHANGESEX_ID, 1);

	CLProtocol::SceneNotifyChangeSexToWorld notify;
	notify.playerId = player->GetID();
	notify.sex = player->GetSex();
	Router::SendToWorld(notify);
}

void HandleProtocol(Player* player, CLProtocol::SceneNotifyAppOnPanel& protocol)
{
	// by huchenhui 2017.4.24
	/*
	if(player->SetActionFlag(ACTION_FLAG_APPONPANEL))
	{
		const SysNotify* title = SysNotifyMgr::Instance()->FindNotify(5001);
		const SysNotify* content = SysNotifyMgr::Instance()->FindNotify(5002);
		RewardGroup* group = player->GetRewardGroup();
		group->AddReward(11, 200, 1, 0);
		player->SendSysMail(title->format, content->format, 0, group);
	}
	*/
}

void HandleProtocol(Player* player, CLProtocol::SceneChangeOccu& protocol)
{
	player->ChangeOccu(protocol.occu);
}

void HandleProtocol(Player* player, CLProtocol::SceneNotifyLoadingInfo& protocol)
{
	player->SetClientLoading(protocol.isLoading);
	player->GetTeamCopyMgr().NotifyTeamCopyPlayerLoading(protocol.isLoading);
}

void HandleProtocol(Player* player, CLProtocol::SceneGameSetReq& protocol)
{
	player->SetGameSet(protocol.setType, protocol.setValue);
}

void HandleProtocol(Player* player, CLProtocol::SceneReportNotify& protocol)
{
	InfoLogStream << "player(" << player->GetID() << ") send report." << LogStream::eos;

	// 消耗活跃度
	auto actScore = player->GetCounter(DAILY_ACTIVE_SCORE);
	UInt32 needActScore = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_REPORT_CONSUME);
	if (needActScore > actScore)
	{
		player->SetCounter(DAILY_ACTIVE_SCORE, 0);
	}
	else
	{
		player->SetCounter(DAILY_ACTIVE_SCORE, actScore - needActScore);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneShortcutKeySetReq& protocol)
{
	CLProtocol::SceneShortcutKeySetRes res;
	res.info = protocol.info;
	res.retCode = player->SetShortcutKey(protocol.info.setType, protocol.info.setValue);

	player->SendProtocol(res);
}
