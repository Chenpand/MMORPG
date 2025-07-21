#include "AvalonProtocol.h"
#include "AvalonPacket.h"
#include "AvalonNetService.h"

namespace Avalon
{
	Protocol::Protocol()
	{
		m_ConnID = 0;
		m_Sequence = 0;
		m_pService = NULL;
	}

	Protocol::~Protocol()
	{
	}

	bool Protocol::Encode(Packet* packet)
	{
		NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			Output(stream);
		}
		catch(const NetSerializeException& exception)
		{
			NETLOG_ERROR("%s protocolid=%u.", exception.what(), GetProtocolID());
			return false;
		}

		packet->SetID(GetProtocolID());
		packet->SetSize(stream.GetPos());
		packet->SetSequence(m_Sequence);
		return true;
	}

	bool Protocol::Decode(Packet* packet)
	{
		NetInputStream stream(packet->GetBuffer(), packet->GetSize());
		try
		{
			Input(stream);
		}
		catch(const NetSerializeException& exception)
		{
			NETLOG_ERROR("%s protocolid=%u.",exception.what(),GetProtocolID());
			return false;
		}

		m_ConnID = packet->GetConnID();
		m_pService = packet->GetNetService();
		m_Sequence = packet->GetSequence();
		return true;
	}

	bool Protocol::SendBack(Protocol& protocol)
	{
        bool ret = false;
		if(m_pService != NULL)
		{
			Avalon::NetConnectionPtr conn = m_pService->FindConnection(m_ConnID);
			if(conn != NULL)
			{
				Avalon::Packet* packet = Avalon::Packet::Create();
				if(protocol.Encode(packet))
				{
					ret = conn->SendPacket(packet);
				}
				Packet::Destroy(packet);
			}
		}
		return ret;
	}
}



