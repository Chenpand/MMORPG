#include "GSExternalNetwork.h"

#include <CLLoginProtocol.h>
#include <CLGateProtocol.h>
#include <CLErrorCode.h>
#include <CLLeaveGameProtocol.h>
#include <CLEnterGameProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLDungeonProtocol.h>

#include <AvalonLogger.h>
#include "GSApplication.h"
#include "GSAccountMgr.h"
#include "GSPlayerMgr.h"
#include "GSRouter.h"


GSExternalConnection::GSExternalConnection(CLNetwork* network)
	:CLExternalConnection(network)
{
	m_AccId = 0;
	m_bWaitOffine = false;
}

GSExternalConnection::~GSExternalConnection()
{
}

bool GSExternalConnection::PreHandlePacket(Avalon::Packet *packet)
{
	if(CLExternalConnection::PreHandlePacket(packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::GATE_CLIENTLOGIN_REQ:
		{
            DebugLogStream << "conn(" << GetID() << ") recv login req." << LogStream::eos;

			if(IsVerified()) return true;
			
			CLProtocol::GateClientLoginReq req;
			if(!req.Decode(packet))
			{
                DebugLogStream << "conn(" << GetID() << ") decode packet failed." << LogStream::eos;
				Close();
				return true;
			}
			
			if(!GSAccountMgr::Instance()->Verify(req.accId, req.hashValue))
			{
                DebugLogStream << "conn(" << GetID() << ") verify failed." << LogStream::eos;

				CLProtocol::GateClientLoginRet ret;
				ret.result = ErrorCode::LOGIN_WRONG_PASSWD;
				SendProtocol(ret);
				Close();
				return true;
			}

			m_AccId = req.accId;

			if(!GetNetwork()->AddVerifiedConnection(m_AccId, this))
			{
                DebugLogStream << "conn(" << GetID() << ") add conn failed." << LogStream::eos;

				Avalon::NetConnectionPtr oldConn = GetNetwork()->FindConnection(m_AccId);
				if(oldConn != NULL)
				{
					if (CURRENT_TIME.Sec() <= oldConn->GetOpenTime() + 10)
					{
						DebugLogStream << "conn(" << oldConn->GetID() << ") login frequently." << LogStream::eos;
					}
					else
					{
						DebugLogStream << "repeat login, disconnect conn(" << oldConn->GetID() << ")." << LogStream::eos;

						//通知重复登陆被踢
						CLProtocol::GateNotifyKickoff notify;
						notify.errorCode = ErrorCode::LOGIN_REPEAT;
						CL_SEND_PROTOCOL(oldConn, notify);
					}
                    
					oldConn->Close();
					m_bWaitOffine = true;
				}
			}

            DebugLogStream << "conn(" << GetID() << ") verified success." << LogStream::eos;
			return true;
		}
		break;
	case CLProtocol::GATE_RECONNECT_GAME_REQ:
	{
			CLProtocol::GateReconnectGameReq req;
			if(!req.Decode(packet))
			{
                DebugLogStream << "conn(" << GetID() << ") decode packet failed." << LogStream::eos;
				Close();
				return true;
			}

			m_AccId = req.accid;
			GSAccountMgr::Instance()->AddReconnAccount(m_AccId, req.sequence);

			if(!GetNetwork()->AddVerifiedConnection(m_AccId, this))
			{
				DebugLogStream << "conn(" << m_AccId << ") add conn failed." << LogStream::eos;

				Avalon::NetConnectionPtr oldConn = GetNetwork()->FindConnection(m_AccId);
				if(oldConn != NULL)
				{
                    DebugLogStream << "repeat login, disconnect conn(" << oldConn->GetID() << ")." << LogStream::eos;

					//通知重复登陆被踢
					CLProtocol::GateNotifyKickoff notify;
					notify.errorCode = ErrorCode::LOGIN_REPEAT;
					CL_SEND_PROTOCOL(oldConn, notify);
					oldConn->Close();

					m_bWaitOffine = true;
				}
			}

            DebugLogStream << "player(" << m_AccId << ") reconnect success." << LogStream::eos;
			return true;
	}
		break;
	}

	return false;
}

void GSExternalConnection::OnTick(const Avalon::Time& now)
{
	CLExternalConnection::OnTick(now);

	if(m_bWaitOffine)
	{
		if(GetNetwork()->AddVerifiedConnection(m_AccId,this))
		{
			m_bWaitOffine = false;
		}
	}
}



GSExternalNetwork::GSExternalNetwork()
{
}

GSExternalNetwork::~GSExternalNetwork()
{
}

bool GSExternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	// 统计时间
	auto _startTime = GSPlayerMgr::Instance()->GetUSecsNow();

	UInt32 servType = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	{
		auto protocolId = packet->GetID();
		if (GSApplication::Instance()->IsLogTransProtocol() && packet->GetID() != CLProtocol::SCENE_MOVE_REQUIT)
		{
			InfoLogStream << "player(" << packet->GetConnID() << ") trans protocol(" << protocolId << ") to server(" << servType << ")." << LogStream::eos;
		}
	}

	//分发到场景
	if (servType == ST_SCENE || packet->GetID() == CLProtocol::WorldDungeonReportFrameReq::__ID || packet->GetID() == CLProtocol::WorldDungeonEnterRaceReq::__ID)	
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if(player != NULL && player->GetStatus() == GPS_NORMAL)
		{
// #ifdef _DEBUG
// 			if (player->GetLastRecvSequence() + 1 != packet->GetSequence())
// 			{
// 				ErrorLogStream << "recv player(" << player->GetAccID() << ", " << player->GetID() << ") error sequence" << LogStream::eos;
// 			}
// 
// 			DebugLogStream << "recv player(" << player->GetAccID() << ", " << player->GetID() << ") msg(" << packet->GetID() << ") sequence(" << packet->GetSequence() << ") len(" << packet->GetPacketSize() << ")." << LogStream::eos;
// #endif
			player->RecordRecord(packet, true);
			if (packet->GetSequence() != 0)
			{
				player->SetLastRecvSequence(packet->GetSequence());
			}
			CLProtocol::GateTransmitPacket trans(packet);
			trans.id = player->GetID();
			player->SendToSceneServer(trans);
			trans.Detach();
		}

		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);

		return true;
	}
	else if(servType == ST_WORLD)	//分发到world
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if(player != NULL && player->GetStatus() == GPS_NORMAL)
		{
// #ifdef _DEBUG
// 			if (player->GetLastRecvSequence() + 1 != packet->GetSequence())
// 			{
// 				ErrorLogStream << "recv player(" << player->GetAccID() << ", " << player->GetID() << ") error sequence" << LogStream::eos;
// 			}
// 
// 			DebugLogStream << "recv player(" << player->GetAccID() << ", " << player->GetID() << ") msg(" << packet->GetID() << ") sequence(" << packet->GetSequence() << ") len(" << packet->GetPacketSize() << ")." << LogStream::eos;
// #endif
			player->RecordRecord(packet, true);
			if (packet->GetSequence() != 0)
			{
				player->SetLastRecvSequence(packet->GetSequence());
			}
			CLProtocol::GateTransmitPacket trans(packet);
			trans.id = player->GetID();
			Router::SendToWorld(trans);
			trans.Detach();
		}

		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);
		return true;
	}
	else if (servType == ST_GLOBAL_ACTIVITY)	//分发到全局活动服
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if (player != NULL && player->GetStatus() == GPS_NORMAL && player->GetID() > 0)
		{
			player->RecordRecord(packet, true);
			if (packet->GetSequence() != 0)
			{
				player->SetLastRecvSequence(packet->GetSequence());
			}
			player->SendToGlobalActivity(packet);
		}

		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);
		return true;

	}
	else if (servType == ST_BATTLE)
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if (player != NULL && player->GetStatus() == GPS_NORMAL)
		{
			player->SendToBattle(packet);
		}

		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);
		return true;
	}
	else if (servType == ST_TEAMCOPY)
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if (player != NULL && player->GetStatus() == GPS_NORMAL)
		{
			player->SendToTeamCopy(packet);
		}

		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);
		return true;
	}
	else if (servType == ST_UNION)
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(packet->GetConnID());
		if (player != NULL && player->GetStatus() == GPS_NORMAL)
		{
			player->SendToUnion(packet);
		}
		return true;
	}
	else
	{
		bool result = CLExternalNetwork::ExecutePacket(packet);
		// 统计时间
		auto _endTime = GSPlayerMgr::Instance()->GetUSecsNow();
		GSPlayerMgr::Instance()->RecordUsedTime(packet->GetID(), _endTime - _startTime);
		return result;
	}
}

Avalon::NetConnection* GSExternalNetwork::CreateConnection()
{
	return new GSExternalConnection(this);
}

void GSExternalNetwork::OnConnected(UInt32 id)
{
	GSPlayerMgr::Instance()->OnGateConnected(id);
}

void GSExternalNetwork::OnDisconnected(UInt32 id)
{
	GSPlayerMgr::Instance()->OnGateDisconnected(id);
}
