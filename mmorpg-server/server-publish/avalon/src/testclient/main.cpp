#include "AvalonServerApplication.h"
#include "AvalonSingleton.h"
#include "TestNetwork.h"
#include "AvalonRandom.h"

Avalon::NetConnectionPtr connection = NULL;
Avalon::UInt32 numOfPacket = 0;
Avalon::UInt64 totalInter = 0;

class ClientNetwork : public TestNetwork
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
				TestTimeProtocol protocol;
				protocol.Decode(packet);
				totalInter += Avalon::Time::CurrentTime().MSec() - protocol.time;
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
		m_pNetwork = new ClientNetwork();
		if(!m_pNetwork->Startup(properties))
		{
			delete m_pNetwork;
			m_pNetwork = NULL;
			return false;
		}
		std::string ip = GetProperties()->GetProperty("server","ip").GetString();
		Avalon::UInt32 port = GetProperties()->GetProperty("server","port");
		connection = m_pNetwork->Connect(1,Avalon::SockAddr(ip,port));
		if(connection == NULL)
		{
			GetSysLog()->Error("connect error");
			return false;
		}
		LoginProtocol protocol;
		protocol.id = 1;
		Avalon::Packet* packet = Avalon::Packet::Create();
		protocol.Encode(packet);
		connection->SendPacket(packet);
		Avalon::Packet::Destroy(packet);
		return true;
	}

	void OnTick(const Avalon::Time& now)
	{
		if(connection != NULL)
		{
			TestTimeProtocol protocol;
			protocol.time = Avalon::Time::CurrentTime().MSec();
			Avalon::Packet* packet = Avalon::Packet::Create();
			protocol.Encode(packet);
			int num = 1; 
			while(num-- > 0)
			{
				connection->SendPacket(packet);
			}
			Avalon::Packet::Destroy(packet);
		}
		Avalon::UInt32 interval = numOfPacket == 0 ? 0 : (Avalon::UInt32)totalInter / numOfPacket;
		GetSysLog()->Info("received %u packets!,interval %u",numOfPacket,interval);
		numOfPacket = 0;
		totalInter = 0;
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
	ClientNetwork* m_pNetwork;
};

int main(int argc,char* argv[])
{
	TestApplication::Instance()->Init("TestClient","TestClient.cfg");
	TestApplication::Instance()->SetInterval(1000);
	TestApplication::Instance()->Main(argc,argv);
	TestApplication::Destroy();

	return 0;
}