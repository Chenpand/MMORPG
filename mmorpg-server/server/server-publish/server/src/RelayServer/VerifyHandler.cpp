#include "VerifyHandler.h"
#include <CLSysProtocol.h>

#include "VerifyServerMgr.h"
#include "RLInternalNetwork.h"
#include "RaceMgr.h"
#include "Race.h"
#include "RelaySvrRouter.h"

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSInfo& protocol)
{
	for (auto tmpNode : protocol.nodes)
	{
		auto node = VerifyServerMgr::Instance()->FindNode(tmpNode.GetID());
		if (node)
		{
			continue;
		}

		node = new CLVerifyNode();
		Avalon::NetConnectionPtr conn = RLInternalNetwork::Instance()->Connect(tmpNode.GetAddr());
		if (conn != NULL)
		{
			CLProtocol::SysLoginReq req;
			req.nodeid = NODE_ID;

			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			CL_SAFE_DELETE(node);
			continue;
		}

		node->Init(tmpNode.GetID(), tmpNode.GetAddr(), conn);
		node->SetAbality(tmpNode.GetAbality());
		VerifyServerMgr::Instance()->AddNode(node);
	}
}

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSOnline& protocol)
{
	auto node = VerifyServerMgr::Instance()->FindNode(protocol.node.GetID());
	if (node)
	{
		return;
	}

	node = new CLVerifyNode();
	Avalon::NetConnectionPtr conn = RLInternalNetwork::Instance()->Connect(protocol.node.GetAddr());
	if (conn != NULL)
	{
		CLProtocol::SysLoginReq req;
		req.nodeid = NODE_ID;

		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		return;
	}

	node->Init(protocol.node.GetID(), protocol.node.GetAddr(), conn);
	node->SetAbality(protocol.node.GetAbality());
	VerifyServerMgr::Instance()->AddNode(node);
}

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSOffline& protocol)
{
	VerifyServerMgr::Instance()->DelNode(protocol.id);
}

void HandleProtocol(CLProtocol::VerifyCenterNotifyAbility& protocol)
{
	VerifyServerMgr::Instance()->UpdateAbality(protocol.nodeId, protocol.ability);
}

void HandleProtocol(CLProtocol::VerifyReportTaskRaceEnd& res)
{
	auto race = RaceMgr::Instance()->FindRace(res.raceId);
	if (!race)
	{
		ErrorLogStream << "can't find race(" << res.raceId << ")." << LogStream::eos;
		return;
	}

	Router::SendToWorldSvr(race->RandGetWSID(), res);

	InfoLogStream << "race(" << race->GetID() << ") set verify server's race end info." << LogStream::eos;
	race->SetVerifyRaceEndPacket(res.packet);
	res.Detach();
}