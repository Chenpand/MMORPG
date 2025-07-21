#include <AvalonNetworkAPI.h>
#include "AvalonPacket.h"

namespace Avalon
{

	UInt32 Packet::s_MagicCodeSeed = 0;

	MemPool<Mutex>* g_PacketMemPool[] = { //noted by aprilliu 不同级别的固定大小分片  牺牲内存利用率 换去时间效率
		new MemPool<Mutex>(128),
		new MemPool<Mutex>(256),
		new MemPool<Mutex>(512),
		new MemPool<Mutex>(1024),
		new MemPool<Mutex>(2048),
		new MemPool<Mutex>(4096),
		new MemPool<Mutex>(8192),
		new MemPool<Mutex>(16384),
		new MemPool<Mutex>(32768),
		new MemPool<Mutex>(65536),
	};

	int GetMemPoolIndex(size_t size)
	{
		if(size > 65536) size = 65536;

		size = (size - 1) / 128;
		int index = 0;
		while(size > 0)
		{
			++index;
			size = size / 2;
		} 
		return index;
	}

	Packet* Packet::Create()
	{
		return Create(MAX_BODY_SIZE);
	}

	Packet* Packet::Create(size_t size)
	{
		if(size > MAX_BODY_SIZE) size = MAX_BODY_SIZE;

		int index = GetMemPoolIndex(sizeof(Packet) + HEAD_SIZE + size);
#ifdef _CLOSE_PACKET_POOL_
		void* ptr = malloc(g_PacketMemPool[index]->GetBlockSize());
#else
		void* ptr = g_PacketMemPool[index]->Alloc();
#endif
		Packet* ret = new(ptr) Packet();  //在分配到的缓冲区创建Packet对象，对象+HEAD_SIZE(2B包长度+4B messageID) 之后才是真正的数据区
		ret->m_MaxSize = g_PacketMemPool[index]->GetBlockSize() - (sizeof(Packet) + HEAD_SIZE);
		return ret;
	}

	void Packet::Destroy(Packet*& pack)
	{
		if(pack == NULL) return;

		int index = GetMemPoolIndex(pack->m_MaxSize + sizeof(Packet) + HEAD_SIZE);
		pack->~Packet();
#ifdef _CLOSE_PACKET_POOL_
		free(pack);
#else
		g_PacketMemPool[index]->Free(pack);
#endif
		pack = NULL;
	}

	UInt32 Packet::GetNodeCount()
	{
#ifdef _CLOSE_PACKET_POOL_
		return 0;
#else
		UInt32 usedCount = 0;
		for (size_t i=0; i < sizeof(g_PacketMemPool) /sizeof(g_PacketMemPool[0]); ++i)
		{
			usedCount += g_PacketMemPool[i]->GetNodeCount();
		}
		return usedCount;
#endif
	}

	UInt32 Packet::GetFreeCount()
	{
#ifdef _CLOSE_PACKET_POOL_
		return 0;
#else
		UInt32 freeCount = 0;
		for (size_t i = 0; i < sizeof(g_PacketMemPool) / sizeof(g_PacketMemPool[0]); ++i)
		{
			freeCount += g_PacketMemPool[i]->GetFreeCount();
		}
		return freeCount;
#endif
	}

	void Packet::CopyTo(Avalon::Packet *packet)
	{
		if(packet->GetMaxSize() < GetSize()) return;

		packet->SetID(m_Id);
		packet->SetSize(m_Size);
		memmove(packet->GetBuffer(), GetBuffer(), m_Size);
	}

	//added by aprilliu, 2016.2.29
	bool Packet::EncodeHeader()
	{
		Avalon::UInt8* buffer = this->GetBuffer() - Packet::HEAD_SIZE;
		*(UInt16*)(buffer) = htons(this->GetSize());
		*(UInt32*)(buffer + sizeof(UInt16)) = htonl(this->GetID());
		*(UInt32*)(buffer + sizeof(UInt16) + sizeof(UInt32)) = htonl(this->GetSequence());
		return true;
	}

	bool Packet::DecodeHeader(UInt8* header, size_t size)
	{
		if (NULL == header || size < Packet::HEAD_SIZE)
		{
			return false;
		}

		this->m_Size = ntohs(*(UInt16*)header);
		this->m_Id = ntohl(*(UInt32*)(header + sizeof(UInt16)));
		this->m_Sequence = ntohl(*(UInt32*)(header + sizeof(UInt16)+sizeof(UInt32)));
		return true;
	}

	UInt8* Packet::GetPacketBuff()
	{
		return (this->GetBuffer() - Packet::HEAD_SIZE);
	}

	Avalon::UInt16 Packet::GetPacketSize()
	{
		return(m_Size + Packet::HEAD_SIZE);
	}

}
