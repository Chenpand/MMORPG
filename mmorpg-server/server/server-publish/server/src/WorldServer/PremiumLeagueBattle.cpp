#include "PremiumLeagueBattle.h"
#include <CLRecordClient.h>
#include <CLPremiumLeagueProtocol.h>
#include "WSRouter.h"

PremiumLeagueBattle::PremiumLeagueBattle()
{

}

PremiumLeagueBattle::~PremiumLeagueBattle()
{

}

void PremiumLeagueBattle::InsertToDB()
{
	auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_premium_league_battle", guid);
	cmd->PutData("race_id", raceId);
	cmd->PutData("type", type);
	cmd->PutData("role_id1", fighter1.id);
	cmd->PutData("role_name1", fighter1.name);
	cmd->PutData("role_occu1", fighter1.occu);
	cmd->PutData("role_id2", fighter2.id);
	cmd->PutData("role_name2", fighter2.name);
	cmd->PutData("role_occu2", fighter2.occu);
	cmd->PutData("is_end", isEnd);
	cmd->PutData("winner", winnerId);

	CLRecordClient::Instance()->SendCommand(cmd);
}

void PremiumLeagueBattle::UpdateToDB()
{
	auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_premium_league_battle", guid, false);
	cmd->PutData("is_end", isEnd);
	cmd->PutData("race_id", raceId);
	cmd->PutData("winner", winnerId);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void PremiumLeagueBattle::SyncToPlayer()
{
	CLProtocol::WorldPremiumLeagueBattleInfoUpdate sync;
	sync.battle = *this;
	Router::BroadcastToPlayer(sync);
}