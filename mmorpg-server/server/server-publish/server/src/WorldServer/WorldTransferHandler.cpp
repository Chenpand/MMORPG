#include "WorldTransferHandler.h"
#include "WSNetwork.h"
#include "GuildMgr.h"
#include "AccountMgr.h"
#include "WSSortListMgr.h"
#include "WSRouter.h"
#include "ChatDataMgr.h"

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTwo& protocol)
{
	// 获取公会信息
	auto guildMember = GuildMgr::Instance()->FindGuildMember(protocol.watchInfo.id);
	if (guildMember)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(guildMember->GetGuildID());
		if (guild && guild->FindMember(protocol.watchInfo.id))
		{
			protocol.watchInfo.guildTitle.guildName = guild->GetName();
			protocol.watchInfo.guildTitle.guildPost = guildMember->GetPost();
		}

		protocol.watchInfo.emblemLevel = guildMember->GetEmblemLevel();
	}

	auto targetPlayer = WSPlayerMgr::Instance()->FindPlayer(protocol.watchInfo.id);
	if (targetPlayer)
	{
		protocol.watchInfo.activeTimeType = targetPlayer->GetMasterSystem().GetQuestionnaire().activeTimeType;
		protocol.watchInfo.masterType = targetPlayer->GetMasterSystem().GetQuestionnaire().masterType;
		protocol.watchInfo.regionId = targetPlayer->GetMasterSystem().GetQuestionnaire().regionId;
		protocol.watchInfo.declaration = targetPlayer->GetMasterSystem().GetQuestionnaire().declaration;
	}

	UInt32 targetAccId = AccountMgr::Instance()->GetAccIDByRoleID(protocol.watchInfo.id);
	auto targetAccount = AccountMgr::Instance()->FindAccount(targetAccId);
	if (targetAccount)
	{
		protocol.watchInfo.adventureTeamName = targetAccount->GetAdventureTeamName();
		protocol.watchInfo.adventureTeamGrade = targetAccount->GetAdventureTeamGradeStr();
		protocol.watchInfo.adventureTeamRanking = WSSortListMgr::Instance()->GetRanking(SORTLIST_ADVENTURE_TEAM_GRADE, targetAccount->GetAccID());
	}

	tHeader.SwapHeader();
	tHeader.destServer = ST_SCENE;
	tHeader.srcServer = SERVER_TYPE;
	Router::SendToTransfer(tHeader, protocol);
}

void HandleProtocol(WSPlayer* player, TransferHeader& tHeader, CLProtocol::TransferQueryPlayerDetailTwo& protocol)
{
	if (NULL == player) return;

	CLProtocol::WorldQueryPlayerDetailsRet ret;
	ret.info = protocol.info;
	player->SendProtocol(ret);
}

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTipsOne& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();

	if ( !ChatDataMgr::Instance()->OtherZoneQueryTips(protocol.type, protocol.id, packet))
	{
		ErrorLogStream << "other zone tips error " << tHeader.LogStr() << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}

	CLProtocol::TransferQueryPlayerTipsTwo two(packet);
	tHeader.SwapHeader();
	Router::SendToTransfer(tHeader, two);
}

void HandleProtocol(WSPlayer* player, TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTipsTwo& protocol)
{
	if (NULL == player) return;

	player->SendPacket(protocol.packet);
}

