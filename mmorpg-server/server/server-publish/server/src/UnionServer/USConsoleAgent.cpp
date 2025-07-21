#include "USConsoleAgent.h"

#include <json/writer.h>

#include "USConsoleCmdHandler.h"
#include "USConsoleNetwork.h"


USConsoleAgent::USConsoleAgent()
{
}

USConsoleAgent::~USConsoleAgent()
{
}

void USConsoleAgent::Init()
{
	RegisterHandler(new SubmitOrderHandler());
}

void USConsoleAgent::Final()
{
}

void USConsoleAgent::DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json)
{
	UInt32 timestamp = UInt32(CURRENT_TIME.Sec());

	Avalon::NetConnectionPtr conn = USConsoleNetwork::Instance()->FindConnection(connid);
	if (conn == NULL)
	{
		ErrorLogStream << "can not find connection " << connid << " when send response!" << LogStream::eos;
		return;
	}

	std::ostringstream jsonstream;
	jsonstream << "{\"head\":";
	try
	{
		json::Object headobj;
		headobj["cmd"] = json::String(cmd);
		headobj["para"] = json::String(subcmd);
		headobj["length"] = json::String("");
		headobj["timestamp"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(timestamp));
		headobj["len"] = json::String("");
		json::Writer::Write(headobj, jsonstream);
	}
	catch (json::Exception&)
	{
		return;
	}
	jsonstream << ",\"body\":" << json << "}";

	UInt32 jsonlen = jsonstream.str().length();
	conn->Send(&jsonlen, sizeof(jsonlen));
	conn->Send(jsonstream.str().c_str(), jsonlen);
	conn->Close();
}
