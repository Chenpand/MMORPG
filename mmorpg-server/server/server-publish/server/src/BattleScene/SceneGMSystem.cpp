#include "SceneGMSystem.h"
#include <AvalonStringUtil.h>
#include <AvalonMemoryTracker.h>

#include <CLSortListProtocol.h>
#include <CLRelationProtocol.h>
#include <CLRoutineProtocol.h>
#include <CLChatProtocol.h>
#include <CLTeamProtocol.h>
#include <CLInviteGameProtocol.h>
#include <CLGateProtocol.h>
#include <CLRetinueProtocol.h>
#include <CLDungeonTimesDataEntry.h>
#include <CLUtf8Define.h>
#include <CLForbitWord.h>
#include <CLSettingProtocol.h>
#include <CLNpcDataEntry.h>
#include <CLDungeonProtocol.h>
#include <CLScoreWarProtocol.h>

#include "SSRouter.h"
#include "Player.h"
#include "Scene.h"
#include "ScriptMgr.h"
#include "PlayerMgr.h"
#include "ItemMaker.h"
#include "SceneSysNotify.h"
#include "PlayerScript.h"
#include "TeamMgr.h"
#include "ItemScript.h"
#include "SSDungeonConfigMgr.h"
#include "SceneMgr.h"
#include "CityMonster.h"
#include "StrengthenTicketPlanMgr.h"
#include "SSApplication.h"

void GMCmdHandler::TransmitToWorld(Player* player, const CLParameter& params)
{
	CLProtocol::SysTransmitGmCommand req;
	req.playerid = player->GetID();
	req.cmdname = GetCmdName();
	req.params = params;
	Router::SendToWorld(req);
}

void GMCmdHandler::BroadcastToScenes(Player* player, const CLParameter& params)
{
	CLProtocol::SysTransmitGmCommand req;
	req.playerid = player->GetID();
	req.cmdname = GetCmdName();
	req.params = params;
	Router::BroadcastToScene(req);
}

/**
 *@brief 加经验
 */
class AddExpCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "addexp"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 exp = params.GetParam("num");
		player->IncExp(GetReason(LogSourceType::SOURCE_TYPE_GM).c_str(), exp);
		//player->Levelup();

		return true;
	}
};

class UpLevelCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "uplevel"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt16 level = params.GetParam("num");
		if (level > 1 && level <= MAX_LEVEL) level = level - 1;
		UInt64 needExp = player->GetLevelExp((UInt8)level);
		player->IncExp(GetReason(SOURCE_TYPE_GM).c_str(), needExp);
		return true;
	}
};

/**
 *@brief 加银元
 */
class AddGoldCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addgold"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddGold(GetReason(LogSourceType::SOURCE_TYPE_GM).c_str(), num);
		
		return true;
	}
};

/**
*@brief 减少银元
*/
class RemoveSilverCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "removegold"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->RemoveGoldBoth(GetReason(SOURCE_TYPE_GM).c_str(), num);

		return true;
	}
};

class AddSPCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addsp"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddSP(GetReason(SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
 *@brief 加金元
 */
class AddPointCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addpoint"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddPoint(GetReason(SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
*@brief 加金罐积分
*/
class AddGoldJarPointCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addgoldjarpoint"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddGoldJarPoint(GetReason(SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
*@brief 加魔罐积分
*/
class AddMagJarPointCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addmagjarpoint"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddMagJarPoint(GetReason(SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
*@brief 改名回调
*/
class GMChangeNameCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(GMChangeNameCallback, Avalon::Mutex)
public:
	GMChangeNameCallback(ObjID_t playerId, const std::string& newname, UInt64 itemUid)
		:m_PlayerId(playerId), m_NewName(newname), m_ItemUid(itemUid){}

	void OnFinished()
	{
		CLProtocol::SceneChangeNameRes res;

		Player* player = PlayerMgr::Instance()->FindPlayer(m_PlayerId);
		if (player != NULL)
		{
			player->SetName(m_NewName);
			player->SyncRedPacketInfoToCenter();
		}
		player = PlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if (player != NULL)
		{
			if (m_ItemUid > 0)
			{
				player->ReduceItemNum("change_name", player->FindItem(m_ItemUid));
			}
			else
			{
				//扣钱
			}

			player->OnNameChanged();
		}

		player->SyncProperty();

		CLProtocol::SceneNotifyChangeNameToWorld notify;
		notify.playerId = m_PlayerId;
		notify.newName = m_NewName;
		Router::SendToWorld(notify);
	}

	void OnFailed(UInt32 errorcode)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if (player != NULL)
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
*@brief 加魔罐积分
*/
class ChangeNameCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "changename"; }
	const char* GetHelp() const { return " name=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		std::string newname = params.GetParam("name").GetString();
		
		CLProtocol::SceneChangeNameRes res;

		char namebuf[NAME_LENGTH + 1];
		avalon_strncpy(namebuf, newname.c_str(), NAME_LENGTH);
		if (ForbitWordParser::Instance()->Parse(namebuf) != FORBIT_WORD_UNCHANGED)
		{
			res.code = ErrorCode::SETTING_CHANGENAME_FORBID_ERROR;
			player->SendProtocol(res);
			return false;
		}

		if (player->GetName().compare(namebuf) == 0)
		{
			res.code = ErrorCode::SETTING_CHANGENAME_UNCHANGE_ERROR;
			player->SendProtocol(res);
			return false;
		}

		//检查消耗
		//使用改名卡
		if (false)
		{
			Item* card = player->FindItem(0);
			if (!card)
			{
				res.code = ErrorCode::SETTING_CHANGENAME_CARD_NOT_ENOUGH_ERROR;
				player->SendProtocol(res);
				return false;
			}

			if (card->GetNum() == 0)
			{
				res.code = ErrorCode::SETTING_CHANGENAME_CARD_NOT_ENOUGH_ERROR;
				player->SendProtocol(res);
				return false;
			}
		}
		else
		{
			//使用钱
			// 		if (money not enough)
			// 		{
			// 			res.code = ErrorCode::SETTING_CHANGENAME_BUYCARD_NOT_ENOUGH_MONEY;
			// 			player->SendProtocol(res);
			// 			return;
			// 		}

		}

		/*CLChangeNameCommand* cmd = CLRecordClient::Instance()->PrepareChangeNameCommand(player->GetSrcZoneId(), "t_player_info", player->GetID(), std::string(namebuf), 1);
		CLRecordClient::Instance()->SendCommand(cmd, new GMChangeNameCallback(player->GetID(), std::string(namebuf), 0));*/

		return true;
	}
};


class AddBindPointCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addbindpoint"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddBindPoint(GetReason(SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
 *@brief 加金元
 */
class AddBindGoldCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addbindgold"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddBindGold(GetReason(SOURCE_TYPE_GM).c_str(),num);

		return true;
	}
};

/**
*@brief 加疲劳值
*/
class AddFatigueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addfatigue"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddFatigue(GetReason(SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
*@brief 消耗疲劳值
*/
class RemoveFatigueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "removefatigue"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->RemoveFatigue(GetReason(LogSourceType::SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
*@brief 转职
*/
class ChangeOccuCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "changeoccu"; }
	const char* GetHelp() const { return " occu=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt8 occu = params.GetParam("occu");
		player->ChangeOccu(occu);
		return true;
	}
};

class AwakenCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "awaken"; }
	const char* GetHelp() const { return " // awaken"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		if (player->Awaken() == ErrorCode::SUCCESS)
		{
			return true;
		}
		return false;
	}
};

class AddVipExpHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addvipexp"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddVipExp("gm_command", num);
		return true;
	}

};

class RemovePointHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "removepoint"; }
	const char* GetHelp() const { return " num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->RemovePointBoth(GetReason(LogSourceType::SOURCE_TYPE_GM).c_str(), num);
		return true;
	}
};

/**
*@brief 设置新手引导
*/
class SetBootHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setboot"; }
	const char* GetHelp() const { return " id=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		/*
		for (UInt32 i = 1; i <= id; ++i)
		{
			player->SetBootFlag(i);
		}
		*/
		player->SetNewBoot(id);
		return true;
	}
};

class ResetBootHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resetboot"; }
	const char* GetHelp() const { return " id=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		//player->ResetBootFlag(id);
		player->SetNewBoot(id);
		return true;
	}
};

/**
*@brief 设置弱引导
*/
class SetBootFlagHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setbootflag"; }
	const char* GetHelp() const { return " id=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		player->SetBootFlag(id);
		return true;
	}
};

class ResetBootFlagHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resetbootflag"; }
	const char* GetHelp() const { return " id=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		player->ResetBootFlag(id);
		return true;
	}
};


/**
*@brief 加荣誉
*/
class AddHonourCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addhonour"; }

	bool Execute(Player *player, const CLParameter &params)
	{
		//UInt32 num = params.GetParam("num");
		

		return true;
	}
};

/**
 *@brief 加铁石                                                                     
 */
class AddIronStoneCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "addironstone"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddBindPoint(GetReason(SOURCE_TYPE_GM).c_str(), num);

		return true;
	}
};

/**
 *@brief 加灵石                                                                     
 */
class AddSpiritStoneCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "addspiritstone"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		//UInt32 num = params.GetParam("num");

		return true;
	}
};

/**
 *@brief 加玉石                                                                     
 */
class AddJadeStoneCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "addjadestone"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		//UInt32 num = params.GetParam("num");
		

		return true;
	}
};

/**
 *@brief 加道具
 */
class AddItemCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "additem"; }
	const char* GetHelp() const { return "id=xxxx num=xxx ql=xxx str=xxx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		UInt16 num = params.GetParam("num");
		if(num == 0) num = 1;
		UInt8 ql = params.GetParam("ql");
		UInt8 strengthen = params.GetParam("str");
		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_GM);

		if (player->CheckAddItem(id, num))
		{
			if (num == player->AddItem(GetReason(SOURCE_TYPE_GM).c_str(), id, num, ql, strengthen))
			{
				return true;
			}
		}

		return false;
	}
};

//卖道具
class SellItemCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "sellitem"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 uid = params.GetParam("uid");
		UInt16 num = params.GetParam("num");
		if (num == 0) num = 1;

		player->SellItem(uid, num);
		return true;
	}
};

//使用道具
class UseItemCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "useitem"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 uid = params.GetParam("uid");

		player->UseItem(uid,1,0,0);

		return true;
	}
};

//分解装备
class DecomposeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "decompose"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 uid = params.GetParam("uid");

		player->DecomposeEquip(uid);

		return true;
	}
};


//强化装备
class StrengthenCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "strenth"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 uid = params.GetParam("uid");
		UInt8 useUnbreak = params.GetParam("protect");

		player->StrengthenEquip(uid, (useUnbreak != 0), 0);

		return true;
	}
};

//强化消耗模拟
class StrengthSimCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "strfarseer"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 strLv = params.GetParam("strlv");
		UInt32 equipLv = params.GetParam("lv");
		UInt32 color = params.GetParam("color");
		UInt32 subType = params.GetParam("subtype");
		UInt32 destStrLv = params.GetParam("tostrlv");

		//参数合法性
		if (strLv > ITEM_STRENTH_MAX) return false;
		if (equipLv > 60) return false;
		if (color > ITEM_QUALITY_YELLOW) return false;
		if (subType > ST_EQUIP_TITLE) return false;
		if (destStrLv == 0 || destStrLv > ITEM_STRENTH_MAX) return false;

		//output
		UInt32 costGold = 0;
		UInt32 costColor = 0;
		UInt32 costColorless = 0;
		UInt32 useProtectNum = 0;
		UInt32 strNum = 0;
		UInt32 failNum = 0;
		UInt32 fix1Ct = 0;
		UInt32 fix2Ct = 0;
		UInt32 lv2FailNum[ITEM_STRENTH_MAX];
		for (int i = 0; i < ITEM_STRENTH_MAX; ++i)
			lv2FailNum[i] = 0;

		char output[1024];

		while (strLv < destStrLv)
		{
			if (!player->SimStrength(strLv, equipLv, color, subType, destStrLv,
				fix1Ct, fix2Ct, strNum, failNum, lv2FailNum, useProtectNum, costGold, costColor, costColorless))
				return false;
		}

		sprintf(output, "costGold = %u, costColor = %u, costColorless = %u, strNum = %u, failNum = %u", 
			costGold, costColor, costColorless, strNum, failNum);

		player->Speak(CHAT_CHANNEL_AROUND, std::string(output), 0);

		DebugLogStream << output << LogStream::eos;

		return true;
	}
};

//开罐子样本模拟
class OpenJarSampleCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ojsmp"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 type = params.GetParam("type");
		UInt8 combo = params.GetParam("combo");
		UInt32 num = params.GetParam("num");

		player->OpenJarSample(type, combo, num);
		//auto thread = new Task(player, type, combo, num);
		//thread->Activate();
		return true;
	}
};

//装备附魔
class AddMagicCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addmagic"; }

	bool Execute(Player* player, const CLParameter& params)
	{
// 		UInt64 uid = params.GetParam("uid");
// 		UInt64 cardUid = params.GetParam("carduid");
// 
// 		player->AddMagic(cardUid, uid);

		return true;
	}
};

//批量加测试道具(版署版本test)
class BatchAddItem : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "bs"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		ItemScript* script = ScriptMgr::Instance()->FindItemScript(900000000);
		if (!script)
			return false;

		UInt32 ret = script->OnUseSelf(player, NULL);
		if (ret != 0)
			return false;
		
		return true;
	}
};

//开罐子
class OpenJar : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "openjar"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 type = params.GetParam("type");
		UInt8 combo = params.GetParam("combo");
		UInt32 opActId = params.GetParam("opActId");

		player->OpenJar(type, combo, opActId);

		return true;
	}
};

//重置装备回收
class ResetEquipRecoHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resetEqreco"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->GetItemMgr().resetEqRec(true);

		return true;
	}
};

//测试GetStrMaxLv接口结果
class TestGetStrMaxLv : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "getsml"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		/*
		UInt8 num = params.GetParam("num");
		UInt8 lv = params.GetParam("lv");
		UInt8 maxLv = player->GetStrMaxLv(num, lv);
		*/
		return true;
	}
};

//附魔卡合成
class MagicCardCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "magiccardcomp"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		//UInt64 card1 = params.GetParam("card1");
		//UInt64 card2 = params.GetParam("card2");

		//player->ComposeMagicCard(card1, card2);

		return true;
	}
};

//存仓库
class PushStorageHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "pushstorage"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 uid = params.GetParam("uid");
		UInt16 num = params.GetParam("num");
		if (0 == num) num = 1;

		player->PushStorage(uid, num);

		return true;
	}
};

//取仓库
class PullStorageHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "pullstorage"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 uid = params.GetParam("uid");
		UInt16 num = params.GetParam("num");
		if (0 == num) num = 1;
		
		player->PullStorage(uid, num);

		return true;
	}
};

//查询商店
class QueryShopCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "queryshop"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt8 shopid = params.GetParam("shopid");
		UInt8 cache = params.GetParam("cache");

		player->GetShopMgr().OnQueryItem(shopid, cache);

		return true;
	}
};

//商店购买
class BuyShopCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "buyshop"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt8	shopId = params.GetParam("shopid");
		UInt32 shopItemId = params.GetParam("itemid");
		UInt16 num = params.GetParam("num");

		UInt16 newNum = 0;
		UInt32 leaseEndTimeStamp = 0;

		CLProtocol::SceneShopBuy protocol;
		protocol.shopId = shopId;
		protocol.shopItemId = shopItemId;
		protocol.num = num;

		CLProtocol::ItemInfo itemInfo;
		itemInfo.uid = params.GetParam("extraUid");
		itemInfo.num = params.GetParam("extraNum");
		protocol.costExtraItems.push_back(itemInfo);

		player->GetShopMgr().OnBuy(protocol, newNum, leaseEndTimeStamp);

		return true;
	}
};

//商店刷新
class RefreshShopCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "refreshshop"; }
	const char* GetHelp() const { return "shopid=xxxx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt8	shopId = params.GetParam("shopid");

		player->GetShopMgr().OnRefreshShop(shopId);

		return true;
	}
};

/**
 *@brief 学技能
 */
class ChangeSkillCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "changeskill"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt16 id = params.GetParam("id");
		Int8 dif = params.GetParam("dif");
		if (id == 0 || dif == 0) return false;
		std::vector<ChangeSkill> skills;
		ChangeSkill skill(id, dif);
		skills.push_back(skill);
		UInt32 res = player->GetSkillMgr().ChangeSkills(skills);
		if (res != ErrorCode::SUCCESS)
		{
			GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") errorcode "
				<< res << LogStream::eos;
			return false;
		}

		return true;
	}
};

/**
 *@brief 重置任务
 */
class ResetTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resettask"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->ResetTask();

		return true;
	}
};

/**
 *@brief 设置人物等级
 */
class SetLevelCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "setlevel"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->SetTestLevel(params.GetParam("num")); 

		return true;
	}
};

/**
 *@brief 重新加载脚本
 */
class ReloadScriptsCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "reloadscripts"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		BroadcastToScenes(player, params);
		TransmitToWorld(player, params);

		return true;
	}

	bool Execute(ObjID_t playerId, const CLParameter& params)
	{
		if(ScriptMgr::Instance()->ReloadScripts())
		{
			Sys::SendNotify(playerId, 201, NODE_ID);
		}
		else
		{
			Sys::SendNotify(playerId, 202, NODE_ID);
		}
		
		return true;
	}
};

/**
*@brief 重新加载脚本
*/
class ReloadScriptCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "reloadscript"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		BroadcastToScenes(player, params);
		TransmitToWorld(player, params);
		return true;
	}

	bool Execute(ObjID_t playerId, const CLParameter& params)
	{
		SScriptType type = (SScriptType)((UInt32)params.GetParam("type"));
		if (ScriptMgr::Instance()->ReloadScript(type))
		{
			Sys::SendNotify(playerId, 201, NODE_ID);
		}
		else
		{
			Sys::SendNotify(playerId, 202, NODE_ID);
		}

		return true;
	}
};

/**
 *@brief 清掉已学习技能
 */
class ClearSkillsCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "clearskills"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		//player->GetSkillMgr().ClearSkills();
		player->GetSkillMgr().ForgetAllSkills();

		return true;
	}
};

/**
 *@brief 加友好度
 */
class AddIntimacyCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "addintimacy"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		TransmitToWorld(player, params);

		return true;
	}
};

/**
*@brief 回复与后院操作相关的自身信息
*/
class RestoreBKCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "restorebk"; }

	bool Execute(Player *pPlayer, const CLParameter &params)
	{
		TransmitToWorld(pPlayer, params);

		return true;
	}
};

#ifdef _DEBUG

//TEST设置强化概率
class StrengthSucCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "neverlose"; }
	const char* GetHelp() const { return " open=1"; }


	bool Execute(Player *player, const CLParameter &params)
	{
		UInt32 open = params.GetParam("open");
		if (open > 0)
		{
			open = 1;
		}
		player->m_testEquipStrengthFlag = open;
		return true;
	}
};

//TEST设置合成天空套概率
class HappySkyCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "happysky"; }
	const char* GetHelp() const { return " open=1"; }


	bool Execute(Player *player, const CLParameter &params)
	{
		UInt32 open = params.GetParam("open");
		if (open > 0)
		{
			open = 1;
		}
		player->m_testHappySky = open;
		return true;
	}
};

#endif

/**
 *@brief 接任务  id=xxx force=0
 */
class AcceptTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "accepttask"; }
	const char* GetHelp() const { return " id=xxxx force=1"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 taskid = params.GetParam("id");
		//modified by aprilliu
		UInt8 force = params.GetParam("force");
		return pPlayer->AcceptTask(taskid, 0, (force != 0));
	}
};

class RefreshDailyTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "redailytask"; }
	const char* GetHelp() const { return " //refresh daily task"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->GetDailyTaskMgr().RefreshDailyTask();
		return true;
	}
};

//added by aprilliu  id=xxx  force=x (1 or 0)
class SubmitTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "submittask"; }

	const char* GetHelp() const { return " id=xxxx force=1"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 taskid = params.GetParam("id");
//modified by aprilliu, 2016.05.26 把提交任务的gm命令修改为  submittask id=xxx  force=x 
// 		UInt32 npcid = params.GetParam("npcid");
// 		UInt8 submitType = params.GetParam("submittype");
// 
// 		//pPlayer->SubmitTask(taskid);
// 		CLProtocol::SceneSubmitTask request;
// 		request.taskId = taskid;
// 		request.npcId = npcid;
// 		request.submitType = submitType;
// 
// 		pPlayer->HandleProtocol(request);
		UInt8 force = params.GetParam("force");

		pPlayer->SubmitTask(taskid, (force != 0));

		return true;

	}
protected:
private:
};

//added by aprilliu,放弃任务   !!abandontask id=xxx
class AbandonTaskCmdHandler: public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "abandontask"; }
	const char* GetHelp() const { return "id=xx"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 taskid = params.GetParam("id");
		pPlayer->AbandonTask(taskid);
		return true;
	}
protected:
private:
};

/**
 *@brief 清除道具
 */
class ClearItemsCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "clearitems"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		class ClearItemsVisitor : public ItemVisitor
		{
		public:
			ClearItemsVisitor(Player* owner):m_pOwner(owner){}

			bool operator()(Item* item)
			{
				m_pOwner->ReduceItemNum("clearitems", item, item->GetNum());
				return true;
			}

		private:
			Player* m_pOwner;
		};
		ClearItemsVisitor visitor(pPlayer);

		std::string type = params.GetParam("type").GetString();
		if (type == "all")
		{
			pPlayer->VisitItems(PACK_EQUIP, visitor);
			pPlayer->VisitItems(PACK_MATERIAL, visitor);
			pPlayer->VisitItems(PACK_EXPENDABLE, visitor);
			pPlayer->VisitItems(PACK_FASHION, visitor);
			pPlayer->VisitItems(PACK_TASK, visitor);
		}
		else
		{
			PackType type = (PackType)(UInt32)params.GetParam("type");
			pPlayer->VisitItems(type, visitor);
		}
		return true;
	}
};

/**
*@brief 添加buff
*/
class AddBuffCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addbuff"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		pPlayer->GetBuffMgr().AddBuff(id);
		return true;
	}
};

/**
 *@brief 禁言
 */
class ForbidTalkHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "forbidtalk"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		TransmitToWorld(pPlayer, params);

		return true;
	}
};

/**
 *@brief 封号
 */
class ForbidLoginHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "forbidlogin"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		TransmitToWorld(pPlayer, params);

		return true;
	}
};

/**
 *@brief 通告
 */
class AnnounceHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "announce"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		TransmitToWorld(pPlayer, params);

		return true;
	}
};

/**
 *@brief 广播邮件
 */
class BroadcastMailHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "broadcastmail"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		TransmitToWorld(player, params);

		return true;
	}
};

/**
 *@brief
 */

class AddSocialConsume : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "addsocialconsume"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");

		if(num > 0)
		{
			CLProtocol::SocialInviteNotifyConsume notify;
			notify.uid = pPlayer->GetAccID();
			notify.gold = num;
			Router::SendToSocial(notify);

			return true;
		}

		return false;
	}
};

class TestConsumeHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "testconsume"; }

	bool Execute(Player* player, const CLParameter& params)
	{
// 		UInt32 itemId = params.GetParam("itemid");
// 		UInt16 num = params.GetParam("num");
// 		UInt32 cost = params.GetParam("cost");
// 
// 		player->OnConsumed(itemId, num, cost);

		return true;
	}
};

class SysNotifyHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "sysnotify"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 type = params.GetParam("type");
		UInt32 color = params.GetParam("color");
		std::string content = params.GetParam("content").GetString();

		player->Notify(type, color, content.c_str());

		return true;
	}
};

class OpenFunctionsHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "openfunctions"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		for(UInt32 id = 1; id <= MAX_FUNCTION_OPEN_NUM; ++id)
		{
			player->SetFuncFlag(id);
		}

		return true;
	}
};

/**
*@brief 单笔消费Q点
*/
class ConsumePoint : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "consumepoint"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
// 		UInt32 num = params.GetParam("num");
// 		pPlayer->OnConsumed(3, num, num);

		return true;
	}

protected:
private:
};

/**
*@brief 发送系统邮件
*/
class TestSysMailHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "testsysmail"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt64 recvId = params.GetParam("playerid");
		UInt32 itemId = params.GetParam("itemid");
		UInt32 num = params.GetParam("num");
		UInt32 bind = params.GetParam("bind");
		UInt32 quality = params.GetParam("quality");
		std::string senderName = params.GetParam("sender").GetString();
		UInt32 vaildDay = params.GetParam("day");
		std::string reason = GetReason(SOURCE_TYPE_GM);
		RewardGroup* rewardGroup = player->GetRewardGroup();
		rewardGroup->AddReward(itemId, num, bind, quality);
		if (recvId == 0)
		{
			Sys::SendSysMail(player->GetID(), senderName.c_str(), "test", "test", reason.c_str(), vaildDay, rewardGroup);
		}
		else
		{
			Sys::SendSysMail(recvId, senderName.c_str(), "test", "test", reason.c_str(), vaildDay, rewardGroup);
		}
		return true;
	}

protected:
private:
};

class TestMailsHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "testmails"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		std::string reason = GetReason(SOURCE_TYPE_GM);
		std::string sender = "《地下城盟约》项目组";
		std::string title = "账号密码保存提醒通知";
		std::string content = "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest";
		for (UInt32 i = 0; i <= 10000; i++)
		{
			Sys::SendSysMail(player->GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), MAIL_VALID_DAY);
		}
		return true;
	}
};

/**
*@brief 发送系统邮件
*/
class TestItemMailHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "testitemmail"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		std::string itemIdsStr = params.GetParam("itemids").GetString();
		std::vector<std::string> tmpIds;
		std::string split = ",";
		Avalon::StringUtil::Split(itemIdsStr, tmpIds, split);

		std::string simpleItemIdsStr = params.GetParam("sitemids").GetString();
		std::vector<std::string> simpleIds;
		Avalon::StringUtil::Split(simpleItemIdsStr ,simpleIds, split);

		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::WORLD_TEST_ITEM_MAIL);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		std::string senderName = "system";
		std::string title = "testitem";
		std::string content = "testitem";
		stream & player->GetID() & senderName & title & content;
		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_GM);
		RewardGroup* rewardGroup = player->GetRewardGroup();
		for (UInt32 i = 0; i < simpleIds.size(); i++)
		{
			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(simpleIds[i].c_str());
			rewardGroup->AddReward(itemId, 1, 1);
		}
		std::vector<ItemReward> items = rewardGroup->GetRewards();
		stream & items;

		for (UInt32 i = 0; i < tmpIds.size(); i++)
		{
			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(tmpIds[i].c_str());

			Item* item = ItemMaker::Instance()->MakeItem(player, itemId, 1, 1, 4);
			if (item == NULL)
			{
				Avalon::Packet::Destroy(packet);
				return false;
			}

			item->SetAccId(player->GetAccID());
			item->SetOwnerId(player->GetID());

			stream & item->GetID() & item->GetDataID();
			item->Encode(stream, SST_WORLD, false);

			Item::DestroyItem(item);
		}
		stream & ObjID_t(0);
		packet->SetSize(stream.GetPos());
		Router::SendToWorld(packet);
		Avalon::Packet::Destroy(packet);
		
		return true;
	}
};

/**
*@brief pkValue
*/
class AddPkValueCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "addpkvalue"; }
	const char* GetHelp() const { return "num=xx"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        int num = params.GetParam("num");

        if (num > 0)
        {
            player->IncPkValue(num);
        }
        else
        {
            player->DecPkValue(-1 * num);
        }
        
        return true;
    }
};

/**
*@brief pkCoinFromRace
*/
class AddRacePkCoinCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addracepkcoin"; }
	const char* GetHelp() const { return "num=xx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddPkCoinFromRace(num);
		
		return true;
	}
};

/**
*@brief 开放所有地下城和难度
*/
class OpenDungeonCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "opendungeon"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        class OpenDungeonVistor : public Avalon::DataEntryVisitor<DungeonDataEntry>
        {
        public:
            OpenDungeonVistor(Player* player) { m_player = player; }

            bool operator()(DungeonDataEntry *dataEntry)
            {
                if (!dataEntry)
                {
                    return false;
                }
				std::vector<ObjID_t> temp;
				m_player->GetDungeonMgr().OnDungeonRaceEnd(dataEntry->id, LS_SSS, 0, 0, 0, 0, 0, false, temp);
                return true;
            }
            

            Player* m_player;
        };

        OpenDungeonVistor vistor(player);
        DungeonDataEntryMgr::Instance()->Visit(vistor);

        return true;
    }
};

/**
*@brief 显示服务器内存占用
*/
class ReportMemoryTrackerCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "memtracker"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        REPORT_MEMORY(gSysLogger);
        return true;
    }
};

/**
*@brief 创建队伍
*/
class CreateTeamCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "cteam"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        CLProtocol::GateTransmitPacket trans;
        trans.id = player->GetID();
        CLProtocol::WorldCreateTeam createTeam;
		createTeam.target = params.GetParam("target");
        createTeam.Encode(trans.packet);

        Router::SendToWorld(trans);

        return true;
    }
};

/**
*@brief 加入队伍
*/
class JoinTeamCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "jteam"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        Team* team = TeamMgr::Instance()->GetRandTeam();
        if (!team)
        {
            return true;
        }

        CLProtocol::WorldTransmitRequest req;
        req.requester = player->GetID();
        req.type = REQUEST_JOINTEAM;
        req.target = team->GetMaster();
        Router::SendToWorld(req);

        CLProtocol::WorldTransmitReply rep;
        rep.requester = player->GetID();
        rep.type = REQUEST_JOINTEAM;
        rep.target = team->GetMaster();
        rep.result = 1;
        Router::SendToWorld(rep);

        return true;
    }
};

/**
*@brief 挑战玩家
*/
class ChallengeCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "challenge"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        std::string name = params.GetParam("name").GetString();
        Player* other = PlayerMgr::Instance()->FindPlayer(name);
        if (!other)
        {
            return false;
        }

        CLProtocol::WorldTransmitRequest req;
        req.requester = player->GetID();
        req.type = REQUEST_CHALLENGE_PK;
        req.target = other->GetID();
        Router::SendToWorld(req);
        return true;
    }
};

/**
*@brief 请求好友
*/
class RequestCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "request"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt8 type = params.GetParam("type");
		std::string targetName = params.GetParam("name").GetString();
		
		CLProtocol::WorldTransmitRequest req;
		req.type = type;
		req.requester = player->GetID();
		req.target = 0;
		req.targetName = targetName;
		req.id = 0;
		Router::SendToWorld(req);

		return true;
	}
};


/**
*@brief 解散队伍
*/
class DismissTeamCmdHandler : public GMCmdHandler
{
public:
    const char* GetCmdName() const { return "dteam"; }

    bool Execute(Player* player, const CLParameter& params)
    {
        CLProtocol::GateTransmitPacket trans;
        trans.id = player->GetID();
        CLProtocol::WorldDismissTeam dismiss;
        dismiss.Encode(trans.packet);

        Router::SendToWorld(trans);

        return true;
    }
};

class AddRetinueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addretinue"; }
	const char* GetHelp() const { return "id=xxxx"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 dataId = params.GetParam("id");
		if(player->GetRetinueMgr().AddRetinue(dataId) != ErrorCode::SUCCESS)
			return false;
		return true;
	}
};

class SetRetinueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setretinue"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 dataId = params.GetParam("id");
		UInt32 index = params.GetParam("index");
		ObjID_t id = 0;
		Retinue* retinue = player->GetRetinueMgr().FindRetinue(dataId);
		if (retinue != NULL) id = retinue->GetID();
		CLProtocol::SceneChanageRetinueRes res;
		res.result = player->GetRetinueMgr().SetRetinue(id, index);
		res.mainId = player->GetRetinueMgr().GetRetinueFighting();
		res.offRetinueIds = player->GetRetinueMgr().GetOffRetinueIds();
		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
			return false;
		return true;
	}
};

class RetinueChangeSkillCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "retinueskill"; }
	const char* GetHelp() const { return "id=xxxx skillid=xx,xx // lock skill id"; }

	bool Execute(Player* player, const CLParameter& params) 
	{
		UInt32 dataId = params.GetParam("id");
		ObjID_t id = 0;
		std::string strSkillIds = params.GetParam("skillid").GetString();
		std::vector<std::string> strSkill;
		std::vector<UInt32> skillIds;
		Avalon::StringUtil::Split(strSkillIds, strSkill, ",");
		for (UInt32 i = 0; i < strSkill.size(); ++i)
		{
			UInt32 skillId = Avalon::StringUtil::ConvertToValue<UInt32>(strSkill[i]);
			skillIds.push_back(skillId);
		}

		Retinue* retinue = player->GetRetinueMgr().FindRetinue(dataId);
		if (retinue != NULL) id = retinue->GetID();

		CLProtocol::SceneRetinueChangeSkillRes res;
		res.result = player->GetRetinueMgr().ChangeSkill(id, skillIds);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
			return false;
		return true;
	}
};

class RetinueSetSkillCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "rsetskill"; }
	const char* GetHelp() const { return "id=xxxx skillid=xx,xx // set skill id"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 dataId = params.GetParam("id");
		ObjID_t id = 0;
		std::string strSkillIds = params.GetParam("skillid").GetString();
		std::vector<std::string> strSkill;
		std::vector<UInt32> skillIds;
		Avalon::StringUtil::Split(strSkillIds, strSkill, ",");
		for (UInt32 i = 0; i < strSkill.size(); ++i)
		{
			UInt32 skillId = Avalon::StringUtil::ConvertToValue<UInt32>(strSkill[i]);
			skillIds.push_back(skillId);
		}

		Retinue* retinue = player->GetRetinueMgr().FindRetinue(dataId);
		if (retinue != NULL) id = retinue->GetID();

		if (player->GetRetinueMgr().TestChangeSkill(id,skillIds) == ErrorCode::SUCCESS)
		{
			return true;
		}
		return false;
	}
};

class ResetRetinueChangeSkillCountCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "rercount"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->GetRetinueMgr().ResetChangeSkillCount();
		return true;
	}

};

class UpLevelRetinueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "uplvlretinue"; }
	const char* GetHelp() const { return " id=xx //retinue level +1"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 dataId = params.GetParam("id");
		Retinue* retinue = player->GetRetinueMgr().FindRetinue(dataId);
		if (retinue == NULL) return false;
		player->GetRetinueMgr().UpLevelRetinue(retinue->GetID());
		return true;
	}
};

class UpStarRetinueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "upstarretinue"; }
	const char* GetHelp() const { return "id=xx //retinue star +1"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 dataId = params.GetParam("id");
		Retinue* retinue = player->GetRetinueMgr().FindRetinue(dataId);
		if (retinue == NULL) return false;
		player->GetRetinueMgr().UpStarRetinue(retinue->GetID());
		return true;
	}
};

class WudaoJoinCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "wudaojoin"; }
	const char* GetHelp() const { return ""; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->JoinWudaodahui();
		return true;
	}
};

class WudaoRewardCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "wudaoreward"; }
	const char* GetHelp() const { return ""; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->RequestWudaodahuiReward();
		return true;
	}
};

class AddPetCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addpet"; }
	
	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 itemId = params.GetParam("id");
		player->AddPet(itemId);
		return true;
	}
};

class FinishChapter1Handler : public GMCmdHandler
{
public:
	FinishChapter1Handler()
	{
		m_Level = 5; 
		m_StartTask = 2001;
		m_EndTask = 2008;
	}
	const char* GetCmdName() const { return "FinishChapter1"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		ExecuteLevel(player);
		ExecuteTask(player);
		return true;
	}

protected:
	virtual void ExecuteLevel(Player* player)
	{
		UInt64 needExp = player->GetLevelExp(m_Level);
		UInt64 exp = player->GetExp();
		if (needExp > exp)
		{
			player->IncExp(GetReason(LogSourceType::SOURCE_TYPE_GM).c_str(), needExp - exp);
		}
	}

	virtual void ExecuteTask(Player* player)
	{
		player->FinishAllTask();
		/*
		//完成任务
		for (UInt32 i = m_StartTask; i <= m_EndTask; ++i)
		{
			if (player->CheckTaskFinished(i)) continue;
			BaseTask* task = player->GetTaskMgr().FindTask(i);

			if (task == NULL && !player->AcceptTask(i, 0, true))
			{
				continue;
			}
			player->SubmitTask(i, true);
		}

		if (m_EndTask >= 2215 && !player->CheckTaskFinished(2271))
		{
			BaseTask* task = player->GetTaskMgr().FindTask(2271);
			if (task != NULL || player->AcceptTask(2271, 0, true))
			{
				player->SubmitTask(2271, true);
			}
		}
		*/
	}

protected:
	UInt32 m_Level;
	UInt32 m_StartTask;
	UInt32 m_EndTask;

};

class FinishChapter2Handler : public FinishChapter1Handler
{
public:
	FinishChapter2Handler() 
	{
		m_Level = 23;
		m_StartTask = 2201;
		m_EndTask = 2037;
	}
	const char* GetCmdName() const { return "FinishChapter2"; }
};

class FinishChapter3Handler : public FinishChapter1Handler
{
public:
	FinishChapter3Handler()
	{
		m_Level = 38;
		m_StartTask = 2201;
		m_EndTask = 2246;
	}
	const char* GetCmdName() const { return "FinishChapter3"; }
};

class FinishChapter4Handler : public FinishChapter1Handler
{
public:
	FinishChapter4Handler()
	{
		m_Level = 37;
		m_StartTask = 2001;
		m_EndTask = 2084;
	}
	const char* GetCmdName() const { return "FinishChapter4"; }
};

class FinishChapter5Handler : public FinishChapter1Handler
{
public:
	FinishChapter5Handler()
	{
		m_Level = 59;
		m_StartTask = 2001;
		m_EndTask = 2118;
	}
	const char* GetCmdName() const { return "FinishChapter5"; }
};

class ToTaskCmdHandler : public FinishChapter1Handler
{
public:
	ToTaskCmdHandler()
	{
		m_Level = 59;
		m_StartTask = 2001;
		m_EndTask = 2118;
	}
	const char* GetCmdName() const { return "totask"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		m_EndTask = id;
		ExecuteLevel(player);
		ExecuteTask(player);
		return true;
	}
};

//模拟活动任务变更日
class ActiveTaskDayChange : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "atdc"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		return true;
	}
};

class RefreshCycleTask : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "recycle"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 taskId = params.GetParam("id");
		player->GetCycleTaskMgr().RefreshCycleTask(taskId);
		return true;
	}
};

class RefreshCycleDailyCountTask : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "recyclecount"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->GetCycleTaskMgr().RefreshCycleTaskDailyCount();
		return true;
	}
};

class SubmitCycleTask : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "submitcycletask"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 taskId = params.GetParam("id");
		BaseTask* task = player->GetCycleTaskMgr().FindTask(taskId);
		if (task != NULL)
		{
			return player->GetCycleTaskMgr().SubmitTask(task, true);
		}
		return true;
	}
};

class SetTaskItem : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "settaskitem"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 taskId = params.GetParam("id");
		UInt64 itemId = params.GetParam("itemid");
		std::vector<UInt64> itemIds;
		itemIds.push_back(itemId);
		if (player->SetTaskItem(taskId, itemIds) == ErrorCode::SUCCESS)
		{
			return true;
		}
		return false;
	}
};

class SetDailyTaskStatus : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "fdtask"; }
	const char* GetHelp() const { return " //set daily task status finish"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->GetDailyTaskMgr().SetAllTaskStatus();
		return true;
	}
};

class SetTaskStatus : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ftask"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 taskId = params.GetParam("id");
		BaseTask* task = player->GetTask(taskId);
		if (task == NULL) return false;
		task->SetStatus(TaskStatus::TASK_FINISHED);
		return true;
	}
};

class FinishAllTaskStatus : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "fatask"; }
	const char* GetHelp() const { return " //finish all task"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->FinishAllTask();
		return true;
	}

};

class FinishMainTask : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "fmtask"; }
	const char* GetHelp() const { return "//finish main task"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 taskId = params.GetParam("id");
		player->FinishAllTask(TaskType::TASK_MAIN, taskId);
		return true;
	}
};

class AddAchievementScoreCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addas"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddAchievementScore(num);
		return true;
	}

};

/**
*@brief 更新活动
*/
class UpdateDailyActivityCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "updateDailyActivity"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		TransmitToWorld(player, params);

		return true;
	}
};

/**
*@brief 添加死亡币
*/
class AddWarriorSoulCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addwarriorsoul"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddWarriorSoul(GetReason(SOURCE_TYPE_GM).c_str(), num);

		return true;
	}
};

/**
*@brief 添加决斗币
*/
class AddPkCoinCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addpkcoin"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		player->AddPkCoin("gmcmd", num);

		return true;
	}
};

/**
*@brief 查看玩家信息
*/
class QueryPlayerInfoCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "query"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		CLProtocol::GateTransmitPacket trans;
		trans.id = player->GetID();
		CLProtocol::WorldQueryPlayerReq req;
		req.roleId = 0;
		req.name = params.GetParam("name").GetString();
		req.Encode(trans.packet);

		Router::SendToWorld(trans);

		return true;
	}
};

/**
*@brief 设置匹配积分
*/
class MatchScoreCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "matchscore"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 score = params.GetParam("num");
		player->SetMatchScore(score);
		return true;
	}
};

class SortListGiveRewardCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "slgive"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		TransmitToWorld(pPlayer, params);

		return true;
	}
};

class ResetDungeonNumCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "reset"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		class DungeonTimesDataVistor : public Avalon::DataEntryVisitor<DungeonTimesDataEntry>
		{
		public:
			bool operator()(DungeonTimesDataEntry* dataEntry)
			{
				datas.push_back(dataEntry);
				return true;
			}

			std::vector<DungeonTimesDataEntry*> datas;
		};
		
		DungeonTimesDataVistor vistor;
		DungeonTimesDataEntryMgr::Instance()->Visit(vistor);
		for (auto data : vistor.datas)
		{
			pPlayer->SetCounter(data->buyTimesCounter.c_str(), 0);
			pPlayer->SetCounter(data->usedTimesCounter.c_str(), 0);
		}

		return true;
	}
};

class ResetTowerCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resettower"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		//pPlayer->SetCounter(COUNTER_TOWER_RESET_REMAIN_TIMES, 1);
		pPlayer->OnResetDeathTower();
		return true;
	}
};

class ResetTowerCounterCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resettowercounter"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->SetCounter(COUNTER_TOWER_RESET_REMAIN_TIMES, (UInt32)params.GetParam("num"));
		return true;
	}
};

class SetTowerTopFloorCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setttf"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 floor = params.GetParam("num");

		pPlayer->SetCounter(COUNTER_TOWER_TOP_FLOOR, floor);
		return true;
	}
};


class TowerWipeoutCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "towerwipeout"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->OnBeginWipeoutDeathTower();
		return true;
	}
};

class TowerWipeoutResultCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "towerresult"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->OnRequestDeathTowerWipeoutResult();
		return true;
	}
};

class TowerQuickFinishCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "towerfinish"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->OnQuickFinishDeathTowerWipeout();
		return true;
	}
};

class TowerReceiveFloorAwardCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "toweraward"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->OnRequestDeathTowerPassFloorAward(params.GetParam("num"));
		return true;
	}
};

class GuildExchangeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildexchange"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->RequestGuildExchange();
		return true;
	}
};

class BuyMonthCardCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "monthcard"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->OnBuyMonthCard("123");
		return true;
	}
};

class BuyDungeonTimesCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "buytimes"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		DungeonSubType subType = (DungeonSubType)(UInt32)params.GetParam("type");
		pPlayer->RequestBuyDungeonTimes(subType);
		return true;
	}
};

class SetSeasonLevelCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setsl"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 level = params.GetParam("level");
		UInt32 star = params.GetParam("star");
		pPlayer->SetSeasonLevel(level, star);
		return true;
	}
};

class SetSeasonRecordCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setsr"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		std::string recordStr = params.GetParam("records").GetString();
		std::vector<std::string> vecTmp;
		std::vector<UInt8> vecRecords;
		Avalon::StringUtil::Split(recordStr, vecTmp, ",");
		for (UInt32 i = 0; i < vecTmp.size(); ++i)
		{
			recordStr.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(vecTmp[0]));
		}

		pPlayer->SetSeasonUplevelBattleRecord(vecRecords);
		return true;
	}
};

class AddSeasonExpCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addsexp"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 exp = params.GetParam("num");
		pPlayer->AddSeasonExp(exp);
		return true;
	}
};

class DecSeasonExpCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "decsexp"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 exp = params.GetParam("num");
		pPlayer->DecSeasonExp(exp);
		return true;
	}
};

class SetSeasonAttrCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setsa"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 attr = params.GetParam("id");
		pPlayer->SetSeasonAttr((UInt32)Avalon::Time::CurrentTime().Sec(), (UInt8)attr);
		return true;
	}
};

class GMSystemHelp :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "help"; }
	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		std::string key = params.GetParam("key").GetString();
		std::vector<std::string> helps = GMSystem::Instance()->GetCmdHelp(key);
		std::string str = "";
		for (UInt32 i = 0; i < helps.size(); ++i)
		{
			str += ("\n" + helps[i]);
		}

		CLProtocol::SceneSyncChat protocol;
		protocol.channel = CHAT_CHANNEL_SYSTEM;
		protocol.objname = "";
		protocol.word = str;
		protocol.bLink = true;
		protocol.isGm = 0;
		protocol.voiceKey = " ";
		protocol.voiceDuration = 0;
		pPlayer->SendProtocol(protocol);

		return true;
	}
};

class ClearPkStatisticCmdHelper :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "clearpk"; }
	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		pPlayer->GetPkStatisticMgr().OnClearPkStatistic();
		return true;
	}
};

class PvPCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "pk"; }
	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 result = params.GetParam("result");

		pPlayer->UpdateSeasonPkStraightCount((PkRaceResult)result);
		return true;
	}
};

class ShowMatchScoreCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "showms"; }
	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		char str[1024];
		avalon_snprintf(str, sizeof(str)-1, "score=%u win=%d lose=%d", pPlayer->GetMatchScore(), pPlayer->GetSeasonPkWinStraight(), pPlayer->GetSeasonPkLoseStraight());
		pPlayer->Speak(CHAT_CHANNEL_WORLD, str);
		return true;
	}
};

class RemovePetHungerCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "removeph"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt16 value = params.GetParam("num");
		pPlayer->RemovePetHunger(value);
		return true;
	}

};

class AddPetHungerCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addph"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt16 value = params.GetParam("num");
		pPlayer->AddPetHunger(value);
		return true;
	}

};

class SendHornCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "horn"; }
	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		std::string content = params.GetParam("content").GetString();
		UInt8 num = params.GetParam("num");

		pPlayer->SendHorn(content, num);
		return true;
	}
};


// 切地图
class ChangeSceneCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "changescene"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 sceneid = params.GetParam("id");
		CLPosition pos;
		player->ChangeScene(sceneid, pos, 0);

		return true;
	}
};

//理财卡
class BuyMoneyManageCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "bmm"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		player->OnBuyMoneyManageCard(GetReason(LogSourceType::SOURCE_TYPE_GM).c_str());
		return true;
	}
};

//补交活动任务
class ReSubmitActiveTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resubmitTask"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		UInt32 id2 = params.GetParam("id2");
		UInt32 id3 = params.GetParam("id3");

		std::vector<UInt32> taskIds = { id, id2, id3 };

		return true;
	}
};

class UsedOpActHellTicketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "useOpActHellTicket"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		return true;
	}
};

class RemoveOpActFatigueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "rmOpActFatigue"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		return true;
	}
};

class DungeonDropCounterCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ddc"; } 

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 dungeonId = params.GetParam("id");
		UInt32 times = params.GetParam("t");

		CLProtocol::WorldDungeonCalDropReq req;
		req.roleId = pPlayer->GetID();
		req.dungeonId = dungeonId;
		req.times = times;
		req.playerInfo = pPlayer->GetDungeonMgr().GetDungeonPlayerInfo(dungeonId);
		Router::SendToWorld(req);
		return true;
	}
};

// 提交运营活动任务
class SubmitOpActTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "sbmtOpActTask"; }
	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		return true;
	}
};

class SummonCityMonsterCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "summon"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		SceneMgr::Instance()->GenActivityCityMonster();
		return true;
	}
};

class ClearCityMonsterCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "clear"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		class CityMonsterSceneVisitor : public CLVisitor<Scene>
		{
		public:
			CityMonsterSceneVisitor() {}

			bool operator()(Scene* scene)
			{
				if (!scene)
				{
					return true;
				}

				auto info = scene->GetNpcInfo(NULL);
				for (auto npc : info.npcs)
				{
					auto monster = scene->FindCityMonster(npc.guid);
					scene->DelCityMonster(monster, true);
				}

				return true;
			}

		};

		CityMonsterSceneVisitor vistor;
		SceneMgr::Instance()->VisitScenes(vistor);
		return true;
	}
};

class StartCityMonsterDungeomCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "monster"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		auto monsters = SceneMgr::Instance()->GetSceneNpcInfo(pPlayer);

		if (monsters.empty())
		{
			return true;
		}

		Player* player = pPlayer;
		ObjID_t cityMonsterId = monsters[0].npcs[0].guid;
		SceneNpc cityMonster;
		UInt32 dungeonId = 0;
		
		if (cityMonsterId > 0)
		{
			auto monster = SceneMgr::Instance()->FindCityMonster(cityMonsterId);
			if (!monster)
			{
				CLProtocol::SceneDungeonStartRes res;
				res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
				player->SendProtocol(res);
				return true;
			}

			if (monster->GetOwnerID() > 0 && monster->GetOwnerID() != player->GetID())
			{
				CLProtocol::SceneDungeonStartRes res;
				res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
				player->SendProtocol(res);

				ErrorLogStream << "player(" << player->GetID() << ") want to kill other player's monster(" << monster->GetID() << ", " << monster->GetOwnerID() << ")." << LogStream::eos;
				return true;
			}

			auto npc = NpcDataEntryMgr::Instance()->FindEntry(monster->GetDataID());
			if (!npc)
			{
				CLProtocol::SceneDungeonStartRes res;
				res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
				player->SendProtocol(res);

				ErrorLogStream << "player(" << player->GetID() << ") want to kill o monster(" << monster->GetID() << ", " << monster->GetOwnerID() << ") data(" << monster->GetDataID() << ") can't find in npc data." << LogStream::eos;
				return true;
			}

			if (npc->mustTeam && !player->GetTeam())
			{
				CLProtocol::SceneDungeonStartRes res;
				res.result = ErrorCode::DUNGEON_START_MUST_TEAM;
				player->SendProtocol(res);

				return true;
			}

			dungeonId = monster->GetDungeonID();
			cityMonster = monster->GetInfo();
		}

		if (player->GetTeam())
		{
			player->GetTeam()->OnPlayerRequestStartDungeonRace(player, dungeonId, cityMonster);
			return true;
		}

		player->GetDungeonMgr().OnStartDungeonReq(dungeonId, std::vector<UInt32>(), cityMonster);

		return true;
	}
};

//计算装备评分
class CalcEqScoreHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "calceqs"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		ObjID_t uid = params.GetParam("uid");
		Item* equip = player->FindItem(uid);
		if (equip)
		{
			player->GetItemMgr().CalculateValuedScore(equip);
		}
		return true;
	}
};

class ScoreWarSetWinBattleCountHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "swswbc"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		pPlayer->SetScoreWarWinBattleCount(num);
		return true;
	}
};

class ScoreWarSetBattleScoreHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "swsbs"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		pPlayer->SetScoreWarScore(num);
		pPlayer->SendScoreWarSortListInfo();
		return true;
	}
};

class ScoreWarSortListTestHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "swslt"; }

	void SendSortList(UInt64 playerId, UInt32 score)
	{
		CLProtocol::CrossScoreWarSortListSync sync;
		sync.entry.playerId = playerId;
		sync.entry.playerName = "Test" + Avalon::StringUtil::ConvertToString(sync.entry.playerId);
		sync.entry.score = score;
		sync.entry.serverName = SERVER_NAME;
		sync.entry.sortType = (UInt32)SortListType::SORTLIST_SCORE_WAR;
		Router::SendToCross(sync);
	}

	bool Execute(Player* pPlayer, const CLParameter& params)
	{

		SendSortList(10000001, 150);
		SendSortList(10000002, 100);
		SendSortList(10000003, 100);
		SendSortList(10000004, 50);
		SendSortList(10000005, 50);
		SendSortList(10000006, 50);
		SendSortList(10000010, 0);
		SendSortList(10000011, 0);
		SendSortList(10000007, 0);
		SendSortList(10000008, 0);
		SendSortList(10000009, 0);
		SendSortList(10000003, 100);

		return true;
	}
};

class ScoreWarGetRewardHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "swgr"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		pPlayer->GiveScoreWarReward(id);
		return true;
	}
};

class MasterSysSetDailyTaskCompsHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "mssdtc"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 st = params.GetParam("st");
		class TaskVisitor : public BaseTaskVisitor
		{
		public:
			TaskVisitor(UInt32 st) : m_st(st) {}
			bool operator()(BaseTask* object)
			{
				if (m_st == 1)
				{
					object->SetStatus(TaskStatus::TASK_FINISHED);
				}
				else if (m_st == 0)
				{
					object->SetStatus(TaskStatus::TASK_UNFINISH);
				}
				return true;
			}
		private:
			UInt32 m_st;
		};
		TaskVisitor task_vister(st);
		pPlayer->GetMasterDailyTaskMgr().VisitTasks(task_vister);
		return true;
	}
};

class MasterSysAddGrowthHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "msaddg"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		std::string  resaon = GetReason(SOURCE_TYPE_GM, 0, num);
		
		pPlayer->AddMasterSysAcademicGrowth(resaon.c_str(), num);
		return true;
	}
};

class ScoreWarChangeStatusHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "swcstatus"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		ScoreWarStatus status = (ScoreWarStatus)((UInt32)params.GetParam("status"));

		CLProtocol::SceneCrossGMSync sync;
		sync.isByTime = false;
		sync.targetStatus = status;
		Router::SendToCross(sync);

		return true;
	}
};

class ScoreWarSetBattleCountHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "swsbc"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		pPlayer->SetScoreWarBattleCount(num);
		return true;
	}
};

class SysAddIncDailyTaskScore : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addActivity"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		pPlayer->IncDailyTaskScore(num);
		return true;
	}
};

class GCGMCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "gc"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		InfoLogStream << "start to gc." << LogStream::eos;
		ScriptMgr::Instance()->GC();
		InfoLogStream << "gc finish." << LogStream::eos;
		return true;
	}
};

class StrengthenTickeSynthesisPlanGenerateCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "genStrenTicketSynthPlan"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		StrengthenTicketPlanMgr::Instance()->RecordSynthesisPlanToFile();

		return true;
	}
};

class StrengthenTickeFusePlanGenerateCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "genStrenTicketFusePlan"; }

	class FusePlanGenrator : public Avalon::Thread
	{
	public:
		FusePlanGenrator(UInt32 beginCalcIndex, UInt32 endCalcIndex, bool needSaveTimeOutPlan, UInt64 timeMax)
			: m_BeginCalcIndex(beginCalcIndex), m_EndCalcIndex(endCalcIndex), m_Finish(false)
			, m_NeedSaveTimeOutPlan(needSaveTimeOutPlan), m_TimeMax(timeMax)
		{
		}

		virtual void Run()
		{
			UInt64 startTime = Avalon::Time::CurrentTime().MSec();

			if (!StrengthenTicketPlanMgr::Instance()->CalcAssignFusePlanAndRecordFile(m_BeginCalcIndex, m_EndCalcIndex, m_NeedSaveTimeOutPlan, m_TimeMax))
			{
				m_Finish = true;
				return;
			}

			UInt64 endTime = Avalon::Time::CurrentTime().MSec();
			UInt64 usedTime = endTime - startTime;

			InfoLogStream << "thread params(" << m_BeginCalcIndex << ", " << m_EndCalcIndex << "), used time : " << usedTime << "ms." << LogStream::eos;

			m_Finish = true;
		}

		bool IsFinish() const { return m_Finish; }

	public:
		UInt32 m_BeginCalcIndex;
		UInt32 m_EndCalcIndex;
		bool   m_Finish;
		bool   m_NeedSaveTimeOutPlan;
		UInt64 m_TimeMax;
	};

	class FusePlanGenratorMgr : public Avalon::Thread
	{
	public:
		FusePlanGenratorMgr(UInt32 threadNum, UInt64 timeMax)
		{
			std::vector<StrTicketData*> fuseTicketDatas = StrTicketDataMgr::Instance()->GetFuseStrTicketDatas();
			if (fuseTicketDatas.empty())
			{
				ErrorLogStream << "Fuse strengthen ticket datas are empty!" << LogStream::eos;
				return;
			}

			UInt32 fuseTicketDataNum = fuseTicketDatas.size();
			UInt32 perQueryNum = fuseTicketDataNum / threadNum;
			UInt32 nextBeginQueryIndex = 0;

			for (UInt32 i = 0; i < threadNum; i++)
			{
				UInt32 nextEndIndex = nextBeginQueryIndex + perQueryNum;
				if (nextEndIndex == (perQueryNum * threadNum))
				{
					nextEndIndex = fuseTicketDataNum;
				}

				auto thread = new FusePlanGenrator(nextBeginQueryIndex, nextEndIndex, threadNum == 1 ? true : false, timeMax);
				nextBeginQueryIndex += perQueryNum;
				m_Threads.push_back(thread);
			}
		}

		~FusePlanGenratorMgr()
		{
			for (auto thread : m_Threads)
			{
				thread->Stop();
				thread->Join();
				delete thread;
			}

			m_Threads.clear();
		}

		virtual void Run()
		{
			UInt64 startTime = Avalon::Time::CurrentTime().MSec();

			for (auto thread : m_Threads)
			{
				thread->Activate();
			}

			while (true)
			{
				bool allFinish = true;
				for (auto thread : m_Threads)
				{
					if (!thread->IsFinish())
					{
						allFinish = false;
						break;
					}
				}

				if (allFinish)
				{
					break;
				}

				Sleep(1);
			}

			UInt64 endTime = Avalon::Time::CurrentTime().MSec();
			UInt64 usedTime = endTime - startTime;

			InfoLogStream << "Calcuate strengthen ticket fuse all plan, used time: " << usedTime << "ms." << LogStream::eos;

			m_Threads.clear();
		}

	public:
		std::vector<FusePlanGenrator*>		m_Threads;
	};

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 threadNum = params.GetParam("num");
		UInt64 timeMax = params.GetParam("time");
		auto thread = new FusePlanGenratorMgr(threadNum == 0 ? 2 : threadNum, timeMax);
		thread->Activate();

		return true;
	}
};

class StrengthenTicketSynthesisCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "synthStrenTicket"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		StrengthenTicketPlanMgr::Instance()->SynthesisStrengthenTicket(pPlayer, params.GetParam("id"));
		return true;
	}
};

class StrengthenTicketFuseCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "fuseStrenTicket"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		class StrenTicketVisitor : public ItemVisitor
		{
		public:
			bool operator()(Item* item)
			{
				if (item->GetSubType() == ST_COUPON)
				{
					auto strenTicketData = StrTicketDataMgr::Instance()->FindEntry(item->GetDataEntry()->strenTicketDataIndex);
					if (!strenTicketData) return true;

					if (strenTicketData->canFuse)
					{
						strenTickets.push_back(item);
					}
				}

				if (strenTickets.size() == 1)
				{
					if (strenTickets.front()->GetNum() >= 2)
					{
						return false;
					}
				}

				if (strenTickets.size() == 2)
				{
					return false;
				}

				return true;
			}

			std::vector<Item*> strenTickets;
		};

		Package* package = pPlayer->GetItemMgr().GetPackage(PACK_MATERIAL);
		if (!package) return false;

		StrenTicketVisitor visitor;
		package->VisitItems(visitor);

		ObjID_t itemA = 0;
		ObjID_t itemB = 0;

		if (visitor.strenTickets.size() == 1)
		{
			if (visitor.strenTickets.front()->GetNum() < 2)
			{
				return false;
			}
			itemA = visitor.strenTickets.front()->GetID();
			itemB = itemA;
		}
		else if (visitor.strenTickets.size() == 2)
		{
			itemA = visitor.strenTickets[0]->GetID();
			itemB = visitor.strenTickets[1]->GetID();
		}
		else
		{
			return false;
		}

		ObjID_t timeA = CURRENT_TIME.MSec();
		StrengthenTicketPlanMgr::Instance()->FuseStrengthenTickets(pPlayer, itemA, itemB);
		ObjID_t timeB = CURRENT_TIME.MSec();

		ObjID_t useTime = timeB - timeA;

		if (useTime > 10)
		{
			DebugLogStream << PLAYERINFO(pPlayer) << " fuse strengthen ticket with item(" << visitor.strenTickets[0]->GetDataID() << ") and item(" 
				<< visitor.strenTickets[1]->GetDataID() << ") use time(" << useTime << ")." << LogStream::eos;
		}

		return true;
	}
};

class StrengthenTicketFusePlanCalcCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "strenTicketFuseCalc"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		StrengthenTicketPlanMgr::Instance()->CalculateFusePlans(params.GetParam("idA"), params.GetParam("idB"));
		return true;
	}
};

class PlayerClearDungeonForActivityTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "clearDunegonForAct"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		return true;
	}
};


class SceneArtifactJarExtractDiscountReqCmdHandler : public GMCmdHandler
{
	class MyGenerator : public Avalon::Thread
	{
	public:
		MyGenerator(UInt32 accid, UInt32 lv, UInt32 cnt)
			: m_Finish(false), m_AccId(accid), m_Lv(lv), m_GenCnt(cnt)
		{
		}

		virtual void Run()
		{
			struct MyFileElem
			{
				MyFileElem()
				:rate(0), times(0) {}

				UInt32 rate;
				UInt32 times;
			};

			std::vector<MyFileElem> myFileElems;
			for (UInt32 idx = 0; idx < m_GenCnt; ++idx)
			{
				UInt32 discountRate = 0;
				UInt32 effectTimes = 0;

				if (!VipDataEntryMgr::Instance()->RandArtifactJarDiscount(m_Lv, discountRate, effectTimes))
				{
					ErrorLogStream << " rand artifact jar discount(" << m_Lv << ") failed!" << LogStream::eos;
					continue;
				}

				MyFileElem myElem;
				myElem.rate = discountRate;
				myElem.times = effectTimes;
				myFileElems.push_back(myElem);
			}

			if (myFileElems.empty())
			{
				ErrorLogStream << "File elems are empty!" << LogStream::eos;
				m_Finish = true;
				return;
			}

			std::string rootpath = SSApplication::Instance()->GetArtifactJarRecordPath();
			std::ostringstream fileNameOss;
			fileNameOss << rootpath << "artifactJar_" << m_Lv << "_" << m_GenCnt << "_" << m_AccId << ".csv";
			std::string filename = fileNameOss.str();

			std::ofstream ofs;
			ofs.open(filename.c_str());
			if (!ofs.good())
			{
				ErrorLogStream << "open file(" << filename << ") failed!" << LogStream::eos;
				m_Finish = true;
				return;
			}

			std::string headStr = "vipLevel,discountRate,discountEffectTimes\n";
			ofs.write(headStr.c_str(), headStr.size());

			for (const auto& fileElem : myFileElems)
			{
				std::ostringstream oss;
				oss << m_Lv << "," << fileElem.rate << "," << fileElem.times << "\n";
				ofs.write(oss.str().c_str(), oss.str().size());
			}

			m_Finish = true;
		}

		bool IsFinish() const { return m_Finish; }

	public:
		bool   m_Finish;
		UInt32 m_AccId;
		UInt32 m_Lv;
		UInt32 m_GenCnt;
	};

	class MyGeneratorMgr : public Avalon::Thread
	{
	public:
		MyGeneratorMgr(UInt32 accid, UInt32 lv, UInt32 genCnt)
		{
			auto thread = new MyGenerator(accid, lv, genCnt);
			m_Threads.push_back(thread);
		}

		~MyGeneratorMgr()
		{
			for (auto thread : m_Threads)
			{
				thread->Stop();
				thread->Join();
				delete thread;
			}

			m_Threads.clear();
		}

		virtual void Run()
		{
			for (auto thread : m_Threads)
			{
				thread->Activate();
			}

			while (true)
			{
				bool allFinish = true;
				for (auto thread : m_Threads)
				{
					if (!thread->IsFinish())
					{
						allFinish = false;
						break;
					}
				}

				if (allFinish)
				{
					break;
				}

				Sleep(1);
			}

			m_Threads.clear();
		}

	public:
		std::vector<MyGenerator*>		m_Threads;
	};

	const char* GetCmdName() const { return "afjed"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		UInt32 lv = params.GetParam("lv");
		UInt32 cnt = params.GetParam("cnt");
		auto thread = new MyGeneratorMgr(player->GetAccID(), lv, cnt);
		thread->Activate();
		return true;
	}
};

class SpecifyRobotCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "robot"; }

	bool Execute(Player* player, const CLParameter& params)
	{
		CLProtocol::MatchServerSpecifyPkRobot protocol;
		protocol.occu = params.GetParam("occu");
		protocol.hard = params.GetParam("hard");
		protocol.ai = params.GetParam("ai");
		Router::SendToMatch(protocol);
		return true;
	}
};

class PlayerSetCounterCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setCnt"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		std::string name = params.GetParam("name").GetString();
		UInt32 cnt = params.GetParam("cnt");
		UInt32 type = params.GetParam("type");

		// 默认为自然天的
		if (type == 0)
			type = COUNTER_CYCLE_DAY;

		if (name.length() != 0)
		{
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(name);
			if (NULL == counterConfig)
			{
				CounterCfg::Instance()->RegCounter(name.c_str(), type);
			}

			pPlayer->SetCounter(name.c_str(), cnt);
		}

		return true;
	}
};

class LostDungeonSetTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "lsTask"; }

	bool Execute(Player* pPlayer, const CLParameter& params)
	{
		UInt32 taskId = params.GetParam("id");
		pPlayer->GetLostDungeonTaskMgr().HandlerChoiceTask(taskId);

		return true;
	}
};

GMSystem::GMSystem()
{
	RegisterHandlers();
}

GMSystem::~GMSystem()
{
	for(HandlerMap::iterator iter = m_Handlers.begin();
		iter != m_Handlers.end();++iter)
	{
		CL_SAFE_DELETE(iter->second.second);
	}
	m_Handlers.clear();
}

bool GMSystem::ExecuteCommand(Player* player,const std::string& cmd)
{
	std::string::size_type index = cmd.find_first_of(" \n\t\r");
	if(index != std::string::npos)
	{
		std::string cmdname = cmd.substr(0,index);
		if(cmdname.empty()) return false;
		
		CLParameter param;
		if(!param.ParseParams(cmd.substr(index+1))) return false;

		return ExecuteCommand(player,cmdname,param);
	}
	else
	{ 
		CLParameter param;
		return ExecuteCommand(player,cmd,param);
	}
}

bool GMSystem::ExecuteCommand(Player* player, const std::string& cmd, const CLParameter& params)
{
	HandlerMap::iterator iter = m_Handlers.find(cmd);

#ifdef _DEBUG
	if (iter != m_Handlers.end())
	{
		return iter->second.second->Execute(player, params);
	}
	else
	{
		CLProtocol::SysTransmitGmCommand req;
		req.playerid = player->GetID();
		req.cmdname = cmd;
		req.params = params;
		Router::SendToWorld(req);
	}
	GameInfoLogStream("GMCommand") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ")"
		"executed command:" << cmd << " params:" << params.ToString() << LogStream::eos;

	return false;
#else

	if (iter != m_Handlers.end())
	{
		if (!player->HasGmAuthority(iter->second.first))
		{
			return false;
		}

		iter->second.second->Execute(player, params);
	}
	else
	{
		if (!player->HasGmAuthority(GM_AUTHORITY_DEBUG))
		{
			return false;
		}
		
		CLProtocol::SysTransmitGmCommand req;
		req.playerid = player->GetID();
		req.cmdname = cmd;
		req.params = params;
		Router::SendToWorld(req);
	}
	GameInfoLogStream("GMCommand") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ")"
		"executed command:" << cmd << " params:" << params.ToString() << LogStream::eos;
#endif
	return true;
	
}

bool GMSystem::ExecuteCommand(ObjID_t playerId, const std::string& cmd, const CLParameter& params)
{
	HandlerMap::iterator iter = m_Handlers.find(cmd);
	if(iter != m_Handlers.end())
	{
		return iter->second.second->Execute(playerId, params);
	}

	return false;
}

void GMSystem::RegisterHandlers()
{
	// system
	RegisterHandler(GM_AUTHORITY_NORMAL, new ReloadScriptsCmdHandler());
	RegisterHandler(GM_AUTHORITY_NORMAL, new ReloadScriptCmdHandler());

	// player
	RegisterHandler(GM_AUTHORITY_SUPER, new AddExpCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new UpLevelCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddGoldCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddBindGoldCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new RemoveSilverCmdHandler());		//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new AddSPCmdHandler());		//added by huchenhui by 2016.07.12
	RegisterHandler(GM_AUTHORITY_SUPER, new AddPointCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddBindPointCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddFatigueCmdHandler());	//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new RemoveFatigueCmdHandler());	//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new ChangeOccuCmdHandler());	//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new AwakenCmdHandler());		//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new AddVipExpHandler());		//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new RemovePointHandler());		//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new SetBootHandler());			//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new ResetBootHandler());		//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new SetBootFlagHandler());			//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new ResetBootFlagHandler());		//added by huchenhui
	RegisterHandler(GM_AUTHORITY_SUPER, new AddGoldJarPointCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddMagJarPointCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new ChangeNameCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new ChangeSceneCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new BuyMoneyManageCmdHandler());

	// item
	RegisterHandler(GM_AUTHORITY_SUPER, new AddItemCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SellItemCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new UseItemCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ClearItemsCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new PushStorageHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new PullStorageHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new DecomposeCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthenCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddMagicCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new MagicCardCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new BatchAddItem());
	RegisterHandler(GM_AUTHORITY_DEBUG, new OpenJar());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TestGetStrMaxLv());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ResetEquipRecoHandler());
#ifdef _DEBUG
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthSucCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new HappySkyCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new CalcEqScoreHandler());

#endif // _DEBUG
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthSimCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new OpenJarSampleCmdHandler());
	
	

	
	RegisterHandler(GM_AUTHORITY_DEBUG, new QueryShopCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new BuyShopCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new RefreshShopCmdHandler());
	
	// task
	RegisterHandler(GM_AUTHORITY_SUPER, new SubmitTaskCmdHandler());    //added by aprilliu
	RegisterHandler(GM_AUTHORITY_SUPER, new AbandonTaskCmdHandler());	//added by aprilliu
	RegisterHandler(GM_AUTHORITY_SUPER, new ResetTaskCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AcceptTaskCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new RefreshDailyTaskCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new ActiveTaskDayChange());
	RegisterHandler(GM_AUTHORITY_SUPER, new RefreshCycleTask());
	RegisterHandler(GM_AUTHORITY_SUPER, new RefreshCycleDailyCountTask());
	RegisterHandler(GM_AUTHORITY_SUPER, new SubmitCycleTask());
	RegisterHandler(GM_AUTHORITY_SUPER, new SetTaskItem());
	RegisterHandler(GM_AUTHORITY_SUPER, new SetDailyTaskStatus());
	RegisterHandler(GM_AUTHORITY_SUPER, new SetTaskStatus());
	RegisterHandler(GM_AUTHORITY_SUPER, new FinishAllTaskStatus());
	RegisterHandler(GM_AUTHORITY_SUPER, new FinishMainTask());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddAchievementScoreCmdHandler());

	// mail
	RegisterHandler(GM_AUTHORITY_DEBUG, new BroadcastMailHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TestSysMailHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TestItemMailHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TestMailsHandler());

	// skill
	RegisterHandler(GM_AUTHORITY_NORMAL, new ClearSkillsCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new ChangeSkillCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddBuffCmdHandler());

	// chat
	RegisterHandler(GM_AUTHORITY_DEBUG, new RequestCmdHandler());

	// pet
	RegisterHandler(GM_AUTHORITY_SUPER, new UpdateDailyActivityCmdHandler());
	
	RegisterHandler(GM_AUTHORITY_NORMAL, new AddSocialConsume());
	
	RegisterHandler(GM_AUTHORITY_SUPER, new AddHonourCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddIronStoneCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddSpiritStoneCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new AddJadeStoneCmdHandler());
	RegisterHandler(GM_AUTHORITY_SUPER, new SetLevelCmdHandler());
	
	RegisterHandler(GM_AUTHORITY_DEBUG, new RestoreBKCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddIntimacyCmdHandler());
	
	RegisterHandler(GM_AUTHORITY_DEBUG, new ForbidTalkHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ForbidLoginHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AnnounceHandler());
	
	RegisterHandler(GM_AUTHORITY_DEBUG, new TestConsumeHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SysNotifyHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new OpenFunctionsHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ConsumePoint());
	
    RegisterHandler(GM_AUTHORITY_DEBUG, new AddPkValueCmdHandler());
    RegisterHandler(GM_AUTHORITY_DEBUG, new OpenDungeonCmdHandler());
    RegisterHandler(GM_AUTHORITY_DEBUG, new ReportMemoryTrackerCmdHandler());
    RegisterHandler(GM_AUTHORITY_DEBUG, new CreateTeamCmdHandler());
    RegisterHandler(GM_AUTHORITY_DEBUG, new JoinTeamCmdHandler());
    RegisterHandler(GM_AUTHORITY_DEBUG, new DismissTeamCmdHandler());

    RegisterHandler(GM_AUTHORITY_DEBUG, new ChallengeCmdHandler());

	//随从 adder by huchenhui
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddRetinueCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SetRetinueCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new RetinueChangeSkillCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new RetinueSetSkillCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ResetRetinueChangeSkillCountCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new UpLevelRetinueCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new UpStarRetinueCmdHandler());

	//宠物
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddPetCmdHandler());

	//Chapter adder by huchenhui 2016.08.02
	RegisterHandler(GM_AUTHORITY_DEBUG, new FinishChapter1Handler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new FinishChapter2Handler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new FinishChapter3Handler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new FinishChapter4Handler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new FinishChapter5Handler());
	RegisterHandler(GM_AUTHORITY_SUPER, new ToTaskCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new AddWarriorSoulCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddPkCoinCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new QueryPlayerInfoCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new MatchScoreCmdHandler());

	//SortList
	RegisterHandler(GM_AUTHORITY_DEBUG, new SortListGiveRewardCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new ResetDungeonNumCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ResetTowerCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ResetTowerCounterCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SetTowerTopFloorCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TowerWipeoutCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TowerWipeoutResultCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TowerQuickFinishCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new TowerReceiveFloorAwardCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new GuildExchangeCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ClearPkStatisticCmdHelper());

	RegisterHandler(GM_AUTHORITY_DEBUG, new WudaoJoinCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new WudaoRewardCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new AddRacePkCoinCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new BuyMonthCardCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new BuyDungeonTimesCmdHandler());

	//Season
	RegisterHandler(GM_AUTHORITY_DEBUG, new SetSeasonLevelCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SetSeasonRecordCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddSeasonExpCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new DecSeasonExpCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new PvPCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ShowMatchScoreCmdHandler());

	//Pet
	RegisterHandler(GM_AUTHORITY_DEBUG, new RemovePetHungerCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new AddPetHungerCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new SendHornCmdHandler());

	//help
	RegisterHandler(GM_AUTHORITY_DEBUG, new GMSystemHelp());
	RegisterHandler(GM_AUTHORITY_DEBUG, new UsedOpActHellTicketCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new RemoveOpActFatigueCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SubmitOpActTaskCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new DungeonDropCounterCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new SummonCityMonsterCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ClearCityMonsterCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new StartCityMonsterDungeomCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new SceneArtifactJarExtractDiscountReqCmdHandler());

	//师门
	RegisterHandler(GM_AUTHORITY_DEBUG, new MasterSysSetDailyTaskCompsHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new MasterSysAddGrowthHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new ScoreWarChangeStatusHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ScoreWarSetBattleCountHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ScoreWarSetWinBattleCountHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ScoreWarSetBattleScoreHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new ScoreWarGetRewardHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new SysAddIncDailyTaskScore());
	RegisterHandler(GM_AUTHORITY_DEBUG, new GCGMCmdHandler());
	
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthenTicketSynthesisCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthenTicketFuseCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthenTickeSynthesisPlanGenerateCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthenTickeFusePlanGenerateCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new StrengthenTicketFusePlanCalcCmdHandler());
	RegisterHandler(GM_AUTHORITY_DEBUG, new PlayerClearDungeonForActivityTaskCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new SpecifyRobotCmdHandler());

	RegisterHandler(GM_AUTHORITY_DEBUG, new PlayerSetCounterCmdHandler());

	// 爬塔
	RegisterHandler(GM_AUTHORITY_DEBUG, new LostDungeonSetTaskCmdHandler());
}

void GMSystem::RegisterHandler(UInt8 level,GMCmdHandler* handler)
{
	if(handler == NULL) return;
	HandlerMap::iterator iter = m_Handlers.find(handler->GetCmdName());
	if(iter != m_Handlers.end())
	{
		CL_SAFE_DELETE(iter->second.second);
		iter->second.first = level;
		iter->second.second = handler;
	}
	else
	{
		m_Handlers.insert(std::make_pair(handler->GetCmdName(),std::make_pair(level,handler)));
	}
}

std::vector<std::string> GMSystem::GetCmdHelp(std::string str)
{
	std::vector<std::string> result;
	HandlerMap::iterator iter = m_Handlers.begin();
	while (iter != m_Handlers.end())
	{
		if (iter->second.second == NULL) continue;
		std::string cmdName(iter->second.second->GetCmdName());
		if (str.empty() || cmdName.find(str) != std::string::npos)
		{
			cmdName = "!!" + cmdName + " " + iter->second.second->GetHelp();
			result.push_back(cmdName);
		}
		++iter;
	}
	return result;
}
