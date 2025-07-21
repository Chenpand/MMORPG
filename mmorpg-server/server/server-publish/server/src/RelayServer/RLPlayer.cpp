#include <AvalonPacket.h>
#include <AvalonProtocol.h>
#include <CLGameSessionProtocol.h>
#include "RaceDefine.h"
#include "RLPlayer.h"
#include "CLNetHeader.h"
#include "RLChannels.h"
#include "RLPlayerMgr.h"
#include "RelaySvrRouter.h"
#include "RLPlayerMonitorMgr.h"

RLPlayer::RLPlayer(UInt32 accId, RLGameSession* session) :m_Session(session), m_AccId(accId)
{
	//m_Session = NULL;

	m_Status.reset();
	//m_framesChecksum.clear();
	m_framesChecksum.framesequence = 0;
	m_framesChecksum.checksum = 0;
	m_IsMonitored = RLPlayerMonitorMgr::Instance()->IsPlayerMonitored(accId);
	m_NetDetailFile = NULL;
	m_NetSimpleFile = NULL;
}

RLPlayer::~RLPlayer()
{
	CloseMonitorFile();
    return;
}

void RLPlayer::set_gamesession(RLGameSession* session)
{
	m_Session = session;
}

RLGameSession* RLPlayer::get_gamesession()
{
	return m_Session;
}

UInt32 RLPlayer::get_accId()
{
	return m_AccId;
}




void RLPlayer::set_offline()
{
	m_Status.set(rlplayerStatus_offline);
}

void RLPlayer::set_online()
{
	m_Status.reset(rlplayerStatus_offline);
}

bool RLPlayer::is_offline()
{
	return m_Status.test(rlplayerStatus_offline);
}

void RLPlayer::disconnect()
{
    static UInt8 szSendBuff[max_relaysvr_sc_packet_size];

    CNetHead stHead;
    stHead.m_cCtrlId = enmControlId_request_disconnect_client;
    stHead.m_unAccId = m_AccId;
    stHead.m_RoleId = GetID();
    stHead.m_iPackageSize = stHead.size();

    Int32 iNetHeadSize = (Int32)sizeof(szSendBuff);
    if (stHead.encode((char*)szSendBuff, iNetHeadSize) != success)
    {
        ErrorLogStream << "RLPlayer::disconnect: Net Head encode fail" << LogStream::eos;
        return;
    }

    if (RLChannels::Instance()->send_data((char*)szSendBuff, (size_t)stHead.m_iPackageSize) != success)
    {
        ErrorLogStream << "In  RLPlayer::disconnect, send disconnect request failed." << LogStream::eos;
        return;
    }
}

bool RLPlayer::send_packet(Avalon::Packet& packet, bool disconnect /*= false*/)
{
	//InfoLogStream << "in RLPlayer::send_packet,  player(accId:" << m_AccId << ", roleId:" << GetID() << ") to send packet(protocolId:" << packet.GetID() << ", body size:" << packet.GetSize()<< ")" << LogStream::eos;
	//
	static UInt8 szSendBuff[max_relaysvr_sc_packet_size];
	//
	CNetHead stHead;
	stHead.m_cCtrlId = enmControlId_request_transmit;
	stHead.m_unAccId = m_AccId;
	stHead.m_RoleId = GetID();
	stHead.m_stControlInfo.m_iPing = (disconnect ? 1 : 0);

	stHead.m_iPackageSize = stHead.size() + packet.GetPacketSize();			//待发送到CodeQueue的总长度

	//超过包的最大长度
	if (stHead.m_iPackageSize > (Int32)sizeof(szSendBuff))
	{
		ErrorLogStream << "RLPlayer::send_packet: packet is so big! size:" << (UInt32)stHead.m_iPackageSize << LogStream::eos;
		return false;
	}

	Int32 iNetHeadSize = (Int32)sizeof(szSendBuff);
	if (stHead.encode((char*)szSendBuff, iNetHeadSize) != success)
	{
		ErrorLogStream << "RLPlayer::send_packet: Net Head encode fail"  << LogStream::eos;
		return false;
	}

	UInt8* pszOut = szSendBuff + iNetHeadSize;

	//encode packet header
	packet.EncodeHeader();

	UInt8* csData = packet.GetPacketBuff(); //包含cs头部的buff

	//
	memcpy(pszOut, csData, packet.GetPacketSize());


	//
	if (RLChannels::Instance()->send_data((char*)szSendBuff, (size_t)stHead.m_iPackageSize) != success)
	{
		ErrorLogStream << "In  RLPlayer::send_packet, append code(size:" << (UInt32)stHead.m_iPackageSize << ") failed" << LogStream::eos;
		return false;
	}
	
	//InfoLogStream << "In  RLPlayer::send_packet, append code(size:" << (UInt32)stHead.m_iPackageSize << ") success" << LogStream::eos;

	RecordMsg(&packet, false);

	return true;
}

bool RLPlayer::send_protocol(Avalon::Protocol& protocol, bool disconnect /*= false*/)
{
	//DebugLogStream << "in RLPlayer::send_protocol,  player(accId:"  << m_AccId << ", roleId:" << GetID() <<  ") //to send packet to client,  protocol(id:" << protocol.GetProtocolID() << "), disconnect:" << disconnect << LogStream::eos;
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (!protocol.Encode(packet))
	{
		DebugLogStream << "in RLPlayer::send_protocol,  protocol.Encode fail! protocol(id:" << protocol.GetProtocolID() << "), disconnect:" << disconnect << LogStream::eos;
		Avalon::Packet::Destroy(packet); 
		throw Avalon::NetSerializeException("encode protocol failed!"); 
		return false;
	}

	bool result = send_packet(*packet, disconnect);

	//destroy packet
	Avalon::Packet::Destroy(packet);

	return result;
}

void RLPlayer::on_gamesession_event(const GameSessionEvent& event)
{
	//TODO: 未来如果需要通知client，可以在此进行通知
	//InfoLogStream << "in RLPlayer::on_gamesession_event, player(accId:" << m_AccId << ", roleId:" << GetID() << "), received session event(id:" << event.id << ")" << LogStream::eos;
	if (m_Session)
	{
		//InfoLogStream << "in RLPlayer::on_gamesession_event, player(accId:" << m_AccId << ", roleId:" << GetID() << "), received session event(id:" << event.id << "), game session (id:" << m_Session->GetID() << ")" << LogStream::eos;
	}
	if (event.id == gamesessionEventId_dismiss)
	{
		//InfoLogStream << "in RLPlayer::on_gamesession_event, player(accId:" << m_AccId << ", roleId:" << GetID() << "), to be removed from manager, game session (id:" << m_Session->GetID() << ")" << LogStream::eos;
		//
		m_Session = NULL; // ?? OK

		//

		RLPlayerMgr::Instance()->remove_player(this);
		RLPlayerMgr::Instance()->FreePlayer(this); //??

	}
	else if (event.id == gamesessionEventId_start)
	{
		
	}
	else if (event.id == gamesessionEventId_fighter_login)
	{

	}
	else if (event.id == gamesessionEventId_fighter_offline)
	{
	}
	else if (event.id == gamesessionEventId_fighter_want_endgame)
	{

	}
	
}

CLProtocol::FrameChecksum& RLPlayer::get_currentFrameChecksum()
{
	return m_framesChecksum;
}

void RLPlayer::set_currentFrameChecksum(CLProtocol::FrameChecksum& frameChecksum)
{
	m_framesChecksum.framesequence = frameChecksum.framesequence;
	m_framesChecksum.checksum = frameChecksum.checksum;
}

void RLPlayer::OnTick(const Avalon::Time& now)
{
	//tick TODO: ...
}

void RLPlayer::RecordMsg(Avalon::Packet* packet, bool isRecv)
{
	if (!m_IsMonitored || !packet)
	{
		return;
	}

	if (!m_NetSimpleFile)
	{
		std::string filename = std::string("Monitor/Simple_") + Avalon::StringUtil::ConvertToString(get_accId()) + ".txt";
		m_NetSimpleFile = fopen(filename.c_str(), "w");
	}

	if (m_NetSimpleFile)
	{
		fprintf(m_NetSimpleFile, "[%llu] player(%u,%llu) %s packet(%u) len(%d).\n", CURRENT_TIME.MSec(), get_accId(), GetID(), isRecv ? "recv" : "send", packet->GetID(), packet->GetPacketSize());
	}

	if (!m_NetDetailFile)
	{
		std::string filename = std::string("Monitor/Detail_") + Avalon::StringUtil::ConvertToString(get_accId()) + ".txt";
		m_NetDetailFile = fopen(filename.c_str(), "wb");
	}

	if (m_NetDetailFile)
	{
		Avalon::UInt8* buffer = packet->GetBuffer() - Avalon::Packet::HEAD_SIZE;  //得到消息头部的起始位置， 创建Packet时已经给头部预留出HEAD_SIZE（2+4）个字节
		packet->EncodeHeader();
		fwrite(buffer, packet->GetPacketSize(), 1, m_NetDetailFile);
	}
}

void RLPlayer::SetMonitored(bool monitored)
{
	m_IsMonitored = monitored;
	if (!m_IsMonitored)
	{
		CloseMonitorFile();
	}
}

void RLPlayer::CloseMonitorFile()
{
	if (m_NetDetailFile)
	{
		fclose(m_NetDetailFile);
		m_NetDetailFile = NULL;
	}

	if (m_NetSimpleFile)
	{
		fclose(m_NetSimpleFile);
		m_NetSimpleFile = NULL;
	}
}