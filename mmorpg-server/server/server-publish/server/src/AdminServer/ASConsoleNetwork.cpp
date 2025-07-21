#include "ASConsoleNetwork.h"

#include <AvalonMutex.h>
#include <sstream>
#include <CLNetAddress.h>
#include <CLConsoleProtocol.h>
#include <CLSysProtocol.h>

#include <json/reader.h>
#include <json/writer.h>

#include "ASRouter.h"
#include "DBLoggerMgr.h"

ConsoleConnection::ConsoleConnection(CLNetwork* network)
	:CLNetConnection(network)
{
}

ConsoleConnection::~ConsoleConnection()
{
}

void ConsoleConnection::OnDataReached()
{
	do
	{
		Avalon::Packet* packet = RecvPacket();
		if(packet == NULL) break;

		GetNetwork()->DispatchPacket(packet);
	}
	while(true);
}

void ConsoleConnection::OnTick(const Avalon::Time& now)
{
	CLNetConnection::OnTick(now);
}

bool ConsoleConnection::PreHandlePacket(Avalon::Packet* packet)
{
	if(CLNetConnection::PreHandlePacket(packet)) return true;

	if(!IsVerified())
	{
		UInt32 id = GenID();
		if(!GetNetwork()->AddVerifiedConnection(id, this))
		{
			GameErrorLogStream("Console") << "repeat console connection id:" << id << "!" << LogStream::eos;
			Close();
			return true;
		}

		packet->SetConnID(GetID());
	}

	return false;
}

void ConsoleConnection::EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence)
{
	*(UInt32*)header = UInt32(size);
	*(UInt16*)(header + sizeof(UInt32)) = 0;
}
		
void ConsoleConnection::DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence)
{
	size = *(UInt32*)header;
	id = 0xFFFFFFFF;
}
	
UInt32 g_ConsoleIdSeed = 0;
Avalon::Mutex g_ConsoleMutex;

UInt32 ConsoleConnection::GenID()
{
	UInt32 ret = 0;
	g_ConsoleMutex.Acquire();
	if(++g_ConsoleIdSeed == 0) g_ConsoleIdSeed = 1;
	ret = g_ConsoleIdSeed;
	g_ConsoleMutex.Release();
	return ret;
}


ConsoleNetwork::ConsoleNetwork()
{
}

ConsoleNetwork::~ConsoleNetwork()
{
}

Avalon::NetConnection* ConsoleNetwork::CreateConnection()
{
	return new ConsoleConnection(this);
}

bool ConsoleNetwork::Startup(Avalon::PropertySet* properties, CLNetAddress* addresses)
{
	if(!CLNetwork::Startup(properties))
	{
		ErrorLogStream << "startup network failed!" << LogStream::eos;
		return false;
	}

	Avalon::SockAddr addr;
	if(!addresses->FindConsoleAddress(NODE_ID, addr))
	{
		ErrorLogStream << "lose console address!" << LogStream::eos;
		return false;
	}
#ifdef _DEBUG
	addr.SetIP("0.0.0.0");
#endif

	if(!Bind(addr))
	{
		CLNetwork::Shutdown(0);
		return false;
	}

	return true;
}

void ConsoleNetwork::Shutdown(UInt32 inputTimeout)
{
	CLNetwork::Shutdown(inputTimeout);
}


bool ConsoleNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if(packet->GetID() == CLProtocol::SYS_NOTIFY_CONNECTED)
	{
		CLProtocol::SysNotifyConnected protocol;
		protocol.Decode(packet);
		OnConnected(protocol.id);
		return true;
	}
	else if(packet->GetID() == CLProtocol::SYS_NOTIFY_DISCONNECTED)
	{
		CLProtocol::SysNotifyDisconnected protocol;
		protocol.Decode(packet);
		OnDisconnected(protocol.id);
		return true;
	}

	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	//UInt32 timestamp = 0;

	std::string jsonstr;
	//auto buffer = packet->GetBuffer();
	jsonstr.assign(packet->GetBuffer(), packet->GetBuffer() + packet->GetSize());
	GameInfoLogStream("Console") << "recved request:" << jsonstr << LogStream::eos;

	if (jsonstr.empty())
	{
		ErrorLogStream << "invalid json str from conn(" << packet->GetConnID() << ")." << LogStream::eos;
		return false;
	}

	std::istringstream iss(jsonstr);
	json::Object jsonobj;

	CLProtocol::AdminConsoleRequest req;
	req.nodeid = packet->GetConnID();
	try
	{
		json::Reader::Read(jsonobj, iss);

		const json::Object& headobj = jsonobj["head"];
		const json::String& cmdstr = headobj["cmd"];
		req.cmd = cmdstr.Value();
		const json::String& subcmdstr = headobj["subcmd"];
		req.subcmd = subcmdstr.Value();
		const json::String& numstr = headobj["timestamp"];
		req.timestamp = Avalon::StringUtil::ConvertToValue<UInt32>(numstr.Value());

		const json::Object& bodyobj = jsonobj["body"];

		if(req.cmd.compare("gameControl") == 0 && req.subcmd.compare("logQuery") == 0)
		{
			const json::String& logtype =	bodyobj["type"];
			const json::String& roleid =	bodyobj["roleid"];
			const json::String& beginTime = bodyobj["beginTime"];
			const json::String& endTime =	bodyobj["endTime"];
			const json::String& page =		bodyobj["page"];
			const json::String& pagesize =	bodyobj["pagesize"];
			const json::String& itemid =	bodyobj["itemid"];
			DBLoggerMgr::Instance()->QueryLog(packet->GetConnID(), logtype.Value(), Avalon::StringUtil::ConvertToValue<ObjID_t>(roleid.Value())
				, Avalon::StringUtil::ConvertToValue<UInt32>(beginTime.Value()), Avalon::StringUtil::ConvertToValue<UInt32>(endTime.Value())
				, Avalon::StringUtil::ConvertToValue<UInt16>(page.Value()), Avalon::StringUtil::ConvertToValue<UInt16>(pagesize.Value())
				, Avalon::StringUtil::ConvertToValue<UInt32>(itemid.Value()));
			return true;
		}

		std::ostringstream oss;
		json::Writer::Write(bodyobj,oss);
		req.body = oss.str();
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleNetwork::ExecutePacket failed:" << e.what() << LogStream::eos;
		return false;
	}

	Router::SendToWorld(req);

	return true;
}

void ConsoleNetwork::OnConnected(UInt32 id)
{
	GameInfoLogStream("Console") << "console " << id << " connected!" << LogStream::eos;
}

void ConsoleNetwork::OnDisconnected(UInt32 id)
{
	GameInfoLogStream("Console") << "console " << id << " disconnected!" << LogStream::eos;
}
