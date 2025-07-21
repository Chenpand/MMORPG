#include "AvalonServerApplication.h"
#include "AvalonSingleton.h"
#include "TestNetwork.h"

Avalon::NetConnectionPtr connection = NULL;
Avalon::UInt32 numOfPacket = 0;


class ServerNetwork : public TestNetwork
{
public:
	Avalon::NetConnection* CreateConnection()
	{
		return new TestConnection(this);
	}

	void OnAsynConnected(Avalon::UInt32 id, Avalon::NetConnectionType)
	{
		connection = FindConnection(id);
	}

	void OnAsynDisconnected(Avalon::UInt32 id, Avalon::NetConnectionType)
	{
		connection = NULL;
	}

	void DispatchPacket(Avalon::Packet* packet)
	{
		switch(packet->GetID())
		{
		case TEST_TIME:
			{
				connection->SendPacket(packet);
				++numOfPacket;
			}
			break;
		}

		Avalon::Packet::Destroy(packet);
	}
};

class TestApplication : public Avalon::ServerApplication, public Avalon::Singleton<TestApplication>
{
public:
	bool OnInit()
	{
		Avalon::NetProperties properties;
		m_pNetwork = new ServerNetwork();
		if(!m_pNetwork->Startup(properties))
		{
			delete m_pNetwork;
			m_pNetwork = NULL;
			return false;
		}
		std::string ip = GetProperties()->GetProperty("server","ip").GetString();
		Avalon::UInt32 port = GetProperties()->GetProperty("server","port");
		if(!m_pNetwork->Bind(Avalon::SockAddr(ip,port)))
		{
			GetSysLog()->Error("bind error!");
			return false;
		}
		return true;
	}

	void OnTick(const Avalon::Time& now)
	{
		GetSysLog()->Info("received %u packets!",numOfPacket);
		numOfPacket = 0;
	}

	void OnQuit()
	{
		if(m_pNetwork != NULL)
		{
			m_pNetwork->Shutdown();
			delete m_pNetwork;
		}
	}

private:
	Avalon::NetService* m_pNetwork;
};

int main(int argc,char* argv[])
{
	TestApplication::Instance()->Init("TestServer","TestServer.cfg");
	TestApplication::Instance()->SetInterval(1000);
	TestApplication::Instance()->Main(argc,argv);
	TestApplication::Destroy();

	return 0;
}