#include "WSConsoleCmdHandler.h"

#include <sstream>
#include <json/writer.h>
#include <CLRecordClient.h>
#include <CLSysNotify.h>
#include <CLConsoleProtocol.h>
#include <CLSceneDataEntry.h>
#include <CLCenterProtocol.h>
#include <CLGameDefine.h>
#include <CLOpActiveProtocol.h>
#include <CLGiftBagProtocol.h>
#include <CLItemProtocol.h>

#include "WSConsoleAgent.h"
#include "WSPlayerMgr.h"
#include "PunishmentMgr.h"
#include "AnnouncementMgr.h"
#include "BroadcastMailMgr.h"
#include "WSSceneMgr.h"
#include "WSNetwork.h"
#include "WSScene.h"
#include "ScriptMgr.h"
#include "WSRouter.h"
#include "ChargeManager.h"
#include "AccountMgr.h"
#include "GuildMgr.h"
#include "WSSortListMgr.h"
#include "MallMgr.h"
#include "WSActivityMgr.h"
#include "CLActivityDefine.h"
#include "GuildBattleMgr.h"
#include "WSApplication.h"

void WSConsoleCmdHandler::TransmitToScene(UInt32 nodeid, json::Object& jsonobj)
{
	std::ostringstream oss;
	try
	{
		json::Writer::Write(jsonobj, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	CLProtocol::AdminConsoleRequest req;
	req.nodeid = nodeid;
	req.cmd = GetCmd();
	req.subcmd = GetSubCmd();
	req.timestamp = (UInt32)CURRENT_TIME.Sec();
	req.body = oss.str();
	Router::BroadcastToScene(req);

	GameInfoLogStream("Console") << "transmit console command(cmd:" << GetCmd() << ", subcmd:" << GetSubCmd() << ") to scene." << LogStream::eos;
}

void ConsoleOnlineNumHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string pfs;
	try
	{
		pfs = (json::String)jsonobj["pf"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleOnlineNumHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}
	
	json::Object body;

	std::vector<std::string> pf_vec;
	Avalon::StringUtil::Split(pfs, pf_vec, ",");

	UInt32 totalNum = 0;
	if (pf_vec.size() == 0)
	{
		totalNum = WSPlayerMgr::Instance()->GetOnlineNum();
	}
	else
	{
		for (std::vector<std::string>::iterator iter = pf_vec.begin();
			iter != pf_vec.end(); ++iter)
		{
			std::string& pf = *iter;

			if (pf == "")
			{
				totalNum = WSPlayerMgr::Instance()->GetOnlineNum();
				break;
			}
			else
			{
				totalNum += WSPlayerMgr::Instance()->GetPfPlayerNum(pf);
			}
		}
	}
	

	body["num"] = json::Number(totalNum);
	body["error"] = json::Boolean(false);
	
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void ConsoleSceneOnlineNumHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::map<UInt32, UInt32> playerNums;
	class GetScenesVisitor : public SceneDataEntryVisitor
	{
	public:
		GetScenesVisitor(std::map<UInt32, UInt32>& maps)
			:m_Maps(maps){}
		~GetScenesVisitor(){}

		bool operator()(SceneDataEntry* dataEntry)
		{
			m_Maps[dataEntry->id] = 0;
			return true;
		}
	private:
		std::map<UInt32, UInt32>& m_Maps;
	};
	GetScenesVisitor visitor(playerNums);
	SceneDataEntryMgr::Instance()->Visit(visitor);

	class GetSceneOnlineNumVisitor : public WSSceneVisitor
	{
	public:
		GetSceneOnlineNumVisitor(std::map<UInt32, UInt32>& nums)
			:m_Nums(nums){}

		bool operator()(WSScene* scene)
		{
			if(m_Nums.find(scene->GetMapID()) != m_Nums.end())
			{
				m_Nums[scene->GetMapID()] += scene->GetPlayerNum();
			}
			return true;
		}

	private:
		std::map<UInt32, UInt32>& m_Nums;
	};
	GetSceneOnlineNumVisitor sceneVisitor(playerNums);
	WSSceneMgr::Instance()->VisitScenes(sceneVisitor);

	json::Object body;
	body["scenes"] = json::Array();
	json::Array& scenes = body["scenes"];
	for(std::map<UInt32, UInt32>::iterator iter = playerNums.begin();
		iter != playerNums.end(); ++iter)
	{
		SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(iter->first);
		if(dataEntry == NULL) continue;

		json::Object scene;
		scene["id"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(dataEntry->id));
		scene["name"] = json::String(dataEntry->name);
		scene["num"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(iter->second));
		scenes.Insert(scene);
 	}
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void ConsoleRoleNumHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	class SelectRoleNumCallback : public CLRecordCallback
	{
	public:
		SelectRoleNumCallback(UInt32 connid):m_ConnId(connid){}

		void OnFinished()
		{
			if(!NextRow())
			{
				OnFailed(0);
				return;
			}

			UInt32 num = GetData("count(*)");
			json::Object body;
			body["error"] = json::Boolean(false);
			body["msg"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(num));
			WSConsoleAgent::Instance()->SendResponse(m_ConnId, "stateOnline", "roleNum", body);
		}

		void OnFailed(UInt32 errorcode)
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("database error");
			WSConsoleAgent::Instance()->SendResponse(m_ConnId, "stateOnline", "roleNum", body);
		}

	private:
		UInt32 m_ConnId;
	};
	
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selectCmd->PutColumn("count(*)");
	CLRecordClient::Instance()->SendCommand(selectCmd, new SelectRoleNumCallback(connid));
}


//批量查询玩家
class ConsoleBatchQueryPlayerCb : public CLRecordCallback
{
public:
	ConsoleBatchQueryPlayerCb(UInt32 connid, std::string cmd, std::string subCmd)
		: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid){}

	/**
	*@brief 成功
	*/
	virtual void OnFinished()
	{
		json::Object body;

		UInt32 accid = 0;
		json::Array players;
		while (NextRow())
		{
			accid = GetData("accid");
			ObjID_t roleId = GetKey();

			json::Object playerData;
			playerData["playername"] = json::String(GetData("name").GetString());
			playerData["playerid"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(roleId));
			playerData["accid"] = json::Number(accid);
			playerData["platform"] = json::String(GetData("platform").GetString());
			playerData["openid"] = json::String(GetData("openid").GetString());
			players.Insert(playerData);
		}

		body["players"] = players;
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

	/**
	*@brief 失败
	*/
	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("select batch player error!");
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

private:
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
};

class ConsoleQueryAccountCb : public CLRecordCallback
{
public:
	ConsoleQueryAccountCb(UInt32 connid, std::string cmd, std::string subCmd, json::Object* body)
	: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid), m_Body(body) {}

	virtual void OnFinished()
	{
		if (!m_Body)
		{
			return;
		}

		json::Array& players = (*m_Body)["players"];
		while (NextRow())
		{
			UInt32 accid = GetData("accid");
			for (auto itr = players.Begin(); itr != players.End(); ++itr)
			{
				json::Number jsonAccid = (*itr)["accid"];
				if (accid == jsonAccid.Value())
				{
					(*itr)["viplevel"] = json::Number((UInt32)GetData("viplvl"));
					(*itr)["point"] = json::Number((UInt32)GetData("point"));
					(*itr)["vipexp"] = json::Number((UInt32)GetData("vipexp"));
				}
			}
		}
		
		(*m_Body)["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, *m_Body);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		return;
	}

private:
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
	json::Object* m_Body;
};


class ConsoleQueryPlayerCallback : public CLRecordCallback
{
public:
	ConsoleQueryPlayerCallback(UInt32 connid, std::string cmd, std::string subCmd)
		: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid) {}

	/**
	*@brief 成功
	*/
	virtual void OnFinished()
	{
		json::Object* body = new json::Object();

		json::Array players;
		UInt32 count = 0;
		while (NextRow())
		{
			if (count >= WSApplication::Instance()->GetGMQueryPlayerMaxNum())
			{
				break;
			}
			count++;

			UInt32 accid = GetData("accid");
			ObjID_t roleId = GetKey();
			bool isOnline = WSPlayerMgr::Instance()->FindNormalPlayer(roleId) != 0;

			json::Object playerData;
			playerData["playername"] = json::String(GetData("name").GetString());
			playerData["playerid"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(roleId));
			playerData["accid"] = json::Number(accid);
			playerData["level"] = json::Number((UInt16)GetData("level"));
			playerData["occu"] = json::Number((UInt8)GetData("occu"));
			playerData["createtime"] = json::Number((UInt32)GetData("createtime"));
			UInt32 saveTime = (UInt32)((UInt64)GetData("savetime") / 1000);
			playerData["offlinetime"] = json::Number(saveTime);
			playerData["spendtime"] = json::Number((UInt32)GetData("totleonlinetime"));
			playerData["loginip"] = json::String(GetData("login_ip").GetString());
			playerData["isonline"] = json::Boolean(isOnline);
			playerData["bindpoint"] = json::Number((UInt32)GetData("bindpoint"));
			playerData["gold"] = json::Number((UInt32)GetData("gold"));
			playerData["bindgold"] = json::Number((UInt32)GetData("bindgold"));
			playerData["onlinetime"] = json::Number((UInt32)GetData("onlinetime"));
			playerData["chargenum"] = json::Number((UInt32)GetData("totlechargenum"));
			playerData["deletetime"] = json::Number((UInt32)GetData("deletetime"));

			std::string strFatigue = GetData("fatigue").GetString();
			std::istringstream is(strFatigue);
			char split = ',';
			UInt16 fatigue = 0;
			Avalon::Time lastRefreshTime;
			UInt16 useFatigue = 0;
			UInt32 time = 0;
			if (!is.eof())
			{
				is >> fatigue >> split >> useFatigue >> split >> time;
				lastRefreshTime = Avalon::Time((UInt64)time * (UInt64)Avalon::Time::MSECS_OF_SEC);
			}

			playerData["fatigue"] = json::Number(fatigue);
			playerData["exp"] = json::Number((UInt64)GetData("exp"));
			playerData["pkLevel"] = json::Number((UInt32)GetData("pkvalue"));
			playerData["matchscore"] = json::Number((UInt32)GetData("match_score"));
			playerData["pkcoin"] = json::Number((UInt32)GetData("pk_coin"));
			playerData["warriorsoul"] = json::Number((UInt32)GetData("warrior_soul"));
			playerData["guildcontri"] = json::Number((UInt32)GetData("guild_contri"));
			playerData["mouthcardexpire"] = json::Number((UInt32)GetData("month_card_expire_time"));

			ObjID_t guildId = 0;
			std::string guildName;
			auto guildMember = GuildMgr::Instance()->FindGuildMember(roleId);
			if (guildMember && guildMember->GetLeaveTime() == 0)
			{
				auto guild = GuildMgr::Instance()->FindGuildByID(guildMember->GetGuildID());
				if (guild)
				{
					guildId = guild->GetID();
					guildName = guild->GetName();
				}
			}

			playerData["guildid"] = json::Number(guildId);
			playerData["guildname"] = json::String(guildName);

			bool forbidLogin = PunishmentMgr::Instance()->GetPunishRemainTime(accid, PUNISH_FORBIDLOGIN) > 0;
			//bool forbidTalk = PunishmentMgr::Instance()->GetPunishRemainTime(accid, PUNISH_FORBIDTALK) > 0;
			playerData["forbidlogin"] = json::Boolean(forbidLogin);
			//playerData["forbidtalk"] = json::Boolean(forbidTalk);
			UInt32 dueTime = (UInt32)GetData("forbid_talk_duetime");
			playerData["forbidtalk"] = json::Boolean(dueTime > 0 ? true : false);
			playerData["platform"] = json::String(GetData("platform").GetString());
			playerData["openid"] = json::String(GetData("openid").GetString());
			playerData["bornzoneid"] = json::Number((UInt32)GetData("born_zoneid"));

			players.Insert(playerData);
		}

		if (players.Size() == 0)
		{
			// 没有角色
			//(*body)["players"] = players;
			(*body)["error"] = json::Boolean(true);
			(*body)["msg"] = json::String("Can Not Find Player!");
			WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, *body);
			CL_SAFE_DELETE(body);
			return;
		}

		std::vector<UInt32> dbSelectAccId;
		for (auto itr = players.Begin(); itr != players.End(); ++itr)
		{
			json::Number accid = (*itr)["accid"];
			auto accountInfo = AccountMgr::Instance()->FindAccount(accid.Value());
			if (accountInfo)
			{
				(*itr)["viplevel"] = json::Number(accountInfo->GetVipLevel());
				(*itr)["point"] = json::Number(accountInfo->GetPoint());
				(*itr)["vipexp"] = json::Number(accountInfo->GetVipExp());
			}
			else
			{
				bool find = false;
				for (size_t i = 0; i < dbSelectAccId.size(); ++i)
				{
					if (dbSelectAccId[i] == accid)
					{
						find = true;
						break;
					}
				}

				if (!find)
				{
					dbSelectAccId.push_back(accid);
				}
			}
		}

		//有需要去account表查询
		if (dbSelectAccId.size() != 0)
		{
			(*body)["players"] = players;

			auto sqlCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
			sqlCmd->PutColumn("point");
			sqlCmd->PutColumn("viplvl");
			sqlCmd->PutColumn("accid");
			sqlCmd->PutColumn("vipexp");
			//sqlCmd->PutCond("accid", accid);
			std::ostringstream oss;
			oss << " WHERE";
			
			for (size_t i = 0; i < dbSelectAccId.size(); ++i)
			{
				if (i > 0)
				{
					oss << " OR `accid` = " << dbSelectAccId[i];
				}
				else
				{
					oss << " `accid` = " << dbSelectAccId[i];
				}
			}
			sqlCmd->SetLimit(oss.str());

			CLRecordClient::Instance()->SendCommand(sqlCmd, new ConsoleQueryAccountCb(m_Connid, m_Cmd, m_SubCmd, body));
		}
		else
		{
			(*body)["players"] = players;
			(*body)["error"] = json::Boolean(false);
			WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, *body);
		}
	}

	/**
	*@brief 失败
	*/
	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Can Not Find Player!");
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

private:
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
};

void ConsoleQueryPlayer(UInt32 connid, std::string cmd, std::string subCmd, UInt32 accid, ObjID_t roleId, std::string name, std::string openid, UInt8 filterDelete, UInt8 isLike)
{
	auto sqlCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");

	if (accid != 0)
	{
		sqlCmd->PutCond("accid", accid);
		if (filterDelete == 1)
		{
			sqlCmd->PutCond("deletetime", 0);
		}
		
	}
	else if (roleId != 0)
	{
		sqlCmd->PutCond("guid", roleId);
	}
	else if (!name.empty())
	{
		if (isLike > 0)
		{
			if (!sqlCmd->SqlDefendInfusion(name))
			{
				json::Object body;
				body["error"] = json::Boolean(true);
				body["msg"] = json::String("name is error!");
				WSConsoleAgent::Instance()->SendResponse(connid, "playerQueries", "player", body);
				return;
			}

			std::ostringstream oss;
			oss << " WHERE  `name` LIKE '%" << name << "%'";
			sqlCmd->SetLimit(oss.str());
		}
		else
		{
			sqlCmd->PutCond("name", name);
		}
	}
	else if (!openid.empty())
	{
		sqlCmd->PutCond("openid", openid);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		WSConsoleAgent::Instance()->SendResponse(connid, "playerQueries", "player", body);
		return;
	}

	CLRecordClient::Instance()->SendCommand(sqlCmd, new ConsoleQueryPlayerCallback(connid, cmd, subCmd));
}

void ConsoleBatchQueryPlayers(UInt32 connid, std::string cmd, std::string subCmd, UInt8 filterDelete, std::vector<UInt32>& accids, std::vector<ObjID_t>& roleIds, std::vector<std::string>& names)
{
	auto sqlCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	sqlCmd->PutColumn("accid");
	sqlCmd->PutColumn("name");
	sqlCmd->PutColumn("platform");
	sqlCmd->PutColumn("openid");

	std::ostringstream limitoss;
	if (accids.size() > 0)
	{
		for (size_t i = 0; i < accids.size(); ++i)
		{
			if (0 == i)
			{
				limitoss << " WHERE (";
			}
			else
			{
				limitoss << " OR";
			}
			limitoss << " `accid` = " << accids[i];
		}

		limitoss << ")";
		if (filterDelete > 0)
		{
			limitoss << "AND `deletetime` = 0";
		}
		
		sqlCmd->SetLimit(limitoss.str());
	}
	else if (roleIds.size() > 0)
	{
		for (size_t i = 0; i < roleIds.size(); ++i)
		{
			if (0 == i)
			{
				limitoss << " WHERE";
			}
			else
			{
				limitoss << " OR";
			}
			limitoss << " `guid` = " << roleIds[i];
		}

		sqlCmd->SetLimit(limitoss.str());
	}
	else if (names.size() > 0)
	{
		for (size_t i = 0; i < names.size(); ++i)
		{
			if (!sqlCmd->SqlDefendInfusion(names[i]))
			{
				json::Object body;
				body["error"] = json::Boolean(true);
				body["msg"] = json::String("name is error!");
				WSConsoleAgent::Instance()->SendResponse(connid, "playerQueries", "player", body);
				return;
			}

			if (0 == i)
			{
				limitoss << " WHERE";
			}
			else
			{
				limitoss << " OR";
			}
			limitoss << " `name` = '" << names[i] << "'";
		}

		sqlCmd->SetLimit(limitoss.str());
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		WSConsoleAgent::Instance()->SendResponse(connid, "playerQueries", "player", body);
		return;
	}

	CLRecordClient::Instance()->SendCommand(sqlCmd, new ConsoleBatchQueryPlayerCb(connid, cmd, subCmd));
}

class ConsoleQueryAccountCallback : public AccountCallback
{
public:
	ConsoleQueryAccountCallback(UInt32 connid, std::string cmd, std::string subCmd, UInt32 accid, ObjID_t roleId, std::string name)
		: AccountCallback(accid), m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid), m_Accid(accid), m_RoleId(roleId), m_PlayerName(name) {}

	virtual void OnFinish(AccountInfo* info)
	{

	}
	virtual void OnFailure()
	{

	}

	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
	UInt32		m_Accid;
	ObjID_t		m_RoleId;
	std::string m_PlayerName;
};

void ConsoleQueryPlayerHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
// 	UInt32 accid = 0;
// 	UInt8 type = 0;
// 	std::string playername;
// 	ObjID_t	roleid = 0;

	std::vector<UInt32> accIds;
	std::vector<std::string> names;
	std::vector<ObjID_t> roleUids;
	UInt8 filterDelete = 0;
	UInt8 isLike = 0;

	std::string accidStr = "";
	std::string nameStr = "";
	std::string roleStr = "";
	std::string openIdStr = "";
	
	try
	{
// 		accid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accid"]);
// 		roleid = Avalon::StringUtil::ConvertToValue<ObjID_t>((json::String)jsonobj["roleid"]);
// 		playername = (json::String)jsonobj["name"];

		accidStr = (json::String)jsonobj["accid"];
		roleStr = (json::String)jsonobj["roleid"];
		nameStr = (json::String)jsonobj["name"];
		openIdStr = (json::String)jsonobj["openid"];
		Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["page_size"]);

		const json::String& filterStr = (json::String)jsonobj["filterdelete"];
		filterDelete = Avalon::StringUtil::ConvertToValue<UInt8>(filterStr.Value());
		isLike = Avalon::StringUtil::ConvertToValue<UInt8>(((json::String)jsonobj["islike"]).Value());

		UInt8 split;
		std::istringstream issAcc(accidStr);
		while (!accidStr.empty() && issAcc)
		{
			UInt32 accid = 0;
			issAcc >> accid >> split;
			accIds.push_back(accid);
		}

		std::vector<std::string> nameVec;
		Avalon::StringUtil::Split(nameStr, nameVec, ",");
		for (std::vector<std::string>::iterator iter = nameVec.begin();
			iter != nameVec.end(); ++iter)
		{
			names.push_back(*iter);
		}

		std::istringstream issRole(roleStr);
		while (!roleStr.empty() && issRole)
		{
			ObjID_t roleUid;
			issRole >> roleUid >> split;
			roleUids.push_back(roleUid);
		}
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleQueryPlayerHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	for (size_t i = 0; i < accIds.size(); ++i)
	{
		ConsoleQueryPlayer(connid, GetCmd(), GetSubCmd(), accIds[i], 0, "", "", filterDelete, isLike);
		return;
	}

	for (size_t i = 0; i < names.size(); ++i)
	{
		ConsoleQueryPlayer(connid, GetCmd(), GetSubCmd(), 0, 0, names[i], "", filterDelete, isLike);
		return;
	}

	for (size_t i = 0; i < roleUids.size(); ++i)
	{
		ConsoleQueryPlayer(connid, GetCmd(), GetSubCmd(), 0, roleUids[i], "", "", filterDelete, isLike);
		return;
	}

	if (!openIdStr.empty())
	{
		ConsoleQueryPlayer(connid, GetCmd(), GetSubCmd(), 0, 0, "", openIdStr, filterDelete, isLike);
		return;
	}
}

class ConsoleQueryPlayerByNameCallback : public CLRecordCallback
{
public:
	ConsoleQueryPlayerByNameCallback(UInt32 connid, std::string cmd, std::string subCmd, UInt32 param, UInt32 param2)
		: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid), m_Param(param), m_Param2(param2){}

	/**
	*@brief 成功
	*/
	virtual void OnFinished()
	{
		if (!NextRow())
		{
			// 玩家不存在
			json::Object body;
			body["error"] = json::Boolean(true);
			WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
			return;
		}

		ObjID_t roleId = GetKey();
		UInt32 accid = GetData("accid");

		auto serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleId);
		if (serverId == 0)
		{
			serverId = WSNetwork::Instance()->SelectRandSceneServer();
		}

		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
		if (conn != NULL)
		{
			CLProtocol::SceneConsoleQueryPlayer req;
			req.nodeid = m_Connid;
			req.subcmd = m_SubCmd;
			req.playerId = roleId;
			req.accId = accid;
			req.param = m_Param;
			req.param2 = m_Param2;

			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
		}
	}

	/**
	*@brief 失败
	*/
	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
	UInt32		m_Param;
	UInt32		m_Param2;
};

void ConsoleQueryPlayerItemHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleId = 0;
	std::string name;
	UInt8 position = 0;
	UInt32 itemid = 0;

	try
	{
		const json::String& roleidstr = jsonobj["roleid"];
		roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleidstr.Value());
		name = (json::String)jsonobj["name"];
		
		position = Avalon::StringUtil::ConvertToValue<UInt8>(((json::String)jsonobj["position"]).Value());
		itemid = Avalon::StringUtil::ConvertToValue<UInt32>(((json::String)jsonobj["itemid"]).Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleQueryPlayerItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (position == 1)	position = 0;
	else if (position == 2) position = PACK_STORAGE;
	else position = 0;

	UInt32 serverId = 0;
	UInt32 accid = 0;

	if (roleId != 0)
	{
		auto player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
		if (player)
		{
			serverId = player->GetSceneNode();
			accid = player->GetAccId();
		}
		else
		{
			auto sqlCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			sqlCmd->PutColumn("accid");
			sqlCmd->PutCond("guid", roleId);
			CLRecordClient::Instance()->SendCommand(sqlCmd, new ConsoleQueryPlayerByNameCallback(connid, GetCmd(), GetSubCmd(), position, itemid));
			return;
		}
	}
	else if (!name.empty())
	{
		auto player = WSPlayerMgr::Instance()->FindNormalPlayerByName(name);
		if (player)
		{
			serverId = player->GetSceneNode();
			accid = player->GetAccId();
		}
		else
		{
			auto sqlCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			sqlCmd->PutColumn("accid");
			sqlCmd->PutCond("name", name);
			CLRecordClient::Instance()->SendCommand(sqlCmd, new ConsoleQueryPlayerByNameCallback(connid, GetCmd(), GetSubCmd(), position, itemid));
			return;
		}
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

		return;
	}

	CLProtocol::SceneConsoleQueryPlayer req;
	req.nodeid = connid;
	req.subcmd = GetSubCmd();
	req.playerId = roleId;
	req.name = name;
	req.accId = accid;
	req.param = position;
	req.param2 = itemid;

	if (serverId == 0)
	{
		serverId = WSNetwork::Instance()->SelectRandSceneServer();
	}

	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
	}
}

void ConsoleUserControlHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::vector<UInt32> accids;
	std::vector<ObjID_t> roleids;
	std::vector<std::string> names;
	//UInt32 serverId = 0;
	UInt32 time = 0;
	UInt8 type = 0;
	try
	{
		const json::String& accidstr = jsonobj["accid"];
		std::vector<std::string> accidVec;
		Avalon::StringUtil::Split(accidstr.Value(), accidVec, ",");
		for (std::vector<std::string>::iterator iter = accidVec.begin();
			iter != accidVec.end(); ++iter)
		{
			UInt32 accid = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
			accids.push_back(accid);
		}

		const json::String& roleidstr = jsonobj["roleid"];
		std::vector<std::string> roleidVec;
		Avalon::StringUtil::Split(roleidstr.Value(), roleidVec, ",");
		for (std::vector<std::string>::iterator iter = roleidVec.begin();
			iter != roleidVec.end(); ++iter)
		{
			ObjID_t roleid = Avalon::StringUtil::ConvertToValue<ObjID_t>(*iter);
			roleids.push_back(roleid);
		}

		const json::String& namestr = jsonobj["name"];
		std::vector<std::string> nameVec;
		Avalon::StringUtil::Split(namestr.Value(), nameVec, ",");
		for (std::vector<std::string>::iterator iter = nameVec.begin();
			iter != nameVec.end(); ++iter)
		{
			names.push_back(*iter);
		}

		//const json::String& serveridStr = jsonobj["serverid"];
		//serverId = Avalon::StringUtil::ConvertToValue<UInt32>(serveridStr.Value());

		const json::String& timestr = jsonobj["time"];
		time = Avalon::StringUtil::ConvertToValue<UInt32>(timestr.Value());

		const json::String& typestr = jsonobj["type"];
		type = Avalon::StringUtil::ConvertToValue<UInt8>(typestr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleUserControlHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (accids.size() != 0)
	{
		for (size_t i = 0; i < accids.size(); ++i)
		{
			
// 			if (0 == serverId && 3 == type || 4 == type)
// 			{
// 				// 发送给center通知
// 				CLProtocol::CenterForbidAccountReq protocol;
// 				protocol.accidList = accids;
// 				protocol.time = time;
// 				Router::SendToCenter(protocol);
// 			}
// 			else
// 			{
				DoForbid(type, connid, 0, accids[i], "", time);
//			}
		}
	}
	else if (roleids.size() != 0)
	{
		for (size_t i = 0; i < roleids.size(); ++i)
		{
			DoForbid(type, connid, roleids[i], 0, "", time);
		}
	}
	else if (names.size() != 0)
	{
		for (size_t i = 0; i < names.size(); ++i)
		{
			DoForbid(type, connid, 0, 0, names[i], time);
		}
	}
}

void ConsoleUserControlHandler::DoForbid(UInt8 type, UInt32 connId, ObjID_t roleUid, UInt32 accId, std::string name, UInt32 time)
{
	switch (type)
	{
	case 1:	//禁言
		DoForbidTalk(connId, roleUid, accId, name, time);
		break;
	case 2: //解除禁言
		DoForbidTalk(connId, roleUid, accId, name, 0);
		break;
	case 3: //封号
		DoForbigAccount(connId, accId, time);
		break;
	case 4: //解除封号
		DoForbigAccount(connId, accId, 0);
		break;
	}
}

void ConsoleUserControlHandler::DoForbidTalk(UInt32 connId, ObjID_t roleUid, UInt32 accId, std::string name, UInt32 time)
{
	CLProtocol::SceneConsoleControlPlayer req;
	req.nodeid = connId;
	req.subcmd = GetSubCmd();
	req.accId = accId;
	req.playerId = roleUid;
	req.name = name;
	req.param1 = time;

	UInt32 serverId = 0;
	if (roleUid != 0)
	{
		serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleUid);
		req.keyType = 2;
	}
	else if (!name.empty())
	{
		auto player = WSPlayerMgr::Instance()->FindNormalPlayerByName(name);
		if (player)
		{
			serverId = player->GetSceneNode();
		}

		req.keyType = 3;
	}
	else if (accId != 0)
	{
		req.keyType = 1;
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("No Operation key!");
		WSConsoleAgent::Instance()->SendResponse(connId, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (serverId == 0)
	{
		serverId = WSNetwork::Instance()->SelectRandSceneServer();
	}

	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Can Not Find SceneServer!");
		WSConsoleAgent::Instance()->SendResponse(connId, GetCmd(), GetSubCmd(), body);
	}
}

class DelSortListCallback : public CLRecordCallback
{
public:
	DelSortListCallback(UInt32 connid) :m_connid(connid){}
	~DelSortListCallback(){}

public:
	void OnFinished()
	{
		while (NextRow())
		{
			
			WSSortListMgr::Instance()->DeleteAllEntry(GetKey());
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		
	}

private:
	//账号id
	UInt32 m_connid;
};

void ConsoleUserControlHandler::DoForbigAccount(UInt32 connId, UInt32 accId, UInt32 time)
{
	json::Object body;
	if (accId != 0)
	{
		if (time != 0)
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(accId);
			if (!player)
			{
				InfoLogStream << "player(" << accId << ") offline when forbid login." << LogStream::eos;
			}
			else
			{
				if (player->GetGameStatus() != PS_NORMAL)
				{
					InfoLogStream << "player(" << accId << ", " << player->GetID() << ") invalid status(" << (UInt32)player->GetGameStatus() << ") when forbid login." << LogStream::eos;
				}

				player->KickOff(ErrorCode::SYS_FORBIT_LOGIN);
			}

			PunishmentMgr::Instance()->Punish(accId, PUNISH_FORBIDLOGIN, time);

			CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			selectCmd->PutCond("accid", accId);
			CLRecordClient::Instance()->SendCommand(selectCmd, new DelSortListCallback(connId));

			//删除红包排名
			CLProtocol::CenterDeleteRedPacketInfo stReq;
			stReq.zoneId = ZONE_ID;
			stReq.accId = accId;
			Router::SendToCenter(stReq);
		}
		else
		{
			PunishmentMgr::Instance()->UnPunish(accId, PUNISH_FORBIDLOGIN);
		}

		body["error"] = json::Boolean(false);
	}
	else
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("DoForbigAccount accId is 0!");
	}
	
	WSConsoleAgent::Instance()->SendResponse(connId, GetCmd(), GetSubCmd(), body);
}

void ForbidTalkAllHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 accid = 0;
	UInt8 type = 0;
	try
	{
		const json::String& uidstr = jsonobj["userid"];
		accid = Avalon::StringUtil::ConvertToValue<UInt32>(uidstr.Value());
		const json::String& typestr = jsonobj["type"];
		type = Avalon::StringUtil::ConvertToValue<UInt8>(typestr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ForbidTalkAllHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if(type == 1)
	{
		CLProtocol::CenterForbidTalkReq req;
		req.accid = accid;
		Router::SendToCenter(req);
	}
	else
	{
		CLProtocol::CenterUnForbidTalkReq req;
		req.accid = accid;
		Router::SendToCenter(req);
	}

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

class QueryPlayerNumCallback : public CLRecordCallback
{
public:
	QueryPlayerNumCallback(UInt32 connid, ObjID_t roleid) :m_connid(connid), m_roleid(roleid){}
	~QueryPlayerNumCallback(){}

public:
	void OnFinished()
	{
		//UInt32 num = 0;
		//while (NextRow())
		//{
		//	UInt32 deleteTime = GetData("deletetime");
		//	if (deleteTime == 0)
		//	{
		//		num++;
		//	}
		//}

		json::Object body;
		//if (num < 8) 恢复角色去掉数量限制
		{
			CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", m_roleid, true);
			updateCmd->PutData("deletetime", 0);
			CLRecordClient::Instance()->SendCommand(updateCmd);

			body["error"] = json::Boolean(false);
			body["msg"] = json::Boolean(true);
			WSConsoleAgent::Instance()->SendResponse(m_connid, "userControl", "recoveryPlayer", body);
		}
		//else
		//{
		//	body["error"] = json::Boolean(true);
		//	body["msg"] = json::String("role num more then 3!");
		//	WSConsoleAgent::Instance()->SendResponse(m_connid, "userControl", "recoveryPlayer", body);
		//}

	}

	void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("select player fail!");
		WSConsoleAgent::Instance()->SendResponse(m_connid, "userControl", "recoveryPlayer", body);

		return;
	}

private:
	//账号id
	UInt32 m_connid;
	//角色uid
	ObjID_t m_roleid;
};

void RecoverRoleHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t playerId = 0;
	UInt32 accid = 0;
	try
	{
		const json::String& idstr = jsonobj["roleid"];
		const json::String& accidStr = jsonobj["accid"];
		playerId = Avalon::StringUtil::ConvertToValue<UInt64>(idstr.Value());
		accid = Avalon::StringUtil::ConvertToValue<UInt64>(accidStr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "RecoverRoleHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(playerId);
	if (player)
	{
		player->KickOff(ErrorCode::SYS_GM_KICK);
	}

	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selectCmd->PutColumn("name");
	selectCmd->PutColumn("deletetime");
	selectCmd->PutCond("accid", accid);

	CLRecordClient::Instance()->SendCommand(selectCmd, new QueryPlayerNumCallback(connid, playerId));

}

void PublicAnnouncementHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string content;
	UInt32	begintime = 0;
	UInt32	interval = 0;
	UInt32	times = 0;
	UInt32	gmid = 0;
	UInt8	color = 0;

	try
	{
		const json::String& contentstr = jsonobj["content"];
		content = contentstr.Value();
		const json::String& timestr = jsonobj["begintime"];
		begintime = Avalon::StringUtil::ConvertToValue<UInt32>(timestr.Value());
		const json::String& intervalstr = jsonobj["repeatinterval"];
		interval = Avalon::StringUtil::ConvertToValue<UInt32>(intervalstr.Value());
		const json::String& timesstr = jsonobj["repeattimes"];
		times = Avalon::StringUtil::ConvertToValue<UInt32>(timesstr.Value());
		const json::String& gmidstr = jsonobj["announce_id"];
		gmid = Avalon::StringUtil::ConvertToValue<UInt32>(gmidstr.Value());
		const json::String& colorstr = jsonobj["color"];
		color = Avalon::StringUtil::ConvertToValue<UInt8>(colorstr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "PublicAnnouncementHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	//立即发送
	if (begintime == 0)
	{
		begintime = UInt32(CURRENT_TIME.Sec());
		interval = 1;
	}

	std::ostringstream os;
	if (color != 3)
	{
		os << "<color=#ffd042>" << content << "</color>";
	}
	else
	{
		os << content;
	}
	
	AnnouncementMgr::Instance()->PublicCustomAnnouncement(8, begintime, interval, times, gmid, os.str());
	//AnnouncementMgr::Instance()->PublicAnnouncement(showzone, color, content, begintime, interval, times);

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void BroadcastMailHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string senderName;
	std::string title;
	std::string content;
	std::string items;
	std::string reason = GetReason(SOURCE_TYPE_GM);
	bool	bAll = false;
	std::string players;
	std::string pf;
	UInt8 occu = 0;
	UInt32 playerChargeNum = 0;
	try
	{
		const json::String& senderNameStr = jsonobj["sendername"];
		senderName = senderNameStr.Value();
		const json::String& titlestr = jsonobj["title"];
		title = titlestr.Value();
		const json::String& contentstr = jsonobj["content"];
		content = contentstr.Value();
		const json::String& itemsstr = jsonobj["items"];
		items = itemsstr.Value();
		const json::String& isall = jsonobj["isall"];
		bAll = (Avalon::StringUtil::ConvertToValue<UInt32>(isall.Value()) == 1);
		const json::String& playersstr = jsonobj["roleid"];
		players = playersstr.Value();
		pf = ((json::String)jsonobj["pf"]).Value();
		const json::String& occuStr = jsonobj["occu"];
		occu = Avalon::StringUtil::ConvertToValue<UInt8>(occuStr.Value());
		const json::String& playerChargeNumStr = jsonobj["playerChargeNum"];
		playerChargeNum = Avalon::StringUtil::ConvertToValue<UInt32>(playerChargeNumStr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "BroadcastMailHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	std::vector<std::vector<ItemReward> > rewardGroups;
	std::vector<ItemReward>	rewards;
	UInt32 itemid = 0;
	UInt32 num = 0;
	char split = 0;
	std::istringstream stream1(items);
	UInt32 itemnum = 0;
	while(stream1)
	{
		if(++itemnum > MAX_MAILITEM_NUM)
		{
			itemnum = 1;
			rewardGroups.push_back(rewards);
			rewards.clear();
		}

		stream1 >> itemid;
		if(itemid == 0) break;
		stream1 >> split >> num >> split;
		ItemReward reward;
		reward.id = itemid;
		reward.qualityLv = 0;
		reward.num = num;
		rewards.push_back(reward);
	}

	if(!rewards.empty())
	{
		rewardGroups.push_back(rewards);
		rewards.clear();
	}

	std::set<ObjID_t>	playerSet;
	std::istringstream	stream2(players);
	ObjID_t	playerid = 0;
	while(stream2)
	{
		stream2 >> playerid;
		if(playerid == 0) break;
		playerSet.insert(playerid);
		stream2 >> split;
	}

	if (senderName.empty())
	{
		senderName = "GM";
	}

	if(bAll)
	{	
		if(rewardGroups.empty())
		{
			json::Object body;

			UInt32 ckRet = CheckMail(0, title, content, rewards.size(), senderName);
			if (ckRet != ErrorCode::SUCCESS)
			{
				body["error"] = json::Boolean(true);
				std::ostringstream msg;
				msg << "check mail info error! code = " << ckRet;
				body["msg"] = json::String(msg.str());
				WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
				return;
			}
			else
			{
				BroadcastMailMgr::Instance()->PutMail(senderName, title, content, reason, rewards, 0, pf, occu, playerChargeNum);

				body["error"] = json::Boolean(false);
				WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
				return;
			}
			
		} 
		else
		{
			for(std::vector<std::vector<ItemReward> >::iterator iter = rewardGroups.begin(); iter != rewardGroups.end(); ++iter)
			{
				json::Object body;
				UInt32 ckRet = CheckMail(0, title, content, (*iter).size(), senderName);
				if (ckRet != ErrorCode::SUCCESS)
				{
					body["error"] = json::Boolean(true);
					std::ostringstream msg;
					msg << "check mail info error! code = " << ckRet;
					body["msg"] = json::String(msg.str());
					WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
					return;
				}
				else
				{
					BroadcastMailMgr::Instance()->PutMail(senderName, title, content, reason, *iter, 0, pf, occu, playerChargeNum);
					body["error"] = json::Boolean(false);
					WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
					return;
				}
			}
		}
	}
	else if(!playerSet.empty())
	{
		class SendSysMailCallback : public SendMailCallback
		{
		public:
			SendSysMailCallback(UInt32 connid, const std::string& cmd, const std::string& subcmd, int refCount)
				:SendMailCallback(refCount), m_ConnId(connid), m_Cmd(cmd), m_SubCmd(subcmd){}

		public:
			void OnSucceed(ObjID_t receiverId){}

			void OnFailed(ObjID_t receiverId, int errcode)
			{
				m_Ids.insert(receiverId);
				json::Object body;
				body["error"] = json::Boolean(true);
				std::ostringstream oss;
				oss << "send system mail failed! code = " << errcode;
				body["msg"] = json::String(oss.str());
				WSConsoleAgent::Instance()->SendResponse(m_ConnId, m_Cmd, m_SubCmd, body);
			}

			void OnFinished()
			{
				bool bError = false;
				if(!m_Ids.empty())
				{
					bError = true;
					std::ostringstream oss;
					for(std::set<ObjID_t>::iterator iter = m_Ids.begin();
						iter != m_Ids.end(); ++iter)
					{
						oss << *iter << ",";
					}
					oss << " not finded!";
				}
				json::Object body;
				body["error"] = json::Boolean(bError);
				WSConsoleAgent::Instance()->SendResponse(m_ConnId, m_Cmd, m_SubCmd, body);
			}

		private:
			UInt32 m_ConnId;
			std::string	m_Cmd;
			std::string	m_SubCmd;
			std::set<ObjID_t> m_Ids;
		};

		SendSysMailCallback* callback = new SendSysMailCallback(connid, GetCmd(), GetSubCmd()
			, playerSet.size() * (rewardGroups.empty() ? 1 : rewardGroups.size()));
		for(std::set<ObjID_t>::iterator iter = playerSet.begin();
			iter != playerSet.end(); ++iter)
		{
			if(rewardGroups.empty())
			{
				json::Object body;
				UInt32 ckRet = CheckMail(0, title, content, 0, senderName);
				if (ckRet != ErrorCode::SUCCESS)
				{
					body["error"] = json::Boolean(true);
					std::ostringstream msg;
					msg << "check mail info error! code = " << ckRet;
					body["msg"] = json::String(msg.str());
					WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
					return;
				}
				else
				{
					MailBox::SendSysMail(*iter, senderName, title, content, reason, std::vector<ItemReward>(), callback);
					body["error"] = json::Boolean(false);
					WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
				}
			}
			else
			{
				for(std::vector<std::vector<ItemReward> >::iterator rewardIter = rewardGroups.begin();
					rewardIter != rewardGroups.end(); ++rewardIter)
				{
					json::Object body;
					UInt32 ckRet = CheckMail(0, title, content, rewards.size(), senderName);
					if (ckRet != ErrorCode::SUCCESS)
					{
						body["error"] = json::Boolean(true);
						std::ostringstream msg;
						msg << "check mail info error! code = " << ckRet;
						body["msg"] = json::String(msg.str());
						WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
						return;
					}
					else
					{
						MailBox::SendSysMail(*iter, senderName, title, content, reason, *rewardIter, callback);
						body["error"] = json::Boolean(false);
						WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
					}
				}
			}
		}
	}
}

void SendDetailMailHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	//宝珠随机BUFF  m_Param2
	//宝珠摘取次数  m_BeadExtripeCnt
	//宝珠置换次数

	std::string senderName;
	std::string title;
	std::string content;
	std::string reason = GetReason(SOURCE_TYPE_GM);
	ObjID_t playerId;
	std::vector<WSItem*> items;
	WSItem* item = NULL;

	try
	{
		const json::String& senderNameStr = jsonobj["sendername"];
		senderName = senderNameStr.Value();
		const json::String& titlestr = jsonobj["title"];
		title = titlestr.Value();
		const json::String& contentstr = jsonobj["content"];
		content = contentstr.Value();
		const json::String& playersstr = jsonobj["roleid"];
		playerId = Avalon::StringUtil::ConvertToValue<UInt64>(playersstr.Value());

		const json::Array& jsonItems = jsonobj["items"];

		if (jsonItems.Size() > MAX_MAILITEM_NUM)
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("item size not greate 4");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		}

		auto itemIter = jsonItems.Begin();
		while (itemIter != jsonItems.End())
		{
			item = NULL;
			const json::Object& obj = *itemIter;
			++itemIter;

			const json::Number itemId = obj["itemId"];
			item = WSItem::CreateItem((UInt32)itemId.Value());
			if (item == NULL) return;
			item->SetID(CLRecordClient::Instance()->GenGuid());
			item->SetNum(1);
			const json::Number beadExtripeCnt = obj["beadExtripeCnt"];
			item->SetBeadExtripeCnt((UInt32)beadExtripeCnt.Value());
			const json::Number beadReplaceCnt = obj["beadReplaceCnt"];
			item->SetBeadReplaceCnt((UInt32)beadReplaceCnt.Value());
			const json::Number buff = obj["buff"];
			item->SetParam2((UInt32)buff);
			items.push_back(item);
		}

		MailBox::SendSysMail(playerId, senderName, title, content, reason, items);

		json::Object body;
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SendDetailMailHandler failed:" << e.what() << "!" << LogStream::eos;
		for (auto destroyItem : items)
		{
			if (item != destroyItem)
			{
				WSItem::DestroyItem(destroyItem);
			}
		}

		if (item != NULL)
		{
			WSItem::DestroyItem(item);
		}
	}

}

void SendAwardHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string title;
	std::string content;
	std::string items;
	std::string reason = GetReason(SOURCE_TYPE_GM);
	UInt32		userid = 0;

	try
	{
		const json::String& titlestr = jsonobj["title"];
		title = titlestr.Value();
		const json::String& contentstr = jsonobj["content"];
		content = contentstr.Value();
		const json::String& itemsstr = jsonobj["items"];
		items = itemsstr.Value();
		const json::String& uidstr = jsonobj["userid"];
		userid = Avalon::StringUtil::ConvertToValue<UInt32>(uidstr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SendAwardHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	std::vector<ItemReward>	rewards;
	UInt32 itemid = 0;
	int quality = 0;
	UInt32 num = 0;
	char split = 0;
	std::istringstream stream1(items);
	UInt32 itemnum = 0;
	while(stream1 && ++itemnum <= MAX_MAILITEM_NUM)
	{
		stream1 >> itemid;
		if(itemid == 0) break;
		stream1 >> split >> quality >> split >> num >> split;
		ItemReward reward;
		reward.id = itemid;
		reward.strenth = UInt8(quality);
		reward.qualityLv = 0;
		reward.num = num;
		rewards.push_back(reward);
	}

	MailBox::SendSysMailByAccId(userid, title, content, reason, 0, rewards);

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void ShowAnnouncementsHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 page = 0;
	UInt32 pagesize = 0;
	try
	{
		json::String pagestr = jsonobj["page"];
		page = Avalon::StringUtil::ConvertToValue<UInt32>(pagestr.Value());
		if(page == 0) page = 1;
		json::String pagesizestr = jsonobj["pagesize"];
		pagesize = Avalon::StringUtil::ConvertToValue<UInt32>(pagesizestr.Value());
		if(pagesize == 0) pagesize = 1;
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ShowAnnouncementsHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	json::Object body;
	body["announcements"] = json::Array();

	class GetAnnouncementsVisitor : public AnnouncementVisitor
	{
	public:
		GetAnnouncementsVisitor(json::Array& list, UInt32 page, UInt32 pagesize)
			:m_AnnouncementList(list),m_Num(0),m_No(0),m_Page(page),m_PageSize(pagesize){}

		bool operator()(Announcement* announcement)
		{
			++m_No;
			if(m_No > (m_Page - 1) * m_PageSize && m_No <= m_Page * m_PageSize)
			{
				json::Object obj;
				obj["id"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(announcement->id));
				//obj["color"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(announcement->color));
				obj["content"] = json::String(announcement->content);
				obj["time"] = json::Number(announcement->beginTime);
				obj["interval"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(announcement->interval));
				obj["repeattimes"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(announcement->repeattimes));
				//obj["showzone"] = json::String(Avalon::StringUtil::ConvertToString<UInt8>(announcement->showzone));
				m_AnnouncementList.Insert(obj);
			}
			return true;
		}

	private:
		json::Array& m_AnnouncementList;
		UInt32	m_Num;
		UInt32	m_No;
		UInt32	m_Page;
		UInt32	m_PageSize;
	};
	GetAnnouncementsVisitor visitor(body["announcements"], page, pagesize);
	body["total"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(AnnouncementMgr::Instance()->GetAnnouncementNum()));
	AnnouncementMgr::Instance()->VisitAnnouncements(visitor);

	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void DelAnnounceHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 gmid = 0;
	try
	{
		json::String idstr = jsonobj["announce_id"];
		gmid = Avalon::StringUtil::ConvertToValue<UInt32>(idstr.Value());
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "DelAnnounceHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	AnnouncementMgr::Instance()->RemoveCustomAnnouncement(gmid);

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void ReloadScriptsHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	//ScriptMgr::Instance()->ReloadScripts();

	std::vector<UInt32> wsNeedReloadScripts;
	std::vector<UInt32> ssNeedReloadScripts;
	try
	{
		auto worldScriptsItr = jsonobj.Find("WorldScript");
		if (worldScriptsItr != jsonobj.End())
		{
			auto f = *worldScriptsItr;
			json::Array& worldScripts = f.element;
			for (auto itr = worldScripts.Begin(); itr != worldScripts.End(); ++itr)
			{
				json::String scriptTypeStr = *itr;
				wsNeedReloadScripts.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(scriptTypeStr.Value()));
			}

			if (wsNeedReloadScripts.empty())
			{
				json::String scriptTypeStr = worldScriptsItr->element;
				wsNeedReloadScripts.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(scriptTypeStr.Value()));
			}
		}

		auto sceneScriptsItr = jsonobj.Find("SceneScript");
		if (sceneScriptsItr != jsonobj.End())
		{
			auto f = *sceneScriptsItr;
			json::Array& sceneScripts = f.element;
			for (auto itr = sceneScripts.Begin(); itr != sceneScripts.End(); ++itr)
			{
				json::String scriptTypeStr = *itr;
				ssNeedReloadScripts.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(scriptTypeStr.Value()));
			}

			if (ssNeedReloadScripts.empty())
			{
				json::String scriptTypeStr = sceneScriptsItr->element;
				ssNeedReloadScripts.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(scriptTypeStr.Value()));
			}
		}

	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ReloadScriptsHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	for (auto scriptType : wsNeedReloadScripts)
	{
		if (!ScriptMgr::Instance()->ReloadScript(scriptType))
		{
			ErrorLogStream << "Reload Script(" << scriptType << ") failed." << LogStream::eos;
		}
	}

	CLProtocol::SceneConsoleReloadScripts req;
	req.scripts = ssNeedReloadScripts;
	Router::BroadcastToScene(req);

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void ReturnItemHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t playerId = 0;
	std::string params;
	try
	{
		const json::String& roleidstr = jsonobj["playerid"];
		playerId = Avalon::StringUtil::ConvertToValue<UInt64>(roleidstr.Value());
		const json::String& paramsstr = jsonobj["params"];
		params = paramsstr.Value();
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ReturnItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	std::map<std::string, std::string>	paramMap;

	std::vector<std::string> pairs;
	Avalon::StringUtil::Split(params, pairs, "|");
	for(std::vector<std::string>::iterator iter = pairs.begin();
		iter != pairs.end(); ++iter)
	{
		std::vector<std::string> keyvalue;
		Avalon::StringUtil::Split(*iter, keyvalue, "=");
		if(keyvalue.size() >= 2)
		{
			paramMap[keyvalue[0]] = keyvalue[1];
		}
	}

	std::string errmsg;

	WSItem* item = new WSItem();
	item->SetID(CLRecordClient::Instance()->GenGuid());
	item->m_DataId = Avalon::StringUtil::ConvertToValue<UInt32>(paramMap["itemid"]);
	item->m_Num = Avalon::StringUtil::ConvertToValue<UInt16>(paramMap["num"]);
	item->m_Bind = 1;
	item->m_Quality = Avalon::StringUtil::ConvertToValue<UInt8>(paramMap["quality"]);
	item->m_Strengthen = Avalon::StringUtil::ConvertToValue<UInt8>(paramMap["strengthen"]);
	//item->validAttrNum = Avalon::StringUtil::ConvertToValue<UInt8>(paramMap["validattrnum"]);
	//item->unlockDegree = 0;
	item->m_Param1 = Avalon::StringUtil::ConvertToValue<UInt32>(paramMap["param1"]);
	item->m_Param2 = Avalon::StringUtil::ConvertToValue<UInt32>(paramMap["param2"]);
	std::istringstream randattrstr(paramMap["randattr"]);
	item->m_RandProperties.DecodeString(randattrstr);
	item->ClearDirty();
	std::vector<WSItem*> itemVec;
	itemVec.push_back(item);

	const SysNotify* pTitle = SysNotifyMgr::Instance()->FindNotify(224);
	const SysNotify* pContent = SysNotifyMgr::Instance()->FindNotify(225);
	std::string reason = GetReason(SOURCE_TYPE_GM);
	MailBox::SendSysMail(playerId, pTitle->format, pContent->format, reason, 0, itemVec);

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void GiveItemHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ItemRewardVec items;
	std::string content;
	std::vector<ObjID_t> playerList;
	UInt8 serverBroadcast = 0;
	std::string reason = GetReason(SOURCE_TYPE_GM);
	std::string params;
	UInt8 occu = 0;
	UInt32 playerChargeNum = 0;
	try
	{
		const json::Array& playerListObj = jsonobj["player_list"];
		for (auto iter = playerListObj.Begin(); iter != playerListObj.End(); ++iter)
		{
			json::String playerIdStr = *iter;
			ObjID_t playerId = Avalon::StringUtil::ConvertToValue<UInt64>(playerIdStr.Value());
			if (playerId > 0)
			{
				playerList.push_back(playerId);
			}
		}

		serverBroadcast = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)jsonobj["server_broadcast"]);

		json::Array& itemListObj = jsonobj["items"];
		for (auto iter = itemListObj.Begin(); iter != itemListObj.End(); ++iter)
		{
			json::Object& itemObj = *iter;
			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)itemObj["id"]);
			UInt32 itemNum = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)itemObj["num"]);
			ItemReward item;
			item.id = itemId;
			item.num = itemNum;
			items.push_back(item);
		}

		
		content = (json::String)jsonobj["content"];

		const json::String& occuStr = jsonobj["occu"];
		occu = Avalon::StringUtil::ConvertToValue<UInt8>(occuStr.Value());
		const json::String& playerChargeNumStr = jsonobj["playerChargeNum"];
		playerChargeNum = Avalon::StringUtil::ConvertToValue<UInt32>(playerChargeNumStr.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ReturnPetHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (serverBroadcast == 0)
	{
		for (auto playerId : playerList)
		{
			MailBox::SendSysMail(playerId, "system", "gm", content, reason, items);
		}
	}
	else
	{
		BroadcastMailMgr::Instance()->PutMail("system", "gm", content, reason, items, 0, "", occu, playerChargeNum);
	}
	

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

class QueryPlayerCallback : public CLRecordCallback
{
public:
	QueryPlayerCallback(UInt32 connid) :m_connid(connid){}
	~QueryPlayerCallback(){}

public:
	void OnFinished()
	{
		json::Object body;
		
		json::Array playerInfoes;
		while (NextRow())
		{
			json::Object info;
			ObjID_t id = GetKey();
			info["id"] = json::String(Avalon::StringUtil::ConvertToString(id));
			info["accid"] = json::Number(GetData("accid"));
			info["name"] = json::String(GetData("name").GetString());
			info["level"] = json::Number(GetData("level"));
			info["gold"] = json::Number(GetData("gold"));
			info["point"] = json::Number(GetData("point"));
			info["reg_time"] = json::Number(GetData("createtime"));

			playerInfoes.Insert(info);
		}

		body["info"] = playerInfoes;
		body["error"] = json::Boolean(false);
		body["msg"] = json::Boolean(true);
		WSConsoleAgent::Instance()->SendResponse(m_connid, "playerManager", "query", body);
	}

	void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::Number(errorcode);
		WSConsoleAgent::Instance()->SendResponse(m_connid, "playerManager", "query", body);

		return;
	}

private:
	//账号id
	UInt32 m_connid;
};

void QueryPlayerHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 accid = 0;

	try
	{
		accid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["account_id"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ReturnPetHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	cmd->PutColumn("accid");
	cmd->PutColumn("name");
	cmd->PutColumn("level");
	cmd->PutColumn("gold");
	cmd->PutColumn("point");
	cmd->PutColumn("createtime");

	cmd->PutCond("accid", accid);

	std::ostringstream limitoss;
	limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DAY_TO_SEC << ")";
	cmd->SetLimit(limitoss.str());

	DebugLogStream << "player(" << accid << ") start to query role list." << LogStream::eos;

	CLRecordClient::Instance()->SendCommand(cmd, new QueryPlayerCallback(connid));
}

void PunishPlayerHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 accid = 0;
	UInt8 type = 0;
	UInt32 durTime = 0;

	try
	{
		accid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["account_id"]);
		type = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)jsonobj["type"]);
		durTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["duration_time"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "PunishPlayerHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	PunishmentMgr::Instance()->Punish(accid, type, durTime);
	if (type == PUNISH_FORBIDLOGIN)
	{
		auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(accid);
		if (player)
		{
			player->KickOff(ErrorCode::SYS_FORBIT_LOGIN);
		}
	}

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, "playerManager", "punish", body);
}

void ChargeHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string orderId;
	std::string token;
	std::string channel;
	UInt32 accid = 0;
	ObjID_t roleId = 0;
	UInt32 goodsId = 0;
	UInt32 chargeMoney = 0;
	UInt32 orderTime = 0;
	UInt8 mallType = 0;

	try
	{
		channel = (json::String)jsonobj["channel"];
		orderId = (json::String)jsonobj["orderid"];
		token = (json::String)jsonobj["token"];
		accid = Avalon::StringUtil::ConvertToValue<ObjID_t>((json::String)jsonobj["accid"]);
		roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>((json::String)jsonobj["roleid"]);
		goodsId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["goodsid"]);
		chargeMoney = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["money"]);
		orderTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["ts"]);
		mallType = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["malltype"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ChargeHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (mallType == CHARGE_MALL_CHARGE || mallType == CHARGE_MALL_MONEY_MANAGE || mallType == CHARGE_DAY_CHARGE_WELFARE ||
		mallType == CHARGE_ADVENTURE_PASS_KING || mallType == CHARGE_GIGT_RIGHT_CARD || mallType == CHARGE_DUNGEON_RIGHT_CARD)
	{
		ChargeManager::Instance()->StartCharge(connid, channel, orderId, token, accid, roleId, goodsId, chargeMoney, orderTime, (ChargeMallType)mallType);
	}
	else
	{
		ChargeManager::Instance()->BuyChargePacket(connid, channel, orderId, token, accid, roleId, goodsId, chargeMoney, orderTime);
	}
}

void ConsoleDeleteItemHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleUid = 0;
	ObjID_t itemUid = 0;
	UInt32 serverId = 0;
	UInt16 num = 0;

	try
	{
		const json::String& roleidstr = jsonobj["roleid"];
		roleUid = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleidstr.Value());
		const json::String& itemUidstr = jsonobj["itemGuid"];
		itemUid = Avalon::StringUtil::ConvertToValue<ObjID_t>(itemUidstr.Value());
		const json::String& numStr = jsonobj["num"];
		num = Avalon::StringUtil::ConvertToValue<UInt16>(numStr.Value());

	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleDeleteItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (roleUid != 0)
	{
		serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleUid);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("roleUid is 0!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

		return;
	}

	CLProtocol::SceneConsoleControlPlayer req;
	req.nodeid = connid;
	req.subcmd = GetSubCmd();
	req.keyType = 2;
	req.playerId = roleUid;
	req.param1 = itemUid;
	req.param2 = num;

	if (serverId == 0)
	{
		serverId = WSNetwork::Instance()->SelectRandSceneServer();
	}

	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Can Not Find SceneServer!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
	}
}

void ConsoleDeleteMoneyHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleUid = 0;
	std::string removeMap;
	try
	{
		const json::String& roleidstr = jsonobj["roleid"];
		roleUid = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleidstr.Value());
		const json::String& removeStr = jsonobj["removeMap"];
		removeMap = removeStr.Value();
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleDeleteMoneyHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	UInt32 moneyTypeTmp = 0;
	std::vector<ItemReward>	moneys;
	UInt32 num = 0;
	char split = 0;
	std::istringstream iss(removeMap);
	UInt32 itemnum = 0;

	// 最多支持8种类别货币一次性操作
	while (iss && ++itemnum <= 8)
	{
		iss >> moneyTypeTmp;
		iss >> split >> num >> split;

		ItemReward money;
		money.id = moneyTypeTmp;
		money.num = num;
		moneys.push_back(money);
	}

	UInt32 serverId = 0;
	if (roleUid != 0)
	{
		serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleUid);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("roleUid is 0!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

		return;
	}

	CLProtocol::SceneConsoleControlPlayer req;
	req.nodeid = connid;
	req.subcmd = GetSubCmd();
	req.playerId = roleUid;
	req.items = moneys;

	if (serverId == 0)
	{
		serverId = WSNetwork::Instance()->SelectRandSceneServer();
	}

	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Can Not Find SceneServer!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
	}
}

void ConsoleSetPlayerLvHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleUid = 0;
	UInt16 newLv = 0;
	try
	{
		const json::String& roleidstr = jsonobj["roleid"];
		roleUid = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleidstr.Value());
		const json::String& lvStr = jsonobj["level"];
		newLv = Avalon::StringUtil::ConvertToValue<UInt16>(lvStr.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleDeleteItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	UInt32 serverId = 0;
	if (roleUid != 0)
	{
		serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleUid);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("roleUid is 0!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

		return;
	}

	CLProtocol::SceneConsoleControlPlayer req;
	req.nodeid = connid;
	req.subcmd = GetSubCmd();
	req.playerId = roleUid;
	req.param2 = newLv;

	if (serverId == 0)
	{
		serverId = WSNetwork::Instance()->SelectRandSceneServer();
	}

	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Can Not Find SceneServer!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
	}
}

void ConsoleSetPlayerVipLvHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 accId = 0;
	UInt8 newVipLv = 0;
	//UInt32 serverId = 0;
	try
	{
		const json::String& accidstr = jsonobj["accid"];
		accId = Avalon::StringUtil::ConvertToValue<UInt32>(accidstr.Value());
		const json::String& lvStr = jsonobj["level"];
		newVipLv = Avalon::StringUtil::ConvertToValue<UInt8>(lvStr.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleDeleteItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	AccountInfo* accInfo = AccountMgr::Instance()->FindAccount(accId);
	if (accInfo)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(accId);
		if (player)
		{
			player->KickOff(ErrorCode::SYS_GM_KICK);

// 			serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(player->GetID());
// 			CLProtocol::SceneConsoleControlPlayer req;
// 			req.nodeid = connid;
// 			req.subcmd = GetSubCmd();
// 			req.playerId = player->GetID();
// 			req.param2 = newVipLv;
// 
// 			Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
// 			if (conn != NULL)
// 			{
// 				CL_SEND_PROTOCOL(conn, req);
// 			}
// 			else
// 			{
// 				json::Object body;
// 				body["error"] = json::Boolean(true);
// 				body["msg"] = json::String("Can Not Find SceneServer!");
// 				WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
// 				return;
// 			}
		}

		accInfo->SetVipLevel(newVipLv);
		accInfo->SetVipExp(0);
		accInfo->SaveToDB();

		json::Object body;
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
	}
	else
	{
		std::ostringstream condstream;
		condstream << "accid =" << accId;
		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", condstream.str());
		updateCmd->PutData("viplvl", newVipLv);
		updateCmd->PutData("vipexp", 0);
		CLRecordClient::Instance()->SendCommand(updateCmd);

		json::Object body;
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

	}
}

void ConsoleSetPlayerFinishTaskHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleUid = 0;
	UInt32 taskId = 0;
	UInt32 serverId = 0;
	try
	{
		const json::String& roleuidstr = jsonobj["roleid"];
		roleUid = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleuidstr.Value());
		const json::String& taskidStr = jsonobj["taskID"];
		taskId = Avalon::StringUtil::ConvertToValue<UInt32>(taskidStr.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleDeleteItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleUid);
	if (!player)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Player Is Not Online!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}
	else
	{
		CLProtocol::SceneConsoleControlPlayer req;
		req.nodeid = connid;
		req.subcmd = GetSubCmd();
		req.playerId = roleUid;
		req.param2 = taskId;

		serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleUid);

		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
		if (conn != NULL)
		{
			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Can Not Find SceneServer!");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}
}

class SelectChargeOrderCb : public CLRecordCallback
{
public:
	SelectChargeOrderCb(UInt32 connid, std::string cmd, std::string subCmd, UInt32 curPage, UInt32 maxPage, bool hasCond, std::string pf)
		: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid), m_curPage(curPage), m_maxPage(maxPage), m_hasCond(hasCond), m_pf(pf){}

	/**
	*@brief 成功
	*/
	virtual void OnFinished()
	{
		json::Object body;
		json::Array records;
		UInt32 maxpage = 0;
		while (NextRow())
		{
			std::string pf = GetData("channel").GetString();
			std::string orderid = GetData("orderid").GetString();
			//如果pf是空得只有两种渠道可能:xy或i4, i4得orderid长度是16byte，大于16得是xy
// 			if (pf == "")
// 			{
// 				if (orderid.size() > 16)
// 				{
// 					pf = "xy";
// 				}
// 				else
// 				{
// 					pf = "i4";
// 				}
// 			}

// 			if (m_pf != "" && m_pf != pf)
// 			{
// 				continue;
// 			}

			json::Object record;
			record["orderId"] = json::String(orderid);
			record["status"] = json::Number((UInt8)GetData("status"));
			record["accid"] = json::Number((UInt32)GetData("accid"));
			ObjID_t roleId = GetData("roleid");
			record["roleid"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(roleId));
			record["charge_goods_id"] = json::Number((UInt32)GetData("charge_goods_id"));
			record["charge_item_id"] = json::Number((UInt32)GetData("charge_item_id"));
			record["charge_item_num"] = json::Number((UInt32)GetData("charge_item_num"));
			record["vip_score"] = json::Number((UInt32)GetData("vip_score"));
			record["charge_money"] = json::Number((UInt32)GetData("charge_money"));
			record["time"] = json::Number((UInt32)GetData("time"));
			record["pf"] = json::String(pf);

			records.Insert(record);
			maxpage++;
		}

		body["records"] = records;
		body["error"] = json::Boolean(false);
		body["page_id"] = json::Number((UInt32)m_curPage);

		body["total_num"] = json::Number((UInt32)m_maxPage);
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

	/**
	*@brief 失败
	*/
	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Select Charge Order Failed!");
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

private:
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
	UInt32		m_curPage;
	UInt32		m_maxPage;
	bool		m_hasCond;
	std::string m_pf;
};

struct QueryOrderInfo
{
	UInt32 accid;
	std::string	orderId;
	ObjID_t roleUid = 0;
	UInt8	status;
	UInt32 startTime;
	UInt32 endTime;
	UInt16 curPage;
	UInt16 pageSize;
	std::string pf;
};

class SelectChargeOrderCountCb : public CLRecordCallback
{
public:
	SelectChargeOrderCountCb(UInt32 connid, std::string cmd, std::string subCmd, QueryOrderInfo& info)
		: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid), m_queryInfo(info) {}

	/**
	*@brief 成功
	*/
	virtual void OnFinished()
	{
		if (NextRow())
		{
			if (m_queryInfo.curPage < 1)
			{
				json::Object body;
				body["error"] = json::Boolean(true);
				body["msg"] = json::String("Page Error!");
				WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
				return;
			}

			CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_order");
			UInt32 count = GetData("count(*)");

			bool hasCond = false;
			if (!m_queryInfo.orderId.empty())	{
				selCmd->PutCond("orderid", m_queryInfo.orderId.c_str());
				hasCond = true;
			}
			if (m_queryInfo.roleUid != 0)		{
				selCmd->PutCond("roleid", m_queryInfo.roleUid);
				hasCond = true;
			}
			if ((Int8)m_queryInfo.status > 0)		{
				selCmd->PutCond("status", m_queryInfo.status);
				hasCond = true;
			}
			if (m_queryInfo.accid > 0)	{
				selCmd->PutCond("accid", m_queryInfo.accid);
				hasCond = true;
			}

			if (m_queryInfo.pf != "")
			{
				selCmd->PutCond("channel", m_queryInfo.pf);
				hasCond = true;
			}

			std::ostringstream oss;
			
			if (m_queryInfo.startTime != 0 && m_queryInfo.endTime != 0)
			{
				if (hasCond)
				{
					oss << "AND";
				}
				else
				{
					oss << "WHERE";
				}
				oss << " time >= " << m_queryInfo.startTime << " AND " << " time <= " << m_queryInfo.endTime << " limit " << (m_queryInfo.curPage - 1)*m_queryInfo.pageSize << "," << m_queryInfo.pageSize;
				selCmd->SetLimit(oss.str());
				hasCond = true;
			}
			else if (m_queryInfo.startTime != 0)
			{
				if (hasCond)
				{
					oss << "AND";
				}
				else
				{
					oss << "WHERE";
				}
				oss << " time >= " << m_queryInfo.startTime << " limit " << (m_queryInfo.curPage - 1)*m_queryInfo.pageSize << "," << m_queryInfo.pageSize;
				selCmd->SetLimit(oss.str());
				hasCond = true;
			}
			else if (m_queryInfo.endTime != 0)
			{
				if (hasCond)
				{
					oss << "AND";
				}
				else
				{
					oss << "WHERE";
				}
				oss << " time <= " << m_queryInfo.endTime << " limit " << (m_queryInfo.curPage - 1)*m_queryInfo.pageSize << "," << m_queryInfo.pageSize;
				selCmd->SetLimit(oss.str());
				hasCond = true;
			}
			else
			{
				oss << " limit " << (m_queryInfo.curPage - 1)*m_queryInfo.pageSize << "," << m_queryInfo.pageSize;
				selCmd->SetLimit(oss.str());
			}
			
			
			CLRecordClient::Instance()->SendCommand(selCmd, new SelectChargeOrderCb(m_Connid, m_Cmd, m_SubCmd, m_queryInfo.curPage, count, hasCond, m_queryInfo.pf));
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Select Charge Order Count Failed!");
			WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
			return;
		}
	}

	/**
	*@brief 失败
	*/
	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("Select Charge Order Count Failed!");
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

private:
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
	QueryOrderInfo m_queryInfo;
};


void QueryChargeOrderHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	QueryOrderInfo info;

	try
	{
		const json::String& orderstr = jsonobj["orderId"];
		info.orderId = orderstr.Value();
		const json::String& roleidStr = jsonobj["roleId"];
		info.roleUid = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleidStr.Value());
		const json::String& statusStr = jsonobj["status"];
		info.status = Avalon::StringUtil::ConvertToValue<UInt8>(statusStr.Value());
		const json::String& startTimeStr = jsonobj["startTime"];
		info.startTime = Avalon::StringUtil::ConvertToValue<UInt32>(startTimeStr.Value());
		const json::String& endTimeStr = jsonobj["endTime"];
		info.endTime = Avalon::StringUtil::ConvertToValue<UInt32>(endTimeStr.Value());
		const json::String& curpageStr = jsonobj["page_id"];
		info.curPage = Avalon::StringUtil::ConvertToValue<UInt16>(curpageStr.Value());
		const json::String& pageszStr = jsonobj["page_size"];
		info.pageSize = Avalon::StringUtil::ConvertToValue<UInt16>(pageszStr.Value());
		const json::String& accidStr = jsonobj["accid"];
		info.accid = Avalon::StringUtil::ConvertToValue<UInt32>(accidStr.Value());
		const json::String& pfStr = jsonobj["pf"];
		info.pf = pfStr.Value();
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleDeleteItemHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_order");
	selCmd->PutColumn("count(*)");

	bool hasCond = false;
	if (!info.orderId.empty())	{
		selCmd->PutCond("orderid", info.orderId.c_str());
		hasCond = true;
	}
	if (info.roleUid != 0)	{
		selCmd->PutCond("roleid", info.roleUid);
		hasCond = true;
	}
	if ((Int8)info.status > 0)	{
		selCmd->PutCond("status", info.status);
		hasCond = true;
	}
	if (info.accid > 0)	{
		selCmd->PutCond("accid", info.accid);
		hasCond = true;
	}

	if (info.pf != "")
	{
		selCmd->PutCond("channel", info.pf);
		hasCond = true;
	}

	std::ostringstream oss;

	if (info.startTime != 0 && info.endTime != 0)
	{
		if (hasCond)
		{
			oss << "AND";
		}
		else
		{
			oss << "WHERE";
		}
		oss << " time >= " << info.startTime << " AND " << " time <= " << info.endTime;
		selCmd->SetLimit(oss.str());
		hasCond = true;
	}
	else if (info.startTime != 0)
	{
		if (hasCond)
		{
			oss << "AND";
		}
		else
		{
			oss << "WHERE";
		}
		oss << " time >= " << info.startTime;
		selCmd->SetLimit(oss.str());
		hasCond = true;
	}
	else if (info.endTime != 0)
	{
		if (hasCond)
		{
			oss << "AND";
		}
		else
		{
			oss << "WHERE";
		}
		oss << " time <= " << info.endTime;
		selCmd->SetLimit(oss.str());
		hasCond = true;
	}

	CLRecordClient::Instance()->SendCommand(selCmd, new SelectChargeOrderCountCb(connid, GetCmd(), GetSubCmd(), info));	
}

void ConsoleBatchQueryPlayerHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::vector<UInt32> accIds;
	std::vector<std::string> names;
	std::vector<ObjID_t> roleUids;

	std::string accidStr = "";
	std::string nameStr = "";
	std::string roleStr = "";

	UInt8 filterDelete = 0;
	try
	{
		accidStr = (json::String)jsonobj["accid"];
		roleStr = (json::String)jsonobj["roleid"];
		nameStr = (json::String)jsonobj["name"];

		UInt8 split;
		std::istringstream issAcc(accidStr);
		while (!accidStr.empty() && issAcc)
		{
			UInt32 accid = 0;
			issAcc >> accid >> split;
			accIds.push_back(accid);
		}

		std::vector<std::string> nameVec;
		Avalon::StringUtil::Split(nameStr, nameVec, ",");
		for (std::vector<std::string>::iterator iter = nameVec.begin();
			iter != nameVec.end(); ++iter)
		{
			names.push_back(*iter);
		}

		std::istringstream issRole(roleStr);
		while (!roleStr.empty() && issRole)
		{
			ObjID_t roleUid;
			issRole >> roleUid >> split;
			roleUids.push_back(roleUid);
		}

		const json::String& filterStr = (json::String)jsonobj["filterdelete"];
		filterDelete = Avalon::StringUtil::ConvertToValue<UInt8>(filterStr.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleQueryPlayerHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	ConsoleBatchQueryPlayers(connid, GetCmd(), GetSubCmd(), filterDelete, accIds, roleUids, names);
}

void DeleteRoleHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleId = 0;

	try
	{
		const json::String& strRoleId = jsonobj["roleid"];
		roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(strRoleId.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "DeleteRoleHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
	if (player)
	{
		player->KickOff(ErrorCode::SYS_GM_KICK);
	}

	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleId, true);
	updateCmd->PutData("deletetime", UInt32(CURRENT_TIME.Sec()));
	CLRecordClient::Instance()->SendCommand(updateCmd);

	json::Object body;
	body["error"] = json::Boolean(false);
	body["msg"] = json::Boolean(true);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

}

void PhoneBindHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	ObjID_t roleId = 0;

	try
	{
		const json::String& strRoleId = jsonobj["roleid"];
		roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(strRoleId.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "PhoneBindHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	InfoLogStream << "recv player(" << roleId << ") bind phone." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
	if (!player)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("player not online");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);

		ErrorLogStream << "player(" << roleId << ") bind phone, not online." << LogStream::eos;

		return;
	}

	CLProtocol::SceneConsolePhoneBind protocol;
	protocol.nodeid = connid;
	protocol.roleid = roleId;
	player->SendToScene(protocol);
}

class SelectChargeMoneyCallback : public CLRecordCallback
{
public:
	SelectChargeMoneyCallback(UInt32 connid, std::string cmd, std::string subCmd)
	: m_Cmd(cmd), m_SubCmd(subCmd), m_Connid(connid) {}

	/**
	*@brief 成功
	*/
	virtual void OnFinished()
	{
		UInt32 totalMoney = 0;
		while (NextRow())
		{
			 totalMoney += (UInt32)GetData("charge_money");
		}

		json::Object body;
		body["error"] = json::Boolean(false);
		body["msg"] = json::Boolean(true);
		body["money"] = json::Number(totalMoney);
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

	/**
	*@brief 失败
	*/
	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("DBError");
		WSConsoleAgent::Instance()->SendResponse(m_Connid, m_Cmd, m_SubCmd, body);
	}

private:
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32		m_Connid;
	QueryOrderInfo m_queryInfo;
};

void QueryChargeMoneyHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 startTime, endTime;

	try
	{
		const json::String& startTimeStr = jsonobj["startTime"];
		startTime = Avalon::StringUtil::ConvertToValue<UInt32>(startTimeStr.Value());
		const json::String& endTimeStr = jsonobj["endTime"];
		endTime = Avalon::StringUtil::ConvertToValue<UInt32>(endTimeStr.Value());
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryChargeOrderHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_order");
	selCmd->PutColumn("charge_money");
	selCmd->PutCond("time", startTime, ">=");
	selCmd->PutCond("time", endTime, "<=");
	selCmd->PutCond("status", (UInt32)ORDER_STATUS_FINISH);

	CLRecordClient::Instance()->SendCommand(selCmd, new SelectChargeMoneyCallback(connid, GetCmd(), GetSubCmd()));
}

void QueryMallConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	UInt8 type = 0;
	UInt8 subType = 0;
	UInt8 job = 0;

	try
	{
		type = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)body["t"]);
		subType = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)body["st"]);
		job = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)body["jt"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryMallConfigHandler failed, read failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	ItemKey itemKey(type, subType, job);
	VecItem mallItems;
	MallMgr::Instance()->GetIdxItems(itemKey, mallItems);

	if (mallItems.empty())
	{
		GameErrorLogStream("Console") << "QueryMallConfigHandler failed, can not find mall items by key(" << type << ", " << subType << ", " << job << ")!" << LogStream::eos;

		body["msg"] = json::String("can not find mall item!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	json::Array itemsArray;

	for (auto mallItem : mallItems)
	{
		if (!mallItem) continue;

		json::Object item;
		item["g"] = json::Number(MallMgr::Instance()->TransUidLocalToGm(mallItem->id));
		item["t"] = json::Number(mallItem->type);
		item["st"] = json::Number(mallItem->subType);
		item["jt"] = json::Number(mallItem->jobType);
		item["lst"] = json::Number(mallItem->startTime);
		item["let"] = json::Number(mallItem->endTime);

		itemsArray.Insert(item);
	}

	body["goods"] = itemsArray;
	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void SetMallConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	json::Array items;

	try
	{
		std::string itemsStr = (json::String)jsonobj["goods"];
		if (itemsStr.empty())
		{
			body["msg"] = json::String("goods are empty!");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			GameErrorLogStream("Console") << "SetMallConfigHandler failed: goods are empty!" << LogStream::eos;
			return;
		}

		std::istringstream is(itemsStr);
		json::Reader::Read(items, is);
	}
	catch (const json::Exception& e)
	{
		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "SetMallConfigHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	json::Array::const_iterator itr = items.Begin();
	for (; itr != items.End(); ++itr)
	{
		json::Object item = *itr;
		MallItemInfo* mallItemInfo = new MallItemInfo();

		try
		{
			UInt32 gmUid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["g"]);
			mallItemInfo->id = MallMgr::Instance()->TransUidGMToLocal(gmUid);
			mallItemInfo->type = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["t"]);
			mallItemInfo->subType = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["st"]);
			mallItemInfo->jobType = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["jt"]);
			mallItemInfo->itemid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["ii"]);
			mallItemInfo->itemnum = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["gn"]);
			mallItemInfo->price = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["p"]);
			mallItemInfo->discountprice = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["dp"]);
			mallItemInfo->moneytype = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["mt"]);
			mallItemInfo->bLimit = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["lt"]);
			mallItemInfo->limitnum = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)item["ln"]);
			mallItemInfo->icon = (json::String)item["gpi"];
			mallItemInfo->startTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["lst"]);
			mallItemInfo->endTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["let"]);

			if (mallItemInfo->endTime > 0 && mallItemInfo->endTime < curTime)
			{
				GameErrorLogStream("Console") << "SetMallConfigHandler failed: " << mallItemInfo->LogStr() << " endTime(" << TimeUtil::TimestampToStandardFormat(mallItemInfo->endTime) << ") reach current time." << LogStream::eos;
				CL_SAFE_DELETE(mallItemInfo);
				continue;
			}

			mallItemInfo->limitTotalNum = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)item["ltn"]);
			mallItemInfo->vipScore = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)item["vs"]);
			mallItemInfo->giftName = (json::String)item["gpn"];
			mallItemInfo->tagType = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["ih"]);
			mallItemInfo->sortIdx = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["s"]);
			mallItemInfo->hotSortIdx = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)item["hs"]);
			mallItemInfo->goodsType = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)item["ig"]);
			mallItemInfo->desc = (json::String)item["desc"];
			std::string rewardStr = (json::String)item["gpr"];
			MallMgr::Instance()->DecodeRewardItems(mallItemInfo->rewards, rewardStr);
		}
		catch (const json::Exception& e)
		{
			CL_SAFE_DELETE(mallItemInfo);
			body["msg"] = json::String("decode json failed!");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			GameErrorLogStream("Console") << "SetMallConfigHandler failed while read mall item info: " << e.what() << "!" << LogStream::eos;
			return;
		}

		MallMgr::Instance()->GmSetGoods(mallItemInfo);
	}

	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void DelMallConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	std::string uidStr;
	try
	{
		uidStr = (json::String)jsonobj["g"];
	}
	catch (const json::Exception& e)
	{
		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "DelMallConfigHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (uidStr.empty())
	{
		body["msg"] = json::String("the param g, which value is empty!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "DelMallConfigHandler failed: uids are empty!" << LogStream::eos;
		return;
	}

	std::vector<std::string> uidStrs;
	Avalon::StringUtil::Split(uidStr, uidStrs, ",");
	for (auto elem : uidStrs)
	{
		UInt32 uid = Avalon::StringUtil::ConvertToValue<UInt32>(elem);
		if (!MallMgr::Instance()->GmDelGoods(MallMgr::Instance()->TransUidGMToLocal(uid)))
		{
			GameErrorLogStream("Console") << "DelMallConfigHandler failed, can not find mall item(" << uid << ")!" << LogStream::eos;

			std::string errormsg("can not find mall item ");
			errormsg.append(Avalon::StringUtil::ConvertToString<UInt32>(uid));
			body["msg"] = json::String(errormsg);
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}

	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void QueryActConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Array jsonArray;

	WSActivityMgr::Instance()->VisitOpActivity([&jsonArray](const OpActivityRegInfo& opAct){
		json::Object active;
		active["g"] = json::Number(opAct.dataId);
		active["tmp"] = json::Number(opAct.tmpType);
		active["state"] = json::Number(opAct.state);
		active["n"] = json::String(opAct.name);
		active["pt"] = json::Number(opAct.prepareTime);
		active["st"] = json::Number(opAct.startTime);
		active["et"] = json::Number(opAct.endTime);

		jsonArray.Insert(active);

		return true;
	});

	json::Object body;
	body["activities"] = jsonArray;
	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void SetActConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	json::Array actives;
	UInt32 totalNum = 0;

	try
	{
		std::string activeStr = (json::String)jsonobj["activities"];
		if (activeStr.empty())
		{
			GameErrorLogStream("Console") << "SetActConfigHandler failed: activities are empty!" << LogStream::eos;
		}
		else
		{
			std::istringstream is(activeStr);
			json::Reader::Read(actives, is);
		}

		totalNum = (json::Number)jsonobj["count"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SetActConfigHandler failed, read activeStr failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (actives.Empty())
	{
		GameErrorLogStream("Console") << "SetActConfigHandler failed: activities are empty!" << LogStream::eos;
	}

	//UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	json::Array::const_iterator itr = actives.Begin();
	for (; itr != actives.End(); ++itr)
	{
		json::Object active = *itr;
		OpActivityRegInfo activeInfo;
		
		try
		{
			activeInfo.dataId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["g"]);
			if (activeInfo.dataId == 0)
			{
				GameErrorLogStream("Console") << "SetActConfigHandler failed: uid is zero!" << LogStream::eos;

				body["msg"] = json::String("uid error!");
				WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
				return;
			}

			activeInfo.name = (json::String)active["n"];
			activeInfo.tag = Avalon::StringUtil::ConvertToValue<UInt8>((json::String)active["t"]);
			activeInfo.tagEndTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["tt"]);
			activeInfo.startTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["st"]);
			activeInfo.endTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["et"]);
			activeInfo.circleType = (Avalon::StringUtil::ConvertToValue<UInt8>((json::String)active["lt"])) - 1;
			activeInfo.ruleDesc = (json::String)active["r"];
			activeInfo.tmpType = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["tmp"]);

			switch ((OpActivityType)activeInfo.tmpType)
			{
			case OAT_GAMBING:
			case OAT_BET_HORSE:
			case OAT_JAR_DRAW_LOTTERY:
			case OAT_ARTIFACT_JAR_SHOW:
			case OAT_WEEK_SIGN_ACTIVITY:
			case OAT_WHOLE_BARGAIN_DISCOUNT:
				continue;

			default:
				break;
			}

			activeInfo.parm = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["param"]);
			activeInfo.taskDesc = (json::String)active["d"];
			activeInfo.logoDesc = (json::String)active["lgdesc"];
			activeInfo.prepareTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["pt"]);
			activeInfo.playerLevelLimit = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)active["pl"]);
			activeInfo.minLevel = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)active["dlmin"]);
			activeInfo.maxLevel = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)active["dlmax"]);
			activeInfo.desc = (json::String)active["desc"];
			activeInfo.tabId = (json::Number)active["tabid"];
			activeInfo.countParam = (json::String)active["countParam"];

			if (activeInfo.tabId == 0)
			{
				GameErrorLogStream("Console") << "SetActConfigHandler, opAct(" << activeInfo.dataId << ")'s tabid is zero!" << LogStream::eos;
			}

			{
				std::string parm2Str = (json::String)active["param2"];
				std::vector<std::string> strVec;
				Avalon::StringUtil::Split(parm2Str, strVec, "|");
				for (auto elem : strVec)
				{
					activeInfo.parm2.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
				}
			}

			std::string taskStr = (json::String)active["task"];
			if (!taskStr.empty())
			{
				std::istringstream is(taskStr);
				json::Array tasks;
				json::Reader::Read(tasks, is);

				json::Array::const_iterator itrt = tasks.Begin();
				for (; itrt != tasks.End(); ++itrt)
				{
					json::Object taskObj = *itrt;
					OpActTaskData task;
					task.dataId = activeInfo.dataId * OPACT_ID_TRANS_VALUE + Avalon::StringUtil::ConvertToValue<UInt32>((json::String)taskObj["g"]);
					task.completeNum = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)taskObj["c"]);

					std::string rewardStr = (json::String)taskObj["b"];
					if (!WSActivityMgr::Instance()->GMDecodeRewardItems(task.rewards, rewardStr))
					{
						GameErrorLogStream("Console") << "SetActConfigHandler failed: decode opAct(" << activeInfo.dataId << ")'s reward faield!" << LogStream::eos;

						body["msg"] = json::String("decode b error!");
						WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
						return;
					}

					{
						json::Array taskVars = (json::Array)taskObj["v"];
						json::Array::const_iterator itrv = taskVars.Begin();
						for (; itrv != taskVars.End(); ++itrv)
						{
							std::string str = (json::String)(*itrv);
							task.variables.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
						}
					}

					if (taskObj.Find("v2") != taskObj.End())
					{
						json::Array taskVars = (json::Array)taskObj["v2"];
						json::Array::const_iterator itrv = taskVars.Begin();
						for (; itrv != taskVars.End(); ++itrv)
						{
							std::string str = (json::String)(*itrv);
							task.variables2.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
						}
					}

					if (taskObj.Find("adt") != taskObj.End())
					{
						auto accountDailyTimes = (json::String)taskObj["adt"];
						task.accountDailySubmitLimit = Avalon::StringUtil::ConvertToValue<UInt32>(accountDailyTimes);
					}

					if (taskObj.Find("att") != taskObj.End())
					{
						auto accountTotalTimes = (json::String)taskObj["att"];
						task.accountTotalSubmitLimit = Avalon::StringUtil::ConvertToValue<UInt32>(accountTotalTimes);
					}

					activeInfo.tasks.push_back(task);
				}
			}
		}
		catch (const json::Exception& e)
		{
			GameErrorLogStream("Console") << "SetActConfigHandler failed, read active json array failed, error msg:" << e.what() << "!" << LogStream::eos;

			body["msg"] = json::String("decode json failed!");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}

		activeInfo.dataSource = OADS_FROM_GM;
		WSActivityMgr::Instance()->UpdateOpActData(activeInfo);
	}

	if (!WSActivityMgr::Instance()->UpdateActivityTab())
	{
		GameErrorLogStream("Console") << "SetActConfigHandler failed, update activity tab failed!" << LogStream::eos;

		body["msg"] = json::String("error tabid!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	WSActivityMgr::Instance()->OnCheckGmOpActLoadOver(totalNum);

	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void DelActConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	std::string uidStr;
	try
	{
		uidStr = (json::String)jsonobj["g"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "DelActConfigHandler failed, read uidStr failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("uid invalid!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (uidStr.empty())
	{
		GameErrorLogStream("Console") << "DelActConfigHandler failed: uids are empty!" << LogStream::eos;

		body["msg"] = json::String("the param g, which value is empty!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	std::vector<std::string> uidStrs;
	Avalon::StringUtil::Split(uidStr, uidStrs, ",");
	for (auto elem : uidStrs)
	{
		UInt32 uid = Avalon::StringUtil::ConvertToValue<UInt32>(elem);
		if (!WSActivityMgr::Instance()->DeleteOpActData(uid))
		{
			GameErrorLogStream("Console") << "DelActConfigHandler failed, can not find opActivity(" << uid << ")!" << LogStream::eos;

			std::string errormsg("can not find activity ");
			errormsg.append(Avalon::StringUtil::ConvertToString<UInt32>(uid));
			body["msg"] = json::String(errormsg);
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}

	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void SendBindPointHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;

	std::string title;
	std::string sender;
	std::string content;
	std::string filename;
	try
	{
		title = (json::String)jsonobj["title"];
		sender = (json::String)jsonobj["sender"];
		content = (json::String)jsonobj["content"];
		filename = (json::String)jsonobj["filename"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SendBindPointHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	FILE* fp = fopen(filename.c_str(), "r");
	if (!fp)
	{
		body["error"] = (json::Boolean)false;
		body["msg"] = json::String("read file failed!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	while (!feof(fp))
	{
		char line[1024] = "";
		fgets(line, sizeof(line), fp);
		std::vector<std::string> params;
		Avalon::StringUtil::Split(line, params, ",");
		if (params.size() != 2)
		{
			ErrorLogStream << "invalid line:" << line << LogStream::eos;
			continue;
		}

		ObjID_t roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(params[0]);
		UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);

		std::vector<ItemReward> rewards;
		if (num <= 100)
		{
			rewards.push_back(ItemReward(600000007, 30000, 1, 0));
			rewards.push_back(ItemReward(300000106, 300, 1, 0));
		}
		else if (num <= 200)
		{
			rewards.push_back(ItemReward(BIND_POINT_ITEM_ID, 500, 1, 0));
		}
		else if (num <= 300)
		{
			rewards.push_back(ItemReward(BIND_POINT_ITEM_ID, 1000, 1, 0));
		}
		else if (num <= 400)
		{
			rewards.push_back(ItemReward(BIND_POINT_ITEM_ID, 1500, 1, 0));
		}
		else if (num <= 500)
		{
			rewards.push_back(ItemReward(BIND_POINT_ITEM_ID, 2000, 1, 0));
		}
		else
		{
			ErrorLogStream << "player(" << roleId << ") invalid num(" << num << ")." << LogStream::eos;
			continue;
		}

		MailBox::SendSysMail(roleId, sender, title, content, GetReason(SOURCE_TYPE_GM), rewards);
	}

	fclose(fp);

	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void SendPointHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;

	std::string title;
	std::string sender;
	std::string content;
	std::string filename;
	try
	{
		title = (json::String)jsonobj["title"];
		sender = (json::String)jsonobj["sender"];
		content = (json::String)jsonobj["content"];
		filename = (json::String)jsonobj["filename"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SendPointHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	FILE* fp = fopen(filename.c_str(), "r");
	if (!fp)
	{
		body["error"] = (json::Boolean)false;
		body["msg"] = json::String("read file failed!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	while (!feof(fp))
	{
		char line[1024] = "";
		fgets(line, sizeof(line), fp);
		std::vector<std::string> params;
		Avalon::StringUtil::Split(line, params, ",");
		if (params.size() != 2)
		{
			continue;
		}

		ObjID_t roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(params[0]);
		UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);

		std::vector<ItemReward> rewards;
		rewards.push_back(ItemReward(ITEM_POINT, num, 1, 0));
		MailBox::SendSysMail(roleId, sender, title, content, GetReason(SOURCE_TYPE_GM), rewards);
	}

	fclose(fp);

	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

class QueryChangeNameCb : public CLRecordCallback
{
public:
	QueryChangeNameCb(UInt32 connid, std::string cmd, std::string subCmd)
	{
		m_connid = connid;
		m_cmd = cmd;
		m_subCmd = subCmd;
	}

	void OnFinished()
	{
		json::Object body;
		json::Array records;
		while (NextRow())
		{
			json::Object record;
			std::string  name = GetData("name").GetString();
			UInt32 time = GetData("time");
			UInt64 guid = GetKey();

			record["roleid"] = json::String(Avalon::StringUtil::ConvertToString<ObjID_t>(guid));
			record["name"] = json::String(name);
			record["time"] = json::Number(time);
			records.Insert(record);
		}

		body["records"] = records;
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(m_connid,m_cmd, m_subCmd, body);
	}

	void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("no record!");
		WSConsoleAgent::Instance()->SendResponse(m_connid, m_cmd, m_subCmd, body);
	}

private:
	UInt32 m_connid;
	std::string m_cmd;
	std::string m_subCmd;
};

void QueryPlayerChangeNameRecordHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt64 roleid = 0;
	try
	{
		roleid = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["roleid"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryPlayerChangeNameRecordHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	// 查询记录
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info_name");
	cmd->PutCond("guid", roleid);
	CLRecordClient::Instance()->SendCommand(cmd, new QueryChangeNameCb(connid, GetCmd(), GetSubCmd()));
}

class SelChargeRankCb : public CLRecordCallback
{
public:
	SelChargeRankCb(UInt32 connid, std::string cmd, std::string subCmd){
		m_connid = connid;
		m_cmd = cmd;
		m_subCmd = subCmd;
	}

	void OnFinished()
	{
		json::Object body;
		json::Array records;
		while (NextRow())
		{
			json::Object record;
			UInt64 roleId = GetKey();
			std::string  name = GetData("name").GetString();
			UInt32 accId = GetData("accid");
			UInt32 openId = GetData("openid");
			UInt32 totChargeNum = GetData("totlechargenum");

			record["accid"] = json::Number(accId);
			record["name"] = json::String(name);
			record["roleid"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(roleId));
			record["openid"] = json::Number(openId);
			record["charge_money"] = json::Number(totChargeNum);
			records.Insert(record);
		}

		body["records"] = records;
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(m_connid, m_cmd, m_subCmd, body);
	}

	void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("no record!");
		WSConsoleAgent::Instance()->SendResponse(m_connid, m_cmd, m_subCmd, body);
	}

private:
	UInt32 m_connid;
	std::string m_cmd;
	std::string m_subCmd;
};

void QueryChargeRankHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 pageId = 0;
	UInt32 pageSz = 0;
	std::string pf;

	try
	{
		pageId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["page_id"]);
		pageSz = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["page_size"]);
		pf = (json::String)jsonobj["pf"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryChargeRankHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}
	
	if (0 == pageId)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("pageid is 0!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selCmd->PutColumn("accid");
	selCmd->PutColumn("name");
	selCmd->PutColumn("openid");
	selCmd->PutColumn("totlechargenum");

	std::ostringstream oss;
	oss << " ORDER BY totlechargenum DESC limit " << (pageId - 1) * pageSz << ",100";
	selCmd->SetLimit(oss.str());

	SelChargeRankCb* cb = new SelChargeRankCb(connid, GetCmd(), GetSubCmd());
	CLRecordClient::Instance()->SendCommand(selCmd, cb);
}

class SelMailInfoCb : public CLRecordCallback
{
public:
	SelMailInfoCb(UInt32 connid, std::string cmd, std::string subCmd, UInt32 curPage, UInt32 maxPageNum){
		m_connid = connid;
		m_cmd = cmd;
		m_subCmd = subCmd;
		m_maxPageNum = maxPageNum;
		m_curPage = curPage;
	}

	void OnFinished()
	{
		json::Object body;
		json::Array mails;

		while (NextRow())
		{
			json::Object mail;

			std::string title = GetData("title").GetString();
			std::string content = GetData("content").GetString();
			std::string items = GetData("items").GetString();

			json::Array enclosures;

			std::istringstream stream(items);
			char split = 0;
			while (stream)
			{
				json::Object enclosure;
				ItemReward item;
				item.DecodeString(stream);
				if (item.id == 0) break;
				stream >> split;

				enclosure["itemid"] = json::Number(item.id);
				enclosure["itemnum"] = json::Number(item.num);
				enclosures.Insert(enclosure);
			}

			mail["title"] = json::String(title);
			mail["content"] = json::String(content);
			mail["enclosure"] = enclosures;
			
			mails.Insert(mail);
		}

		body["mails"] = mails;
		body["page_id"] = json::Number(m_curPage);
		body["total_num"] = json::Number(m_maxPageNum);
		body["error"] = json::Boolean(false);
		WSConsoleAgent::Instance()->SendResponse(m_connid, m_cmd, m_subCmd, body);
	}

	void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("no record!");
		WSConsoleAgent::Instance()->SendResponse(m_connid, m_cmd, m_subCmd, body);
	}

private:
	UInt32 m_connid;
	std::string m_cmd;
	std::string m_subCmd;
	UInt32	m_maxPageNum;
	UInt32	m_curPage;
};

class SelMailCountCb : public CLRecordCallback
{
public:
	SelMailCountCb(UInt32 connid, std::string cmd, std::string subCmd, UInt32 pageId, UInt32 pageSz, UInt64 roleId){
		m_connid = connid;
		m_cmd = cmd;
		m_subCmd = subCmd;
		m_pageId = pageId;
		m_pageSz = pageSz;
		m_roleId = roleId;
	}

	void OnFinished()
	{
		if (NextRow())
		{
			if (0 == m_pageId)
			{
				return;
			}

			UInt32 total = GetData("count(*)");
			
			CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_mail", m_roleId, WSApplication::Instance()->GetTableCount());
			selCmd->PutColumn("title");
			selCmd->PutColumn("content");
			selCmd->PutColumn("items");
			selCmd->PutCond("owner", m_roleId);

			std::ostringstream oss;
			oss << " limit " << (m_pageId - 1) * m_pageSz << "," << m_pageSz;
			selCmd->SetLimit(oss.str());

			SelMailInfoCb* cb = new SelMailInfoCb(m_connid, m_cmd, m_subCmd, m_pageId, total);
			CLRecordClient::Instance()->SendCommand(selCmd, cb, m_roleId);
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("no record!");
		WSConsoleAgent::Instance()->SendResponse(m_connid, m_cmd, m_subCmd, body);
	}

private:
	UInt32 m_connid;
	std::string m_cmd;
	std::string m_subCmd;
	UInt32 m_pageId;
	UInt32 m_pageSz;
	UInt64 m_roleId;
};

void QueryMailHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt64 roleId = 0;
	UInt32 pageId = 0;
	UInt32 pageSz = 0;

	try
	{
		roleId = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["roleid"]);
		pageId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["page_id"]);
		pageSz = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["page_size"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryChargeRankHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (0 == pageId)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("pageid is 0!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	CLSelectCommand* selCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_mail", roleId, WSApplication::Instance()->GetTableCount());
	selCmd->PutColumn("count(*)");
	selCmd->PutCond("owner", roleId);

	SelMailCountCb* cb = new SelMailCountCb(connid, GetCmd(), GetSubCmd(), pageId, pageSz, roleId);
	CLRecordClient::Instance()->SendCommand(selCmd, cb, roleId);
}

void QueryPlayerChangeNameRecordByNameHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string name;
	try
	{
		name = (json::String)jsonobj["name"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryPlayerChangeNameRecordByNameHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	// 查询记录
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info_name");
	cmd->PutCond("name", name);
	CLRecordClient::Instance()->SendCommand(cmd, new QueryChangeNameCb(connid, GetCmd(), GetSubCmd()));
}

void QueryGuildHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt64		guildId;
	std::string name;
	try
	{
		name = (json::String)jsonobj["name"];
		guildId = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["id"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryGuildHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	Guild* guild = NULL;
	if (guildId > 0)
	{
		guild = GuildMgr::Instance()->FindGuildByID(guildId);
	}
	else
	{
		guild = GuildMgr::Instance()->FindGuildByName(name);
	}

	json::Object body;
	body["error"] = json::Boolean(false);

	json::Array guildList;
	if (guild)
	{
		auto leader = guild->GetLeader();
		json::Object info;
		info["id"] = json::String(Avalon::StringUtil::ConvertToString(guild->GetID()));
		info["name"] = json::String(guild->GetName());
		info["leader_id"] = json::String(Avalon::StringUtil::ConvertToString((leader == NULL ? 0 : leader->GetID())));
		info["leader_name"] = json::String(leader == NULL ? "" : leader->GetName());
		info["member_num"] = json::Number(guild->GetMemberNum());

		guildList.Insert(info);
	}

	body["guild_list"] = guildList;

	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void DismissGuildHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt64		guildId;
	try
	{
		guildId = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["id"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "QueryGuildHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	json::Object body;
	Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
	if (!guild)
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("guild not exist.");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (!GuildBattleMgr::Instance()->Dismiss(guildId))
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("in guild battle.");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	guild->SetDismissTime((UInt32)CURRENT_TIME.Sec());
	
	body["error"] = json::Boolean(false);
	body["msg"] = json::String("");
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void GuildAddFundHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt64		guildId;
	UInt32		fund;
	try
	{
		guildId = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["id"]);
		fund = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["fund"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "GuildAddFundHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	json::Object body;
	Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
	if (!guild)
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("guild not exist.");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	guild->AddFund(fund);
	body["error"] = json::Boolean(false);
	body["msg"] = json::String("");
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void ClearTowerDataHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::vector<ObjID_t> roleids;
	std::vector<UInt32> accids;
	try
	{
		const json::String& roleidstr = jsonobj["roleid"];
		const json::String& accidStr = jsonobj["accid"];
		std::vector<std::string> roleidVec;
		Avalon::StringUtil::Split(roleidstr.Value(), roleidVec, ",");
		for (std::vector<std::string>::iterator iter = roleidVec.begin(); iter != roleidVec.end(); ++iter)
		{
			ObjID_t roleid = Avalon::StringUtil::ConvertToValue<ObjID_t>(*iter);
			roleids.push_back(roleid);
		}

		std::vector<std::string> accidVec;
		Avalon::StringUtil::Split(accidStr.Value(), accidVec, ",");
		for (std::vector<std::string>::iterator iter = accidVec.begin(); iter != accidVec.end(); ++iter)
		{
			UInt32 accId = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
			accids.push_back(accId);
		}
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ClearTowerDataHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	if (roleids.size() > 100 || accids.size() > 100)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("role or accid size max 100!");
		WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	for (UInt32 i = 0; i < accids.size(); ++i)
	{
		UInt32 serverId = 0;
		CLProtocol::SceneConsoleControlPlayer req;
		req.nodeid = connid;
		req.playerId = 0;
		req.accId = accids[i];
		req.subcmd = GetSubCmd();

		if (serverId == 0)
		{
			serverId = WSNetwork::Instance()->SelectRandSceneServer();
		}

		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
		if (conn != NULL)
		{
			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Can Not Find SceneServer!");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}

	for (UInt32 i = 0; i < roleids.size(); ++i)
	{
		UInt32 serverId = 0;
		if (roleids[i] != 0)
		{
			serverId = WSPlayerMgr::Instance()->GetPlayerSceneServerID(roleids[i]);
		}
		CLProtocol::SceneConsoleControlPlayer req;
		req.nodeid = connid;
		req.playerId = roleids[i];
		req.accId = 0;
		req.subcmd = GetSubCmd();

		if (serverId == 0)
		{
			serverId = WSNetwork::Instance()->SelectRandSceneServer();
		}

		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
		if (conn != NULL)
		{
			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("Can Not Find SceneServer!");
			WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}

	json::Object body;
	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void SendMailBatchHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	struct SendInfo
	{
		UInt64 roleUid;
		std::vector<std::vector<ItemReward>> rewardGroups;
	};

	std::string title;
	std::string content;
	std::string pf;
	std::vector<SendInfo> sendInfos;
	std::string reason = GetReason(SOURCE_TYPE_GM);

	try
	{
		pf = (json::String)jsonobj["pf"];
		title = (json::String)jsonobj["title"];
		content = (json::String)jsonobj["content"];
		
		json::Array sendInfoArray = (json::Array)jsonobj["sendinfos"];
		
		json::Array::iterator itr = sendInfoArray.Begin();
		for (; itr != sendInfoArray.End(); ++itr)
		{
			SendInfo sendInfo;
			json::Object sendInfoObj = (json::Object)(*itr);
			sendInfo.roleUid = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)sendInfoObj["roleid"]);
			std::string itemInfoStr = (json::String)sendInfoObj["items"];

			std::vector<ItemReward>	rewards;
			UInt32 itemid = 0;
			UInt32 num = 0;
			char split = 0;
			std::istringstream stream1(itemInfoStr);
			UInt32 itemnum = 0;
			while (stream1)
			{
				if (++itemnum > MAX_MAILITEM_NUM)
				{
					itemnum = 1;
					sendInfo.rewardGroups.push_back(rewards);
					rewards.clear();
				}

				stream1 >> itemid;
				if (itemid == 0) break;
				stream1 >> split >> num >> split;
				ItemReward reward;
				reward.id = itemid;
				reward.qualityLv = 0;
				reward.num = num;
				rewards.push_back(reward);
			}

			if (!rewards.empty())
			{
				sendInfo.rewardGroups.push_back(rewards);
				rewards.clear();
			}

			sendInfos.push_back(sendInfo);
		}
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "BroadcastMailHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	std::string senderName = "GM";
	for (std::vector<SendInfo>::iterator iter = sendInfos.begin();
		iter != sendInfos.end(); ++iter)
	{
		std::vector<std::vector<ItemReward>>& rewardGroups = iter->rewardGroups;

		if (rewardGroups.empty())
		{
			UInt32 ckRet = CheckMail(0, title, content, 0, senderName);
			if (ckRet != ErrorCode::SUCCESS)
			{
				json::Object body;
				body["error"] = json::Boolean(true);
				std::ostringstream msg;
				msg << "check mail info error! code = " << ckRet << " roleid = " << iter->roleUid;
				body["msg"] = json::String(msg.str());
				WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
				return;
			}
			else
			{
				MailBox::SendSysMail(iter->roleUid, senderName, title, content, reason, std::vector<ItemReward>());
			}
		}
		else
		{
			for (std::vector<std::vector<ItemReward> >::iterator rewardIter = rewardGroups.begin();
				rewardIter != rewardGroups.end(); ++rewardIter)
			{
				std::vector<ItemReward>& rewards = *rewardIter;
				
				UInt32 ckRet = CheckMail(0, title, content, rewards.size(), senderName);
				if (ckRet != ErrorCode::SUCCESS)
				{
					json::Object body;
					body["error"] = json::Boolean(true);
					std::ostringstream msg;
					msg << "check mail info error! code = " << ckRet << " roleid = " << iter->roleUid;
					body["msg"] = json::String(msg.str());
					WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
					return;
				}
				else
				{
					MailBox::SendSysMail(iter->roleUid, senderName, title, content, reason, rewards);
				}
			}
		}
	}

	json::Object body;
	body["error"] = json::Boolean(false);
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void CDKDataHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 accid = 0;
	ObjID_t roleId = 0;
	std::string cdkey;
	std::string platform;

	try
	{
		const json::String& accidstr = jsonobj["accid"];
		accid = Avalon::StringUtil::ConvertToValue<UInt32>(accidstr.Value());
		const json::String& roleidstr = jsonobj["roleid"];
		roleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(roleidstr.Value());
		cdkey = (json::String)jsonobj["cdkey"];
		platform = (json::String)jsonobj["pf"];
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ViVoCDKDataHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	CLProtocol::WCCDKVerifyReq centerReq;
	centerReq.accId = accid;
	centerReq.playerUid = roleId;
	centerReq.platform = platform;
	centerReq.serverId = ZONE_ID;
	centerReq.cdk = cdkey;
	centerReq.connId = connid;
	centerReq.queryTime = CURRENT_TIME.MSec();

	if (platform == "oppo")
	{
		centerReq.reason = SOURCE_OPPO_CDK;
	}
	else
	{
		GameErrorLogStream("Console") << "ViVoCDKDataHandler failed: invalid platform:" << platform << LogStream::eos;
		return;
	}
	
	

	Router::SendToCenter(centerReq);
}

void ClearSecurityLockHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 accId = 0;
	try
	{
		accId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accid"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ClearSecurityLockHandler failed:" << e.what() << "!" <<" accid:"<<accId<< LogStream::eos;
		return;
	}

	json::Object body;
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(accId);
	if (player != NULL)
	{
		player->ClearSecurityLockNotifyClient();
	}
	else
	{
		std::ostringstream cond;
		cond << "accid=" << accId;
		auto sqlCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", cond.str());
		sqlCmd->PutData("security_passwd", "");
		sqlCmd->PutData("security_freeze_time", 0);
		sqlCmd->PutData("security_unfreeze_time", 0);
		sqlCmd->PutData("common_device", "");
		sqlCmd->PutData("security_passwd_error_num", 0);
		CLRecordClient::Instance()->SendCommand(sqlCmd);
	}

	body["error"] = json::Boolean(false);
	body["msg"] = json::String("success");
	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void AbnormalTransQueryHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	TransmitToScene(connid, jsonobj);
}

void AbnormalTransUnfreezeHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	TransmitToScene(connid, jsonobj);
}

void AuctionSetAveraPriceHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	std::string priceItems;
	try
	{
		const json::String& priceItemss = jsonobj["priceItems"];
		priceItems = priceItemss.Value();
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "AuctionSetAveraPriceHandler failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	std::vector<std::string> errItems;

	std::vector<std::string> items;
	Avalon::StringUtil::Split(priceItems, items, "|");
	for (std::vector<std::string>::iterator iter = items.begin();
		iter != items.end(); ++iter)
	{
		std::vector<std::string> values;
		Avalon::StringUtil::Split(*iter, values, ",");
		if (values.size() < 6)
		{
			ErrorLogStream << "values.size() < 6  str : " << iter->c_str() << LogStream::eos;
			continue;
		}

		UInt32 itemTypeId = Avalon::StringUtil::ConvertToValue<UInt32>(values[0]);
		UInt8  enhanceType = Avalon::StringUtil::ConvertToValue<UInt8>(values[1]);
		UInt32 strengthen = Avalon::StringUtil::ConvertToValue<UInt32>(values[2]);
		UInt32 beadBuffId = Avalon::StringUtil::ConvertToValue<UInt32>(values[3]);
		bool setAverPrice = true;
		bool setAverVisPrice = true;
		Int32 averPrice = Avalon::StringUtil::ConvertToValue<Int32>(values[4]);
		Int32 averVisPrice = Avalon::StringUtil::ConvertToValue<Int32>(values[5]);

		if (averPrice < 0)
		{
			setAverPrice = false;
		}
		if (averVisPrice < 0)
		{
			setAverVisPrice = false;
		}
		UInt32 transNum = Avalon::StringUtil::ConvertToValue<UInt32>(values[5]);

		bool res = zjy_auction::AuctionMgr::Instance()->SetAverPriceGM(itemTypeId, strengthen, beadBuffId, enhanceType, setAverPrice, averPrice, setAverVisPrice, averVisPrice, transNum);
		if (!res)
		{
			std::string ss = Avalon::StringUtil::ConvertToString(itemTypeId);
			ss += ",";
			ss += strengthen;
			errItems.push_back(ss);
		}

	}

	json::Object body;

	if (errItems.empty())
	{
		body["error"] = json::Boolean(false);
		body["msg"] = json::String("success");
	}
	else
	{
		std::string errItemss;
		for (auto& item : errItems)
		{
			errItemss += item;
			errItemss += "|";
		}
		body["error"] = json::Boolean(true);
		body["msg"] = json::String(errItemss.c_str());
	}


	WSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void AccounterSetCounterSHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	class SelectAccCntCallback : public CLRecordCallback
	{
	public:
		SelectAccCntCallback(UInt32 connid, UInt32 accid, UInt32 type, UInt64 cnt): m_ConnId(connid), m_Accid(accid), m_Type(type), m_Cnt(cnt) {}

		void OnFinished()
		{
			json::Object body;

			if (!NextRow())
			{
				body["error"] = json::Boolean(false);
				body["msg"] = json::String("not has record!");
			}
			else
			{
				UInt64 num = GetData("counter_num");

				CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_counter", GetKey(), true);
				cmd->PutData("counter_num", m_Cnt + num);
				cmd->PutData("refresh_time", CURRENT_TIME.Sec());
				CLRecordClient::Instance()->SendCommand(cmd);

				InfoLogStream << "Account Counter GM Accid:" << m_Accid << " Type:" << m_Type << " OldNum:" << num << " AddNum:" << m_Cnt << LogStream::eos;

				body["error"] = json::Boolean(false);
				body["msg"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(num + m_Cnt));
			}

			WSConsoleAgent::Instance()->SendResponse(m_ConnId, "accCnt", "setAccCnt", body);
		}

		void OnFailed(UInt32 errorcode)
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			body["msg"] = json::String("database error");
			WSConsoleAgent::Instance()->SendResponse(m_ConnId, "accCnt", "setAccCnt", body);
		}

	private:
		UInt32 m_ConnId;
		UInt32 m_Accid;
		UInt32 m_Type;
		UInt64 m_Cnt;
	};

	UInt32 accId = 0;
	UInt32 type = 0;
	UInt64 cnt = 0;
	try
	{
		accId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accid"]);
		type = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["type"]);
		cnt = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["cnt"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "AccounterSetCounterSHandler failed:" << e.what() << "!" <<
			" accid:" << accId << " type:" << type << " cnt:"<< cnt <<LogStream::eos;
		return;
	}

	json::Object body;
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(accId);
	if (player != NULL)
	{
		UInt32 num = player->GetAccountInfo()->GetAccountCounterNum(AccountCounterType(type));
		player->GetAccountInfo()->SetAccountCounterNum(player, AccountCounterType(type), cnt + num);
		if (type == ACC_COUNTER_ENERGY_COIN)
		{
			player->SyncAccountCounterToScene(ACC_COUNTER_ENERGY_COIN);
		}

		InfoLogStream << "Account Counter GM Accid:"<< accId <<" Type:"<< type <<" OldNum:"<< num <<" AddNum:"<< cnt << LogStream::eos;

		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(cnt + num));
		WSConsoleAgent::Instance()->SendResponse(connid, "accCnt", "setAccCnt", body);
	}
	else
	{
		auto sqlCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_counter");
		sqlCmd->PutCond("owner", accId);
		sqlCmd->PutCond("counter_type", type);
		sqlCmd->PutColumn("counter_num");
		CLRecordClient::Instance()->SendCommand(sqlCmd, new SelectAccCntCallback(connid, accId, type, cnt));
	}
}

void SetPlayerGMAuthoriyHandler::operator()(UInt32 nodeid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	try
	{
		UInt32 accid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accid"]);
		UInt32 flag = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["flag"]);
		//UInt32 activeStr = (UInt32)(json::Number)jsonobj["flag"];
		UInt8 auth = 0;
		if (flag > 0)
		{
			auth = 60;
		}

		auto account = AccountMgr::Instance()->FindAccount(accid);
		if (!account)
		{
			GameErrorLogStream("Console") << "SetPlayerGMAuthoriyHandler failed, can't find account:" << accid << "!" << LogStream::eos;

			std::ostringstream oss;
			oss << "can't find account:" << accid;
			body["msg"] = json::String(oss.str());
			WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
			return;
		}

		account->SetGMAuthority(auth);

		auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(accid);
		if (player)
		{
			CLProtocol::WorldSyncPlayerGMAuthority protocol;
			protocol.roleid = player->GetID();
			protocol.gmAuth = auth;
			player->SendToScene(protocol);
		}

		body["error"] = json::Boolean(false);
		GameInfoLogStream("Console") << "SetPlayerGMAuthoriyHandler: player(" << accid << ")'s gm authority(" << (UInt32)auth << ")." << LogStream::eos;
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SetActConfigHandler failed, read activeStr failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		return;
	}

	WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
}


class GMIncreaseGnomeCoinSelectCallback : public CLRecordCallback
{
public:
	GMIncreaseGnomeCoinSelectCallback(UInt32 nodeid, const std::string& cmd, const std::string subCmd, UInt32 accid, UInt32 incNum)
		: m_NodeId(nodeid), m_Cmd(cmd), m_SubCmd(subCmd), m_AccId(accid), m_IncNum(incNum)
	{

	}

	virtual void OnFinished()
	{
		json::Object body;
		body["error"] = json::Boolean(false);

		auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(m_AccId);
		if (player)
		{
			player->IncGnomeCoinNum(GetReason(SOURCE_TYPE_GM), m_IncNum, false);
			WSConsoleAgent::Instance()->SendResponse(m_NodeId, m_Cmd, m_SubCmd, body);
		}
		else
		{
			auto account = AccountMgr::Instance()->FindAccount(m_AccId);
			if (account)
			{
				UInt32 oldNum = account->GetGnomeCoinNum();
				account->SetGnomeCoinNum(account->GetGnomeCoinNum() + m_IncNum);
				WSConsoleAgent::Instance()->SendResponse(m_NodeId, m_Cmd, m_SubCmd, body);
				GameInfoLogStream("Console") << "IncGnomeCoinHandler success, account(" << m_AccId << ") increase gnome coin(" << m_IncNum
					<< "), old num=" << oldNum << ", new num=" << account->GetGnomeCoinNum() << LogStream::eos;
			}
			else
			{
				if (!NextRow())
				{
					body["error"] = json::Boolean(true);
					std::ostringstream oss;
					oss << "inexistent account:" << m_AccId;
					body["msg"] = json::String(oss.str());
					WSConsoleAgent::Instance()->SendResponse(m_NodeId, m_Cmd, m_SubCmd, body);
					return;
				}

				UInt32 oldNum = GetData("gnome_coin_num");
				UInt32 newNum = oldNum + m_IncNum;
				std::ostringstream condOss;
				condOss << "accid=" << m_AccId;
				auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", condOss.str());
				cmd->PutData("gnome_coin_num", newNum);
				CLRecordClient::Instance()->SendCommand(cmd);
				WSConsoleAgent::Instance()->SendResponse(m_NodeId, m_Cmd, m_SubCmd, body);
				GameInfoLogStream("Console") << "IncGnomeCoinHandler success, account(" << m_AccId << ") increase gnome coin(" << m_IncNum
					<< "), old num=" << oldNum << ", new num=" << newNum << LogStream::eos;
			}
		}
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("select table failed");
		WSConsoleAgent::Instance()->SendResponse(m_NodeId, m_Cmd, m_SubCmd, body);
	}

private:
	UInt32 m_NodeId;
	std::string m_Cmd;
	std::string m_SubCmd;
	UInt32 m_AccId;
	UInt32 m_IncNum;
};

void IncGnomeCoinHandler::operator()(UInt32 nodeid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(false);

	try
	{
		UInt32 accid = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accid"]);
		UInt32 incNum = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["incNum"]);

		auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(accid);
		if (player)
		{
			player->IncGnomeCoinNum(GetReason(SOURCE_TYPE_GM), incNum, false);
			WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		}
		else
		{
			auto account = AccountMgr::Instance()->FindAccount(accid);
			if (account)
			{
				UInt32 oldNum = account->GetGnomeCoinNum();
				account->SetGnomeCoinNum(account->GetGnomeCoinNum() + incNum);
				WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
				GameInfoLogStream("Console") << "IncGnomeCoinHandler success, account(" << accid << ") increase gnome coin(" << incNum
					<< "), old num=" << oldNum << ", new num=" << account->GetGnomeCoinNum() << LogStream::eos;
			}
			else
			{
				auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
				cmd->PutCond("accid", accid);
				cmd->PutColumn("gnome_coin_num");
				CLRecordClient::Instance()->SendCommand(cmd, new GMIncreaseGnomeCoinSelectCallback(nodeid, GetCmd(), GetSubCmd(), accid, incNum));
			}
		}
	}
	catch (const json::Exception& e)
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "IncGnomeCoinHandler failed, decode failed, error msg:" << e.what() << "!" << LogStream::eos;
		return;
	}
}

void CurrencyFrozenHandler::operator()(UInt32 nodeid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(false);

	try
	{
		UInt64 roleId = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["roleid"]);
		UInt64 orderId = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["orderid"]);
		UInt32 opType = Avalon::StringUtil::ConvertToValue<UInt64>((json::String)jsonobj["optype"]);

		UInt32 serverId = WSNetwork::Instance()->SelectRandSceneServer();
		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(serverId);
		if (conn != NULL)
		{
			CLProtocol::SceneConsoleCurrencyFrozen req;
			req.nodeId = nodeid;
			req.roleId = roleId;
			req.orderId = orderId;
			req.opType = opType;
			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			json::Object body;
			body["error"] = json::Boolean(true);
			WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		}
	}
	catch (const json::Exception& e)
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "HandlerCurrencyFrozenHandler failed, decode failed, error msg:" << e.what() << "!" << LogStream::eos;
		return;
	}
}

class GmAddTitleCallback : public AccountCallback
{
public:
	GmAddTitleCallback(UInt32 accid, UInt32 titleId, UInt32 dueTime)
		: AccountCallback(accid), m_titleId(titleId), m_dueTime(dueTime) {}
	~GmAddTitleCallback() {}
	void OnFinish(AccountInfo* info)
	{
		if (info)
		{
			info->GetTitleMgr().AddOneTitleByTitleId(m_titleId, m_dueTime);
		}
	}

	void OnFailure()
	{
		ErrorLogStream << "load account fail for give rewards for sortlist accid : " << m_AccId << ", titleId : " << m_titleId << ", dueTime : " << m_dueTime << LogStream::eos;
	}

private:
	UInt32 m_titleId;
	UInt32 m_dueTime;
};

void AddTitleHandler::operator()(UInt32 nodeid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(false);

	try
	{
		UInt32 accId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accId"]);
		UInt32 titleId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["titleId"]);
		UInt32 dueTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["dueTime"]);

		AccountInfo* accountInfo = AccountMgr::Instance()->FindAccount(accId);
		if (accountInfo)
		{
			accountInfo->GetTitleMgr().AddOneTitleByTitleId(titleId, dueTime);
		}
		else
		{
			AccountMgr::Instance()->LoadAccountInfo(accId, new GmAddTitleCallback(accId, titleId, dueTime));
		}
		body["msg"] = json::String("success");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
	}
	catch (const json::Exception& e)
	{
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "AddTitleHandler failed, decode failed, error msg:" << e.what() << "!" << LogStream::eos;
		return;
	}
}

class SelectAccountRemoveCreditPoint : public AccountCallback
{
public:
	SelectAccountRemoveCreditPoint(UInt32 accid, UInt32 num, UInt32 nodeId, const std::string& cmd, const std::string& subCmd)
		: AccountCallback(accid), m_Num(num), m_NodeId(nodeId), m_Cmd(cmd), m_SubCmd(subCmd) {}

	void OnFinish(AccountInfo* info)
	{
		if (NULL == info) return;

		InfoLogStream << "gm load account add credit point accid:" << m_AccId << " get credit point:" << m_Num << LogStream::eos;

		UInt32 oldNum = info->GetCreditPoint();
		info->RemoveCreditPoint(m_Num);
		UInt32 newNum = info->GetCreditPoint();
		info->SendUdpLog("asset", LOG_ASSET_FORMAT, "GM", AssetType::ASSET_CREDIT_POINT, oldNum, m_Num, newNum - oldNum, newNum);

		json::Object body;
		body["error"] = json::Boolean(false);
		body["msg"] = json::String("success");
		WSConsoleAgent::Instance()->SendResponse(m_NodeId, m_Cmd, m_SubCmd, body);
	}

	void OnFailure()
	{
		ErrorLogStream << "gm load account add credit point error accid:" << m_AccId << " get credit point:" << m_Num << LogStream::eos;
	}

private:

	UInt32 m_Num;
	UInt32 m_NodeId;
	std::string m_Cmd;
	std::string m_SubCmd;
};

void CreditPointHandler::operator()(UInt32 nodeid, json::Object& jsonobj)
{
	try
	{
		UInt32 accId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["accId"]);
		UInt32 num = Avalon::StringUtil::ConvertToValue<Int32>((json::String)jsonobj["num"]);
		if (accId == 0 || num == 0)
		{
			ErrorLogStream << "GM credit point accId:" << accId << " num:" << num << LogStream::eos;
			return;
		}

		// 有角色在线，发到Scene上操作
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(accId);
		if (player != NULL)
		{
			CLProtocol::SceneConsoleRemoveCreditPoint removeCp;
			removeCp.nodeId = nodeid;
			removeCp.onlineId = player->GetID();
			removeCp.creditPoint = num;
			removeCp.reason = "GM";
			player->SendToScene(removeCp);
			return;
		}

		// 没有角色在线，用账号数据操作
		AccountInfo* accountInfo = AccountMgr::Instance()->FindAccount(accId);
		if (accountInfo != NULL)
		{
			InfoLogStream << "gm remove credit point! accId:" << accId << " num:" << num << LogStream::eos;

			UInt32 oldNum = accountInfo->GetCreditPoint();
			accountInfo->RemoveCreditPoint(num);
			UInt32 newNum = accountInfo->GetCreditPoint();
			accountInfo->SendUdpLog("asset", LOG_ASSET_FORMAT, "GM", AssetType::ASSET_CREDIT_POINT, oldNum, num, newNum - oldNum, newNum);
		}
		else
		{
			AccountMgr::Instance()->LoadAccountInfo(accId, new SelectAccountRemoveCreditPoint(accId, num, nodeid, GetCmd(), GetSubCmd()));
		}

		json::Object body;
		body["error"] = json::Boolean(false);
		body["msg"] = json::String("success");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
	}
	catch (const json::Exception& e)
	{
		json::Object body;
		body["error"] = json::Boolean(true);
		body["msg"] = json::String("decode json failed!");
		WSConsoleAgent::Instance()->SendResponse(nodeid, GetCmd(), GetSubCmd(), body);
		GameErrorLogStream("Console") << "CreditPointHandler failed, decode failed, error msg:" << e.what() << "!" << LogStream::eos;
		return;
	}
}
