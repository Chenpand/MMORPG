#include "ASConsoleHandler.h"

#include <json/writer.h>
#include "ASConsoleNetwork.h"

void HandleProtocol(CLProtocol::AdminConsoleResponse& response)
{
	Avalon::NetConnectionPtr conn = ConsoleNetwork::Instance()->FindConnection(response.nodeid);
	if(conn == NULL)
	{
		GameErrorLogStream("Console") << "can not find connection " << response.nodeid << " when send response!" << LogStream::eos;
		return;
	}

	std::ostringstream jsonstream;
	jsonstream << "{\"head\":";
	try
	{
		json::Object headobj;
		headobj["cmd"] = json::String(response.cmd);
		headobj["para"] = json::String(response.subcmd);
		headobj["length"] = json::String("");
		headobj["timestamp"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(response.timestamp));
		headobj["len"] = json::String("");
		json::Writer::Write(headobj, jsonstream);
	}
	catch(json::Exception&)
	{
		return;
	}
	jsonstream << ",\"body\":" << response.body << "}";
	
	UInt32 jsonlen = jsonstream.str().length();
	UInt32 buffLen = jsonlen + 1024;
	UInt8* buffer = new UInt8[buffLen];
	Avalon::NetOutputStream stream(buffer, buffLen);
	try {
		stream & jsonlen;
		stream.SeriaArray(jsonstream.str().c_str(), jsonlen);
		conn->Send(buffer, stream.GetPos());
		InfoLogStream << "send use stream" << LogStream::eos;
	}
	catch (const Avalon::NetException& e)
	{
		conn->Send(&jsonlen, sizeof(jsonlen));
		conn->Send(jsonstream.str().c_str(), jsonlen);
		InfoLogStream << "send use old mode: " << e.what() << LogStream::eos;
	}

	delete buffer;
	conn->Close();
}
