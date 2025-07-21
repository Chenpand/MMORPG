#include "RSNetwork.h"

#include <CLRecordProtocol.h>
#include "RSApplication.h"
#include "RSTableMgr.h"
#include "RSRouter.h"

#include "PerformanceCheck.h"

RSNetwork::RSNetwork()
{
	m_PacketNum = 0;
	m_ByteNum = 0;
	m_Timer.SetInterval(5 * 60 * 1000);
}

RSNetwork::~RSNetwork()
{
}

void RSNetwork::OnTick(const Avalon::Time& now)
{
	CLInternalNetwork::OnTick(now);

	if(m_Timer.IsTimeout(now))
	{
		InfoLogStream << "received " << m_PacketNum << " packets, " << m_ByteNum << " bytes." << LogStream::eos;
		m_PacketNum = m_ByteNum = 0;
	}
}

void RSNetwork::OnConnected(UInt32 id)
{
	Router::OnConnected(id);
}

void RSNetwork::OnDisconnected(UInt32 id)
{
	Router::OnDisconnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_DBPROXY)
	{
		CLProtocol::RecordNotifyOtherDisconnected notify;
		notify.zoneId = 0;
		Router::BroadcastToScene(notify);
	}
	else
	{
		RSTableMgr::Instance()->Flush();
	}
}

void RSNetwork::OnShutdownService()
{
	RSApplication::Instance()->Exit();
}

bool RSNetwork::ExecutePacket(Avalon::Packet* packet)
{
	++m_PacketNum;
	m_ByteNum += packet->GetSize() + Avalon::Packet::HEAD_SIZE;

	return CLInternalNetwork::ExecutePacket(packet);
}
