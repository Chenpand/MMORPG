#include "DSRouter.h"
#include "ServiceSessionMgr.h"
#include "RelaySessionMgr.h"
#include "SceneSessionMgr.h"

bool DSRouter::SendToService(UInt32 id, Avalon::Packet* packet)
{
	ServiceSession* session = ServiceSessionMgr::Instance()->FindServiceSession(id);
	if (!session)
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find node(" << id << ") when send protocol(" << packet->GetID() << ")." << LogStream::eos;
		return false;
	}

	session->SendPacket(packet);
	return true;
}

bool DSRouter::SendToService(UInt32 id, Avalon::Protocol& protocol)
{
	ServiceSession* session = ServiceSessionMgr::Instance()->FindServiceSession(id);
	if (!session)
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find node(" << id << ") when send protocol(" << protocol.GetProtocolID() << ")." << LogStream::eos;
		return false;
	}

	session->SendProtocol(protocol);
	return true;
}

bool DSRouter::SendToRelay(UInt32 nodeid, Avalon::Protocol& protocol)
{
	auto relaySessiones = RelaySessionMgr::Instance()->FindSessionByNodeId(nodeid);
	if (!relaySessiones)
	{
		return false;
	}

	relaySessiones->SendProtocol(protocol);
	return true;
}

bool DSRouter::SendToScene(UInt32 serviceId, UInt32 sceneId, Avalon::Protocol& protocol)
{
	ServiceSession* serviceSession = ServiceSessionMgr::Instance()->FindServiceSession(serviceId);
	if (!serviceSession)
	{
		return false;
	}
	CLProtocol::SysTransmitInNodes req;
	req.destnode = sceneId;
	if (!protocol.Encode(req.packet))
	{
		return false;
	}

	serviceSession->SendProtocol(req);
	return true;
}

bool DSRouter::SendToScene(UInt32 sceneSessionId, Avalon::Protocol& protocol)
{
	SceneSession* session = SceneSessionMgr::Instance()->FindSceneSession(sceneSessionId);
	if (!session)
	{
		return false;
	}
	session->SendProtocol(protocol);
	return true;
}

bool DSRouter::SendToWorld(UInt32 serviceId, UInt32 sceneWorldId, Avalon::Protocol& protocol)
{
	ServiceSession* serviceSession = ServiceSessionMgr::Instance()->FindServiceSession(serviceId);
	if (!serviceSession)
	{
		ErrorLogStream << "[TCS_PROTOCOL] not find service id : " << serviceId << ", WorldId : " << sceneWorldId << LogStream::eos;
		return false;
	}
	CLProtocol::SysTransmitInNodes req;
	req.destnode = sceneWorldId;
	if (!protocol.Encode(req.packet))
	{
		ErrorLogStream << "[TCS_PROTOCOL] protocol encode err " << LogStream::eos;
		return false;
	}

#ifdef _DEBUG
	DebugLogStream << "send protocol(" << protocol.GetProtocolID() << ") to service(" << serviceId << ") wsId(" << sceneWorldId << ")." << LogStream::eos;
#endif

	serviceSession->SendProtocol(req);
	return true;
}