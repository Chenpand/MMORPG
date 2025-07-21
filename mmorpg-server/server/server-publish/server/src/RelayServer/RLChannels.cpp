#include <AvalonSharedMemory.h>
#include <AvalonCodeQueue.h>
#include <AvalonPropertySet.h>
#include <AvalonPacket.h>
#include "RLChannels.h"
#include "CLNetHeader.h"
#include "CLRelayServerProtocol.h"


Avalon::CShmMemory* g_pstCSChannelShm = NULL;
Avalon::CShmMemory* g_pstSCChannelShm = NULL;

RLChannels::RLChannels()
{
	m_pstCSChannel = NULL;
	m_pstSCChannel = NULL;
}

RLChannels::~RLChannels()
{
	//TODO: delete the shared memory ?
}

bool RLChannels::Init(Avalon::PropertySet* prop)
{
	if (!make_channels(prop))
	{
		ErrorLogStream << "make_channels failed" << LogStream::eos;
		return false;
	}

	//register protocols
	RegisterProtocols();

	//someelse

	return true;
}

Int32 RLChannels::send_data(char* data, size_t size)
{
	if (NULL == data || !size)
	{
		return fail;
	}

	return m_pstSCChannel->append((const char*)data, (Int32)size);;
}

Int32 RLChannels::recv_data(char* data, size_t& size)
{
	if (NULL == data || !size)
	{
		return fail;
	}

	return m_pstCSChannel->pop(data, (Int32&)size);
}

void RLChannels::OnTick(const Avalon::Time& time)
{
	//check channels,  log something or warning
	check_channels(time);

	//pop message and handle it
	process_packets();


	return;
}

void RLChannels::OnQuit()
{
	//TODO: delete the channels?
}

bool RLChannels::make_channels(Avalon::PropertySet* prop)
{
	if (NULL == prop)
	{
		return false;
	}

	UInt32 unCSChannelKey = prop->GetProperty("Channels", "CSChannelKey");
	UInt32 unSCChannelKey = prop->GetProperty("Channels", "SCChannelKey");
	if (!unCSChannelKey || !unSCChannelKey)
	{
		ErrorLogStream << "invalid cs channel key: " << unCSChannelKey << ", sc channel key: " << unSCChannelKey << LogStream::eos;
		return false;
	}

	UInt32 unShmSize = Avalon::CCodeQueue::size(MAX_CS_CHANNEL_SIZE);

	g_pstCSChannelShm = Avalon::create_shm_memory(unCSChannelKey, unShmSize);
	if (NULL == g_pstCSChannelShm)
	{
		ErrorLogStream << "create cs channel memory failed, key= " << unCSChannelKey << LogStream::eos;
		return false;
	}

	g_pstSCChannelShm = Avalon::create_shm_memory(unSCChannelKey, unShmSize);
	if (NULL == g_pstSCChannelShm)
	{
		ErrorLogStream << "create sc channel memory failed, key= " << unSCChannelKey << LogStream::eos;
		return false;
	}

	//
	Avalon::CCodeQueue::set_sharedmemory(g_pstCSChannelShm);
	m_pstCSChannel = new Avalon::CCodeQueue(MAX_CS_CHANNEL_SIZE);
	if (NULL == m_pstCSChannel)
	{
		ErrorLogStream << "create cs channel queue failed, key= " << unCSChannelKey << LogStream::eos;
		return false;
	}

	Avalon::CCodeQueue::set_sharedmemory(g_pstSCChannelShm);
	m_pstSCChannel = new Avalon::CCodeQueue(MAX_CS_CHANNEL_SIZE);
	if (NULL == m_pstSCChannel)
	{
		ErrorLogStream << "create sc channel queue failed, key= " << unSCChannelKey << LogStream::eos;
		return false;
	}

	return true;
}



void RLChannels::check_channels(const Avalon::Time& now)
{
	//do nothing  now
}

void RLChannels::process_packets()
{
	//
	static char szRecvBuff[max_relaysvr_cs_packet_size];

	//
	Int32 iRecvSize = 0;

	//
	Int32 toRecvPacketCount = 512;	//每次处理的包数量
	Avalon::Packet* packet = NULL;
	while (toRecvPacketCount--)
	{
// 		InfoLogStream << "in RLChannels::process_packets, cs channel code size: " << m_pstCSChannel->get_codesize() << LogStream::eos;
// 		if (m_pstCSChannel->empty())
// 		{
// 			InfoLogStream << "in RLChannels::process_packets, cs channel is empty" << LogStream::eos;
// 		}

		if (!m_pstCSChannel->empty())
		{
			//InfoLogStream << "in RLChannels::process_packets, cs channel code size: " << m_pstCSChannel->get_codesize() << LogStream::eos;
		}

		iRecvSize = (Int32)sizeof(szRecvBuff);
		if (m_pstCSChannel->pop(szRecvBuff, iRecvSize) != Avalon::success)
		{
			//InfoLogStream << "in RLChannels::process_packets, cs channel pop fail" << LogStream::eos;
			break;
		}

		if (iRecvSize <= 0)
		{
			InfoLogStream << "in RLChannels::process_packets, cs channel pop invalid size:" << iRecvSize  << LogStream::eos;
			break;
		}

        ObjID_t roleId = 0;
		if (!decode_to_packet(szRecvBuff, (size_t)iRecvSize, packet, roleId))
		{
			InfoLogStream << "in RLChannels::process_packets, decode_to_packet fail, and size:" << iRecvSize << LogStream::eos;
			continue;
		}

		if (packet)
		{
			ExecutePacket(roleId, packet);
			Avalon::Packet::Destroy(packet);
		}
		

	}//while (true)

	return;
}

bool RLChannels::ExecutePacket(ObjID_t roleId, Avalon::Packet* packet)
{
	if (NULL != packet)
	{
		return m_Executor.ExecutePacket(packet, roleId);
	}
	
	return false;
}

bool RLChannels::decode_to_packet(char* buff, size_t size, Avalon::Packet*& packet, ObjID_t& roleId)
{
	//InfoLogStream << "in RLChannels::decode_to_packet, to decode code to message, code size:" << (UInt32)size << LogStream::eos;
	if (NULL == buff || !size)
	{
		return false;
	}

	CNetHead stHead;
	int16_t nethead_length = 0;

	if (Avalon::success != stHead.decode((const char*)buff, (int32_t)size, &nethead_length))
	{
		InfoLogStream << "in RLChannels::decode_to_packet, Net head decode failed! code size:" << (UInt32)size << LogStream::eos;
		return false;
	}
    roleId = stHead.m_RoleId;

	//InfoLogStream << "in RLChannels::decode_to_packet, received ctrl id :" << (UInt16)stHead.m_cCtrlId << " from Udp conn server" << LogStream::eos;
	//
	Avalon::Packet* pcsPacket = NULL;
	char* pszCSHeadBuff = buff + nethead_length;

	int8_t cmdId = stHead.m_cCtrlId;
	if (enmControlId_notify_disconnect == cmdId) //client断线
	{
		UInt8 reason = (UInt8)stHead.m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason;

		//
		//pcsPacket = Avalon::Packet::Create(0); //
		CLProtocol::RelaySvrDisconnectNotify notify;
		notify.disconnectReason = reason;
		notify.accId = stHead.m_unAccId;
		notify.roleId = stHead.m_RoleId;

		pcsPacket = Avalon::Packet::Create(CLProtocol::RelaySvrDisconnectNotify::_size_);
		if (NULL == pcsPacket)
		{
			return false;
		}

		if (!notify.Encode(pcsPacket)) //模拟client发送的协议包
		{
			Avalon::Packet::Destroy(pcsPacket);
			throw Avalon::NetSerializeException("encode protocol failed!"); 
			return false;
		}

		InfoLogStream << "player(" << stHead.m_unAccId << ", " << (UInt64)stHead.m_RoleId << ") was disconnected, reason(" << (UInt16)reason << ")." << LogStream::eos;

		packet = pcsPacket;

		return true;
	}//end of if (enmControlId_notify_disconnect == cmdId) 
	else if (enmControlId_notify_start == cmdId)
	{
		//InfoLogStream << "in RLChannels::decode_to_packet, received ctrl id :" << (UInt16)stHead.m_cCtrlId << " from Udp conn server" << LogStream::eos;
		//some player connect to server!
		InfoLogStream << "in RLChannels::decode_to_packet, received new player connected notify, player(accId:" << (UInt32)stHead.m_unAccId << ", roleId:" << (UInt64)stHead.m_RoleId << ") just received, not handling!" << LogStream::eos;
		return true; //
	}
	else if (enmControlId_notify_ping == cmdId) // ping
	{
		UInt32 ping = (UInt32)stHead.m_stControlInfo.m_iPing;

		CLProtocol::RelaySvrPingNotify notify;
		notify.ping = ping;
		notify.accId = stHead.m_unAccId;
		notify.roleId = stHead.m_RoleId;

		pcsPacket = Avalon::Packet::Create();
		if (NULL == pcsPacket)
		{
			return false;
		}

		if (!notify.Encode(pcsPacket))
		{
			Avalon::Packet::Destroy(pcsPacket);
			throw Avalon::NetSerializeException("encode protocol failed!");
			return false;
		}

		packet = pcsPacket;
		return true;
	}

	//
	if (size - nethead_length < Avalon::Packet::HEAD_SIZE) //说明不是一个有效的cs packet
	{
		ErrorLogStream << "in RLChannels::decode_to_packet, size:" << (UInt32) size << " is so short, net head length:" << (Int16)nethead_length << ",  so just discard!" << LogStream::eos;
		return false;
	}

	size_t remainSize = size - nethead_length;// 应该是：Avalon::Packet::HEAD_SIZE + body size

	//InfoLogStream << "in RLChannels::decode_to_packet, to create packet(size:" << (UInt32)remainSize << ")" << LogStream::eos;
	pcsPacket = Avalon::Packet::Create(remainSize); //实际上是多分配了Avalon::Packet::HEAD_SIZE大小的内存
	if (NULL == pcsPacket)
	{
		ErrorLogStream << "in RLChannels::decode_to_packet, create packet size:" << (UInt32)remainSize << " fail, just discard!" << LogStream::eos;
		return false;
	}

	//把size 和 协议id 解码到Avalon::Packet相应的字段中
	if (!pcsPacket->DecodeHeader((Avalon::UInt8*)pszCSHeadBuff, remainSize))
	{
		ErrorLogStream << "in RLChannels::decode_to_packet, Packet::DecodeHeader failed, cs code size: " << (UInt32)remainSize << " fail, just discard!" << LogStream::eos;
		return false;
	}

	//InfoLogStream << "in RLChannels::decode_to_packet, received cs protocol(id:" << pcsPacket->GetID() << ", body size:" << pcsPacket->GetSize() << ")" << LogStream::eos;

	//check packet size, should be equal!!
	if (pcsPacket->GetSize() != remainSize - Avalon::Packet::HEAD_SIZE)
	{
		//
		ErrorLogStream << "in RLChannels::decode_to_packet, received cs code size:" << (UInt32)remainSize << ", cs packet body size:" << pcsPacket->GetSize() << ", just not valid, discard!" << LogStream::eos;
		return false;
	}
	
	//指向协议体
	char* pszCSBody = buff + nethead_length + Avalon::Packet::HEAD_SIZE;
	//
	//InfoLogStream << "in RLChannels::decode_to_packet, copy body(size:" << (UInt32)(remainSize - Avalon::Packet::HEAD_SIZE) << ") to cs packet buffer" << LogStream::eos;
	memcpy(pcsPacket->GetBuffer(), pszCSBody, remainSize - Avalon::Packet::HEAD_SIZE);

	//to caller
	packet = pcsPacket;

	return true;
}

