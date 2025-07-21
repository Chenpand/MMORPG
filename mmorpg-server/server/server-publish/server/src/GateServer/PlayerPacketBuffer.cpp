#include "PlayerPacketBuffer.h"
#include "GSApplication.h"

#ifdef _CHECK_THREAD_
UInt32 GetThreadID()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}
#endif

PlayerPacketBuffer::PlayerPacketBuffer()
{
	m_accid = 0;
	m_minSequence = 0;
	m_maxSequence = 0;
	m_packetSize = 0;
#ifdef _CHECK_THREAD_
	m_MainThreadId = GetThreadID();
#endif
}

PlayerPacketBuffer::~PlayerPacketBuffer()
{
#ifdef _CHECK_THREAD_
	CheckThread();
#endif

	Clear();
}

bool PlayerPacketBuffer::StorePacket(Avalon::Packet* packet)
{
#ifdef _CHECK_THREAD_
	CheckThread();
#endif

	if (GSApplication::Instance()->IsFilterPacketBuffer(packet->GetID()))
	{
		return false;
	}

	if (!packet || packet->GetSequence() == 0)
	{
		return false;
	}

	Avalon::Packet* tmpPacket = Avalon::Packet::Create(((size_t)packet->GetSize()));
	if (!tmpPacket)
	{
		return false;
	}

	packet->CopyTo(tmpPacket);
	tmpPacket->SetSequence(packet->GetSequence());

	UInt32 packetSize = packet->GetSize() + Avalon::Packet::HEAD_SIZE;

	if (m_packets.size() >= MAX_STORE_PACKET_NUM || m_packetSize + packetSize >= Avalon::Packet::MAX_BODY_SIZE)
	{
		auto begin = m_packets.begin();
		if (begin != m_packets.end())
		{
			auto firstPacket = *begin;
			if (firstPacket)
			{
				auto firstPacketSize = firstPacket->GetSize() + Avalon::Packet::HEAD_SIZE;
				if (firstPacketSize > m_packetSize)
				{
					m_packetSize = 0;
				}
				else
				{
					m_packetSize -= firstPacketSize;
				}
			}

			Avalon::Packet::Destroy(*begin);
			m_packets.erase(begin);
		}

		begin = m_packets.begin();
		if (begin != m_packets.end())
		{
			m_minSequence = (*begin)->GetSequence();
		}
	}

	m_packets.push_back(tmpPacket);
	m_maxSequence = m_maxSequence < packet->GetSequence() ? packet->GetSequence() : m_maxSequence;
	m_packetSize += packetSize;

	if (m_minSequence == 0)
	{
		m_minSequence = tmpPacket->GetSequence();
	}

	return true;
}

UInt32 PlayerPacketBuffer::FetchPacket(UInt32 startSequence, Avalon::NetOutputStream& stream)
{
#ifdef _CHECK_THREAD_
	CheckThread();
#endif

	if (!IsValidSequence(startSequence))
	{
		return 0;
	}

	UInt32 sequence = startSequence;
	for (auto itr = m_packets.begin(); itr != m_packets.end(); )
	{
		auto packet = *itr;
		if (packet->GetSequence() <= startSequence)
		{
			Avalon::Packet::Destroy(packet);
			itr = m_packets.erase(itr);
			continue;
		}
		++itr;

		Avalon::UInt8* buffer = packet->GetBuffer() - Avalon::Packet::HEAD_SIZE;  //得到消息头部的起始位置， 创建Packet时已经给头部预留出HEAD_SIZE（2+4）个字节
		packet->EncodeHeader();
		Int32 sndsize = packet->GetSize() + Avalon::Packet::HEAD_SIZE;
		try
		{
			stream.SeriaArray(buffer, sndsize);
			sequence = packet->GetSequence();
		}
		catch (const Avalon::NetException&)
		{
			return sequence;
		}
	}

	return 0;
}

bool PlayerPacketBuffer::IsValidSequence(UInt32 sequence) const
{
	if (m_minSequence > sequence || sequence > m_maxSequence)
	{
		return false;
	}
	return true;
}

void PlayerPacketBuffer::Clear()
{
#ifdef _CHECK_THREAD_
	CheckThread();
#endif

	for (auto packet : m_packets)
	{
		Avalon::Packet::Destroy(packet);
	}

	m_packets.clear();
	m_minSequence = m_maxSequence = 0;
	m_packetSize = 0;
}
#ifdef _CHECK_THREAD_
void PlayerPacketBuffer::CheckThread()
{
	if (GetThreadID() != m_MainThreadId)
	{
		//assert(0 && "multi thread...");
		ErrorLogStream << "player(" << GetAccID() << ") call in multi thread main thread(" << m_MainThreadId << ") current thread(" << GetThreadID() << ")." << LogStream::eos;;
	}
}
#endif