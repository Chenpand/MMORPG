#include "SceneTeamCopyHandler.h"

#include "PlayerMgr.h"
#include "SSRouter.h"
#include "TeamCopyMgr.h"
#include "CurrencyFrozenMgr.h"
#include "ActivityMgr.h"
#include "TCSMgr.h"

void HandleProtocol(CLProtocol::TeamCopyQueryPlayerInfoReq& protocol)
{
	CLProtocol::TeamCopyQueryPlayerInfoRes res;
	res.teamSquadId = protocol.teamSquadId;
	res.roleId = protocol.roleId;

	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (NULL == player)
	{
		res.result = MATCH_QUERY_PLAYER_OFFLINE;
	}
	else
	{
		res.result = MATCH_QUERY_SUCCESS;
		res.dungeonPlayerInfo = player->GetDungeonMgr().GetDungeonPlayerInfo(protocol.dungeonId);
	}

	Router::SendToTeamCopy(res);
}

void HandleProtocol(CLProtocol::TeamCopyPlayerCheckReq& protocol)
{
	CLProtocol::TeamCopyPlayerCheckRes res;
	res.teamId = protocol.teamId;
	res.roleId = protocol.roleId;

	do 
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
		if (NULL == player)
		{
			res.retCode = TEAM_COPY_CHECK_CODE_UNKNOWN;
			break;
		}


		// 检查是否参与团本扶持活动，有门票消耗减少
		std::vector<UInt32> discountVec;
		std::vector<OpActivityRegInfo*> actVec = ActivityMgr::Instance()->GetOpActDataByTmpTypes(OAT_TEAM_COPY_SUPPORT);
		if (actVec.size() == 1)
		{
			discountVec = (*actVec.begin())->parm3;
		}

		// 检查道具，门票
		bool flag = false;
		UInt32 m = 0;
		for (auto& iter : protocol.consumeItemVec)
		{
			UInt32 dis = 0;
			if (discountVec.size() > m)
			{
				dis = discountVec[m];
			}
			++m;

			if (player->GetItemNum(iter.id) < (iter.num > dis ? iter.num - dis:0))
			{
				flag = true;
				break;
			}
		}

		if (flag)
		{
			res.retCode = TEAM_COPY_CHECK_CODE_TICKET;
			break;
		}

		// 检查佣金
		if (protocol.forzenCommission.num > 0)
		{
			if (player->GetItemNum(protocol.forzenCommission.id) < protocol.forzenCommission.num)
			{
				res.retCode = TEAM_COPY_CHECK_CODE_COMMISSION;
				break;
			}
		}

		// 扣除道具，门票
		std::ostringstream logString;
		std::string reason = GetReason(SOURCE_TYPE_TEAM_COPY_DEDUCT, protocol.teamGrade, protocol.teamMode);
		UInt32 n = 0;
		for (auto& iter : protocol.consumeItemVec)
		{
			logString << iter.id << "," << iter.num << ".";

			UInt32 dis = 0;
			if (discountVec.size() > n)
			{
				dis = discountVec[n];
			}
			++n;

			if (iter.num > dis)
			{
				UInt32 consum = iter.num - dis;
				player->RemoveItem(reason.c_str(), iter.id, consum);
				res.consumeItemVec.push_back(TCConsumeItem(iter.id, consum));
			}

			InfoLogStream << "consume item id:" << iter.id << " num:" << iter.num << " active discount num:" << dis << LogStream::eos;
		}

		// 扣除佣金
		if (protocol.forzenCommission.num > 0)
		{
			player->RemoveItem(reason.c_str(), protocol.forzenCommission.id, protocol.forzenCommission.num);

			// 一定成功
			res.commissionOrder = CurrencyFrozenMgr::Instance()->ApplyFrozen(protocol.roleId, protocol.forzenCommission.id, protocol.forzenCommission.num, reason.c_str());
			res.forzenCommission = protocol.forzenCommission;
			if (res.commissionOrder == 0)
			{
				res.retCode = TEAM_COPY_CHECK_CODE_COMMISSION;
				break;
			}
		}

		InfoLogStream << "team copy teamSquadId:" << protocol.teamId << 
			"teamGrade" << protocol.teamGrade <<
			"teamMode" << protocol.teamMode <<
			" check player:" << protocol.roleId <<
			" deduct item id:" << logString.str() << 
			" frozen commission id:" << protocol.forzenCommission.id <<
			" num:" << protocol.forzenCommission.num <<
			" commissionOrder:" << res.commissionOrder << LogStream::eos;

		res.retCode = TEAM_COPY_CHECK_CODE_SUCCESS;

	} while (0);

	Router::SendToTeamCopy(res);
}

void HandleProtocol(CLProtocol::TeamCopyPlayerInfoSync& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (NULL == player)
	{
		ErrorLogStream << "not find player id:" << protocol.roleId << LogStream::eos;
		return;
	}

	player->GetTeamCopyMgr().HandlerTeamCopyPlayerInfo(protocol);
}

void HandleProtocol(CLProtocol::TeamCopyCfgValRes& protocol)
{
	for (auto& it : protocol.valCfgMap)
	{
		InfoLogStream << "team copy config type:" << it.first << " val:" << it.second << LogStream::eos;
		TCSMgr::Instance()->SetCfgval(it.first, it.second);
	}
}

void HandleProtocol(CLProtocol::TeamCopyGetItem& protocol)
{
	std::ostringstream logStr;
	for (auto& it : protocol.rewards)
	{
		logStr << " itemId:" << it.id << " itemNum:" << it.num;
	}

	InfoLogStream << "team copy item playerId:" << protocol.roleId << " reason:" << protocol.reason
		<< " reward:" << logStr.str() << LogStream::eos;

	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (NULL == player)
	{
		ErrorLogStream << "team copy not find playerId:" << protocol.roleId << LogStream::eos;
		return;
	}

	RewardGroup* group = player->GetRewardGroup();
	group->AddRewards(protocol.rewards);
	player->AddRewards(protocol.reason.c_str(), group, protocol.notice);
}
