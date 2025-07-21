#include "GMNetwork.h"

#include <CLCenterProtocol.h>
#include "GMClientMgr.h"

GMConnection::GMConnection(CLNetwork* network)
	:CLExternalConnection(network)
{

}

GMConnection::~GMConnection()
{
}

bool GMConnection::PreHandlePacket(Avalon::Packet *packet)
{
	if(CLExternalConnection::PreHandlePacket(packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::CENTER_CHATMONITOR_LOGIN:
		{
			if(IsVerified()) return true;

			CLProtocol::CenterChatMonitorLogin protocol;
			if(!protocol.Decode(packet))
			{
				Close();
				return true;
			}

			if(GMNetwork::Instance()->GetPasswd().compare(protocol.passwd) != 0)
			{
				Close();
				ErrorLogStream << "connection verify failed!" << LogStream::eos;
				return true;
			}

			if(!GMNetwork::Instance()->AddVerifiedConnection(GMNetwork::Instance()->GenID(), this))
			{
				Close();
				return true;
			}
		}
		return true;
	}

	if(CL_IS_SYS_PROTOCOL(packet->GetID())) return true;

	return false;
}

void GMConnection::OnTick(const Avalon::Time& now)
{
	CLExternalConnection::OnTick(now);
}



GMNetwork::GMNetwork()
{
	m_IDGen.Init(1, 1000);
}

GMNetwork::~GMNetwork()
{
}

bool GMNetwork::Startup(Avalon::PropertySet* properties, CLNetAddress* addresses)
{
	if(!CLExternalNetwork::Startup(properties, addresses)) return false;

	m_Passwd = properties->GetProperty(GetName(), "passwd").GetString();
	return true;
}

Avalon::NetConnection* GMNetwork::CreateConnection()
{
	return new GMConnection(this);
}

void GMNetwork::OnConnected(UInt32 id)
{
	GMClientMgr::Instance()->OnConnected(id);
}

void GMNetwork::OnDisconnected(UInt32 id)
{
	GMClientMgr::Instance()->OnDisconnected(id);
	m_IDGen.Release(id);
}
