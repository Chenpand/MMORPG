#include "WSConsoleAgent.h"

#include <CLConsoleProtocol.h>
#include "WSRouter.h"

#include "WSConsoleCmdHandler.h"


WSConsoleAgent::WSConsoleAgent()
{
}

WSConsoleAgent::~WSConsoleAgent()
{
}

void WSConsoleAgent::Init()
{
	RegisterHandler(new ConsoleOnlineNumHandler());
	RegisterHandler(new ConsoleQueryPlayerHandler());
	RegisterHandler(new ConsoleQueryPlayerItemHandler());
	RegisterHandler(new ConsoleUserControlHandler());
	RegisterHandler(new ForbidTalkAllHandler());
	RegisterHandler(new PublicAnnouncementHandler());
	RegisterHandler(new QueryMailHandler());
	RegisterHandler(new BroadcastMailHandler());
	RegisterHandler(new SendDetailMailHandler());
	RegisterHandler(new SendMailBatchHandler());
	RegisterHandler(new ShowAnnouncementsHandler());
	RegisterHandler(new DelAnnounceHandler());
	RegisterHandler(new ConsoleSceneOnlineNumHandler());
	RegisterHandler(new ReloadScriptsHandler());
	RegisterHandler(new SendAwardHandler());
	RegisterHandler(new ReturnItemHandler());
	RegisterHandler(new RecoverRoleHandler());
	RegisterHandler(new ConsoleRoleNumHandler());
	RegisterHandler(new GiveItemHandler());
	RegisterHandler(new QueryPlayerHandler());
	RegisterHandler(new PunishPlayerHandler());
	RegisterHandler(new ChargeHandler());
	RegisterHandler(new ConsoleDeleteItemHandler());
	RegisterHandler(new ConsoleDeleteMoneyHandler());
	RegisterHandler(new ConsoleSetPlayerLvHandler());
	RegisterHandler(new ConsoleSetPlayerVipLvHandler());
	RegisterHandler(new ConsoleSetPlayerFinishTaskHandler());
	RegisterHandler(new QueryChargeOrderHandler());
	RegisterHandler(new ConsoleBatchQueryPlayerHandler());
	RegisterHandler(new DeleteRoleHandler());
	RegisterHandler(new QueryChargeMoneyHandler());
	RegisterHandler(new QueryChargeRankHandler());
	RegisterHandler(new QueryMallConfigHandler());
	RegisterHandler(new SetMallConfigHandler());
	RegisterHandler(new DelMallConfigHandler());

	RegisterHandler(new QueryActConfigHandler());
	RegisterHandler(new SetActConfigHandler());
	RegisterHandler(new DelActConfigHandler());
	RegisterHandler(new SendBindPointHandler());
	RegisterHandler(new SendPointHandler());
	RegisterHandler(new PhoneBindHandler());
	RegisterHandler(new QueryPlayerChangeNameRecordHandler());
	RegisterHandler(new QueryPlayerChangeNameRecordByNameHandler());
	RegisterHandler(new QueryGuildHandler());
	RegisterHandler(new DismissGuildHandler());
	RegisterHandler(new GuildAddFundHandler());
	RegisterHandler(new ClearTowerDataHandler());
	RegisterHandler(new CDKDataHandler());
	RegisterHandler(new ClearSecurityLockHandler());
	RegisterHandler(new AbnormalTransQueryHandler());
	RegisterHandler(new AbnormalTransUnfreezeHandler());
	RegisterHandler(new AuctionSetAveraPriceHandler());
	RegisterHandler(new AccounterSetCounterSHandler());
	RegisterHandler(new SetPlayerGMAuthoriyHandler());
	RegisterHandler(new IncGnomeCoinHandler());
	RegisterHandler(new CurrencyFrozenHandler());
	RegisterHandler(new AddTitleHandler());
	RegisterHandler(new CreditPointHandler());
}

void WSConsoleAgent::Final()
{
}

void WSConsoleAgent::DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd,  const std::string& json)
{
	CLProtocol::AdminConsoleResponse response;
	response.nodeid = connid; 
	response.cmd = cmd;
	response.subcmd = subcmd;
	response.timestamp = UInt32(CURRENT_TIME.Sec());
	response.body = json;
	Router::SendToAdmin(response);
}
