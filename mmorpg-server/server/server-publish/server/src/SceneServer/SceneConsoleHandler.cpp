#include "SceneConsoleHandler.h"

#include "SSConsoleAgent.h"
#include "ScriptMgr.h"
#include "PlayerMgr.h"
#include "CurrencyFrozenMgr.h"

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

void HandleProtocol(CLProtocol::SceneConsoleCurrencyFrozen& protocol)
{
	InfoLogStream << "SceneConsoleCurrencyFrozen nodeid:" << protocol.nodeId << " roleId:" << protocol.roleId 
		<< " orderId:" << protocol.orderId << " opType:" << protocol.opType << LogStream::eos;

	UInt32 ret = ErrorCode::SUCCESS;
	switch (protocol.opType)
	{
	case 1: // ²éÑ¯
	{
		json::Object body;
		body["error"] = json::Boolean(false);

		body["msg"] = json::Object();
		json::Object& msg = body["msg"];

		msg["orders"] = json::Array();
		json::Array& orders = msg["orders"];

		std::vector<CurrencyFrozen> orderVec;
		CurrencyFrozenMgr::Instance()->GetRoleFrozenOrder(protocol.roleId, orderVec);
		for (auto& it : orderVec)
		{
			json::Object order;
			order["orderid"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(it.guid));
			order["roleid"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(it.roleId));
			order["itemid"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(it.itemId));
			order["num"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(it.num));
			order["time"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(it.frozenTime));
			order["reason"] = json::String(it.reason);
			orders.Insert(order);
		}

		SSConsoleAgent::Instance()->SendResponse(protocol.nodeId, "CurrencyFrozen", "handler", body);
		return;
	}
	case 2:	// ½â¶³
	{
		ret = CurrencyFrozenMgr::Instance()->RemoveFrozen(protocol.orderId);
		break;
	}
	case 3:	// ¿Û³ý
	{
		ret = CurrencyFrozenMgr::Instance()->DeductFrozen(protocol.orderId);
		break;
	}
	default:
		ret = ErrorCode::ITEM_DATA_INVALID;
		break;
	}

	json::Object body;
	if (ret == ErrorCode::SUCCESS)
		body["error"] = json::Boolean(false);
	else
		body["error"] = json::Boolean(true);
	
	SSConsoleAgent::Instance()->SendResponse(protocol.nodeId, "CurrencyFrozen", "handler", body);
}

void HandleProtocol(CLProtocol::SceneConsoleRemoveCreditPoint& protocol)
{
	json::Object body;

	do 
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.onlineId);
		if (NULL == player)
		{
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("not find player!");
			break;
		}

		body["error"] = json::Boolean(false);
		body["msg"] = json::String("success");

		InfoLogStream << PLAYERINFO(player) << "gm remove credit point num:" << protocol.creditPoint << LogStream::eos;

		player->RemoveCreditPoint(protocol.reason.c_str(), protocol.creditPoint);

	} while (0);

	SSConsoleAgent::Instance()->SendResponse(protocol.nodeId, "CreditPoint", "remove", body);
}