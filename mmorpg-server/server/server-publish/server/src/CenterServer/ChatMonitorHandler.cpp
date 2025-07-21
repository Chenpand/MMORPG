#include "ChatMonitorHandler.h"
#include "GMClientMgr.h"
#include "AvalonMd5.h"
#include "HttpClient.h"
#include "AvalonStringUtil.h"
#include "CSApplication.h"

std::string getMd5(CLProtocol::CenterSyncChatInfo& protocol)
{
	std::string md5Str;
	md5Str += "server_id=" + Avalon::StringUtil::ConvertToString(protocol.serverId);
	md5Str += "&pindao_id=" + Avalon::StringUtil::ConvertToString(protocol.channel);
	md5Str += "&accid=" + Avalon::StringUtil::ConvertToString(protocol.accId);
	md5Str += "&roleid=" + Avalon::StringUtil::ConvertToString(protocol.roleUid);
	md5Str += "&name=" + protocol.name;
	md5Str += "&msg=" + protocol.word;
	md5Str += "&time=" + Avalon::StringUtil::ConvertToString(protocol.time);
	md5Str += "&tar_id=" + Avalon::StringUtil::ConvertToString(protocol.tarId);
	md5Str += "&sign_key=" + CSApplication::Instance()->GetOperateInfo().chatMonitorMD5Key;
	md5Str += "&tar_name=" + protocol.tarName;
	md5Str += "&level=" + Avalon::StringUtil::ConvertToString(protocol.level);

	InfoLogStream << "chat monitor md5: " << md5Str << LogStream::eos;
	char md5Buff[33];
	Avalon::md5_encode32(md5Buff, (void*)md5Str.c_str(), md5Str.length());
	md5Buff[32] = 0;

	return std::string(md5Buff);
}

std::string getPram(CLProtocol::CenterSyncChatInfo& protocol)
{
	std::string keyParam;
	std::string sign = getMd5(protocol);
	keyParam += "server_id=" + Avalon::StringUtil::ConvertToString(protocol.serverId);
	keyParam += "&pindao_id=" + Avalon::StringUtil::ConvertToString(protocol.channel);
	keyParam += "&accid=" + Avalon::StringUtil::ConvertToString(protocol.accId);
	keyParam += "&roleid=" + Avalon::StringUtil::ConvertToString(protocol.roleUid);
	keyParam += "&name=" + HttpClient::getInstance()->urlEscape(protocol.name.c_str(), protocol.name.size());
	keyParam += "&msg=" + HttpClient::getInstance()->urlEscape(protocol.word.c_str(), protocol.word.size());
	keyParam += "&time=" + Avalon::StringUtil::ConvertToString(protocol.time);
	keyParam += "&tar_id=" + Avalon::StringUtil::ConvertToString(protocol.tarId);
	keyParam += "&tar_name=" + HttpClient::getInstance()->urlEscape(protocol.tarName.c_str(), protocol.tarName.size());
	keyParam += "&level=" + Avalon::StringUtil::ConvertToString(protocol.level);
	keyParam += "&pf=" + Avalon::StringUtil::ConvertToString(protocol.pf);
	keyParam += "&qudao=" + Avalon::StringUtil::ConvertToString(protocol.qudao);
	keyParam += "&sign=" + sign;
	return keyParam;
}

void ChatMonitorRespons(HttpClient* client, HttpResponse* response)
{
	if (response)
	{
		std::string tokenXML;
		for (size_t i = 0; i < response->getResponseData()->size(); ++i)
		{
			tokenXML += response->getResponseData()->at(i);
		}
			
		InfoLogStream << "chat monitor http response: " << tokenXML << LogStream::eos;
	}
	else
	{
		ErrorLogStream << "chat monitor response is null, error!" << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::CenterSyncChatInfo& protocol)
{
// 	class SyncChatVisitor : public GMClientVisitor
// 	{
// 	public:
// 		SyncChatVisitor(CLProtocol::CenterSyncChatInfo& protocol):m_Protocol(protocol){}
// 
// 		bool operator()(GMClient* client)
// 		{
// 			if(client->IsChatMonitorServer(m_Protocol.serverId))
// 			{
// 				client->SendProtocol(m_Protocol);
// 			}
// 			return true;
// 		}
// 	private:
// 		CLProtocol::CenterSyncChatInfo& m_Protocol;
// 	};
// 	SyncChatVisitor visitor(protocol);
// 	GMClientMgr::Instance()->VisitGMClients(visitor);

	std::string pram = getPram(protocol);

	HttpClient* http = HttpClient::getInstance();
	HttpRequest* req = new HttpRequest();
	req->setUrl(CSApplication::Instance()->GetOperateInfo().chatMonitorURL.c_str());
	req->setResponseCallback(ChatMonitorRespons);
	req->setRequestType(HttpRequest::Type::POST);
	req->setRequestData(pram.c_str(), pram.length());
	http->send(req);

	InfoLogStream << "send chat monitor http request suc! pram:" << pram << LogStream::eos;
}

void HandleProtocol(CLProtocol::CenterChatMonitorSetServers& protocol)
{
	GMClient* client = GMClientMgr::Instance()->FindGMClient(protocol.GetConnID());
	if(client == NULL) return;

	client->SetChatMonitorServers(protocol.servers);
}
