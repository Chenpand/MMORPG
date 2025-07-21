#include "SceneConsoleHandler.h"

#include "SSConsoleAgent.h"
#include "ScriptMgr.h"
#include "PlayerMgr.h"

void HandleProtocol(CLProtocol::SceneConsoleQueryPlayer& protocol)
{
	std::string subcmd = protocol.subcmd;
	if(subcmd.compare("items") == 0)
	{
		SSConsoleAgent::Instance()->QueryPlayerItems(protocol.nodeid, protocol.playerId, protocol.accId, protocol.param, protocol.param2);
	}
	else if(subcmd.compare("player") == 0)
	{
		SSConsoleAgent::Instance()->QueryPlayerInfo(protocol.nodeid, protocol.keyType, protocol.accId, protocol.playerId, protocol.name);
	}
}

void HandleProtocol(CLProtocol::SceneConsoleReloadScripts& protocol)
{
	for (auto scriptType : protocol.scripts)
	{
		if (!ScriptMgr::Instance()->ReloadScript(scriptType))
		{
			ErrorLogStream << "Reload Script(" << scriptType << ") failed." << LogStream::eos;
		}
	}
}

void HandleProtocol(CLProtocol::SceneConsoleControlPlayer& protocol)
{
	std::string subcmd = protocol.subcmd;
	if (subcmd.compare("forbidPlayer") == 0)
	{
		SSConsoleAgent::Instance()->ForbidTalkPlayer(protocol.nodeid, protocol.keyType, protocol.accId, protocol.playerId, protocol.name, (UInt32)protocol.param1);
	}
	else if (subcmd.compare("deleteItem") == 0)
	{
		SSConsoleAgent::Instance()->DeleteItem(protocol.nodeid, protocol.playerId, protocol.param1, (UInt16)protocol.param2);
	}
	else if (subcmd.compare("removeMoney") == 0)
	{
		SSConsoleAgent::Instance()->DeleteMoney(protocol.nodeid, protocol.playerId, protocol.items);
	}
	else if (subcmd.compare("setLevel") == 0)
	{
		SSConsoleAgent::Instance()->SetPlayerLv(protocol.nodeid, protocol.playerId, (UInt16)protocol.param2);
		
	}
	else if (subcmd.compare("setVipLevel") == 0)
	{
		SSConsoleAgent::Instance()->SetPlayerVipLv(protocol.nodeid, protocol.playerId, (UInt8)protocol.param2);
	}
	else if (subcmd.compare("finishTask") == 0)
	{
		SSConsoleAgent::Instance()->FinishTask(protocol.nodeid, protocol.playerId, protocol.param2);
	}
	else if (subcmd.compare("clearTower") == 0)
	{
		SSConsoleAgent::Instance()->ClearTower(protocol.nodeid, protocol.accId, protocol.playerId);
	}
}

void HandleProtocol(CLProtocol::SceneConsolePhoneBind& protocol)
{
	SSConsoleAgent::Instance()->PhoneBind(protocol.nodeid, protocol.roleid);
}

void HandleProtocol(CLProtocol::AdminConsoleRequest& req)
{
	SSConsoleAgent::Instance()->HandleRequest(req.nodeid, req.cmd, req.subcmd, req.body);
}

void HandleProtocol(CLProtocol::WorldSyncPlayerGMAuthority& protocol)
{
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.roleid);
	if (!player) return;
	player->SetGmAuthority(protocol.gmAuth);
}