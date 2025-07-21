#include "ASInternalNetwork.h"

#include <AvalonThread.h>
#include <CLProtocolDefine.h>
#include <CLSysProtocol.h>
#include <CLUnionServerProtocol.h>

#include "ASApplication.h"
#include "ASRouter.h"
#include "GateSessionMgr.h"

ASInternalNetwork::ASInternalNetwork()
{
	m_bCenterConnected = false;
}

ASInternalNetwork::~ASInternalNetwork()
{
}

bool ASInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if(!CLInternalNetwork::Startup(properties, addresses)) return false;

	int times = 10;
	//while(!m_bCenterConnected && times-- > 0) Avalon::Thread::Sleep(500);
	if(times <= 0)
	{
		ErrorLogStream << "can not connect to center server!" << LogStream::eos;
		return false;
	}

	return true;
}

bool ASInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if(CLInternalNetwork::ExecutePacket(packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::CENTER_FORBIDTALK_LIST_RET:
	case CLProtocol::CENTER_NOTIFY_NEW_FORBIDTALK:
	case CLProtocol::CENTER_NOTIFY_DEL_FORBIDTALK:
	case CLProtocol::CENTER_AWARDPOOL_INFO_RET:
	case CLProtocol::CENTER_AWARDPOOL_SYNC_GOLDNUM:
	case CLProtocol::CENTER_AWARDPOOL_NOTIFY_NEWROUND:
	case CLProtocol::CENTER_AWARDPOOL_NOTIFY_FAILED:
	case CLProtocol::CENTER_AWARDPOOL_PUTSTONE_RET:
	case CLProtocol::CENTER_AWARDPOOL_GETAWARD_RET:
	case CLProtocol::SC_CDK_VERIFY_RES:
	case CLProtocol::FORBIDACCOUNT_NOTIFY:
	case CLProtocol::CENTER_CUSTOM_SERVICE_SIGN_RES:
	case CLProtocol::CENTER_GLOBAL_SERVER_INFO_RET:
	case CLProtocol::CENTER_QUERY_CONVERT_ACCOUNT_RES:
	case CLProtocol::WORLD_CHECK_RECHARGE_PUSH_RES:
	case CLProtocol::GAS_CHECK_HIRE_BIND_RET:
	case CLProtocol::WORLD_QUERY_HIRE_ALREADY_BIND_RES:
		{
			Router::SendToWorld(packet);
		}
		break;
	case CLProtocol::SYS_TRANSMIT_TO_PLAYER:
		{
			Router::SendToWorld(packet);

			if (ASApplication::Instance()->IsLogTeamCopyProtocol())
			{
				UInt32 nodeid = packet->GetConnID();
				CLProtocol::SysTransmitToPlayer protocol;
				if (protocol.Decode(packet))
				{
					auto protocolid = protocol.packet ? protocol.packet->GetID() : 0;
					UInt32 servType = CL_GET_SERVERTYPE_FROM_PROTOCOLID(protocolid);
					UInt32 funcType = (protocolid % CL_SERVERTYPE_OFFSET_IN_PROTOCOLID) / 100;
					if (servType == ST_TEAMCOPY || servType == ST_DUNGEON_SVR || funcType == CLProtocol::FUNCTION_DUNGEON)
					{
						InfoLogStream << "[TCS_PROTOCOL] player(" << protocol.playerId << ") trans teamcopy protocol(" << protocolid << ") from node(" << nodeid << ")." << LogStream::eos;

						if (!Router::IsWorldConnected())
						{
							ErrorLogStream << "[TCS_PROTOCOL] player(" << protocol.playerId << ") trans teamcopy protocol(" << protocolid << ") from node(" << nodeid << ") to world failed." << LogStream::eos;
						}
					}
				}
				else
				{
					ErrorLogStream << "[TCS_PROTOCOL] decode from node(" << nodeid << ") to world failed." << LogStream::eos;
				}
			}
		}
		break;
	case CLProtocol::GATE_CHANGEZONE_CHECK_REQ:
		{
			GateSession* session = GateSessionMgr::Instance()->SelectGate();
			if(session != NULL)
			{
				session->SendPacket(packet);
			}
		}
		break;
	case CLProtocol::GATE_CHANGEZONE_CHECK_RET:
		{
			Router::SendToWorld(packet);
		}
		break;
	}

	return true;
}

bool ASInternalNetwork::PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet)
{
	if(CLInternalNetwork::PreHandlePacket(conn, packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::CENTER_NOTIFY_CONNECTED:
		{
			m_bCenterConnected = true;
		}
		return true;
	}
	return false;
}

Avalon::NetConnectionPtr ASInternalNetwork::FindUnionConnectionByFun(UnionServerFunctionType funType)
{
	auto iter = m_UnionFunctionConnID.find(funType);
	if (iter == m_UnionFunctionConnID.end())
	{
		return nullptr;
	}

	return FindConnection(iter->second);
}

void ASInternalNetwork::OnConnected(UInt32 id)
{
	Router::OnConnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_GATE)
	{
		GateSessionMgr::Instance()->OnGateConnected(id);
	}
	if (servType == ST_UNION)
	{
		//while (true)
		{
			CLProtocol::UnionSceneIsReady notify;
			notify.id = ++m_addID;
			auto conn = FindConnection(id);
			if (conn != nullptr)
			{
				InfoLogStream << "seq id is" << notify.id << LogStream::eos;
				CL_SEND_PROTOCOL(conn, notify);
			}
		}

	}
}

void ASInternalNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_GATE)
	{
		GateSessionMgr::Instance()->OnGateDisconnected(id);
	}
	else if (servType == ST_UNION)
	{
		for (auto& conn : m_UnionFunctionConnID)
		{
			if (conn.second == id)
			{
				m_UnionFunctionConnID.erase(conn.first);
				break;
			}
		}
	}

	Router::OnDisconnected(id);
}

void ASInternalNetwork::OnShutdownService()
{
	ASApplication::Instance()->Exit();
}

void ASInternalNetwork::OnUnionServerNotifyOpenFunction(UInt32 connID, std::vector<UInt32> funVec)
{
	for (auto funType : funVec)
	{
		auto type = static_cast<UnionServerFunctionType>(funType);
		auto iter = m_UnionFunctionConnID.find(type);
		if (iter != m_UnionFunctionConnID.end())
		{
			ErrorLogStream << "Already have this function UnionServer connection,function id is " << static_cast<UInt32>(iter->first) << "connection id is " << iter->second << LogStream::eos;
			continue;
		}
		m_UnionFunctionConnID.insert(std::make_pair(type, connID));
	}
	
}
