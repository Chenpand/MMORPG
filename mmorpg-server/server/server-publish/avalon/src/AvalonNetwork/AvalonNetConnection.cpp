#include "AvalonNetConnection.h"
#include "AvalonNetExceptions.h"
#include "AvalonNetService.h"

namespace Avalon
{
	NetConnection::NetConnection()
	{
		m_pNetService = NULL;

		m_Type = NCT_ACTIVE;
		m_Status = NCS_CLOSED;
		m_NodeID = 0;
		m_pRecvPacket = NULL;
		m_OpenTime = 0;
		m_CloseWaitTime = 0;
		m_TickNum = 0;
		m_RecvSize = 0;
		memset(m_HeaderBuf, 0, sizeof(m_HeaderBuf));
	}

	NetConnection::~NetConnection()
	{
		m_Socket.Close();
		Packet::Destroy(m_pRecvPacket);
	}

	bool NetConnection::Open(const Socket& socket,NetConnectionType type,NetService* service)
	{
		if(GetStatus() != NCS_CLOSED) return false;
		
		m_OpenTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		m_TickNum = 0;
		m_NodeID = 0;
		m_CloseWaitTime = 0;
		m_Socket = socket;
		m_Socket.GetSockAddr(m_SockAddr);
		m_Socket.GetPeerAddr(m_PeerAddr);
		m_Type = type;
		m_RecvSize = 0;
		Packet::Destroy(m_pRecvPacket);
		m_pNetService = service;
		m_Status = NCS_VERIFY;

		return true;
	}

	void NetConnection::Close(UInt32 waittime)
	{
		if(GetStatus() == NCS_CLOSED) return;
		m_Status = NCS_CLOSED;
		m_TickNum = 0;
		m_CloseWaitTime = waittime;
		if(m_CloseWaitTime > 10) m_CloseWaitTime = 10;
	}

	Int32 NetConnection::Send(const void* buf, Int32 len)
	{
		m_Mutex.Acquire();

		Int32 nLeft = len;
		Int32 nWrite = 0;
		const Int8* bytes = (const Int8*)buf;

		while(nLeft > 0)
		{
			nWrite = SendChunk(bytes, nLeft);
			if(nWrite == AVALON_SOCK_ERROR)
			{
				m_Mutex.Release();
				return nWrite;
			}

			nLeft -= nWrite;
			bytes += nWrite;
		}

		m_Mutex.Release();
		return len - nLeft;
	}

	Int32 NetConnection::SendChunk(const void* buf, Int32 len)
	{
		Int32 ret = 0;
		while((ret = m_Socket.Send(buf, len)) == AVALON_SOCK_ERROR)
		{
			int errcode = Avalon::Socket::GetError();

			if(errcode == AVALON_EWOULDBLOCK || errcode == AVALON_EAGAIN)
			{
				if(!GetNetService()->GetNetProperties()->send_wait) return AVALON_SOCK_ERROR;

				//等待可发送
				timeval waittime;
				waittime.tv_sec = 0;
				waittime.tv_usec = 3000;
				int trytimes = 1000;
				fd_set	writeset;
				while(true)
				{
					FD_ZERO(&writeset);
					FD_SET(m_Socket.GetHandle(), &writeset);

					int ret = select(m_Socket.GetHandle() + 1, NULL, &writeset, NULL, &waittime);
					if(AVALON_SOCK_ERROR == ret)
					{
						if(AVALON_EINTR == Avalon::Socket::GetError()) continue;
						else return AVALON_SOCK_ERROR;
					}
					else if(0 == ret)
					{
						if(trytimes-- > 0) continue;
						else return AVALON_SOCK_ERROR;
					}
					else break;
				}
			}
			else
			{
				return AVALON_SOCK_ERROR;
			}
		}
		return ret;
	}

	Int32 NetConnection::Recv(void* buf, Int32 len)
	{
		return m_Socket.Recv(buf, len);
	}

	bool NetConnection::SendPacket(Packet* packet)
	{
		if(GetStatus() == NCS_CLOSED || NULL == packet) return false;

		try
		{
			ProcessSendPacket(packet);
		}
		catch(NetException& exception)
		{
			NETLOG_ERROR("exception (%s) happened when send packet!(%s:%u)"
				,exception.what(),m_PeerAddr.GetIP().c_str(),m_PeerAddr.GetPort());
			return false;
		}

		Avalon::UInt8* buffer = packet->GetBuffer() - Packet::HEAD_SIZE;  //得到消息头部的起始位置， 创建Packet时已经给头部预留出HEAD_SIZE（2+4）个字节

		EncodeHeader(buffer, Packet::HEAD_SIZE, packet->GetSize(), packet->GetID(), packet->GetSequence()); //编码头部
		
		Int32 sndsize = packet->GetSize() + Packet::HEAD_SIZE;

		int ret = Send(buffer, sndsize);
		
		if(ret < 0)
		{
			int errcode = Avalon::Socket::GetError();
			NETLOG_ERROR("error %d happened when send packet(%u,%d) to (%s:%u)",
				errcode, packet->GetID(), sndsize, m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());
			Close(); 
			return false;
		}
		else if(ret < sndsize)
		{
			NETLOG_ERROR("send less then requested.(%s:%u)"
				,m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());
			Close();
			return false;
		}

		return true;
	}


#define AVALON_CHECK_RECV_RET(ret) \
	if(0 == ret){ \
		NETLOG_INFO("connection %u closed by peer!(%s:%u)", GetID(), m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());\
		Close(); \
		return NULL; \
	} else if(ret < 0) { \
		int err = Avalon::Socket::GetError(); \
		if(err != AVALON_EAGAIN && err != AVALON_EWOULDBLOCK) { \
			if(err == AVALON_ECONNRESET) { \
				NETLOG_INFO("connection %u closed by peer!(%s:%u)", GetID(), m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());\
			}else{ \
				NETLOG_ERROR("error %d happened when recv packet!(%s:%u)",err,m_PeerAddr.GetIP().c_str(),m_PeerAddr.GetPort());\
			} \
			Close(); \
		} \
		return NULL; \
	}

	Packet* NetConnection::RecvPacket()
	{
		if(GetStatus() == NCS_CLOSED) return NULL;

		Packet* packet = NULL;

		do
		{
			//接收数据，并进行组包
			Packet::Destroy(packet);

			if (NULL == m_pRecvPacket)
			{
				int ret = Recv(m_HeaderBuf + m_RecvSize, Packet::HEAD_SIZE - m_RecvSize);
				AVALON_CHECK_RECV_RET(ret);

				m_RecvSize += ret;
				if (m_RecvSize < Packet::HEAD_SIZE) return NULL; //noted by aprilliu 如果接收到的数据过少（太少无法进行处理），则继续接收数据.

				UInt16 size = 0;
				UInt32 id = 0;
				UInt32 sequence = 0;
				DecodeHeader(m_HeaderBuf, Packet::HEAD_SIZE, size, id, sequence);

				if (size > Packet::MAX_BODY_SIZE && m_pNetService && m_pNetService->IsCheckBodySize())
				{
					// 包体超过最大值，先直接断开连接
					NETLOG_ERROR("conn %d recv too big packet(%u) from (%s:%u)", GetID(), size, m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());
					Close(); 
					return NULL;
				}

				m_pRecvPacket = Packet::Create(size);
				m_pRecvPacket->SetID(id);
				m_pRecvPacket->SetSize(size);
				m_pRecvPacket->SetSequence(sequence);
				m_RecvSize = 0;
			}

			if(m_pRecvPacket->GetSize() > m_RecvSize)
			{
				int ret = Recv(m_pRecvPacket->GetBuffer() + m_RecvSize, m_pRecvPacket->GetSize() - m_RecvSize);
				AVALON_CHECK_RECV_RET(ret);

				m_RecvSize += ret;
				if(m_RecvSize < m_pRecvPacket->GetSize()) return NULL;  //noted by aprilliu. 没有接收到完整数据，则继续接收
			}

			packet = m_pRecvPacket;
			m_pRecvPacket = NULL;
			m_RecvSize = 0;

			try
			{
				ProcessRecvPacket(packet);  //子类也没有更多的处理逻辑
			}
			catch(NetException& exception)
			{
				NETLOG_ERROR("exception (%s) happened when recv packet!(%s:%u)"
					,exception.what(),m_PeerAddr.GetIP().c_str(),m_PeerAddr.GetPort());
				continue;
			}

		}
		while(PreHandlePacket(packet));

		return packet;
	}

	void NetConnection::EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence)
	{
		*(UInt16*)(header) = htons(size);
		*(UInt32*)(header + sizeof(UInt16))= htonl(id);
		*(UInt32*)(header + sizeof(UInt16) + sizeof(UInt32)) = htonl(sequence);
	}

	void NetConnection::DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence)
	{
		size = ntohs(*(UInt16*)header);
		id = ntohl(*(UInt32*)(header + sizeof(UInt16)));
		sequence = ntohl(*(UInt32*)(header + sizeof(UInt16)+sizeof(UInt32)));
	}

	void NetConnection::SetVerified()
	{
		if(GetStatus() == NCS_VERIFY)
		{
			m_Status = NCS_NORMAL;
		}
	}

	void NetConnection::HandleInput()
	{
		if(GetStatus() == NCS_CLOSED) return;

		OnDataReached();
	}

	void NetConnection::HandleOutput()
	{
		NETLOG_ERROR("unexpect HandleOutput() happened!(%s:%u)",m_PeerAddr.GetIP().c_str(),m_PeerAddr.GetPort());
	}

	void NetConnection::HandleException()
	{
		GetReactor()->RemoveHandler(this, NETEVENT_EXCEPTION);
		Close();
	}

	void NetConnection::HandleClose()
	{
		GetNetService()->RemoveConnection(this);
		m_Status = NCS_CLOSED;
		m_Socket.Close();

		NETLOG_ERROR("connection %u closed!(%s:%u)", GetID(), m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());
		DecRef(); //noted by aprilliu, 关闭连接要减少引用计数
	}

	void NetConnection::HandleTick(const Time& now)
	{
		++m_TickNum;

		if(GetStatus() == NCS_CLOSED)  
		{
			if(m_TickNum > m_CloseWaitTime)
			{
				GetReactor()->RemoveHandler(this);  //noted by aprilliu, 移除处于已关闭状态的连接
			}
			return;
		}
		else if(GetStatus() == NCS_VERIFY)
		{
			if(now.Sec() > m_OpenTime + GetNetService()->GetNetProperties()->verify_timeout)
			{
				NETLOG_ERROR("connection verify timeout!(%s:%u)",m_PeerAddr.GetIP().c_str(),m_PeerAddr.GetPort());
				Close();
				return;
			}
		}

		UInt32 heartbeat = GetNetService()->GetNetProperties()->heartbeat_timeout;
		if(m_TickNum > heartbeat)
		{
			NETLOG_ERROR("connection %u heartbeat timeout!(%s:%u)", GetID(), m_PeerAddr.GetIP().c_str(), m_PeerAddr.GetPort());
			Close();
			return;
		}
		else if(GetType() == NCT_PASSIVE  && m_TickNum % 10 == 0) // 改成每10s发一次
		{
			Packet* packet = Packet::Create(0);
			packet->SetID(0);
			packet->SetSize(0);
			SendPacket(packet);
			Packet::Destroy(packet);
		}

		OnTick(now);
	}

	bool NetConnection::PreHandlePacket(Packet* packet)
	{
		packet->SetConnID(GetID());
		packet->SetNetService(GetNetService());

		switch(packet->GetID())
		{
		case 0://心跳包
			{
				if(GetType() == NCT_ACTIVE)
				{
					SendPacket(packet);
				}
				m_TickNum = 0;
			}
			return true;
		}

		return false;
	}
}
