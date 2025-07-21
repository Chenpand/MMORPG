#include "SSConsoleAgent.h"

#include <CLConsoleProtocol.h>
#include <CLRecordClient.h>
#include <json/writer.h>

#include "SSRouter.h"
#include "PlayerMgr.h"
#include <CLEventMgr.h>
#include "SSConsoleCmdHandler.h"

/**
*@brief 更新玩家等级
*/
class UpdatePlayerLvCb : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(UpdatePlayerLvCb, Avalon::Mutex)
public:
	UpdatePlayerLvCb(UInt32 connid) : m_ConnId(connid){}

	void OnFinished()
	{
		json::Object body;
		body["error"] = json::Boolean(false);
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "setLevel", body);
	}

	void OnFailed(UInt32 errcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Update Player level Failed!");
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "setLevel", body);
	}

private:
	UInt32	m_ConnId;
};


/**
*@brief 查询玩家删除货币
*/
class DeleteMoneyCb : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(DeleteMoneyCb, Avalon::Mutex)
public:
	DeleteMoneyCb(UInt32 connid, std::vector<ItemReward>& moneys) : m_ConnId(connid), m_moneys(moneys){}

	void OnFinished()
	{
		if (NextRow())
		{
			for (size_t i = 0; i < m_moneys.size(); ++i)
			{
				std::string column = GetMoneyDBColumnName(m_moneys[i].id);
				UInt32 oldValue = GetData(column);
				if (m_moneys[i].num > oldValue)
				{
					json::Object body;
					body["error"] = json::Boolean(true);
					body["msg"] = json::String("Player Money Value Less Then Input Value!");
					SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "removeMoney", body);
					return;
				}

				UInt32 newValue = oldValue - m_moneys[i].num;

				CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetKey(), true);
				updateCmd->PutData(column.c_str(), newValue);
				CLRecordClient::Instance()->SendCommand(updateCmd);
			}

			json::Object body;
			body["error"] = json::Boolean(false);
			SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "removeMoney", body);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Select Player Money Failed!");
			SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "removeMoney", body);
		}
	}

	void OnFailed(UInt32 errcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Select Player Money Failed!");
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "removeMoney", body);
	}

	std::string GetMoneyDBColumnName(UInt32 id)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL) return std::string("");
		if (dataEntry->type != ITEM_INCOME) return std::string("");
		
		switch (dataEntry->subType)
		{
		case ST_ITEM_GOLD:		return std::string("gold");
		case ST_ITEM_POINT:		return std::string("point");
		case ST_ITEM_EXP:		return std::string("exp");
		case ST_PK_COIN:		return std::string("pk_coin");
		case ST_REVIVE_COIN:	return std::string("revive_coin");
		case ST_BIND_GOLD:		return std::string("bindgold");
		case ST_BIND_POINT:		return std::string("bindpoint");
		case ST_GUILD_CONTRI:	return std::string("guild_contri");
		case ST_CREDIT_POINT:	return std::string("credit_point");

		default:	return std::string("");
		}
	}

private:
	UInt32	m_ConnId;
	std::vector<ItemReward> m_moneys;
};

/**
*@brief 查询玩家禁言
*/
class SelectForbidTalkTimeCb : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectForbidTalkTimeCb, Avalon::Mutex)
public:
	SelectForbidTalkTimeCb(UInt32 connid, UInt32 forbidTime, bool isAcc)
		:m_ConnId(connid), m_ForbidTime(forbidTime), m_IsAccForbid(isAcc) {}

	void OnFinished()
	{
		bool hasResult = false;
		while (NextRow())
		{
			hasResult = true;
			UInt32 newDueTime = GetNewDueTime(GetData("forbid_talk_duetime"));

			if (m_IsAccForbid)
			{
				Player* player = PlayerMgr::Instance()->FindNormalPlayer(GetKey());
				if (player != NULL)
				{
					if (newDueTime == 0)
					{
						player->ClearForbitTalkDueTime();
					}
					else
					{
						player->SetForbitTalkDueTime(newDueTime);
					}
				}
			}

			CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetKey(), true);
			updateCmd->PutData("forbid_talk_duetime", newDueTime);
			CLRecordClient::Instance()->SendCommand(updateCmd);
		}

		if (!hasResult)
		{
			OnFailed(0);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(false);
			SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "forbidPlayer", body);
		}
	}

	void OnFailed(UInt32 errcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Select Player Failed!");
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "forbidPlayer", body);
	}

	UInt32 GetNewDueTime(UInt32 oldDueTime)
	{
		if (m_ForbidTime == 0)
		{
			return 0;
		}

		UInt32 curTime = UInt32(CURRENT_TIME.Sec());
		if (oldDueTime > curTime)
		{
			return oldDueTime + m_ForbidTime;
		}
	
		return curTime + m_ForbidTime;
	}

private:
	UInt32	m_ConnId;
	UInt32	m_ForbidTime;
	UInt32  m_IsAccForbid;
};

/**
*@brief 删除道具返回
*/
class DeleteItemCb : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(DeleteItemCb, Avalon::Mutex)
public:
	DeleteItemCb(UInt32 connid, UInt16 reduceNum) : m_ConnId(connid), m_reduceNum(reduceNum){}

	void OnFinished()
	{
		if (NextRow())
		{
			UInt16 oldNum = GetData("num");
			UInt16 newNum = 0;
			if (m_reduceNum > oldNum)
			{
				json::Object body;
				body["error"] = json::Boolean(true);
				body["msg"] = json::String("Reduce Num More Then Current Num!");
				SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "deleteItem", body);
			}
			else
			{
				newNum = oldNum - m_reduceNum;

				if (newNum > 0)
				{
					CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_item", GetKey(), true);
					updateCmd->PutData("num", newNum);
					CLRecordClient::Instance()->SendCommand(updateCmd);
				}
				else
				{

					CLDeleteCommand* delCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_item", GetKey());
					CLRecordClient::Instance()->SendCommand(delCmd);
				}

				json::Object body;
				body["error"] = json::Boolean(false);
				body["itemGuid"] = json::Number((UInt64)GetKey());
				body["num"] = json::Number((UInt16)newNum);
				SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "deleteItem", body);
			}
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Select Item Failed!");
			SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "deleteItem", body);
		}
	}

	void OnFailed(UInt32 errcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Select Item Failed!");
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "userControl", "deleteItem", body);
	}

private:
	UInt32	m_ConnId;
	UInt16	m_reduceNum;
};

/**
 *@brief 查询道具列表回调
 */
class SelectRoleItemsCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectRoleItemsCallback, Avalon::Mutex)
public:
	SelectRoleItemsCallback(UInt32 connId, ObjID_t ownerUid) : m_ConnId(connId), m_ownerUid(ownerUid){}

	void OnFinished()
	{
		json::Object body;
		
		body["roleid"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(m_ownerUid));
		body["items"] = json::Array();
		json::Array& items = body["items"];

		while(NextRow())
		{
			UInt8 pack = GetData("pack");
			if (pack == PACK_INVALID || pack > PACK_STORAGE) continue;

			json::Object obj;
			obj["g"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(GetKey()));
			obj["i"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(GetData("itemid")));
			obj["b"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(GetData("bind")));
			obj["n"] = json::String(Avalon::StringUtil::ConvertToString<UInt16>(GetData("num")));
			obj["s"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(GetData("strengthen")));

			items.Insert(obj);
		}
		body["error"] = json::Boolean(false);
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "playerQueries", "items", body);
	}

	void OnFailed(UInt32 errcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "playerQueries", "items", body);
	}

private:
	//连接id
	UInt32	m_ConnId;
	//玩家uid
	ObjID_t	m_ownerUid;
};


/**
 *@brief 查询角色基本信息回调
 */
class SelectRoleBaseInfoCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectRoleBaseInfoCallback, Avalon::Mutex)
public:
	SelectRoleBaseInfoCallback(UInt32 connid, UInt8 keyType, UInt32 accId, ObjID_t playerId, const std::string& playerName)
		:m_ConnId(connid),m_KeyType(keyType),m_AccId(accId),m_PlayerId(playerId),m_PlayerName(playerName){}

	void OnFinished()
	{
		if(!NextRow())
		{
			OnFailed(0);
			return;
		}
		json::Object body;
		body["msg"] = json::Object();

		json::Object& msg = body["msg"];

		msg["roles"] = json::Array();
		json::Array& roles = msg["roles"];

		do
		{
			json::Object role;
			role["player"] = json::String(GetData("name").GetString());
			role["playerid"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(GetKey()));
			role["playerlevel"] = json::String(Avalon::StringUtil::ConvertToString<UInt16>(GetData("level")));
			role["playerclass"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(GetData("occu")));
			role["playersex"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(GetData("sex")));
			role["playerctime"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(GetData("createtime")));
			role["viplevel"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(GetData("viplvl")));
			role["userid"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(GetData("accid")));
			role["gold"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(GetData("gold")));
			role["deleteTime"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(GetData("deletetime")));
			std::istringstream iss(GetData("itemlock").GetString());
			
			if(PlayerMgr::Instance()->FindNormalPlayer(GetKey()) != NULL) role["isonline"] = json::String("1");
			else role["isonline"] = json::String("0");

			roles.Insert(role);
		}while(NextRow());

		body["error"] = json::Boolean(false);
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "playerQueries", "player", body);
	}

	void OnFailed(UInt32 errorcode)
	{
		std::ostringstream oss;

		if(m_KeyType == 1) oss << m_AccId;
		else if(m_KeyType == 2) oss << m_PlayerName;
		else oss << m_PlayerId;
		oss << " not finded!";

		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String(oss.str());
		SSConsoleAgent::Instance()->SendResponse(m_ConnId, "playerQueries", "player", body);
	}
private:
	UInt32	m_ConnId;
	UInt8	m_KeyType;
	UInt32	m_AccId;
	ObjID_t	m_PlayerId;
	std::string	m_PlayerName;
};

class ClearTowerCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(ClearTowerCallback, Avalon::Mutex)
public:
	void OnFinished()
	{
		while (NextRow())
		{
			ObjID_t roleid = (ObjID_t)GetKey();

			Player* player = PlayerMgr::Instance()->FindNormalPlayer(roleid);
			if (player != NULL)
			{
				player->ResetTowerCounter();
			}
			else
			{
				player = PlayerMgr::Instance()->FindPlayer(roleid);
				if (player != NULL)
				{
					player->ResetTowerCounter();
				}

				std::ostringstream counterCondStream;
				counterCondStream << "owner =" << roleid << " and (name='" << COUNTER_TOWER_TOP_FLOOR_TOTAL << "' or name='" << COUNTER_TOWER_USED_TIME_TOTAL << "' or name='" << COUNTER_TOWER_TOP_FLOOR << "')";
				CLUpdateCommand	*counterCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_counter", counterCondStream.str());
				counterCmd->PutData("value", 0);
				CLRecordClient::Instance()->SendCommand(counterCmd);
			}
		}
	}

	void OnFailed(UInt32 errorcode)
	{

	}
};

SSConsoleAgent::SSConsoleAgent()
{
}

SSConsoleAgent::~SSConsoleAgent()
{
}

void SSConsoleAgent::Init()
{
	RegisterHandler(new AbnormalTransQueryHandler());
	RegisterHandler(new AbnormalTransUnfreezeHandler());
}

void SSConsoleAgent::Final()
{
}

void SSConsoleAgent::QueryPlayerItems(UInt32 connid, ObjID_t playerId, UInt32 accid, UInt8 pack, UInt32 itemid)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerId);
	if(player != NULL)
	{
		json::Object body;
		body["roleid"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(playerId));
		body["items"] = json::Array();
		json::Array& items = body["items"];

		class GetItemsVisitor : public ItemVisitor
		{
		public:
			GetItemsVisitor(json::Array& itemList, UInt32 itemid):m_ItemList(itemList), m_itemid(itemid){}

			bool operator()(Item* item)
			{
				if (m_itemid > 0)
				{
					if (item->GetDataID() == m_itemid)
					{
						json::Object obj;
						obj["g"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(item->GetID()));
						obj["i"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetDataID()));
						obj["b"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(item->GetBind()));
						obj["n"] = json::String(Avalon::StringUtil::ConvertToString<UInt16>(item->GetNum()));
						obj["s"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(item->GetStrengthen()));
						obj["p"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetPos().pack));
						obj["m"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetMagicCardId()));
						obj["r"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetFirstPreciosBeadId()));
						m_ItemList.Insert(obj);
						return false;
					}
				}
				else
				{
					json::Object obj;
					obj["g"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(item->GetID()));
					obj["i"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetDataID()));
					obj["b"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(item->GetBind()));
					obj["n"] = json::String(Avalon::StringUtil::ConvertToString<UInt16>(item->GetNum()));
					obj["s"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(item->GetStrengthen()));
					obj["p"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetPos().pack));
					obj["m"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetMagicCardId()));
					obj["r"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(item->GetFirstPreciosBeadId()));
					m_ItemList.Insert(obj);
				}
				
				return true;
			}
		private:
			json::Array& m_ItemList;
			UInt32	m_itemid;
		};

		GetItemsVisitor visitor(items, itemid);
		if (0 == pack)
		{
			for (UInt8 i = PACK_EQUIP; i <= PACK_MAX; ++i)
			{
				player->VisitItems(i, visitor);
			}
		}
		else
		{
			player->VisitItems(pack, visitor);
		}
		
		
		body["error"] = json::Boolean(false);
		SendResponse(connid, "playerQueries", "items", body);
	}
	else
	{
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_item");
		command->PutColumn("itemid");
		command->PutColumn("bind");
		command->PutColumn("num");
		command->PutColumn("pack");
		command->PutColumn("strengthen");

		if (PACK_STORAGE == pack)
		{
			command->PutCond("pack", pack);
			command->PutCond("accid", accid);
		}
		else if (pack != 0)
		{
			command->PutCond("pack", pack);
			command->PutCond("owner", playerId);
		}
		else
		{
			command->PutCond("owner", playerId);
		}

		if (itemid > 0)
		{
			command->PutCond("itemid", itemid);
		}
		

		CLRecordClient::Instance()->SendCommand(command, new SelectRoleItemsCallback(connid, playerId));
	}
}

void SSConsoleAgent::QueryPlayerInfo(UInt32 connid, UInt8 keyType, UInt32 accId, ObjID_t playerId, const std::string& playerName)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerId);
	if(player != NULL)
	{
		json::Object body;
		body["msg"] = json::Object();

		json::Object& msg = body["msg"];

		msg["roles"] = json::Array();
		json::Array& roles = msg["roles"];

		json::Object role;
		role["player"] = json::String(player->GetName());
		role["playerid"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(player->GetID()));
		role["playerlevel"] = json::String(Avalon::StringUtil::ConvertToString<UInt16>(player->GetLevel()));
		role["playerclass"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(player->GetOccu()));
		role["playersex"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(player->GetSex()));
		role["playerctime"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(player->GetCreateTime()));
		role["viplevel"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(player->GetVipLvl()));
		role["userid"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(player->GetAccID()));
		role["isonline"] = json::String("1");
		role["gold"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(player->GetPoint()));
		role["deleteTime"] = json::String("0");
		roles.Insert(role);

		body["error"] = json::Boolean(false);

		SSConsoleAgent::Instance()->SendResponse(connid, "playerQueries", "player", body);
	}
	else
	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectCmd->PutColumn("accid");
		selectCmd->PutColumn("name");
		selectCmd->PutColumn("level");
		selectCmd->PutColumn("occu");
		selectCmd->PutColumn("sex");
		selectCmd->PutColumn("createtime");
		selectCmd->PutColumn("viplvl");
		selectCmd->PutColumn("gold");
		selectCmd->PutColumn("deletetime");
		selectCmd->PutColumn("itemlock");

		if(keyType == 1)
		{
			selectCmd->PutCond("accid", accId);
		}
		else if(keyType == 2)
		{
			selectCmd->PutCond("name", playerName);
		}
		else
		{
			selectCmd->PutCond("guid", playerId);
		}
		CLRecordClient::Instance()->SendCommand(selectCmd, new SelectRoleBaseInfoCallback(connid, keyType, accId, playerId, playerName));
	}
}

void SSConsoleAgent::DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd,  const std::string& json)
{
	CLProtocol::AdminConsoleResponse response;
	response.nodeid = connid; 
	response.cmd = cmd;
	response.subcmd = subcmd;
	response.timestamp = UInt32(CURRENT_TIME.Sec());
	response.body = json;
	Router::SendToAdmin(response);
}

void SSConsoleAgent::ForbidTalkPlayer(UInt32 connid, UInt8 keyType, UInt32 accId, ObjID_t playerId, const std::string& playerName, UInt32 forbidTime)
{
	if (forbidTime > forbidTalkMaxMin)
	{
		forbidTime = forbidTalkMaxMin;
	}

	Player* player = NULL;
	if (keyType == 2)
	{
		player = PlayerMgr::Instance()->FindNormalPlayer(playerId);
	}
	else if (keyType == 3)
	{
		player = PlayerMgr::Instance()->FindNormalPlayer(playerName);
	}
	
	if (player != NULL)
	{
		if (forbidTime == 0)
		{
			player->ClearForbitTalkDueTime();
		}
		else
		{
			Avalon::Time now = Avalon::Time::CurrentTime();
			if (player->GetForbitTaleDueTime() > (UInt32)now.Sec())
			{
				player->SetForbitTalkDueTime(player->GetForbitTaleDueTime() + forbidTime);
			}
			else
			{
				player->SetForbitTalkDueTime((UInt32)now.Sec() + forbidTime);
			}
		}
		
		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", player->GetID(), true);
		updateCmd->PutData("forbid_talk_duetime", player->GetForbitTaleDueTime());
		CLRecordClient::Instance()->SendCommand(updateCmd);

		json::Object body;
		body["error"] = json::Boolean(false);

		SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "forbidPlayer", body);
	}
	else
	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectCmd->PutColumn("forbid_talk_duetime");
		
		if (keyType == 1)
		{
			selectCmd->PutCond("accid", accId);
		}
		else if (keyType == 2)
		{
			selectCmd->PutCond("guid", playerId);
		}
		else if (keyType == 3)
		{
			selectCmd->PutCond("name", playerName);
		}

		CLRecordClient::Instance()->SendCommand(selectCmd, new SelectForbidTalkTimeCb(connid, forbidTime, keyType == 1));
	}
}

void SSConsoleAgent::DeleteItem(UInt32 connid, ObjID_t roleUid, ObjID_t itemUid, UInt16 num)
{
	Player* player = NULL;
	
	player = PlayerMgr::Instance()->FindNormalPlayer(roleUid);
	if (player)
	{
		Item* item = player->FindItem(itemUid);
		if (!item)
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Can not find item!");
			SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "deleteItem", body);
			return;
		}


		UInt32 retCode = player->ReduceItemNum(GetReason(SOURCE_TYPE_GM).c_str(), item, num);
		if (retCode != ErrorCode::SUCCESS)
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			std::ostringstream oss;
			oss << "reduce item error! code = " << retCode;
			body["msg"] = json::String(oss.str());
			SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "deleteItem", body);
			return;
		}
		else
		{
			UInt16 restNum = 0;
			Item* item = player->FindItem(itemUid);
			if (item)
			{
				restNum = item->GetNum();
			}

			json::Object body;
			body["error"] = json::Boolean(false);
			body["itemGuid"] = json::Number((UInt64)itemUid);
			body["num"] = json::Number((UInt16)restNum);
			SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "deleteItem", body);
		}
	}
	else
	{
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_item");
		command->PutColumn("num");
		command->PutCond("guid", itemUid);

		CLRecordClient::Instance()->SendCommand(command, new DeleteItemCb(connid, num));
	}
}

void SSConsoleAgent::DeleteMoney(UInt32 connid, ObjID_t roleUid, std::vector<ItemReward>& items)
{
	Player* player = NULL;

	player = PlayerMgr::Instance()->FindNormalPlayer(roleUid);
	if (player)
	{
		for (size_t i = 0; i < items.size(); ++i)
		{
			player->RemoveItem(GetReason(SOURCE_TYPE_GM).c_str(), items[i].id, items[i].num);
		}

		json::Object body;
		body["error"] = json::Boolean(false);
		SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "removeMoney", body);
	}
	else
	{
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		command->PutColumn("gold");
		command->PutColumn("point");
		command->PutColumn("exp");
		command->PutColumn("pk_coin");
		command->PutColumn("revive_coin");
		command->PutColumn("bindgold");
		command->PutColumn("bindpoint");
		command->PutColumn("guild_contri");
		command->PutCond("guid", roleUid);

		CLRecordClient::Instance()->SendCommand(command, new DeleteMoneyCb(connid, items));
	}
}

void SSConsoleAgent::SetPlayerLv(UInt32 connid, ObjID_t roleUid, UInt16 newLv)
{
	Player* player = NULL;

	player = PlayerMgr::Instance()->FindNormalPlayer(roleUid);
	if (player)
	{
		player->SetLevel(newLv);
	}
	
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleUid, true);
	updateCmd->PutData("level", newLv);
	CLRecordClient::Instance()->SendCommand(updateCmd, new UpdatePlayerLvCb(connid));
}

void SSConsoleAgent::SetPlayerVipLv(UInt32 connid, ObjID_t roleUid, UInt8 newVipLv)
{
	Player* player = NULL;

	player = PlayerMgr::Instance()->FindNormalPlayer(roleUid);
	if (player)
	{
		player->SetVipLvl(newVipLv);

		json::Object body;
		body["error"] = json::Boolean(false);
		SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "setVipLevel", body);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Player Is Not Exsit!");
		SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "setVipLevel", body);
	}
}

void SSConsoleAgent::FinishTask(UInt32 connid, ObjID_t roleUid, UInt32 taskId)
{
	Player* player = NULL;

	player = PlayerMgr::Instance()->FindNormalPlayer(roleUid);
	if (player)
	{
		bool ret = player->SubmitTask(taskId, true);
		if (ret)
		{
			json::Object body;
			body["error"] = json::Boolean(false);
			SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "finishTask", body);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Finish Task Error!");
			SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "finishTask", body);
		}
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Player Is Not Exsit!");
		SSConsoleAgent::Instance()->SendResponse(connid, "userControl", "finishTask", body);
	}
}

void SSConsoleAgent::PhoneBind(UInt32 connid, ObjID_t roleid)
{
	InfoLogStream << "recv player(" << roleid << ") bind phone." << LogStream::eos;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(roleid);
	if (!player)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("player not online");
		SSConsoleAgent::Instance()->SendResponse(connid, "playerManage", "phoneBind", body);
		
		ErrorLogStream << "player(" << roleid << ") bind phone, not online." << LogStream::eos;

		return;
	}

	CLEventMgr::Instance()->SendPlayerEvent(roleid, CLEventMgr::ET_PHONE_BIND, NULL);

	json::Object body;
	body["error"] = json::Boolean(false);
	SSConsoleAgent::Instance()->SendResponse(connid, "playerManage", "phoneBind", body);
}

void SSConsoleAgent::ClearTower(UInt32 connid, UInt32 accId, ObjID_t roleid)
{
	InfoLogStream << "recv player(" << accId << ", " << roleid << ") clear tower." << LogStream::eos;
	if (accId != 0)
	{
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		command->PutColumn("accid");
		command->PutCond("accid", accId);

		CLRecordClient::Instance()->SendCommand(command, new ClearTowerCallback());
	}
	else if (roleid != 0)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(roleid);
		if (player != NULL)
		{
			player->ResetTowerCounter();
		}
		else
		{
			player = PlayerMgr::Instance()->FindPlayer(roleid);
			if (player != NULL)
			{
				player->ResetTowerCounter();
			}

			std::ostringstream counterCondStream;
			counterCondStream << "owner =" << roleid << " and (name='" << COUNTER_TOWER_TOP_FLOOR_TOTAL << "' or name='" << COUNTER_TOWER_USED_TIME_TOTAL << "' or name='" << COUNTER_TOWER_TOP_FLOOR << "')";
			CLUpdateCommand	*counterCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_counter", counterCondStream.str());
			counterCmd->PutData("value", 0);
			CLRecordClient::Instance()->SendCommand(counterCmd);
		}
	}
}
void SSConsoleAgent::HandleRequest(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& param)
{
	ConsoleAgent::HandleRequest(connid, cmd, subcmd, param);
}
