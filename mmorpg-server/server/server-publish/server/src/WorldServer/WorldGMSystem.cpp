#include "WorldGMSystem.h"
#include <CLGameDefine.h>
#include <CLCenterProtocol.h>
#include <CLPremiumLeagueProtocol.h>
#include <CLMsgParser.h>
#include <CLMallDataEntry.h>
#include <CLScoreWarProtocol.h>
#include <CLGlobalActivityProtocol.h>
#include <CLRecordClient.h>
#include <AbnormalTransHelper.h>
#include <CLBlackMarketProtocol.h>
#include <CLAdventureTeamProtocol.h>
#include <CLShopProtocol.h>
#include <CLForbitWord.h>
#include <CLNewTitleDataEntry.h>

#include "WSPlayer.h"
#include "PunishmentMgr.h"
#include "AnnouncementMgr.h"
#include "BroadcastMailMgr.h"
#include "ScriptMgr.h"
#include "WSActivityMgr.h"
#include "WSSortListMgr.h"
#include "GuildMgr.h"
#include "RedPacketMgr.h"
#include "GuildBattleMgr.h"
#include "WSServiceSwitchMgr.h"
#include "ChargeManager.h"
#include "TeamMatchMgr.h"
#include "SysRecordMgr.h"
#include "AccountMgr.h"
#include "CLSystemValueDataEntry.h"
#include "WSSeasonMgr.h"
#include "CLOpActiveProtocol.h"
#include "PremiumLeagueMgr.h"
#include "WSActivityMonsterMgr.h"
#include "AccountActivityTaskMgr.h"
#include "RoomMgr.h"
#include "WSRouter.h"
#include "DigMapMgr.h"
#include "MallMgr.h"
#include "LoginPushMgr.h"
#include "WSPlayerMgr.h"
#include "DungeonRace.h"
#include "DungeonRaceMgr.h"
#include "WSConsoleAgent.h"
#include "GuildDungeonMgr.h"
#include "ExpeditionMap.h"
#include "WSShopMgr.h"
#include "GuildAuctionMgr.h"
#include "WSItemTransactionUtil.h"
#include "PlayerJoins.h"
#include "AdventurePassMgr.h"
#include "GameParamMgr.h"

class AddIntimacyCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const{ return "addintimacy"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		std::string name = params.GetParam("name").GetString();
		UInt16 val = params.GetParam("num");

		WSRelation* relation = player->GetRelationMgr().FindRelation(RELATION_FRIEND, name);
		if(relation == NULL) return;

		player->AddIntimacy(relation->GetID(), val);
	}
};

class ForbidTalkHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "forbidtalk"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 userid = params.GetParam("userid");
		UInt32 time = params.GetParam("time");

		PunishmentMgr::Instance()->Punish(userid, PUNISH_FORBIDTALK, time);
	}
};

class MonopolyHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "mono"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");

		pPlayer->GetMonopolyInfo().ForwardToMap(num);
	}
};

class MonopolyBuffHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "monobuff"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");

		pPlayer->GetMonopolyInfo().SetBuff(num);
	}
};

class ForbidLoginHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "forbidlogin"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 userid = params.GetParam("userid");
		UInt32 time = params.GetParam("time");

		PunishmentMgr::Instance()->Punish(userid, PUNISH_FORBIDLOGIN, time);
	}
};

class UnpunishCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "unpunish"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 userid = params.GetParam("userid");
		UInt8 type = params.GetParam("type");
		if (userid == 0)
		{
			userid = pPlayer->GetAccId();
		}

		PunishmentMgr::Instance()->UnPunish(userid, type);
	}
};

class AnnounceTestHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "announcetest"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string systemLinkStr;
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_ACTIVITY, 0);
		AnnouncementMgr::Instance()->PublicAnnouncement(0, 57, UInt32(CURRENT_TIME.Sec()), systemLinkStr);
	}
};

class AnnounceHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "announce"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{

		UInt32 id = params.GetParam("id");
		std::string p1 = params.GetParam("p1").GetString();
		std::string p2 = params.GetParam("p2").GetString();
		std::string p3 = params.GetParam("p3").GetString();

		if (p1.empty())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, id, UInt32(CURRENT_TIME.Sec()));
		}
		else if (p2.empty())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, id, UInt32(CURRENT_TIME.Sec()), p1.c_str());
		}
		else if (p3.empty())
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, id, UInt32(CURRENT_TIME.Sec()), p1.c_str(), p2.c_str());
		}
		else
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(0, id, UInt32(CURRENT_TIME.Sec()), p1.c_str(), p2.c_str(), p3.c_str());
		}
	}
};

class BroadcastMailGMHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "broadcastmail"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		std::string senderName = params.GetParam("sendername").GetString();
		std::string title = params.GetParam("title").GetString();
		std::string content = params.GetParam("content").GetString();
		std::vector<ItemReward>	items;
		std::string reason = GetReason(SOURCE_TYPE_GM);
		UInt8 occu = params.GetParam("occu");
		UInt32 playerChargeNum = params.GetParam("playerChargeNum");
		BroadcastMailMgr::Instance()->PutMail(senderName, title, content, reason, items, 0, "xy,test,i4,7977,mg,hw,oppo,vivo,", occu, playerChargeNum);
	}
};

class ReloadScriptsCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "reloadscripts"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if(ScriptMgr::Instance()->ReloadScripts())
		{
			player->SendNotify(201, NODE_ID);
		}
		else
		{
			player->SendNotify(202, NODE_ID);
		}
	}
};

class ReloadScriptCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const{ return "wsreloadscript"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 type = params.GetParam("type");
		ScriptMgr::Instance()->ReloadScript(type);
	}
};

/**
*@brief 更新每日活动
*/
class UpdateDailyActivityCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "updateDailyActivity"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		UInt16 lv = params.GetParam("lv");
		std::string startTime = params.GetParam("starttime").GetString();
		UInt32 lastTime = params.GetParam("lasttime");

		WSActivityMgr::Instance()->UpdateDailyActivity(id, lv, startTime, 0, lastTime);
	}
};

/**
*@brief 排行榜每日奖励
*/
class SortListGiveRewardCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "slgive"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		WSSortListMgr::Instance()->TestGiveReward();
	}
};

/**
*@brief 创建公会
*/
class CreateGuildCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "cguild"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildMgr::Instance()->OnPlayerRequestCreateGuild(pPlayer, "test", "fuck");
	}
};

/**
*@brief 公会列表
*/
class GuildListCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildlist"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildMgr::Instance()->OnPlayerRequestGuildList(pPlayer, 0, 10);
	}
};

/**
*@brief 加入公会
*/
class JoinGuildCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "joinguild"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string name = params.GetParam("name").GetString();
		auto guild = GuildMgr::Instance()->FindGuildByName(name);
		if (guild)
		{
			GuildMgr::Instance()->OnPlayerRequestJoinGuild(pPlayer, guild->GetID());
		}
	}
};

/**
*@brief 修改公会繁荣度状态
*/
class GuildProsperityCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "gp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt8 status = params.GetParam("status");
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->SetProsperity(status);
		}
	}
};


/**
*@brief 修改公会繁荣度状态
*/
class GuildOnWeekCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ow"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildMgr::Instance()->OnWeekChanged();
	}
};


/**
*@brief 冒险通行证
*/
class AdventurePassCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ap"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		AdventurePassMgr::Instance()->AddAdventurePass(pPlayer->GetAccId());
		auto pass = AdventurePassMgr::Instance()->GetAdventurePass(pPlayer->GetAccId());
		pass->InsertToDB();
	}
};


/**
*@brief 回复加入公会
*/
class ReplyGuildCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "replyguild"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		ObjID_t target = params.GetParam("target");
		UInt8 agree = params.GetParam("agree");

		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->ReplyJoinRequest(pPlayer, target, agree != 0);
		}
	}
};

/**
*@brief 修改公会名
*/
class ChangeGuildNameCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildname"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string name = params.GetParam("name").GetString();

		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestChangeGuildName(pPlayer, name, 0, 0);
		}
	}
};

/**
*@brief 任命职务
*/
class ChangeGuildPostCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildpost"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt64 targetId = params.GetParam("t");
		UInt8 post = params.GetParam("p");
		UInt64 replaceId = params.GetParam("r");

		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestChangePost(pPlayer, targetId, replaceId, (GuildPost)post);
		}
	}
};

/**
*@brief 任命职务
*/
class UpgradeGuildBuildingCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildbuild"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt8 type = params.GetParam("type");

		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestUpgradeBuilding(pPlayer, (GuildBuildingType)type);
		}
	}
};

/**
*@brief 捐赠
*/
class GuildDonateCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guilddonate"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt8 type = params.GetParam("type");
		UInt8 num = params.GetParam("num");

		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestDonate(pPlayer, (GuildDonateType)type, num);
		}
	}
};

/**
*@brief 升级公会技能
*/
class GuildUpgradeSkillCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildskill"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt16 skillId = params.GetParam("id");

		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestUpgradeSkill(pPlayer, skillId);
		}
	}
};

/**
*@brief 解散公会
*/
class GuildDismissCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guilddismiss"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestDismiss(pPlayer);
		}
	}
};

/**
*@brief 发送公会邮件
*/
class GuildSendMailCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildmail"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->SendGuildMail(pPlayer, "123");
		}
	}
};

/**
*@brief 添加公会资金
*/
class AddGuildFundCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addfund"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->AddFund(params.GetParam("num"));
		}
	}
};

/**
*@brief 公会膜拜
*/
class GuildOrzCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildorz"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestOrz(pPlayer, (GuildOrzType)(UInt8)params.GetParam("type"));
		}
	}
};

/**
*@brief 圆桌会议
*/
class JoinGuildTableCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "table"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->RequestJoinTable(pPlayer, (UInt8)params.GetParam("seat"), (UInt8)params.GetParam("type"));
		}
	}
};

/**
*@brief 公会自费红包
*/
class GuidPayRedPacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "guildrp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild)
		{
			guild->OnRequestPayRedPacket(pPlayer, (UInt16)params.GetParam("reason"), "", (UInt8)params.GetParam("num"));
		}
	}
};

/**
*@brief 自费红包
*/
class CustomRedPacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "customrp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		RedPacketMgr::Instance()->OnPlayerSendCustomRedPacket(pPlayer, (UInt16)params.GetParam("reason"), "", (UInt8)params.GetParam("num"));
	}
};

/**
*@brief 添加红包
*/
class AddRedPacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addrp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt16 reason = params.GetParam("reason");
		ObjID_t id = 0;
		RedPacketMgr::Instance()->OnPlayerAddRedPacket(pPlayer, reason, "", id);
	}
};

/**
*@brief 发送红包
*/
class SendRedPacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "sendrp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		RedPacketMgr::Instance()->OnPlayerSendRedPacket(pPlayer->GetID(), 0);
	}
};

/**
*@brief 新年红包结算
*/
class NewYearRedPacketEndCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "nyrpe"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		WSSortListMgr::Instance()->OnNewYearRedPacketEnd();
	}
};

class GetSystemValueCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "getsv"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 tmp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_RETINUE_SKILL);
		DebugLogStream << "player(" << pPlayer->GetID() << ") skill count(" << tmp << ")." << LogStream::eos;
	}
};

/**
*@brief 打开红包
*/
class OpendRedPacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "openrp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		RedPacketMgr::Instance()->OnPlayerOpenRedPacket(pPlayer, 0);
	}
};

class GuildBattleByTimeHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "gbbytime"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildBattleMgr::Instance()->SetIsByTime(true);
	}
};

class GuildBattleChangeStatusHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "gbcstatus"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildBattleStatus status = (GuildBattleStatus)((UInt32)params.GetParam("status"));
		GuildBattleType type = (GuildBattleType)((UInt32)params.GetParam("type"));
		if (type == GuildBattleType::GBT_INVALID)
			type = GuildBattleType::GBT_NORMAL;
		GuildBattleMgr::Instance()->SetIsByTime(false);
		GuildBattleMgr::Instance()->ChangeTargetStatus(status);
		GuildBattleMgr::Instance()->SetGuildBattleType(type);

		CLProtocol::CrossGuildBattleGMSync sync;
		if (type == GuildBattleType::GBT_CROSS)
		{
			sync.isByTime = false;
			sync.targetStatus = status;
	}
		else
		{
			sync.isByTime = true;
		}
		Router::SendToCross(sync);
	}
};

class GuildBattleEnrollCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "gbenroll"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt8 terrId = params.GetParam("id");
		GuildBattleMgr::Instance()->OnRequestEnroll(pPlayer, terrId);
	}
};

class GuildBattleHistoryTerrCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gbht"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild != NULL)
		{
			UInt8 terrId = params.GetParam("id");
			guild->SetHistoryTerrId(terrId);

			DebugLogStream << PLAYERINFO(player) << "set history terr id:" << terrId << LogStream::eos;
		}
	}
};

class GuildMatchBattleCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "bmatch"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildBattleMgr::Instance()->OnMatchBattleStart(pPlayer);
	}
};

class GuildBattleInspireCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "binspire"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt8 tmpInspire = 0;
		GuildBattleMgr::Instance()->OnRequestInspire(pPlayer, tmpInspire);
	}
};

class GuildBattleAddRecordCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addrecord"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;
		if (!GuildBattleMgr::Instance()->IsGuildBattle())
		{
			return;
		}

		UInt8 num = params.GetParam("num");

		for (UInt32 i = 0; i < num; ++i)
		{
			GuildBattleMgr::Instance()->AddTestRecord(pPlayer);
		}
	}

};

class GuildBattleSetNumberCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "sbnumber"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;
		UInt32 num = params.GetParam("num");

		GuildMember* member = GuildMgr::Instance()->FindGuildMember(pPlayer->GetID());
		member->SetGuildBattleNumber(num);
	}

};

class GuildBattleLotteryCmdHandler :public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "gblottery"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;

		UInt8 isWin = params.GetParam("win");

		Guild* guild = GuildMgr::Instance()->FindGuildByID(pPlayer->GetGuildID());
		if (guild == NULL) return;
		guild->Lottery(pPlayer, isWin == 1, false);
		return;
	}
};

class GuildBattleAddScoreCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addgs"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;
		UInt32 num = params.GetParam("num");

		GuildMember* member = GuildMgr::Instance()->FindGuildMember(pPlayer->GetID());
		if (member == NULL) return;
		member->SetGuildBattleScore(member->GetGuildBattleScore() + num);
		pPlayer->OnIncGuildBattleScore(num);
		return;
	}
};

class GuildBattleCrossTimeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "getct"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;

		GuildBattleTimeDataEntryMgr::Instance()->GetGuildCrossBattleEndTime(CURRENT_TIME);
		return;

	}
};

class SetSeasonIdCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setsid"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;
		UInt32 id = params.GetParam("id");

		WSSeasonMgr::Instance()->SetSeasonId(id);
	}
};

class DeleteSortListCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "delsl"; }
	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		if (pPlayer == NULL) return;
		WSSortListMgr::Instance()->DeleteAllEntry(pPlayer->GetID());
	}
};

class ChangeActiveCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "changeactive"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		UInt8 state = params.GetParam("state");

		WSActivityMgr::Instance()->GmOpenActive(id, state);
	}
};

class ChargeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "charge"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string orderId = Avalon::StringUtil::ConvertToString<ObjID_t>(CLRecordClient::Instance()->GenGuid());
		UInt32 goodsId = params.GetParam("id");
		UInt32 money = params.GetParam("money");
		UInt8 mallType = params.GetParam("mallType");
		UInt32 accid = params.GetParam("accid");
		ObjID_t roleId = params.GetParam("roleid");

		if (accid == 0 || roleId == 0)
		{
			accid = pPlayer->GetAccId();
			roleId = pPlayer->GetID();
		}
		if (mallType == CHARGE_MALL_CHARGE || mallType == CHARGE_MALL_MONEY_MANAGE || mallType == CHARGE_DAY_CHARGE_WELFARE ||
			mallType == CHARGE_ADVENTURE_PASS_KING || mallType == CHARGE_GIGT_RIGHT_CARD || mallType == CHARGE_DUNGEON_RIGHT_CARD)
		{
			ChargeManager::Instance()->StartCharge(0, "gm", orderId, "", accid, roleId, goodsId, money, (UInt32)CURRENT_TIME.Sec(), ChargeMallType(mallType));
		}
		else
		{
			ChargeManager::Instance()->BuyChargePacket(0, "gm", orderId, "", accid, roleId, goodsId, money, (UInt32)CURRENT_TIME.Sec());
		}
	}
};

class ReChargeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "recharge"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string orderId = params.GetParam("order").GetString();
		ChargeManager::Instance()->ReCharge(0, orderId);
	}
};

class ResetChargeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "resetcharge"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		pPlayer->ResetChargeGoodsTimes();
	}
};

class TeamMatchCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "tm"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		TeamMatchMgr::Instance()->OnPlayerJoinMatch(pPlayer, params.GetParam("id"));
	}
};

class RareItemCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "r"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 itemId = params.GetParam("item");
		UInt32 num = params.GetParam("num");

		auto remainNum = SysRecordMgr::Instance()->RareItemRemainNum(pPlayer->GetID(), itemId, num);
		num = num > remainNum ? remainNum : num;
		SysRecordMgr::Instance()->RecordGotRareItem(pPlayer->GetID(), itemId, num);
	}
};

class ChargePacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "cp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		ChargePacket packet;

		packet.id = params.GetParam("id");
		packet.money = params.GetParam("money");
		packet.vipScore = params.GetParam("vip");
		packet.limitDailyNum = params.GetParam("dailylimit");
		packet.limitTotalNum = params.GetParam("totallimit");
		packet.startTime = params.GetParam("start");
		packet.endTime = params.GetParam("end");

		std::string rewardsStr = params.GetParam("rewards").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(rewardsStr, strs, "|");
		for (auto rewardStr : strs)
		{
			std::vector<std::string> tmpStrs;
			Avalon::StringUtil::Split(rewardStr, tmpStrs, ":");
			if (tmpStrs.size() != 2)
			{
				return;
			}

			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(tmpStrs[0]);
			UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpStrs[1]);
			packet.rewards.push_back(ItemReward(itemId, num, 1, 0));
		}

		ChargeManager::Instance()->AddChargePacket(packet);
	}
};

class BuyChargePacketCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "buycp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string orderId = params.GetParam("order").GetString();
		UInt32 goodsId = params.GetParam("id");
		UInt32 money = params.GetParam("money");

		UInt32 accid = params.GetParam("accid");
		ObjID_t roleId = params.GetParam("roleid");

		if (accid == 0 || roleId == 0)
		{
			accid = pPlayer->GetAccId();
			roleId = pPlayer->GetID();
		}

		ChargeManager::Instance()->BuyChargePacket(0, "gm", orderId, "", accid, roleId, goodsId, money, (UInt32)CURRENT_TIME.Sec());
	}
};

class LoadAccountCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "loadaccount"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		class TestAccountCallback : public AccountCallback

		{
		public:
			TestAccountCallback(UInt32 accid) : AccountCallback(accid) {}
			virtual void OnFinish(AccountInfo* info)
			{
				DebugLogStream << "account(" << m_AccId << ") load finish." << LogStream::eos;
			}
			virtual void OnFailure() {}
		};

		AccountMgr::Instance()->LoadAccountInfo(pPlayer->GetAccId(), new TestAccountCallback(pPlayer->GetAccId()));
	}
};

class AuctionItemNumCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "auction"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::AuctionQueryCondition cond;
		cond.itemMainType = params.GetParam("main");
		std::string subTypeStr = params.GetParam("sub").GetString();
		std::vector<std::string> paramsStr;
		Avalon::StringUtil::Split(subTypeStr, paramsStr, "|");

		for (auto param : paramsStr)
		{
			UInt32 subType = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			cond.itemSubTypes.push_back(subType);
		}

		zjy_auction::AuctionMgr::Instance()->OnPlayerQueryAuctionItemNum(pPlayer, cond);
	}
};

class CDKeyVerifyCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "cdkverify"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		
		std::string cdk = params.GetParam("cdk").GetString();

		CLProtocol::WCCDKVerifyReq centerReq;
		centerReq.accId = pPlayer->GetAccId();
		centerReq.playerUid = pPlayer->GetID();
		centerReq.platform = pPlayer->GetPf();
		centerReq.serverId = ZONE_ID;
		centerReq.cdk = cdk;
		centerReq.reason = params.GetParam("reason");

		Router::SendToCenter(centerReq);
	}
};


//激活商城限时礼包
class ActivateGiftPack : public GMCmdHandler
{
	const char* GetCmdName() const { return "activateGift"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt8 val = params.GetParam("cond");

		player->GetMallGiftPackMgr().ActivateLimitGiftPack(val);
	}
};

struct DropStatistic
{
	DropStatistic()
	{
		memset(this, 0, sizeof(*this));
	}

	UInt32 itemId;
	// 掉落数量
	UInt64 num;
	// 掉落次数
	UInt64 count;
};

// 统计地下城道具掉落
class CountDungenDrop : public GMCmdHandler
{
	const char* GetCmdName() const { return "startRace"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 dungeonId = params.GetParam("id");
		UInt32 times = params.GetParam("times");

		Avalon::SockAddr addr;
		UInt32 hellDungeonEntryId = dungeonId;

		auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
		bool isHell = dungeonData->IsHellEntry();


		//dungeonRace->OpenDropCount();

		UInt64 startTime = Avalon::Time::CurrentTime().MSec();

		std::map<DropKey, DropStatistic> dropStatics;

		//for (UInt32 idx = 0; idx != times; idx++)
		{
			if (isHell)
			{
				dungeonId = DungeonRaceMgr::Instance()->GenHellDungeonID(dungeonId);
			}

			auto dungeonRace = DungeonRaceMgr::Instance()->CreateDungeonRace(dungeonId);
			if (!dungeonRace)
			{
				ErrorLogStream << "DungeonRace prt is null!" << LogStream::eos;
				return;
			}

			dungeonRace->SetHellDungeonEntryID(hellDungeonEntryId);

			DungeonRacePlayerInfo playerInfo;
			playerInfo.raceInfo.roleId = player->GetID();
			playerInfo.accId = player->GetAccId();
			playerInfo.zoneId = player->GetZoneId();
			playerInfo.raceInfo.occu = player->GetOccu();
			playerInfo.raceInfo.level = player->GetLevel();
			playerInfo.totalCount = 0;
			playerInfo.isAssist = 0;

			auto roleid = playerInfo.raceInfo.roleId;
			DungeonRaceMgr::Instance()->LeaveDungeonRace(roleid, false);
			DungeonRaceMgr::Instance()->JoinDungeonRace(dungeonRace, playerInfo);

			dungeonRace->SetPassedAreaIndex(0);

			if (isHell)
			{
				dungeonRace->SetHell();
			}

			dungeonRace->SetDropCountState(true);
			dungeonRace->GenDropStatistic(times);

			auto& drops = dungeonRace->GetDropSta();
			for (auto& drop : drops)
			{
				auto& sta = dropStatics[drop.first];
				sta.num += drop.second.num;
				sta.count += drop.second.times;
			}

			DungeonRaceMgr::Instance()->DestoryDungeonRace(dungeonRace);
		}
		UInt64 endTime = Avalon::Time::CurrentTime().MSec();
		UInt64 usedTime = endTime - startTime;

		InfoLogStream << PLAYERINFO(player) << ", params(dungenId: " << dungeonId << ", isHell: " << isHell << ", times: " << times << "), used time: " << usedTime << "msec." << LogStream::eos;

		/*for (auto itr : dropStatics)
		{
		DebugLogStream << itr.first << ", " << itr.second.num << ", " << itr.second.count << LogStream::eos;
		}*/
		RecordDropCountToFile(player->GetAccId(), dungeonId, isHell, times, dropStatics);
	}

public:
	static void RecordDropCountToFile(UInt32 accid, UInt32 dungenId, UInt8 isHell, UInt32 times, std::map<DropKey, DropStatistic>& dropStas, DungeonHellMode hellMode = HELL_MODE_NULL)
	{
		std::string rootpath = WSApplication::Instance()->GetDropCountPath();
		std::string accidStr = Avalon::StringUtil::ConvertToString(accid);
		std::string dungenIdStr = Avalon::StringUtil::ConvertToString(dungenId);
		std::string isHellStr = Avalon::StringUtil::ConvertToString(isHell);
		std::string timesStr = Avalon::StringUtil::ConvertToString(times);
		std::string hellModeStr = Avalon::StringUtil::ConvertToString((UInt32)hellMode);

		std::string postfix = accidStr + "_" + dungenIdStr + "_" + isHellStr + "_" + timesStr + "_" + hellModeStr + ".csv";
		std::string filename = rootpath + "DropItemCount_" + postfix;

		std::ofstream ofs;
		ofs.open(filename.c_str());
		if (!ofs.good())
		{
			ErrorLogStream << "FILE(" << filename << ") ptr is null!" << LogStream::eos;
			return;
		}

		std::string header = "itemid,equipType,name,times,num,color\n";
		ofs.write(header.c_str(), header.length());
		for (auto itr : dropStas)
		{
			auto data = ItemDataEntryMgr::Instance()->FindEntry(itr.first.itemTypeId);
			if (!data)
			{
				continue;
			}

			std::ostringstream oss;
			oss << itr.first.itemTypeId << ", " << (UInt32)itr.first.equipType << ", " << data->name << ", " << itr.second.count << ", " << itr.second.num << ", " << (UInt32)data->color << "\n";
			std::string line = oss.str();
			ofs.write(line.c_str(), line.size());
		}

		ofs.close();
	}
};

// 统计地下城道具掉落
class CountDungenDrop2 : public GMCmdHandler
{
	const char* GetCmdName() const { return "startRace2"; }

	class DropCounter : public Avalon::Thread
	{
	public:
		DropCounter(WSPlayer* player, UInt32 dungeonId, UInt32 times) : m_Race(NULL), m_DungeonId(dungeonId), m_Times(times), m_Finish(false)
		{
			Avalon::SockAddr addr;
			UInt32 hellDungeonEntryId = dungeonId;

			DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
			if (NULL == dungeonData)
			{
				return;
			}

			bool isHell = dungeonData->IsHellEntry();
			if (isHell)
			{
				dungeonId = DungeonRaceMgr::Instance()->GenHellDungeonID(dungeonId);
			}

			DungeonDataEntry* dataEntry = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
			if (NULL == dataEntry)
			{
				return;
			}

			DungeonRace* race = new DungeonRace();
			race->SetID(0);
			race->SetDungeonId(dungeonId);
			race->SetDungeonData(dataEntry);
			race->SetHellDungeonEntryID(hellDungeonEntryId);

			DungeonRacePlayerInfo playerInfo;
			playerInfo.raceInfo.roleId = player->GetID();
			playerInfo.accId = player->GetAccId();
			playerInfo.zoneId = player->GetZoneId();
			playerInfo.raceInfo.occu = player->GetOccu();
			playerInfo.raceInfo.level = player->GetLevel();
			playerInfo.totalCount = 0;
			playerInfo.isAssist = 0;

			race->AddPlayer(playerInfo);
			race->SetPassedAreaIndex(0);

			if (isHell)
			{
				race->SetHell();
			}

			m_Race = race;
		}

		virtual void Run()
		{
			if (!m_Race)
			{
				return;
			}

			UInt64 startTime = Avalon::Time::CurrentTime().MSec();

			m_Race->SetDropCountState(true);
			m_Race->GenDropStatistic(m_Times);


			m_HellMode = m_Race->GetHellMode();

			auto& drops = m_Race->GetDropSta();
			for (auto& drop : drops)
			{
				auto& sta = m_DropStatics[drop.first];
				sta.num += drop.second.num;
				sta.count += drop.second.times;
			}

			UInt64 endTime = Avalon::Time::CurrentTime().MSec();
			UInt64 usedTime = endTime - startTime;
			InfoLogStream << "thread params(dungenId: " << m_DungeonId << ", times: " << m_Times << "), used time: " << usedTime << "ms." << LogStream::eos;
			m_Finish = true;
		}

		bool IsFinish() const { return m_Finish; }

	public:
		DungeonRace*					m_Race;
		UInt32							m_DungeonId;
		UInt32							m_Times;
		std::map<DropKey, DropStatistic> m_DropStatics;
		bool							m_Finish;
		DungeonHellMode					m_HellMode;
	};

	class DropCounterMgrThread : public Avalon::Thread
	{
	public:
		DropCounterMgrThread(WSPlayer* player, UInt32 dungeonId, UInt32 times, UInt32 threadNum)
		{
			m_DungeonId = dungeonId;
			m_Times = times;
			for (UInt32 i = 0; i < threadNum; ++i)
			{
				auto thread = new DropCounter(player, dungeonId, times / threadNum);
				m_Threads.push_back(thread);
			}
			m_Accid = player == NULL ? 0 : player->GetAccId();
		}

		~DropCounterMgrThread()
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
			InfoLogStream << m_Accid << ", params(dungenId: " << m_DungeonId << ", times: " << m_Times << "), used time: " << usedTime << "ms." << LogStream::eos;

			DungeonHellMode hellMode = HELL_MODE_NULL;
			std::map<DropKey, DropStatistic> dropSta;
			for (auto thread : m_Threads)
			{
				hellMode = thread->m_HellMode;

				for (auto drop : thread->m_DropStatics)
				{
					auto& sta = dropSta[drop.first];
					sta.itemId = drop.second.itemId;
					sta.num += drop.second.num;
					sta.count += drop.second.count;
				}

				thread->Stop();
				delete thread;
			}
			CountDungenDrop::RecordDropCountToFile(m_Accid, m_DungeonId, 0, m_Times, dropSta, hellMode);
			m_Threads.clear();
		}

	public:
		UInt32							m_Accid;
		UInt32							m_DungeonId;
		UInt32							m_Times;
		std::vector<DropCounter*>		m_Threads;
	};

	class DropCounterMgrThread2 : public Avalon::Thread
	{
	public:
		DropCounterMgrThread2(WSPlayer* player, UInt32 dungeonId, UInt32 times)
		{
			m_DungeonId = dungeonId;
			m_Times = times;
			m_Accid = player->GetAccId();
			m_Player = player;
		}

		~DropCounterMgrThread2()
		{
		}

		virtual void Run()
		{
			class DungeonVistor : public Avalon::DataEntryVisitor<DungeonDataEntry>
			{
			public:
				bool operator()(DungeonDataEntry* dataEntry)
				{
					if (dataEntry->type == LT_TOWER)
					{
						return true;
					}
					if (dataEntry->subType == DST_PK)
					{
						return true;
					}
					if (dataEntry->IsHellDungeon())
					{
						return true;
					}
					if (dataEntry->hardType != LHT_WARRIOR)
					{
						return true;
					}
					datas.push_back(dataEntry);
					return true;
				}
				std::vector<DungeonDataEntry*> datas;
			};

			DungeonVistor vistor;
			DungeonDataEntryMgr::Instance()->Visit(vistor);

			for (auto data : vistor.datas)
			{
				UInt64 startTime = Avalon::Time::CurrentTime().MSec();

				m_DungeonId = data->id;
				auto thread = new DropCounter(m_Player, m_DungeonId, m_Times);
				thread->Activate();

				while (!thread->IsFinish())
				{
					Sleep(1);
				}


				UInt64 endTime = Avalon::Time::CurrentTime().MSec();
				UInt64 usedTime = endTime - startTime;
				InfoLogStream << m_Accid << ", params(dungenId: " << m_DungeonId << ", times: " << m_Times << "), used time: " << usedTime << "ms." << LogStream::eos;

				CountDungenDrop::RecordDropCountToFile(m_Accid, m_DungeonId, 0, (UInt32)endTime, thread->m_DropStatics);

				thread->Stop();
				delete thread;
			}
		}

	public:
		UInt32							m_Accid;
		UInt32							m_Times;
		UInt32							m_DungeonId;
		WSPlayer*						m_Player;
	};

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		//UInt32 accid = player->GetAccId();
		UInt32 dungeonId = params.GetParam("id");
		UInt32 times = params.GetParam("times");
		UInt32 threadNum = params.GetParam("t");
		UInt8 isAll = params.GetParam("all");

		if (isAll)
		{
			auto thread = new DropCounterMgrThread2(player, dungeonId, times);
			thread->Activate();
		}
		else
		{
			auto thread = new DropCounterMgrThread(player, dungeonId, times, threadNum == 0 ? 1 : threadNum);
			thread->Activate();
		}
	}
};

class RandomOpenDigCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "startDig"; }

	class RandomOpenDigThread : public Avalon::Thread
	{
	public:
		RandomOpenDigThread(WSPlayer* player, UInt32 count, DigType digType)
			:m_Player(player), m_Count(count), m_DigType(digType){}

		virtual void Run()
		{
			for (UInt32 i = 0; i < m_Count; ++i)
			{
				UInt32 itemId;
				UInt32 itemNum;
				UInt32 nRes = DigMapMgr::Instance()->GMRandomOpenDig(m_Player, m_DigType, itemId, itemNum);
				if (nRes != ErrorCode::SUCCESS)
				{
					ErrorLogStream << "RandomOpenDigThread DigType(" << m_DigType << ") Count(" << m_Count << ") nRes(" << nRes << ")" << LogStream::eos;
					continue;
				}
				std::map<UInt32, UInt32>::iterator iter = m_ItemMap.find(itemId);
				if (iter != m_ItemMap.end())
				{
					iter->second++;
				}
				else
				{
					m_ItemMap.insert(std::make_pair(itemId, 1));
				}
			}

			std::string rootpath = WSApplication::Instance()->GetDigCountPath();
			std::string timeStr = Avalon::StringUtil::ConvertToString(CURRENT_TIME.MSec());
			std::string typeStr = Avalon::StringUtil::ConvertToString((UInt32)m_DigType);
			std::string countStr = Avalon::StringUtil::ConvertToString(m_Count);
			std::string postfix = typeStr + "_" + countStr + "_" + timeStr + ".csv";
			std::string fileName = rootpath + "DigCount_" + postfix;

			std::ofstream ofs;
			ofs.open(fileName.c_str());
			if (!ofs.good())
			{
				ErrorLogStream << "FILE(" << fileName << ") ptr is null!" << LogStream::eos;
				return;
			}

			std::string header = "itemId,itemNum\n";
			ofs.write(header.c_str(), header.length());


			std::map<UInt32, UInt32>::iterator iter = m_ItemMap.begin();
			while (iter != m_ItemMap.end())
			{
				//InfoLogStream << "RandomOpenDigThread DigType(" << m_DigType << ") Count(" << m_Count << ") ItemId(" << iter->first << ") ItemNum(" << iter->second << ")" << LogStream::eos;

				std::ostringstream oss;
				oss << iter->first << "," << iter->second << "\n";
				std::string line = oss.str();
				ofs.write(line.c_str(), line.size());

				++iter;
			}
			ofs.close();
		}

	public:
		WSPlayer* m_Player;
		UInt32 m_Count;
		DigType m_DigType;
		std::map<UInt32, UInt32> m_ItemMap;
	};

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 count = params.GetParam("count");
		DigType digType = (DigType)(UInt32)params.GetParam("type");
		if (digType == DigType::DIG_INVALID)
		{
			digType = DigType::DIG_GLOD;
		}

		RandomOpenDigThread* thread = new RandomOpenDigThread(player, count, digType);
		thread->Activate();
	}

};
//触发一个条件
class ActivateCondCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "activateCond"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt8 cond = params.GetParam("cond");
		UInt8 condType = params.GetParam("type");

		player->GetCondActivateMgr().ActivateCond(cond, condType);
	}
};
//客服系统
class CustomServiceSignCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "cs"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		std::string info = params.GetParam("info").GetString();
		CLProtocol::CenterCustomServiceSignReq req;
		req.roleId = player->GetID();
		req.info = info;
		Router::SendToCenter(req);
	}
};

// 赏金联赛报名
class PremiumLeagueEnrollCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "pl_enroll"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		PremiumLeagueMgr::Instance()->OnPlayerEnroll(player);
	}
};

// 赏金联赛报名
class PremiumLeagueReEnrollCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "pl_reenroll"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		PremiumLeagueMgr::Instance()->OnPlayerReEnroll(player);
	}
};

// 修改赏金联赛状态
class PremiumLeagueChangeStatusCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "pl_status"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt8 destStatus = params.GetParam("s");
		auto curStatus = PremiumLeagueMgr::Instance()->GetStatus();
		if (curStatus == destStatus)
		{
			return;
		}

		if (destStatus < curStatus)
		{
			for (int status = curStatus + 1; status <= destStatus + PLS_NUM; status++)
			{
				PremiumLeagueMgr::Instance()->ChangeStatus((PremiumLeagueStatus)(status % PLS_NUM));
			}
		}
		else
		{
			for (int status = curStatus + 1; status <= destStatus; status++)
			{
				PremiumLeagueMgr::Instance()->ChangeStatus((PremiumLeagueStatus)status);
			}
		}
	}
};

class PremiumLeagueAddRecordCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "pl_record"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		auto& recordMgr = PremiumLeagueMgr::Instance()->GetBattleRecordMgr();

		for (UInt32 i = 0; i < num; i++)
		{
			auto entry = new PremiumLeagueRecordEntry();
			auto winner = RandFighter();
			auto loser = RandFighter();
			entry->SetWinner(winner);
			entry->SetLoser(loser);
			recordMgr.AddEntry(entry);

			CLProtocol::WorldPremiumLeagueBattleRecordSync sync;
			sync.record = *entry;
			PremiumLeagueMgr::Instance()->Broadcast(sync);
		}
	}

	PremiumLeagueRecordFighter RandFighter()
	{
		PremiumLeagueRecordFighter fighter;
		fighter.id = 0;
		fighter.name = "test_" + Avalon::StringUtil::ConvertToString(Avalon::Random::Rand());
		fighter.winStreak = Avalon::Random::RandBetween(1, 10);
		fighter.gotScore = Avalon::Random::RandBetween(5, 100);
		fighter.totalScore = Avalon::Random::RandBetween(5, 1000) + fighter.gotScore;
		return fighter;
	}

};

class PremiumLeagueGetRecordCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "pl_get_record"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 start = params.GetParam("start");
		UInt32 num = params.GetParam("num");

		auto entrys = PremiumLeagueMgr::Instance()->GetBattleRecordMgr().GetRecord(start, num);
		for (auto tmpEntry : entrys)
		{
			PremiumLeagueRecordEntry* entry = (PremiumLeagueRecordEntry*)tmpEntry;
			if (!entry)
			{
				continue;
			}

			InfoLogStream << "index(" << entry->GetIndex() << ") winner(" << entry->GetWinner().name << ") loser(" << entry->GetLoser().name << ")." << LogStream::eos;
		}
	}
};

class ReLoadDataTableCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ws_reload"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		InfoLogStream << PLAYERINFO(player) << " gm reload..." << LogStream::eos;
		WSApplication::Instance()->ReloadDataTables();
	}
};

class ActivityMonsterQueryCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "am_query"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		auto info = WSActivityMonsterMgr::Instance()->GetActivityMonsterInfo(id);
		InfoLogStream << "id(" << id << ") activity(" << info.activity << ") remain(" << info.remainNum << ") next roll(" << info.nextRollStartTime << ")." << LogStream::eos;
	}
};

class SubmitAccountActivityTaskCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "submit_acc_task"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");
		
		AccountActivityTaskMgr::Instance()->SubmitTask(player, id);
	}
};

class OpenConditionCheckCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "close_acc_task"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt8 id = params.GetParam("close");

		AccountActivityTaskMgr::Instance()->SetCondCheck((id > 0 ? true : false));
	}
};

class OpenCheckDrawCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "close_draw"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt8 id = params.GetParam("close");

		AccountActivityTaskMgr::Instance()->SetDrawCountCheck((id > 0 ? true : false));
	}
};


class RoomMatchCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "room"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		CLProtocol::MatchServerPkRoomReq req;
		req.roomId = 0;
		req.pkType = params.GetParam("type");
		
		PkRoomMatchPlayerInfo info;
		info.id = player->GetID();
		info.accid = player->GetAccId();
		info.name = player->GetName();
		info.score = 0;
		info.level = player->GetLevel();
		req.players.push_back(info);

		Router::SendToMatch(req);
	}
};

class OppoAmberLevelCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "oppoLevel"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;

		UInt8 level = params.GetParam("num");
		
		if (level > 11)
		{
			level = 11;
		}

		player->SetOppoAmberLevel(level);
		player->SyncEventToScene(SET_OPPO_VIP_LEVEL, level);
		AccountActivityTaskMgr::Instance()->UpdateAmberPrivilegeTask(player);
		AccountActivityTaskMgr::Instance()->UpdateOppoVipLevelTask(player);

		InfoLogStream << PLAYERINFO(player) << "setAmberLevel(" << level << ")." << LogStream::eos;
	}
};
class DismissRoomCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "disroom"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (player == NULL) return;

		UInt32 roomId = params.GetParam("id");

		UInt32 nRes = RoomMgr::Instance()->DismissRoom(NULL, roomId);
		if (nRes != ErrorCode::SUCCESS)
		{
			ErrorLogStream << "DismissRoom nRes = " << nRes << LogStream::eos;
		}
	}
};

class SetOpActTimeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "setOpAct"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (player == NULL) return;

		WSActivityMgr::Instance()->CustomSetOpActData(params.GetParam("id"), params.GetParam("stTm"), params.GetParam("edTm"));

		return;
	}
};

class AddOpActCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "addOpAct"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		OpActivityRegInfo activeInfo;

		activeInfo.dataId = 28;
		activeInfo.name = "新服冲级赛";
		activeInfo.tag = 1;
		activeInfo.tagEndTime = 1515686400;
		activeInfo.startTime = 1527782400;
		activeInfo.endTime = 1528387199;
		activeInfo.circleType = 1;
		activeInfo.ruleDesc = "在这个时间段内，只要角色达到规定等级，就可以领取对应奖励哦！";
		activeInfo.tmpType = 15;
		activeInfo.parm = 0;
		activeInfo.taskDesc = "等级达成";

		WSActivityMgr::Instance()->UpdateOpActData(activeInfo);

		return;
	}
};

class SendGoldMailCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "sendGoldMail"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (player == NULL) return;

		std::vector<ItemReward> rewards;
		ItemReward reward;
		reward.id = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLD_ITEM_DATA_ID);
		reward.num = params.GetParam("num");
		rewards.push_back(reward);
		MailBox::SendSysMail(player->GetID(), "gold", "gold", "gold", "gold", rewards);

		return;
	}
};


class MallQuerySingleItemCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "mallQuerySingle"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		MallMgr::Instance()->OnQueryItem(pPlayer, params.GetParam("id"));
	}
};

class QueryChargeMallGoodsCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "queryChargeGoods"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		ChargeManager::Instance()->OnPlayerQueryChargeGoods(pPlayer);
	}
};

class ClearMasterSysPunishmentTimeCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "cmspt"; }
	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		pPlayer->GetMasterSystem().SetApprenticMasterPunishTime(0);
		pPlayer->GetMasterSystem().SetRecruitDisciplePunishTime(0);
	}
};

class SetMasterGoodTeachCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "smgt"; }
	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 num = params.GetParam("num");
		pPlayer->SyncEventToScene(SET_ADD_GOODTEACH_VALUE, num, 0);
	}
};

class AddIntimacyNewCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "AddIntimacyNew"; }
	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string name = params.GetParam("name").GetString();
		UInt32 Intimacy = params.GetParam("value");
		WSPlayer* friendPlayer = WSPlayerMgr::Instance()->FindNormalPlayerByName(name);
		if (!friendPlayer) return;
		if (!WSRelationMgr::IsMutualFriend(pPlayer, friendPlayer)
			&& !WSRelationMgr::IsMasterDisciple(pPlayer, friendPlayer)) return;
		pPlayer->AddIntimacy(friendPlayer->GetID(), Intimacy);
		friendPlayer->AddIntimacy(pPlayer->GetID(), Intimacy);
	}
};

class DigRefreshCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "digr"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 lastRefreshTime = params.GetParam("time");
		
		if (lastRefreshTime == 0)
		{
			Avalon::Date refreshDate = CURRENT_DATE;
			refreshDate.Min(0);
			refreshDate.Sec(0);
			refreshDate.MSec(0);
			lastRefreshTime = (UInt32)refreshDate.ToTime().Sec();
		}

		DigMapMgr::Instance()->Refresh(lastRefreshTime);
	}

};

class DigSupplyCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "digs"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		Avalon::Date refreshDate = CURRENT_DATE;
		refreshDate.Min(0);
		refreshDate.Sec(0);
		refreshDate.MSec(0);
		UInt32 lastRefreshTime = (UInt32)refreshDate.ToTime().Sec();
		DigMapMgr::Instance()->Supply(lastRefreshTime);
	}
};

class DigMapOpenCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "digmo"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 id = params.GetParam("id");

		std::vector<DigInfo> info;
		//UInt32 size = 0;
		DigMapMgr::Instance()->GMOpenDig(pPlayer, id);

	}
};

class PlayerForeverDeletedCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "deleteplayer"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		ObjID_t id = params.GetParam("id");
		UInt32 tp = params.GetParam("tp");

		WSPlayerMgr::Instance()->OnPlayerForeverDeleted(id, tp);
	}
};

class ClearTowerCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "cleartower"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 accId = params.GetParam("id");

		std::string paramStr = "{\"accid\":\"" + Avalon::StringUtil::ConvertToString(accId) + "\"}";
		
		WSConsoleAgent::Instance()->HandleRequest(0, "towerManager", "clearTower", paramStr);
	}
};

class GuildDungeonStatusCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gdungeon"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 status = params.GetParam("id");

		GuildDungeonMgr::Instance()->GMUpdateStatus(status);
	}
};

class GuildDungeonClearBossCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gboss"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildDungeonMgr::Instance()->ClearSmallBoss(pPlayer->GetGuildID(), pPlayer->GetID(), pPlayer->GetOccu(), pPlayer->GetName());
	}
};

class GuildDungeonClearBigBossCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gbigboss"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt64 damage = params.GetParam("num");

		GuildDungeonMgr::Instance()->ClearBigBoss(pPlayer->GetGuildID(), pPlayer->GetID(), pPlayer->GetOccu(), damage, pPlayer->GetName());
	}
};

class GuildDungeonAddBuffCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gbuff"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt32 cnt = params.GetParam("cnt");
		UInt32 buffId = params.GetParam("id");

		GuildDungeonMgr::Instance()->GMAddBuffCnt(pPlayer->GetGuildID(), buffId, cnt);
	}
};

class GuildDungeonSetBigBossDamageCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gdamage"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt64 val = params.GetParam("val");
		pPlayer->SetGuildDungeonDebugDamage(val);
	}
};

class GuildAuctionStopCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "stopga"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		GuildAuctionMgr::Instance()->StopAuction(pPlayer->GetGuildID());
	}
};

class GuildClearSecurityLockCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "slock"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		pPlayer->ClearSecurityLockNotifyClient();
	}
};

class AdventureTeamRenameCmdHandler : public GMCmdHandler
{
	class AdventureTeamRenameCallback : public CLRecordCallback
	{
	public:
		AdventureTeamRenameCallback(ObjID_t roleid, const CLProtocol::WorldAdventureTeamRenameReq& req)
			: m_RoleId(roleid), m_RenameReq(req) {}

	public:
		virtual void OnFinished()
		{
			if (NextRow())
			{
				SendRespose(ErrorCode::ADVENTURE_TEAM_NAME_EXIST);
				return;
			}

			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_RoleId);
			if (!player)
			{
				ErrorLogStream << "Can not find player(" << m_RoleId << ") while try to rename(" << m_RenameReq.newName << ")!" << LogStream::eos;
				return;
			}

			auto itemData = ItemDataEntryMgr::Instance()->FindEntry(m_RenameReq.costItemDataId);
			if (!itemData)
			{
				ErrorLogStream << PLAYERINFO(player) << " try to rename(" << m_RenameReq.newName << "), but item(" << m_RenameReq.costItemUId << "," << m_RenameReq.costItemDataId << ") not exist!" << LogStream::eos;
				SendRespose(player, ErrorCode::ITEM_DATA_INVALID);
				return;
			}
			else if (itemData->thirdType != IT_ITEM_RENAME_ADVENTURE_TEAM)
			{
				ErrorLogStream << PLAYERINFO(player) << " try to rename(" << m_RenameReq.newName << "), but item(" << m_RenameReq.costItemUId << "," << m_RenameReq.costItemDataId << ")'s third type is error!" << LogStream::eos;
				SendRespose(player, ErrorCode::ITEM_TYPE_INVALID);
				return;
			}

			auto trans = new AdventureTeamRenameTransaction(player, m_RenameReq);
			trans->AddDetailItem(m_RenameReq.costItemUId, m_RenameReq.costItemDataId, 1);
			if (!player->BeginItemTransaction(trans))
			{
				ErrorLogStream << PLAYERINFO(player) << " failed begin adventure team rename transaction!" << LogStream::eos;
				SendRespose(player, ErrorCode::ITEM_USE_ADVENTURE_TEAM_RENAME_FAILED);
			}
		}

		virtual void OnFailed()
		{
			ErrorLogStream << "Failed to select table t_account!" << LogStream::eos;
			SendRespose(ErrorCode::RECORD_ERROR);
		}

		void SendRespose(WSPlayer* player, UInt32 errorCode)
		{
			if (!player) return;

			CLProtocol::WorldAdventureTeamRenameRes protocol;
			protocol.resCode = errorCode;
			player->SendProtocol(protocol);
		}

		void SendRespose(UInt32 errorCode)
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_RoleId);
			if (!player)
			{
				ErrorLogStream << "Can not find player(" << m_RoleId << ") while try to rename(" << m_RenameReq.newName << ")!" << LogStream::eos;
				return;
			}
			SendRespose(player, errorCode);
		}

	private:
		ObjID_t m_RoleId;
		CLProtocol::WorldAdventureTeamRenameReq m_RenameReq;
	};

	const char* GetCmdName() const { return "renameAdvT"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		std::string newName = params.GetParam("n").GetString();
		ObjID_t costItemUId = params.GetParam("uid");
		UInt32 costItemDataId = params.GetParam("id");

		CLProtocol::WorldAdventureTeamRenameReq req;
		req.newName = newName;
		req.costItemUId = costItemUId;
		req.costItemDataId = costItemDataId;

		CLProtocol::WorldAdventureTeamRenameRes res;
		res.resCode = ErrorCode::SUCCESS;

		//UInt32 length = req.newName.length();

		if (req.newName.empty() || req.newName.length() > NAME_LENGTH)
		{
			res.resCode = ErrorCode::ADVENTURE_TEAM_NAME_INVALID;
			pPlayer->SendProtocol(res);
			return;
		}

		char namebuf[NAME_LENGTH + 1];
		avalon_strncpy(namebuf, req.newName.c_str(), NAME_LENGTH);
		if (ForbitWordParser::Instance()->Parse(namebuf) != FORBIT_WORD_UNCHANGED)
		{
			res.resCode = ErrorCode::ADVENTURE_TEAM_NAME_INVALID;
			pPlayer->SendProtocol(res);
			return;
		}

		if (pPlayer->GetAdventureTeamName().compare(namebuf) == 0)
		{
			res.resCode = ErrorCode::ADVENTURE_TEAM_NAME_SAME;
			pPlayer->SendProtocol(res);
			return;
		}

		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
		cmd->PutCond("adventure_team_name", req.newName);
		cmd->PutColumn("adventure_team_name");
		CLRecordClient::Instance()->SendCommand(cmd, new AdventureTeamRenameCallback(pPlayer->GetID(), req));
	}
};

class BlessCrystalInfoCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "blessCrystalQuery"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldBlessCrystalInfoRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.ownBlessCrystalNum = pPlayer->GetBlessCrystalNum();
		res.ownBlessCrystalExp = pPlayer->GetBlessCrystalExp();
		pPlayer->SendProtocol(res);
	}
};

class InheritBlessInfoCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "inheritBlessQuery"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldInheritBlessInfoRes res;
		res.resCode = ErrorCode::SUCCESS;
		res.ownInheritBlessNum = pPlayer->GetInheritBlessNum();
		res.ownInheritBlessExp = pPlayer->GetInheritBlessExp();
		pPlayer->SendProtocol(res);
	}
};

class InheritExpCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "inheritExp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldInheritExpRes res;
		res.resCode = pPlayer->UseInheritBless();
		res.ownInheritBlessExp = pPlayer->GetInheritBlessExp();
		res.ownInheritBlessNum = pPlayer->GetInheritBlessNum();
		pPlayer->SendProtocol(res);
	}
};

class ExtensibleRoleFieldUnlockCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "unlockRole"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldExtensibleRoleFieldUnlockReq req;
		req.costItemUId = params.GetParam("uid");
		req.costItemDataId = params.GetParam("id");
		pPlayer->UnlockExtensibleRoleField(req.costItemUId, req.costItemDataId);
	}
};

class AccountShopItemQueryCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "queryAccShopItem"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldAccountShopItemQueryReq req;
		req.queryIndex.shopId = params.GetParam("id");
		req.queryIndex.tabType = params.GetParam("tab");
		req.queryIndex.jobType = params.GetParam("job");
		WSShopMgr::Instance()->OnPlayerQueryShopItem(pPlayer, req);
	}
};

class AccountShopItemBuyCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "buyAccShopItem"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldAccountShopItemBuyReq req;
		req.queryIndex.shopId = params.GetParam("id");
		req.queryIndex.tabType = params.GetParam("tab");
		req.queryIndex.jobType = params.GetParam("job");
		req.buyShopItemId = params.GetParam("itemid");
		req.buyShopItemNum = params.GetParam("num");
		WSShopMgr::Instance()->OnPlayerBuyShopItem(pPlayer, req);
	}
};

class AdventureTeamExpIncreaseCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "incAdtExp"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		UInt64 incExp = params.GetParam("num");
		InfoLogStream << PLAYERINFO(pPlayer) << " gm increase adventure team exp(" << incExp << ")." << LogStream::eos;

		std::string reason = "";
		pPlayer->GetAccountInfo()->TryIncAdventureTeamExp(pPlayer, reason, incExp);
		pPlayer->GetAccountInfo()->TryIncBlessExp(pPlayer, reason, incExp, ACC_COUNTER_BLESS_CRYSTAL_EXP);
		pPlayer->GetAccountInfo()->TryIncBlessExp(pPlayer, reason, incExp, ACC_COUNTER_INHERIT_BLESS_EXP);
	}
};

class IncGnomeCoinCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "incGnomeCoin"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		pPlayer->IncGnomeCoinNum("", params.GetParam("num"));
	}
};

class InsertAbnormalTransCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "insertAbnorTrans"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		AbnormalTransDBInfo info;
		info.buyer = params.GetParam("bid");
		info.seller = params.GetParam("sid");
		info.transTime = (UInt32)CURRENT_TIME.Sec();
		info.transId = CLRecordClient::Instance()->GenGuid();
		info.itemUId = CLRecordClient::Instance()->GenGuid();
		info.itemDataId = 60000001;
		info.itemNum = 1;
		info.moneyType = MONEY_GOLD;
		info.transAmount = 100;
		info.freezeObj = (AbnormalTransFreezeObject)(UInt8)(params.GetParam("fo"));
		info.backWay = (AbnormalTransFrozenMoneyBackWay)(UInt8)(params.GetParam("bw"));
		info.isFrozenPermanet = (UInt8)(params.GetParam("p")) > 0;
		info.isTreasure = (UInt8)(params.GetParam("t")) > 0;
		info.mailNotifyType = params.GetParam("mnt");
		info.unfrozenAmount = params.GetParam("ufa");

		InfoLogStream << PLAYERINFO(pPlayer) << " gm insert abnormal transaction info(" << info.transId << ")." << LogStream::eos;

		if (!AbnormalTransHelper::SaveAbnormalTransactionInfo(info))
		{
			ErrorLogStream << PLAYERINFO(pPlayer) << " save abnormal transaction info(" << info.transId << ") failed!" << LogStream::eos;
		}
	}
};

class WorldGmOpActivtyReqCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gmOpActReq"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		CLProtocol::WorldGmOpActivtyReq protocol;
		protocol.mergeServers = MERGED_SERVERS;
		Router::SendToCenter(protocol);
	}
};

class BlackMarketNpcCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "bmnpc"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt8 type = params.GetParam("type");
		class Visitor : public WSPlayerVisitor
		{
		public:
			Visitor(UInt8 auction_type)
			{
				protocol.type = auction_type;
			}
			virtual bool operator()(WSPlayer* player)
			{
				if (player)
				{
					player->SendProtocol(protocol);
				}
				return true;
			}

			CLProtocol::WorldBlackMarketSyncType protocol;
		};
		Visitor visitor(type);
		WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
	}
};

class WorldQueryExpeditionMapCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "queryExpMap"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto account = pPlayer->GetAccountInfo();
		if (!account) return;

		account->GetExpeditionMgr().OnPlayerQueryExpeditionMap(pPlayer, params.GetParam("mapId"));
	}
};

class WorldQueryOptionalExpeditionRolesCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "queryOptExpRoles"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto account = pPlayer->GetAccountInfo();
		if (!account) return;

		account->GetExpeditionMgr().OnPlayerQueryOptionalExpeditionRoles(pPlayer);
	}
};

class WorldDispatchExpeditionTeamCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "dispatchExpTeam"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto account = pPlayer->GetAccountInfo();
		if (!account) return;

		std::vector<ObjID_t> roleids;
		roleids.push_back((ObjID_t)params.GetParam("members"));
		account->GetExpeditionMgr().OnPlayerDispatchExpeditionTeam(pPlayer, params.GetParam("mapId"), roleids, params.GetParam("housOfduration"));
	}
};

class WorldCancelExpeditionCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "cancelExpedition"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto account = pPlayer->GetAccountInfo();
		if (!account) return;

		account->GetExpeditionMgr().OnPlayerCancelExpedition(pPlayer, params.GetParam("mapId"));
	}
};

class WorldGetExpeditionRewardsCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "getExpedRewards"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto account = pPlayer->GetAccountInfo();
		if (!account) return;

		account->GetExpeditionMgr().OnPlayerGetExpeditionRewards(pPlayer, params.GetParam("mapId"));
	}
};

class WorldFinishExpeditionCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "finishExpedition"; }

	void Execute(WSPlayer* pPlayer, const CLParameter& params)
	{
		auto account = pPlayer->GetAccountInfo();
		if (!account) return;

		auto expeditionMap = account->GetExpeditionMgr().FindExpeditionMap(params.GetParam("mapId"));
		if (!expeditionMap) return;

		expeditionMap->FinishExpedition();
	}
};


class SetRoleValueScoreCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "setRoleValueScore"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;

		UInt32 score = params.GetParam("score");
		player->OnRoleValueScoreSync(score);
		DebugLogStream << PLAYERINFO(player) << " set role value score(" << score << ")." << LogStream::eos;
	}
};

class SetAccountCounterCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "setAccCount"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;

		UInt32 type = params.GetParam("type");
		UInt64 num = params.GetParam("num");

		auto account = player->GetAccountInfo();
		if (!account) return;

		account->SetAccountCounterNum(player, (AccountCounterType)type, num);
		DebugLogStream << PLAYERINFO(player) << " set account counter(" << type << "," << num << ")." << LogStream::eos;
	}
};

class RefreshAccountWeeklyTaskCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "rfaccwt"; }




	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;
		auto account = player->GetAccountInfo();
		if (!account) return;
		account->GetWeeklyTaskMgr().RefreshTasks();
		DebugLogStream << PLAYERINFO(player) << " refresh account weekly tasks." << LogStream::eos;
	}
};

class PlayerJoinsCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "playerJoins"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;
		UInt32 type = params.GetParam("type");
		UInt32 param = params.GetParam("param");
		PlayerJoinMgr::Instance()->AddOnePlayer(type, param, player->GetGuildID(), player->GetID());
	}
};

class PlayerAddNewTitleCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "playerAddTitle"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;
		UInt32 titleId = params.GetParam("titleId");
		UInt32 dueTime = params.GetParam("dueTime");
		
		auto entry = NewTitleItemDataEntryMgr::Instance()->FindEntry(titleId);
		if (!entry)
		{
			return;
		}
		if (entry->ownerType == NTOTP_ROLE)
		{
			player->GetTitleMgr().AddOneTitleByTitleId(titleId, dueTime);
		}
		else if (entry->ownerType == NTOTP_ACCT)
		{
			player->GetAccountInfo()->GetTitleMgr().AddOneTitleByTitleId(titleId, dueTime);
		}
	}
};

class FinshAccountWeeklyTaskCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "finAccWeekTask"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;
		UInt32 taskid = params.GetParam("id");

		player->GetAccountInfo()->GetWeeklyTaskMgr().FinishTask(taskid, true);
	}
};

class TestDropMarkFlagCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "testDropMarkFlag"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;
		UInt32 dropId = params.GetParam("dropId");
		auto dropResult = DropItemMgr::Instance()->GenDropItems(0, player->GetID(), dropId, player->GetVipLvl(), player->GetOccu());

		for (auto dropItem : dropResult.dropItems)
		{
			dropItem.CheckMark(DIMT_TEAM_COPY_GOLD);
		}
	}
};

class StatistTeamCopyDropItemCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "stTpDropItem"; }

	struct DropItemStatistic
	{
		DropItemStatistic()
		{
			memset(this, 0, sizeof(*this));
		}

		UInt32 itemId;
		// 生成数量
		UInt32 num;
		// 生成次数
		UInt32 times;
	};

	class Task : public Avalon::Thread
	{
	public:
		Task(UInt32 dropId_, UInt32 times_, UInt64 roleId_, UInt8 vipLv_, UInt8 playerOcc_)
			: dropId(dropId_), times(times_), roleId(roleId_), vipLv(vipLv_), playerOcc(playerOcc_)
		{
		}

		~Task() {}

		virtual void Run()
		{
			std::map<UInt32, DropItemStatistic> dropItems;
			for (UInt32 i = 0; i < times; ++i)
			{
				auto dropResult = DropItemMgr::Instance()->GenDropItems(0, roleId, dropId, vipLv, playerOcc);
				for (auto dropItem : dropResult.dropItems)
				{
					auto iter = dropItems.find(dropItem.typeId);
					if (iter == dropItems.end())
					{
						DropItemStatistic dropItemStatistic;
						dropItemStatistic.itemId = dropItem.typeId;
						dropItemStatistic.num = dropItem.num;
						dropItemStatistic.times = 1;
						dropItems[dropItem.typeId] = dropItemStatistic;
					}
					else
					{
						DropItemStatistic& dropItemStatistic = iter->second;
						dropItemStatistic.num += dropItem.num;
						dropItemStatistic.times += 1;
					}
				}
			}

			if (dropItems.size() > 0)
			{
				std::string rootpath = WSApplication::Instance()->GetDropItemCountPath();
				std::string dropIdStr = Avalon::StringUtil::ConvertToString(dropId);
				std::string timesStr = Avalon::StringUtil::ConvertToString(times);

				std::string postfix = dropIdStr + "_" + timesStr + ".csv";
				std::string filename = rootpath + "DropItemCount_" + postfix;

				std::ofstream ofs;
				ofs.open(filename.c_str());
				if (!ofs.good())
				{
					ErrorLogStream << "count drop item err, FILE ptr is null!" << LogStream::eos;
					return;
				}

				std::string header = "itemid,name,times,num\n";
				ofs.write(header.c_str(), header.length());
				for (auto itr : dropItems)
				{
					auto data = ItemDataEntryMgr::Instance()->FindEntry(itr.first);
					if (!data)
					{
						continue;
					}

					DropItemStatistic& dropItemStatistic = itr.second;

					std::ostringstream oss;
					oss << dropItemStatistic.itemId << ", " << data->name << ", " << dropItemStatistic.times << ", " << itr.second.num <<"\n";
					std::string line = oss.str();
					ofs.write(line.c_str(), line.size());
				}

				ofs.close();

				return;
			}
		}
	private:
		UInt32 dropId;
		UInt32 times;
		UInt64 roleId;
		UInt8  vipLv;
		UInt8  playerOcc;
	};

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (!player) return;
		UInt32 dropId = params.GetParam("id");
		UInt32 times = params.GetParam("times");
		
		auto task = new Task(dropId, times, player->GetID(), player->GetVipLvl(), player->GetOccu());
		task->Activate();
	}
};

class SetReturnAccCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SetReturnAccCallback, Avalon::Mutex)
public:

	SetReturnAccCallback(UInt32 accid) : m_Accid(accid) {}
	void OnFinished()
	{
		while (NextRow())
		{
			// 处理账号数据
			{
				std::ostringstream accCond;
				accCond << "accid=" << m_Accid;
				CLUpdateCommand* accCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", accCond.str());

				UInt32 returnTime = GetData("veteran_return_time");
				if (returnTime != 0)
					accCmd->PutData("veteran_return_time", 0);
				else
					accCmd->PutData("veteran_return_time", 1);

				CLRecordClient::Instance()->SendCommand(accCmd);
			}

			// 处理角色数据
			{
				std::ostringstream roleCond;
				roleCond << "accid=" << m_Accid;
				CLUpdateCommand* roleCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleCond.str());
				roleCmd->PutData("savetime", 1);
				roleCmd->PutData("return_time", 0);
				roleCmd->PutData("return_level", 0);
				roleCmd->PutData("return_first_login", 0);
				CLRecordClient::Instance()->SendCommand(roleCmd);
			}
		}
	}

	void OnFailed(UInt32 errorcode) {}

private:
	UInt32 m_Accid;
};

class SetReturnAccCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "setReturnAcc"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (NULL == player)
			return;

		UInt32 accId = params.GetParam("id");
		if (player->GetAccId() == accId)
		{
			ErrorLogStream << PLAYERINFO(player) << " set cur accid!" << LogStream::eos;
			return;
		}

		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
		command->PutCond("accid", accId);
		command->PutColumn("accid");
		CLRecordClient::Instance()->SendCommand(command, new SetReturnAccCallback(accId));
	}
};

class ChangeServiceSwitchCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "changeSwitch"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		if (NULL == player)
			return;

		UInt32 op = params.GetParam("open");
		UInt32 sw = params.GetParam("type");
		if (op)
		{
			WSServiceSwitchMgr::Instance()->OpenService((ServiceType)sw);
		}
		else
		{
			WSServiceSwitchMgr::Instance()->CloseService((ServiceType)sw);
		}
	}
};

class GameParamCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "gamep"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		std::string param = params.GetParam("p").GetString();
		UInt32 val = params.GetParam("v");

		GameParamMgr::Instance()->SetValue(param.c_str(), val);
		GameParamMgr::Instance()->GameParamSyncScene();
	}
};

//深渊怪物模拟
class HellMonsterCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "HellMonster"; }

	struct HellMonsterStatistic
	{
		HellMonsterStatistic()
		{
			memset(this, 0, sizeof(*this));
		}
		//组id
		UInt32 groupId;
		// 生成次数
		UInt32 times;
	};

	class Task : public Avalon::Thread
	{
	public:
		Task(UInt32 dungeonId, UInt32 times)
		{
			m_dungeonId = dungeonId;
			m_times = times;
		}

		~Task() {}

		virtual void Run()
		{
			InfoLogStream << "HellMonsterCmdHandler task!" << LogStream::eos;
		
			DungeonMonsterGenerator::Instance()->GenHellMonstersForGm(m_dungeonId, m_times);
			return;
		}

	private:
		UInt32	m_dungeonId; //地下城id
		UInt32	m_times;	//次数
		DungeonRacePlayerInfo			m_DungeonRacePlayerInfo;
	};

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		InfoLogStream << "enter HellMonsterCmdHandler::Execute" << LogStream::eos;
		UInt32 dungeonId = params.GetParam("dungeonId");
		UInt32 times = params.GetParam("times");

		auto thread = new Task(dungeonId, times);
		thread->Activate();
		return;
	}
};

class AuctionNumCacheTimeCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "AuctNumCacheTime"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 time = params.GetParam("t");

		WSApplication::Instance()->SetAuctionNumCacheTime(time);
	}
};

class DailyTodoSetCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "DailyTodoSet"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 subType = params.GetParam("t");

		player->GetDailyTodoMgr().CompleteByGm(subType);
	}
};

//深渊地下城ID随机模拟测试
class HellDungeonIdRandSimCmdHandler : public GMCmdHandler
{
	const char* GetCmdName() const { return "HellDungeonIdRand"; }
	const char* GetHelp() const { return "times=xx entryId=xx"; }

	void Execute(WSPlayer* player, const CLParameter& params)
	{
		UInt32 times = params.GetParam("times");
		UInt32 entryId = params.GetParam("entryId");

		std::map<UInt32, UInt32> resultMap;
		UInt32 count = times;
		while (count--)
		{
			UInt32 dungeonId = DungeonRaceMgr::Instance()->GenHellDungeonID(entryId);
			resultMap[dungeonId] += 1;
		}

		std::vector<std::string> vstr;
		vstr.push_back("entry,dungeon,num\n");
		for (auto itr : resultMap)
		{
			vstr.emplace_back(std::to_string(entryId) + ',' + std::to_string(itr.first) + ',' + std::to_string(itr.second) + '\n');
		}

		std::string rootpath = WSApplication::Instance()->GetDropCountPath();
		std::string accidStr = Avalon::StringUtil::ConvertToString(player->GetAccId());
		std::string timesStr = Avalon::StringUtil::ConvertToString(times);
		std::string bStr = Avalon::StringUtil::ConvertToString(entryId);

		std::string postfix = accidStr + "_" + timesStr + "_" + bStr + ".csv";
		std::string filename = rootpath + "hellDungeonIdRand_" + postfix;

		std::ofstream ofs;
		ofs.open(filename.c_str());
		if (!ofs.good())
		{
			ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
			return;
		}

		for (auto& elem : vstr)
		{
			ofs.write(elem.c_str(), elem.size());
		}

		ofs.close();
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
		CL_SAFE_DELETE(iter->second);
	}
	m_Handlers.clear();
}

void GMSystem::ExecuteCommand(WSPlayer* player, const std::string& cmd, const CLParameter& params)
{
	HandlerMap::iterator iter = m_Handlers.find(cmd);
	if(iter != m_Handlers.end())
	{

#ifndef _DEBUG
		if (player->GetAccountInfo() && player->GetAccountInfo()->HasGmAuthority(GM_AUTHORITY_DEBUG))
#endif
		{
			iter->second->Execute(player, params);
		}
	}
	else
	{
		CLProtocol::GASGMCommand gmCmd;
		gmCmd.cmdName = cmd;
		gmCmd.param = params;
		Router::SendToGlobalActivity(gmCmd);
	}
}

void GMSystem::RegisterHandlers()
{
	RegisterHandler(new AdventurePassCmdHandler());
	RegisterHandler(new AddIntimacyCmdHandler());
	RegisterHandler(new ForbidTalkHandler());
	RegisterHandler(new ForbidLoginHandler());
	RegisterHandler(new MonopolyHandler());
	RegisterHandler(new MonopolyBuffHandler());
	RegisterHandler(new AnnounceHandler());
	RegisterHandler(new AnnounceTestHandler());
	RegisterHandler(new BroadcastMailGMHandler());
	RegisterHandler(new ReloadScriptsCmdHandler());
	RegisterHandler(new ReloadScriptCmdHandler());
	RegisterHandler(new UpdateDailyActivityCmdHandler());
	RegisterHandler(new SortListGiveRewardCmdHandler());
	RegisterHandler(new CreateGuildCmdHandler());
	RegisterHandler(new GuildListCmdHandler());
	RegisterHandler(new JoinGuildCmdHandler());
	RegisterHandler(new GuildProsperityCmdHandler());
	RegisterHandler(new GuildOnWeekCmdHandler());
	RegisterHandler(new ReplyGuildCmdHandler());
	RegisterHandler(new ChangeGuildNameCmdHandler());
	RegisterHandler(new ChangeGuildPostCmdHandler());
	RegisterHandler(new UpgradeGuildBuildingCmdHandler());
	RegisterHandler(new GuildDonateCmdHandler());
	RegisterHandler(new GuildUpgradeSkillCmdHandler());
	RegisterHandler(new GuildDismissCmdHandler());
	RegisterHandler(new GuildSendMailCmdHandler());
	RegisterHandler(new AddGuildFundCmdHandler());
	RegisterHandler(new GuildOrzCmdHandler());
	RegisterHandler(new JoinGuildTableCmdHandler());
	RegisterHandler(new GuidPayRedPacketCmdHandler());
	RegisterHandler(new AddRedPacketCmdHandler());
	RegisterHandler(new OpendRedPacketCmdHandler());
	RegisterHandler(new SendRedPacketCmdHandler());
	RegisterHandler(new CustomRedPacketCmdHandler());
	RegisterHandler(new NewYearRedPacketEndCmdHandler());
	RegisterHandler(new GetSystemValueCmdHandler());
	RegisterHandler(new UnpunishCmdHandler());

	RegisterHandler(new GuildBattleChangeStatusHandler());		//公会战改变状态
	RegisterHandler(new GuildBattleByTimeHandler());			//公会战根据时间变换状态
	RegisterHandler(new GuildBattleEnrollCmdHandler());			//公会战报名
	RegisterHandler(new GuildBattleHistoryTerrCmdHandler());	//公会历史领地设置
	RegisterHandler(new GuildMatchBattleCmdHandler());			//公会战匹配
	RegisterHandler(new GuildBattleInspireCmdHandler());		//公会战鼓舞
	RegisterHandler(new GuildBattleAddRecordCmdHandler());		//增加公会战记录
	RegisterHandler(new GuildBattleSetNumberCmdHandler());		//设置公会战次数
	RegisterHandler(new GuildBattleLotteryCmdHandler());		//公会战抽奖
	RegisterHandler(new GuildBattleAddScoreCmdHandler());		//公会战抽奖
	RegisterHandler(new GuildBattleCrossTimeCmdHandler());		//公会战时间

	RegisterHandler(new SetSeasonIdCmdHandler());							//设置赛季

	RegisterHandler(new DeleteSortListCmdHandler());						//删除跟玩家关联的排行榜

	RegisterHandler(new ChangeActiveCmdHandler());				//更改活动状态

	RegisterHandler(new ChargeCmdHandler());
	RegisterHandler(new ResetChargeCmdHandler());

	RegisterHandler(new TeamMatchCmdHandler());
	RegisterHandler(new RareItemCmdHandler());
	RegisterHandler(new ChargePacketCmdHandler());
	RegisterHandler(new BuyChargePacketCmdHandler());
	RegisterHandler(new ReChargeCmdHandler());
	RegisterHandler(new LoadAccountCmdHandler());
	RegisterHandler(new CDKeyVerifyCmdHandler());

	RegisterHandler(new ActivateCondCmdHandler());
	RegisterHandler(new ReLoadDataTableCmdHandler());
	RegisterHandler(new CountDungenDrop2());
	RegisterHandler(new ActivateGiftPack());

	RegisterHandler(new CustomServiceSignCmdHandler());



	RegisterHandler(new PremiumLeagueEnrollCmdHandler());
	RegisterHandler(new PremiumLeagueChangeStatusCmdHandler());
	RegisterHandler(new PremiumLeagueReEnrollCmdHandler());
	RegisterHandler(new PremiumLeagueAddRecordCmdHandler());
	RegisterHandler(new PremiumLeagueGetRecordCmdHandler());
	RegisterHandler(new ActivityMonsterQueryCmdHandler());
	RegisterHandler(new SubmitAccountActivityTaskCmdHandler());
	RegisterHandler(new OpenConditionCheckCmdHandler());
	RegisterHandler(new OpenCheckDrawCmdHandler());
	RegisterHandler(new RoomMatchCmdHandler());
	RegisterHandler(new OppoAmberLevelCmdHandler());
	RegisterHandler(new DismissRoomCmdHandler());
	RegisterHandler(new SetOpActTimeCmdHandler());
	RegisterHandler(new AddOpActCmdHandler());
	RegisterHandler(new SendGoldMailCmdHandler());
	RegisterHandler(new MallQuerySingleItemCmdHandler());
	RegisterHandler(new QueryChargeMallGoodsCmdHandler());

	RegisterHandler(new ClearMasterSysPunishmentTimeCmdHandler());
	RegisterHandler(new SetMasterGoodTeachCmdHandler);
	RegisterHandler(new AddIntimacyNewCmdHandler);

	RegisterHandler(new DigRefreshCmdHandler);
	RegisterHandler(new DigSupplyCmdHandler);
	RegisterHandler(new DigMapOpenCmdHandler);

	RegisterHandler(new PlayerForeverDeletedCmdHandler);

	RegisterHandler(new ClearTowerCmdHandler);
	RegisterHandler(new RandomOpenDigCmdHandler);
	RegisterHandler(new GuildDungeonStatusCmdHandler);
	RegisterHandler(new GuildDungeonClearBossCmdHandler);
	RegisterHandler(new GuildDungeonClearBigBossCmdHandler);
	RegisterHandler(new GuildClearSecurityLockCmdHandler);
	RegisterHandler(new IncGnomeCoinCmdHandler);
	RegisterHandler(new InsertAbnormalTransCmdHandler);
	RegisterHandler(new WorldGmOpActivtyReqCmdHandler);
	RegisterHandler(new GuildDungeonAddBuffCmdHandler);
	RegisterHandler(new GuildDungeonSetBigBossDamageCmdHandler);
	RegisterHandler(new GuildAuctionStopCmdHandler);
	RegisterHandler(new WorldQueryExpeditionMapCmdHandler);
	RegisterHandler(new WorldQueryOptionalExpeditionRolesCmdHandler);
	RegisterHandler(new WorldDispatchExpeditionTeamCmdHandler);
	RegisterHandler(new WorldCancelExpeditionCmdHandler);
	RegisterHandler(new WorldGetExpeditionRewardsCmdHandler);
	RegisterHandler(new WorldFinishExpeditionCmdHandler);
	RegisterHandler(new SetRoleValueScoreCmdHandler);
	RegisterHandler(new SetAccountCounterCmdHandler);
	RegisterHandler(new RefreshAccountWeeklyTaskCmdHandler);
	RegisterHandler(new AdventureTeamExpIncreaseCmdHandler);
	
	//黑市商人
	RegisterHandler(new BlackMarketNpcCmdHandler());

	RegisterHandler(new PlayerJoinsCmdHandler());

	RegisterHandler(new PlayerAddNewTitleCmdHandler());

	//冒险团周常任务
	RegisterHandler(new FinshAccountWeeklyTaskCmdHandler());

	RegisterHandler(new TestDropMarkFlagCmdHandler());	
	
	RegisterHandler(new StatistTeamCopyDropItemCmdHandler());
	RegisterHandler(new SetReturnAccCmdHandler());

	RegisterHandler(new ChangeServiceSwitchCmdHandler());
	RegisterHandler(new GameParamCmdHandler());

	RegisterHandler(new HellMonsterCmdHandler());

	RegisterHandler(new AuctionNumCacheTimeCmdHandler());

	RegisterHandler(new DailyTodoSetCmdHandler());

	RegisterHandler(new HellDungeonIdRandSimCmdHandler());
}

void GMSystem::RegisterHandler(GMCmdHandler* handler)
{
	if(handler == NULL) return;
	HandlerMap::iterator iter = m_Handlers.find(handler->GetCmdName());
	if(iter != m_Handlers.end())
	{
		CL_SAFE_DELETE(iter->second);
		iter->second = handler;
	}
	else
	{
		m_Handlers.insert(std::make_pair(handler->GetCmdName(),handler));
	}
}
