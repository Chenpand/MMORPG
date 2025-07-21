#include "CLNetworkPerformance.h"
#include "CLProtocolHelper.h"

#if defined(AVALON_OS_WINDOWS)
#	include<windows.h>
#else
#	include<sys/time.h>
#endif

CLNetworkPerfMgr::CLNetworkPerfMgr()
{
	m_OpenConnectionPerf = false;
	m_OpenPacketPerf = false;
	m_ShowProtocolName = false;
	m_PerfIntervalMs = 0;
	m_LastShowPerfTime = 0;
}

CLNetworkPerfMgr::~CLNetworkPerfMgr()
{

}

void CLNetworkPerfMgr::Init(Avalon::PropertySet* propertys)
{
	if (propertys)
	{
		m_OpenConnectionPerf = propertys->GetProperty("NetPerf", "perf_connection");
		m_OpenPacketPerf = propertys->GetProperty("NetPerf", "perf_packet");
		m_PerfIntervalMs = propertys->GetProperty("NetPerf", "perf_interval_ms");
		m_ShowProtocolName = propertys->GetProperty("NetPerf", "show_protocol_name");
	}
	
	InfoLogStream << "OpenConnectionPerf(" << m_OpenConnectionPerf << ") OpenPacketPerf(" << m_OpenPacketPerf 
					<< ") PerfIntervalMs(" << m_PerfIntervalMs << ") m_ShowProtocolName(" << m_ShowProtocolName << ")." << LogStream::eos;
}

void CLNetworkPerfMgr::OnTick(const Avalon::Time& now)
{
	if (now.MSec() >= m_LastShowPerfTime + m_PerfIntervalMs)
	{
		m_LastShowPerfTime = now.MSec();

		// 连接统计
		if (m_OpenConnectionPerf)
		{
			m_Mutex.Acquire();
			for (auto& itr : m_ConnectionPerfInfoes)
			{
				auto connId = itr.first;
				auto& info = itr.second;
				InfoLogStream << "connection(" << connId << ") recv(" << info.recvPacketNum << ", " << info.recvPacketSize
					<< ") send(" << info.sendPacketNum << ", " << info.sendPacketSize << ")." << LogStream::eos;
			}
			m_ConnectionPerfInfoes.clear();
			m_Mutex.Release();
		}
		

		// 消息包统计
		if (m_OpenPacketPerf)
		{
			m_Mutex.Acquire();
			for (auto& itr : m_PacketPerfInfoes)
			{
				auto protocolid = itr.first;
				auto& info = itr.second;
				float averageUsedTime = info.procNum == 0 ? 0.0f : ((float)info.totalProcUsedTime / info.procNum);
				const char* protocolname = "";
				if (m_ShowProtocolName)
				{
					protocolname = CLProtocolHelper::Instance()->GetName(protocolid);
				}

				InfoLogStream << "packet(" << protocolid << ", " << protocolname << ") recv(" << info.recvPacketNum << ", " << info.recvPacketSize
					<< ") send(" << info.sendPacketNum << ", " << info.sendPacketSize << ") process(" << info.procNum
					<< ", " << info.totalProcUsedTime << ", " << info.maxProcUsedTime << ", " << averageUsedTime << ")" << LogStream::eos;
			}
			m_PacketPerfInfoes.clear();
			m_Mutex.Release();
		}
	}
}

void CLNetworkPerfMgr::OnSendPacket(UInt32 connId, UInt32 protocolid, UInt32 size)
{
	if (protocolid == 0)
	{
		return;
	}

	// 连接统计
	if (IsOpenPerfConnection())
	{
		m_Mutex.Acquire();
		auto itr = m_ConnectionPerfInfoes.find(connId);
		if (itr == m_ConnectionPerfInfoes.end())
		{
			CLConnectionPerfInfo info;
			info.sendPacketNum = 1;
			info.sendPacketSize = size;
			m_ConnectionPerfInfoes[connId] = info;
		}
		else 
		{
			auto& info = itr->second;
			info.sendPacketNum++;
			info.sendPacketSize += size;
		}
		m_Mutex.Release();
	}

	// 消息包统计
	if (IsOpenPerfPacket())
	{
		m_Mutex.Acquire();
		auto itr = m_PacketPerfInfoes.find(protocolid);
		if (itr == m_PacketPerfInfoes.end())
		{
			CLPacketPerfInfo info;
			info.sendPacketNum = 1;
			info.sendPacketSize = size;
			m_PacketPerfInfoes[protocolid] = info;
		}
		else
		{
			auto& info = itr->second;
			info.sendPacketNum++;
			info.sendPacketSize += size;
		}
		m_Mutex.Release();
	}
}

void CLNetworkPerfMgr::OnRecvPacket(UInt32 connId, UInt32 protocolid, UInt32 size)
{
	if (protocolid == 0)
	{
		return;
	}

	// 连接统计
	if (IsOpenPerfConnection())
	{
		m_Mutex.Acquire();
		auto itr = m_ConnectionPerfInfoes.find(connId);
		if (itr == m_ConnectionPerfInfoes.end())
		{
			CLConnectionPerfInfo info;
			info.recvPacketNum = 1;
			info.recvPacketSize = size;
			m_ConnectionPerfInfoes[connId] = info;
		}
		else
		{
			auto& info = itr->second;
			info.recvPacketNum++;
			info.recvPacketSize += size;
		}
		m_Mutex.Release();
	}

	// 消息包统计
	if (IsOpenPerfPacket())
	{
		m_Mutex.Acquire();
		auto itr = m_PacketPerfInfoes.find(protocolid);
		if (itr == m_PacketPerfInfoes.end())
		{
			CLPacketPerfInfo info;
			info.recvPacketNum = 1;
			info.recvPacketSize = size;
			m_PacketPerfInfoes[protocolid] = info;
		}
		else
		{
			auto& info = itr->second;
			info.recvPacketNum++;
			info.recvPacketSize += size;
		}
		m_Mutex.Release();
	}
}

void CLNetworkPerfMgr::OnProcessPacket(UInt32 connId, UInt32 protocolid, UInt64 usedTime)
{
	if (protocolid == 0)
	{
		return;
	}

	// 消息包统计
	if (IsOpenPerfPacket())
	{
		m_Mutex.Acquire();
		auto itr = m_PacketPerfInfoes.find(protocolid);
		if (itr == m_PacketPerfInfoes.end())
		{
			CLPacketPerfInfo info;
			info.procNum = 1;
			info.totalProcUsedTime = usedTime;
			info.maxProcUsedTime = usedTime;
			m_PacketPerfInfoes[protocolid] = info;
		}
		else
		{
			auto& info = itr->second;
			info.procNum++;
			info.totalProcUsedTime += usedTime;
			if (usedTime > info.maxProcUsedTime)
			{
				info.maxProcUsedTime = usedTime;
			}
		}
		m_Mutex.Release();
	}
}

void CLNetworkPerfMgr::OpenPerfConnection(bool open)
{
	m_OpenConnectionPerf = open;
}

bool CLNetworkPerfMgr::IsOpenPerfConnection() const
{
	return m_OpenConnectionPerf;
}

void CLNetworkPerfMgr::OpenPerfPacket(bool open)
{
	m_OpenPacketPerf = open;
}

bool CLNetworkPerfMgr::IsOpenPerfPacket() const
{
	return m_OpenPacketPerf;
}

UInt64 CLNetworkPerfMgr::GetUSecsNow()
{
	if (!m_OpenPacketPerf)
	{
		return 0;
	}

#if defined(AVALON_OS_WINDOWS)
	return 0;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (UInt64)tv.tv_sec * 1000000 + (UInt64)tv.tv_usec;
#endif
}

void CLNetworkPerfMgr::SetPerfIntervalMs(UInt32 ms)
{
	m_PerfIntervalMs = ms;
}