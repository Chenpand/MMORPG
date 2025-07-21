#include "CenterHandler.h"

#include "GameZoneMgr.h"
#include "PunishmentMgr.h"
#include "CSApplication.h"
#include "HttpClient.h"
#include "AvalonMd5.h"
#include <json/reader.h>
#include <json/writer.h>
#include <CLErrorCode.h>

struct CDKCtx
{
	GameZone* zone;
	ObjID_t playerUid;
	UInt32 connId;
	UInt32 reason;
	UInt64 recvTime;
	UInt64 startTime;
};

struct CustomServiceCtx
{
	UInt32	zoneid;
	ObjID_t	roleid;
};

struct ReponseBaseRouterInfo
{
	UInt32	zoneid;
	ObjID_t	roleid;
};

struct QueryConvertAccountInfo : public ReponseBaseRouterInfo
{
	UInt8	fromGate;
	UInt32	accid;
};

struct GlobalServerRouter : public ReponseBaseRouterInfo
{
	std::string platform;
	std::string openid;
};

void HandleProtocol(CLProtocol::CenterForbidTalkListReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone != NULL)
	{
		PunishmentMgr::Instance()->SendForbidList(zone);
	}
}

void HandleProtocol(CLProtocol::CenterForbidTalkReq& protocol)
{
	PunishmentMgr::Instance()->ForbitTalk(protocol.accid);
}

void HandleProtocol(CLProtocol::CenterUnForbidTalkReq& protocol)
{
	PunishmentMgr::Instance()->UnForbitTalk(protocol.accid);
}

void HandleProtocol(CLProtocol::CenterTransmitToZone& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(protocol.zoneId);
	if(zone != NULL) zone->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::CenterBroadcastToZones& protocol)
{
	GameZoneMgr::Instance()->BroadcastToGameZone(protocol.packet);
}

std::string getMd5(CLProtocol::WCCDKVerifyReq& req)
{
	std::string md5Str;
	md5Str += "channel_id=" + Avalon::StringUtil::ConvertToString(req.platform);
	md5Str += "&server_id=" + Avalon::StringUtil::ConvertToString(req.serverId);
	md5Str += "&code_key=" + Avalon::StringUtil::ConvertToString(req.cdk);
	md5Str += "&player_uid=" + Avalon::StringUtil::ConvertToString(req.playerUid);
	md5Str += "&acc_id=" + Avalon::StringUtil::ConvertToString(req.accId);
	md5Str += "&sign_key=" + CSApplication::Instance()->GetOperateInfo().cdkVerifyMD5Key;

	InfoLogStream << "cdk verify md5: " << md5Str << LogStream::eos;
	char md5Buff[33];
	Avalon::md5_encode32(md5Buff, (void*)md5Str.c_str(), md5Str.length());
	md5Buff[32] = 0;

	return std::string(md5Buff);
}

std::string getPram(CLProtocol::WCCDKVerifyReq& req)
{
	std::string keyParam;
	std::string sign = getMd5(req);
	keyParam += "channel_id=" + Avalon::StringUtil::ConvertToString(req.platform);
	keyParam += "&server_id=" + Avalon::StringUtil::ConvertToString(req.serverId);
	keyParam += "&code_key=" + Avalon::StringUtil::ConvertToString(req.cdk);
	keyParam += "&player_uid=" + Avalon::StringUtil::ConvertToString(req.playerUid);
	keyParam += "&acc_id=" + Avalon::StringUtil::ConvertToString(req.accId);
	keyParam += "&sign=" + sign;
	return keyParam;
}

Int32 GetCDKItemReward(std::string& cdkRes, std::vector<ItemReward>& outItems, std::string& outErrMsg)
{
	if (cdkRes.size() == 0)
	{
		ErrorLogStream << "decode json item failed! cdkRes size is 0:" << LogStream::eos;
		return -1;
	}

	std::string decodeCdkRes = HttpClient::getInstance()->urlUnescape(cdkRes.c_str(), cdkRes.size());

	std::istringstream iss(decodeCdkRes);
	json::Object jsonobj;

	UInt32 result = 0;
	try
	{
		json::Reader::Read(jsonobj, iss);

		const json::Number& resultNum = jsonobj["result"];
		result = resultNum.Value();
		if (result == 0)
		{
			const json::String& msg = jsonobj["msg"];
			outErrMsg = msg.Value();
			const json::Number& codeKey = jsonobj["error_code"];
			return (Int32)codeKey.Value();
		}

		const json::Object& msg = jsonobj["msg"];
		const json::String& items = msg["items"];
		
		std::vector<std::string> itemRewardStr;
		Avalon::StringUtil::Split(items.Value(), itemRewardStr, "|");

		for (size_t i = 0; i < itemRewardStr.size(); ++i)
		{
			std::vector<std::string> itemInfoStr;
			Avalon::StringUtil::Split(itemRewardStr[i], itemInfoStr, ",");
			if (itemInfoStr.size() != 2)
			{
				ErrorLogStream << "decode json item failed! struct size error:" << (UInt32)itemInfoStr.size() << LogStream::eos;
				return -1;
			}

			ItemReward item;
			item.id = Avalon::StringUtil::ConvertToValue<UInt32>(itemInfoStr[0].c_str());
			item.num = Avalon::StringUtil::ConvertToValue<UInt32>(itemInfoStr[1].c_str());
			outItems.push_back(item);
		}
		
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "decode cdk res item json string failed:" << e.what() << LogStream::eos;
		return -1;
	}

	return result;

}

void CdkVerifyRespons(HttpClient* client, HttpResponse* response)
{
	if (response)
	{
		std::string tokenXML;
		for (size_t i = 0; i < response->getResponseData()->size(); ++i)
		{
			tokenXML += response->getResponseData()->at(i);
		}

		InfoLogStream << "cdk verify http response: " << tokenXML << LogStream::eos;

		if (!response->getHttpRequest())
		{
			ErrorLogStream << "cdk verify request is null, error!" << LogStream::eos;
			return;
		}

		CDKCtx* ctx = (CDKCtx*)(response->getHttpRequest()->getUserData());
		if (!ctx)
		{
			ErrorLogStream << "cdk verify ctx is null, error!" << LogStream::eos;
			return;
		}

		if (!ctx->zone)
		{
			ErrorLogStream << "cdk verify zone is null, error!" << LogStream::eos;
			return;
		}

		auto curTime = CURRENT_TIME.MSec();
		DebugLogStream << "cdk verify used " << curTime - ctx->startTime << "(ms)" << LogStream::eos;

		std::vector<ItemReward> items;
		std::string errMsg;
		Int32 result = GetCDKItemReward(tokenXML, items, errMsg);

		//返回给scene,发送相应cdk礼包
		CLProtocol::CSCDKVerifyRes res;
		res.playerUid = ctx->playerUid;
		res.result = result;
		res.errorMsg = errMsg;
		res.itemRewards = items;
		res.connId = ctx->connId;
		res.reason = ctx->reason;
		res.queryTime = ctx->recvTime;
		ctx->zone->SendProtocol(res);

		CL_SAFE_DELETE(ctx);
	}
	else
	{
		ErrorLogStream << "cdk verify response is null, error!" << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::WCCDKVerifyReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "can't find zone by connid:" << req.GetConnID() << LogStream::eos;
		return;
	}

	std::string pram = getPram(req);

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(CSApplication::Instance()->GetOperateInfo().cdkVerifyURL.c_str());
	httpReq->setResponseCallback(CdkVerifyRespons);
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(pram.c_str(), pram.length());

	CDKCtx* ctx = new CDKCtx();
	ctx->zone = zone;
	ctx->playerUid = req.playerUid;
	ctx->connId = req.connId;
	ctx->reason = req.reason;
	ctx->startTime = CURRENT_TIME.MSec();
	ctx->recvTime = req.queryTime;
	httpReq->setUserData((void*)ctx);
	http->send(httpReq);

	InfoLogStream << "send cdk verify http request suc! pram:" << pram << LogStream::eos;
}

void QueryCustomServiceSignRespons(HttpClient* client, HttpResponse* response)
{
	auto ctx = (CustomServiceCtx*)response->getHttpRequest()->getUserData();
	if (!ctx)
	{
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(ctx->zoneid);
	if (!zone)
	{
		ErrorLogStream << "can't find zone(" << ctx->zoneid << ")." << LogStream::eos;
		return;
	}

	CLProtocol::CenterCustomServiceSignRes res;

	if (response)
	{
		for (size_t i = 0; i < response->getResponseData()->size(); ++i)
		{
			res.sign += response->getResponseData()->at(i);
		}

		if (res.sign.empty())
		{
			res.result = ErrorCode::CUSTOM_SERVICE_INVALID_PARAM;
		}
		else if (res.sign == "close")
		{
			res.result = ErrorCode::CUSTOM_SERVICE_CLOSED;
		}
		else
		{
			res.result = ErrorCode::SUCCESS;
		}
	}
	else
	{
		ErrorLogStream << "custom service agent response is null, error!" << LogStream::eos;
		res.result = ErrorCode::CUSTOM_SERVICE_SERVER_ERROR;
	}

	res.roleId = ctx->roleid;

	InfoLogStream << "send player(" << res.roleId << ")'s sign to zone(" << zone->GetZoneID() << ")." << LogStream::eos;

	zone->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CenterCustomServiceSignReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "can't find zone by connid:" << req.GetConnID() << LogStream::eos;
		return;
	}

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	std::string url = CSApplication::Instance()->GetCustomServiceAgentAddr() + "?pf=" + req.platform;
	httpReq->setUrl(url.c_str());
	httpReq->setResponseCallback(QueryCustomServiceSignRespons);
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(req.info.c_str(), req.info.length());

	CustomServiceCtx* ctx = new CustomServiceCtx();
	ctx->zoneid = zone->GetZoneID();
	ctx->roleid = req.roleId;
	httpReq->setUserData((void*)ctx);
	http->send(httpReq);

	InfoLogStream << "send custom service http request suc! pram:" << req.info << LogStream::eos;
}

void HandleProtocol(CLProtocol::CenterReportPlayerRoleList& protocol)
{
	json::Object root;
	root["platform"] = json::String(protocol.platform);
	root["openid"] = json::String(protocol.openid);
	root["zoneid"] = json::Number(protocol.zoneid);

	json::Array roles;
	for (auto& role : protocol.roles)
	{
		json::Object roleInfo;
		roleInfo["id"] = json::String(Avalon::StringUtil::ConvertToString(role.roleId));
		roleInfo["occu"] = json::String(Avalon::StringUtil::ConvertToString(role.occu));
		roleInfo["level"] = json::String(Avalon::StringUtil::ConvertToString(role.level));
		roles.Insert(roleInfo);
	}

	root["roles"] = roles;
	
	std::ostringstream oss;
	try
	{
		json::Writer::Write(root, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	std::string str = oss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	std::string url = CSApplication::Instance()->GetRoleListServerAddr();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());
	http->send(httpReq);
}

void HandleProtocol(CLProtocol::CenterReportPlayerCheat& protocol)
{
	json::Object root;
	root["platform"] = json::String(protocol.platform);
	root["openid"] = json::String(protocol.openid);
	root["zoneid"] = json::Number(protocol.zoneid);
	root["accid"] = json::Number(protocol.accid);
	root["roleId"] = json::String(Avalon::StringUtil::ConvertToString(protocol.roleId));
	root["name"] = json::String(protocol.name);
	root["occu"] = json::Number(protocol.occu);
	root["level"] = json::Number(protocol.level);
	root["role_create_time"] = json::Number(protocol.roleCreateTime);
	root["login_time"] = json::Number(protocol.loginTime);
	root["ip"] = json::String(protocol.ip);
	root["is_forbid_login"] = json::Number(protocol.isForbidLogin);
	root["vip_level"] = json::Number(protocol.vipLevel);
	root["total_charge_num"] = json::Number(protocol.totalCharge);
	root["time"] = json::Number(protocol.time);
	root["reason"] = json::Number(protocol.reason);
	root["param1"] = json::Number(protocol.param1);
	root["param2"] = json::Number(protocol.param2);

	
	std::ostringstream oss;
	try
	{
		json::Writer::Write(root, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	std::string str = oss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	std::string url = CSApplication::Instance()->GetCheatReportAddr();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());
	http->send(httpReq);
}

void HandleProtocol(CLProtocol::CenterPushNotify& protocol)
{
	if (!CSApplication::Instance()->CanPush())
	{
		return;
	}

	std::string platform = protocol.platform;
	std::string accid = Avalon::StringUtil::ConvertToString<UInt32>(protocol.accid);
	std::string title = protocol.title;
	std::string content = protocol.content;

	std::string infoStr;
	infoStr += "platform=" + platform;
	infoStr += "&accid=" + accid;
	infoStr += "&title=" + title;
	infoStr += "&content=" + content;
	
	std::string md5Str;
	std::string key = CSApplication::Instance()->GetPushKey();
	md5Str += infoStr;
	md5Str += "&key=" + key;

	char md5Buff[512] = { 0 };
	Avalon::md5_encode32(md5Buff, (void*)md5Str.c_str(), md5Str.length());
	std::string sign(md5Buff);

	std::string pram;
	pram += infoStr;
	pram += "&sign=" + sign;

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(CSApplication::Instance()->GetPushUrl().c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(pram.c_str(), pram.length());
	http->send(httpReq);
}

void HandleProtocol(CLProtocol::CenterForbidAccountReq& protocol)
{
	CLProtocol::CenterForbidAccountNotify send;
	send.accidList = protocol.accidList;
	send.time = protocol.time;
	GameZoneMgr::Instance()->BroadcastToGameZone(send);
}

void GlobalServerInfoRet(HttpClient* client, HttpResponse* response)
{
	if (!response)
	{
		ErrorLogStream << "global server info response is null, error!" << LogStream::eos;
		return;
	}

	if (!response->getHttpRequest())
	{
		ErrorLogStream << "global server info request is null, error!" << LogStream::eos;
		return;
	}

	GlobalServerRouter* routerinfo = (GlobalServerRouter*)(response->getHttpRequest()->getUserData());
	if (!routerinfo)
	{
		ErrorLogStream << "global server info request's routerinfo is null, error!" << LogStream::eos;
		return;
	}

	if (!routerinfo->zoneid)
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") global server info request's routerinfo zone is null, error!" << LogStream::eos;
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(routerinfo->zoneid);
	if (!zone)
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") can't find zone(" << routerinfo->zoneid << ")." << LogStream::eos;
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	std::string resDataStr;
	for (auto data : *response->getResponseData())
	{
		resDataStr += data;
	}

	CLProtocol::CenterGlobalServerInfoRet res;
	res.platform = routerinfo->platform;
	res.openid = routerinfo->openid;
	res.roleId = routerinfo->roleid;

	if (resDataStr.empty())
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") respose data is empty!" << LogStream::eos;
		zone->SendProtocol(res);
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	std::istringstream iss(resDataStr);
	json::Object jsonobj;

	try
	{
		json::Reader::Read(jsonobj, iss);
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") decode global server info json string failed:" << e.what() << LogStream::eos;
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	res.vipLevel = (json::String)jsonobj["vipLevel"];
	zone->SendProtocol(res);

	InfoLogStream << "player(" << routerinfo->roleid << "), pf(" << routerinfo->platform << "), openid(" << routerinfo->openid 
		<< ") get global server info success, vipLevel(" << res.vipLevel << ")." << LogStream::eos;

	CL_SAFE_DELETE(routerinfo);
}

void HandleProtocol(CLProtocol::CenterGlobalServerInfoReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "can't find zone by connid:" << protocol.GetConnID() << LogStream::eos;
		return;
	}

	json::Object root;
	root["platform"] = json::String(protocol.platform);
	root["openid"] = json::String(protocol.openid);

	std::ostringstream oss;
	try
	{
		json::Writer::Write(root, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	std::string str = oss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	std::string url = CSApplication::Instance()->GetPlayerInfoQueryUrl();
	httpReq->setUrl(url.c_str());
	httpReq->setResponseCallback(GlobalServerInfoRet);
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());

	GlobalServerRouter* routerinfo = new GlobalServerRouter();
	routerinfo->zoneid = zone->GetZoneID();
	routerinfo->roleid = protocol.roleId;
	routerinfo->platform = protocol.platform;
	routerinfo->openid = protocol.openid;
	httpReq->setUserData((void*)routerinfo);

	http->send(httpReq);

	InfoLogStream << "player(" << protocol.roleId << "), pf(" << protocol.platform << "), openid("
		<< protocol.openid << ") start to request global server info" << LogStream::eos;
}

void HandleProtocol(CLProtocol::CenterPlayerAddictTimeSync& protocol)
{
	json::Object root;
	root["platform"] = json::String(protocol.platform);
	root["openid"] = json::String(protocol.openid);
	root["addict_time"] = json::String(Avalon::StringUtil::ConvertToString(protocol.addictTime));
	root["server_id"] = json::String(Avalon::StringUtil::ConvertToString(ZONE_ID));
	root["rold_id"] = json::String(Avalon::StringUtil::ConvertToString(protocol.roleid));

	std::ostringstream oss;
	try
	{
		json::Writer::Write(root, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	std::string str = oss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	std::string url = CSApplication::Instance()->GetPlayerAntiAddictInfoUrl();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());
	http->send(httpReq);

	InfoLogStream << "player(" << protocol.roleid << "), pf(" << protocol.platform << "), openid(" << protocol.openid 
		<< ") start to sync addict time(" << protocol.addictTime << ")." << LogStream::eos;
}


void HandleProtocol(CLProtocol::CenterActivityYearSortListSync& protocol)
{
	std::string url = CSApplication::Instance()->GetActivityYearSortListUrl();

	if (url.empty())
	{
		ErrorLogStream << "ActivityYearSortListFunction url is empty" << LogStream::eos;
	}

	std::string uid = Avalon::StringUtil::ConvertToString(protocol.id);

	json::Object root;
	//root["sort_type"] = json::Number(protocol.sortType);
	root["uid"] = json::String(uid);
	//["username"] = json::Number(protocol.accId);
	root["channel_id"] = json::String(protocol.openId);
	root["channel_name"] = json::String(protocol.platform);
	root["server_id"] = json::Number(protocol.zoneId);
	root["server_name"] = json::String(protocol.zoneName);
	root["role"] = json::String(protocol.name);
	root["job"] = json::String(Avalon::StringUtil::ConvertToString(protocol.occu));
	root["level"] = json::Number(protocol.level);
	root["t_rank"] = json::Number(protocol.value1);
	root["star"] = json::Number(protocol.value2);
	//root["value3"] = json::Number(protocol.value3);
	UInt32 nowTime = (UInt32)CURRENT_TIME.Sec();
	root["rank_time"] = json::Number(nowTime);

	std::string appKey = "fdasfg12*";
	char key[33];
	std::string strNowTime = Avalon::StringUtil::ConvertToString(nowTime);
	std::string strSeasonLevel = Avalon::StringUtil::ConvertToString(protocol.value1);
	std::string keyseed = "uid=" + uid + "&channel_id=" + protocol.openId + "&t_rank=" + strSeasonLevel + "&rank_time=" + strNowTime + appKey;
	Avalon::md5_encode32(key, (void*)keyseed.c_str(), keyseed.length());
	key[32] = 0;
	root["sign"] = json::String(std::string(key));

	std::ostringstream oss;
	try
	{
		json::Writer::Write(root, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	std::string str = oss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();

	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);

	httpReq->setRequestData(str.c_str(), str.length());
	http->send(httpReq);

	InfoLogStream << "CenterActivityYearSortListSync id(" << protocol.id << "), pf(" << protocol.platform << "), openid(" << protocol.openId
		<< ") value(" << protocol.value1 << "|" << protocol.value2 << "|" << protocol.value3 << ")." << LogStream::eos;
}

void QueryConvertAccountRet(HttpClient* client, HttpResponse* response)
{
	if (!response)
	{
		ErrorLogStream << "query convert account response is null, error!" << LogStream::eos;
		return;
	}

	if (!response->getHttpRequest())
	{
		ErrorLogStream << "query convert account request is null, error!" << LogStream::eos;
		return;
	}

	QueryConvertAccountInfo* routerinfo = (QueryConvertAccountInfo*)(response->getHttpRequest()->getUserData());
	if (!routerinfo)
	{
		ErrorLogStream << "global server info request's routerinfo is null, error!" << LogStream::eos;
		return;
	}

	if (!routerinfo->zoneid)
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") global server info request's routerinfo zone is null, error!" << LogStream::eos;
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(routerinfo->zoneid);
	if (!zone)
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") can't find zone(" << routerinfo->zoneid << ")." << LogStream::eos;
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	std::string resDataStr;
	for (auto data : *response->getResponseData())
	{
		resDataStr += data;
	}

	CLProtocol::CenterQueryConvertAccountRes res;
	res.fromGate = routerinfo->fromGate;
	res.roleid = routerinfo->roleid;
	res.accid = routerinfo->accid;

	if (resDataStr.empty())
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") respose data is empty!" << LogStream::eos;
		zone->SendProtocol(res);
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	std::istringstream iss(resDataStr);
	json::Object jsonobj;

	//UInt32 result = 0;
	try
	{
		json::Reader::Read(jsonobj, iss);
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "player(" << routerinfo->roleid << ") decode global server info json string failed:" << e.what() << LogStream::eos;
		CL_SAFE_DELETE(routerinfo);
		return;
	}

	res.account = (json::String)jsonobj["account"];
	res.password = (json::String)jsonobj["password"];
	zone->SendProtocol(res);

	CL_SAFE_DELETE(routerinfo);
}

void HandleProtocol(CLProtocol::CenterQueryConvertAccountReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "can't find zone by connid:" << protocol.GetConnID() << LogStream::eos;
		return;
	}

	std::string url = CSApplication::Instance()->GetAccountConvertQueryUrl();
	if (url.empty())
	{
		return;
	}

	std::stringstream ss;
	ss << url << "?pf=" << protocol.platform << "&openid=" << protocol.openid;
	url = ss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();

	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::GET);

	QueryConvertAccountInfo* routerinfo = new QueryConvertAccountInfo();
	routerinfo->zoneid = zone->GetZoneID();
	routerinfo->roleid = protocol.roleid;
	routerinfo->fromGate = protocol.fromGate;
	routerinfo->accid = protocol.accid;
	httpReq->setUserData(routerinfo);
	httpReq->setResponseCallback(QueryConvertAccountRet);
	
	http->send(httpReq);
}

void HandleProtocol(CLProtocol::CenterPunishInfoReq& protocol)
{
	std::string url = CSApplication::Instance()->GetCheatQueryUrl();
	if (url.empty())
	{
		return;
	}

	std::stringstream ss;
	ss << url << "?centerPlatform=" << protocol.centerPlatform << "&zoneid=" << protocol.zoneid;
	url = ss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::GET);
	http->send(httpReq);

	InfoLogStream << "start request punish info, url=" << url << LogStream::eos;
}

void WorldGmOpActivtyRes(HttpClient* client, HttpResponse* response)
{
	InfoLogStream << "gm activity response..." << LogStream::eos;

	if (!response)
	{
		return;
	}

	if (!response->getHttpRequest())
	{
		return;
	}

	std::string reqDataStr(response->getHttpRequest()->getRequestData());
	std::string resDataStr;
	for (auto data : *response->getResponseData())
	{
		resDataStr += data;
	}

	InfoLogStream << "Request(" << reqDataStr << ") and response(" << resDataStr << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::WorldGmOpActivtyReq& protocol)
{
	InfoLogStream << "start request gm activities..." << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
	{
		ErrorLogStream << "can't find zone by connid:" << protocol.GetConnID() << LogStream::eos;
		return;
	}

	std::string url = CSApplication::Instance()->GetGmOpActivityLibUrl();
	if (url.empty())
	{
		ErrorLogStream << "gmOpActivityLibUrl is empty!" << LogStream::eos;
		return;
	}

	std::string applyServer = Avalon::StringUtil::ConvertToString<UInt32>(zone->GetZoneID());

	{
		// 请求本服的数据

		std::string md5Str;
		std::string key = CSApplication::Instance()->GetGmOpActivityKey();
		md5Str += applyServer + key;

		char md5Buff[512] = { 0 };
		Avalon::md5_encode32(md5Buff, (void*)md5Str.c_str(), md5Str.length());
		std::string sign(md5Buff);

		std::string str;
		str.append("apply_server_id=").append(applyServer)
			.append("&serverid=").append(applyServer).append("&sign=").append(sign);

		HttpRequest* httpReq = new HttpRequest();
		httpReq->setUrl(url.c_str());
		httpReq->setRequestType(HttpRequest::Type::POST);
		httpReq->setRequestData(str.c_str(), str.length());
		httpReq->setResponseCallback(WorldGmOpActivtyRes);
		HttpClient::getInstance()->send(httpReq);

		InfoLogStream << "start request activities from gm(url:" << url << ", reqdata:" << str << ")..." << LogStream::eos;
	}

	if (!protocol.mergeServers.empty())
	{
		// 请求合入的服务器的数据

		std::vector<std::string> strs;
		Avalon::StringUtil::Split(protocol.mergeServers, strs, ",");
		for (const auto& elem : strs)
		{
			std::string md5Str;
			std::string key = CSApplication::Instance()->GetGmOpActivityKey();
			md5Str += elem + key;

			char md5Buff[512] = { 0 };
			Avalon::md5_encode32(md5Buff, (void*)md5Str.c_str(), md5Str.length());
			std::string sign(md5Buff);

			std::string str;
			str.append("apply_server_id=").append(applyServer)
				.append("&serverid=").append(elem).append("&sign=").append(sign);

			HttpRequest* httpReq = new HttpRequest();
			httpReq->setUrl(url.c_str());
			httpReq->setRequestType(HttpRequest::Type::POST);
			httpReq->setRequestData(str.c_str(), str.length());
			httpReq->setResponseCallback(WorldGmOpActivtyRes);
			HttpClient::getInstance()->send(httpReq);

			InfoLogStream << "start request merged server's activities from gm(url:" << url << ", reqdata:" << str << ")..." << LogStream::eos;
		}
	}
}

void HandleProtocol(CLProtocol::CenterSyncRedPacketInfo& protocol)
{
	std::string url = CSApplication::Instance()->GetRedPacketRankUrl();
	if (url.empty())
	{
		return;
	}

	std::stringstream ss;
	ss << url << "/update";
	//ss << url << "/update" <<
	//	"?zoneid=" << protocol.zoneId << 
	//	"&accid=" << protocol.accId << 
	//	"&roleid=" << protocol.roleUid << 
	//	"&total_money=" << protocol.totalMoney << 
	//	"&role_name=" << HttpClient::urlEscape(protocol.roleName.c_str(), protocol.roleName.length()) <<
	//	"&server_name=" << HttpClient::urlEscape(protocol.serverName.c_str(), protocol.serverName.length());
	url = ss.str();

	std::stringstream ssParam;
	ssParam << "zoneid=" << protocol.zoneId <<
		"&accid=" << protocol.accId <<
		"&roleid=" << protocol.roleUid <<
		"&total_money=" << protocol.totalMoney <<
		"&role_name=" << protocol.roleName <<
		"&server_name=" << protocol.serverName;
	std::string str = ssParam.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());
	http->send(httpReq);

	InfoLogStream << "start request update red packet info, url=" << url << LogStream::eos;
}

void HandleProtocol(CLProtocol::CenterDeleteRedPacketInfo& protocol)
{
	std::string url = CSApplication::Instance()->GetRedPacketRankUrl();
	if (url.empty())
	{
		return;
	}

	std::stringstream ss;
	ss << url << "/delete_account" <<
		"?zoneid=" << protocol.zoneId <<
		"&accid=" << protocol.accId;
	url = ss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::GET);
	http->send(httpReq);

	InfoLogStream << "start request delete red packet info, url=" << url << LogStream::eos;
}

void CheckRechargePushRet(HttpClient* client, HttpResponse* response)
{
	if (!response)
	{
		ErrorLogStream << "query convert account response is null, error!" << LogStream::eos;
		return;
	}

	if (!response->getHttpRequest())
	{
		ErrorLogStream << "query convert account request is null, error!" << LogStream::eos;
		return;
	}

	CLProtocol::WorldCheckRechargePushRes* res = (CLProtocol::WorldCheckRechargePushRes*)(response->getHttpRequest()->getUserData());
	if (!res)
	{
		ErrorLogStream << "global server info request user data is null, error!" << LogStream::eos;
		return;
	}

	if (!res->zoneId)
	{
		ErrorLogStream << "player(" << res->zoneId << ") global server info request user data zone is null, error!" << LogStream::eos;
		CL_SAFE_DELETE(res);
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(res->zoneId);
	if (!zone)
	{
		ErrorLogStream << "player(" << res->zoneId << ") can't find zone(" << res->zoneId << ")." << LogStream::eos;
		CL_SAFE_DELETE(res);
		return;
	}

	std::string resDataStr;
	for (auto data : *response->getResponseData())
	{
		resDataStr += data;
	}

	if (resDataStr.empty())
	{
		ErrorLogStream << "player(" << res->roleId << ") response data is empty!" << LogStream::eos;
		zone->SendProtocol(*res);
		CL_SAFE_DELETE(res);
		return;
	}

	std::istringstream iss(resDataStr);
	json::Object jsonobj;

	UInt32 result = 1;
	try
	{
		json::Reader::Read(jsonobj, iss);

		const json::Number& resultNum = jsonobj["result"];
		result = resultNum.Value();
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "player(" << res->roleId << ") decode global server info json string failed:" << e.what() << LogStream::eos;
		CL_SAFE_DELETE(res);
		return;
	}

	res->retCode = result;
	zone->SendProtocol(*res);

	CL_SAFE_DELETE(res);
}

void HandleProtocol(CLProtocol::WorldCheckRechargePushReq& protocol)
{
	InfoLogStream << "receive check recharge message" << protocol.zoneId << "," << protocol.roleId << "," << protocol.pushId << LogStream::eos;

	std::string url = CSApplication::Instance()->GetCheckRechargePushUrl();
	if (url.empty())
	{
		InfoLogStream << "check recharge url is null" << LogStream::eos;
		return;
	}

	if (protocol.judgePropIDs.size() != 1)
	{
		InfoLogStream << "check recharge too many judge props" << LogStream::eos;
		return;
	}

	std::stringstream ss;
	ss << url;
	if (1 == protocol.parameterType)
	{
		ss << "/recharge_push_result_on_buy/get";
	}
	else
	{
		ss << "/recharge_push_result_on_use/get";
	}
	
	ss << "?zoneid=" << protocol.zoneId
		<< "&roleid=" << protocol.roleId
		<< "&itemid=" << protocol.judgePropIDs[0]
		<< "&rolelvlmin=" << protocol.lvLower
		<< "&rolelvlmax=" << protocol.lvToplimit
		<< "&viplvlmin=" << protocol.vipLvLower
		<< "&param1=" << protocol.parameter1
		<< "&param2=" << protocol.parameter2
		<< "&day1=" << protocol.daysBeforMin
		<< "&day2=" << protocol.daysBeforeMax
		<< "&day3=" << protocol.recent;

	url = ss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::GET);
	
	CLProtocol::WorldCheckRechargePushRes* res = new CLProtocol::WorldCheckRechargePushRes();
	res->retCode = 1;
	res->roleId = protocol.roleId;
	res->pushId = protocol.pushId;
	res->zoneId = protocol.zoneId;
	httpReq->setUserData(res);
	httpReq->setResponseCallback(CheckRechargePushRet);

	http->send(httpReq);

	InfoLogStream << "start check recharge push, url=" << url << LogStream::eos;
}

void HandleProtocol(CLProtocol::CenterAccountLoginTimeSync& protocol)
{
	json::Object root;
	root["platform"] = json::String(protocol.platform);
	root["openid"] = json::String(protocol.openId);
	root["loginTime"] = json::String(Avalon::StringUtil::ConvertToString(protocol.loginTime));
	root["server_id"] = json::String(Avalon::StringUtil::ConvertToString(ZONE_ID));
	root["rold_id"] = json::String(Avalon::StringUtil::ConvertToString(protocol.roleId));

	std::ostringstream oss;
	try
	{
		json::Writer::Write(root, oss);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "encode json failed: " << e.what() << LogStream::eos;
		return;
	}

	std::string str = oss.str();

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* httpReq = new HttpRequest();
	std::string url = CSApplication::Instance()->GetAccLoginTimeUrl();
	httpReq->setUrl(url.c_str());
	httpReq->setRequestType(HttpRequest::Type::POST);
	httpReq->setRequestData(str.c_str(), str.length());
	http->send(httpReq);

	InfoLogStream << "player(" << protocol.roleId << "), pf(" << protocol.platform << "), openid(" << protocol.openId
		<< ") up node login time(" << protocol.loginTime << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::GASCheckHireBindRet& protocol)
{
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(protocol.zoneId);
	if (zone == nullptr)
	{
		return;
	}
	zone->SendProtocol(protocol);
}


void HandleProtocol(CLProtocol::WorldQueryHireAlreadyBindRes& protocol)
{
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(protocol.zoneId);
	if (zone == nullptr)
	{
		return;
	}
	zone->SendProtocol(protocol);
}
