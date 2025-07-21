#ifndef _TEST_NETWORK_H_
#define _TEST_NETWORK_H_

#include <AvalonNetConnection.h>
#include <AvalonNetService.h>
#include <AvalonProtocol.h>

#define LOGIN 1
#define TEST_TIME 2

class LoginProtocol : public Avalon::Protocol
{
	AVALON_DEFINE_PROTOCOLID(LOGIN)
public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id;
	}

	Avalon::UInt32 id;
};

class TestTimeProtocol : public Avalon::Protocol
{
	AVALON_DEFINE_PROTOCOLID(TEST_TIME)
public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & time;
	}

	Avalon::UInt64 time;
};

class TestNetwork : public Avalon::NetService
{
public:

	virtual void DispatchPacket(Avalon::Packet* packet) = 0;

private:
};

class TestConnection : public Avalon::NetConnection
{
public:
	
	TestConnection(TestNetwork* network)
	{
		m_pNetwork = network;
	}

	bool PreHandlePacket(Avalon::Packet* packet)
	{
		if(NetConnection::PreHandlePacket(packet)) return true;

		switch(packet->GetID())
		{
		case LOGIN:
			{
				LoginProtocol protocol;
				protocol.Decode(packet);
				m_pNetwork->AddVerifiedConnection(protocol.id,this);
			}
			return true;
		}
		return false;
	}

	void OnDataReached()
	{
		Avalon::Packet* packet = RecvPacket();
		while(packet != NULL)
		{
			m_pNetwork->DispatchPacket(packet);
			packet = RecvPacket();
		}
	}

	void OnTick(const Avalon::Time& now)
	{
	}

private:
	TestNetwork* m_pNetwork;
};


#endif
