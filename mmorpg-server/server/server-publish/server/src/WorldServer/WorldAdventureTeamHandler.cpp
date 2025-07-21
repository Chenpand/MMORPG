#include "WorldAdventureTeamHandler.h"

#include <CLRecordClient.h>
#include <CLForbitWord.h>
#include <CLPlayerDataEntry.h>

#include "WSPlayerMgr.h"
#include "WSItemTransactionUtil.h"
#include "WSSortListMgr.h"
#include "AccountMgr.h"
#include "WorldSysNotify.h"


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

		// ÓÃµãÈ¯
		if (m_RenameReq.costItemUId == 0)
		{
			UInt32 costPoint = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_TEAM_RENAME_COST);
			auto trans = new AdventureTeamRenameTransaction(player, m_RenameReq);
			trans->SetGold(costPoint);
			if (!player->BeginItemTransaction(trans))
			{
				ErrorLogStream << PLAYERINFO(player) << " failed begin adventure team rename transaction!" << LogStream::eos;
				SendRespose(player, ErrorCode::ITEM_USE_ADVENTURE_TEAM_RENAME_FAILED);
			}
		}
		else
		{
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

class QueryOwnOccupationsCallback : public CLRecordCallback
{
public:
	QueryOwnOccupationsCallback(ObjID_t roleid)
		: m_RoleId(roleid) {}

public:
	virtual void OnFinished()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		UInt16 maxLevel = PlayerDataEntryMgr::Instance()->GetMaxLevel();

		std::set<UInt8> occus;
		while (NextRow())
		{
			UInt16 level = GetData("level");
			UInt8 occu = GetData("occu");
			if (level == maxLevel && occu % 10 != 0)
			{
				occus.insert(occu);
			}
		}

		CLProtocol::WorldQueryOwnOccupationsRes res;
		res.resCode = ErrorCode::SUCCESS;
		for (auto elem : occus)
		{
			res.occus.push_back(elem);
		}
		player->SendProtocol(res);
	}

	virtual void OnFailed()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player) return;

		CLProtocol::WorldQueryOwnOccupationsRes res;
		res.resCode = ErrorCode::RECORD_ERROR;
		player->SendProtocol(res);

		ErrorLogStream << PLAYERINFO(player) << "Failed to select table t_player_info!" << LogStream::eos;
	}

private:
	ObjID_t m_RoleId;
};


void HandleProtocol(WSPlayer* player, CLProtocol::WorldAdventureTeamExtraInfoReq& req)
{
	if (!player) return;

	CLProtocol::WorldAdventureTeamExtraInfoRes res;
	res.resCode = ErrorCode::SUCCESS;

	res.extraInfo.adventureTeamName = player->GetAdventureTeamName();
	res.extraInfo.adventureTeamLevel = player->GetAdventureTeamLevel();
	res.extraInfo.adventureTeamExp = player->GetAdventureTeamExp();
	res.extraInfo.adventureTeamGrade = player->GetAdventureTeamGrade();
	res.extraInfo.adventureTeamRanking = WSSortListMgr::Instance()->GetRanking(SORTLIST_ADVENTURE_TEAM_GRADE, player->GetAccId());;
	res.extraInfo.roleTotalScore = player->GetAdventureRoleScore();
	res.extraInfo.ownRoleFileds = player->GetRoleFileds();

	player->SyncAdventureTeamMoney();
	player->SendProtocol(res);
	player->SyncUnlockedNewOccus();
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAdventureTeamRenameReq& req)
{
	if (!player) return;

	CLProtocol::WorldAdventureTeamRenameRes res;
	res.resCode = ErrorCode::SUCCESS;

	if (req.newName.empty() || req.newName.length() > NAME_LENGTH)
	{
		res.resCode = ErrorCode::ADVENTURE_TEAM_NAME_INVALID;
		player->SendProtocol(res);
		return;
	}

	char namebuf[NAME_LENGTH + 1];
	avalon_strncpy(namebuf, req.newName.c_str(), NAME_LENGTH);
	if (ForbitWordParser::Instance()->Parse(namebuf) != FORBIT_WORD_UNCHANGED)
	{
		res.resCode = ErrorCode::ADVENTURE_TEAM_NAME_INVALID;
		player->SendProtocol(res);
		return;
	}

	if (player->GetAdventureTeamName().compare(namebuf) == 0)
	{
		res.resCode = ErrorCode::ADVENTURE_TEAM_NAME_SAME;
		player->SendProtocol(res);
		return;
	}

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
	cmd->PutCond("adventure_team_name", req.newName);
	cmd->PutColumn("adventure_team_name");
	CLRecordClient::Instance()->SendCommand(cmd, new AdventureTeamRenameCallback(player->GetID(), req));

	InfoLogStream << PLAYERINFO(player) << " try to rename adventure team(" << req.newName << ")." << LogStream::eos;
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlessCrystalInfoReq& req)
{
	if (!player) return;

	CLProtocol::WorldBlessCrystalInfoRes res;
	res.resCode = ErrorCode::SUCCESS;
	res.ownBlessCrystalNum = player->GetBlessCrystalNum();
	res.ownBlessCrystalExp = player->GetBlessCrystalExp();
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldInheritBlessInfoReq& req)
{
	if (!player) return;

	CLProtocol::WorldInheritBlessInfoRes res;
	res.resCode = ErrorCode::SUCCESS;
	res.ownInheritBlessNum = player->GetInheritBlessNum();
	res.ownInheritBlessExp = player->GetInheritBlessExp();
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldInheritExpReq& req)
{
	if (!player) return;

	CLProtocol::WorldInheritExpRes res;
	res.resCode = player->UseInheritBless();
	res.ownInheritBlessNum = player->GetInheritBlessNum();
	res.ownInheritBlessExp = player->GetInheritBlessExp();
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryExpeditionMapReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;

	account->GetExpeditionMgr().OnPlayerQueryExpeditionMap(player, req.mapId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryOptionalExpeditionRolesReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;

	account->GetExpeditionMgr().OnPlayerQueryOptionalExpeditionRoles(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDispatchExpeditionTeamReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;
	
	account->GetExpeditionMgr().OnPlayerDispatchExpeditionTeam(player, req.mapId, req.members, req.housOfduration);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldCancelExpeditionReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;

	account->GetExpeditionMgr().OnPlayerCancelExpedition(player, req.mapId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetExpeditionRewardsReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;

	account->GetExpeditionMgr().OnPlayerGetExpeditionRewards(player, req.mapId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryAllExpeditionMapsReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;
	
	account->GetExpeditionMgr().OnPlayerQueryAllExpeditionMaps(player, req.mapIds);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryOwnOccupationsReq& req)
{
	if (!player) return;

	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	cmd->PutCond("accid", player->GetAccId());
	cmd->PutColumn("level");
	cmd->PutColumn("occu");
	std::ostringstream limitoss;
	limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DELETE_PLAYER_SAVE_TIME << ")";
	cmd->SetLimit(limitoss.str());
	CLRecordClient::Instance()->SendCommand(cmd, new QueryOwnOccupationsCallback(player->GetID()));
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRemoveUnlockedNewOccupationsReq& req)
{
	if (!player) return;

	auto account = player->GetAccountInfo();
	if (!account) return;
	
	if (account->RemoveUnlockedNewOccus(req.newOccus))
	{
		player->SyncUnlockedNewOccus();
	}
}

void HandleProtocol(CLProtocol::GateNotifyReCalculateTotalRoleValueScore& protocol)
{
	auto account = AccountMgr::Instance()->FindAccount(protocol.accid);
	if (!account) return;

	UInt32 oldScore = account->GetTotalScoreOfAllRoleValue();
	UInt32 newScore = account->CalcTotalScoreOfAllRoleValue();
	account->SetTotalScoreOfAllRoleValue(newScore);

	GameInfoLogStream("Property") << "player(" << account->GetAccID() << ") change totalRoleScore: "
		<< protocol.reason << "|" << (Int64)newScore - (Int64)oldScore << "|" << newScore << LogStream::eos;
	Sys::SendUdpLog("asset", LOG_ASSET_FORMAT, protocol.reason, ASSET_TOTAL_ROLE_SCORE, (UInt64)oldScore, 0, (Int64)newScore - (Int64)oldScore, (UInt64)newScore);

	account->OnTotalScoreOfAllRoleValueChange(NULL, protocol.reason);
}