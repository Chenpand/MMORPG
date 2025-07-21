#include "VerifyCenterHandler.h"
#include <CLErrorCode.h>
#include "VCInternalNetwork.h"
#include "VerifyServerMgr.h"
#include "VerifyClientMgr.h"

void HandleProtocol(CLProtocol::VerifyCenterRegisterReq& req)
{
	CLProtocol::VerifyCenterRegisterRes res;
	auto node = VerifyServerMgr::Instance()->FindNode(req.GetConnID());
	if (node)
	{
		res.result = 1;
		req.SendBack(res);

		InfoLogStream << "node(" << req.GetConnID() << ") repeat register, ability("
			<< req.ability << ") address(" << req.addr.ToString() << ")." << LogStream::eos;

		return;
	}

	auto conn = VCInternalNetwork::Instance()->FindConnection(req.GetConnID());
	if (conn == NULL)
	{
		return;
	}

	node = new CLVerifyNode();
	node->Init(req.GetConnID(), req.addr, conn);
	node->SetAbality(req.ability);
	VerifyServerMgr::Instance()->AddNode(node);

	// ·µ»Ø
	res.result = ErrorCode::SUCCESS;
	req.SendBack(res);

	InfoLogStream << "node(" << req.GetConnID() << ") register, ability("
		<< req.ability << ") address(" << req.addr.ToString() << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::VerifyCenterReportAbility& protocol)
{
	auto node = VerifyServerMgr::Instance()->FindNode(protocol.GetConnID());
	if (!node)
	{
		ErrorLogStream << "unexist node(" << protocol.GetConnID() << ") report ability." << LogStream::eos;
		return;
	}

	VerifyServerMgr::Instance()->UpdateAbality(protocol.GetConnID(), protocol.ability);
}